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
		socket_write($this->socket,$msg, strlen($msg) );
		$buf = socket_read($this->socket,8192);
		$pkg_arr=@unpack("Nproto_len",$buf);
		$proto_len= $pkg_arr["proto_len"];
		while ($proto_len > strlen($buf) ){
			$buf .=	socket_read($this->socket,4096);
		}	
		return $buf;
	}
	function sendmsg_without_returnmsg($msg){
		//$this->log_msg($msg);
		$result = socket_write($this->socket,$msg, strlen($msg) );
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
	var $online_cmd_function = array();

	function __construct( $payserip,$payserport){
		$this->sock = new proto_socket($payserip,$payserport);
		$this->register_function();
	}

	function __destruct(){
		if ($this->sock) $this->sock->close(); 
	}

	function set_adminid($adminid){
		$this->adminid=$adminid;
	}


	//park_function
	function park($cmdid,$userid,$seqno_add,$private_msg){
		global $_SESSION;
		global $g_seqno;
		//18：报文头部长度
		$pkg_len = 18 + strlen($private_msg);
		//print_r($pkg_len);
		$result=0;
		$proto_id=$this->adminid;
		if ($proto_id==0){
			$proto_id=$_SESSION["adminid" ];
		}
		$ver = 1;
		$seqnum = $g_seqno;
		$g_seqno += $seqno_add;
		return pack("NnN3",$pkg_len,$cmdid,$userid,$seqnum,$result)
			.$private_msg;
	}

	function unpark($sockpkg, $private_fmt){
		$pkg_arr=@unpack("Nproto_len/ncommandid/Nuserid/Nseq/Nret",$sockpkg);
//    	print_r($pkg_arr);
		if ($private_fmt!="" && $pkg_arr["result"]==0){//成功
			$pkg_arr=@unpack("Nproto_len/ncommandid/Nuserid/Nseq/Nret".$private_fmt, 
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

//--------------------------------------------------------------------------------
//-------------login cmd------------------------
//--------------------------------------------------------------------------------
	function main_login($userid,$pass,$channal,$whichgame,$ip){
		print_r("*****main login[103]*****\n");
		$img_id = "0000000000000000";
		$verif_code = "000000";
		$pri_msg=pack("a32NNNa16a6",$pass,$channal,$whichgame,$ip, $img_id, $verif_code);
		$sendbuf=$this->park("103",$userid,0,$pri_msg);
		//print_r($sendbuf);
		$fmt="/Nzero/a16session/Nrolecreate";
		//return $this->unpark($this->sock->sendmsg($sendbuf),$fmt);
		$sockpkg = $this->sock->sendmsg($sendbuf);
		$pkg_arr=@unpack("Nproto_len/ncommandid/Nuserid/Nresult/Nseq".$fmt,$sockpkg);
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

//--------------------------------------------------------------------------------
//-------------online cmd------------------------
//--------------------------------------------------------------------------------

	function register_function() {
		$this->online_cmd_function[2001] = "enter_map";
		$this->online_cmd_function[2201] = "undertake_task";
	}

	function execute_online_cmd($userid, $data) {
		$online_fun = $this->online_cmd_function[$data[1]];
		$need_data = array_slice($data, 2, count($data) - 2);
		
		//print send data information
		print "****** $online_fun [$data[1]] *****\n";
		print "send:[";
		for ($i = 0; $i < count($need_data); $i++) {
			print " $need_data[$i] ";
		}
		print "]\n";

		return $this->$online_fun($userid, $need_data);
	}

	function login($userid,$loginsession,$role_tm,$ver_data){
		print_r("*****login[1001]*****\n");
		$pri_msg=pack("a16NN",$loginsession,$role_tm,$ver_data);
		$sendbuf=$this->park("1001",$userid,1,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function enter_map($userid, $data){
		$pri_msg=pack("NNNN",$data[0], $data[1], $data[2], $data[3]);
		$sendbuf=$this->park("2001",$userid,1,$pri_msg);
		$fmt="/Nid";
		return $this->unpark($this->sock->sendmsg($sendbuf),$fmt);
	}

	function undertake_task($userid,$data){
		$pri_msg=pack("N",$data[0]);
		$sendbuf=$this->park("2201",$userid,1,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	} 
?>
