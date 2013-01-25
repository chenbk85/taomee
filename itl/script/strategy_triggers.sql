SET NAMES UTF8;
USE db_itl;

--给oaadmin delete权限
GRANT DELETE ON `db_itl`.`t_host_info` TO 'oaadmin'@'192.168.6.51'; 

delimiter :
--增加报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_add:
CREATE TRIGGER tr_alarm_strategy_info_add AFTER INSERT ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT service_type FROM t_alarm_info WHERE alarm_strategy_id=NEW.id) THEN
UPDATE t_head_status SET is_updated='Y',action=action|4 WHERE head_id=-1;
ELSE 
UPDATE t_head_status SET is_updated='Y' WHERE head_id<>-1;
END IF;
END:

--删除报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_del:
CREATE TRIGGER tr_alarm_strategy_info_del AFTER DELETE ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT service_type FROM t_alarm_info WHERE alarm_strategy_id=OLD.id) THEN
UPDATE t_head_status SET is_updated='Y',action=action|4 WHERE head_id=-1;
ELSE 
UPDATE t_head_status SET is_updated='Y' WHERE head_id<>-1;
END IF;
END :

--更新报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_alt:
CREATE TRIGGER tr_alarm_strategy_info_alt AFTER UPDATE ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT service_type FROM t_alarm_info WHERE alarm_strategy_id=OLD.id) THEN
UPDATE t_head_status SET is_updated='Y',action=action|4 WHERE head_id=-1;
ELSE 
UPDATE t_head_status SET is_updated='Y' WHERE head_id<>-1;
END IF;
END :

--增加metric
DROP TRIGGER IF EXISTS tr_switch_metric_info_add:
CREATE TRIGGER tr_switch_metric_info_add AFTER INSERT ON t_metric_info 
FOR EACH ROW 
BEGIN
IF NEW.metric_type=2 THEN
UPDATE t_head_status SET is_updated='Y',action=action|2 WHERE head_id=-1;
ELSEIF NEW.metrictype_id&1=1 THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id<>-1;
END IF;
END :

--删除metric
DROP TRIGGER IF EXISTS tr_switch_metric_info_del:
CREATE TRIGGER tr_switch_metric_info_del AFTER DELETE ON t_metric_info 
FOR EACH ROW 
BEGIN
IF OLD.metric_type=2 THEN
UPDATE t_head_status SET is_updated='Y',action=action|2 WHERE head_id=-1;
ELSEIF OLD.metrictype_id&1=1 THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id<>-1;
END IF;
END :

--更新metric
DROP TRIGGER IF EXISTS tr_switch_metric_info_alt:
CREATE TRIGGER tr_switch_metric_info_alt AFTER UPDATE ON t_metric_info 
FOR EACH ROW 
BEGIN
IF (NEW.metric_type=2 OR OLD.metric_type=2) THEN
UPDATE t_head_status SET is_updated='Y',action=action|2 WHERE head_id=-1;
ELSEIF (OLD.metrictype_id&1=1 AND NEW.metrictype_id&1=0) OR (NEW.metrictype_id&1=0 AND OLD.metrictype_id&1=1) OR (NEW.metrictype_id&1=1 AND OLD.metrictype_id&1=1) THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id<>-1;
END IF;
END :

delimiter ;
