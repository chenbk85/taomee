/**
 * @file test_1.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-09-20
 */

#include <stdio.h>

#include <boost/test/minimal.hpp>

#include "i_ring_queue.h"

/**
 * 初始化multipopable_queue的实例，buffer_len分别取大于0、等于0、小于0的值
 */
int test_main(int argc, char **argv)
{
	i_ring_queue *p_ring_queue = NULL;

	BOOST_REQUIRE(create_variable_queue_instance(&p_ring_queue, 2) == 0);
	BOOST_REQUIRE((p_ring_queue = create_multipopable_queue_instance(p_ring_queue)) != NULL);
	BOOST_CHECK(p_ring_queue->init(-1) == -1);
	BOOST_CHECK(p_ring_queue->init(0) == -1);
	BOOST_CHECK(p_ring_queue->init(9) == 0);
	BOOST_REQUIRE(p_ring_queue->release() == 0);

	return 0;
}

