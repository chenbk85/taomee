<?php
require_once "conf.php";

# ============================================================================== 

function is_supported_svc($svc)
{
	if (empty($svc)) return FALSE;

	global $CONF;
	$count = count($CONF['valid_svc']);
	for ($i = 0; $i < $count; $i++ ) {
		if ($CONF['valid_svc'][$i] == $svc) return TRUE;
	}
	return FALSE;
}

function is_valid_page_size($page_size)
{
	global $CONF;
	if ($page_size > $CONF['max_page_size']) {
		return FALSE;
	}
	return TRUE;
}

function get_remote_ip()
{
    if (getenv('HTTP_X_FORWARDED_FOR')) { 
        $ip = getenv('HTTP_X_FORWARDED_FOR'); 
    } else if (getenv('HTTP_CLIENT_IP')) { 
        $ip = getenv('HTTP_CLIENT_IP' ); 
    } else { 
        $ip = getenv('REMOTE_ADDR'); 
    } 
    return $ip;
}

function check_remote_ip($ip_str)
{
    global $CONF;
	$accept_ip = $CONF['accept_ip'];
	$count = count($accept_ip);
	for ($i = 0; $i < $count; $i++) {
		if (preg_match($accept_ip[$i], $ip_str)) return TRUE;
	}
	$log = "Client [{$ip_str}] denied for querying log";
	do_log($log);
	return FALSE;
}

/* log */
function do_log($str, $log_file="")
{
    global $CONF;
	$log_dir=$CONF['logdir'];
    if(empty($log_file)) {
        $log_name = "search_".date('ymd').".log" ;
        $log_file = $log_dir.$log_name;
        if(!is_dir($log_dir)) {
            exec("mkdir ".$log_dir) ;
            exec("chmod -R 777 ".$log_dir) ;
        }
    }

    if(!is_file($log_file)) {
        exec("touch ".$log_file) ;
        exec("chmod 666 ".$log_file);
    }
    clearstatcache();
    if(is_file($log_file) && !is_writable($log_file)) {
        exec("touch ".$log_file) ;
        exit('logdir unwritable: '.$log_file) ;
    }
    $fp = fopen($log_file, 'a+b');
    if($fp) {
        fwrite($fp, $str."\r\n") ;
        fclose($fp) ;
    } 
    return true;
}

function set_filters(&$cl, $query_cmd, $uid, $cmd)
{
	global $CMD_LIST;

	$zero = array(0);
	$uid_filter=NULL;
	$cmd_filter=NULL;
	switch($query_cmd) {
	case $CMD_LIST['query_summary']:
		break;

	case $CMD_LIST['query_sw_freq']:
	case $CMD_LIST['query_sw_interval']:
		if ($uid > 0) {
			$uid_filter = array($uid);
		}
		if ($cmd > 0) {
			$cmd_filter = array($cmd);
		}
		$cl->SetFilter('sw_ill_code', $zero, true);
		break;

	case $CMD_LIST['query_tw_freq']:
	case $CMD_LIST['query_tw_interval']:
		if ($uid > 0) {
			$uid_filter = array($uid);
		}
		$cl->SetFilter('tw_ill_code', $zero, true);
		break;
	}

	if (!empty($uid_filter)) {
		$cl->SetFilter('uid', $uid_filter);
	}
	if (!empty($cmd_filter)) {
		$cl->SetFilter('cmd', $cmd_filter);
	}
}

function set_groupby(&$cl, $query_cmd, $uid, $cmd)
{
	global $CMD_LIST;

	$groupby=NULL;
	$groupsort=NULL;
	switch($query_cmd) {
	case $CMD_LIST['query_summary']:
		$groupby="uid";
		break;

	case $CMD_LIST['query_sw_freq']:
	case $CMD_LIST['query_sw_interval']:
		if ($cmd == 0) {
			$groupby="cmd";
		}
		break;

	case $CMD_LIST['query_tw_freq']:
	case $CMD_LIST['query_tw_interval']:
		if ($cmd == 0) {
			$groupby="cmd";
		}
		break;
	}

	$groupsort="@count DESC, cmd_recv_sec ASC, @id ASC";
	if (!empty($groupby) && !empty($groupsort)) {
		$cl->SetGroupBy ( $groupby, SPH_GROUPBY_ATTR, $groupsort);
		return 1;
	}
	return 0;
}

function set_select(&$cl, $query_cmd, $uid, $cmd, $is_grouped)
{
	global $CMD_LIST;

	$select=NULL;
	switch($query_cmd) {
	case $CMD_LIST['query_summary']:
	case $CMD_LIST['query_sw_freq']:
	case $CMD_LIST['query_sw_interval']:
	case $CMD_LIST['query_tw_freq']:
	case $CMD_LIST['query_tw_interval']:
		if ($is_grouped) {
			$select="*, @count";
		} else {
			$select="*";
		}
		break;
	}

	if (!empty($select)) {
		$cl->SetSelect ( $select );
	}
}


?>
