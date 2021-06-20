/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7eb3fd4083c98e6dffc8b02b6373b7ce9cbf228d */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_add_aspect, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, expr, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, advises, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, order, IS_LONG, 0, "10")
ZEND_END_ARG_INFO()

ZEND_FUNCTION(add_aspect);


static const zend_function_entry ext_functions[] = {
    ZEND_FE(add_aspect, arginfo_add_aspect)
	ZEND_FE_END
};
