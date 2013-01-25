<?php
require_once("proto_base.php");

class Cmole_proto extends Cproto_base {
	function __construct( $proxyip,$proxyport){
		parent::__construct($proxyip,$proxyport) ;
	}

	function msgboard_set_flag ($msgid,$boardid, $flag){
		$pri_msg=pack("LL",$boardid , $flag );
		$sendbuf=$this->park("7104", $msgid  , $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function msgboard_add_qa ($userid,$nick, $q,$a ){
		$pri_msg=pack("A12A16a213a361",$userid,$nick, $q,$a);
		$sendbuf=$this->park("7120", 0 , $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function msgboard_get_qa_list_ex($index){
		$in_msg=pack("LL",$index,8);
		$cmdid="7023";
		$sendbuf=$this->park($cmdid,0, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lmsgid_$i/a241question_$i/a361answer_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	function msgboard_del_qa ($msgid ){
		$pri_msg=pack("L",$msgid);
		$sendbuf=$this->park("7122", 0 , $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}





	function msgboard_clear_by_del_flag(){
		$sendbuf=$this->park("7107", 0, "");
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function msgboard_get_msg_by_msgid($msgid ){
		$pri_msg=pack("L",  $msgid);
		$sendbuf=$this->park("7009", 0 , $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),
				"a1024msg");
	}

	function msgboard_reset_msg($boardid, $msgid,$msg ){
		$msglen=strlen($msg)+1;
		$pri_msg=pack("LLLa".$msglen,$boardid,$msgid,$msglen, $msg);
		$sendbuf=$this->park("7110", 0 , $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}


	function mms_get_record($userid ){
		$sendbuf=$this->park("4409" ,$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ltype_$i/Lopt_time_$i/Lopt_count_$i/Lv1_$i/Lv2_$i/Lv3_$i/Lv4_$i/Lv5_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function mms_set_record($userid,$type,$opt_time,$opt_count,$v1,$v2,$v3,$v4,$v5 ){
		$pri_msg=pack("LLLLLLLL", $type,$opt_time,$opt_count,$v1,$v2,$v3,$v4,$v5  );
		$sendbuf=$this->park("4507" ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}


	function msgboard_get_record($board,$flag,$page  ){
		$pri_msg=pack("LLL", $board, $flag, $page   );
		$sendbuf=$this->park(7006 ,0, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lmsgid_$i/Luserid_$i/Llogdate_$i/Lhot_$i/Lcolor_$i".
				"/a16nick_$i/a200msg_$i"  ;
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function roommsg_add_msg($userid,$nick,$add_time, $msg ){
		$pri_msg=pack("La16La200", $userid,$nick,$add_time, $msg );
		$sendbuf=$this->park("C511" ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}
	function roommsg_del_msg($userid,$msgid ){
		$pri_msg=pack("LL", $msgid,0);
		$sendbuf=$this->park("C515" ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}
	function userclassmsg_del_msg($userid,$msgid ){
		$pri_msg=pack("LL", $msgid,0);
		$sendbuf=$this->park("119D" ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}

	function roommsg_show_message($userid,$msgid,$isshow ){
		$pri_msg=pack("LC", $msgid,$isshow );
		$sendbuf=$this->park("C512" ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}


	function user_classmsg_report($userid,$msgid, $msg ){
		$pri_msg=pack("La100", $msgid, $msg );
		$sendbuf=$this->park("119E" ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}


	function roommsg_report_msg($userid,$msgid, $msg ){
		$pri_msg=pack("La100", $msgid,$msg );
		$sendbuf=$this->park("C516" ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}
	
	function class_get_msg($userid,$pageid ){
		$pri_msg=pack("LL", $userid, $pageid );
		$sendbuf=$this->park("109C" ,$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Ltotal/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lmsgid_$i/Lguest_id_$i/a16guest_nick_$i/Ledit_time_$i/Lshow_time_$i".
				"/a200msg_$i/a100res_$i"  ;
		}
		return $this->unpark( $recvbuf, $fmt); 
	}




	function roommsg_get_msg($userid,$pageid ){
		$pri_msg=pack("LL", $userid, $pageid );
		$sendbuf=$this->park("C414" ,$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Ltotal/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lmsgid_$i/Lguest_id_$i/a16guest_nick_$i/Ledit_time_$i/Lshow_time_$i".
				"/a200msg_$i/a100res_$i"  ;
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function hope_get_hope_all($userid){
		$sendbuf=$this->park("1406" ,$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lhopedate_$i/Lsend_id_$i/a16send_nick_$i/Lrecv_type_$i/".
				"a30recv_type_name_$i/Luseflag_$i"  ;
		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	
	
	function user_day_limit_change_value_get_list($userid){
		$sendbuf=$this->park("3071" ,$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lpetid_$i/Lchange_value_$i" ;
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function user_mdog_get_mdog_info($userid){
		$pri_msg=pack("L", 1320001);
		$sendbuf=$this->park("3068" ,$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt = "";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		$fmt="Lmdog/Ltime/Lplant_water/Linsent_kill/Lanimal_drink/Lanimal_catch" ;
		return $this->unpark( $recvbuf, $fmt); 
	}

	function user_set_seed ( 
				$userid ,
				$id,
				$attireid,
				$x,
				$y,
				$value,
				$sickflag,
				$fruitnum,
				$cal_value_time,
				$water_time,
				$kill_bug_time,
				$status,
				$mature_time,
				$earth,
				$pollinate_num
			){
			
		$cmdid="1199";
		$in_msg= pack("L14", $id, $attireid, $x, $y, $value,
			   	$sickflag, $fruitnum, $cal_value_time,
   				$water_time, $kill_bug_time, $status, $mature_time,$earth,$pollinate_num
 				);
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function user_del_seed ( 
				$userid ,
				$id){
			
		$cmdid="1126";
		$in_msg= pack("L", $id);
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function user_class_get_simple_info( 
				$userid ){
		$cmdid="10FC";
		$out_msg= "Lclass_logo/Lclass_color/Lclass_word/Lclass_jion_flag/Lclass_access_flag/a16class_name/a60class_slogan/a16onwernick/Lclass_member_count";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}






	function user_get_seed_list($userid ){
		$sendbuf=$this->park("1098",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt 
				. "/Lid_$i"
				. "/Lattireid_$i"
				. "/Lx_$i"
				. "/Ly_$i"
				. "/Lvalue_$i"
				. "/Lsickflag_$i"
				. "/Lfruitnum_$i"
				. "/Lcal_value_time_$i"
				. "/Lwater_time_$i"
				. "/Lkill_bug_time_$i"
				. "/Lstatus_$i"
				. "/Lmature_time_$i"
				."/Ldiff_mature_$i"
				."/Lcur_grow_$i"
				."/Learth_$i"
				."/Lpollinate_num_$i"
				;
		}
		return $this->unpark($recvbuf,$fmt );
	}




	function msgboard_get_record_list($board,$page  ){
		$pri_msg=pack("LL", $board,  $page   );
		$sendbuf=$this->park(7002 ,0, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Ldbcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lmsgid_$i/Luserid_$i/Llogdate_$i/Lhot_$i/Lcolor_$i".
				"/a16nick_$i/a200msg_$i"  ;
		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	function userinfo_log_for_blw($userid, $md5_passwd  ,$ip=0 ){
		$pri_msg=pack("a16L", hex2bin_pri($md5_passwd),$ip);
		$sendbuf=$this->park("000D",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lgameflag");
	}



	function user_login_by_id($userid, $md5_passwd  ,$ip=0,$login_channel=0 ){
		$pri_msg=pack("a16LL", hex2bin_pri($md5_passwd),$ip,$login_channel );
		$sendbuf=$this->park("000C",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lgameflag");
	}

	function user_get_value($userid){
		$cmdid="1034";
		$in_msg="";
		$out_msg= "Lvip/Lflag/Lpetcolor/Lpetbirthday/Lxiaomee/Lxiaomee_max/Lsale_xiaomee_max/Lexp/".
			"Lstrong/Liq/Lcharm/Lgame/Lwork/Lfashion/LOl_count/LOl_today/LOl_last/LOl_time".
				"/Clast_ip_1/Clast_ip_2/Clast_ip_3/Clast_ip_4/Lmm_birthday/a150tasklist/Lflag2/a16nick/Ldriver/Llevel".
				"/Lvip_mark_last/Lvip_start_time/Lvip_end_time/Cvip_is_auto_charge/Lvip_charge_changel_last/Lbuild_level";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function user_get_jy($userid){
		$cmdid="1021";
		$in_msg="";
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function user_get_ip_history ($userid){
		$cmdid="104C";
		$in_msg="";
		$out_msg= "Nip_1/Llogtime_1/".
			"Nip_2/Llogtime_2/".
			"Nip_3/Llogtime_3" ;
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function userinfo_get_passwd_history ($userid){
		$cmdid="001C";
		$in_msg="";
		$out_msg= "a16passwd_1/Llogtime_1/".
			 "a16passwd_2/Llogtime_2/". 
			 "a16passwd_3/Llogtime_3/";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}



	function user_connect_set_childcount($userid,$childcount){
		$cmdid="1163";
		$in_msg= pack("L",$childcount );
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}




	function pp_register($userid,$nick,$color,$parentid){
		$cmdid="1900";
		$in_msg= pack("a16LL",$nick,$color,$parentid );
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function pp_pet_add($userid){
		$cmdid="1910";
		$in_msg= pack("La130",time(NULL),"" );
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function pp_pet_del($userid,$catchtime){
		$cmdid="191C";
		$in_msg= pack("L", $catchtime );
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}





	function pp_pet_index_set($userid,$petid,$is_catched){
		$cmdid="1950";
		$in_msg= pack("LL", $petid,$is_catched );
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function pp_pet_index_get_list($userid){
		$pri_msg=pack("LL",0,0xFFFFFFFF);
		$sendbuf=$this->park("1851",$userid,  $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .  "/Lpetid_$i/Lsee_count_$i/Lis_catched_$i/Lis_beat_$i"  ;
		}

		return $this->unpark($recvbuf,$fmt );
	}


	function user_info_get_set_passwd_flag($userid) {
        $sendbuf = $this->park("0045", $userid, $pri_msg);
        return $this->unpark($this->sock->sendmsg($sendbuf), "Lflag");
    }


	function userinfo_get_user_email($userid) {
        $sendbuf = $this->park("0044", $userid, $pri_msg);
        return $this->unpark($this->sock->sendmsg($sendbuf), "a64email");
    }



	function su_login_by_id($userid, $md5_passwd ){
		$pri_msg=pack("a16", hex2bin_pri($md5_passwd));
		$sendbuf=$this->park("A001",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"a16nick/Lflag");
	}

	function user_farm_animal_add($userid){
		$cmdid="11DF";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function user_farm_animal_del($userid,$id ){
		$cmdid="11BC";
		$in_msg=pack("L",$id);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function user_task_ex_del($userid,$taskid ){
		$cmdid="11BA";
		$in_msg=pack("L",$taskid);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}



	function user_set_tempbuf($userid){
		$cmdid="1185";
		$in_msg=pack("a40","");
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function plw_submit_add_msg($userid,$type,$nick,$title,$msg){
		$msglen=strlen($msg);
		$pri_msg=pack("La16a60La".$msglen,$type,$nick,$title ,$msglen,$msg);
		$sendbuf=$this->park("F124",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lid");
	}

	function pp_submit_msg_add($userid,$type,$msg){
		$msglen=strlen($msg);
		$pri_msg=pack("LLa".$msglen,$type,$msglen,$msg);
		$sendbuf=$this->park("F121",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function usermsg_add_writing($userid,$type,$msg){
		$msglen=strlen($msg);
		$pri_msg=pack("LLa".$msglen,$type,$msglen,$msg);
		$sendbuf=$this->park("9102",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}






	function pp_other_msg_attime_add($date,$hour,$minute,$flag,$pic_id,$msg){
		$msglen=strlen($msg);
		$pri_msg=pack("LLLLLLa".$msglen, $date,$hour, $minute,$flag,$pic_id,$msglen,$msg);
		$sendbuf=$this->park("4901",0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function pp_other_msg_attime_get($index){
		$msglen=strlen($msg);
		$pri_msg=pack( "L", $index );
		$sendbuf=$this->park("4802",0, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$recvarr=$this->unpark($recvbuf,"Lmsgid/Ldate/Lhour/Lminute/Lflag/Lpic_id/Lmsglen");
		$fmt="Lmsgid/Ldate/Lhour/Lminute/Lflag/Lpic_id/Lmsglen/a".$recvarr["msglen"]."msg";
		return $this->unpark($recvbuf,$fmt);
	}
	function pp_other_msg_attime_get_by_date($date,$hour,$minute,$index ){
		$pri_msg=pack("LLLL", $date,$hour, $minute,$index);
		$sendbuf=$this->park("4803",0, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$recvarr=$this->unpark($recvbuf,"Lmsgid/Ldate/Lhour/Lminute/Lflag/Lpic_id/Lmsglen");
		$fmt="Lmsgid/Ldate/Lhour/Lminute/Lflag/Lpic_id/Lmsglen/a".$recvarr["msglen"]."msg";
		return $this->unpark($recvbuf,$fmt);
	}

	function pp_other_msg_attime_del($msgid){
		$pri_msg=pack("L", $msgid);
		$sendbuf=$this->park("4904",0, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		return $this->unpark($recvbuf,"");
	}







//-----------------------------------------------------------------------
	function su_add_msg_attime($date,$hour,$minute,$flag,$pic_id,$msg){
		$msglen=strlen($msg);
		$pri_msg=pack("LLLLLLa".$msglen, $date,$hour, $minute,$flag,$pic_id,$msglen,$msg);
		$sendbuf=$this->park("A104",0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function su_get_msg_attime_all($index){
		$msglen=strlen($msg);
		$pri_msg=pack( "L", $index );
		$sendbuf=$this->park("A005",0, $pri_msg);

		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$recvarr=$this->unpark($recvbuf,"Ldate/Lhour/Lminute/Lflag/Lpic_id/Lmsglen");
		$fmt="Ldate/Lhour/Lminute/Lflag/Lpic_id/Lmsglen/a".$recvarr["msglen"]."msg";
		return $this->unpark($recvbuf,$fmt);

	}

	function su_get_msg_attime($date,$hour,$minute,$index ){
		$pri_msg=pack("LLLL", $date,$hour, $minute,$index);
		$sendbuf=$this->park("A007",0, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$recvarr=$this->unpark($recvbuf,"Ldate/Lhour/Lminute/Lflag/Lpic_id/Lmsglen");
		$fmt="Ldate/Lhour/Lminute/Lflag/Lpic_id/Lmsglen/a".$recvarr["msglen"]."msg";
		return $this->unpark($recvbuf,$fmt);
	}

	function su_del_msg_attime($date,$hour,$minute ){
		$pri_msg=pack("LLL", $date,$hour, $minute);
		$sendbuf=$this->park("A106",0, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		return $this->unpark($recvbuf,"");
	}


	function su_change($adminid, $userid, $attrid,$changevalue,$reason  ){
		$pri_msg=pack("LLlL", $userid,$attrid,$changevalue , $reason );
		$sendbuf=$this->park("A102",$adminid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function su_change_passwd($adminid,$md5_oldpasswd ,$md5_newpasswd ){
		$pri_msg=pack("A16A16", hex2bin_pri( $md5_oldpasswd ), 
				hex2bin_pri($md5_newpasswd));
		$sendbuf=$this->park("A103",$adminid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function serial_get_noused_id($type){
		$pri_msg=pack("L", $type );
		$sendbuf=$this->park(6103 ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"a20serialid");
	}

	function pay_add_damee_by_serial($userid, $serialid ){
		$pri_msg=pack("a20", $serialid  );
		$sendbuf=$this->park(3103 ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lprice/Lleave_damee");
	}

	function user_set_online_time($userid,$onlinetime ){
		$pri_msg=pack("L", $onlinetime);
		$sendbuf=$this->park(1110  ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_add_friend($userid ,$friendid ){
	$pri_msg=pack("L", $friendid);
	$sendbuf=$this->park("1111", $userid, $pri_msg);
	return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_add_black($userid ,$friendid ){
		$pri_msg=pack("L", $friendid);
		$sendbuf=$this->park("11B1", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function msgboard_add_msg($userid,$msg ){
		$msglen=strlen($msg);
		$pri_msg=pack("LLa16La".$msglen,0,4,"xcwen",$msglen,$msg  );
		$sendbuf=$this->park("7101", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}


	function user_pet_add_pet($userid , $flag, $birthday, $nick, $color, 
			$sicktime, $pos_x, $pos_y, $hungry, $thirsty, $sanitary, $spirit, $stamp
			){
		$pri_msg=pack("LLa16LLCCCCCCL", $flag, $birthday, $nick, $color, 
			$sicktime, $pos_x, $pos_y, $hungry, $thirsty, $sanitary, $spirit, $stamp
			   	);
		$sendbuf=$this->park("1151", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lpetid");
	}

	function user_pet_set_all($userid ,$petid, $flag,$flag2, $birthday, $logic_birthday, $nick, $color, 
			$sicktime, $pos_x, $pos_y, $hungry, $thirsty, $sanitary, $spirit, $starttime,$taskid,
			$endtime , $skill, $stamp,$sick_type, $fire,$water,$wood,$skill_flag,$change_value,
			$hot_skill_one,$hot_skill_two,$hot_skill_three,$pet_flag
			){
		//print "sicktime".$sicktime;
		$pri_msg=pack("LLLLLa16LLCCCCCCLLLLLLLLLLLLLLL", $petid,$flag, $flag2, $birthday, $logic_birthday, $nick, $color, 
			$sicktime, $pos_x, $pos_y, $hungry, $thirsty, $sanitary, $spirit,
			$endtime,$starttime,$taskid ,$skill, $stamp,$sick_type, $fire, $water, $wood,$skill_flag, $change_value,$hot_skill_one,$hot_skill_two,$hot_skill_three, $pet_flag)
;
		$sendbuf=$this->park("1156", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_pet_del_pet($userid ,$petid){
		$pri_msg=pack("L", $petid);
		$sendbuf=$this->park("1155", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function user_del_friend($userid ,$friendid){
		$pri_msg=pack("L", $friendid);
		$sendbuf=$this->park("1112", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_del_black($userid ,$friendid){
		$pri_msg=pack("L", $friendid);
		$sendbuf=$this->park("11B2", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_friend($userid1 ,$userid2 ){
		$this->user_add_friend($userid1,$userid2);
		return $this->user_add_friend($userid2,$userid1);
	}

	function user_get_info_list($userid ){
		$cmdid="1006";
		$in_msg="";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function user_get_black_list($userid ){
		$sendbuf=$this->park("10B3",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lfriendid_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function user_get_friend_list($userid ){
		$sendbuf=$this->park("1013",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lfriendid_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function user_get_task_ex_list($userid ){
		$sendbuf=$this->park("10BD",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ltaskid_$i/a50taskvalue_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}



	function user_add_offline_msg($userid, $msg){
		$msglen=strlen($msg)+1;
		$pri_msg=pack("LNCNNNNNNNNA16NNNNa".$msglen , 

				65+8+$msglen,61+8+$msglen,1,10003,0,0, 
				0,0,0,0,0,"淘米委员会",0,time(null),0,
			   	$msglen,$msg);
		$sendbuf=$this->park("112A", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_change_value($userid,$addexp,$addstrong,$addiq,$addcharm,$addxiaomee)
	{
		$pri_msg=pack("lllllLL",$addexp,$addstrong,$addiq,$addcharm,$addxiaomee,5,0);
		$sendbuf=$this->park("1139", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function userinfo_set_paypasswd_with_check($userid, $md5_oldpasswd ,$md5_newpasswd){
		$pri_msg=pack("A16A16", hex2bin_pri($md5_oldpasswd),
				hex2bin_pri($md5_newpasswd) );
		$sendbuf=$this->park("0190",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}


	function user_change_passwd($userid, $md5_oldpasswd ,$md5_newpasswd){
		$pri_msg=pack("A16A16", hex2bin_pri($md5_oldpasswd),
				hex2bin_pri($md5_newpasswd) );
		$sendbuf=$this->park("0109",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_change_passwd_without_check($userid, $md5_newpasswd){
		$pri_msg=pack("A16", hex2bin_pri($md5_newpasswd) );
		$sendbuf=$this->park("0140",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function userinfo_su_change_passwd($userid, $md5_newpasswd){
		$pri_msg=pack("A16", hex2bin_pri($md5_newpasswd) );
		$sendbuf=$this->park("010A",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function userinfo_su_set_passwd($userid, $md5_newpasswd){
		$pri_msg=pack("A16", hex2bin_pri($md5_newpasswd) );
		$sendbuf=$this->park("010A",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function userinfo_su_change_paypasswd($userid, $md5_newpasswd){
		$pri_msg=pack("A16", hex2bin_pri($md5_newpasswd) );
		$sendbuf=$this->park("010B",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}





	function userinfo_isset_question($userid){
		$cmdid="0085";
		$in_msg="";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function userinfo_check_existed($userid){
		$cmdid="0005";
		$in_msg="";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function userinfo_get_security_flag($userid){
		$cmdid="0086";
		$in_msg="";
		$out_msg="Lflag";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function serial_get_info_by_serial_str($serial_str ){
		$pri_msg=pack("a12",$serial_str);
		$sendbuf=$this->park(6005,0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lserial_type");
	}
	function serial_set_used_by_serial_str($serial_str ){
		$pri_msg=pack("a12",$serial_str);
		$sendbuf=$this->park(6104,0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lserial_type");
	}



	function user_get_userid_by_email($email){
		$pri_msg=pack("A64", $email );
		$sendbuf=$this->park(4004,0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Luserid");
	}

	function user_set_user_email($userid,$email){
		$pri_msg=pack("A64", $email );
		$sendbuf=$this->park("0143",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function email_set_email($userid,$old_email,$new_email){
		$pri_msg=pack("A64A64", $old_email,$new_email );
		$sendbuf=$this->park("4107", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function set_email_all($userid,$new_email){
		//得到原有的email
		$ret_arr=$this->userinfo_get_info_all($userid);
		if ($ret_arr["result"]==0){//
			//设置新的email	
			$old_email=$ret_arr["email"];
			$ret_arr=$this->email_set_email($userid,$old_email,$new_email);
			if ($ret_arr["result"]==0){//
				return $this->user_set_user_email($userid,$new_email);
			}else{
				return $ret_arr;
			}
		}else{
			return $ret_arr;
		}
	}
	
	function game_score_init($gameid){
		$pri_msg=pack("LL",$gameid ,10 );
		$sendbuf=$this->park(5103,0,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function game_reset_score($gameid,$userid,$score){
		$pri_msg=pack("LLL",$gameid ,$userid,$score);
		$sendbuf=$this->park(5104,0,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function pay_get_user_pay_info($userid){
		$sendbuf=$this->park("300C",$userid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),
				"Ldamee/Lmonth_used/Lmonth_paytype/Lmonth_enabledate/Lmonth_duedate/Lmonth_nexterrcount");
	}

	function user_profession_get($userid){
		$sendbuf=$this->park("105D",$userid, "" );
		$outmsg="";
		for ($i=0;$i<50;$i++){
			$outmsg.="Lprofession_$i/" ;
		}

		return $this->unpark($this->sock->sendmsg($sendbuf),
				$outmsg);
	}

	function user_profession_set($userid,$professionid,$value ){
		$cmdid="115E";
		$in_msg=pack("LL",$professionid,$value );
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}




	function usermsg_get_writing( $type,$userid, $date){
		$pri_msg=pack("LLa21",$type,$userid,$date);
		$sendbuf=$this->park("9003",0,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$recvarr=$this->unpark($recvbuf,"a21date/Ltype/Lflag/Lmsgid/Luserid/Lmsglen/Lreportlen");
		if ($recvarr && $recvarr["result"]!=SUCC){
				return $recvarr;
		}
		//$fmt="a21date/Ltype/Lflag/Lmsgid/Luserid/Lmsglen/Lreportlen/a104title/a16nick".
		//		"/a". ($recvarr["msglen"] - 120)  ."msg/Lreport_time/a".($recvarr["reportlen"]-4)."report";
		$fmt="a21date/Ltype/Lflag/Lmsgid/Luserid/Lmsglen/Lreportlen/a104title/a16nick".
				"/a". ($recvarr["msglen"] - 120)  ."msg"."/Lreport_time/a".($recvarr["reportlen"]-4)."report";
		//echo $fmt;
		return $this->unpark($recvbuf, $fmt);
		
	}

	function usermsg_get_writing_count($userid, $startdate, $enddate,$flag, $type, $key ,$index ){
		$pri_msg=pack("LLLLLa30L",$userid , $startdate, $enddate, $flag, $type,$key,$index );
		$sendbuf=$this->park("9009",0,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "Lcount");
	}

	function usermsg_get_report($objuserid, $startdate, $enddate,$sortflag, $index ){
		$pri_msg=pack("LLLL",$startdate,$enddate,$sortflag,$index);
		$sendbuf=$this->park("9004",$objuserid,  $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Luserid_$i/Lcount_$i/Lone_count_$i/Lreason1_$i".
				"/Lreason2_$i/Lreason3_$i/Lreason4_$i"  ;
		}

		return $this->unpark($recvbuf,$fmt );
	}
	
	function user_swap_card_get_list($userid){
		$sendbuf=$this->park("0x300C",$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lcard_type_$i/Lcount_$i/Lswap_flag_$i/Lneed_card_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}


	function user_get_petlist($userid){
		$pri_msg=pack("L",0);
		$sendbuf=$this->park("1050",$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lpetid_$i/Lflag_$i/Lflag2_$i/Lbirthday_$i/Llogic_birthday_$i/a16nick_$i".
				"/Lcolor_$i/Lsicktime_$i/Cpos_x_$i" . 
				"/Cpos_y_$i/Chungry_$i/Cthirsty_$i/Csanitary_$i/Cspirit_$i/".
				"Lendtime_$i/Lstarttime_$i/Ltaskid_$i/Lskill_$i/Lstamp_$i/Lsick_type_$i".
				"/Lfire_skill_bitflag_$i/Lwater_skill_bitflag_$i/Lwood_skill_bitflag_$i".
				"/Lskill_flag_$i/Lchange_value_$i".
				"/Lhot_skill_one_$i/Lhot_skill_two_$i/Lhot_skill_three_$i/Lpet_flag_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}
	function usermsg_get_writing_list( $userid, $startdate, $enddate,$flag, $type ,$key,$index ){
		$pri_msg=pack("LLLLLa30L",$userid , $startdate, $enddate, $flag, $type ,$key,  $index );
		$sendbuf=$this->park("900D",0,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	

		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/a21date_$i/Ltype_$i/Lflag_$i/Lmsgid_$i/Luserid_$i".
				"/Lmsglen_$i/a104title_$i/a16nick_$i/a436msg_$i/Lreportlen_$i/Lreport_time_$i/a192report_$i"; 
		}
		return $this->unpark($recvbuf,$fmt );
	}



	function user_get_attire_list($userid, $flag,$usedflag ){
		$pri_msg=pack("LLLC",$flag,0,0xFFFFFFFF ,$usedflag );
		$sendbuf=$this->park("1018",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/Lcount_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}
	function user_set_task($userid,$petid,$taskid,$flag,$usetime,$starttime,$endtime ){
		$pri_msg=pack("L8",$petid,$taskid,$flag,$usetime,$starttime,$endtime,$usetime,$flag );
		$sendbuf=$this->park("1171",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function user_swap_card_set_card($userid,$card_type,$count,$swapid ){
		if ($swapid>0){
			$swapflag=1;
		}else{
			$swapflag=0;
		}
		$pri_msg=pack("L4",$card_type,$count,$swapflag ,$swapid );
		$sendbuf=$this->park("310D",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}




	function user_get_task_list($userid){
		$pri_msg=pack("LLLL",0xFFFFFFFF,0,0xFFFFFFFF ,0);
		$sendbuf=$this->park("1073",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lpetid_$i/Ltaskid_$i/Lflag_$i/Lusetime_$i/Lstarttime_$i/Lendtime_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function user_get_pet_attire_all($userid){
		$pri_msg="";
		$sendbuf=$this->park("1069",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lpetid_$i/Lattireid_$i/Lusedcount_$i/Lallcount_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function user_add_attire_ex ($userid,$attiretype ,$attireid,$count,$maxcount=0xFFFFFFFF ){
		$pri_msg=pack("LLLL",$attiretype ,$attireid,$count,$maxcount );
		$sendbuf=$this->park("111C",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function user_add_attire ($userid, $attireid,$count,$maxcount=0xFFFFFFFF ){
		$pri_msg=pack("LLLL",0,$attireid,$count,$maxcount );
		$sendbuf=$this->park("111C",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_add_home_attire ($userid, $attireid,$count,$maxcount=0xFFFFFFFF ){
		$pri_msg=pack("LLLL",1,$attireid,$count, $maxcount  );
		$sendbuf=$this->park("111C",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_add_jy_attire ($userid, $attireid,$count,$maxcount=0xFFFFFFFF ){
		$pri_msg=pack("LLLL",2,$attireid,$count, $maxcount  );
		$sendbuf=$this->park("111C",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_del_jy_attire ($userid, $attireid,$count, $maxcount=0xFFFFFFFF   ){
		$pri_msg=pack("LLLLL",2,$attireid,$count,$maxcount,0);
		//通过卖出协议
		$sendbuf=$this->park("1117",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function user_del_home_attire ($userid, $attireid,$count, $maxcount=0xFFFFFFFF   ){
		$pri_msg=pack("LLLLL",1,$attireid,$count,$maxcount,0);
		//通过卖出协议
		$sendbuf=$this->park("1117",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_attire_ex($userid, $attireid,$usedcount,$chestcount,$count){
		$pri_msg=pack("LLLL",$attireid,$usedcount,$chestcount,$count);
		$sendbuf=$this->park("111A",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_xiaomee_max_info($userid, $xiaomee_max, $sale_xiaomee_max,$ol_last){
		$pri_msg=pack("LLL",$xiaomee_max, $sale_xiaomee_max,$ol_last );
		$sendbuf=$this->park("113D",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function user_set_pet_attire_ex($userid,$petid, $attireid,$usedcount,$count){
		$pri_msg=pack("LLLL",$petid ,$attireid,$usedcount,$count);
		$sendbuf=$this->park("116A",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function user_sale_attire ($userid,$attiretype, $attireid,$count,$xiaomee=0){
		$pri_msg=pack("LLLLL",$attiretype, $attireid,$count,0xFFFFFFFF,$xiaomee);
		$sendbuf=$this->park("1117",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_task_value($userid,$taskid, $value){
		$pri_msg=pack("LL",$taskid,$value );
		$sendbuf=$this->park("1107",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function user_set_knight_data($userid )
	{
		$attireidlist= array ( 12356,	12357,	12358,	12359,	12360,	
			12361,	12362,	12363,	12364,	12365,	12366,	12367,	);
		foreach ($attireidlist as $attireid ){
			 $this->user_set_attire_ex($userid,$attireid,0,0,1 );
			
		}
		//设置其被rickey选中
		$this->user_card_set_flag_bit($userid,1,1 );
		//设置骑士标志为勋章骑士
		$this->user_profession_set($userid,3,1  );

		//设置见习骑士任务已完成
		$this->user_set_task_value($userid,151,2  );
	}






	function user_set_attire_used($userid,$attireid,$count){
		$pri_msg=pack("LLLL",$attireid,3,$count,0xFFFFFFFF);
		$sendbuf=$this->park("1136",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_attire($userid,$optertype, $attireid,$count){
		$pri_msg=pack("LLLL",$attireid,$optertype,$count,0xFFFFFFFF);
		$sendbuf=$this->park("1136",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_user_enabled($userid){
		$pri_msg="";
		$sendbuf=$this->park("110F",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function user_card_set_flag_bit($userid,$bitid,$value ){
		$pri_msg=pack("LL",$bitid,$value );
		$sendbuf=$this->park("117E",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}


	function su_add_admin($adminid,$nick){
		$pri_msg=pack("La16",$adminid,$nick );
		$sendbuf=$this->park("A109",0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function su_get_adminid_by_nick($nick){
		$pri_msg=pack("a16",$nick);
		$sendbuf=$this->park("A00C",0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Ladminid");
	}


	function user_change_nick($userid,$nick){
		$pri_msg=pack("a16",$nick );
		$sendbuf=$this->park("114A",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_home_attire_noused($userid,$attireid, $type=1){
		$pri_msg=pack("LL", $type,$attireid );
		$sendbuf=$this->park("1145",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function su_admin_set_flag($adminid,$flag){
		$pri_msg=pack("LL",$adminid,$flag);
		$sendbuf=$this->park("A10A",0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function other_send_email($userid){
		$pri_msg=pack("LL",1,0);
		$sendbuf=$this->park("1114",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_farm_get_animal_list($userid){
		$pri_msg="";
		$sendbuf=$this->park("10CF",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . 
				"/Lid_$i".
				"/Luserid_$i".
				"/Lanimalid_$i".
				"/Lstate_$i".
				"/Lgrowth_$i".
				"/Leattime_$i".
				"/Ldrinktime_$i".
				"/Ltotal_output_$i".
				"/Loutput_time_$i".
				"/Llastcal_time_$i".
				"/Lmature_$i".
				"/Lanimal_type_$i".
				"/Lfavor_time_$i".
				"/Lfavor_$i".
				"/Loutgo_$i".
				"/Lpollinate_num_$i".
				"/Lmax_output_$i";

		}
		return $this->unpark($recvbuf,$fmt );
	}
	function user_reset_home($userid){
		$pri_msg=pack("La12",0,"");
		$sendbuf=$this->park("11A3",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_reset_init_home($userid){
		$pri_msg=pack("LC12",160030, 0, 0, 0, 0, 0, 1, 6, 3, 0, 0, 0, 0);
		$sendbuf=$this->park("11A3",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function user_farm_set_animal(
				$id,
				$userid,
				$animalid,
				$state,
				$growth,
				$eattime,
				$drinktime,
				$total_output,
				$output_time,
				$lastcal_time,
				$mature,
				$animal_type,
				$favor_time,
				$favor,
				$outgo,
				$pollinate_num,
				$max_output
		  	){

		$pri_msg=pack("L17",
				$id,
				$userid,
				$animalid,
				$state,
				$growth,
				$eattime,
				$drinktime,
				$total_output,
				$output_time,
				$lastcal_time,
				$mature,
				$animal_type,
				$favor_time,
				$favor,
				$outgo,
				$pollinate_num,
				$max_output
			   	);
		$sendbuf=$this->park("11DC",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function user_get_home($userid,$homeid){
		$pri_msg=pack("L", $homeid);
		$sendbuf=$this->park("101D",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="a16home_type_info/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/a12value_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function user_get_used_attirelist($userid,$attiretype){
		$pri_msg=pack("L", $attiretype );
		$sendbuf=$this->park("102F",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/a12value_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function user_get_jy_ex($userid){
		$pri_msg="";
		$sendbuf=$this->park("1024",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/a12value_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}
	function user_class_change_info($userid,$class_jion_flag,$class_access_flag,$class_name,$class_slogan  ){
		$pri_msg=pack("LLa16a60", $class_jion_flag,$class_access_flag,$class_name,$class_slogan  );
		$sendbuf=$this->park("11F2",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function user_class_add_attire ($userid, $attireid,$count ){
		$pri_msg=pack("LLLLa25",$attireid,$count,9999,$userid,"" );
		$sendbuf=$this->park("11F5",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_class_add_member($userid, $memberid){
		$pri_msg=pack("L",$memberid );
		$sendbuf=$this->park("11F3",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_add_class($userid, $classid){
		$pri_msg=pack("L",$classid);
		$sendbuf=$this->park("116C",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function roominfo_gen_vip_del($userid ){
		$sendbuf=$this->park("B53A",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function user_class_reduce_attire ($userid, $attireid,$count ){
		$pri_msg=pack("LL",$attireid,$count);
		$sendbuf=$this->park("11FB",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_class_get_noused_attire_list($userid){
		$sendbuf=$this->park("10FA",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/Lcount_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}
	
	function user_get_classlist($userid){
		$sendbuf=$this->park("106F",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lclass_firstid/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lclassid_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}
	


	function user_class_get_info($userid){
		$pri_msg="";
		$sendbuf=$this->park("10F1",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Linterest/Lclass_logo/Lclass_color/Lclass_word/Lclass_jion_flag/Lclass_access_flag".
			"/a16class_name/a60class_slogan/a16ownernick/Lmember_count/Lused_attire_count";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$member_count=$recvarr["member_count"];
		for ($i=0;$i<$member_count;$i++){
			$fmt=$fmt . "/Lmemberid_$i";
		}
		$used_attire_count=$recvarr["used_attire_count"];
		for ($i=0;$i<$used_attire_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/a12value_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}



	function usermsg_add_report_deal($userid, $adminid, $deal_type ,$reason,$nick ){
		$pri_msg=pack("LLLA16",$adminid, $deal_type ,$reason,$nick );
		$sendbuf=$this->park("910A",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function usermsg_get_report_userid($userid,$startdate, $enddate, $index ){
		$pri_msg=pack("LLL",$startdate,$enddate,$index);
		$sendbuf=$this->park("9005",$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/a21date_$i/Lsrcuserid_$i/Lreason_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function usermsg_get_deal_report_list($userid,$startdate, $enddate, $index ){
		$pri_msg=pack("LLL",$startdate,$enddate,$index);
		$sendbuf=$this->park("900C",$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/a21date_$i/Lobjuserid_$i/Ladminid_$i/Ldeal_type_$i/Lreason_$i/a16nick_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}


	function usermsg_clear_by_report_userid($userid,$startdate, $enddate ){
		$pri_msg=pack("LLL",$startdate,$enddate,0);
		$sendbuf=$this->park("910B",$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lsrcuserid_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function game_score_get_list($gameid){
		$pri_msg=pack("L",$gameid );
		$sendbuf=$this->park("5001",0 , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Luserid_$i/a16nick_$i/Lscore_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function su_get_admin_list($index){
		$pri_msg=pack("L",$index);
		$sendbuf=$this->park("A008",0 , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ladminid_$i/Lflag_$i/a16nick_$i/Lused_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}




	function user_set_flag_change_passwd($userid){
		$pri_msg=pack("L", 1);
		$sendbuf=$this->park("015A",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_flag_noused($userid,$flag  ){
		$pri_msg=pack("L", $flag);
		$sendbuf=$this->park("115B",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_flag_offline24($userid,$flag  ){
		$pri_msg=pack("L", $flag);
		$sendbuf=$this->park("115C",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_vip_flag($userid, $vip, $level, $vip_start_time, $vip_end_time, $vip_is_auto_charge, $viP_charge_change_last ){
		$pri_msg=pack("LLLLCL", $vip,$level,$vip_start_time, $vip_end_time, $vip_is_auto_charge, $vip_charge_change_last);
		$sendbuf=$this->park("1114",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function usermsg_del_writing($userid,$date,$adminid  ){
		$msg=$adminid;
		$msglen=strlen($msg);
		$pri_msg=pack("La21LLa".$msglen,$userid, $date,$msglen+4,time(NULL), $msg);
		$sendbuf=$this->park("9106",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function usermsg_set_report_msg($userid,$objid,$reason ){
		$pri_msg=pack("LL",  $objid,$reason );
		$sendbuf=$this->park("9101",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function usermsg_clear_writing($startdate,$enddate ){
		$pri_msg=pack("LL",  $startdate,$enddate );
		$sendbuf=$this->park("9108",0,$pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function usermsg_set_writing_report($userid, $date,$type,$flag,$msg){
		$msglen=strlen($msg);
		$pri_msg=pack("a21LLLLa".$msglen, $date,$type, $flag,$msglen+4,time(NULL), $msg);
		$sendbuf=$this->park("9107",$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		return $this->unpark($recvbuf, "");
	}

	function user_del_month($userid){
		$sendbuf=$this->park("3120",$userid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_connect_add_child($userid,$childid){
		$pri_msg=pack("L",  $childid);
		$sendbuf=$this->park("1160",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_connect_set_parentid($userid,$parentid){
		$pri_msg=pack("L",  $parentid);
		$sendbuf=$this->park("1161",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function user_connect_get_info($userid){
		$sendbuf=$this->park("1062" ,$userid, "");
		return $this->unpark($this->sock->sendmsg($sendbuf),
				"Lparentid/Lold_childcount/Lchildcount");
	}

	function su_admin_login($user_info,$md5_passwd){
		$adminid=0;
		if (!ereg("^[0-9]+$",$user_info)){
			$recv_arr=$this->su_get_adminid_by_nick( $user_info);
			if ($recv_arr["result"]==0){//成功
				$adminid=$recv_arr["adminid"];
			}else {
				return $recv_arr;
			}
		}else{
			$adminid=$user_info;
		}
		return $this->su_login_by_id($adminid ,$md5_passwd );
	}

	function user_login($user_info,$md5_passwd , $ip=0,$login_channel=0 ){
		$userid=0;
		if (!ereg("^[0-9]+$",$user_info)){
			$recv_arr=$this->user_get_userid_by_email( $user_info);
			if ($recv_arr["result"]==0){//成功
				$userid=$recv_arr["userid"];
			}else {
				return $recv_arr;
			}
		}else{
			$userid=$user_info;
		}
		return $this->user_login_by_id($userid ,$md5_passwd, $ip,$login_channel );
	}

	function pay_get_damee_record ($userid,$startdate,$enddate){
		$tmp_startindex=0;
		$tmp_startdate=$startdate;
		$count=0;
		$result_arr=array();
		while ( $tmp_startdate !=0  ){
			$pri_msg=pack("LLLL",$tmp_startdate,$enddate,$tmp_startindex,10);
			$sendbuf=$this->park("301B",$userid, $pri_msg);
			$recvbuf=$this->sock->sendmsg($sendbuf);	
			$fmt="Lnext_startdate/Lnext_startindex/Lcount";
			$recvarr=$this->unpark($recvbuf,$fmt );		
			if ($recvarr && $recvarr["result"]!=SUCC){
				return $recvarr;
			}
			//SUCC
			$recv_count=$recvarr["count"];
			for ($i=$count;$i<$count+$recv_count;$i++){
				$fmt=$fmt . "/Ldate_$i/Lpaytype_$i/ldamee_$i/Lleave_damee_$i/A20desc_$i"  ;
			}

			$tmp_arr=$this->unpark($recvbuf,$fmt );
			for ($i=$count;$i<$count+$recv_count;$i++){
				$result_arr["date_$i"]=date ("Y-m-d H:G:s", $tmp_arr["date_$i"] );
				$result_arr["paytype_$i"]=$tmp_arr["paytype_$i"];
				$result_arr["damee_$i"]=$tmp_arr["damee_$i"];
				$result_arr["leave_damee_$i"]=$tmp_arr["leave_damee_$i"];
				$result_arr["desc_$i"]=$tmp_arr["desc_$i"];
				//echo "$i==".$result_arr["date_$i"]."==".$result_arr["paytype_$i"].
				//	"==". $result_arr["damee_$i"] ."==" . $result_arr["leave_damee_$i"].
				//   "==".$result_arr["desc_$i"] . "<br>" ;
			}
			$count+=$recv_count;
			$tmp_startdate=$tmp_arr["next_startdate"];
			$tmp_startindex=$tmp_arr["next_startindex"];
		}
		$result_arr["count"]=$count;
		$result_arr["result"]=$tmp_arr["result"];
		return  $result_arr; 
	}

	function user_get_nick($userid){
		$pri_msg="";
		$sendbuf=$this->park("104B",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"a16nick");
	}

	function pp_get_nick($userid){
		$pri_msg="";
		$sendbuf=$this->park("181B",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"a16nick");
	}
	function pp_dding_get_info($userid ){
		$pri_msg="";
		$sendbuf=$this->park(1877,$userid, $pri_msg);
		$out_msg="Lflag/Lstat/a16nick/Lsuper_flag/Lcolor/Lenergy_value/Lclose_value/Lwisdom_value/Sai/Lbirth/Ltime_start/Lenergy_stamp/Lclose_stamp/a20funclist/Lprct_count/".
			"Lprct_cap_tm_1/Lprct_mon_id_1/Lprct_end_tm_1/Lprct_day_1/".
			"Lprct_cap_tm_2/Lprct_mon_id_2/Lprct_end_tm_2/Lprct_day_2/".
			"Lprct_cap_tm_3/Lprct_mon_id_3/Lprct_end_tm_3/Lprct_day_3/".
			"Lprct_cap_tm_4/Lprct_mon_id_4/Lprct_end_tm_4/Lprct_day_4/" ;
		return $this->unpark($this->sock->sendmsg($sendbuf),$out_msg);
	}


	function temp_gs_get_value($userid){
		$pri_msg="";
		$sendbuf=$this->park("D010",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Ltask/Lcount");
	}

	function temp_gs_set_task($userid ){
		$pri_msg=pack("L",10);
		$sendbuf=$this->park("D111",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function temp_gs_set_count($userid ){
		$pri_msg=pack("L",1);
		$sendbuf=$this->park("D112",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}






	function pay_record_once($paytype,$old_transid,$userid, $damee, $months,$msg=""){
		$pri_msg=pack("LLLLLLA30", date("Ymd"), $old_transid ,
				$paytype,$userid, $months,$damee,$msg );
		$sendbuf=$this->park(3119,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Ltransid");
	}

	function pay_genrecord_pre($paytype,$userid, $damee, $months,$msg=""){
		$old_transid=0;	
		$pri_msg=pack("LLLLLLA30", date("Ymd"), $old_transid ,
				$paytype,$userid, $months,$damee,$msg );
		$sendbuf=$this->park(3117,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Ltransid");
	}

	function pay_record_true($transid, $damee ){
		$pri_msg=pack("LL",$transid,$damee );
		$sendbuf=$this->park(3118,0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function month_by_damee($userid,  $months ,$usedamee,$automonthed ){
		$pri_msg=pack("LLL",$months ,$usedamee,$automonthed  );
		$sendbuf=$this->park(3104, $userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function set_no_month($userid){
		$pri_msg=null;
		$sendbuf=$this->park("310B", $userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	// send to on line ....
	function online_set_user_offline($userid, $flag){
		$pri_msg=pack("Ll",$userid ,$flag );
		$sendbuf=$this->park("EA61", 0 , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	// send to on line ....
	function switch_set_user_offline($userid, $flag){
		$pri_msg=pack("Ll",$userid ,$flag );
		$sendbuf=$this->park("F618", 0 , $pri_msg);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	function switch_send_npc($serverid , $msg,$msglen ){
		$pri_msg=pack("LLa".$msglen ,$serverid,$msglen+4, $msg );
		$sendbuf=$this->park("F61B", 0 , $pri_msg);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}
	/*
		PP
	   */
	function pp_switch_send_msg($userid,$npcid, $msg){
		$msglen=strlen($msg)+1;
		$in=pack("LLLLa".$msglen, $userid,$npcid,time(NULL),$msglen,$msg   );

		$sendbuf=$this->park("EA61", 0 ,$in);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	/*
	    通过switch server 让用户下线
		reason定义:
			100101使用不文明昵称永久封停
			100102使用不文明昵称24小时封停
			100103使用不文明昵称一周封停
			100104使用不文明昵称两周封停
			
			100201使用不文明用语永久封停
			100202使用不文明用语24小时封停
			100203使用不文明用语一周封停
			100204使用不文明用语两周封停
			
			100301索要个人信息永久封停
			100302索要个人信息24小时封停
			100303索要个人信息一周封停
			100304索要个人信息两周封停
			
			100401外挂永久封停
			100402外挂24小时封停
			100403外挂一周封停
			100404外挂两周封停
			
			100501强制改名被踢下线
	*/
	function pp_switch_set_user_offline($userid,$reason){
		$in=pack("LL", $reason);
		$sendbuf=$this->park("EA62", 0 ,$in);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}





	// send to switch
	/*
	   $onlineid: 	接收消息的OnlineID，0：不指定
	   $mapid:		接收消息的地图ID，	0：不指定
	   $picid:		消息图片的ID，		0：文本	
	   $jumptomapid:跳转的地图ID，		0：不指定
	   $uid_array:	接收消息的用户米米号数组，空数组：不指定
	   $taskid:		任务ID，			0：不指定			
	   $taskstate:	任务状态，			0：未接，1：已接，2：完成
	   $msg:		消息内容，小于400字节
	*/
	function switch_send_bubble_msg($onlineid,$mapid,$picid,$jump_to_mapid,$uid_array,$taskid,$taskstate,$msg) {
		$msgtype = 0;
		if($picid>0 && $picid<10){
			$msgtype=$picid;   
			$picid=0;   
		}

		if($taskid > 400) {
			$taskid = 0;
			$taskstate = 0;
		}

		$i = 0;
		$msglen = strlen($msg) + 1;

		$len = $msglen;
		$uid_count=count($uid_array);
		for($i=0;$i < $uid_count; $i++) {
			$msg = pack("La".$len,$uid_array[$i],$msg);
			$len = $len + 4;
		}

		$packet = pack("LLLLLLLLLa".$len, $onlineid, $mapid, $picid,
				$jump_to_mapid, $uid_count, $taskid, $taskstate, $msgtype, $msglen, $msg);

		$sendbuf=$this->park("F61A", 0 ,$packet);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	function switch_send_msg($serverid,$userid,$pic_id, $msg){
		$msgtype=0;

		//$pic_id 是指类型
		if($pic_id>0 &&  $pic_id<10 ){
			$msgtype=$pic_id;	
			$pic_id=0;
		}

		$msglen=strlen($msg)+1;
		$off_line_pri_msg=pack("LNCNNNNNNNNA16NNNNa".$msglen , 
				65+4+4+$msglen,61+4+4+$msglen, 1 ,10003,0,0, 
				$msgtype ,0,0,0,0, "淘米委员会",0,time(null),$pic_id ,
			   	$msglen,$msg  );

		$off_line_pri_msg_ex=pack("NCNNNNNNNNA16NNNNa".$msglen , 
				61+4+4+$msglen,1  ,10003,0,0, 
				$msgtype ,0,0,0,0, "淘米委员会",0,time(null),$pic_id ,
			   	$msglen,$msg  );

		$msglen=strlen($off_line_pri_msg_ex);	
		$on_line_pri_msg=pack("LLLsa".$msglen ,$serverid,$userid ,0 , $msglen,$off_line_pri_msg_ex);
		$sendbuf=$this->park("F619", $userid ,$on_line_pri_msg . $off_line_pri_msg);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	function switch_user_online_info($userid){
		$pri_msg=pack("L",$userid);
		$sendbuf=$this->park("F61A", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lserverid/a16servername");
	}

	// send to on line ....
	function online_send_msg($userid, $msg){

		$msglen=strlen($msg)+1;
		$pri_msg=pack("LLsa".$msglen ,$userid ,0 , $msglen,$msg );
		$sendbuf=$this->park("EA62", 0 , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function su_del_admin($adminid){
		$sendbuf=$this->park("A10B",$adminid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_test($userid){
		$sendbuf=$this->park("1000",$userid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),"a201sermsg");
	}

	function get_server_info(){
		$sendbuf=$this->park("1000",$userid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),"a201sermsg");
	}


	function email_map_userid_email($userid,$email){
		$pri_msg=pack("A64",  $email);
		$sendbuf=$this->park("4105",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function cup_add_cup($userid,$groupid,$gameid,$cup_type ){
		$pri_msg=pack("LLL", $groupid,$gameid,$cup_type );
		$sendbuf=$this->park("D101",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function cup_get_cup($userid){
		$sendbuf=$this->park("D002",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcup1/Lcup2/Lcup3/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lgroupid_$i/Lcup1_$i/Lcup2_$i/Lcup3_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function cup_get_last_userlist(){
		$sendbuf=$this->park("D003",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Luserid_$i/Lgameid_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function cup_get_group_msg(){
		$sendbuf=$this->park("D004",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lgroupid_$i/Lcup1_$i/Lcup2_$i/Lcup3_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	function cup_get_group_history_msg(){
		$sendbuf=$this->park("D005",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ldate_$i/Lgroupid_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	//EMAILSYS
	function emailsys_add_email($userid,$type,$senderid,$sendernick,$msg ){
		$msglen=strlen($msg);
		$pri_msg=pack("LLLA16LLa".$msglen,$type,time(NULL),$senderid,$sendernick,0,$msglen,$msg  );
		$sendbuf=$this->park("E101",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function emailsys_get_email($userid,$emailid){
		$msglen=strlen($msg);
		$pri_msg=pack("L",$emailid );
		$sendbuf=$this->park("E102",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function emailsys_get_email_msg($userid){
		$pri_msg="";
		$sendbuf=$this->park("E005",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lemail_noread");
	}


	function emailsys_del_email($userid,$emailid){
		$msglen=strlen($msg);
		$pri_msg=pack("L",$emailid );
		$sendbuf=$this->park("E104",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function emailsys_get_emailid_list($userid){
		$sendbuf=$this->park("E003",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function hope_add_hope($send_id,$recv_id,$recv_type){
		$pri_msg=pack("La16LLa30",$send_id, "showessage", $recv_id, $recv_type,"小帽帽");
		$sendbuf=$this->park("1501",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function hope_check_hope($send_id){
		$pri_msg=pack("L",$send_id );
		$sendbuf=$this->park("1402",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function hope_get_hope($send_id,$recv_id,$hopedate){
		$pri_msg=pack("LLL",$send_id,$recv_id,$hopedate);
		$sendbuf=$this->park("1503",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function hope_set_hope($send_id,$recv_id,$old_hopedate,$new_hopedate,$new_recv_type){
		$pri_msg=pack("LLLLL",$send_id,$recv_id,$old_hopedate,$new_hopedate,$new_recv_type );
		$sendbuf=$this->park("1507",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function hope_get_hope_list($recv_id){
		$pri_msg=pack("L",$recv_id);
		$sendbuf=$this->park("1404",$userid, $pri_msg );
		$recvbuf=$this->sock->sendmsg($sendbuf);

		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lhopedate_$i/Lsend_id_$i/a16send_nick_$i/Lrecv_type_$i"  ;
		}
		return $this->unpark( $recvbuf, $fmt); 
	}


	function user_check_userid($userid){
		$sendbuf=$this->park("1005",$userid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_home_attire_change($userid){
		$pri_msg=pack("LLLL",160065,0,1,12 );
		$sendbuf=$this->park("1147",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_swap_homeattire($userid){
		$pri_msg=pack("LLLLLLL",1,1,160006,1, 160068,10, 99 );
		$sendbuf=$this->park("111F",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function userinfo_check_userid($userid){
		$cmdid="0005";
		$in_msg="";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function temp_pet_sports_set_score($userid,$petid,$stype,$score){
		$cmdid="D148";
		$in_msg=pack("LLLA16A16",$petid,$stype,$score,"jim","天才");
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}



	function userinfo_get_grouplist($userid){
		$sendbuf=$this->park("10C0" ,$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		//echo "fffff ".$recv_count;
		//echo "fffff ".$recv_count;
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lgroupid_$i/Lflag_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}



	function userinfo_sync_dv($userid){
		$cmdid="018A";
		$in_msg="";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function user_get_temp_value($userid){
		$cmdid="1083";
		$in_msg="";
		$out_msg="Lvalue";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}



	function confect_change_value($userid,$vipflag,$type,$change_value){
		$cmdid="D106";
		$in_msg=pack("LLl",$vipflag,$type,$change_value );
		$out_msg="Lflag/lreal_add_count/Lcount_all";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function confect_get_value($userid){
		$cmdid="D007";
		$in_msg="";
		$out_msg="Lflag/Ltask/Lcount_all";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function confect_set_count_all($userid, $task,$flag ,$count_all){//
		$cmdid="D109";
		$in_msg=pack("LLL",$task,$flag ,$count_all   );
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function sysarg_clear_user_day($userid){
		$cmdid="C104";
		$in_msg="";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function emailsys_clean_email($userid){
		$cmdid="E106";
		$in_msg="";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function sysarg_day_add($userid){
		$cmdid="C101";
		$in_msg=pack("LL",1000000100,1 );
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function sysarg_day_query($userid){
		$cmdid="C002";
		$in_msg=pack("L",1000000100);
		$out_msg="Lcount";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function sysarg_day_set($userid,$type,$count){
		$cmdid="C107";
		$in_msg=pack("LL",$type,$count );
		return  $this->send_cmd($cmdid,$userid,$in_msg,"");
	}
	
	
	function sysarg_get_value_list(){
		$in_msg=pack("LL",0,0xFFFFFFFF);
		$cmdid="C013";
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ltype_$i/Lvalue_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function sysarg_get_questionnaire_list(){
		$cmdid="C014";
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lmaintype_$i/Lsubtype_$i/Lvalue_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function sysarg_day_get_list_by_userid( $userid){
		$cmdid="C006";
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ltype_$i/Lcount_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}


	function userinfo_get_gameflag($userid){
		$cmdid="0007";
		$in_msg="";
		$out_msg="Lgameflag";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function userinfo_get_info_all($userid){
		$cmdid="0082";
		$in_msg="";
		$out_msg= "Lflag1/Lregflag/a16passwd/a16paypasswd/a64email/".
			"a64parent_email/a64passwd_email/a64question/a64answer/a128signature/".
			"Lsex/Lbirthday/a16telephone/a16mobile/Lmail_number/".
			"vaddr_province/vaddr_city/a64addr/a64interest";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function userinfo_set_for_pay($userid,$parent_email, $question,$answer,
			$birthday, $telephone,$mobile, $mail_number,
			$addr_province,$addr_city, $addr, $interest 	){
		$cmdid="0183";
		$in_msg=pack("a64a64a64La16a16Lvva64a64",
			$parent_email, $question,$answer,
			$birthday, $telephone,$mobile, $mail_number,
			$addr_province,$addr_city, $addr, $interest 	
				);
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function userinfo_set_pemail_by_qa($userid,$parent_email, $question,$answer ){
		$cmdid="0187";
		$in_msg=pack("a64a64a64", $parent_email, $question,$answer);
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function userinfo_set_pemail_isok($userid ){
		$cmdid="0188";
		$in_msg="";
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	

	
	function userinfo_set_info_ex($userid,$sex,$birthday,$telephone,$mobile,
		 $mail_number,$addr_province,$addr_city,$addr,$interest ){
		$pri_msg=pack("LLA16A16LvvA64A64",
			  $sex,$birthday,$telephone,$mobile,
		           $mail_number,$addr_province,$addr_city,$addr,$interest);
		$sendbuf=$this->park("0181",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function userinfo_get_info_ex($userid ){
		$cmdid="0080";
		$in_msg="";
		$out_msg="Lsex/Lbirthday/a16telephone/a16mobile/Lmail_number".
			"/vaddr_province/vaddr_city/a64addr/a64interest";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}




	function room_add_vocter($userid,$voctoer){
		$pri_msg=pack("LA25", $voctoer,"useid".$voctoer  );
		$sendbuf=$this->park("B504",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function room_get_hot($userid){
		$pri_msg="";
		$sendbuf=$this->park("B401",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lhot/Lflower/Lmud");
	}

	function room_set_value($userid,$flower,$mud){
		$pri_msg=pack("LL", $flower,$mud );
		$sendbuf=$this->park("B512",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function roominfo_question_get($userid){
		$pri_msg="";
		$sendbuf=$this->park("B431",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lquestion_id/Lright_count/Lquestion_count");
	}

	function roominfo_get_point($userid){
		$pri_msg="";
		$sendbuf=$this->park("B43D",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Cv_1/Cv_2/Cv_3/Cv_4/Cv_5");
	}
	function roominfo_set_point($userid,$v ){
		$pri_msg=pack("L",$v   );
		$sendbuf=$this->park("B53E",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function roominfo_clear_pos($userid,$v ){
		$pri_msg=pack("L",$v  );
		$sendbuf=$this->park("B543",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function roominfo_get_pos($userid){
		$pri_msg="";
		$sendbuf=$this->park("B43F",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lcount/Cv_1/Cv_2/Cv_3/Cv_4/Cv_5");
	}
	
	function roominfo_set_pos($userid,$v ){
		$pri_msg=pack("L",$v   );
		$sendbuf=$this->park("B542",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}





	function roominfo_question_set($userid){
		$pri_msg=pack("LL", 1 , 1000000000 );
		$sendbuf=$this->park("B530",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function room_get_v_list($userid){

		$sendbuf=$this->park("B405",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function room_init_user($userid){
		$sendbuf=$this->park("B510",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function userinfo_check_question($userid,$q,$a ){
		$cmdid="0084";
		$in_msg=pack("a64a64",  $q,$a  );
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function groupmain_gen_id(){
		$sendbuf=$this->park("3501",0, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lgroupid");
	}
	function group_add_group($userid,$groupid ){
		$pri_msg=pack("LA25LA255", $userid,"groupnamename",1, "showmessage"  );
		$sendbuf=$this->park("D501",$groupid , $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function group_add_member($groupid,$ownerid,$memberid ){
		$pri_msg=pack("LL",$ownerid, $memberid);
		$sendbuf=$this->park("D502",$groupid , $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function group_del_member($groupid,$ownerid,$memberid ){
		$pri_msg=pack("LL",$ownerid, $memberid);
		$sendbuf=$this->park("D503",$groupid , $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function group_del_group($groupid,$ownerid){
		$pri_msg=pack("L",$ownerid );
		$sendbuf=$this->park("D506",$groupid , $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function group_get_info($groupid){
		//$pri_msg=pack("L",$ownerid );
		$sendbuf=$this->park("D405",$groupid , $pri_msg  );
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lownerid/a25groupname/Ltype/a121groupmsg/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lmemberid_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function userinfo_join_group($userid,$groupid){
		$pri_msg=pack("LL",$groupid,0 );
		$sendbuf=$this->park("0191",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function userinfo_del_group($userid,$groupid){
		$pri_msg=pack("L",$groupid );
		$sendbuf=$this->park("0194",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function userinfo_set_group_flag($userid,$groupid,$flag){
		$pri_msg=pack("LL",$groupid,$flag );
		$sendbuf=$this->park("0195",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}




//支付平台
	//验正支付密码	
	function userinfo_check_paypasswd($userid,$md5_pay_passwd){
		$pri_msg=pack("A16",hex2bin_pri($md5_pay_passwd) );
		$sendbuf=$this->park("008B",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//设置支付密码	
	function userinfo_set_paypasswd($userid,$md5_pay_passwd){
		$pri_msg=pack("A16",hex2bin_pri($md5_pay_passwd) );
		$sendbuf=$this->park("018C",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//是否已经设置过支付密码
	function userinfo_isset_paypasswd($userid){
		//$pri_msg=pack("A16",hex2bin_pri($md5_pay_passwd) );
		$sendbuf=$this->park("008D",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lis_set_flag");
	}

	//清空支付passwd
	function userinfo_clean_paypasswd($userid){
		//$pri_msg=pack("A16",hex2bin_pri($md5_pay_passwd) );
		$sendbuf=$this->park("018F",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	//设置问题与答案	
	function userinfo_set_qa($userid, $question,$answer ){
		$cmdid="018E";
		$in_msg=pack("a64a64", $question,$answer);
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	//得到问题
	function userinfo_get_question($userid){
		$cmdid="0070";
		$out_msg="a64question";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	//检查问题与答案	
	function userinfo_check_qa($userid,$q,$a ){
		$cmdid="0084";
		$in_msg=pack("a64a64",  $q,$a  );
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function user_set_birthday_sex($userid,$birthday,$sex ){
		$pri_msg=pack("LL",$birthday,$sex);
		$sendbuf=$this->park("1141",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function switch_sync_vip($userid){
		$pri_msg=pack("LLLL", 1,1,1,1);
		$sendbuf=$this->park("EA63",$userid,$pri_msg );
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	function log_add_change ($type,$userid,$logtime,$changevalue,$v1 ,$v2  ){
		$pri_msg=pack("LLLlll", $type,$userid,$logtime,$changevalue,$v1 ,$v2 );
		$sendbuf=$this->park("F103",$userid,$pri_msg );
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	function ff_login_add($userid,$loginflag,$onlineid,$logintime){
		$pri_msg=pack("LLL", $loginflag,$onlineid,$logintime );
		$sendbuf=$this->park("F105",$userid,$pri_msg );
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	function ff_login_get_list($userid,$logdate){
		$in_msg=pack("L",$logdate);
		$cmdid="F007";
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Luserid_$i/Lloginflag_$i/Lonlineid_$i/Llogintime_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}





	function history_ip_record ($userid,$game,$ip ){
		$in_msg=pack("LLL", $userid,$game,$ip );
		$cmdid="F106";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function history_ip_get($userid){
		$in_msg="";
		$cmdid="F008";
		return  $this->send_cmd($cmdid,$userid,$in_msg,"Lip");
	}





	//得到摩尔豆
	function user_get_xiaomee ($userid){
		$in_msg="";
		$cmdid="100D";
		$out_msg="Lxiaomee";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	//得到vip标志和昵称
	function user_get_vip_nick ($userid){
		$in_msg="";
		$cmdid="100E";
		$out_msg="Lvipflag/a16nick";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	//得到登入邮箱
	function user_get_email ($userid){
		$type=0;
		$in_msg="";
		$cmdid="0044";
		$out_msg="a64email";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}





	//产生随机码
	function userserial_gen_serial ($userid){
		$type=0;
		$in_msg=pack("L", $type);
		$cmdid="D130";
		$out_msg="A8serialid";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	//查询随机码
	function userserial_get_serial ($userid){
		$type=0;
		$in_msg=pack("L", $type);
		$cmdid="D032";
		$out_msg="A8serialid";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	//删除随机码
	function userserial_del_serial ($userid){
		$type=0;
		$in_msg=pack("L", $type);
		$cmdid="D131";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	//设置密码邮箱
	function userinfo_set_passwdemail($userid,$passwdemail){
		$in_msg=pack("A64", $passwdemail);
		$cmdid="0117";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	//设置密码邮箱-ex 不要确认的
	function userinfo_set_passwdemail_ex($userid,$passwdemail){
		$in_msg=pack("A64", $passwdemail);
		$cmdid="011D";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}



	//是否设置过密码邮箱
	function userinfo_isset_passwdemail($userid){
		$in_msg="";
		$cmdid="0018";
		$out_msg="Lis_seted";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	//是否确认过密码邮箱
	function userinfo_isconfirm_passwdemail($userid){
		$in_msg="";
		$cmdid="0019";
		$out_msg="Lis_seted";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	//确认密码邮箱
	function userinfo_confirm_passwdemail($userid){
		$in_msg="";
		$cmdid="011A";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	//得到密码邮箱	
	function userinfo_get_passwdemail($userid){
		$in_msg="";
		$cmdid="001B";
		$out_msg="A64email";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function admin_add_admin($adminid,$add_adminid,$nick,$passwd ){
		$in_msg=pack("La16a16",$add_adminid,$nick, hex2bin_pri($passwd));
		$cmdid="5501";
		$out_msg="";
		return  $this->send_cmd($cmdid,$adminid,$in_msg,$out_msg);
	}

	function admin_del_admin($adminid,$del_adminid ){
		$in_msg=pack("L",$del_adminid);
		$cmdid="5504";
		$out_msg="";
		return  $this->send_cmd($cmdid,$adminid,$in_msg,$out_msg);
	}
	function admin_add_power($adminid,$edit_adminid,$powerid ){
		$in_msg=pack("LL",$edit_adminid,$powerid);
		$cmdid="5507";
		$out_msg="";
		return  $this->send_cmd($cmdid,$adminid,$in_msg,$out_msg);
	}
	function admin_del_power($adminid,$edit_adminid,$powerid ){
		$in_msg=pack("LL",$edit_adminid,$powerid);
		$cmdid="5508";
		$out_msg="";
		return  $this->send_cmd($cmdid,$adminid,$in_msg,$out_msg);
	}

	function user_bank_get_account_list($userid){
		$in_msg="";
		$cmdid="1091";
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lnowtime/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lmoney_$i/Lflag_$i/Ltime_$i/Ltimelimit_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	function user_bank_set_account($userid,$old_time,$time,$money,$flag,$timelimit){
		$in_msg=pack("LLLLL",$old_time,$money,$flag,$time,$timelimit );
		$cmdid="1193";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}




	function admin_get_adminlist($index){
		$in_msg=pack("L",$index);
		$cmdid="5402";
		$sendbuf=$this->park($cmdid,0, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ladminid_$i/Lflag_$i/a16nick_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function admin_get_powerlist($adminid ){
		$in_msg="";
		$cmdid="5406";
		$sendbuf=$this->park($cmdid,$adminid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lpowerid_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function admin_login_by_id($adminid,$passwd){
		$in_msg=pack("A16", hex2bin_pri( $passwd )); 
		$cmdid="5405";
		$sendbuf=$this->park($cmdid,$adminid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="a16nick/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lpowerid_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	function admin_get_adminidlist_by_powerid($powerid){
		$in_msg=pack("L", $powerid ); 
		$cmdid="5410";
		$sendbuf=$this->park($cmdid,$adminid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ladminid_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}



	function admin_get_adminid_by_nick($nick)
	{
		$pri_msg=pack("a16",$nick);
		$sendbuf=$this->park("5409",0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Ladminid");
	}

	function admin_login($user_info,$passwd){
		$adminid=0;
		if (!ereg("^[0-9]+$",$user_info)){
			$recv_arr=$this->admin_get_adminid_by_nick( $user_info);
			if ($recv_arr["result"]==0){//成功
				$adminid=$recv_arr["adminid"];
			}else {
				return $recv_arr;
			}
		}else{
			$adminid=$user_info;
		}
		return $this->admin_login_by_id($adminid ,$passwd );
	}




	
	function admin_change_passwd($adminid, $md5_oldpasswd ,$md5_newpasswd ){
		$pri_msg=pack("A16A16", hex2bin_pri( $md5_oldpasswd ), 
				hex2bin_pri($md5_newpasswd));
		$sendbuf=$this->park("5503",$adminid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function admin_root_change_passwd($adminid,$edit_adminid,$md5_newpasswd ){
		$pri_msg=pack("LA16",$edit_adminid, hex2bin_pri($md5_newpasswd));
		$sendbuf=$this->park("5509",$adminid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function admin_set_use_flag($adminid,$edit_adminid,$useflag ){
		$pri_msg=pack("LL",$edit_adminid,$useflag);
		$sendbuf=$this->park("5510",$adminid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}


	function pic_add_pic($userid,$albumid,$fileid){
		$pri_msg=pack("La50",$albumid, $fileid );
		$sendbuf=$this->park("7501",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lphotoid");
	}
	function pic_get_user_ablum_piclist($userid,$albumid,$startindex,$count ){
		$pri_msg=pack("LLL",$albumid,$startindex,$count );
		$sendbuf=$this->park("7402" ,$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lphotoid_$i/a50fileid_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function pic_get_pic_by_photoid_userid($userid,$photoid){
		$pri_msg=pack("L",$photoid);
		$sendbuf=$this->park("7403",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lalbumid/a50fileid");
	}

	function pic_del_pic_by_photoid_userid($userid,$photoid){
		$pri_msg=pack("L",$photoid);
		$sendbuf=$this->park("7504",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"a50fileid" );
	}

	function mole_register($userid,$petcolor){
		$pri_msg=pack("L",$petcolor);
		$sendbuf=$this->park("1103",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"" );
	}







	function user_card_get_info($userid) {
		$sendbuf=$this->park("1075",$userid, "" );
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lflag/Lwin_count/Llose_count/Llower_count/Lsuper_count/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .  "/Lcardid_$i"  ;
		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	function user_card_get_info_ex($userid) {
		$sendbuf=$this->park("107D",$userid, "" );
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lflag/Lwin_count/Llose_count/Llower_count/Lsuper_count/Llogdate/Ldayexp/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .  "/Lcardid_$i"  ;
		}
		return $this->unpark( $recvbuf, $fmt); 
	}


	function user_card_set_win_lose($userid,$flag,$wincount,$losecount,$logdate,$dayexp){
		$pri_msg=pack("LLLLL",$flag,$wincount,$losecount,$logdate,$dayexp);
		$sendbuf=$this->park("117C",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"" );
	}
	function spring_get_value($userid){
		$sendbuf=$this->park("D040",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lflag/Lgold/Lsilver" );
	}

	function spring_init($userid){
		$sendbuf=$this->park("D144",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"" );
	}

	function spring_get_msg($userid){
		$sendbuf=$this->park("D043",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"a100msg" );
	}

	function spring_set_msg($userid,$msg){
		$pri_msg=pack("a100",$msg);
		$sendbuf=$this->park("D142",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"" );
	}

	function spring_change_value($userid,$change_g , $change_l){
		$pri_msg=pack("ll",$change_g,$change_l);
		$sendbuf=$this->park("D141",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"" );
	}

	function spring_get_spring($userid){
		$sendbuf=$this->park("D046",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),
				"Lflag/Lgold/Lgold_logdate/Lgold_count/Lsilver/Lsilver_logdate/Lsilver_count/a100spring_msg" );
	}

	function spring_set_spring($userid, $flag,$gold,$gold_logdate,$gold_count_today,
				$silver,$silver_logdate,$silver_count_today,$spring_msg ){
		$pri_msg=pack("LLLLLLLa100", $flag,$gold,$gold_logdate,$gold_count_today,
				$silver,$silver_logdate,$silver_count_today,$spring_msg
				);
		$sendbuf=$this->park("D147",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "" );
	}



	function bank_add($userid,$xiaomee,$flag,$time ){
		$pri_msg=pack("LLL",$xiaomee, $flag,$time  );
		$sendbuf=$this->park("1190",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//设置用户封停信息
	function user_set_punish_flag($userid,$flag_bit,$bool_set ){
		$pri_msg=pack("LL", $flag_bit,$bool_set  );
		$sendbuf=$this->park("3110",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}


	
	//同步游戏标志位
	function userinfo_add_game($userid,$gameflag ){
		$cmdid="0108";
		$in_msg=pack("L",$gameflag );
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function userinfo_get_login_info($userid){
		$cmdid="0091";
		$in_msg="";
		$out_msg= "a16passwd/Lgameflag";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function userinfo_do_cache($max_cache_count,$del_count_once_over_max,$check_db_passwd_flag=-1,
			$clear_temp_map_cache=0){
		$cmdid="01F0";
		$in_msg=pack("LLLL",$max_cache_count,$del_count_once_over_max,$check_db_passwd_flag,
				$clear_temp_map_cache);
		$out_msg= "Lstart_time/Lcur_cache_count/Lmax_cache_count/Ldel_count_once_over_max/Lcheck_db_passwd_flag/Ldiff_db_count/Lrequest_count/Lhit_count/Lpasswd_err_count/Lcur_user_info_map_count/Lcur_user_flag_map_count/Lcur_user_passwd_map_count";
		return  $this->send_cmd($cmdid,0,$in_msg,$out_msg);
	}

	function userinfo_remove_cache_by_logintime($before_date){
		$cmdid="01F1";
		$in_msg="";
		$out_msg= "";
		return  $this->send_cmd($cmdid,$before_date ,$in_msg,$out_msg);
	}









	function appeal_ex_add_appeal(
				$appeal_type,
				$gameid,
				$userid,
				$realname,
				$moblie,
				$telephone,
				$email,
				$games,
				$birth,
				$first_login,
				$last_login,
				$last_place,
				$vip,
				$vip_type,
				$vip_no,
				$vip_time,
				$mole_tasks,
				$mole_WizardLevel,
				$mole_KnightLevel,
				$mole_IsLastWish,
				$mole_BankBalance1,
				$mole_BankBalance2,
				$seer_tasks,
				$seer_has_teacher,
				$seer_has_student,
				$seer_expirence1,
				$seer_expirence2,
				$seer_money1,
				$seer_money2,
				$seer_fairy_num1,
				$seer_fairy_num2,
				$seer_fairy_name,
				$seer_fairy_level,
				$seer_tower_level
			){
			$id=0;
			$dealtime=0;
			$state=0;
			$adminid=0;
			$logtime=0;
		
			$cmdid="8510";
			$in_msg=pack("LLLLLLLLa16a16a16a64LLLLa64LLa20LLLLLLLLLLLLLLLLa16LL",
				$id,
				$appeal_type,
				$gameid,
				$userid,
				$state,
				$adminid,
				$logtime,
				$dealtime,
				$realname,
				$moblie,
				$telephone,
				$email,
				$games,
				$birth,
				$first_login,
				$last_login,
				$last_place,
				$vip,
				$vip_type,
				$vip_no,
				$vip_time,
				$mole_tasks,
				$mole_WizardLevel,
				$mole_KnightLevel,
				$mole_IsLastWish,
				$mole_BankBalance1,
				$mole_BankBalance2,
				$seer_tasks,
				$seer_has_teacher,
				$seer_has_student,
				$seer_expirence1,
				$seer_expirence2,
				$seer_money1,
				$seer_money2,
				$seer_fairy_num1,
				$seer_fairy_num2,
				$seer_fairy_name,
				$seer_fairy_level,
				$seer_tower_level
			);
		$out_msg= "Lid";
		return  $this->send_cmd($cmdid,0,$in_msg,$out_msg);
	}

	function appeal_get_list( $state ,$dealflag, $userid, $pageid ){
		$cmdid="8402";
		$in_msg=pack("LLLL", $state ,$dealflag, $userid, $pageid);
		$sendbuf=$this->park($cmdid,0, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lid_$i/Lstate_$i/Ldealflag_$i/Ldealtime_$i/Ladminid_$i/Llogtime_$i".
				"/Luserid_$i".
				"/Lflag_$i".
				"/a200why_$i".
				"/Lvip_$i".
				"/Lvip_way_$i".
				"/Lreg_time_$i".
				"/a64reg_email_$i".
				"/Lbirthday_$i".
				"/LOl_last_$i".
				"/a64question_$i".
				"/a64answer_$i".
				"/a16realname_$i/a64email_$i".
				"/a16mobile_$i/a16telephone_$i".
				"/a20realpasswd1_$i/a20realpasswd2_$i".
				"/a20realpasswd3_$i/a200addr_$i/a600othermsg_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	function appeal_ex_get_list( $state , $userid, $pageid){
		$cmdid="8413";
		$in_msg=pack("LLL", $state, $userid ,$pageid );
		$sendbuf=$this->park($cmdid,0, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt.
				"/Lid_$i".
				"/Luserid_$i".
				"/Lstate_$i".
				"/Ladminid_$i".
				"/Llogtime_$i".
				"/Ldealtime_$i".
				"/a16realname_$i".
				"/a16moblie_$i".
				"/a16telephone_$i".
				"/a64email_$i".
				"/Lgames_$i".
				"/Lbirth_$i".
				"/Lfirst_login_$i".
				"/Llast_login_$i".
				"/a64last_place_$i".
				"/Lvip_$i".
				"/Lvip_type_$i".
				"/a20vip_no_$i".
				"/Lvip_time_$i".
				"/Lmole_tasks_$i".
				"/Lmole_WizardLevel_$i".
				"/Lmole_KnightLevel_$i".
				"/Lmole_IsLastWish_$i".
				"/Lmole_BankBalance1_$i".
				"/Lmole_BankBalance2_$i".
				"/Lseer_tasks_$i".
				"/Lseer_has_teacher_$i".
				"/Lseer_has_student_$i".
				"/Lseer_expirence1_$i".
				"/Lseer_expirence2_$i".
				"/Lseer_money1_$i".
				"/Lseer_money2_$i".
				"/Lseer_fairy_num1_$i".
				"/Lseer_fairy_num2_$i".
				"/a16seer_fairy_name_$i".
				"/Lseer_fairy_level_$i".
				"/Lseer_tower_level_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}




	function appeal_get_appeal($id){
		$cmdid="8403";
		$in_msg=pack("L",$id );
		$out_msg= "Lid/Lstate/Ldealflag/Ldealtime/Ladminid/Llogtime".
				"/Lopt_userid".
				"/Lflag".
				"/a200why".
				"/Lvip".
				"/Lvip_way".
				"/Lreg_time".
				"/a64reg_email".
				"/Lbirthday".
				"/LOl_last".
				"/a64question".
				"/a64answer".
				"/a16realname/a64email".
				"/a16mobile/a16telephone".
				"/a20realpasswd1/a20realpasswd2/a20realpasswd3".
				"/a200addr/a600othermsg";
		return  $this->send_cmd($cmdid,0,$in_msg,$out_msg);
	}

	//设置处理状态
	function appeal_ex_get_user_last_state($userid){
		$cmdid="8414";
		$in_msg=pack("L",$userid);
		$out_msg="Lid/Lstate/Llogtime";
		return  $this->send_cmd($cmdid,0,$in_msg,$out_msg);
	}
	//
	function appeal_deal($id, $state,$dealflag,$adminid ){
		$cmdid="8504";
		$in_msg=pack("LLLL",$id,$state,$dealflag,$adminid );
		$out_msg="";
		return  $this->send_cmd($cmdid,0,$in_msg,$out_msg);
	}



	//设置处理状态
	function appeal_ex_deal($id, $state,$adminid ){
		$cmdid="8511";
		$in_msg=pack("LLL",$id,$state,$adminid );
		$out_msg="";
		return  $this->send_cmd($cmdid,0,$in_msg,$out_msg);
	}

	//得到处理状态
	function appeal_ex_get_state($id ){
		$cmdid="8412";
		$in_msg=pack("L",$id);
		$out_msg="Lstate";
		return  $this->send_cmd($cmdid,0,$in_msg,$out_msg);
	}


	//交换物品
	function user_swap_list($userid){
		$cmdid="114D";
		$in_msg=pack("LLL3L4L4",
				1,2,
				0, 0, 10000,
				0, 12002, 5, 99,
				0, 12001, 5, 12
			  	);
		$out_msg="" ;
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

/////////////////////////////////////////////////////////////////////////
	function pp_taskday_clean_by_userid($userid){
		$cmdid="3904";
		$in_msg="";
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function pp_get_info_all($userid){
		$cmdid="180C";
		$in_msg="";

		$friend_str="";
		for ($i=0;$i<200;$i++){
			$friend_str.="/Lfriendid_$i";
		}
		$breakuser_str="";
		for ($i=0;$i<200;$i++){
			$breakuser_str.="/Lbreakuserid_$i";
		}
			
		$out_msg="Lflag/a16nick/Lvip/Lgold/Lvip_rank/Lbase_svalue/Lstart_time/Lvip_month_count/Lend_time/Lauto_incr/Lcolor".
			"/Ltexturize/Lxiaomee/Lxiaomee_max".
			"/Lxiaomee_changetime/Lachievement_title/Lore/LOl_count/LOl_today/LOl_last/LOl_time".
			"/Lparentid/Lchildcount/Lnewchildcount/Lmasterid/Ldiscipleid/Ldisciplecount".
			"/Lexp_pool/a50pet_isswap_map/Lmon_king_wins/Lmon_king_losses/Lcur_stages".
			"/Lfriend_count".$friend_str."/Lbreakuser_count".$breakuser_str.
			"/Lregtime/Lmax_stages/Lnew_cur_stages/Lnew_max_stages/".
			"Lpet_exp_pool/Larena_win_count/Lpk_team_id/Lenergy_curdate/Lcurdate_energy".
			"/Lcontribute_equip/Lcontribute_attire/Lcontribute_team_exp/Lcontribute_score/Ldouble_exp/Lthird_exp/Lnono_add_exp".
			"/Lnono_add_exp_date/Lpk_medal_nums/Lenergy_absorb_times/Lshape_energy_block_nums/Lpk_person_times".
			"/Lpk_equip_times/Lmvp/Lwin_times/Llost_times/Lequal_times/Lscore/Lpk_person_times_week/Lpk_equip_times_week".
			"/Lmvp_week/Lwin_times_week/Llost_times_week/Lequal_times_week/Lscore_week/Lmax_puni_lv/Lonoff/Llefttimes".
			"/Lmess_fight_win_count/Lmess_fight_lose_count/Lset_pet_study_times";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);

	}
	
	function user_set_int_value($userid,$fieldname,$value){
		$cmdid="3150";
		$in_msg=pack("a50L", $fieldname,$value);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function user_ex_set_int_value($userid,$fieldname,$value){
		$cmdid="3151";
		$in_msg=pack("a50L", $fieldname,$value);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}



	function pp_set_vip($userid,$flag,$vip_month_count ){
		$cmdid="196E";
		$in_msg=pack("LLLLL", $flag,$vip_month_count,2 ,2, 1);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function pp_buy_attire($userid,$attireid,$addcount,$maxcount=9999,$add_duetime=0,$usexiaomee=0 ){
		$cmdid="1940";
		$in_msg=pack("LLLLL",  $attireid,$addcount,$maxcount, $add_duetime,$usexiaomee );
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function pp_set_int_value($userid,$fieldname,$value){
		$cmdid="190D";
		$in_msg=pack("a50L", $fieldname,$value);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function pp_dding_set_int($userid,$fieldname,$value){
		$cmdid="197B";
		$in_msg=pack("a50L", $fieldname,$value);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function pp_pk_team_base_set_int($userid,$fieldname,$value){
		$cmdid="5911";
		$in_msg=pack("a50L", $fieldname,$value);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function pp_dding_set_nick($userid,$nick){
		$cmdid="1972";
		$in_msg=pack("a16", $nick);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function pp_set_nick($userid,$nick){
		$cmdid="1902";
		$in_msg=pack("a16", $nick);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function pp_pet_get_list($userid){
		$page=0;
		$arr_all_count=0;
		$arr=array();
		do{
			$tmp_arr=$this->pp_pet_get_list_ex_db($userid,$page);
			//print_r($tmp_arr);
			$cur_count=$tmp_arr['count'];
			$arr+=$tmp_arr;
			$arr_all_count+=$cur_count;
			$page++;
		}while($cur_count==24 );
		$arr['count']=$arr_all_count;
		return $arr;
	}
	function pp_add_attire_mrg ($userid ){
		$cmdid='19C7';
		$abtain_time=time(NULL);
		$in_msg=pack('LLLLLLLLLLLLLCCCCCCCCCC' ,$attireid,$addcount,$maxcount,$add_duetime,$attire_rank,$abtain_time,$expire_time,$yuanshenid,$monid,$yuanshen_se_type,$master_id,$nature,$dv,$buf_0,$buf_1,$buf_2,$buf_3,$buf_4,$buf_5,$buf_6,$buf_7,$buf_8,$buf_9 );
		$out_msg='Lbuy_attireid/Lbuy_count/Lyuanshen_count';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function pp_pet_get_list_ex_db($userid,$page){
		$cmdid="1819";
		$in_msg=pack("LL",24*$page ,24);
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=$page*24;$i<$page*24+$recv_count;$i++){
			$fmt=$fmt .
				"/Lcatchtime_$i/Luseflag_$i/Latk_frist_$i/Lpetid_$i/Lmasterid_$i/Lexp_$i/".
				"Llevel_$i/Lbright_flag_$i/Ldv_$i/Lnature_$i/Lhp_$i/Lmax_hp_$i/Lmax_adj_$i/Latk_$i/Lsp_atk_$i/".
				"Ldef_$i/Lsp_def_$i/Lspeed_$i/".
				"Cev_1_$i/".
				"Cev_2_$i/".
				"Cev_3_$i/".
				"Cev_4_$i/".
				"Cev_5_$i/".
				"Cev_6_$i/".
				"Lmove_count_$i/".
				"Lmove_id_1_$i/Lmove_pp_1_$i/".
				"Lmove_id_2_$i/Lmove_pp_2_$i/".
				"Lmove_id_3_$i/Lmove_pp_3_$i/".
				"Lmove_id_4_$i/Lmove_pp_4_$i/".
				"Smagic_count_$i/".
				"Lmagic_attire_1_$i/Cmagic_type_1_$i/Cmagic_lv_1_$i/Smagic_id_1_$i/smagic_pp_11_$i/".
				"smagic_pp_12_$i/smagic_pp_13_$i/smagic_pp_14_$i/".
				"smagic_pp_15_$i/smagic_pp_16_$i/smagic_pp_17_$i/smagic_pp_18_$i/".
				"Lmagic_attire_2_$i/Cmagic_type_2_$i/Cmagic_lv_2_$i/Smagic_id_2_$i/smagic_pp_21_$i/".
				"smagic_pp_22_$i/smagic_pp_23_$i/smagic_pp_24_$i/".
				"smagic_pp_25_$i/smagic_pp_26_$i/smagic_pp_27_$i/smagic_pp_28_$i/".
				"Lmagic_attire_3_$i/Cmagic_type_3_$i/Cmagic_lv_3_$i/Smagic_id_3_$i/smagic_pp_31_$i/".
				"smagic_pp_32_$i/smagic_pp_33_$i/smagic_pp_34_$i/".
				"smagic_pp_35_$i/smagic_pp_36_$i/smagic_pp_37_$i/smagic_pp_38_$i/".
				"Lmagic_attire_4_$i/Cmagic_type_4_$i/Cmagic_lv_4_$i/Smagic_id_4_$i/smagic_pp_41_$i/".
				"smagic_pp_42_$i/smagic_pp_43_$i/smagic_pp_44_$i/".
				"smagic_pp_45_$i/smagic_pp_46_$i/smagic_pp_47_$i/smagic_pp_48_$i/".
				"Lmagic_attire_5_$i/Cmagic_type_5_$i/Cmagic_lv_5_$i/Smagic_id_5_$i/smagic_pp_51_$i/".
				"smagic_pp_52_$i/smagic_pp_53_$i/smagic_pp_54_$i/".
				"smagic_pp_55_$i/smagic_pp_56_$i/smagic_pp_57_$i/smagic_pp_58_$i/".
				"Lmagic_attire_6_$i/Cmagic_type_6_$i/Cmagic_lv_6_$i/Smagic_id_6_$i/smagic_pp_61_$i/".
				"smagic_pp_62_$i/smagic_pp_63_$i/smagic_pp_64_$i/".
				"smagic_pp_65_$i/smagic_pp_66_$i/smagic_pp_67_$i/smagic_pp_68_$i/".
				"Lmagic_attire_7_$i/Cmagic_type_7_$i/Cmagic_lv_7_$i/Smagic_id_7_$i/smagic_pp_71_$i/".
				"smagic_pp_72_$i/smagic_pp_73_$i/smagic_pp_74_$i/".
				"smagic_pp_75_$i/smagic_pp_76_$i/smagic_pp_77_$i/smagic_pp_78_$i/".
				"Lmagic_attire_8_$i/Cmagic_type_8_$i/Cmagic_lv_8_$i/Smagic_id_8_$i/smagic_pp_81_$i/".
				"smagic_pp_82_$i/smagic_pp_83_$i/smagic_pp_84_$i/".
				"smagic_pp_85_$i/smagic_pp_86_$i/smagic_pp_87_$i/smagic_pp_88_$i/".
				"Lcatch_map_$i/Lcatch_region_$i/Lcatch_level_$i/Lfreetime_$i";

		}
		return $this->unpark( $recvbuf, $fmt); 
	}


	function pp_pet_get_list_ex($userid,$start=0,$count=15){
		$cmdid="1819";
		$in_msg=pack("LL",$start,$count );
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .
				"/Lcatchtime_$i/Luseflag_$i/Latk_frist_$i/Lpetid_$i/Lmasterid_$i/Lexp_$i/".
				"Llevel_$i/Lbright_flag_$i/Ldv_$i/Lnature_$i/Lhp_$i/Lmax_hp_$i/Lmax_adj_$i/Latk_$i/Lsp_atk_$i/".
				"Ldef_$i/Lsp_def_$i/Lspeed_$i/".
				"Cev_1_$i/".
				"Cev_2_$i/".
				"Cev_3_$i/".
				"Cev_4_$i/".
				"Cev_5_$i/".
				"Cev_6_$i/".
				"Lmove_count_$i/".
				"Lmove_id_1_$i/Lmove_pp_1_$i/".
				"Lmove_id_2_$i/Lmove_pp_2_$i/".
				"Lmove_id_3_$i/Lmove_pp_3_$i/".
				"Lmove_id_4_$i/Lmove_pp_4_$i/".
				"Smagic_count_$i/".
				"Lmagic_attire_1_$i/Cmagic_type_1_$i/Cmagic_lv_1_$i/Smagic_id_1_$i/smagic_pp_11_$i/".
				"smagic_pp_12_$i/smagic_pp_13_$i/smagic_pp_14_$i/".
				"smagic_pp_15_$i/smagic_pp_16_$i/smagic_pp_17_$i/smagic_pp_18_$i/".
				"Lmagic_attire_2_$i/Cmagic_type_2_$i/Cmagic_lv_2_$i/Smagic_id_2_$i/smagic_pp_21_$i/".
				"smagic_pp_22_$i/smagic_pp_23_$i/smagic_pp_24_$i/".
				"smagic_pp_25_$i/smagic_pp_26_$i/smagic_pp_27_$i/smagic_pp_28_$i/".
				"Lmagic_attire_3_$i/Cmagic_type_3_$i/Cmagic_lv_3_$i/Smagic_id_3_$i/smagic_pp_31_$i/".
				"smagic_pp_32_$i/smagic_pp_33_$i/smagic_pp_34_$i/".
				"smagic_pp_35_$i/smagic_pp_36_$i/smagic_pp_37_$i/smagic_pp_38_$i/".
				"Lmagic_attire_4_$i/Cmagic_type_4_$i/Cmagic_lv_4_$i/Smagic_id_4_$i/smagic_pp_41_$i/".
				"smagic_pp_42_$i/smagic_pp_43_$i/smagic_pp_44_$i/".
				"smagic_pp_45_$i/smagic_pp_46_$i/smagic_pp_47_$i/smagic_pp_48_$i/".
				"Lmagic_attire_5_$i/Cmagic_type_5_$i/Cmagic_lv_5_$i/Smagic_id_5_$i/smagic_pp_51_$i/".
				"smagic_pp_52_$i/smagic_pp_53_$i/smagic_pp_54_$i/".
				"smagic_pp_55_$i/smagic_pp_56_$i/smagic_pp_57_$i/smagic_pp_58_$i/".
				"Lmagic_attire_6_$i/Cmagic_type_6_$i/Cmagic_lv_6_$i/Smagic_id_6_$i/smagic_pp_61_$i/".
				"smagic_pp_62_$i/smagic_pp_63_$i/smagic_pp_64_$i/".
				"smagic_pp_65_$i/smagic_pp_66_$i/smagic_pp_67_$i/smagic_pp_68_$i/".
				"Lmagic_attire_7_$i/Cmagic_type_7_$i/Cmagic_lv_7_$i/Smagic_id_7_$i/smagic_pp_71_$i/".
				"smagic_pp_72_$i/smagic_pp_73_$i/smagic_pp_74_$i/".
				"smagic_pp_75_$i/smagic_pp_76_$i/smagic_pp_77_$i/smagic_pp_78_$i/".
				"Lmagic_attire_8_$i/Cmagic_type_8_$i/Cmagic_lv_8_$i/Smagic_id_8_$i/smagic_pp_81_$i/".
				"smagic_pp_82_$i/smagic_pp_83_$i/smagic_pp_84_$i/".
				"smagic_pp_85_$i/smagic_pp_86_$i/smagic_pp_87_$i/smagic_pp_88_$i/".
				"Lcatch_map_$i/Lcatch_region_$i/Lcatch_level_$i/Lfreetime_$i";

		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	function pp_home_buy_attire($userid,$attireid,$count,$usexiaomee){
		$cmdid="1955";
		$in_msg=pack("LLLL", $attireid,$count,0xFFFF,$usexiaomee );
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function pp_home_sale_attire($userid,$attireid,$count,$usexiaomee){
		$cmdid="1956";
		$in_msg=pack("LLLL", $attireid,$count,$usexiaomee,0 );
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function pp_home_get_list($userid){
		$cmdid="1858";
		$in_msg="";
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .  "/Lattireid_$i/Lusedcount_$i/Lcount_$i";

		}
		return $this->unpark( $recvbuf, $fmt); 
	}



	function pp_attire_get_list_ex($userid){
		$cmdid="1848";
		$in_msg=pack("L",0);
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .  "/Lattireid_$i/Lusedcount_$i/Lcount_$i/Lattire_rank_$i/Lduetime_$i";

		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function pp_change_xiaomee($userid,$change_value,$is_add_xiaomee_day=0){
		$pri_msg=pack("lL",$change_value,$is_add_xiaomee_day );
		$sendbuf=$this->park("190B",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf));
	}



	function pp_attire_set_ex($userid,$attireid,$usedcount,$count,$attire_rank,$duetime){
		$cmdid="1949";
		$in_msg=pack("LLLLL",$attireid,$usedcount,$count,$attire_rank,$duetime );
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function pp_pet_set_ex($userid,
			$catchtime,
			$useflag,
			$petid,
			$exp,
			$level,
			$bright_flag,
			$dv,
			$nature,
			$hp,
			$max_hp,
			$max_adj,
			$atk,
			$atk_adj,
			$sp_atk,
			$def,
			$sp_def,
			$speed,
			$catch_map,
			$catch_region,
			$catch_level,
			$ev_1,
			$ev_2,
			$ev_3,
			$ev_4,
			$ev_5,
			$ev_6,
			$move_count,
			$move_id_1, $move_ip_1,
			$move_id_2, $move_ip_2,
			$move_id_3, $move_ip_3,
			$move_id_4, $move_ip_4,
		    $magic_count,
			$magic_attire_1,$magic_type_1,$magic_lv_1,$magic_id_1,
			$magic_ip_11,$magic_ip_12,$magic_ip_13,$magic_ip_14,$magic_ip_15,$magic_ip_16,$magic_ip_17,$magic_ip_18,
		    $magic_attire_2,$magic_type_2,$magic_lv_2,$magic_id_2,
			$magic_ip_21,$magic_ip_22,$magic_ip_23,$magic_ip_24,$magic_ip_25,$magic_ip_26,$magic_ip_27,$magic_ip_28,
			$magic_attire_3,$magic_type_3,$magic_lv_3,$magic_id_3,
			$magic_ip_31,$magic_ip_32,$magic_ip_33,$magic_ip_34,$magic_ip_35,$magic_ip_36,$magic_ip_37,$magic_ip_38,
			$magic_attire_4,$magic_type_4,$magic_lv_4,$magic_id_4,
			$magic_ip_41,$magic_ip_42,$magic_ip_43,$magic_ip_44,$magic_ip_45,$magic_ip_46,$magic_ip_47,$magic_ip_48,
			$magic_attire_5,$magic_type_5,$magic_lv_5,$magic_id_5,
			$magic_ip_51,$magic_ip_52,$magic_ip_53,$magic_ip_54,$magic_ip_55,$magic_ip_56,$magic_ip_57,$magic_ip_58,
			$magic_attire_6,$magic_type_6,$magic_lv_6,$magic_id_6,
			$magic_ip_61,$magic_ip_62,$magic_ip_63,$magic_ip_64,$magic_ip_65,$magic_ip_66,$magic_ip_67,$magic_ip_68,
			$magic_attire_7,$magic_type_7,$magic_lv_7,$magic_id_7,
			$magic_ip_71,$magic_ip_72,$magic_ip_73,$magic_ip_74,$magic_ip_75,$magic_ip_76,$magic_ip_77,$magic_ip_78,
			$magic_attire_8,$magic_type_8,$magic_lv_8,$magic_id_8,
			$magic_ip_81,$magic_ip_82,$magic_ip_83,$magic_ip_84,$magic_ip_85,$magic_ip_86,$magic_ip_87,$magic_ip_88,
			$freetime
		   	){
		$cmdid="191A";
		$in_msg=pack("LLLLLLLLLLLLLLLLC6L9SLCCSs8LCCSs8LCCSs8LCCSs8LCCSs8LCCSs8LCCSs8LCCSs8LLLL",
			$catchtime,
			$useflag,
			$petid,
			$exp,
			$level,
			$bright_flag,
			$dv,
			$nature,
			$hp,
			$max_hp,
			$max_adj,
			$atk,
			$sp_atk,
			$def,
			$sp_def,
			$speed,
			$ev_1,
			$ev_2,
			$ev_3,
			$ev_4,
			$ev_5,
			$ev_6,
			$move_count,
			$move_id_1, $move_ip_1,
			$move_id_2, $move_ip_2,
			$move_id_3, $move_ip_3,
			$move_id_4, $move_ip_4,
			$magic_count,
			$magic_attire_1,$magic_type_1,$magic_lv_1,$magic_id_1,
			$magic_ip_11,$magic_ip_12,$magic_ip_13,$magic_ip_14,$magic_ip_15,$magic_ip_16,$magic_ip_17,$magic_ip_18,
			$magic_attire_2,$magic_type_2,$magic_lv_2,$magic_id_2,$magic_ip_2,
			$magic_ip_21,$magic_ip_22,$magic_ip_23,$magic_ip_24,$magic_ip_25,$magic_ip_26,$magic_ip_27,$magic_ip_28,
			$magic_attire_3,$magic_type_3,$magic_lv_3,$magic_id_3,$magic_ip_3,
			$magic_ip_31,$magic_ip_32,$magic_ip_33,$magic_ip_34,$magic_ip_35,$magic_ip_36,$magic_ip_37,$magic_ip_38,
			$magic_attire_4,$magic_type_4,$magic_lv_4,$magic_id_4,$magic_ip_4,
			$magic_ip_41,$magic_ip_42,$magic_ip_43,$magic_ip_44,$magic_ip_45,$magic_ip_46,$magic_ip_47,$magic_ip_48,
			$magic_attire_5,$magic_type_5,$magic_lv_5,$magic_id_5,$magic_ip_5,
			$magic_ip_51,$magic_ip_52,$magic_ip_53,$magic_ip_54,$magic_ip_55,$magic_ip_56,$magic_ip_57,$magic_ip_58,
			$magic_attire_6,$magic_type_6,$magic_lv_6,$magic_id_6,$magic_ip_6,
			$magic_ip_61,$magic_ip_62,$magic_ip_63,$magic_ip_64,$magic_ip_65,$magic_ip_66,$magic_ip_67,$magic_ip_68,
			$magic_attire_7,$magic_type_7,$magic_lv_7,$magic_id_7,$magic_ip_7,
			$magic_ip_71,$magic_ip_72,$magic_ip_73,$magic_ip_74,$magic_ip_75,$magic_ip_76,$magic_ip_77,$magic_ip_78,
			$magic_attire_8,$magic_type_8,$magic_lv_8,$magic_id_8,$magic_ip_8,                                            			
			$magic_ip_81,$magic_ip_82,$magic_ip_83,$magic_ip_84,$magic_ip_85,$magic_ip_86,$magic_ip_87,$magic_ip_88,		
			$catch_map,
			$catch_region,
			$catch_level,
			get_time_ex($freetime)
				);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function taskday_get_list($userid){
		$cmdid="3805";
		$in_msg=pack("LL",0,0xFFFFFFFF);
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .  "/Ltype_$i/Lcount_$i";

		}
		return $this->unpark( $recvbuf, $fmt); 
	}
	function taskday_set_ex($userid,$type,$count){
		$cmdid="3906";
		$in_msg=pack("LL",$type,$count);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function dwg_sendmsg_to_friends($userid,$msg,$friendlist,$flag){
		$in_msg=pack("La2000L",$flag,$msg,count($friendlist ));
		foreach ($friendlist  as  $friendid){
			$in_msg.=pack("L", $friendid);
		}
		$sendbuf=$this->park("F120", $userid  , $in_msg);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	function pp_set_task_flag($userid,$task,$flag){
		$cmdid="1925";
		$in_msg=pack("LL",$task,$flag);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function pp_task_set($userid,$task,$flag,$optdate){
		$cmdid="1923";
		$in_msg=pack("LLa100L",$task,$flag,"",$optdate);
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}




	function pp_task_get_list($userid ){
		$page=0;
		$arr_all_count=0;
		$arr=array();
		do{
			$tmp_arr=$this->pp_task_get_list_page($userid,$page);
			$cur_count=$tmp_arr['count'];
			//print_r ($tmp_arr);
			$arr+=$tmp_arr;
			$arr_all_count+=$cur_count;
			$page++;
		}while($cur_count==60 );
		$arr['count']=$arr_all_count;
		return $arr;
	}

	function pp_task_get_list_page($userid ,$page){
		$pri_msg=pack("L", $page*60);
		$sendbuf=$this->park("1827",$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=$page*60;$i<$page*60+$recv_count;$i++){
			$fmt=$fmt . "/Ltaskid_$i/Lflag_$i/a100value_$i/Loptdate_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}




	function pp_get_friend_list($userid ){
		$sendbuf=$this->park("1836",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lfriendid_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}


	function sysarg_month_task_get_list($userid){
		$cmdid="C030";
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .  "/Ltype_$i/Lcount_$i";

		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function roominfo_presentlist_get($userid){
		$cmdid="B451";
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .  "/Luserid_$i/Lpresentid_$i/a16v1_$i/a5v2_$i/Lsend_time_$i/a151present_msg_$i";

		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function pp_add_friend($userid ,$friendid ){
		$pri_msg=pack("L", $friendid);
		$sendbuf=$this->park("1930", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function pp_add_friend_ex($userid1 ,$userid2){
		$this->pp_add_friend( $userid1 ,$userid2);
		$this->pp_add_friend( $userid2 ,$userid1);
	}

	//设置pp用户封停信息
	function pp_set_punish_flag($userid,$flag_bit,$bool_set ){
		$pri_msg=pack("LL", $flag_bit,$bool_set  );
		$sendbuf=$this->park("1928",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function ppother_cup_get_groupid($userid){
		$sendbuf=$this->park("4806",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lgroupid");
	}

	function pp_pk_get_members_list($teamid){
		$sendbuf=$this->park("5806",$teamid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Luserid_$i/Lrank_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}
	function pp_pk_team_set_member_info(
			$teamid,
			$member_id,
			$member_donation_nums_per_day,
			$member_rank,
			$member_prmt_time,
			$member_work_date,
			$super_nono_contribute,
			$contribute_degree,
			$chg_contribute
			){
		$cmdid="5932";
		$in_msg=pack("LLLLLLLL",
			$member_id,
			$member_donation_nums_per_day,
			$member_rank,
			$member_prmt_time,
			$member_work_date,
			$super_nono_contribute,
			$contribute_degree,
			$chg_contribute);
		return  $this->send_cmd($cmdid,$teamid,$in_msg,$out_msg);
	}


	function pp_pk_team_get_members_info($teamid){
		$sendbuf=$this->park("5831",$teamid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lmember_id_$i/Lmember_donation_nums_per_day_$i".
			"/Lmember_rank_$i/Lmember_prmt_time_$i/Lmember_work_date_$i/Lsuper_nono_contribute_$i/Lcontribute_degree_$i".
			"/Lchg_contribute_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}






	function pp_pk_team_get_team_info($teamid){
		$sendbuf=$this->park("5807", $teamid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),
				"Lteamid/a16name/Ljoin_method/Lvisited_flag/Lpk_team_exp/Llogo/".
				"Llogo_color/a4logo_name/a60team_slogan/a60board/Lfavorite/".
				"Lpk_person_times/".
				"Lpk_equip_times/Lmvp/Lwin_times/Llost_times/Lequal_times/Lscore/Lpk_person_times_week/Lpk_equip_times_week".
				"/Lmvp_week/Lwin_times_week/Llost_times_week/Lequal_times_week/Lscore_week");
	}



	function ppother_cup_set_groupid($userid,$groupid){
		$pri_msg=pack("L", $groupid);
		$sendbuf=$this->park("4905",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lgroupid");
	}

	function ppother_cup_get_info($userid){
		$sendbuf=$this->park("4807",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),
				"Lgroupid/Lcup/Lcup_today/Lcup_opt_date/Lis_bonus");
	}

	function ppother_cup_set_info ($userid,$groupid,$cup,$cup_today,$cup_opt_date,$is_bonus){
		$pri_msg=pack("LLLLL", $groupid,$cup,$cup_today,$cup_opt_date,$is_bonus );
		$sendbuf=$this->park("4908",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function ppother_sysarg_get_list(){
		$in_msg=pack("LL",0,0xFFFFFFFF);
		$cmdid="4812";
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ltype_$i/Lvalue_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function ppother_sys_set_value($type,$value ){
		$pri_msg=pack("LL", $type,$value );
		$sendbuf=$this->park("4913", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf));
	}


//-----------------------11DD11---------------------------------------------------------
	function dd_check_friend_existed($userid,$friendid){
		$pri_msg=pack("L",$friendid);
		$sendbuf=$this->park("1C3D",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lis_existed");
	}

    function dd_attire_get_list_ex ($userid,$start,$page_count)
    {
		$pri_msg=pack("LL", $start,$page_count );
        $sendbuf=$this->park("1C63",$userid , $pri_msg);
        $recvbuf=$this->sock->sendmsg($sendbuf);
        $fmt="Lcount";
        $recvarr=$this->unpark($recvbuf,$fmt );
        if ($recvarr && $recvarr["result"]!=SUCC){
            return $recvarr;
        }
        //SUCC
        $recv_count=$recvarr["count"];
        for ($i=0;$i<$recv_count;$i++){
            $fmt=$fmt . "/Lattireid_$i/Lattiretype_$i/Lboy_$i/Lgirl_$i/Ltime_$i"  ;
        }
        return $this->unpark($recvbuf,$fmt );
    }

	function dd_get_info_all($userid){
		$cmdid="1C3C";
		$in_msg="";

		$friend_str="";
		for ($i=0;$i<200;$i++){
			$friend_str.="/Lfriendid_$i";
		}
		$access_str="";
		for ($i=0;$i<100;$i++){
			$access_str.="/Laccess_guestid_$i/Laccess_time_$i/a16access_nick_$i";
		}
		$honor_str="";
		for ($i=0;$i<200;$i++){
			$honor_str.="/Lhonorid_$i";
		}
		
			
		$out_msg="Lflag/Lregtime/Lbirthday/a16nick/Lxiaomee/Lxiaomee_max".
			"/Lxiaomee_changetime/Lexp/a300signature".
			"/Lfriend_count".$friend_str."/Laccess_count".$access_str.
			"/Lhonor_count".$honor_str;
		
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	

	function dd_reg($userid,$sex,$birthday,$nick){
		$pri_msg=pack("LLa16", $sex,$birthday,$nick);
		$sendbuf=$this->park("1D00",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf));
	}
	function dd_get_info($userid){
		$sendbuf=$this->park("1C01",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),
				"Lflag/Lregtime/Lbirthday/a16nick/Lxiaomee/Lexp/a300signature");
	}
	
	function dd_change_xiaomee($userid,$change_value,$is_add_xiaomee_day=0){
		$pri_msg=pack("lL",$change_value,$is_add_xiaomee_day );
		$sendbuf=$this->park("1D02",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf));
	}
	function dd_change_exp_xiaomee($userid,$change_expvalue,$change_xiaomeevalue,$is_add_xiaomee_day=0){
		$pri_msg=pack("llL",$change_expvalue,$change_xiaomeevalue,$is_add_xiaomee_day );
		$sendbuf=$this->park("1D06",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lleft_xiaomee/Lchange_xiaomeevalue");
	}

	function dd_change_nick($userid,$nick){
		$pri_msg=pack("a16",$nick );
		$sendbuf=$this->park("1D03",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_set_signature($userid,$nick){
		$pri_msg=pack("a300",$nick );
		$sendbuf=$this->park("1C05",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_change_exp($userid,$change_exp){
		$pri_msg=pack("l",$change_exp );
		$sendbuf=$this->park("1D04",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_change_sex($userid,$change){
		$pri_msg=pack("L",$change );
		$sendbuf=$this->park("1D09",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_change_birthday($userid,$change){
		$pri_msg=pack("L",$change );
		$sendbuf=$this->park("1D16",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_access_add($userid,$guestid,$access_time, $nick){
		$pri_msg=pack("LLa16",$guestid,$access_time , $nick );
		$sendbuf=$this->park("1D07",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}


	function dd_add_friend($userid ,$friendid ){
		$pri_msg=pack("L", $friendid);
		$sendbuf=$this->park("1D10", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
/*
	function dd_attire_get_count($userid ,$attireid){
		$pri_msg=pack("L", $attireid);
		$sendbuf=$this->park("1C1F", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lusecount/Lcount");
	}
*/
	function dd_del_friend($userid ,$friendid){
		$pri_msg=pack("L", $friendid);
		$sendbuf=$this->park("1D11", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_get_friend_list($userid ){
		$sendbuf=$this->park("1C12",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lfriendid_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}
	function dd_add_backfriend($userid ,$friendid ){
		$pri_msg=pack("L", $friendid);
		$sendbuf=$this->park("1D13", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_get_backfriend_list($userid ){
		$sendbuf=$this->park("1C15",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lfriendid_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}
	function dd_del_backfriend($userid ,$friendid){
		$pri_msg=pack("L", $friendid);
		$sendbuf=$this->park("1D14", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	///////////////////////////////////////////////////////////////////
	//attire
	function dd_attire_add($userid ,$attireid,$attiretype){
		$pri_msg=pack("LL", $attireid,$attiretype );
		$sendbuf=$this->park("1D1A", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_attire_del($userid ,$attireid){
		$pri_msg=pack("L", $attireid);
		$sendbuf=$this->park("1D1B", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_attire_used($userid,$attireid,$sex){
		$pri_msg=pack("LL", $attireid,$sex);
		$sendbuf=$this->park("1D1C", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_attire_unused($userid,$attireid,$sex){
		$pri_msg=pack("LL", $attireid,$sex);
		$sendbuf=$this->park("1D1D", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_get_attire_list($userid, $attiretype,$start,$num )
	{
		$pri_msg=pack("LLLL",$userid, $attiretype,$start,$num );
		$sendbuf=$this->park("1C1E",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/Lboy_$i/Lgirl_$i/Ltime_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function dd_attire_get_list_all($userid)
	{
		$sendbuf=$this->park("1C1F",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/Lattiretype_$i/Ltime_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}
	function dd_attire_owned($userid ,$attireid){
		$pri_msg=pack("L",$attireid);
		$sendbuf=$this->park("1C61", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lcount");
	}

	function	dd_opt_history_add($userid ,$type,$optid,$opttime,$nick){
		$pri_msg=pack("LLLa16", $type,$optid,$opttime,$nick );
		$sendbuf=$this->park("1D20", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lid");
	}
	function	dd_opt_history_del($userid ,$type,$optid){
		$pri_msg=pack("LL", $type,$optid);
		$sendbuf=$this->park("1D21", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf));
	}
	
	function	dd_opt_history_get_list($userid ,$type){
		$pri_msg=pack("L", $type);
		$sendbuf=$this->park("1C22", $userid, $pri_msg);

		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
//			$fmt=$fmt . "/Loptid_$i/Lopttime_$i/a16nick_$i"  ;
			$fmt=$fmt . "/Lid_$i/Loptid_$i/Lopttime_$i/a16nick_$i"  ;

		}
		return $this->unpark($recvbuf,$fmt );
	}

	function	dd_present_add	($userid ,$guestid,$access_time,$nick,$presentid ){
		$pri_msg=pack("LLa16L",$guestid,$access_time,$nick,$presentid  );
		$sendbuf=$this->park("1D26", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf));
	}


	function	dd_access_get_list($userid ){
		$sendbuf=$this->park("1C08", $userid, $pri_msg);

		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lguestid_$i/Laccess_time_$i/a16nick_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}


	function dd_msg_add($userid,$type,$senderid,$sendernick,$msg ){
		$msglen=strlen($msg);
		$pri_msg=pack("LLLA16La".$msglen,$type,$senderid,time(NULL),
				$sendernick,$msglen,$msg  );
		$sendbuf=$this->park("1D2A",$userid, $pri_msg );
		$sendbuf=$this->park("1D2A",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lmsgid");
	}
	function dd_msg_del($userid,$msgid ){
		$msglen=strlen($msg);
		$pri_msg=pack("L",$msgid);
		$sendbuf=$this->park("1D2B",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function dd_msg_del_bytype($userid,$type ){
		$msglen=strlen($msg);
		$pri_msg=pack("L",$type);
		$sendbuf=$this->park("1D32",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function	dd_msg_get_msg_count($userid,$senderid ,$type_str,$read_flag){
		$pri_msg=pack("La50L",$senderid, $type_str,$read_flag );
		$sendbuf=$this->park("1C33", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lcount");
	}



	function	dd_msg_get_msg_list($userid,$senderid ,$type_str,$begin,$getcount){
		$pri_msg=pack("La50LL",$senderid, $type_str,$begin,$getcount);
		$sendbuf=$this->park("1C31", $userid, $pri_msg);

		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );	
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt ."/Lmsgid_$i/Lflag_$i/Ltype_$i/Lsenderid_$i/Lsendtime_$i/a16sendick_$i/Lmsglen_$i/a512msg_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function dd_present_del($userid,$id ){
		$pri_msg=pack("L",$id);
		$sendbuf=$this->park("1D28",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function	dd_present_get_list($userid ,$begin,$getcount){
		$pri_msg=pack("LL", $begin,$getcount);
		$sendbuf=$this->park("1C27", $userid, $pri_msg);

		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];

		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt ."/Lid_$i/Lguestid_$i/Laccess_time_$i/a16nick_$i/Lpresentid_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}

	//dd_ud
	function	dd_ud_add($userid,$type,$ip){
		$pri_msg=pack("LL", $type,$ip);
		$sendbuf=$this->park("2D00", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//for ud server
	function	dd_ud_server_get_ip($userid,$type){
		$pri_msg=pack("L", $type);
		$sendbuf=$this->park("0008", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	//end for ud server

	function	dd_ud_update($userid,$type,$ip,$confirm){
		$pri_msg=pack("LLL", $type,$ip,$confirm);
		$sendbuf=$this->park("2D01", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function dd_ud_get($userid ,$type){
		$pri_msg=pack("L", $type);
		$sendbuf=$this->park("2C02", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lprotoid_1/Lret/Lip");
	}
	function dd_ud_del($userid ,$type){
		$pri_msg=pack("L", $type);
		$sendbuf=$this->park("2D03", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	////得到图片所在的IP(对应DB命令：0x2C02   DD_UD_GET   获取  )
    function ud_get_pic_store_ip($userid,$type){
        $pri_msg=pack("L",$type);
        $sendbuf=$this->park("0003",$userid, $pri_msg );
        return $this->unpark($this->sock->sendmsg($sendbuf),"Lprotoid_1/Lret/Lip");
    }
	

	//dd_challenge
	function	dd_challenge_add($userid_c,$userid,$gameid,$nick,$sroce_c){
		$pri_msg=pack("LLA16L", $userid,$gameid,$nick,$sroce_c);
		$sendbuf=$this->park("1D35", $userid_c, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lmsgid");
	}
	function dd_challenge_del($userid ,$msgid){
		$pri_msg=pack("L", $msgid);
		$sendbuf=$this->park("1D37", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function	dd_challenge_get_list($userid ,$gameid){
		$pri_msg=pack("L", $gameid);
		$sendbuf=$this->park("1D36", $userid, $pri_msg);

		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];

		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt ."/Luserid_c_$i/Lscore_c_$i/Lmsgid_$i/a16nick_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}
	function dd_challenge_get_maxscore($userid ,$userid_c,$gameid){
		$pri_msg=pack("LL",$userid_c,$gameid);
		$sendbuf=$this->park("1C38", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lmaxscore");
	}
	function dd_challenge_get_score_c($userid ,$msgid){
		$pri_msg=pack("L",$msgid);
		$sendbuf=$this->park("1C39", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lscore_c");
	}
	function	dd_challenge_set_score($userid,$msgid,$sroce){
		$pri_msg=pack("LL", $msgid,$sroce);
		$sendbuf=$this->park("1D38", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	//dd_winlost

	function	dd_winlost_update_win($userid,$gameid){
		$pri_msg=pack("L",$gameid);
		$sendbuf=$this->park("1D51", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function	dd_winlost_update_lost($userid,$gameid){
		$pri_msg=pack("L",$gameid);
		$sendbuf=$this->park("1D52", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_winlost_get($userid ,$gameid){
		$pri_msg=pack("L", $gameid);
		$sendbuf=$this->park("1C53", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lwin/Llost");
	}


	//honor
	function dd_add_honor($userid ,$honorid ){
		$pri_msg=pack("L", $honorid);
		$sendbuf=$this->park("1D3A", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function dd_ud_set_pic_ip($userid ,$type,$ip ){
		$pri_msg=pack("LLL", 0,$type,$ip );
		$sendbuf=$this->park("2D05", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lid/Lip");
	}
	function dd_get_honor_list($userid ){
		$sendbuf=$this->park("1C3B",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lhonorid_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function dd_attire_get_used_list($userid,$sex ){
		$pri_msg=pack("L", $sex);
		$sendbuf=$this->park("1C60",$userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/Lattiretype_$i/Ltime_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}



//-----------------------11DD11--END-------------------------------------------------------

//-----------------------UD-------------------------------------------------------
	function ud_report_pic_server_info($ip,$loadavg,$disk_free,$disk_use){
		$pri_msg=pack("LLLL",$ip,$loadavg,$disk_free,$disk_use);
		$sendbuf=$this->park("0001",$userid, $pri_msg );
		#return $this->unpark($this->sock->sendmsg($sendbuf),"");
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	function ud_get_pic_server_list( ){
		$sendbuf=$this->park("0002",$userid, $pri_msg );
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];

		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt ."/Lip_$i/Lupdatetime_$i/Lload_avg_$i/Ldisk_free_$i/Ldisk_use_$i/Lpower_value_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function ud_get_power_ip_map( ){
		$sendbuf=$this->park("0004",$userid, $pri_msg );
		$fmt="Lip_0";
		for ($i=1;$i<100;$i++){
			$fmt=$fmt."/Lip_$i";
		}
		return $this->unpark($this->sock->sendmsg($sendbuf),$fmt);
	}
//===============================================================
	//得到一个合适的IP，用于保存图片(对应DB命令： 0x2D00	DD_UD_ADD	添加 )
	function ud_get_suitable_pic_store_ip($userid,$type){
		$pri_msg=pack("L",$type);
		$sendbuf=$this->park("0005",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lip");
	}


	//删除图片所在的IP(对应DB命令：0x2D03	DD_UD_DEL	删除 )
	function ud_del_pic_store_ip($userid,$type){
		$pri_msg=pack("L",$type);
		$sendbuf=$this->park("0006",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//确认图片所在的IP(对应DB命令：0x2D04	DD_UD_SET_CONFIRM	设置确认 )
	function ud_confirm_pic_store_ip($userid,$type){
		$pri_msg=pack("L",$type);
		$sendbuf=$this->park("0007",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	//得到图片所在的IP_ex(对应DB命令：0x2D05	DD_UD_SET_PIC_IP )
	function ud_get_pic_store_ip_ex($userid,$type){
		$pri_msg=pack("L",$type);
		$sendbuf=$this->park("0008",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lonline_protoid/Lip");
	}
//----------------------------------------------------------------------
	// send to on line ....
	function test_by_flash(){
		$msg=pack("NCNNN",17,1,30000,0,0 );
		$result=socket_write($this->sock->socket,$msg, strlen($msg) );
		$buf= socket_read($this->sock->socket,8192);
		return bin2hex($buf);
	}

	function test_login_server($userid,$md5_passwd){
		$msg=pack("NCNNNa32NNN",61,1,104,$userid,0,$md5_passwd,1,1,0);
		$result=socket_write($this->sock->socket,$msg, strlen($msg) );
		print "ssss1\n";
		$buf= socket_read($this->sock->socket,1000);
		print "ssss2\n";
		return bin2hex($buf);
	}

	function test_login_server_by_email($email,$md5_passwd){
		$msg=pack("NCNNNa64a32NNN",125,1,104,0,0,$email,$md5_passwd,1,1,0);
		$result=socket_write($this->sock->socket,$msg, strlen($msg) );
		$buf= socket_read($this->sock->socket,1000);
		return bin2hex($buf);
	}





	// send to on line ....
	function ts_add ($v1,$v2,$v3,$v4){
		$pri_msg=pack("LLLL",$v1,$v2,$v3,$v4 );
		return $this->sock->sendmsg_without_returnmsg($pri_msg);
	}

	function test_by_local(){
		$sendbuf=$this->park("7530",$userid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function user_magic_task_pet_get_all($userid) {
		$sendbuf = $this->park("108D", $userid, $pri_msg);
		$fmt="Lcount";
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$recvarr=$this->unpark($recvbuf, $fmt);
		$recv_count = $recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lpetid_$i/Ltaskid_$i/Lflag_$i/Lusetime_$i/Lstarttime_$i".
				"/Lendtime_$i/Lfirst_time_$i/Lclient_$i/Lstep_$i";
		}
		return $this->unpark($recvbuf, $fmt);
	}
	function pp_pk_team_fort_set_info_all(
			$teamid,
			$equip_id,
			$equip_buy_time,
			$equit_stat,
			$equit_isshow,
			$equit_hp,
			$equit_times_per_day,
			$equit_nums_per_day,
			$attire1,
			$attire2,
			$attire3,
			$attire4,
			$work_date,
			$client_flag
			 ) {
		$pri_msg =  pack("L13",
			$equip_id,
			$equip_buy_time,
			$equit_stat,
			$equit_isshow,
			$equit_hp,
			$equit_times_per_day,
			$equit_nums_per_day,
			$attire1,
			$attire2,
			$attire3,
			$attire4,
			$work_date,
			$client_flag);
			$sendbuf = $this->park("592D", $teamid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}



	function user_magic_task_pet_update($userid, $petid, $taskid, $flag, $usetime,
			 $starttime, $endtime, $first_time, $client, $step) {
		$pri_msg =  pack("LLLLLLLLL", $petid, $taskid, $flag, $usetime, $starttime, $endtime, $first_time,
				$client, $step);
		$sendbuf = $this->park("118E", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}

	function user_attire_get_list_ex($userid,$start,$count ) {
		$pri_msg =  pack("LL", $start ,$count );
		$sendbuf = $this->park("10A4", $userid, $pri_msg);
		$fmt="Lcount";
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$recvarr=$this->unpark($recvbuf, $fmt);
		$recv_count = $recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/Lusedcount_$i/Lchest_$i/Lcount_$i";
		}
		return $this->unpark($recvbuf, $fmt);
	}
	function pp_pk_team_fort_get_info_all($teamid) {
		$sendbuf = $this->park("582C", $teamid, $pri_msg);
		$fmt="Lcount";
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$recvarr=$this->unpark($recvbuf, $fmt);
		$recv_count = $recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lequip_id_$i/Lequip_buy_time_$i/Lequit_stat_$i/Lequit_isshow_$i".
				"/Lequit_hp_$i/Lequit_times_per_day_$i/Lequit_nums_per_day_$i".
				"/Lattire_1_$i/Lattire_2_$i/Lattire_3_$i/Lattire_4_$i".
				"/Lwork_date_$i/Lclient_flag_$i/a16buffer_$i";
		}
		return $this->unpark($recvbuf, $fmt);
	}


	function game_get_list($userid) {
		$pri_msg =  pack("L", $userid);
		$sendbuf = $this->park("8002", $userid, $pri_msg);
		$fmt="Lcount";
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$recvarr=$this->unpark($recvbuf, $fmt);
		$recv_count = $recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lgameid_$i/Lscore_$i/Lpkflag_$i";
		}
		return $this->unpark($recvbuf, $fmt);
	}

	function pp_submit_questionnaire_get_list(){
		$cmdid="F023";
		$sendbuf=$this->park($cmdid,$userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lmaintype_$i/Lsubtype_$i/Lvalue_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function user_get_auto_list($userid) {
		//$pri_msg =  pack("L", $userid);
		$sendbuf = $this->park("302D", $userid, $pri_msg);
		$fmt="Lcount";
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$recvarr=$this->unpark($recvbuf, $fmt);
		$recv_count = $recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lid_$i/Lauto_id_$i/Loil_$i/Lengine_$i/Lcolor_$i/Loil_time_$i/Ltotal_oil_$i/a20propery_$i/a8adorn_$i";
		}
		return $this->unpark($recvbuf, $fmt);
	}


	function user_auto_set_auto($userid, $id, $autoid, $oil, $engine, $color, $oil_time) {
		echo "this is user auto set";
		$pri_msg = pack("LLLLLL", $id, $autoid, $oil, $engine, $color, $oil_time);
		$sendbuf = $this->park("312E", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}

	function roominfo_rally_get_info($userid){
		$sendbuf=$this->park("B41F", $userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lteam/Lscore/Lrace1/Lrace2/Lrace3/Lrace4/Lday_score");
	}

	function roominfo_get_user_info($userid){
		$sendbuf=$this->park("B425", $userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lcandy_count/Lprop_count/Lflag");
	}
	
	function roominfo_set_info($userid, $candy_count, $prop_count, $flag) {
		$pri_msg = pack("LLL", $candy_count, $prop_count, $flag);
		$sendbuf = $this->park("B528", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}


	function sysarg_db_msg_edit($userid, $date, $start, $end, $content) {
		$pri_msg = pack("LLLa100", $date, $start, $end, $content);
		$sendbuf = $this->park("C115", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}
	
	function sysarg_db_msg_get($userid) {
		$sendbuf = $this->park("C018", $userid, $pri_msg);
		$fmt="Lcount";
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$recvarr=$this->unpark($recvbuf, $fmt);
		$recv_count = $recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ldate_$i/Lstart_$i/Lend_$i/a100content_$i";
		}
		return $this->unpark($recvbuf, $fmt);
	}

	
	function sysarg_db_msg_del($userid, $date, $start) {
		$pri_msg = pack("LL", $date, $start);
		$sendbuf = $this->park("C116", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}

	function user_farm_get_web($userid){
		$sendbuf = $this->park("10B8", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "Lfarm_state/Lwater_time/Lnet/Lplant/Lbreed/Lplant_limit".
				"/Lbreed_limit/Ltime");
	}

	function user_farm_set_web($userid, $farm_state, $water_time, $net, $plant, $breed, $plant_limit, $breed_limit, $time) {
		$pri_msg = pack("LLLLLLLL",  $farm_state, $water_time, $net, $plant, $breed, $plant_limit, $breed_limit, $time);
		$sendbuf = $this->park("11B7", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}
	

	function user_npc_get($userid) {
		$sendbuf = $this->park("300F", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "snpc_1/snpc_2/snpc_3");
	}

	function user_npc_set($userid, $npc_1, $npc_2, $npc_3) {
		$pri_msg = pack("sss", $npc_1, $npc_2, $npc_3);
		$sendbuf = $this->park("3107", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}

	function user_egg_get_all_web($userid) {
		$sendbuf = $this->park("303A", $userid, $pri_msg);
		$fmt="Lcount";
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$recvarr=$this->unpark($recvbuf, $fmt);
		$recv_count = $recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/Lpos_$i/Lnum_$i/Ltime_$i";
		}
		return $this->unpark($recvbuf, $fmt);
	}
	
	function user_egg_set_egg($userid, $attrieid, $pos, $num, $time) {
		$pri_msg = pack("LLLL", $attireid, $pos, $num, $time);
		$sendbuf = $this->park("313B", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}

	function roominfo_fish_weight_get($userid) {
		$sendbuf = $this->park("B45F", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "Lweight");
	}

	function user_login_get ($userid ,$login_date_from,$login_date_to){
		$cmdid='3C10';
		$in_msg=pack('LL' ,$login_date_from,$login_date_to );

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Ltype_$i/Llogin_date_$i/Llogin_time_$i/Llogout_time_$i/Ltotal_time_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}

    function pp_get_vip_and_rank($userid ){
#   $pri_msg=pack("L", $honorid);
		$sendbuf=$this->park("189A", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lvip/Lvip_rank");
	}
	function pp_client_info($userid){
		$sendbuf=$this->park("189D", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),
				"Ltask/Lmasterid/Ldiscipleid/Lpet_exp_pool/Lxiaomee/Lmax_stages/Lpet_count/a16nick/Lmax_level");
	}
	function pp_task_get_flag_list($userid,$start,$end){
		$in_msg=pack("LL",$start,$end);
		$cmdid="1824";
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ltaskid_$i/Lflag_$i";
		}
		return $this->unpark( $recvbuf, $fmt); 
	}

	function user_info_change_paypasswd_without_check($userid, $md5_newpaypasswd){
		$pri_msg=pack("A16", hex2bin_pri($md5_newpasswd) );
		$sendbuf=$this->park("0193",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_info_set_flag_change_paypasswd($userid){
		$pri_msg=pack("L", 1);
		$sendbuf=$this->park("0192",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function submit_msg_add($gameid,$userid,$type,$nick,$title, $msg ){
		$msglen=strlen($msg)+1;
		$pri_msg=pack("LLa16a60La".$msglen,$gameid,$type,$nick,$title, $msglen, $msg);
		$sendbuf=$this->park("F130", $userid , $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}

	function submit_questionnaire_add(){
		$pri_msg=pack("LLLLLL",5,2,1,1333,10,100);
		$sendbuf=$this->park("F131", 0 , $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}


	function user_login_add($type,$starttime,$endtime){
		$cmdid="3D00";
		$in_msg=pack("LLL",$type,$starttime,$endtime );
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function chat_add($gameid,$sendid,$recvid,$logtime, $msg ){
		$msglen=strlen($msg)+1;
		$pri_msg=pack("LLLLLLLLa".$msglen,$gameid,$sendid,$recvid ,0,0,0,$logtime ,$msglen, $msg);
		$sendbuf=$this->park("F133", 0 , $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}

	function dd_task_add_progress ($userid,$taskid,$progress_add_value,$progress_max_value){
		$cmdid='1D56';
		$in_msg=pack('LLL',$taskid,$progress_add_value,$progress_max_value );
		$out_msg='lprogress_real_add_value/Lprogress_cur_value';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function dd_task_add ($userid,$taskid,$progress){
		$cmdid='1D55';
		$in_msg=pack('LL',$taskid,$progress );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function dd_task_get_list ($userid ,$taskid_start,$taskid_end){
		$cmdid='1C57';
		$in_msg=pack('LL' ,$taskid_start,$taskid_end );

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Ltaskid_$i/Lflag_$i/Lprogress_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
	function pp_pet_mrg_get_all_ex($userid){
		$cmdid='18C3';
		//$in_msg=pack('L',$abtain_time );
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
	$fmt=$fmt .'/'. "Labtain_time_$i/Lyuanshenid_$i/Lmonid_$i/Lexpire_time_$i/Lyuanshen_se_type_$i/Lmaster_id_$i/Lnature_$i/Ldv_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}


	function test_add_value ($value1,$value2){
		$cmdid='F010';
		$in_msg=pack('LL' ,$value1,$value2 );
		$out_msg='Lall_value';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function user_login_get_sql_data($userid){
		$cmdid='3C11';
		$in_msg="";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function dd_get_action_date ($userid ){
		$cmdid='1C64';
		$in_msg='';
		$out_msg='Laction_date';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function dd_set_action_date ($userid ,$action_date){
		$cmdid='1D65';
		$in_msg=pack('L' ,$action_date );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
    function pp_swap_list($userid){ 
        $pri_msg=pack("LLLLLLLL", 0,0,1,1,400119,1,10000,0); 
        $sendbuf=$this->park("1909", $userid, $pri_msg); 
        return $this->unpark($this->sock->sendmsg($sendbuf),""); 
    }

    function pp_msg_add_item($userid){

        $t=time();
$len=strlen("亲爱的赛尔，虎年行大运，谜之精元送给你哦！快点放入转化仪，让我们一起来期待它的诞生吧！ ");
        $pri_msg=pack("La16LLLa$len",0,"博士派特",$t,10012,
                $len,
                "亲爱的赛尔，虎年行大运，谜之精元送给你哦！".
				"快点放入转化仪，让我们一起来期待它的诞生吧！");
        $sendbuf=$this->park("199F", $userid, $pri_msg);
        return $this->unpark($this->sock->sendmsg($sendbuf),"");
    }

	function pp_magiclist_set_item ($userid ,$catchtime,$attireid,$stat,$left_count,$effect_id,$arg_0,$arg_1,$arg_2,$arg_3,$arg_4,$arg_5,$arg_6,$arg_7){
		$cmdid='19B2';
		$in_msg=pack('LLCCSssssssss' ,$catchtime,$attireid,$stat,$left_count,$effect_id,$arg_0,$arg_1,$arg_2,$arg_3,$arg_4,$arg_5,$arg_6,$arg_7 );
		$out_msg='Lcatchtime/Seffect_id/Scount';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}



/*
	function history_ip_record($userid,$which_game,$ip ){
		$pri_msg=pack("LLL", $userid,$which_game,$ip );
		$sendbuf=$this->park("F106",0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	*/

	function pp_pet_mrg_set_ex ($userid ,$abtain_time,$yuanshenid,$monid,$expire_time,$yuanshen_se_type,$master_id,$nature,$dv){
		$cmdid='19C6';
		$in_msg=pack('LLLLLLLL' ,$abtain_time,$yuanshenid,$monid,$expire_time,$yuanshen_se_type,$master_id,$nature,$dv );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function proxy_reload_proxy_config($passwd){
		$pri_msg="";
		$sendbuf=$this->park("01FA",$passwd, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function mole2_login ($userid ){
		$cmdid='0801';
		$in_msg='';
		$out_msg='a16nick/a32signature/Lregister_time/Crace/Cprofesstion/Chonor/Lxiaomee/Llevel/Lexperience/Sphysique/Sstrength/Singenious/Squick/Sintelligence/Sattr_addition/Lhp/Lmp/Cearth/Cwater/Cfire/Cwind';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function proto_get_info(){
		$cmdid='01FB';
		$in_msg='';
		$out_msg='Llog_level/Lstart_time/Lreload_time/a200msg';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function alarm_add ($project_type,$server_type,$userid,$post_cmdid){
		$cmdid='F000';
		$in_msg=pack('LLLL' ,$project_type,$server_type,$userid,$post_cmdid );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function mole_add_friend($userid1,$userid2){
		$cmdid='F100';
		$in_msg=pack('LL' ,$userid1,$userid2);
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid1,$in_msg,$out_msg);
	}

	function pp_dding_get_funclist($userid){
    	$cmdid='1879';
    	$out_msg='a20funclist';
    	return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
    }

	function userinfo_set_question2($userid ,$set_time,$q1,$q2,$q3,$a1,$a2,$a3){
		$in_msg=pack('La64a64a64a64a64a64' ,
			$set_time,	$q1,$q2,$q3,$a1,$a2,$a3
				);
    	$cmdid='0171';
    	return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
    }

	function userinfo_verify_question2($userid,$a1,$a2,$a3){
		$in_msg=pack('a64a64a64' ,
				$a1,$a2,$a3
				);
    	$cmdid='0073';
    	return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
    }

	function userinfo_get_question2($userid){
		$in_msg="";
		$out_msg="Lset_time/a64q1/a64q2/a64q3/a64a1/a64a2/a64a3";
    	$cmdid='0072';
    	return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
    }
	function get_grow_monid($userid){
		$cmdid='18D9';
		//$in_msg=pack('L' ,$userid);
		$out_msg='Lmonid';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function userinfo_set_passwd_ex($userid,$passwd_type,$passwd_change_way,$newpasswd){
		$cmdid='015B';
		$in_msg=pack('LLa16' ,$passwd_type,$passwd_change_way,$newpasswd);
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function userinfo_set_change_passwd_flag_ex($userid,$passwd_type){
		$cmdid='015C';
		$in_msg=pack('L' ,$passwd_type);
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function userinfo_set_passwd_only_check_change_flag($userid,$passwd_type,$passwd_change_way,$newpasswd){
		$cmdid='015D';
		$in_msg=pack('LLa16' ,$passwd_type,$passwd_change_way,$newpasswd);
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function pp_get_pp_idlist ($userid ){
		$cmdid='1812';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Lcatchtime_$i/Lpetid_$i/Lbright_flag_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}

	function userinfo_login_by_md5_two($userid,$passwd_md5_two,$ip,$login_channel  )
	{
		$cmdid='005F';
		$in_msg=pack("A16LL", hex2bin_pri($passwd_md5_two),$ip,$login_channel );
		$out_msg='Lgameflag';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function db_get_server_version($cmdid){
		$sendbuf=$this->park(dechex($cmdid),$userid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),"a201sermsg");
	}

	function online_get_server_version($cmdid){
		$msg=pack("NCNNN",17,1,$cmdid,0,0 );
		$result=socket_write($this->sock->socket,$msg, strlen($msg) );
		$buf= socket_read($this->sock->socket,8192);
		$pkg_arr=@unpack("a17header/a201sermsg", $buf);
		return bin2hex($buf);
	}

	function get_sub_achievement_list ($userid ){
		$cmdid='18DF';
		$in_msg='';
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Lachievementid_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}

   	function get_sub_achievement_info ($userid ,$achievementid){
		$cmdid='18E0';
		$in_msg=pack('L' ,$achievementid );
		$out_msg='Lachievementid/Lachievement/Ltitle_count/L10achievement_title/Lvalue/Lrule_mask';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function pp_attire_get_list ($userid ,$attire_startid,$attire_endid,$useflag){
		$cmdid='1841';
		$in_msg=pack('LLL' ,$attire_startid,$attire_endid,$useflag );
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Lattireid_$i/Lcount_$i/Lleft_duetime_$i/Lattire_rank_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}

	function user_diningroom_get_head($userid, $typeid, $flag) {
		$in_msg = pack('L3', $userid, $typeid, $flag);
		$cmdid = '3083';
		$sendbuf = $this->park($cmdid, $userid, $in_msg);
		$fmt = "Luserid/Lroomid/a16room_name/Ltype_id/Lexp".
			"/lxiaomee/Levaluate/Llevel/Linner_style/Lout_style".
			"/Lcount/Loff_xiaomee/a8honor_flag/Llevel_5_count/Ldish_count/Lemployer_count";
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$recvarr_head = $this->unpark($recvbuf,$fmt);
		for ($i = 0; $i < $recvarr_head["dish_count"]; $i++) {
			$fmt .= "/Lid_$i/Ldish_id_$i/Lstate_$i/Llocation_$i/Lcount_$i/Ldish_time_$i/Lsale_xiaomee_$i";
		}
		for ($j = 0; $j < $recvarr_head["employer_count"]; $j++) {
			$fmt .= "/Lem_userid_$j/a16em_user_name_$j/Lem_petid_$j/Lpet_color_$j/Lpet_level_$j/Lpet_skill_$j".
				"/Llevel_$j/Lemploy_time_$j/Ltime_limit_$j";
		}
		return $this->unpark($recvbuf, $fmt);
	}
	
	function msgboard_qa_get_count(){
		$sendbuf=$this->park("7024",0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lcount" );
	}
	
	function user_get_user_board_info($userid){
		$cmdid="30B5";
		$in_msg="";
		$fmt= "Lflag/a16nick/a200professionlist/Lpetcolor/Lpetbirthday/Lxiaomee/Lexp/".
			"Lstrong/Liq/Lcharm/Lgame/Lwork/Lfashion".
				"/Lplant/Lbreed/Ldining_flag/Ldining_level/Litem_cnt";
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$head=$this->unpark($recvbuf,$fmt);
		for($i=0;$i < $head["item_cnt"];$i++){
			$fmt .="/Lattireid_$i/Lcount_$i";		        
		}

		return  $this->unpark($recvbuf,$fmt);
	}

	function user_get_dragons_info($userid){
		$cmdid="30AE";
		$in_msg="";
		$fmt="Lcount";
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$count=$this->unpark($recvbuf,$fmt);
		for($i=0;$i < $count["count"];$i++){
			$fmt .= "/Ldragonid_$i/a16nick_$i/Lgrowth_$i/Lstate_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function user_new_card_get_card_info($userid){
		$cmdid="30A3";
		$in_msg="";
		$fmt="Lsl_flag/Lexp/Lflag/Lday_exp/Lday_monster/Lmole_exp/Lcount";
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$recvarr_head=$this->unpark($recvbuf,$fmt);
		for($i=0;$i < $recvarr_head["count"];$i++){
			$fmt .= "/Lcard_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function user_get_lamu_diary_simple($userid){
		$cmdid="30B8";
		$in_msg="";
		$fmt="Lcount";
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$count=$this->unpark($recvbuf,$fmt);
		for($i=0;$i < $count["count"];$i++){
			$fmt .= "/a32title_$i/a604content_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}
	/*
	   添加新版卡牌
	*/
	function user_add_new_card($userid,$cardid){
		$cmdid = "31A5";
		$in_msg = pack('L',$cardid - 1240061);
        $sendbuf=$this->park($cmdid, $userid, $in_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	/*
	   获取登陆mole庄园的的url，不需要用户密码
	*/
	function user_add_session($userid){
		$cmdid = "0B01";
		$in_msg = pack('LL',1,0);
		$sendbuf=$this->park($cmdid, $userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$session = $this->unpark($recvbuf,"a16session");
		$url= pack('Na16',$userid,$session[session]);
		$url = "http://www.51mole.com/?sid=".bin2hex($url);
		$session[session] = $url;
		return $session;
	}

	function put_item_to_friendbox($userid,$itemtype,$itemid,$count){
		$cmdid="31BB";
		$in_msg = pack('LLL',$itemtype,$itemid,$count);
		$sendbuf=$this->park($cmdid, $userid, $in_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf));
	}

	function get_item_from_friendbox($userid,$itemid,$count){
		$cmdid="31BC";
		$admin = "admin";
		$in_msg = pack('LLa16LLLL',$userid,$itemid,$admin,0,0,0,$count);
		$sendbuf=$this->park($cmdid, $userid, $in_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf));
	}
	
	function get_lahm_classroom_exp_and_name($userid){
		$cmdid="2213";
		$in_msg="";
		$sendbuf=$this->park($cmdid, $userid, $in_msg);
		$fmt="Lteacher_exp/a16name";
		return $this->unpark($this->sock->sendmsg($sendbuf),$fmt);
	}

	function calc_vip_level($value) {
		$lvl = 0;
		$lvl_arr = array(1,311,776,1706,3566,5426,7286,9146,11006,14726);

		for($lvl=0;$lvl < 6;$lvl++) {
			if($value < $lvl_arr[$lvl]) break;
		}
		return $lvl;
	}

	function get_user_info($userid) {
		$cmdid = "1031";
		$sendbuf = $this->park($cmdid, $userid,"");
		$fmt = "Lflag/Lmonth/Lvalue/Lstart/Lend/Llevel/Lauto_pay";
		$ret=$this->unpark($this->sock->sendmsg($sendbuf),$fmt);
		$flag = $ret['flag'];
		$month = $ret['month'];
		$value = $ret['value'];
		$start = $ret['start'];
		$end = $ret['end'];
		$level = $ret['level'];
		$auto_pay = $ret['auto_pay'];

		$now = time();

		if($start > $now) $start = $now;
		if($end < $start) $end = $start;

		if($end < 1264706788) $end = 1264706788;
		if($start < 1264706788) $start = 1264706788;

		if(($flag & 1) || $auto_pay) {//vip
			$value = $value + 5 * intval(($now - $start) / (24 * 3600));
			$ret['value'] = $value;
			$ret['level'] = $this->calc_vip_level($value);
		} else if($flag & 2) {//use to be vip
			$value = $value - 5 * intval(($now - $end) / (24 * 3600));
			$ret['value'] = $value;
			$ret['level'] = $this->calc_vip_level($value);
		}

		return $ret;
	}

	function get_lahm_classroom_teach_record($userid){
		$cmdid="30C9";
		$in_msg="";
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$fmt="Luserid/Lexp/Ldifficulty/Llevel_s/Lgraduate_sum/Loutstand_sum/Levaluate/Lclass_sum/Lcourse_sum/Lcount";
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$recvarr_head=$this->unpark($recvbuf,$fmt);
		if ($recvarr_head && $recvarr_head["result"]!=SUCC){
		    return $recvarr_head;       
		}

		for($i=0;$i < $recvarr_head["count"];$i++){
			$fmt .= "/Lcourse_id_$i/Lcourse_cnt_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}
	
	function get_lahm_classroom_exam_info($userid,$exam_times){
		$cmdid="30E0";
		$in_msg=pack('L',$exam_times);
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$fmt="Lquality/Lscore/Lget_exp/Levaluate/Lprize_cnt";
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$recvarr_head=$this->unpark($recvbuf,$fmt);
		if ($recvarr_head && $recvarr_head["result"]!=SUCC){
		    return $recvarr_head;       
		}
		for($i=0;$i<$recvarr_head["prize_cnt"];$i++){
			$fmt.="/Lprize_id_$i/Lprize_cnt_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function get_lahm_classroom_teach_items($userid){
		$pri_msg=pack("LLLC",0,190750,190800,2);
		$sendbuf=$this->park("1018",$userid , $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lattireid_$i/Lcount_$i"  ;
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function get_lahm_classroom_student_info($userid){
		$cmdid="30C8";
		$in_msg="";
		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Luserid/Lexam_times/Lcount";
		$recvarr_head=$this->unpark($recvbuf,$fmt);
		if ($recvarr_head && $recvarr_head["result"]!=SUCC){
		    return $recvarr_head;  
		}

		for($i=0;$i<$recvarr_head["count"];$i++){
			$fmt.="/Llamuid_$i/Lnameid_$i/Lnature_$i/Lmood_$i/Llevel_$i/Lcolor_$i/Lskill_$i".
			"/Llatent_$i/Lmoral_$i/Liq_$i/Lsport_$i/Lart_$i/Llabor_$i/Lvipflag_$i/Lcourse_cnt_$i".
			"/Lcid1_$i/Ldegree1_$i/Lcid2_$i/Ldegree2_$i/Lcid3_$i/Ldegree3_$i/Lcid4_$i/Ldegree4_$i".
			"/Lcid5_$i/Ldegree5_$i/Lcid6_$i/Ldegree6_$i/Lcid7_$i/Ldegree7_$i/Lcid8_$i/Ldegree8_$i".
			"/Lcid9_$i/Ldegree9_$i/Lcid10_$i/Ldegree10_$i/Lcid11_$i/Ldegree11_$i/Lcid12_$i/Ldegree12_$i".
			"/Lcid13_$i/Ldegree13_$i/Lcid14_$i/Ldegree14_$i/Lcid15_$i/Ldegree15_$i/Lcid16_$i/Ldegree16_$i".
			"/Lcid17_$i/Ldegree17_$i/Lcid18_$i/Ldegree18_$i/Lcid19_$i/Ldegree19_$i/Lcid20_$i/Ldegree20_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function check_condtion($ret,$channel,$condtion){
		if($channel == 0) return true;
		if($channel == 1) {
			if($ret["result"] != 0) return false;
			if($ret["level"] >= $condtion) return true;
		} else if($channel == 2) {
			return false;
		} else if($channel == 3) {
			if($ret["result"] != 0) return false;
			$honor = str_split($ret["honor_flag"]);
			$ibit = $condtion % 8;
			$byte = (int)($condtion / 8);
			if($honor[$byte] & (1<<$ibit) ){
				return true;
			}
		}

		return false;
	}

	function get_cook_item_list($userid) {
		$a = array();
		$cook_items = array(
		array("ID"=>1340001,"Order"=>1,"Channel"=>1,"Condition"=>1),
		array("ID"=>1340002,"Order"=>2,"Channel"=>1,"Condition"=>1),
		array("ID"=>1340003,"Order"=>3,"Channel"=>1,"Condition"=>1),
		array("ID"=>1340004,"Order"=>5,"Channel"=>1,"Condition"=>2),
		array("ID"=>1340005,"Order"=>7,"Channel"=>1,"Condition"=>3),
		array("ID"=>1340006,"Order"=>8,"Channel"=>1,"Condition"=>4),
		array("ID"=>1340007,"Order"=>9,"Channel"=>1,"Condition"=>5),
		array("ID"=>1340008,"Order"=>10,"Channel"=>1,"Condition"=>6),
		array("ID"=>1340009,"Order"=>11,"Channel"=>1,"Condition"=>7),
		array("ID"=>1340010,"Order"=>12,"Channel"=>1,"Condition"=>8),
		array("ID"=>1340011,"Order"=>14,"Channel"=>1,"Condition"=>10),
		array("ID"=>1340012,"Order"=>15,"Channel"=>1,"Condition"=>11),
		array("ID"=>1340013,"Order"=>16,"Channel"=>1,"Condition"=>12),
		array("ID"=>1340014,"Order"=>17,"Channel"=>1,"Condition"=>13),
		array("ID"=>1340015,"Order"=>18,"Channel"=>1,"Condition"=>14),
		array("ID"=>1340016,"Order"=>19,"Channel"=>1,"Condition"=>15),
		array("ID"=>1340017,"Order"=>20,"Channel"=>1,"Condition"=>16),
		array("ID"=>1340018,"Order"=>21,"Channel"=>1,"Condition"=>17),
		array("ID"=>1340019,"Order"=>22,"Channel"=>1,"Condition"=>18),
		array("ID"=>1340020,"Order"=>23,"Channel"=>1,"Condition"=>20),
		array("ID"=>1340021,"Order"=>24,"Channel"=>1,"Condition"=>21),
		array("ID"=>1340022,"Order"=>25,"Channel"=>1,"Condition"=>22),
		array("ID"=>1340023,"Order"=>26,"Channel"=>1,"Condition"=>23),
		array("ID"=>1340024,"Order"=>27,"Channel"=>1,"Condition"=>24),
		array("ID"=>1340025,"Order"=>28,"Channel"=>1,"Condition"=>25),
		array("ID"=>1340026,"Order"=>29,"Channel"=>1,"Condition"=>26),
		array("ID"=>1340027,"Order"=>30,"Channel"=>1,"Condition"=>27),
		array("ID"=>1340028,"Order"=>31,"Channel"=>1,"Condition"=>28),
		array("ID"=>1340029,"Order"=>32,"Channel"=>1,"Condition"=>29),
		array("ID"=>1340030,"Order"=>33,"Channel"=>1,"Condition"=>30),
		array("ID"=>1340031,"Order"=>34,"Channel"=>1,"Condition"=>32),
		array("ID"=>1340032,"Order"=>37,"Channel"=>1,"Condition"=>38),
		array("ID"=>1340033,"Order"=>35,"Channel"=>1,"Condition"=>34),
		array("ID"=>1340034,"Order"=>36,"Channel"=>1,"Condition"=>36),
		array("ID"=>1340035,"Order"=>44,"Channel"=>1,"Condition"=>50),
		array("ID"=>1340036,"Order"=>41,"Channel"=>1,"Condition"=>44),
		array("ID"=>1340037,"Order"=>38,"Channel"=>1,"Condition"=>40),
		array("ID"=>1340038,"Order"=>0,"Channel"=>1,"Condition"=>0),
		array("ID"=>1340039,"Order"=>49,"Channel"=>1,"Condition"=>60),
		array("ID"=>1340040,"Order"=>39,"Channel"=>1,"Condition"=>42),
		array("ID"=>1340041,"Order"=>0,"Channel"=>1,"Condition"=>40),
		array("ID"=>1340042,"Order"=>0,"Channel"=>1,"Condition"=>1),
		array("ID"=>1340043,"Order"=>46,"Channel"=>1,"Condition"=>54),
		array("ID"=>1340044,"Order"=>47,"Channel"=>1,"Condition"=>56),
		array("ID"=>1340045,"Order"=>0,"Channel"=>1,"Condition"=>1),
		array("ID"=>1340046,"Order"=>6,"Channel"=>3,"Condition"=>4),
		array("ID"=>1340047,"Order"=>0,"Channel"=>1,"Condition"=>0),
		array("ID"=>1340048,"Order"=>42,"Channel"=>1,"Condition"=>46),
		array("ID"=>1340049,"Order"=>45,"Channel"=>1,"Condition"=>52),
		array("ID"=>1340050,"Order"=>48,"Channel"=>1,"Condition"=>58),
		array("ID"=>1340051,"Order"=>43,"Channel"=>1,"Condition"=>48),
		array("ID"=>1340052,"Order"=>0,"Channel"=>1,"Condition"=>1),
		array("ID"=>1340053,"Order"=>40,"Channel"=>1,"Condition"=>43),
		array("ID"=>1340054,"Order"=>0,"Channel"=>1,"Condition"=>6),
		array("ID"=>1340055,"Order"=>0,"Channel"=>1,"Condition"=>21),
		array("ID"=>1340056,"Order"=>0,"Channel"=>1,"Condition"=>33),
		array("ID"=>1340057,"Order"=>0,"Channel"=>1,"Condition"=>47),
		array("ID"=>1340058,"Order"=>0,"Channel"=>1,"Condition"=>57),
		array("ID"=>1340059,"Order"=>13,"Channel"=>1,"Condition"=>9),
		array("ID"=>1340060,"Order"=>0,"Channel"=>1,"Condition"=>1),
		array("ID"=>1340061,"Order"=>4,"Channel"=>1,"Condition"=>1),
		);
		$ret = $this->user_diningroom_get_head($userid,31,1);
		for($i = 0;$i < count($cook_items);$i++) {
			$item = $cook_items[$i];
			if($item["Order"] && $this->check_condtion($ret,$item["Channel"],$item["Condition"])) {
				array_push($a,$item["ID"]);
			}
		}
		return $a;
	}
} 

?>
