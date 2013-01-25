
// needed for pthread
#include <pthread.h>

// needed for exit
#include <unistd.h>
#include <stdlib.h>

// needed for write_log
#include "log.h"

// needed for start_listen
#include "listen.h"

#include "thread.h"

static pthread_t thread_listen_server;//创建监听线程

//node组成的file_queue队列是多线程共享的必须互斥来获得和归还一个头节点
pthread_mutex_t file_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  file_queue_cond = PTHREAD_COND_INITIALIZER;

//线程计数变量，当线程数达到最大线程数时，需要主线程等待，多个线程共享读写，需要volatile
volatile int thread_count = 0;

//线程计数变量也需要互斥访问
pthread_mutex_t thread_count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  thread_count_cond = PTHREAD_COND_INITIALIZER;


/*
 * @brief 开启监听线程
 */
void start_threads()
{
	if (pthread_create(&thread_listen_server,NULL,(void*)start_listen,NULL) != 0) {
		write_log("Create 'Loop_Client_Server_Que' thread failed!\n");
		exit(-1);
	}
}

/*
 * @brief 等待监听线程结束
 */
void wait_threads()
{
	//等待线程结束
	if (thread_listen_server != 0) {
		pthread_join(thread_listen_server,NULL);
		write_log("Listening thread finished!\n");
	}

	pthread_mutex_destroy(&thread_count_mutex);
	pthread_cond_destroy(&thread_count_cond);

	pthread_mutex_destroy(&file_queue_mutex);
	pthread_cond_destroy(&file_queue_cond);

}
