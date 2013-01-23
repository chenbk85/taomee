/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file variable_queue_impl.h
 * @brief 变长环形缓冲区实现(每次压入的数据长度可适当变化)
 * @author xml <xml@taomee.com>
 * @date 2010-03-22
 */

#ifndef ROUTED_QUEUE_IMPL_H_2010_03_22
#define ROUTED_QUEUE_IMPL_H_2010_03_22

#include "ring_queue_impl.h"

/**
 * @class c_variable_queue_impl
 * @brief 变长环形缓冲区
 */
class c_variable_queue_impl : public c_ring_queue_impl
{
public:
    c_variable_queue_impl(int len);
    virtual ~c_variable_queue_impl();
    virtual int init(int buffer_len);
    virtual int push_data(const char *p_data, int data_len, int is_atomic);
    virtual int pop_data(char *p_recv_buffer, int buffer_len, int timeout = 0);
    virtual int pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout = 0);

private:
	int m_len; /**< @brief 本对象内部操作环形缓冲区的最小长度单位 */
};

#endif //ROUTED_QUEUE_IMPL_H_2010_03_22

