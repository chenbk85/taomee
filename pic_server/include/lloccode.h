#ifndef _LLOCCODE_H
#define _LLOCCODE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <libtaomee/log.h>

#include "common.h"


#define LLOCCODE_LEN			64
#define MAX_TRANS_NUM			16
#define GET_PIC_URL_LEN		175
#define file_root   			"/opt/taomee/file_root"

#if 0
static inline int
get_pic_suffix(char branch_type, char asuffix[16])
{
	memset(asuffix, 0, sizeof(asuffix));
	switch(branch_type){
	case '1':		
		memcpy(asuffix, ".JPEG", 5);
		return 0;
	case '2':
		memcpy(asuffix, ".PNG", 4);
		return 0;
	case '3':
		memcpy(asuffix, ".GIF", 4);
		return 0;
	default:
		memcpy(asuffix, ".JPEG", 5);
		return 0;
	}
}
#endif

static inline uint16_t
lloccode_checksum(uint8_t* buf, int len)
{
	uint16_t	sum = 0;
	for ( ; len > 0; len--) {
		sum += *buf++;
	}
	sum = (sum >> 8) + (sum && 0xff);
	sum += (sum >> 8);
	return ~sum;
}

static inline void
create_lloccode(char alloccode[LLOCCODE_LEN + 1], uint32_t thmbid0, uint32_t thmbid1, uint32_t thmbid2, uint32_t thmbid3)
{
	int len = 2 + 1 + 1 + 8 + 3 + 3 + 10;
	char* str_begin = len + alloccode;
	snprintf(str_begin, LLOCCODE_LEN - len - 3, "%08X%08X%08X%08X",
		thmbid0, thmbid1, thmbid2, thmbid3);

	uint16_t chksum = lloccode_checksum((uint8_t*)(alloccode), LLOCCODE_LEN - 4);
	uint8_t	 short_chksum = chksum;
	DEBUG_LOG("CHKSUM\t[%x %x]", chksum, short_chksum);
	snprintf(alloccode + LLOCCODE_LEN - 4, 5, "%02X%02u", short_chksum, 0);
	DEBUG_LOG("LLOCCODE\t[%s]", alloccode);
}
#if 0
static inline int
analyse_lloccode(char alloccode[LLOCCODE_LEN], int* hostid, uint32_t thumbids[MAX_THUMB_CNT_PER_PIC], uint32_t* thumb_cnt, char path[MAX_PATH_LEN], int* path_len)
{
	uint32_t uid = 0;
	char type = 0, branchtype = 0;
	char date[9] = {0}, dir[4] = {0}, pic[4] = {0};
	sscanf(alloccode, "%02X%c%c%8s%3s%3s%010u%08X%08X%08X%08X",
			hostid, &type, &branchtype, date, dir, pic, &uid, 
			&thumbids[0], &thumbids[1], &thumbids[2], &thumbids[3]);

	if (type != '1') { // only for picture now
		ERROR_LOG("invalid lloccode type\t[%u %s]", type, alloccode);
		return -1;
	}
	char suffix[16];
	get_pic_suffix(branchtype, suffix);
	*path_len = snprintf(path, MAX_PATH_LEN, "%c/%s/%s/%s%s", type, 
		date, dir, pic, suffix);
	
	int lp;
	for (lp = 0; lp < MAX_THUMB_CNT_PER_PIC; lp++) {
		if (thumbids[lp]) (*thumb_cnt)++;
	}
	
	return 0;
}
#endif

static inline int
get_pic_suffix_int(int branch_type, char asuffix[16])
{
        memset(asuffix, 0, sizeof(asuffix));
        switch(branch_type){
        case 1:
                memcpy(asuffix, ".JPEG", 5);
                return 0;
        case 2:
                memcpy(asuffix, ".PNG", 4);
                return 0;
        case 3:
                memcpy(asuffix, ".GIF", 4);
                return 0;
        default:
                memcpy(asuffix, ".JPEG", 5);
                return 0;
        }
}


static inline int
analyse_lloccode_ex(char alloccode[LLOCCODE_LEN], int* hostid, uint32_t thumbids[MAX_THUMB_CNT_PER_PIC], uint32_t* thumb_cnt, char path[MAX_PATH_LEN], int* path_len, int* type, int* branchtype)
{
	uint32_t uid = 0;
	char date[9] = {0}, dir[4] = {0}, pic[4] = {0};
	sscanf(alloccode, "%02X%1u%1u%8s%3s%3s%010u%08X%08X%08X%08X",
			hostid, type, branchtype, date, dir, pic, &uid, 
			&thumbids[0], &thumbids[1], &thumbids[2], &thumbids[3]);

	if (*type != 1) { // only for picture now
		ERROR_LOG("invalid lloccode type\t[%u %s]", type, alloccode);
		return -1;
	}
	char suffix[16] = {0};
	get_pic_suffix_int(*branchtype, suffix);
	*path_len = snprintf(path, MAX_PATH_LEN, "%s/%u/%s/%s/%s%s", file_root, *type, 
		date, dir, pic, suffix);

	DEBUG_LOG("path:%s %s", path, suffix);
	
	int lp;
	for (lp = 0; lp < MAX_THUMB_CNT_PER_PIC; lp++) {
		if (thumbids[lp]) (*thumb_cnt)++;
	}

	return 0;
	
}

#endif
