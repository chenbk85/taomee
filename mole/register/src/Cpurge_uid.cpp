/*
 * =====================================================================================
 *
 *       Filename:  Cpurge_uid.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/2009 10:16:36 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Miller (Miller), nsnwz@126.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */


extern "C" {
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
}

#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cerrno>
#include "service.h"



/* @brief 把已经使用的米米号覆盖，用最后未使用的米米号覆盖前面已使用的米米号
 */
int del_used_id(void *uid_idx_map, int usr_id_fd, void *usr_id_map)
{
	/* 得到已使用的米米号的个数和总的米米号的个数  */
	uint32_t *user_idx = reinterpret_cast<uint32_t*>(uid_idx_map);
	uint32_t idx = 0, cnt = 0;
	/* 得到已使用的米米号的位置  */
	idx = user_idx[0];
	/* 米米号的总数 */
	cnt = user_idx[1];
	if (idx > cnt) {
		idx = cnt;
	}
	/* 用后面为使用的米米号对前面的米米号覆盖 */
	//uint32_t last_idx = (st.st_size / 4) - 1;
	uint32_t last_idx = cnt - 1;
	uint32_t *user_id = reinterpret_cast<uint32_t*>(usr_id_map);
	for (uint32_t i = 0; i != idx; ++i) {
		user_id[i] = user_id[last_idx - i];
	}
	/* 解除映射 */
	if (munmap(usr_id_map, cnt * 4) == -1) {
	    DEBUG_LOG("munmap error:%s", strerror(errno));
	    return -1;
	}
	//if (ftruncate(usr_id_fd, st.st_size - idx * 4) == -1) {
	if (ftruncate(usr_id_fd, (cnt - idx) * 4) == -1) {
		DEBUG_LOG("ftruncate error:%s", strerror(errno));
		return -1;
	}
	return 0;
}

/* @brief 使未使用的米米号和使用的米米号合并
 * @param unreg_fd 保存未使用米米号的fd
 * @param id_fd 保存使用米米号的fd
 */
int cat_unreg_to_id(int unreg_fd, int id_fd)
{
	if (lseek(unreg_fd, 0, SEEK_SET) == -1) {
		DEBUG_LOG("lseek error : %s", strerror(errno));
		return -1;
	}
	FILE *unreg_file = NULL;
	if ((unreg_file = fdopen(unreg_fd, "r")) == NULL) {
		DEBUG_LOG("fdopen error %s", strerror(errno));
		return -1;
	}
	char buf[30];
	uint32_t temp = 0;
	while(fgets(buf, 20, unreg_file) != NULL) {
		temp = atoll(buf);
		DEBUG_LOG("====write %u", temp);
		if (write(id_fd, &temp, 4) == -1) {
			DEBUG_LOG("write error:%s", strerror(errno));
			return -1;
		}
	}
	/* 文件长度截为零 */
	if (ftruncate(unreg_fd, 0) == -1) {
		DEBUG_LOG("ftruncate error %s", strerror(errno));
		return -1;
	}
	return 0;
}

/* @breif 更新米米号的索引文件 
 * @param uid_idx_fd 米米号的索引fd
 * @param usr_id_fd 保存米米号的fd
 */
int update_idx(void *uid_idx_map, int usr_id_fd)
{
	struct stat st;
	if (fstat(usr_id_fd, &st) == -1) {
		DEBUG_LOG("fstat error:%s", strerror(errno));
		return -1;
	}
	uint32_t *idx = reinterpret_cast<uint32_t*>(uid_idx_map);
	idx[0] = 0;
	idx[1] = st.st_size / 4;
	if (idx[1] < 2000000UL) {
		char buf[8192];
		sprintf(buf, "wget -q  \"http://10.1.1.5/sendsms.php?verify=add6b4bb1b0d2f3f6d859eeab7f1eb4b&telephone=15202115826&message=xxxx7\"");
		system(buf);
	}
	return 0;
}


/* @brief 备份文件
 * @param fd 需要备份的文件
 * @param dst 备份的文件名
 */ 
int copy_file(int fd, char *dst)
{
	struct stat stat;
	int rsize = 0;
	int wsize = 0;
	char buf[5000];
	int dst_fd;
	if (lseek(fd, 0, SEEK_SET) == -1) {
		DEBUG_LOG("lseek error:%s", strerror(errno));
		return -1;
	}
	if (fstat(fd, &stat) == -1) {
		DEBUG_LOG("fstat error:%s", strerror(errno));
		return -1;
	}
	DEBUG_LOG("====%s", dst);
	if ((dst_fd = creat(dst, stat.st_mode)) == -1) {
		DEBUG_LOG("create error:%s", strerror(errno));
		return -1;
	}
	while((rsize = read(fd, buf, 4096))) {
		if (rsize == -1) {
			if (errno == EINTR) {
				continue;
			} else {
				DEBUG_LOG("read error:%s", strerror(errno));
				return -1;
			}
		}
		while((wsize = write(dst_fd, buf, rsize))) {
			if (wsize == -1) {
				if (errno == EINTR) {
					continue;
				} else {
					DEBUG_LOG("write error:%s", strerror(errno));
					return -1;
				}
			}
			rsize -= wsize;
		}
	}
	return 0;
}

/* @brief 得到年月日
 * @param  要得到的对应的秒数
 */
int get_date(time_t t) 
{
	struct tm tm_tmp; 
	localtime_r(&t, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*10000+(tm_tmp.tm_mon+1)*100+tm_tmp.tm_mday;
}
