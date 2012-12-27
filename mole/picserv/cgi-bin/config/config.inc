<?php


// pic server id list
define(G_PIC_SERVER1_ID, 201);
define(G_PIC_SERVER2_ID, 202);

// static pic server
define(G_DISPLAY_WALL_SERVER_ID, G_PIC_SERVER1_ID);
define(G_SERVER_ID, G_PIC_SERVER1_ID);

$g_server_map = array( G_PIC_SERVER1_ID => "114.80.98.80:80",
					   G_PIC_SERVER2_ID => "114.80.98.81:80");

// proxy server
$g_db_proxy_map = array('ip' => '192.168.0.84',
						'port' => 11001);


// dir configure
define(PICSERVER_ROOT, dirname(__FILE__).'/../../');
define(SCREENSHOT_TMP_DIR, PICSERVER_ROOT.'/screenshot_tmp_upload/');
define(PICTURE_UPLOAD_DIR, PICSERVER_ROOT.'/mole_pictures/');

define(DISPLAY_WALL_DIR, PICSERVER_ROOT.'/display_wall/');

define(PICSERVER_LOG_DIR, PICSERVER_ROOT.'/log/');
define(SCREENSHOT_LOG_FILE, 'screenshot_upload.error');
define(PHOTOBOOK_LOG_FILE, 'photobook.error');

define(LOG_FILE, PHOTOBOOK_LOG_FILE);



?>
