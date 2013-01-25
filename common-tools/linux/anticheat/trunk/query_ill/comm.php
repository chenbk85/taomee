<?php
require_once "conf.php";

#########################################
# Helper Functions 

function linktoself($params,$selflink= '') {
    $a = array();
    $b = explode('?',$_SERVER['REQUEST_URI']);
    if (isset($b[1])) 
        parse_str($b[1],$a);

    if (isset($params['value']) && isset($a[$params['name']])) {
        if ($params['value'] == 'null') {
            unset($a[$params['name']]);
        } else {
            $a[$params['name']] = $params['value'];
        }

    } else {
        foreach ($params as $key => $value)
            $a[$key] = $value;
    }

    if (!empty($params['delete'])) {
        if (is_array($params['delete'])) {
            foreach ($params['delete'] as $del) {
                unset($a[$del]);
            }
        } else {
            unset($a[$params['delete']]);
        }
        unset($a['delete']);
    } 
    if (empty($selflink)) {
        $selflink = $_SERVER['SCRIPT_NAME'];
    } 
    if ($selflink == '/index.php') {
        $selflink = '/';
    }

    return htmlentities($selflink.(count($a)?("?".http_build_query($a,'','&')):''));
}

function pagesString($currentPage,$numberOfPages,$postfix = '',$extrahtml ='') {
    static $r;
    if (!empty($r))
        return($r);

    if ($currentPage > 1) 
        $r .= "<a href=\"".linktoself(array('page'=>$currentPage-1))."$postfix\"$extrahtml>&lt; &lt; prev</a> ";
    $start = max(1,$currentPage-5);
    $endr = min($numberOfPages+1,$currentPage+8);

    if ($start > 1)
        $r .= "<a href=\"".linktoself(array('page'=>1))."$postfix\"$extrahtml>1</a> ... ";

    for($index = $start;$index<$endr;$index++) {
        if ($index == $currentPage) 
            $r .= "<b>$index</b> "; 
        else
            $r .= "<a href=\"".linktoself(array('page'=>$index))."$postfix\"$extrahtml>$index</a> ";
    }
    if ($endr < $numberOfPages+1) 
        $r .= "... ";

    if ($numberOfPages > $currentPage) 
        $r .= "<a href=\"".linktoself(array('page'=>$currentPage+1))."$postfix\"$extrahtml>next &gt;&gt;</a> ";

    return $r;
}


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
	$one = array(1);
	$two = array(2);

	$uid_filter=NULL;
	$cmd_filter=NULL;
	switch($query_cmd) {
	case $CMD_LIST['query_summary']:
		break;

	case $CMD_LIST['query_sw_freq']:
		if ($uid > 0) {
			$uid_filter = array($uid);
		}
		if ($cmd > 0) {
			$cmd_filter = array($cmd);
		}
		$cl->SetFilter('sw_ill_code', $two);
		break;

	case $CMD_LIST['query_sw_interval']:
		if ($uid > 0) {
			$uid_filter = array($uid);
		}
		if ($cmd > 0) {
			$cmd_filter = array($cmd);
		}
		$cl->SetFilter('sw_ill_code', $one);
		break;

	case $CMD_LIST['query_tw_freq']:
		if ($uid > 0) {
			$uid_filter = array($uid);
		}
		$cl->SetFilter('tw_ill_code', $two);
		break;

	case $CMD_LIST['query_tw_interval']:
		if ($uid > 0) {
			$uid_filter = array($uid);
		}
		$cl->SetFilter('tw_ill_code', $one);
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

function store_array(&$data_array, $table, $mysqli)
{
	$cols = implode(',', array_keys($data_array));
	foreach (array_values($data_array) as $value) {
		isset($vals) ? $vals .= ',' : $vals = '';
		$vals .= '\''.$mysqli->real_escape_string($value).'\'';
	}
	$mysqli->real_query('INSERT INTO '.$table.' ('.$cols.') VALUES ('.$vals.')');
}

?>
