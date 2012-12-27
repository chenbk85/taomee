#!/usr/bin/php5

<?php

$_colors = array(
        LIGHT_RED      => "[1;31m",
        LIGHT_GREEN     => "[1;32m",
        YELLOW         => "[1;33m",
        LIGHT_BLUE     => "[1;34m",
        MAGENTA     => "[1;35m",
        LIGHT_CYAN     => "[1;36m",
        WHITE         => "[1;37m",
        NORMAL         => "[0m",
        BLACK         => "[0;30m",
        RED         => "[0;31m",
        GREEN         => "[0;32m",
        BROWN         => "[0;33m",
        BLUE         => "[0;34m",
        CYAN         => "[0;36m",
        BOLD         => "[1m",
        UNDERSCORE     => "[4m",
        REVERSE     => "[7m",

);

$out_color = $_colors["LIGHT_RED"];

$server_ip = "10.1.1.24";
$server_port = 11001;

$g_socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if(!$g_socket)
{
    echo "socket create failed.";
    exit();
}

if(!socket_connect($g_socket, $server_ip, $server_port))
{
    echo "socket connected to ".$server_ip.":".$server_port." failed";
    exit();
}

$g_fp = fopen("/dev/stdin", "r");
main();

function main()
{
	global $g_socket;
	global $g_fp;
	global $out_color;

	//pkg_header_t
	$len = 18;
	$seq = 0;
	$type = 1000;
	$result = 0;
	$user_id = 0;

	$pkg = pack(LLvLL, $len, $seq, $type, $result, $user_id);

	$snd_len = socket_write($g_socket, $pkg, strlen($pkg));
	if($snd_len != strlen($pkg))
	{
    		echo "send data failed.";
    		if($snd_len > 0)
    		{
        		echo " Just send out ".$snd_len." bytes";
    		}
    		exit();
	}

	echo "snd_len: ".$snd_len."\n";
	$recved_data = socket_read($g_socket, 4096, PHP_BINARY_READ);
	echo "recved_data_len ".strlen($recved_data)."\n";
	if($recved_data == false || strlen(recved_data) <= 0)
	{
    		echo "recv data from server failed.";
    		exit();
	}



	$recved_data_array = unpack("Llen/Lseq/vtype/Lresult/Luser_id/Lusernum/vipnum", $recved_data);

	if($recved_data_array == false)
	{
    		echo "unpack failed.";
    		exit();
	}


	$ip_num = $recved_data_array['ipnum'];
	echo "------------------------------------------------------------------------------------------\n";
	echo chr(27).$out_color;
	echo "当前在线人数: ".$recved_data_array['usernum']."\n";
	echo "当前online机器总数量: ".$online_num."\n";

	$i = 0;
	while($ip_num)
	{ 
    		$bef_len = 24 + $i* 20;
    		$pack_style = "c".$bef_len."bef/a16ip/Lnum";
    		$online_info = unpack($pack_style, $recved_data);
    		$ip_num--;
    		$i++;
    		echo "online机器IP: ".trim($online_info['ip'])."          ";
    		echo "用户数量: ".$online_info['num']."\n";
	}

	echo chr(27).chr(27)."[0m"."\n";

	echo "-------------------------------------------------------------------------------------------\n";
	echo "please select:\n";
	echo "ip------------查看该ip对应的所有online进程\n";
	echo "id------------查看该米米号是否在线以及所处的online\n";
	echo "exit/quit-----退出)\n";

	while($input = fgets($g_fp, 100000))
	{
		$result = trim($input);
		switch($result)
		{
			case "exit":
			case "quit":
				echo "logout"."\n";
				exit();
			case "ip":
				get_online_info();
				break;
			case "id":
				get_user_info();
				break;	
			default:
				echo "invalid input\n";
				break;
		}
		echo "please select:\n";
		echo "ip-------------查看该ip对应的所有online进程\n";
		echo "id-------------查看该米米号是否在线以及所处的online\n";
		echo "exit/quit------退出\n";
	
	}

	fclose($g_fp);
	socket_close($g_socket);
}

function get_online_info()
{
	global $g_socket;
	global $g_fp;
	global $out_color;
	
	echo "Please input IP:";
	$input = fgets($g_fp, 100);
	$input_ip = trim($input);

	//pkg_header_t
	$len = 34;
	$seq = 0;
	$type = 1001;
	$result = 0;
	$user_id = 0;

	$pkg = pack(LLvLLa16, $len, $seq, $type, $result, $user_id, $input_ip);

	$snd_len = socket_write($g_socket, $pkg, strlen($pkg));
	if($snd_len != strlen($pkg))
	{
    		echo "send data failed.";
    		if($snd_len > 0)
    		{
        		echo " Just send out ".$snd_len." bytes";
    		}
    		exit();
	}

	echo "snd_len: ".$snd_len."\n";
	$recved_data = socket_read($g_socket, 4096, PHP_BINARY_READ);
	echo "recved_data_len ".strlen($recved_data)."\n";
	if($recved_data == false || strlen(recved_data) <= 0)
	{
    		echo "recv data from server failed.";
    		exit();
	}
		
		
	$recved_data_array = unpack("Llen/Lseq/vtype/Lresult/Luser_id/vonlinenum", $recved_data);

	if($recved_data_array == false)
	{
    		echo "unpack failed.";
    		exit();
	}


	$online_num = $recved_data_array['onlinenum'];
	echo "-------------------------------------------------------------------------------------------\n";
	echo chr(27).$out_color;
	echo "机器". $input_ip."共启动了 ". $online_num." 个online进程\n";

	$i = 0;
	while($online_num)
	{ 
    		$bef_len = 20 + $i* 16;
    		$pack_style = "c".$bef_len."bef/Lonlineid/Lonlinefd/Lusernum/vcuridx/vport";
    		$online_info = unpack($pack_style, $recved_data);
    		$online_num--;
    		$i++;
    		echo "online进程id: ".$online_info['onlineid']."   ";
    		echo "fd: ".$online_info['onlinefd']."   ";
    		echo "用户数量: ".$online_info['usernum']."   ";
    		echo "当前索引值: ".$online_info['curidx']."   ";
    		echo "online进程端口号: ".$online_info['port']."\n";
	}

	echo chr(27).chr(27)."[0m"."\n";
	echo "-------------------------------------------------------------------------------------------\n";

}

function get_user_info()
{
	global $g_socket;
	global $g_fp;
	global $out_color;
	echo "Please input userid:";
	$input = fgets($g_fp, 100);
	$input_user_id = trim($input);
	
	//pkg_header_t
	$len = 22;
	$seq = 0;
	$type = 1002;
	$result = 0;
	$user_id = 0;

	$pkg = pack(LLvLLL, $len, $seq, $type, $result, $user_id, $input_user_id);

	$snd_len = socket_write($g_socket, $pkg, strlen($pkg));
	if($snd_len != strlen($pkg))
	{
    		echo "send data failed.";
    		if($snd_len > 0)
    		{
        		echo " Just send out ".$snd_len." bytes";
    		}
    		exit();
	}

	echo "snd_len: ".$snd_len."\n";
	$recved_data = socket_read($g_socket, 4096, PHP_BINARY_READ);
	echo "recved_data_len ".strlen($recved_data)."\n";
	if($recved_data == false || strlen(recved_data) <= 0)
	{
    		echo "recv data from server failed.";
    		exit();
	}
		
		
	$online_info = unpack("Llen/Lseq/vtype/Lresult/Luser_id/cison/Llogintime/Lonlineid/a16ip/vport", $recved_data);

	if($online_info == false)
	{
    		echo "unpack failed.";
    		exit();
	}

	echo "-------------------------------------------------------------------------------------------\n";
	echo chr(27).$out_color;

		echo "user ".$input_user_id;
		echo "isonline:".$online_info['ison']."\n";
		if($online_info['ison'] == '0')
		{
			echo "不在线.\n";
		}
		else
		{
    			echo "登录时间".date("Y-m-d H:i:s",$online_info['logintime'])."   ";
    			echo "所在的online_id: ".$online_info['onlineid']."   ";
    			echo "所在的online_ip: ".$online_info['ip']."   ";
    			echo "所在的端口: ".$online_info['port']."\n";
		}
	echo chr(27).chr(27)."[0m"."\n";
	echo "-------------------------------------------------------------------------------------------\n";
}
?>
