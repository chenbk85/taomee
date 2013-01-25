SET names utf8;
USE db_itl;
ALTER TABLE t_switch_interface_info CHANGE COLUMN if_type if_type INT UNSIGNED NOT NULL COMMENT '接口类型';
ALTER TABLE t_head_status ADD COLUMN action TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '更新操作动作';
--交换机的更新状态
INSERT INTO t_head_status VALUES(-1, 'Y');

delimiter :
--增加交换机
DROP TRIGGER IF EXISTS tr_switch_info_add:
CREATE TRIGGER tr_switch_info_add AFTER INSERT ON t_switch_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y',action=action|1 WHERE head_id=-1;
:

--删除交换机
DROP TRIGGER IF EXISTS tr_switch_info_del:
CREATE TRIGGER tr_switch_info_del BEFORE DELETE ON t_switch_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y',action=action|1 WHERE head_id=-1;
:

--更新交换机信息
DROP TRIGGER IF EXISTS tr_switch_info_atl:
DROP TRIGGER IF EXISTS tr_switch_info_alt:
CREATE TRIGGER tr_switch_info_alt AFTER UPDATE ON t_switch_info  
FOR EACH ROW 
BEGIN
IF (OLD.ip_inside!=NEW.ip_inside OR OLD.community!=NEW.community OR 
OLD.pattern_id!=NEW.pattern_id) THEN
UPDATE t_head_status SET is_updated='Y',action=action|1 WHERE head_id=-1;
END IF;
END
:

--增加交换机监控的metric
DROP TRIGGER IF EXISTS tr_switch_metric_info_add:
CREATE TRIGGER tr_switch_metric_info_add AFTER INSERT ON t_metric_info 
FOR EACH ROW 
BEGIN
IF NEW.metric_type=2 THEN
UPDATE t_head_status SET is_updated='Y',action=action|2 WHERE head_id=-1;
END IF;
END  
:

--删除交换机监控的metric
DROP TRIGGER IF EXISTS tr_switch_metric_info_del:
CREATE TRIGGER tr_switch_metric_info_del AFTER DELETE ON t_metric_info 
FOR EACH ROW 
BEGIN
IF OLD.metric_type=2 THEN
UPDATE t_head_status SET is_updated='Y',action=action|2 WHERE head_id=-1;
END IF;
END :

--更新交换机监控的metric
DROP TRIGGER IF EXISTS tr_switch_metric_info_alt:
CREATE TRIGGER tr_switch_metric_info_alt AFTER UPDATE ON t_metric_info 
FOR EACH ROW 
BEGIN
IF (OLD.metric_type <> NEW.metric_type) or (NEW.metric_type=2 and OLD.metric_type=2) THEN
UPDATE t_head_status SET is_updated='Y',action=action|2 WHERE head_id=-1;
END IF;
END :

--增加报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_add:
CREATE TRIGGER tr_alarm_strategy_info_add AFTER INSERT ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT alarm_type FROM t_alarm_info WHERE alarm_strategy_id=NEW.id) THEN
UPDATE t_head_status SET is_updated='Y',action=action|4 WHERE head_id=-1;
END IF;
END :

--删除报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_del:
CREATE TRIGGER tr_alarm_strategy_info_del AFTER DELETE ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT alarm_type FROM t_alarm_info WHERE alarm_strategy_id=OLD.id) THEN
UPDATE t_head_status SET is_updated='Y',action=action|4 WHERE head_id=-1;
END IF;
END :

--更新报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_alt:
CREATE TRIGGER tr_alarm_strategy_info_alt AFTER UPDATE ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT alarm_type FROM t_alarm_info WHERE alarm_strategy_id=OLD.id) THEN
UPDATE t_head_status SET is_updated='Y',action=action|4 WHERE head_id=-1;
END IF;
END :

--t_switch_interface_speed
--增加交换机接口报警策略
DROP TRIGGER IF EXISTS tr_switch_interface_speed_add:
CREATE TRIGGER tr_switch_interface_speed_add AFTER INSERT ON t_switch_interface_speed 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y',action=action|8 WHERE head_id=-1;
:

--删除交换机接口报警策略
DROP TRIGGER IF EXISTS tr_switch_interface_speed_del:
CREATE TRIGGER tr_switch_interface_speed_del BEFORE DELETE ON t_switch_interface_speed 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y',action=action|8 WHERE head_id=-1;
:

--更新交换机接口报警策略
DROP TRIGGER IF EXISTS tr_switch_interface_speed_alt:
CREATE TRIGGER tr_switch_interface_speed_alt AFTER UPDATE ON t_switch_interface_speed  
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y',action=action|8 WHERE head_id=-1;
:

delimiter ;

