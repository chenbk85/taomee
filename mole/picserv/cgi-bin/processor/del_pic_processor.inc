<?php

require_once(dirname(__FILE__).'/./processor.inc');


/*******************************************
input: ownerid, photoid
output: 0, 1

db input: uid, ownerid, photoid
db output: fileid
********************************************/

class DelPicProcessor extends Processor
{
	// input
	private $uid;
	private $ownerid;
	private $photoid;

	// 
	private $photo_url;
	private $rec_url;

	private $cmdid;

	function __construct($parser) {
		parent::__construct();
		$this->parser = $parser;
	}


	function execute() {
		$this->cmdid = 	$this->parser->get_val('cmdid');

		$this->uid = $this->parser->get_val('uid');
		$this->ownerid = $this->parser->get_val('ownerid');
		$this->photoid = $this->parser->get_val('photoid');

		if ( $this->uid == -1 	 ||
			$this->ownerid == -1 ||
			$this->photoid == -1 
		   ) 
			error_cmd($this->cmdid, ERR_NO_KEY, $this->uid.": check post key error");


		$proto = new Cproto($this->db_proxy_ip, $this->db_proxy_port);
		$db_result = $proto->pic_del_pic_by_photoid_userid($this->ownerid, 
															$this->photoid);

		$this->format_output($db_result);

	}

	function format_output($result) {
		if ($result['result'] != 0)
			error_cmd($this->cmdid, ERR_DB, $this->uid.": db request error");

		$response_head = 'cmdid='.$this->cmdid."\n"
						."result=0\n";

		$fileid = $result['fileid'];
		$path = $this->fileid_path($fileid);
		$photo_path = $path.'.jpg';
		$rec_path 	= $path.'.rec';
		if (file_exists($photo_path))
			unlink($photo_path);
		if (file_exists($rec_path))
			unlink($rec_path);

		echo $response_head;
	}

}


?>

