//
// Created by xycc on 6/20/21.
//
#pragma once
#ifndef ASPECT_SIGNATURE_H
#define ASPECT_SIGNATURE_H

extern "C" {
#include "php.h"
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_sign_construct, 0, 0, 4)
    ZEND_ARG_TYPE_INFO(0, className, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, methodName, IS_STRING, 0)
    ZEND_ARG_ARRAY_INFO(0, args, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(arginfo_sign_get_class_name, IS_STRING, false)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(arginfo_sign_get_method_name, IS_STRING, false)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(arginfo_sign_get_args, IS_ARRAY, false)
ZEND_END_ARG_INFO()

#define GETTER(v) zend_read_property(ZEND_THIS->value.obj->ce, ZEND_THIS->value.obj, #v, strlen(#v), true, nullptr)
#define SETTER_VAL(p, v) zend_update_property(ZEND_THIS->value.obj->ce, ZEND_THIS->value.obj, #p, strlen(#p), v)
#define SETTER_STR(p, v) zend_update_property_str(ZEND_THIS->value.obj->ce, ZEND_THIS->value.obj, #p, strlen(#p), v)

#define DECLARE_TYPED_PROP_UNDEF_DEFAULT(ce, name, type_mask) do { \
    zval default_val; \
    ZVAL_UNDEF(&default_val); \
    DECLARE_TYPED_PROP_DEFAULT(ce, name, type_mask, &default_val); \
} while (0);

#define DECLARE_TYPED_PROP_DEFAULT(ce, name, type_mask, default_val) do { \
    zend_string *argName = zend_string_init(#name, strlen(#name), true); \
    zend_declare_typed_property(ce, argName, default_val, ZEND_ACC_PRIVATE, nullptr, (zend_type) ZEND_TYPE_INIT_MASK(type_mask)); \
    zend_string_release(argName); \
} while (0);

#define DECLARE_TYPED_PROP_UNDEF_DEFAULT_EX(ce, name, zt) do { \
    zval default_val; \
    ZVAL_UNDEF(&default_val); \
    zend_string *argName = zend_string_init(#name, strlen(#name), true); \
    zend_declare_typed_property(ce, argName, &default_val, ZEND_ACC_PRIVATE, nullptr, zt); \
    zend_string_release(argName); \
} while (0);

BEGIN_EXTERN_C()
static PHP_METHOD(signature, __construct);
static PHP_METHOD(signature, getClassName);
static PHP_METHOD(signature, getMethodName);
static PHP_METHOD(signature, getArgs);
END_EXTERN_C()

const static zend_function_entry signature_methods[] = {
    PHP_ME(signature, __construct, arginfo_sign_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(signature, getClassName, arginfo_sign_get_class_name, ZEND_ACC_PUBLIC)
    PHP_ME(signature, getMethodName, arginfo_sign_get_method_name, ZEND_ACC_PUBLIC)
    PHP_ME(signature, getArgs, arginfo_sign_get_args, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void init_signature_ce();

zval new_signature(zend_execute_data *);
#endif //ASPECT_SIGNATURE_H
