/* aspect extension for PHP (c) 2021 xycc<xycc.invoker@gmail.com> */

extern "C" {
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "aspect_arginfo.h"
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
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR(expr)
        Z_PARAM_ARRAY(advises)
    ZEND_PARSE_PARAMETERS_END();

    zend_long h;
    zend_string *key;
    static const std::vector<std::string> valid_keys {"before", "after", "afterReturning", "afterThrowing", "around"};

    // 检查元素， 必须是一个二维数组， 第一维的key是通知类型， 第二维的元素是通知方法
    ZEND_HASH_FOREACH_KEY(Z_ARR_P(advises), h, key)
        std::string s{key->val};
        if (std::find(valid_keys.begin(), valid_keys.end(), s) == valid_keys.end()) {
            RETURN_FALSE;
        }
        if (Z_TYPE_P(_z) != IS_ARRAY) {
            RETURN_FALSE;
        }

        zval *handler;
        ZEND_HASH_FOREACH_VAL(Z_ARR_P(_z), handler)
            if (Z_TYPE_P(handler) != IS_CALLABLE) {
                RETURN_FALSE;
            }
        ZEND_HASH_FOREACH_END();
    ZEND_HASH_FOREACH_END();

    // 然后加入全局变量中
    zend_hash_update(Z_ARR(ASPECT_G(aspects)), expr, advises);
    RETURN_TRUE;
}
/* }}} */

/**
 * 当执行一个函数的时候， 检查类和方法是否符合全局变量$_ASPECT的切点表达式 常量复杂度， 如果启用了缓存功能，先检查缓存
 * 如果启用了缓存功能， 缓存符合条件的类::方法到 $_CACHE_AOP 这个数组中去, 缓存不符合条件的类::方法到$_NO_CACHE_AOP这个数组去
 */
static int inject(zend_execute_data *execute_data)
{
    if (execute_data->call->func->type != ZEND_INTERNAL_FUNCTION) {
        if (execute_data->call->func->common.scope != nullptr) {
            char *class_name = execute_data->call->func->common.scope->name->val;
            std::string dot_name{class_name};
            std::replace(dot_name.begin(), dot_name.end(), '\\', '.');
            php_printf("call call: %s::%s\n", dot_name.c_str(), execute_data->call->func->common.function_name->val);
        } else {
            php_printf("call call: %s\n", execute_data->call->func->common.function_name->val);
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
