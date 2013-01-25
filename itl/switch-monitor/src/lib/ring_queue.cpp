/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file ring_queue.cpp
 * @author xml <xml@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2010-03-23
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>
#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "ring_queue.h"
#include "error_list.h"

int create_ring_queue_instance(i_ring_queue **pp_instance)
{
    if (pp_instance == NULL) {
        return -1;
    }

    c_ring_queue *p_instance = new (std::nothrow)c_ring_queue();     
	if (p_instance == NULL) {
        return -1;
    } else {
        *pp_instance = dynamic_cast<i_ring_queue *>(p_instance);
        return 0;
    }
}

c_ring_queue::c_ring_queue() : m_inited(0), m_last_errno(0), m_p_meta(NULL) 
{

}

c_ring_queue::~c_ring_queue()
{
	if (m_inited) {
		uninit();
	}
}

int c_ring_queue::init(int buffer_len)
{
    if (m_inited) {
		m_last_errno = E_ALREADY_INITED;
		return -1;
    }

	// 预留一个字节的空间，用来区别缓冲区满、缓冲区空这两种状态
	++buffer_len;  

    // 缓冲区的长度不能太大
    if (buffer_len <= 1 || buffer_len >= 1 *1024 * 1024 * 1024) {
		m_last_errno = E_PARAM;
        return -1;
    }

	// 为缓冲区分配内存
	assert(m_p_meta == NULL);
	m_p_meta = (ring_queue_meta_t *)mmap(NULL, sizeof(ring_queue_meta_t) + buffer_len, 
									     PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (m_p_meta == MAP_FAILED) {
		if (errno == ENOMEM) {
			m_last_errno = E_MEMORY;
		} else {
			m_last_errno = E_SYSTEM; 
		}
		return -1;
	}

    // 初始化成员变量
	m_p_meta->p_buffer = (char *)m_p_meta + sizeof(ring_queue_meta_t);
    m_p_meta->buffer_len = buffer_len;
    m_p_meta->read_index = 0;
    m_p_meta->write_index = 0;
    m_last_errno = 0;
	m_inited = 1;

    return 0;
}

int c_ring_queue::push_data(const char* p_data, int data_len, int is_atomic)
{
	if (!m_inited) {
		m_last_errno = E_NOT_INIT;
        return -1;
    }

    if (p_data == NULL || data_len < 1) {
		m_last_errno = E_PARAM;
        return -1;
    }

    int current_read_index = m_p_meta->read_index;

    if (m_p_meta->write_index >= current_read_index) {
        int current_empty_buffer_len = m_p_meta->buffer_len - 
											(m_p_meta->write_index - current_read_index) - 1;
        int copy_len = std::min(data_len, current_empty_buffer_len);

        if (is_atomic && copy_len < data_len) {
			m_last_errno = E_IDLE_BUFFER;
            return -1;
        }

        int right_empty_buffer_len = m_p_meta->buffer_len - m_p_meta->write_index;
        if (copy_len < right_empty_buffer_len) {
            memcpy(m_p_meta->p_buffer + m_p_meta->write_index,p_data,copy_len);
            m_p_meta->write_index += copy_len;
            return copy_len;
        } else {
            memcpy(m_p_meta->p_buffer + m_p_meta->write_index,p_data,right_empty_buffer_len);
            memcpy(m_p_meta->p_buffer,p_data + right_empty_buffer_len,copy_len - right_empty_buffer_len);
            m_p_meta->write_index = copy_len - right_empty_buffer_len;
            return copy_len;
        }
    } else {
        int current_empty_buffer_len = current_read_index - m_p_meta->write_index - 1;
        int copy_len = std::min(data_len,current_empty_buffer_len);

        if (is_atomic && copy_len < data_len) {
			m_last_errno = E_IDLE_BUFFER;
            return -1;
        }

        memcpy(m_p_meta->p_buffer + m_p_meta->write_index,p_data,copy_len);
		m_p_meta->write_index += copy_len;
		return copy_len;
    }
}

int c_ring_queue::pop_data(char *p_recv_buffer, int buffer_len, int timeout)
{
	if (!m_inited) {
		m_last_errno = E_NOT_INIT;
        return -1;
    }

    if (buffer_len < 1 || timeout != 0) {
        m_last_errno = E_PARAM;
		return -1;
    }

	if (get_data_len() <= 0) {
		return 0;
	}

	int current_write_index = m_p_meta->write_index; /** get current write index snapshot */

	if (current_write_index >= m_p_meta->read_index) {
		int current_data_len = current_write_index - m_p_meta->read_index;
		int copy_len = std::min(current_data_len,buffer_len);
		if (p_recv_buffer != NULL) {
			memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,copy_len);
		}
		m_p_meta->read_index += copy_len;
		return copy_len;
	} else {
		int current_data_len = m_p_meta->buffer_len - (m_p_meta->read_index - current_write_index);
		int copy_len = std::min(current_data_len,buffer_len);

		int right_data_len = m_p_meta->buffer_len - m_p_meta->read_index;
		if (buffer_len < right_data_len) {
			if (p_recv_buffer != NULL) {
				memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,buffer_len);
			}
			m_p_meta->read_index += buffer_len;
			return buffer_len;
		} else {
			if (p_recv_buffer != NULL) {
				memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,right_data_len);
				memcpy(p_recv_buffer + right_data_len,m_p_meta->p_buffer,copy_len - right_data_len);
			}
			m_p_meta->read_index = copy_len - right_data_len;
			return copy_len;
		}
	}
}

int c_ring_queue::pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout)
{
	if (!m_inited) {
		m_last_errno = E_NOT_INIT;
        return -1;
    }

    if (buffer_len < 1 || timeout != 0) {
        m_last_errno = E_PARAM;
		return -1;
    }

	if (get_data_len() <= 0) {
		return 0;
	}
	int current_write_index = m_p_meta->write_index;

	if (current_write_index >= m_p_meta->read_index) {
		int current_data_len = current_write_index - m_p_meta->read_index;
		int copy_len = std::min(current_data_len,buffer_len);
		if (p_recv_buffer != NULL) {
			memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,copy_len);
		}
		///m_p_meta->read_index += copy_len;//dummy read
		return copy_len;
	} else {
		int current_data_len = m_p_meta->buffer_len - (m_p_meta->read_index - current_write_index);
		int copy_len = std::min(current_data_len,buffer_len);

		int right_data_len = m_p_meta->buffer_len - m_p_meta->read_index;
		if (buffer_len < right_data_len) {
			if (p_recv_buffer != NULL) {
				memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,buffer_len);
			}
			///m_p_meta->read_index += buffer_len;//dummy read
			return buffer_len;
		} else {
			if (p_recv_buffer != NULL) {
				memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,right_data_len);
				memcpy(p_recv_buffer + right_data_len,m_p_meta->p_buffer,copy_len - right_data_len);
			}
			///m_p_meta->read_index = copy_len - right_data_len;//dummy read
			return copy_len;
		}
	}
}


int c_ring_queue::get_buffer_len()
{
    if (!m_inited) {
		m_last_errno = E_NOT_INIT;
        return -1;
    } else {
		// 预留一个字节的空间，用来区别缓冲区满、缓冲区空这两种状态
        return m_p_meta->buffer_len - 1;
    }
}

int c_ring_queue::get_data_len()
{
    if (!m_inited) {
		m_last_errno = E_NOT_INIT;
        return -1;
    }

    ///read index and write index are snapshot
    int current_write_index = m_p_meta->write_index;
    int current_read_index = m_p_meta->read_index;

    if (current_write_index >= current_read_index) {
        return current_write_index - current_read_index;
    } else {
        return m_p_meta->buffer_len - (current_read_index - current_write_index);
    }
}

int c_ring_queue::get_empty_buffer_len()
{
    if (!m_inited) {
		m_last_errno = E_NOT_INIT;
        return -1;
    }

    return m_p_meta->buffer_len - get_data_len() - 1;
}

int c_ring_queue::get_last_errno()
{
	return m_last_errno;
}

const char * c_ring_queue::get_last_errstr()
{
	return get_errstr(m_last_errno);
}

int c_ring_queue::uninit()
{
    if (!m_inited) {
		m_last_errno = E_NOT_INIT;
        return -1;
    }

	assert(m_p_meta != NULL);
    munmap((char *)m_p_meta, sizeof(m_p_meta) + m_p_meta->buffer_len);

    ///restore all variables
    m_p_meta = NULL;
	m_inited = 0;

    return 0;
}

int c_ring_queue::release()
{
    delete this;
    return 0;
}

