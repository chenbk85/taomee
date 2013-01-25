<?php

ini_set("memory_limit","256M");
ini_set("max_execution_time","60");

require_once("sphinxapi.php");
require_once ('conf.php');
require_once ('comm.php');

$R = $_REQUEST;
$SVR_IP=$_SERVER[ 'SERVER_ADDR'];

$query_client_ip=get_remote_ip();
if (check_remote_ip($query_client_ip) == FALSE) {
?>
<html>
	<head>
		<title>403 Forbidden</title>
	</head>
	<body>
		<h1>Forbidden</h1>
		<p>You don't have permission to access on this server.</p>
	</body>
</html>
<?php
	return ;
}
?>

<?php
	/* Usage: QUERY-URL?svc=xxx&query_cmd=xxx&start=yyyy-mm-dd&end=yyyy-mm-dd&uid=xxx&cmd=xxx&page=xxx&page_size=xxx */
	$svc=strtolower($R['svc']);
	$query_cmd=$R['query_cmd'];
	$start=$R['start'];
	$end=$R['end'];
	$uid=empty($R['uid']) ? 0 : $R['uid'];
	$cmd=empty($R['cmd']) ? 0 : $R['cmd'];

	/* JUST FOR INNER TEST */
	if ($SVR_IP == "10.1.1.32" || $SVR_IP == "10.1.1.155"
		|| $SVR_IP == "10.1.1.122" || $SVR_IP == "10.1.1.154") {
		$svc="test";
	}

/*
	result = array (
		'result' => $ret, // 0 on succ, errcode on fail
		'err_desc' => $desc, // only when result != 0
		'out' => array ( // query_cmd == 1000, only when result == 0
			'total_count' => total_count,
			'out_items' => array (
				array ( ...),
				);
			);

		);
 */
function build_cmd_1000_result_out(&$query_info, &$res, &$result)
{
	$n = 0;
	foreach ( $res["matches"] as $docinfo ) {
		$out = & $result['out'];
		$out[$n] = array();
		$out_item = & $out[$n];

		$out_item['userid'] = $docinfo["attrs"]["uid"];
		$out_item['times'] = $docinfo["attrs"]["@count"];
		$out_item['recvtime'] = date("Y-m-d H:i:s", $docinfo["attrs"]["cmd_recv_sec"]);
		$n++;
	}
}

function build_cmd_1001_result_out(&$query_info, &$res, &$result)
{
	$n = 0;
	foreach ( $res["matches"] as $docinfo ) {
		$out = & $result['out'];
		$out[$n] = array();
		$out_item = & $out[$n];

		$out_item['userid'] = $docinfo["attrs"]["uid"];
		$out_item['cmd'] = $docinfo["attrs"]["cmd"];
		$out_item['freq'] = $docinfo["attrs"]["sw_freq"];
		$out_item['recvtime'] = date("Y-m-d H:i:s", $docinfo["attrs"]["cmd_recv_sec"]);
		if ($query_info['cmd'] == 0) {
			$out_item['times'] = $docinfo["attrs"]["@count"];
		}
		$n++;
	}
}

function build_cmd_1002_result_out(&$query_info, &$res, &$result)
{
	$n = 0;
	foreach ( $res["matches"] as $docinfo ) {
		$out = & $result['out'];
		$out[$n] = array();
		$out_item = & $out[$n];

		$out_item['userid'] = $docinfo["attrs"]["uid"];
		$out_item['cmd'] = $docinfo["attrs"]["cmd"];
		$out_item['interval'] = $docinfo["attrs"]["sw_last_int"];
		$out_item['recvtime'] = date("Y-m-d H:i:s", $docinfo["attrs"]["cmd_recv_sec"]);
		if ($query_info['cmd'] == 0) {
			$out_item['times'] = $docinfo["attrs"]["@count"];
		}
		$n++;
	}
}

function build_cmd_1003_result_out(&$query_info, &$res, &$result)
{
	$n = 0;
	foreach ( $res["matches"] as $docinfo ) {
		$out = & $result['out'];
		$out[$n] = array();
		$out_item = & $out[$n];

		$out_item['userid'] = $docinfo["attrs"]["uid"];
		$out_item['freq'] = $docinfo["attrs"]["tw_freq"];
		$out_item['recvtime'] = date("Y-m-d H:i:s", $docinfo["attrs"]["cmd_recv_sec"]);
		$n++;
	}
}

function build_cmd_1004_result_out(&$query_info, &$res, &$result)
{
	$n = 0;
	foreach ( $res["matches"] as $docinfo ) {
		$out = & $result['out'];
		$out[$n] = array();
		$out_item = & $out[$n];

		$out_item['userid'] = $docinfo["attrs"]["uid"];
		$out_item['interval'] = $docinfo["attrs"]["tw_last_int"];
		$out_item['recvtime'] = date("Y-m-d H:i:s", $docinfo["attrs"]["cmd_recv_sec"]);
		$n++;
	}
}

?>


<?php
	/* result to client */
	$result['result'] = 0;
	$errcode = 0;
	$errmsg;
	$warnmsg;


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

	$search_index=$svc . $CONF['dist_index_base'];
	$search_port=$CONF['svc_dist_searchd_port'][$svc];
	$dist_search_host=$CONF['svc_dist_searchd_ip'][$svc];

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

	$query_info = array (
		'query_client_ip' => $query_client_ip,
		'svc' => $svc,
		'start' => "$start",
		'end' => "$end",
		'query_cmd' => $query_cmd,
		'uid' => $uid,
		'cmd' => $cmd,
		);
	$cur_search="[$now] Cli: $query_client_ip, Svc: $svc, QueryTime: $start - $end, query_cmd: $query_cmd, uid: $uid, cmd: $cmd";
	do_log($cur_search);

	//default value
	$ranker = SPH_RANK_NONE;
	$filterrange= "cmd_recv_sec";
	$filterrangemin= $startsec;
	$filterrangemax= $endsec;
	$sortby = "cmd_recv_sec ASC, @id ASC";

	//setup paging...
	if (!empty($R['page'])) {
		$currentPage = intval($R['page']);
		if (empty($currentPage) || $currentPage < 1) {$currentPage = 1;}

		$currentOffset = ($currentPage -1)* $page_size;

		if ($currentOffset > ($CONF['max_matches']-$page_size) ) {
			die("Only the first {$CONF['max_matches']} results accessible");
		}
	} else {
		$currentPage = 1;
		$currentOffset = 0;
	}

	//Connect to sphinx, and run the query
	$cl = new SphinxClient();
	$cl->SetServer($dist_search_host, $search_port);
	$cl->SetConnectTimeout (2);
	$cl->SetMaxQueryTime(300000);
	$cl->SetArrayResult (true);
	$cl->SetRankingMode ($ranker);
	$cl->SetFilterRange($filterrange, $filterrangemin, $filterrangemax);
	$cl->SetLimits($currentOffset, $page_size, $CONF['max_matches']); 

	/* 设置 filter */
	set_filters($cl, $query_cmd, $uid, $cmd);

	/* 设置 groupby */
	$is_grouped=set_groupby($cl, $query_cmd, $uid, $cmd);

	/* 设置 sortby */
	//$cl->SetSortMode(SPH_SORT_EXTENDED, $sortby);

	/* 设置 select */
	set_select($cl, $query_cmd, $uid, $cmd, $is_grouped);

	$empty_query="";
	$res = $cl->Query($empty_query, $search_index);


	/* build result */
	//Check for failure
	if (empty($res)) {
		$errcode = ERR_NODATA;
		if ($cl->GetLastError())
			$errmsg = $cl->GetLastError();
	} else {
		//We have results to display!
		if ($cl->GetLastWarning())
			$warnmsg = $cl->GetLastWarning();

		$resultCount = $res['total_found'];
		$numberOfPages = ceil($res['total']/$page_size);
	}

	if ($errcode) {
		$result['result'] = $errcode;
		$result['err_desc'] = $errmsg;
		$json_result = json_encode($result);
		echo "$json_result";
		return ;
	}

/*
	$page_param = array (
		'page_size' => $page_size,
		'currentOffset' => $currentOffset,
		'currentPage' => $currentPage,
		'resultCount' => $resultCount,
		'numberOfPages' => $numberOfPages,
		'query_info' => $query_info,
	);
*/
	$result['total_count'] = $resultCount;
	$result['out'] = array ();

	if (is_array($res["matches"])) {
		switch($query_cmd) {
		case $CMD_LIST['query_summary']:
			build_cmd_1000_result_out($query_info, $res, $result);
			break;

		case $CMD_LIST['query_sw_freq']:
			build_cmd_1001_result_out($query_info, $res, $result);
			break;

		case $CMD_LIST['query_sw_interval']:
			build_cmd_1002_result_out($query_info, $res, $result);
			break;

		case $CMD_LIST['query_tw_freq']:
			build_cmd_1003_result_out($query_info, $res, $result);
			break;

		case $CMD_LIST['query_tw_interval']:
			build_cmd_1004_result_out($query_info, $res, $result);
			break;

		default:
			$result['result'] = ERR_UNSPCMD;
			$result['err_desc'] = "Unsupported query_cmd: $query_cmd";
			break;
		}
	}
	$json_result = json_encode($result);
	echo "$json_result";
?>
