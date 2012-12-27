<?php
	require_once("proto.php");
	function read_file_by_signal($fname, $signal) {
		$fp = fopen($fname, "r");
		if ($fp == "") {
			return 0;
		}
		$ret_array = array();
		while ($data = fgetcsv($fp, 0, "\t")) {
			if ($signal != $data[0]) {
				continue;
			}
			array_push($ret_array, $data);
		}
		fclose($fp);
		return $ret_array;
	}

	//get file name
	$file_name = "reward.conf";
	if ($argv[1] != "") {
		$file_name = $argv[1];
	}

	//read login information from file
	$ret = read_file_by_signal($file_name, "login");
	$login_info = $ret[0];
	$proto_login=new Cproto($login_info[1],$login_info[2]);

	//read online information from file
	$ret = read_file_by_signal($file_name, "online");
	$online_info = $ret[0];
	$proto_online=new Cproto($online_info[1],$online_info[2]);

	//read online version from file
	$ret = read_file_by_signal($file_name, "ol_ver");
	$version_info = $ret[0];
	$ol_ver = $version_info[1];

	//read user information from file
	$ret = read_file_by_signal($file_name, "userid");
	$user_info = $ret[0];
	$userid = $user_info[1];
	$roletm = $user_info[2];
	
	//user login...
	$session = $proto_login->main_login($userid,md5(md5("142857")),0,6,0);
	print_r($proto_online->login($userid,$session,$roletm,$ol_ver));

	//user execute any cmds...
	$ret = read_file_by_signal($file_name, "cmd");
	for ($i = 0; $i < count($ret); $i++) {
		$cmd_data = $ret[$i];
		print_r($proto_online->execute_online_cmd($userid, $cmd_data));
	}
?>
