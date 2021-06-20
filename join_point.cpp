//
// Created by xycc on 6/21/21.
//

#include "join_point.h"
#include "signature.h"

zend_class_entry *join_point_ce;
extern zend_class_entry *signature_ce;

BEGIN_EXTERN_C()
static PHP_METHOD(JoinPoint, __construct);
static PHP_METHOD(JoinPoint, getSignature);
END_EXTERN_C()

static const zend_function_entry join_point_ms[] = {
        PHP_ME(JoinPoint, __construct, arginfo_jp_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
        PHP_ME(JoinPoint, getSignature, arginfo_jp_get_signature, ZEND_ACC_PUBLIC)
        PHP_FE_END
};

PHP_METHOD(JoinPoint, __construct)
{
    zval *sign;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(sign, signature_ce)
    ZEND_PARSE_PARAMETERS_END();

    SETTER_VAL(signature, sign);
}

PHP_METHOD(JoinPoint, getSignature)
{
    auto p = GETTER(signature);
    RETURN_OBJ(Z_OBJ_P(p));
}

void init_join_point()
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Xycc\\Aspect\\JoinPoint", join_point_ms);
    join_point_ce = zend_register_internal_class(&ce);

    zend_type zt;
    ZEND_TYPE_SET_CE(zt, signature_ce);
    DECLARE_TYPED_PROP_UNDEF_DEFAULT_EX(join_point_ce, signature, zt)
}