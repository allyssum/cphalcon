
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (websocket://www.phalconphp.com)       |
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

#include "websocket/exception.h"
#include "websocket/client.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/object.h"

/**
 * Phalcon\WebSocket\Client
 *
 */
zend_class_entry *phalcon_websocket_client_ce;

PHP_METHOD(Phalcon_WebSocket_Client, __construct);

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_websocket_client___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, sockfd)
	ZEND_ARG_INFO(0, address)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_websocket_client_sendtext, 0, 0, 1)
	ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_websocket_client_sendbinary, 0, 0, 1)
	ZEND_ARG_INFO(0, binary)
ZEND_END_ARG_INFO()

static const zend_function_entry phalcon_websocket_client_method_entry[] = {
	PHP_ME(Phalcon_WebSocket_Client, __construct, arginfo_phalcon_websocket_client___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Phalcon_WebSocket_Client, sendText, arginfo_phalcon_websocket_client_sendtext, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_WebSocket_Client, sendBinary, arginfo_phalcon_websocket_client_sendbinary, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_WebSocket_Client, close, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Phalcon\WebSocket\Client initializer
 */
PHALCON_INIT_CLASS(Phalcon_WebSocket_Client){

	PHALCON_REGISTER_CLASS(Phalcon\\WebSocket, Client, websocket_client, phalcon_websocket_client_method_entry, 0);

	zend_declare_property_null(phalcon_websocket_client_ce, SL("_sockfd"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(phalcon_websocket_client_ce, SL("_address"), ZEND_ACC_PUBLIC TSRMLS_CC);
	return SUCCESS;
}

/**
 * Phalcon\WebSocket\Client constructor
 */
PHP_METHOD(Phalcon_WebSocket_Client, __construct){
	zval *sockfd, *address;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 2, 0, &sockfd, &address);

	phalcon_update_property_this(this_ptr, SL("_sockfd"), sockfd TSRMLS_CC);
	phalcon_update_property_this(this_ptr, SL("_address"), address TSRMLS_CC);

	PHALCON_MM_RESTORE();
}

PHP_METHOD(Phalcon_WebSocket_Client, sendText){

}

PHP_METHOD(Phalcon_WebSocket_Client, sendBinary){

}

PHP_METHOD(Phalcon_WebSocket_Client, close){

}
