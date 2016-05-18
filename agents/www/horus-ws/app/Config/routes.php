<?php

/**
 * Routes configuration
 *
 * In this file, you set up routes to your controllers and their actions.
 * Routes are very important mechanism that allows you to freely connect
 * different URLs to chosen controllers and their actions (functions).
 *
 * CakePHP(tm) : Rapid Development Framework (http://cakephp.org)
 * Copyright (c) Cake Software Foundation, Inc. (http://cakefoundation.org)
 *
 * Licensed under The MIT License
 * For full copyright and license information, please see the LICENSE.txt
 * Redistributions of files must retain the above copyright notice.
 *
 * @copyright     Copyright (c) Cake Software Foundation, Inc. (http://cakefoundation.org)
 * @link          http://cakephp.org CakePHP(tm) Project
 * @package       app.Config
 * @since         CakePHP(tm) v 0.2.9
 * @license       http://www.opensource.org/licenses/mit-license.php MIT License
 */
$regex_name = '[\p{N}\p{L}\s\_\p{M}\p{Z}\p{S}\p{Pd}]+';

/**
 * Importante modifica avvenuta sul framework!!!
 * [CakeRoute::_writeRoute] linea 168
 * $this->_compiledRoute = '#^' . $parsed . '[/]*$#'; aggiunto una u infondo per l'unicode
 */
Router::connect('/', array('controller' => 'DocApi', 'action' => 'index'));

/**
 * Horus General function
 */
Router::connect(
        "/aya", array("controller" => "Horus", "action" => "connectionTest", "[method]" => "GET")
);
Router::connect(
        "/check", array("controller" => "Horus", "action" => "check", "[method]" => "GET")
);
Router::connect(
        "/gen_key", array("controller" => "Horus", "action" => "genKey", "[method]" => "GET")
);


/**
 * Gestione utenti
 */
Router::connect(
        "/Authmanager/Users", array("controller" => "Authmanager", "action" => "getUserList", "[method]" => "GET")
);
Router::connect(
        "/Authmanager/Users", array("controller" => "Authmanager", "action" => "putUserList", "[method]" => "POST")
);
Router::connect(
        "/Authmanager/Users/:user_name", array("controller" => "Authmanager", "action" => "getUser", "[method]" => "GET"), array('pass' => array('user_name'), 'user_name' => $regex_name)
);
Router::connect(
        "/Authmanager/Users/:user_name", array("controller" => "Authmanager", "action" => "putUser", "[method]" => "POST"), array('pass' => array('user_name'), 'user_name' => $regex_name)
);

Router::connect(
        "/Authmanager/:level/:passwd", array("controller" => "Authmanager", "action" => "externalAuthLevel", "[method]" => "GET"), array('pass' => array('level', 'passwd'), 'level' => $regex_name, 'passwd' => $regex_name)
);

/**
 * Configurazioni esterne per tablet, smartphone, ecc..
 */
Router::mapResources('ExternalConfig');
Router::parseExtensions();

Router::connect(
       "/Storage/**",
       array("controller" => "ExternalConfig", "action" => "upload", "[method]" => "POST"),
       array('pass' => array('folder'), 'folder' => '[a-zA-Z0-9_!\-\.]+')
);

Router::connect(
       "/Storage/:folder",
       array("controller" => "ExternalConfig", "action" => "file_list", "[method]" => "GET"),
       array('pass' => array('folder'), 'folder' => '[a-zA-Z0-9_!\-\.]+')
);

Router::connect(
       "/Storage/:file",
       array("controller" => "ExternalConfig", "action" => "delete", "[method]" => "DELETE"),
       array('pass' => array('file'), 'file' => '[a-zA-Z0-9_!\-\.\/]+')
);
Router::connect(
       "/Storage/:file",
       array("controller" => "ExternalConfig", "action" => "download", "[method]" => "GET"),
       array('pass' => array('file'), 'file' => '[a-zA-Z0-9_!\-\.\/]+')
);
    
/**
 * Inoltro agente
 */
Router::connect(
        "/**",
        array("controller" => "AgentWebRoute", "action" => "forward")
);

/**
 * Load all plugin routes. See the CakePlugin documentation on
 * how to customize the loading of plugin routes.
 */
CakePlugin::routes();

/**
 * Load the CakePHP default routes. Only remove this if you do not want to use
 * the built-in default routes.
 */
require CAKE . 'Config' . DS . 'routes.php';
