/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file variable_queue.cpp
 * @author xml <xml@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2010-03-22
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>

#include "variable_queue.h"
#include "error_list.h"

int create_variable_queue_instance(i_ring_queue **pp_instance, int len)
{
    if (NULL == pp_instance) {
        return -1;
    }
	
	if (len != 2 && len != 4) {
		return -1;
	}

    c_ring_queue *p_instance = new (std::nothrow)c_variable_queue(len);
    if (NULL == p_instance) {
        return -1;
    } else {
        *pp_instance = dynamic_cast<i_ring_queue*>(p_instance);
        return 0;
    }
}

c_variable_queue::c_variable_queue(int len) : m_len(len)
{
}

c_variable_queue::~c_variable_queue()
{
	if (m_inited) {
		uninit();
	}
}

int c_variable_queue::init(int buffer_len)
{
	if (m_len != 2 && m_len != 4) {
		m_last_errno = E_PARAM;
		return -1;
	}

	return c_ring_queue::init(buffer_len);
}


int c_variable_queue::push_data(const char *p_data,int data_len,int is_atomic)
{
    if (!m_inited) {
        m_last_errno = E_NOT_INIT;
		return -1;
    }

    if (p_data == NULL || data_len <= m_len) {
		m_last_errno = E_PARAM;
        return -1;
    }

	char buffer[64] = {0};
	memcpy(buffer, p_data, m_len);
	int pack_len = *(int *)buffer; 

    if (pack_len <= m_len || pack_len > data_len) {
		m_last_errno = E_PARAM;
        return -1;
    }

    return c_ring_queue::push_data(p_data, pack_len, 1);
}

int c_variable_queue::pop_data(char *p_recv_buffer,int buffer_len, int timeout)
{
    if (!m_inited) {
        m_last_errno = E_NOT_INIT;
        return -1;
    }

    if (buffer_len < 1) {
		m_last_errno = E_PARAM;
        return -1;
    }

    int pack_len = 0;
    int result = c_ring_queue::pop_data_dummy((char*)&pack_len, m_len, timeout);
    if (result < 0) {
        return result;
    } else if (result != m_len) {
        return 0;
    } else {
        ///result == sizeof(pack_len)
    }

    if (pack_len <= m_len || pack_len > buffer_len) {
		m_last_errno = E_PARAM;
        return -1;
    }

    int data_len = get_data_len();
    if (pack_len > data_len) {
        return 0;
    }

    return c_ring_queue::pop_data(p_recv_buffer, pack_len, timeout);
}

int c_variable_queue::pop_data_dummy(char* p_recv_buffer,int buffer_len, int timeout)
{
    if (!m_inited) {
        m_last_errno = E_NOT_INIT;
		return -1;
    }

    if (buffer_len < 1) {
		m_last_errno = E_PARAM;
        return -1;
    }

    int pack_len = 0;
    int result = c_ring_queue::pop_data_dummy((char*)&pack_len, m_len, timeout);
    if (result < 0) {
        return result;
    } else if (result != m_len) {
        return 0;
    } else {
        ///result == sizeof(pack_len)
    }

    if (pack_len <= m_len || pack_len > buffer_len) {
		m_last_errno = E_PARAM;
        return -1;
    }

    int data_len = get_data_len();
    if (pack_len > data_len) {
        return 0;
    }

    return c_ring_queue::pop_data_dummy(p_recv_buffer, pack_len, timeout);
}

