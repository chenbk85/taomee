#include <sys/mman.h>
#include <pthread.h>
#include <glib.h>
#include <stdlib.h>

extern "C" {
#include <libtaomee/log.h>
#include "net_if.h"
}

#include "include/proto.h"
#include "include/common.h"
#include "load_conf_ip_hash_mmap.hpp"
#include "polling_cdn_url_files.hpp"
#include "polling_idc_files.hpp"
#include "update_mmap.hpp"
#include "send_data.hpp"

extern "C" {

int init_service()
{
	DEBUG_LOG("INIT...");

	pthread_spin_init(&lock_net_send, 0);
	pthread_spin_init(&lock_cdn_ip, 0);

	if (load_conf_ip_hash_mmap_connect() == -1)
		ERROR_RETURN(("load config failed!\t"), -1);


	if(pthread_create(&idc_thread, NULL, polling_files, NULL)!=0)
		ERROR_RETURN(("can not create thread:polling files\t"), -1);

	if(pthread_create(&cdn_and_url_thread, NULL, send_file, NULL)!=0)
		ERROR_RETURN(("can not create thread:polling files\t"), -1);

	
	return 0;
}

int fini_service()
{
	DEBUG_LOG("FINI...");

	g_hash_table_destroy(idc_hash_ip);

	int k;
	for (k = 0; k < max_project_number; k++)
		g_hash_table_destroy(cdn_hash_ip[k]);

	for (k = 0; k < max_project_number; k++)
		g_hash_table_destroy(url_page_index[k]);

	g_hash_table_destroy(hash_cityisp);
	g_hash_table_destroy(link_hash_ip);

	munmap(idc_test_ip_mmap.mmap_head, idc_mmap_size_ip + MMAP_FILE_HEAD_LEN);
	munmap(idc_test_area_mmap.mmap_head, idc_mmap_size_area + MMAP_FILE_HEAD_LEN);
	int i;
	for (i = 0; i < max_project_number; i++) {
		munmap(cdn_speed_ip_mmap[i].mmap_head, cdn_mmap_size_ip + MMAP_FILE_HEAD_LEN);
		munmap(cdn_speed_area_mmap[i].mmap_head, cdn_mmap_size_area + MMAP_FILE_HEAD_LEN);
	}
	for (i = 0; i < max_project_number * max_page_per_proj; i++)
		munmap(url_page_mmap[i].mmap_head, url_mmap_size_page + MMAP_FILE_HEAD_LEN);

	free(cdn_speed_ip_mmap);
	free(url_page_mmap);
	free(url_page_index);
	free(url_page_status);
	free_linkip();

	pthread_spin_destroy(&lock_net_send);
	pthread_spin_destroy(&lock_cdn_ip);

	pthread_mutex_destroy(&idc_mutex);
	pthread_cond_destroy(&idc_cond);

	pthread_mutex_destroy(&cdn_and_url_mutex);
	pthread_cond_destroy(&cdn_and_url_cond);

	return 0;
}

void proc_events()
{
}

int get_pkg_len(int fd, const void* pkg, int pkglen)
{
	if (pkglen < 4)
		return 0;
	const protocol_t* pp = (const protocol_t*)pkg;
	int len = pp->len;
	if ((len > proto_max_len) || (len < (int)sizeof(protocol_t))) {
		ERROR_LOG("[c] invalid len=%d from fd=%d", len, fd);
		return -1;
	}
	return len;
}

int on_pkg_received(int sockfd, void* pkg, int pkglen)
{
	static uint32_t total_packet = 0;
	static uint32_t bad_packet = 0;
	static uint32_t ret_packet = 0;
	protocol_t* pp = (protocol_t*)pkg;
	uint16_t cmd = pp->cmd;


	total_packet ++;
	if (pp->len != (uint32_t)pkglen) {
		bad_packet ++;
		ERROR_RETURN(("error pkg len\t[%u %u]", pp->len, pkglen), -1);
	}

	if(pp->ret != 0 && pp->cmd != proto_cache_url_area) {
		ERROR_LOG("RET NOT ZERO RECEIVE DATA\t[ID:%u,LEN:%u,CMD:%x,PKGLEN:%u,RET:%u]",
				pp->id, pp->len, cmd, pkglen,pp->ret);
		ret_packet ++;
		return 0;
	}

	switch (cmd) {
	case proto_as_cdn_report_ip	:
	case proto_as_cdn_report_cityisp:
	case proto_as_net_stat_report_ip:
	case proto_as_net_stat_report_cityisp:
	case proto_as_net_stat_report_link:
	case proto_as_net_stat_report_idc:
	case proto_as_net_stat_report_all_idc:
	case proto_as_url_stat_page_time:
		return update_mmap(pp->cmd, pp->id, pp->seq);//根据回执包更新正在发送数据的MMAP,以判定所有数据是否发完
	case proto_as_cdn_report_ip_isp:
	case proto_as_url_stat_forward:
		return 0;
	default:
		ERROR_LOG("invalid cmd\t[%u]", cmd);
		break;
	}
	return 0;
}

void on_conn_closed(int sockfd)
{
	int i;
	for (i = 0; i < MAX_DB_SERVER; i++) {
		if (db_server_set[i].fd == sockfd) {
			db_server_set[i].fd = -1;
			break;
		}
	}
}

}
