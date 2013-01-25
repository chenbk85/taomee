extern "C"{
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <iter_serv/net_if.h>
}
#include "libtaomee++/utils/strings.hpp"
#include <map>

#include "service.h"
#include "mmap.h"
#include "key_proto.h"

#include <map>
using namespace std;


typedef struct _tmr{
	list_head_t timer_list;
} tmr_t;
tmr_t  g_timer;

map<int,int> g_user_cnt_map;
Cmmap *g_mmap = NULL; 

int clear_user_cnt_map(void* owner, void* data)
{
	time_t exptm;
	struct tm tm_tmp = *get_now_tm();
	if (!data) {		
		tm_tmp.tm_hour = 23;
		tm_tmp.tm_min  = 54;
		tm_tmp.tm_sec  = 0;
		exptm   = mktime(&tm_tmp);
		if (exptm < get_now_tv()->tv_sec)
			exptm += 86400;
	} else {
		g_user_cnt_map.clear();
		exptm = get_now_tv()->tv_sec + 86400;
	}
	
	ADD_TIMER_EVENT(&g_timer, clear_user_cnt_map, (void*)1, exptm);
	return 0;
}
	
int init_service()
{
	DEBUG_LOG("INIT...");
	g_mmap = new Cmmap();
	g_mmap->init();

	setup_timer();
	INIT_LIST_HEAD(&g_timer.timer_list);
	clear_user_cnt_map(&g_timer, 0);
	
	return 0;
}

int fini_service()
{
	DEBUG_LOG("FINI...");
	return 0;
}

void proc_events()
{
}

int	get_pkg_len(int fd, const void* pkg, int pkglen)
{
	DEBUG_LOG("===========pkglen:%d", pkglen);
    uint32_t reallen;
    if (pkglen >=4 ){
	    reallen=*((uint32_t* )pkg );
		if(reallen < PROTO_HEADER_SIZE){
			return -1;
		}
		return reallen;
	}
	return 0;
}

int on_pkg_received(int sockfd, void* pkg, int pkglen)
{
	if (pkglen < (int)sizeof(PROTO_HEADER))
		ERROR_RETURN(("pkg too small"), -1);
	
	char sendbuf[256]; 
	PROTO_HEADER *ph = (PROTO_HEADER *)sendbuf;
	ph->cmd = PH_CMD((char*)pkg);
	ph->id = PH_ID((char*)pkg);
	switch(ph->cmd){
		case proto_create_album:
		{
			stru_back_index *p_out = (stru_back_index *)((char*)sendbuf+PROTO_HEADER_SIZE);
			map<int,int>::iterator itr = g_user_cnt_map.find(ph->id);
			if (itr == g_user_cnt_map.end()) {
				g_user_cnt_map[ph->id] = 1;
			} else if (g_user_cnt_map[ph->id] < MAX_COUNT_DAY) {
				g_user_cnt_map[ph->id]++;
			} else {
				ph->len = PROTO_HEADER_SIZE;
				ph->ret = err_get_too_many_keys;
				net_send(sockfd, sendbuf, ph->len);
				ERROR_RETURN(("get too many keys\t[%u %u]", ph->id, ph->cmd), -1);
			}
			
			ph->len = PROTO_HEADER_SIZE + sizeof(*p_out);
			ph->ret = 0;
			p_out->index = g_mmap->get_index();
			DEBUG_LOG("KEY ALBUM ID\t[%u %u]", ph->id, p_out->index);
			net_send(sockfd, sendbuf, ph->len);
			break;
		}
		default:
			ERROR_RETURN(("unknown cmd\t[%u %u]", ph->id, ph->cmd), -1);
	}

	return -1;
}

