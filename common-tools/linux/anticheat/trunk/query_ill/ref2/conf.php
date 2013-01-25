<?php

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
	'mole' => '10.1.1.32',
	'seer' => '10.1.1.32',
	'xhx' => '10.1.1.32',
	'gf' => '10.1.1.32',
	'hero' => '10.1.1.32',
	'saier2' => '10.1.1.32',
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
$CONF['def_page_size'] = 50;
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

?>
