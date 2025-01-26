<?php
file_put_contents('/tmp/cgi_debug.log', "Started PHP script\n", FILE_APPEND);
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $data = file_get_contents("php://input");
	file_put_contents('/tmp/cgi_debug.log', "STDIN: " . $data . "\n", FILE_APPEND);
    parse_str($data, $parsed_data);
    echo "Received POST data:<br>";
    foreach ($parsed_data as $key => $value) {
        echo htmlspecialchars($key) . ": " . htmlspecialchars($value) . "<br>";
    }
} else {
    echo "Please send a POST request.";
}
?>
