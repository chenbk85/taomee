<?php
#changelog
#09-05-14 :修正appeal_get_appeal, appeal_get_list没有返回telphone的错误;
#09-05-12 :调整appeal_add_appeal,appeal_get_appeal, appeal_get_list;：分别加入两个字段
#09-05-04 :设置协议超时时间 socket_set_option($socket,SOL_SOCKET,SO_REUSEADDR,1);
#09-04-27 :change user_get_userid_by_email


define( "SUCC" ,							0	);

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
		socket_set_option($this->socket,SOL_SOCKET,SO_RCVTIMEO,array("sec"=>60, "usec"=>0 ) );
		socket_set_option($this->socket,SOL_SOCKET,SO_SNDTIMEO,array("sec"=>60, "usec"=>0 ) );
		#socket_connect socketfd, SOL_SOCKET, SO_RCVTIMEO, 
		
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
		//print bin2hex($msg);
		socket_write($this->socket,$msg, strlen($msg) );
		$buf = "";
		$buf= socket_read($this->socket,8192);
		//print  bin2hex($buf);
		$pkg_arr=@unpack("Nproto_len",$buf);
		$proto_len= $pkg_arr["proto_len"];
		print $proto_len;
		while ($proto_len>strlen($buf) ){
			//print "sadfadsf\n";
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

	function wait_msg(){
		$buf = "";
		$buf = socket_read($this->socket,8192);
		$pkg_arr=@unpack("Nproto_len",$buf);
		$proto_len= $pkg_arr["proto_len"];
		//print $proto_len;
		while ($proto_len>strlen($buf) ){
			//print "sadfadsf\n";
			$buf .=	socket_read($this->socket,4096);
		}	
		return $buf;
	}

	function close(){
		socket_close($this->socket);
	}
	function __destruct(){
	}
} 

$g_seqno = 1;
class Cproto{
	var $sock; 
	var $adminid;
	function __construct( $payserip,$payserport){
		$this->sock = new proto_socket ($payserip,$payserport);
	}
	function __destruct(){
		if ($this->sock) $this->sock->close(); 
	}

	function set_adminid($adminid){
		$this->adminid=$adminid;
	}


	//park_function
	function park($cmdid,$userid,$private_msg){
		global $_SESSION;
		global $g_seqno;
		//18：报文头部长度
		$pkg_len=18+strlen($private_msg);
		//print_r($pkg_len);
		$result=0;
		$proto_id=$this->adminid;
		if ($proto_id==0){
			$proto_id=$_SESSION["adminid" ];
		}
		$ver = 1;
		$seqnum = 1;
		return pack("NnN3",$pkg_len,$cmdid,$userid,$seqnum,$result)
			.$private_msg;
	}

	function unpark($sockpkg, $private_fmt){
		//echo bin2hex($sockpkg);
//		print_r($sockpkg);
		$pkg_arr=@unpack("Nproto_len/ncommandid/Nuserid/Nseq/Nret",$sockpkg);
		//$tmp_len=$pkg_arr["proto_len"];

//    	print_r($pkg_arr);
//		print_r($sockpkg);
		if ($private_fmt!="" && $pkg_arr["result"]==0){//成功
			$pkg_arr=@unpack("Nproto_len/Ncommandid/Nuserid/Nseq/Nret".$private_fmt, 
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

//--------------------------------------------------------------------------------
	function send_mcast_reload($serverid,$maincmd,$cmd,$targetid){
		$sendbuf=pack("N4",$serverid,$maincmd,$cmd,$targetid);
		$this->sock->sendmsg($sendbuf);
	}
	//gf_test_start
	function wait($userid){
		return $this->unpark($this->sock->wait_msg(),"");
	}

	function login($userid,$homeid){
		$pri_msg=pack("N",$homeid);
		$sendbuf=$this->park("10002",$userid,$pri_msg);
		//return $this->unpark($this->sock->sendmsg_without_returnmsg($sendbuf),"");
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function logout($userid,$reason){

		$pri_msg=pack("N",$reason);
		$sendbuf=$this->park("10003",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function enter_map($userid,$mapid,$x,$y){
		$pri_msg=pack("NNN",$mapid,$x,$y);
		$sendbuf=$this->park("10004",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
	function main_login($userid,$pass,$channal,$whichgame,$ip){
		$pri_msg=pack("a32NNN",$pass,$channal,$whichgame,$ip);
		$sendbuf=$this->park("104",$userid,0,$pri_msg);
		//print_r(strlen($sendbuf));
		$fmt="/a16session/Nrolecreate";
		//return $this->unpark($this->sock->sendmsg($sendbuf),$fmt);
		$sockpkg = $this->sock->sendmsg($sendbuf);
		$pkg_arr=@unpack("Nproto_len/ncommandid/Nuserid/Nresult/Nseq".$fmt,$sockpkg);
		print_r($pkg_arr);
		return $pkg_arr["session"];
	}

	function create_role($userid,$session,$role_type,$nick){
		$pri_msg=pack("a16ca16",$session,$role_type,$nick);
		$sendbuf=$this->park("108",$userid,0,$pri_msg);
		//print_r(strlen($sendbuf));
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function delete_role($userid,$session,$role_tm){
		$pri_msg=pack("a16N",$session,$role_tm);
		$sendbuf=$this->park("110",$userid,0,$pri_msg);
		//print_r(strlen($sendbuf));
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function get_role_list($userid){
		$pri_msg="";
		$sendbuf=$this->park("107",$userid,0,$pri_msg);
		//print_r(strlen($sendbuf));
		$sockpkg = $this->sock->sendmsg($sendbuf);
		$fmt="/Ncount";
		$pkg_arr=@unpack("Nproto_len/ncommandid/Nuserid/Nresult/Nseq".$fmt,$sockpkg);
		print_r($pkg_arr);
		return;
		//return $this->unpark($this->sock->sendmsg($sendbuf),$fmt);
	}

	function get_svr_list($userid,$role_tm,$session,$channel){
		$pri_msg=pack("a16NN",$session,$channel,$role_tm);
		$sendbuf=$this->park("105",$userid,0,$pri_msg);
		$sockpkg=$this->sock->sendmsg($sendbuf);
		$fmt="/Nmaxid/Nvip/Nonlinecnt";
		$pkg_arr=@unpack("Nproto_len/ncommandid/Nuserid/Nresult/Nseq".$fmt,$sockpkg);
		print_r($pkg_arr);
		return;
		//return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function get_range_svr_list($userid,$startid,$endid){
		$pri_msg=pack("N3",$startid,$endid,0);
		$sendbuf=$this->park("106",$userid,0,$pri_msg);
		//$sockpkg=$this->sock->sendmsg($sendbuf);
		//$fmt="/Nmaxid/Nvip/Nonlinecnt";
		//$pkg_arr=@unpack("Nproto_len/Cver/Ncommandid/Nuserid/Nresult".$fmt,$sockpkg);
		//print_r($pkg_arr);
		//return;
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function undertake_task($userid,$taskid){
		$pri_msg=pack("N",$taskid);
		$sendbuf=$this->park("2201",$userid,1,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function cancel_task($userid,$taskid){
		$pri_msg=pack("N",$taskid);
		$sendbuf=$this->park("2206",$userid,1,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function get_task_buf($userid,$taskid){
		$pri_msg=pack("N",$taskid);
		$sendbuf=$this->park("2203",$userid,1,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function set_task_buf($userid,$taskid,$buf){
		$pri_msg=pack("Na40",$taskid,$buf);
		$sendbuf=$this->park("2204",$userid,1,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function finish_task($userid,$taskid,$outid){
		$pri_msg=pack("NN",$taskid,$outid);
		$sendbuf=$this->park("2202",$userid,1,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function buy_clothes($userid,$itemid){
		$pri_msg=pack("N",$itemid);
		$sendbuf=$this->park("2602",$userid,1,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function leave_map($userid){
		//print_r($pos_x);
		$pri_msg="";
		$sendbuf=$this->park("2002",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function walk($userid,$pos_x,$pos_y){
		//print_r($pos_x);
		$pri_msg=pack("NN",$pos_x,$pos_y);
		$sendbuf=$this->park("2101",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function battle_npc($userid){
		$pri_msg="";
		$sendbuf=$this->park("2406",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	
	function battle_invite($userid,$invitee_id){
		$pri_msg=pack("N",$invitee_id);
		$sendbuf=$this->park("2401",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function battle_invite_rsp($userid,$invitor_id,$agree){
		$pri_msg=pack("NC",$invitor_id,$agree);
		$sendbuf=$this->park("2404",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function battle_invite_cancel($userid){
		$pri_msg="";
		$sendbuf=$this->park("2402",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function battle_ready($userid){
		$sendbuf=$this->park("2410",$userid,"");
		//return $this->unpark($this->sock->sendmsg($sendbuf),"");
		
		$fmt="/ncount";
		for ($i=0;$i<7;$i++){
			$fmt=$fmt . "/nid_$i/ntmp_id_$i";
		}
		$fmt = $fmt."/Nid0/nblood0/Nid1/nblood1";
		return $this->unpark($this->sock->sendmsg($sendbuf),$fmt);
		//return $this->unpark($recvbuf,$fmt );
	}
	function battle_play_card($userid,$card_id){
		$pri_msg=pack("n",$card_id);
		$sendbuf=$this->park("2421",$userid,$pri_msg);
		$fmt="/Nid/ncardid/ncardtmpid";
		return $this->unpark($this->sock->sendmsg($sendbuf),$fmt);
	}
	function battle_next_stage($userid){
		$sendbuf=$this->park("2412",$userid,"");
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function battle_attack($userid,$card_id,$target){
		$pri_msg=pack("nn",$card_id,$target);
		$sendbuf=$this->park("2425",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function battle_drop_card($userid,$card_id){
		$pri_msg=pack("nn",1,$card_id);
		$sendbuf=$this->park("2432",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function battle_drop_cards($userid,$card_id1,$card_id2){
		$pri_msg=pack("nnn",2,$card_id1,$card_id2);
		$sendbuf=$this->park("2432",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function battle_giveup($userid){
		$sendbuf=$this->park("2413",$userid,"");
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function empty_cmd($userid){
		$sendbuf=$this->park("000000",$userid);
		//$fmt="/Nid/ncardid/ncardtmpid";
		$fmt="";
		return $this->unpark($this->sock->sendmsg($sendbuf),$fmt);
	}
	//gf_test_end
    
	function user_login_info($userid,$type,$login_time,$logout_time){
		$pri_msg=pack("LLL",$type,$login_time,$logout_time);
		$sendbuf=$this->park("3D00",$userid, $pri_msg );
		$recvbuf=$this->unpark($this->sock->sendmsg($sendbuf));

		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ltype_$i/Llogin_date_$i/Llogin_time_$i/Llogout_time_$i/Ltotal_time_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function get_login_info($userid,$login_date_form,$login_date_to){
		$pri_msg=pack("LL",$login_date_form,$login_date_to);
        $sendbuf=$this->park("3C10",$userid, $pri_msg );
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Ltype_$i/Llogin_date_$i/Llogin_time_$i/Llogout_time_$i/Ltotal_time_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

    function login_info_write($userid,$onlineid){
		$pri_msg=pack("L",$onlineid);
		$sendbuf=$this->park("3D20",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf));
	}

	function login_info_read($userid,$login_date){
		$pri_msg=pack("L",$login_date);
        $sendbuf=$this->park("3C30",$userid, $pri_msg );
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lonline_id_$i/Llogin_time_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}
	
	} 
?>
