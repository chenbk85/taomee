<?php
ini_set('include_path', ini_get('include_path') . ':./Rmail/');
define("LOG_DIR", "./log/");
define("TEST_VERSION", "0");
define("LOG_PREFIX", "oa_alarm_");

//字符串长度限制
define('MOBILE_MIN_LEN', 11);
define('EMAIL_MIN_LEN', 11);
define('RTX_MIN_LEN', 1);
define('MSG_MIN_LEN', 10);
define('MOBILE_MSG_MAX_LEN', 69);

//命令
define('COMMAND_SEND_TO_MOBILE',10001);
define('COMMAND_SEND_TO_EMAIL', 10002);
define('COMMAND_SEND_TO_RTX',   10003);

//命令处理成功返回值
define('ALARM_SUCC',   "succ");

//邮件服务器配置
define('SMTP_HOST', 'mail.shidc.taomee.com');
define('SMTP_FROM_EMAIL', 'itl_alarm@taomee.com');
define('EMAIL_TITLE', 'ITL报警信息');

//短信服务器配置
define('MOBILE_MESSAGE_URL', 'http://192.168.6.19:8080/send_msg?sign=098f6bcd4621d373cade4e832627b4f6&');

//RTX服务器配置
//define('RTX_MESSAGE_URL', 'http://pdc.taomee-ex.com:8012/sendnotify.cgi');
define('RTX_MESSAGE_URL', 'http://10.1.1.4:8012/sendnotify.cgi');
?>
