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

void init_signature_ce();
#endif //ASPECT_SIGNATURE_H
