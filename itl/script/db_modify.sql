DROP TABLE IF EXISTS t_ds_info;
CREATE TABLE IF NOT EXISTS t_ds_info( 
ds_id INT UNSIGNED NOT NULL COMMENT "the project(or module) id",
project_id INT UNSIGNED NOT NULL COMMENT "the project id this data source belongs to",
ds_type INT UNSIGNED NOT NULL DEFAULT 2 COMMENT "ds type{1=grid|2=cluster}",  
metric_name VARCHAR(255) NOT NULL DEFAULT '' COMMENT "the metric name",
metric_val VARCHAR(255) NOT NULL DEFAULT '' COMMENT "the metric value",
PRIMARY KEY(ds_id,project_id,ds_type,metric_name),
INDEX(ds_id,project_id,ds_type,metric_name)
)ENGINE=INNODB,DEFAULT CHARSET=UTF8;

DROP TABLE IF EXISTS t_host_info;
CREATE TABLE IF NOT EXISTS t_host_info( 
module_id INT UNSIGNED NOT NULL COMMENT "the module id this host belongs to",
host_name VARCHAR(255) NOT NULL DEFAULT '' COMMENT "the server tag",
metric_name VARCHAR(255) NOT NULL DEFAULT '' COMMENT "the metric name",
metric_val VARCHAR(255) NOT NULL DEFAULT '' COMMENT "the metric value",
PRIMARY KEY(module_id,host_name,metric_name),
INDEX(module_id,host_name,metric_name)
)ENGINE=INNODB,DEFAULT CHARSET=UTF8;

--修改t_head_info表
----删除head_id，parent_id两个字段
ALTER TABLE t_head_info DROP COLUMN head_id,DROP COLUMN parent_id;
ALTER TABLE t_head_info CHANGE COLUMN proj_id project_id INT UNSIGNED NOT NULL FIRST;
--UPDATE t_head_info SET project_id=listen_port-59000;

--修改t_server_info表
----删除project_id字段
ALTER TABLE t_server_info drop COLUMN project_id;

--删除t_cluster_info表
DROP TABLE t_cluster_info;

--修改t_business_info表
----添加project_id字段
ALTER TABLE t_business_info ADD COLUMN project_id INT UNSIGNED NOT NULL AFTER module_id;
----更新project_id字段
UPDATE t_business_info SET project_id=(SELECT project_id FROM t_project_business 
WHERE t_business_info.moudle_id=t_project_business.moudle_id)

--删除t_project_business表
DROP TABLE t_project_business;

--SHOW TRIGGERS LIKE '%project_business%';

--创建grid信息的视图
CREATE OR REPLACE VIEW v_grid_info AS 
SELECT b.project_id AS grid_id,a.service_flag as grid_service_flag,b.project_name AS grid_name,
b.project_english_name AS grid_alias,
a.ip_inside AS listen_ip,a.listen_port AS listen_port,a.summary_interval AS summary_interval,
(SELECT c.url FROM t_url_info c WHERE a.url_id=c.url_id) AS alarm_server_url,
(SELECT ip_inside FROM t_head_info WHERE project_id=0 and service_flag=a.service_flag) AS trust_host,
a.rrd_rootdir AS rrd_rootdir,e.is_updated as update_status
FROM t_head_info a,t_project_info b,t_head_status e 
WHERE a.project_id=b.project_id and a.project_id=e.head_id
ORDER BY b.project_id;

CREATE OR REPLACE VIEW v_cluster_info AS SELECT c.project_id AS head_id,b.module_id AS cluster_id,
c.module_name AS cluster_name,b.ip_inside AS listen_ip,a.listen_port AS listen_port
FROM t_node_info a,t_server_info b,t_business_info c
WHERE a.server_id=b.server_id AND a.set_ip=1 AND b.module_id=c.module_id
ORDER BY c.project_id,b.module_id;

delimiter |
--将head信息同步到t_head_status表中
--更新grid的父亲(儿子有了)
DROP TRIGGER IF EXISTS tr_head_info_sync;
delimiter |
CREATE TRIGGER tr_head_info_sync AFTER INSERT ON t_head_info 
FOR EACH ROW
BEGIN
INSERT INTO t_head_status(head_id) VALUES(NEW.project_id);
UPDATE t_head_status SET is_updated='Y' WHERE head_id=0;
END |

--url改变
DROP TRIGGER IF EXISTS tr_url_info|
CREATE TRIGGER tr_url_info AFTER UPDATE ON t_url_info 
FOR EACH ROW 
BEGIN
IF OLD.url!=NEW.url THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id IN(SELECT project_id FROM t_head_info WHERE url_id=NEW.url_id);
END IF;
END|

--project名字改变
DROP TRIGGER IF EXISTS tr_project_info|
CREATE TRIGGER tr_project_info AFTER UPDATE ON t_project_info 
FOR EACH ROW 
BEGIN
IF OLD.project_name!=NEW.project_name  OR OLD.project_english_name!=NEW.project_english_name THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=NEW.project_id;
UPDATE t_head_status SET is_updated='Y' WHERE head_id=0;
END IF;
END |

--更新grid的儿子门
--更新grid的父亲(儿子变了)
DROP TRIGGER IF EXISTS tr_head_info_update_child|
CREATE TRIGGER tr_head_info_update_child AFTER UPDATE ON t_head_info 
FOR EACH ROW 
BEGIN
IF NEW.project_id=0 THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id<>0;
ELSE
UPDATE t_head_status SET is_updated='Y' WHERE head_id=0;
END IF;
UPDATE t_head_status SET is_updated='Y' WHERE head_id=NEW.project_id;
END|

--更新grid的父亲(儿子没了)
DROP TRIGGER IF EXISTS tr_head_info_del|
CREATE TRIGGER tr_head_info_del BEFORE DELETE ON t_head_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=0|

--增加模块了
DROP TRIGGER IF EXISTS tr_business_info_add|
CREATE TRIGGER tr_business_info_add AFTER INSERT ON t_business_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=NEW.project_id|

--删除模块了
DROP TRIGGER IF EXISTS tr_business_info_del|
CREATE TRIGGER tr_business_info_del BEFORE DELETE ON t_business_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=OLD.project_id|

--模块信息改变了
DROP TRIGGER IF EXISTS tr_business_info|
CREATE TRIGGER tr_business_info AFTER UPDATE ON t_business_info  
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=NEW.module_id|

--cluster内的发送主机变了
DROP TRIGGER IF EXISTS tr_node_info_alt|
CREATE TRIGGER tr_node_info_alt AFTER UPDATE ON t_node_info  
FOR EACH ROW   
BEGIN  
IF NEW.set_ip!=OLD.set_ip OR(NEW.set_ip=1 AND OLD.set_ip=1 AND(NEW.server_id!=OLD.server_id OR NEW.listen_port!=OLD.listen_port)) THEN  
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT project_id FROM t_business_info WHERE module_id=
(SELECT module_id FROM t_server_info WHERE server_id=NEW.server_id));
END IF; 
END|

--cluster内新增发送主机
DROP TRIGGER IF EXISTS tr_node_info_add|
CREATE TRIGGER tr_node_info_add AFTER INSERT ON t_node_info  
FOR EACH ROW   
BEGIN  
IF NEW.set_ip=1 THEN  
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT project_id FROM t_business_info WHERE module_id=
(SELECT module_id FROM t_server_info WHERE server_id=NEW.server_id));
END IF; 
END|

--cluster内删除发送主机
DROP TRIGGER IF EXISTS tr_node_info_del|
CREATE TRIGGER tr_node_info_del BEFORE DELETE ON t_node_info  
FOR EACH ROW   
BEGIN  
IF OLD.set_ip=1 THEN  
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT project_id FROM t_business_info WHERE module_id=
(SELECT module_id FROM t_server_info WHERE server_id=OLD.server_id));
END IF; 
END|

--发送server的ip变了或者server属于别的模块了
DROP TRIGGER IF EXISTS tr_server_info_alt|
CREATE TRIGGER tr_server_info_alt AFTER UPDATE ON t_server_info  
FOR EACH ROW   
BEGIN  
IF (NEW.ip_inside!=OLD.ip_inside OR NEW.module_id!=OLD.module_id) AND (SELECT set_ip FROM t_node_info WHERE server_id=NEW.server_id)=1 THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=(SELECT project_id FROM t_business_info WHERE module_id= 
(SELECT module_id FROM t_server_info WHERE server_id=OLD.server_id));
END IF; 
END|

delimiter ;
