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


#include "container.h"
#include "metric.h"
#include "timer.h"
#include "metric_group.h"
#include "itl_common.h"
#include "proto.h"
#include "collect.h"


const char * g_metric_so_dir = config_get_strval("so_dir", "./so/");

#define SO_LOAD_FUN(h, fptr, fname) \
    do { \
        fptr = (typeof(fptr))dlsym(h, fname); \
        if (!fptr) { \
            dlerror(); \
            dlclose(h); \
            fptr = NULL; \
            ERROR_LOG("dlsym not find:%s", fname); \
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



int register_so(metric_so_t * p_so, void * param);
int route_metric_group(void * owner, void * data);

int find_metric_by_name(c_metric * p_metric, void * param)
{
    const char * p_name = reinterpret_cast< const char * >(param);
    int ret = strncmp(p_metric->get_metric_name(), p_name, MAX_METRIC_NAME_LEN);
    if (0 == ret)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int init_collect_so(char *so_name_list)
{
    if (-1 == access(g_metric_so_dir, F_OK))
    {
        if (0 != mkdir(g_metric_so_dir, 0755))
        {
            ERROR_LOG("make so dir %s failed, %s", g_metric_so_dir, strerror(errno));
            return -1;
        }
    }

    char * str = so_name_list;
    char * save_ptr = NULL;
    char * token = NULL;
    while (NULL != (token = strtok_r(str, ";", &save_ptr)))
    {
        char so_file[PATH_MAX] = {0};
        snprintf(so_file, PATH_MAX, "%s/%s", g_metric_so_dir, token);

        if (-1 == access(so_file, F_OK | R_OK))
        {
            //if (0 != download_metric_so(token))
            {
                ERROR_LOG("switch so: %s must exist and have F_OK | R_OK permission, reason: %s", 
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


int init_switch_collect_so(Cmessage * c_out)
{
    db_p_get_node_switch_config_out * p_out = P_OUT;
    return init_collect_so(p_out->so_name_list);
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

    DEBUG_LOG("succ load so: %s", p_so->file.m_full);

    if (0 != p_so->handle_init(param))
    {
        ERROR_LOG("init so %s failed", p_so->file.m_full);
        dlclose(p_so->handle);
        p_so->handle = NULL;
        return -1;
    }

    DEBUG_LOG("succ init so: %s", p_so->file.m_full);

    int index = 0;
    const metric_info_t * p_metric_info = NULL;
    while (NULL != (p_metric_info = p_so->get_metric_info(index)))
    {
        const char *p_name = p_metric_info->name;
        map_for_each(*(metric_mgr.get_object_map()), metric_it)
        {
            c_metric * p_metric = metric_it->second;
            if (NULL == p_metric || 0 != strncmp(p_metric->get_metric_name(), p_name, MAX_METRIC_NAME_LEN))
            {
                continue;
            }
            p_metric->set_metric_so(p_so);
            p_metric->set_metric_index(index);
            p_metric->set_metric_info(p_metric_info);

            DEBUG_LOG("from so: %s, metric id: %u, name: %s, value type: %u, slope: %u", 
                    p_so->file.m_full, 
                    p_metric->get_metric_id(),
                    p_metric->get_metric_name(),
                    p_metric->get_metric_value_type(),
                    p_metric->get_metric_slope());

        }

        index++;
    }

    return 0;
}


int init_metric_group_collect_timer()
{
    container_for_each(metric_group_mgr, group_it)
    {
        c_metric_group * p_group = group_it->second;
        uint32_t collect_interval =  p_group->get_collect_interval();

        DEBUG_LOG("add group[%u] to timer, collect_interval: %u", group_it->first, collect_interval);

        // do collect
        p_group->do_group_collect_and_send();

        ADD_TIMER_EVENT(&g_collect_timer, route_metric_group, p_group, get_now_tv()->tv_sec + collect_interval);
        //sleep(1);
    }

    return 0;
}


int route_metric_group(void * owner, void * data)
{
    uint32_t now = get_now_tv()->tv_sec;
    //DEBUG_LOG("enter route metric group at %u", now);

    if (NULL == data)
    {
        return init_metric_group_collect_timer();
    }

    c_metric_group * p_group = reinterpret_cast< c_metric_group * >(data);
    if (NULL == p_group)
    {
        return 0;
    }
    c_switch *p_switch = p_group->get_swich_handler();
    DEBUG_LOG("enter switch[%s] route metric group at %u", p_switch->get_switch_ip_str(), now);

    p_group->do_group_collect_and_send();

    uint32_t collect_interval =  p_group->get_collect_interval();
    ADD_TIMER_EVENT(&g_collect_timer, route_metric_group, p_group, get_now_tv()->tv_sec + collect_interval);

    return 0;
}


int fini_metric_timer()
{
    REMOVE_TIMERS(&g_collect_timer);

    return 0;
}
