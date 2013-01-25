set names utf8;
USE db_itl;
UPDATE t_metric_info as mi set normal_interval=2*(select time_interval from t_metric_group_info as mgi where mgi.metric_group_id=mi.metric_group_id),retry_interval=(select time_interval from t_metric_group_info as mgi where mgi.metric_group_id=mi.metric_group_id),max_attempt=2; 
