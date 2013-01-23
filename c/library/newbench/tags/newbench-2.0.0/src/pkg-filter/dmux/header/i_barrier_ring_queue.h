/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file i_barrier_ring_queue.h
 * @brief 带包屏障过滤功能的环形队列接口
 * @author baron baron@taomee.com
 * @version 1.0
 * @date 2010-08-25
 */

#ifndef __I_BARRIER_RING_QUEUE_H__
#define __I_BARRIER_RING_QUEUE_H__

#include <sys/types.h>

/**
 * @brief 带屏障功能的环形队列接口,需要继承 */
struct i_barrier_ring_queue
{
public:
    typedef struct {
        int32_t used_list_length;
        int32_t empty_list_length;
        int32_t reserve_length;
        int32_t conflict_cnt;
        int32_t tree_node_cnt;
    } debug_info_t;

    /**
     * @brief 初始化函数
     *
     * @param rq_length         环形队列长度
     * @param avg_buf_length    环形队列接收数据buf的平均长度
     *
     * @return 0成功 -1失败
     */
    virtual int init(int32_t rq_length, int32_t avg_buf_length) = 0;

    /**
     * @brief 反初始化函数
     *
     * @return 0成功 -1失败
     */
    virtual int uninit() = 0;

    virtual int release() = 0;

    /**
     * @brief 把数据压入到环状缓冲区中
     *
     * @param p_data    指向要压入的数据
     * @param length    压入的数据的长度
     * @param key       屏障判断的关键字
     * @param flag      是否进行屏障的标志位
     *
     * @return 0成功 -1失败
     */
    virtual int push(const char *p_data, int32_t length, int32_t key, int32_t flag) = 0;

    /**
     * @brief 把数据从环形缓冲区取出
     *
     * @param p_data    指向传出buf的缓冲区
     * @param data_len  缓冲区长度,用于内部判断是否会发生缓冲区溢出
     *
     * @return 0成功 -1失败
     */
    virtual int pop(char *p_data, int32_t data_len) = 0;

    /**
     * @brief 为指定进程ID清理相应的数据包,以及信息结构.当进程异常退出时,或者处理超时时,由外部进程调用此接口进行清理
     *
     * @param pid   进程ID
     */
    virtual void clean_4pid(int32_t pid) = 0;

    /**
     * @brief 为指定关键字进行清理数据包,以及信息结构.当进程异常退出时,或者超时时,由外部进程调用此接口进行清理
     *
     * @param key   屏障关键字
     */
    virtual void clean_4key(int32_t key) = 0;

    /**
     * @brief 当前状况时候可以pop
     *
     * @return 0可以pop 1没有可pop的数据
     */
    virtual int is_able_pop() = 0;

    /**
     * @brief 获取上一次出错操作的错误码
     *
     * @return 整型错误码
     */
    virtual int get_last_errno() = 0;

    /**
     * @brief 获取上一次出错操作的错误描述
     *
     * @return 错误描述
     */
    virtual const char *get_last_errstr() = 0;

    /**
     * @brief 获取debug信息
     */
    virtual void get_debug_info(debug_info_t *p_debug_info) = 0;
};

int create_barrier_ring_queue_instance(i_barrier_ring_queue **pp_instance);
#endif // !__I_BARRIER_RING_QUEUE_H__
