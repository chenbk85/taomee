<?php

require('../common/error.inc');
require('../common/global.inc');
require('../db/proto.inc');

	if ( strncmp($_SERVER['REMOTE_ADDR'], "192", 3) != 0)
		error("DISPLAY WALL: ".$_SERVER['REMOTE_ADDR']." is not local ip");

	if (!isset($_GET['cmdid']))
		error("DISPLAY WALL: no cmdid");
	
	//dbg_log($_SERVER['QUERY_STRING']);
	$cmdid   = $_GET['cmdid'];
	switch ($cmdid) {
	case 1:		// get url by userid photoid
		$ownerid = $_GET['uid'];
		$photoid = $_GET['photoid'];
	
		$url = get_url_by_uid_photoid($ownerid, $photoid);
		echo $url;
		break;

	case 2: 	// get wall fileid by userid, photoid
		$ownerid = $_GET['uid'];
		$photoid = $_GET['photoid'];

		$wall_fileid = get_wfileid_by_uid_photoid($ownerid, $photoid);
		echo $wall_fileid;
		break;

	case 3: 	// get wall url by wall fileid
		$wall_fileid = $_GET['w_fileid'];

		$wall_url = get_wurl_by_wfileid($wall_fileid);
		echo $wall_url;
		break;

	case 4: 	// delete picture by wall fileid
		$wall_fileid = $_GET['w_fileid'];
	
		$ret = del_pic_by_wfileid($wall_fileid);
		echo $ret;
		break;

	default:
		error_cmd($cmdid, 600, "DISPLAY WALL: not support cmdid");

	}

	// cmd 1
	function get_url_by_uid_photoid($ownerid, $photoid) {
		$fileid = db_get_fileid($ownerid, $photoid);

        // just review the photos after 20090123:08    
        $ctime = substr(strchr($fileid, '_'), 1, -5);
        if ($ctime < 1232672161)
            return 0;

		// get photo url
		$url = fileid_url($fileid);
		$photo_url 	= $url.'.jpg'."\n";

		return $photo_url;
	}

	// cmd 2 verify ok: todo: create thumbnail
	function get_wfileid_by_uid_photoid($ownerid, $photoid) {
		$fileid = db_get_fileid($ownerid, $photoid);

		// get photo url
		$url = fileid_url($fileid);
		$photo_url 	= $url.'.jpg';
		$rec_url 	= $url.'.rec';
	
		// create des path
		$wall_fileid = create_wall_fileid($ownerid);
		$filename = substr($wall_fileid, 3);
		$des_pic_path = DISPLAY_WALL_DIR.$filename.'.jpg';
		$des_rec_path = DISPLAY_WALL_DIR.$filename.'.rec';
	

		$photo_data = file_get_contents($photo_url);
		$rec_data = file_get_contents($rec_url);
		if ($photo_data == false ||
			$rec_data == false )
			error_cmd($cmdid, 600, "DISPLAY_WALL: open url error: ".$photo_url);
	
		// copy picture and paint record to display wall directory
		$fp = fopen($des_pic_path, "w+");
		if ($fp == false)
			error_cmd($cmdid, 600, "DISPLAY_WALL: create file error");
		fwrite($fp, $photo_data);
		fclose($fp);
		$fp = fopen($des_rec_path, "w+");
		if ($fp == false)
			error_cmd($cmdid, 600, "DISPLAY_WALL: create file error");
		fwrite($fp, $rec_data);
		fclose($fp);
		//
	
		return $wall_fileid;
	}

	// cmd 3
	function get_wurl_by_wfileid($wfileid) {
		$servid = substr($wfileid, 0, 3);
		$filename = substr($wfileid, 3);

		global $g_server_map;
		if (strlen($g_server_map[$servid]) == 0)
			error_cmd($cmdid, 600, "DISPLAY_WALL: server id error");

		$url = "http://".$g_server_map[$servid].'/display_wall/'.$filename.".jpg";
	
		return $url;
	}

	// cmd 4
	function del_pic_by_wfileid($wfileid) {
		$filename = substr($wall_fileid, 3);
		$des_pic_path = DISPLAY_WALL_DIR.$filename.'.jpg';
		$des_rec_path = DISPLAY_WALL_DIR.$filename.'.rec';

		if (!file_exists($des_pic_path) || !file_exists($des_rec_path))
			return -1;

		unlink($des_pic_path);
		unlink($des_rec_path);

		return 0;
	}

	function db_get_fileid($ownerid, $photoid) {
		global $g_db_proxy_map;
		$db_proxy_ip = $g_db_proxy_map['ip'];
		$db_proxy_port = $g_db_proxy_map['port'];
	
		$proto = new Cproto($db_proxy_ip, $db_proxy_port);
		$result = $proto->pic_get_pic_by_photoid_userid($ownerid, $photoid);
		
		if ($result['result'] != 0)
			error_cmd($cmdid, 600, "DISPLAY_WALL: db query error: ".$result['result']);

		$fileid = $result['fileid'];
		return $fileid;
	}
	

	// wall fileid: serverid(3)uid(n)_time(10)
	function create_wall_fileid($uid) {
		$servid = G_DISPLAY_WALL_SERVER_ID;

		$now = time();
		$wall_fileid = $servid.$uid.'_'.$now;
		$check_path = DISPLAY_WALL_DIR.$uid.'_'.$now.'.jpg';

		while ( file_exists($check_path) ) {
			$now++;
			$check_path = DISPLAY_WALL_DIR.$uid.'_'.$now.'.jpg';
		}

		return $wall_fileid;
	}

	function fileid_url($fileid) {
		$servid = substr($fileid, 0, 3);
		$dir1 = substr($fileid, 3, 2);
		$dir2 = substr($fileid, 5, 2);
		$filename = substr($fileid, 7);

		global $g_server_map;
		$url = "http://".$g_server_map[$servid].'/mole_pictures/'.$dir1.'/'.$dir2.'/'.$filename;

		return $url;
	}


?>
