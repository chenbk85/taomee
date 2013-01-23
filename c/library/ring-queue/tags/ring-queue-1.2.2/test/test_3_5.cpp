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
 * 从variable_queue中读数据
 */
int test_main(int argc, char **argv)
{
	i_ring_queue *p_ring_queue = NULL;

	BOOST_REQUIRE(create_variable_queue_instance(&p_ring_queue, 2) == 0);
	BOOST_REQUIRE(p_ring_queue->init(10) == 0);
	BOOST_CHECK(p_ring_queue->push_data("\003\0a", 3, 1) == 3);
	BOOST_CHECK(p_ring_queue->push_data("\003\0b", 3, 1) == 3);
	BOOST_CHECK(p_ring_queue->push_data("\003\0c", 3, 1) == 3);
	char buffer[4096] = {0};
	BOOST_CHECK(p_ring_queue->pop_data_dummy(buffer, sizeof(buffer), 1) == -1);
	BOOST_CHECK(p_ring_queue->pop_data_dummy(buffer, sizeof(buffer), -1) == -1);
	BOOST_CHECK(p_ring_queue->pop_data_dummy(buffer, 1, 0) == -1);
	BOOST_CHECK(p_ring_queue->pop_data_dummy(buffer, 2, 0) == -1);
	BOOST_CHECK(p_ring_queue->pop_data_dummy(buffer, 3, 0) == 3);
	BOOST_CHECK(p_ring_queue->pop_data_dummy(buffer, 4, 0) == 3);
	BOOST_CHECK(p_ring_queue->pop_data_dummy(buffer, 5, 0) == 3);
	BOOST_CHECK(p_ring_queue->pop_data_dummy(buffer, 6, 0) == 3);
	BOOST_CHECK(buffer[0] == '\003');
	BOOST_CHECK(buffer[1] == '\0');
	BOOST_CHECK(buffer[2] == 'a');
	BOOST_CHECK(p_ring_queue->uninit() == 0);
	BOOST_CHECK(p_ring_queue->release() == 0);

	return 0;
}

