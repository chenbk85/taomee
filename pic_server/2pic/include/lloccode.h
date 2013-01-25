#ifndef _LLOCCODE_H
#define _LLOCCODE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <libtaomee/log.h>
#include <libtaomee/crypt/qdes.h>
#include "common.h"


#define LLOCCODE_LEN			64
#define MAX_TRANS_NUM			16
#define DIR_PIC_SESS_LEN		16
#define FILESERV_DES_KEY 		"FiL:sRv$"
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
get_type_from_suffix(char* asuffix, int* type, int* branch_type)
{
        if(!strcmp(asuffix, ".JPEG")) {
		*type = 1;
		*branch_type = 1;
                return 0;
	}
	else if(!strcmp(asuffix, ".PNG")) {
		*type = 1;
		*branch_type = 2;
                return 0;
	}
	else if(!strcmp(asuffix, ".GIF")) {
		*type = 1;
		*branch_type = 3;
                return 0;
	}
                
	return -1;
}
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
create_dirpicsess_picid(char adps[DIR_PIC_SESS_LEN + 1], uint32_t dir_id, uint32_t* pic_id)
{
	dir_id = dir_id << 16 | (rand()%65535);	
	*pic_id = (*pic_id) << 16 | (rand()%65535);
	
	uint8_t inbuf[8], outbuf[8];

	(*(uint32_t*)&inbuf) = dir_id;
	(*(uint32_t*)(inbuf + 4)) = *pic_id;
	des_encrypt(FILESERV_DES_KEY, inbuf, outbuf);
	int loop;
	for (loop = 0; loop < DIR_PIC_SESS_LEN / 2; loop++) {
		sprintf(&adps[loop*2], "%02X", outbuf[loop]);
	}
	DEBUG_LOG("dir_pic_sess\t[%s %u %u]", adps, dir_id, *pic_id);
}

static inline void
get_dir_pic_from_sess(char asess[DIR_PIC_SESS_LEN + 1], uint32_t* dir_id, uint32_t* pic_id)
{
	uint8_t inbuf[8], outbuf[8];
	uint32_t tmp;
	
	int loop;
	for (loop = 0; loop < DIR_PIC_SESS_LEN / 2; loop++) {
		sscanf(&asess[loop*2], "%2X", &tmp);
		inbuf[loop] = tmp;
	}

	des_decrypt(FILESERV_DES_KEY, inbuf, outbuf);
	*dir_id = *(uint32_t*)outbuf;
	*dir_id = (*dir_id) >> 16;
	*pic_id = *(uint32_t*)(outbuf + 4);
	DEBUG_LOG("DECRYPT\t[%s dir=%u pic=%u]", asess, *dir_id, *pic_id);
}

static inline int
analyse_lloccode_ex(char alloccode[LLOCCODE_LEN + 1], char path[MAX_PATH_LEN], int* path_len, int* f_type, int* b_type)
{
	uint32_t dir_id = 0, pic_id = 0;	
	char date[9] = {0}, sess[DIR_PIC_SESS_LEN + 1] = {0}, ext[10] = {0};
	sscanf(alloccode, "%9s%16s%s", date, sess, ext);

	if (f_type && b_type) {
		if(get_type_from_suffix(ext, f_type, b_type) == -1) {
			ERROR_RETURN(("invalid lloccode suffix\t[%s %s]", alloccode, ext), -1);
		}
	}

	int len = 0;	
	get_dir_pic_from_sess(sess, &dir_id, &pic_id);
	if (pic_id <= MAX_PIC_ID) {
		len = snprintf(path, MAX_PATH_LEN, "%s/%u/%s%03u/%03u%s", file_root, *f_type, 
		date, dir_id, pic_id, ext);
	}
	else {
		len = snprintf(path, MAX_PATH_LEN, "%s/%u/%s%03u/%u%s", file_root, *f_type, 
		date, dir_id, pic_id, ext);
	}

	if (path_len) *path_len = len;
	DEBUG_LOG("PATH:%s", path);
	
	return 0;
	
}

static inline int
trans_old_llocc_to_new(char old_lloc[LLOCCODE_LEN + 1], char new_lloc[LLOCCODE_LEN + 1])
{

        uint32_t uid = 0, hostid;
        char date[9] = {0};
	uint32_t dir, pic;
	uint32_t  thumbids[4] = {0};
	int  	f_type, b_type;

        sscanf(old_lloc, "%02X%1u%1u%8s%3u%3u%010u%08X%08X%08X%08X",
                        &hostid, &f_type, &b_type, date, &dir, &pic, &uid,
                        &thumbids[0], &thumbids[1], &thumbids[2], &thumbids[3]);

        if (f_type != 1) { // only for picture now
                return -1;
        }

	char ext[10] = {0};
	get_pic_suffix_int(b_type, ext);


	uint32_t dir_id, pic_id;
	dir_id = dir << 16 | (rand()%65535);
        pic_id = pic;

        uint8_t inbuf[8], outbuf[8];

        (*(uint32_t*)&inbuf) = dir_id;
        (*(uint32_t*)(inbuf + 4)) = pic_id;
        des_encrypt(FILESERV_DES_KEY, inbuf, outbuf);
	char sess[DIR_PIC_SESS_LEN + 1] = {0};
        int loop;
        for (loop = 0; loop < DIR_PIC_SESS_LEN / 2; loop++) {
                sprintf(&sess[loop*2], "%02X", outbuf[loop]);
        }
        DEBUG_LOG("dir_pic_sess\t[%s %u %u]", sess, dir_id, pic_id);

	snprintf(new_lloc, LLOCCODE_LEN, "%s/%s%s", date, sess, ext);
	DEBUG_LOG("NEW LLOC:%s", new_lloc);
	
	return 0;
}
	
#endif
