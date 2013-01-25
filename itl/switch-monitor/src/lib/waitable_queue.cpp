/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file waitable_queue.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-09-20
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "waitable_queue.h"

i_ring_queue * create_waitable_queue_instance(i_ring_queue *p_ring_queue)
{
	if (p_ring_queue == NULL) {
        return NULL;
    }

    return new (std::nothrow)c_waitable_queue(p_ring_queue);
}

c_waitable_queue::c_waitable_queue(i_ring_queue *p_ring_queue) : 
		m_inited(0), m_read_fd(-1), m_write_fd(-1), m_p_ring_queue(p_ring_queue)
{

}

c_waitable_queue::~c_waitable_queue()
{
	if (m_inited) {
		uninit();
	}
}

int c_waitable_queue::init(int buffer_len)
{
    if (m_inited) {
        return -1;
    }

    // 初始化管道文件描述符
    int pipe_fds[2] = {0};
    if (pipe(pipe_fds) != 0) {
        return -1;
    }

    m_read_fd = pipe_fds[0];
    m_write_fd = pipe_fds[1];

    // 设置管道文件描述符为非阻塞模式
    int flags = fcntl(m_read_fd, F_GETFL);
    fcntl(m_read_fd, F_SETFL, flags | O_NONBLOCK);

    flags = fcntl(m_write_fd, F_GETFL);
    fcntl(m_write_fd, F_SETFL, flags | O_NONBLOCK);

    int rv = m_p_ring_queue->init(buffer_len);
    if (rv == 0) {
        m_inited = 1;
    }
    return rv;
}

int c_waitable_queue::push_data(const char *p_data, int data_len, int is_atomic)
{
	if (!m_inited) {
		return -1;
	}

	int rv = m_p_ring_queue->push_data(p_data, data_len, is_atomic);
    if (rv > 0) {
        write(m_write_fd, "w", 1);
    }

	return rv;
}

int c_waitable_queue::pop_data(char *p_recv_buffer, int buffer_len, int timeout)
{
	if (!m_inited) {
		return -1;
	}

	int rv = pop_data_dummy(p_recv_buffer, buffer_len, timeout);
	if (rv > 0) {
		rv = m_p_ring_queue->pop_data(NULL, rv, 0);
	}

	return rv;
}

int c_waitable_queue::pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout)
{
	if (!m_inited) {
		return -1;
	}
	
	for (;;) {
		int rv = m_p_ring_queue->pop_data_dummy(p_recv_buffer, buffer_len, 0);

		if (rv != 0 || timeout == 0) {
			return rv;
		}

		fd_set read_set;
		FD_ZERO(&read_set);
		FD_SET(m_read_fd, &read_set);

		int fd_num = 0;
		if (timeout < 0) { 
			//block indefinitely
			fd_num = select(m_read_fd + 1, &read_set, NULL, NULL, NULL);
		} else {
			timeval time_interval;
			time_interval.tv_sec = 0;
			time_interval.tv_usec = timeout;
			fd_num = select(m_read_fd + 1, &read_set, NULL, NULL, &time_interval);
		}

		if (fd_num > 0) {
			char temp_buffer[1024];
			read(m_read_fd, temp_buffer, sizeof(temp_buffer));
			continue;
		} else {
			return fd_num;
		}
	}
}

int c_waitable_queue::get_buffer_len()
{
	if (!m_inited) {
		return -1;
	}
	
	return m_p_ring_queue->get_buffer_len();
}

int c_waitable_queue::get_data_len()
{
	if (!m_inited) {
		return -1;
	}
	
	return m_p_ring_queue->get_data_len();
}

int c_waitable_queue::get_empty_buffer_len()
{
	if (!m_inited) {
		return -1;
	}
	
	return m_p_ring_queue->get_empty_buffer_len();
}

int c_waitable_queue::get_last_errno()
{
	return m_p_ring_queue->get_last_errno();
}

const char * c_waitable_queue::get_last_errstr()
{
	return m_p_ring_queue->get_last_errstr();
}

int c_waitable_queue::uninit()
{
	if (!m_inited) {
		return -1;
	}
	
	if (m_read_fd != -1) {
		close(m_read_fd);
		m_read_fd = -1;
	}
	if (m_write_fd != -1) {
		close(m_write_fd);
		m_write_fd = -1;
	}
	
	int rv = 0;
	if (m_p_ring_queue != NULL) {
		rv = m_p_ring_queue->uninit();
	}
	if (rv == 0) {
		m_inited = 0;
	}
	
	return rv;
}

int c_waitable_queue::release()
{
	if (m_p_ring_queue != NULL) {
		m_p_ring_queue->release();
		m_p_ring_queue = NULL;
	}
	
	delete this;

	return 0;
}

