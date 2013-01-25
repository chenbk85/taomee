SET NAMES UTF8;
USE db_itl;

ALTER TABLE t_alarm_log_info ADD COLUMN alarm_arg VARCHAR(255) NOT NULL DEFAULT '-';
ALTER TABLE t_alarm_event_info ADD COLUMN alarm_arg VARCHAR(255) NOT NULL DEFAULT '-';

ALTER TABLE t_alarm_log_info ADD COLUMN alarm_mail_list VARCHAR(512) NOT NULL DEFAULT '-';
ALTER TABLE t_alarm_log_info ADD COLUMN alarm_rtx_list  VARCHAR(255) NOT NULL DEFAULT '-';
ALTER TABLE t_alarm_log_info ADD COLUMN alarm_msg_list  VARCHAR(255) NOT NULL DEFAULT '-';
