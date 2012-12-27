<?php
	require_once("proto.php");
	$proto=new Cproto("10.1.1.64",22005);
	$msg_proto=new Cproto("10.1.1.24",40022);

function prase_str($str)
{
    $arr = array();
    $arr=explode(' ',$str);
    print "X$arr[0]X, X$arr[1]X, X$arr[2]X";
}
	if($argv[1]==""){
        /*
        $fname1 = "system_message.conf";
        $fp1 = fopen($fname1, "r");
        $fmsg = fread($fp1, filesize($fname1));
        print "message: $fmsg";
        fclose($fp1);

        $fname2 = "reward.conf";
        $fp2 = fopen($fname2, "r");
        fgets($fp2);
        while (($fcontent = fgets($fp2))) {
            $arr = explode(' ',$fcontent);
            print "$arr[0], $arr[1], $arr[2], $arr[3]";
            print "";
            print_r($msg_proto->gf_post_vipsender_msg($arr[0], $arr[2], $fmsg, strlen($fmsg)));
            print_r($proto->gf_add_item_cmd($arr[0], $arr[1], $arr[2], $arr[3], 50));
        };
        fclose($fp2);*/
		//print_r($proto->dd_reg(20000,1,1,"ninini"));
		//print_r($proto->get_user_info_cmd(127601));
		//print_r($proto->get_user_itemlist_cmd(128100));
		//print_r($proto->add_friend_cmd(102401,2));
		//print_r($proto->del_friend_cmd(102401,2));
		//print_r($proto->get_friendlist_cmd(102401,1));
		//	print_r($proto->get_user_info_cmd(102401));
		//print_r($proto->get_user_partial_cmd(102401));
		//print_r($proto->gf_task_swap_item(135800));
        print_r($msg_proto->gf_sync_vip(105083,3,15,1319522407,1,1292587228,0, 131));
        print_r($proto->gf_sync_vip(105083,3,15,1319522407,1,1292587228,0, 131));
        //print_r($msg_proto->gf_post_vipsender_msg(100233, 1270715929, "vipvipvip", 9));
	}
	else{
		//print_r($proto->dd_reg($argv[1],1,1,"ninini"));
        print "$argv[1], $argv[2], $argv[3]";
        print "";
        //print_r($proto->gf_set_account_forbid ($argv[1], $argv[2], $argv[3]));
        //print_r($proto->gf_get_user_vip($argv[1]));
        print_r($proto->gf_get_all_task($argv[1], $argv[2]));
	}
?>
