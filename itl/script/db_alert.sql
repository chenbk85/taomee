SET NAMES UTF8;
USE db_itl;

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
delimiter ;
