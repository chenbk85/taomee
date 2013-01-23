<?
require_once('stubsys.class.php');

if ($argc != 4) {
    echo("Usage {$argv[0]} user_id, need_filter val\n");
    exit();
}

$stubsys = new stubsys();
if (! $stubsys->connect('127.0.0.1', 13299)) {
    echo("Conn fail \n");
    exit();
}

var_export($stubsys->do_filter($argv[1], $argv[2], $argv[3]));

?>
