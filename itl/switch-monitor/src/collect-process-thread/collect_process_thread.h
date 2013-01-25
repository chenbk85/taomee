/**
 * =====================================================================================
 *       @file  collect_process_thread.h
 *      @brief  
 *
 *  request the metric .then alarm ,and save them into hash tree
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
#ifndef COLLECT_PROCESS_THREAD_H
#define COLLECT_PRocess_THREAD_H

#include <map>
#include <string>
#include <pthread.h>
#include "../lib/i_ring_queue.h"
#include "../lib/c_mysql_iface.h"
#include "../defines.h"
#include "./data_processer.h"
#include "./alarm_thread.h"

class c_collect_process_thread
{
public :
    c_collect_process_thread();
    ~c_collect_process_thread(); 
    int init(const config_var_t *p_config,
            switch_group_vec_t *switch_set, 
            if_alarm_map_t *p_if_alarm,
            metric_info_vec_t *metric_set, 
            metric_alarm_vec_t *default_alarm_conf,
            metric_alarm_vec_t *special_alarm_conf, 
            hash_t *switch_table); 
    int uninit();
private:
    int m_inited;
    c_alarm_thread m_alarm_thread;                  /**<报警线程*/
    std::vector<c_data_processer*> m_process_thread;/**<保存c_data_processer指针的数组*/
    std::map<unsigned int, i_ring_queue*> m_queues; /**<保存每个收集线程的queue指针*/
};
#endif
