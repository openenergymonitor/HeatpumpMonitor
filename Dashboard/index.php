<?php
/*

Heatpump Monitor web dashboard code is released under the: 
GNU Affero General Public License and is based on emoncms.
Part of the OpenEnergyMonitor project:
http://openenergymonitor.org

*/

require "emoncms-api.php";
$emoncms = new Emoncms();
$emoncms->host = "http://emoncms.org";
    
require "core.php";
require "route.php";
$route = new Route($_GET['q']);

session_start();

if ($route->controller == "") {
    $output = array();
    $output['content'] = view("Views/login.php",array('session'=>$_SESSION));
    print view("theme.php", $output);
}
 
elseif ($route->controller=="dashboard") {
    print view("Views/dashboard.php",array('session'=>$_SESSION));
}

else if ($route->controller == "user") {
    
    if ($route->action=="login") {
        $auth = $emoncms->user_auth(post("username"),post("password"));
        
        if (isset($auth->success) && $auth->success==true) {
            session_regenerate_id();
            $_SESSION['apikey_read'] = $auth->apikey_read;
            $_SESSION['apikey_write'] = $auth->apikey_write;
            $_SESSION['valid'] = true;
            $user = $emoncms->user_get($auth->apikey_write);
            $_SESSION['username'] = $user->username;
            print json_encode($user);
        } else {
            print false;
        }
    }
    
    if ($route->action=="logout") {
      session_unset();
      session_destroy();
      print true;
    }
}

else if ($route->controller == "feed") {
    if ($route->action=="list") {
        print $emoncms->feed_list($_GET['apikey']);
    }
    
    if ($route->action=="data") {
        print $emoncms->feed_data(get('id'),get('start'),get('end'),get('interval'),get('skipmissing'),get('limitinterval'),get('apikey'));
    }
}

