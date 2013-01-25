#!/usr/bin/php
<?php
	#加载proto_base_new.php 所在的目录
	$proto_base_dir="../gen_proto_app/pub_file/";
	ini_set('include_path', ini_get("include_path").":$proto_base_dir");


  	require_once( "../php/mole2_db_proto.php" );

 	//$p=new Ctest_online_proto("10.1.1.58", 13330);
 	//$p=new Ctest_online_proto("10.1.1.24", 30004);
	#$p=new Cmole2_db_proto("10.1.1.46",  13001);
 	$p=new Cmole2_db_proto("10.1.1.46",  14001);
	//print_r($arr=$p->user_login_add(100000,1, 3,4  ));
	//print_r($arr=$p->mole2_user_set_vip_info (100000,1, 3,4 ,2,8,7,6 ));
	//$arr=$p->mole2_get_user_sql( 50066);
	//echo $arr["out" ]->sql_str;
//	print_r($p->su_mole2_get_friends(56436588));
//	print_r($p->su_mole2_get_blacks(56436588));

	print_r( $p->su_mole2_get_equips(50021));
	//print_r($p->mole2_add_val(50712,50715,4000));
?>
