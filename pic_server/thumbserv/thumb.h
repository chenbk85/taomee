#ifndef __THUMB_H
#define __THUMB_H

#include <gd.h>

#include "common.h"
#include "lloccode.h"

#define SERV_PORT 10001
#define TSMD_PROCS_NUM 10

#define MAX_FIEL_PATH 256
#define MAX_THUMB_SIZE	2 * 1024 * 1024

#define PHOTOS_ROOT_PATH "/opt/taomee/photofs/data"
#define PHOTOS_FIEL_PRE "photofs"



typedef struct _thumb {
	//make thumb, later will be sent to thumb server
	uint32_t	thumb_id;
	int			thumb_len;
	char 		thumb_data[MAX_THUMB_SIZE];
}__attribute__((packed))thumb_t;

typedef struct _up_thumb {
	uint32_t	u_key;
	uint32_t	key_albumid;
	int			thumb_cnt;
	thumb_t     stthumb[MAX_THUMB_CNT_PER_PIC];
}__attribute__((packed))up_thumb_t;

typedef struct _thumb_ids {
	int32_t		thumb_cnt;
	uint32_t	thumb_id[MAX_THUMB_CNT_PER_PIC];
}__attribute__((packed))thumb_ids_t;

typedef struct _trans_multi_thumbs_id {
	int32_t				file_cnt;
	thumb_ids_t 		sti[MAX_TRANS_NUM];
}__attribute__((packed))trans_multi_thumbs_id_t;

typedef struct _del_multi_thumbs_id {
	int32_t				file_cnt;
	thumb_ids_t 		sti[MAX_PIC_NUM_PER_ABLUM];
}__attribute__((packed))del_multi_thumbs_id_t;

int init_mmaps();
int save_photo(unsigned int iAlbumId,unsigned int iPhotosLen, char *pPhotosBuf);
int del_photo(unsigned int iAlbumId,unsigned int iPhotosId);
int read_photo(unsigned int iAlbumId,unsigned int iPhotosId,char *pPhotosBuf);
//int ReadPhotosForQhttp(unsigned int iAlbumId,unsigned int iPhotosId,char *pPhotosBuf);
int upload_thumb_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen);
int get_album_thumbs_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen);
int get_thumb_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen);
int del_thumbs_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen);
int del_album_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen);

int trans_multi_get_thumbs_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen);
int trans_multi_add_thumbs_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen);
int trans_multi_del_thumbs_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen);
int change_thumb_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen);


#endif

