/* aspect extension for PHP (c) 2021 xycc<xycc.invoker@gmail.com> */
#pragma once
#ifndef PHP_ASPECT_H
# define PHP_ASPECT_H
#include "expr.h"
#include "signature.h"
#include "join_point.h"

extern "C" {
    extern zend_module_entry aspect_module_entry;
    # define phpext_aspect_ptr &aspect_module_entry

    ZEND_BEGIN_MODULE_GLOBALS(aspect)
        zval aspects;
        zend_bool enable_cache;
        // user_opcode_handler_t origin;
        // user_opcode_handler_t origin_name;
        // zval fit;
        // zval no_fit;
    ZEND_END_MODULE_GLOBALS(aspect);

    # define PHP_ASPECT_VERSION "0.1.0"

    # if defined(ZTS) && defined(COMPILE_DL_ASPECT)
    ZEND_TSRMLS_CACHE_EXTERN()
    # endif

    #define ASPECT_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(aspect, v)

}

#define FETCH_RUNTIME_E (zend_fetch_class(zend_string_init_interned("RuntimeException", strlen("RuntimeException"), 1), ZEND_FETCH_CLASS_EXCEPTION))
#define PERMANENT_STR(s) zend_string_init_interned(#s, strlen(#s), true)

int compare_aop(Bucket*, Bucket*);

void call_before(Bucket*, zend_string*, zval*);
void call_around(Bucket*, zend_string*, zval*);
void call_after_returning(Bucket*, zend_string*);
void call_after_throwing(Bucket*, zend_string*);
void call_after(Bucket*, zend_string*);
#endif	/* PHP_ASPECT_H */
