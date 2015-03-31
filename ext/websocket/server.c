
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
#include "websocket/server.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/object.h"

#ifdef PHALCON_USE_LIBWEBSOCKET
#include <websock/websock.h>

typedef struct {
	zend_object object;
} websocket_message;

typedef struct {
	zend_object object;
	libwebsock_client_state *state;
} websocket_client;

typedef struct {
	zend_object object;
	libwebsock_context *ctx;
} websocket;

static int websocket_onopen(libwebsock_client_state *state);
static int websocket_onclose(libwebsock_client_state *state);
static int websocket_onmessage(libwebsock_client_state *state, libwebsock_message *msg);
#endif

/**
 * Phalcon\WebSocket\Server
 *
 * Creates a new request object for the given URI.
 *
 *<code>
 *	$server = new Phalcon\WebSocket\Server("0.0.0.0", "8080");
 *	$server->on(function($event, $client, $data) {
 *		switch($event) {
 *			case 'connection':
 *			break;
 *			case 'close':
 *			break;
 *			case 'message':
 *			break;
 *		}
 *	});

 *</code>
 *
 */
zend_class_entry *phalcon_websocket_server_ce;

PHP_METHOD(Phalcon_WebSocket_Server, __construct);

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_websocket_server___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, ip)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_websocket_server_on, 0, 0, 1)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

static const zend_function_entry phalcon_websocket_server_method_entry[] = {
	PHP_ME(Phalcon_WebSocket_Server, __construct, arginfo_phalcon_websocket_server___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Phalcon_WebSocket_Server, on, arginfo_phalcon_websocket_server_on, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_WebSocket_Server, start, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Phalcon\WebSocket\Server initializer
 */
PHALCON_INIT_CLASS(Phalcon_WebSocket_Server){

	PHALCON_REGISTER_CLASS(Phalcon\\WebSocket, Server, websocket_server, phalcon_websocket_server_method_entry, 0);

	zend_declare_property_null(phalcon_websocket_server_ce, SL("_ip"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_websocket_server_ce, SL("_port"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_websocket_server_ce, SL("_callback"), ZEND_ACC_PROTECTED TSRMLS_CC);
	return SUCCESS;
}

/**
 * Phalcon\WebSocket\Server constructor
 */
PHP_METHOD(Phalcon_WebSocket_Server, __construct){
	zval *ip, *port;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 2, 0, &ip, &port);

	phalcon_update_property_this(this_ptr, SL("_ip"), ip TSRMLS_CC);
	phalcon_update_property_this(this_ptr, SL("_port"), port TSRMLS_CC);

#ifdef PHALCON_USE_LIBWEBSOCKET
	websocket *socket = (websocket *)zend_object_store_get_object(this_ptr TSRMLS_CC);
	socket->ctx = libwebsock_init();
	socket->ctx->onopen = websocket_onopen;
	socket->ctx->onclose = websocket_onclose;
	socket->ctx->onmessage = websocket_onmessage;
	socket->ctx->user_data = (void *) this_ptr;
#endif

	PHALCON_MM_RESTORE();
}

/**
 * Adds a user-defined callback
 *
 * @param callable $callback
 * @return Phalcon\WebSocket\Server
 */
PHP_METHOD(Phalcon_WebSocket_Server, on){
	zval *callback;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &callback);

	if (!phalcon_is_callable(callback TSRMLS_CC) &&
			!(Z_TYPE_P(callback) == IS_OBJECT && instanceof_function(Z_OBJCE_P(callback), zend_ce_closure TSRMLS_CC))) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_websocket_exception_ce, "Callback must be an object or callable");
		return;
	}

	phalcon_update_property_this(this_ptr, SL("_callback"), callback TSRMLS_CC);

	RETURN_THIS();
}

PHP_METHOD(Phalcon_WebSocket_Server, start){
	zval *callback;

	PHALCON_MM_GROW();

	PHALCON_OBS_VAR(callback);
	phalcon_read_property_this(&callback, this_ptr, SL("_callback"), PH_NOISY TSRMLS_CC);


	RETURN_THIS();
}
