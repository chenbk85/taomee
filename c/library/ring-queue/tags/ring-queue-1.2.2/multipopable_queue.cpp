/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file multipopable_queue.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-03-22
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>

#include <sys/mman.h>

#include "multipopable_queue.h"

i_ring_queue * create_multipopable_queue_instance(i_ring_queue *p_ring_queue)
{
	if (p_ring_queue == NULL) {
        return NULL;
    }

    return new (std::nothrow)c_multipopable_queue(p_ring_queue);
}

c_multipopable_queue::c_multipopable_queue(i_ring_queue *p_ring_queue) : 
		m_inited(0), m_p_mutex(NULL), m_p_ring_queue(p_ring_queue)
{

}

c_multipopable_queue::~c_multipopable_queue()
{
	if (m_inited) {
		uninit();
	}
}

int c_multipopable_queue::init(int buffer_len)
{
	if (m_inited) {
		return -1;
	}

	m_p_mutex = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 
																									-1, 0);
	if (m_p_mutex == MAP_FAILED) {
		return -1;
	}

	if (sem_init(m_p_mutex, 1, 1) != 0) {
		munmap(m_p_mutex, sizeof(sem_t));
		return -1;
	}

	int rv = m_p_ring_queue->init(buffer_len);
	if (rv == 0) {
		m_inited = 1;
	}

	return rv;
}

int c_multipopable_queue::push_data(const char *p_data, int data_len, int is_atomic)
{
	if (!m_inited) {
		return -1;
	}

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

int c_multipopable_queue::pop_data(char *p_recv_buffer, int buffer_len, int timeout)
{
	if (!m_inited) {
		return -1;
	}
	
	int rv = -1;

	rv = m_p_ring_queue->pop_data_dummy(NULL, buffer_len, timeout);

	if (rv > 0) {
		if (sem_wait(m_p_mutex) != 0) {
			return -1;
		}
		rv = m_p_ring_queue->pop_data(p_recv_buffer, buffer_len, 0);
		if (sem_post(m_p_mutex) != 0) {
			return -1;
		}
	}

	return rv;
}

int c_multipopable_queue::pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout)
{
	if (!m_inited) {
		return -1;
	}
	
	return m_p_ring_queue->pop_data_dummy(p_recv_buffer, buffer_len, timeout);
}

int c_multipopable_queue::get_buffer_len()
{
	if (!m_inited) {
		return -1;
	}
	
	return m_p_ring_queue->get_buffer_len();
}

int c_multipopable_queue::get_data_len()
{
	if (!m_inited) {
		return -1;
	}
	
	return m_p_ring_queue->get_data_len();
}

int c_multipopable_queue::get_empty_buffer_len()
{
	if (!m_inited) {
		return -1;
	}
	
	return m_p_ring_queue->get_empty_buffer_len();
}

int c_multipopable_queue::get_last_errno()
{
	return m_p_ring_queue->get_last_errno();
}

const char * c_multipopable_queue::get_last_errstr()
{
	return m_p_ring_queue->get_last_errstr();
}

int c_multipopable_queue::uninit()
{
	if (!m_inited) {
		return -1;
	}
	
	if (sem_destroy(m_p_mutex) != 0) {
		return -1;
	}
	if (munmap(m_p_mutex, sizeof(sem_t)) != 0) {
		return -1;
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

int c_multipopable_queue::release()
{
	if (m_p_ring_queue != NULL) {
		m_p_ring_queue->release();
		m_p_ring_queue = NULL;
	}
	
	delete this;

	return 0;
}

