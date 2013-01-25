/*
 * cache.cpp
 *
 * Created on:	2011-7-26
 * Author:		Singku
 * Paltform:		Linux Fedora Core 8 x86-32
 *	Compiler:		GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "load_conf_ip_hash_mmap.hpp"
#include "common.h"
#include "proto.h"
#include "process_packet.hpp"
#include "ip_dict.hpp"

extern "C"{
#include <libtaomee/log.h>
#include "net.h"
#include "net_if.h"
}

//#include "send_data.hpp"
#include "cache.hpp"

pthread_spinlock_t lock_cdn_node;

cdn_speed_area_avg_t *cache_cdn = NULL;

idc_test_area_avg_t *cache_idc_area = NULL;

idc_test_ip_avg_t *cache_idc_ip = NULL;

url_test_area_avg_t *cache_url = NULL;

cdn_node_area_avg_t **cache_node = NULL;


uint8_t hash_node(uint32_t node)
{
	uint16_t hash1 = 0, hash0 = 0, h = 0, l = 0;
	h = node; l = node >> 16;
	hash1 = h << 3 | h >> (16-3); hash1 = hash1 ^ l; hash0 = hash1;
	hash1 = l << 3 | l >> (16-3); hash1 = hash1 ^ h; hash0 ^= hash1;

	uint8_t hash11 = 0, hash00 = 0, hh = 0, hl = 0;
	hh = hash0; hl = hash0 >> 8;
	hash11 = hh << 2 | hh >> (8-2); hash11 = hash11 ^ hl; hash00 = hash11;
	hash11 = hl << 2 | hl >> (8-2); hash11 = hash11 ^ hh; hash00 ^= hash11;

	return hash00;
}

int init_cache()
{
	cache_cdn = (cdn_speed_area_avg_t *)malloc(sizeof(cdn_speed_area_avg_t) * max_project_number * MAX_PROVINCE);
	if (cache_cdn == NULL)
		ERROR_RETURN(("INIT CACHE CDN:malloc failed"), -1);
	memset(cache_cdn, 0x0, sizeof(cdn_speed_area_avg_t) * max_project_number * MAX_PROVINCE);

	cache_idc_area = (idc_test_area_avg_t *)malloc(sizeof(idc_test_area_avg_t) * max_idc_number * MAX_PROVINCE);
	if (cache_idc_area == NULL)
		ERROR_RETURN(("INIT CACHE IDC AREA: malloc failed"), -1);
	memset(cache_idc_area, 0x0, sizeof(idc_test_area_avg_t) * max_idc_number * MAX_PROVINCE);

	cache_idc_ip = (idc_test_ip_avg_t *)malloc(sizeof(idc_test_ip_avg_t) * max_idc_number * MAX_IP_RECORD);
	if (cache_idc_ip == NULL)
		ERROR_RETURN(("INIT CACHE IDC IP: malloc failed"), -1);
	memset(cache_idc_ip, 0x0, sizeof(idc_test_ip_avg_t) * max_idc_number * MAX_IP_RECORD);

	cache_url = (url_test_area_avg_t *)malloc(sizeof(url_test_area_avg_t)
							* max_project_number * max_page_per_proj * MAX_PROVINCE);
	if (cache_url == NULL)
		ERROR_RETURN(("INIT CACHE URL:malloc failed"), -1);
	memset(cache_url, 0x0, sizeof(url_test_area_avg_t)
			* max_project_number * max_page_per_proj * MAX_PROVINCE);

	//cdn 节点 为每个项目分配256个hash指针
	cache_node = (cdn_node_area_avg_t **)malloc(sizeof(cdn_node_area_avg_t*) * max_project_number * 256);
	if (cache_node == NULL)
		ERROR_RETURN(("INIT CACHE CDN NODE:malloc failed"), -1);
	int i, j;
	for (i = 0; i < max_project_number; i++) {
		for (j = 0; j < 256; j++) {
			cache_node[i * 256 + j] = NULL;
		}
	}

	return 0;
}

int destroy_cache()
{
	cdn_node_area_avg_t *p,*pnext;
	free(cache_cdn);
	free(cache_idc_area);
	free(cache_idc_ip);
	free(cache_url);
	int i,j;
	for (i = 0; i < max_project_number; i++) {
		for (j = 0; j < 256; j++) {
			pnext = cache_node[i * 256 + j];
			while (pnext != NULL) {
				p = pnext;
				pnext = pnext->next;
				free(p);
			}
		}
	}
	return 0;
}

int update_cache(uint16_t type, void *data)
{
	cdn_speed_area_t *src_cdn_area;
	cdn_speed_area_avg_t *dest_cdn_area;

	cdn_speed_ip_t *src_cdn_ip;
	cdn_node_area_avg_t *dest_cdn_node;
	cdn_node_area_avg_t *tmp;

	idc_test_area_t *src_idc_area;
	idc_test_area_avg_t *dest_idc_area;

	idc_test_ip_t *src_idc_ip;
	idc_test_ip_avg_t *dest_idc_ip;

	url_test_t *src_url;
	url_test_area_avg_t *dest_url;

	int offset;
	int time_step;
	switch (type) {
	case proto_cache_idc_area: {
			src_idc_area = (idc_test_area_t *)data;
			offset = (g_ipDict.m_ipmap[src_idc_area->isp_key].province_code / 10000) % MAX_PROVINCE;
			dest_idc_area = cache_idc_area + ((src_idc_area->idcid - 1) % max_idc_number) * MAX_PROVINCE + offset;
			time_step = (src_idc_area->test_time / 60) % CACHE_TIME;
			if (dest_idc_area->isp_key == 0) {
				dest_idc_area->machine_number = src_idc_area->idcid;
				dest_idc_area->isp_key = src_idc_area->isp_key;
			}
			if (src_idc_area->test_time / 60 > dest_idc_area->data[time_step].test_time / 60) {
				dest_idc_area->data[time_step].test_time = src_idc_area->test_time;
				dest_idc_area->data[time_step].delay = src_idc_area->delay;
				dest_idc_area->data[time_step].loss = src_idc_area->loss;
				dest_idc_area->data[time_step].hop = src_idc_area->hop;
				dest_idc_area->data[time_step].count = src_idc_area->count;
			} else {
				dest_idc_area->data[time_step].test_time =
						max(src_idc_area->test_time, dest_idc_area->data[time_step].test_time);
				dest_idc_area->data[time_step].delay += src_idc_area->delay;
				dest_idc_area->data[time_step].loss += src_idc_area->loss;
				dest_idc_area->data[time_step].hop += src_idc_area->hop;
				dest_idc_area->data[time_step].count += src_idc_area->count;
			}
		}
		break;
	case proto_cache_idc_ip: {
			src_idc_ip = (idc_test_ip_t *)data;
			offset = src_idc_ip->offset;
			dest_idc_ip = cache_idc_ip + ((src_idc_ip->idcid - 1) % max_idc_number) * MAX_IP_RECORD + offset;
			time_step = (src_idc_ip->test_time / 60) % CACHE_TIME;
			if (dest_idc_ip->isp_key == 0) {
				dest_idc_ip->machine_number = src_idc_ip->idcid;
				dest_idc_ip->ipvalue = src_idc_ip->ip;
				dest_idc_ip->isp_key = src_idc_ip->isp_key;
			}
			if (src_idc_ip->test_time / 60 > dest_idc_ip->data[time_step].test_time / 60) {
				dest_idc_ip->data[time_step].test_time = src_idc_ip->test_time;
				dest_idc_ip->data[time_step].delay = src_idc_ip->delay;
				dest_idc_ip->data[time_step].loss = src_idc_ip->loss;
				dest_idc_ip->data[time_step].hop = src_idc_ip->hop;
				dest_idc_ip->data[time_step].count = src_idc_ip->count;
			} else {
				dest_idc_ip->data[time_step].test_time =
						max(src_idc_ip->test_time, dest_idc_ip->data[time_step].test_time);
				dest_idc_ip->data[time_step].delay += src_idc_ip->delay;
				dest_idc_ip->data[time_step].loss += src_idc_ip->loss;
				dest_idc_ip->data[time_step].hop += src_idc_ip->hop;
				dest_idc_ip->data[time_step].count += src_idc_ip->count;
			}
		}
		break;
	case proto_cache_cdn_area: {
			src_cdn_area = (cdn_speed_area_t *)data;
            //INFO_LOG("CCCC:time[%u] speed[%u] count[%u]", 
            //        src_cdn_area->t_time, src_cdn_area->t_speed, src_cdn_area->t_count);
            //int m = 0;
            //for (m = 0; m < 6; m++)
            //{
            //    INFO_LOG("CCCC:dis[%d]: %u", m, src_cdn_area->speed_distribution[m]);
            //}
			offset = (g_ipDict.m_ipmap[src_cdn_area->isp_key].province_code / 10000) % MAX_PROVINCE;
			dest_cdn_area = cache_cdn +
					((src_cdn_area->project_number-1) % max_project_number) * MAX_PROVINCE + offset;
			time_step = (src_cdn_area->t_time / 60) % CACHE_TIME;
			if (dest_cdn_area->isp_key == 0) {
				dest_cdn_area->project_number = src_cdn_area->project_number;
				dest_cdn_area->isp_key = src_cdn_area->isp_key;
			}

            int step = src_cdn_area->step;
			if (src_cdn_area->t_time/60 > dest_cdn_area->data[time_step].test_time/60) {
				dest_cdn_area->data[time_step].test_time = src_cdn_area->t_time;
				dest_cdn_area->data[time_step].speed = src_cdn_area->t_speed;
				dest_cdn_area->data[time_step].count = src_cdn_area->t_count;
				int m;
				for (m = 0; m < 6; m++) {
					dest_cdn_area->speed_distribution[time_step][m] = src_cdn_area->speed_distribution[step][m];
				}
				//memcpy(dest_cdn_area->speed_distribution[time_step], src_cdn_area->speed_distribution, sizeof(dest_cdn_area->speed_distribution[time_step]));
			} else {
				dest_cdn_area->data[time_step].test_time =
						max(src_cdn_area->t_time, dest_cdn_area->data[time_step].test_time);
				dest_cdn_area->data[time_step].speed += src_cdn_area->t_speed;
				dest_cdn_area->data[time_step].count += src_cdn_area->t_count;
				int m;
				for (m = 0; m < 6; m++) {
					dest_cdn_area->speed_distribution[time_step][m] += src_cdn_area->speed_distribution[step][m];
				}
			}
		}
		break;
	case proto_cache_cdn_node: {
			src_cdn_ip = (cdn_speed_ip_t *)data;
			uint32_t node = src_cdn_ip->ip / 256;
			uint8_t index = hash_node(node);

			pthread_spin_lock(&lock_cdn_node);
			tmp = cache_node[ (src_cdn_ip->project_number-1) * 256 + index];
			while (tmp != NULL) {
				if (tmp->node_value == node)
					break;
				tmp = tmp->next;
			}
			if (tmp == NULL) {
				tmp = (cdn_node_area_avg_t *)malloc(sizeof(cdn_node_area_avg_t));
				if(tmp == NULL) {
					pthread_spin_unlock(&lock_cdn_node);
					ERROR_RETURN(("UPDATE CACHE CDN NODE:malloc failed"), -1);
				}

				memset(tmp, 0x0, sizeof(cdn_node_area_avg_t));
				tmp->project_number = src_cdn_ip->project_number;
				tmp->node_value = node;
				//insert into first
				tmp->next = cache_node[ (src_cdn_ip->project_number-1) * 256 + index];
				cache_node[ (src_cdn_ip->project_number-1) * 256 + index] = tmp;
			}
			dest_cdn_node = tmp;
			//find.. then update
			if (dest_cdn_node->isp_key == 0) {
				dest_cdn_node->isp_key = src_cdn_ip->isp_key;
			}

            int step = src_cdn_ip->step;
			time_step = (src_cdn_ip->t_time / 60) % CACHE_TIME;
			if (src_cdn_ip->t_time/60 > dest_cdn_node->data[time_step].test_time/60) {
				dest_cdn_node->data[time_step].test_time = src_cdn_ip->t_time;
				dest_cdn_node->data[time_step].speed = src_cdn_ip->t_speed;
				dest_cdn_node->data[time_step].count = src_cdn_ip->t_count;
				memcpy(dest_cdn_node->speed_distribution[time_step], src_cdn_ip->speed_distribution[step], sizeof(dest_cdn_node->speed_distribution[time_step]));
			}
			else {
				dest_cdn_node->data[time_step].test_time =
						max(src_cdn_ip->t_time, dest_cdn_node->data[time_step].test_time);
				dest_cdn_node->data[time_step].speed += src_cdn_ip->t_speed;
				dest_cdn_node->data[time_step].count += src_cdn_ip->t_count;
				int m;
				for (m = 0; m < 6; m++) {
					dest_cdn_node->speed_distribution[time_step][m] += src_cdn_ip->speed_distribution[step][m];
				}
			}
			pthread_spin_unlock(&lock_cdn_node);
		}
		break;
	case proto_cache_url_area: {
			src_url = (url_test_t*)data;
			offset = (g_ipDict.m_ipmap[src_url->isp_key].province_code / 10000) % MAX_PROVINCE;
			int proj_offset = (src_url->project_number -1 ) % max_project_number;
			int page_offset = ((mmap_file_head_t*)mmap_tmpfile_pmap)->value;
			dest_url = cache_url + proj_offset * max_page_per_proj * MAX_PROVINCE
							+ page_offset * MAX_PROVINCE + offset;
			time_step = (src_url->test_time / 60) % CACHE_TIME;;
			if (dest_url->isp_key == 0) {
				dest_url->project_number = src_url->project_number;
				dest_url->page_id = src_url->page_id;
				dest_url->v_count = src_url->v_count;
				dest_url->isp_key = src_url->isp_key;
			}
			dest_url->v_count = src_url->v_count;
			if(src_url->test_time / 60 > dest_url->data[time_step].test_time / 60) {
				dest_url->data[time_step].test_time = src_url->test_time;
				dest_url->data[time_step].count = src_url->count;
				memcpy(dest_url->data[time_step].vlist, src_url->vlist,
						sizeof(dest_url->data[time_step].vlist));
			} else {
				dest_url->data[time_step].test_time =
						max(src_url->test_time, dest_url->data[time_step].test_time);
				dest_url->data[time_step].count += src_url->count;
				int k;
				for(k = 0;  k < (int)src_url->v_count; k++){
					dest_url->data[time_step].vlist[k] += src_url->vlist[k];
				}
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

void echo_idc_area_really(cache_cmd_t *arg)
{
	pthread_detach(pthread_self());

	idc_test_area_avg_t *p_idc = cache_idc_area + ((arg->idcid-1) % max_idc_number) * MAX_PROVINCE ;
	proto_cache_idc_area_t *tmp;
	char *buf,*buf_ptr;
	uint32_t total_len = 0;
	uint32_t p_len = sizeof(proto_cache_idc_area_t);

	buf = (char*)malloc(1000000);
	if (buf == NULL) {
		ERROR_LOG("ECHO IDC AREA: malloc failed");
		pthread_exit(0);
	}

	buf_ptr = buf;
	buf_ptr += sizeof(protocol_t);
	total_len += sizeof(protocol_t);
	int i, j, pos;
	int now = time(NULL);
	int base_pos = (now / 60) % CACHE_TIME;
	uint32_t avg_delay;
	uint32_t avg_loss;
	uint32_t avg_hop;
	int count;
	for (i = 0 ; i < MAX_PROVINCE; i++) {
		count = 0;
		avg_delay = 0;
		avg_loss = 0;
		avg_hop = 0;
		for (j = 0; j < (int)arg->granularity; j++) {
			pos = (base_pos - j + CACHE_TIME) % CACHE_TIME;

			if (p_idc[i].data[pos].count != 0
					&& (p_idc[i].data[pos].test_time / 60 + j) >= (uint32_t)(now / 60)) {/*跳过过期数据*/
				avg_delay += (p_idc[i].data[pos].delay / p_idc[i].data[pos].count);
				avg_loss += (p_idc[i].data[pos].loss / p_idc[i].data[pos].count);
				avg_hop += (p_idc[i].data[pos].hop / p_idc[i].data[pos].count);
				count++;
			}
		}
		if (count == 0)
			continue;
		p_idc[i].avg_delay = avg_delay / count;
		p_idc[i].avg_loss = avg_loss / count;
		p_idc[i].avg_hop = avg_hop / count;
		tmp = (proto_cache_idc_area_t*)buf_ptr;
		tmp->avg_delay = p_idc[i].avg_delay;
		tmp->avg_loss = p_idc[i].avg_loss;
		tmp->avg_hop = p_idc[i].avg_hop;
		tmp->province_code = g_ipDict.m_ipmap[p_idc[i].isp_key].province_code;
		total_len += p_len;
		buf_ptr += p_len;
	}

	init_proto_head(buf, total_len, arg->cmd, arg->idcid, arg->granularity, 0);
	net_send(arg->sockfd, buf, total_len);
	do_del_conn(arg->sockfd);
	free(buf);
	free(arg);
	pthread_exit(0);
}

int echo_idc_area(int sockfd, uint16_t cmd, uint32_t idcid, uint32_t granularity)
{
	cache_cmd_t *tmp;
	pthread_t athread;
	if (granularity > CACHE_TIME) {
		do_del_conn(sockfd);
		ERROR_RETURN(("IDC_AREA:Requested cache time is too long: %u,max value %u",
				granularity, CACHE_TIME),-1);
	}
	tmp = (cache_cmd_t*)malloc(sizeof(cache_cmd_t));
	if (tmp == NULL)
		ERROR_RETURN(("CACHE IDC MALLOC:failed"),-1);
	tmp->sockfd = sockfd;
	tmp->cmd = cmd;
	tmp->idcid = idcid;
	tmp->granularity = granularity;
	if (pthread_create(&athread, NULL, (void*(*)(void*))echo_idc_area_really, (void*)tmp) != 0)
		ERROR_LOG("can not create thread:echo_idc_data_really");

	return 0;
}

void echo_idc_ip_really(cache_cmd_t *arg)
{
	pthread_detach(pthread_self());

	idc_test_ip_avg_t *p_idc = cache_idc_ip + ((arg->idcid-1) % max_idc_number) * MAX_IP_RECORD ;
	proto_cache_idc_ip_t *tmp;
	char *buf,*buf_ptr;
	uint32_t total_len = 0;
	uint32_t p_len = sizeof(proto_cache_idc_ip_t);

	buf = (char*)malloc(1000000);
	if (buf == NULL) {
		ERROR_LOG("ECHO IDC IP: malloc failed");
		pthread_exit(0);
	}

	buf_ptr = buf;
	buf_ptr += sizeof(protocol_t);
	total_len += sizeof(protocol_t);
	int i, j, pos;
	int now = time(NULL);
	int base_pos = (now / 60) % CACHE_TIME;
	uint32_t avg_delay;
	uint32_t avg_loss;
	uint32_t avg_hop;
	int count;
	for (i = 0 ; i < (int)idc_test_ip_mmap.records_num; i++) {
		count = 0;
		avg_delay = 0;
		avg_loss = 0;
		avg_hop = 0;
		for (j = 0; j < (int)arg->granularity; j++) {
			pos = (base_pos - j + CACHE_TIME) % CACHE_TIME;
			if (p_idc[i].data[pos].count != 0
					&& (p_idc[i].data[pos].test_time / 60 + j) >= (uint32_t)(now / 60)) {/*跳过过期数据*/
				avg_delay += (p_idc[i].data[pos].delay / p_idc[i].data[pos].count);
				avg_loss += (p_idc[i].data[pos].loss / p_idc[i].data[pos].count);
				avg_hop += (p_idc[i].data[pos].hop / p_idc[i].data[pos].count);
				count++;
			}
		}
		if (count == 0)
			continue;
		p_idc[i].avg_delay = avg_delay / count;
		p_idc[i].avg_loss = avg_loss / count;
		p_idc[i].avg_hop = avg_hop / count;
		tmp = (proto_cache_idc_ip_t*)buf_ptr;
		tmp->avg_delay = p_idc[i].avg_delay;
		tmp->avg_loss = p_idc[i].avg_loss;
		tmp->avg_hop = p_idc[i].avg_hop;
		tmp->province_code = g_ipDict.m_ipmap[p_idc[i].isp_key].province_code;
		tmp->city_code = g_ipDict.m_ipmap[p_idc[i].isp_key].city_code;
		tmp->ipvalue = p_idc[i].ipvalue;
		total_len += p_len;
		buf_ptr += p_len;
	}

	init_proto_head(buf, total_len, arg->cmd, arg->idcid, arg->granularity, 0);
	net_send(arg->sockfd, buf, total_len);
	do_del_conn(arg->sockfd);
	free(buf);
	free(arg);
	pthread_exit(0);
}

int echo_idc_ip(int sockfd, uint16_t cmd, uint32_t idcid, uint32_t granularity)
{
	cache_cmd_t *tmp;
	pthread_t athread;
	if (granularity > CACHE_TIME) {
		do_del_conn(sockfd);
		ERROR_RETURN(("IDC_ip:Requested cache time is too long: %u,max value %u",
				granularity, CACHE_TIME),-1);
	}
	tmp = (cache_cmd_t*)malloc(sizeof(cache_cmd_t));
	if (tmp == NULL)
		ERROR_RETURN(("CACHE IDC MALLOC:failed"),-1);
	tmp->sockfd = sockfd;
	tmp->cmd = cmd;
	tmp->idcid = idcid;
	tmp->granularity = granularity;
	if (pthread_create(&athread, NULL, (void*(*)(void*))echo_idc_ip_really, (void*)tmp) != 0)
		ERROR_LOG("can not create thread:echo_idc_data_really");

	return 0;
}


void echo_cdn_area_really(cache_cmd_t *arg)
{
	pthread_detach(pthread_self());

	cdn_speed_area_avg_t *p_cdn = cache_cdn +
										((arg->project_number-1) % max_project_number) * MAX_PROVINCE ;
	proto_cache_cdn_area_t *tmp;
	char *buf,*buf_ptr;
	uint32_t total_len = 0;
	uint32_t p_len = sizeof(proto_cache_cdn_area_t);

	buf = (char*)malloc(1000000);
	if (buf == NULL) {
		ERROR_LOG("ECHO CDN AREA: malloc failed");
		pthread_exit(0);
	}

	buf_ptr = buf;
	buf_ptr += sizeof(protocol_t);
	total_len += sizeof(protocol_t);
	int i, j, m, pos;
	int now = time(NULL);
	int base_pos = (now / 60) % CACHE_TIME;
	uint32_t avg_speed;
    uint32_t speed_dis[6],count_dis[6];
	int count;
    INFO_LOG("####project_number: %u", arg->project_number);
	for (i = 0; i < MAX_PROVINCE; i++) {
		count = 0;
		avg_speed = 0;
        memset(speed_dis, 0, sizeof(speed_dis));
        memset(count_dis, 0, sizeof(count_dis));
		for (j = 0; j < (int)arg->granularity; j++) {
			pos = (base_pos - j + CACHE_TIME) % CACHE_TIME;
			if (p_cdn[i].data[pos].count != 0
				&& (p_cdn[i].data[pos].test_time / 60 + j) >= (uint32_t)(now / 60)) {
				avg_speed += (p_cdn[i].data[pos].speed / p_cdn[i].data[pos].count);
				count++;
                INFO_LOG("pos[%d]: %u/%u(speed/count)", pos, p_cdn[i].data[pos].speed, p_cdn[i].data[pos].count);

                for (m = 0; m <6; m++) { 
                    INFO_LOG(">>>pos[%d]--dis[%d]: %u(count)", pos, m, p_cdn[i].speed_distribution[pos][m]);
                    speed_dis[m] += p_cdn[i].speed_distribution[pos][m];
                    count_dis[m] += (p_cdn[i].speed_distribution[pos][m] == 0) ? 0 : 1;
                }
			}
		}
		if (count == 0)
			continue;
		p_cdn[i].avg_speed = avg_speed / 1024 / count;
		tmp = (proto_cache_cdn_area_t*)buf_ptr;
		tmp->avg_speed = p_cdn[i].avg_speed;
		tmp->province_code = g_ipDict.m_ipmap[p_cdn[i].isp_key].province_code;
		INFO_LOG("province[%u] avg_speed[%u], avg_speed/count[%u/%d]",
                    tmp->province_code, tmp->avg_speed, avg_speed, count);
        int zero_count = 0;
        for (m = 0; m <6; m++) {
            if (count_dis[m] != 0) {
                INFO_LOG("speed_dis[%d]: %u/%u(speed_dis/count_dis)", m, speed_dis[m], count_dis[m]);
                speed_dis[m] /= count_dis[m];
            }
            else {
                zero_count++;
                INFO_LOG("speed_dis[%d]: %u", m, speed_dis[m]);
            }
        }
        if (zero_count == 6)
        {
		    ERROR_LOG("province[%u] avg_speed[%u], no speed_distribution, but avg_speed/count[%u/%d]",
                    tmp->province_code, tmp->avg_speed, avg_speed, count);
        }

		memcpy(tmp->speed_distribution, speed_dis, sizeof(tmp->speed_distribution));
		total_len += p_len;
		buf_ptr += p_len;
	}

	init_proto_head(buf, total_len, arg->cmd, arg->project_number, arg->granularity, 0);
	net_send(arg->sockfd, buf, total_len);
	do_del_conn(arg->sockfd);
	free(buf);
	free(arg);
	pthread_exit(0);
}

int echo_cdn_area(int sockfd, uint16_t cmd, uint32_t project_number, uint32_t granularity)
{
	cache_cmd_t *tmp;
	pthread_t athread;
	if (granularity > CACHE_TIME) {
		do_del_conn(sockfd);
		ERROR_RETURN(("CDN_AREA:Requested cache time is too long: %u,max value %u",
				granularity, CACHE_TIME),-1);
	}
	tmp = (cache_cmd_t*)malloc(sizeof(cache_cmd_t));
	if (tmp == NULL)
		ERROR_RETURN(("CACHE CDN MALLOC:failed"),-1);
	tmp->sockfd = sockfd;
	tmp->cmd = cmd;
	tmp->project_number = project_number;
	tmp->granularity = granularity;
	if (pthread_create(&athread, NULL, (void*(*)(void*))echo_cdn_area_really, (void*)tmp) != 0)
		ERROR_LOG("can not create thread:echo_cdn_data_really");

	return 0;
}

void echo_cdn_node_really(cache_cmd_t *arg)
{
	pthread_detach(pthread_self());

	int addr_offset = ((arg->project_number-1) % max_project_number) * 256;
	cdn_node_area_avg_t *pnext,*ppre;
	proto_cache_cdn_node_t *tmp;
	char *buf,*buf_ptr;
	uint32_t total_len = 0;
	uint32_t p_len = sizeof(proto_cache_cdn_node_t);

	buf = (char*)malloc(1000000);
	if (buf == NULL) {
		ERROR_LOG("ECHO CDN NODE: malloc failed");
		pthread_exit(0);
	}

	buf_ptr = buf;
	buf_ptr += sizeof(protocol_t);
	total_len += sizeof(protocol_t);
	int i, j, m, pos;

	int now = time(NULL);
	int base_pos = (now / 60) % CACHE_TIME;
	uint32_t avg_speed;
	int count;
    uint32_t speed_dis[6], count_dis[6];

	pthread_spin_lock(&lock_cdn_node);
	for (i = 0; i < 256; i++) {
		pnext = cache_node[addr_offset + i];//(p_cdn + i);
		ppre = pnext;
		while (pnext != NULL) {
			count = 0;
			avg_speed = 0;
            memset(speed_dis, 0, sizeof(speed_dis));
            memset(count_dis, 0, sizeof(count_dis));

			int k = 0, loc = 0;
			for (k = 0; k < 5; k++) {
				loc = (base_pos - k + CACHE_TIME) % CACHE_TIME;
				if ((now - (int)(pnext->data[loc].test_time)) < (k+1)*60) //测试现在到过去的5个位置有没有数据
					break;//break for
			}
			if (k == 5) { //5分钟无数据 则删除
				if (pnext == ppre) {
					pnext = pnext->next;
					free(ppre);
					cache_node[addr_offset + i] = pnext;
					ppre = pnext;
				} else {
					ppre->next = pnext->next;
					free(pnext);
					pnext = ppre->next;
				}
				continue;
			}

			for (j = 0; j < (int)(arg->granularity); j++) {
				pos = (base_pos - j + CACHE_TIME) % CACHE_TIME;
				if (pnext->data[pos].count != 0
					&& (pnext->data[pos].test_time / 60 + j) >= (uint32_t)(now / 60)) {
					avg_speed += (pnext->data[pos].speed / pnext->data[pos].count);
					count++;

                    for (m = 0; m < 6; m++) {
                        speed_dis[m] += pnext->speed_distribution[pos][m];
                        count_dis[m] += (pnext->speed_distribution[pos][m] == 0) ?0 :1;
                    }
				}
			}

			if (count == 0)
				pnext->avg_speed = 0;
			else
				pnext->avg_speed = avg_speed / 1024 / count;

			tmp = (proto_cache_cdn_node_t*)buf_ptr;
			tmp->avg_speed = pnext->avg_speed;
			tmp->province_code = g_ipDict.m_ipmap[pnext->isp_key].province_code;
			tmp->city_code = g_ipDict.m_ipmap[pnext->isp_key].city_code;
			tmp->node_value = pnext->node_value;
            for (m = 0; m < 6; m++) {
                if (count_dis[m] != 0) {
                    speed_dis[m] /= count_dis[m];
                }
            }
			memcpy(tmp->speed_distribution, speed_dis, sizeof(tmp->speed_distribution));
			total_len += p_len;
			buf_ptr += p_len;

			ppre = pnext;
			pnext = pnext->next;
		}//while
	}//for
	pthread_spin_unlock(&lock_cdn_node);

	init_proto_head(buf, total_len, arg->cmd, arg->project_number, arg->granularity, 0);
	net_send(arg->sockfd, buf, total_len);
	do_del_conn(arg->sockfd);
	free(buf);
	free(arg);
	pthread_exit(0);
}

int echo_cdn_node(int sockfd, uint16_t cmd, uint32_t project_number, uint32_t granularity)
{
	cache_cmd_t *tmp;
	pthread_t athread;
	if (granularity > CACHE_TIME) {
		do_del_conn(sockfd);
		ERROR_RETURN(("CDN_NODE:Requested cache time is too long: %u,max value %u",
				granularity, CACHE_TIME),-1);
	}
	tmp = (cache_cmd_t*)malloc(sizeof(cache_cmd_t));
	if (tmp == NULL)
		ERROR_RETURN(("CACHE NODE MALLOC:failed"),-1);
	tmp->sockfd = sockfd;
	tmp->cmd = cmd;
	tmp->project_number = project_number;
	tmp->granularity = granularity;
	if (pthread_create(&athread, NULL, (void*(*)(void*))echo_cdn_node_really, (void*)tmp) != 0)
		ERROR_LOG("can not create thread:echo_cdn_node_really");

	return 0;
}

void echo_url_area_really(cache_cmd_t *arg)
{
	pthread_detach(pthread_self());

	int proj_offset = (arg->project_number - 1) % max_project_number;
	int *offset = (int *)g_hash_table_lookup(url_page_index[proj_offset], &arg->pageid);
	if (offset == NULL) {
		do_del_conn(arg->sockfd);
		free(arg);
		pthread_exit(0);
	}

	int page_offset = *offset;
	url_test_area_avg_t *p_url = cache_url
										+ proj_offset * max_page_per_proj * MAX_PROVINCE
										+ page_offset * MAX_PROVINCE;
	proto_cache_url_area_t *tmp;
	char *buf,*buf_ptr;
	uint32_t total_len = 0;
	uint32_t p_len = sizeof(proto_cache_url_area_t);

	buf = (char*)malloc(1000000);
	if (buf == NULL) {
		ERROR_LOG("ECHO URL AREA: malloc failed");
		pthread_exit(0);
	}

	buf_ptr = buf;
	buf_ptr += sizeof(protocol_t);
	total_len += sizeof(protocol_t);
	int i, j, k, pos;
	int now = time(NULL);
	int base_pos = (now / 60) % CACHE_TIME;
	uint32_t avg_spent[MAX_POINT];
	int count[MAX_POINT];
	for (i = 0 ; i < MAX_PROVINCE; i++) {
		if (p_url[i].isp_key == 0)
			continue;
		memset(count, 0x0, sizeof(count));
		memset(avg_spent, 0x0, sizeof(avg_spent));
		tmp = (proto_cache_url_area_t*)buf_ptr;
		for (j = 0; j < (int)arg->granularity; j++) {
			pos = (base_pos - j + CACHE_TIME) % CACHE_TIME;
			if (p_url[i].data[pos].count != 0
				&& (p_url[i].data[pos].test_time / 60 + j) >= (uint32_t)(now / 60)) {
				for (k = 0; k < (int)p_url[i].v_count; k++) {
					avg_spent[k] += (p_url[i].data[pos].vlist[k] / p_url[i].data[pos].count);
					count[k]++;
				}
			}
		}
		for (k = 0; k < (int)p_url[i].v_count; k++) {
			if (count[k] == 0) {
				p_url[i].avg_spent[k] = 0;
				tmp->avg_spent[k] = 0;
				continue;
			}
			p_url[i].avg_spent[k] = avg_spent[k] / count[k];
			tmp->avg_spent[k] = p_url[i].avg_spent[k];
		}
		tmp->province_code = g_ipDict.m_ipmap[p_url[i].isp_key].province_code;
		if (tmp->avg_spent[k-1] !=0) {
			total_len += p_len;
			buf_ptr += p_len;
		}
	}

	init_proto_head(buf, total_len, arg->cmd, arg->project_number, arg->granularity, arg->pageid);
	net_send(arg->sockfd, buf, total_len);

	do_del_conn(arg->sockfd);
	free(buf);
	free(arg);
	pthread_exit(0);
}

int echo_url_area(int sockfd, uint16_t cmd, uint32_t project_number, uint32_t page_id, uint32_t granularity)
{
	cache_cmd_t *tmp;
	pthread_t athread;
	if (granularity > CACHE_TIME) {
		do_del_conn(sockfd);
		ERROR_RETURN(("URL_AREA:Requested cache time is too long: %u,max value %u",
				granularity, CACHE_TIME),-1);
	}

	tmp = (cache_cmd_t*)malloc(sizeof(cache_cmd_t));
	if (tmp == NULL)
		ERROR_RETURN(("CACHE URL MALLOC:failed"),-1);
	tmp->sockfd = sockfd;
	tmp->cmd = cmd;
	tmp->project_number = project_number;
	tmp->pageid = page_id;
	tmp->granularity = granularity;
	if (pthread_create(&athread, NULL, (void*(*)(void*))echo_url_area_really, (void*)tmp) != 0)
		ERROR_LOG("can not create thread:echo_url_data_really");

	return 0;
}

