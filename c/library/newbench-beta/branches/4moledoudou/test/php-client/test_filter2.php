<?
require_once('stubsys.class.php');

if ($argc != 4) {
    echo("Usage {$argv[0]} loop_cnt sleep_cnt usecond\n");
    exit();
}

$stubsys = new stubsys();
$old_time = time();
$idx_old = 0;
$loop_cnt = floor($argv[1] / 100);
if ($stubsys->connect('10.1.14.47', 13299)) {
    for ($i = 0; $i < $loop_cnt; $i++) {
        $user_id = rand(1, 200);
        $need_filter = rand(0, 1);
        //var_export($stubsys->do_filter_no_recv($user_id, $need_filter, $argv[2], $i));
        for ($j = 0; $j < 100; $j++) {
            $stubsys->do_filter_no_recv($user_id, $need_filter, $argv[2], $i);
        }
        for ($j = 0; $j < 100; $j++) {
            $stubsys->do_filter_recv();
        }

        if ($argv[3] != 0) {
            usleep($argv[3]);
        }

        //echo("idx: $i\n");
        $new_time = time();
        if (floor($new_time / 5) != floor($old_time / 5)) {
            echo('speed: '.floor(($i - $idx_old) * 20)."\n");
            $idx_old = $i;
            $old_time = $new_time;
        }
    }
    //echo('total snd time: '.$stubsys->get_total_snd_time()."\n");
    //echo('total rcv time: '.$stubsys->get_total_rcv_time()."\n");
}
else {
    echo("Conn fail \n");
}
$stubsys->close();
?>
