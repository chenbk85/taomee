SET names utf8;
USE db_itl;

DROP TABLE IF EXISTS t_host_info;
CREATE TABLE IF NOT EXISTS t_host_info( 
host_name   VARCHAR(127) NOT NULL DEFAULT '' COMMENT "the server tag",
metric_name VARCHAR(127) NOT NULL DEFAULT '' COMMENT "the metric name",
metric_val  VARCHAR(127) NOT NULL DEFAULT '' COMMENT "the metric value",
PRIMARY KEY(host_name,metric_name),
INDEX(host_name,metric_name)
)ENGINE = INNODB,DEFAULT CHARSET = UTF8;

DROP TABLE IF EXISTS t_switch_interface_info;
CREATE TABLE IF NOT EXISTS t_switch_interface_info(
switch_id INT UNSIGNED NOT NULL COMMENT 'switch id',
if_idx INT UNSIGNED NOT NULL COMMENT 'the interface index', 
if_status TINYINT UNSIGNED NOT NULL COMMENT 'the interface status{1=up|2=down}',
if_speed INT UNSIGNED NOT NULL COMMENT 'the interface speed', 
is_selected TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '是否选中{1=selected|0=unselected}',
if_type VARCHAR(127) NOT NULL COMMENT 'the interface type', 
if_desc VARCHAR(255) NOT NULL DEFAULT '-' COMMENT 'the interface desc(name)',
if_alias VARCHAR(255) NOT NULL COMMENT 'the interface alias',
PRIMARY KEY(switch_id,if_idx)
)ENGINE = INNODB DEFAULT CHARSET = utf8;

DROP TABLE IF EXISTS t_alarm_log_info;
CREATE TABLE IF NOT EXISTS t_alarm_log_info(
id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
alarm_time INT UNSIGNED NOT NULL COMMENT 'the alarm report time', 
alarm_type VARCHAR(127) NOT NULL COMMENT 'the alarm type{warning|critical|host_down|up_failed}',
alarm_way INT UNSIGNED  NOT NULL COMMENT 'the alarm way,1:短信报警,2:email报警,3:email+短信报警,4:rtx报警, 5:rtx+短信报警,6:rtx+email报警,7:rtx+email+短信报警',
alarm_host CHAR(16) NOT NULL COMMENT 'alarm host,当是metric报警和host_down|up_failed时是所在的host的ip',
host_type TINYINT UNSIGNED  NOT NULL COMMENT 'alarm host type{1=server|2=switch}',
alarm_metric VARCHAR(127) NOT NULL COMMENT '如果是metric报警，是metric_name，如果是{host_down|up_failed}时是{host_down_metric|host_upfailed_metric}'
)ENGINE = INNODB DEFAULT CHARSET = utf8;

DROP TABLE IF EXISTS t_alarm_event_info;
CREATE TABLE IF NOT EXISTS t_alarm_event_info(
alarm_start INT UNSIGNED NOT NULL COMMENT '警告事件开始时间', 
alarm_end INT UNSIGNED NOT NULL DEFAULT 0 COMMENT '警告事件结束时间', 
alarm_host CHAR(16) NOT NULL COMMENT 'alarm host,是metric报警和host_down|up_failed时是所在的host的ip',
host_type TINYINT UNSIGNED  NOT NULL COMMENT 'alarm host type{1=server|2=switch}',
alarm_type VARCHAR(63) NOT NULL COMMENT 'the alarm type{warning|critical|host_down|up_failed}',
alarm_metric VARCHAR(127) NOT NULL COMMENT '如果是metric报警，是metric_name，如果是{host_down|up_failed}时是{host_down_metric|host_upfailed_metric}',
alarm_way INT UNSIGNED  NOT NULL COMMENT 'the alarm way,1:短信报警,2:email报警,3:email+短信报警,4:rtx报警, 5:rtx+短信报警,6:rtx+email报警,7:rtx+email+短信报警',
alarm_content VARCHAR(255) NOT NULL COMMENT '报警详情'
)ENGINE = INNODB DEFAULT CHARSET = utf8;

