<?php
	require_once("proto.php");
	$proto=new Cproto("10.1.1.24",40001);
	if($argv[1]==""){
		//print_r($proto->dd_reg(20000,1,1,"ninini"));
		print_r($proto->get_user_info_cmd(127601));
		//print_r($proto->get_user_itemlist_cmd(128100));
		//print_r($proto->add_friend_cmd(102401,2));
		//print_r($proto->del_friend_cmd(102401,2));
		//print_r($proto->get_friendlist_cmd(102401,1));
		//	print_r($proto->get_user_info_cmd(102401));
		//print_r($proto->get_user_partial_cmd(102401));
		print_r($proto->gf_task_swap_item(135800));
	}
	else{
		print_r($proto->dd_reg($argv[1],1,1,"ninini"));
	}
?>
