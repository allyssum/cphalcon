
#ifdef HAVE_CONFIG_H
#include "../../../ext_config.h"
#endif

#include <php.h>
#include "../../../php_ext.h"
#include "../../../ext.h"

#include <Zend/zend_operators.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/object.h"
#include "ext/spl/spl_exceptions.h"
#include "kernel/exception.h"
#include "kernel/operators.h"
#include "kernel/string.h"
#include "kernel/fcall.h"
#include "kernel/array.h"

ZEPHIR_INIT_CLASS(Phalcon_Mvc_Model_Validator) {

	ZEPHIR_REGISTER_CLASS(Phalcon\\Mvc\\Model, Validator, phalcon, mvc_model_validator, phalcon_mvc_model_validator_method_entry, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

	zend_declare_property_null(phalcon_mvc_model_validator_ce, SL("_options"), ZEND_ACC_PROTECTED TSRMLS_CC);

	zend_declare_property_null(phalcon_mvc_model_validator_ce, SL("_messages"), ZEND_ACC_PROTECTED TSRMLS_CC);

	return SUCCESS;

}

/**
 * Phalcon\Mvc\Model\Validator constructor
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, __construct) {

	zval *options_param = NULL, *_0;
	zval *options = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &options_param);

	options = options_param;



	ZEPHIR_INIT_VAR(_0);
	array_init(_0);
	zephir_update_property_this(this_ptr, SL("_messages"), _0 TSRMLS_CC);
	zephir_update_property_this(this_ptr, SL("_options"), options TSRMLS_CC);
	ZEPHIR_MM_RESTORE();

}

/**
 * Appends a message to the validator
 *
 * @param string message
 * @param string|array field
 * @param string type
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, appendMessage) {

	int ZEPHIR_LAST_CALL_STATUS;
	zval *message_param = NULL, *field = NULL, *type = NULL, *_0 = NULL, _1, _2;
	zval *message = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 2, &message_param, &field, &type);

	if (unlikely(Z_TYPE_P(message_param) != IS_STRING && Z_TYPE_P(message_param) != IS_NULL)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'message' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (likely(Z_TYPE_P(message_param) == IS_STRING)) {
		zephir_get_strval(message, message_param);
	} else {
		ZEPHIR_INIT_VAR(message);
		ZVAL_EMPTY_STRING(message);
	}
	if (!field) {
		field = ZEPHIR_GLOBAL(global_null);
	}
	if (!type) {
		ZEPHIR_CPY_WRT(type, ZEPHIR_GLOBAL(global_null));
	} else {
		ZEPHIR_SEPARATE_PARAM(type);
	}


	if (!(zephir_is_true(type))) {
		ZEPHIR_INIT_VAR(_0);
		zephir_get_class(_0, this_ptr, 0 TSRMLS_CC);
		ZEPHIR_SINIT_VAR(_1);
		ZVAL_STRING(&_1, "Validator", 0);
		ZEPHIR_SINIT_VAR(_2);
		ZVAL_STRING(&_2, "", 0);
		ZEPHIR_INIT_NVAR(type);
		zephir_fast_str_replace(&type, &_1, &_2, _0 TSRMLS_CC);
	}
	ZEPHIR_INIT_NVAR(_0);
	object_init_ex(_0, phalcon_mvc_model_message_ce);
	ZEPHIR_CALL_METHOD(NULL, _0, "__construct", NULL, 296, message, field, type);
	zephir_check_call_status();
	zephir_update_property_array_append(this_ptr, SL("_messages"), _0 TSRMLS_CC);
	ZEPHIR_MM_RESTORE();

}

/**
 * Returns messages generated by the validator
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, getMessages) {


	RETURN_MEMBER(this_ptr, "_messages");

}

/**
 * Returns all the options from the validator
 *
 * @return array
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, getOptions) {


	RETURN_MEMBER(this_ptr, "_options");

}

/**
 * Returns an option
 *
 * @param	string option
 * @return	mixed
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, getOption) {

	zval *option_param = NULL, *options, *value;
	zval *option = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &option_param);

	if (unlikely(Z_TYPE_P(option_param) != IS_STRING && Z_TYPE_P(option_param) != IS_NULL)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'option' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (likely(Z_TYPE_P(option_param) == IS_STRING)) {
		zephir_get_strval(option, option_param);
	} else {
		ZEPHIR_INIT_VAR(option);
		ZVAL_EMPTY_STRING(option);
	}


	options = zephir_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	if (zephir_array_isset_fetch(&value, options, option, 1 TSRMLS_CC)) {
		RETURN_CTOR(value);
	}
	RETURN_MM_STRING("", 1);

}

/**
 * Check whether a option has been defined in the validator options
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, isSetOption) {

	zval *option_param = NULL, *_0;
	zval *option = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &option_param);

	if (unlikely(Z_TYPE_P(option_param) != IS_STRING && Z_TYPE_P(option_param) != IS_NULL)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'option' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (likely(Z_TYPE_P(option_param) == IS_STRING)) {
		zephir_get_strval(option, option_param);
	} else {
		ZEPHIR_INIT_VAR(option);
		ZVAL_EMPTY_STRING(option);
	}


	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	RETURN_MM_BOOL(zephir_array_isset(_0, option));

}

