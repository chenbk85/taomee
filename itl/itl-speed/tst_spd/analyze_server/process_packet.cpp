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
#include "net.h"
#include "net_if.h"
}
#include "proto.h"
#include "common.h"
#include "ip_dict.hpp"
#include "load_conf_ip_hash_mmap.hpp"
#include "send_data.hpp"
#include "cache.hpp"	//added by singku for cache 2011-07-26
#include "process_packet.hpp"

static uint32_t other_start = 0;
char* mmap_tmpfile_pmap = NULL;
tmr_t g_timer;		//处理CDN MMAP的定时器

uint32_t tmp_packet_tag = 1;//用以在协议头标记发出的数据包，收到回应包时判定此字段是否属于当前正在处理的tmp mmap

uint32_t backup_interval = 1; //备份的分钟间隔，默认1分钟备份1次，如果收包超时，则指数增长.

pthread_spinlock_t lock_cdn_ip;

static inline int wait_all_tmpdata_ok(char *pmap)
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
		//backup_interval *= 2;//收包超时，则备份时间指数增长
		return -1;
	}
	backup_interval = 1;//收包成功，则备份时间重置.
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
	uint32_t send_count=0;
	uint32_t ready = file_head_p->ready_num;
	int number;// = cdn_ip_p->project_number;
	uint32_t packet_tag = tmp_packet_tag << SHIFT_BITS;
	int now;

resend_cdnip:
	ready -= send_count;
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
			pthread_spin_lock(&lock_cdn_ip);
			if (cur->t_count == 0 && (i+1) < cdn_speed_ip_mmap[proj].records_num) {//如果对应的当前mmap中该cdn ip也无数据
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
			}
			pthread_spin_unlock(&lock_cdn_ip);
		}
		//end-added by singku for remove ip which is out of date 2011-08-04

		if ((cdn_ip_p + i)->t_count == 0)
			continue;

		/*start---added by singku for cache 11-07-27*/
		if (other_start == 1)
			update_cache(proto_cache_cdn_node, (void*)(cdn_ip_p + i));
		/*end ----added by singku for cache 11-07-27*/

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
		ERROR_RETURN(("SEND COUNT == 0:map_tag:%u,type:CDN IP", tmp_packet_tag), 0);
	if (wait_all_tmpdata_ok(mmap_p) == -1) {
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
	uint32_t send_count=0;
	uint32_t ready = file_head_p->ready_num;
	int number;// = cdn_area_p->project_number;
	uint32_t packet_tag = tmp_packet_tag << SHIFT_BITS;

resend_cdnarea:
	ready -= send_count;
	send_count = 0;
	for (i = 0; i < file_head_p->records_num; i++) {

		(cdn_area_p + i)->t_time = (cdn_area_p + i)->time[(cdn_area_p + i)->step];
		(cdn_area_p + i)->t_speed = (cdn_area_p + i)->speed[(cdn_area_p + i)->step];
		(cdn_area_p + i)->t_count = (cdn_area_p + i)->count[(cdn_area_p + i)->step];


		if ((cdn_area_p + i)->t_count == 0)
			continue;

		/*start---added by singku for cache 11-07-26*/
		if (other_start == 1)
        {
            cdn_speed_area_t *p_cdn_area = cdn_area_p + i;
            INFO_LOG("AAAA:time[%u] speed[%u] count[%u]", p_cdn_area->t_time, p_cdn_area->t_speed, p_cdn_area->t_count);
            int m = 0;
            for (m = 0; m < 6; m++)
            {
                INFO_LOG("AAAA:dis[%d]: %u", m, p_cdn_area->speed_distribution[(cdn_area_p + i)->step][m]);
            }

			update_cache(proto_cache_cdn_area, (void*)(cdn_area_p + i));
        }
		/*end ----added by singku for cache 11-07-26*/

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
		buf_ptr->test_time = (cdn_area_p + i)->t_time;
		buf_ptr->speed = (cdn_area_p + i)->t_speed;
		buf_ptr->count = (cdn_area_p + i)->t_count;

		if (send_data(buf, len, proto_as_cdn_report_cityisp) == -1)
			return -1;

		send_count ++;
		if (send_count == ready)
			break;
	}

	if (send_count == 0)
		ERROR_RETURN(("SEND COUNT == 0:map_tag:%u,type:CDN AREA", tmp_packet_tag), 0);
	if (wait_all_tmpdata_ok(mmap_p) == -1) {
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
	uint32_t send_count = 0;
	uint32_t ready = file_head_p->ready_num;
	int number;// = url_page_p->project_number;
	uint32_t packet_tag = tmp_packet_tag << SHIFT_BITS;

resend_url:
	ready -= send_count;
	send_count = 0;
	for (i = 0; i < file_head_p->records_num; i++) {
		if ((url_page_p + i)->count == 0)
			continue;
		if ((url_page_p + i)->v_count == 0) {
			file_head_p->ready_num --;
			continue;
		}

		/*start---added by singku for cache 11-07-27*/
		if (other_start == 1)
			update_cache(proto_cache_url_area, (void*)(url_page_p + i));
		/*end ----added by singku for cache 11-07-27*/

		//init protocol_head;
		number = ((url_page_p + i)->project_number -1) % max_project_number + 1;
		int vlen = (url_page_p + i)->v_count * sizeof((url_page_p + i)->vlist[0]);
		len = sizeof(protocol_t) + sizeof(proto_url_test_fixed_t) + vlen;
		init_proto_head(buf, len, proto_as_url_stat_page_time, (packet_tag + number), i, 0);
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
		ERROR_RETURN(("SEND COUNT == 0:map_tag:%u,type:URL PAGE", tmp_packet_tag), 0);
	if (wait_all_tmpdata_ok(mmap_p) == -1) {
		ERROR_LOG("URL PAGE:WAIT RET PACKETS TIME OUT");
		goto resend_url;
	}

	return 0;
}

static int process_tmpfile(char *filepath)
{
	mmap_file_head_t tmp_head;
	int fd = open(filepath, O_RDWR);
	if (fd == -1) {
		ERROR_RETURN(("fail to open file %s", filepath), 0);
	}
	read(fd, &tmp_head, sizeof(mmap_file_head_t));
	if (memcmp(tmp_head.file_symbol, MMAP_FILE_SYMBOL, MMAP_FILE_SYMBOL_LEN) != 0) {
		close(fd);
		remove(filepath);
		ERROR_RETURN(("invalid mmap tmp file %s", filepath), 0);
	}
	if (tmp_head.records_num == 0 || tmp_head.ready_num == 0) {
		close(fd);
		remove(filepath);
		ERROR_RETURN(("ready or records equal 0 %s:records:%u,ready:%u",
				filepath, tmp_head.records_num, tmp_head.ready_num), 0);
	}
	if (tmp_head.records_num < tmp_head.ready_num) {
		close(fd);
		remove(filepath);
		ERROR_RETURN(("ready > records %s:records:%u,ready:%u",
				filepath, tmp_head.records_num, tmp_head.ready_num), 0);
	}
    
	lseek(fd, 0, SEEK_SET);//如果正常，则回到文件头
	uint32_t size;
	if (tmp_head.type == MMAP_TYPE_CDN_IP)
		size = cdn_mmap_size_ip;
	else if (tmp_head.type == MMAP_TYPE_CDN_AREA)
		size = cdn_mmap_size_area;
	else if (tmp_head.type == MMAP_TYPE_URL)
		size = url_mmap_size_page;
	else {
		close(fd);
		return 0;//other file
	}
	size += MMAP_FILE_HEAD_LEN;

	mmap_tmpfile_pmap = (char *)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mmap_tmpfile_pmap == (char *)-1) {
		close (fd);
		ERROR_RETURN(("fail to mmap %s", filepath), 0);
	}
	close(fd);
	//发送mmap的内容
	DEBUG_LOG("TMP MMAP TYPE:%02u,READY:%u,TAG:%u", tmp_head.type,tmp_head.ready_num, tmp_packet_tag);
    int ret_val;
	if (tmp_head.type == MMAP_TYPE_CDN_IP) {
		ret_val = send_cdn_ip_2_dbserver(mmap_tmpfile_pmap);
    }
	else if (tmp_head.type == MMAP_TYPE_CDN_AREA) {
        ret_val = send_cdn_area_2_dbserver(mmap_tmpfile_pmap);
    }
	else if (tmp_head.type == MMAP_TYPE_URL) {
		ret_val = send_url_page_2_dbserver(mmap_tmpfile_pmap);
	}

	munmap(mmap_tmpfile_pmap, size);
    
    if (ret_val == -1)
        ERROR_RETURN(("send mmap:file not complete %s", filepath) , -1);

	remove(filepath);
	return 0;
}

static int process_tmpdir(DIR *dir)
{
	struct dirent 	*ptr;
	char	full_path[DIR_PATH_LEN];

	//将备份文件中的文件名中的前缀数字提取出来，如果该数字等于pid 则说明是本进程生成的文件.
	while ((ptr = readdir(dir)) != NULL) {
		if (memcmp(ptr->d_name, "mmap_backup_file", (sizeof("mmap_backup_file") - 1)) == 0) {
			snprintf(full_path, sizeof(full_path), "%s/%s", mmap_tmpfile_dirpath, ptr->d_name);

			if (-1 == process_tmpfile(full_path))
				return -1;
		}
	}
	return 0;
}

void* send_file(void*)
{
	DIR *dir;
	while (1) {
		//对于CDN和url的数据，由于直接保存了整个mmap，所以发送的时候可以直接将这个文件重新映
		//到一个临时的mmap，然后按照这个mmap的类型做相应的处理.
		dir = opendir((const char *)mmap_tmpfile_dirpath);
		if (dir == NULL)
			ERROR_LOG("cant open idc_file_dirpath,%s\t", mmap_tmpfile_dirpath);

		process_tmpdir(dir);

		closedir(dir);

		//先将旧的文件发完，再生成新文件，因为旧文件不能更新缓存
		if (other_start == 0) {
			//下一个整点分钟时间触发事件
			ADD_TIMER_EVENT(&g_timer, backup_mmap_file, NULL,
					get_now_tv()->tv_sec + 60 - get_now_tv()->tv_sec % 60);
			other_start = 1;
		}

		sleep(30);
	}
}

int backup_mmap_file(void* owner, void* data)
{
	//转储MMAP文件并发送该文件
	char filepath[DIR_PATH_LEN];
   char newpath[DIR_PATH_LEN];
	uint32_t i, j, pos;
	struct tm p;
	uint64_t sec;
	FILE *fp;
	//按时间、类型、项目号来备份mmap
	sec = time((time_t*)NULL);
	localtime_r((const time_t*)&sec,&p);
	//备份cdn的mmap
	for (i = 0; i < max_project_number; i++) {
		if (cdn_speed_ip_mmap[i].mmap_head->ready_num != 0) {
			snprintf(filepath, sizeof(filepath),
					"%s/tmp.mmap_backup_file_cdn_ip_[%03d]%04d-%02d-%02d-%02d-%02d-%02d",
					mmap_tmpfile_dirpath, i + 1, p.tm_year + 1900, p.tm_mon + 1, p.tm_mday,
					p.tm_hour, p.tm_min, p.tm_sec);
			snprintf(newpath, sizeof(newpath),
					"%s/mmap_backup_file_cdn_ip_[%03d]%04d-%02d-%02d-%02d-%02d-%02d",
					mmap_tmpfile_dirpath, i + 1, p.tm_year + 1900, p.tm_mon + 1,p.tm_mday,
					p.tm_hour, p.tm_min, p.tm_sec);
				 fp = fopen(filepath, "wb+");
			if (fp == NULL)
				ERROR_RETURN(("can't open backup file %s",filepath), -1);

			if (fwrite(cdn_speed_ip_mmap[i].mmap_head, 1, cdn_mmap_size_ip + MMAP_FILE_HEAD_LEN, fp)
					!= (cdn_mmap_size_ip + MMAP_FILE_HEAD_LEN)) {
				ERROR_LOG("BACK UP CDN IP MMAP FILE ERROR:proj:%u,records:%u,ready:%u", i,
						cdn_speed_ip_mmap[i].mmap_head->records_num,cdn_speed_ip_mmap[i].mmap_head->ready_num);
				fclose(fp);
				remove(filepath);
			} else {
				fclose(fp);
				rename(filepath, newpath);
				cdn_speed_ip_mmap[i].ready_num = cdn_speed_ip_mmap[i].mmap_head->ready_num = 0;

				cdn_speed_ip_t *ptr = (cdn_speed_ip_t*)cdn_speed_ip_mmap[i].mmap_p;
				for (j = 0; j < cdn_speed_ip_mmap[i].mmap_head->records_num; j++) {
					pos = (ptr + j)->step;
					(ptr + j)->speed[pos] = 0;
					(ptr + j)->count[pos] = 0;
					memset((ptr + j)->speed_distribution[pos], 0, sizeof((ptr + j)->speed_distribution[pos]));
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
			snprintf(filepath, sizeof(filepath),
					"%s/tmp.mmap_backup_file_cdn_area_[%03d]%04d-%02d-%02d-%02d-%02d-%02d",
					mmap_tmpfile_dirpath, i + 1, p.tm_year + 1900, p.tm_mon + 1, p.tm_mday,
					p.tm_hour, p.tm_min, p.tm_sec);
			snprintf(newpath, sizeof(newpath),
					"%s/mmap_backup_file_cdn_area_[%03d]%04d-%02d-%02d-%02d-%02d-%02d",
					mmap_tmpfile_dirpath, i + 1, p.tm_year + 1900, p.tm_mon + 1, p.tm_mday,
					p.tm_hour, p.tm_min, p.tm_sec);
			fp = fopen(filepath, "wb+");
			if (fp == NULL)
				ERROR_RETURN(("can't open backup file %s",filepath), -1);
			if (fwrite(cdn_speed_area_mmap[i].mmap_head, 1, cdn_mmap_size_area + MMAP_FILE_HEAD_LEN, fp)
					!= (cdn_mmap_size_area + MMAP_FILE_HEAD_LEN)) {
				ERROR_LOG("BACK UP CDN AREA MMAP FILE ERROR:proj:%u,records:%u,ready:%u", i,
						cdn_speed_area_mmap[i].mmap_head->records_num,cdn_speed_area_mmap[i].mmap_head->ready_num);
				fclose(fp);
				remove(filepath);
			} else {
				fclose(fp);
				rename(filepath, newpath);
				cdn_speed_area_mmap[i].ready_num = cdn_speed_area_mmap[i].mmap_head->ready_num = 0;

				cdn_speed_area_t *ptr = (cdn_speed_area_t*)cdn_speed_area_mmap[i].mmap_p;
                INFO_LOG("BBBB: cdn_speed_area_mmap[%d].mmap_head->records_num: %u",
                        i,  cdn_speed_area_mmap[i].mmap_head->records_num);
				for (j = 0; j < cdn_speed_area_mmap[i].mmap_head->records_num; j++) {
					pos = (ptr + j)->step;

                    //comments////////////////////////////////////////////////////////////////
                    cdn_speed_area_t *p_cdn_area = ptr + j;
                    if (p_cdn_area->count[pos] != 0 || p_cdn_area->t_count != 0)
                    {
                        INFO_LOG("BBBB:time[%u] speed[%u] count[%u]",
                                p_cdn_area->t_time, p_cdn_area->t_speed, p_cdn_area->t_count);
                        INFO_LOG("BBBB:pos[%u] time[%u] speed[%u] count[%u]",
                                pos, p_cdn_area->time[pos], p_cdn_area->speed[pos], p_cdn_area->count[pos]);
                        int m = 0;
                        for (m = 0; m < 6; m++)
                        {
                            INFO_LOG("BBBB:pos[%u] dis[%d]: %u", pos, m, p_cdn_area->speed_distribution[pos][m]);
                        }
                        int next_pos = (pos + 1) % CDN_TIME_INTER;
                        INFO_LOG("BBBB:next_pos[%u] time[%u] speed[%u] count[%u]",
                                next_pos, p_cdn_area->time[next_pos], 
                                p_cdn_area->speed[next_pos], p_cdn_area->count[next_pos]);
                        for (m = 0; m < 6; m++)
                        {
                            INFO_LOG("BBBB:next_pos[%u] dis[%d]: %u", 
                                    next_pos, m, p_cdn_area->speed_distribution[next_pos][m]);
                        }
                    }
                    /**
                    */
                    //////////////////////////////////////////////////////////////////////////

					(ptr + j)->count[pos] = 0;
					(ptr + j)->speed[pos] = 0;
					memset((ptr + j)->speed_distribution[pos], 0, sizeof((ptr + j)->speed_distribution[pos]));
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
	//备份url的mmap
	int now = (int)time((time_t*)NULL);
	for (i = 0; i < max_project_number; i++) {
		for (j = 0; j < max_page_per_proj; j++) {
			pos = i * max_page_per_proj + j;
			if (url_page_mmap[pos].mmap_head->ready_num != 0) {
				snprintf(filepath, sizeof(filepath),
						"%s/tmp.mmap_backup_file_url_page_[%03d_%d]%04d-%02d-%02d-%02d-%02d-%02d",
						mmap_tmpfile_dirpath, i + 1, url_page_mmap[pos].mmap_head->key,
						p.tm_year + 1900, p.tm_mon + 1, p.tm_mday,
						p.tm_hour, p.tm_min, p.tm_sec);
				snprintf(newpath, sizeof(newpath),
						"%s/mmap_backup_file_url_page_[%03d_%d]%04d-%02d-%02d-%02d-%02d-%02d",
						mmap_tmpfile_dirpath, i + 1, url_page_mmap[pos].mmap_head->key,
						p.tm_year + 1900, p.tm_mon + 1, p.tm_mday,
						p.tm_hour, p.tm_min, p.tm_sec);
				fp = fopen(filepath, "wb+");
				if (fp == NULL)
					ERROR_RETURN(("can't open backup file %s",filepath), -1);
				if (fwrite(url_page_mmap[pos].mmap_head, 1, url_mmap_size_page + MMAP_FILE_HEAD_LEN, fp)
						!= (url_mmap_size_page + MMAP_FILE_HEAD_LEN)) {
					ERROR_LOG("BACK UP URL MMAP FILE ERROR:proj:%u,records:%u,ready:%u", url_page_mmap[pos].value,
							url_page_mmap[pos].mmap_head->records_num,url_page_mmap[pos].mmap_head->ready_num);
					fclose(fp);
					remove(filepath);
				} else {
					fclose(fp);
					rename(filepath, newpath);
					url_page_mmap[pos].ready_num = url_page_mmap[pos].mmap_head->ready_num = 0;
					uint32_t k;
					url_test_t *p = (url_test_t*)url_page_mmap[pos].mmap_p;
					for (k = 0; k < url_page_mmap[pos].mmap_head->records_num; k++)
						(p + k)->count = 0;
				}
			} else if ((now - (int)(url_page_mmap[pos].latest_time)) > 86400) {//mmap一天没用,则回收.
				g_hash_table_remove(url_page_index[i], &(url_page_mmap[pos].key));
				url_page_mmap[pos].key = url_page_mmap[pos].mmap_head->key = 0;
				url_page_mmap[pos].value = url_page_mmap[pos].mmap_head->value = 0;
				url_page_mmap[pos].latest_time = url_page_mmap[pos].mmap_head->latest_time = 0;
				url_page_mmap[pos].ready_num = url_page_mmap[pos].mmap_head->ready_num = 0;
				url_page_status[i].status[url_page_mmap[pos].value] = 0;
				if (url_page_status[i].used_count > 0)
					url_page_status[i].used_count --;
			}
		}
	}
	//下一个整点分钟时间继续触发事件
	if (data == NULL) {
		ADD_TIMER_EVENT(&g_timer, backup_mmap_file, NULL,
			get_now_tv()->tv_sec + 60 * backup_interval - get_now_tv()->tv_sec % 60);
	}

	return 0;
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

int set_cdn_mmap(char *buf, uint32_t data_len)
{
    int k;
	cdn_speed_ip_t *cdn_ip_p;
	cdn_speed_area_t *cdn_area_p;
	cdn_speed_area_t *tmp_for_cdn_node;
	static struct timeval start ={0, 0};
	static struct timeval now ={0, 0};
	static uint64_t recv_cdn = 0;
	uint32_t	isp_key;
	code_t code_client, code_cdn;
	bool find_client = false;
	bool speed_invalid = false;
	fcgi_packet_t *packet = (fcgi_packet_t*)buf;

	if(data_len != (sizeof(fcgi_packet_common_t) + sizeof(fcgi_cdn_t)))
		ERROR_RETURN(("BAD CDN PACKETS"), 0);

	if (start.tv_sec == 0)
		gettimeofday(&start, NULL);
	gettimeofday(&now, NULL);

    if (ignore_project[packet->project_number] == 1)
        return 0;

	++recv_cdn;
	if (!(start.tv_sec == now.tv_sec && start.tv_usec == now.tv_usec))
		DEBUG_LOG("TOTAL_RECVED_CDN_PACKETS:%lu,CDN/DAY=%.2f", recv_cdn,
				recv_cdn/(((now.tv_sec - start.tv_sec)*1000000 + now.tv_usec - start.tv_usec)/(86400*1000000.0)));

    if (packet->record.m_cdn.speed == 0)
        packet->record.m_cdn.speed = CDN_SPEED_INVALID;

	if (packet->project_number == 0
		|| (packet->record.m_cdn.speed > cdn_max_speed && packet->record.m_cdn.speed != CDN_SPEED_INVALID))
		return 0;
	if (packet->record.m_cdn.speed == CDN_SPEED_INVALID)
		speed_invalid = true;

	if (speed_invalid) {
		DEBUG_LOG("GET CDN PACKET:time=%u,proj=%u,client=%u,cdn=%u,speed=-1",packet->test_time,
			packet->project_number,packet->client_ip,packet->record.m_cdn.cdn_ip);
	} else {
		DEBUG_LOG("GET CDN PACKET:time=%u,proj=%u,client=%u,cdn=%u,speed=%u",packet->test_time,
			packet->project_number,packet->client_ip,packet->record.m_cdn.cdn_ip,packet->record.m_cdn.speed);
	}

	if (g_ipDict.find(packet->client_ip, code_client, &isp_key) == true)
		find_client = true;
    if (code_client.province_code == 830000)//国外
        return 0;

	if (((int)(now.tv_sec) - (int)(packet->test_time)) > CDN_TIME_INTER*60 && speed_invalid == false) {
		//old数据 直接转发，统计数据加和，需要过滤掉speed=-1的数据.
		init_proto_head(cdn_forward_buf, sizeof(protocol_t) + sizeof(proto_cdn_speed_ip_t),
				proto_as_cdn_report_ip, 0, 0, 0);
		forward_ip = (proto_cdn_speed_ip_t*)(cdn_forward_buf + sizeof(protocol_t));
		forward_ip->ip = packet->record.m_cdn.cdn_ip;
		forward_ip->project_number = packet->project_number;
		forward_ip->speed = packet->record.m_cdn.speed;
		forward_ip->count = 1;
		for (k = 0; k < CDN_TIME_INTER; k++) {
			forward_ip->test_time = packet->test_time + k * 60 ;//add 60 secs
			send_data(cdn_forward_buf, sizeof(protocol_t) + sizeof(proto_cdn_speed_ip_t),
					proto_as_cdn_report_ip);
		}

		if (!find_client)
			return 0;

		init_proto_head(cdn_forward_buf, sizeof(protocol_t) + sizeof(proto_cdn_speed_area_t),
				proto_as_cdn_report_cityisp, 0, 0, 0);
		forward_area = (proto_cdn_speed_area_t*)(cdn_forward_buf + sizeof(protocol_t));
		forward_area->project_number = packet->project_number;
		forward_area->province_code = code_client.province_code;
		forward_area->city_code = code_client.city_code;
		memcpy(forward_ip_isp->isp, code_client.isp, ISP_LEN);
		forward_area->speed = packet->record.m_cdn.speed;
		forward_area->count = 1;
		for (k = 0; k < CDN_TIME_INTER; k++) {
			forward_area->test_time = packet->test_time + k * 60;//add 60 secs
			send_data(cdn_forward_buf, sizeof(protocol_t) + sizeof(proto_cdn_speed_area_t),
					proto_as_cdn_report_cityisp);
		}
		return 0;
	}

	//added by singku 2011-09-01 for cdn details forward ---start
	//cdn详情数据直接转发，不需要过滤speed=-1的数据。
    
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
    
	//如果send_cdn_ip_2_dbser线程要删除过期的某个cdn_ip,为了保证数据安全，必须等待其删除完毕
	pthread_spin_lock(&lock_cdn_ip);
	int proj = (packet->project_number-1) % max_project_number;
	cdn_ip_p = (cdn_speed_ip_t*)cdn_speed_ip_mmap[ proj ].mmap_p;
	cdn_area_p = (cdn_speed_area_t*)cdn_speed_area_mmap[ proj ].mmap_p;
	tmp_for_cdn_node = (cdn_speed_area_t*)cdn_speed_area_mmap[ proj ].mmap_p;

	//更新ip mmap
	uint32_t time_step;
    int pos;
	uint32_t tmp_speed = packet->record.m_cdn.speed / (1024); //KB
	uint32_t *offset = (uint32_t*)g_hash_table_lookup(cdn_hash_ip[proj], &packet->record.m_cdn.cdn_ip);
	if (offset == NULL) {
		if ((cdn_speed_ip_mmap[ proj ].records_num + 1) > MAX_IP_RECORD) {
			pthread_spin_unlock(&lock_cdn_ip);
			ERROR_RETURN(("ip records reach max number %u",MAX_IP_RECORD), 0);
		}

		if (g_ipDict.find(packet->record.m_cdn.cdn_ip, code_cdn, &isp_key) == false) {
			pthread_spin_unlock(&lock_cdn_ip);
			return 0;
		}
        if (code_cdn.province_code == 830000) {//国外
            pthread_spin_unlock(&lock_cdn_ip);
            return 0;
        }
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

        for (k = 0; k < CDN_TIME_INTER; k++) {
            pos = (time_step + k) % CDN_TIME_INTER;
            cdn_ip_p->speed[pos] = (speed_invalid == false) ? packet->record.m_cdn.speed : 0;
            cdn_ip_p->count[pos] = 1;//(speed_invlaid == false) ? 1 : 0;

		    //added by singku for speed distribution 2011-11-21
            //modify by tonyliu 2012-11-07
		    if (speed_invalid) {
		    	cdn_ip_p->speed_distribution[pos][0] = 1;
		    } else {
		    	if (tmp_speed < cdn_speed_dis[0]) {
		    		cdn_ip_p->speed_distribution[pos][1] = 1;
		    	} else if (tmp_speed < cdn_speed_dis[1]) {
		    		cdn_ip_p->speed_distribution[pos][2] = 1;
		    	} else if (tmp_speed < cdn_speed_dis[2]) {
		    		cdn_ip_p->speed_distribution[pos][3] = 1;
		    	} else if (tmp_speed < cdn_speed_dis[3]) {
		    		cdn_ip_p->speed_distribution[pos][4] = 1;
		    	} else {
		    		cdn_ip_p->speed_distribution[pos][5] = 1;
		    	}
		    }
		    //added by singku for speed distribution 2011-11-21
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

		for (k = 0; k < CDN_TIME_INTER; k++) {
            pos = (time_step + k) % CDN_TIME_INTER;
			cdn_ip_p->speed[pos] += (speed_invalid == false) ? packet->record.m_cdn.speed : 0;
            cdn_ip_p->count[pos] += 1;
            //if (cdn_ip_p->count[pos] == 0) {
            //    cdn_ip_p->count[pos] = 1;
            //} else {
			//    cdn_ip_p->count[pos] += (speed_invalid == false) ? 1 : 0;
		    //}

		    //added by singku for speed distribution 2011-11-21
            //modify by tonyliu 2012-11-07
		    if (speed_invalid) {
		    	cdn_ip_p->speed_distribution[pos][0] ++;
		    } else {
		    	if (tmp_speed < cdn_speed_dis[0]) {
		    		cdn_ip_p->speed_distribution[pos][1] ++;
		    	} else if (tmp_speed < cdn_speed_dis[1]) {
		    		cdn_ip_p->speed_distribution[pos][2] ++;
		    	} else if (tmp_speed < cdn_speed_dis[2]) {
		    		cdn_ip_p->speed_distribution[pos][3] ++;
		    	} else if (tmp_speed < cdn_speed_dis[3]) {
		    		cdn_ip_p->speed_distribution[pos][4] ++;
		    	} else {
		    		cdn_ip_p->speed_distribution[pos][5] ++;
		    	}
		    }
		    //added by singku for speed distribution 2011-11-21
        }
	}
	pthread_spin_unlock(&lock_cdn_ip);

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

		for (k = 0; k < CDN_TIME_INTER; k++) {
            pos = (time_step + k) % CDN_TIME_INTER;
			cdn_area_p->speed[pos] += (speed_invalid == false) ? packet->record.m_cdn.speed : 0;
            cdn_area_p->count[pos] += 1;
            //if (cdn_area_p->count[pos] == 0) {
            //    cdn_area_p->count[pos] = 1;
            //} else {
			//    cdn_area_p->count[pos] += (speed_invalid == false) ? 1 : 0;
		    //}

		    //added by singku for speed distribution 2011-11-21
            //modify by tonyliu 2012-11-07
		    if (speed_invalid) {
		    	cdn_area_p->speed_distribution[pos][0] ++;
		    } else {
		    	if (tmp_speed < cdn_speed_dis[0]) {
		    		cdn_area_p->speed_distribution[pos][1] ++;
		    	} else if (tmp_speed < cdn_speed_dis[1]) {
		    		cdn_area_p->speed_distribution[pos][2] ++;
		    	} else if (tmp_speed < cdn_speed_dis[2]) {
		    		cdn_area_p->speed_distribution[pos][3] ++;
		    	} else if (tmp_speed < cdn_speed_dis[3]) {
		    		cdn_area_p->speed_distribution[pos][4] ++;
		    	} else {
		    		cdn_area_p->speed_distribution[pos][5] ++;
		    	}
		    }
		    //added by singku for speed distribution 2011-11-21
        }

	}

	return 0;
}

fcgi_url_t a_url;
uint8_t url_forward_buf[BUF_NET_SEND_LEN];
proto_url_test_fixed_t *forward_url;
int set_url_mmap(char *buf, uint32_t data_len)
{
	fcgi_packet_t *packet = (fcgi_packet_t*)buf;
	static struct timeval now;
	gettimeofday(&now, NULL);

	if(data_len < (sizeof(fcgi_packet_common_t) + sizeof(a_url.count) + sizeof(a_url.page_id)
						+ packet->record.m_url.count * sizeof(a_url.vlist[0])))
		return 0;//ERROR_RETURN(("BAD URL PACKETS"), 0);

    if (ignore_project[packet->project_number] == 1)
        return 0;

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
    if (code.province_code == 830000)//国外
        return 0;

	offset = (uint32_t*)g_hash_table_lookup(hash_cityisp, code.key);
	if (offset == NULL)
		return 0;

	if (((int)(now.tv_sec) - (int)(packet->test_time)) > CDN_TIME_INTER*60) {
		int len = sizeof(protocol_t) + sizeof(proto_url_test_fixed_t);
		int vlen = packet->record.m_url.count * sizeof(packet->record.m_url.vlist[0]);

		init_proto_head(buf, len + vlen, proto_as_url_stat_page_time, 0, 0, 0);
		forward_url = (proto_url_test_fixed_t*)(buf + sizeof(protocol_t));
		forward_url->project_number = packet->project_number;
		forward_url->page_id = packet->record.m_url.page_id;
		forward_url->provice_code = code.province_code;
		forward_url->city_code = code.city_code;
		forward_url->isp_id = code.isp_id;
		memcpy(forward_url->isp, code.isp, ISP_LEN);
		forward_url->test_time = packet->test_time;
		forward_url->count = 1;
		forward_url->v_count = packet->record.m_url.count;
		memcpy(buf + sizeof(protocol_t) + sizeof(proto_url_test_fixed_t), packet->record.m_url.vlist, vlen);
		send_data(url_forward_buf, (len + vlen), proto_as_url_stat_page_time);
	}


	//added by singku 2011-08-30 for url raw data forward----start
	init_proto_head(url_forward_buf, sizeof(protocol_t) + data_len,
			proto_as_url_stat_forward, 0, 0, 0);
	memcpy(url_forward_buf + sizeof(protocol_t), buf, data_len);
	send_data(url_forward_buf, sizeof(protocol_t) + data_len, proto_as_url_stat_forward);
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

int set_cdn_max_speed(int sockfd, char *buf, uint32_t data_len)
{
    int retval = 1;
    uint32_t tmp;

    if (data_len != sizeof(proto_set_cdn_max_speed_t)) {
        ERROR_LOG("set max cdn speed: bad packets len[%u](should be 4)", data_len);
        retval = 1;
    } else if ((tmp = *((uint32_t*)buf)) == CDN_SPEED_INVALID ) {
        ERROR_LOG("set max cdn speed: bad value[%u]", tmp);
        retval = 1;
    } else if (tmp < 10){
        retval = 1;
    } else{
        cdn_max_speed = tmp * 1024;
        DEBUG_LOG("set cdn max speed to %u", cdn_max_speed);
        retval = 0;
    }

    char netbuf[1024];
    init_proto_head(netbuf, sizeof(protocol_t), proto_set_cdn_max_speed, 0, 0, retval);
    net_send(sockfd, netbuf, sizeof(protocol_t));
    do_del_conn(sockfd);
    return retval;
}

int set_cdn_speed_dis(int sockfd, char *buf, uint32_t data_len)
{
    int retval;
    uint32_t dis[4];

    if (data_len != sizeof(proto_set_cdn_speed_dis_t)) {
        ERROR_LOG("set cdn speed dis: bad packets len[%u](should be 16)", data_len);
        retval = -1;
    } else {
        memcpy(dis, buf, data_len);
        if (!(dis[0] < dis[1] && dis[1] < dis[2] && dis[2] < dis[3] && dis[3] <= cdn_max_speed)) {
            ERROR_LOG("set cdn speed dis: invalid value(%u, %u, %u, %u), max:%u", dis[0],dis[1], dis[2], dis[3], cdn_max_speed);
            retval = -1;
        } else {
            cdn_speed_dis[0] = dis[0];
            cdn_speed_dis[1] = dis[1];
            cdn_speed_dis[2] = dis[2];
            cdn_speed_dis[3] = dis[3];
            DEBUG_LOG("set cdn speed dis:%u,%u,%u,%u", dis[0], dis[1], dis[2], dis[3]);
            retval = 0;
        }
    }

    char netbuf[1024];
    init_proto_head(netbuf, sizeof(protocol_t), proto_set_cdn_speed_dis, 0, 0, retval);
    net_send(sockfd, netbuf, sizeof(protocol_t));
    do_del_conn(sockfd);

	return retval;
}
