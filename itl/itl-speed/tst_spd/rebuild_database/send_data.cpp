#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

extern "C"{
#include "net_if.h"
#include <libtaomee/log.h>
}

#include "load_conf_ip_hash_mmap.hpp"
#include "send_data.hpp"
//#include "proto.h"

uint64_t send_idc = 0;
uint64_t send_cdn = 0;
uint64_t send_url = 0;
static uint64_t total_send = 0;

pthread_spinlock_t lock_net_send;

int send_data(uint8_t* buf, int len, int flag)
{
	int ret_of_net_send;
	uint32_t wait_secs = 1;
	while(db_server_set[0].fd == -1)
	{
		sleep(wait_secs);
		db_server_set[0].fd = connect_to_svr(
			db_server_set[0].ip, db_server_set[0].port, 8*1024, 1);
		wait_secs = (wait_secs > 128) ?1 :wait_secs*2 ;
	}
	if (db_server_set[0].fd == -1)
		ERROR_RETURN(("cant connect to db server\t"), -1);
	if (flag >= 0x1214 && flag <= 0x1215)
		return 0;
	if (flag >= 0x1211 && flag <= 0x1213)
		DEBUG_LOG("\tSEND IDC:%lu",++send_idc);
	else if (flag == 0x1001 || flag ==0x1002)
		DEBUG_LOG("\tSEND CDN:%lu",++send_cdn);
	else if (flag == 0x1018)
		DEBUG_LOG("\tSEND URL:%lu",++send_url);
	
	pthread_spin_lock(&lock_net_send);
	
   total_send ++;
	ret_of_net_send = net_send(db_server_set[0].fd, buf, len);

	pthread_spin_unlock(&lock_net_send);
    
	if( ret_of_net_send == 0)
		return 0;
	else
		ERROR_RETURN(("send data failed\t"), -1);
}
