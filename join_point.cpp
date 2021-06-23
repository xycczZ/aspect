//
// Created by xycc on 6/21/21.
//

#include "join_point.h"
#include "signature.h"

zend_class_entry *join_point_ce;
extern zend_class_entry *signature_ce;
extern zend_class_entry *zend_ce_closure;
zend_class_entry *proceed_join_point_ce;

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

PHP_METHOD(ProceedJoinPoint, __construct)
{
    zval *sign;
    zval *origin_closure;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(sign, signature_ce)
        Z_PARAM_OBJECT_OF_CLASS(origin_closure, zend_ce_closure)
    ZEND_PARSE_PARAMETERS_END();

    zval *_this = getThis();
    zend_call_known_instance_method_with_1_params(_this->value.obj->ce->parent->constructor, Z_OBJ_P(_this), nullptr, sign);
    SETTER_VAL(origin, origin_closure);
}

PHP_METHOD(ProceedJoinPoint, proceed)
{
    auto this_ptr = Z_OBJ_P(getThis());
    auto origin = zend_string_init("origin", strlen("origin"), false);
    auto func = zend_get_closure_invoke_method(Z_OBJ_P(zend_read_property_ex(this_ptr->ce, this_ptr, origin, false, nullptr)));

    // args
    auto func_name = zend_string_init("getArgs", strlen("getArgs"), false);
    auto get_args = zend_hash_find(&this_ptr->ce->parent->function_table, func_name);
    zval *args;
    zend_call_known_instance_method_with_0_params(Z_FUNC_P(get_args), this_ptr, args);

    // call origin method
    zend_call_known_function(func, nullptr, func->common.scope, return_value, func->common.num_args, args, nullptr);
//    RETURN_ZVAL(retval, true, false);
}

PHP_METHOD(ProceedJoinPoint, proceedWith)
{
    zval *dest;
    uint32_t dest_num;
    zend_array *dest_named;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_VARIADIC_WITH_NAMED(dest, dest_num, dest_named)
    ZEND_PARSE_PARAMETERS_END();

    auto this_ptr = Z_OBJ_P(getThis());
    auto origin = zend_string_init("origin", strlen("origin"), false);
    auto func = zend_get_closure_invoke_method(Z_OBJ_P(zend_read_property_ex(this_ptr->ce, this_ptr, origin, false, nullptr)));

    zend_call_known_function(func, nullptr, func->common.scope, return_value, dest_num, dest, dest_named);
}

void init_join_point()
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Xycc\\Aspect\\JoinPoint", join_point_ms);
    join_point_ce = zend_register_internal_class(&ce);

    zend_type zt;
    ZEND_TYPE_SET_CE(zt, signature_ce);
    DECLARE_TYPED_PROP_UNDEF_DEFAULT_EX(join_point_ce, signature, zt)


    zend_class_entry pce;
    INIT_CLASS_ENTRY(pce, "Xycc\\Aspect\\ProceedJoinPoint", pjpm);
    proceed_join_point_ce = zend_register_internal_class_ex(&pce, join_point_ce);

    zend_type zt_closure;
    ZEND_TYPE_SET_CE(zt_closure, zend_ce_closure);
    DECLARE_TYPED_PROP_UNDEF_DEFAULT_EX(proceed_join_point_ce, origin, zt_closure);

    zend_declare_property_bool(proceed_join_point_ce, "isVoid", strlen("isVoid"), 0, ZEND_ACC_PRIVATE);

    zval result;
    ZVAL_UNDEF(&result);
    zend_declare_property(proceed_join_point_ce, "result", strlen("result"), &result, ZEND_ACC_PRIVATE);
}