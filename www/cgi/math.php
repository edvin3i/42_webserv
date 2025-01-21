<?php
// Define a function to calculate the factorial
function factorial($n) {
    if ($n == 0) {
        return 1;
    } else {
        return $n * factorial($n - 1);
    }
}

// Define a function to check if a number is prime
function isPrime($n) {
    if ($n <= 1) {
        return false;
    }
    for ($i = 2; $i <= sqrt($n); $i++) {
        if ($n % $i == 0) {
            return false;
        }
    }
    return true;
}

// Display a welcome message
echo "<h1>Welcome to our PHP server!</h1>";

// Calculate the factorial of 5
$factorialResult = factorial(5);

// Check if a number is prime
$number = 29;
$primeStatus = isPrime($number) ? "yes" : "no";

// Display the results
echo "<p>The factorial of 5 is: " . $factorialResult . "</p>";
echo "<p>Is the number $number prime? $primeStatus</p>";

echo "<p>Here are some additional facts:</p>";
echo "<ul>";
echo "<li>You are on a PHP CGI web server!</li>";
echo "</ul>";
?>
