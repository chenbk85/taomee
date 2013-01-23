/**
 * @file ring_queue_test.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-06-28
 */

#include <cstring>
#include <stdint.h>
#include <iostream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#include <boost/test/minimal.hpp>

#include "i_ring_queue.h"

using namespace std;

void * ring_queue_test(void * p_arg);

/**
 * 创建可变长度的、可多个进程同时读取的、带通知机制的ring_queue，并压入、读取数据
 */
int test_main(int argc, char **argv)
{
	i_ring_queue *p_ring_queue = NULL;
	char buffer[1024] = {0};

	BOOST_REQUIRE(create_ring_queue_instance(&p_ring_queue) == 0);
//	BOOST_REQUIRE((p_ring_queue = create_multipopable_queue_instance(p_ring_queue)) != NULL);
	BOOST_REQUIRE((p_ring_queue = create_waitable_queue_instance(p_ring_queue)) != NULL);
	BOOST_REQUIRE(p_ring_queue->init(1024) == 0);
	
	BOOST_CHECK(p_ring_queue->get_buffer_len() == 1024);
	BOOST_CHECK(p_ring_queue->get_data_len() == 0);
	BOOST_CHECK(p_ring_queue->get_empty_buffer_len() == 1024);
	
	pid_t pid = fork();
	switch (pid) {
		case -1:
			BOOST_ERROR("FORK");
			return -1;
		case 0:
			for (int i = 0; i != 1000000; ++i) {
				memset(buffer, 0, sizeof(buffer));
				while (p_ring_queue->pop_data(buffer, 3, -1) != (int)strlen("abc")) {
					;
				}
				BOOST_CHECK(buffer[0] == 'a');
				BOOST_CHECK(buffer[1] == 'b');
				BOOST_CHECK(buffer[2] == 'c');
			}

			_exit(0);
		default:
			for (int i = 0; i != 1000000; ++i) {
				while (p_ring_queue->push_data("abc", strlen("abc"), 1) != (int)strlen("abc")) {
					;
				}
				//sleep(1);
			}
			
			break;
	}
	
	waitpid(pid, NULL, 0);

	BOOST_CHECK(p_ring_queue->uninit() == 0);
	BOOST_CHECK(p_ring_queue->release() == 0);

	return 0;
}

void * ring_queue_test(void * p_arg)
{
	i_ring_queue *p_ring_queue = (i_ring_queue *)p_arg; 

	char buffer[1024] = {0};
	for (int i = 0; i != 10000; ++i) {
		memset(buffer, 0, sizeof(buffer));
		while (p_ring_queue->pop_data(buffer, 3, -1) != (int)strlen("abc")) {
			;
		}
		BOOST_CHECK(buffer[0] == 'a');
		BOOST_CHECK(buffer[1] == 'b');
		BOOST_CHECK(buffer[2] == 'c');
	}

	return 0;
}

