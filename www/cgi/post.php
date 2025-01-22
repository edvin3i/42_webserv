<?php
// Vérifier que la méthode de la requête est POST
file_put_contents('/tmp/cgi_debug.log', "Started PHP script\n", FILE_APPEND);
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // Lire le corps de la requête (les données POST)
    $data = file_get_contents("php://input");
	file_put_contents('/tmp/cgi_debug.log', "STDIN: " . $data . "\n", FILE_APPEND);

    // Traiter les données reçues (par exemple, sous forme de JSON ou de données de formulaire)
    parse_str($data, $parsed_data);

    // Afficher les données reçues
    echo "Received POST data:<br>";
    foreach ($parsed_data as $key => $value) {
        echo htmlspecialchars($key) . ": " . htmlspecialchars($value) . "<br>";
    }
} else {
    echo "Please send a POST request.";
}
?>
