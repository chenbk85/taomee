#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

extern "C"{
#include <libtaomee/timer.h>
#include <libtaomee/log.h>
}
#include "proto.h"
#include "common.h"
#include "ip_dict.hpp"
#include "load_conf_ip_hash_mmap.hpp"
#include "send_data.hpp"
#include "polling_cdn_url_files.hpp"

uint32_t cdn_url_packet_tag = 1;//用以在协议头标记发出的数据包，收到回应包时判定此字段是否属于当前正在处理的mmap

pthread_spinlock_t lock_cdn_ip;

static inline int wait_cdn_url_data_ok(char *pmap)
{
	//发完了需要等到所有记录的回执都收到才发下一次
	timespec timeout;
	int ret;

	pthread_mutex_lock(&cdn_and_url_mutex);
	if (((mmap_file_head_t*)pmap)->ready_num != 0) {
		//由于mmap是每分钟60秒备份一个，故折中取30秒为本次发送文件的等待超时时间
		timeout.tv_sec = time(0) + 30;
		timeout.tv_nsec = 0;
		ret = pthread_cond_timedwait(&cdn_and_url_cond, &cdn_and_url_mutex, &timeout);
	}
	pthread_mutex_unlock(&cdn_and_url_mutex);
	if (ret == ETIMEDOUT) {
		return -1;
	}
	return 0;
	//===================
}

proto_cdn_speed_ip_isp_t cdn_speed_ip_isp;
static int send_cdn_ip_2_dbserver(char *mmap_p)
{
	mmap_file_head_t *file_head_p = (mmap_file_head_t*)mmap_p;
	cdn_speed_ip_t *cdn_ip_p = (cdn_speed_ip_t*)(mmap_p + sizeof(mmap_file_head_t));
	uint8_t buf[BUF_NET_SEND_LEN];
	int len;
	uint32_t i;
	uint32_t send_count;
	uint32_t ready = file_head_p->ready_num;
	int number;// = cdn_ip_p->project_number;
	uint32_t packet_tag = cdn_url_packet_tag << SHIFT_BITS;
	int now;

	DEBUG_LOG("CDN IP READY:%u",ready);
resend_cdnip:
	send_count = 0;
	now = (int)time(0);
	for (i = 0; i < file_head_p->records_num; i++) {

		(cdn_ip_p + i)->t_time = (cdn_ip_p + i)->time[(cdn_ip_p + i)->step];
		(cdn_ip_p + i)->t_speed = (cdn_ip_p + i)->speed[(cdn_ip_p + i)->step];
		(cdn_ip_p + i)->t_count = (cdn_ip_p + i)->count[(cdn_ip_p + i)->step];

		//start-added by singku for remove ip which is out of date 2011-08-04
		if ((cdn_ip_p + i)->t_count == 0 && (cdn_ip_p + i)->ip != 0
				&& (now - (int)((cdn_ip_p + i)->t_time)) > 86400)	{
			//i如果此处有ip记录ip不为0，说明还没有到空白记录处,则判断该ip是否过期 ，一天都无数据 则删除.
			//先从hash表中删除当前ip.
			int proj = ((cdn_ip_p + i)->project_number - 1) % max_project_number;
			//tmp指向最后一个结果，将最后一个结果搬移过来，覆盖此处的数据.
			cdn_speed_ip_t *tmp = (cdn_speed_ip_t *)(cdn_speed_ip_mmap[proj].mmap_p);
			cdn_speed_ip_t *cur = tmp + i;
			if (cur->t_count == 0) {//如果对应的当前mmap中该cdn ip也无数据
				tmp += (cdn_speed_ip_mmap[proj].records_num -1);
				g_hash_table_remove(cdn_hash_ip[proj], &(cur->ip));
				if (cur != tmp) {
					memcpy(cur, tmp, sizeof(cdn_speed_ip_t));
					cur->offset = i;//重置offset
					//重新插入hash表 replace会替代老的key,value,所以不用担心没有remove掉原来的末尾ip.
					g_hash_table_replace(cdn_hash_ip[proj], &(cur->ip), &(cur->offset));
				}
				if (cdn_speed_ip_mmap[proj].records_num > 0) {
					cdn_speed_ip_mmap[proj].records_num --;
					cdn_speed_ip_mmap[proj].mmap_head->records_num --;
				}
				(cdn_ip_p + i)->t_time = (cdn_ip_p + i)->time[(cdn_ip_p + i)->step];
				(cdn_ip_p + i)->t_speed = (cdn_ip_p + i)->speed[(cdn_ip_p + i)->step];
				(cdn_ip_p + i)->t_count = (cdn_ip_p + i)->count[(cdn_ip_p + i)->step];
			}
		}
		//end-added by singku for remove ip which is out of date 2011-08-04

		if ((cdn_ip_p + i)->t_count == 0)
			continue;

		//init protocol_head;
		number = ((cdn_ip_p + i)->project_number-1) % max_project_number + 1;
		len = sizeof(protocol_t) + sizeof(proto_cdn_speed_ip_t);
		init_proto_head(buf, len, proto_as_cdn_report_ip, (packet_tag + number), i, 0);

		DEBUG_LOG("SEND CDN IP PACKET___PROTO:%x,ID:%u,Number:%u,Time:%u,Speed:%u,Count:%u",
				proto_as_cdn_report_ip,i,number,(cdn_ip_p+i)->t_time,
				(cdn_ip_p+i)->t_speed/(cdn_ip_p+i)->t_count, (cdn_ip_p+i)->t_count);
		memcpy(buf + sizeof(protocol_t), &((cdn_ip_p + i)->project_number), sizeof(proto_cdn_speed_ip_t));
		if (send_data(buf, len, proto_as_cdn_report_ip) == -1)
			return -1;

		send_count ++;
		if (send_count == ready)
			break;
	}

	if (send_count == 0)
		ERROR_RETURN(("SEND COUNT == 0:map_tag:%u,type:CDN IP", cdn_url_packet_tag), 0);
	if (wait_cdn_url_data_ok(mmap_p) == -1) {
		ERROR_LOG("CDN IP:WAIT RET PACKETS TIME OUT");
		goto resend_cdnip;
	}

	return 0;
}
static int send_cdn_area_2_dbserver(char *mmap_p)
{
	mmap_file_head_t *file_head_p = (mmap_file_head_t*)mmap_p;
	cdn_speed_area_t *cdn_area_p = (cdn_speed_area_t*)(mmap_p + sizeof(mmap_file_head_t));
	uint8_t buf[BUF_NET_SEND_LEN];
	proto_cdn_speed_area_t *buf_ptr;
	int len;
	uint32_t i;
	uint32_t send_count;
	uint32_t ready = file_head_p->ready_num;
	int number;// = cdn_area_p->project_number;
	uint32_t packet_tag = cdn_url_packet_tag << SHIFT_BITS;

	DEBUG_LOG("CDN AREA READY:%u",ready);
resend_cdnarea:
	send_count = 0;
	for (i = 0; i < file_head_p->records_num; i++) {

		(cdn_area_p + i)->t_time = (cdn_area_p + i)->time[(cdn_area_p + i)->step];
		(cdn_area_p + i)->t_speed = (cdn_area_p + i)->speed[(cdn_area_p + i)->step];
		(cdn_area_p + i)->t_count = (cdn_area_p + i)->count[(cdn_area_p + i)->step];


		if ((cdn_area_p + i)->t_count == 0)
			continue;


		//init protocol_head;
		number = ((cdn_area_p + i)->project_number -1) % max_project_number + 1;
		len = sizeof(protocol_t) + sizeof(proto_cdn_speed_area_t);

		init_proto_head(buf, len, proto_as_cdn_report_cityisp, (packet_tag + number), i, 0);
		DEBUG_LOG("SEND CDN AREA PACKET___PROTO:%x,ID:%u,Number:%u,%s,Time:%u,Speed:%u,Count:%u",
				proto_as_cdn_report_cityisp,i,(cdn_area_p+i)->project_number,
				g_ipDict.m_ipmap[(cdn_area_p+i)->isp_key].isp, (cdn_area_p+i)->t_time,
				(cdn_area_p+i)->t_speed/(cdn_area_p+i)->t_count,
				(cdn_area_p+i)->t_count);
		buf_ptr = (proto_cdn_speed_area_t*)(buf + sizeof(protocol_t));
		buf_ptr->project_number = (cdn_area_p + i)->project_number;
		buf_ptr->province_code = g_ipDict.m_ipmap[(cdn_area_p+i)->isp_key].province_code;
		buf_ptr->city_code = g_ipDict.m_ipmap[(cdn_area_p+i)->isp_key].city_code;
		memcpy(buf_ptr->isp, g_ipDict.m_ipmap[(cdn_area_p+i)->isp_key].isp, ISP_LEN);
		buf_ptr->test_time = (cdn_area_p + i )->t_time;
		buf_ptr->speed = (cdn_area_p + i)->t_speed;
		buf_ptr->count = (cdn_area_p + i)->t_count;

		if (send_data(buf, len, proto_as_cdn_report_cityisp) == -1)
			return -1;

		send_count ++;
		if (send_count == ready)
			break;
	}

	if (send_count == 0)
		ERROR_RETURN(("SEND COUNT == 0:map_tag:%u,type:CDN AREA", cdn_url_packet_tag), 0);
	if (wait_cdn_url_data_ok(mmap_p) == -1) {
		ERROR_LOG("CDN AREA:WAIT RET PACKETS TIME OUT");
		goto resend_cdnarea;
	}

	return 0;
}


static int send_url_page_2_dbserver(char *mmap_p)
{
	mmap_file_head_t *file_head_p = (mmap_file_head_t*)mmap_p;
	url_test_t *url_page_p = (url_test_t*)(mmap_p + sizeof(mmap_file_head_t));
	uint8_t buf[BUF_NET_SEND_LEN];
	proto_url_test_fixed_t *buf_ptr;
	int len;
	uint32_t i;
	uint32_t send_count;
	uint32_t ready = file_head_p->ready_num;
	int number;// = url_page_p->project_number;
	uint32_t packet_tag = cdn_url_packet_tag << SHIFT_BITS;

	DEBUG_LOG("URL PAGE READY:%u",ready);
resend_url:
	send_count = 0;
	for (i = 0; i < file_head_p->records_num; i++) {
		if ((url_page_p + i)->count == 0)
			continue;

		//init protocol_head;
		number = ((url_page_p + i)->project_number -1) % max_project_number + 1;
		int vlen = (url_page_p + i)->v_count * sizeof((url_page_p + i)->vlist[0]);
		len = sizeof(protocol_t) + sizeof(proto_url_test_fixed_t) + vlen;
		init_proto_head(buf, len, proto_as_url_stat_page_time,
				(packet_tag + number), (file_head_p->value << SHIFT_BITS) + i, 0);
		DEBUG_LOG("SEND URL AREA PACKET___PROTO:%x,ID:%u,Number:%u,%s,Page:%u,v_count:%u,"
				"list:%u,%u,%u,%u,%u,%u,%u,%u,%u,%u",
				proto_as_url_stat_page_time,i,number, g_ipDict.m_ipmap[(url_page_p+i)->isp_key].isp,
				(url_page_p+i)->page_id,(url_page_p+i)->v_count,
				(url_page_p+i)->vlist[0],
				(url_page_p+i)->vlist[1],
				(url_page_p+i)->vlist[2],
				(url_page_p+i)->vlist[3],
				(url_page_p+i)->vlist[4],
				(url_page_p+i)->vlist[5],
				(url_page_p+i)->vlist[6],
				(url_page_p+i)->vlist[7],
				(url_page_p+i)->vlist[8],
				(url_page_p+i)->vlist[9]);

		buf_ptr = (proto_url_test_fixed_t*)(buf + sizeof(protocol_t));
		buf_ptr->project_number = (url_page_p + i)->project_number;
		buf_ptr->page_id = (url_page_p + i)->page_id;
		buf_ptr->provice_code = g_ipDict.m_ipmap[(url_page_p + i)->isp_key].province_code;
		buf_ptr->city_code = g_ipDict.m_ipmap[(url_page_p + i)->isp_key].city_code;
		buf_ptr->isp_id = g_ipDict.m_ipmap[(url_page_p + i)->isp_key].isp_id;
		memcpy(buf_ptr->isp, g_ipDict.m_ipmap[(url_page_p + i)->isp_key].isp, ISP_LEN);
		buf_ptr->test_time = (url_page_p + i)->test_time;
		buf_ptr->count = (url_page_p + i)->count;
		buf_ptr->v_count = (url_page_p + i)->v_count;
		memcpy(buf + sizeof(protocol_t) + sizeof(proto_url_test_fixed_t), (url_page_p + i)->vlist, vlen);
		if (send_data(buf, len, proto_as_url_stat_page_time) == -1)
			return -1;

		send_count ++;
		if (send_count == ready)
			break;
	}

	if (send_count == 0)
		ERROR_RETURN(("SEND COUNT == 0:map_tag:%u,type:URL PAGE", cdn_url_packet_tag), 0);
	if (wait_cdn_url_data_ok(mmap_p) == -1) {
		ERROR_LOG("URL PAGE:WAIT RET PACKETS TIME OUT");
		goto resend_url;
	}

	return 0;
}

static int process_file(char *filepath)
{
	fcgi_store_t tmp;
	char zero[1024];
	memset(zero, 0x0, sizeof(zero));
	int fd = open(filepath, O_RDWR);
	if (fd == -1) {
		ERROR_RETURN(("fail to open file %s", filepath), 0);
	}
	int read_len;
	while ((read_len = read(fd, &tmp, sizeof(fcgi_store_t))) >= -1) {
		if (read_len == -1) {
			if (errno == EINTR) {
				continue;
			} else {
				DEBUG_LOG("Error  when read files <%s>: %s", filepath, strerror(errno));
				break;
			}
		} else if (read_len < (int)sizeof(fcgi_store_t)) {
			break;
		} else {
			if (memcmp(&tmp, zero, read_len) == 0) {	//读到分钟分段
				int i, j, pos;
				for (i = 0; i < max_project_number; i++) {
					if (cdn_speed_ip_mmap[i].mmap_head->ready_num != 0) {
						if (send_cdn_ip_2_dbserver((char*)(cdn_speed_ip_mmap[i].mmap_head)) == -1) {
							close(fd);
							ERROR_RETURN(("SEND CDN IP, FILE NOT COMPLETE!"), -1);
						} else {
							cdn_speed_ip_t *ptr = (cdn_speed_ip_t*)cdn_speed_ip_mmap[i].mmap_p;
							int j, pos;
							for (j = 0; j < (int)cdn_speed_ip_mmap[i].mmap_head->records_num; j++) {
								pos = (ptr + j)->step;
								(ptr + j)->speed[pos] = 0;
								(ptr + j)->count[pos] = 0;
								pos = (pos + 1) % CDN_TIME_INTER;
								if ((ptr + j)->count[pos] != 0) {
									(ptr + j)->time[pos] = (ptr + j)->time[(ptr + j)->step] + 60;
									cdn_speed_ip_mmap[i].ready_num ++;
									cdn_speed_ip_mmap[i].mmap_head->ready_num ++;
								}
								(ptr + j)->step = pos;
							}
						}
					}
					if (cdn_speed_area_mmap[i].mmap_head->ready_num != 0) {
						if (send_cdn_area_2_dbserver((char*)(cdn_speed_area_mmap[i].mmap_head)) == -1) {
							close(fd);
							ERROR_RETURN(("SEND CDN AREA, FILE NOT COMPLETE!"), -1);
						} else {
							cdn_speed_area_t *ptr = (cdn_speed_area_t*)cdn_speed_area_mmap[i].mmap_p;
							int j, pos;
							for (j = 0; j < (int)cdn_speed_area_mmap[i].mmap_head->records_num; j++) {
								pos = (ptr + j)->step;
								(ptr + j)->speed[pos] = 0;
								(ptr + j)->count[pos] = 0;
								pos = (pos + 1) % CDN_TIME_INTER;
								if ((ptr + j)->count[pos] != 0) {
									(ptr + j)->time[pos] = (ptr + j)->time[(ptr + j)->step] + 60;
									cdn_speed_area_mmap[i].ready_num ++;
									cdn_speed_area_mmap[i].mmap_head->ready_num ++;
								}
								(ptr + j)->step = pos;
							}
						}
					}
				}
				for (i = 0; i < max_project_number; i++) {
					for (j = 0; j < max_page_per_proj; j++) {
						pos = i * max_page_per_proj + j;
						if (url_page_mmap[pos].mmap_head->ready_num != 0)
							if (send_url_page_2_dbserver((char*)(url_page_mmap[pos].mmap_head)) == -1) {
								close(fd);
								ERROR_RETURN(("SEND URL PAGE, FILE NOT COMPLETE!"), -1);
							}
					}
				}
			} else {
				if (tmp.type == proto_fcgi_cdn_report) {
					set_cdn_mmap(&tmp, (uint32_t)read_len);
				} else if (tmp.type == proto_fcgi_url_report) {
					set_url_mmap(&tmp, (uint32_t)read_len);
				} else {
					ERROR_LOG("Invalid meta file:%s", filepath);
				}
			}//不是分钟分段
		}//读到完整记录
	}

	close(fd);
	remove(filepath);
	return 0;
}

static int process_dir(DIR *dir)
{
	struct dirent 	*ptr;

	file_t *mylink = NULL;
	file_t *tmp = NULL;
	file_t *fnext = NULL;
	file_t *pos = NULL;

	while ((ptr = readdir(dir)) != NULL) {
		if (ptr->d_name[0]=='M') {
			if ((tmp = (file_t*)malloc(sizeof(file_t))) == NULL) {
				ERROR_LOG("Process dir:malloc failed!\n");
				return -1;
			}
			snprintf(tmp->filename, FILE_NAME_LEN, "%s", ptr->d_name);
			snprintf(tmp->fullpath, FULL_PATH_LEN, "%s/%s",
					cdn_url_file_dirpath, ptr->d_name);
			tmp->next = NULL;
			//insert sort;
			if (mylink == NULL)
				mylink = tmp;
			else {
				fnext = mylink;
				while (fnext != NULL) {
					if (strcmp(tmp->filename, fnext->filename) > 0) {
						pos = fnext;//暂存插入点的前一点
						fnext = fnext->next;
					}
					else
						break;
				}
				if (fnext == mylink) {
					tmp->next = mylink;
					mylink = tmp;
				}
				else if (fnext == NULL)
					pos->next = tmp;
				else {
					pos->next = tmp;
					tmp->next = fnext;
				}
			}//insert sort
		}//if
	}//while

	//process file in link;
	tmp = mylink;
	while (tmp != NULL) {
		ERROR_LOG("Process File: %s",tmp->fullpath);
		if (process_file(tmp->fullpath) == -1)
			return -1;
		tmp = tmp->next;
	}

	//free memory
	tmp = mylink;
	while (mylink != NULL) {
		tmp = mylink;
		mylink = mylink->next;
		free(tmp);
	}

	return 0;
}

void* send_file(void*)
{
	DIR *dir;
	while (1) {
		//对于CDN和url的数据，由于直接保存了整个mmap，所以发送的时候可以直接将这个文件重新映
		//到一个临时的mmap，然后按照这个mmap的类型做相应的处理.
		dir = opendir((const char *)cdn_url_file_dirpath);
		if (dir == NULL)
			ERROR_LOG("cant open idc_file_dirpath,%s\t", cdn_url_file_dirpath);

		process_dir(dir);

		closedir(dir);

		sleep(30);
	}
}

int find_available_url_page(int relative_proj_no)
{//相对项目编号
	int i;
	int proj_no = relative_proj_no;//(abs_proj_no - 1) % max_project_number;
	if (url_page_status[proj_no].used_count == max_page_per_proj)
		return -1;
	for (i = 0; i < max_page_per_proj; i++) {
		if (url_page_status[proj_no].status[i] == 0)
			return i;
	}
	return -1;
}

uint8_t cdn_forward_buf[BUF_NET_SEND_LEN];
proto_cdn_speed_ip_isp_t *forward_ip_isp;
proto_cdn_speed_ip_t *forward_ip;
proto_cdn_speed_area_t *forward_area;

int set_cdn_mmap(fcgi_store_t *buf, uint32_t data_len)
{
	cdn_speed_ip_t *cdn_ip_p;
	cdn_speed_area_t *cdn_area_p;
	cdn_speed_area_t *tmp_for_cdn_node;
	static struct timeval now ={0, 0};
	uint32_t	isp_key;
	code_t code_client, code_cdn;
	bool find_client = false;
	fcgi_store_t *packet = buf;

	gettimeofday(&now, NULL);

	if (packet->project_number == 0 || packet->record.m_cdn.speed > max_cdn_speed)
		return 0;

	DEBUG_LOG("GET CDN PACKET:time=%u,proj=%u,client=%u,cdn=%u,speed=%u",packet->test_time,
			packet->project_number,packet->client_ip,packet->record.m_cdn.cdn_ip,packet->record.m_cdn.speed);

	if (g_ipDict.find(packet->client_ip, code_client, &isp_key) == true)
		find_client = true;

	//added by singku 2011-09-01 for cdn details forward ---start
	if (find_client) {
		init_proto_head(cdn_forward_buf, sizeof(protocol_t) + sizeof(proto_cdn_speed_ip_isp_t),
				proto_as_cdn_report_ip_isp, 0, 0, 0);
		forward_ip_isp = (proto_cdn_speed_ip_isp_t*)(cdn_forward_buf + sizeof(protocol_t));
		forward_ip_isp->project_number = packet->project_number;
		forward_ip_isp->test_time = packet->test_time;
		forward_ip_isp->ip = packet->record.m_cdn.cdn_ip;
		forward_ip_isp->province_id = code_client.province_code;
		forward_ip_isp->city_id = code_client.city_code;
		memcpy(forward_ip_isp->isp, code_client.isp, ISP_LEN);
		forward_ip_isp->speed = packet->record.m_cdn.speed;
		forward_ip_isp->count = 1;
		send_data(cdn_forward_buf, sizeof(protocol_t) + sizeof(proto_cdn_speed_ip_isp_t),
				proto_as_cdn_report_ip_isp);
	}
	//added by singku 2011-09-01 for cdn details forward ---end

	int proj = (packet->project_number-1) % max_project_number;
	cdn_ip_p = (cdn_speed_ip_t*)cdn_speed_ip_mmap[ proj ].mmap_p;
	cdn_area_p = (cdn_speed_area_t*)cdn_speed_area_mmap[ proj ].mmap_p;
	tmp_for_cdn_node = (cdn_speed_area_t*)cdn_speed_area_mmap[ proj ].mmap_p;

	//更新ip mmap
	uint32_t time_step;
	uint32_t *offset = (uint32_t*)g_hash_table_lookup(cdn_hash_ip[proj], &packet->record.m_cdn.cdn_ip);
	if (offset == NULL) {
		if ((cdn_speed_ip_mmap[ proj ].records_num + 1) > MAX_IP_RECORD)
			ERROR_RETURN(("ip records reach max number %u",MAX_IP_RECORD), 0);

		if (g_ipDict.find(packet->record.m_cdn.cdn_ip, code_cdn, &isp_key) == false)
			return 0;
		cdn_speed_ip_mmap[ proj ].records_num++;
		cdn_speed_ip_mmap[ proj ].mmap_head->records_num++;
		cdn_speed_ip_mmap[ proj ].ready_num++;
		cdn_speed_ip_mmap[ proj ].mmap_head->ready_num++;
		cdn_ip_p += cdn_speed_ip_mmap[ proj ].records_num - 1;
		cdn_ip_p->offset = cdn_speed_ip_mmap[ proj ].records_num - 1;
		cdn_ip_p->isp_key = isp_key;
		cdn_ip_p->step = 0;
		cdn_ip_p->ip = packet->record.m_cdn.cdn_ip;
		cdn_ip_p->project_number = packet->project_number;
		time_step = cdn_ip_p->step;
		cdn_ip_p->time[time_step] = packet->test_time;
		int k;
		for ( k = 0; k < CDN_TIME_INTER; k++) {
			cdn_ip_p->speed[(time_step + k) % CDN_TIME_INTER] = packet->record.m_cdn.speed;
			cdn_ip_p->count[(time_step + k) % CDN_TIME_INTER] = 1;
		}

		g_hash_table_insert(cdn_hash_ip[proj], &cdn_ip_p->ip, &cdn_ip_p->offset);
	}
	else {
		cdn_ip_p += *offset;
		time_step = cdn_ip_p->step;
		if (cdn_ip_p->count[time_step] == 0) {
			cdn_speed_ip_mmap[ proj ].ready_num++;
			cdn_speed_ip_mmap[ proj ].mmap_head->ready_num++;
		}
		cdn_ip_p->offset = *offset;
		cdn_ip_p->ip = packet->record.m_cdn.cdn_ip;
		cdn_ip_p->project_number = packet->project_number;
		cdn_ip_p->time[time_step] = max(cdn_ip_p->time[time_step], packet->test_time);

		int k;
		for ( k = 0; k < CDN_TIME_INTER; k++) {
			cdn_ip_p->speed[(time_step + k) % CDN_TIME_INTER] += packet->record.m_cdn.speed;
			cdn_ip_p->count[(time_step + k) % CDN_TIME_INTER] ++;
		}
	}

	//更新area mmap
	if (!find_client)
		return 0;

	offset = (uint32_t*)g_hash_table_lookup(hash_cityisp, code_client.key);
	if (offset == NULL)
		return 0;
	else {
		cdn_area_p += *offset;
		time_step = cdn_area_p->step;
		if (cdn_area_p->count[time_step] == 0) {
			cdn_speed_area_mmap[ proj ].ready_num++;
			cdn_speed_area_mmap[ proj ].mmap_head->ready_num++;
		}
		cdn_area_p->project_number = packet->project_number;
		cdn_area_p->time[time_step] = max(packet->test_time, cdn_area_p->time[time_step]);

		int k;
		for ( k = 0; k < CDN_TIME_INTER; k++) {
			cdn_area_p->speed[(time_step + k) % CDN_TIME_INTER] += packet->record.m_cdn.speed;
			cdn_area_p->count[(time_step + k) % CDN_TIME_INTER] ++;
		}
	}

	return 0;
}

fcgi_url_t a_url;
uint8_t url_forward_buf[BUF_NET_SEND_LEN];
proto_url_test_fixed_t *forward_url;
int set_url_mmap(fcgi_store_t *buf, uint32_t data_len)
{
	fcgi_store_t *packet = buf;
	static struct timeval now;
	gettimeofday(&now, NULL);


	if (packet->record.m_url.count == 0 || packet->record.m_url.count > MAX_POINT)
		return 0;//ERROR_RETURN(("BAD URL PACKET"), 0);

	DEBUG_LOG("GET URL PACKET:time=%u,proj=%u,client=%u,page=%u,vcount=%u,v[0]=%u",packet->test_time,
			packet->project_number,packet->client_ip,packet->record.m_url.page_id,
			packet->record.m_url.count,packet->record.m_url.vlist[0]);

	uint32_t counter = 0, ccc = 0;
	int n = 0;
	char str[1024];
	for (counter = 0; counter < packet->record.m_url.count; counter ++) {
		if (packet->record.m_url.vlist[counter] > 100000) {
			for (ccc = 0; ccc < packet->record.m_url.count; ccc ++)
				n += sprintf(str + n, " %u", packet->record.m_url.vlist[ccc]);
			break;
		}
	}
	if (ccc != 0)
		ERROR_RETURN(("GET URL PACKET:time=%u,proj=%u,client=%u,page=%u,vcount=%u,list:%s",					packet->test_time,
				packet->project_number,packet->client_ip,packet->record.m_url.page_id,
				packet->record.m_url.count, str), 0);

	uint32_t *offset;
	uint32_t isp_key;
	code_t code;

	if (g_ipDict.find(packet->client_ip, code, &isp_key)== false)
		return 0;
	offset = (uint32_t*)g_hash_table_lookup(hash_cityisp, code.key);
	if (offset == NULL)
		return 0;

	//added by singku 2011-08-30 for url raw data forward----start
	uint32_t forward_len = sizeof(fcgi_common_t) + sizeof(a_url.page_id) + sizeof(a_url.count)
									+ packet->record.m_url.count * sizeof(a_url.vlist[0]);
	init_proto_head(url_forward_buf, sizeof(protocol_t) + forward_len,
			proto_as_url_stat_forward, 0, 0, 0);
	memcpy(url_forward_buf + sizeof(protocol_t), &(packet->project_number), forward_len);
	send_data(url_forward_buf, sizeof(protocol_t) + forward_len, proto_as_url_stat_forward);
	//added by singku 2011-08-30 for url raw data forward----end

	int proj_id = (packet->project_number - 1) % max_project_number;
	int page_id = packet->record.m_url.page_id;
	int *page_index;
	int page_offset;
	int pos;
	page_index = (int*)g_hash_table_lookup(url_page_index[proj_id], &page_id);
	if (page_index == NULL) {
		if (url_page_status[proj_id].used_count == max_page_per_proj)
			ERROR_RETURN(("PROJ %d's URL PAGE IS RUNNING OUT",packet->project_number), 0);
		else {
			page_offset = find_available_url_page(proj_id);
			if (page_offset == -1)
				ERROR_RETURN(("PROJ %d's URL PAGE IS NOT RUNNING OUT,BUT CANT FIND USABLE PAGE",
						packet->project_number), 0);
			pos = proj_id * max_page_per_proj + page_offset;
			url_page_mmap[pos].mmap_head->key = page_id;
			url_page_mmap[pos].mmap_head->value = page_offset;
			url_page_mmap[pos].key = page_id;
			url_page_mmap[pos].value = page_offset;
			g_hash_table_insert(url_page_index[proj_id],
					&(url_page_mmap[pos].key),&(url_page_mmap[pos].value));
			url_page_status[proj_id].used_count ++;
			url_page_status[proj_id].status[page_offset] = 1;
		}

	}
	else {
		page_offset = *page_index;
		pos = proj_id * max_page_per_proj + page_offset;
	}

	if (packet->test_time > url_page_mmap[pos].latest_time) {
		url_page_mmap[pos].latest_time = packet->test_time;
		url_page_mmap[pos].mmap_head->latest_time = packet->test_time;
	}
	url_test_t *url_test_p = (url_test_t*)url_page_mmap[pos].mmap_p;


	url_test_p += *offset;
	if (url_test_p->count == 0) {
		url_page_mmap[pos].ready_num++;
		url_page_mmap[pos].mmap_head->ready_num++;
		url_test_p->page_id = packet->record.m_url.page_id;
		url_test_p->project_number = packet->project_number;
		memset(url_test_p->vlist, 0x0, sizeof(url_test_p->vlist));
	}
	int k;
	for(k = 0; k < (int)packet->record.m_url.count && k < MAX_POINT; k++){
		url_test_p->vlist[k] += packet->record.m_url.vlist[k];
	}
	url_test_p->v_count = packet->record.m_url.count;
	url_test_p->test_time = max(url_test_p->test_time, packet->test_time);
	url_test_p->count ++;

	return 0;
}
