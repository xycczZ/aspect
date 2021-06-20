//
// Created by xycc on 6/20/21.
//
#pragma once
#ifndef ASPECT_SIGNATURE_H
#define ASPECT_SIGNATURE_H

extern "C" {
#include "php.h"
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_construct, 0, 0, 3)
    ZEND_ARG_TYPE_INFO(0, className, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, methodName, IS_STRING, 0)
    ZEND_ARG_ARRAY_INFO(0, args, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_class_name, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_method_name, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_args, 0, 0, 0)
ZEND_END_ARG_INFO()

#define GETTER(v) zend_read_property(ZEND_THIS->value.obj->ce, ZEND_THIS->value.obj, #v, strlen(#v), true, nullptr)
#define SETTER_VAL(p, v) zend_update_property(ZEND_THIS->value.obj->ce, ZEND_THIS->value.obj, #p, strlen(#p), v)
#define SETTER_STR(p, v) zend_update_property_str(ZEND_THIS->value.obj->ce, ZEND_THIS->value.obj, #p, strlen(#p), v)

#define DECLARE_TYPED_PROP_UNDEF_DEFAULT(ce, name, type_mask) do { \
    zval default_val; \
    ZVAL_UNDEF(&default_val); \
    zend_string *argName = zend_string_init(#name, strlen(#name), true); \
    zend_declare_typed_property(ce, argName, &default_val, ZEND_ACC_PRIVATE, nullptr, (zend_type) ZEND_TYPE_INIT_MASK(type_mask)); \
    zend_string_release(argName); \
} while (0);

#define DECLARE_TYPED_PROP_UNDEF_DEFAULT_EX(ce, name, zt) do { \
    zval default_val; \
    ZVAL_UNDEF(&default_val); \
    zend_string *argName = zend_string_init(#name, strlen(#name), true); \
    zend_declare_typed_property(ce, argName, &default_val, ZEND_ACC_PRIVATE, nullptr, zt); \
    zend_string_release(argName); \
} while (0);

void init_signature_ce();
#endif //ASPECT_SIGNATURE_H
