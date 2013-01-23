<?
require_once('stubsys.class.php');

if ($argc != 4) {
    echo("Usage {$argv[0]} loop_cnt sleep_cnt usecond\n");
    exit();
}

$stubsys = new stubsys();
$old_time = time();
$idx_old = 0;
if ($stubsys->connect('10.1.1.47', 13299)) {
    for ($i = 0; $i < $argv[1]; $i++) {
        $user_id = rand(1, 200);
        $need_filter = rand(0, 1);
        //var_export($stubsys->do_filter_no_recv($user_id, $need_filter, $argv[2], $i));
        $stubsys->do_filter_no_recv($user_id, $need_filter, $argv[2], $i);

        if ($argv[3] != 0) {
            usleep($argv[3]);
        }

        //echo("idx: $i\n");
        $new_time = time();
        if (floor($new_time / 5) != $old_time) {
            echo('speed: '.floor(($i - $idx_old) / 5)."\n");
            $idx_old = $i;
        }
    }
}
else {
    echo("Conn fail \n");
}
$stubsys->close();
?>
