SET names utf8;
USE db_itl;

--录入基础数据
---录入日志配置信息
TRUNCATE TABLE t_log_monitor_info;
INSERT INTO t_log_monitor_info VALUES(1,10,'../log',7,'oa_head_',32000000,UNIX_TIMESTAMP(NOW()));
INSERT INTO t_log_monitor_info VALUES(2,10,'../log',7,'oa_node_',32000000,UNIX_TIMESTAMP(NOW()));

--录入metric信息
--LOCK TABLES `t_metric_info` WRITE;
TRUNCATE TABLE t_metric_info;
/*!40000 ALTER TABLE `t_metric_info` DISABLE KEYS */;
INSERT INTO `t_metric_info`(metric_name,metric_group_id,so_id,value_threshold,metric_title,metric_log_date,metrictype_id,argument) VALUES ('boottime',1,1,0,'Last Boot Time',1299666956,1,''),('bytes_in',6,1,1024,'Bytes     Received',1299667536,1,''),('bytes_out',6,1,1024,'Bytes Sent',1299667513,1,''),('cpu_aidle',2,1,1,'CPU aidle',1299667138, 1,''),('cpu_idle',2,1,1,'CPU Idle',1299667104,1,''),('cpu_intr',2,1,1,'cpu intr',1299667178,1,''),('cpu_nice',2,1,1,'CPU Nice',1299667120,1,''),('cpu_num',1,1,0,'CPU Count',1299662874,1,''),('cpu_sintr',2,1,1,'CPU Sintr',1299667196,1,''),    ('cpu_speed',1,1,0,'CPU Speed',1299664743,1,''),('cpu_system',2,1,1,'CPU System',1299667088,1,''),('cpu_user',2,1,1,'CPU User',1299667046,1,''),('cpu_wio',2,1,1,'CPU wio',1299667154,1,''),('disk_free',9,1,1,'Disk Space Available',1299667823, 1,''),('disk_total',7,1,1,'Total Disk Space',1299667796,1,''),('eth0_recv',6,1,4096,'eth0 Bytes Received',1299667717,1,  ''),('eth0_send',6,1,0,'eth0 Bytes Send',1299667630,1,''),('eth1_recv',6,1,0,'eth1 Bytes Received',1299667771,1,''),     ('eth1_send',6,1,4096,'eth1 Bytes Send',1299667751,1,''),('load_fifteen',3,1,0,'Fifteen Minute Load Average',1299739717, 1,''),('load_five',3,1,1,'Five Minute Load Average',1299667237,1,''),('load_one',3,1,1,'One Minute Load Average',        1299667220,1,''),('machine_type',1,1,0,'Machine Type',1299666979,1,''),('mem_buffers',5,1,1024,'Memory Buffers',         1299667404,1,''),('mem_cached',5,1,1024,'Cached Memory',1299667423,1,''),('mem_free',5,1,1024,'Free Memory',1299667382,1, ''),('mem_total',1,1,0,'Memory Total',1299666898,1,''),('os_name',1,1,0,'Operating System',1299666999,1,''),             ('os_release',1,1,0,'Operating System Release',1299667024,1,''),('part_max_used',9,1,1,'Maximum Disk Space Used',        1299667845,1,''),('pkts_in',6,1,256,'Packets Received',1299667559,1,''),('pkts_out',6,1,1,'Packets Sent',1299667590,1,   ''),('proc_run',4,1,1,'Total Running Processes',1299667331,1,''),('proc_total',4,1,1,'Total Processes',1299667354,1,''), ('swap_free',5,1,1024,'Free Swap Space',1299667472,1,''),('swap_total',1,1,0,'Swap Space Total',1299666920,1,'');
 /*!40000 ALTER TABLE `t_metric_info` ENABLE KEYS */;
--UNLOCK TABLES;
 
--录入metric_group信息
--LOCK TABLES `t_metric_group_info` WRITE;
TRUNCATE TABLE t_metric_group_info;
/*!40000 ALTER TABLE `t_metric_group_info` DISABLE KEYS */;
INSERT INTO `t_metric_group_info` VALUES (1,'system',0,1200,1299668001,1),(2,'cpu',90,20,1299668067,1),(3,'load',90,20,  1299668101,1),(4,'proc',950,80,1299668144,1),(5,'memory',180,40,1299668189,1),(6,'network',300,40,1299668262,1),(7,      'disk_fix',3600,1800,1299668290,1),(9,'disk_var',1800,40,1299740022,1);
/*!40000 ALTER TABLE `t_metric_group_info` ENABLE KEYS */;
--UNLOCK TABLES;

INSERT INTO t_project_info VALUES(0,'China.TaoMee.Project',1,1,'The Root Project',UNIX_TIMESTAMP(NOW())) 
ON DUPLICATE KEY UPDATE project_id=0;

---录入更新服务器信息
START TRANSACTION;
TRUNCATE TABLE t_url_info;
---调整为更新服务器的ip地址
SET @update_server_ip='10.1.1.28'; 
INSERT INTO t_url_info(url_id,url_name,ip_inside,url_log_date,url) VALUES(1,'OA_Update WebServer',@update_server_ip,
UNIX_TIMESTAMP(NOW()),CONCAT('http://',@update_server_ip,'/oa-update/index.php'));
COMMIT;

DROP TABLE t_head_status;
CREATE TABLE IF NOT EXISTS t_head_status(
head_id INT NOT NULL,
is_updated enum('Y','N') NOT NULL DEFAULT 'Y',
FOREIGN KEY(head_id) REFERENCES t_head_info(head_id)
)ENGINE = INNODB;


ALTER TABLE t_head_info ADD COLUMN listen_port INT(5) UNSIGNED  NOT NULL DEFAULT 59000;
--导入head信息
TRUNCATE TABLE t_head_info;
INSERT INTO t_head_info(parent_id,ip_inside,listen_port,log_id,summary_interval,trust_hosts,rrd_rootdir,url_id,head_log_date,proj_id)
VALUES(0,'10.1.1.44',59000,1,15,'127.0.0.1,10.1.1.44','/opt/taomee/oa/rrds',1,UNIX_TIMESTAMP(NOW()),0);

INSERT INTO t_head_info(parent_id,ip_inside,listen_port,log_id,summary_interval,trust_hosts,rrd_rootdir,url_id,head_log_date,proj_id)
SELECT 1,'10.1.1.44',59000+project_id,1,15,'127.0.0.1,10.1.1.44','/opt/taomee/oa/rrds',1,UNIX_TIMESTAMP(NOW()),project_id FROM t_project_info b;

--导入cluster信息
TRUNCATE TABLE t_cluster_info;
INSERT INTO t_cluster_info(module_id,cluster_log_date) SELECT module_id,UNIX_TIMESTAMP(NOW())
FROM t_business_info;

--导入node信息
TRUNCATE TABLE t_node_info;
INSERT INTO t_node_info(server_id,node_log_date,contact_mail_list,contact_phone_list) 
SELECT server_id,UNIX_TIMESTAMP(NOW()),(SELECT email FROM t_user_info a WHERE a.user_id=b.user_id),(SELECT cellphone
FROM t_user_info a WHERE a.user_id=b.user_id) FROM t_server_info b;

--导入metric_group 和node映射表信息
TRUNCATE TABLE t_metric_group_node;
INSERT INTO t_metric_group_node(node_id,metric_group_id) SELECT a.node_id,b.metric_group_id FROM t_node_info a CROSS JOIN t_metric_group_info b;

--ALTER TABLE t_head_info ADD COLUMN is_updated enum('Y','N')  NOT NULL DEFAULT 'Y';

--创建grid信息的视图
DROP VIEW v_grid_info;
CREATE  VIEW IF NOT EXISTS v_grid_info AS 
SELECT b.project_name AS grid_name,a.head_id AS grid_id,a.ip_inside AS listen_ip,
a.listen_port AS listen_port,a.summary_interval AS summary_interval,
(SELECT c.url AS url FROM t_url_info c WHERE a.url_id=c.url_id) AS alarm_server_url,
(SELECT d.ip_inside AS ip_inside FROM t_head_info d WHERE a.parent_id=d.head_id) AS trust_host,
a.rrd_rootdir AS rrd_rootdir 
FROM t_head_info a,t_project_info b WHERE b.project_id=a.proj_id;

CREATE TABLE IF NOT EXISTS t_ds_info( 
ds_id INT UNSIGNED AUTO_INCREMENT COMMENT "to identify the cluster,because the cluster name maybe duplicated",
parent_id INT UNSIGNED NOT NULL COMMENT "the parent id(grid id) of data source",
ds_name VARCHAR(255) NOT NULL COMMENT "the ds(include the grid and cluster) name" ,
ds_type INT UNSIGNED NOT NULL DEFAULT 2 COMMENT "ds type{1=grid|2=cluster}",  
metric_name VARCHAR(255)  NOT NULL DEFAULT '' comment "metric name",
metric_value VARCHAR(64)  NOT NULL DEFAULT '' comment "metric value",
PRIMARY KEY(parent_id,ds_name),
INDEX(parent_id,ds_name),
INDEX(ds_id)
)ENGINE = INNODB;

CREATE TABLE IF NOT EXISTS t_host_info( 
ds_id INT UNSIGNED  NOT NULL  COMMENT "the ds id reference to the t_ds_info(ds_id)",
host_name VARCHAR(255) NOT NULL COMMENT "the server tag",
host_alive ENUM('Y','N') NOT NULL DEFAULT 'Y' COMMENT "is the hoat is alive Y=alive|N=down",  
boot_time INT UNSIGNED NOT NULL DEFAULT 0 COMMENT "the host boot timestamp" ,
node_start INT UNSIGNED NOT NULL DEFAULT 0 COMMENT "the node start timestamp" ,
last_report INT UNSIGNED NOT NULL DEFAULT 0 COMMENT "the node last report timestamp" ,
mathine_type VARCHAR(127) NOT NULL DEFAULT '' COMMENT "host mathine type" ,
os_name VARCHAR(127) NOT NULL DEFAULT '' COMMENT "host os name" ,
os_release VARCHAR(127) NOT NULL DEFAULT '' COMMENT "host os release" ,
cpu_num INT UNSIGNED NOT NULL DEFAULT 0 COMMENT "the number of cpu in this host",  
cpu_speed DOUBLE(15,2) NOT NULL DEFAULT 0 COMMENT "the cpu speed",  
load_one DOUBLE(15,2) NOT NULL DEFAULT 0.0 COMMENT "the load one percent",  
load_five DOUBLE(15,2) NOT NULL DEFAULT 0.0 COMMENT "the load five percent",  
load_fifteen DOUBLE(15,2) NOT NULL DEFAULT 0.0 COMMENT "the load fifteen percent",  
mem_total INT UNSIGNED NOT NULL DEFAULT 0 COMMENT "the total space of memary",  
swap_total INT UNSIGNED NOT NULL DEFAULT 0 COMMENT "the total space of swap",
disk_total int(10) unsigned NOT NULL default '0' COMMENT 'the total space of disk',
disk_free int(10) unsigned NOT NULL default '0' COMMENT 'the free space of disk',
PRIMARY KEY(ds_id,host_name),
FOREIGN KEY(ds_id) REFERENCES t_ds_info(ds_id),
INDEX(ds_id,host_name)
)ENGINE = INNODB;

--将head信息同步到t_head_status表中
DROP TRIGGER tr_head_info_sync;
delimiter |
CREATE TRIGGER tr_head_info_sync before INSERT ON t_head_info 
FOR EACH ROW
INSERT INTO t_head_status(head_id) VALUES(new.head_id);
|

--url改变
DROP TRIGGER tr_url_info;
delimiter |
CREATE TRIGGER tr_url_info AFTER UPDATE ON t_url_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE url_id=new.url_id);
|

--project名字改变
DROP TRIGGER tr_project_info;
delimiter |
CREATE TRIGGER tr_project_info AFTER UPDATE ON t_project_info 
FOR EACH ROW 
UPDATE t_head_stauts SET is_updated='Y' WHERE head_id=( SELECT head_id FROM t_head_info WHERE Proj_id=new.project_id);
|

--更新grid的儿子门
DROP TRIGGER tr_head_info_update_child;
delimiter |
CREATE TRIGGER tr_head_info_update_child AFTER UPDATE ON t_head_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id IN(SELECT head_id FROM t_head_info WHERE parent_id=new.head_id);
|

--更新grid的父亲(儿子变了)
DROP TRIGGER tr_head_info_alt;
delimiter |
CREATE TRIGGER tr_head_info_alt AFTER UPDATE ON t_head_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT parent_id FROM t_head_info WHERE head_id=new.head_id);
|
--更新grid的父亲(儿子没了)
DROP TRIGGER tr_head_info_del;
delimiter |
CREATE TRIGGER tr_head_info_del BEFORE DELETE ON t_head_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT parent_id FROM t_head_info WHERE head_id=old.head_id);
|
--更新grid的父亲(儿子有了)
DROP TRIGGER tr_head_info_add;
delimiter |
CREATE TRIGGER tr_head_info_add AFTER INSERT ON t_head_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT parent_id FROM t_head_info WHERE head_id=new.head_id);
|

--增加模块了
DROP TRIGGER tr_project_business_add;
delimiter |
CREATE TRIGGER tr_project_business_add AFTER INSERT ON t_project_business 
FOR EACH ROW 
UPDATE t_head_staus SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id=NEW.project_id);
|

--删除模块了
DROP TRIGGER tr_project_business_del;
delimiter |
CREATE TRIGGER tr_project_business_del BEFORE DELETE ON t_project_business 
FOR EACH ROW 
UPDATE t_head_staus SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id=OLD.project_id);
|

--模块信息改变了
DROP TRIGGER tr_business_info;
delimiter |
CREATE TRIGGER tr_business_info AFTER UPDATE ON t_business_info  
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=
(SELECT head_id FROM t_head_info WHERE proj_id=(SELECT project_id FROM t_project_business WHERE module_id=NEW.module_id));
|

--cluster内的发送主机变了
DROP TRIGGER tr_node_info_alt;
delimiter |
CREATE TRIGGER tr_node_info_alt AFTER UPDATE ON t_node_info  
FOR EACH ROW   
BEGIN  
IF NEW.set_ip != OLD.set_ip THEN  
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id=
(SELECT project_id FROM t_project_business WHERE module_id=
(SELECT module_id FROM t_server_info WHERE server_id=NEW.server_id)));
END IF; 
END|

--cluster内新增发送主机
DROP TRIGGER tr_node_info_add;
delimiter |
CREATE TRIGGER tr_node_info_add AFTER INSERT ON t_node_info  
FOR EACH ROW   
BEGIN  
IF NEW.set_ip=1 THEN  
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id=
(SELECT project_id FROM t_project_business WHERE module_id=
(SELECT module_id FROM t_server_info WHERE server_id=NEW.server_id)));
END IF; 
END|

--cluster内删除发送主机
DROP TRIGGER tr_node_info_del;
delimiter |
CREATE TRIGGER tr_node_info_del BEFORE DELETE ON t_node_info  
FOR EACH ROW   
BEGIN  
IF OLD.set_ip=1 THEN  
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id=
(SELECT project_id FROM t_project_business WHERE module_id=
(SELECT module_id FROM t_server_info WHERE server_id=OLD.server_id)));
END IF; 
END|
