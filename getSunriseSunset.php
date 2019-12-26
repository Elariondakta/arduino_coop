<?php 
    session_start();
    ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);

    $sunset = Array();
    $sunrise = Array();

    for ($i = 0; $i < 365; $i++) {
        // $step = $i*604800;      //timestamp d'une semaine (nbr de secondes en une semaine)
        $step = $i*86400;    //timestamp d'un jour
        $base_year = 1577836800;    //timestamp de 01/01/20 00:00:00
        $sunset[$i] = date_sunset($base_year + $step, SUNFUNCS_RET_STRING, 48.8566969, 2.3514616, 90, 2);
        $sunrise[$i] = date_sunrise($base_year + $step, SUNFUNCS_RET_STRING, 48.8566969, 2.3514616, 90, 2);
    }

    file_put_contents("sunset.json", json_encode($sunset));
    file_put_contents("sunrise.json", json_encode($sunrise));
?>