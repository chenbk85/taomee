<?php
require_once("proto_base.php");

class Cpub_proto extends Cproto_base {
	function __construct( $proxyip,$proxyport){
		parent::__construct($proxyip,$proxyport) ;
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

	function userinfo_get_passwd_history ($userid){
		$cmdid="001C";
		$in_msg="";
		$out_msg= "a16passwd_1/Llogtime_1/".
			 "a16passwd_2/Llogtime_2/". 
			 "a16passwd_3/Llogtime_3/";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
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

	//设置用户账号是否可用标志(冻结，解冻) 
	function userinfo_set_user_enable_flag($userid,$enable_flag){
		$pri_msg=pack("L", $enable_flag );
		$sendbuf=$this->park("0194",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//得到用户账号是否可用标志(冻结，解冻) 
	function userinfo_get_user_enable_flag($userid) {
        $sendbuf = $this->park("0095", $userid,"" );
        return $this->unpark($this->sock->sendmsg($sendbuf), "Lenable_flag");
    }






	function user_set_flag_change_passwd($userid){
		$pri_msg=pack("L", 1);
		$sendbuf=$this->park("015A",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
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

	function userinfo_check_userid($userid){
		$cmdid="0005";
		$in_msg="";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
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

	function userinfo_check_question($userid,$q,$a ){
		$cmdid="0084";
		$in_msg=pack("a64a64",  $q,$a  );
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
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



	function log_add_change ($type,$userid,$logtime,$changevalue,$v1 ,$v2  ){
		$pri_msg=pack("LLLlll", $type,$userid,$logtime,$changevalue,$v1 ,$v2 );
		$sendbuf=$this->park("F103",$userid,$pri_msg );
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}



	function history_ip_record ($userid,$game,$ip ){
		$in_msg=pack("LLL", $userid,$game,$ip );
		$cmdid="F106";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function ip_history_get_lastip ($userid ){
		$cmdid='F008';
		$in_msg='';
		$out_msg='Lip';
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









	function dwg_sendmsg_to_friends($userid,$msg,$friendlist,$flag){
		$in_msg=pack("La2000L",$flag,$msg,count($friendlist ));
		foreach ($friendlist  as  $friendid){
			$in_msg.=pack("L", $friendid);
		}
		$sendbuf=$this->park("F120", $userid  , $in_msg);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
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
	function login_server_login($userid,$md5_passwd_two,$ip){
		$msg=pack("NCNNNa32NNN",61,1,103,$userid,0,$md5_passwd_two,1,1,$ip );
		$result=socket_write($this->sock->socket,$msg, strlen($msg) );
		$buf= socket_read($this->sock->socket,1000);
		$ret_arr=array("result"=>1003);
		if (strlen($buf)==17){//正常报文
			$ret_arr=unpack("Npkg_len/Cversion/Ncmdid/Nuserid/Nresult",$buf );
		}else if (strlen($buf)==37){//正常报文
			$ret_arr=unpack("Npkg_len/Cversion/Ncmdid/Nuserid/Nresult/a16session/Ngameflag",$buf );
		}
		echo bin2hex($buf);
		echo strlen($buf);
		return $ret_arr;
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
		$pri_msg=pack("LLLLLa".$msglen,$gameid,$sendid,$recvid ,$logtime ,$msglen, $msg);
		$sendbuf=$this->park("F133", 0 , $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}

	function proxy_reload_proxy_config($passwd){
		$pri_msg="";
		$sendbuf=$this->park("01FA",$passwd, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function alarm_add ($project_type,$server_type,$userid,$post_cmdid){
		$cmdid='F000';
		$in_msg=pack('LLLL' ,$project_type,$server_type,$userid,$post_cmdid );
		$out_msg='';
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
	function proto_get_info(){
		$cmdid='01FB';
		$in_msg='';
		$out_msg='Llog_level/Lstart_time/Lreload_time/Lpage_size/a200msg';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}


	function submit_questionnaire_get_list ($gameid,$start,$count){
		$cmdid='F034';
		$in_msg=pack('LLL' ,$gameid,$start,$count );

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
			$fmt=$fmt .'/'. "Lmaintype_$i/Lsubtype_$i/Lcount_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}

	//添加报警通用
	function alarm_add_comm ($type_str,$fail_userid,
			$fail_cmdid,$is_hex_cmd,$fail_ip,
			$recv_count_per_min_to_send,$send_interval,$telephone_list){
		$cmdid='F101';
		$in_msg=pack('a120LLLa16LLa256' ,$type_str,$fail_userid,$fail_cmdid,$is_hex_cmd,$fail_ip,$recv_count_per_min_to_send,$send_interval,$telephone_list );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

    function userinfo_set_mee_fans($userid,$mee_fans){
        $cmdid="0161";
        $in_msg=pack("L",$mee_fans );
        $out_msg="";
        return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
    }

    function userinfo_get_mee_fans($userid){
        $cmdid="0060";
        $in_msg="";
        $out_msg="Lmee_flags";
        return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
    }
    function user_login_get_last_gameid ($userid){
        $cmdid="3C12";
        $in_msg="";
        $out_msg="Lgameid";
        return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
    }
    function session_add_session($userid,$gameid,$ip){
        $cmdid='0B01';
        $in_msg = pack("LL",$gameid,$ip);
        $out_msg='a16session';
        return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
    }

    function session_check_session($userid,$gameid,$session,$del_flag){
        $cmdid='0B02';
        $in_msg = pack("La16L",$gameid,$session,$del_flag);
        $out_msg='';
        return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
    }

    function userinfo_register_ex($userid ,$md5_passwd,$email  ){
        $cmdid='0103';
        $in_msg = pack("a16a64a228",$md5_passwd,$email,"" );
        $out_msg='';
        return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
    }

	function map_email_userid($userid,$email){
		$pri_msg=pack("A64",$email);
		$sendbuf=$this->park("4105", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lis_succ/Lold_map_userid");
	}
	function register($userid ,$passwd,$email ){
		$ret_arr=$this->map_email_userid($userid, $email );
		if ($ret_arr["result"]==0){
			return $this->userinfo_register_ex($userid,hex2bin_pri( md5($passwd)),$email  );
		}
	}


	//设置该游戏已激活
	function userinfo_add_act_flag($userid,$gameflag ){
		$cmdid="0196";
		$in_msg=pack("L",$gameflag );
		$out_msg= "";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function userinfo_get_game_act_flag($userid){
		$cmdid="0097";
		$in_msg="";
		$out_msg="Lgameflag";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function im_user_get_info($userid){
		$cmdid="F072";
		$in_msg="";
		$out_msg="Lgameflag";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}



	function ff_login_get_list ($userid ,$login_date){
		$cmdid='3C07';
		$in_msg=pack('L' ,$login_date );

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
			$fmt=$fmt .'/'. "Luserid_$i/Lgameid_$i/Lstarttime_$i/Lendtime_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
	function pp_submit_questionnaire_add ($userid ){
		$cmdid='F122';
		$in_msg=pack('LLL' ,1,31,1);
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

} 

?>
