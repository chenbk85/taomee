/** 
 * ========================================================================
 * @file collect.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-06
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "collect.h"
#include "metric.h"
#include "itl_timer.h"
#include "metric_group.h"
#include "itl_common.h"
#include "itl_head.h"
#include "../proto.h"
#include "../db.h"



#define SO_LOAD_FUN(h, fptr, fname) \
    do { \
        fptr = (typeof(fptr))dlsym(h, fname); \
        if (!fptr) { \
            dlerror(); \
            dlclose(h); \
            fptr = NULL; \
            ERROR_LOG("dlsym no find:%s", fname); \
            return -1; \
        } \
    } while (0) \

#define SO_LOAD_FUN_IGNORE(h, fptr, fname) \
    do { \
        fptr = (typeof(fptr))dlsym(h, fname); \
        if (!fptr) { \
            dlerror(); \
        } \
    } while (0) \


using namespace taomee;

static char g_pkg_buf[PKG_BUF_SIZE];
bool g_inited = false;


// 采集用的timer
timer_head_t g_collect_timer;
timer_head_t g_collect_event;


int register_so(metric_so_t * p_so, void * param);
int route_metric_group(void * owner, void * data);

int find_metric_by_name(c_metric * p_metric, const void * param)
{
    const char * p_name = reinterpret_cast< const char * >(param);
    int ret = strncmp(p_metric->get_name(), p_name, MAX_METRIC_NAME_LEN);
    if (0 == ret)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int init_server_collect(Cmessage * c_out)
{

    db_p_get_node_server_config_out * p_out = P_OUT;





    if (-1 == access(g_metric_so_dir, F_OK))
    {
        if (0 != mkdir(g_metric_so_dir, 0755))
        {
            ERROR_LOG("make so dir %s failed, %s", g_metric_so_dir, strerror(errno));
            return -1;
        }
    }


    char * str = p_out->so_name_list;
    char * save_ptr = NULL;
    char * token = NULL;
    while (NULL != (token = strtok_r(str, ";", &save_ptr)))
    {

        char so_file[PATH_MAX] = {0};
        snprintf(so_file, PATH_MAX, "%s/%s",
                g_metric_so_dir, token);

        if (-1 == access(so_file, F_OK | R_OK))
        {
            if (0 != download_metric_so(token))
            {
                ERROR_LOG("metric so: %s must exist and have F_OK | R_OK permission, reason: %s", 
                        so_file, strerror(errno));
                return -1;

            }
        }


        metric_so_t * p_so = new metric_so_t(so_file);
        if (NULL == p_so)
        {
            ERROR_LOG("new metric_so_t failed, %s", strerror(errno));
            return -1;
        }

        

        if (0 != register_so(p_so, NULL))
        {
            delete p_so;
            p_so = NULL;
            return -1;

        }




        so_file_mgr.insert_object(so_file_mgr.size(), p_so);


        str = NULL;
    }




    return 0;
}


int init_mysql_collect(Cmessage * c_out)
{
    db_p_get_node_mysql_config_out * p_out = P_OUT;

    if (p_out->db_group_info.empty())
    {
        return 0;
    }


    std::vector<mysql_so_param_t> config_vec;

    vector_for_each(p_out->db_group_info, it)
    {
        db_node_config_t * p_info = &(*it);
        mysql_so_param_t param;
        param.port = p_info->instance_info.port;
        STRNCPY(param.sock, p_info->instance_info.sock, SOCK_LEN);
        vector_for_each(p_info->group_info, group_it)
        {
            vector_for_each((*group_it).metric_config, metric_it)
            {
                const char * metric_name = (*metric_it).metric_name;
                param.metric_vec.push_back(std::string(metric_name));
            }
        }

        config_vec.push_back(param);
    }


    static const char * filename = "libmysql.so";
    char so_file[PATH_MAX] = {0};

    snprintf(so_file, PATH_MAX, "%s/%s",
            g_metric_so_dir, filename);

    if (-1 == access(so_file, F_OK | R_OK))
    {
        if (0 != download_metric_so(filename))
        {
            ERROR_LOG("metric so: %s must exist and have F_OK | R_OK permission, reason: %s", 
                    so_file, strerror(errno));
            return -1;

        }
    }


    metric_so_t * p_so = new metric_so_t(so_file);
    if (NULL == p_so)
    {
        ERROR_LOG("new metric_so_t failed, %s", strerror(errno));
        return -1;
    }


    if (0 != register_so(p_so, &config_vec))
    {
        delete p_so;
        p_so = NULL;
        return -1;

    }


    so_file_mgr.insert_object(so_file_mgr.size(), p_so);





    return 0;
}

int init_collect_timely(void * owner, void * data)
{
    ADD_TIMER_EVENT(&g_collect_event, init_collect_timely, NULL, get_now_tv()->tv_sec + GET_NODE_CONFIG_INTERVAL);


    if (g_inited)
    {
        return 0;
    }

    if (0 == g_node_ip)
    {
        return 0;
    }

    db_get_node_server_config(g_node_ip_str);


    return 0;
}


int init_collect()
{
    init_collect_timely(0, 0);


    return 0;
}


bool is_collect_inited()
{
    return g_inited;
}


int refresh_collect()
{
    db_get_node_server_config(g_node_ip_str);
    return 0;
}


int fini_collect()
{
    container_for_each(so_file_mgr, it)
    {
        metric_so_t * p_so = it->second;
        p_so->handle_fini();
        dlclose(p_so->handle);
    }


    so_file_mgr.uninit();

    g_inited = false;

    return 0;
}


int register_so(metric_so_t * p_so, void * param)
{
    if (NULL == p_so)
    {
        ERROR_LOG("p_so is null");
        return -1;
    }

    p_so->handle = dlopen(p_so->file.m_full, RTLD_NOW);
    if (NULL == p_so->handle)
    {
        ERROR_LOG("load so file: %s, err: %s", p_so->file.m_full, dlerror());
        return -1;
    }

    dlerror();


    SO_LOAD_FUN(p_so->handle, p_so->handle_init, "handle_init");
    SO_LOAD_FUN(p_so->handle, p_so->handle_fini, "handle_fini");
    SO_LOAD_FUN(p_so->handle, p_so->get_metric_info, "get_metric_info");
    SO_LOAD_FUN(p_so->handle, p_so->metric_handler, "metric_handler");


    DEBUG_LOG("load so: %s", p_so->file.m_full);


    if (0 != p_so->handle_init(param))
    {
        ERROR_LOG("init so %s failed", p_so->file.m_full);
        dlclose(p_so->handle);
        p_so->handle = NULL;
        return -1;
    }


    DEBUG_LOG("inited so %s", p_so->file.m_full);


    return 0;
}







int do_group_collect(c_metric_group * p_group)
{
    // collect功能暂时关闭了，则不采集
    if (!g_collect_flag)
    {
        return 0;
    }

    // 只有连接上了head才进行采集
    if (!g_head->is_connected())
    {
        return 0;
    }


    int index = sizeof(head_proto_t);


    pack_h(g_pkg_buf, p_group->m_collect_interval, index);

    uint32_t metric_count = 0;
    // 记下metric_count的位置
    int metric_count_index = index;
    // 先打上，后面再改
    pack_h(g_pkg_buf, metric_count, index);

    vector_for_each(p_group->m_metric_arg, arg_it)
    {
        c_metric_arg * p_arg = *arg_it;
        if (0 != do_metric_collect(p_arg))
        {
            continue;
        }

#ifdef METRIC_LOG
        c_metric * p_metric = p_arg->m_metric;
        DEBUG_LOG("collect DONE, metric[%u: %s](%s)", 
                p_metric->get_id(), 
                p_metric->get_name(),
                p_arg->m_arg);
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, p_metric->m_arg_map);
        while (g_hash_table_iter_next(&iter, &key, &value))
        {
            c_metric_arg * p_arg = reinterpret_cast< c_metric_arg * >(value);
            if (NULL == p_arg)
            {
                continue;
            }
            DEBUG_LOG("\t%s(%s) = %s", 
                    p_metric->get_name(),
                    p_arg->m_arg,
                    p_arg->m_value.get_string(p_metric->get_fmt()));
        }
#endif

        metric_count++;

        pack_metric_data_to_itl_head(g_pkg_buf, p_arg, index);

    }

    if (0 == metric_count)
    {
        return 0;
    }


    pack_h(g_pkg_buf, metric_count, metric_count_index);

    init_head_proto_header(g_pkg_buf, (uint32_t)index, 0, head_p_metric_data_cmd, 0, g_node_id);
    if (-1 == g_head->send(g_pkg_buf, index))
    {
        ERROR_LOG("send metric data to itl head failed");
        return -1;
    }

    return 0;

}

int init_metric_timer()
{
    container_for_each(metric_group_mgr, group_it)
    {
        c_metric_group * p_group = group_it->second;

        DEBUG_LOG("add group[%u] to timer, interval: %u", p_group->m_group_id, p_group->m_collect_interval);

        // do collect
        do_group_collect(p_group);

        p_group->m_timer = ADD_TIMER_EVENT(&g_collect_timer, route_metric_group, p_group, get_now_tv()->tv_sec + p_group->m_collect_interval);

    }

    // 到这里才算是初始化好了
    g_inited = true;

    return 0;
}

int fini_metric_timer()
{
    REMOVE_TIMERS(&g_collect_timer);

    return 0;
}


int route_metric_group(void * owner, void * data)
{
    if (NULL == data)
    {
        return init_metric_timer();
    }

    c_metric_group * p_group = reinterpret_cast<c_metric_group *>(data);
    if (NULL == p_group)
    {
        return 0;
    }

    uint32_t now = get_now_tv()->tv_sec;
    DEBUG_LOG("enter route metric group[%u], %u", p_group->m_group_id, now);

    do_group_collect(p_group);

    p_group->m_timer = ADD_TIMER_EVENT(&g_collect_timer, route_metric_group, p_group, get_now_tv()->tv_sec + p_group->m_collect_interval);





    return 0;
}
