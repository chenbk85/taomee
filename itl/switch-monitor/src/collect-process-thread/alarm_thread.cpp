/**
 * =====================================================================================
 *       @file  alarm_thread.cpp
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
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <openssl/md5.h>
#include "../lib/log.h"
#include "alarm_thread.h"
using namespace std;
/** 
 * @brief  构造函数
 * @param   
 * @return   
 */
c_alarm_thread::c_alarm_thread()
{
    m_inited = 0;
    m_stop = false; 
    m_alarm_interval = 0;
    m_p_queue_map = NULL;
    m_pid  = 0;
    m_alarm_counter.clear();
}

/** 
 * @brief  析构函数
 * @param   
 * @return   
 */
c_alarm_thread::~c_alarm_thread()
{
    uninit();
}


/** 
 * @brief  反初始化
 * @param   
 * @return   
 */
int c_alarm_thread::uninit()
{
    if(!m_inited)
    {
        return -1;
    }

    assert(m_pid !=  0);
    m_stop = true;
    pthread_join(m_pid, NULL);

    if(!m_alarm_counter.empty())
    {
        m_alarm_counter.clear();
    }

    m_stop = false;
    m_pid  = 0;
    m_alarm_interval = 0;
    m_p_queue_map = NULL;
    m_inited = 0;

    return 0;
}

/** 
 * @brief   初始化函数,要么init成功，要么失败会uninit已经init成功的变量
 * @param   p_config     config_var_t结构指针
 * @param   p_queue_map  queue_map_t对象指针
 * @return  0 = success -1 = failed
 */
int c_alarm_thread::init(const config_var_t *p_config, queue_map_t *p_queue_map)
{
    if(m_inited)
    {
        return -1;
    }

    if(p_config == NULL || p_queue_map == NULL)
    {
        ERROR_LOG("paraments invalid");
        return -1;
    }

    m_p_queue_map = p_queue_map;
    m_alarm_interval = p_config->alarm_interval;

    if(m_alarm_interval <= 0 || m_alarm_interval > 200)
    {
        DEBUG_LOG("alarm_interval config error,use default 10");
        m_alarm_interval = 10;
    }

    strncpy(m_alarm_server_url, p_config->alarm_server_url, sizeof(m_alarm_server_url) - 1);
    m_alarm_server_url[sizeof(m_alarm_server_url) - 1] = '\0';

    int ret = pthread_create(&m_pid, NULL, alarm_main, this);
    if(ret == 0)
    {
        m_inited = 1;
        return 0;
    }
    else
    {
        m_stop = false;
        m_pid  = 0;
        m_p_queue_map = NULL;
        m_alarm_interval = 0;
        m_alarm_counter.clear();
        return -1;
    }
}

/** 
 * @brief   线程主函数
 * @param   p_data  用户数据
 * @return  NULL = success UNNULL = failed
 */
void* c_alarm_thread::alarm_main(void  *p_data)
{
    c_alarm_thread     *alarmer = (c_alarm_thread*)p_data;
    queue_map_t        *queues = alarmer->m_p_queue_map;              /**<queue的指针的map*/
    Chttp_transfer     *http_transfer = &alarmer->m_http_transfer;    /**<Chttp_transfer对象指针*/
    char                post_data[MAX_STR_LEN + 1] = {'\0'};
    char               *alarm_server_url = alarmer->m_alarm_server_url;

    DEBUG_LOG("Enter the main while loop of alarm_main");
    while(!alarmer->m_stop)
    {
        queue_map_t::iterator   it;
        i_ring_queue  *queue = NULL;
        for(it = queues->begin(); it != queues->end(); it++)
        {
            memset(post_data, 0, sizeof(post_data));
            queue = it->second;
            if(queue == NULL)
            {
                //unlikely to come here
                ERROR_LOG("Critical error.the ring queue has uninited.");
                return (void*)-1;
            }
            //队列里没有数据或出错
            if(queue->get_data_len() <= 0)
            {
                continue;
            }

            int ret = queue->pop_data_dummy(post_data, MAX_STR_LEN, 8);
            if(ret < 0)
            {
                ERROR_LOG("Dummy data from [%u]'s queue failed", it->first);
                continue;
            }
            else if(ret == 0)
            {
                //pop data time out
                continue;
            }
            else
            {
                DEBUG_LOG("Got the alarm data of collect thread:[%u],the post data is:[%s].",
                        it->first, post_data + sizeof(uint16_t));

                http_transfer->http_post(alarm_server_url, post_data + sizeof(uint16_t));
                string ret_str = http_transfer->get_post_back_data(); 

                map<unsigned int, unsigned int>::iterator it_p;
                it_p = alarmer->m_alarm_counter.find(it->first);

                if(!strcasecmp(ret_str.c_str(), "eok"))//成功
                {
                    queue->pop_data(post_data, MAX_STR_LEN, 0);
                    if(it_p != alarmer->m_alarm_counter.end())
                    {
                        alarmer->m_alarm_counter.erase(it_p);
                    }
                }
                else if(!strcasecmp(ret_str.c_str(), "einval"))//参数错误
                {
                    queue->pop_data(post_data, MAX_STR_LEN, 0);
                    if(it_p != alarmer->m_alarm_counter.end())
                    {
                        alarmer->m_alarm_counter.erase(it_p);
                    }
                }
                else
                {
                    //如果失败，把失败计数加一
                    if(it_p == alarmer->m_alarm_counter.end())
                    {
                        alarmer->m_alarm_counter.insert(pair<unsigned int, unsigned int>(it->first, 1));
                    }
                    else
                    {
                        if(++(it_p->second) > MAX_ALARM_ATTEMP_COUNT)
                        {
                            DEBUG_LOG("Post alarm data:[%s] of collect thread:[%u]"
                                    " reach the max alarm attemp count,will remove from queue.", 
                                    post_data + sizeof(uint16_t), it->first);
                            //从队列中删除
                            queue->pop_data(post_data, MAX_STR_LEN, 0);
                            //计数器也清除
                            alarmer->m_alarm_counter.erase(it_p);
                        }
                    }
                }
            }
        }
        DEBUG_LOG("Alarm_thread take a break.....");
        unsigned int sec = 0;
        //每隔一秒检测是否需要退出
        while(!alarmer->m_stop && sec++ < alarmer->m_alarm_interval)
        {
            sleep(1);
        }
    }

    DEBUG_LOG("Exit main while loop of alarm_main");
    return NULL;
}

