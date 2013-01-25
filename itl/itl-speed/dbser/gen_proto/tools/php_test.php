#!/usr/bin/php
<?php
	#加载proto_base_new.php 所在的目录
	$proto_base_dir="../gen_proto_app/pub_file/";
	ini_set('include_path', ini_get("include_path").":$proto_base_dir");


  	require_once( "../php/cdn_rate_proto.php" );
 	//$p=new Ctest_online_proto("10.1.1.58", 13330);
 	$p=new Ccdn_rate_proto("10.1.1.141", 13880);

	//print_r($arr=$p->user_login_add(100000,1, 3,4  ));
	//print_r($arr=$p->user_login_add(100000,1, 3,4  ));
	//print_r ($p->pop_reg(50066,1,15,"jim"));
	print_dbret($p-> cdn_report_ip(0,1,2,3,4));
?>
