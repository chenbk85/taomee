/**
 * =====================================================================================
 *       @file  alarm_thread.h
 *      @brief  
 *
 *  handle the alarm 
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
 
 
#ifndef ALARM_THREAD_H
#define ALARM_THREAD_H

#include <map>
#include "../lib/i_ring_queue.h"
#include "../defines.h"
#include "../proto.h"
#include "../lib/http_transfer.h"

typedef std::map<unsigned int, i_ring_queue*> queue_map_t; 
const unsigned int MAX_ALARM_ATTEMP_COUNT = 10;     /**<post报警数据失败后的最大重试次数*/ 

class c_alarm_thread
{
    public :
        c_alarm_thread();
        ~c_alarm_thread();
        int init(const config_var_t *p_config, queue_map_t *p_queue_map);
        int uninit();
    protected:
        /** 
         * @brief   线程主函数
         * @param   p_data  用户数据
         * @return  NULL success UNNULL failed
         */
        static void* alarm_main(void *p_data);
    private:
        int                   m_inited;           /**<是否初始化标志*/
        pthread_t             m_pid;              /**<线程id*/
        queue_map_t          *m_p_queue_map;      /**<queue_map_t对象指针*/
        bool                  m_stop;
        unsigned int          m_alarm_interval;
        char                  m_alarm_server_url[MAX_URL_LEN + 1];
        Chttp_transfer        m_http_transfer;    /**<Chttp_transfer对象*/
        std::map<unsigned int, unsigned int>  m_alarm_counter;/**<保存失败post数据的计数*/
};

#endif
