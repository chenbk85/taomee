SET names utf8;
USE db_itl;

DROP TABLE IF EXISTS t_mysql_error_log_info;
CREATE TABLE IF NOT EXISTS t_mysql_error_log_info(
mysql_host CHAR(16) NOT NULL COMMENT 'mysql host',
mysql_port SMALLINT UNSIGNED NOT NULL COMMENT 'mysql port', 
line_num INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'line number',
error_time INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'error time',
error_content VARCHAR(512) NOT NULL DEFAULT '-' COMMENT 'error content',
PRIMARY KEY(mysql_host,mysql_port,line_num,error_time),
INDEX(mysql_host,mysql_port,line_num,error_time)
)ENGINE = INNODB DEFAULT CHARSET = utf8;

DROP TABLE IF EXISTS t_mysql_innodb_deadlock_event;
CREATE TABLE IF NOT EXISTS t_mysql_innodb_deadlock_event(
mysql_host CHAR(16) NOT NULL COMMENT 'mysql host',
mysql_port SMALLINT UNSIGNED NOT NULL COMMENT 'mysql port', 
event_time INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'event time',
event_content VARCHAR(512) NOT NULL DEFAULT '-' COMMENT 'event content',
PRIMARY KEY(mysql_host,mysql_port,event_time),
INDEX(mysql_host,mysql_port,event_time)
)ENGINE = INNODB DEFAULT CHARSET = utf8;

DROP TABLE IF EXISTS t_mysql_locked_processlist;
CREATE TABLE IF NOT EXISTS t_mysql_locked_processlist(
mysql_host CHAR(16) NOT NULL COMMENT 'mysql host',
mysql_port SMALLINT UNSIGNED NOT NULL COMMENT 'mysql port', 
process_id INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'process id',
process_host CHAR(64) NOT NULL DEFAULT '-' COMMENT 'process host',
process_info VARCHAR(512) NOT NULL DEFAULT '-' COMMENT 'process info',
PRIMARY KEY(mysql_host,mysql_port,process_id),
INDEX(mysql_host,mysql_port,process_id)
)ENGINE = INNODB DEFAULT CHARSET = utf8;

GRANT UPDATE,INSERT ON db_itl.t_mysql_error_log_info TO oaadmin@192.168.6.51; 
GRANT UPDATE,INSERT ON db_itl.t_mysql_innodb_deadlock_event TO oaadmin@192.168.6.51; 
GRANT UPDATE,INSERT ON db_itl.t_mysql_locked_processlist TO oaadmin@192.168.6.51; 


