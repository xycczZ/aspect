//
// Created by xycc on 6/20/21.
//

#include "signature.h"
zend_class_entry *signature_ce;

#define GETTER(v) zend_read_property(ZEND_THIS->value.obj->ce, ZEND_THIS->value.obj, #v, strlen(#v), true, nullptr)
#define SETTER_VAL(p, v) zend_update_property(ZEND_THIS->value.obj->ce, ZEND_THIS->value.obj, #p, strlen(#p), v)
#define SETTER_STR(p, v) zend_update_property_str(ZEND_THIS->value.obj->ce, ZEND_THIS->value.obj, #p, strlen(#p), v)

#define DECLARE_TYPED_PROP(name, type_mask) do { \
    zval default_val; \
    ZVAL_UNDEF(&default_val); \
    zend_string *argName = zend_string_init(#name, strlen(#name), true); \
    zend_declare_typed_property(signature_ce, argName, &default_val, ZEND_ACC_PRIVATE, nullptr, (zend_type) ZEND_TYPE_INIT_MASK(type_mask)); \
    zend_string_release(argName);                                  \
} while (0);

BEGIN_EXTERN_C()
static PHP_METHOD(signature, __construct);
static PHP_METHOD(signature, getClassName);
static PHP_METHOD(signature, getMethodName);
static PHP_METHOD(signature, getArgs);
END_EXTERN_C()

PHP_METHOD(signature, getClassName)
{
    auto prop = GETTER(className);
    RETURN_STR_COPY(Z_STR_P(prop));
}

PHP_METHOD(signature, getMethodName)
{
    auto prop = GETTER(methodName);
    RETURN_STR_COPY(Z_STR_P(prop));
}

PHP_METHOD(signature, getArgs)
{
    auto prop = GETTER(args);
    RETURN_ARR(Z_ARR_P(prop));
}

PHP_METHOD(signature, __construct)
{
    zend_string *class_name, *method_name;
    zval *args;
    ZEND_PARSE_PARAMETERS_START(3, 3)
            Z_PARAM_STR(class_name)
            Z_PARAM_STR(method_name)
            Z_PARAM_ARRAY(args)
    ZEND_PARSE_PARAMETERS_END();

    SETTER_STR(className, class_name);
    SETTER_STR(methodName, method_name);
    SETTER_VAL(args, args);
}

const static zend_function_entry signature_methods[] = {
    PHP_ME(signature, __construct, arginfo_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(signature, getClassName, arginfo_get_class_name, ZEND_ACC_PUBLIC)
    PHP_ME(signature, getMethodName, arginfo_get_method_name, ZEND_ACC_PUBLIC)
    PHP_ME(signature, getArgs, arginfo_get_args, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void init_signature_ce()
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Xycc\\Aspect\\Signature", signature_methods);
    signature_ce = zend_register_internal_class(&ce);

    DECLARE_TYPED_PROP(className, MAY_BE_STRING);
    DECLARE_TYPED_PROP(methodName, MAY_BE_STRING);
    DECLARE_TYPED_PROP(args, MAY_BE_ARRAY);
}