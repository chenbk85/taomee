<?php

$g_base_keys = array('uid', 'cmdid');
$g_option_keys = array('ownerid', 'filename', 'albumid', 'photoid', 'start_index', 'req_count', 'author', 'order_type');

$g_addpic_keys = array('filename', 'albumid');
$g_reqpic_keys = array('ownerid', 'albumid', 'start_index', 'req_count');	// request pic list from photobook
$g_reqwall_keys = array('order_type', 'start_index', 'req_count');	// request pic list from wall
$g_search_wall_keys = array('ownerid');
$g_delpic_keys = array('ownerid', 'photoid');


// cmd id list
define(G_CMD_ADDPIC, 1);
define(G_CMD_REQPIC, 2);
define(G_CMD_DELPIC, 3);

// error list
define(ERR_SESSION, 1);
define(ERR_UPLOAD, 100);
define(ERR_NO_KEY, 101);
define(ERR_NO_IMAGE, 102);
define(ERR_NOT_IMAGE, 103);
define(ERR_CMD_NOT_SUPPORT, 104);

define(ERR_DB, 200);

define(ERR_SAVE_FILE, 300);
?>
