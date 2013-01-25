<?php

ini_set("memory_limit","256M");
ini_set("max_execution_time","60");

require_once ('conf.php');
require_once ('comm.php');

$R = $_REQUEST;
$SVR_IP=$_SERVER[ 'SERVER_ADDR'];

/* Usage: QUERY-URL?svc=xxx&query_cmd=xxx&start=yyyy-mm-dd&end=yyyy-mm-dd&uid=xxx&op=xxx&oper=xxx&reason=xxx&page=xxx&page_size=xxx */
$svc=strtolower($R['svc']);
$query_cmd=$R['query_cmd'];
$start=$R['start'];
$end=$R['end'];
$uid=empty($R['uid']) ? 0 : $R['uid'];
$op=empty($R['op']) ? 0 : $R['op'];
$oper=empty($R['oper']) ? "NA" : $R['oper'];
$reason=empty($R['reason']) ? "NA" : $R['reason'];


$result['result'] = 0;
if (empty($R['page_size'])
	|| is_valid_page_size($R['page_size']) == FALSE) {
		$page_size=$CONF['def_page_size'] + 0;
	} else {
		$page_size=$R['page_size'] + 0;
	}

if (is_supported_svc($svc) == FALSE) {
	$result['result'] = ERR_UNSPSVC;
	$result['err_desc'] = "Unsupported service: $svc";
	$json_result = json_encode($result);
	echo "$json_result";
	return ;
}

if ($query_cmd == $CMD_LIST['query_user_op']) {
	$startsec=strtotime($start);
	if ($startsec == FALSE) {
		$result['result'] = ERR_INVSTART;
		$result['err_desc'] = "Invalid start-time: $start";
		$json_result = json_encode($result);
		echo "$json_result";
		return ;
	}
	$endsec=strtotime($end);
	if ($endsec == FALSE) {
		$result['result'] = ERR_INVEND;
		$result['err_desc'] = "Invalid end-time: $end";
		$json_result = json_encode($result);
		echo "$json_result";
		return ;
	}

	if ($startsec > $endsec) {
		$tmp=$startsec;
		$startsec=$endsec;
		$endsec=$tmp;
	}

	$now=date('Y-m-d H:i:s');
	$nowsec=strtotime($now);
	$search_time_limit=$nowsec;
	if ($startsec > $search_time_limit) {
		$result['result'] = ERR_STARTOVERNOW;
		$result['err_desc'] = "start-time over now: start=$start, now=$now";
		$json_result = json_encode($result);
		echo "$json_result";
		return ;
	}
	if ($endsec > $search_time_limit)
		$endsec=$search_time_limit;
}


function db_init(&$result, &$mysqli, $host, $user, $pass, $db)
{   
	$mysqli = new mysqli($host, $user, $pass, $db);                                    
	if(mysqli_connect_errno()) {
		$result['result'] = ERR_MYCONN;
		$result['err_desc'] = mysqli_connect_error();
		return false;                                                                  
	}
	/* change character set to utf8 */ 
	if (!$mysqli->set_charset("utf8")) {
		$result['result'] = ERR_MYSETUTF8;
		$result['err_desc'] = $mysqli->error;
		return false;                                                                  
	}
	return true;                                                                       
}                                                                                      

function db_finish(&$mysqli)                                                           
{   
	$mysqli->close();                                                                  
}

function do_insert_user_op(&$result, &$mysqli, $svc, $uid, $op, $oper, $reason)
{
	global $OP_TAB;
	global $OP_INST_FDS;
	$sql = "INSERT INTO $OP_TAB ($OP_INST_FDS) VALUES ('$svc', '$uid', '$op', '$oper', '$reason')";
	echo $sql;
	if (!$mysqli->query($sql)) {
		$result['result'] = ERR_MYINSRT;
		$result['err_desc'] = $mysqli->error;
	}
}

function do_query_user_op(&$result, &$mysqli, $svc, $uid, $op, $oper, $start, $end, $page, $page_size)
{
	global $OP_TAB;

	if ($uid == 0) {
		$uid_in_where = "true";
	} else {
		$uid_in_where = "uid = '$uid'";
	}

	if ($op == 0) {
		$op_in_where = "true";
	} else {
		$op_in_where = "op = '$op'";
	}

	$sql_cnt="SELECT count(*) AS tot_cnt FROM $OP_TAB WHERE svc='$svc' and op_time BETWEEN '$start' and '$end' and $uid_in_where and $op_in_where";
	if ($mysql_result = $mysqli->query($sql_cnt)) {
		$row = $mysql_result->fetch_array(MYSQLI_ASSOC);
		$result['total_count'] = $row['tot_cnt'];
	} else {
		$result['result'] = ERR_MYQUERY1;
		$result['err_desc'] = $mysqli->error;
		return ;
	}

	if (!empty($page)) {
		$currentPage = intval($page);
		if (empty($currentPage) || $currentPage < 1) {$currentPage = 1;}

		$currentOffset = ($currentPage -1)* $page_size;

		if ($currentOffset > ($result['total_count'] - $page_size) ) {
			$result['result'] = ERR_OVERRESULT;
			$result['err_desc'] = "Only ".$result['total_count']." results";
			return ;
		}
	} else {
		$currentPage = 1;
		$currentOffset = 0;
	}
	$limit_start = $currentOffset;
	$limit_count = $page_size;

	$sql="SELECT * FROM $OP_TAB WHERE svc='$svc' and op_time BETWEEN '$start' and '$end' and $uid_in_where and $op_in_where LIMIT $limit_start, $limit_count";
	if ($mysql_result = $mysqli->query($sql)) {
		$result['out'] = array ();
		$n = 0;
		while ($row = $mysql_result->fetch_array(MYSQLI_ASSOC)) {
			$out = & $result['out'];
			$out[$n] = array();
			$out_item = & $out[$n];
			$out_item['userid'] = $row['uid'];
			$out_item['time'] = $row['op_time'];
			$out_item['opt'] = $row['op'];
			$out_item['operator'] = $row['oper'];
			$out_item['reason'] = $row['reason'];
			$n++;
		}
	} else {
		$result['result'] = ERR_MYQUERY2;
		$result['err_desc'] = $mysqli->error;
	}
}

/* result to client */
$mysqli = NULL;
$dbname = $OPDB_CONF['dbname'];
$host = $OPDB_CONF['my_host'];
$user = $OPDB_CONF['my_user'];
$pass = $OPDB_CONF['my_passwd'];
if (db_init($result, $mysqli, $host, $user, $pass, $dbname) == FALSE) {
	$json_result = json_encode($result);
	print "$json_result";
	return ;
}

switch ($query_cmd) {
case $CMD_LIST['update_user_op']:
	do_insert_user_op($result, $mysqli, $svc, $uid, $op, $oper, $reason);
	break;

case $CMD_LIST['query_user_op']:
	do_query_user_op($result, $mysqli, $svc, $uid, $op, $oper, $start, $end, $page, $page_size);
	break;

default:
	$result['result'] = ERR_UNSPCMD;
	$result['err_desc'] = "Unsupported query_cmd: $query_cmd";
	break;
}

$json_result = json_encode($result);
print "$json_result";

?>
