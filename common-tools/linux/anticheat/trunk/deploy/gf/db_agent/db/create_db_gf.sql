/* DROP DATABASE IF EXISTS anticheat; */
CREATE DATABASE IF NOT EXISTS `anticheat` DEFAULT CHARACTER SET UTF8;
USE `anticheat`;

CREATE TABLE IF NOT EXISTS `gf_main_anticheat` ( /* 上报记录 */
	`id` BIGINT UNSIGNED PRIMARY KEY NOT NULL AUTO_INCREMENT,
	`tw_ill_code` INT UNSIGNED NOT NULL,
	`sw_ill_code` INT UNSIGNED NOT NULL,
	`uid` INT UNSIGNED NOT NULL,
	`cmd` INT UNSIGNED NOT NULL,
	`cmd_recv_sec` TIMESTAMP NOT NULL,
	`ip` INT UNSIGNED,
	`port` SMALLINT UNSIGNED,
	`rpt_ip` INT UNSIGNED,
	`rpt_port` SMALLINT UNSIGNED,
	`tw_last_int` BIGINT,
	`tw_int_limit` BIGINT,
	`tw_start_int` BIGINT,
	`tw_count` BIGINT UNSIGNED,
	`tw_freq` FLOAT,
	`tw_freq_limit` BIGINT,
	`sw_last_int` BIGINT,
	`sw_int_limit` BIGINT,
	`sw_start_int` BIGINT,
	`sw_count` BIGINT UNSIGNED,
	`sw_freq` FLOAT,
	`sw_freq_limit` BIGINT
) ENGINE=MyISAM DEFAULT CHARSET=UTF8;

CREATE TABLE IF NOT EXISTS `gf_today_anticheat` ( /* 上报记录 */
	`id` BIGINT UNSIGNED PRIMARY KEY NOT NULL AUTO_INCREMENT,
	`tw_ill_code` INT UNSIGNED NOT NULL,
	`sw_ill_code` INT UNSIGNED NOT NULL,
	`uid` INT UNSIGNED NOT NULL,
	`cmd` INT UNSIGNED NOT NULL,
	`cmd_recv_sec` TIMESTAMP NOT NULL,
	`ip` INT UNSIGNED,
	`port` SMALLINT UNSIGNED,
	`rpt_ip` INT UNSIGNED,
	`rpt_port` SMALLINT UNSIGNED,
	`tw_last_int` BIGINT,
	`tw_int_limit` BIGINT,
	`tw_start_int` BIGINT,
	`tw_count` BIGINT UNSIGNED,
	`tw_freq` FLOAT,
	`tw_freq_limit` BIGINT,
	`sw_last_int` BIGINT,
	`sw_int_limit` BIGINT,
	`sw_start_int` BIGINT,
	`sw_count` BIGINT UNSIGNED,
	`sw_freq` FLOAT,
	`sw_freq_limit` BIGINT
) ENGINE=MyISAM DEFAULT CHARSET=UTF8;


CREATE TABLE IF NOT EXISTS `gf_tw_config` ( /* tw 配置 */
	`db_addr_ip` CHAR(16) NOT NULL,
	`db_addr_port` SMALLINT UNSIGNED,
	`min_tw_interval` BIGINT,
	`max_tw_freq` FLOAT,
	`update_tw_start_interval` INT
) ENGINE=InnoDB DEFAULT CHARSET=UTF8;

CREATE TABLE IF NOT EXISTS `gf_sw_config` ( /* sw 配置 */
	`cmd` INT UNSIGNED PRIMARY KEY NOT NULL,
	`min_sw_interval` BIGINT,
	`max_sw_freq` FLOAT,
	`update_sw_start_interval` INT,
	`label` CHAR(255)
) ENGINE=InnoDB DEFAULT CHARSET=UTF8;
