/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file ring_queue_impl.cpp
 * @brief 环形缓冲区接实
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

/**
@brief 创建的环形缓冲区实例

@param pp_instance  二级指针，返回创建实例的地址

@return 成功返回0， 失败返回－1
*/
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

/**
@brief 构造函数

@return 无
*/
c_ring_queue_impl::c_ring_queue_impl()
{
    m_p_meta    = NULL;
    m_write_fd  = -1;
    m_read_fd   = -1;
    m_inited    = 0;
}

/**
@brief 析构函数

@return 无
*/
c_ring_queue_impl::~c_ring_queue_impl()
{
    uninit();
}

/**
@brief 初始化环形缓冲区

@param buffer_len  要建立的环形缓冲区实际站的内存大小(有1 BYTE差别)

@return */
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

/**
@brief 将数据压入环形缓冲区

@param p_data    要压入数据的内存指针;
@param data_len  要压入数据的大小, 超过缓冲区的物理内存大小将是不安全的；
@param is_atomic 本函数操作对压入的数据来说是否是原子的(true:要么全压入， 要么1字节也不压入;fasle:根据环形缓冲区内部的空闲空间大小尽可能的压入;)

@return 成功压入缓冲区的数据的长度
*/
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

/**
@brief 从环形缓冲区内存里获取数据

@param p_recv_buffer  接收获取到的数据的存储内存
@param buffer_len     接收数据的内存的大小
@param timeout 等待接收数据的时长限制 

@return 成功返回获取的字节数据，失败返回－1
*/
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
			if (timeout > 0) {
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
        timeval time_interval;
        time_interval.tv_sec = 0;
        time_interval.tv_usec = timeout;

        int fd_num = select(m_read_fd + 1, &read_set, NULL, NULL, &time_interval);
        if (fd_num > 0) {
            char temp_buffer[1024 * 2];
            read(m_read_fd, temp_buffer, sizeof(temp_buffer));
        } else {
            return 0;
        }
    }///while
}

/**
@brief 仅仅从环形缓冲区中读数据，但不移动内部的读指针位置，即读出的数据仍然可以再读对到

@param p_recv_buffer 接收数据的内存指针 
@param buffer_len 接收的数据的内存长度 
@param timeout 等待读数据的时长限制 

@return */
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
            if (timeout > 0) {
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
                return copy_len;
            } else {
                int current_data_len = m_p_meta->buffer_len - (m_p_meta->read_index - current_write_index);
                int copy_len = std::min(current_data_len,buffer_len);

                int right_data_len = m_p_meta->buffer_len - m_p_meta->read_index;
                if (buffer_len < right_data_len) {
                    memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,buffer_len);
                    return buffer_len;
                } else {
                    memcpy(p_recv_buffer,m_p_meta->p_buffer + m_p_meta->read_index,right_data_len);
                    memcpy(p_recv_buffer + right_data_len,m_p_meta->p_buffer,copy_len - right_data_len);
                    return copy_len;
                }
            }
        }

SELECT_READ_FD:
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(m_read_fd,&read_set);
        timeval time_interval;
        time_interval.tv_sec = 0;
        time_interval.tv_usec = timeout;

        int fd_num = select(m_read_fd + 1, &read_set, NULL, NULL, &time_interval);
        if (fd_num > 0) {
            char temp_buffer[1024 * 2];
            read(m_read_fd, temp_buffer, sizeof(temp_buffer));
        } else {
            return 0;
        }
    }///while
}

/**
@brief 获取缓冲区物理内存的字节大小

@return 失败返回－1， 成功返回字节数
*/
int c_ring_queue_impl::get_buffer_len()
{
    if (!m_inited) {
        return -1;
    } else {
		// 预留一个字节的空间，用来区别缓冲区满、缓冲区空这两种状态
        return m_p_meta->buffer_len - 1;
    }
}

/**
@brief 获取缓冲区内有多少字节的可读数据

@return 失败返回－1， 成功返回字节数
*/
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

/**
@brief 获取缓冲区内有多少空闲字节可写数据

@return 失败返回－1， 成功返回0
*/
int c_ring_queue_impl::get_empty_buffer_len()
{
    if (!m_inited) {
        return -1;
    }

    return m_p_meta->buffer_len - get_data_len() - 1;
}

/**
@brief 获取本对象实例的最近一次错误码

@return 错误码*/
int c_ring_queue_impl::get_last_error()
{
    if (!m_inited) {
        return -1;
    }

    return m_p_meta->last_error;
}

/**
@brief 反初始化

@return 成功返回0， 错误返回－1
*/
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

/**
@brief 释放本实例对象资源

@return 返回0
*/
int c_ring_queue_impl::release()
{
    delete this;
    return 0;
}

