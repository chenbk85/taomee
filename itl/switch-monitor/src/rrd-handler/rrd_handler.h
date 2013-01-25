/**
 * =====================================================================================
 *       @file  rrd_handler.h
 *      @brief  
 *
 *      封装的rrd操作函数类
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

#ifndef RRD_HANDLER_H
#define RRD_HANDLER_H
 
#include <pthread.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "../lib/log.h"
#include "../proto.h"
 
class rrd_handler
{
    public :
        rrd_handler();
        ~rrd_handler();
        int init(const config_var_t  *p_config);

        int uninit();
        /** 
         * @brief  写数据到RRD数据库中
         * @param   switch_ip    交换机ip
         * @param   metric       metric的名字
         * @param   sum          数值
         * @param   num          个数
         * @param   step         抽取间隔
         * @param   process_time 更新时间戳
         * @param   slope        数据增长率
         * @return  成功0失败-1
         */
        int write_data_to_rrd(const char *switch_id, const char *metric, const char *sum,
                const char *num, unsigned int step, unsigned int process_time, slope_t slope);
    protected:

        /** 
         * @brief  更新RRD数据库
         * @param   rrd  rrd数据库路径
         * @param   sum  数值 
         * @param   num  个数
         * @param   process_time 更新时间戳
         * @return  成功0失败-1
         */
        int  RRD_update(const char *rrd, const char *sum, const char *num, unsigned int process_time);

        /** 
         * @brief  创建RRD数据库
         * @param   summary 标志位是否summary
         * @param   step    步长
         * @param   slope   增长率，决定数据源的类型
         * @param   num     数值
         * @param  process_time 更新时间戳
         * @return  成功0失败-1
         */
        int  RRD_create(const char *rrd, int summary, unsigned int step, unsigned int process_time, slope_t slope);

        /** 
         * @brief  存储数据到RRD数据库中
         * @param   rrd  rrd数据库路径
         * @param   sum  数值
         * @param   num  个数
         * @param   step 抽取间隔 
         * @param   process_time 更新时间戳
         * @param   slope slope   增长率，决定数据源的类型
         * @return  成功 0 失败-1
         */
        int  push_data_to_rrd(char *rrd, const char *sum, const char *num,unsigned int step, 
                unsigned int process_time, slope_t slope);
        void my_mkdir(const char *dir);
    private:
        pthread_mutex_t  m_rrd_fil_mutex;
        const char      *m_rrd_root_dir;
};

/** 
 * @brief   创建目录
 * @param   dir  文件路径
 * @return  void 
 */
inline void rrd_handler::my_mkdir(const char *dir)
{
    pthread_mutex_lock(&m_rrd_fil_mutex);
    if(mkdir(dir, 0755) < 0 && errno != EEXIST)
    {
        ERROR_LOG("Unable to mkdir(%s):error:%s", dir, strerror(errno));
    }
    pthread_mutex_unlock(&m_rrd_fil_mutex);
}

#endif
