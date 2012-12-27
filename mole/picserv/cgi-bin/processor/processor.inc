<?php

class Processor
{
	protected   $parser;
	protected	$db_proxy_ip;
	protected	$db_proxy_port;

	function __construct() {
		global $g_db_proxy_map;
		$this->db_proxy_ip = $g_db_proxy_map[ip];
		$this->db_proxy_port = $g_db_proxy_map[port];
	}


	function execute() {}

	function fileid_url($fileid) {
		$servid = substr($fileid, 0, 3);
		$dir1 = substr($fileid, 3, 2);
		$dir2 = substr($fileid, 5, 2);
		$filename = substr($fileid, 7);

		global $g_server_map;
		$url = "http://".$g_server_map[$servid].'/mole_pictures/'.$dir1.'/'.$dir2.'/'.$filename;

		return $url;
	}

	function fileid_path($fileid) {
		$dir1 = substr($fileid, 3, 2);
		$dir2 = substr($fileid, 5, 2);
		$filename = substr($fileid, 7);

		$path = PICTURE_UPLOAD_DIR.$dir1.'/'.$dir2.'/'.$filename;
		return $path;
	}

}

?>
