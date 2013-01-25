/** 
 * ========================================================================
 * @file db_interface.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-17
 * Modify $Date: 2012-11-15 11:12:57 +0800 (四, 15 11月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
#ifndef H_DB_INTERFACE_H_2012_07_17
#define H_DB_INTERFACE_H_2012_07_17

#include "proto.h"
#include "db_macro.h"
#include "config.h"


typedef struct {
    uint32_t group_id;
    uint32_t collect_interval;
} db_group_inter_t;

typedef struct {
    uint32_t warning_way;
    uint32_t critical_way;
    char no_alarm_range[ITL_MAX_STR_LEN];
    char alarm_frequency[ITL_MAX_STR_LEN];
    char alarm_mobile[ITL_MAX_STR_LEN];
    char alarm_email[ITL_MAX_STR_LEN];
    char alarm_rtx[ITL_MAX_STR_LEN];
} alarm_default_conf_t;

typedef struct {
    uint32_t start_time;
    uint32_t end_time;
} shield_slot_t;

typedef struct {
    uint32_t switch_id;
    char switch_tag[32];
    uint32_t switch_type;
    char switch_ip[16];
    char switch_community[64];
} switch_base_info_t;


int init_db(const char * host, uint16_t port, const char * db_name, const char * user, const char * password);


int fini_db();


int rollback();

int commit();

/** 
 * @brief 将mysql数据类型写入数据库
 * 
 * @param p_host    机器信息: id:name
 * @param p_metric  监控项: id:name
 * @param p_arg     参数: id:name
 * @param p_val     metric值: type:val
 * 
 * @return DB_SUCC:成功， DB_ERR:失败
 */
int db_store_collect_data(const db_name_t *p_host, const db_name_t *p_metric, const db_name_t *p_arg, char *p_val, uint32_t value_len);


/** 
 * @brief 保存发出去的告警消息到数据库 
 * 
 * @param p_data 接收包体
 * 
 * @return 
 */
int db_store_alarm_message_data(const db_p_store_alarm_message_data_in *p_data);


/** 
 * @brief 保存发生的告警事件到数据库
 * 
 * @param p_date 接收包体
 * 
 * @return 
 */
int db_store_alarm_event_data(const db_p_store_alarm_event_data_in *p_date);

char* get_alarm_type(const uint32_t type);
/** 
 * @brief 获取node服务器配置信息
 * 
 * @param host_ip   node所在服务器内网IP
 * @param p_out     返回包体
 * 
 * @return DB_SUCC:成功， DB_ERR:失败
 */
int db_get_node_server_config(const char *host_ip, db_p_get_node_server_config_out *p_out);

 /* @brief 获取node的mysql配置信息*/
int db_get_node_mysql_config(const char *host_ip, db_p_get_node_mysql_config_out *p_out);

 /* @brief 获取node的alarm配置信息*/
int db_get_node_alarm_config(uint32_t host_id, uint32_t service_type, db_p_get_node_alarm_config_out *p_out);

 /* @brief 获取node的交换机配置信息*/
int db_get_switch_node_config(uint32_t work_id, uint32_t work_num, db_p_get_node_switch_config_out *p_out);

 /* @brief 获取配置更新信息*/
int db_get_update_notice(db_p_get_update_notice_out *p_out);

 /* @brief 保存服务器状态*/
int db_store_host_status(uint32_t node_id, uint32_t node_type, uint32_t node_status);

 /* @brief 获取node的head配置信息 */
int db_get_node_head_config(const char * node_ip, db_p_get_node_head_config_out * p_out);


 /* @brief 获取node的control配置信息 */
int db_get_node_control_config(const char * node_ip, db_p_get_node_control_config_out * p_out);

 /* @brief 记录数据库管理执行SQL语句后的结构 */
int db_record_db_mgr_exec_result(const db_p_record_db_mgr_exec_result_in * p_in);


 /* @brief 获取node上mysql实例 */
int db_get_node_mysql_instance(const char * host_ip, db_p_get_node_mysql_instance_out * p_out);


#endif
