/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file ring_queue_impl.h
 * @brief 环形缓冲区实现
 * @author xml <xml@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2010-03-23
 */

#ifndef RING_QUEUE_IMPL_H_2010_03_23
#define RING_QUEUE_IMPL_H_2010_03_23

#include "./i_ring_queue.h"

/**
 * @class c_ring_queue_impl 
 * @brief ring queue implementation for binary data
 */
class c_ring_queue_impl : public i_ring_queue
{
public:
    c_ring_queue_impl();
    virtual ~c_ring_queue_impl();
    virtual int init(int buffer_len);
    virtual int push_data(const char *p_data,int data_len,int is_atomic);
    virtual int pop_data(char *p_recv_buffer, int buffer_len, int timeout = 0);
    virtual int pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout = 0);
    virtual int get_buffer_len();
    virtual int get_data_len();
    virtual int get_empty_buffer_len();
    virtual int get_last_error();
    virtual int uninit();
    virtual int release();

protected:
    
    /**
     * @class ring_queue_meta_t
     * @brief 维护环形缓冲区的物理内存结构
     */
	typedef struct {
		char* p_buffer;                          /**<@brief buffer to simulate ring queue */
		int buffer_len;                          /**<@brief length of whole buffer */
		int read_index;                          /**<@brief read index,start from zero */
		int write_index;                         /**<@brief write index,start from zero */
		int last_error;                          /**<@brief last error code (not used yet) */
	} ring_queue_meta_t;

	int m_inited;                                /**<@brief module status */
    int m_read_fd;                               /**<@brief 如果使用select机制，管道的读描述符 */
    int m_write_fd;                              /**<@brief 如果使用select机制，管道的写描述符 */
	ring_queue_meta_t *m_p_meta;
};

#endif //RING_QUEUE_IMPL_H_2010_03_23

