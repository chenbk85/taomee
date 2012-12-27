<?php
	#加载proto_base_new.php 所在的目录
	$proto_base_dir="../gen_proto_app/pub_file/";
	ini_set('include_path', ini_get("include_path").":$proto_base_dir");


  	require_once( "../php/test_online_proto.php" );
 	//$p=new Ctest_online_proto("10.1.1.58", 13330);
 	$p=new Ctest_online_proto("10.1.1.24", 30004);
	//print_r($arr=$p->user_login_add(100000,1, 3,4  ));
	//print_r($arr=$p->user_login_add(100000,1, 3,4  ));
	print_r ($p->user_login_get(100000, 0,0xFFFFFFFF));
?>
