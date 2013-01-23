<?
require_once('stubsys.class.php');

$stubsys = new stubsys();
if (! $stubsys->connect('10.1.1.5', 13299)) {
    echo("Conn fail \n");
    exit();
}
var_export($stubsys->do_echo(50077, 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890'));


?>
