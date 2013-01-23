<?
require_once('stubsys.class.php');

if ($argc != 2) {
    echo("Usage {$argv[0]} loop_cnt\n");
    exit();
}

function microtime_float()
{
    list($usec, $sec) = explode(" ", microtime());
    return ((float)$usec + (float)$sec);
}

$stubsys = new stubsys();
$idx_old = 0;
$old_time = time();
$loop_cnt = $argv[1];
$avg_spd = 0;
//if ($stubsys->connect('10.1.1.47', 13299)) {
    for ($i = 0; $i < $loop_cnt; $i++) {
        $user_id = rand(1, 200);
        $need_filter = rand(0, 1);

        //$time_start = microtime_float();
        if (! $stubsys->connect('10.1.1.47', 13299)) {
            echo('Connect fail.'."\n");
            exit();
        }
        $stubsys->do_filter($user_id, $need_filter, 0, $i);
        $stubsys->close();
        //$time_itv = microtime_float() - $time_start;
        //file_put_contents('./spd.txt', $time_itv."\n", FILE_APPEND);

        $new_time = time();
        if (floor($new_time / 5) != floor($old_time / 5)) {
            if ($avg_spd == 0) {
                $avg_spd = floor(($i - $idx_old) / 5);
            }
            else {
                $avg_spd = floor(($avg_spd + floor(($i - $idx_old) / 5)) / 2);
            }
            echo($avg_spd."\n");
            $idx_old = $i;
            $old_time = $new_time;
        }
    }
//}
//else {
//    echo("Conn fail \n");
//}
//$stubsys->close();
?>
