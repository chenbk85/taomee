/**
 * =====================================================================================
 *       @file  work.h
 *      @brief  work进程
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/04/2010 10:27:00 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu(LCT) , tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef _NB_WORK_H_
#define _NB_WORK_H_

int spawn_work_proc(int argc, char *argv[], pid_t *wpids, int work_count, bool use_barrier);
int run_work_proc(int argc, char *argv[], bool use_barrier);
int monitor_work_proc(int argc, char* argv[], pid_t *wpids, int pid_count, bool use_barrier);

/**
 * @brief 发送数据 - 仅用在work进程
 *
 * @param connection_id 连接id
 * @param p_data        指向数据缓冲区
 * @param data_len      数据缓冲区长度
 * @param is_atomic     是否保持发送的原子性(暂无效,都为原子性发送)
 * @param is_broadcast  是否位广播发送
 *
 * @return 0-成功 -1-失败
 */
int send_data(int connection_id, const char *p_data, int data_len, int is_atomic, int is_broadcast);

/**
 * @brief 发送数据 - 仅用在work进程
 *
 * @param connection_id 连接id
 * @param p_data        指向数据缓冲区
 * @param data_len      数据缓冲区长度
 * @param is_broadcast  是否位广播发送
 *
 * @return 0-成功 -1-失败
 */
int send_data(int connection_id, const char *p_data, int data_len, int is_broadcast);

#endif

