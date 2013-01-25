<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">

<?php

ini_set("memory_limit","256M");
ini_set("max_execution_time","60");

$R = $_REQUEST;
$q = isset($R['query']) ? $R['query'] : '';
//$q = preg_replace('/ OR /', ' | ', $q);
//$q = preg_replace('/[^\w~\|\(\)"\/=-]+/', ' ', trim(strtolower($q)));
if (!empty($q)) require_once("sphinxapi.php");

require_once ('conf.php');
require_once ('comm.php');

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
	//Get param from http request
	$svc=$R['svc'];
	$start=$R['start'];
	$end=$R['end'];

	if (empty($R['page_size'])
		|| is_valid_page_size($R['page_size']) == FALSE) {
		$page_size=$CONF['page_size'];
	} else {
		$page_size=$R['page_size'] + 0;
	}

	if (empty($R['svrtype']))
		$svrtype=array();
	else
		$svrtype=array_map('trim', explode(',', $R['svrtype']));
	if (empty($R['logtype']))
		$logtype=array();
	else
		$logtype=array_map('trim', explode(',', $R['logtype']));

	if (empty($R['gamezone']))
		$gamezone=array();
	else
		$gamezone=array_map('trim', explode(',', $R['gamezone']));

	if (empty($R['ip']))
		$ip=array();
	else
		$ip=array_map('trim', explode(',', $R['ip']));

	if (empty($R['svrid']))
		$svrid=array();
	else
		$svrid=array_map('trim', explode(',', $R['svrid']));

	$reqmode=$R['reqmode'];
?>
<html>
	<head>
		<title>Taomee-raptor</title>
		<link rel="stylesheet" type="text/css" href="search_client.css" />

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

			Service:<br/><select id='service' name="svc" size="1" >
				<option <? if (htmlentities($svc) == "mole") echo "selected=\"selected\"" ?> value="mole">mole
				<option <? if (htmlentities($svc) == "seer") echo "selected=\"selected\"" ?> value="seer">seer
				<option <? if (htmlentities($svc) == "xhx") echo "selected=\"selected\"" ?> value="xhx">xhx
				<option <? if (htmlentities($svc) == "gf") echo "selected=\"selected\"" ?> value="gf">gf
				<option <? if (htmlentities($svc) == "hero") echo "selected=\"selected\"" ?> value="hero">hero
				<option <? if (htmlentities($svc) == "saier2") echo "selected=\"selected\"" ?> value="saier2">seer2
			</select>

			<br /> <br />

			From:<br /><input id="start-time" name="start" type="text" size="18" maxlength="27" value="<? echo htmlentities($start); ?>" >
			<br />
			To:<br /><input id="end-time" name="end" type="text" size="18" maxlength="27" value="<? echo htmlentities($end); ?>" >

			<br /> <br />

			Results Per Page:<br/><select id='page_size' name="page_size" size="1" >
				<option <? if (htmlentities($page_size) == "50") echo "selected=\"selected\"" ?> value="50">50
				<option <? if (htmlentities($page_size) == "100") echo "selected=\"selected\"" ?> value="100">100
				<option <? if (htmlentities($page_size) == "1000") echo "selected=\"selected\"" ?> value="1000">1000
			</select>

			<br /> <br />

			GamezoneID:<br/><input id="gamezone" name="gamezone" type="text" size="18" maxlength="11" value="<? echo htmlentities($R['gamezone']); ?>" />

			<br /> <br />

			SvrID:<br/><input id="svrid" name="svrid" type="text" size="18" maxlength="24" value="<? echo htmlentities($R['svrid']); ?>" />

			<br /> <br />

			Svrtype:<br/><input id='svrtype' name="svrtype" type="text" size="18" maxlength="64" value="<? echo htmlentities($R['svrtype']); ?>" />

			<br /> <br />

			Logtype:<br/><input id='logtype' name="logtype" type="text" size="18" maxlength="64" value="<? echo htmlentities($R['logtype']); ?>" />

			<br /> <br />

			SvrIP:<br/><input id="svrip" name="ip" type="text" size="18" maxlength="128" value="<? echo htmlentities($R['ip']); ?>" />

			<br /> <br />

			MatchMode:<br/><select id='reqmode' name="reqmode" size="1" >
				<option value="">Auto
				<option value="all">Match all
				<option value="any">Match any
				<option value="phrase">Match exact phrase
				<option value="bool">Match in bool mode
				<option value="ext">Match in ext mode
				<option value="ext2">Match in ext2 mode
			</select>

			<br /> <br />

			<hr style="border: 1px inset #A8A8A8; width: 90%" align="left" />

			<br /> <br />
			<a href="http://192.168.11.115/search/get_uidlist.php" align="right" target="_blank">get_uidlist</a>
</div>

<div id="searchhead_left">
			<img src="./images/search_logo.png" width="128" height="96" alt="Taomee Log Search System" />
</div>

<div id="searchhead">
			<input id="query" name="query" type="text" size="64" maxlength="64" value="<? echo htmlentities($q); ?>"/>

			<input type="submit" value="Taomee Search" id="submit" name="submit"/>

			<a href="http://dev.taomee.com/index.php/%E6%97%A5%E5%BF%97%E5%B9%B3%E5%8F%B0%E6%90%9C%E7%B4%A2%E5%AE%A2%E6%88%B7%E7%AB%AF%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E" align="right" target="_blank">help</a>
		</form>
</div>

<div id="bodycenter" >
<?php
#might need to put in path to your file
//if (!empty($q)) require_once("sphinxapi.php");

$omit_gamezone=TRUE;
$invalid_gamezone="";

$omit_svrtype=TRUE;
$invalid_svrtype="";

$omit_logtype=TRUE;
$invalid_logtype="";

$omit_svrid=TRUE;
$invalid_svrid=0;

$omit_ip=TRUE;
$invalid_ip=0;

//If the user entered something
if (!empty($q)) {
	if (is_supported_svc($svc) == FALSE) {
		echo "Unsupported service: $svc";
		return ;
	}
	if (is_supported_gamezone($svc, $gamezone, $omit_gamezone, $invalid_gamezone) == FALSE) {
		echo "Unsupported gamezone($invalid_gamezone) for service: $svc";
		return ;
	}
	if (is_supported_svrtype($svrtype, $omit_svrtype, $invalid_svrtype) == FALSE) {
		echo "Unsupported svrtype($invalid_svrtype) for service: $svc";
		return ;
	}
	if (is_supported_logtype($logtype, $omit_logtype, $invalid_logtype) == FALSE) {
		echo "Unsupported logtype: $invalid_logtype";
		return ;
	}
	if (is_valid_req_svrid($svrid, $omit_svrid, $invalid_svrid) == FALSE) {
		echo "SvrID MUST be pure-numeric, invalid input: $invalid_svrid";
		return ;
	}
	if (is_valid_req_ip($ip, $omit_ip, $invalid_ip) == FALSE) {
		echo "Invalid IP: $invalid_ip";
		return ;
	}
	if (!empty($reqmode) && is_supported_match_mode($reqmode) == FALSE) {
		echo "Unsupported request match_mode: $reqmode";
		return ;
	}

	$dist_search_host=$CONF['svc_dist_searchd_ip'][$svc];
	$DB=$svc;
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

	$start_date=date('Ymd', $startsec);
	$end_date=date('Ymd', $endsec);
	if ($start_date != $end_date) {
		echo "start-time should be the same day with end-time: start=$start, end=$end";
		return ;
	}

	$cur_search="[$now] Cli: $query_client_ip, Svc: $svc, QueryTime: $start - $end, Query: \"$q\"";
	do_log($cur_search);

	$daysec=strtotime($start_date." 00:00:00");
	$day_off=$daysec / 86400;
	//echo "startsec: ".$startsec."day_off: ".$day_off;
	$port_off=$day_off % MAX_RETAIN_DAY;
	$search_port=$CONF['svc_dist_searchd_port'][$svc] + $port_off;

	$search_index=get_best_fit_index($start_date, $startsec, $endsec);

	//default value
	$filterrange= "logtime";
	$filterrangemin= $startsec;
	$filterrangemax= $endsec;
	$sortby = "logtime ASC, @id ASC";
	$ranker = SPH_RANK_NONE;

	//Choose an appriate mode (depending on the query)
//	if (strpos($q,'~') === 0) {
//		$q = preg_replace('/^\~/','',$q);
//		if (substr_count($q,' ') > 1) //over 2 words
//			$mode = SPH_MATCH_ANY;
//	} elseif (preg_match('/[\|\(\)"\/=-]/',$q)) {
//		$mode = SPH_MATCH_EXTENDED2;
//	}
	if (!empty($reqmode)) {
		$mode = $CONF['match_mode_id_map'][$reqmode];
	} else {
		$mode = SPH_MATCH_ALL;
	}

    //produce a version for display
    $qo = $q;
    if (strlen($qo) > 64) {
        $qo = '--too many query words (>64)--';
    } 

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
	$cl->SetSortMode(SPH_SORT_EXTENDED, $sortby);
	//do_log("=== mode=".$mode);
	$cl->SetMatchMode($mode);
	$cl->SetRankingMode ($ranker);
	$cl->SetFilterRange($filterrange, $filterrangemin, $filterrangemax);
	//current page and number of results
	$cl->SetLimits($currentOffset, $page_size, $CONF['max_matches']); 

	/* 根据参数设置 filter */
	if ($omit_svrtype == FALSE) {
		$count=count($svrtype);
		$svrtype_id = array();
		for ($i = 0; $i < $count; $i++) {
			$svrtype_id[$i] = $CONF['svrtype_id_map'][$svrtype[$i]];
		}
		$cl->Setfilter('svrtype', $svrtype_id);
	}
	if ($omit_logtype == FALSE) {
		$count=count($logtype);
		$logtype_id = array();
		for ($i = 0; $i < $count; $i++) {
			$logtype_id[$i] = $CONF['logtype_id_map'][$logtype[$i]];
		}
		$cl->Setfilter('logtype', $logtype_id);
	}
	if ($omit_gamezone == FALSE) {
		$cl->Setfilter('gamezone', $gamezone);
	}
	if ($omit_svrid == FALSE) {
		$cl->Setfilter('svrid', $svrid);
	}
	if ($omit_ip == FALSE) {
		$count=count($ip);
		$iplong = array();
		for ($i = 0; $i < $count; $i++) {
			$iplong[$i] = ip2long($ip[$i]);
		}
		$cl->Setfilter('ip', $iplong);
	}


	$res = $cl->Query($q, $search_index);

	//Check for failure
	if (empty($res)) {
		print "Query Failed: -- Maybe the data is not stored (Try closer Start and End time).\n";
		if ($CONF['debug'] && $cl->GetLastError())
			print "<br/>Error: ".$cl->GetLastError()."\n\n";
		return;
	} else {
		//We have results to display!
		if ($CONF['debug'] && $cl->GetLastWarning())
			print "<br/>WARNING: ".$cl->GetLastWarning()."\n\n";
		$query_info = "Query '<b>".htmlentities($qo)."</b>' retrieved ".count($res['matches'])." of $res[total_found] matches in $res[time] sec.\n";

		$resultCount = $res['total_found'];
		$numberOfPages = ceil($res['total']/$page_size);
	}

	if (is_array($res["matches"])) {
		echo "<pre>\n";
		if ( $cl->GetLastWarning() )
			print "WARNING: " . $cl->GetLastWarning() . "\n\n";

		//print "Query '<b>$q</b>' retrieved $res[total] of $res[total_found] matches in $res[time] sec.\n";
		print $query_info;
		print "Query stats(From <b>".date('Y-m-d H:i:s', $startsec)."</b> to <b>".date('Y-m-d H:i:s', $endsec)."</b>, mode=<b>".$CONF['match_mode_name_map'][$mode]."</b>):\n";
		if ( is_array($res["words"]) )
			foreach ( $res["words"] as $word => $info )
				print "    '$word' found $info[hits] times in $info[docs] documents\n";
		print "\n";

		$page_param = array (
			'page_size' => $page_size,
			'currentOffset' => $currentOffset,
			'currentPage' => $currentPage,
			'resultCount' => $resultCount,
			'numberOfPages' => $numberOfPages,
			'query_info' => $query_info,
			'qo' => $qo,
			);
		query_log($cl, $res, $svc, $search_index, $q, $page_param);
		echo "</pre>";
	} else {
		print "<pre class=\"results\">No Results for '".htmlentities($qo)."'</pre>";
	}
}

?>

</div>

<?php
function query_log(&$cl, &$res, $svc, $search_index, &$q, &$page_param)
{
	global $CONF;
	global $DB_CONF;
	$user = $DB_CONF['user'];
	$pass = $DB_CONF['pass'];
	$DB = $svc;
	$dbconns = array ();
	$tables = array ();
	$docs = array();

	print "\nMatches:\n";
	print "<ol class=\"results\" start=\"".($page_param['currentOffset']+1)."\">";
	foreach ( $res["matches"] as $docinfo ) {
		$storeip = $docinfo['attrs']['storeip'];
		if (!isset($dbconns[$storeip])) {
			/* 还没有建立的mysql连接 */
			$ip_str = long2ip($storeip);
			if (db_init($dbconns[$storeip], $ip_str, $user, $pass, $svc) == FALSE) {
				$log="Failed to connect to DB: " . $storeip . "(". $ip_str . ")";
				//do_log($log);
				$err['db_init'] = $DB;
				$err['desc'] = $log;
				echo(var_export($err,true));
				return ;
			}
		}

		$mysqli = $dbconns[$storeip];
		$ID = $docinfo[id]; // high32="Ymmdd{15No:03d}${slicenum:02d}"
		if (!isset($tables[$ID])) {
			$TABLE = get_tablename_by_id($ID);
			$tables[$ID] = $TABLE;
		} else {
			$TABLE = $tables[$ID];
		}
		$sql = str_replace('$TABLE', $TABLE, $DB_CONF['sql_query']);
		$sql = str_replace('$ID', $ID, $sql);
		if ($mysql_result = $mysqli->query($sql)) {
			if($mysql_result->num_rows > 0) {
				$row = $mysql_result->fetch_array(MYSQLI_ASSOC);

				print "<li>";
				//print "id=$docinfo[id] | ";
				/* meta_info */
				foreach ( $res["attrs"] as $attrname => $attrtype ) {
					$value = $docinfo["attrs"][$attrname];
					if ( $attrtype & SPH_ATTR_MULTI ) {
						$value = "(" . join ( ",", $value ) .")";
					} else {
						if ( $attrtype==SPH_ATTR_TIMESTAMP )
							$value = date ( "Y-m-d H:i:s", $value );
						if ( $attrname == "ip" || $attrname == "storeip" )
							$value = long2ip($value);
					}
					print "$attrname=$value | ";
				}
				/* show log */
				echo "[".$row['logtime']."]\t".$row['logline']."</li>";
			} else {
				//Error Message
				print "<pre class=\"results\">Unable to get results for '".htmlentities($page_param['qo'])."'</pre>";
			}
		} else {
			printf("Failed to query log for ID: $ID, TABLE: $TABLE, STOREIP:$storeip mysqlerr: %s\n", $mysqli->error);
			continue;
		}
		print "\n";
	}
	print "</ol>";

	if ($page_param['numberOfPages'] > 1) {
		print "<p class='pages'>Page " . $page_param['currentPage'] . " of " . $page_param['numberOfPages'] . ". ";
		printf("Result %d..%d of %d. ",($page_param['currentOffset'])+1,min(($page_param['currentOffset'])+$page_param['page_size'],$page_param['resultCount']),$page_param['resultCount']);
		print pagesString($page_param['currentPage'], $page_param['numberOfPages'])."</p>";

		print "<pre class=\"results\">".$page_param['query_info']."</pre>";

	} else {
		print "<pre class=\"results\">".$page_param['query_info']."</pre>";
	}
	finish_dbconns($dbconns);
}


?>
	</body>

</html>
