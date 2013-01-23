/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file fixed_queue_impl.cpp
 * @brief 固定长度的环形缓冲区实现(每次压入的数据长度有固定要求)
 * @author xml <xml@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2010-03-22
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>

#include "fixed_queue_impl.h"

/**
@brief 创建固定长度的环形缓冲区实例

@param pp_instance  二级指针，返回创建实例的地址
@param len  缓冲区每次操作的内存最小单位(fixed是针对这个量描述的)

@return 成功返回0， 失败返回－1
*/
int create_fixed_queue_instance(i_ring_queue **pp_instance, int len)
{
    if (NULL == pp_instance) {
        return -1;
    }

    c_ring_queue_impl* p_instance = new (std::nothrow)c_fixed_queue_impl(len);
    if (NULL == p_instance) {
        return -1;
    } else {
        *pp_instance = dynamic_cast<i_ring_queue*>(p_instance);
        return 0;
    }
}

/**
@brief 构造函数

@param len  缓冲区每次操作的内存最小单位(fixed是针对这个量描述的)

@return 无
*/
c_fixed_queue_impl::c_fixed_queue_impl(int len) : m_len(len)
{

}

/**
@brief 析构函数

@param  无
@return 无
*/
c_fixed_queue_impl::~c_fixed_queue_impl()
{
    uninit();
}

/**
@brief 初始化操作

@param buffer_len  环形缓冲区占用的物理内存长度

@return 成功返回0， 失败返回－1
*/
int c_fixed_queue_impl::init(int buffer_len)
{
	if (m_len <= 0) {
		return -1;
	}

	return c_ring_queue_impl::init(buffer_len);
}

/**
@brief 往缓冲区内压入数据

@param p_data 要压入的数据的内存地址 
@param data_len 要压入的数据的字节长度, 该长度必须是对象内部操作内存最小字节单位(m_len)的整数倍。
@param is_atomic 是否是原子操作，要么全压入，要么一个字节也不压入.

@return 失败返回－1， 失败返回压入字节数*/
int c_fixed_queue_impl::push_data(const char *p_data, int data_len, int is_atomic)
{
    if (!m_inited) {
        return -1;
    }

    if (NULL == p_data || (data_len % m_len) != 0) {
        return -1;
    }

    return c_ring_queue_impl::push_data(p_data, data_len, 1);
}

/**
@brief 从环形缓冲区内取出数据, 一次只取出一个内部操作的单位长度(m_len)

@param p_recv_buffer 接收数据的内存地址 
@param buffer_len 接收数据的内存字节长度 
@param timeout 取数据的等待时长 

@return 成功返回0， 失败返回－1.
*/
int c_fixed_queue_impl::pop_data(char *p_recv_buffer, int buffer_len, int timeout)
{
    if (!m_inited) {
        return -1;
    }

    if (NULL == p_recv_buffer || buffer_len < m_len) {
        return -1;
    }

    return c_ring_queue_impl::pop_data(p_recv_buffer, m_len, timeout);
}

/**
@brief 仅仅从环形缓冲区中读数据， 不改变内部的读指针位置，即可以再次读到刚读出的数据

@param p_recv_buffer 用来接收数据的内存指针 
@param buffer_len 期望读取的数据长度 
@param timeout 等待读数据的时长限制 

@return 成功返回读到的数据字节数， 失败返回－1
*/
int c_fixed_queue_impl::pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout)
{
    if (!m_inited) {
        return -1;
    }

    if (NULL == p_recv_buffer || buffer_len < m_len) {
        return -1;
    }

    return c_ring_queue_impl::pop_data_dummy(p_recv_buffer, m_len, timeout);
}

