#ifndef		PROCESS_PACKET_H_
#define		PROCESS_PACKET_H_
extern "C" {
#include <libtaomee/list.h>
}

typedef struct tmr{
	list_head_t timer_list;
}tmr_t;

extern tmr_t g_timer;
extern char* mmap_tmpfile_pmap;
extern pthread_spinlock_t lock_cdn_ip;

extern uint32_t tmp_packet_tag;
extern "C" int backup_mmap_file(void * owner, void *data);
extern "C" void* send_file(void*);
extern "C" int set_cdn_mmap(char *buf, uint32_t data_len);
extern "C" int set_url_mmap(char *buf, uint32_t data_len);
extern "C" int set_cdn_max_speed(int sockfd, char *buf, uint32_t data_len);
extern "C" int set_cdn_speed_dis(int sockfd, char *buf, uint32_t data_len);

#endif
