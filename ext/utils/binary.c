/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2013 Phalcon Team (http://www.phalconphp.com)       |
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
  |          ZhuZongXin <dreamsxin@qq.com>                                 |
  +------------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef PHALCON_USE_PNG
#include <png.h>
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "php_open_temporary_file.h"

#include "kernel/main.h"
#include "kernel/memory.h"

#include "kernel/exception.h"
#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/array.h"
#include "kernel/hash.h"
#include "kernel/concat.h"
#include "kernel/operators.h"
#include "kernel/string.h"
#include "kernel/output.h"

#include "utils/binary.h"

/* Whether machine is little endian */
char machine_little_endian;

/* Mapping of byte from char (8bit) to long for machine endian */
static int byte_map[1];

/* Mappings of bytes from int (machine dependent) to int for machine endian */
static int int_map[sizeof(int)];

/* Mappings of bytes from shorts (16bit) for all endian environments */
static int machine_endian_short_map[2];
static int big_endian_short_map[2];
static int little_endian_short_map[2];

/* Mappings of bytes from longs (32bit) for all endian environments */
static int machine_endian_long_map[4];
static int big_endian_long_map[4];
static int little_endian_long_map[4];

#if SIZEOF_ZEND_LONG > 4
/* Mappings of bytes from quads (64bit) for all endian environments */
static int machine_endian_longlong_map[8];
static int big_endian_longlong_map[8];
static int little_endian_longlong_map[8];
#endif

/**
 * Phalcon\Utils\Binary
 * 
 *<code>
 * $binary = new \Phalcon\Utils\Binary::readSignedShort($data);
 * $short = $binary->readSignedShort($data);
 *</code>
 */
zend_class_entry *phalcon_utils_binary_ce;

PHP_METHOD(Phalcon_Utils_Binary, __construct);
PHP_METHOD(Phalcon_Utils_Binary, readSignedShort);
PHP_METHOD(Phalcon_Utils_Binary, readInt);
PHP_METHOD(Phalcon_Utils_Binary, readBool);
PHP_METHOD(Phalcon_Utils_Binary, readByte);
PHP_METHOD(Phalcon_Utils_Binary, readShort);
PHP_METHOD(Phalcon_Utils_Binary, readFloat);
PHP_METHOD(Phalcon_Utils_Binary, readDouble);

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_utils_binary___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

static const zend_function_entry phalcon_utils_binary_method_entry[] = {
	PHP_ME(Phalcon_Utils_Binary, __construct, arginfo_phalcon_utils_binary___construct, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL|ZEND_ACC_CTOR)
	PHP_ME(Phalcon_Utils_Binary, readSignedShort, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Utils_Binary, readInt, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Utils_Binary, readBool, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Utils_Binary, readByte, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Utils_Binary, readShort, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Utils_Binary, readFloat, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Utils_Binary, readDouble, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Phalcon\Utils\Binary initializer
 */
PHALCON_INIT_CLASS(Phalcon_Utils_Binary){

	PHALCON_REGISTER_CLASS(Phalcon\\Utils, Binary, utils_binary, phalcon_utils_binary_method_entry, 0);

	zend_declare_property_null(phalcon_utils_binary_ce, SL("_data"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(phalcon_utils_binary_ce, SL("_pos"), 0, ZEND_ACC_PROTECTED TSRMLS_CC);

	return SUCCESS;
}

/**
 * Phalcon\Mvc\Model constructor
 *
 * @param Phalcon\DiInterface $dependencyInjector
 * @param Phalcon\Mvc\Model\ManagerInterface $modelsManager
 */
PHP_METHOD(Phalcon_Utils_Binary, __construct){

	zval *data = NULL;
	int machine_endian_check = 1;
	int i;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &data);

	phalcon_update_property_this(this_ptr, SL("_data"), data TSRMLS_CC);

	machine_little_endian = ((char *)&machine_endian_check)[0];

	if (machine_little_endian) {
		/* Where to get lo to hi bytes from */
		byte_map[0] = 0;

		for (i = 0; i < (int)sizeof(int); i++) {
			int_map[i] = i;
		}

		machine_endian_short_map[0] = 0;
		machine_endian_short_map[1] = 1;
		big_endian_short_map[0] = 1;
		big_endian_short_map[1] = 0;
		little_endian_short_map[0] = 0;
		little_endian_short_map[1] = 1;

		machine_endian_long_map[0] = 0;
		machine_endian_long_map[1] = 1;
		machine_endian_long_map[2] = 2;
		machine_endian_long_map[3] = 3;
		big_endian_long_map[0] = 3;
		big_endian_long_map[1] = 2;
		big_endian_long_map[2] = 1;
		big_endian_long_map[3] = 0;
		little_endian_long_map[0] = 0;
		little_endian_long_map[1] = 1;
		little_endian_long_map[2] = 2;
		little_endian_long_map[3] = 3;

#if SIZEOF_ZEND_LONG > 4
		machine_endian_longlong_map[0] = 0;
		machine_endian_longlong_map[1] = 1;
		machine_endian_longlong_map[2] = 2;
		machine_endian_longlong_map[3] = 3;
		machine_endian_longlong_map[4] = 4;
		machine_endian_longlong_map[5] = 5;
		machine_endian_longlong_map[6] = 6;
		machine_endian_longlong_map[7] = 7;
		big_endian_longlong_map[0] = 7;
		big_endian_longlong_map[1] = 6;
		big_endian_longlong_map[2] = 5;
		big_endian_longlong_map[3] = 4;
		big_endian_longlong_map[4] = 3;
		big_endian_longlong_map[5] = 2;
		big_endian_longlong_map[6] = 1;
		big_endian_longlong_map[7] = 0;
		little_endian_longlong_map[0] = 0;
		little_endian_longlong_map[1] = 1;
		little_endian_longlong_map[2] = 2;
		little_endian_longlong_map[3] = 3;
		little_endian_longlong_map[4] = 4;
		little_endian_longlong_map[5] = 5;
		little_endian_longlong_map[6] = 6;
		little_endian_longlong_map[7] = 7;
#endif
	}
	else {
		zval val;
		int size = sizeof(Z_LVAL(val));
		Z_LVAL(val)=0; /*silence a warning*/

		/* Where to get hi to lo bytes from */
		byte_map[0] = size - 1;

		for (i = 0; i < (int)sizeof(int); i++) {
			int_map[i] = size - (sizeof(int) - i);
		}

		machine_endian_short_map[0] = size - 2;
		machine_endian_short_map[1] = size - 1;
		big_endian_short_map[0] = size - 2;
		big_endian_short_map[1] = size - 1;
		little_endian_short_map[0] = size - 1;
		little_endian_short_map[1] = size - 2;

		machine_endian_long_map[0] = size - 4;
		machine_endian_long_map[1] = size - 3;
		machine_endian_long_map[2] = size - 2;
		machine_endian_long_map[3] = size - 1;
		big_endian_long_map[0] = size - 4;
		big_endian_long_map[1] = size - 3;
		big_endian_long_map[2] = size - 2;
		big_endian_long_map[3] = size - 1;
		little_endian_long_map[0] = size - 1;
		little_endian_long_map[1] = size - 2;
		little_endian_long_map[2] = size - 3;
		little_endian_long_map[3] = size - 4;

#if SIZEOF_ZEND_LONG > 4
		machine_endian_longlong_map[0] = size - 8;
		machine_endian_longlong_map[1] = size - 7;
		machine_endian_longlong_map[2] = size - 6;
		machine_endian_longlong_map[3] = size - 5;
		machine_endian_longlong_map[4] = size - 4;
		machine_endian_longlong_map[5] = size - 3;
		machine_endian_longlong_map[6] = size - 2;
		machine_endian_longlong_map[7] = size - 1;
		big_endian_longlong_map[0] = size - 8;
		big_endian_longlong_map[1] = size - 7;
		big_endian_longlong_map[2] = size - 6;
		big_endian_longlong_map[3] = size - 5;
		big_endian_longlong_map[4] = size - 4;
		big_endian_longlong_map[5] = size - 3;
		big_endian_longlong_map[6] = size - 2;
		big_endian_longlong_map[7] = size - 1;
		little_endian_longlong_map[0] = size - 1;
		little_endian_longlong_map[1] = size - 2;
		little_endian_longlong_map[2] = size - 3;
		little_endian_longlong_map[3] = size - 4;
		little_endian_longlong_map[4] = size - 5;
		little_endian_longlong_map[5] = size - 6;
		little_endian_longlong_map[6] = size - 7;
		little_endian_longlong_map[7] = size - 8;
#endif
	}

	PHALCON_MM_RESTORE();
}

/* {{{ php_pack
 */
static void php_pack(zval **val, int size, int *map, char *output)
{
	int i;
	char *v;

	convert_to_long_ex(val);
	v = (char *) &Z_LVAL_PP(val);

	for (i = 0; i < size; i++) {
		*output++ = v[map[i]];
	}
}
/* }}} */

/* {{{ php_unpack
 */
static long php_unpack(char *data, int size, int issigned, int *map)
{
	long result;
	char *cresult = (char *) &result;
	int i;

	result = issigned ? -1 : 0;

	for (i = 0; i < size; i++) {
		cresult[map[i]] = *data++;
	}

	return result;
}
/* }}} */

/**
 *
 * @return int
 */
PHP_METHOD(Phalcon_Utils_Binary, readSignedShort){

	zval *data, *pos;
	char *format, *input;
	int inputpos, v, issigned = 0, size = 2;

	PHALCON_MM_GROW();

	PHALCON_OBS_VAR(data);
	phalcon_read_property_this(&data, this_ptr, SL("_data"), PH_NOISY TSRMLS_CC);

	PHALCON_OBS_VAR(pos);
	phalcon_read_property_this(&pos, this_ptr, SL("_pos"), PH_NOISY TSRMLS_CC);

	input = Z_STRVAL_P(data);
	inputpos = Z_LVAL_P(pos);

	v = php_unpack(&input[inputpos], size, issigned, big_endian_short_map);

#ifdef ZEND_ENABLE_ZVAL_LONG64
	v = v << 48 >> 48;
#else
	v = v << 16 >> 16;
#endif

	ZVAL_LONG(pos, inputpos + size);
	phalcon_update_property_this(this_ptr, SL("_pos"), pos TSRMLS_CC);

	RETVAL_LONG(v);
	PHALCON_MM_RESTORE();
}

/**
 *
 * @return int
 */
PHP_METHOD(Phalcon_Utils_Binary, readInt){

	zval *data, *pos;
	char *format, *input;
	int inputpos, v, issigned = 0, size = 2;

	PHALCON_MM_GROW();

	PHALCON_OBS_VAR(data);
	phalcon_read_property_this(&data, this_ptr, SL("_data"), PH_NOISY TSRMLS_CC);

	PHALCON_OBS_VAR(pos);
	phalcon_read_property_this(&pos, this_ptr, SL("_pos"), PH_NOISY TSRMLS_CC);

	input = Z_STRVAL_P(data);
	inputpos = Z_LVAL_P(pos);

	v = php_unpack(&input[inputpos], size, issigned, big_endian_short_map);

#ifdef ZEND_ENABLE_ZVAL_LONG64
	v = v << 32 >> 32;
#else
	v = v << 16 >> 16;
#endif

	ZVAL_LONG(pos, inputpos + size);
	phalcon_update_property_this(this_ptr, SL("_pos"), pos TSRMLS_CC);

	RETVAL_LONG(v);
	PHALCON_MM_RESTORE();
}

/**
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Utils_Binary, readBool){

	zval *data, *pos;
	char *format, *input;
	int inputpos, v, issigned = 0, size = 1;

	PHALCON_MM_GROW();

	PHALCON_OBS_VAR(data);
	phalcon_read_property_this(&data, this_ptr, SL("_data"), PH_NOISY TSRMLS_CC);

	PHALCON_OBS_VAR(pos);
	phalcon_read_property_this(&pos, this_ptr, SL("_pos"), PH_NOISY TSRMLS_CC);

	input = Z_STRVAL_P(data);
	inputpos = Z_LVAL_P(pos);

	v = (unsigned char) (input[inputpos]);

	ZVAL_LONG(pos, inputpos + size);
	phalcon_update_property_this(this_ptr, SL("_pos"), pos TSRMLS_CC);

	if (v > 0) {
		RETURN_MM_TRUE;
	}

	RETURN_MM_FALSE;
}

/**
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Utils_Binary, readByte){

	zval *data, *pos;
	char *format, *input;
	int inputpos, v, issigned = 0, size = 1;

	PHALCON_MM_GROW();

	PHALCON_OBS_VAR(data);
	phalcon_read_property_this(&data, this_ptr, SL("_data"), PH_NOISY TSRMLS_CC);

	PHALCON_OBS_VAR(pos);
	phalcon_read_property_this(&pos, this_ptr, SL("_pos"), PH_NOISY TSRMLS_CC);

	input = Z_STRVAL_P(data);
	inputpos = Z_LVAL_P(pos);

	v = (unsigned char) (input[inputpos]);

	ZVAL_LONG(pos, inputpos + size);
	phalcon_update_property_this(this_ptr, SL("_pos"), pos TSRMLS_CC);

	RETVAL_LONG(v);
	PHALCON_MM_RESTORE();
}

/**
 *
 * @return int
 */
PHP_METHOD(Phalcon_Utils_Binary, readShort){

	zval *data, *pos;
	char *format, *input;
	int inputpos, v, issigned = 0, size = 2;

	PHALCON_MM_GROW();

	PHALCON_OBS_VAR(data);
	phalcon_read_property_this(&data, this_ptr, SL("_data"), PH_NOISY TSRMLS_CC);

	PHALCON_OBS_VAR(pos);
	phalcon_read_property_this(&pos, this_ptr, SL("_pos"), PH_NOISY TSRMLS_CC);

	input = Z_STRVAL_P(data);
	inputpos = Z_LVAL_P(pos);


	v = php_unpack(&input[inputpos], size, issigned, big_endian_short_map);

	ZVAL_LONG(pos, inputpos + size);
	phalcon_update_property_this(this_ptr, SL("_pos"), pos TSRMLS_CC);

	RETVAL_LONG(v);
	PHALCON_MM_RESTORE();
}

/**
 *
 * @return int
 */
PHP_METHOD(Phalcon_Utils_Binary, readFloat){

	zval *data, *pos;
	char *format, *input;
	int inputpos, issigned = 0, size = sizeof(float);
	float v;

	PHALCON_MM_GROW();

	PHALCON_OBS_VAR(data);
	phalcon_read_property_this(&data, this_ptr, SL("_data"), PH_NOISY TSRMLS_CC);

	PHALCON_OBS_VAR(pos);
	phalcon_read_property_this(&pos, this_ptr, SL("_pos"), PH_NOISY TSRMLS_CC);

	input = Z_STRVAL_P(data);
	inputpos = Z_LVAL_P(pos);

	memcpy(&v, &input[inputpos], size);

	ZVAL_LONG(pos, inputpos + size);
	phalcon_update_property_this(this_ptr, SL("_pos"), pos TSRMLS_CC);

	RETURN_DOUBLE((double)v);
	PHALCON_MM_RESTORE();
}

/**
 *
 * @return int
 */
PHP_METHOD(Phalcon_Utils_Binary, readDouble){

	zval *data, *pos;
	char *format, *input;
	int inputpos, size = sizeof(double);
	double v;

	PHALCON_MM_GROW();

	PHALCON_OBS_VAR(data);
	phalcon_read_property_this(&data, this_ptr, SL("_data"), PH_NOISY TSRMLS_CC);

	PHALCON_OBS_VAR(pos);
	phalcon_read_property_this(&pos, this_ptr, SL("_pos"), PH_NOISY TSRMLS_CC);

	input = Z_STRVAL_P(data);
	inputpos = Z_LVAL_P(pos);

	memcpy(&v, &input[inputpos], size);

	ZVAL_LONG(pos, inputpos + size);
	phalcon_update_property_this(this_ptr, SL("_pos"), pos TSRMLS_CC);

	RETURN_DOUBLE(v);
	PHALCON_MM_RESTORE();
}
