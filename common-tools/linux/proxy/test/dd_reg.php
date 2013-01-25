<?php
    
	require_once("proto.php");
	$proto=new proto("10.1.1.24",2424);
	$game_mole = 1;
	$game_seer = 2;
	$game_xhx = 5;
	$game_gf = 6;
	$game_hero = 7;
	$game_mdd = 10;

	$zone_tel = 0;
	$zone_cnc = 1;
	$svr_db = 1;
	$svr_switch = 2;
	if($argv[1]==""){
        print "parament err";
		//print_r($proto->get_friends_list(101276,1268274342,$game_gf,$zone_cnc,$svr_db));
		//print_r($proto->get_friends_list(100086,0,$game_seer,$zone_tel,$svr_db));
		//print_r($proto->get_user_nick(100086,0,$game_seer,$zone_tel,$svr_db));
		//print_r($proto->get_user_info(100086,0,$game_seer,$zone_tel,$svr_db));
		//print_r($proto->add_items(100086,0,$game_seer,$zone_tel,$svr_db));
		//print_r($proto->add_items(101276,0,$game_mole,$zone_tel,$svr_db));
		//print_r($proto->add_items(100025,0,$game_xhx,$zone_tel,$svr_db));
		print_r($proto->add_items(47159775,0,$game_mdd,$zone_tel,$svr_db));

		$title = "校巴奖励通知";
		$titlelen = strlen($title);
		$msg = "校巴奖励通知!!!!!";
		$msglen = strlen($msg);
		//print_r($proto->send_mail(100025,0,$game_xhx,$zone_tel,$svr_db,0,"伊尔使者",time(),12,$titlelen,$title,$msglen,$msg));
		//print_r($proto->send_mail(5005468,0,$game_hero,$zone_tel,$svr_db,0,"伊尔使者2",time(),12,$titlelen,$title,$msglen,$msg));

		//print_r($proto->send_mail(101276,0,$game_mole,$zone_tel,$svr_db,0,"",time(),1000217,$titlelen,$title,$msglen,$msg));
		//print_r($proto->send_mail(101276,0,$game_gf,$zone_tel,$svr_db,0,"",time(),999,$titlelen,$title,$msglen,$msg));
		//print_r($proto->send_mail(100833,0,$game_gf,$zone_tel,$svr_db,0,"",time(),999,$titlelen,$title,$msglen,$msg));
		//print_r($proto->send_mail(5005468,0,$game_hero,$zone_tel,$svr_db,0,"伊尔使者2",time(),12,$titlelen,$title,$msglen,$msg));
		//print_r($proto->send_mail(100086,0,$game_seer,$zone_tel,$svr_db,0,"",time(),108,0,"",0,""));
	}
	else{
        $uid=$argv[1];
        $msg=$argv[2];
        $len=strlen($msg);
        print "uid=$uid msg=$msg len=$len \n";
        print_r($proto->gf_post_offline_msg($uid, 0, $msg, $len));
		//print_r($proto->dd_reg($argv[1],1,1,"ninini"));
	}
?>
