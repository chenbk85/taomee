/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file work_proc.h
 * @author tonyliu <tonyliu@taomee.com>
 * @author richard <richard@taomee.com>
 */

#ifndef WORK_PROC_H_2011_08_10
#define WORK_PROC_H_2011_08_10

int work_proc(int argc, char **argv);
pid_t spawn_work_proc(int argc, char **argv);

/**
 * @brief 发送数据 - 仅用在work进程
 * @param connection_id 连接id
 * @param p_data        指向数据缓冲区
 * @param data_len      数据缓冲区长度
 * @param is_atomic     是否保持发送的原子性(暂无效,都为原子性发送)
 * @param is_broadcast  是否位广播发送
 * @return 0-成功 -1-失败
 */
int send_data(int connection_id, const char *p_data, int data_len, int is_atomic, int is_broadcast);

/**
 * @brief 发送数据 - 仅用在work进程
 * @param connection_id 连接id
 * @param p_data        指向数据缓冲区
 * @param data_len      数据缓冲区长度
 * @param is_broadcast  是否位广播发送
 * @return 0-成功 -1-失败
 */
int send_data(int connection_id, const char *p_data, int data_len, int is_broadcast);

/**
 * @brief 关闭连接
 * @param connection_id 连接id,负数表示所有连接
 * @return 0-成功 -1-失败
 */
int close_connection(int connection_id);

#endif /* WORK_PROC_H_2011_08_10 */
