<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $message = isset($_POST['userText']) ? $_POST['userText'] : 'No message submitted.';
    $message = htmlspecialchars($message);
} else {
    echo "Invalid request method.";
}
?>
