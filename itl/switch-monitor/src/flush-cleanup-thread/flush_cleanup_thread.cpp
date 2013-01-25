/**
 * =====================================================================================
 *       @file  flush_cleanup_thread.cpp
 *      @brief 
 *
 *  summary the metric of this source,and cleanup the metrics timeouted
 *
 *   @internal
 *     Created  10/18/2010 09:57:16 AM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason , mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include "../lib/log.h"
#include "../proto.h"
#include "../lib/hash.h"
#include "../lib/utils.h"
#include "flush_cleanup_thread.h"
#include "../db_operator.h"

c_flush_cleanup_thread::c_flush_cleanup_thread(): m_inited(false), m_work_thread_id(0), m_continue_working(false), m_rrd_handler(NULL),m_p_config(NULL),m_db_conn(NULL),m_p_root(NULL)
{

}

c_flush_cleanup_thread::~c_flush_cleanup_thread()
{
    uninit();
}

/**
 * @brief  初始化对象,要么成功，要么失败会uninit已经init成功的变量
 * @param  p_config: 指向config类的指针
 * @param  p_root    保存本地数据源的根节点的指针
 * @param  p_db_info 保存数据库信息结构的指针
 * @return 0:success, -1:failed
 */
int c_flush_cleanup_thread::init(const config_var_t *p_config, hash_t *p_root, db_info_t *p_db_info, rrd_handler *p_rrd)
{
    if(m_inited)
    {
        return -1;
    } 

    if(p_config == NULL || p_root == NULL || p_db_info == NULL || p_rrd == NULL)
    {
        ERROR_LOG("ERROR:parameter invalid,cann't be NULL.");
        return -1;
    }

    m_p_root = p_root;
    m_p_config = (config_var_t *)p_config;
    m_rrd_handler = p_rrd;

    if(create_mysql_iface_instance(&m_db_conn) != 0)
    {
        ERROR_LOG("Create mysql instance failed."); 
        m_continue_working = false;
        m_work_thread_id = 0;
        m_p_root = NULL;
        m_p_config = NULL;
        m_rrd_handler = NULL;
        m_db_conn = NULL;
        return -1;
    }

    if(m_db_conn->init(p_db_info->db_host, p_db_info->db_port, p_db_info->db_name,
                p_db_info->db_user, p_db_info->db_pass, "utf8") != 0)
    {
        ERROR_LOG("Init mysql failed."); 
        m_continue_working = false;
        m_work_thread_id = 0;
        m_p_root = NULL;
        m_p_config = NULL;
        m_rrd_handler = NULL;
        m_db_conn->release();
        m_db_conn = NULL;
        return -1;
    }

    m_continue_working = true;
    int result = pthread_create(&m_work_thread_id, NULL, work_thread, this);
    if(result != 0)
    {
        ERROR_LOG("ERROR: pthread_create() failed."); 

        m_continue_working = false;
        m_work_thread_id = 0;
        m_p_root = NULL;
        m_p_config = NULL;
        m_rrd_handler = NULL;
        m_db_conn->uninit();
        m_db_conn->release();
        m_db_conn = NULL;

        return -1;
    }

    m_inited = true;
    return 0;
}

/**
 * @brief  反初始化
 * @param  无
 * @return 0:success, -1:failed
 */
int c_flush_cleanup_thread::uninit()
{
    if(!m_inited)
    {
        return -1; 
    } 

    assert(m_work_thread_id != 0);

    m_continue_working = false;
    pthread_join(m_work_thread_id, NULL);
    m_work_thread_id = 0;
    m_db_conn->uninit();
    m_db_conn->release();
    m_db_conn = NULL;
    m_p_config = NULL;
    m_rrd_handler = NULL;
    m_p_root = NULL;
    m_inited = false;

    return 0;
}

/**
 * @brief  子线程的线程函数 
 * @param  p_data: 指向当前对象的this指针
 * @return (void *)0:success
 */
void *c_flush_cleanup_thread::work_thread(void *p_data)  
{
    int do_cleanup = 0;
    unsigned int start = 0, interval = 0;
    c_flush_cleanup_thread  *p_instance = (c_flush_cleanup_thread *)p_data;

    do
    {   
        start = time(NULL);//开始处理的时间

        //先执行清理,每做两次flush做一次cleanup
        if(do_cleanup)
        {
            cleanup_arg_t cleanup;  
            struct timeval tv;

            DEBUG_LOG("cleanup start");

            gettimeofday(&tv, NULL);
            cleanup.tv = &tv;
            cleanup.key = 0;
            cleanup.val = 0;
            cleanup.hashval = 0;

            hash_walkfrom(p_instance->m_p_root, cleanup.hashval, cleanup_switch, (void *)&cleanup);
            DEBUG_LOG("cleanup finished");
        } 
        do_cleanup = do_cleanup ? 0 : 1;

        //将数据写到RRD和DB
        hash_foreach(p_instance->m_p_root, write_switch_data, (void*)p_instance);

        interval = time(NULL) - start;
        if(p_instance->m_p_config->collect_interval > interval)
        {
            unsigned int sec = 0;
            while(p_instance->m_continue_working && sec++ < (p_instance->m_p_config->collect_interval - interval))
            {
                sleep(1);
            }
        }
    }while(p_instance->m_continue_working);
    DEBUG_LOG("Exit the main while loop of flush&cleanup thread.");
    return NULL;
}

/**
 * @brief  将交换机的信息写到rrd
 * @param  key: hash的键
 * @param  val: hash里对应key的值
 * @param  arg: 传给回调函数的用户自定义参数
 * @return 0-success -1-failed
 */
int c_flush_cleanup_thread::write_switch_data(datum_t *key, datum_t *val, void *arg)
{
    int rc = -1;
    char id[12] = {0};
    switch_t *switch_data = (switch_t*)val->data;
    rrd_handler *rrd = ((c_flush_cleanup_thread*)arg)->m_rrd_handler;
    c_mysql_iface *db_conn = ((c_flush_cleanup_thread*)arg)->m_db_conn; 
    sprintf(id, "%u", switch_data->id);

    unsigned int now = time(0);
    for(unsigned int i = 0; i < switch_data->if_num; i++) {
        usleep(10);
        if(db_update_switch_interface_info(db_conn, switch_data->id, switch_data->if_table[i].idx, 
                    switch_data->if_table[i].status, switch_data->if_table[i].desc, 
                    switch_data->if_table[i].type, switch_data->if_table[i].speed, now) != 0) {
            ERROR_LOG("Update interface[%u] of switch[%u] info in mysql failed.", 
                    switch_data->if_table[i].idx, switch_data->id);
        }

        if(switch_data->if_table[i].status == 2 ||
           switch_data->if_table[i].speed == 0 ||
           switch_data->if_table[i].type == 1) {
            continue; 
        }

        char interface_metric_name[MAX_NAME_SIZE] = {0};
        char interface_metric_data[MAX_NAME_SIZE] = {0};
        unsigned int step = ((c_flush_cleanup_thread*)arg)->m_p_config->collect_interval;
        snprintf(interface_metric_name, MAX_NAME_SIZE, "if_%u_in", switch_data->if_table[i].idx);
        //snprintf(interface_metric_data, MAX_NAME_SIZE, "%.2lf", switch_data->if_table[i].in_bits_per_sec);
        snprintf(interface_metric_data, MAX_NAME_SIZE, "%u", switch_data->if_table[i].last_in_bytes);
        if(rrd->write_data_to_rrd(id, interface_metric_name, interface_metric_data, NULL, 
                    step, time(NULL), SLOPE_POSITIVE) != 0)
                    //step, time(NULL), cstr_to_slope("both")) != 0)
            //step, switch_data->if_table[i].last_in_report, cstr_to_slope("both")) != 0)
        {
            ERROR_LOG("Write interface[%u] of switch[%u] in_bits data in rrd failed.", 
                    switch_data->if_table[i].idx, switch_data->id);
        }
        snprintf(interface_metric_name, MAX_NAME_SIZE, "if_%u_out", switch_data->if_table[i].idx);
        //snprintf(interface_metric_data, MAX_NAME_SIZE, "%.2lf", switch_data->if_table[i].out_bits_per_sec);
        snprintf(interface_metric_data, MAX_NAME_SIZE, "%u", switch_data->if_table[i].last_out_bytes);
        if(rrd->write_data_to_rrd(id, interface_metric_name, interface_metric_data, NULL, 
                    step, time(NULL), SLOPE_POSITIVE) != 0)
                    //step, time(NULL), cstr_to_slope("both")) != 0)
            //step, switch_data->if_table[i].last_out_report, cstr_to_slope("both")) != 0)
        {
            ERROR_LOG("Write interface[%u] of switch[%u] out_bits data in rrd failed.", 
                    switch_data->if_table[i].idx, switch_data->id);
        }
    }

    if(0 != db_delete_overdue_switch_interface_info(db_conn, switch_data->id, now)) {
        ERROR_LOG("db_delete_overdue_switch_interface_info(switch[%u]) from mysql failed.", switch_data->id);
    }

    write_rrd_arg_t tmp_arg = {arg, id};
    rc = hash_foreach(switch_data->metrics, write_metric_rrd_data, (void*)&tmp_arg);

    return rc;
}

/**
 * @brief  将交换机下的metric数据写到rrd
 * @param  key: hash的键
 * @param  val: hash里对应key的值
 * @param  arg: 传给回调函数的用户自定义参数
 * @return 0-success
 */
int c_flush_cleanup_thread::write_metric_rrd_data(datum_t *key, datum_t *val, void *arg)
{
    char *name = NULL;
    char sum[256] = {0};
    int rc = -1;
    metric_t *metric = NULL;
    write_rrd_arg_t *rrd_arg = (write_rrd_arg_t*)arg;
    rrd_handler *rrd = ((c_flush_cleanup_thread*)(rrd_arg->val))->m_rrd_handler;

    name = (char *)key->data;
    metric = (metric_t *)val->data;
    sprintf(sum, "%.2lf", metric->val.d);
    unsigned int step = ((c_flush_cleanup_thread *)(rrd_arg->val))->m_p_config->collect_interval;

    //rc = rrd->write_data_to_rrd(rrd_arg->id, name, sum, NULL, step, metric->recv_time, cstr_to_slope(metric->slope));
    rc = rrd->write_data_to_rrd(rrd_arg->id, name, sum, NULL, step, time(NULL), cstr_to_slope(metric->slope));

    if(rc) {
        ERROR_LOG("Unable to write rrd data of metric[%s]", name);
    }

    return 0;
}


/**
 * @brief  将switch里时间大于dmax的数据全部删掉  
 * @param  key: hash的键
 * @param  val: hash里对应key的值
 * @param  arg: 传给回调函数的用户自定义参数
 * @return (void *)0:success, (void *)-1:failed
 */
int c_flush_cleanup_thread::cleanup_switch(datum_t *key, datum_t *val, void *arg)
{
    cleanup_arg_t    *p_node_cleanup = (cleanup_arg_t *)arg;
    switch_t         *p_switch = (switch_t *)val->data; 
    datum_t          *rv = NULL;

    unsigned interval = p_node_cleanup->tv->tv_sec - p_switch->recv_time; 
    if(p_switch->dmax && interval > p_switch->dmax) 
    {
        ///switch is older than dmax. Delete.
        p_node_cleanup->key = key;
        p_node_cleanup->val = val;
        return -1;
    }

    cleanup_arg_t cleanup;
    cleanup.tv = p_node_cleanup->tv;
    cleanup.key = 0;
    cleanup.hashval = 0;

    while(hash_walkfrom(p_switch->metrics, cleanup.hashval, cleanup_metric, (void *)&cleanup))
    {
        ///将metric里时间大于dmax的数据删掉 
        if(cleanup.key)
        {
            cleanup.hashval = hashval(cleanup.key, p_switch->metrics);
            rv = hash_delete(cleanup.key, p_switch->metrics);
            if(rv) 
            {
                datum_free(rv);
            }
            cleanup.key=0;
        }
        break;
    } 

    cleanup.key = 0;
    cleanup.hashval = 0;
    rv = NULL;

    while(hash_walkfrom(p_switch->metrics_status, cleanup.hashval, cleanup_metric_status, (void *)&cleanup))
    {
        if(cleanup.key)
        {
            cleanup.hashval = hashval(cleanup.key, p_switch->metrics_status);
            rv = hash_delete(cleanup.key, p_switch->metrics_status);
            if(rv) 
            {
                datum_free(rv);
            }
            cleanup.key=0;
        }
        break;
    } 
    return 0;
}

/**
 * @brief  找出时间大于dmax的metric  
 * @param  key: hash的键
 * @param  val: hash里对应key的值
 * @param  arg: 传给回调函数的用户自定义参数
 * @return (void *)0:找不到, (void *)-1:找到大于dmax的metric
 */
int c_flush_cleanup_thread::cleanup_metric(datum_t *key, datum_t *val, void *arg)
{
    cleanup_arg_t *p_cleanup = (cleanup_arg_t *)arg;
    metric_t *p_metric = (metric_t *)val->data; 

    if(p_metric->recv_time >= p_cleanup->tv->tv_sec)
    {
        return 0;
    }
    else if(p_metric->dmax && p_cleanup->tv->tv_sec - p_metric->recv_time > p_metric->dmax)
    {
        p_cleanup->key = key;
        return -1;
    }
    return 0;
}

/**
 * @brief  找出最后报警时间和当前时间超过dmax的
 * @param  key: hash的键
 * @param  val: hash里对应key的值
 * @param  arg: 传给回调函数的用户自定义参数
 * @return 0:未超时,-1:超时
 */
int c_flush_cleanup_thread::cleanup_metric_status(datum_t *key, datum_t *val, void *arg)
{
    cleanup_arg_t        *p_cleanup = (cleanup_arg_t *)arg;
    metric_status_info_t *p_metric_status = (metric_status_info_t *)val->data; 

    if(p_metric_status->last_alarm >= p_cleanup->tv->tv_sec)
    {
        return 0;
    }
    else if(p_cleanup->tv->tv_sec - p_metric_status->last_alarm > DEFAULT_DMAX)
    {
        p_cleanup->key = key;
        return -1;
    }
    return 0;
}
