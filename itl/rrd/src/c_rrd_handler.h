/**
 * =====================================================================================
 *       @file  c_rrd_handler.h
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

#ifndef C_RRD_HANDLER_H
#define C_RRD_HANDLER_H
 
#include <pthread.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include <libtaomee/log.h>

#include "proto.h"


class c_rrd_handler
{
    public :
        c_rrd_handler();
        ~c_rrd_handler();
        int init(const char *p_rrd_dir);
        int uninit();

        /** 
         * @brief  写数据到RRD数据库中
         * @param   host    host名字
         * @param   metric  metric的名字
         * @param   value     数值
         * @param   num     个数
         * @param   step     抽取间隔
         * @return  success:0, fail:-1
         */
        int write_data_to_rrd(
                const char *host,
                const char *metric,
                const char *value,
                const char *num,
                unsigned int step);

    protected:
        /** 
         * @brief  创建RRD数据库
         * @param   summary 标志位是否summary
         * @param   step    步长
         * @return  成功0失败-1
         */
        int  RRD_create(const char *rrd, bool summary, unsigned int step);

        /** 
         * @brief  更新RRD数据库
         * @param   rrd  rrd数据库路径
         * @param   value  数值 
         * @param   num  个数
         * @return  成功0失败-1
         */
        int  RRD_update(const char *rrd, const char *value, const char *num);

        int rrd_mkdir(const char *dir);

    private:
        bool m_inited;

        /**<保存rrd文件的基础目录*/
        const char * m_rrd_base_dir;
};


inline int c_rrd_handler::rrd_mkdir(const char *dir)
{
    if(mkdir(dir, 0755) < 0 && errno != EEXIST) {
        ERROR_LOG("Unable to mkdir(%s): %s", dir, strerror(errno));
        return -1;
    }

    return 0;
}

#endif
