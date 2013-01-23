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
 * 原子地向ring_queue中压入数据
 */
int test_main(int argc, char **argv)
{
	i_ring_queue *p_ring_queue = NULL;

	BOOST_REQUIRE(create_ring_queue_instance(&p_ring_queue) == 0);
	BOOST_REQUIRE(p_ring_queue->init(5) == 0);
	BOOST_CHECK(p_ring_queue->push_data("abc", strlen("abc"), 1) == strlen("abc"));
	BOOST_CHECK(p_ring_queue->push_data("abc", strlen("abc"), 1) == -1);
	BOOST_CHECK(p_ring_queue->push_data("ab", strlen("ab"), 1) == strlen("ab"));
	BOOST_CHECK(p_ring_queue->uninit() == 0);
	BOOST_CHECK(p_ring_queue->release() == 0);

	return 0;
}

