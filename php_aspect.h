/* aspect extension for PHP (c) 2021 xycc<xycc.invoker@gmail.com> */
#pragma once
#ifndef PHP_ASPECT_H
# define PHP_ASPECT_H
#include <expr.h>

extern "C" {
    extern zend_module_entry aspect_module_entry;
    # define phpext_aspect_ptr &aspect_module_entry

    ZEND_BEGIN_MODULE_GLOBALS(aspect)
        zval aspects;
        zend_bool enable_cache;
        // zval fit;
        // zval no_fit;
    ZEND_END_MODULE_GLOBALS(aspect);

    # define PHP_ASPECT_VERSION "0.1.0"

    # if defined(ZTS) && defined(COMPILE_DL_ASPECT)
    ZEND_TSRMLS_CACHE_EXTERN()
    # endif

    #define ASPECT_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(aspect, v)
}
#endif	/* PHP_ASPECT_H */
