<?php
	require_once("proto.php");
	$proto=new Cproto("10.1.1.24",6655);
	$userid = 101276;
	if($argv[1]==""){
		print_r($proto->login($userid,1));
		print_r($proto->enter_map($userid,2,0,0));
		//sleep(3);
		//print_r($proto->logout($userid,1));
	}
	else{
		print_r($proto->walk($argv[1],$argv[2],$argv[3]));
	}
?>
