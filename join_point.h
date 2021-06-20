//
// Created by xycc on 6/21/21.
//

#pragma once
#ifndef ASPECT_JOIN_POINT_H
#define ASPECT_JOIN_POINT_H
extern "C" {
#include "php.h"
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_jp_construct, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, signature, Xycc\\Aspect\\Signature, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_jp_get_signature, 0, 0, 0)
ZEND_END_ARG_INFO()

void init_join_point();

#endif //ASPECT_JOIN_POINT_H
