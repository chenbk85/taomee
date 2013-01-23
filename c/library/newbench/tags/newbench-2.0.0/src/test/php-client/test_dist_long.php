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
$start_time = time();
$loop_cnt = $argv[1];
if ($stubsys->connect('10.1.14.47', 13299)) {
    for ($i = 0; $i < $loop_cnt; $i++) {
        $user_id = rand(1, 200);
        $need_filter = rand(0, 1);

        $stubsys->do_filter($user_id, $need_filter, 0, $i);

        $new_time = time();
        if (floor($new_time / 5) != floor($old_time / 5)) {
            $spd = round(($i + 1) / (time() - $start_time));
            echo($spd."\n");
            $old_time = $new_time;
        }
    }
}
else {
    echo("Conn fail \n");
}
$stubsys->close();
?>
