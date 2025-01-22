#!/usr/bin/php
<?php

// Activer la sortie en tant que CGI
header("Content-Type: text/plain");

// Récupérer le chemin après le script (extra-path)
$scriptPath = $_SERVER['SCRIPT_FILENAME']; // Le chemin du script
$extraPath = isset($_SERVER['PATH_INFO']) ? $_SERVER['PATH_INFO'] : ''; // L'extra-path

// Afficher les informations reçues
echo "Script Path: $scriptPath\n";
echo "Extra Path: $extraPath\n";

// Traiter l'extra-path (par exemple, diviser en segments)
if (!empty($extraPath)) {
    $segments = explode('/', trim($extraPath, '/'));
    echo "\nExtra Path Segments:\n";
    foreach ($segments as $index => $segment) {
        echo "Segment $index: $segment\n";
    }
} else {
    echo "\nNo Extra Path Provided.\n";
}
?>
