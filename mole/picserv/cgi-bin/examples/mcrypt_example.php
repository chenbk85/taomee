<?php

require_once('../auth/mcrypt.inc');

$cryptor = new Mcrypt();

if ($cryptor->init() < 0) 
	echo "cryptor init error\n";

for ($i = 0; $i < 1000000; $i++) {

$rand = rand(10000, 10000000);
$now = time();
$rand_n = pack('N', $rand);
$now_n = pack('N', $now);

$encrypted = $cryptor->encrypt($rand_n.$now_n);
$de_session =  $cryptor->decrypt($encrypted);

$array = unpack('Nrand_out/Nnow_out', $de_session);

if ($array['rand_out'] != $rand 
	|| $array['now_out'] != $now ) {
	echo $i.":\n";
	echo "before: rand = ".$rand."\tnow = ".$now."\n";
	echo "after : rand = ".$array['rand_out']."\tnow = ".$array['now_out']."\n";
}
}

$cryptor->fini();

?>
