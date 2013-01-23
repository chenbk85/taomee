<?
require_once('stubsys.class.php');

if ($argc != 4) {
    echo("Usage {$argv[0]} loop_cnt sleep_cnt usecond\n");
    exit();
}

$stubsys = new stubsys();
$old_time = time();
$idx_old = 0;
$loop_cnt = $argv[1];
if ($stubsys->connect('127.0.0.1', 13299)) {
    for ($i = 0; $i < $loop_cnt; $i++) {
        $user_id = 113;
        $need_filter = 1;
        $stubsys->do_filter_no_recv($user_id, $need_filter, $argv[2], $i);

        if ($argv[3] != 0) {
            usleep($argv[3]);
        }
    }
}
else {
    echo("Conn fail \n");
}
$stubsys->close();
?>
