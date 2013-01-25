/**
 * =====================================================================================
 *       @file  flush_cleanup_thread.h
 *      @brief    
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  10/21/2010 08:34:12 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason, mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_FLUSH_CLEANUP_THREAD_H_2010_11_2
#define H_FLUSH_CLEANUP_THREAD_H_2010_11_2
#include "../rrd-handler/rrd_handler.h"
#include "../lib/c_mysql_iface.h"
#include "../proto.h"

class c_flush_cleanup_thread
{
    public:
        c_flush_cleanup_thread();
        ~c_flush_cleanup_thread();

        /**
         * @brief  初始化
         * @param  p_config: 指向config类的指针
         * @param  p_root:   指向保存数据的树的跟节点的指针
         * @param  p_db_info:指向数据库信息结构的指针
         * @return 0:success, -1:failed
         */
        int init(const config_var_t *p_config, hash_t *p_root, db_info_t *p_db_info, rrd_handler *p_rrd);

        /**
         * @brief  反初始化
         * @param  无
         * @return 0:success, -1:failed
         */
        int uninit();

    protected:
        /**
         * @brief  子线程的线程函数 
         * @param  p_data: 指向当前对象的this指针
         * @return (void *)0:success, (void *)-1:failed
         */
        static void *work_thread(void *p_data);
        /**
         * @brief  将交换机的信息写到rrd和mysql中
         * @param  key: hash的键
         * @param  val: hash里对应key的值
         * @param  arg: 传给回调函数的用户自定义参数
         * @return 0-success
         */
        static int write_switch_data(datum_t *key, datum_t *val, void *arg);
        /**
         * @brief  将交换机下的metric信息写到rrd中
         * @param  key: hash的键
         * @param  val: hash里对应key的值
         * @param  arg: 传给回调函数的用户自定义参数
         * @return 0-success
         */
        static int write_metric_rrd_data(datum_t *key, datum_t *val, void *arg);
        /**
         * @brief  将交换机里时间大于dmax的数据全部删掉  
         * @param  key: hash的键
         * @param  val: hash里对应key的值
         * @param  arg: 传给回调函数的用户自定义参数
         * @return (void *)0:success, (void *)-1:failed
         */
        static int cleanup_switch(datum_t *key, datum_t *val, void *arg);

        /**
         * @brief  找出时间大于dmax的metric  
         * @param  key: hash的键
         * @param  val: hash里对应key的值
         * @param  arg: 传给回调函数的用户自定义参数
         * @return (void *)0:找不到, (void *)-1:找到大于dmax的metric
         */
        static int cleanup_metric(datum_t *key, datum_t *val, void *arg);

        /**
         * @brief  找出最后报警时间和当前时间超过dmax的
         * @param  key: hash的键
         * @param  val: hash里对应key的值
         * @param  arg: 传给回调函数的用户自定义参数
         * @return 0:未超时,-1:超时
         */
        static int cleanup_metric_status(datum_t *key, datum_t *val, void *arg);

    private:
        /** 
         *  @struct 传递给cleanup回调函数的参数
         */
        typedef struct
        {
            struct   timeval *tv;
            datum_t *key;
            datum_t *val;
            size_t   hashval;
        }cleanup_arg_t;

        /** 
         *  @struct 回调函数的参数结构
         */
        typedef struct
        {
            void         *val;
            char         *id;
        }write_rrd_arg_t;

        bool           m_inited;
        pthread_t      m_work_thread_id;      /**<子线程id*/
        bool           m_continue_working;    /**<控制是否继续循环的bool值*/
        rrd_handler   *m_rrd_handler;         /**<rrd操作对象指针*/
        config_var_t    *m_p_config;
        c_mysql_iface *m_db_conn;             /**<数据库对象指针*/
        hash_t        *m_p_root;              /**<保存本head数据源的根节点*/
};

#endif //H_FLUSH_CLEANUP_THREAD_H
