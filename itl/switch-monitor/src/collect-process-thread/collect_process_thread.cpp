/**
 * =====================================================================================
 *       @file  collect_process_thread.cpp
 *      @brief  
 *
 *  request the xml data from data source.then parse them ,and save them into hash tree
 *
 *   @internal
 *     Created  2010-10-18 11:13:42
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason, mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <algorithm>
#include <functional>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include "../lib/log.h"
#include "collect_process_thread.h"

using namespace std;

/** 
 * @brief  构造函数
 * @param   
 * @return  
 */
c_collect_process_thread::c_collect_process_thread()
{
    m_inited   = 0;
    m_process_thread.clear();
    m_queues.clear();
}

/** 
 * @brief  析构函数
 * @param   
 * @return  
 */
c_collect_process_thread::~c_collect_process_thread()
{
    uninit();
}

/** 
 * @brief   反初始化本对象
 * @return  0success -1failed 
 */
int c_collect_process_thread::uninit()
{
    if(!m_inited)
    {
        return -1;
    }

    //首先退出alarm thread
    m_alarm_thread.uninit();

    if(!m_process_thread.empty())
    {
        vector<c_data_processer*>::iterator itp = m_process_thread.begin();
        for(; itp != m_process_thread.end(); itp++)
        {
            (*itp)->uninit();
            (*itp)->release();
        }
        m_process_thread.clear();
    }

    if(!m_queues.empty())
    {
        map<unsigned int, i_ring_queue*>::iterator itq = m_queues.begin();
        for(; itq != m_queues.end(); itq++)
        {
            (itq->second)->uninit();
            (itq->second)->release();
        }
        m_queues.clear();
    }

    m_inited = 0;
    return 0;
}


/** 
 * @brief   初始化函数,要么成功，要么失败会uninit已经init成功的变量
 * @param   p_config congfig对象的指针
 * @param   switch_group_set switch group 结构指针的数组
 * @param   metric_set  metric结构的数组
 * @param   switch_table  保存switch信息的hash表
 * @return  0success -1failed 
 */
int c_collect_process_thread::init(
        const config_var_t *p_config,
        switch_group_vec_t *switch_group_set,
        if_alarm_map_t *p_if_alarm,
        metric_info_vec_t *metric_set,
        metric_alarm_vec_t *default_alarm_conf,
        metric_alarm_vec_t *special_alarm_conf,
        hash_t *switch_table)
{
    if(m_inited) {
        ERROR_LOG("ERROR: c_collect_process_thread been inited.");
        return -1;
    }

    if(NULL == p_config || NULL == metric_set || NULL == switch_group_set 
       || NULL == default_alarm_conf || NULL == special_alarm_conf
       || NULL == p_if_alarm || NULL == switch_table) {
        ERROR_LOG("ERROR: parament cannot be NULL.");
        return -1;
    }

    int  queue_len = p_config->queue_len;
    //queue如果大于8M,当做8M,小于等于0当做8k
    if(queue_len <= 0) {
        queue_len = 8 * 1024;
    } else if(queue_len > 8 * 1024 * 1024) {
        queue_len = 8 * 1024 * 1024;
    }

    i_ring_queue  *raw_queue, *usable_queue; 
    c_data_processer *p_process_thread; 
    switch_group_vec_t::iterator its = switch_group_set->begin();
    unsigned int count = 0;
    for(; its != switch_group_set->end(); its++, count++) {
        raw_queue = usable_queue = NULL;
        p_process_thread = NULL;

        if(create_data_processer_instance(&p_process_thread) != 0) {
            continue;
        }

        if(0 != create_variable_queue_instance(&raw_queue, sizeof(uint16_t))) {
            ERROR_LOG("Failed create the variable queue instance.");
            p_process_thread->release();
            continue;
        }

        if((usable_queue = create_waitable_queue_instance(raw_queue)) == NULL) {
            ERROR_LOG("Failed create the waitable queue instance.");
            p_process_thread->release();
            raw_queue->release();
            continue;
        }

        if(0 != usable_queue->init(queue_len)) {
            ERROR_LOG("Failed init the waitable queue.");
            p_process_thread->release();
            usable_queue->release();
            continue;
        }

        if(p_process_thread->init(metric_set, default_alarm_conf, special_alarm_conf,
                    &(*its), p_if_alarm, switch_table, p_config, usable_queue) != 0) {
            ERROR_LOG("Start data processer thread failed.");
            usable_queue->uninit();
            usable_queue->release();
            p_process_thread->release();
            continue;
        }
        m_process_thread.push_back(p_process_thread);
        m_queues.insert(pair<unsigned int, i_ring_queue*>(count, usable_queue));
        sleep(1);//错开几秒去启动线程，让线程的初始化处理不要扎堆儿
    }

    if(0 != m_alarm_thread.init(p_config, &m_queues)) {
        ERROR_LOG("Start alarm thread failed.");
        if(!m_process_thread.empty()) {
            vector<c_data_processer*>::iterator itp = m_process_thread.begin();
            for(; itp != m_process_thread.end(); itp++) {
                (*itp)->uninit();
                (*itp)->release();
            }
            m_process_thread.clear();
        }

        if(!m_queues.empty()) {
            map<unsigned int, i_ring_queue*>::iterator itq = m_queues.begin();
            for(; itq != m_queues.end(); itq++) {
                (itq->second)->uninit();
                (itq->second)->release();
            }
            m_queues.clear();
        }
        return -1;
    }

    m_inited = 1;
    return 0;
}

