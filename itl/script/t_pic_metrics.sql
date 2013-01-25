SET NAMES UTF8;
USE db_itl;
INSERT IGNORE INTO t_metric_group_info(metric_group_name,time_threshold,time_interval,metric_group_date,default_flag,metric_group_type)\
VALUES('图片服务器metric组',3600,60,UNIX_TIMESTAMP(NOW()),1,4);

--版本信息
INSERT IGNORE INTO t_metric_info(metric_name,metric_group_id,plugin_id,\
value_threshold,metric_title,metric_unit,metric_log_date,metrictype_id,metric_type) 
VALUES('fdfs_free_disk',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='图片服务器metric组'),0,0,'FDFS Free Disk','GB', UNIX_TIMESTAMP(NOW()),3,4),
('varnish_hit_ratio',(SELECT metric_group_id FROM t_metric_group_info WHERE metric_group_name='mysql_version'),0,0,'Varnish Hit Ratio','%', UNIX_TIMESTAMP(NOW()),3,4);

UPDATE IGNORE t_metric_info SET plugin_id=(SELECT plugin_id FROM t_plugin_info WHERE plugin_name='monit_fdfs.so' LIMIT 1) WHERE metric_type=4 and metric_name='fdfs_free_disk';
UPDATE IGNORE t_metric_info SET plugin_id=(SELECT plugin_id FROM t_plugin_info WHERE plugin_name='monit_varnish.so' LIMIT 1) WHERE metric_type =4 AND metric_name='varnish_hit_ratio';


