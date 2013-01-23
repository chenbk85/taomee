/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file multipopable_queue_impl.h
 * @author richard <richard@taomee.com>
 * @date 2010-08-05
 */

#ifndef	MULTIPOPABLE_QUEUE_IMPL_H_2010_08_05
#define MULTIPOPABLE_QUEUE_IMPL_H_2010_08_05

#include <semaphore.h>

#include "i_ring_queue.h"

class c_multipopable_queue_impl : public i_ring_queue
{
public:
    c_multipopable_queue_impl(i_ring_queue *p_ring_queue);
    virtual ~c_multipopable_queue_impl();
    virtual int init(int buffer_len);
    virtual int push_data(const char *p_data, int data_len, int is_atomic);
    virtual int pop_data(char *p_recv_buffer, int buffer_len, int timeout = 0);
    virtual int pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout = 0);
	virtual int get_buffer_len();
    virtual int get_data_len();
	virtual int get_empty_buffer_len();
	virtual int get_last_error();
	virtual int uninit();
	virtual int release();

private:
	sem_t *m_p_mutex;
	i_ring_queue *m_p_ring_queue;
};

#endif //MULTIPOPABLE_QUEUE_IMPL_H_2010_08_05

