/* aspect extension for PHP (c) 2021 xycc<xycc.invoker@gmail.com> */

extern "C" {
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "aspect_arginfo.h"
#include "zend_interfaces.h"
#include "zend_closures.h"
#include "zend_exceptions.h"
}
#include "php_aspect.h"
#include <algorithm>
#include <vector>
#include <string>

ZEND_DECLARE_MODULE_GLOBALS(aspect)

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ bool add_aspect(string $expr, array $advises) */
PHP_FUNCTION(add_aspect)
{
    zend_string *expr;
    zval *advises;
	zend_long order = 10;
    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_STR(expr)
        Z_PARAM_ARRAY(advises)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(order)
    ZEND_PARSE_PARAMETERS_END();

    zend_long h;
    zend_string *key;
    static const std::vector<std::string> valid_keys {"before", "after", "afterReturning", "afterThrowing", "around"};

    // 检查元素， 必须是一个二维数组， 第一维的key是通知类型， 第二维的元素是通知方法
    ZEND_HASH_FOREACH_KEY(Z_ARR_P(advises), h, key)
        std::string s{key->val};
        if (std::find(valid_keys.begin(), valid_keys.end(), s) == valid_keys.end()) {
            auto msg = "invalid advise type: '" + s + "', must in ['before', 'after', 'afterReturning', 'afterThrowing', 'around']";
            zend_throw_exception(FETCH_RUNTIME_E, msg.c_str(), 0);
            RETURN_FALSE;
        }
        if (Z_TYPE_P(_z) != IS_ARRAY) {
            zend_throw_exception(FETCH_RUNTIME_E, "advises need advise array", 1);
            RETURN_FALSE;
        }

        zval *handler;
        ZEND_HASH_FOREACH_VAL(Z_ARR_P(_z), handler)
            if (! zend_is_callable(handler, IS_CALLABLE_CHECK_SILENT, nullptr)) {
                zend_throw_exception(FETCH_RUNTIME_E, "advise must be callable", 2);
                RETURN_FALSE;
            }
        ZEND_HASH_FOREACH_END();
    ZEND_HASH_FOREACH_END();

    // 然后加入全局变量中
	add_assoc_long(advises, "order", order);
    zend_hash_update(Z_ARR(ASPECT_G(aspects)), expr, advises);
	// 排序
	zend_hash_sort(Z_ARR(ASPECT_G(aspects)), compare_aop, 0);
    Z_TRY_ADDREF_P(advises);
    // 生成切点表达式
//    exprs.push_back(new Expr(std::string{expr->val}));
    RETURN_TRUE;
}
/* }}} */

static zval fit(const std::string& class_name, const std::string& method_name)
{
    Bucket *item;
    zval arr;
    array_init(&arr);
    ZEND_HASH_FOREACH_BUCKET(Z_ARR(ASPECT_G(aspects)), item)
        // key pointcut expr, values=> before=>, after=>
        auto expr = new Expr(std::string{item->key->val});
        if (expr->isMatch(class_name, method_name)) {
            zend_hash_next_index_insert(Z_ARR(arr), _z);
            Z_TRY_ADDREF_P(_z);
        }
    ZEND_HASH_FOREACH_END();

    return arr;
}

/**
 * 当执行一个函数的时候， 检查类和方法是否符合全局变量$_ASPECT的切点表达式 常量复杂度， 如果启用了缓存功能，先检查缓存
 * 如果启用了缓存功能， 缓存符合条件的类::方法到 $_CACHE_AOP 这个数组中去, 缓存不符合条件的类::方法到$_NO_CACHE_AOP这个数组去
 */
static int inject(zend_execute_data *execute_data)
{
    if (ASPECT_G(aspects).value.arr->nNumOfElements == 0) {
        return ZEND_USER_OPCODE_DISPATCH;
    }

    if (execute_data->call->func->type != ZEND_INTERNAL_FUNCTION) {
        if (execute_data->call->func->common.scope != nullptr) {
            char *class_name = execute_data->call->func->common.scope->name->val;
            std::string dot_name{class_name};
            std::replace(dot_name.begin(), dot_name.end(), '\\', '.');
            // 然后判断当前执行的类和方法符合不符合切点表达式
            auto cb = fit(dot_name, std::string{execute_data->call->func->common.function_name->val});
            if (zend_array_count(Z_ARR(cb)) != 0) {
                // before
				auto before_key = PERMANENT_STR(before);
				auto after_key = PERMANENT_STR(after);
				auto after_returning_key = PERMANENT_STR(afterReturning);
				auto after_throwing_key = PERMANENT_STR(afterThrowing);
				auto around_key = PERMANENT_STR(around);

                Bucket *cbs;
                ZEND_HASH_FOREACH_BUCKET(Z_ARR(cb), cbs)
                    call_before(cbs, before_key);
                ZEND_HASH_FOREACH_END();

//                ZEND_HASH_FOREACH_BUCKET(Z_ARR(cb), cbs)
//                    call_around(cbs, around_key);
//                ZEND_HASH_FOREACH_END();
            }
			// execute origin function
			zval retval;
			zend_execute(&execute_data->call->func->op_array, &retval);
			php_printf("done!\n");
			execute_data->call->return_value = &retval;
			Z_TRY_ADDREF(retval);
			php_printf("really done!\n");
        } else {
            php_printf("call: %s\n", execute_data->call->func->common.function_name->val);
        }
    }
	return ZEND_USER_OPCODE_DISPATCH;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(aspect)
{
#if defined(ZTS) && defined(COMPILE_DL_ASPECT)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

    zend_set_user_opcode_handler(ZEND_DO_FCALL, inject);
    zend_set_user_opcode_handler(ZEND_DO_FCALL_BY_NAME, inject);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(aspect)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "aspect support", "enabled");
	php_info_print_table_end();
}
/* }}} */

static zend_bool create_aspect_cb(zend_string *name)
{
    if (Z_TYPE(ASPECT_G(aspects)) == IS_UNDEF) {
        array_init(&ASPECT_G(aspects));

        zend_hash_update(&EG(symbol_table), name, &ASPECT_G(aspects));
        Z_ADDREF(ASPECT_G(aspects));
    }

    return true;
}

PHP_MINIT_FUNCTION(aspect)
{
    init_signature_ce();
    init_join_point();
    zend_register_auto_global(zend_string_init_interned("_ASPECT", strlen("_ASPECT"), true), false, create_aspect_cb);
    return SUCCESS;
}

/* {{{ aspect_module_entry */
zend_module_entry aspect_module_entry = {
	STANDARD_MODULE_HEADER,
	"aspect",					/* Extension name */
	ext_functions,					/* zend_function_entry */
	PHP_MINIT(aspect),							/* PHP_MINIT - Module initialization */
    nullptr,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(aspect),			/* PHP_RINIT - Request initialization */
	nullptr,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(aspect),			/* PHP_MINFO - Module info */
	PHP_ASPECT_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ASPECT
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(aspect)
#endif


void call_before(Bucket *cbs, zend_string *before_key)
{
	auto before = zend_hash_find(Z_ARR(cbs->val), before_key);
	if (before != nullptr) {
		Bucket *item;
		ZEND_HASH_FOREACH_BUCKET(Z_ARR_P(before), item)
			auto item_type = Z_TYPE(item->val);
			if (item_type == IS_OBJECT) {
				auto obj = Z_OBJ(item->val);
				zend_class_entry *ce;
				zend_function *fe;
				zend_object *self;
				auto res = zend_std_get_closure(obj, &ce, &fe, &self, true);
				if (res == FAILURE) {
					auto func = zend_get_closure_invoke_method(item->val.value.obj);
					zend_call_known_function(func, item->val.value.obj, item->val.value.obj->ce, nullptr, 0, nullptr, nullptr);
				} else {
					zend_call_known_function(fe, self, ce, nullptr, 0, nullptr, nullptr);
				}
			} else if (item_type == IS_ARRAY) {
				auto cb = Z_ARR(item->val);
				auto first = zend_hash_index_find(cb, 0);
				auto method = zend_hash_index_find(cb, 1);
				zval *func;
				zend_object *obj;
				zend_class_entry *scope;
				if (Z_TYPE_P(first) == IS_OBJECT) {
					obj = Z_OBJ_P(first);
					scope = obj->ce;
					func = zend_hash_find(&Z_OBJ_P(first)->ce->function_table, Z_STR_P(method));
				} else {
					obj = nullptr;
					scope = zend_fetch_class(Z_STR_P(first), ZEND_FETCH_CLASS_AUTO);
					func = zend_hash_find(&scope->function_table, Z_STR_P(method));
				}
				zend_call_known_function(Z_FUNC_P(func), obj, scope, nullptr, 0, nullptr, nullptr);
			}
		ZEND_HASH_FOREACH_END();
	}
}

int compare_aop(Bucket* a, Bucket* b)
{
	php_printf("bucket type: %d\n", Z_TYPE(a->val));
	zval *ao = zend_hash_str_find(Z_ARR(a->val), "order", strlen("order"));
	zval *bo = zend_hash_str_find(Z_ARR(b->val), "order", strlen("order"));
	
	if (Z_LVAL_P(ao) == Z_LVAL_P(bo)) {
		return 0;
	}

	return Z_LVAL_P(ao) > Z_LVAL_P(bo) ? 1 : -1;
}