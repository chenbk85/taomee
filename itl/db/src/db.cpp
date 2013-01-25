/** 
 * ========================================================================
 * @file db.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-04
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <arpa/inet.h>
#include "proto.h"
#include "db_interface.h"
#include "itl_common.h"

int db_p_get_version(DEFAULT_ARG)
{
    //db_p_get_version_in * p_in = P_IN;
    //db_p_get_version_out * p_out = P_OUT;

    //DEBUG_LOG("%u", p_in->test);
    return 0;
}

int db_p_store_collect_data(DEFAULT_ARG)
{
    db_p_store_collect_data_in *p_in = P_IN;
    if (NULL == p_in)
    {
        ERROR_LOG("db_p_store_collect_data_in is NULL");
        return -1;
    }

    db_name_t *p_host = &p_in->host;
    db_name_t *p_metric = &p_in->metric;
    db_name_t *p_arg = &p_in->arg;
    itl_value_t *p_val = &p_in->value;
    if (0 == strcasecmp(g_log_node.node_tag, p_host->name))
    {
        INFO_LOG("host[%u:%s] metric[%u:%s] arg[%u:%s] metric[%u:%s]", 
            p_host->id, p_host->name, p_metric->id, p_metric->name,
            p_arg->id, p_arg->name, p_val->type, p_val->value);
    }

    return db_store_collect_data(p_host, p_metric, p_arg, p_val->value, p_val->_value_len);
}

int db_p_get_node_server_config(DEFAULT_ARG)
{
    db_p_get_node_server_config_in *p_in = P_IN;
    db_p_get_node_server_config_out *p_out = P_OUT;
    if (NULL == p_in || NULL == p_out)
    {
        ERROR_LOG("db_p_get_node_server_config_in/out is NULL");
        return -1;
    }
    char * ip_inside = p_in->node_ip;
    DEBUG_LOG("Start to get node[%s] server conf...", ip_inside);
    int ret_code = db_get_node_server_config(ip_inside, p_out);

    if (IS_LOG_NODE_IP(ip_inside))
    {
        INFO_LOG("-------------------Node[%s] server metric conf-------------------", ip_inside);
        INFO_LOG("host_id: %u", p_out->node_id);
        // INFO_LOG("head ip:port[%s:%u]", p_out->head_addr.ip, p_out->head_addr.port);
        INFO_LOG("so_num: %u", p_out->so_num);
        INFO_LOG("so_name_list: %s", p_out->so_name_list);
        int group_num = 1;
        vector_for_each( p_out->group_info, group_iter)
        {
            INFO_LOG("---------------group%d info---------------", group_num);
            INFO_LOG("collect_interval: %u", group_iter->collect_interval);
            int metric_num = 1;
            vector_for_each(group_iter->metric_config, metric_iter)
            {
                INFO_LOG("%d--->metric_id: %u", metric_num, metric_iter->metric_id);
                INFO_LOG("%d--->metric_name: %s", metric_num, metric_iter->metric_name);
                INFO_LOG("%d--->metric_arg: %s", metric_num, metric_iter->metric_arg);
                INFO_LOG("%d--->metric_type: %u", metric_num, metric_iter->metric_type);
                ++metric_num;
            }

            ++group_num;
        }
    }

    return ret_code;
}

int db_p_get_node_mysql_config(DEFAULT_ARG)
{
    db_p_get_node_mysql_config_in *p_in = P_IN;
    db_p_get_node_mysql_config_out *p_out = P_OUT;
    if (NULL == p_in || NULL == p_out)
    {
        ERROR_LOG("db_p_get_node_mysql_config_in/out is NULL");
        return -1;
    }
    char * ip_inside = p_in->node_ip;
    DEBUG_LOG("Start to get node[%s] mysql conf...", ip_inside);
    int ret_code = db_get_node_mysql_config(ip_inside, p_out);

    if (IS_LOG_NODE_IP(ip_inside))
    {
        INFO_LOG("=========================Node[%s] mysql metric conf=========================",
                ip_inside);
        INFO_LOG("host_id: %u", p_out->node_id);
        INFO_LOG("so_num: %u", p_out->so_num);
        INFO_LOG("so_name_list: %s", p_out->so_name_list);
        int instance_num = 1;
        vector_for_each(p_out->db_group_info, node_iter)
        {
            INFO_LOG("---------------instance%d info---------------", instance_num);
            INFO_LOG("prot: %u", node_iter->instance_info.port);
            INFO_LOG("sock: %s", node_iter->instance_info.sock);
            int group_num = 1;
            vector_for_each(node_iter->group_info, group_iter)
            {
                INFO_LOG("---------------group%d info---------------", group_num);
                INFO_LOG("collect_interval: %u", group_iter->collect_interval);
                int metric_num = 1;
                vector_for_each(group_iter->metric_config, metric_iter)
                {
                    INFO_LOG("%d--->metric_id: %u", metric_num, metric_iter->metric_id);
                    INFO_LOG("%d--->metric_name: %s", metric_num, metric_iter->metric_name);
                    INFO_LOG("%d--->metric_arg: %s", metric_num, metric_iter->metric_arg);
                    INFO_LOG("%d--->metric_type: %u", metric_num, metric_iter->metric_type);
                    ++metric_num;
                }
                ++group_num;
            }
        }
    }

    return ret_code;
}


int db_p_get_node_switch_config(DEFAULT_ARG)
{
    db_p_get_node_switch_config_in *p_in = P_IN;
    db_p_get_node_switch_config_out *p_out = P_OUT;
    if (NULL == p_in || NULL == p_out)
    {
        ERROR_LOG("db_p_get_node_switch_config_in/out is NULL");
        return -1;
    }
    uint32_t work_id = p_in->work_id;
    uint32_t work_num = p_in->work_num;
    DEBUG_LOG("start to get work_id[%u](work_num[%u]))switch conf...", work_id, work_num);
    int ret_code = db_get_switch_node_config(work_id, work_num, p_out);

    if (g_log_switch_flag)
    {
        INFO_LOG("=======================switch node conf=======================");
        INFO_LOG("work_id: %u", work_id);
        INFO_LOG("work_num: %u", work_num);
        INFO_LOG("so_num: %u", p_out->so_num);
        INFO_LOG("so_name_list: %s", p_out->so_name_list);
        int switch_num = 1;
        vector_for_each (p_out->switch_confs, switch_iter)
        {
            switch_node_conf_t *p_switch_conf = &(*switch_iter);
            INFO_LOG("================switch%d info================", switch_num);
            INFO_LOG("switch_id:   %u", p_switch_conf->switch_id);
            INFO_LOG("switch_tag:  %s", p_switch_conf->switch_tag);
            INFO_LOG("switch_type: %u", p_switch_conf->switch_type);
            INFO_LOG("switch_ip:   %s", p_switch_conf->switch_ip);
            INFO_LOG("community:   %s", p_switch_conf->switch_community);

            int group_num = 1;
            vector_for_each (p_switch_conf->group_info, group_iter)
            {
                group_config_t *p_group_conf = &(*group_iter);
                INFO_LOG("-----------group%d info-----------", group_num);
                INFO_LOG(">>>collect_interval: %u", p_group_conf->collect_interval);
                p_group_conf->collect_interval = 60;
                vector_for_each (p_group_conf->metric_config, metric_iter)
                {
                    metric_config_t *p_metric_conf = &(*metric_iter);
                    INFO_LOG(">>>>>>metric_id: %u", p_metric_conf->metric_id);
                    INFO_LOG("\tmetric_name: %s", p_metric_conf->metric_name);
                    INFO_LOG("\tmetric_arg: %s", p_metric_conf->metric_arg);
                    INFO_LOG("\tmetric_type: %u", p_metric_conf->metric_type);
                }
                INFO_LOG("----------------------------------");
                ++group_num;
            }
            INFO_LOG("=============================================");

            ++switch_num;
        }
        INFO_LOG("==============================================================");
    }

    return ret_code;
}


int db_p_get_node_alarm_config(DEFAULT_ARG)
{
    db_p_get_node_alarm_config_in *p_in = P_IN;
    db_p_get_node_alarm_config_out *p_out = P_OUT;
    if (NULL == p_in || NULL == p_out)
    {
        ERROR_LOG("db_p_get_alarm_config_in/out is NULL");
        return -1;
    }
    uint32_t host_id = p_in->node_id;
    uint32_t service_type = p_in->service_type;
    DEBUG_LOG("Start to get node[%u] alarm config...", host_id);
    int ret_code = db_get_node_alarm_config(host_id, service_type, p_out);

    if (host_id == (uint32_t)g_log_node.node_id)
    {
        INFO_LOG("-------------------Node[%u] alarm info-------------------", host_id);
        INFO_LOG("host_id: %u", p_out->host_id);
        INFO_LOG("host_tag: %s", p_out->host_tag);
        INFO_LOG("project_name: %s", p_out->project_name);
        int metric_num = 1;
        vector_for_each (p_out->alarm_metric_list, metric_iter)
        {
            INFO_LOG("---------------metric%d alarm conf---------------", metric_num);
            INFO_LOG("metric_id: %u", metric_iter->alarm_metric.id);
            INFO_LOG("metric_name: %s", metric_iter->alarm_metric.name);
            INFO_LOG("arg_id: %u", metric_iter->alarm_arg.id);
            INFO_LOG("arg_name: %s", metric_iter->alarm_arg.name);
            INFO_LOG("warning_val: %s", metric_iter->warning_val);
            INFO_LOG("critical_val: %s", metric_iter->critical_val);
            INFO_LOG("operator: %u", metric_iter->op);
            INFO_LOG("normal_interval: %u", metric_iter->normal_interval);
            INFO_LOG("abnormal_interval: %u", metric_iter->abnormal_interval);
            INFO_LOG("max_attempt: %u", metric_iter->max_attempt);
            INFO_LOG("no_alarm_range: %s", metric_iter->no_alarm_range);
            INFO_LOG("warning_way: %u", metric_iter->warning_way);
            INFO_LOG("critical_way: %u", metric_iter->critical_way);
            INFO_LOG("alarm_frequency: %s", metric_iter->alarm_frequency);
            INFO_LOG("mobile_list: %s", metric_iter->alarm_contact.mobile_list);
            INFO_LOG("email_list: %s", metric_iter->alarm_contact.email_list);
            INFO_LOG("rtx_list: %s", metric_iter->alarm_contact.rtx_list);
            ++metric_num;
        }
        INFO_LOG("---------------node alarm conf---------------");
        INFO_LOG("alarm_way: %u", p_out->node_contact.alarm_way);
        INFO_LOG("mobile_list: %s", p_out->node_contact.alarm_contact.mobile_list);
        INFO_LOG("email_list: %s", p_out->node_contact.alarm_contact.email_list);
        INFO_LOG("rtx_list: %s", p_out->node_contact.alarm_contact.rtx_list);
    }

    return ret_code;
}

int db_p_store_alarm_message_data(DEFAULT_ARG)
{
    db_p_store_alarm_message_data_in *p_in = P_IN;
    
    DEBUG_LOG("begin to db_p_get_alarm_message_data_in");
    
    if (NULL == p_in)
    {
        ERROR_LOG("db_p_get_alarm_message_data_in is null");
        return -1;
    } 

    return db_store_alarm_message_data(p_in);

}

//elva
int db_p_store_alarm_event_data(DEFAULT_ARG)
{
    db_p_store_alarm_event_data_in *p_in = P_IN;

    if(NULL == p_in)
    {
        ERROR_LOG("db_p_get_alarm_event_data_in is null");
        return -1;
    }

    return db_store_alarm_event_data(p_in);
}


int db_p_get_update_notice(DEFAULT_ARG)
{
    db_p_get_update_notice_out *p_out = P_OUT;
    if (NULL == p_out)
    {
        ERROR_LOG("db_p_get_update_notice_out is NULL");
        return -1;
    }
    DEBUG_LOG("Start to get config update notice...");

    return db_get_update_notice(p_out);
}

int db_p_store_host_status(DEFAULT_ARG)
{
    db_p_store_host_status_in *p_in = P_IN;
    if(NULL == p_in)
    {
        ERROR_LOG("db_p_store_host_status_in is NULL");
        return -1;
    }

    return db_store_host_status(p_in->node_id, p_in->node_type, p_in->node_status);
}


int db_p_get_node_head_config(DEFAULT_ARG)
{
    db_p_get_node_head_config_in * p_in = P_IN;
    db_p_get_node_head_config_out * p_out = P_OUT;
    if (NULL == p_in || NULL == p_out)
    {
        return -1;
    }

    char * ip_inside = p_in->node_ip;
    DEBUG_LOG("Start to get node[%s] head config...", ip_inside);
    int ret = db_get_node_head_config(ip_inside, p_out);

    if (IS_LOG_NODE_IP(ip_inside))
    {
        INFO_LOG("------------------- node[%s] head configuration -------------------", ip_inside);
        INFO_LOG("node_id: %u", p_out->node_id);
        INFO_LOG("host_tag: %s", p_out->host_tag);
        INFO_LOG("head[%s:%u]", p_out->head_addr.ip, p_out->head_addr.port);
    }

    return ret;
}


int db_p_get_node_control_config(DEFAULT_ARG)
{
    db_p_get_node_control_config_in * p_in = P_IN;
    db_p_get_node_control_config_out * p_out = P_OUT;
    if (NULL == p_in || NULL == p_out)
    {
        return -1;
    }

    char * ip_inside = p_in->node_ip;
    DEBUG_LOG("Start to get node[%s] control config...", ip_inside);
    int ret = db_get_node_control_config(ip_inside, p_out);

    if (IS_LOG_NODE_IP(ip_inside))
    {
        INFO_LOG("------------------- node[%s] control configuration -------------------", ip_inside);
        INFO_LOG("node_id: %u", p_out->node_id);
        INFO_LOG("control[%s:%u]", p_out->control_addr.ip, p_out->control_addr.port);
    }

    return ret;
}

int db_p_record_db_mgr_exec_result(DEFAULT_ARG)
{
    db_p_record_db_mgr_exec_result_in * p_in = P_IN;
    if (NULL == p_in)
    {
        return -1;
    }
    int ret = db_record_db_mgr_exec_result(p_in);

    return ret;
}





int db_p_get_node_mysql_instance(DEFAULT_ARG)
{
    db_p_get_node_mysql_instance_in * p_in = P_IN;
    db_p_get_node_mysql_instance_out * p_out = P_OUT;
    if (!p_in || !p_out)
    {
        return -1;
    }

    int ret = db_get_node_mysql_instance(p_in->node_ip, p_out);
    return ret;
}
