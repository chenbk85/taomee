SET names utf8;
USE db_itl;

DROP TABLE IF EXISTS t_mysql_info;
CREATE TABLE IF NOT EXISTS t_mysql_info(
mysql_host CHAR(16) NOT NULL COMMENT 'mysql host',
mysql_port SMALLINT UNSIGNED NOT NULL COMMENT 'mysql port', 
metric_name VARCHAR(127) NOT NULL DEFAULT 'UNKNOWN' COMMENT 'metric name',
metric_val  VARCHAR(127) NOT NULL DEFAULT 'UNKNOWN' COMMENT 'metric value',
PRIMARY KEY(mysql_host,mysql_port,metric_name),
INDEX(mysql_host,mysql_port,metric_name)
)ENGINE = INNODB DEFAULT CHARSET = utf8;


