/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file fixed_queue.cpp
 * @author xml <xml@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2010-03-22
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>

#include "fixed_queue.h"
#include "error_list.h"

int create_fixed_queue_instance(i_ring_queue **pp_instance, int len)
{
    if (NULL == pp_instance || len <= 0) {
        return -1;
    }

    c_ring_queue *p_instance = new (std::nothrow)c_fixed_queue(len);
    if (NULL == p_instance) {
        return -1;
    } else {
        *pp_instance = dynamic_cast<i_ring_queue*>(p_instance);
        return 0;
    }
}

c_fixed_queue::c_fixed_queue(int len) : m_len(len)
{

}

c_fixed_queue::~c_fixed_queue()
{
	if (m_inited) {
		uninit();
	}
}

int c_fixed_queue::init(int buffer_len)
{
	if (m_len <= 0) {
		m_last_errno = E_PARAM;
		return -1;
	}

	return c_ring_queue::init(buffer_len);
}

int c_fixed_queue::push_data(const char *p_data, int data_len, int is_atomic)
{
    if (!m_inited) {
		m_last_errno = E_NOT_INIT;
        return -1;
    }

    if (NULL == p_data || (data_len % m_len) != 0) {
		m_last_errno = E_PARAM;
        return -1;
    }

    return c_ring_queue::push_data(p_data, data_len, 1);
}

int c_fixed_queue::pop_data(char *p_recv_buffer, int buffer_len, int timeout)
{
    if (!m_inited) {
		m_last_errno = E_NOT_INIT;
        return -1;
    }

    if (buffer_len < m_len) {
		m_last_errno = E_PARAM;
        return -1;
    }

    return c_ring_queue::pop_data(p_recv_buffer, m_len, timeout);
}

int c_fixed_queue::pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout)
{
    if (!m_inited) {
		m_last_errno = E_NOT_INIT;
        return -1;
    }

    if (buffer_len < m_len) {
		m_last_errno = E_PARAM;
        return -1;
    }

    return c_ring_queue::pop_data_dummy(p_recv_buffer, m_len, timeout);
}

