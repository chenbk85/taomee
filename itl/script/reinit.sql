SET names utf8;
USE db_itl;

ALTER TABLE t_head_info ADD COLUMN listen_port INT(5) UNSIGNED  NOT NULL DEFAULT 59000;
--导入head信息
TRUNCATE TABLE t_head_info;

--DROP TABLE IF EXISTS t_head_status;
CREATE TABLE IF NOT EXISTS t_head_status(
head_id INT NOT NULL,
is_updated enum('Y','N') NOT NULL DEFAULT 'Y',
PRIMARY KEY(head_id)
)ENGINE = INNODB;

CREATE TABLE IF NOT EXISTS t_alarm_log_info(
id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
alarm_time INT UNSIGNED NOT NULL COMMENT 'the alarm time', 
alarm_type VARCHAR(127) NOT NULL COMMENT 'the alarm type{warning|critical|down|up_failed}',
alarm_way INT UNSIGNED  NOT NULL COMMENT 'the alarm way,1:短信报警,2:email报警,3:email+短信报警,4:rtx报警, 5:rtx+短信报警,6:rtx+email报警,7:rtx+email+短信报警',
alarm_host VARCHAR(255) NOT NULL COMMENT 'alarm host,当是metric报警和host_down时是所在的host,当是ds_down时是ds内的host用","分隔的list',
alarm_metric VARCHAR(127) NOT NULL COMMENT '如果是metric报警，是metric_name，当是host|ds down时是形如 <host_ip|ds_name>_down_metric的字符串',
alarm_content VARCHAR(255) NOT NULL DEFAULT '-' comment '报警详情'
)ENGINE = INNODB DEFAULT CHARSET = utf8;


--将head信息同步到t_head_status表中
--更新grid的父亲(儿子有了)
DROP TRIGGER IF EXISTS tr_head_info_sync;
delimiter |
CREATE TRIGGER tr_head_info_sync AFTER INSERT ON t_head_info 
FOR EACH ROW
BEGIN
INSERT INTO t_head_status(head_id) VALUES(NEW.head_id);
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT parent_id FROM t_head_info WHERE head_id=NEW.head_id);
END |
delimiter ;

START TRANSACTION;
---调整为安装head的主机的ip地址
SET @head_install_ip='10.1.1.44';
---调整为head主机的rrd路径
SET @rrd_rootdir='/opt/taomee/oa/rrds';
INSERT INTO t_head_info(parent_id,ip_inside,listen_port,log_id,summary_interval,trust_hosts,rrd_rootdir,url_id,head_log_date,proj_id)
VALUES(0,@head_install_ip,59000,1,15,CONCAT('127.0.0.1,',@head_install_ip),@rrd_rootdir,1,UNIX_TIMESTAMP(NOW()),0);

INSERT INTO t_head_info(parent_id,ip_inside,listen_port,log_id,summary_interval,trust_hosts,rrd_rootdir,url_id,head_log_date,proj_id)
SELECT 1,@head_install_ip,59000+project_id,1,15,CONCAT('127.0.0.1,',@head_install_ip),@rrd_rootdir,1,UNIX_TIMESTAMP(NOW()),project_id FROM t_project_info b WHERE project_id<>0;
COMMIT;

--创建grid信息的视图
--DROP VIEW IF EXISTS v_grid_info;
CREATE OR REPLACE VIEW v_grid_info AS 
SELECT b.project_name AS grid_name,b.project_english_name AS grid_alias,a.head_id AS grid_id,
a.ip_inside AS listen_ip, a.listen_port AS listen_port,a.summary_interval AS summary_interval,
(SELECT c.url AS url FROM t_url_info c WHERE a.url_id=c.url_id) AS alarm_server_url,
(SELECT d.ip_inside AS ip_inside FROM t_head_info d WHERE a.parent_id=d.head_id) AS trust_host,
a.rrd_rootdir AS rrd_rootdir,e.is_updated as update_status
FROM t_head_info a,t_project_info b,t_head_status e WHERE b.project_id=a.proj_id and a.head_id=e.head_id;

CREATE TABLE IF NOT EXISTS t_ds_info( 
ds_id INT UNSIGNED NOT NULL COMMENT "the project(or module) id",
project_id INT UNSIGNED NOT NULL COMMENT "the project id this data source belongs to",
ds_type INT UNSIGNED NOT NULL DEFAULT 2 COMMENT "ds type{1=grid|2=cluster}",  
metric_name VARCHAR(255) NOT NULL DEFAULT '' COMMENT "the metric name",
metric_val VARCHAR(255) NOT NULL DEFAULT '' COMMENT "the metric value",
PRIMARY KEY(ds_id,project_id,ds_type),
INDEX(ds_id,project_id,ds_type)
)ENGINE = INNODB,DEFAULT CHARSET = UTF8;

DROP TABLE IF EXISTS t_host_info;
CREATE TABLE IF NOT EXISTS t_host_info( 
host_name   VARCHAR(127) NOT NULL DEFAULT '' COMMENT "the server tag",
metric_name VARCHAR(127) NOT NULL DEFAULT '' COMMENT "the metric name",
metric_val  VARCHAR(127) NOT NULL DEFAULT '' COMMENT "the metric value",
PRIMARY KEY(host_name,metric_name),
INDEX(host_name,metric_name)
)ENGINE = INNODB,DEFAULT CHARSET = UTF8;

delimiter |

--url改变
DROP TRIGGER IF EXISTS tr_url_info|
CREATE TRIGGER tr_url_info AFTER UPDATE ON t_url_info 
FOR EACH ROW 
BEGIN
IF OLD.url!=NEW.url THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id IN(SELECT head_id FROM t_head_info WHERE url_id=NEW.url_id);
END IF;
END|

--project名字改变
DROP TRIGGER IF EXISTS tr_project_info|
CREATE TRIGGER tr_project_info AFTER UPDATE ON t_project_info 
FOR EACH ROW 
BEGIN
IF OLD.project_name!=NEW.project_name THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id=NEW.project_id);
END IF;
END |

--更新grid的儿子门
--更新grid的父亲(儿子变了)
DROP TRIGGER IF EXISTS tr_head_info_update_child|
CREATE TRIGGER tr_head_info_update_child AFTER UPDATE ON t_head_info 
FOR EACH ROW 
BEGIN
UPDATE t_head_status SET is_updated='Y' WHERE head_id IN(SELECT head_id FROM t_head_info WHERE parent_id=NEW.head_id);
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT parent_id FROM t_head_info WHERE head_id=NEW.head_id);
END|


--更新grid的父亲(儿子变了)
--DROP TRIGGER tr_head_info_alt;
--CREATE TRIGGER tr_head_info_alt AFTER UPDATE ON t_head_info 
--FOR EACH ROW 
--UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT parent_id FROM t_head_info WHERE head_id=new.head_id);

--更新grid的父亲(儿子没了)
DROP TRIGGER IF EXISTS tr_head_info_del|
CREATE TRIGGER tr_head_info_del BEFORE DELETE ON t_head_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT parent_id FROM t_head_info WHERE head_id=OLD.head_id)|

--更新grid的父亲(儿子有了)
--DROP TRIGGER tr_head_info_add;
--CREATE TRIGGER tr_head_info_add AFTER INSERT ON t_head_info 
--FOR EACH ROW 
--UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT parent_id FROM t_head_info WHERE head_id=new.head_id);

--增加模块了
DROP TRIGGER IF EXISTS tr_project_business_add|
CREATE TRIGGER tr_project_business_add AFTER INSERT ON t_project_business 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id=NEW.project_id)|

--删除模块了
DROP TRIGGER IF EXISTS tr_project_business_del|
CREATE TRIGGER tr_project_business_del BEFORE DELETE ON t_project_business 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id=OLD.project_id)|

--模块信息改变了
DROP TRIGGER IF EXISTS tr_business_info|
CREATE TRIGGER tr_business_info AFTER UPDATE ON t_business_info  
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=
(SELECT head_id FROM t_head_info WHERE proj_id=(SELECT project_id FROM t_project_business WHERE module_id=NEW.module_id))|

--cluster内的发送主机变了
DROP TRIGGER IF EXISTS tr_node_info_alt|
CREATE TRIGGER tr_node_info_alt AFTER UPDATE ON t_node_info  
FOR EACH ROW   
BEGIN  
IF NEW.set_ip!=OLD.set_ip OR(NEW.set_ip=1 AND OLD.set_ip=1 AND(NEW.server_id!=OLD.server_id OR NEW.listen_port!=OLD.listen_port)) THEN  
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id=
(SELECT project_id FROM t_project_business WHERE module_id=
(SELECT module_id FROM t_server_info WHERE server_id=NEW.server_id)));
END IF; 
END|

--cluster内新增发送主机
DROP TRIGGER IF EXISTS tr_node_info_add|
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
DROP TRIGGER IF EXISTS tr_node_info_del|
CREATE TRIGGER tr_node_info_del BEFORE DELETE ON t_node_info  
FOR EACH ROW   
BEGIN  
IF OLD.set_ip=1 THEN  
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id=
(SELECT project_id FROM t_project_business WHERE module_id=
(SELECT module_id FROM t_server_info WHERE server_id=OLD.server_id)));
END IF; 
END|

--发送server的ip变了或者server属于别的模块了
DROP TRIGGER IF EXISTS tr_server_info_alt|
CREATE TRIGGER tr_server_info_alt AFTER UPDATE ON t_server_info  
FOR EACH ROW   
BEGIN  
IF (NEW.ip_inside!=OLD.ip_inside OR NEW.module_id!=OLD.module_id) AND (SELECT set_ip FROM t_node_info WHERE server_id=NEW.server_id)=1 THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT head_id FROM t_head_info WHERE proj_id= 
(SELECT project_id FROM t_project_business WHERE module_id= 
(SELECT module_id FROM t_server_info WHERE server_id=OLD.server_id)));
END IF; 
END|

delimiter ;
