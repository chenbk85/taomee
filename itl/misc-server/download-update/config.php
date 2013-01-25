<?php
ini_set('include_path', ini_get('include_path') . ':./third-party/Rmail/');

define('LOG_DIR', './log/');                                 //日志目录
define('LOG_PREFIX', 'oa_update_');                          //日志文件前缀
define('LOCK_FILE', 'daemon.pid');                      

//define('UPDATE_URL', 'http://10.1.1.27/jackycao_project/taomee-service/itl_service/webroot/update.php');             // 请求下载更新的URL地址
define('UPDATE_URL', 'http://10.1.1.27/itl/webroot/update.php');             // 请求下载更新的URL地址
define('UPDATE_PATH', '/var/www/oa-auto-update/');                     // oa后台处理更新的web服务的路径
define('UPDATE_FILE_NAME', 'update_cmd_list');                     // oa后台处理更新的web服务的路径
define('SIGN_KEY', 'monitor');                     // oa后台处理更新的web服务的路径

define('REPORT_ERROR', 'true');                              //是否发邮件汇报错误
//define('SMTP_HOST', '192.168.0.8');                          //SMTP_HOST
define('SMTP_HOST', 'mail.shidc.taomee.com');
define('SMTP_FROM_EMAIL', 'tonyliu@taomee.com');                //发件人EMAIL
//define('SMTP_TO_EMAIL', 'luis@taomee.com, tonyliu@taomee.com, mason@taomee.com, sevenchen@taomee.com, jackycao@taomee.com');        //收件人EMAIL
define('SMTP_TO_EMAIL', 'luis@taomee.com,mason@taomee.com,tonyliu@taomee.com');        //收件人EMAIL
?>
