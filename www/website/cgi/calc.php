<?php

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $num1 = isset($_POST['num1']) ? floatval($_POST['num1']) : null;
    $num2 = isset($_POST['num2']) ? floatval($_POST['num2']) : null;
    $operator = isset($_POST['operator']) ? $_POST['operator'] : null;
    
    if ($num1 !== null && $num2 !== null && $operator !== null) {
        $result = null;
        switch ($operator) {
            case '+':
                $result = $num1 + $num2;
                break;
            case '-':
                $result = $num1 - $num2;
                break;
            case '*':
                $result = $num1 * $num2;
                break;
            case '/':
                if ($num2 != 0) {
                    $result = $num1 / $num2;
                } else {
                    echo "Error: Division by zero!";
                    exit;
                }
                break;
            default:
                echo "Error: Invalid operator!";
                exit;
        }
        echo "$num1 $operator $num2 = $result";
    } else {
        echo "Error: Missing parameters!";
    }
} else {
    echo "Hey! GET method is not allowed here! ";
    echo "Send me a POST request from calculator ^^^ above ^^^ !";
}
?>