#!/usr/bin/env php
<?php

header("Content-Type: text/plain");


$query = $_SERVER['QUERY_STRING'];

if (empty($query)) {
    echo "No query string\n";
    exit;
}


parse_str($query, $params);

echo "Values in the query string\n";
foreach ($params as $key => $value) {
    echo "- $key: $value\n";
}
?>