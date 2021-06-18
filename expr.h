//
// Created by xycc on 6/18/21.
//

#ifndef ASPECT_EXPR_H
#define ASPECT_EXPR_H

#include <memory>
#include <string>
#include <vector>
#include <locale>
extern "C" {
#include "php.h"
#include "ext/standard/info.h"
};

std::pair<std::string, std::string> explode(const std::string& str, char sep, bool last = false);
std::string trim(const std::string &);
std::string str_replace(const std::string&, char, const std::string&);
std::string str_replace(const std::string&, char, char);

inline zend_string* str_to_zend(const std::string& s) {
    auto str = zend_string_init_fast(s.c_str(), s.size());
    return str;
}

inline std::string str_to_lower(const std::string& s) {
    std::string result;
    for (const auto c: s) {
        result.push_back(std::tolower(c, std::locale()));
    }
    return result;
}


class AbstractExpr {
protected:
    std::unique_ptr<std::string> expr;
    explicit AbstractExpr(const std::string& str): expr(std::make_unique<std::string>(str)) {};
    explicit AbstractExpr(std::string&& str): expr(std::make_unique<std::string>(str)) {};
public:
    inline bool isMatchAll() const { return *expr == "*"; }
    virtual ~AbstractExpr() = default;
};

class ClassExpr;
class MethodExpr;
class ParamExpr;

class Expr : public AbstractExpr {
protected:
    ClassExpr *class_expr;
    MethodExpr *method_expr;
public:
    explicit Expr(const std::string& str);
    // 这里传入的是 需要匹配的当前的class名和方法名
    bool isMatch(const std::string &clazz, const std::string &method) const;
    ~Expr() override = default;
};

class ClassExpr: public AbstractExpr {
public:
    explicit ClassExpr(const std::string& str): AbstractExpr(str) {};
    explicit ClassExpr(zend_string& str): AbstractExpr(std::string{str.val}) {};
    bool isMatch(const std::string& str) const;
};

class ParamExpr: public AbstractExpr {
private:
    static bool check_type(const std::string& type, zend_type actual_type) ;
    static bool check_name(const std::string& name, const zend_string* actual_name) ;
    static bool check_default_value(const std::string& default_value, const zend_string* actual_default) ;
public:
    explicit ParamExpr(const std::string& str): AbstractExpr(str) {};
    explicit ParamExpr(zend_string& str): AbstractExpr(std::string{str.val}) {};
    bool isMatch(const zend_arg_info* args, uint32_t num) const;
};

class MethodExpr: public AbstractExpr {
private:
    bool match_access(uint32_t) const;
    bool match_method(const std::string& method) const;
    bool match_return_type(zend_arg_info) const;
    bool match_return_type(zend_internal_arg_info) const;
protected:
    uint32_t acc;
    std::unique_ptr<std::string> return_type;
    ParamExpr *param_expr;
public:
    MethodExpr(const std::string& acc, const std::string& method,
                        const std::string& return_type, ParamExpr *param):

                        AbstractExpr(method), param_expr(param),
                        return_type(std::make_unique<std::string>(return_type)) {
        if (acc == "*") {
            this->acc = ZEND_ACC_PUBLIC | ZEND_ACC_PROTECTED | ZEND_ACC_PRIVATE;
        } else {
            auto access = str_to_lower(acc);
            if (access.find("public") != std::string::npos) {
                this->acc |= ZEND_ACC_PUBLIC;
            }
            if (access.find("protected") != std::string::npos) {
                this->acc |= ZEND_ACC_PROTECTED;
            }
            if (access.find("private") != std::string::npos) {
                this->acc |= ZEND_ACC_PRIVATE;
            }
        }
    };

    bool isMatch(const std::string& class_name, const std::string& method_name) const {
        if (isMatchAll()) {
            return true;
        }
        auto clazz = zend_fetch_class(str_to_zend(class_name), ZEND_FETCH_CLASS_AUTO);
        auto method = zend_hash_find(&clazz->function_table, str_to_zend(method_name));

        auto func = Z_FUNC_P(method);

        return match_access(func->common.fn_flags) && match_method(std::string{func->common.function_name->val})
            && param_expr->isMatch(func->common.arg_info, func->common.num_args)
            && ((func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) || match_return_type(func->common.arg_info[-1]));
    }
};
#endif //ASPECT_EXPR_H
