SET NAMES UTF8;
USE db_itl;

--删除t_ds_info
DROP TABLE IF EXISTS t_ds_info;

--删除t_mysql_info
DROP TABLE IF EXISTS t_mysql_info;

--删除cluster信息的视图
DROP VIEW IF EXISTS v_cluster_info;

--修改t_mysql_locked_processlist
ALTER TABLE t_mysql_locked_processlist ADD COLUMN process_user VARCHAR(128) NOT NULL DEFAULT '';
ALTER TABLE t_mysql_locked_processlist ADD COLUMN process_time INT UNSIGNED NOT NULL DEFAULT 0; 
ALTER TABLE t_mysql_locked_processlist ADD COLUMN add_time INT UNSIGNED NOT NULL DEFAULT 0; 

--删除报警的历史数据
TRUNCATE TABLE t_alarm_log_info;
TRUNCATE TABLE t_alarm_event_info;

--删除n多无用的触发器
DROP TRIGGER IF EXISTS tr_business_info_add;
DROP TRIGGER IF EXISTS tr_business_info;
DROP TRIGGER IF EXISTS tr_business_info_del;
DROP TRIGGER IF EXISTS tr_server_info_alt;
DROP TRIGGER IF EXISTS tr_project_info;
DROP TRIGGER IF EXISTS tr_node_info_del;
DROP TRIGGER IF EXISTS tr_node_info_alt;
DROP TRIGGER IF EXISTS tr_node_info_add;
DROP TRIGGER IF EXISTS tr_head_info_del;
DROP TRIGGER IF EXISTS tr_project_business_add;
DROP TRIGGER IF EXISTS tr_project_business_del;

--修改t_host_info
DROP TABLE IF EXISTS t_host_info;
CREATE TABLE IF NOT EXISTS t_host_info( 
host_name   VARCHAR(127) NOT NULL DEFAULT '' COMMENT "server tag",
metric_name VARCHAR(127) NOT NULL DEFAULT '' COMMENT "metric name",
metric_arg  VARCHAR(255) NOT NULL DEFAULT '' COMMENT "metric argument",
metric_val  VARCHAR(255) NOT NULL DEFAULT '' COMMENT "metric value",
PRIMARY KEY(host_name,metric_name,metric_arg),
INDEX(host_name,metric_name,metric_arg),
INDEX(metric_val)
)ENGINE = INNODB,DEFAULT CHARSET = UTF8;

--创建grid信息的视图
CREATE OR REPLACE VIEW v_grid_info AS 
SELECT a.segment_id AS grid_id,a.ip_inside AS listen_ip,a.listen_port AS listen_port,a.summary_interval AS summary_interval,
c.url AS alarm_server_url,a.trust_hosts AS trust_host,a.rrd_rootdir AS rrd_rootdir,a.export_hosts AS export_hosts,
e.is_updated AS update_status
FROM t_network_segment_info a,t_url_info c,t_head_status e WHERE a.segment_id=e.head_id AND a.url_id=c.url_id;

delimiter :
--将网段信息同步到t_head_status表中
DROP TRIGGER IF EXISTS tr_head_info_sync:
CREATE TRIGGER tr_head_info_sync AFTER INSERT ON t_network_segment_info 
FOR EACH ROW
BEGIN
INSERT IGNORE INTO t_head_status(head_id) VALUES(NEW.segment_id);
END :

--t_host_info的mysql信息同步到t_mysql_error_log_info,t_mysql_locked_processlist,t_mysql_innodb_deadlock_event
DROP TRIGGER IF EXISTS tr_host_to_mysql_add:
CREATE TRIGGER tr_host_to_mysql_add AFTER INSERT ON t_host_info 
FOR EACH ROW
BEGIN
IF(NEW.metric_name='last_log_error') THEN
INSERT IGNORE INTO t_mysql_error_log_info(mysql_host, mysql_port, line_num, error_time, error_content)\
VALUES(NEW.host_name, NEW.metric_arg, SUBSTRING_INDEX(NEW.metric_val,':',1), \
SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val,':',2),':',-1),\
RIGHT(NEW.metric_val, length(NEW.metric_val) - length(SUBSTRING_INDEX(NEW.metric_val,':',2)) - 1));
END IF;
IF(NEW.metric_name='last_locked_processlist') THEN
INSERT IGNORE INTO t_mysql_locked_processlist(add_time,mysql_host,mysql_port,process_id,process_user,process_host,process_time,process_info) \
VALUES(UNIX_TIMESTAMP(NOW()),NEW.host_name, NEW.metric_arg,\
SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-5),';',1),\
SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-4),';',1),\
SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-3),';',1),\
SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-2),';',1),\
SUBSTRING_INDEX(NEW.metric_val ,';',-1)) ON DUPLICATE KEY UPDATE process_time=SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-2),';',1);
END IF;
IF(NEW.metric_name='innodb_deadlock_event') THEN
INSERT IGNORE INTO t_mysql_innodb_deadlock_event(mysql_host, mysql_port, event_time, event_content) \
VALUES(NEW.host_name, NEW.metric_arg, SUBSTRING_INDEX(NEW.metric_val,':',1),\
RIGHT(NEW.metric_val, length(NEW.metric_val)-length(SUBSTRING_INDEX(NEW.metric_val,':',1))-1));
END IF;
END:

--t_host_info的mysql信息同步到t_mysql_error_log_info,t_mysql_locked_processlist,t_mysql_innodb_deadlock_event
DROP TRIGGER IF EXISTS tr_host_to_mysql_mod:
CREATE TRIGGER tr_host_to_mysql_mod AFTER UPDATE ON t_host_info 
FOR EACH ROW
BEGIN
IF(NEW.metric_name='last_log_error') THEN
INSERT IGNORE INTO t_mysql_error_log_info(mysql_host, mysql_port, line_num, error_time, error_content)\
VALUES(NEW.host_name, NEW.metric_arg, SUBSTRING_INDEX(NEW.metric_val,':',1), \
SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val,':',2),':',-1),\
RIGHT(NEW.metric_val, length(NEW.metric_val)-length(SUBSTRING_INDEX(NEW.metric_val,':',2)) - 1))\
ON DUPLICATE KEY UPDATE error_content=RIGHT(NEW.metric_val, length(NEW.metric_val)-length(SUBSTRING_INDEX(NEW.metric_val,':',2)) - 1);
END IF;
IF(NEW.metric_name='last_locked_processlist') THEN
INSERT IGNORE INTO t_mysql_locked_processlist(add_time,mysql_host,mysql_port,process_id,process_user,process_host,process_time,process_info) \
VALUES(UNIX_TIMESTAMP(NOW()),NEW.host_name, NEW.metric_arg,\
SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-5),';',1),\
SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-4),';',1),\
SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-3),';',1),\
SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-2),';',1),\
SUBSTRING_INDEX(NEW.metric_val ,';',-1)) ON DUPLICATE KEY UPDATE process_time=SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val,';',-2),';',1);
END IF;
IF(NEW.metric_name='innodb_deadlock_event') THEN
INSERT IGNORE INTO t_mysql_innodb_deadlock_event(mysql_host, mysql_port, event_time, event_content) \
VALUES(NEW.host_name, NEW.metric_arg, SUBSTRING_INDEX(NEW.metric_val,':',1),\
RIGHT(NEW.metric_val, length(NEW.metric_val)-length(SUBSTRING_INDEX(NEW.metric_val,':',1))-1))
ON DUPLICATE KEY UPDATE event_content=RIGHT(NEW.metric_val, length(NEW.metric_val)-length(SUBSTRING_INDEX(NEW.metric_val,':',1))-1);
END IF;
END:

--url改变
DROP TRIGGER IF EXISTS tr_url_info:
CREATE TRIGGER tr_url_info AFTER UPDATE ON t_url_info 
FOR EACH ROW 
BEGIN
IF OLD.url!=NEW.url THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id IN(SELECT segment_id FROM t_network_segment_info WHERE url_id=NEW.url_id);
END IF;
END:

--更新网段的head信息
DROP TRIGGER IF EXISTS tr_head_info_update_child:
CREATE TRIGGER tr_head_info_update_child AFTER UPDATE ON t_network_segment_info 
FOR EACH ROW 
BEGIN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=NEW.segment_id;
END:

--增加报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_add:
CREATE TRIGGER tr_alarm_strategy_info_add AFTER INSERT ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT service_type FROM t_alarm_info WHERE alarm_strategy_id=NEW.id) THEN
UPDATE t_head_status SET is_updated='Y',action=action|4 WHERE head_id=-1;
END IF;
END:

--删除报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_del:
CREATE TRIGGER tr_alarm_strategy_info_del AFTER DELETE ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT service_type FROM t_alarm_info WHERE alarm_strategy_id=OLD.id) THEN
UPDATE t_head_status SET is_updated='Y',action=action|4 WHERE head_id=-1;
END IF;
END :

--更新报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_alt:
CREATE TRIGGER tr_alarm_strategy_info_alt AFTER UPDATE ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT service_type FROM t_alarm_info WHERE alarm_strategy_id=OLD.id) THEN
UPDATE t_head_status SET is_updated='Y',action=action|4 WHERE head_id=-1;
END IF;
END :

delimiter ;

