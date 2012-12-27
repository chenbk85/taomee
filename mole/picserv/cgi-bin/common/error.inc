<?php

require_once(dirname(__FILE__).'/../config/config.inc');

function error($error) 
{
	dbg_log($error);
	echo "cmdid=x"."\n"."result=1"."\n";
    exit; 
} // end error handler 

function error_cmd($cmdid, $errid, $error) 
{
	dbg_log($cmdid.':'.$errid.':'.$error);
	echo "cmdid=".$cmdid."\n"."result=".$errid."\n";
    exit; 
}


//
function dbg_log($word)
{
    $fp = fopen(PICSERVER_LOG_DIR.LOG_FILE, "a+");
    flock($fp, LOCK_EX) ;
    fwrite($fp, strftime($_SERVER['REMOTE_ADDR']."\t%Y/%m/%d %H:%M:%S",time()).": ".$word."\n");
    flock($fp, LOCK_UN);
    fclose($fp);
}

?>
