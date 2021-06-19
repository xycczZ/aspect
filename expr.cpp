//
// Created by xycc on 6/18/21.
//

#include "expr.h"

#include <tuple>
#include <regex>

using namespace std;

string str_replace(const string& s, char search, const std::string& rep) {
    string result;
    for (auto const c: s) {
        if (c == search) {
            result += rep;
        } else {
            result.push_back(c);
        }
    }
    return result;
}

string replace(const string& s, char search, char rep) {
    string result;
    for (auto const c: s) {
        if (c == search) {
            result.push_back(rep);
        } else {
            result.push_back(c);
        }
    }
    return result;
}

pair<string, string> explode(const string &str, char sep, bool last) {
    auto trim_str = trim(str);
    size_t pos;
    if (last) {
        pos = trim_str.find_last_of(sep);
    } else {
        pos = trim_str.find_first_of(sep);
    }

    if (pos == string::npos) {
        return make_pair(trim_str, string{});
    }

    auto rest = trim_str.substr(pos, trim_str.size() - 1);
    rest = trim(rest);
    return make_pair(trim_str.substr(0, pos - 1), rest);
}

vector<string> split(const string& str, char sep) {
    size_t pos = 0;
    vector<string> result;
    do {
        auto next_pos = str.find(sep, pos);
        if (next_pos != string::npos) {
            result.push_back(trim(str.substr(pos, next_pos - 1)));
        }
        pos = next_pos;
    } while (pos != string::npos);

    return result;
}

string trim(const string& str) {
    if (str.empty()) { return string{}; }
    auto first_pos = str.find_first_not_of(' ');
    if (first_pos == string::npos) {
        return string{};
    }
    auto end_pos = str.find_last_of(' ');
    auto result = str.substr(first_pos, end_pos);
    return result;
}

/**
 * 构造一个表达式实例
 * 保存有 类表达式和方法表达式
 * 类表达式保存了 命名空间和类名
 * 方法表达式保存了 方法的访问权限，方法名，方法的参数， 方法的返回值类型
 * example:
 * auto expr = new Expr("public|protected App.*.*Service::method(type param1, type2 param2): return_type");
 * @param str
 */
Expr::Expr(const string& str): AbstractExpr(trim(str)) {
    // split acc
    if (!isMatchAll()) {
        string acc, rest, fqn, method, params, return_type;
        tie(acc, rest) = explode(str, ' ');
        tie(fqn, rest) = explode(rest, '.', true);
        tie(method, rest) = explode(rest, '(');
        tie(params, return_type) = explode(rest, ')');
        // then create child expr
        class_expr = new ClassExpr(fqn);
        auto param_expr = new ParamExpr(params);
        method_expr = new MethodExpr(acc, method, return_type, param_expr);
    }
}

bool Expr::isMatch(const string& clazz, const string &method) const {
    if (isMatchAll()) {
        return true;
    }

    return class_expr->isMatch(clazz) && method_expr->isMatch(clazz, method);
}

bool ClassExpr::isMatch(const string &fqn) const {
    if (isMatchAll()) {
        return true;
    }

    auto pos = fqn.find('*');

    if (pos == string::npos) {
        return fqn == *expr;
    }

    auto pattern = str_replace(fqn, '*', ".*?");
    regex reg(pattern);
    return regex_match(fqn, reg);
}

bool MethodExpr::match_access(uint32_t access) const {
    return access & acc;
}

bool MethodExpr::match_method(const string &method) const {
    auto pos = method.find('*');
    if (pos == string::npos) {
        return method == *expr;
    }

    regex reg(str_replace(*expr, '*', ".*?"));
    return regex_match(method, reg);
}

bool MethodExpr::match_return_type(const zend_arg_info arg) const {
    if (*return_type == "*") {
        return true;
    }

    auto pos = return_type->find('*');
    if (pos == string::npos) {
        return return_type->c_str() == zend_type_to_string(arg.type)->val;
    }

    regex reg(str_replace(*return_type, '*', ".*?"));
    return regex_match(string{zend_type_to_string(arg.type)->val}, reg);
}

bool MethodExpr::match_return_type(const zend_internal_arg_info arg) const {
    if (*return_type == "*") {
        return true;
    }

    auto pos = return_type->find('*');
    if (pos == string::npos) {
        return return_type->c_str() == zend_type_to_string(arg.type)->val;
    }

    regex reg(str_replace(*return_type, '*', ".*?"));
    return regex_match(string{zend_type_to_string(arg.type)->val}, reg);
}

/**
 * 参数的匹配， 要么是*号全匹配， 要么是每个参数都写上， 逗号隔开， 每段都可以写*号模糊匹配
 */
bool ParamExpr::isMatch(const zend_arg_info *args, uint32_t num) const {
    if (isMatchAll()) {
        return true;
    }

    auto segments = split(*expr, ',');

    if (segments.size() != num) {
        return false;
    }

    string type, name, default_value;
    for (uint32_t i = 0; i < num; i++) {
        if (segments[i] != "*") {
            // 如果不是星号的全匹配， 那么将这一段按照空格分开，分成一个pair， 第一个是类型， 第二个是参数名
            // 先判断当前的方法的当前参数是否和类型和参数名符合， 每一段都需要考虑星号的模糊匹配
            tie(type, name) = explode(segments[i], ' ');
            tie(name, default_value) = explode(name, '=');
            bool match = check_type(type, args[i].type) && check_name(name, args[i].name) && check_default_value(default_value, args[i].default_value);
            if (!match) {
                return false;
            }
        }
    }

    return true;
}

bool ParamExpr::check_type(const std::string &type, const zend_type actual_type) {
    if (type == "*") {
        return true;
    }

    if (type.find('*') != string::npos) {
        return type.c_str() == zend_type_to_string(actual_type)->val;
    }

    regex reg(str_replace(type, '*', ".*?"));
    auto actual = string{zend_type_to_string(actual_type)->val};
    return regex_match(actual, reg);
}

bool ParamExpr::check_name(const std::string &name, const zend_string *actual_name) {
    if (name == "*") {
        return true;
    }

    if (name.find('*') != string::npos) {
        return name.c_str() == actual_name->val;
    }

    regex reg(str_replace(name, '*', ".*?"));
    auto actual = string{actual_name->val};
    return regex_match(actual, reg);
}

// 默认值没有模糊匹配， 要么*号全匹配， 要么字符串的全匹配
bool ParamExpr::check_default_value(const std::string &default_value, const zend_string *actual_default) {
    if (default_value == "*") {
        return true;
    }

    return default_value.c_str() == actual_default->val;
}