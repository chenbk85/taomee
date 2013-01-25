SET NAMES UTF8;
USE db_itl;
INSERT IGNORE INTO t_metric_group_info(metric_group_name,time_threshold,time_interval,metric_group_date,default_flag,metric_group_type)\
VALUES('mysql_binlog_cache',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_bytes_io',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_dml',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_tmp_table',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_innodb_buffer_pool_pages',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_innodb_data',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_instance_alive',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_key_buffer',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_table_locks',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_threads',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_slow_query',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_open_handle',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_sort',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_connection',3600,60,UNIX_TIMESTAMP(NOW()),1,3),
('mysql_slave_info',3600,60,UNIX_TIMESTAMP(NOW()),0,3);
--binlog缓存项统计
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('binlog_cache_disk_use',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_binlog_cache'),0,0,'binlog_cache_disk_use','Byte', UNIX_TIMESTAMP(NOW()),1,3),
('binlog_cache_use',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_binlog_cache'),0,0,'binlog_cache_use','Byte/sec', UNIX_TIMESTAMP(NOW()),1,3);
--数据库与外界交互流量统计
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('bytes_received',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_bytes_io'),0,0,'bytes_received','Byte/sec', UNIX_TIMESTAMP(NOW()),1,3),
('bytes_sent',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_bytes_io'),0,0,'bytes_sent','Byte/sec', UNIX_TIMESTAMP(NOW()),1,3);
--数据库DML语句统计
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('com_call_procedure',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_dml'),0,0,'com_call_procedure','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('com_delete',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_dml'),0,0,'com_delete','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('com_select',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_dml'),0,0,'com_select','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('com_update',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_dml'),0,0,'com_update','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('questions',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_dml'),0,0,'questions','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('com_insert',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_dml'),0,0,'com_insert','次/秒', UNIX_TIMESTAMP(NOW()),1,3);
--临时表使用情况统计
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('created_tmp_disk_tables',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_tmp_table'),0,0,'created_tmp_disk_tables','个/秒', UNIX_TIMESTAMP(NOW()),1,3),
('created_tmp_files',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_tmp_table'),0,0,'created_tmp_files','个/秒', UNIX_TIMESTAMP(NOW()),1,3),
('created_tmp_tables',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_tmp_table'),0,0,'created_tmp_tables','个/秒', UNIX_TIMESTAMP(NOW()),1,3);
--innoDB缓存页统计
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('innodb_buffer_pool_pages_data',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_innodb_buffer_pool_pages'),0,0,'innodb_buffer_pool_pages_data','个', UNIX_TIMESTAMP(NOW()),1,3),
('innodb_buffer_pool_pages_dirty',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_innodb_buffer_pool_pages'),0,0,'innodb_buffer_pool_pages_dirty','个', UNIX_TIMESTAMP(NOW()),1,3),
('innodb_buffer_pool_pages_free',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_innodb_buffer_pool_pages'),0,0,'innodb_buffer_pool_pages_free','个', UNIX_TIMESTAMP(NOW()),1,3),
('innodb_buffer_pool_pages_total',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_innodb_buffer_pool_pages'),0,0,'innodb_buffer_pool_pages_total','个', UNIX_TIMESTAMP(NOW()),1,3);
--innoDB数据刷新情况统计
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('innodb_data_fsyncs',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_innodb_data'),0,0,'innodb_data_fsyncs','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('innodb_data_pending_fsyncs',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_innodb_data'),0,0,'innodb_data_pending_fsyncs','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('innodb_data_reads',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_innodb_data'),0,0,'innodb_data_reads','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('innodb_data_writes',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_innodb_data'),0,0,'innodb_data_writes','次/秒', UNIX_TIMESTAMP(NOW()),1,3);
--instance alive
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('instance_alive',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_instance_alive'),0,0,'instance_alive','', UNIX_TIMESTAMP(NOW()),1,3);
--key_buffer读写命中率统计
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('key_buffer_read_hit_rate',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_key_buffer'),0,0,'key_buffer_read_hit_rate','百分比', UNIX_TIMESTAMP(NOW()),1,3),
('key_buffer_write_hit_rate',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_key_buffer'),0,0,'key_buffer_write_hit_rate','百分比', UNIX_TIMESTAMP(NOW()),1,3);
--数据库连接数监控
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('max_connections',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_connection'),0,0,'max_connections','个', UNIX_TIMESTAMP(NOW()),1,3),
('max_used_connections',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_connection'),0,0,'max_used_connections','个', UNIX_TIMESTAMP(NOW()),1,3);
--文件句柄相关
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('open_files',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_open_handle'),0,0,'open_files','个/秒', UNIX_TIMESTAMP(NOW()),1,3),
('open_tables',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_open_handle'),0,0,'open_tables','个/秒', UNIX_TIMESTAMP(NOW()),1,3),
('opened_tables',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_open_handle'),0,0,'opened_tables','个/秒', UNIX_TIMESTAMP(NOW()),1,3),
('slave_open_temp_tables',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_open_handle'),0,0,'slave_open_temp_tables','个/秒', UNIX_TIMESTAMP(NOW()),1,3);
--慢查询数量统计
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('slow_queries',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slow_query'),0,0,'slow_queries','个/秒', UNIX_TIMESTAMP(NOW()),1,3);
--排序操作统计
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('sort_range',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_sort'),0,0,'sort_range','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('sort_rows',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_sort'),0,0,'sort_rows','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('sort_scan',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_sort'),0,0,'sort_scan','次/秒', UNIX_TIMESTAMP(NOW()),1,3);
--数据库表锁统计
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('table_locks_immediate',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_table_locks'),0,0,'table_locks_immediate','次/秒', UNIX_TIMESTAMP(NOW()),1,3),
('table_locks_waited',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_table_locks'),0,0,'table_locks_waited','次/秒', UNIX_TIMESTAMP(NOW()),1,3);
--数据库活动状态信息
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('threads_cached',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_threads'),0,0,'threads_cached','个', UNIX_TIMESTAMP(NOW()),1,3),
('threads_connected',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_threads'),0,0,'threads_connected','个', UNIX_TIMESTAMP(NOW()),1,3),
('threads_created',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_threads'),0,0,'threads_created','个', UNIX_TIMESTAMP(NOW()),1,3),
('slow_launch_threads',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_threads'),0,0,'slow_launch_threads','个', UNIX_TIMESTAMP(NOW()),1,3),
('threads_running',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_threads'),0,0,'threads_running','个', UNIX_TIMESTAMP(NOW()),1,3);
--slave实例的信息
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('exec_master_log_pos',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info'),0,0,'exec_master_log_pos','', UNIX_TIMESTAMP(NOW()),1,3),
('master_host',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info'),0,0,'master_host','', UNIX_TIMESTAMP(NOW()),1,3),
('master_log_file',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info'),0,0,'master_log_file','', UNIX_TIMESTAMP(NOW()),1,3),
('master_port',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info'),0,0,'master_port','', UNIX_TIMESTAMP(NOW()),1,3),
('slave_sql_running',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info'),0,0,'slave_sql_running','', UNIX_TIMESTAMP(NOW()),1,3),
('slave_io_running',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info'),0,0,'slave_io_running','', UNIX_TIMESTAMP(NOW()),1,3),
('seconds_behind_master',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info'),0,0,'seconds_behind_master','Second', UNIX_TIMESTAMP(NOW()),1,3),
('relay_log_space',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info'),0,0,'relay_log_space','Byte', UNIX_TIMESTAMP(NOW()),1,3),
('read_master_log_pos',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info'),0,0,'read_master_log_pos','', UNIX_TIMESTAMP(NOW()),1,3);

UPDATE IGNORE t_metric_info SET plugin_id=(SELECT plugin_id FROM t_plugin_info WHERE plugin_name='libmysqlmaster.so' LIMIT 1) WHERE metric_type=3;

UPDATE IGNORE t_metric_info SET plugin_id=(SELECT plugin_id FROM t_plugin_info WHERE plugin_name='libmysqlslave.so' LIMIT 1) WHERE metric_type =3 AND metric_group_id=(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_slave_info');
