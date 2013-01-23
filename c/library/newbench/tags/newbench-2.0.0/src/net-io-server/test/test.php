<?php
require 'socket_handler.class.php';

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$timeout = array('sec'=>1, 'usec'=>500000);
socket_set_option($socket,SOL_SOCKET,SO_RCVTIMEO,$timeout);
socket_connect($socket, '10.1.14.41', 3333);
socket_write($socket, 'start', strlen('start'));

$story = '';

$data = '';
echo "\n";

$i = 0;
$j = 0;

do
{
    $data = socket_read($socket, 1, PHP_BINARY_READ);

    if (is_string($data) && strlen($data))
    {
        $i += strlen($data);
        $story .= $data;

        if ('*' == $data)
        {
            ++ $j;
        }

        if ($j >= 6)
        {
            $j
        }
    }

    if (0 == ($i % 1000000))
    {
        echo "\nstop 1 sencond\n";
        sleep(1);
    }

} while($i < $length);

file_put_contents('./story2.txt', $story);

echo "\nend\n";
