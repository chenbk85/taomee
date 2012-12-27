<?php

define( "SUCC" ,							0	);
define(  "PAY_TYPE_DAMEE_DAMEE" , 1000 );
define(  "PAY_TYPE_DAMEE_NCP"   , 1001 ); //宽联
define(  "PAY_TYPE_DAMEE_ZFB"   , 1002 ); //支付宝
define(  "PAY_TYPE_DAMEE_CFT"   , 1003 ); //财付通
define(  "PAY_TYPE_DAMEE_SINA"  , 1004 ); //sina 短信
define(  "PAY_TYPE_DAMEE_SERIAL", 1005 ); //序列号充值

define(  "PAY_TYPE_MONTH_DAMEE" , 2000.);
define(  "PAY_TYPE_MONTH_NCP"   , 2001 ); 
define(  "PAY_TYPE_MONTH_ZFB"   , 2002 );
define(  "PAY_TYPE_MONTH_CFT"   , 2003 );
define(  "PAY_TYPE_MONTH_SINA"  , 2004 );
//===========================================

define(  "OFFLINE_BAD"  , 	-10012 );
define(  "OFFLINE_REPORT"  , -10013 );


function hex2bin($hexdata) {
    $bindata = '';
    for($i=0; $i < strlen($hexdata); $i += 2) {
        $bindata .= chr(hexdec(substr($hexdata, $i, 2)));
    }
    return $bindata;
}

class proto_socket{
	//private:
	var $socket; //socket 句柄
	//var $debug = 1;
	function __construct( $payserip,$payserport){
		$address = gethostbyname($payserip );
		if (($this->socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP))< 0 )
		{
			trigger_error("Couldn't create socket: " . socket_strerror(socket_last_error()) . "\n");
		}	
		$result = socket_connect($this->socket,$address,$payserport );
	}
	function log_msg($msg){
		global $_SESSION;
		$log_pkg_arr=@unpack("Lproto_len/Lproto_id/Scommandid/Lresult/Luserid",$msg);
		if ( ($log_pkg_arr["commandid"] & 0x0100) ==0x0100) {
			dbg_log( $_SESSION["adminid"].":".  dechex($log_pkg_arr["commandid"] ).
					":".$log_pkg_arr["userid"].":".bin2hex($msg));
		}
	}

	function sendmsg($msg){
		//$this->log_msg($msg);
		socket_write($this->socket,$msg, strlen($msg) );
		$buf= socket_read($this->socket,4096);
		$pkg_arr=@unpack("Lproto_len",$buf);
		$proto_len= $pkg_arr["proto_len"];
		//echo $proto_len;
		while ($proto_len!=strlen($buf) ){
			$buf .=	socket_read($this->socket,4096);
		}	
		return $buf;
	}
	function sendmsg_without_returnmsg($msg){
		//$this->log_msg($msg);
		$result=socket_write($this->socket,$msg, strlen($msg) );
		if ($result){
			return array("result"=>0);
		}else
			return array("result"=>1003);
	}

	function close(){
		socket_close($this->socket);
	}
	function __destruct(){
	}
} 


class Cproto{
	var $sock; 
	function __construct( $payserip,$payserport){
		$this->sock = new proto_socket ($payserip,$payserport);
	}
	function __destruct(){
		if ($this->sock) $this->sock->close(); 
	}

	function park($cmdid,$userid,$private_msg){
		//18：报文头部长度
		$pkg_len=18+strlen($private_msg) ;
		$result=0;
		$proto_id=0x00000000;
		return pack("L2SL2",$pkg_len,$proto_id,hexdec($cmdid),$result,$userid)
			.$private_msg;
	}

	function unpark($sockpkg, $private_fmt){
		$pkg_arr=@unpack("Lproto_len/Lproto_id/Scommandid/Lresult/Luserid",$sockpkg);
		//$tmp_len=$pkg_arr["proto_len"];
		if ($private_fmt!="" && $pkg_arr["result"]==0){//成功
			$pkg_arr=@unpack("Lproto_len/Lproto_id/Scommandid/Lresult/Luserid/".$private_fmt, 
					$sockpkg);
		}	 
		if ($pkg_arr){
			return $pkg_arr;
		}else{
			return array("result"=>1003 );
		}	
	}

	function send_cmd($cmdid ,$userid, $pri_msg , $out_msg ){
		$sendbuf=$this->park( $cmdid , $userid,  $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),$out_msg );
	}

	function msgboard_set_flag ($msgid,$boardid, $flag){
		$pri_msg=pack("LL",$boardid , $flag );
		$sendbuf=$this->park("7104", $msgid  , $pri_msg );
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
	function roommsg_show_message($userid,$msgid,$isshow ){
		$pri_msg=pack("LC", $msgid,$isshow );
		$sendbuf=$this->park("C512" ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
	}





	function roommsg_report_msg($userid,$msgid, $msg ){
		$pri_msg=pack("La100", $msgid,$msg );
		$sendbuf=$this->park("C516" ,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf), "");
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

	function user_login_by_id($userid, $md5_passwd ){
		$pri_msg=pack("A16LL", hex2bin($md5_passwd),0,0);
		$sendbuf=$this->park("0001",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lvip/Lfcount");
	}

	function user_get_value($userid){
		$cmdid="1034";
		$in_msg="";
		$out_msg= "Lvip/Lflag/Lpetcolor/Lpetbirthday/Lxiaomee/Lxiaomee_max/Lexp/".
				"Lstrong/Liq/Lcharm/LOl_count/LOl_today/LOl_last/LOl_time/Clast_ip_1/Clast_ip_2/Clast_ip_3/Clast_ip_4/a50tasklist";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function su_login_by_id($userid, $md5_passwd ){
		$pri_msg=pack("A16", hex2bin($md5_passwd));
		$sendbuf=$this->park("A001",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"a16nick/Lflag");
	}

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
		$pri_msg=pack("A16A16", hex2bin( $md5_oldpasswd ), 
				hex2bin($md5_newpasswd));
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
		$sendbuf=$this->park("0111", $userid, $pri_msg);
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

	function user_pet_set_all($userid ,$petid, $flag,$flag2, $birthday, $nick, $color, 
			$sicktime, $pos_x, $pos_y, $hungry, $thirsty, $sanitary, $spirit, $starttime,$taskid,
			$endtime , $skill, $stamp
			){
		$pri_msg=pack("LLLLa16LLCCCCCCLLLLL", $petid,$flag, $flag2, $birthday, $nick, $color, 
			$sicktime, $pos_x, $pos_y, $hungry, $thirsty, $sanitary, $spirit,
			$endtime,$starttime,$taskid ,$skill, $stamp);
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
		$sendbuf=$this->park("0112", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function user_set_friend($userid1 ,$userid2 ){
		$this->user_add_friend($userid1,$userid2);
		return $this->user_add_friend($userid2,$userid1);
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

	function user_add_offline_msg($userid, $msg){
		$msglen=strlen($msg)+1;
		$pri_msg=pack("LNCNNNNNNA16NNNNa".$msglen , 

				61+4 +$msglen,57+4+$msglen,1,10003,0,0, 
				0,0,0, "淘米委员会",0,time(null),0,
			   	$msglen,$msg  );
		$sendbuf=$this->park("112A", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_change_value($userid,$addexp,$addstrong,$addiq,$addcharm,$addxiaomee)
	{
		$pri_msg=pack("lllllLL",$addexp,$addstrong,$addiq,$addcharm,$addxiaomee,5,0);
		$sendbuf=$this->park("1139", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_change_passwd($userid, $md5_oldpasswd ,$md5_newpasswd){
		$pri_msg=pack("A16A16", hex2bin($md5_oldpasswd),
				hex2bin($md5_newpasswd) );
		$sendbuf=$this->park("0109",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_change_passwd_without_check($userid, $md5_newpasswd){
		$pri_msg=pack("A16", hex2bin($md5_newpasswd) );
		$sendbuf=$this->park("0140",$userid, $pri_msg);
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
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
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

	function usermsg_get_writing($userid, $startdate, $enddate,$flag, $type, $key, $index ){
		$pri_msg=pack("LLLLLa30L",$userid , $startdate, $enddate, $flag, $type,$key,$index );
		$sendbuf=$this->park("9003",0,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$recvarr=$this->unpark($recvbuf,"a21date/Ltype/Lflag/Lmsgid/Luserid/Lmsglen/Lreportlen");
		if ($recvarr && $recvarr["result"]!=SUCC){
				return $recvarr;
		}
		//$fmt="a21date/Ltype/Lflag/Lmsgid/Luserid/Lmsglen/Lreportlen/a104title/a16nick".
		//		"/a". ($recvarr["msglen"] - 120)  ."msg/Lreport_time/a".($recvarr["reportlen"]-4)."report";
		$fmt="a21date/Ltype/Lflag/Lmsgid/Luserid/Lmsglen/Lreportlen/a104title/a16nick".
				"/a". ($recvarr["msglen"] - 120)  ."msg";
		//		"/Lreport_time/a".($recvarr["reportlen"]-4)."report";
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
			$fmt=$fmt . "/Lpetid_$i/Lflag_$i/Lflag2_$i/Lbirthday_$i/a16nick_$i".
				"/Lcolor_$i/Lsicktime_$i/Cpos_x_$i" . 
				"/Cpos_y_$i/Chungry_$i/Cthirsty_$i/Csanitary_$i/Cspirit_$i/".
				"Lendtime_$i/Lstarttime_$i/Ltaskid_$i/Lskill_$i/Lstamp_$i"  ;
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

	function user_add_attire ($userid, $attireid,$count){
		$pri_msg=pack("LLLL",0,$attireid,$count,0xFFFFFFFF);
		$sendbuf=$this->park("111C",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_add_home_attire ($userid, $attireid,$count){
		$pri_msg=pack("LLLL",1,$attireid,$count,1);
		$sendbuf=$this->park("111C",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_del_home_attire ($userid, $attireid,$count){
		$pri_msg=pack("LLLLL",1,$attireid,$count,0xFFFFFFFF,0);
		//通过卖出协议
		$sendbuf=$this->park("1117",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_attire_ex($userid, $attireid,$usedcount,$count){
		$pri_msg=pack("LLL",$attireid,$usedcount,$count);
		$sendbuf=$this->park("111A",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_sale_attire ($userid,$attiretype, $attireid,$count,$xiaomee){
		$pri_msg=pack("LLLLL",$attiretype, $attireid,$count,0xFFFFFFFF,$xiaomee);
		$sendbuf=$this->park("1117",$userid , $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}



	function user_set_attire_used($userid,$attireid,$count){
		$pri_msg=pack("LLLL",$attireid,3,$count,0xFFFFFFFF);
		$sendbuf=$this->park("1136",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function user_set_user_enabled($userid){
		$pri_msg="";
		$sendbuf=$this->park("110F",$userid, $pri_msg);
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
		$sendbuf=$this->park("013A",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_set_home_attire_noused($userid,$attireid ){
		$pri_msg=pack("L", $attireid );
		$sendbuf=$this->park("1145",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function su_admin_set_flag($adminid,$flag){
		$pri_msg=pack("LL",$adminid,$flag);
		$sendbuf=$this->park("A10A",0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function other_send_email($userid){
		$pri_msg=pack("L",1);
		$sendbuf=$this->park("0114",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}


	function user_get_home($userid){
		$pri_msg="";
		$sendbuf=$this->park("101D",$userid , $pri_msg);
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
	function usermsg_add_report_deal($userid, $adminid, $deal_type ,$reason ){
		$pri_msg=pack("LLL",$adminid, $deal_type ,$reason );
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
			$fmt=$fmt . "/a21date_$i/Lobjuserid_$i/Ladminid_$i/Ldeal_type_$i/Lreason_$i"  ;
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

	function user_set_vip_flag( $userid, $flag ){
		$pri_msg=pack("L", $flag);
		$sendbuf=$this->park("0114",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function usermsg_del_writing($userid,$date ){
		$pri_msg=pack("a21",  $date );
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
		$pri_msg=pack("a21LLLLa".($msglen-4), $date,$type, $flag,$msglen,time(NULL), $msg);
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
				"Lparentid/Lchildcount");
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

	function user_login($user_info,$md5_passwd){
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
		return $this->user_login_by_id($userid ,$md5_passwd );
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

	/*
	function get_nick_email($userid){
		$pri_msg="";
		$sendbuf=$this->park(1015,$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lflag/Lvip/A16nick/A64email");
	}
	*/

	function user_get_nick($userid){
		$pri_msg="";
		$sendbuf=$this->park("003B",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"a16nick");
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

	// send to on line ....
	function switch_send_msg($serverid,$userid,$pic_id, $msg){
		$msglen=strlen($msg)+1;
		$off_line_pri_msg=pack("LNCNNNNNNA16NNNNa".$msglen , 
				61+4 +$msglen,57+4+$msglen,1,10003,0,0, 
				0,0,0, "淘米委员会",0,time(null),$pic_id ,
			   	$msglen,$msg  );

		$off_line_pri_msg_ex=pack("NCNNNNNNA16NNNNa".$msglen , 
				57+4+$msglen,1,10003,0,0, 
				0,0,0, "淘米委员会",0,time(null),$pic_id ,
			   	$msglen,$msg  );
		//echo "===$pic_id===";
		$msglen=strlen($off_line_pri_msg_ex);	
		$on_line_pri_msg=pack("LLLsa".$msglen ,$serverid,$userid ,0 , $msglen,$off_line_pri_msg_ex);
		$sendbuf=$this->park("F619", $userid ,$on_line_pri_msg . $off_line_pri_msg);
		//echo bin2hex($sendbuf);
		//echo "======";
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
		$cmdid="0080";
		$in_msg="";
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	function userinfo_get_grouplist($userid){
		$sendbuf=$this->park("0090" ,$userid, $pri_msg);
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
	


	/*get_nick_email*/
	function userinfo_get_info($userid){
		$sendbuf=$this->park("0015",$userid,"");
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lflag1/Lflag2/a16nick/Lcount/Lg1/Lg2/Lg3");
	}

	function userinfo_get_info_all($userid){
		$cmdid="0082";
		$in_msg="";
		$out_msg= "Lflag1/Lflag2/Lregflag/a16passwd/a16paypasswd/a16nick/a64email/".
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

	function user_card_getinfo($userid){
		//$pri_msg=pack("LL",$groupid,$flag );
		$sendbuf=$this->park("1075",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lwin/Llose/Lcount");
	}






//支付平台
	//验正支付密码	
	function userinfo_check_paypasswd($userid,$md5_pay_passwd){
		$pri_msg=pack("A16",hex2bin($md5_pay_passwd) );
		$sendbuf=$this->park("008B",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//设置支付密码	
	function userinfo_set_paypasswd($userid,$md5_pay_passwd){
		$pri_msg=pack("A16",hex2bin($md5_pay_passwd) );
		$sendbuf=$this->park("018C",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//是否已经设置过支付密码
	function userinfo_isset_paypasswd($userid){
		//$pri_msg=pack("A16",hex2bin($md5_pay_passwd) );
		$sendbuf=$this->park("008D",$userid, $pri_msg  );
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lis_set_flag");
	}

	//清空支付passwd
	function userinfo_clean_paypasswd($userid){
		//$pri_msg=pack("A16",hex2bin($md5_pay_passwd) );
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
		$pri_msg=pack("L", 1);
		$sendbuf=$this->park("F101",$userid,$pri_msg );
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	function log_add_change ($type,$userid,$logtime,$changevalue,$v1 ,$v2  ){
		$pri_msg=pack("LLLlll", $type,$userid,$logtime,$changevalue,$v1 ,$v2 );
		$sendbuf=$this->park("F103",$userid,$pri_msg );
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
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
		$in_msg=pack("La16a16",$add_adminid,$nick, hex2bin($passwd));
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
	function admin_login($adminid,$passwd){
		$in_msg=pack("A16", hex2bin( $passwd )); 

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
	
	function admin_change_passwd($adminid, $md5_oldpasswd ,$md5_newpasswd ){
		$pri_msg=pack("A16A16", hex2bin( $md5_oldpasswd ), 
				hex2bin($md5_newpasswd));
		$sendbuf=$this->park("5503",$adminid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function admin_root_change_passwd($adminid,$edit_adminid,$md5_newpasswd ){
		$pri_msg=pack("LA16",$edit_adminid, hex2bin($md5_newpasswd));
		$sendbuf=$this->park("5509",$adminid, $pri_msg);
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








/*	
	function email_del_map_userid_email($userid,$email){
		$pri_msg=pack("A64",  $email);
		$sendbuf=$this->park("4106",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_del_user($userid){
		$sendbuf=$this->park("1104",$userid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function pay_init_user($userid){
		$sendbuf=$this->park("3102",$userid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function room_add_vocter($userid,$voctoer){
		$sendbuf=$this->park("B504",$userid, "" );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}




	function user_register($userid, 
			$md5_passwd,$sex,$birthday,$petcolor,
		$nick,$email,$mobile,$addr_province,$addr_city,$addr,$signature){
		$pri_msg=pack("A16LLLA16A64A16vvA64A128",  hex2bin($md5_passwd),
			$sex,$birthday,$petcolor,
		$nick,$email,$mobile,$addr_province,$addr_city,$addr,$signature);
		$sendbuf=$this->park("1102",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
*/
} 

?>
