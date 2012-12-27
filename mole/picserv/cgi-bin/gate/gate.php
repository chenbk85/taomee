<?php

require_once('../common/error.inc');
require_once('../auth/auth_session.inc');
require_once('../parser/form_parser.inc');
require_once('../processor/add_pic_processor.inc');
require_once('../processor/req_pics_processor.inc');
require_once('../processor/del_pic_processor.inc');

if (!isset($_POST['uid']))
	error("no user id in post");

if (!isset($_POST['cmdid']))
	error("no cmdid in post");

if (!isset($_FILES['session']))
	error("no session in post");

$session_file = $_FILES['session']['tmp_name'];
$uid = $_POST['uid'];
$cmdid = $_POST['cmdid'];
$servip = $_SERVER['SERVER_ADDR'];
$port = $_SERVER['SERVER_PORT'];

if ($uid > 20000000 && $uid < 30000000)
	error($uid." is tw uid");

$tuya_key = 'tVYa:vr@';
$author = new AuthSession($tuya_key);
$author->decode_session(file_get_contents($session_file));
if ( !$author->check_uid($uid) 								 ||
	 !$author->check_time(time()) 							 ||
	 (!$author->check_ip($servip) && ($cmdid != G_CMD_DELPIC)) ||	// don't check servip if cmdid is del
	 (!$author->check_port($port)   && ($cmdid != G_CMD_DELPIC))		// don't check port if cmdid is del
	) 
{
	$author->print_member();
	error_cmd($cmdid, ERR_SESSION, $uid."\tauth error");
}

$parser = new FormParser();
$parser->parse_form();

switch ($cmdid) {
	case G_CMD_ADDPIC:
		$processor = new AddPicProcessor($parser);
		break;
	case G_CMD_REQPIC:
		$processor = new ReqPicsProcessor($parser);
		break;
	case G_CMD_DELPIC:
		$processor = new DelPicProcessor($parser);
		break;
	default:
		error_cmd($cmdid, ERR_CMD_NOT_SUPPORT, " is not support\n");
}

$processor->execute();


?>
