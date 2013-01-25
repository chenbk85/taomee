-- MySQL dump 10.11
--
-- Host: localhost    Database: db_itl
-- ------------------------------------------------------
-- Server version	5.0.51a-24+lenny2-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `db_itl`
--

USE `db_itl`;

--
-- Table structure for table `t_address_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_address_info`(
  `address_id` int(10) NOT NULL auto_increment COMMENT '地址id',
  `address_name` varchar(255) NOT NULL COMMENT '地址',
  `areas` varchar(32) NOT NULL COMMENT '区域',
  `address_desc` varchar(255) NOT NULL COMMENT '描述',
  `address_log_date` int(10) NOT NULL COMMENT '创建记录时间',
  PRIMARY KEY  (`address_id`),
  UNIQUE KEY `address_name` (`address_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='地址配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_alarm_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_alarm_info` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `metric_name` varchar(128) NOT NULL,
  `node_id` int(10) unsigned NOT NULL,
  `alarm_expr` varchar(256) NOT NULL,
  `alarm_formular` varchar(256) NOT NULL,
  `warning_way` tinyint(2) NOT NULL,
  `critical_way` tinyint(2) NOT NULL,
  `contact_mail_list` varchar(256) NOT NULL,
  `contact_phone_list` varchar(256) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_asset_status_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_asset_status_info` (
  `status_id` int(10) NOT NULL auto_increment COMMENT '资产状态id',
  `status_name` varchar(64) NOT NULL COMMENT '资产状态名称',
  PRIMARY KEY  (`status_id`),
  UNIQUE KEY `status_name` (`status_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='资产状态配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_asset_type_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_asset_type_info` (
  `asset_type_id` int(10) NOT NULL auto_increment COMMENT '资产类型id',
  `asset_type_name` varchar(64) NOT NULL COMMENT '名称',
  `asset_desc` varchar(255) NOT NULL,
  `asset_type_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`asset_type_id`),
  UNIQUE KEY `asset_type_name` (`asset_type_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='资产类型配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_bar_default_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_bar_default_info` (
  `user_id` int(10) NOT NULL ,
  `visible` varchar(512) NOT NULL ,
  `table_name` varchar(50) NOT NULL ,
  PRIMARY KEY  (`user_id`,`table_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_bin_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
drop table if EXISTS `t_bin_info`;
CREATE TABLE `t_bin_info` (
  `bin_id` int(11) NOT NULL auto_increment ,
  `bin_name` varchar(128) default NULL ,
  `bin_version` char(8) default NULL ,
  `is_32bit` tinyint(4) default NULL ,
  `path` varchar(255) default NULL,
  `type` tinyint(4) default NULL ,
  `user_id` int(11) NOT NULL ,
  `bin_log_date` int(11) NOT NULL ,
  PRIMARY KEY  (`bin_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_brand_type_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_brand_type_info` (
  `brand_id` int(10) NOT NULL auto_increment COMMENT '品牌类型id',
  `brand_name` varchar(64) NOT NULL COMMENT '品牌名字',
  PRIMARY KEY  (`brand_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='品牌类型配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_business_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_business_info` (
  `module_id` int(10) NOT NULL auto_increment COMMENT '模块id',
  `module_name` varchar(64) NOT NULL COMMENT '模块名称',
  `module_leader_id` int(10) NOT NULL COMMENT '负责人',
  `module_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`module_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='业务模块信息表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_cabinet_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_cabinet_info` (
  `cabinet_id` int(10) NOT NULL auto_increment COMMENT '机柜id',
  `cabinet_name` varchar(64) NOT NULL,
  `idc_id` int(10) NOT NULL COMMENT 'idcid',
  `cabinet_electricity` float NOT NULL COMMENT '电流',
  PRIMARY KEY  (`cabinet_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='机柜信息表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_cluster_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
drop table if EXISTS `t_cluster_info`;
CREATE TABLE `t_cluster_info` (
  `cluster_id` int(11) NOT NULL auto_increment ,
  `module_id` int(11) default NULL,
  `cluster_log_date` int(11) NOT NULL COMMENT ,
  PRIMARY KEY  (`cluster_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_company_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_company_info` (
  `company_id` int(10) NOT NULL auto_increment COMMENT '公司id',
  `company_name` varchar(64) NOT NULL COMMENT '公司名称',
  `company_desc` varchar(255) NOT NULL,
  `address_id` int(10) NOT NULL COMMENT '地址id',
  `company_postcode` int(10) NOT NULL COMMENT '邮编',
  `company_mail` varchar(64) NOT NULL COMMENT '公司邮箱',
  `company_call` varchar(32) NOT NULL COMMENT '电话号码',
  `company_fax` varchar(32) NOT NULL COMMENT '传真',
  `web_site` varchar(128) NOT NULL COMMENT '网站',
  `company_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`company_id`),
  UNIQUE KEY `company_name` (`company_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='公司信息表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_department_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_department_info` (
  `department_id` int(10) NOT NULL COMMENT '部门id',
  `department_name` varchar(64) NOT NULL COMMENT '部门名称',
  `parent_id` int(10) NOT NULL COMMENT '父级部门id',
  `department_desc` varchar(255) NOT NULL COMMENT '描述',
  `department_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`department_id`),
  KEY `parent_id` (`parent_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='部门信息表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_department_user`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_department_user` (
  `user_id` int(10) NOT NULL COMMENT '用户id',
  `department_id` int(10) NOT NULL COMMENT '部门id',
  PRIMARY KEY  (`user_id`,`department_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='部门用户关联表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_device_failure_history`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_device_failure_history` (
  `failure_id` int(10) NOT NULL auto_increment COMMENT '设备故障id',
  `user_id` int(10) NOT NULL COMMENT '操作人',
  `device_type` tinyint(2) NOT NULL COMMENT '设备类型（1服务器，2交换机）',
  `type_id` int(10) NOT NULL COMMENT '类型下设备id',
  `status` int(10) NOT NULL,
  `remark` varchar(512) NOT NULL COMMENT '备注',
  `failure_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`failure_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='设备故障历史记录表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_firewall_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_firewall_info` (
  `firewall_id` int(10) NOT NULL auto_increment COMMENT '编号id',
  `pattern_id` int(10) NOT NULL COMMENT '型号id',
  `ip_address` varchar(64) NOT NULL COMMENT 'ip地址',
  `brand_id` int(10) NOT NULL COMMENT '品牌类型',
  `network_id` int(10) NOT NULL COMMENT '网络带宽id',
  `remark` varchar(512) NOT NULL COMMENT '备注',
  `firewall_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`firewall_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='防火墙表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_head_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
drop table if EXISTS `t_head_info`;
CREATE TABLE  `t_head_info` (
  `head_id` int(11) NOT NULL auto_increment ,
  `parent_id` int(11) default NULL ,
  `ip_inside` char(16) default NULL,
  `log_id` int(11) NOT NULL,
  `summary_interval` tinyint(4) default NULL,
  `trust_hosts` varchar(512) default NULL,
  `rrd_rootdir` varchar(255) default NULL,
  `url_id` int(11) NOT NULL ,
  `head_log_date` int(11) NOT NULL ,
  `proj_id` int(10) NOT NULL,
  PRIMARY KEY  (`head_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_idc_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_idc_info` (
  `idc_id` int(10) NOT NULL auto_increment COMMENT 'idcid',
  `idc_name` varchar(64) NOT NULL COMMENT 'idc名字',
  `address_id` int(10) NOT NULL COMMENT 'idc地址',
  `tech_contact_id` int(10) NOT NULL COMMENT '技术联系人',
  `bandwidth` int(10) NOT NULL COMMENT '带宽',
  `idc_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`idc_id`),
  UNIQUE KEY `idc_name` (`idc_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='IDC信息表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_kernel_version_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_kernel_version_info` (
  `kernel_id` int(10) NOT NULL auto_increment COMMENT '内核id',
  `kernel_name` varchar(64) NOT NULL,
  PRIMARY KEY  (`kernel_id`),
  UNIQUE KEY `kerenl_name` (`kernel_name`),
  UNIQUE KEY `kernel_name` (`kernel_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='内核版本配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_leader_history`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_leader_history` (
  `history_id` int(10) NOT NULL auto_increment COMMENT '记录id',
  `leader_type` tinyint(2) NOT NULL COMMENT '负责人类型（1项目，2模块）',
  `type_id` int(10) NOT NULL COMMENT '某类型下所对应表id',
  `leader_id` int(10) NOT NULL COMMENT '负责人',
  `history_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`history_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='负责人历史变更表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_log_history_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_log_history_info` (
  `log_id` int(10) NOT NULL auto_increment COMMENT '记录id',
  `device_type` tinyint(2) NOT NULL COMMENT '设备类型（1服务器，2交换机）',
  `type_id` int(10) NOT NULL COMMENT '类型下设备id',
  `history` varchar(255) NOT NULL COMMENT '历史记录',
  `operate_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`log_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='设备操作历史记录表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_log_monitor_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_log_monitor_info` (
  `log_id` int(11) NOT NULL COMMENT '1:head,2:node',
  `log_count` int(11) NOT NULL ,
  `log_dir` varchar(255) NOT NULL ,
  `log_lvl` tinyint(4) NOT NULL ,
  `log_prefix` varchar(255) NOT NULL ,
  `log_size` int(11) NOT NULL ,
  `create_log_date` int(11) NOT NULL ,
  PRIMARY KEY  (`log_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_mail_server_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_mail_server_info` (
  `mail_server_id` int(10) NOT NULL auto_increment COMMENT '邮件服务器id',
  `mail_server_name` varchar(64) NOT NULL COMMENT '名称',
  `server_address` varchar(255) NOT NULL COMMENT '服务器地址',
  `user_name` varchar(32) NOT NULL COMMENT '用户名',
  `password` varchar(32) NOT NULL COMMENT '密码',
  `port` int(10) NOT NULL COMMENT '端口',
  `mail_server_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`mail_server_id`),
  UNIQUE KEY `mail_server_name` (`mail_server_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='邮件服务器配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_message_center_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_message_center_info` (
  `message_id` int(10) NOT NULL auto_increment COMMENT '短信id',
  `message_name` varchar(64) NOT NULL COMMENT '名称',
  `url_address` varchar(255) NOT NULL COMMENT 'url地址',
  `user_name` varchar(32) NOT NULL COMMENT '用户名',
  `password` varchar(32) NOT NULL COMMENT '密码',
  `user_key` varchar(255) NOT NULL,
  `message_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`message_id`),
  UNIQUE KEY `message_name` (`message_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='短信中心配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_metric_group_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
drop table if EXISTS `t_metric_group_info`;
CREATE TABLE `t_metric_group_info` (
  `metric_group_id` int(11) NOT NULL auto_increment,
  `metric_group_name` varchar(128) NOT NULL,
  `time_threshold` int(11) default '0',
  `time_interval` int(11) default '0',
  `metric_group_date` int(11) NOT NULL ,
  `default_flag` tinyint(2) NOT NULL,
  PRIMARY KEY  (`metric_group_id`),
  UNIQUE KEY `metric_group_name` (`metric_group_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_metric_group_node`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_metric_group_node` (
  `node_id` int(11) NOT NULL,
  `metric_group_id` int(11) NOT NULL,
  PRIMARY KEY  (`node_id`,`metric_group_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_metric_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
drop table if EXISTS `t_metric_info`;
CREATE TABLE  `t_metric_info` (
  `metric_name` varchar(128) NOT NULL,
  `metric_group_id` int(11) default NULL ,
  `so_id` int(11) default NULL ,
  `value_threshold` int(11) default '0',
  `metric_title` varchar(255) default NULL,
  `metric_log_date` int(11) default NULL,
  `metrictype_id` tinyint(2) default NULL,
  `argument` varchar(256) default NULL,
  PRIMARY KEY  (`metric_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_network_bandwidth_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_network_bandwidth_info` (
  `network_id` int(10) NOT NULL auto_increment COMMENT '编号id',
  `bandwidth_capacity` int(10) NOT NULL COMMENT '带宽容量',
  `supplier_id` int(10) NOT NULL COMMENT '供应商id',
  `operator` varchar(64) NOT NULL COMMENT '运营商',
  `network_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`network_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='网络带宽表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_node_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
drop table if EXISTS `t_node_info`;
CREATE TABLE `t_node_info` (
  `node_id` int(11) NOT NULL auto_increment, 
  `server_id` int(11) NOT NULL,
  `log_id` tinyint(2) NOT NULL default '2',
  `update_interval` int(11) NOT NULL default '3600',
  `host_dmax` int(11) default '0',
  `listen_port` int(11) default '55000',
  `heartbeat_time` int(11) NOT NULL default '20' ,
  `heartbeat_threshold` int(11) NOT NULL default '0' ,
  `node_log_date` int(11) NOT NULL ,
  `contact_mail_list` varchar(256) NOT NULL,
  `contact_phone_list` varchar(256) NOT NULL,
  `down_way` tinyint(2) default '1',
  `alarm_expr` varchar(256) NOT NULL default 'f(n)=f(n-1)+f(n-2),f(1)=300,f(2)=600',
  `set_ip` tinyint(2) NOT NULL default '1',
  PRIMARY KEY  (`node_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_notice_board_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_notice_board_info` (
  `board_id` int(10) NOT NULL auto_increment COMMENT '公告id',
  `content` varchar(512) NOT NULL COMMENT '公告内容',
  `board_log_date` int(10) NOT NULL COMMENT '创建记录时间',
  PRIMARY KEY  (`board_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='公告表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_pattern_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_pattern_info` (
  `pattern_id` int(10) NOT NULL auto_increment COMMENT '型号id',
  `pattern_name` varchar(64) NOT NULL COMMENT '型号名称',
  `brand_id` int(10) NOT NULL COMMENT '品牌类型id',
  PRIMARY KEY  (`pattern_id`),
  UNIQUE KEY `pattern_name` (`pattern_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='型号配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_priority_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_priority_info` (
  `priority_id` int(10) NOT NULL auto_increment ,
  `priority_name` varchar(64) NOT NULL ,
  PRIMARY KEY  (`priority_id`),
  UNIQUE KEY `priority_name` (`priority_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_project_business`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_project_business` (
  `project_id` int(10) NOT NULL COMMENT '项目id',
  `module_id` int(10) NOT NULL COMMENT '模块id',
  PRIMARY KEY  (`project_id`,`module_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='项目模块关联表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_project_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_project_info` (
  `project_id` int(10) NOT NULL auto_increment COMMENT '项目id',
  `project_name` varchar(64) NOT NULL COMMENT '项目名称',
  `project_leader_id` int(10) NOT NULL,
  `project_type` int(10) NOT NULL,
  `project_desc` varchar(255) NOT NULL COMMENT '描述',
  `project_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`project_id`),
  UNIQUE KEY `project_name` (`project_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='项目信息表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_project_type_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_project_type_info` (
  `project_type_id` int(10) NOT NULL auto_increment COMMENT '项目类型id',
  `project_type_name` varchar(64) NOT NULL COMMENT '项目类型名称',
  PRIMARY KEY  (`project_type_id`),
  UNIQUE KEY `project_type_name` (`project_type_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='项目类型配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_project_user`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_project_user` (
  `project_id` int(10) NOT NULL COMMENT '项目id',
  `user_id` int(10) NOT NULL COMMENT '用户id',
  PRIMARY KEY  (`project_id`,`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='项目人员联系表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_rank_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_rank_info` (
  `rank_id` int(10) NOT NULL auto_increment,
  `rank_name` varchar(64) NOT NULL COMMENT '等级名称',
  PRIMARY KEY  (`rank_id`),
  UNIQUE KEY `rank_name` (`rank_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='等级配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_server_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_server_info` (
  `server_id` int(10) NOT NULL auto_increment COMMENT '服务器id',
  `ip_outside` varchar(64) NOT NULL COMMENT '外网ip',
  `server_tag` varchar(64) NOT NULL COMMENT '服务标签',
  `cpu` varchar(32) NOT NULL COMMENT 'cpu',
  `harddisk_count` int(10) NOT NULL COMMENT '硬盘数目',
  `harddisk_cap` int(10) NOT NULL COMMENT '硬盘大小',
  `raid` int(10) NOT NULL COMMENT '0-5',
  `pur_date` int(10) NOT NULL COMMENT '购入日期',
  `switch_id` int(10) NOT NULL COMMENT '交换机id',
  `status_id` int(10) NOT NULL COMMENT '资产状态id',
  `kernel_id` int(10) NOT NULL COMMENT '内核版本id',
  `version_id` int(10) NOT NULL COMMENT '系统版本id',
  `module_id` int(10) NOT NULL COMMENT '模块id',
  `project_id` int(10) NOT NULL COMMENT '项目id',
  `address_id` int(10) NOT NULL COMMENT '地址id',
  `department_id` int(10) NOT NULL,
  `user_id` int(10) NOT NULL COMMENT '使用者',
  `wait_approve` tinyint(2) NOT NULL default '0' COMMENT '等待状态',
  `memory` int(10) NOT NULL COMMENT '内存',
  `pattern_id` int(10) NOT NULL COMMENT '型号',
  `ip_inside` varchar(64) NOT NULL COMMENT '内网ip',
  `ipmi` varchar(64) NOT NULL COMMENT 'IPMI地址',
  `specification` varchar(32) NOT NULL COMMENT '规格',
  `server_electricity` float NOT NULL COMMENT '电流',
  `added_date` int(10) NOT NULL COMMENT '上架时间',
  `remark` varchar(512) NOT NULL COMMENT '备注',
  PRIMARY KEY  (`server_id`),
  UNIQUE KEY `server_tag` (`server_tag`)
) ENGINE=InnoDB AUTO_INCREMENT=52 DEFAULT CHARSET=utf8 COMMENT='服务器信息表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_so_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
drop table if exists `t_so_info`;
CREATE TABLE  `t_so_info` (
  `so_id` int(11) NOT NULL auto_increment ,
  `so_name` varchar(128) default NULL ,
  `so_version` varchar(8) default NULL ,
  `is_32bit` tinyint(4) default NULL ,
  `path` varchar(255) default NULL ,
  `user_id` int(11) NOT NULL ,
  `so_log_date` int(11) NOT NULL ,
  PRIMARY KEY  (`so_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_sql_log_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_sql_log_info` (
  `sql_id` int(11) NOT NULL auto_increment ,
  `sql_query` varchar(512) NOT NULL ,
  `user_id` int(11) NOT NULL ,
  `sql_log_date` int(11) NOT NULL ,
  PRIMARY KEY  (`sql_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_storage_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_storage_info` (
  `storage_id` int(10) NOT NULL auto_increment COMMENT '存储id',
  `pattern_id` int(10) NOT NULL COMMENT '型号id',
  `brand_id` int(10) NOT NULL COMMENT '品牌类型',
  `capacity` int(10) NOT NULL COMMENT '容量',
  `status_id` int(10) NOT NULL,
  `remark` varchar(512) NOT NULL COMMENT '备注',
  `storage_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`storage_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='存储表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_supplier_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_supplier_info`(
  `supplier_id` int(10) NOT NULL auto_increment COMMENT '供应商id',
  `supplier_name` varchar(64) NOT NULL COMMENT '供应商名字',
  `credit` varchar(255) NOT NULL COMMENT '资质',
  `contact` varchar(32) NOT NULL COMMENT '联系人',
  `telephone` varchar(32) NOT NULL COMMENT '联系电话',
  `supplier_log_date` int(10) NOT NULL COMMENT '记录创建时间',
  PRIMARY KEY  (`supplier_id`),
  UNIQUE KEY `supplier_name` (`supplier_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='供应商信息表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_switch_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_switch_info` (
  `switch_id` int(11) NOT NULL auto_increment COMMENT '交换机id',
  `switch_name` varchar(64) NOT NULL,
  `cabinet_id` int(10) NOT NULL COMMENT '机柜id',
  `pattern_id` int(10) NOT NULL COMMENT '交换机类型',
  `inerf_count` int(10) NOT NULL COMMENT '接口数',
  `switch_func` tinyint(2) NOT NULL COMMENT '交换机作用\n1-内网 2-外网 3-内外合一',
  `status_id` int(10) NOT NULL,
  `remark` varchar(512) NOT NULL COMMENT '备注',
  PRIMARY KEY  (`switch_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='交换机信息表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_system_version_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_system_version_info` (
  `version_id` int(10) NOT NULL auto_increment COMMENT '版本id',
  `version_name` varchar(64) NOT NULL COMMENT '版本名称',
  PRIMARY KEY  (`version_id`),
  UNIQUE KEY `version_name` (`version_name`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='系统版本配置表';
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_temp_notice_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS `t_temp_notice_info` (
  `temp_id` int(11) NOT NULL auto_increment,
  `cmd` int(11) NOT NULL,
  `proto` varchar(512) NOT NULL ,
  `flag` tinyint(4) NOT NULL ,
  PRIMARY KEY  (`temp_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_url_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
drop table if exists `t_url_info`;
CREATE TABLE  `t_url_info`(
  `url_id` int(11) NOT NULL auto_increment ,
  `url_name` varchar(255) NOT NULL ,
  `ip_inside` char(16) NOT NULL ,
  `url_log_date` int(11) NOT NULL ,
  `url` varchar(255) NOT NULL,
  PRIMARY KEY  (`url_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 ;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `t_user_info`
--

SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE IF NOT EXISTS`t_user_info` (
  `user_id` int(10) NOT NULL COMMENT '用户id',
  `user_name` varchar(32) NOT NULL COMMENT '英文名',
  `chinese_name` varchar(32) NOT NULL COMMENT '中文名',
  `telephone` varchar(32) NOT NULL COMMENT '电话号码',
  `cellphone` varchar(20) NOT NULL COMMENT '手机号',
  `job_position` varchar(64) NOT NULL COMMENT '职务',
  `email` varchar(128) NOT NULL COMMENT '邮箱',
  `gender` tinyint(2) NOT NULL COMMENT '性别（1男，0女）',
  `status` tinyint(2) NOT NULL COMMENT '状态（1激活，0未激活，-1删除）',
  PRIMARY KEY  (`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户信息表';
SET character_set_client = @saved_cs_client;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2011-03-10 10:49:47
