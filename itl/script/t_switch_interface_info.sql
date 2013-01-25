SET names utf8;
USE db_itl;

DROP TABLE IF EXISTS t_switch_interface_info;
CREATE TABLE IF NOT EXISTS t_switch_interface_info(
switch_id INT UNSIGNED NOT NULL COMMENT 'switch id',
if_idx INT UNSIGNED NOT NULL COMMENT 'the interface index', 
if_status TINYINT UNSIGNED NOT NULL COMMENT 'the interface status{1=up|2=down}',
if_speed INT UNSIGNED NOT NULL COMMENT 'the interface speed', 
is_selected TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '是否选中{1=selected|0=unselected}',
if_type VARCHAR(127) NOT NULL COMMENT 'the interface type', 
if_desc VARCHAR(255) NOT NULL DEFAULT '-' COMMENT 'the interface desc(name)',
if_alias VARCHAR(255) NOT NULL COMMENT 'the interface alias',
PRIMARY KEY(switch_id,if_idx)
)ENGINE = INNODB DEFAULT CHARSET = utf8;

delimiter |

--增加交换机
DROP TRIGGER IF EXISTS tr_switch_info_add|
CREATE TRIGGER tr_switch_info_add AFTER INSERT ON t_switch_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=-1;
|

--删除交换机
DROP TRIGGER IF EXISTS tr_switch_info_del|
CREATE TRIGGER tr_switch_info_del BEFORE DELETE ON t_switch_info 
FOR EACH ROW 
UPDATE t_head_status SET is_updated='Y' WHERE head_id=-1;
|

--更新交换机信息
DROP TRIGGER IF EXISTS tr_switch_info_atl|
CREATE TRIGGER tr_switch_info_atl AFTER UPDATE ON t_switch_info  
FOR EACH ROW 
BEGIN
IF (OLD.ip_inside!=NEW.ip_inside OR OLD.community!=NEW.community OR 
OLD.pattern_id!=NEW.pattern_id) THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=-1;
END IF;
END
|

--增加交换机监控的metric
DROP TRIGGER IF EXISTS tr_switch_metric_info_add|
CREATE TRIGGER tr_switch_metric_info_add AFTER INSERT ON t_metric_info 
FOR EACH ROW 
BEGIN
IF NEW.metric_type=2 THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=-1;
END IF;
END |

--删除交换机监控的metric
DROP TRIGGER IF EXISTS tr_switch_metric_info_del|
CREATE TRIGGER tr_switch_metric_info_del AFTER DELETE ON t_metric_info 
FOR EACH ROW 
BEGIN
IF OLD.metric_type=2 THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=-1;
END IF;
END |

--更新交换机监控的metric
DROP TRIGGER IF EXISTS tr_switch_metric_info_alt|
CREATE TRIGGER tr_switch_metric_info_alt AFTER UPDATE ON t_metric_info 
FOR EACH ROW 
BEGIN
IF (OLD.metric_type <> NEW.metric_type) or (NEW.metric_type=2 and OLD.metric_type=2) THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=-1;
END IF;
END |

--增加报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_add|
CREATE TRIGGER tr_alarm_strategy_info_add AFTER INSERT ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT alarm_type FROM t_alarm_info WHERE alarm_strategy_id=NEW.id) THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=-1;
END IF;
END |

--删除报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_del|
CREATE TRIGGER tr_alarm_strategy_info_del AFTER DELETE ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT alarm_type FROM t_alarm_info WHERE alarm_strategy_id=OLD.id) THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=-1;
END IF;
END |

--更新报警策略时
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_alt|
CREATE TRIGGER tr_alarm_strategy_info_alt AFTER UPDATE ON t_alarm_strategy_info 
FOR EACH ROW 
BEGIN
IF 2=(SELECT alarm_type FROM t_alarm_info WHERE alarm_strategy_id=OLD.id) THEN
UPDATE t_head_status SET is_updated='Y' WHERE head_id=-1;
END IF;
END |

delimiter ;

