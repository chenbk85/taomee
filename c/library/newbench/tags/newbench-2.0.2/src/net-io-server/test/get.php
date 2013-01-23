<?php
require 'socket_handler.class.php';

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$timeout = array('sec'=>1, 'usec'=>500000);

socket_set_option($socket,SOL_SOCKET,SO_RCVTIMEO,$timeout);
socket_connect($socket, '10.1.14.41', 3333);

$f = fopen('php://stdin', 'r');

while ($line = fgets($f))
{
    $file = '';
    $length = 0;

    switch (intval($line))
    {
        case 0:
            $file = 'story.txt';
            $length = 1184640;
            break;
        case 1:
            $file = 'linux.txt';
            $length = 1461903;
            break;
        case 2:
            $file = '大话设计模式.txt';
            $length = 76178;
            break;
        default:
            $file = 'story.txt';
            $length = 1184640;
            break;
    }

    $i = 0;
    $content = '';
    socket_write($socket, intval($line), 1);
    $j = 0;

    do
    {
        $data = socket_read($socket, 1, PHP_BINARY_READ);

        if (is_string($data) && strlen($data))
        {
            $i += strlen($data);
            $content .= $data;

            if ($data == '*')
            {
                ++ $j;
            }

            if ($j >= 6)
            {
                break;
            }
        }

    } while(true);

    file_put_contents('./down/' . $file, $content);

    echo "\nfinish\n";
}
