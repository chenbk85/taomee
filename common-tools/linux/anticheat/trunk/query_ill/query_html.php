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
	$svc=strtolower($R['svc']);
	$query_cmd=$R['query_cmd'];
	$start=$R['start'];
	$end=$R['end'];
	$uid=empty($R['uid']) ? 0 : $R['uid'];
	$cmd=empty($R['cmd']) ? 0 : $R['cmd'];
?>

<html>
	<head>
		<title>Taomee-Anticheat</title>
		<link rel="stylesheet" type="text/css" href="query.css" />

		<link type="text/css" href="timepicker/css/ui-lightness/jquery-ui-1.7.2.custom.css" rel="stylesheet" />
		<script type="text/javascript" src="timepicker/js/jquery-1.3.2.min.js"></script>
		<script type="text/javascript" src="timepicker/js/jquery-ui-1.7.2.custom.min.js"></script>
		<script type="text/javascript" src="timepicker/js/timepicker.js"></script>

		<script type="text/javascript">
		$(function() {
			$('#start-time').datepicker({
				duration: '',
					showTime: true,           //日期控件是否显示时间
					constrainInput: false,
					stepMinutes: 1,
					stepHours: 1,
					altTimeField: '',
					time24h: true //是否是24h制
			});
			$('#end-time').datepicker({
				duration: '',
					showTime: true,           //日期控件是否显示时间
					constrainInput: false,
					stepMinutes: 1,
					stepHours: 1,
					altTimeField: '',
					time24h: true //是否是24h制
			});
		});
		</script>
	</head>

	<body>
<div id="bodyleft" >
		<form action="?" method="get" id="search">

			游戏名称:<br/><select id='service' name="svc" size="1" >
				<option <? if (htmlentities($svc) == "mole" || htmlentities($svc) == "") echo "selected=\"selected\"" ?> value="mole">mole
				<option <? if (htmlentities($svc) == "seer" || htmlentities($svc) == "") echo "selected=\"selected\"" ?> value="seer">seer
			</select>

			<br /> <br />

			开始时间:<br /><input id="start-time" name="start" type="text" size="18" maxlength="27" value="<? echo htmlentities($start); ?>" >
			<br />
			结束时间:<br /><input id="end-time" name="end" type="text" size="18" maxlength="27" value="<? echo htmlentities($end); ?>" >

			<br /> <br />

			每页显示:<br/><select id='page_size' name="page_size" size="1" >
				<option <? if (htmlentities($page_size) == "20") echo "selected=\"selected\"" ?> value="20">20
				<option <? if (htmlentities($page_size) == "50") echo "selected=\"selected\"" ?> value="50">50
				<option <? if (htmlentities($page_size) == "100") echo "selected=\"selected\"" ?> value="100">100
			</select>

			<hr style="border: 1px inset #A8A8A8; width: 90%" align="left" />
</div>

<div id="searchhead_left">
			<img src="./images/search_logo.png" width="128" height="96" alt="Taomee Anticheat System" />
</div>

<div id="searchhead">
			米米号(option): <input id="uid" name="uid" type="text" size="16" maxlength="32" value="<? echo htmlentities($uid); ?>"/>
			命令号(option): <input id="cmd" name="cmd" type="text" size="16" maxlength="32" value="<? echo htmlentities($cmd); ?>"/>

			查找方式:<select id='query_cmd' name="query_cmd" >
				<option <? if (htmlentities($query_cmd) == $CMD_LIST['query_summary']) echo "selected=\"selected\"" ?> value="1000">概要 (按uid分组, 按违规次数从大到小排序)
				<option <? if (htmlentities($query_cmd) == $CMD_LIST['query_sw_freq']) echo "selected=\"selected\"" ?> value="1001">单个命令号频率 (需填 uid + cmd)
				<option <? if (htmlentities($query_cmd) == $CMD_LIST['query_sw_interval']) echo "selected=\"selected\"" ?> value="1002">单个命令号间隔 (需填 uid + cmd)
				<option <? if (htmlentities($query_cmd) == $CMD_LIST['query_tw_freq']) echo "selected=\"selected\"" ?> value="1003">不分命令号频率 (需填uid, 可填cmd)
				<option <? if (htmlentities($query_cmd) == $CMD_LIST['query_tw_interval']) echo "selected=\"selected\"" ?> value="1004">不分命令号间隔 (需填uid, 可填cmd)
			</select>
			<input type="submit" value="Query" id="submit" name="submit"/>

		</form>
</div>

<div id="bodycenter" >


<?php
	if (empty($svc) || empty($query_cmd) || empty($start) || empty($end)) {
		return ;
	}

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
	$page_param = array (
		'page_size' => $page_size,
		'currentOffset' => $currentOffset,
		'currentPage' => $currentPage,
		'resultCount' => $resultCount,
		'numberOfPages' => $numberOfPages,
		'query_info' => $query_info,
	);
/*
	print "<pre>\n<ol class=\"results\" start=\"".($page_param['currentOffset']+1)."\">";
	if (is_array($res["matches"])) {
		foreach ( $res["matches"] as $docinfo ) {
			print "<li>doc_id=$docinfo[id], weight=$docinfo[weight]";
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
			print "</li>";
		}
	} else {
		print "No Result\n";
	}
	print "</lo>";
*/

	//print "\n<ol class=\"results\" start=\"".($page_param['currentOffset']+1)."\">";

	print "\n<table><tr>";
	foreach ($ATTR_SHOW as $attr => $show_str) {
		print "<th>$show_str</th>";
	}
	print "</tr>";

	if (is_array($res["matches"])) {
		foreach ( $res["matches"] as $docinfo ) {
			//print "<li>";
			print "<tr>";
			foreach ( $res["attrs"] as $attrname => $attrtype ) {
				if ($attrname == "@groupby") continue;
				$attr_show = $ATTR_SHOW[$attrname];
				$value = $docinfo["attrs"][$attrname];
				if ( $attrtype==SPH_ATTR_MULTI || $attrtype==SPH_ATTR_MULTI64 ) {
					$value = "(" . join ( ",", $value ) .")";
				} else {
					if ( $attrtype==SPH_ATTR_TIMESTAMP )
						$value = date ( "Y-m-d H:i:s", $value );
				}
				print "<td>$value</td>";
			}
			print "</tr>";
			//print "</li>";
		}
	} else {
		print "No Result\n";
	}
	print "</table>";
	//print "</lo>";

	if ($page_param['numberOfPages'] > 1) {
		print "<p class='pages'>Page " . $page_param['currentPage'] . " of " . $page_param['numberOfPages'] . ". ";
		printf("Result %d..%d of %d. ",($page_param['currentOffset'])+1,min(($page_param['currentOffset'])+$page_param['page_size'],$page_param['resultCount']),$page_param['resultCount']);
		print pagesString($page_param['currentPage'], $page_param['numberOfPages'])."</p>";

		print "<pre class=\"results\">".$page_param['query_info']."</pre>";

	} else {
		print "<pre class=\"results\">".$page_param['query_info']."</pre>";
	}

	print "</pre>";
?>

</div>

</body>

</html>
