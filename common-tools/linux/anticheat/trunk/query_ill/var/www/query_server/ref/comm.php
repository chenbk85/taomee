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

function is_supported_long_svc($svc)
{
	if (empty($svc)) return FALSE;

	global $CONF;
	$count = count($CONF['valid_long_svc']);
	for ($i = 0; $i < $count; $i++ ) {
		if ($CONF['valid_long_svc'][$i] == $svc) return TRUE;
	}
	return FALSE;
}


function is_supported_gamezone($svc, &$gamezone, &$omit_gamezone, &$invalid_gamezone)
{
	/* 注意: 这里进来的 $svc 必须是合法的, 因此不对 $svc 做任何检查; */
	if (empty($gamezone)) return TRUE;

	global $CONF;
	$svc_gamezone = $CONF['svc_gamezone'][$svc];

	$count=count($svc_gamezone);
	$input_count=count($gamezone);
	for ($i = 0; $i < $input_count; $i++) {
		/* 只要有一个 all, 就不需要设置这个 filter 了 */
		if ($gamezone[$i] == "all") return TRUE;

		$found=FALSE;
		for ($j=0; $j < $count; $j++) {
			if ($gamezone[$i] == $svc_gamezone[$j]) {
				$found=TRUE;
				break;
			}
		}
		if ($found==FALSE) {
			/* 只要有一个gamezone在 $svc 所支持的gamezone里找不到, 就是非法的请求 */
			$invalid_gamezone=$gamezone[$i];
			return FALSE;
		}
	}

	/* 到此, 没有遇到一个 all, 但也没有一个非法的 gamezone, 因此后续需要设置该filter */
	$omit_gamezone=FALSE;
	return TRUE;
}

function is_supported_svrtype(&$svrtype, &$omit_svrtype, &$invalid_svrtype)
{
	if (empty($svrtype)) return TRUE;

	global $CONF;
	global $SVRTYPE_COUNT;

	$input_count=count($svrtype);
	for ($i = 0; $i < $input_count; $i++) {
		if ($svrtype[$i] == "all") return TRUE;

		$found=FALSE;
		for ($j=1; $j < $SVRTYPE_COUNT; $j++) {
			if ($CONF['valid_svrtype'][$j] == $svrtype[$i]) {
				$found=TRUE;
				break;
			}
		}
		if ($found==FALSE) {
			$invalid_svrtype=$svrtype[$i];
			return FALSE;
		}
	}

	$omit_svrtype=FALSE;
	return TRUE;
}

function is_supported_logtype(&$logtype, &$omit_logtype, &$invalid_logtype)
{
	if (empty($logtype)) return TRUE;

	global $CONF;
	global $LOGTYPE_COUNT;

	$input_count=count($logtype);
	for ($i = 0; $i < $input_count; $i++) {
		if ($logtype[$i] == "all") return TRUE;

		$found=FALSE;
		for ($j=1; $j < $LOGTYPE_COUNT; $j++) {
			if ($CONF['valid_logtype'][$j] == $logtype[$i]) {
				$found=TRUE;
				break;
			}
		}
		if ($found==FALSE) {
			$invalid_logtype=$logtype[$i];
			return FALSE;
		}
	}

	$omit_logtype=FALSE;
	return TRUE;
}

function is_valid_req_svrid(&$svrid, &$omit_svrid, &$invalid_svrid)
{
	if (empty($svrid)) return TRUE;

	$input_count=count($svrid);
	for ($i = 0; $i < $input_count; $i++) {
		if ($svrid[$i] == "all") return TRUE;
		if (!is_numeric($svrid[$i])) {
			$invalid_svrid=$svrid[$i];
			return FALSE;
		}
	}

	$omit_svrid=FALSE;
	return TRUE;
}

function is_valid_req_ip(&$ip, &$omit_ip, &$invalid_ip)
{
	if (empty($ip)) return TRUE;

	$input_count=count($ip);
	for ($i = 0; $i < $input_count; $i++) {
		if ($ip[$i] == "all") return TRUE;
		if (!preg_match("/^\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3}$/", $ip[$i])) {
			$invalid_ip=$ip[$i];
			return FALSE;
		}
	}

	$omit_ip=FALSE;
	return TRUE;
}

function is_valid_page_size(&$page_size, $is_batch=0)
{
	global $CONF;

	if ($is_batch == 0) {
		for ($i=0; $i < count($CONF['valid_page_size']); $i++) {
			if ($CONF['valid_page_size'][$i] == $page_size) return TRUE;
		}
	} else {
		if ($page_size <= $CONF['max_matches_batch']) return TRUE;
	}

	return FALSE;
}

function is_supported_match_mode($reqmode)
{
	global $CONF;
	global $MATCH_MODE_COUNT;
	for ($i=0; $i < $MATCH_MODE_COUNT; $i++) {
		if ($CONF['valid_match_mode'][$i] == $reqmode) return TRUE;
	}
	return FALSE;
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

function return_403_html()
{
	echo '<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<html><head>
<title>403 Forbidden</title>
</head><body>
<h1>Forbidden</h1>
<p>You don\'t have permission to access /
on this server.</p>
</body></html>
';
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

function db_init(&$mysqli, $host, $user, $pass, $db)
{
    global $DB_CONF;
    $mysqli = new mysqli($host, $user, $pass, $db);
    if(mysqli_connect_errno()) {
        do_log(mysqli_connect_error());
        do_log("Falied to connect storage's mysql server!");
        return false;
    }
    /* change character set to utf8 */
    if (!$mysqli->set_charset("utf8")) {
        $log = sprintf("Error loading character set utf8:%s", $mysqli->error);
        do_log($log);
        return false;
	}
    return true;
}

function db_finish(&$mysqli)
{
    $mysqli->close();
}

function finish_dbconns(&$dbconns)
{
	foreach ($dbconns as $storeip => $mysqli) {
		$mysqli->close();
		//print "Closed mysqli for ".long2ip($storeip)."\n";
	}
}

function get_tablename_by_id($ID)
{
	// $ID format: "Ymmdd{15No:03d}${slicenum:02d}"

	$Y = substr($ID, 0, 1);
	$YYYY = $Y + 2010;
	$mmdd = substr($ID, 1, 4);
	$fifteenNo_03d = substr($ID, 5, 3);
	$slicenum_02d = substr($ID, 8, 2);
	$HHMM_pre= sprintf ("%d", $fifteenNo_03d );
	$HH_pre = $HHMM_pre / 4;
	$MM_pre = ($HHMM_pre % 4) * 15;
	$HH = sprintf("%02d", $HH_pre);
	$MM = sprintf("%02d", $MM_pre);
	$TABLE = $YYYY . $mmdd . $HH . $MM . "_" . $slicenum_02d;
	return $TABLE;
}

function get_best_fit_index($start_date, $startsec, $endsec)
{
	global $CONF;
	global $INDEX_GRANS_COUNT;

	$start_hour=date('G', $startsec);
	$end_hour=date('G', $endsec);

	for ($i = 0; $i < $INDEX_GRANS_COUNT; $i++) {
		$cur_gran = $CONF['index_grans'][$i];
		$seg_count = 24 / $cur_gran;
		for ($seg = 0; $seg < $seg_count; $seg++) {
			if ($start_hour >= $seg * $cur_gran
				&& $end_hour <= ($seg + 1) * $cur_gran - 1) {
				//di20110630_g01_00
				$cur_gran_02d = sprintf("%02d", $cur_gran);
				$cur_gran_seg_02d = sprintf("%02d", $seg * $cur_gran);
				return $CONF['dist_index_head'].$start_date."_g".$cur_gran_02d."_".$cur_gran_seg_02d;
			}
		}
	}

	return $CONF['dist_index_head'].$start_date."_g24_00";
}

function get_svc_logip_list($svcname)
{
	global $CC_CONF;

	$filter_mysqli;
	$host=$CC_CONF['addr'];
	$user=$CC_CONF['user'];
	$pass=$CC_CONF['pass'];
	$db=$svcname.$CC_CONF['suffix'];

	if (db_init(&$filter_mysqli, $host, $user, $pass, $db) == FALSE) {
		$log="Failed to connect to DB: " . $db. "(". $host. ")";
		do_log($log);
		$err['db_init'] = $db;
		$err['desc'] = $log;
		echo(var_export($err,true));
		return FALSE;
	}

	$sql="SELECT ip FROM ".$CC_CONF['t_svrip'].";";
	if ($mysql_result = $filter_mysqli->query($sql)) {
		if($mysql_result->num_rows > 0) {
			for($i=0; $i < $mysql_result->num_rows; $i++) {
				$row = $mysql_result->fetch_array(MYSQLI_NUM);
				if ($i == 0) {
					print $row[0];
				} else {
					print " | $row[0]";
				}
			}
		} else {
			echo "";
		}
	} else {
		printf("Failed($svcname): %s\n", $filter_mysqli->error);
		$filter_mysqli->close();
		return FALSE;
	}

    $filter_mysqli->close();
	return TRUE;
}

function formatBytes($b,$p = null)
{
	/**
	 *
	 * @author Martin Sweeny
	 * @version 2010.0617
	 *
	 * returns formatted number of bytes.
	 * two parameters: the bytes and the precision (optional).
	 * if no precision is set, function will determine clean
	 * result automatically.
	 *
	 * eg:
	 * echo formatBytes(81000000);   //returns 77.25 MB
	 * echo formatBytes(81000000,0); //returns 81,000,000 B
	 * echo formatBytes(81000000,1); //returns 79,102 kB 
	 **/
	$units = array("B","kB","MB","GB","TB","PB","EB","ZB","YB");
	$c=0;
	if(!$p && $p !== 0) {
		foreach($units as $k => $u) {
			if(($b / pow(1024,$k)) >= 1) {
				$r["bytes"] = $b / pow(1024,$k);
				$r["units"] = $u;
				$c++;
			}
		}
		return number_format($r["bytes"],2) . " " . $r["units"];
	} else {
		return number_format($b / pow(1024,$p)) . " " . $units[$p];
	}
}

?>
