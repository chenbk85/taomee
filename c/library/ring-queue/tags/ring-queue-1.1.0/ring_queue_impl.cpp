/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file ring_queue_impl.cpp
 * @author xml <xml@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2010-03-23
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "ring_queue_impl.h"

int create_ring_queue_instance(i_ring_queue **pp_instance)
{
    if (NULL == pp_instance) {
        return -1;
    }

    c_ring_queue_impl* p_instance = new (std::nothrow)c_ring_queue_impl();     
	if (NULL == p_instance) {
        return -1;
    } else {
        *pp_instance = dynamic_cast<i_ring_queue*>(p_instance);
        return 0;
    }
}

c_ring_queue_impl::c_ring_queue_impl()
{
	m_p_meta = NULL;
	m_write_fd = -1;
	m_read_fd = -1;
    m_inited = 0;
}

c_ring_queue_impl::~c_ring_queue_impl()
{
    uninit();
}

int c_ring_queue_impl::init(int buffer_len)
{
    if (m_inited) {
        return -1;
    }

	// 预留一个字节的空间，用来区别缓冲区满、缓冲区空这两种状态
	++buffer_len;  

    // 缓冲区的长度不能太大
    if (buffer_len <= 1 || buffer_len >= 1 *1024 * 1024 * 1024) {
        return -1;
    }

	// 为缓冲区分配内存
	assert(m_p_meta == NULL);
	m_p_meta = (ring_queue_meta_t *)mmap(NULL, sizeof(ring_queue_meta_t) + buffer_len, 
									     PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (m_p_meta == MAP_FAILED) {
		return -1;
	}

	// 初始化管道文件描述符
	int pipe_fds[2] = {0};
	if (pipe(pipe_fds) != 0) {
		munmap((char *)m_p_meta, sizeof(m_p_meta) + m_p_meta->buffer_len);
		m_p_meta = NULL;
		return -1;
	}

	m_read_fd = pipe_fds[0];
	m_write_fd = pipe_fds[1];

	// 设置管道文件描述符为非阻塞模式
	int flags = fcntl(m_read_fd, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(m_read_fd,F_SETFL,flags);

	flags = fcntl(m_write_fd, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(m_write_fd,F_SETFL,flags);

    // 初始化成员变量
	m_p_meta->p_buffer = (char *)m_p_meta + sizeof(ring_queue_meta_t);
    m_p_meta->buffer_len = buffer_len;
    m_p_meta->read_index = 0;
    m_p_meta->write_index = 0;
    m_p_meta->last_error = 0;
    m_inited = 1;

    return 0;
}

int c_ring_queue_impl::push_data(const char* p_data, int data_len, int is_atomic)
{
	if (!m_inited) {
        return -1;
    }

    if (p_data == NULL || data_len < 1) {
        return -1;
    }

    int current_read_index = m_p_meta->read_index;

    if (m_p_meta->write_index >= current_read_index) {
        int current_empty_buffer_len = m_p_meta->buffer_len - 
											(m_p_meta->write_index - current_read_index) - 1;
        int copy_len = std::min(data_len, current_empty_buffer_len);

        if (is_atomic && copy_len < data_len) {
            return -1;
        }

        int right_empty_buffer_len = m_p_meta->buffer_len - m_p_meta->write_index;
        if (copy_len < right_empty_buffer_len) {
            memcpy(m_p_meta->p_buffer + m_p_meta->write_index,p_data,copy_len);
            m_p_meta->write_index += copy_len;
			write(m_write_fd,"w",1);
            return copy_len;
        } else {
            memcpy(m_p_meta->p_buffer + m_p_meta->write_index,p_data,right_empty_buffer_len);
            memcpy(m_p_meta->p_buffer,p_data + right_empty_buffer_len,copy_len - right_empty_buffer_len);
            m_p_meta->write_index = copy_len - right_empty_buffer_len;
			write(m_write_fd,"w",1);
            return copy_len;
        }
    } else {
        int current_empty_buffer_len = current_read_index - m_p_meta->write_index - 1;
        int copy_len = std::min(data_len,current_empty_buffer_len);

        if (is_atomic && copy_len < data_len) {
            return -1;
        }

        memcpy(m_p_meta->p_buffer + m_p_meta->write_index,p_data,copy_len);
		m_p_meta->write_index += copy_len;
		write(m_write_fd,"w",1);
		return copy_len;
    }
}

int c_ring_queue_impl::pop_data(char *p_recv_buffer, int buffer_len, int timeout)
{
	if (!m_inited) {
        return -1;
    }

    if (NULL == p_recv_buffer || buffer_len < 1) {
        return -1;
    }

	while (true) {
          if (get_data_len() <= 0) {
            ///no data available,go to select read_fd
			if (timeout != 0) {
				goto SELECT_READ_FD;
			} else {
				return 0;
			}
        } else {
            char ch;
            read(m_read_fd, &ch, sizeof(ch));
			
			int current_write_index = m_p_meta->write_index; /** get current write index snapshot */

			if (current_write_index >= m_p_meta->read_index) {
				int current_data_len = current_write_index - m_p_meta->read_index;
				int copy_len = std::min(current_data_len,buffer_len);
				memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,copy_len);
				m_p_meta->read_index += copy_len;
				return copy_len;
			} else {
				int current_data_len = m_p_meta->buffer_len - (m_p_meta->read_index - current_write_index);
				int copy_len = std::min(current_data_len,buffer_len);

				int right_data_len = m_p_meta->buffer_len - m_p_meta->read_index;
				if (buffer_len < right_data_len) {
					memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,buffer_len);
					m_p_meta->read_index += buffer_len;
					return buffer_len;
				} else {
					memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,right_data_len);
					memcpy(p_recv_buffer + right_data_len,m_p_meta->p_buffer,copy_len - right_data_len);
					m_p_meta->read_index = copy_len - right_data_len;
					return copy_len;
				}
			}
		}

SELECT_READ_FD:
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(m_read_fd,&read_set);
		
		int fd_num = 0;
		if (timeout < 0) {
			fd_num = select(m_read_fd + 1, &read_set, NULL, NULL, NULL);
		} else {
			timeval time_interval;
			time_interval.tv_sec = 0;
			time_interval.tv_usec = timeout;
			fd_num = select(m_read_fd + 1, &read_set, NULL, NULL, &time_interval);
		}
        
		if (fd_num > 0) {
            char temp_buffer[1024] = {0};
            read(m_read_fd, temp_buffer, sizeof(temp_buffer));
        } else {
            return 0;
        }
    }///while
}

int c_ring_queue_impl::pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout)
{
	if (!m_inited) {
        return -1;
    }

    if (NULL == p_recv_buffer || buffer_len < 1) {
        return -1;
    }

	while (true) {
        if (get_data_len() <= 0) {
            ///no data available,go to select read_fd
			if (timeout != 0) {
				goto SELECT_READ_FD;
			} else {
				return 0;
			}
        } else {
			int current_write_index = m_p_meta->write_index;

			if (current_write_index >= m_p_meta->read_index) {
				int current_data_len = current_write_index - m_p_meta->read_index;
				int copy_len = std::min(current_data_len,buffer_len);
				memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,copy_len);
				///m_p_meta->read_index += copy_len;//dummy read
				return copy_len;
			} else {
				int current_data_len = m_p_meta->buffer_len - (m_p_meta->read_index - current_write_index);
				int copy_len = std::min(current_data_len,buffer_len);

				int right_data_len = m_p_meta->buffer_len - m_p_meta->read_index;
				if (buffer_len < right_data_len) {
					memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,buffer_len);
					///m_p_meta->read_index += buffer_len;//dummy read
					return buffer_len;
				} else {
					memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,right_data_len);
					memcpy(p_recv_buffer + right_data_len,m_p_meta->p_buffer,copy_len - right_data_len);
					///m_p_meta->read_index = copy_len - right_data_len;//dummy read
					return copy_len;
				}
			}
		}

SELECT_READ_FD:
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(m_read_fd,&read_set);
		
		int fd_num = 0;
		if (timeout < 0) {
			fd_num = select(m_read_fd + 1, &read_set, NULL, NULL, NULL);
		} else {
			timeval time_interval;
			time_interval.tv_sec = 0;
			time_interval.tv_usec = timeout;
			fd_num = select(m_read_fd + 1, &read_set, NULL, NULL, &time_interval);
		}

        if (fd_num > 0) {
            char temp_buffer[1024] = {0};
            read(m_read_fd, temp_buffer, sizeof(temp_buffer));
        } else {
            return 0;
        }
    }///while
}

int c_ring_queue_impl::get_buffer_len()
{
    if (!m_inited) {
        return -1;
    } else {
		// 预留一个字节的空间，用来区别缓冲区满、缓冲区空这两种状态
        return m_p_meta->buffer_len - 1;
    }
}

int c_ring_queue_impl::get_data_len()
{
    if (!m_inited) {
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

int c_ring_queue_impl::get_empty_buffer_len()
{
    if (!m_inited) {
        return -1;
    }

    return m_p_meta->buffer_len - get_data_len() - 1;
}

int c_ring_queue_impl::get_last_error()
{
    if (!m_inited) {
        return -1;
    }

    return m_p_meta->last_error;
}

int c_ring_queue_impl::uninit()
{
    if (!m_inited) {
        return -1;
    }

	assert(m_p_meta != NULL);
    munmap((char *)m_p_meta, sizeof(m_p_meta) + m_p_meta->buffer_len);

	close(m_write_fd);
	close(m_read_fd);
	m_write_fd = -1;
	m_read_fd = -1;

    ///restore all variables
    m_p_meta = NULL;
	m_inited = 0;

    return 0;
}

int c_ring_queue_impl::release()
{
    delete this;
    return 0;
}

