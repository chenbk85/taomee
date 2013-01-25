SET NAMES UTF8;
USE db_itl;
INSERT IGNORE INTO t_metric_group_info(metric_group_name,time_threshold,time_interval,metric_group_date,default_flag,metric_group_type)\
VALUES('mysql_version',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_uptime',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_socket',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_user_host_priv',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_query_cache',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_innodb_deadlock_event',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_error_log',7200,3600,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_processlist',3600,60,UNIX_TIMESTAMP(NOW()),1,3);

--版本信息
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('version',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_version'),0,0,'version','', UNIX_TIMESTAMP(NOW()),1,3),
('protocol_version',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_version'),0,0,'protocol_version','', UNIX_TIMESTAMP(NOW()),1,3);

--数据库运行时间
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('uptime',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_uptime'),0,0,'uptime','秒', UNIX_TIMESTAMP(NOW()),1,3),
('uptime_since_flush_status',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_uptime'),0,0,'uptime_since_flush_status','秒', UNIX_TIMESTAMP(NOW()),1,3);

--数据库socket 文件
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('socket',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_socket'),0,0,'socket','', UNIX_TIMESTAMP(NOW()),1,3);

--query cache 信息
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('have_query_cache',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_query_cache'),0,0,'have_query_cache','', UNIX_TIMESTAMP(NOW()),1,3),
('query_cache_type',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_query_cache'),0,0,'query_cache_type','', UNIX_TIMESTAMP(NOW()),1,3),
('qcache_hits',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_query_cache'),0,0,'qcache_hits','次', UNIX_TIMESTAMP(NOW()),1,3);

--innodb 死锁事故
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('innodb_deadlock_event',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_innodb_deadlock_event'),0,0,'innodb_deadlock_event','', UNIX_TIMESTAMP(NOW()),1,3);

--错误日志信息
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('last_log_error',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_error_log'),0,0,'last_log_error','', UNIX_TIMESTAMP(NOW()),1,3),
('error_log_size',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_error_log'),0,0,'error_log_size','B', UNIX_TIMESTAMP(NOW()),1,3),
('error_log_access',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_error_log'),0,0,'error_log_access','', UNIX_TIMESTAMP(NOW()),1,3);

--死锁的processlist
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('have_locked_processlist',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_processlist'),0,0,'have_locked_processlist','', UNIX_TIMESTAMP(NOW()),1,3),
('last_locked_processlist',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_processlist'),0,0,'last_locked_processlist','', UNIX_TIMESTAMP(NOW()),1,3);

--数据库权限信息
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('user_host_priv',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_user_host_priv'),0,0,'user_host_priv','', UNIX_TIMESTAMP(NOW()),1,3),
('have_user_priv_host_wildcard',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_user_host_priv'),0,0,'have_user_priv_host_wildcard','', UNIX_TIMESTAMP(NOW()),1,3),
('have_user_priv_root_grant',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_user_host_priv'),0,0,'have_user_priv_root_grant','', UNIX_TIMESTAMP(NOW()),1,3);

UPDATE IGNORE t_metric_info SET plugin_id=(SELECT plugin_id FROM t_plugin_info WHERE plugin_name='libmysqlmaster.so' LIMIT 1) WHERE metric_type=3;
UPDATE IGNORE t_metric_info SET plugin_id=(SELECT plugin_id FROM t_plugin_info WHERE plugin_name='libmysqlslave.so' LIMIT 1) WHERE metric_type =3 AND metric_group_id=(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info');
