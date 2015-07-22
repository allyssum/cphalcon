
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#include "config/adapter/ini.h"
#include "config/adapter.h"
#include "config/adapterinterface.h"
#include "config/exception.h"
#include "pconfig.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/concat.h"
#include "kernel/exception.h"
#include "kernel/string.h"
#include "kernel/array.h"
#include "kernel/hash.h"
#include "kernel/operators.h"
#include "kernel/object.h"

/**
 * Phalcon\Config\Adapter\Ini
 *
 * Reads ini files and converts them to Phalcon\Config objects.
 *
 * Given the next configuration file:
 *
 *<code>
 *[database]
 *adapter = Mysql
 *host = localhost
 *username = scott
 *password = cheetah
 *dbname = test_db
 *
 *[phalcon]
 *controllersDir = "../app/controllers/"
 *modelsDir = "../app/models/"
 *viewsDir = "../app/views/"
 *</code>
 *
 * You can read it as follows:
 *
 *<code>
 *	$config = new Phalcon\Config\Adapter\Ini("path/config.ini");
 *	echo $config->phalcon->controllersDir;
 *	echo $config->database->username;
 *</code>
 *
 */
zend_class_entry *phalcon_config_adapter_ini_ce;

PHP_METHOD(Phalcon_Config_Adapter_Ini, read);

static const zend_function_entry phalcon_config_adapter_ini_method_entry[] = {
	PHP_ME(Phalcon_Config_Adapter_Ini, read, arginfo_phalcon_config_adapter_read, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static void phalcon_config_adapter_ini_update_zval_directive(zval **arr, zval *section, zval *directive, zval *value TSRMLS_DC)
{
	zval *t1, *t2;
	zval **temp1 = &t1, **temp2 = &t2, *index = NULL;
	int i, n;

	assert(Z_TYPE_PP(arr) == IS_ARRAY);
	assert(Z_TYPE_P(directive) == IS_ARRAY);

	n = zend_hash_num_elements(Z_ARRVAL_P(directive));
	assert(n > 1);

	if (!phalcon_array_isset_fetch(temp1, *arr, section)) {
		PHALCON_ALLOC_GHOST_ZVAL(t1);
		array_init_size(t1, 1);
		phalcon_array_update_zval(arr, section, t1, 0);
	}

	if (Z_TYPE_PP(temp1) != IS_ARRAY) {
		convert_to_array_ex(temp1);
	}

	for (i = 0; i < n - 1; i++) {
		phalcon_array_fetch_long(&index, directive, i, PH_NOISY);

		if (!phalcon_array_isset_fetch(temp2, *temp1, index)) {
			PHALCON_ALLOC_GHOST_ZVAL(t2);
			array_init_size(t2, 1);
			phalcon_array_update_zval(temp1, index, t2, 0);
		}
		else if (Z_TYPE_PP(temp2) != IS_ARRAY) {
			convert_to_array_ex(temp2);
		}

		t1 = t2;
		phalcon_ptr_dtor(&index);
	}

	phalcon_array_fetch_long(&index, directive, n - 1, PH_NOISY);
	phalcon_array_update_zval(temp1, index, value, PH_COPY);

	phalcon_ptr_dtor(&index);
}

/**
 * Phalcon\Config\Adapter\Ini initializer
 */
PHALCON_INIT_CLASS(Phalcon_Config_Adapter_Ini){

	PHALCON_REGISTER_CLASS_EX(Phalcon\\Config\\Adapter, Ini, config_adapter_ini, phalcon_config_adapter_ce, phalcon_config_adapter_ini_method_entry, 0);

	zend_class_implements(phalcon_config_adapter_ini_ce TSRMLS_CC, 1, phalcon_config_adapterinterface_ce);

	return SUCCESS;
}

/**
 * Load config file
 *
 * @param string $filePath
 */
PHP_METHOD(Phalcon_Config_Adapter_Ini, read){

	zval *file_path, *absolute_path = NULL, *config_dir_path, *base_path;
	zval *ini_config = NULL, *config, *directives = NULL;
	zval *section = NULL, *value = NULL, *key = NULL, *directive_parts = NULL;
	HashTable *ah0, *ah1;
	HashPosition hp0, hp1;
	zval **hd;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 1, &file_path, &absolute_path);
	PHALCON_ENSURE_IS_STRING(&file_path);

	if (absolute_path == NULL) {
		absolute_path = PHALCON_GLOBAL(z_false);
	}

	if (zend_is_true(absolute_path)) {
		PHALCON_CPY_WRT(config_dir_path, file_path);
	} else {
		base_path = phalcon_fetch_static_property_ce(phalcon_config_adapter_ce, SL("_basePath") TSRMLS_CC);

		PHALCON_INIT_VAR(config_dir_path);
		PHALCON_CONCAT_VV(config_dir_path, base_path, file_path);
	}

	/** 
	 * Use the standard parse_ini_file
	 */
	PHALCON_CALL_FUNCTION(&ini_config, "parse_ini_file", config_dir_path, PHALCON_GLOBAL(z_true));

	/** 
	 * Check if the file had errors
	 */
	if (Z_TYPE_P(ini_config) != IS_ARRAY) {
		zend_throw_exception_ex(phalcon_config_exception_ce, 0 TSRMLS_CC, "Configuration file '%s' cannot be read", Z_STRVAL_P(config_dir_path));
		PHALCON_MM_RESTORE();
		return;
	}

	PHALCON_INIT_VAR(config);
	array_init(config);

	phalcon_is_iterable(ini_config, &ah0, &hp0, 0, 0);

	while (zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) == SUCCESS) {

		PHALCON_GET_HKEY(section, ah0, hp0);
		PHALCON_GET_HVALUE(directives);

		if (unlikely(Z_TYPE_P(directives) != IS_ARRAY) || zend_hash_num_elements(Z_ARRVAL_P(directives)) == 0) {
			phalcon_array_update_zval(&config, section, directives, PH_COPY);
		} else {
			phalcon_is_iterable(directives, &ah1, &hp1, 0, 0);

			while (zend_hash_get_current_data_ex(ah1, (void**) &hd, &hp1) == SUCCESS) {

				PHALCON_GET_HKEY(key, ah1, hp1);
				PHALCON_GET_HVALUE(value);

				if (Z_TYPE_P(key) == IS_STRING && memchr(Z_STRVAL_P(key), '.', Z_STRLEN_P(key))) {
					PHALCON_INIT_NVAR(directive_parts);
					phalcon_fast_explode_str(directive_parts, SL("."), key);
					phalcon_config_adapter_ini_update_zval_directive(&config, section, directive_parts, value TSRMLS_CC);
				} else {
					phalcon_array_update_multi_2(&config, section, key, value, PH_COPY);
				}

				zend_hash_move_forward_ex(ah1, &hp1);
			}
		}

		zend_hash_move_forward_ex(ah0, &hp0);
	}

	if (Z_TYPE_P(config) == IS_ARRAY) {
		phalcon_config_construct_internal(getThis(), config TSRMLS_CC);
	}

	RETURN_THIS();
}
