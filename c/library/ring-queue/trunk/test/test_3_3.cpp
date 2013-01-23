/**
 * @file test_1.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-09-20
 */

#include <stdio.h>

#include <boost/test/minimal.hpp>

#include "i_ring_queue.h"

/**
 * 多次初始化variable_queue的实例
 */
int test_main(int argc, char **argv)
{
	i_ring_queue *p_ring_queue = NULL;

	BOOST_REQUIRE(create_variable_queue_instance(&p_ring_queue, 2) == 0);
	BOOST_CHECK(p_ring_queue->init(1) == 0);
	BOOST_CHECK(p_ring_queue->init(1) == -1);
	BOOST_CHECK(p_ring_queue->init(1) == -1);
	BOOST_REQUIRE(p_ring_queue->uninit() == 0);
	BOOST_CHECK(p_ring_queue->init(1) == 0);
	BOOST_REQUIRE(p_ring_queue->release() == 0);

	return 0;
}

