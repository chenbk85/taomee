SET names utf8;
USE db_itl;

--录入基础数据
---录入日志配置信息
TRUNCATE TABLE t_log_monitor_info;
INSERT INTO t_log_monitor_info VALUES(1,10,'../log',7,'oa_head_',32000000,UNIX_TIMESTAMP(NOW()));
INSERT INTO t_log_monitor_info VALUES(2,10,'../log',7,'oa_node_',32000000,UNIX_TIMESTAMP(NOW()));

--录入metric信息
TRUNCATE TABLE t_metric_info;
/*!40000 ALTER TABLE `t_metric_info` DISABLE KEYS */;
INSERT INTO `t_metric_info` VALUES ('boottime',1,1,0,'Last Boot Time','s',1299666956,1,''),('bytes_in',6,1,1024,'Bytes Received','bytes/sec',1299667536,1,''),('bytes_out',6,1,1024,'Bytes Sent','bytes/sec',1299667513,1,''),('cpu_aidle',2,1,1,'CPU aidle','%',1299667138,1,''),('cpu_idle',2,1,1,'CPU_idle','%',1299667104,1,''),('cpu_intr',2, 1,1,'cpu intr','%',1299667178,1,''),('cpu_nice',2,1,1,'CPU Nice','%',1299667120,1,''),('cpu_num',1,1,0,'CPU Count ','CPUs',1299662874,1,''),('cpu_sintr',2,1,1,'CPU Sintr','%',1299667196,1,''),('cpu_speed',1,1,0,'CPU Speed','MHz' ,1299664743,1,''),('cpu_system',2,1,1,'CPU System','%',1299667088,1,''),('cpu_user',2,1,1,'CPU User','%',129966704 6,1,''),('cpu_wio',2,1,1,'CPU wio','%',1299667154,1,''),('disk_free',9,1,1,'Disk Space Available','GB',1299667823, 1,''),('disk_total',7,1,1,'Total Disk Space','GB',1299667796,1,''),('eth0_recv',6,1,4096,'eth0 Bytes Received','b ytes/sec',1299667717,1,''),('eth0_send',6,1,0,'eth0 Bytes Send','bytes/sec',1299667630,1,''),('eth1_recv',6,1,0,'eth1 Bytes Received','bytes/sec',1299667771,1,''),('eth1_send',6,1,4096,'eth1 Bytes Send','bytes/sec',1299667751,1, ''),('load_fifteen',3,1,0,'Fifteen Minute Load Average','',1299739717,1,''),('load_five',3,1,1,'Five Minute Load A verage','',1299667237,1,''),('load_one',3,1,1,'One Minute Load Average','',1299667220,1,''),('machine_type',1,1,0, 'Machine Type','',1299666979,1,''),('mem_buffers',5,1,1024,'Memory Buffers','KB',1299667404,1,''),('mem_cached',5, 1,1024,'Cached Memory','KB',1299667423,1,''),('mem_free',5,1,1024,'Free Memory','KB',1299667382,1,''),('mem_total' ,1,1,0,'Memory Total','KB',1299666898,1,''),('os_name',1,1,0,'Operating System','',1299666999,1,''),('os_release', 1,1,0,'Operating System Release','',1299667024,1,''),('part_max_used',9,1,1,'Maximum Disk Space Used','%',12996678 45,1,''),('pkts_in',6,1,256,'Packets Received','packets/sec',1299667559,1,''),('pkts_out',6,1,1,'Packets Sent','pa ckets/sec',1299667590,1,''),('proc_run',4,1,1,'Total Running Processes','',1299667331,1,''),('proc_total',4,1,1,'T otal Processes','',1299667354,1,''),('swap_free',5,1,1024,'Free Swap Space','KB',1299667472,1,''),('swap_total',1 ,1,0,'Swap Space Total','KB',1299666920,1,'');
/*!40000 ALTER TABLE `t_metric_info` ENABLE KEYS */;
 
--录入metric_group信息
TRUNCATE TABLE t_metric_group_info;
/*!40000 ALTER TABLE `t_metric_group_info` DISABLE KEYS */;
INSERT INTO `t_metric_group_info` VALUES (1,'system',0,1200,1299668001,1),(2,'cpu',90,20,1299668067,1),(3,'load',90,20,  1299668101,1),(4,'proc',950,80,1299668144,1),(5,'memory',180,40,1299668189,1),(6,'network',300,40,1299668262,1),(7,      'disk_fix',3600,1800,1299668290,1),(9,'disk_var',1800,40,1299740022,1);
/*!40000 ALTER TABLE `t_metric_group_info` ENABLE KEYS */;

TRUNCATE TABLE t_project_info;
INSERT INTO t_project_info VALUES(0,'China.TaoMee.Project','china.taomee.project',1,1,'The Root Project',UNIX_TIMESTAMP(NOW())) ON DUPLICATE KEY UPDATE project_id=0;
UPDATE t_project_info SET project_id = 0 WHERE project_name='China.TaoMee.Project'
AND project_english_name = 'china.taomee.project';

---录入更新服务器信息
START TRANSACTION;
TRUNCATE TABLE t_url_info;
---调整为更新服务器的ip地址
SET @update_server_ip='10.1.1.101'; 
INSERT INTO t_url_info(url_id,url_name,ip_inside,url_log_date,url) VALUES(1,'OA_Update WebServer',@update_server_ip,
UNIX_TIMESTAMP(NOW()),CONCAT('http://',@update_server_ip,'/oa-update/index.php'));
COMMIT;

--导入根head信息
TRUNCATE TABLE t_head_info;
INSERT INTO t_head_info(parent_id,ip_inside,listen_port,log_id,summary_interval,trust_hosts,rrd_rootdir,url_id,head_log_date,proj_id)
VALUES(0,'10.1.1.44',59000,1,15,'127.0.0.1,10.1.1.44','/opt/taomee/oa/rrds',1,UNIX_TIMESTAMP(NOW()),0);
