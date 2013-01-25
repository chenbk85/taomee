/** 
 * ========================================================================
 * @file itl_node.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-04
 * $Rev: 51185 $
 * Modify $Date: 2012-11-15 18:04:16 +0800 (四, 15 11月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <dlfcn.h>
#include <sys/types.h>


#include "proto.h"
#include "itl_common.h"
#include "db.h"
#include "node.h"
#include "collect_interface.h"
#include "db_mgr_interface.h"





using namespace std;

int node_p_notify_config_change(DEFAULT_ARG)
{
    node_p_notify_config_change_in * p_in = P_IN;

    uint32_t config_type = p_in->config_type;

    switch (config_type)
    {
        case ITL_TYPE_SERVER:
        case ITL_TYPE_MYSQL:
            refresh_collect();
            break;
        default:
            break;

    }

    return NODE_SUCC;
}


int node_p_notify_mysql_instance_change(DEFAULT_ARG)
{
    refresh_mysql_instance();
    return NODE_SUCC;
}

int node_p_get_metric_data(DEFAULT_ARG)
{
    return NODE_SUCC;
}



int node_p_download_file(DEFAULT_ARG)
{
    node_p_download_file_in * p_in = P_IN;
    char * src = p_in->src_file;
    char * md5 = p_in->md5;
    char * dst = p_in->dst_file;
    


    int ret = download_file(dst, src, md5);

    return ret;
}


int node_p_restart_node(DEFAULT_ARG)
{


    restart_node();
    return 0;

}




int node_p_node_info(DEFAULT_ARG)
{
    node_p_node_info_out * p_out = P_OUT;
    p_out->init();
    p_out->node_id = g_node_id;
    STRNCPY(p_out->server_tag, g_server_tag, SERVER_TAG_LEN);
    p_out->node_ip = g_node_ip;
    STRNCPY(p_out->node_ip_str, g_node_ip_str, IP_STRING_LEN);
    p_out->start_time = g_start_timestamp;
    STRNCPY(p_out->start_time_str, timestamp2str(g_start_timestamp), 32);
    snprintf(p_out->compile_time_str, 32, "%s %s", __DATE__, __TIME__);
    snprintf(p_out->head, sizeof(p_out->head), 
            "%s:%u", g_head->get_ip(), g_head->get_port());


#define FILENAME_LEN 64
#define FILE_INFO(dst, p_file) \
    do { \
        STRNCPY((dst).file, (p_file)->m_full, FILENAME_LEN); \
        if ((p_file)->m_is_link) { \
            STRNCPY((dst).link, (p_file)->m_link, FILENAME_LEN); \
        } else { \
            STRNCPY((dst).link, (p_file)->m_full, FILENAME_LEN); \
        } \
        STRNCPY((dst).md5, (p_file)->m_md5, MD5_LEN + 1); \
    } while (0) 

    FILE_INFO(p_out->server_bin, g_server_bin);
    FILE_INFO(p_out->node_bin, g_node_bin);
    FILE_INFO(p_out->bench_conf, g_bench_conf);


    container_for_each(so_file_mgr, it)
    {
        metric_so_t * p_so = it->second;
        file_info_t info;

        FILE_INFO(info, &(p_so->file));

        p_out->so_file.push_back(info);
    }

    
    p_out->collect_flag = g_collect_flag ? 1 : 0;
    p_out->auto_update_flag = g_auto_update_flag ? 1 : 0;

    container_for_each(metric_group_mgr, group_it)
    {
        c_metric_group * p_group = group_it->second;
        group_info_t group_info;
        group_info.collect_interval = p_group->m_collect_interval;

        vector_for_each(p_group->m_metric_arg, arg_it)
        {
            c_metric_arg * p_arg = *arg_it;
            c_metric * p_metric = p_arg->m_metric;
            metric_simple_info_t metric_info;
            metric_info.metric_id = p_metric->get_id();
            metric_info.metric_class = p_metric->get_class();
            metric_info.metric_type = p_metric->get_type();
            const char * time_str = timestamp2str(p_arg->m_collect_time);
            STRNCPY(metric_info.collect_time, time_str, 32);
            metric_info._metric_name_len = strlen(p_metric->get_name()) + 1;
            STRNCPY(metric_info.metric_name, p_metric->get_name(), MAX_METRIC_NAME_LEN);
            if (NULL != p_metric->m_so)
            {
                STRNCPY(metric_info.so_filename, p_metric->m_so->file.m_filename, 64);

            }

            metric_info.fail_flag = p_arg->m_fail;
            metric_info._metric_arg_len = strlen(p_arg->m_arg) + 1;
            STRNCPY(metric_info.metric_arg, p_arg->m_arg, MAX_METRIC_ARG_LEN);

            STRNCPY(metric_info.compact_value, p_arg->m_value.get_string(p_metric->get_fmt()), 32);

            group_info.metrics.push_back(metric_info);
        }

        p_out->collect_group.push_back(group_info);

    }



    return NODE_SUCC;
}


int node_p_update_so(DEFAULT_ARG)
{
    node_p_update_so_in * p_in = P_IN;

    const char * so_url = p_in->so_url;
    const char * so_md5 = p_in->so_md5;
    const char * plugin_file = config_get_strval("plugin_file", "./bin/libnode.so");

    int ret = download_check_link(plugin_file, so_url, so_md5, check_so);
    if (0 != ret)
    {
        return ret;
    }



    restart_node();



    return 0;
}



int node_p_update_bench_conf(DEFAULT_ARG)
{
    node_p_update_bench_conf_in * p_in = P_IN;

    const char * conf_url = p_in->url;
    const char * conf_md5 = p_in->md5;
    const char * conf_file = "./conf/bench.conf";

    int ret = download_check_link(conf_file, conf_url, conf_md5);
    if (0 != ret)
    {
        return ret;
    }


    restart_node();



    return 0;
}


int node_p_update_metric_so(DEFAULT_ARG)
{
    node_p_update_metric_so_in * p_in = P_IN;

    const char * so_url = p_in->so_url;
    const char * so_md5 = p_in->so_md5;
    const char * so_filename = strrchr(so_url, '/');
    if (NULL == so_filename)
    {
        return NODE_ERR_REQ_DATA;
    }
    so_filename++;
    const char * so_dir = g_metric_so_dir;

    char target_so[PATH_MAX] = {0};
    snprintf(target_so, sizeof(target_so), "%s/%s", 
            so_dir, so_filename);

    int ret = download_check_link(target_so, so_url, so_md5, check_so);
    if (0 != ret)
    {
        return ret;
    }


    restart_node();



    return 0;
}


int node_p_restart_node_by_collect(DEFAULT_ARG)
{
    return restart_node();
}


int node_p_execute_command(DEFAULT_ARG)
{
    node_p_execute_command_in * p_in = P_IN;

    gint status = 0;
    gchar * standard_output = NULL;
    gchar * standard_error = NULL;
    GError * err = NULL;
    static char cmd[PATH_MAX];
    snprintf(cmd, sizeof(cmd), "sh -c '%s'", p_in->command);
    gboolean ret = g_spawn_command_line_sync(cmd, &standard_output, &standard_error, &status, &err);
    if (!ret)
    {
        ERROR_LOG("spawn command error: %s", err->message);
        g_error_free(err);
        return NODE_ERR_FORK;
    }

    node_p_execute_command_out * p_out = P_OUT;
    STRNCPY(p_out->standard_output, standard_output, sizeof(p_out->standard_output));
    STRNCPY(p_out->standard_error, standard_error, sizeof(p_out->standard_error));


    g_free(standard_output);
    g_free(standard_error);

    return NODE_SUCC;
}



int node_p_switch_collect(DEFAULT_ARG)
{
    g_collect_flag = !g_collect_flag;

    return NODE_SUCC;
}

int node_p_switch_auto_update(DEFAULT_ARG)
{
    g_auto_update_flag = !g_auto_update_flag;
    return NODE_SUCC;
}


int node_p_check_update(DEFAULT_ARG)
{
    if (check_update())
    {
        restart_node();
    }

    return NODE_SUCC;
}

int node_p_upload_file_by_ftp(DEFAULT_ARG)
{
    node_p_upload_file_by_ftp_in * p_in = P_IN;
    int ret = upload_file(p_in->local_file, p_in->ftp_url);
    return ret;
}


int node_p_get_version(DEFAULT_ARG)
{
    return 0;
}
