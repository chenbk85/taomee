/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file multipopable_queue_impl.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-03-22
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>

#include <sys/mman.h>

#include "multipopable_queue_impl.h"

i_ring_queue * create_multipopable_queue_instance(i_ring_queue *p_ring_queue)
{
	if (p_ring_queue == NULL) {
        return NULL;
    }

    return new (std::nothrow)c_multipopable_queue_impl(p_ring_queue);
}

c_multipopable_queue_impl::c_multipopable_queue_impl(i_ring_queue *p_ring_queue) : 
		m_p_mutex(NULL), m_p_ring_queue(p_ring_queue)
{

}

c_multipopable_queue_impl::~c_multipopable_queue_impl()
{

}

int c_multipopable_queue_impl::init(int buffer_len)
{
	m_p_mutex = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 
																									-1, 0);
	if (m_p_mutex == MAP_FAILED) {
		return -1;
	}

	if (sem_init(m_p_mutex, 1, 1) != 0) {
		munmap(m_p_mutex, sizeof(sem_t));
		return -1;
	}

	return m_p_ring_queue->init(buffer_len);
}

int c_multipopable_queue_impl::push_data(const char *p_data, int data_len, int is_atomic)
{
	int rv = -1;

	if (sem_wait(m_p_mutex) != 0) {
		return -1;
	}
	rv = m_p_ring_queue->push_data(p_data, data_len, is_atomic);
	if (sem_post(m_p_mutex) != 0) {
		return -1;
	}

	return rv;
}

int c_multipopable_queue_impl::pop_data(char *p_recv_buffer, int buffer_len, int timeout)
{
	int rv = -1;

	rv = m_p_ring_queue->pop_data_dummy(p_recv_buffer, buffer_len, timeout);

	if (rv > 0) {
		if (sem_wait(m_p_mutex) != 0) {
			return -1;
		}
		rv = m_p_ring_queue->pop_data(p_recv_buffer, buffer_len, timeout);
		if (sem_post(m_p_mutex) != 0) {
			return -1;
		}
	}

	return rv;
}

int c_multipopable_queue_impl::pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout)
{
	return m_p_ring_queue->pop_data_dummy(p_recv_buffer, buffer_len, timeout);
}

int c_multipopable_queue_impl::get_buffer_len()
{
	return m_p_ring_queue->get_buffer_len();
}

int c_multipopable_queue_impl::get_data_len()
{
	return m_p_ring_queue->get_data_len();
}

int c_multipopable_queue_impl::get_empty_buffer_len()
{
	return m_p_ring_queue->get_empty_buffer_len();
}

int c_multipopable_queue_impl::get_last_error()
{
	return m_p_ring_queue->get_last_error();
}

int c_multipopable_queue_impl::uninit()
{
	if (sem_destroy(m_p_mutex) != 0) {
		return -1;
	}
	if (munmap(m_p_mutex, sizeof(sem_t)) != 0) {
		return -1;
	}
	
	return m_p_ring_queue->uninit();
}

int c_multipopable_queue_impl::release()
{
	if (m_p_ring_queue->release() != 0) {
		return -1;
	}

	delete this;

	return 0;
}

