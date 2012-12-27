<?php

require_once('../parser/form_parser.inc');
require_once('../processor/add_pic_processor.inc');

global $_POST;

$_POST['uid'] = 490293;
$_POST['session'] = 0;
$_POST['cmdid'] = 0;
$_POST['albumid'] = 0;
$_POST['ownerid'] = 1230;
$_POST['paint_record'] = 'l, r, u, d';
$_POST['filename'] = 'sun';

$parser = new FormParser();
$parser->parse_form();

$processor = new AddPicProcessor($parser);
echo $processor->execute();

?>
