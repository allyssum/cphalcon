
#ifdef HAVE_CONFIG_H
#include "../../ext_config.h"
#endif

#include <php.h>
#include "../../php_ext.h"
#include "../../ext.h"

#include <Zend/zend_operators.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "kernel/operators.h"
#include "kernel/string.h"
#include "ext/spl/spl_exceptions.h"
#include "kernel/exception.h"
#include "kernel/concat.h"
#include "kernel/variables.h"
#include "kernel/hash.h"


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
/**
 * Phalcon\Mvc\Router
 *
 * <p>Phalcon\Mvc\Router is the standard framework router. Routing is the
 * process of taking a URI endpoint (that part of the URI which comes after the base URL) and
 * decomposing it into parameters to determine which module, controller, and
 * action of that controller should receive the request</p>
 *
 *<code>
 *
 *	$router = new Phalcon\Mvc\Router();
 *
 *	$router->add(
 *		"/documentation/{chapter}/{name}.{type:[a-z]+}",
 *		array(
 *			"controller" => "documentation",
 *			"action"     => "show"
 *		)
 *	);
 *
 *	$router->handle();
 *
 *	echo $router->getControllerName();
 *</code>
 *
 */
ZEPHIR_INIT_CLASS(Phalcon_Mvc_Router) {

	ZEPHIR_REGISTER_CLASS(Phalcon\\Mvc, Router, phalcon, mvc_router, phalcon_mvc_router_method_entry, 0);

	zend_declare_property_null(phalcon_mvc_router_ce, SL("_dependencyInjector"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_uriSource"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_namespace"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_module"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_controller"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_action"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_params"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_routes"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_matchedRoute"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_matches"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(phalcon_mvc_router_ce, SL("_wasMatched"), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_defaultNamespace"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_defaultModule"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_defaultController"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_defaultAction"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_defaultParams"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_removeExtraSlashes"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_router_ce, SL("_notFoundPaths"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_class_constant_long(phalcon_mvc_router_ce, SL("URI_SOURCE_GET_URL"), 0 TSRMLS_CC);
	zend_declare_class_constant_long(phalcon_mvc_router_ce, SL("URI_SOURCE_SERVER_REQUEST_URI"), 1 TSRMLS_CC);

	return SUCCESS;

}

/**
 * Phalcon\Mvc\Router constructor
 *
 * @param boolean defaultRoutes
 */
PHP_METHOD(Phalcon_Mvc_Router, __construct) {

	zval *defaultRoutes_param = NULL, *routes, *_0, *_1 = NULL, *_2 = NULL, *_3, *_4;
	zend_bool defaultRoutes;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 0, 1, &defaultRoutes_param);

	if (!defaultRoutes_param) {
		defaultRoutes = 1;
	} else {
		defaultRoutes = zephir_get_boolval(defaultRoutes_param);
	}


	ZEPHIR_INIT_VAR(routes);
	array_init(routes);
	if ((defaultRoutes == 1)) {
		ZEPHIR_INIT_VAR(_0);
		object_init_ex(_0, phalcon_mvc_router_route_ce);
		ZEPHIR_INIT_VAR(_1);
		array_init_size(_1, 2);
		add_assoc_long_ex(_1, SS("controller"), 1);
		ZEPHIR_INIT_VAR(_2);
		ZVAL_STRING(_2, "#^/([a-zA-Z0-9\\_\\-]+)[/]{0,1}$#", 1);
		zephir_call_method_p2_noret(_0, "__construct", _2, _1);
		zephir_array_append(&routes, _0, PH_SEPARATE);
		ZEPHIR_INIT_VAR(_3);
		object_init_ex(_3, phalcon_mvc_router_route_ce);
		ZEPHIR_INIT_NVAR(_2);
		array_init_size(_2, 4);
		add_assoc_long_ex(_2, SS("controller"), 1);
		add_assoc_long_ex(_2, SS("action"), 2);
		add_assoc_long_ex(_2, SS("params"), 3);
		ZEPHIR_INIT_VAR(_4);
		ZVAL_STRING(_4, "#^/([a-zA-Z0-9\\_\\-]+)/([a-zA-Z0-9\\.\\_]+)(/.*)*$#", 1);
		zephir_call_method_p2_noret(_3, "__construct", _4, _2);
		zephir_array_append(&routes, _3, PH_SEPARATE);
	}
	ZEPHIR_INIT_NVAR(_1);
	array_init(_1);
	zephir_update_property_this(this_ptr, SL("_params"), _1 TSRMLS_CC);
	ZEPHIR_INIT_NVAR(_2);
	array_init(_2);
	zephir_update_property_this(this_ptr, SL("_defaultParams"), _2 TSRMLS_CC);
	zephir_update_property_this(this_ptr, SL("_routes"), routes TSRMLS_CC);
	ZEPHIR_MM_RESTORE();

}

/**
 * Sets the dependency injector
 *
 * @param Phalcon\DiInterface dependencyInjector
 */
PHP_METHOD(Phalcon_Mvc_Router, setDI) {

	zval *dependencyInjector;

	zephir_fetch_params(0, 1, 0, &dependencyInjector);



	zephir_update_property_this(this_ptr, SL("_dependencyInjector"), dependencyInjector TSRMLS_CC);

}

/**
 * Returns the internal dependency injector
 *
 * @return Phalcon\DiInterface
 */
PHP_METHOD(Phalcon_Mvc_Router, getDI) {


	RETURN_MEMBER(this_ptr, "_dependencyInjector");

}

/**
 * Get rewrite info. This info is read from $_GET['_url']. This returns '/' if the rewrite information cannot be read
 *
 * @return string
 */
PHP_METHOD(Phalcon_Mvc_Router, getRewriteUri) {

	zval *url = NULL, *urlParts, *realUri, *_0, *_GET, *_SERVER;

	ZEPHIR_MM_GROW();

	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_uriSource"), PH_NOISY_CC);
	if (!(zephir_is_true(_0))) {
		ZEPHIR_OBS_VAR(url);
		zephir_get_global(&_GET, SS("_GET") TSRMLS_CC);
		if (zephir_array_isset_string_fetch(&url, _GET, SS("_url"), 0 TSRMLS_CC)) {
			if (!(ZEPHIR_IS_EMPTY(url))) {
				RETURN_CCTOR(url);
			}
		}
	} else {
		ZEPHIR_OBS_NVAR(url);
		zephir_get_global(&_SERVER, SS("_SERVER") TSRMLS_CC);
		if (zephir_array_isset_string_fetch(&url, _SERVER, SS("REQUEST_URI"), 0 TSRMLS_CC)) {
			ZEPHIR_INIT_VAR(urlParts);
			zephir_fast_explode_str(urlParts, SL("?"), url, LONG_MAX TSRMLS_CC);
			zephir_array_fetch_long(&realUri, urlParts, 0, PH_NOISY | PH_READONLY TSRMLS_CC);
			if (!(ZEPHIR_IS_EMPTY(realUri))) {
				RETURN_CTOR(realUri);
			}
		}
	}
	RETURN_MM_STRING("/", 1);

}

/**
 * Sets the URI source. One of the URI_SOURCE_* constants
 *
 *<code>
 *	$router->setUriSource(Router::URI_SOURCE_SERVER_REQUEST_URI);
 *</code>
 *
 * @param string uriSource
 * @return Phalcon\Mvc\Router
 */
PHP_METHOD(Phalcon_Mvc_Router, setUriSource) {

	zval *uriSource;

	zephir_fetch_params(0, 1, 0, &uriSource);



	zephir_update_property_this(this_ptr, SL("_uriSource"), uriSource TSRMLS_CC);
	RETURN_THISW();

}

/**
 * Set whether router must remove the extra slashes in the handled routes
 *
 * @param boolean remove
 * @return Phalcon\Mvc\Router
 */
PHP_METHOD(Phalcon_Mvc_Router, removeExtraSlashes) {

	zval *remove_param = NULL;
	zend_bool remove;

	zephir_fetch_params(0, 1, 0, &remove_param);

		remove = zephir_get_boolval(remove_param);


	zephir_update_property_this(this_ptr, SL("_removeExtraSlashes"), remove ? ZEPHIR_GLOBAL(global_true) : ZEPHIR_GLOBAL(global_false) TSRMLS_CC);
	RETURN_THISW();

}

/**
 * Sets the name of the default namespace
 *
 * @param string namespaceName
 * @return Phalcon\Mvc\Router
 */
PHP_METHOD(Phalcon_Mvc_Router, setDefaultNamespace) {

	zval *namespaceName_param = NULL;
	zval *namespaceName = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &namespaceName_param);

	if (Z_TYPE_P(namespaceName_param) != IS_STRING && Z_TYPE_P(namespaceName_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'namespaceName' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(namespaceName_param) == IS_STRING) {
		namespaceName = namespaceName_param;
	} else {
		ZEPHIR_INIT_VAR(namespaceName);
		ZVAL_EMPTY_STRING(namespaceName);
	}


	zephir_update_property_this(this_ptr, SL("_defaultNamespace"), namespaceName TSRMLS_CC);
	RETURN_THIS();

}

/**
 * Sets the name of the default module
 *
 * @param string moduleName
 * @return Phalcon\Mvc\Router
 */
PHP_METHOD(Phalcon_Mvc_Router, setDefaultModule) {

	zval *moduleName_param = NULL;
	zval *moduleName = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &moduleName_param);

	if (Z_TYPE_P(moduleName_param) != IS_STRING && Z_TYPE_P(moduleName_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'moduleName' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(moduleName_param) == IS_STRING) {
		moduleName = moduleName_param;
	} else {
		ZEPHIR_INIT_VAR(moduleName);
		ZVAL_EMPTY_STRING(moduleName);
	}


	zephir_update_property_this(this_ptr, SL("_defaultModule"), moduleName TSRMLS_CC);
	RETURN_THIS();

}

/**
 * Sets the default controller name
 *
 * @param string controllerName
 * @return Phalcon\Mvc\Router
 */
PHP_METHOD(Phalcon_Mvc_Router, setDefaultController) {

	zval *controllerName_param = NULL;
	zval *controllerName = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &controllerName_param);

	if (Z_TYPE_P(controllerName_param) != IS_STRING && Z_TYPE_P(controllerName_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'controllerName' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(controllerName_param) == IS_STRING) {
		controllerName = controllerName_param;
	} else {
		ZEPHIR_INIT_VAR(controllerName);
		ZVAL_EMPTY_STRING(controllerName);
	}


	zephir_update_property_this(this_ptr, SL("_defaultController"), controllerName TSRMLS_CC);
	RETURN_THIS();

}

/**
 * Sets the default action name
 *
 * @param string actionName
 * @return Phalcon\Mvc\Router
 */
PHP_METHOD(Phalcon_Mvc_Router, setDefaultAction) {

	zval *actionName_param = NULL;
	zval *actionName = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &actionName_param);

	if (Z_TYPE_P(actionName_param) != IS_STRING && Z_TYPE_P(actionName_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'actionName' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(actionName_param) == IS_STRING) {
		actionName = actionName_param;
	} else {
		ZEPHIR_INIT_VAR(actionName);
		ZVAL_EMPTY_STRING(actionName);
	}


	zephir_update_property_this(this_ptr, SL("_defaultAction"), actionName TSRMLS_CC);
	RETURN_THIS();

}

/**
 * Sets an array of default paths. If a route is missing a path the router will use the defined here
 * This method must not be used to set a 404 route
 *
 *<code>
 * $router->setDefaults(array(
 *		'module' => 'common',
 *		'action' => 'index'
 * ));
 *</code>
 *
 * @param array defaults
 * @return Phalcon\Mvc\Router
 */
PHP_METHOD(Phalcon_Mvc_Router, setDefaults) {

	zval *defaults, *namespaceName, *module, *controller, *action, *params;

	zephir_fetch_params(0, 1, 0, &defaults);



	if ((Z_TYPE_P(defaults) == IS_ARRAY)) {
		ZEPHIR_THROW_EXCEPTION_STRW(phalcon_mvc_router_exception_ce, "Defaults must be an array");
		return;
	}
	if (zephir_array_isset_string_fetch(&namespaceName, defaults, SS("namespace"), 1 TSRMLS_CC)) {
		zephir_update_property_this(this_ptr, SL("_defaultNamespace"), namespaceName TSRMLS_CC);
	}
	if (zephir_array_isset_string_fetch(&module, defaults, SS("module"), 1 TSRMLS_CC)) {
		zephir_update_property_this(this_ptr, SL("_defaultModule"), module TSRMLS_CC);
	}
	if (zephir_array_isset_string_fetch(&controller, defaults, SS("controller"), 1 TSRMLS_CC)) {
		zephir_update_property_this(this_ptr, SL("_defaultController"), controller TSRMLS_CC);
	}
	if (zephir_array_isset_string_fetch(&action, defaults, SS("action"), 1 TSRMLS_CC)) {
		zephir_update_property_this(this_ptr, SL("_defaultAction"), action TSRMLS_CC);
	}
	if (zephir_array_isset_string_fetch(&params, defaults, SS("params"), 1 TSRMLS_CC)) {
		zephir_update_property_this(this_ptr, SL("_defaultParams"), params TSRMLS_CC);
	}
	RETURN_THISW();

}

/**
 * x
 */
PHP_METHOD(Phalcon_Mvc_Router, doRemoveExtraSlashes) {

	zval *route;

	zephir_fetch_params(0, 1, 0, &route);



	RETURN_CCTORW(route);

}

/**
 * Handles routing information received from the rewrite engine
 *
 *<code>
 * //Read the info from the rewrite engine
 * $router->handle();
 *
 * //Manually passing an URL
 * $router->handle('/posts/edit/1');
 *</code>
 *
 * @param string uri
 */
PHP_METHOD(Phalcon_Mvc_Router, handle) {

	zend_function *_9 = NULL, *_10 = NULL, *_11 = NULL, *_12 = NULL;
	HashTable *_4, *_15;
	HashPosition _3, _14;
	zval *uri = NULL, *realUri = NULL, *request = NULL, *currentHostName = NULL, *routeFound = NULL, *parts = NULL, *params, *matches, *notFoundPaths, *vnamespace, *module, *controller, *action, *paramsStr, *strParams, *route = NULL, *methods = NULL, *dependencyInjector = NULL, *hostname = NULL, *regexHostName = NULL, *matched = NULL, *pattern = NULL, *handledUri = NULL, *beforeMatch = NULL, *paths = NULL, *converters = NULL, *part = NULL, *position = NULL, *matchPosition, *converter = NULL, *_0, _1, *_2, **_5, *_6, *_7 = NULL, *_8 = NULL, *_13 = NULL, **_16, *_17 = NULL, *_18 = NULL, *_19, *_20, *_21, *_22, *_23;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 0, 1, &uri);

	if (!uri) {
		uri = ZEPHIR_GLOBAL(global_null);
	}


	if (!(zephir_is_true(uri))) {
		ZEPHIR_INIT_VAR(realUri);
		zephir_call_method(realUri, this_ptr, "getrewriteuri");
	} else {
		ZEPHIR_CPY_WRT(realUri, uri);
	}
	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_removeExtraSlashes"), PH_NOISY_CC);
	if (zephir_is_true(_0)) {
		if (!ZEPHIR_IS_STRING(realUri, "/")) {
			ZEPHIR_INIT_VAR(handledUri);
			ZEPHIR_SINIT_VAR(_1);
			ZVAL_STRING(&_1, "/", 0);
			zephir_fast_trim(handledUri, realUri, &_1, ZEPHIR_TRIM_RIGHT TSRMLS_CC);
		} else {
			ZEPHIR_CPY_WRT(handledUri, realUri);
		}
	} else {
		ZEPHIR_CPY_WRT(handledUri, realUri);
	}
	ZEPHIR_INIT_VAR(request);
	ZVAL_NULL(request);
	ZEPHIR_INIT_VAR(currentHostName);
	ZVAL_NULL(currentHostName);
	ZEPHIR_INIT_VAR(routeFound);
	ZVAL_BOOL(routeFound, 0);
	ZEPHIR_INIT_VAR(parts);
	array_init(parts);
	ZEPHIR_INIT_VAR(params);
	array_init(params);
	ZEPHIR_INIT_VAR(matches);
	ZVAL_NULL(matches);
	zephir_update_property_this(this_ptr, SL("_wasMatched"), (0) ? ZEPHIR_GLOBAL(global_true) : ZEPHIR_GLOBAL(global_false) TSRMLS_CC);
	zephir_update_property_this(this_ptr, SL("_matchedRoute"), ZEPHIR_GLOBAL(global_null) TSRMLS_CC);
	_2 = zephir_fetch_nproperty_this(this_ptr, SL("_routes"), PH_NOISY_CC);
	zephir_is_iterable(_2, &_4, &_3, 0, 1);
	for (
		; zend_hash_get_current_data_ex(_4, (void**) &_5, &_3) == SUCCESS
		; zend_hash_move_backwards_ex(_4, &_3)
	) {
		ZEPHIR_GET_HVALUE(route, _5);
		ZEPHIR_INIT_NVAR(methods);
		zephir_call_method(methods, route, "gethttpmethods");
		if ((Z_TYPE_P(methods) != IS_NULL)) {
			if ((Z_TYPE_P(request) == IS_NULL)) {
				_6 = zephir_fetch_nproperty_this(this_ptr, SL("_dependencyInjector"), PH_NOISY_CC);
				ZEPHIR_CPY_WRT(dependencyInjector, _6);
				if ((Z_TYPE_P(dependencyInjector) != IS_OBJECT)) {
					ZEPHIR_THROW_EXCEPTION_STR(phalcon_mvc_router_exception_ce, "A dependency injection container is required to access the 'request' service");
					return;
				}
				ZEPHIR_INIT_NVAR(_7);
				ZEPHIR_INIT_NVAR(_8);
				ZVAL_STRING(_8, "request", 1);
				zephir_call_method_p1_cache(_7, dependencyInjector, "getshared", &_9, _8);
				ZEPHIR_CPY_WRT(request, _7);
			}
			ZEPHIR_INIT_NVAR(_7);
			zephir_call_method_p1_cache(_7, request, "ismethod", &_10, methods);
			if (ZEPHIR_IS_FALSE(_7)) {
				continue;
			}
		}
		ZEPHIR_INIT_NVAR(hostname);
		zephir_call_method(hostname, route, "gethostname");
		if ((Z_TYPE_P(hostname) != IS_NULL)) {
			if ((Z_TYPE_P(request) == IS_NULL)) {
				_6 = zephir_fetch_nproperty_this(this_ptr, SL("_dependencyInjector"), PH_NOISY_CC);
				ZEPHIR_CPY_WRT(dependencyInjector, _6);
				if ((Z_TYPE_P(dependencyInjector) != IS_OBJECT)) {
					ZEPHIR_THROW_EXCEPTION_STR(phalcon_mvc_router_exception_ce, "A dependency injection container is required to access the 'request' service");
					return;
				}
				ZEPHIR_INIT_NVAR(_7);
				ZEPHIR_INIT_NVAR(_8);
				ZVAL_STRING(_8, "request", 1);
				zephir_call_method_p1_cache(_7, dependencyInjector, "getshared", &_11, _8);
				ZEPHIR_CPY_WRT(request, _7);
			}
			if ((Z_TYPE_P(currentHostName) != IS_OBJECT)) {
				ZEPHIR_INIT_NVAR(currentHostName);
				zephir_call_method_cache(currentHostName, request, "gethttphost", &_12);
			}
			if ((Z_TYPE_P(currentHostName) == IS_NULL)) {
				continue;
			}
			ZEPHIR_INIT_NVAR(matched);
			if (zephir_memnstr_str(hostname, SL("("), "phalcon/mvc/router.zep", 430)) {
				if (!(zephir_memnstr_str(hostname, SL("#"), "phalcon/mvc/router.zep", 431))) {
					ZEPHIR_INIT_NVAR(regexHostName);
					ZEPHIR_CONCAT_SVS(regexHostName, "#^", hostname, "$#");
				} else {
					ZEPHIR_CPY_WRT(regexHostName, hostname);
				}
				ZEPHIR_INIT_NVAR(_13);
				zephir_preg_match(matched, &(matched), regexHostName, currentHostName, _13, 0 TSRMLS_CC);
			} else {
				ZVAL_BOOL(matched, ZEPHIR_IS_EQUAL(currentHostName, hostname));
			}
			if (!(zephir_is_true(matched))) {
				continue;
			}
		}
		ZEPHIR_INIT_NVAR(pattern);
		zephir_call_method(pattern, route, "getcompiledpattern");
		if ((Z_TYPE_P(pattern) != IS_STRING)) {
			zephir_var_dump(&this_ptr TSRMLS_CC);
		}
		ZEPHIR_INIT_NVAR(routeFound);
		if (zephir_memnstr_str(pattern, SL("^"), "phalcon/mvc/router.zep", 454)) {
			zephir_preg_match(routeFound, &(routeFound), pattern, handledUri, matches, 0 TSRMLS_CC);
		} else {
			ZVAL_BOOL(routeFound, ZEPHIR_IS_EQUAL(pattern, handledUri));
		}
		if (zephir_is_true(routeFound)) {
			ZEPHIR_INIT_NVAR(beforeMatch);
			zephir_call_method(beforeMatch, route, "getbeforematch");
			if ((Z_TYPE_P(beforeMatch) != IS_NULL)) {
				if (!(zephir_is_callable(beforeMatch TSRMLS_CC))) {
					ZEPHIR_THROW_EXCEPTION_STR(phalcon_mvc_router_exception_ce, "Before-Match callback is not callable in matched route");
					return;
				}
				ZEPHIR_INIT_NVAR(routeFound);
				ZEPHIR_INIT_NVAR(_7);
				array_init_size(_7, 4);
				zephir_array_fast_append(_7, handledUri);
				zephir_array_fast_append(_7, route);
				zephir_array_fast_append(_7, this_ptr);
				ZEPHIR_CALL_USER_FUNC_ARRAY(routeFound, beforeMatch, _7);
			}
		}
		if (zephir_is_true(routeFound)) {
			ZEPHIR_INIT_NVAR(paths);
			zephir_call_method(paths, route, "getpaths");
			ZEPHIR_CPY_WRT(parts, paths);
			if ((Z_TYPE_P(matches) == IS_ARRAY)) {
				ZEPHIR_INIT_NVAR(converters);
				zephir_call_method(converters, route, "getconverters");
				zephir_is_iterable(paths, &_15, &_14, 0, 0);
				for (
					; zend_hash_get_current_data_ex(_15, (void**) &_16, &_14) == SUCCESS
					; zend_hash_move_forward_ex(_15, &_14)
				) {
					ZEPHIR_GET_HMKEY(part, _15, _14);
					ZEPHIR_GET_HVALUE(position, _16);
					if (zephir_array_isset_fetch(&matchPosition, matches, position, 1 TSRMLS_CC)) {
						if ((Z_TYPE_P(converters) == IS_ARRAY)) {
							ZEPHIR_OBS_NVAR(converter);
							if (zephir_array_isset_fetch(&converter, converters, part, 0 TSRMLS_CC)) {
								ZEPHIR_INIT_NVAR(_7);
								ZEPHIR_INIT_NVAR(_8);
								array_init_size(_8, 2);
								zephir_array_fast_append(_8, matchPosition);
								ZEPHIR_CALL_USER_FUNC_ARRAY(_7, converter, _8);
								zephir_array_update_zval(&parts, part, &_7, PH_COPY | PH_SEPARATE);
								continue;
							}
						}
						zephir_array_update_zval(&parts, part, &matchPosition, PH_COPY | PH_SEPARATE);
					} else {
						if ((Z_TYPE_P(converters) == IS_ARRAY)) {
							ZEPHIR_OBS_NVAR(converter);
							if (zephir_array_isset_fetch(&converter, converters, part, 0 TSRMLS_CC)) {
								ZEPHIR_INIT_NVAR(_17);
								ZEPHIR_INIT_NVAR(_18);
								array_init_size(_18, 2);
								zephir_array_fast_append(_18, position);
								ZEPHIR_CALL_USER_FUNC_ARRAY(_17, converter, _18);
								zephir_array_update_zval(&parts, part, &_17, PH_COPY | PH_SEPARATE);
							}
						}
					}
				}
				zephir_update_property_this(this_ptr, SL("_matches"), matches TSRMLS_CC);
			}
			zephir_update_property_this(this_ptr, SL("_matchedRoute"), route TSRMLS_CC);
			break;
		}
	}
	if (zephir_is_true(routeFound)) {
		zephir_update_property_this(this_ptr, SL("_wasMatched"), (1) ? ZEPHIR_GLOBAL(global_true) : ZEPHIR_GLOBAL(global_false) TSRMLS_CC);
	} else {
		zephir_update_property_this(this_ptr, SL("_wasMatched"), (0) ? ZEPHIR_GLOBAL(global_true) : ZEPHIR_GLOBAL(global_false) TSRMLS_CC);
	}
	if (!(zephir_is_true(routeFound))) {
		notFoundPaths = zephir_fetch_nproperty_this(this_ptr, SL("_notFoundPaths"), PH_NOISY_CC);
		if ((Z_TYPE_P(notFoundPaths) != IS_NULL)) {
			ZEPHIR_CPY_WRT(parts, notFoundPaths);
			ZEPHIR_INIT_BNVAR(routeFound);
			ZVAL_BOOL(routeFound, 1);
		}
	}
	if (zephir_is_true(routeFound)) {
		if (zephir_array_isset_string_fetch(&vnamespace, parts, SS("namespace"), 1 TSRMLS_CC)) {
			if (!(zephir_is_numeric(vnamespace))) {
				zephir_update_property_this(this_ptr, SL("_namespace"), vnamespace TSRMLS_CC);
			}
			zephir_array_fetch_string(&_6, parts, SL("namespace"), PH_NOISY | PH_READONLY TSRMLS_CC);
			zephir_array_unset(&parts, _6, PH_SEPARATE);
		} else {
			_19 = zephir_fetch_nproperty_this(this_ptr, SL("_defaultNamespace"), PH_NOISY_CC);
			zephir_update_property_this(this_ptr, SL("_namespace"), _19 TSRMLS_CC);
		}
		if (zephir_array_isset_string_fetch(&module, parts, SS("module"), 1 TSRMLS_CC)) {
			if (!(zephir_is_numeric(module))) {
				zephir_update_property_this(this_ptr, SL("_module"), module TSRMLS_CC);
			}
			zephir_array_fetch_string(&_6, parts, SL("module"), PH_NOISY | PH_READONLY TSRMLS_CC);
			zephir_array_unset(&parts, _6, PH_SEPARATE);
		} else {
			_19 = zephir_fetch_nproperty_this(this_ptr, SL("_defaultModule"), PH_NOISY_CC);
			zephir_update_property_this(this_ptr, SL("_module"), _19 TSRMLS_CC);
		}
		if (zephir_array_isset_string_fetch(&controller, parts, SS("controller"), 1 TSRMLS_CC)) {
			if (!(zephir_is_numeric(controller))) {
				zephir_update_property_this(this_ptr, SL("_controller"), controller TSRMLS_CC);
			}
			zephir_array_fetch_string(&_6, parts, SL("controller"), PH_NOISY | PH_READONLY TSRMLS_CC);
			zephir_array_unset(&parts, _6, PH_SEPARATE);
		} else {
			_19 = zephir_fetch_nproperty_this(this_ptr, SL("_defaultController"), PH_NOISY_CC);
			zephir_update_property_this(this_ptr, SL("_controller"), _19 TSRMLS_CC);
		}
		if (zephir_array_isset_string_fetch(&action, parts, SS("action"), 1 TSRMLS_CC)) {
			if (!(zephir_is_numeric(action))) {
				zephir_update_property_this(this_ptr, SL("_action"), action TSRMLS_CC);
			}
			zephir_array_fetch_string(&_6, parts, SL("action"), PH_NOISY | PH_READONLY TSRMLS_CC);
			zephir_array_unset(&parts, _6, PH_SEPARATE);
		} else {
			_19 = zephir_fetch_nproperty_this(this_ptr, SL("_defaultAction"), PH_NOISY_CC);
			zephir_update_property_this(this_ptr, SL("_action"), _19 TSRMLS_CC);
		}
		if (zephir_array_isset_string_fetch(&paramsStr, parts, SS("params"), 1 TSRMLS_CC)) {
			ZEPHIR_INIT_VAR(strParams);
			zephir_substr(strParams, paramsStr, 1 , 0 );
			if (zephir_is_true(strParams)) {
				ZEPHIR_INIT_BNVAR(params);
				zephir_fast_explode_str(params, SL("/"), strParams, LONG_MAX TSRMLS_CC);
			}
			zephir_array_fetch_string(&_6, parts, SL("params"), PH_NOISY | PH_READONLY TSRMLS_CC);
			zephir_array_unset(&parts, _6, PH_SEPARATE);
		}
		if (zephir_fast_count_int(params TSRMLS_CC)) {
			ZEPHIR_INIT_NVAR(_7);
			zephir_fast_array_merge(_7, &(params), &(parts) TSRMLS_CC);
			zephir_update_property_this(this_ptr, SL("_params"), _7 TSRMLS_CC);
		} else {
			zephir_update_property_this(this_ptr, SL("_params"), parts TSRMLS_CC);
		}
	} else {
		_19 = zephir_fetch_nproperty_this(this_ptr, SL("_defaultNamespace"), PH_NOISY_CC);
		zephir_update_property_this(this_ptr, SL("_namespace"), _19 TSRMLS_CC);
		_20 = zephir_fetch_nproperty_this(this_ptr, SL("_defaultModule"), PH_NOISY_CC);
		zephir_update_property_this(this_ptr, SL("_module"), _20 TSRMLS_CC);
		_21 = zephir_fetch_nproperty_this(this_ptr, SL("_defaultController"), PH_NOISY_CC);
		zephir_update_property_this(this_ptr, SL("_controller"), _21 TSRMLS_CC);
		_22 = zephir_fetch_nproperty_this(this_ptr, SL("_defaultAction"), PH_NOISY_CC);
		zephir_update_property_this(this_ptr, SL("_action"), _22 TSRMLS_CC);
		_23 = zephir_fetch_nproperty_this(this_ptr, SL("_defaultParams"), PH_NOISY_CC);
		zephir_update_property_this(this_ptr, SL("_params"), _23 TSRMLS_CC);
	}
	ZEPHIR_MM_RESTORE();

}

/**
 * Adds a route to the router without any HTTP constraint
 *
 *<code>
 * $router->add('/about', 'About::index');
 *</code>
 *
 * @param string pattern
 * @param string/array paths
 * @param string httpMethods
 * @return Phalcon\Mvc\Router\Route
 */
PHP_METHOD(Phalcon_Mvc_Router, add) {

	zval *pattern_param = NULL, *paths = NULL, *httpMethods = NULL, *route;
	zval *pattern = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 2, &pattern_param, &paths, &httpMethods);

	if (Z_TYPE_P(pattern_param) != IS_STRING && Z_TYPE_P(pattern_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'pattern' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(pattern_param) == IS_STRING) {
		pattern = pattern_param;
	} else {
		ZEPHIR_INIT_VAR(pattern);
		ZVAL_EMPTY_STRING(pattern);
	}
	if (!paths) {
		paths = ZEPHIR_GLOBAL(global_null);
	}
	if (!httpMethods) {
		httpMethods = ZEPHIR_GLOBAL(global_null);
	}


	ZEPHIR_INIT_VAR(route);
	object_init_ex(route, phalcon_mvc_router_route_ce);
	zephir_call_method_p3_noret(route, "__construct", pattern, paths, httpMethods);
	zephir_update_property_array_append(this_ptr, SL("_routes"), route TSRMLS_CC);
	RETURN_CCTOR(route);

}

/**
 * Adds a route to the router that only match if the HTTP method is GET
 *
 * @param string pattern
 * @param string/array paths
 * @return Phalcon\Mvc\Router\Route
 */
PHP_METHOD(Phalcon_Mvc_Router, addGet) {

	zval *pattern_param = NULL, *paths = NULL, *_0;
	zval *pattern = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 1, &pattern_param, &paths);

	if (Z_TYPE_P(pattern_param) != IS_STRING && Z_TYPE_P(pattern_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'pattern' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(pattern_param) == IS_STRING) {
		pattern = pattern_param;
	} else {
		ZEPHIR_INIT_VAR(pattern);
		ZVAL_EMPTY_STRING(pattern);
	}
	if (!paths) {
		paths = ZEPHIR_GLOBAL(global_null);
	}


	ZEPHIR_INIT_VAR(_0);
	ZVAL_STRING(_0, "GET", 1);
	zephir_call_method_p3(return_value, this_ptr, "add", pattern, paths, _0);
	RETURN_MM();

}

/**
 * Adds a route to the router that only match if the HTTP method is POST
 *
 * @param string pattern
 * @param string/array paths
 * @return Phalcon\Mvc\Router\Route
 */
PHP_METHOD(Phalcon_Mvc_Router, addPost) {

	zval *pattern_param = NULL, *paths = NULL, *_0;
	zval *pattern = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 1, &pattern_param, &paths);

	if (Z_TYPE_P(pattern_param) != IS_STRING && Z_TYPE_P(pattern_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'pattern' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(pattern_param) == IS_STRING) {
		pattern = pattern_param;
	} else {
		ZEPHIR_INIT_VAR(pattern);
		ZVAL_EMPTY_STRING(pattern);
	}
	if (!paths) {
		paths = ZEPHIR_GLOBAL(global_null);
	}


	ZEPHIR_INIT_VAR(_0);
	ZVAL_STRING(_0, "POST", 1);
	zephir_call_method_p3(return_value, this_ptr, "add", pattern, paths, _0);
	RETURN_MM();

}

/**
 * Adds a route to the router that only match if the HTTP method is PUT
 *
 * @param string pattern
 * @param string/array paths
 * @return Phalcon\Mvc\Router\Route
 */
PHP_METHOD(Phalcon_Mvc_Router, addPut) {

	zval *pattern_param = NULL, *paths = NULL, *_0;
	zval *pattern = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 1, &pattern_param, &paths);

	if (Z_TYPE_P(pattern_param) != IS_STRING && Z_TYPE_P(pattern_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'pattern' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(pattern_param) == IS_STRING) {
		pattern = pattern_param;
	} else {
		ZEPHIR_INIT_VAR(pattern);
		ZVAL_EMPTY_STRING(pattern);
	}
	if (!paths) {
		paths = ZEPHIR_GLOBAL(global_null);
	}


	ZEPHIR_INIT_VAR(_0);
	ZVAL_STRING(_0, "PUT", 1);
	zephir_call_method_p3(return_value, this_ptr, "add", pattern, paths, _0);
	RETURN_MM();

}

/**
 * Adds a route to the router that only match if the HTTP method is PATCH
 *
 * @param string pattern
 * @param string/array paths
 * @return Phalcon\Mvc\Router\Route
 */
PHP_METHOD(Phalcon_Mvc_Router, addPatch) {

	zval *pattern_param = NULL, *paths = NULL, *_0;
	zval *pattern = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 1, &pattern_param, &paths);

	if (Z_TYPE_P(pattern_param) != IS_STRING && Z_TYPE_P(pattern_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'pattern' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(pattern_param) == IS_STRING) {
		pattern = pattern_param;
	} else {
		ZEPHIR_INIT_VAR(pattern);
		ZVAL_EMPTY_STRING(pattern);
	}
	if (!paths) {
		paths = ZEPHIR_GLOBAL(global_null);
	}


	ZEPHIR_INIT_VAR(_0);
	ZVAL_STRING(_0, "PATCH", 1);
	zephir_call_method_p3(return_value, this_ptr, "add", pattern, paths, _0);
	RETURN_MM();

}

/**
 * Adds a route to the router that only match if the HTTP method is DELETE
 *
 * @param string pattern
 * @param string/array paths
 * @return Phalcon\Mvc\Router\Route
 */
PHP_METHOD(Phalcon_Mvc_Router, addDelete) {

	zval *pattern_param = NULL, *paths = NULL, *_0;
	zval *pattern = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 1, &pattern_param, &paths);

	if (Z_TYPE_P(pattern_param) != IS_STRING && Z_TYPE_P(pattern_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'pattern' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(pattern_param) == IS_STRING) {
		pattern = pattern_param;
	} else {
		ZEPHIR_INIT_VAR(pattern);
		ZVAL_EMPTY_STRING(pattern);
	}
	if (!paths) {
		paths = ZEPHIR_GLOBAL(global_null);
	}


	ZEPHIR_INIT_VAR(_0);
	ZVAL_STRING(_0, "DELETE", 1);
	zephir_call_method_p3(return_value, this_ptr, "add", pattern, paths, _0);
	RETURN_MM();

}

/**
 * Add a route to the router that only match if the HTTP method is OPTIONS
 *
 * @param string pattern
 * @param string/array paths
 * @return Phalcon\Mvc\Router\Route
 */
PHP_METHOD(Phalcon_Mvc_Router, addOptions) {

	zval *pattern_param = NULL, *paths = NULL, *_0;
	zval *pattern = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 1, &pattern_param, &paths);

	if (Z_TYPE_P(pattern_param) != IS_STRING && Z_TYPE_P(pattern_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'pattern' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(pattern_param) == IS_STRING) {
		pattern = pattern_param;
	} else {
		ZEPHIR_INIT_VAR(pattern);
		ZVAL_EMPTY_STRING(pattern);
	}
	if (!paths) {
		paths = ZEPHIR_GLOBAL(global_null);
	}


	ZEPHIR_INIT_VAR(_0);
	ZVAL_STRING(_0, "OPTIONS", 1);
	zephir_call_method_p3(return_value, this_ptr, "add", pattern, paths, _0);
	RETURN_MM();

}

/**
 * Adds a route to the router that only match if the HTTP method is HEAD
 *
 * @param string pattern
 * @param string/array paths
 * @return Phalcon\Mvc\Router\Route
 */
PHP_METHOD(Phalcon_Mvc_Router, addHead) {

	zval *pattern_param = NULL, *paths = NULL, *_0;
	zval *pattern = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 1, &pattern_param, &paths);

	if (Z_TYPE_P(pattern_param) != IS_STRING && Z_TYPE_P(pattern_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'pattern' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(pattern_param) == IS_STRING) {
		pattern = pattern_param;
	} else {
		ZEPHIR_INIT_VAR(pattern);
		ZVAL_EMPTY_STRING(pattern);
	}
	if (!paths) {
		paths = ZEPHIR_GLOBAL(global_null);
	}


	ZEPHIR_INIT_VAR(_0);
	ZVAL_STRING(_0, "HEAD", 1);
	zephir_call_method_p3(return_value, this_ptr, "add", pattern, paths, _0);
	RETURN_MM();

}

/**
 * Mounts a group of routes in the router
 *
 * @param Phalcon\Mvc\Router\Group route
 * @return Phalcon\Mvc\Router
 */
PHP_METHOD(Phalcon_Mvc_Router, mount) {

	HashTable *_1, *_4;
	HashPosition _0, _3;
	zval *group, *groupRoutes, *beforeMatch, *hostname, *routes, *route = NULL, **_2, **_5, *_6;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &group);



	if ((Z_TYPE_P(group) != IS_OBJECT)) {
		ZEPHIR_THROW_EXCEPTION_STR(phalcon_mvc_router_exception_ce, "The group of routes is not valid");
		return;
	}
	ZEPHIR_INIT_VAR(groupRoutes);
	zephir_call_method(groupRoutes, group, "getroutes");
	if (!(zephir_fast_count_int(groupRoutes TSRMLS_CC))) {
		ZEPHIR_THROW_EXCEPTION_STR(phalcon_mvc_router_exception_ce, "The group of routes does not contain any routes");
		return;
	}
	ZEPHIR_INIT_VAR(beforeMatch);
	zephir_call_method(beforeMatch, group, "getbeforematch");
	if ((Z_TYPE_P(beforeMatch) != IS_NULL)) {
		zephir_is_iterable(groupRoutes, &_1, &_0, 0, 0);
		for (
			; zend_hash_get_current_data_ex(_1, (void**) &_2, &_0) == SUCCESS
			; zend_hash_move_forward_ex(_1, &_0)
		) {
			ZEPHIR_GET_HVALUE(route, _2);
			zephir_call_method_p1_noret(route, "beforematch", beforeMatch);
		}
	}
	ZEPHIR_INIT_VAR(hostname);
	zephir_call_method(hostname, group, "gethostname");
	if ((Z_TYPE_P(hostname) != IS_NULL)) {
		zephir_is_iterable(groupRoutes, &_4, &_3, 0, 0);
		for (
			; zend_hash_get_current_data_ex(_4, (void**) &_5, &_3) == SUCCESS
			; zend_hash_move_forward_ex(_4, &_3)
		) {
			ZEPHIR_GET_HVALUE(route, _5);
			zephir_call_method_p1_noret(route, "sethostname", hostname);
		}
	}
	routes = zephir_fetch_nproperty_this(this_ptr, SL("_routes"), PH_NOISY_CC);
	if ((Z_TYPE_P(routes) == IS_ARRAY)) {
		ZEPHIR_INIT_VAR(_6);
		zephir_fast_array_merge(_6, &(routes), &(groupRoutes) TSRMLS_CC);
		zephir_update_property_this(this_ptr, SL("_routes"), _6 TSRMLS_CC);
	} else {
		zephir_update_property_this(this_ptr, SL("_routes"), groupRoutes TSRMLS_CC);
	}
	RETURN_THIS();

}

/**
 * Set a group of paths to be returned when none of the defined routes are matched
 *
 * @param array paths
 * @return Phalcon\Mvc\Router
 */
PHP_METHOD(Phalcon_Mvc_Router, notFound) {

	zval *paths;

	zephir_fetch_params(0, 1, 0, &paths);



	if ((Z_TYPE_P(paths) != IS_ARRAY)) {
		if ((Z_TYPE_P(paths) != IS_STRING)) {
			ZEPHIR_THROW_EXCEPTION_STRW(phalcon_mvc_router_exception_ce, "The not-found paths must be an array or string");
			return;
		}
	}
	zephir_update_property_this(this_ptr, SL("_notFoundPaths"), paths TSRMLS_CC);
	RETURN_THISW();

}

/**
 * Removes all the pre-defined routes
 */
PHP_METHOD(Phalcon_Mvc_Router, clear) {

	zval *_0;

	ZEPHIR_MM_GROW();

	ZEPHIR_INIT_VAR(_0);
	array_init(_0);
	zephir_update_property_this(this_ptr, SL("_routes"), _0 TSRMLS_CC);
	ZEPHIR_MM_RESTORE();

}

/**
 * Returns the processed namespace name
 *
 * @return string
 */
PHP_METHOD(Phalcon_Mvc_Router, getNamespaceName) {


	RETURN_MEMBER(this_ptr, "_namespace");

}

/**
 * Returns the processed module name
 *
 * @return string
 */
PHP_METHOD(Phalcon_Mvc_Router, getModuleName) {


	RETURN_MEMBER(this_ptr, "_module");

}

/**
 * Returns the processed controller name
 *
 * @return string
 */
PHP_METHOD(Phalcon_Mvc_Router, getControllerName) {


	RETURN_MEMBER(this_ptr, "_controller");

}

/**
 * Returns the processed action name
 *
 * @return string
 */
PHP_METHOD(Phalcon_Mvc_Router, getActionName) {


	RETURN_MEMBER(this_ptr, "_action");

}

/**
 * Returns the processed parameters
 *
 * @return array
 */
PHP_METHOD(Phalcon_Mvc_Router, getParams) {


	RETURN_MEMBER(this_ptr, "_params");

}

/**
 * Returns the route that matchs the handled URI
 *
 * @return Phalcon\Mvc\Router\Route
 */
PHP_METHOD(Phalcon_Mvc_Router, getMatchedRoute) {


	RETURN_MEMBER(this_ptr, "_matchedRoute");

}

/**
 * Returns the sub expressions in the regular expression matched
 *
 * @return array
 */
PHP_METHOD(Phalcon_Mvc_Router, getMatches) {


	RETURN_MEMBER(this_ptr, "_matches");

}

/**
 * Checks if the router macthes any of the defined routes
 *
 * @return bool
 */
PHP_METHOD(Phalcon_Mvc_Router, wasMatched) {


	RETURN_MEMBER(this_ptr, "_wasMatched");

}

/**
 * Returns all the routes defined in the router
 *
 * @return Phalcon\Mvc\Router\Route[]
 */
PHP_METHOD(Phalcon_Mvc_Router, getRoutes) {


	RETURN_MEMBER(this_ptr, "_routes");

}

/**
 * Returns a route object by its id
 *
 * @param int id
 * @return Phalcon\Mvc\Router\Route
 */
PHP_METHOD(Phalcon_Mvc_Router, getRouteById) {

	HashTable *_2;
	HashPosition _1;
	zval *id, *route = NULL, *_0, **_3, *_4 = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &id);



	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_routes"), PH_NOISY_CC);
	zephir_is_iterable(_0, &_2, &_1, 0, 0);
	for (
		; zend_hash_get_current_data_ex(_2, (void**) &_3, &_1) == SUCCESS
		; zend_hash_move_forward_ex(_2, &_1)
	) {
		ZEPHIR_GET_HVALUE(route, _3);
		ZEPHIR_INIT_NVAR(_4);
		zephir_call_method(_4, route, "getrouteid");
		if (ZEPHIR_IS_EQUAL(_4, id)) {
			RETURN_CCTOR(route);
		}
	}
	RETURN_MM_BOOL(0);

}

/**
 * Returns a route object by its name
 *
 * @param string name
 * @return Phalcon\Mvc\Router\Route | boolean
 */
PHP_METHOD(Phalcon_Mvc_Router, getRouteByName) {

	HashTable *_2;
	HashPosition _1;
	zval *name_param = NULL, *route = NULL, *_0, **_3, *_4 = NULL;
	zval *name = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &name_param);

	if (Z_TYPE_P(name_param) != IS_STRING && Z_TYPE_P(name_param) != IS_NULL) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL("Parameter 'name' must be a string") TSRMLS_CC);
		RETURN_MM_NULL();
	}

	if (Z_TYPE_P(name_param) == IS_STRING) {
		name = name_param;
	} else {
		ZEPHIR_INIT_VAR(name);
		ZVAL_EMPTY_STRING(name);
	}


	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_routes"), PH_NOISY_CC);
	zephir_is_iterable(_0, &_2, &_1, 0, 0);
	for (
		; zend_hash_get_current_data_ex(_2, (void**) &_3, &_1) == SUCCESS
		; zend_hash_move_forward_ex(_2, &_1)
	) {
		ZEPHIR_GET_HVALUE(route, _3);
		ZEPHIR_INIT_NVAR(_4);
		zephir_call_method(_4, route, "getname");
		if (ZEPHIR_IS_EQUAL(_4, name)) {
			RETURN_CCTOR(route);
		}
	}
	RETURN_MM_BOOL(0);

}

