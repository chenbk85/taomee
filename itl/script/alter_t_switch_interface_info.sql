SET NAMES UTF8;
USE db_itl;
ALTER TABLE t_switch_interface_info ADD COLUMN add_time INT UNSIGNED NOT NULL DEFAULT 0;
GRANT DELETE ON db_itl.t_switch_interface_info TO oaadmin@192.168.6.51;
