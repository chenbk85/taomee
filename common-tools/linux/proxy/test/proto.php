<?php

/**
 *
 * @info 游戏基础信息获取类
 * @author hary
 * @version 1.0
 * @date 2011-03-16
 */

define( "SUCC" ,0);

function hex2bin($hexdata) {
	$bindata = '';
	for($i=0; $i < strlen($hexdata); $i += 2) {
		$bindata .=chr(hexdec(substr($hexdata, $i, 2)));		
	}
	return $bindata;
}

/**
 * 协议包发送处理类
 * @author hary
 *
 */
class proto_socket{

	private $socket;

	/**
	 * 构造函数
	 * @param $payserip
	 * @param $payserport
	 * @return unknown_type
	 */
	function __construct( $payserip,$payserport)
	{
		$address = gethostbyname($payserip );

		if (($this->socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP))< 0 )
		{
			trigger_error("Couldn't create socket: " . socket_strerror(socket_last_error()) . "\n");
		}

		$result = socket_connect($this->socket,$address,$payserport );

		socket_set_option($this->socket,SOL_SOCKET,SO_RCVTIMEO,array("sec"=>60, "usec"=>0 ) );
		socket_set_option($this->socket,SOL_SOCKET,SO_SNDTIMEO,array("sec"=>60, "usec"=>0 ) );
	}

	/**
	 * 记录日志
	 * @param $msg
	 * @return unknown_type
	 */
	function log_msg($msg)
	{
		global $_SESSION;

		$log_pkg_arr=@unpack("Lproto_len/Lproto_id/Scommandid/Lresult/Luserid",$msg);

		if ( ($log_pkg_arr["commandid"] & 0x0100) ==0x0100)
		{
			dbg_log( $_SESSION["adminid"].":".  dechex($log_pkg_arr["commandid"] ).
                    ":".$log_pkg_arr["userid"].":".bin2hex($msg));
		}
	}

	/**
	 * 发送请求包
	 * @param $msg
	 * @return unknown_type
	 */
	function sendmsg($msg)
	{
		socket_write($this->socket,$msg, strlen($msg) );

		$buf       = socket_read($this->socket,8192);
		$pkg_arr   = @unpack("Lproto_len",$buf);
		$proto_len = $pkg_arr["proto_len"];

		while ($proto_len!=strlen($buf) )
		{
			$buf .=	socket_read($this->socket,4096);
		}

		return $buf;
	}

	/**
	 * 无返回包发送
	 * @param $msg
	 * @return unknown_type
	 */
	function sendmsg_without_returnmsg($msg)
	{
		$result=socket_write($this->socket,$msg, strlen($msg) );

		return $result?array("result"=>0):array("result"=>1010);
	}

	/**
	 * socket 链接关闭
	 * @return unknown_type
	 */
	function close()
	{
		socket_close($this->socket);
	}
}


/**
 * proto gameinfo获取类
 * @author hary
 *
 */
class proto{

	private $sock;
	private $adminid;

	/**
	 * 构造函数
	 * @param $payserip
	 * @param $payserport
	 * @return unknown_type
	 */
	function __construct( $payserip,$payserport)
	{
		$this->sock = new proto_socket ($payserip,$payserport);
	}

	/**
	 * 析构函数
	 * @return unknown_type
	 */
	function __destruct()
	{
		if ($this->sock) $this->sock->close();
	}

	function set_adminid($adminid)
	{
		$this->adminid=$adminid;
	}

	function park($cmdid,$userid,$private_msg = NULL){
		global $_SESSION;
		//18：报文头部长度
		//$pkg_len=22+strlen($private_msg) ;
		$pkg_len=18+strlen($private_msg) ;

		$result=0;
		$proto_id=$this->adminid;
		if ($proto_id==0){
			$proto_id=$_SESSION["adminid" ];
		}
		return pack("L2SL2",$pkg_len,$proto_id,hexdec($cmdid),$result,$userid)
		.$private_msg;
	}

	/**
	 * @param $sockpkg
	 * @param $private_fmt
	 * @return unknown_type
	 */
	function unpark($sockpkg, $private_fmt){
		//echo bin2hex($sockpkg);
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

	/**
	 * gf 带有角色id所以比其他游戏多4个字节长度
	 * @param $sockpkg
	 * @param $private_fmt
	 * @return unknown_type
	 */
	function gf_unpark($sockpkg, $private_fmt="")
	{
		$pkg_arr=@unpack("Lproto_len/Scommandid/Lproto_id/Lseq/Lresult/Lrole_time/Sgf/Sgz/Sgs",$sockpkg);

		if ($private_fmt!="" && $pkg_arr["result"]==0)
		{//成功
			$pkg_arr=@unpack("Lproto_len/Scommandid/Lproto_id/Lseq/Lresult/Lrole_time/Sgf/Sgz/Sgs/".$private_fmt,
			$sockpkg);
		}

		if ($pkg_arr)
		{
			return $pkg_arr;
		}
		else
		{
			return array("result"=>1003 );
		}
	}

	/**
	 * 打包
	 * @param $cmdid
	 * @param $userid
	 * @param $role_time
	 * @param $private_msg
	 * @return unknown_type
	 */
	function gf_park($cmdid,$userid,$role_time,$game_flag,$game_zone,$game_svr,$channel,$private_msg)
	{
		global $_SESSION;

		//22：报文头部长度
		$pkg_len = 28+34+strlen($private_msg) ;
		$result  = 0;
		$seqno	 = 0;
		$security = md5("channelId=0&securityCode=12345678&data=$private_msg");

		$proto_id=$this->adminid;

		if ($proto_id==0)
		{
			$proto_id=$_SESSION["adminid" ];
		}

		print_r(hex2bin(pack("LSL4S3",$pkg_len,$cmdid,$userid,$seqno,$result,$role_time,$game_flag,$game_zone,$game_svr)
		.$private_msg));
		//return pack("LSL4S3",$pkg_len,$cmdid,$userid,$seqno,$result,$role_time,$game_flag,$game_zone,$game_svr)
		//.$private_msg;
		return pack("LSL4S3Sa32",$pkg_len,$cmdid,$userid,$seqno,$result,$role_time,$game_flag,$game_zone,$game_svr,
				$channel,$security).$private_msg;
	}

	/**
	 * 执行命令
	 * @param $cmdid
	 * @param $userid
	 * @param $pri_msg
	 * @param $out_msg
	 * @return unknown_type
	 */
	function send_cmd($cmdid ,$userid, $pri_msg , $out_msg )
	{
		$sendbuf=$this->park( $cmdid , $userid,  $pri_msg );

		return $this->unpark($this->sock->sendmsg($sendbuf),$out_msg );
	}

	/**
	 * 获取用户是否已入住游戏
	 * @param $userid
	 * @return unknown_type
	 */
	function userinfo_get_gameflag($userid)
	{
		$cmdid   = "0007";
		$in_msg  = "";
		$out_msg = "Lgameflag";

		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	/**
	 * @param $userid
	 * @param $game_flag
	 * @return unknown_type
	 */
	function userinfo_add_game($userid, $game_flag)
	{
		$cmdid="0108";
		$in_msg=pack('L',$game_flag);
		$out_msg="";
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	/**
	 * @param $email
	 * @return unknown_type
	 */
	function user_get_userid_by_email($email)
	{
		$pri_msg=pack("A64", $email );
		$sendbuf=$this->park(4004,0, $pri_msg);

		return $this->unpark($this->sock->sendmsg($sendbuf),"Luserid");
	}

	/************************************** 摩尔庄园 ****************************************/

	/**
	 * 得到摩尔好友列表
	 * @param $userid
	 */
	function user_get_friend_list($userid )
	{
		$sendbuf=$this->park("1013",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Lcount";

		$recvarr=$this->unpark($recvbuf,$fmt );

		if ($recvarr && $recvarr["result"]!=SUCC)
		{
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];

		for ($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lfriendid_$i";
		}

		return $this->unpark($recvbuf,$fmt );
	}

	/**
	 * 获得摩尔用户昵称
	 * @param $userid
	 */
	function user_get_nick($userid)
	{
		$pri_msg="";
		$sendbuf=$this->park("104B",$userid, $pri_msg);

		return $this->unpark($this->sock->sendmsg($sendbuf),"a16nick");
	}


	/**
	 * 添加好友
	 * @param $userid
	 * @param $friendid
	 */
	function user_add_friend($userid ,$friendid )
	{
		$pri_msg=pack("L", $friendid);
		$sendbuf=$this->park("1111", $userid, $pri_msg);

		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//获取摩尔好友面板信息
	function user_get_user_board_info($userid)
	{
		$cmdid="30B5";
		$in_msg="";
		$out_msg= "Lflag/a16nick/a200professionlist/Lpetcolor/Lpetbirthday/Lxiaomee/Lexp/".
            "Lstrong/Liq/Lcharm/Lgame/Lwork/Lfashion".
                "/Lplant/Lbreed/Ldining_flag/Ldining_level/Litem_cnt";
		$sendbuf=$this->park($cmdid, $userid, $in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);

		$result = $this->unpark($recvbuf,$out_msg);

		$item_cnt = $result['item_cnt'];

		if ($item_cnt>0)
		{
			for($i=0;$i<$item_cnt;++$i)
			{
				$out_msg .= "/Litem_id_{$i}/Litem_count_{$i}";
			}

			$new_result = $this->unpark($recvbuf,$out_msg);

			$items = array();

			foreach($new_result as $k=>$v)
			{
				if (preg_match('/^item_id_(\d+)/', $k, $matches))
				{
					$key = $matches[1];
					$items[$key]['item_id'] = $v;
				}
				elseif (preg_match('/^item_count_(\d+)/', $k, $matches))
				{
					$key = $matches[1];
					$items[$key]['item_count'] = $v;
				}
			}

			$result['items'] = $items;
		}

		return $result;
	}


	/**
	 * mole获取用户装扮信息（人物装扮，nick，颜色，等级）
	 * @param $userid
	 * @return unknown_type
	 */
	function user_get_mole($userid)
	{
		$cmdid = "30F3";
		$in_msg = "";
		$sendbuf=$this->park($cmdid, $userid, $in_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt ="a16nick/Llevel/Lcolor/Lcount";
		$recvarr = $this->unpark($recvbuf, $fmt);
		$count = $recvarr["count"];

		for($i = 0; $i < $count ; $i++)
		{
			$fmt =$fmt."/Lclothe_$i";
		}

		return $this->unpark( $recvbuf,$fmt);
	}

	/**
	 * mole 邮件发送
	 * @param $userid
	 * @param $type
	 * @param $senderid
	 * @param $sendernick
	 * @param $msg
	 * @return unknown_type
	 */
	function emailsys_add_email($userid,$type,$senderid,$sendernick,$msg )
	{
		$msglen=strlen($msg);
		$pri_msg=pack("LLLA16LLa".$msglen,$type,time(NULL),$senderid,$sendernick,0,$msglen,$msg  );
		$sendbuf=$this->park("E101",$userid, $pri_msg );

		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	/*************************************** 赛尔号 *****************************************/

	/**
	 * 得到赛尔好友列表
	 * @param $userid
	 */
	function pp_get_friend_list($userid )
	{
		$sendbuf=$this->park("1836",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );

		if ($recvarr && $recvarr["result"]!=SUCC)
		{
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];

		for ($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lfriendid_$i";
		}

		return $this->unpark($recvbuf,$fmt );
	}

	/**
	 * 得到赛尔用户昵称
	 * @param $userid
	 */
	function pp_get_nick($userid)
	{
		$pri_msg="";
		$sendbuf=$this->park("181B",$userid, $pri_msg);

		return $this->unpark($this->sock->sendmsg($sendbuf),"a16nick");
	}

	//得到赛尔用户面板信息
	function pp_get_paipai_info3 ($userid )
	{

		$cmdid='18F8';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);

		$fmt='Lregtime/a16nick/Lvip/Lvip_rank/Lcolor/Ltexturize/Lmax_petlevel/Lpet_count/Lspt_value/Lmon_king_wins/Lmess_fight_win_count/Lmax_stages/Larena_win_count/Litem_count';
		$result=$this->unpark($recvbuf,$fmt);

		if ($result && $result['result']!=SUCC)
		{
			return $result;
		}

		$item_cnt = $result['item_count'];

		if ($item_cnt>0)
		{
			for($i=0;$i<$item_cnt;++$i)
			{
				$fmt .= "/Litem_id_{$i}/Lattire_rank_{$i}";
			}

			$new_result = $this->unpark($recvbuf,$fmt);

			$items = array();

			foreach($new_result as $k=>$v)
			{
				if (preg_match('/^item_id_(\d+)/', $k, $matches))
				{
					$key = $matches[1];
					$items[$key]['item_id'] = $v;
				}
				elseif (preg_match('/^attire_rank_(\d+)/', $k, $matches))
				{
					$key = $matches[1];
					$items[$key]['attire_rank'] = $v;
				}
			}

			$result['items'] = $items;
		}

		return $result;

	}

	/**
	 * 获取seer装扮信息（人物装扮，昵称，角色类型，等级）
	 * @param $userid
	 * @return unknown_type
	 */
	function get_school_bus_info($userid)
	{
		$sendbuf=$this->park("125a", $userid, $pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);

		$fmt="a16nick/Lpet_cnt/Lpet_top_lv/Lcolor/Lcount";

		$recvarr = $this->unpark($recvbuf, $fmt);

		if ($recvarr && $recvarr["result"]!=SUCC)
		{
			return $recvarr;
		}

		$count = $recvarr["count"];

		for($i=0; $i<$count; $i++)
		{
			$fmt=$fmt."/Litemid_$i/Litem_rank_$i";
		}

		return $this->unpark($recvbuf, $fmt);
	}

	/**
	 * seer 信息发送
	 * @param $userid
	 * @param $senderid
	 * @param $sendernick
	 * @param $msg_time
	 * @param $tmpltid
	 * @param $msglen
	 * @param $msg
	 * @return unknown_type
	 */
	function pp_msg_add_item ($userid ,$senderid,$sendernick,$msg_time,$tmpltid,$msglen,$msg)
	{
		$cmdid='1B01';
		$in_msg=pack('La16LLLa150' ,$senderid,$sendernick,$msg_time,$tmpltid,$msglen,$msg );
		$out_msg='Lis_max_count';

		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	/************************************** 小花仙 *******************************************/

	/**
	 * 得到小花仙好友列表
	 * @param $userid
	 * @param $begin
	 * @param $count
	 */
	function xhx_friend_get_all($userid,$begin,$count )
	{
		$pri_msg=pack("LL",$begin,$count);
		$sendbuf=$this->park("2E2",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);

		$fmt="Lallcount/Lcount";

		$recvarr=$this->unpark($recvbuf,$fmt );

		if ($recvarr && $recvarr["result"]!=SUCC)
		{
			return $recvarr;
		}
		//SUCC
		$recv_count=$recvarr["count"];

		for ($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lfriend_id_$i/a16remarks_$i/Lfriend_type_$i/Lskin_$i/Lface_$i/Leye_$i/Leyebrow_$i/Llip_$i/Lhair_$i";
		}

		return $this->unpark($recvbuf,$fmt );
	}

	/**
	 * 读取用户信息
	 * @param $userid
	 */
	function xhx_get_body($userid )
	{
		$cmdid='0204';
		$in_msg='';
		$out_msg='a16nick/Lskin/Lface/Leye/Leyebrow/Llip/Lregtime/Lbirthday/Llove/Lbeautiful/Lwisdom/Lplant_grade/Lnationality';

		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	/**
	 * 读取装扮列表
	 * @param $userid
	 * @param $begin
	 * @param $count
	 */
	function xhx_attire_get_all($userid,$begin,$count)
	{
		$pri_msg=pack("LL",$begin,$count);
		$sendbuf=$this->park("232",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Lall_count/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );

		if ($recvarr && $recvarr["result"]!=SUCC)
		{
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];

		for ($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lattire_id_$i/Lattirestate_$i";
		}

		return $this->unpark($recvbuf,$fmt);
	}

	//得到小花仙用户面板信息
	function xhx_get_basic_info($userid)
	{
		$sendbuf=$this->park("020D", $userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);

		$all = "Lregtime/Lbirthday/Lnationality/Llove/Lbeautiful/Lwisdom/Lplant_grade/Lol_count/Lol_time/Lol_last/Lol_today".
            "/a16nick/Lskin/Lface/Leye/Leyebrow/Llip/Lvip_flag/Lvip_month/Lvip_begin_time/Lvip_end_time/Lvip_auto_charge/".
            "Llast_charge_chnl_id/Lwonderful_val/Lfriend_add/Llike_medal/Lnew_task/Lcount_msg/Lcount_sys";

		return $this->unpark($recvbuf,$all);
	}

	/**
	 * @param $userid
	 * @param $open
	 * @return unknown_type
	 */
	function xhx_get_bus_info($userid, $open = false)
	{
		if($open){
			return $this->xhx_get_bus_info_20110106($userid);
		}else{
			return $this->xhx_get_bus_info_20101230($userid);
		}
	}

	/**
	 * @param $userid
	 * @return unknown_type
	 */
	function xhx_get_bus_info_20101230($userid)
	{
		$sendbuf=$this->park("022D", $userid);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$all = "Lgrade/a16nick";
		return $this->unpark($recvbuf,$all);
	}

	/**
	 * @param $receive_id
	 * @return unknown_type
	 */
	function bus_receive_gf_friend($receive_id)
	{
		$cmdid   = '063A';
		$in_msg  = pack('LLLLa'.strlen($mail_title).'a'.strlen($mail_body).'a'.strlen($mail_num_enclosure).'a'.strlen($mail_item_enclosure).'a'.strlen($mail_equip_enclosure) ,
		$sender_id,
		$receive_id,
		$mail_templet,
		$max_mail_limit,
		$mail_title,
		$mail_body,
		$mail_num_enclosure,
		$mail_item_enclosure,
		$mail_equip_enclosure);
		$out_msg = '';
		return $result = $this->send_cmd($cmdid,$i_user_id,$in_msg,$out_msg);
	}

	/**
	 * @param $i_user_id
	 * @return unknown_type
	 */
	function bus_receive_xhx_friend($i_user_id)
	{
		$cmdid   = '0355';
		$in_msg  = pack('La16LLLaLa' ,
		$from,
		$nick,
		$style,
		$font_color,
		$topic_len,
		$topic,
		$content_len,
		$content );
		$out_msg = '';
		return $result = $this->send_cmd($cmdid,$i_user_id,$in_msg,$out_msg);
	}

	/**
	 * @param $i_user_id
	 * @return unknown_type
	 */
	function bus_receive_seer_friend($i_user_id)
	{
		$cmdid   = '1B01';
		$msglen  = strlen($msg);
		$in_msg  = pack('La16LLLa' . $msglen,
		$senderid,
		$sendernick,
		$msg_time,
		$tmpltid,
		$msglen,
		$msg);
		$out_msg = 'Lis_max_count';
		return $result = $this->send_cmd($cmdid,$i_user_id,$in_msg,$out_msg);
	}

	/**
	 * @param $i_user_id
	 * @return unknown_type
	 */
	function bus_receive_mole_friend($i_user_id)
	{
		$cmdid   = '1B01';
		$msglen  = strlen($msg);
		$in_msg  = pack('La16LLLa'.$msglen,
		$senderid,
		$sendernick,
		$msg_time,
		$tmpltid,
		$msglen,
		$msg);
		$out_msg = 'Lis_max_count';
		return $result = $this->send_cmd($cmdid,$i_user_id,$in_msg,$out_msg);
	}

	/**
	 * 根据米米号获取用户在小花仙的基本信息、装扮信息
	 *
	 * @author bianwei<bianwei@taomee.com>
	 * @since  2011-01-04
	 * @param  int   $i_user_id  用户米米号
	 * @return array             用户信息关联数组
	 */
	function xhx_get_bus_info_20110106($i_user_id)
	{
		$sendbuf = $this->park("022D", $i_user_id);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$a_user_info = array(
        	'Lgrade',      // 等级
        	'a16nick',     // 昵称
        	'Lshin',       // 皮肤颜色
        	'Lface',       // 脸的类型
        	'Leye',        // 眼睛类型
        	'Leyebrow',    // 眉毛类型
        	'Llip',        // 嘴唇类型
        	'Lnationlity', // 国籍
        	'Lvip_flag',   // 0:不是; >=1:vip等级
        	'Lcount'       // 装扮个数
		);
		$s_user_info = implode('/', $a_user_info);
		$a_result    = $this->unpark($recvbuf, $s_user_info);
		// 装扮个数为0 或空
		if(!$a_result['count']){
			return $a_result;
		}
		// 循环取各装扮
		for($i = 0; $i < $a_result['count']; $i++)
		{
			$s_user_info .= "/Lattireid_$i";
		}
		return $this->unpark($recvbuf, $s_user_info);
	}
	/************************************** 功夫派 *******************************************/

	function get_friends_list($userid,$roletm,$flag,$zone,$svrtype) {
		$cmdid = '1001';
		$in_msg = pack('L', $userid);
		$sendbuf=$this->gf_park($cmdid,$userid,$roletm,$flag,$zone,$svrtype,0,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr = $this->gf_unpark($recvbuf, $fmt);
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;			        
		}
		
		$count = $recvarr['count'];
		for ($i = 0; $i < $count; $i++) {
			$fmt = $fmt.'/'."Lfriends_id_$i";
		}
		return $this->gf_unpark( $recvbuf,$fmt);
	}

	function get_user_nick($userid,$roletm,$flag,$zone,$svrtype) {
		$cmdid = '1002';
		$in_msg = pack('L', $userid);
		$sendbuf=$this->gf_park($cmdid,$userid,$roletm,$flag,$zone,$svrtype,0,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		
		return $this->gf_unpark($recvbuf, "a16nick");
	}

	function get_user_info($userid,$roletm,$flag,$zone,$svrtype) {
		$cmdid = '1003';
		$in_msg = pack('L', $userid);
		$sendbuf=$this->gf_park($cmdid,$userid,$roletm,$flag,$zone,$svrtype,0,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		
		$fmt='Lregtime/a16nick/Lvip/Lvip_rank/Lcolor/Ltexturize/Lmax_petlevel/Lpet_count/Lspt_value/Lmon_king_wins/Lmess_fight_win_count/Lmax_stages/Larena_win_count/Litem_count';
		return $this->gf_unpark($recvbuf, $fmt);
	}

	function send_mail($userid,$roletm,$flag,$zone,$svrtype,$sendid,$nick,$time,$type,$titlelen,$title,$bodylen,$body) {
		$cmdid = '1105';
		$in_msg = pack('La16LLLa40La150',$sendid,$nick,$time,$type,$titlelen,$title,$bodylen,$body);
		$in_msg2 = pack('L7',2, 1,1700906,2, 0,1,100);
		//$in_msg2 = pack('L',0);
		$in_msg = $in_msg.$in_msg2;
		$sendbuf = $this->gf_park($cmdid,$userid,$roletm,$flag,$zone,$svrtype,0,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		return $this->gf_unpark($recvbuf, "");
		
		//$this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	function add_items($userid,$roletm,$flag,$zone,$svrtype) {
		$cmdid = '1106';
		$in_msg = pack('L9', 2 ,0,100002,5,0, 0,60012,10,0);
		$sendbuf = $this->gf_park($cmdid,$userid,$roletm,$flag,$zone,$svrtype,0,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);

		return $this->gf_unpark($recvbuf, "");
	}

	/**
	 * 获取用户装扮，nick，角色，等级
	 * @param $userid
	 * @return unknown_type
	 */
	function gf_get_role_base_info_for_boss ($userid ){
		$cmdid='0670';
		$in_msg='';

		$sendbuf=$this->gf_park($cmdid,$userid,0,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lrole_type/Llevel/Lhonor/a16nick/Lclothes_num';
		$recvarr=$this->gf_unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['clothes_num'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Lclothes_id_$i/Llevel_$i";
		}
		return $this->gf_unpark( $recvbuf,$fmt);
	}

	//gf 发送邮件和奖品
	function gf_send_system_mail_pay($sender_id,$receive_id,$mail_templet,$max_mail_limit,$mail_title,$mail_body,$mail_num_enclosure,$mail_item_enclosure,$mail_equip_enclosure)
	{
		$cmdid='063A';
		$in_msg=pack('LLLLa41a101a1024a1024a1024' ,$sender_id,$receive_id,$mail_templet,$max_mail_limit,$mail_title,$mail_body,$mail_num_enclosure,$mail_item_enclosure,$mail_equip_enclosure );
		$out_msg='';
		$sendbuf=$this->gf_park( $cmdid,$receive_id,0,$in_msg );
		$msg = $this->sock->sendmsg($sendbuf);
		return substr($msg,22);
	}
	//发送即时消息
	function gf_send_switch_msg($msg,$receive_id){
		$cmdid='EA67';
		$out_msg='';
		$sendbuf=$this->gf_switch_park( $cmdid,$receive_id,$msg );
		$this->sock->sendmsg_without_returnmsg($sendbuf);
	}

	/**
	 * 功夫派发送邮件
	 * @param $sender_id
	 * @param $receive_id
	 * @param $mail_templet
	 * @param $max_mail_limit
	 * @param $mail_title
	 * @param $mail_body
	 * @param $mail_num_enclosure
	 * @param $mail_item_enclosure
	 * @param $mail_equip_enclosure
	 * @return unknown_type
	 */
	function gf_send_system_mail($sender_id,$receive_id,$mail_templet,$max_mail_limit,$mail_title,$mail_body,$mail_num_enclosure,$mail_item_enclosure,$mail_equip_enclosure)
	{
		$cmdid='063A';
		$in_msg=pack('LLLLa41a101a1024a1024a1024' ,$sender_id,$receive_id,$mail_templet,$max_mail_limit,$mail_title,$mail_body,$mail_num_enclosure,$mail_item_enclosure,$mail_equip_enclosure );
		$out_msg='';
		return  $this->send_cmd($cmdid,$receive_id,$in_msg,$out_msg);
	}

	/************************************** 大玩国 *******************************************/

	/**
	 * 读取用户信息
	 * @param $userid
	 * @return unknown_type
	 */
	function dd_get_info($userid){
		$sendbuf=$this->park("1C01",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),
                "Lflag/Lregtime/Lbirthday/a16nick/Lxiaomee/Lexp/a300signature");
	}

	/************************************** 登录 *********************************************/
	function userinfo_login_by_md5_two($userid,$passwd_md5_two,$ip,$login_channel  )
	{
		$cmdid='005F';
		$in_msg=pack("A16LL", hex2bin_pri($passwd_md5_two),$ip,$login_channel );
		$out_msg='Lgameflag';
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

	/************************************** 赛尔号 *********************************************/
	function pp_swap_list($userid,$is_add_xiaomee_day,$item1_count,$item2_count){
		$args = func_get_args();
		$args[0] = 'L*';
		//$pri_msg=pack("LLLLLLLL",$is_add_xiaomee_day,$item1_count,$item2_count,1,400119,1,10000,0);
		$pri_msg = call_user_func_array('pack', $args);
		$sendbuf=$this->park("1909", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Lleft_xiaomee/Lleft_energy/Lpk_medal_nums/Lpk_medal2_nums");
	}

	/************************************** 摩尔庄园 *********************************************/
	function user_add_attire($uid,$attire_type,$attire_id,$count,$maxcount){
		$cmdid='111C';
		$in_msg=pack('LLLL',$attire_type,$attire_id,$count,$maxcount);
		$out_msg='';
		return $this->send_cmd($cmdid,$uid,$in_msg,$out_msg);
	}

	/************************************** 小花仙 *********************************************/

	/**
	 * 校巴小花仙好友邀请邮件发送函数
	 *
	 * @param $userid  被邀请人id
	 * @param $npc_id  本次活动是1000
	 * @param $content 前面三个字段（邀请人id，邀请人姓名，邀请码）
	 * @return array
	 */
	function xhx_chat_across_svr($userid,$npc_id,$content)
	{
		$content_len=strlen($content);
		$format='LLa'.$content_len;
		$pri_msg=pack($format,$npc_id,$content_len,$content);
		$sendbuf=$this->park("F237",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	/**
	 * 小花仙装扮添加
	 * @param $uid
	 * @param $attire_id
	 * @return unknown_type
	 */
	function xhx_attire_add($uid,$attire_id){
		$cmdid='0330';
		$in_msg=pack('L',$attire_id);
		$out_msg='';
		return $this->send_cmd($cmdid,$uid,$in_msg,$out_msg);
	}

	/** 米饭标志位 **/
	function userinfo_get_mee_fans($uid)
	{
		$cmdid='0060';
		return $this->send_cmd($cmdid,$uid,'','Lflag');
	}

	/**
	 * @param $uid
	 * @param $flag
	 * @return unknown_type
	 */
	function userinfo_set_mee_fans($uid,$flag)
	{
		$cmdid='0161';
		$in_msg=pack('L',$flag);
		return $this->send_cmd($cmdid,$uid,$in_msg,'');
	}

	/** 摩尔勇士 **/
	/**
	 * @param $userid
	 * @return unknown_type
	 */
	function mole2_login ($userid ){
		$cmdid='0801';
		$in_msg='';
		$out_msg='a16nick/a32signature/Lflag/Lcolor/Lregister_time/Crace/Cprofesstion/Ljoblevel/Lhonor/Lxiaomee/Llevel/Lexperience/Sphysique/Sstrength/Sendurance/Squick/Sintelligence/Sattr_addition/Lhp/Lmp/Linjury_state/Cin_front/Lmax_attire/Lmax_medicine/Lmax_stuff';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}

	/** game act flag **/
	/**
	 * @param $userid
	 * @return unknown_type
	 */
	function userinfo_get_game_act_flag($userid)
	{
		$cmdid='0097';
		return $this->send_cmd($cmdid,$userid,'','Lflag');
	}

	/**
	 * @param $userid
	 * @param $flag_position
	 * @return unknown_type
	 */
	function userinfo_add_game_act_flag($userid, $flag_position)
	{
		$cmdid='0196';
		$in_msg=pack('L', $flag_position);
		return $this->send_cmd($cmdid,$userid,$in_msg,'');
	}

}
