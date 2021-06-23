//
// Created by xycc on 6/20/21.
//

#include "signature.h"
zend_class_entry *signature_ce;

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

void init_signature_ce()
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Xycc\\Aspect\\Signature", signature_methods);
    signature_ce = zend_register_internal_class(&ce);

    DECLARE_TYPED_PROP_UNDEF_DEFAULT(signature_ce, className, MAY_BE_STRING)
    DECLARE_TYPED_PROP_UNDEF_DEFAULT(signature_ce, methodName, MAY_BE_STRING)
    DECLARE_TYPED_PROP_UNDEF_DEFAULT(signature_ce, args, MAY_BE_ARRAY)
}

zval new_signature(zend_execute_data *execute_data)
{
    zval signature;

    zval clazz;
    ZVAL_STR_COPY(&clazz, execute_data->call->func->common.scope->name);
    zval method;
    ZVAL_STR_COPY(&method, execute_data->call->func->common.function_name);
    zval index_args;
    array_init_size(&index_args, ZEND_CALL_NUM_ARGS(execute_data->call));


    for (uint32_t i = 0; i < ZEND_CALL_NUM_ARGS(execute_data->call); ++i) {
        auto arg = ZEND_CALL_VAR_NUM(execute_data->call, i);
        add_index_zval(&index_args, i, arg);
    }

    zval args[3] = {
            clazz, method, index_args
    };

    object_init_ex(&signature, signature_ce);

    zend_call_known_function(signature_ce->constructor, Z_OBJ(signature), Z_OBJCE(signature), nullptr, 3, args, nullptr);

    return signature;
}