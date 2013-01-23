/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file variable_queue_impl.cpp
 * @brief 变长环形缓冲区实现(每次压入的数据长度可适当变化)
 * @author xml <xml@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2010-03-22
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>
#include "variable_queue_impl.h"

/**
@brief 创建变长环形缓冲区

@param pp_instance 二级指针， 返回指向创建的实例的地址的指针 
@param len 环形缓冲区对象内部每次操作内存的最小单位

@return 成功返回0， 失败返回－1
*/
int create_variable_queue_instance(i_ring_queue **pp_instance, int len)
{
    if (NULL == pp_instance) {
        return -1;
    }
	
	if (len != 2 && len != 4) {
		return -1;
	}

    c_ring_queue_impl* p_instance = new (std::nothrow)c_variable_queue_impl(len);
    if (NULL == p_instance) {
        return -1;
    } else {
        *pp_instance = dynamic_cast<i_ring_queue*>(p_instance);
        return 0;
    }
}

/**
@brief 构造函数

@param len 本对象内部每次操作内存的最小单位

@return 无
*/
c_variable_queue_impl::c_variable_queue_impl(int len) : m_len(len)
{
}

/**
@brief 析构函数

@return 无
*/
c_variable_queue_impl::~c_variable_queue_impl()
{
    uninit();
}

/**
@brief 初始化实例所需资源

@param buffer_len 换新缓冲区占用的物理内存大小 

@return */
int c_variable_queue_impl::init(int buffer_len)
{
	if (m_len != 2 && m_len != 4) {
		return -1;
	}

	return c_ring_queue_impl::init(buffer_len);
}


/**
@brief 将数据压入环形缓冲区

@param p_data  要压入的数据的内存指针, 头4个字节是整型数据，是压入数据的长度(变长variable是针对这个描述的)
@param data_len 要压入的数据字节数, 只是个指示信息. 
@param is_atomic 本次操作是否是原子的，要么全压入，要么一个字节也不压入 

@return 错误返回－1， 成功返回压入字节数
*/
int c_variable_queue_impl::push_data(const char* p_data,int data_len,int is_atomic)
{
    if (!m_inited) {
        return -1;
    }

    if (NULL == p_data || data_len <= m_len) {
        return -1;
    }

	char buffer[64] = {0};
	memcpy(buffer, p_data, m_len);
	int pack_len = *(int *)buffer; 

    if (pack_len <= m_len || pack_len > data_len) {
        return -1;
    }

    return c_ring_queue_impl::push_data(p_data, pack_len, 1);
}

/**
@brief 从环形缓冲区获取数据

@param p_recv_buffer 保存获取数据的内存指针
@param buffer_len  保存获取数据的内存长度
@param timeout 等待获取数据的时间限制 

@return */
int c_variable_queue_impl::pop_data(char* p_recv_buffer,int buffer_len, int timeout)
{
    if (!m_inited) {
        return -1;
    }

    if (NULL == p_recv_buffer || buffer_len < 1) {
        return -1;
    }

    int pack_len = 0;
    int result = c_ring_queue_impl::pop_data_dummy((char*)&pack_len, m_len, timeout);
    if (result < 0) {
        return result;
    } else if (result != m_len) {
        return 0;
    } else {
        ///result == sizeof(pack_len)
    }

    if (pack_len <= m_len || pack_len > buffer_len) {
        return -1;
    }

    int data_len = get_data_len();
    if (pack_len > data_len) {
        return 0;
    }

    return c_ring_queue_impl::pop_data(p_recv_buffer, pack_len, timeout);
}

/**
@brief 仅仅从环形缓冲区中读数据， 不改变内部的读指针位置，即可以再次读到刚读出的数据

@param p_recv_buffer 用来接收数据的内存指针 
@param buffer_len 期望读取的数据长度 
@param timeout 等待读数据的时长限制 

@return 成功返回读到的数据字节数， 失败返回－1
*/
int c_variable_queue_impl::pop_data_dummy(char* p_recv_buffer,int buffer_len, int timeout)
{
    if (!m_inited) {
        return -1;
    }

    if (NULL == p_recv_buffer || buffer_len < 1) {
        return -1;
    }

    int pack_len = 0;
    int result = c_ring_queue_impl::pop_data_dummy((char*)&pack_len, m_len, timeout);
    if (result < 0) {
        return result;
    } else if (result != m_len) {
        return 0;
    } else {
        ///result == sizeof(pack_len)
    }

    if (pack_len <= m_len || pack_len > buffer_len) {
        return -1;
    }

    int data_len = get_data_len();
    if (pack_len > data_len) {
        return 0;
    }

    return c_ring_queue_impl::pop_data_dummy(p_recv_buffer, pack_len, timeout);
}

