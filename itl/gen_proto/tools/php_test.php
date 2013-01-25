<?php
	#加载proto_base_new.php 所在的目录
	$proto_base_dir="../gen_proto_app/pub_file/";
	ini_set('include_path', ini_get("include_path").":$proto_base_dir");


  	require_once( "../php/itl_node_proto.php" );
 	$p=new Citl_node_proto("10.1.1.63", 16600);
    $ret = $p->node_p_node_info(0, "10.1.1.63");
    print_r($ret);
?>
