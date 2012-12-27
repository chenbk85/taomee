<?php

require_once("../parser/form_parser.inc");

global $_POST;
$keys = array('cmdid', 'ownerid', 'operatorlist', 'description', 'ctime', 'comment');

$_POST['cmdid'] = 0;
$_POST['ownerid'] = 1230;
$_POST['operatorlist'] = 'l, r, u, d';
$_POST['ctime'] = time();
$_POST['comment'] = "good job";

$parser = new FormParser();
if (!$parser->parse_form())
	return false;

foreach($keys as $key) 
	echo $key."=".$parser->get_val($key)."\n";




?>

