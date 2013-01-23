/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file fixed_queue_impl.cpp
 * @author xml <xml@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2010-03-22
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>

#include "fixed_queue_impl.h"

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

c_fixed_queue_impl::c_fixed_queue_impl(int len) : m_len(len)
{

}

c_fixed_queue_impl::~c_fixed_queue_impl()
{
    uninit();
}

int c_fixed_queue_impl::init(int buffer_len)
{
	if (m_len <= 0) {
		return -1;
	}

	return c_ring_queue_impl::init(buffer_len);
}

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

