//
// Created by xycc on 6/21/21.
//

#pragma once
#ifndef ASPECT_JOIN_POINT_H
#define ASPECT_JOIN_POINT_H
extern "C" {
#include "php.h"
#include "zend_closures.h"
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_jp_construct, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, signature, Xycc\\Aspect\\Signature, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_jp_get_signature, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(arginfo_pjp_proceed, IS_MIXED, true)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pjp_construct, 0, 0, 2)
    ZEND_ARG_OBJ_INFO(0, signature, Xycc\\Aspect\\Signature, 0)
    ZEND_ARG_OBJ_INFO(0, origin, Closure, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO(arginfo_pjp_proceed_with, IS_MIXED, true)
    ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

void init_join_point();

BEGIN_EXTERN_C()
PHP_METHOD(JoinPoint, __construct);
PHP_METHOD(JoinPoint, getSignature);

PHP_METHOD(ProceedJoinPoint, __construct);
PHP_METHOD(ProceedJoinPoint, proceed);
PHP_METHOD(ProceedJoinPoint, proceedWith);
END_EXTERN_C()

const zend_function_entry join_point_ms[] = {
        PHP_ME(JoinPoint, __construct, arginfo_jp_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
        PHP_ME(JoinPoint, getSignature, arginfo_jp_get_signature, ZEND_ACC_PUBLIC)
        PHP_FE_END
};

const zend_function_entry pjpm[] = {
    PHP_ME(ProceedJoinPoint, __construct, arginfo_pjp_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(ProceedJoinPoint, proceed, arginfo_pjp_proceed, ZEND_ACC_PUBLIC)
    PHP_ME(ProceedJoinPoint, proceedWith, arginfo_pjp_proceed_with, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

auto create_join_point(zval *) -> zval;
#endif //ASPECT_JOIN_POINT_H
