#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
//need timespec and time()
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

extern "C"{
#include <libtaomee/log.h>
}
#include "proto.h"
#include "common.h"

#include "ip_dict.hpp"
#include "load_conf_ip_hash_mmap.hpp"
#include "polling_idc_files.hpp"
#include "send_data.hpp"

static int send_idc_ip_2_dbserver();
static int send_idc_area_2_dbserver();

static uint32_t idc_start = 0;
uint32_t idc_ip_packet_tag = 1;
uint32_t idc_area_packet_tag = 1;

static inline void wait_all_data_ok()
{
	//发完了需要等到所有记录的回执都收到才发下一次
	timespec timeout;
	int ret;

re_wait:
	pthread_mutex_lock(&idc_mutex);
	if (idc_test_ip_mmap.mmap_head->ready_num != 0 || idc_test_area_mmap.mmap_head->ready_num != 0) {
		//由于文件是每分钟60秒一个，故折中取30秒为本次发送文件的等待超时时间
		timeout.tv_sec = time(0) + 30;
		timeout.tv_nsec = 0;
		ret = pthread_cond_timedwait(&idc_cond, &idc_mutex, &timeout);
	}
	pthread_mutex_unlock(&idc_mutex);
	if (idc_test_ip_mmap.mmap_head->ready_num != 0) {
		//等待收包的过程超时;
		ERROR_LOG("IDC_IP:WAIT DB RETURN PACKETS OUT OF TIME");
		while (send_idc_ip_2_dbserver() == -1);
	}
	if (idc_test_area_mmap.mmap_head->ready_num != 0) {
		//等待收包的过程超时;
		ERROR_LOG("IDC_AREA:WAIT DB RETURN PACKETS OUT OF TIME");
		while (send_idc_area_2_dbserver() == -1);
	}
	if (ret == ETIMEDOUT)
		goto re_wait;

	idc_test_ip_mmap.ready_num = idc_test_area_mmap.ready_num = 0;
}


static int send_idc_ip_2_dbserver()
{
	idc_test_ip_t *idc_ip;
	uint32_t i;
	uint32_t send_count = 0;
	int len;
	uint8_t	buf[BUF_NET_SEND_LEN];
	idc_ip = (idc_test_ip_t*)idc_test_ip_mmap.mmap_p;
	int now = (int)time((time_t*)NULL);

	proto_link_test_t	*tmp_link;
	proto_idc_t *tmp_idc;
	proto_all_idc_t *tmp_all_idc;
	proto_idc_test_ip_t *tmp_idc_ip;

	//very important bug fix:every packet has a tag to identify it's correspondent mmap.
	uint32_t packet_tag = (idc_ip_packet_tag << SHIFT_BITS) + 0;

	for (i = 0; i < idc_test_ip_mmap.records_num; i++) {

		//start-added by singku for remove ip which is out of date 2011-08-04
		if ((idc_ip + i)->count == 0 && (idc_ip + i)->ip != 0
				&& (now - (int)((idc_ip + i)->test_time)) > 86400)	{
			//i如果此处有ip记录，说明还没有到空白记录处,则判断该ip是否过期 一天都无数据 则删除.
			//先从hash表中删除当前ip.
			//DEBUG_LOG("DELETE now:%d,test_time:%d",now,(idc_ip+i)->test_time);
			g_hash_table_remove(idc_hash_ip, &((idc_ip + i)->ip));
			//tmp指向最后一个结果，将最后一个结果搬移过来，覆盖此处的数据.
			idc_test_ip_t *tmp = (idc_test_ip_t *)idc_test_ip_mmap.mmap_p;
			tmp += (idc_test_ip_mmap.records_num -1);

			if (idc_ip + i != tmp) {
				memcpy((idc_ip+i), tmp, sizeof(idc_test_ip_t));
				(idc_ip + i)->offset = i;//重置offset
				//重新插入hash表 replace会替代老的key,value,所以不用担心没有remove掉原来的末尾ip.
				g_hash_table_replace(idc_hash_ip, &((idc_ip + i)->ip), &((idc_ip + i)->offset));
			}
			if (idc_test_ip_mmap.records_num > 0) {
				idc_test_ip_mmap.records_num --;
				idc_test_ip_mmap.mmap_head->records_num --;
			}
		}
		//end-added by singku for remove ip which is out of date 2011-08-04

		if ((idc_ip + i)->count == 0)
			continue;

		//由于link 的收发结果不是由flag的0,1状态来标识的，所以重启后，可能有数据，且flag不为0 需要重置flag
		//ext的flag和非link ip的flag是由0,1状态标识，重启后，不需要关心flag的数值
		(idc_ip + i)->flag = 0;
		if ((idc_ip + i)->count_link != 0 && (idc_ip + i)->idc_master != IDC_MASTER_INVALID) {
			//分别给每个链路发送一次
			uint32_t j;
			uint32_t send_link = 0;
			for (j = 0; j < MAX_LINK; j++) {
				//link,idc,all_idc 三种
				//=====================================================================link
				if ((idc_ip + i)->link_result[j].count == 0)
					continue;

				DEBUG_LOG("SEND LINK IP PACKET__Time:%u,Delay:%u,Loss:%u,Hop:%u,Count:%u,LinkNo:%u",
							(idc_ip+i)->link_result[j].test_time,
							(idc_ip+i)->link_result[j].delay,
							(idc_ip+i)->link_result[j].loss,
							(idc_ip+i)->link_result[j].hop,
							(idc_ip+i)->link_result[j].count,
							(idc_ip+i)->link_result[j].link_no);
				int sum = (idc_ip+i)->link_result[j].count;
				if ((idc_ip+i)->link_result[j].delay/sum>1000
					||(idc_ip+i)->link_result[j].loss/sum>100
					||(idc_ip+i)->link_result[j].hop/sum>255) {
					ERROR_LOG("SEND LINK IP PACKET__Time:%u,IP:%u,Delay:%u,Loss:%u,Hop:%u,Count:%u,LinkNo:%u",
								(idc_ip+i)->link_result[j].test_time,
								(idc_ip+i)->ip,
								(idc_ip+i)->link_result[j].delay,
								(idc_ip+i)->link_result[j].loss,
								(idc_ip+i)->link_result[j].hop,
								(idc_ip+i)->link_result[j].count,
								(idc_ip+i)->link_result[j].link_no);
				}

				//初始化协议头
				len = sizeof(protocol_t) + sizeof(proto_link_test_t);
				init_proto_head(buf, len, proto_as_net_stat_report_link, packet_tag, i, 0);
				//填写body
				tmp_link = (proto_link_test_t*)(buf + sizeof(protocol_t));
				tmp_link->count = (idc_ip+i)->link_result[j].count;
				tmp_link->number = (idc_ip+i)->link_result[j].link_no;
				tmp_link->test_time = (idc_ip+i)->link_result[j].test_time;
				tmp_link->delay_v = (idc_ip+i)->link_result[j].delay;
				tmp_link->loss_v = (idc_ip+i)->link_result[j].loss;
				tmp_link->hop_v = (idc_ip+i)->link_result[j].hop;
				if (send_data(buf, len, proto_as_net_stat_report_link) == -1)
					return -1;
				send_link ++;
				if (send_link == (idc_ip + i)->count_link)
					break;
			}
			//暂时不发idc之间的以及all idc的数据
			(idc_ip+i)->ext_flag.idc_flag = 1;
			(idc_ip+i)->ext_flag.all_idc_flag = 1;

			//==========================================================================idc
			//初始化协议头
			len = sizeof(protocol_t) + sizeof(proto_idc_t);
			init_proto_head(buf, len, proto_as_net_stat_report_idc, packet_tag, i, 0);
			//填写body
			tmp_idc = (proto_idc_t*)(buf + sizeof(protocol_t));
			tmp_idc->count = (idc_ip + i)->count;
			tmp_idc->number = (idc_ip + i)->idc_master;
			tmp_idc->test_time = (idc_ip + i)->test_time;
			tmp_idc->delay_v = (idc_ip + i)->delay;
			tmp_idc->loss_v = (idc_ip + i)->loss;
			tmp_idc->hop_v = (idc_ip + i)->hop;

			if (send_data(buf, len, proto_as_net_stat_report_idc) == -1)
				return -1;

			//====================================================================all_idc
			//初始化协议头
			len = sizeof(protocol_t) + sizeof(proto_all_idc_t);
			init_proto_head(buf, len, proto_as_net_stat_report_all_idc, packet_tag, i, 0);
			//填写body
			tmp_all_idc = (proto_all_idc_t*)(buf +sizeof(protocol_t));
			tmp_all_idc->count = (idc_ip + i)->count;
			tmp_all_idc->test_time = (idc_ip + i)->test_time;
			tmp_all_idc->delay_v = (idc_ip + i)->delay;
			tmp_all_idc->loss_v = (idc_ip + i)->loss;
			tmp_all_idc->hop_v = (idc_ip + i)->hop;

			if (send_data(buf, len, proto_as_net_stat_report_all_idc) == -1)
				return -1;
		}
		else if ((idc_ip + i)->idc_master == IDC_MASTER_INVALID) {
			//非链路数据 直接发送
			DEBUG_LOG("SEND IDC IP PACKET__Time:%u,idc:%u,IP:%u,Delay:%u,Loss:%u,Hop:%u,Count:%u,Offset:%u",
						(idc_ip+i)->test_time,
						(idc_ip+i)->idcid,
						(idc_ip+i)->ip,
						(idc_ip+i)->delay,
						(idc_ip+i)->loss,
						(idc_ip+i)->hop,
						(idc_ip+i)->count,
						(idc_ip+i)->offset);
			int sum = (idc_ip+i)->count;
			if ((idc_ip+i)->delay/sum>1000 ||(idc_ip+i)->loss/sum>100||(idc_ip+i)->hop/sum>255) {
				ERROR_LOG("SEND IDC IP PACKET__Time:%u,IP:%u,Delay:%u,Loss:%u,Hop:%u,Count:%u",
							(idc_ip+i)->test_time,
							(idc_ip+i)->ip,
							(idc_ip+i)->delay,
							(idc_ip+i)->loss,
							(idc_ip+i)->hop,
							(idc_ip+i)->count);
			}

			//初始化协议头
			len = sizeof(protocol_t) + sizeof(proto_idc_test_ip_t);
			init_proto_head(buf, len, proto_as_net_stat_report_ip, packet_tag, i, 0);
			//填写body
			tmp_idc_ip = (proto_idc_test_ip_t*)(buf + sizeof(protocol_t));
			tmp_idc_ip->idcid = (idc_ip + i)->idcid;
			tmp_idc_ip->count = (idc_ip + i)->count;
			tmp_idc_ip->ip = (idc_ip + i)->ip;
			tmp_idc_ip->test_time = (idc_ip + i)->test_time;
			tmp_idc_ip->delay_v = (idc_ip + i)->delay;
			tmp_idc_ip->loss_v = (idc_ip + i)->loss;
			tmp_idc_ip->hop_v = (idc_ip + i)->hop;

			if (send_data(buf, len, proto_as_net_stat_report_ip) == -1)
				return -1;
		}
		send_count ++;
		if (send_count == idc_test_ip_mmap.ready_num)
			break;
	}

	if (send_count == 0) {
		ERROR_LOG("IDC IP SEND COUNT == 0");
		idc_test_ip_mmap.mmap_head->ready_num = 0;
	}
	return 0;
}

static int send_idc_area_2_dbserver()
{
	idc_test_area_t *idc_area;
	uint32_t i;
	uint32_t send_count = 0;
	int len;
	uint8_t	buf[BUF_NET_SEND_LEN];
	proto_idc_test_area_t	*tmp_idc_area;

	uint32_t packet_tag = (idc_area_packet_tag << SHIFT_BITS) + 0;

	idc_area = (idc_test_area_t*)idc_test_area_mmap.mmap_p;
	for (i = 0; i < idc_test_area_mmap.records_num; i++) {
		if ((idc_area + i)->count == 0)
			continue;

		DEBUG_LOG("SEND IDC AREA PACKET__Time:%u,idc:%u,Delay:%u,Loss:%u,Hop:%u,Count:%u,City:%u,ISP:%s",
					(idc_area+i)->test_time,
					(idc_area+i)->idcid,
					(idc_area+i)->delay,
					(idc_area+i)->loss,
					(idc_area+i)->hop,
					(idc_area+i)->count,
					g_ipDict.m_ipmap[(idc_area+i)->isp_key].city_code,
					g_ipDict.m_ipmap[(idc_area+i)->isp_key].isp);

		int sum = (idc_area+i)->count;
		if ((idc_area+i)->delay/sum>1000 ||(idc_area+i)->loss/sum>100||(idc_area+i)->hop/sum>255) {
			ERROR_LOG("SEND IDC AREA PACKET__Time:%u,Delay:%u,Loss:%u,Hop:%u,Count:%u,City:%u,ISP:%s",
						(idc_area+i)->test_time,
						(idc_area+i)->delay,
						(idc_area+i)->loss,
						(idc_area+i)->hop,
						(idc_area+i)->count,
						g_ipDict.m_ipmap[(idc_area+i)->isp_key].city_code,
						g_ipDict.m_ipmap[(idc_area+i)->isp_key].isp);
		}

		(idc_area + i)->flag = 0;

		//初始化协议头
		len = sizeof(protocol_t) + sizeof(proto_idc_test_area_t);
		init_proto_head(buf, len, proto_as_net_stat_report_cityisp, packet_tag, i, 0);
		//填写body
		tmp_idc_area = (proto_idc_test_area_t*)(buf + sizeof(protocol_t));

		tmp_idc_area->idcid = (idc_area + i)->idcid;
		tmp_idc_area->city_code = g_ipDict.m_ipmap[(idc_area + i)->isp_key].city_code;
		tmp_idc_area->province_code = g_ipDict.m_ipmap[(idc_area + i)->isp_key].province_code;
		memcpy(tmp_idc_area->isp, g_ipDict.m_ipmap[(idc_area + i)->isp_key].isp, ISP_LEN);
		tmp_idc_area->count = (idc_area + i)->count;
		tmp_idc_area->test_time = (idc_area + i)->test_time;
		tmp_idc_area->delay_v = (idc_area + i)->delay;
		tmp_idc_area->loss_v = (idc_area + i)->loss;
		tmp_idc_area->hop_v = (idc_area + i)->hop;

		if (send_data(buf, len, proto_as_net_stat_report_cityisp) == -1)
			return -1;

		send_count ++;
		if (send_count == idc_test_area_mmap.ready_num)
			break;
	}
	if (send_count == 0) {
		ERROR_LOG("IDC AREA SEND COUNT == 0");
		idc_test_area_mmap.mmap_head->ready_num = 0;
	}
	return 0;
}

static int process_record(store_result_t *result, int *status)
{
	if (result == NULL)
		return 0;
	//查找hash_ip
	uint32_t *offset;
	idc_test_ip_t *tmp_ip;
	idc_test_area_t *tmp_area;
	link_ip_t	*link_addr;
	if (result->avg_latency > delay_upper_bound
	    ||result->loss_percentage > 100
	    ||result->hop > 255) {
		ERROR_LOG("BAD RECORD:%u,%u,%u,%u,%u",result->probed_ip,result->probe_time,
					result->avg_latency,result->loss_percentage,result->hop);
	    *status = -1;
	}
	else
	    *status = 0;

	code_t code;
	uint32_t isp_key;
	if (g_ipDict.find(result->probed_ip, code, &isp_key) == false)
	    return 0;
	offset = (uint32_t*)g_hash_table_lookup(idc_hash_ip, &result->probed_ip);
	if (offset == NULL) {
		//DEBUG_LOG("ADD NEW IP:%u(ALL:%u)",result->probed_ip,idc_test_ip_mmap.records_num);
		tmp_ip = (idc_test_ip_t*)idc_test_ip_mmap.mmap_p;
		tmp_ip += (idc_test_ip_mmap.records_num);
		tmp_ip->count_link = 0;
		tmp_ip->offset = (idc_test_ip_mmap.records_num);
		tmp_ip->isp_key = isp_key;
		tmp_ip->test_time = result->probe_time;
		tmp_ip->ip = result->probed_ip;
		tmp_ip->idcid = result->machine_number;
		tmp_ip->delay = result->avg_latency;
		tmp_ip->loss = result->loss_percentage;
		tmp_ip->hop = result->hop;
		tmp_ip->idc_master = IDC_MASTER_INVALID;
		char hashkey[21];
		snprintf(hashkey, sizeof(hashkey), "%d%d", result->machine_number, result->probed_ip);
		link_addr = (link_ip_t *)g_hash_table_lookup(link_hash_ip, hashkey);
		if (link_addr != NULL) {
			tmp_ip->idc_master = link_addr->idc_master;
			memset(&tmp_ip->ext_flag, 0x0, sizeof(tmp_ip->ext_flag));
			memset(tmp_ip->link_result, 0x0, sizeof(tmp_ip->link_result));
			int pos = (link_addr->link_no -1) % MAX_LINK;
			tmp_ip->link_result[pos].link_no= link_addr->link_no;
			tmp_ip->link_result[pos].test_time = result->probe_time;
			tmp_ip->link_result[pos].delay = result->avg_latency;
			tmp_ip->link_result[pos].loss = result->loss_percentage;
			tmp_ip->link_result[pos].hop = result->hop;
			tmp_ip->link_result[pos].count = 1;
			tmp_ip->count_link = 1;
	    }
		tmp_ip->count = 1;
		idc_test_ip_mmap.records_num++;
		idc_test_ip_mmap.mmap_head->records_num++;
		idc_test_ip_mmap.ready_num++;
		idc_test_ip_mmap.mmap_head->ready_num++;
		g_hash_table_insert(idc_hash_ip, &tmp_ip->ip, &tmp_ip->offset);
	}
	else {
		tmp_ip = (idc_test_ip_t*)idc_test_ip_mmap.mmap_p + (*offset);
		if (tmp_ip->count == 0) {
			idc_test_ip_mmap.ready_num++;
			idc_test_ip_mmap.mmap_head->ready_num++;
			tmp_ip->flag = 0;
			memset(&tmp_ip->ext_flag, 0x0, sizeof(tmp_ip->ext_flag));
			memset(tmp_ip->link_result, 0x0, sizeof(tmp_ip->link_result));
			tmp_ip->delay = tmp_ip->hop = tmp_ip->loss = 0;
			tmp_ip->count_link = 0;
			tmp_ip->idcid = result->machine_number;
		}
		tmp_ip->delay += result->avg_latency;
		tmp_ip->loss += result->loss_percentage;
		tmp_ip->hop += result->hop;
		tmp_ip->test_time = result->probe_time;

		char hashkey[21];
		snprintf(hashkey, sizeof(hashkey), "%d%d", result->machine_number, result->probed_ip);
		link_addr = (link_ip_t *)g_hash_table_lookup(link_hash_ip, hashkey);
		if (link_addr != NULL) {
			tmp_ip->idc_master = link_addr->idc_master;
			int pos = (link_addr->link_no -1) % MAX_LINK;
			if (tmp_ip->link_result[pos].count == 0) {
				tmp_ip->count_link++;
				memset((char*)&(tmp_ip->link_result[pos]), 0x0, sizeof(tmp_ip->link_result[pos]));
				tmp_ip->link_result[pos].link_no = link_addr->link_no;
			}
			tmp_ip->link_result[pos].test_time = result->probe_time;
			tmp_ip->link_result[pos].delay += result->avg_latency;
			tmp_ip->link_result[pos].loss += result->loss_percentage;
			tmp_ip->link_result[pos].hop += result->hop;
			tmp_ip->link_result[pos].count ++;
		}
		tmp_ip->count ++;

		if (tmp_ip->delay/tmp_ip->count > delay_upper_bound
				|| tmp_ip->loss/tmp_ip->count > 100
				|| tmp_ip->hop/tmp_ip->count > 255)
				ERROR_LOG("BAD MMAP ip:%u,%u,%u,%u,%u,%u",tmp_ip->ip,tmp_ip->test_time,
								tmp_ip->delay,tmp_ip->loss,tmp_ip->hop,tmp_ip->count);
	}
	//查找hash cityisp 因为hash citysip是初始化时刻建立的，所以hash表中一定有相应的记录
	//否则是非法ip 直接返回。

	offset = (uint32_t*)g_hash_table_lookup(hash_cityisp, code.key);
	if (offset == NULL)
	    return 0;
	//the same city isp may have more than one ip record, set to the last value
	tmp_area = (idc_test_area_t*)idc_test_area_mmap.mmap_p + (*offset);
	if (tmp_area->count == 0) {
		idc_test_area_mmap.ready_num++;
		idc_test_area_mmap.mmap_head->ready_num++;
		tmp_area->delay = 0;
		tmp_area->hop = 0;
		tmp_area->loss = 0;
		tmp_area->idcid = result->machine_number;
	}
	tmp_area->delay += result->avg_latency;
	tmp_area->hop += result->hop;
	tmp_area->loss += result->loss_percentage;
	tmp_area->test_time = result->probe_time;
	tmp_area->flag = 0;
	tmp_area->count ++;

	//tmp_ip->myarea = tmp_area;

	if (tmp_area->delay/tmp_area->count > delay_upper_bound
	    || tmp_area->loss/tmp_area->count > 100
	    || tmp_area->hop/tmp_area->count > 255)
	    ERROR_LOG("BAD MMAP:city:%u,isp:%s,%u,%u,%u,%u,%u",
	    		g_ipDict.m_ipmap[tmp_area->isp_key].city_code,g_ipDict.m_ipmap[tmp_area->isp_key].isp,
	    		tmp_area->test_time,tmp_area->delay,tmp_area->loss,tmp_area->hop,tmp_area->count);
	return 0;
}

static int process_file(const char *filename)
{
	FILE *fp;
	idc_file_head_t f_head;
	store_result_t	result;
	fp = fopen(filename,"rb");
	if (fp == NULL)
		ERROR_RETURN(("idc:can not open file %s!\t",filename), 0);

	//读取文件头，判定是否非法文件
	int read_len = fread(&f_head, sizeof(idc_file_head_t), 1, fp);
	if (read_len < 1
		|| memcmp(f_head.symbol, FILE_HEAD_SYMBOL, FILE_HEAD_LEN) != 0
		|| f_head.create_time > f_head.last_modify
		|| f_head.total_record < f_head.processed_record) {
		fclose(fp);
		remove((const char *)filename);
		ERROR_RETURN(("idc: bad file %s!\t",filename), 0);
	}
	//读取每条记录，更新hash表和mmap.
	int status = 0;
	while (!feof(fp)) {
		read_len=fread(&result, sizeof(store_result_t), 1, fp);
		if (read_len != 1)
			break;
		if (result.machine_number == 0) {
			fclose(fp);
			remove((const char *)filename);
			ERROR_RETURN(("idc file:idc_number == 0 in file %s", filename), 0);
		}
		process_record(&result, &status);
		if (status == -1)
			ERROR_LOG("\t in file %s",filename);
	}//while
	fclose(fp);
	//when read file to mmap, you could delete this file
	char new_name[DIR_PATH_LEN];
	if (status == -1) {
		snprintf(new_name, strlen(filename) - strlen(strrchr(filename, (int)'/')) + 1, "%s", filename);
		snprintf(new_name + strlen(new_name), sizeof(new_name) - strlen(new_name),
					"/Have_Bad_Records_%s", strrchr(filename, (int)'/') + 1);
		rename(filename, new_name);
	}

	switch (remove_or_rename) {
	case REMOVE_IDC_FILE :
		remove((const char *)filename);
		break;
	case RENAME_IDC_FILE :
		snprintf(new_name, sizeof(new_name), "%s/%s",
				idc_rename_dirpath, strrchr(filename, (int)'/') + 1);
		rename(filename, new_name);
		break;
	default:
		break;
	}

	DEBUG_LOG("\t Now_Process_File:%s\t",filename);
	DEBUG_LOG("NEW:IDC IP READY\t:[%u] TAG:[%u]", idc_test_ip_mmap.ready_num, idc_ip_packet_tag);
	DEBUG_LOG("NEW:IDC AREA READY\t:[%u] TAG:[%u]",idc_test_area_mmap.ready_num, idc_area_packet_tag);

	//然后发送给dbserver
	if (send_idc_ip_2_dbserver() == -1)
		return -1;
	if (send_idc_area_2_dbserver() == -1)
		return -1;
	return 0;
}

static int processdir(DIR *dir)
{
	struct dirent *ptr;
	char full_path[DIR_PATH_LEN];

	while ((ptr = readdir(dir)) != NULL) {
	if (ptr->d_name[0]=='N') {
		snprintf(full_path, sizeof(full_path), "%s/%s", idc_file_dirpath, ptr->d_name);
		idc_test_ip_mmap.ready_num = idc_test_ip_mmap.mmap_head->ready_num = 0;
		idc_test_area_mmap.ready_num = idc_test_ip_mmap.mmap_head->ready_num = 0;

		if (0 != process_file(full_path))
			ERROR_RETURN(("idc:send record in mmap file, not completed!\t"), -1);
			//发完了需要等到所有记录的回执都收到才发下一次
			wait_all_data_ok();
		}
	}
	return 0;
}

void* polling_files(void*)
{
	DIR *dir;

	while (1) {
		//对于IDC探测.如果 mmap中有记录，先把已有记录发出去，再处理下一个文件
		//而CDN测速，数据是实时的,可以直接更新记录
		if (idc_test_ip_mmap.ready_num != 0) {
			//先校验未完成文件，因为未完成文件可能因为程序的意外终止导致出现脏数据。
			uint32_t i;
			idc_test_ip_t *p = (idc_test_ip_t*)idc_test_ip_mmap.mmap_p;
			idc_test_ip_mmap.mmap_head->ready_num = idc_test_ip_mmap.ready_num = 0;
			for (i = 0; i < idc_test_ip_mmap.records_num; i++) {
				//count不为0但count link为0 同时又是一个链路IP 这是矛盾的，可能的原因是读入一个数据
				//时，刚置位了count,但是count link还没有置位。
				if ((p + i)->count != 0) {
					if ((p + i)->idc_master != IDC_MASTER_INVALID &&(p+i)->count_link == 0) {
						ERROR_LOG("OLD MMAP_DEFECT DATA: idc_ip_mmap_offset\
										:%u,count:%u,count_link:%u,master:%u",
										(p+i)->offset,(p+i)->count,(p+i)->count_link,(p+i)->idc_master);
										(p + i)->count = 0;
					}
					else
						idc_test_ip_mmap.mmap_head->ready_num ++;
				}
			}
			idc_test_ip_mmap.ready_num = idc_test_ip_mmap.mmap_head->ready_num;
			if (send_idc_ip_2_dbserver() == -1)	//表示没有发完
				continue;
		}
		if (idc_test_area_mmap.ready_num != 0)
			if (send_idc_area_2_dbserver() == -1)
				continue;
		//发完了需要等到所有记录的回执都收到才发下一次
		wait_all_data_ok();

		idc_start = 1; //最初始进入while循环的时候,start = 0;那么发送旧的mmap数据就不用更新缓存
		dir = opendir((const char *)idc_file_dirpath);
		if (dir == NULL) {
			ERROR_LOG("cant open idc_file_dirpath,%s\t", idc_file_dirpath);
			sleep(60);
	    	continue;
	    	//pthread_exit(0);
		}

		if (processdir(dir) == -1) {
			//closedir(dir);
			//pthread_exit(0);
	    }
		closedir(dir);

		sleep(60);
	}
}
