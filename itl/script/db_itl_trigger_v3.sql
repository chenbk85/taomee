USE db_itl;
DELIMITER $

--metric基本信息更新
DROP TRIGGER IF EXISTS tr_switch_metric_info_alt$
DROP TRIGGER IF EXISTS tr_update_metric_info$
CREATE TRIGGER tr_update_metric_info
AFTER UPDATE ON t_metric_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id
        FROM t_metric_info AS TMI
        INNER JOIN t_metric_group_service AS TMGS ON TMI.metric_group_id = TMGS.metric_group_id
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TMGS.service_id AND TSI.service_type = TMGS.service_type 
        WHERE TSI.service_type = NEW.metric_type AND is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    SET @node_update = 0;
    SET @alarm_update = 0;
    IF (NEW.metrictype_id != OLD.metrictype_id) THEN
        SET @node_update = TRUE;
        IF (OLD.metrictype_id & 1 = 1 AND NEW.metrictype_id & 1 != 1) THEN
            SET @alarm_update = 1;
        END IF;
    END IF;
    IF (NEW.warning_val != OLD.warning_val OR NEW.critical_val != OLD.critical_val OR NEW.operation != OLD.operation OR NEW.normal_interval != OLD.normal_interval OR NEW.retry_interval != OLD.retry_interval) THEN
        SET @alarm_update = 1;
    END IF;
    IF (NEW.metric_group_id != OLD.metric_group_id) THEN
        SET @node_update = 1;
    END IF;

    IF (@node_update = 1 OR @alarm_update = 1) THEN
        OPEN cur_ids;
        REPEAT
            fetch cur_ids into var_node_id;
            IF (stop_flag = 0) THEN
                INSERT INTO t_update_notice SET node_id = var_node_id, node_type = NEW.metric_type,
                    collect_update_flag = @node_update, alarm_update_flag = @alarm_update 
                    ON DUPLICATE KEY UPDATE
                    collect_update_flag = (@node_update | collect_update_flag),
                    alarm_update_flag = (@alarm_update | alarm_update_flag),
                    last_update_time = NOW();
            END IF;
        UNTIL stop_flag = 1
        END REPEAT;
        CLOSE cur_ids;
    END IF;
END$

--metric_group采集间隔变更
DROP TRIGGER IF EXISTS tr_update_metric_group_info$
CREATE TRIGGER tr_update_metric_group_info
AFTER UPDATE ON t_metric_group_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_metric_group_info AS TMGI
        INNER JOIN t_metric_group_service AS TMGS ON TMGI.metric_group_id = TMGS.metric_group_id
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TMGS.service_id AND TSI.service_type = TMGS.service_type 
        WHERE is_monitor = 1 AND TMGI.metric_group_id = NEW.metric_group_id;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;
    
    IF (NEW.time_interval != OLD.time_interval) THEN
        OPEN cur_ids;
        REPEAT
            fetch cur_ids into var_node_id, var_node_type;
            IF (stop_flag = 0) THEN
                INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                    collect_update_flag = 1, alarm_update_flag = 0 
                    ON DUPLICATE KEY UPDATE collect_update_flag = 1, last_update_time = NOW();
            END IF;
        UNTIL stop_flag = 1
        END REPEAT;
        CLOSE cur_ids;
    END IF;
END$

--增加监控metric_group
DROP TRIGGER IF EXISTS tr_insert_metric_group_service$
CREATE TRIGGER tr_insert_metric_group_service
AFTER INSERT ON t_metric_group_service
FOR EACH ROW
BEGIN
    DECLARE var_node_id int(10) default 0;
    SELECT TSI.machine_id INTO var_node_id
        FROM t_service_info
        WHERE is_monitor = 1 AND service_id = NEW.service_id AND service_type = NEW.service_type;

    IF (var_node_id > 0) THEN
        INSERT INTO t_update_notice SET node_id = var_node_id, node_type = NEW.service_type,
            collect_update_flag = 1, alarm_update_flag = 0 
            ON DUPLICATE KEY UPDATE collect_update_flag = 1, last_update_time = NOW();
    END IF;
END$

--减少监控metric_group
DROP TRIGGER IF EXISTS tr_delete_metric_group_service$
CREATE TRIGGER tr_delete_metric_group_service
AFTER DELETE ON t_metric_group_service
FOR EACH ROW
BEGIN
    DECLARE var_node_id int(10) default 0;
    SELECT TSI.machine_id INTO var_node_id
        FROM t_service_info
        WHERE is_monitor = 1 AND service_id = OLD.service_id AND service_type = OLD.service_type;

    IF (var_node_id > 0) THEN
        INSERT INTO t_update_notice SET node_id = var_node_id, node_type = OLD.service_type,
            collect_update_flag = 1, alarm_update_flag = 1 
            ON DUPLICATE KEY UPDATE collect_update_flag = 1, alarm_update_flag = 1, last_update_time = NOW();
    END IF;
END$

--service_info不在监控
DROP TRIGGER IF EXISTS tr_update_service_info$
CREATE TRIGGER tr_update_service_info
AFTER UPDATE ON t_service_info
FOR EACH ROW
BEGIN
    IF (NEW.is_monitor != OLD.is_monitor) THEN
        INSERT INTO t_update_notice SET node_id = NEW.machine_id, node_type = NEW.service_type,
            collect_update_flag = 1, alarm_update_flag = 1 
            ON DUPLICATE KEY UPDATE collect_update_flag = 1, alarm_update_flag = 1, last_update_time = NOW();
    END IF;
END$

--增加监控服务类型
DROP TRIGGER IF EXISTS tr_insert_service_info$
CREATE TRIGGER tr_insert_service_info
AFTER INSERT ON t_service_info
FOR EACH ROW
BEGIN
    IF (NEW.is_monitor = 1) THEN
        INSERT INTO t_update_notice SET node_id = NEW.machine_id, node_type = NEW.service_type,
            collect_update_flag = 1, alarm_update_flag = 0 
            ON DUPLICATE KEY UPDATE collect_update_flag = 1, last_update_time = NOW();
    END IF;
END$

--删除监控服务类型
DROP TRIGGER IF EXISTS tr_delete_service_info$
CREATE TRIGGER tr_delete_service_info
AFTER DELETE ON t_service_info
FOR EACH ROW
BEGIN
    IF (OLD.is_monitor = 1) THEN
        INSERT INTO t_update_notice SET node_id = OLD.machine_id, node_type = OLD.service_type,
            collect_update_flag = 1, alarm_update_flag = 1 
            ON DUPLICATE KEY UPDATE collect_update_flag = 1, alarm_update_flag = 1, last_update_time = NOW();
    END IF;
END$


--node基本信息变更
DROP TRIGGER IF EXISTS tr_node_conf_up$
DROP TRIGGER IF EXISTS tr_update_node_info$
CREATE TRIGGER tr_update_node_info
AFTER UPDATE ON t_node_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;

    IF (NEW.update_interval != OLD.update_interval || NEW.listen_port != OLD.listen_port) THEN
        SELECT machine_id INTO var_node_id
            FROM t_service_info
            WHERE service_id = NEW.node_id AND service_type = 1 AND is_monitor = 1;
        IF (var_node_id > 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = 1,
                collect_update_flag = 1, alarm_update_flag = 0 
                ON DUPLICATE KEY UPDATE collect_update_flag = 1, last_update_time = NOW();
        END IF;
    END IF;
END$


--db配置变更
DROP TRIGGER IF EXISTS tr_update_db_info$
CREATE TRIGGER tr_update_db_info
AFTER UPDATE ON t_db_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;

    IF (NEW.port != OLD.port || NEW.socket != OLD.socket) THEN
        SELECT machine_id INTO var_node_id
            FROM t_service_info
            WHERE service_id = NEW.db_id AND service_type = 3 AND is_monitor = 1;
        IF (var_node_id > 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = 3,
                collect_update_flag = 1, alarm_update_flag = 0 
                ON DUPLICATE KEY UPDATE collect_update_flag = 1, last_update_time = NOW();
        END IF;
    END IF;
END$


---------------------------特殊告警配置---------------------------
--屏蔽时间段更新
--增
DROP TRIGGER IF EXISTS tr_shield_time$
DROP TRIGGER IF EXISTS tr_insert_shield_time$
DROP TRIGGER IF EXISTS tr_insert_alarm_controller_info$
CREATE TRIGGER tr_insert_alarm_controller_info
AFTER INSERT ON t_alarm_controller_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id int(10) default 0;
    SELECT TSI.machine_id INTO var_node_id FROM t_service_info
        WHERE service_id = NEW.service_id AND service_type = NEW.service_type AND is_monitor = 1;
    IF (var_node_id > 0) THEN
        INSERT INTO t_update_notice SET node_id = var_node_id, node_type = NEW.service_type,
            collect_update_flag = 0, alarm_update_flag = 1 
            ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
    END IF;
END$

--删
DROP TRIGGER IF EXISTS tr_delete_shield_time$
DROP TRIGGER IF EXISTS tr_delete_alarm_controller_info$
CREATE TRIGGER tr_delete_alarm_controller_info
AFTER DELETE ON t_alarm_controller_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id int(10) default 0;
    SELECT TSI.machine_id INTO var_node_id FROM t_service_info
        WHERE service_id = OLD.service_id AND service_type = OLD.service_type AND is_monitor = 1;
    IF (var_node_id > 0) THEN
        INSERT INTO t_update_notice SET node_id = var_node_id, node_type = OLD.service_type,
            collect_update_flag = 0, alarm_update_flag = 1 
            ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
    END IF;
END$

--改
DROP TRIGGER IF EXISTS tr_update_shield_time$
DROP TRIGGER IF EXISTS tr_update_alarm_controller_info$
CREATE TRIGGER tr_update_alarm_controller_info
AFTER UPDATE ON t_alarm_controller_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id int(10) default 0;
    SELECT TSI.machine_id INTO var_node_id FROM t_service_info
        WHERE service_id = NEW.service_id AND service_type = NEW.service_type AND is_monitor = 1;
    IF (var_node_id > 0) THEN
        INSERT INTO t_update_notice SET node_id = var_node_id, node_type = NEW.service_type,
            collect_update_flag = 0, alarm_update_flag = 1 
            ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
    END IF;
END$


--告警策略变更
--增
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_add$
DROP TRIGGER IF EXISTS tr_insert_alarm_strategy_info$
CREATE TRIGGER tr_insert_alarm_strategy_info
AFTER INSERT ON t_alarm_strategy_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_alarm_info AS TAI
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAI.service_id AND TSI.service_type = TAI.service_type 
        WHERE TAI.alarm_strategy_id = NEW.id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_node_id, var_node_type;
        IF (stop_flag = 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                collect_update_flag = 0, alarm_update_flag = 1 
                ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;
END$

--删
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_del$
DROP TRIGGER IF EXISTS tr_delete_alarm_strategy_info$
CREATE TRIGGER tr_delete_alarm_strategy_info
AFTER DELETE ON t_alarm_strategy_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_alarm_info AS TAI
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAI.service_id AND TSI.service_type = TAI.service_type 
        WHERE TAI.alarm_strategy_id = OLD.id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_node_id, var_node_type;
        IF (stop_flag = 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                collect_update_flag = 0, alarm_update_flag = 1 
                ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;
END$

--改
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_alt$
DROP TRIGGER IF EXISTS tr_update_alarm_strategy_info$
CREATE TRIGGER tr_update_alarm_strategy_info
AFTER UPDATE ON t_alarm_strategy_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_alarm_info AS TAI
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAI.service_id AND TSI.service_type = TAI.service_type 
        WHERE TAI.alarm_strategy_id = OLD.id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_node_id, var_node_type;
        IF (stop_flag = 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                collect_update_flag = 0, alarm_update_flag = 1 
                ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;
END$

--告警信息变更
--增
DROP TRIGGER IF EXISTS tr_insert_alarm_info$
CREATE TRIGGER tr_insert_alarm_info
AFTER INSERT ON t_alarm_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    SELECT machine_id INTO var_node_id FROM t_service_info
        WHERE service_id = NEW.service_id AND service_type = NEW.service_type AND is_monitor = 1;
    IF (var_node_id > 0) THEN
        INSERT INTO t_update_notice SET node_id = var_node_id, node_type = NEW.service_type,
            collect_update_flag = 0, alarm_update_flag = 1 
            ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
    END IF;
END$

--删
DROP TRIGGER IF EXISTS tr_delete_alarm_info$
CREATE TRIGGER tr_delete_alarm_info
AFTER DELETE ON t_alarm_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id int(10) default 0;
    SELECT machine_id INTO var_node_id FROM t_service_info WHERE service_id = OLD.service_id AND service_type = OLD.service_type AND is_monitor = 1;
    IF (var_node_id > 0) THEN
        INSERT INTO t_update_notice SET node_id = var_node_id, node_type = OLD.service_type,
            collect_update_flag = 0, alarm_update_flag = 1 
            ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
    END IF;
END$

--改
DROP TRIGGER IF EXISTS tr_update_alarm_info$
CREATE TRIGGER tr_update_alarm_info
AFTER UPDATE ON t_alarm_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id int(10) default 0;
    SELECT machine_id INTO var_node_id FROM t_service_info
        WHERE service_id = NEW.service_id AND service_type = NEW.service_type AND is_monitor = 1;
    IF (var_node_id > 0) THEN
        INSERT INTO t_update_notice SET node_id = var_node_id, node_type = NEW.service_type,
            collect_update_flag = 0, alarm_update_flag = 1 
            ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
    END IF;
END$

--告警组metric变更
--增
DROP TRIGGER IF EXISTS tr_insert_alarm_group_metric$
CREATE TRIGGER tr_insert_alarm_group_metric
AFTER INSERT ON t_alarm_group_metric
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = NEW.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_node_id, var_node_type;
        IF (stop_flag = 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                collect_update_flag = 0, alarm_update_flag = 1 
                ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;
END$

--删
DROP TRIGGER IF EXISTS tr_delete_alarm_group_metric$
CREATE TRIGGER tr_delete_alarm_group_metric
AFTER DELETE ON t_alarm_group_metric
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = OLD.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_node_id, var_node_type;
        IF (stop_flag = 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                collect_update_flag = 0, alarm_update_flag = 1 
                ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;
END$

--改
DROP TRIGGER IF EXISTS tr_update_alarm_group_metric$
CREATE TRIGGER tr_update_alarm_group_metric
AFTER UPDATE ON t_alarm_group_metric
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = OLD.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_node_id, var_node_type;
        IF (stop_flag = 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                collect_update_flag = 0, alarm_update_flag = 1 
                ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;
END$

--告警联系人变更
--增
DROP TRIGGER IF EXISTS tr_insert_alarm_group_user$
CREATE TRIGGER tr_insert_alarm_group_user
AFTER INSERT ON t_alarm_group_user
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = NEW.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_node_id, var_node_type;
        IF (stop_flag = 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                collect_update_flag = 0, alarm_update_flag = 1 
                ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;
END$

--删
DROP TRIGGER IF EXISTS tr_delete_alarm_group_user$
CREATE TRIGGER tr_delete_alarm_group_user
AFTER DELETE ON t_alarm_group_user
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = OLD.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_node_id, var_node_type;
        IF (stop_flag = 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                collect_update_flag = 0, alarm_update_flag = 1 
                ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;
END$

--改
DROP TRIGGER IF EXISTS tr_update_alarm_group_user$
CREATE TRIGGER tr_update_alarm_group_user
AFTER UPDATE ON t_alarm_group_user
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = OLD.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_node_id, var_node_type;
        IF (stop_flag = 0) THEN
            INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                collect_update_flag = 0, alarm_update_flag = 1 
                ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;
END$

--用户基本信息变更
DROP TRIGGER IF EXISTS tr_update_alarm_group_user$
CREATE TRIGGER tr_update_alarm_group_user
AFTER UPDATE ON t_user_info
FOR EACH ROW
BEGIN
    DECLARE var_node_id  int(10) default 0;
    DECLARE var_node_type  tinyint(4) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT TSI.machine_id AS node_id, TSI.service_type AS node_type
        FROM t_alarm_group_user AS TAGU
        INNER JOIN t_alarm_group_service AS TAGS ON TAGS.alarm_group_id = TAGU.alarm_group_id
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGU.user_id = OLD.user_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    IF (NEW.user_name != OLD.user_name OR NEW.cellphone != OLD.cellphone OR NEW.email != OLD.email) THEN
        OPEN cur_ids;
        REPEAT
            fetch cur_ids into var_node_id, var_node_type;
            IF (stop_flag = 0) THEN
                INSERT INTO t_update_notice SET node_id = var_node_id, node_type = var_node_type,
                    collect_update_flag = 0, alarm_update_flag = 1 
                    ON DUPLICATE KEY UPDATE alarm_update_flag = 1, last_update_time = NOW();
            END IF;
        UNTIL stop_flag = 1
        END REPEAT;
        CLOSE cur_ids;
    END IF;
END$

----------------------------同步数据变更----------------------------
--增
DROP TRIGGER IF EXISTS tr_host_to_mysql_add$
DROP TRIGGER IF EXISTS tr_insert_host_info$
CREATE TRIGGER tr_insert_host_info
AFTER INSERT ON t_host_info
FOR EACH ROW
BEGIN
    IF (NEW.metric_name = 'last_log_error') THEN
        INSERT IGNORE INTO t_mysql_error_log_info(mysql_host, mysql_port, line_num, error_time, error_content)
        VALUES(NEW.host_name, NEW.metric_arg, SUBSTRING_INDEX(NEW.metric_val,':',1), 
            SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val,':',2),':',-1),
            RIGHT(NEW.metric_val, length(NEW.metric_val) - length(SUBSTRING_INDEX(NEW.metric_val,':',2)) - 1));
    END IF;

    IF (NEW.metric_name = 'last_locked_processlist') THEN
        INSERT IGNORE INTO t_mysql_locked_processlist
        (add_time,mysql_host,mysql_port,process_id,process_user,process_host,process_time,process_info)
        VALUES(UNIX_TIMESTAMP(NOW()),NEW.host_name, NEW.metric_arg,
            SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-5),';',1),
            SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-4),';',1),
            SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-3),';',1),
            SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-2),';',1),
            SUBSTRING_INDEX(NEW.metric_val ,';',-1))
        ON DUPLICATE KEY UPDATE
        process_time = SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-2),';',1);
    END IF;

    IF (NEW.metric_name = 'innodb_deadlock_event') THEN
        INSERT IGNORE INTO t_mysql_innodb_deadlock_event(mysql_host, mysql_port, event_time, event_content)
        VALUES(NEW.host_name, NEW.metric_arg, SUBSTRING_INDEX(NEW.metric_val,':',1),
        RIGHT(NEW.metric_val, length(NEW.metric_val)-length(SUBSTRING_INDEX(NEW.metric_val,':',1))-1));
    END IF;

    IF (NEW.metric_name = 'all_mount_device_name') THEN
        UPDATE t_server_info SET updated = 1 WHERE server_tag = NEW.host_name;
    END IF;
END$

--改
DROP TRIGGER IF EXISTS tr_host_to_mysql_mod$
DROP TRIGGER IF EXISTS tr_update_host_info$
CREATE TRIGGER tr_update_host_info
AFTER UPDATE ON t_host_info
FOR EACH ROW
BEGIN
    IF (NEW.metric_name = 'last_log_error') THEN
        INSERT IGNORE INTO t_mysql_error_log_info(mysql_host, mysql_port, line_num, error_time, error_content)
            VALUES(NEW.host_name, NEW.metric_arg, SUBSTRING_INDEX(NEW.metric_val,':',1),
            SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val,':',2),':',-1),
            RIGHT(NEW.metric_val, length(NEW.metric_val)-length(SUBSTRING_INDEX(NEW.metric_val,':',2)) - 1))
            ON DUPLICATE KEY UPDATE
            error_content =
            RIGHT(NEW.metric_val, length(NEW.metric_val)-length(SUBSTRING_INDEX(NEW.metric_val,':',2)) - 1);
    END IF;

    IF (NEW.metric_name = 'last_locked_processlist') THEN
        INSERT IGNORE INTO t_mysql_locked_processlist(add_time,mysql_host,mysql_port,
            process_id,process_user,process_host,process_time,process_info)
        VALUES(UNIX_TIMESTAMP(NOW()), NEW.host_name, NEW.metric_arg,
            SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-5),';',1),
            SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-4),';',1),
            SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-3),';',1),
            SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val ,';',-2),';',1),
            SUBSTRING_INDEX(NEW.metric_val ,';',-1))
        ON DUPLICATE KEY UPDATE
             process_time = SUBSTRING_INDEX(SUBSTRING_INDEX(NEW.metric_val,';',-2),';',1);
    END IF;

    IF (NEW.metric_name = 'innodb_deadlock_event') THEN
        INSERT IGNORE INTO t_mysql_innodb_deadlock_event(mysql_host, mysql_port, event_time, event_content)
        VALUES(NEW.host_name, NEW.metric_arg, SUBSTRING_INDEX(NEW.metric_val,':',1),
        RIGHT(NEW.metric_val, length(NEW.metric_val)-length(SUBSTRING_INDEX(NEW.metric_val,':',1))-1))
        ON DUPLICATE KEY UPDATE
        event_content=RIGHT(NEW.metric_val, length(NEW.metric_val)-length(SUBSTRING_INDEX(NEW.metric_val,':',1))-1);
    END IF;

    IF (NEW.metric_name = 'all_mount_device_name' AND NEW.metric_val != OLD.metric_val) THEN
        UPDATE t_server_info SET updated = 1 WHERE server_tag = NEW.host_name;
    END IF;
END$




DELIMITER ;

