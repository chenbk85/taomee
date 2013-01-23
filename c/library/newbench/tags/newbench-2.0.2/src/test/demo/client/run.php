<?php
require 'random_request_file.class.php';

if ($argc < 3)
{
    echo "\n", 'run.php [host] [port] ', "\n";
    exit();
}

$requester = new random_request_file();
$file_list = include('./file_list.inc.php');
$ret = $requester->init($argv[1], $argv[2], $file_list);

if (false == $ret)
{
    echo "\n", 'fail to init requester', "\n";
    exit();
}

$ret = $requester->run();

if (false === $ret)
{
    echo "\n", 'error: ' . $requester->get_error(), "\n";
}
else
{
    echo "\n", 'exit', "\n";
}

exit();
