/**
 * @file test_1.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-09-20
 */

#include <stdio.h>
#include <string.h>

#include <boost/test/minimal.hpp>

#include "i_ring_queue.h"

/**
 * 从fixed_queue中取数据
 */
int test_main(int argc, char **argv)
{
	i_ring_queue *p_ring_queue = NULL;

	BOOST_REQUIRE(create_fixed_queue_instance(&p_ring_queue, 2) == 0);
	BOOST_REQUIRE(p_ring_queue->init(5) == 0);
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 0);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 5);
	BOOST_CHECK(p_ring_queue->push_data("ab", strlen("ab"), 1) == strlen("ab"));
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 2);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 3);
	BOOST_CHECK(p_ring_queue->push_data("cd", strlen("cd"), 1) == strlen("cd"));
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 4);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 1);
	BOOST_CHECK(p_ring_queue->push_data("ef", strlen("ef"), 1) == -1);
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 4);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 1);
	char buffer[4096] = {0};
	BOOST_CHECK(p_ring_queue->pop_data(buffer, sizeof(buffer), 1) == -1);
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 4);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 1);
	BOOST_CHECK(p_ring_queue->pop_data(buffer, sizeof(buffer), -1) == -1);
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 4);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 1);
	BOOST_CHECK(p_ring_queue->pop_data(buffer, 1, 0) == -1);
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 4);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 1);
	BOOST_CHECK(p_ring_queue->pop_data(NULL, 2, 0) == 2);
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 2);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 3);
	BOOST_CHECK(p_ring_queue->pop_data(buffer, 3, 0) == 2);
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 0);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 5);
	BOOST_CHECK(p_ring_queue->pop_data(buffer + 5, 4, 0) == 0);
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 0);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 5);
	BOOST_CHECK(p_ring_queue->pop_data(buffer + 5, 5, 0) == 0);
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 0);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 5);
	BOOST_CHECK(p_ring_queue->pop_data(buffer + 5, 6, 0) == 0);
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 0);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 5);
	BOOST_CHECK(buffer[0] == 'c');
	BOOST_CHECK(buffer[1] == 'd');
		BOOST_CHECK(p_ring_queue->get_buffer_len() == 5);
		BOOST_CHECK(p_ring_queue->get_data_len() == 0);
		BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 5);
	BOOST_CHECK(p_ring_queue->uninit() == 0);
	BOOST_CHECK(p_ring_queue->release() == 0);

	return 0;
}

