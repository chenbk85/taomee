/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file waitable_queue.h
 * @author richard <richard@taomee.com>
 * @date 2010-09-20
 */

#ifndef	WAITABLE_QUEUE_H_2010_09_20
#define WAITABLE_QUEUE_H_2010_09_20

#include <semaphore.h>

#include "i_ring_queue.h"

class c_waitable_queue : public i_ring_queue
{
public:
    c_waitable_queue(i_ring_queue *p_ring_queue);
    virtual ~c_waitable_queue();
    virtual int init(int buffer_len);
    virtual int push_data(const char *p_data, int data_len, int is_atomic);
    virtual int pop_data(char *p_recv_buffer, int buffer_len, int timeout = 0);
    virtual int pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout = 0);
	virtual int get_buffer_len();
    virtual int get_data_len();
	virtual int get_empty_buffer_len();
	virtual int get_last_errno();
	virtual const char * get_last_errstr();
	virtual int uninit();
	virtual int release();

private:
	int m_inited;
	int m_read_fd;                               /**< 如果使用select机制，管道的读描述符 */
    int m_write_fd;                              /**< 如果使用select机制，管道的写描述符 */
	i_ring_queue *m_p_ring_queue;
};

#endif //WAITABLE_QUEUE_H_2010_09_20

