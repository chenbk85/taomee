/**
 * =====================================================================================
 *       @file  db_operator.h
 *      @brief  
 *
 *      封装的数据库操作函数类
 *
 *   @internal
 *     Created  2011-04-18 11:13:42
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason, mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef DB_OPERATOR_H
#define DB_OPERATOR_H

#include "defines.h"
#include "proto.h"
#include "lib/c_mysql_iface.h"

/** 
* @brief   获取交换机配置更新标志位
* @param   db_conn  数据库连接
* @param   action   更新操做
* @return  true 有更新 false 无更新
*/
bool db_get_update_status(c_mysql_iface* db_conn, unsigned char *action);

/** 
* @brief   设置head更新标志位
* @param   db_conn  数据库连接
* @return  0-success -1-failed
*/
int db_set_update_status(c_mysql_iface* db_conn);

/** 
* @brief   从数据库中获取metric的特殊报警相关信息
* @param   p_metric_set 保存metric_alarm_t结构的vector指针
* @param   db_conn      数据库连接
* @return  0-success -1-failed
*/
int db_get_specified_metric_alarm_info(metric_alarm_vec_t *p_metric_set, c_mysql_iface* db_conn);

/** 
* @brief   从数据库中获取metric的缺省报警相关信息
* @param   p_metric_set 保存metric_alarm_t结构的vector指针
* @param   db_conn      数据库连接
* @return  0-success -1-failed
*/
int db_get_default_metric_alarm_info(metric_alarm_vec_t *p_metric_set, c_mysql_iface* db_conn);

/** 
* @brief   从数据库中获取metric信息
* @param   p_metric_set 保存metric结构的vector指针
* @param   db_conn     数据库连接
* @return  0-success -1-failed
*/
int db_get_metric_info(metric_info_vec_t *p_metric_set, c_mysql_iface* db_conn);

/** 
* @brief   从数据库中获取switch信息
* @param   p_metric_group_set 保存switch group结构的vector指针
* @param   switch_group_num   交换机组的个数
* @param   db_conn     数据库连接
* @return  0-success -1-failed
*/
int db_get_switch_info(switch_group_vec_t *p_switch_group_set, unsigned int switch_group_num, c_mysql_iface* db_conn);

/** 
* @brief   从数据库中获取交换机接口的报警信息
* @param   p_if_alarm_map     保存交换机对应的报警信息
* @param   db_conn            数据库连接
* @return  0-success -1-failed
*/
int db_get_interface_alarm_info(if_alarm_map_t *p_if_alarm_map, c_mysql_iface* db_conn);

/** 
* @brief   更新数据库中一个switch的一个interface的信息
* @param   switch_ip  switch ip
* @param   if_idx     interface index
* @param   if_status  interface status
* @param   if_desc    interface desc
* @return  0-success -1-failed
*/
int db_update_switch_interface_info(c_mysql_iface *db_conn, unsigned int switch_id, unsigned int if_idx, unsigned int if_status, const char *if_desc, unsigned int if_type, unsigned int if_speed, unsigned int add_time);

/** 
* @brief   删除数据库中一个switch的过期的interface的信息
* @param   switch_id  switch id
* @param   add_time   最近一次更新的时间
* @return  0-success -1-failed
*/
int db_delete_overdue_switch_interface_info(c_mysql_iface *db_conn, unsigned int switch_id, unsigned int add_time);
#endif

