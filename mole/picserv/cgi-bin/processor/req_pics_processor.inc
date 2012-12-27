<?php

require_once(dirname(__FILE__).'/./processor.inc');

/*******************************************
input: ownerid, albumid, start_index, req_count
output: list{
	photoid, photo_url, rec_url
}

db input: ownerid, albumid  start_index, req_count
db output: list {
	photoid, fileid
}
********************************************/

class ReqPicsProcessor extends Processor
{
	// input
	private $ownerid;
	private $albumid;	// is albumid in db
	private $start_index;
	private $req_count;

	// output
	private $photoid;
	private $photo_url;
	private $rec_url;

	private $cmdid;

	function __construct($parser) {
		parent::__construct();
		$this->parser = $parser;
	}


	function execute() {
		$this->cmdid = 	$this->parser->get_val('cmdid');

		$this->ownerid = $this->parser->get_val('ownerid');
		$this->albumid = $this->parser->get_val('albumid');
		$this->start_index = $this->parser->get_val('start_index');
		$this->req_count = $this->parser->get_val('req_count');

		if ( $this->ownerid == -1 		||
			$this->albumid == -1 ||
			$this->start_index == -1 	||
			$this->req_count == -1
		   ) 
			error_cmd($this->cmdid, ERR_NO_KEY, $this->ownerid.": check post key error_cmd");
	
		$proto = new Cproto($this->db_proxy_ip, $this->db_proxy_port);
		$db_result = $proto->pic_get_user_ablum_piclist($this->ownerid,
														$this->albumid,
														$this->start_index,
														$this->req_count);

		$this->format_output($db_result);
	}

	function format_output($result) {
		if ($result['result'] != 0)
			error_cmd($this->cmdid, $result['result'], $this->ownerid.": db request error_cmd");
			//error_cmd($this->cmdid, ERR_DB, "db request error_cmd");

		$response_head = 'cmdid='.$this->cmdid."\n"
						."result=0\n";
		echo $response_head;

		$count = $result['count'];
		echo "count=".$count."\n";
		for ($i = 0; $i < $count; $i++) {
			echo 'photoid['.$i.']='.$result['photoid_'.$i]."\n";

			$fileid = $result['fileid_'.$i];
			$url = $this->fileid_url($fileid);
			$photo_url 	= 'photo_url['.$i.']='.$url.'.jpg'."\n";
			$rec_url 	= 'rec_url['.$i.']='.$url.'.rec'."\n";
			echo $photo_url.$rec_url;
		}

	}

}


?>

