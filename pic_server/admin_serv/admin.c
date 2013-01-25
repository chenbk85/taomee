
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <libtaomee/conf_parser/config.h>

#include <iter_serv/net_if.h>
#include "proto.h"
#include "util.h"
#include "lloccode.h"
#include "admin.h"


int send_ok_to_php(int fd, uint16_t cmd, uint32_t userid)
{
	int buff[256] = {0};
	int j =  sizeof(protocol_t);
	init_proto_head(buff, userid, cmd, j);
	net_send(fd, buff, j);
	return 0;
}

int send_err_to_php(int fd, uint16_t cmd, uint32_t userid, int err)
{
	int buff[256] = {0};
	int j =  sizeof(protocol_t);
	protocol_t* p = (protocol_t*)buff;
	p->len = j;
    p->cmd = cmd;
    p->id  = userid;
    p->ret = err;
	
	net_send(fd, buff, j);
	return 0;
}


#if 0
int do_delete_file(uint32_t userid, int key_album, char alloccode[LLOCCODE_LEN])
{
	uint32_t hostid = 0, uid = 0;
	uint8_t type = 0, branchtype = 0;
	char date[9] = {0}, dir[4] = {0}, pic[4] = {0};
	uint32_t thumbid[MAX_THUMB_CNT_PER_PIC] = {0};
	int pathlen = 0;
	char part_file_path[PART_MAX_PATH_LEN];
	sscanf(alloccode, "%02X%c%c%8s%3s%3s%010u%08X%08X%08X%08X",
			&hostid, &type, &branchtype, date, dir, pic, &uid, 
			&thumbid[0], &thumbid[1], &thumbid[2], &thumbid[3]);

	if (type != '1') { // only for picture now
		ERROR_LOG("invalid lloccode type\t[%u %u %s]", userid, type, alloccode);
		return -1;
	}
	char suffix[16];
	get_pic_suffix(branchtype, suffix);
	pathlen = snprintf(part_file_path, PART_MAX_PATH_LEN, "%c/%s/%s/%s%s", type, 
		date, dir, pic, suffix);

	uint8_t buff[512];
	int k = sizeof(protocol_t);
	PKG_H_UINT32(buff, hostid, k);
	PKG_H_UINT32(buff, key_album, k);
	PKG_STR(buff, part_file_path, k, PART_MAX_PATH_LEN);
	int label_k = k; k += 4;
	int lp, thumb_cnt = 0;
	for(lp = 0; lp < MAX_THUMB_CNT_PER_PIC; lp++){
		if (thumbid[lp]) {
			PKG_H_UINT32(buff, thumbid[lp], k);
			thumb_cnt++;
		}
	}
	PKG_H_UINT32(buff, thumb_cnt, label_k);
	DEBUG_LOG("DEL FILE[%u %u %s %u]", hostid, key_album, part_file_path, thumb_cnt);
	init_proto_head(buff, userid, proto_fs_delete_file, k);
	return send_to_fileserv(buff, k, hostid);
}

int delete_file(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN(pkglen, 68);
	char lloccode[64] = {0};
	uint32_t	key_album;
	int 		j = 0;
	UNPKG_H_UINT32(pkg, key_album, j);
	UNPKG_STR(pkg, lloccode, j, LLOCCODE_LEN);

	if (do_delete_file(userid, key_album, lloccode) == -1) {
		ERROR_RETURN(("cannot delete file\t[%u %u %u]", userid, key_album, lloccode), -1);
	}
	send_ok_to_php(fd, cmd, userid);
	return 0;
}

int delete_file_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	return 0;
}
#endif


