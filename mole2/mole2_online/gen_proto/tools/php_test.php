<?php
	#加载proto_base_new.php 所在的目录
	$proto_base_dir="../gen_proto_app/pub_file/";
	ini_set('include_path', ini_get("include_path").":$proto_base_dir");


  	require_once( "../php/mole2_db_proto.php" );
 	//$p=new Ctest_online_proto("10.1.1.58", 13330);
 	$p=new Cmole2_db_proto("10.1.1.46", 21001);
	$p->mole2_set_system_time(0, "13:00:00");
	#print_r ($p->user_login_get(100000, 0,0xFFFFFFFF));
	#$userid=53000;
	#$teamid=4;
	#$count = 200;
	#for ($i = 0; $i < 100; $i ++) {
	#	print_r($p->mole2_set_hero_team($userid + $i, $teamid));
	#	print_r($p->mole2_add_medals($userid +$i, $count));
	#	$count = $count + 10;
	#}
?>
