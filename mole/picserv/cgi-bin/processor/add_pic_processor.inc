<?php

require_once(dirname(__FILE__).'/./processor.inc');
require_once(dirname(__FILE__).'/../common/error.inc');
require_once(dirname(__FILE__).'/../db/proto.inc');

/*******************************************
input: uid, albumid, paint_record, filename
output: photoid, photo_url, rec_url, paint_record, filename

db input: uid, fileid, albumid
db output: photoid
********************************************/

class AddPicProcessor extends Processor
{
	// input
	private $uid;
	private $albumid;
	private $recname;
	private $filename;

	// db input
	private $fileid;

	// db output
	private $db_result;
	private $ctime;

	// output
	private $photoid;
	private $photo_url;
	private $rec_url;

	// member
	private	$fieldname = 'uploadfile';
	private	$rec_fieldname = 'paint_record';
	private $save_filename;		// just filename without format
	private $path;				// the whole pic path without format

	private $cmdid;

	function __construct($parser) {
		parent::__construct();
		$this->parser = $parser;
	}

	function execute() {
		$this->cmdid = 	$this->parser->get_val('cmdid');
		$this->uid = $this->parser->get_val('uid');
		$this->albumid = $this->parser->get_val('albumid');
		$this->recname = $_POST[$this->rec_fieldname][name];
		$this->filename = $_POST[$this->fieldname][name];

		// check upload: exit if some error_cmd occur.
		$this->check_upload();

		if ( $this->uid == -1 		||
			$this->albumid == -1    ||
			$this->filename == -1 	||
			$this->recname == -1 ) 
			error_cmd($this->cmdid, ERR_NO_KEY, $this->uid.': check post key error_cmd');

		$dir1 = str_pad(rand(0,99), 2, '0', STR_PAD_LEFT);
		$dir2 = str_pad(rand(0,99), 2, '0', STR_PAD_LEFT);
		$this->create_save_path($dir1, $dir2);

		if ( !$this->save_pic() ||
			 !$this->save_paint_record() )
			error_cmd($this->cmdid, ERR_SAVE_FILE, $this->uid.": save picture error_cmd\n$this->path");

		$this->create_fileid($dir1, $dir2);
		
		$proto = new Cproto($this->db_proxy_ip, $this->db_proxy_port);
		$db_result = $proto->pic_add_pic($this->uid,
										$this->albumid,
										$this->fileid);

		$this->format_output($db_result);
	}

	function format_output($result) {
		if ($result['result'] != 0) {
			$photo_path = $this->path.'.jpg';
			$rec_path 	= $this->path.'.rec';
			if (file_exists($photo_path))
				unlink($photo_path);
			if (file_exists($rec_path))
				unlink($rec_path);
			error_cmd($this->cmdid, $result['result'], $this->uid.": db request error_cmd");
		}

		$response_head = 'cmdid='.$this->cmdid."\n"
						."result=0\n";

		// follow echo is return to client
		echo $response_head;
		echo 'photoid='.$result['photoid']."\n";
		$url = $this->fileid_url($this->fileid);
		$photo_url 	= 'photo_url='.$url.'.jpg'."\n";
		$rec_url 	= 'rec_url='.$url.'.rec'."\n";
		echo $photo_url.$rec_url;
	}


	private function check_upload() {
		// possible PHP upload error_cmds.
		$error_cmds = array(1 => ': php.ini max file size exceeded', 
        		        2 => ': html form max file size exceeded', 
            		    3 => ': file upload was only partial', 
                		4 => ': no file was attached', 
	                	5 => ': The servers temporary folder is missing', 
	    	            6 => ': Failed to write to the temporary folder.'); 


		($_FILES[$this->fieldname]['error_cmd'] == 0) 
		    or error_cmd($this->cmdid, ERR_UPLOAD, $this->uid.$error_cmds[$_FILES[$this->fieldname]['error_cmd']]); 
	
		@is_uploaded_file($_FILES[$this->fieldname]['tmp_name'])
		    or error_cmd($this->cmdid, ERR_NO_IMAGE, $this->uid.': tmp pic file is not exist'); 

		@is_uploaded_file($_FILES[$this->rec_fieldname]['tmp_name'])
		    or error_cmd($this->cmdid, ERR_NO_IMAGE, $this->uid.': tmp rec file is not exist'); 

		@getimagesize($_FILES[$this->fieldname]['tmp_name']) 
    		or error_cmd($this->cmdid, ERR_NOT_IMAGE, $this->uid.': only image uploads are allowed'); 
	}

	private function save_pic() {
		$format = '.jpg';
		return move_uploaded_file($_FILES[$this->fieldname]['tmp_name'], $this->path.$format);
	}

	private function save_paint_record() {
		$format = '.rec';
		return move_uploaded_file($_FILES[$this->rec_fieldname]['tmp_name'], $this->path.$format);
	}


	private function create_save_path($dir1, $dir2) {
		$reserve_field = rand(1000, 9999);
		$this->ctime = time();

		$this->save_filename = $this->uid.'_'.$this->ctime.'_'.$reserve_field;
		$this->path = PICTURE_UPLOAD_DIR.$dir1.'/'.$dir2.'/'.$this->save_filename;
		while ( file_exists($this->path) ) {
	    	$this->ctime++; 
			$this->save_filename = $this->uid.'_'.$this->ctime.'_'.$reserve_field;
			$this->path = PICTURE_UPLOAD_DIR.$dir1.'/'.$dir2.'/'.$this->save_filename;
		}
	}

	// fileid: servid(3)dir1(2)dir2(2)uid(n)_time(10)_reserve(4)
	private function create_fileid($dir1, $dir2) {
		$servid = G_SERVER_ID;
		$this->fileid = $servid.$dir1.$dir2.$this->save_filename;
	}

	private function build_sql() {
		//$sql = $this->uid, $this->fileid, $this->filename, $this->albumid;

	}
}

?>	 
