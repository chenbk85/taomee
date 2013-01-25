<?php

ini_set("memory_limit","256M");
ini_set("max_execution_time","60");

require_once("sphinxapi.php");
require_once ('conf.php');
require_once ('comm.php');

$R = $_REQUEST;

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
	$svc=$R['svc'];
	$query_cmd=$R['query_cmd'];
	$start=$R['start'];
	$end=$R['end'];
	$uid=empty($R['uid']) ? 0 : $R['uid'];
	$cmd=empty($R['cmd']) ? 0 : $R['cmd'];

	if (empty($R['page_size'])
		|| is_valid_page_size($R['page_size']) == FALSE) {
		$page_size=$CONF['def_page_size'] + 0;
	} else {
		$page_size=$R['page_size'] + 0;
	}

	if (is_supported_svc($svc) == FALSE) {
		echo "Unsupported service: $svc";
		return ;
	}

	$search_index=$svc . $CONF['dist_index_base'];
	$search_port=$CONF['svc_dist_searchd_port'][$svc];
	$dist_search_host=$CONF['svc_dist_searchd_ip'][$svc];

	$startsec=strtotime($start);
	if ($startsec == FALSE) {
		echo "Invalid start-time: $start";
		return ;
	}
	$endsec=strtotime($end);
	if ($endsec == FALSE) {
		echo "Invalid end-time: $end";
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
		echo "start-time over now: start=$start, now=$now";
		return ;
	}
	if ($endsec > $search_time_limit)
		$endsec=$search_time_limit;

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

	//Check for failure
	if (empty($res)) {
		print "Query Failed: -- Maybe the data is not stored (Try closer Start and End time)\n";
		if ($CONF['debug'] && $cl->GetLastError())
			print "Error: ".$cl->GetLastError()."\n\n";
		return;
	} else {
		//We have results to display!
		if ($CONF['debug'] && $cl->GetLastWarning())
			print "WARNING: ".$cl->GetLastWarning()."\n\n";
		$query_info = "Retrieved ".count($res['matches'])." of $res[total_found] matches in $res[time] sec.\n";

		$resultCount = $res['total_found'];
		$numberOfPages = ceil($res['total']/$page_size);
	}

	if ( $cl->GetLastWarning() )
		print "WARNING: " . $cl->GetLastWarning() . "\n\n";
	print $query_info;
	print "Query stats(From ".date('Y-m-d H:i:s', $startsec)." to ".date('Y-m-d H:i:s', $endsec)."\n";

	/* print result */
	if (is_array($res["matches"])) {
		$n = 1;
		foreach ( $res["matches"] as $docinfo ) {
			print "$n. doc_id=$docinfo[id], weight=$docinfo[weight]";
			foreach ( $res["attrs"] as $attrname => $attrtype ) {
				$value = $docinfo["attrs"][$attrname];
				if ( $attrtype==SPH_ATTR_MULTI || $attrtype==SPH_ATTR_MULTI64 ) {
					$value = "(" . join ( ",", $value ) .")";
				} else {
					if ( $attrtype==SPH_ATTR_TIMESTAMP )
						$value = date ( "Y-m-d H:i:s", $value );
				}
				print ", $attrname=$value";
			}
			print "\n";
			$n++;
		}
	} else {
		print "No Result\n";
	}
?>
