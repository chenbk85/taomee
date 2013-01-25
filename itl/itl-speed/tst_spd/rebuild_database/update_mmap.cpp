#include <pthread.h>

extern "C"{
#include <libtaomee/log.h>
#include "common.h"
#include "proto.h"
}

#include "load_conf_ip_hash_mmap.hpp"
#include "polling_cdn_url_files.hpp"
#include "polling_idc_files.hpp"
#include "send_data.hpp"
#include "update_mmap.hpp"

static inline void if_idc_ip_ok(idc_test_ip_t *p)
{
	if (	(p->idc_master == IDC_MASTER_INVALID
			 && p->flag == 1
			 )
		||	(p->idc_master != IDC_MASTER_INVALID
			 && p->flag == p->count_link
			 && p->ext_flag.idc_flag == 1
			 && p->ext_flag.all_idc_flag == 1
			 )
		) {
		if (p->count != 0) {
			p->count = 0;
			if(idc_test_ip_mmap.mmap_head->ready_num != 0)
				idc_test_ip_mmap.mmap_head->ready_num --;
		}
		//idc_test_ip_mmap.ready_num --;
		DEBUG_LOG("IDC NOW:ip_ready\t:[%u]",idc_test_ip_mmap.mmap_head->ready_num);
		DEBUG_LOG("IDC NOW:area_ready\t:[%u]",idc_test_area_mmap.mmap_head->ready_num);

		if (idc_test_ip_mmap.mmap_head->ready_num == 0)
			if (++idc_ip_packet_tag > (PROTO_MAX_TAG >> SHIFT_BITS))//对于每一个文件(mmap)，使用新的标记
				idc_ip_packet_tag = 1;

		if (idc_test_ip_mmap.mmap_head->ready_num == 0
			&& idc_test_area_mmap.mmap_head->ready_num == 0) {
			pthread_mutex_lock(&idc_mutex);
			pthread_cond_signal(&idc_cond);
			pthread_mutex_unlock(&idc_mutex);
		}
	}
}

static inline void if_idc_area_ok(idc_test_area_t *p)
{
	if (p->flag == 1) {
		if(p->count != 0) {
			p->count = 0;
			if(idc_test_area_mmap.mmap_head->ready_num != 0)
				idc_test_area_mmap.mmap_head->ready_num --;
		}
		//idc_test_area_mmap.ready_num -- ;
		DEBUG_LOG("IDC NOW:ip_ready\t:[%u]",idc_test_ip_mmap.mmap_head->ready_num);
		DEBUG_LOG("IDC NOW:area_ready\t:[%u]",idc_test_area_mmap.mmap_head->ready_num);

		if (idc_test_area_mmap.mmap_head->ready_num == 0)
			if (++idc_area_packet_tag > (PROTO_MAX_TAG >> SHIFT_BITS))//对于每一个文件(mmap)，使用新的标记
				idc_area_packet_tag = 1;

		if(idc_test_ip_mmap.mmap_head->ready_num == 0
			&& idc_test_area_mmap.mmap_head->ready_num == 0) {
			pthread_mutex_lock(&idc_mutex);
			pthread_cond_signal(&idc_cond);
			pthread_mutex_unlock(&idc_mutex);
		}
	}
}
static inline void if_cdn_url_mmap_ok(uint32_t ready_num)
{
	if (ready_num == 0) {
		if ( ++cdn_url_packet_tag > (PROTO_MAX_TAG >> SHIFT_BITS))
			cdn_url_packet_tag = 1;
		pthread_mutex_lock(&cdn_and_url_mutex);
		pthread_cond_signal(&cdn_and_url_cond);
		pthread_mutex_unlock(&cdn_and_url_mutex);
	}
}

int update_mmap(uint16_t proto, uint32_t packet_tag, uint32_t offset)
{
	idc_test_ip_t *idc_ip;
	idc_test_area_t *idc_area;
	cdn_speed_ip_t *cdn_ip;
	cdn_speed_area_t *cdn_area;
	url_test_t *url_point;

	static uint64_t recv_idc =0;
	static uint64_t recv_cdn =0;
	static uint64_t recv_url =0;
	uint32_t id = packet_tag & TAG_MASK;
	uint32_t actual_packet_tag = packet_tag >> SHIFT_BITS;

	if (packet_tag == 0) {
		DEBUG_LOG("GET FORWARD RET PACKET___PROTO:%x", proto);
		return 0;
	}

	DEBUG_LOG("GET RET PACKET___PROTO:%x,proj:%u,offset:%u", proto, id, offset);
	if (id == 0) {
		// idc
		DEBUG_LOG("RECV IDC:%lu,SEND IDC:%lu",++recv_idc, send_idc);

		if (actual_packet_tag == idc_ip_packet_tag && actual_packet_tag == idc_area_packet_tag) {
			idc_ip = (idc_test_ip_t*)idc_test_ip_mmap.mmap_p + offset;
			idc_area = (idc_test_area_t*)idc_test_area_mmap.mmap_p + offset;
		}
		else if (actual_packet_tag != idc_ip_packet_tag && actual_packet_tag == idc_area_packet_tag)
			idc_area = (idc_test_area_t*)idc_test_area_mmap.mmap_p + offset;
		else if (actual_packet_tag == idc_ip_packet_tag && actual_packet_tag != idc_area_packet_tag)
			idc_ip = (idc_test_ip_t*)idc_test_ip_mmap.mmap_p + offset;
		else if (actual_packet_tag != idc_ip_packet_tag && actual_packet_tag != idc_area_packet_tag) {
			DEBUG_LOG("THE SAME IDC RESPONSE PACKET");
			return 0;
		}

		switch (proto) {
		case proto_as_net_stat_report_ip: //ip
			idc_ip->flag = 1;
			if_idc_ip_ok(idc_ip);
			return 0;
		case proto_as_net_stat_report_cityisp: //area
			idc_area->flag = 1;
			if_idc_area_ok(idc_area);
			return 0;
		case proto_as_net_stat_report_link: //link
			idc_ip->flag++;
			if_idc_ip_ok(idc_ip);
			return 0;
		case proto_as_net_stat_report_idc: //idc
			idc_ip->ext_flag.idc_flag = 1;
			if_idc_ip_ok(idc_ip);
			return 0;
		case proto_as_net_stat_report_all_idc: //all_idc
			idc_ip->ext_flag.all_idc_flag = 1;
			if_idc_ip_ok(idc_ip);
			return 0;
		default:
			return 0;
		}
	}
	else if (proto == proto_as_cdn_report_cityisp) {
		DEBUG_LOG("RECV CDN:%lu,SEND CDN:%lu",++recv_cdn, send_cdn);

		if (actual_packet_tag != cdn_url_packet_tag) {
			DEBUG_LOG("THE SAME CDN AREA RESPONSE PACKET");
			return 0;
		}

		mmap_file_head_t * cdn_area_file_head;
		cdn_area_file_head = cdn_speed_area_mmap[id-1].mmap_head;
		if(cdn_area_file_head->ready_num == 0)
			ERROR_RETURN(("bad packet"), 0);

		cdn_area_file_head->ready_num--;
		cdn_area = (cdn_speed_area_t*)(cdn_area_file_head + sizeof(mmap_file_head_t)) + offset;
		cdn_area->t_count = 0;

		DEBUG_LOG("CDN AREA NOW:\t:[%u]", cdn_area_file_head->ready_num);
		if_cdn_url_mmap_ok(cdn_area_file_head->ready_num);
		return 0;
	}
	else if (proto == proto_as_cdn_report_ip){
		DEBUG_LOG("RECV CDN:%lu,SEND CDN:%lu",++recv_cdn, send_cdn);
		if (actual_packet_tag != cdn_url_packet_tag) {
			DEBUG_LOG("THE SAME CDN IP RESPONSE PACKET");
			return 0;
		}

		mmap_file_head_t * cdn_ip_file_head;
		cdn_ip_file_head = cdn_speed_ip_mmap[id-1].mmap_head;
		if(cdn_ip_file_head->ready_num == 0)
			ERROR_RETURN(("bad packet"), 0);

		cdn_ip_file_head->ready_num--;
		cdn_ip = (cdn_speed_ip_t*)(cdn_ip_file_head + sizeof(mmap_file_head_t)) + offset;
		cdn_ip->t_count = 0;

		DEBUG_LOG("CDN IP NOW:\t:[%u]", cdn_ip_file_head->ready_num);
		if_cdn_url_mmap_ok(cdn_ip_file_head->ready_num);
		return 0;
	}
	else if (proto == proto_as_url_stat_page_time) {
		DEBUG_LOG("RECV URL:%lu,SEND URL:%lu",++recv_url, send_url);
		if (actual_packet_tag != cdn_url_packet_tag) {
			DEBUG_LOG("THE SAME URL RESPONSE PACKET");
			return 0;
		}

		mmap_file_head_t * url_file_head;
		uint32_t pageid = offset >> SHIFT_BITS;
		uint32_t actual_offset = offset & TAG_MASK;
		uint32_t pos = (id-1) * max_page_per_proj + pageid;
		url_file_head = url_page_mmap[pos].mmap_head;
		if(url_file_head->ready_num == 0)
			ERROR_RETURN(("bad packet"), 0);

		url_file_head->ready_num--;
		url_point = (url_test_t*)(url_file_head + sizeof(mmap_file_head_t)) + actual_offset;
		url_point->count = 0;

		DEBUG_LOG("URL PAGE NOW:\t:[%u]", url_file_head->ready_num);
		if_cdn_url_mmap_ok(url_file_head->ready_num);
		return 0;
	}

	return 0;
}
