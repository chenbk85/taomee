<?php

define ( "ERR_UNSPCMD", 1001 );

define ( "ERR_NODATA", 10001 );
define ( "ERR_UNSPSVC", 10002 );
define ( "ERR_INVSTART", 10003 );
define ( "ERR_INVEND", 10004 );
define ( "ERR_STARTOVERNOW", 10005 );

define ( "ERR_MYCONN", 20001 );
define ( "ERR_MYSETUTF8", 20002 );
define ( "ERR_MYINSRT", 20003 );
define ( "ERR_MYQUERY1", 20004 );
define ( "ERR_MYQUERY2", 20005 );
define ( "ERR_MYFETCHROW", 20006 );
define ( "ERR_OVERRESULT", 20007 );




$CONF = array();
$CONF['debug'] = TRUE;
$CONF['show_attr'] = FALSE;
$CONF['logdir'] = "./log/";
$CONF['accept_ip'] = array (
	"/^10.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}$/",
	"/^172.16.[0-9]{1,3}.[0-9]{1,3}$/",
	"/^192.168.[0-9]{1,3}.[0-9]{1,3}$/",
);

$CONF['valid_svc'] = array ('test', 'mole', 'seer', 'xhx', 'gf', 'hero', 'saier2');

$CONF['svc_dist_searchd_ip'] = array(
	'test' => '10.1.1.32',
	'mole' => '192.168.4.79',
	'seer' => '192.168.4.79',
	'xhx' => '192.168.4.79',
	'gf' => '192.168.4.79',
	'hero' => '192.168.4.79',
	'saier2' => '192.168.4.79',
);
# 注意: 一定要与各 dist_searchd 的配置相同;
$CONF['svc_dist_searchd_port'] = array (
	'test' => 14000,
	'mole' => 14001,
	'seer' => 14002,
	'xhx' => 14003,
	'gf' => 14004,
	'hero' => 14005,
	'saier2' => 14006,
);

# How many results per page
$CONF['def_page_size'] = 20;
$CONF['max_page_size'] = 1000;


# 索引的名称头部字符串
# 注意: 必须与各 dist_searchd 保持相同
$CONF['dist_index_base'] = "_di_index";

# maximum number of results (cached in memory and define the groupby range)
# - should match sphinxes max_matches. default 1000
$CONF['max_matches'] = 1000000;

$CMD_LIST = array (
	'query_summary' => 1000,
	'query_sw_freq' => 1001,
	'query_sw_interval' => 1002,
	'query_tw_freq' => 1003,
	'query_tw_interval' => 1004,
	'update_user_op' => 1005,
	'query_user_op' => 1006,
);

$ATTR_SHOW = array (
	'tw_ill_code' => '不区分cmd违规类型',
	'sw_ill_code' => '区分cmd违规类型',
	'uid' => '米米号',
	'cmd' => '命令号',
	'cmd_recv_sec' => '命令收到时间',
	'tw_last_int' => '不区分cmd间隔',
	'tw_freq' => '不区分cmd频率',
	'sw_last_int' => '区分cmd间隔',
	'sw_freq' => '区分cmd频率',
	'@count' => '违规次数',
);

$OPDB_CONF = array (
	'dbname' => "anticheat",
	'tabname' => "svc_op",
	'my_host' => "10.1.1.32",
	'my_user' => "root",
	'my_passwd' => "ta0mee",
);
$OP_TAB = $OPDB_CONF['dbname'].".".$OPDB_CONF['tabname'];
$OP_INST_FDS = "svc, uid, op, oper, reason";

?>
