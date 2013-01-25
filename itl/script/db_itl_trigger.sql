USE db_itl;
DELIMITER $

--metric基本信息更新
DROP TRIGGER IF EXISTS tr_switch_metric_info_alt$
DROP TRIGGER IF EXISTS tr_update_metric_info$
CREATE TRIGGER tr_update_metric_info
AFTER UPDATE ON t_metric_info
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_metric_info AS TMI
        INNER JOIN t_metric_group_service AS TMGS ON TMI.metric_group_id = TMGS.metric_group_id
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TMGS.service_id AND TSI.service_type = TMGS.service_type 
        WHERE TSI.service_type = NEW.metric_type AND is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    SET @node_update = FALSE;
    SET @alarm_update = FALSE;
    IF (NEW.metrictype_id != OLD.metrictype_id) THEN
        SET @node_update = TRUE;
        IF (OLD.metrictype_id & 1 = 1 AND NEW.metrictype_id & 1 != 1) THEN
            SET @alarm_update = TRUE;
        END IF;
    END IF;
    IF (NEW.warning_val != OLD.warning_val OR NEW.critical_val != OLD.critical_val OR NEW.operation != OLD.operation OR NEW.normal_interval != OLD.normal_interval OR NEW.retry_interval != OLD.retry_interval) THEN
        SET @alarm_update = TRUE;
    END IF;
    IF (NEW.metric_group_id != OLD.metric_group_id) THEN
        SET @alarm_update = TRUE;
    END IF;

    IF (@node_update = TRUE OR @alarm_update = TRUE) THEN
        OPEN cur_ids;
        REPEAT
            fetch cur_ids into var_id;
            IF (stop_flag = 0) THEN
                IF length(var_str_ids) = 0 THEN
                    SET var_str_ids = CONCAT(var_str_ids, var_id);
                ELSE
                    SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
                END IF;
            END IF;
        UNTIL stop_flag = 1
        END REPEAT;
        CLOSE cur_ids;
    END IF;

    IF (@node_update = TRUE AND length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1003, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
    IF (@alarm_update = TRUE AND length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--metric_group采集间隔变更
DROP TRIGGER IF EXISTS tr_update_metric_group_info$
CREATE TRIGGER tr_update_metric_group_info
AFTER UPDATE ON t_metric_group_info
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_metric_group_info AS TMGI
        INNER JOIN t_metric_group_service AS TMGS ON TMGI.metric_group_id = TMGS.metric_group_id
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TMGS.service_id AND TSI.service_type = TMGS.service_type 
        WHERE is_monitor = 1 AND TMGI.metric_group_id = NEW.metric_group_id;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;
    
    IF (NEW.time_interval != OLD.time_interval) THEN
        OPEN cur_ids;
        REPEAT
            fetch cur_ids into var_id;
            IF (stop_flag = 0) THEN
                IF length(var_str_ids) = 0 THEN
                    SET var_str_ids = CONCAT(var_str_ids, var_id);
                ELSE
                    SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
                END IF;
            END IF;
        UNTIL stop_flag = 1
        END REPEAT;
        CLOSE cur_ids;

        IF (length(var_str_ids) > 0) THEN
            INSERT INTO t_temp_notice_info SET cmd = 1003, flag = 0, proto = CONCAT('server_id=', var_str_ids);
        END IF;
    END IF;
END$

--增加监控metric_group
DROP TRIGGER IF EXISTS tr_insert_metric_group_service$
CREATE TRIGGER tr_insert_metric_group_service
AFTER INSERT ON t_metric_group_service
FOR EACH ROW
BEGIN
    DECLARE var_server_id int(10) default 0;
    SELECT machine_id INTO var_server_id
        FROM t_service_info
        WHERE is_monitor = 1 AND service_id = NEW.service_id AND service_type = NEW.service_type;

    IF (var_server_id > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1003, flag = 0, proto = CONCAT('server_id=', var_server_id);
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_server_id);
    END IF;
END$

--减少监控metric_group
DROP TRIGGER IF EXISTS tr_delete_metric_group_service$
CREATE TRIGGER tr_delete_metric_group_service
AFTER DELETE ON t_metric_group_service
FOR EACH ROW
BEGIN
    DECLARE var_server_id int(10) default 0;
    SELECT machine_id INTO var_server_id
        FROM t_service_info
        WHERE is_monitor = 1 AND service_id = OLD.service_id AND service_type = OLD.service_type;

    IF (var_server_id > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1003, flag = 0, proto = CONCAT('server_id=', var_server_id);
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_server_id);
    END IF;
END$

--service_info不在监控
DROP TRIGGER IF EXISTS tr_update_service_info$
CREATE TRIGGER tr_update_service_info
AFTER UPDATE ON t_service_info
FOR EACH ROW
BEGIN
    IF (NEW.is_monitor != OLD.is_monitor) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1003, flag = 0, proto = CONCAT('server_id=', OLD.machine_id);
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', OLD.machine_id);
    END IF;
END$

--增加监控服务类型
DROP TRIGGER IF EXISTS tr_insert_service_info$
CREATE TRIGGER tr_insert_service_info
AFTER INSERT ON t_service_info
FOR EACH ROW
BEGIN
    IF (NEW.is_monitor = 1) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1003, flag = 0, proto = CONCAT('server_id=', NEW.machine_id);
    END IF;
END$

--删除监控服务类型
DROP TRIGGER IF EXISTS tr_delete_service_info$
CREATE TRIGGER tr_delete_service_info
AFTER DELETE ON t_service_info
FOR EACH ROW
BEGIN
    IF (OLD.is_monitor = 1) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1003, flag = 0, proto = CONCAT('server_id=', OLD.machine_id);
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
    DECLARE srv_id int(10) default 0;
    SELECT machine_id INTO srv_id FROM t_service_info
        WHERE service_id = NEW.service_id AND service_type = NEW.service_type AND is_monitor = 1;
    IF (srv_id > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', srv_id);
    END IF;
END$

--删
DROP TRIGGER IF EXISTS tr_delete_shield_time$
DROP TRIGGER IF EXISTS tr_delete_alarm_controller_info$
CREATE TRIGGER tr_delete_alarm_controller_info
AFTER DELETE ON t_alarm_controller_info
FOR EACH ROW
BEGIN
    DECLARE srv_id int(10) default 0;
    SELECT machine_id INTO srv_id FROM t_service_info
        WHERE service_id = OLD.service_id AND service_type = OLD.service_type AND is_monitor = 1;
    IF (srv_id > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', srv_id);
    END IF;
END$

--改
DROP TRIGGER IF EXISTS tr_update_shield_time$
DROP TRIGGER IF EXISTS tr_update_alarm_controller_info$
CREATE TRIGGER tr_update_alarm_controller_info
AFTER UPDATE ON t_alarm_controller_info
FOR EACH ROW
BEGIN
    DECLARE srv_id int(10) default 0;
    SELECT machine_id INTO srv_id FROM t_service_info
        WHERE service_id = NEW.service_id AND service_type = NEW.service_type AND is_monitor = 1;
    IF (srv_id > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', srv_id);
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
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_alarm_info AS TAI
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAI.service_id AND TSI.service_type = TAI.service_type 
        WHERE TAI.alarm_strategy_id = NEW.id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_id;
        IF (stop_flag = 0) THEN
            IF length(var_str_ids) = 0 THEN
                SET var_str_ids = CONCAT(var_str_ids, var_id);
            ELSE
                SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
            END IF;
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;

    IF (length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--删
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_del$
DROP TRIGGER IF EXISTS tr_delete_alarm_strategy_info$
CREATE TRIGGER tr_delete_alarm_strategy_info
AFTER DELETE ON t_alarm_strategy_info
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_alarm_info AS TAI
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAI.service_id AND TSI.service_type = TAI.service_type 
        WHERE TAI.alarm_strategy_id = OLD.id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_id;
        IF (stop_flag = 0) THEN
            IF length(var_str_ids) = 0 THEN
                SET var_str_ids = CONCAT(var_str_ids, var_id);
            ELSE
                SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
            END IF;
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;

    IF (length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--改
DROP TRIGGER IF EXISTS tr_alarm_strategy_info_alt$
DROP TRIGGER IF EXISTS tr_update_alarm_strategy_info$
CREATE TRIGGER tr_update_alarm_strategy_info
AFTER UPDATE ON t_alarm_strategy_info
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_alarm_info AS TAI
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAI.service_id AND TSI.service_type = TAI.service_type 
        WHERE TAI.alarm_strategy_id = OLD.id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_id;
        IF (stop_flag = 0) THEN
            IF length(var_str_ids) = 0 THEN
                SET var_str_ids = CONCAT(var_str_ids, var_id);
            ELSE
                SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
            END IF;
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;

    IF (length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--告警信息变更
--增
DROP TRIGGER IF EXISTS tr_insert_alarm_info$
CREATE TRIGGER tr_insert_alarm_info
AFTER INSERT ON t_alarm_info
FOR EACH ROW
BEGIN
    DECLARE srv_id int(10) default 0;
    SELECT machine_id INTO srv_id FROM t_service_info
        WHERE service_id = NEW.service_id AND service_type = NEW.service_type AND is_monitor = 1;
    IF (length(srv_id) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', srv_id);
    END IF;
END$

--删
DROP TRIGGER IF EXISTS tr_delete_alarm_info$
CREATE TRIGGER tr_delete_alarm_info
AFTER DELETE ON t_alarm_info
FOR EACH ROW
BEGIN
    DECLARE srv_id int(10) default 0;
    SELECT machine_id INTO srv_id FROM t_service_info WHERE service_id = OLD.service_id AND service_type = OLD.service_type AND is_monitor = 1;
    IF (srv_id > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', srv_id);
    END IF;
END$

--改
DROP TRIGGER IF EXISTS tr_update_alarm_info$
CREATE TRIGGER tr_update_alarm_info
AFTER UPDATE ON t_alarm_info
FOR EACH ROW
BEGIN
    DECLARE srv_id int(10) default 0;
    SELECT machine_id INTO srv_id FROM t_service_info
        WHERE service_id = NEW.service_id AND service_type = NEW.service_type AND is_monitor = 1;
    IF (srv_id > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', srv_id);
    END IF;
END$

--告警组metric变更
--增
DROP TRIGGER IF EXISTS tr_insert_alarm_group_metric$
CREATE TRIGGER tr_insert_alarm_group_metric
AFTER INSERT ON t_alarm_group_metric
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = NEW.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_id;
        IF (stop_flag = 0) THEN
            IF length(var_str_ids) = 0 THEN
                SET var_str_ids = CONCAT(var_str_ids, var_id);
            ELSE
                SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
            END IF;
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;

    IF (length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--删
DROP TRIGGER IF EXISTS tr_delete_alarm_group_metric$
CREATE TRIGGER tr_delete_alarm_group_metric
AFTER DELETE ON t_alarm_group_metric
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = OLD.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_id;
        IF (stop_flag = 0) THEN
            IF length(var_str_ids) = 0 THEN
                SET var_str_ids = CONCAT(var_str_ids, var_id);
            ELSE
                SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
            END IF;
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;

    IF (length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--改
DROP TRIGGER IF EXISTS tr_update_alarm_group_metric$
CREATE TRIGGER tr_update_alarm_group_metric
AFTER UPDATE ON t_alarm_group_metric
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = OLD.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_id;
        IF (stop_flag = 0) THEN
            IF length(var_str_ids) = 0 THEN
                SET var_str_ids = CONCAT(var_str_ids, var_id);
            ELSE
                SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
            END IF;
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;

    IF (length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--告警联系人变更
--增
DROP TRIGGER IF EXISTS tr_insert_alarm_group_user$
CREATE TRIGGER tr_insert_alarm_group_user
AFTER INSERT ON t_alarm_group_user
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = NEW.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_id;
        IF (stop_flag = 0) THEN
            IF length(var_str_ids) = 0 THEN
                SET var_str_ids = CONCAT(var_str_ids, var_id);
            ELSE
                SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
            END IF;
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;

    IF (length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--删
DROP TRIGGER IF EXISTS tr_delete_alarm_group_user$
CREATE TRIGGER tr_delete_alarm_group_user
AFTER DELETE ON t_alarm_group_user
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = OLD.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_id;
        IF (stop_flag = 0) THEN
            IF length(var_str_ids) = 0 THEN
                SET var_str_ids = CONCAT(var_str_ids, var_id);
            ELSE
                SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
            END IF;
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;

    IF (length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--改
DROP TRIGGER IF EXISTS tr_update_alarm_group_user$
CREATE TRIGGER tr_update_alarm_group_user
AFTER UPDATE ON t_alarm_group_user
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_alarm_group_service AS TAGS
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGS.alarm_group_id = OLD.alarm_group_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    OPEN cur_ids;
    REPEAT
        fetch cur_ids into var_id;
        IF (stop_flag = 0) THEN
            IF length(var_str_ids) = 0 THEN
                SET var_str_ids = CONCAT(var_str_ids, var_id);
            ELSE
                SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
            END IF;
        END IF;
    UNTIL stop_flag = 1
    END REPEAT;
    CLOSE cur_ids;

    IF (length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--用户基本信息变更
DROP TRIGGER IF EXISTS tr_update_alarm_group_user$
CREATE TRIGGER tr_update_alarm_group_user
AFTER UPDATE ON t_user_info
FOR EACH ROW
BEGIN
    DECLARE var_str_ids text default '';
    DECLARE var_id  int(10) default 0;
    DECLARE stop_flag  tinyint(1) default 0;
    DECLARE cur_ids CURSOR FOR
        SELECT DISTINCT machine_id AS server_id
        FROM t_alarm_group_user AS TAGU
        INNER JOIN t_alarm_group_service AS TAGS ON TAGS.alarm_group_id = TAGU.alarm_group_id
        INNER JOIN t_service_info AS TSI ON TSI.service_id = TAGS.service_id AND TSI.service_type = TAGS.service_type 
        WHERE TAGU.user_id = OLD.user_id AND TSI.is_monitor = 1;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET stop_flag = 1;

    IF (NEW.user_name != OLD.user_name OR NEW.cellphone != OLD.cellphone OR NEW.email != OLD.email) THEN
        OPEN cur_ids;
        REPEAT
            fetch cur_ids into var_id;
            IF (stop_flag = 0) THEN
                IF length(var_str_ids) = 0 THEN
                    SET var_str_ids = CONCAT(var_str_ids, var_id);
                ELSE
                    SET var_str_ids = CONCAT(var_str_ids, ",", var_id);
                END IF;
            END IF;
        UNTIL stop_flag = 1
        END REPEAT;
        CLOSE cur_ids;
    END IF;

    IF (length(var_str_ids) > 0) THEN
        INSERT INTO t_temp_notice_info SET cmd = 1004, flag = 0, proto = CONCAT('server_id=', var_str_ids);
    END IF;
END$

--node基本信息变更
DROP TRIGGER IF EXISTS tr_node_conf_up$
DROP TRIGGER IF EXISTS tr_update_node_info$
CREATE TRIGGER tr_update_node_info
AFTER UPDATE ON t_node_info
FOR EACH ROW
BEGIN
    DECLARE var_server_id int(10) default 0;

    IF (NEW.update_interval != OLD.update_interval || NEW.listen_port != OLD.listen_port) THEN
        SELECT machine_id INTO var_server_id
            FROM t_service_info
            WHERE service_id = NEW.node_id AND service_type = 1 AND is_monitor = 1;
        IF (var_server_id > 0) THEN
            INSERT INTO t_temp_notice_info SET cmd = 1003, flag = 0, proto = CONCAT('server_id=', var_server_id);
        END IF;
    END IF;
END$


--db配置变更
DROP TRIGGER IF EXISTS tr_update_db_info$
CREATE TRIGGER tr_update_db_info
AFTER UPDATE ON t_db_info
FOR EACH ROW
BEGIN
    DECLARE var_server_id int(10) default 0;

    IF (NEW.port != OLD.port || NEW.socket != OLD.socket) THEN
        SELECT machine_id INTO var_server_id
            FROM t_service_info
            WHERE service_id = NEW.db_id AND service_type = 3 AND is_monitor = 1;
        IF (var_server_id > 0) THEN
            INSERT INTO t_temp_notice_info SET cmd = 1003, flag = 0, proto = CONCAT('server_id=', var_server_id);
        END IF;
    END IF;
END$


DELIMITER ;

