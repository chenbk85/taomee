#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <iter_serv/net_if.h>
#include <iter_serv/net.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>

#include "util.h"
#include "proto.h"
#include "lloccode.h"
#include "fs_list.h"
#include "admin.h"
#include "change_file_attr.h"


static GHashTable* change_file_attr_maps;

//------------------------------------------------------
static inline void
free_change_file_attr(void* atm)
{
	change_file_attr_t* p = atm;
	DEBUG_LOG("Free trans Multi: [fd=%u action=%u lloccode=%s]", 
		p->fd, p->action,p->lloccode);
	g_slice_free1(sizeof(change_file_attr_t), atm);
}

void init_change_file_attr_hash()
{
	change_file_attr_maps = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_change_file_attr);
}

void fini_change_file_attr_hash()
{
	g_hash_table_destroy(change_file_attr_maps);
}

int get_change_file_attr_hash_size()
{
	return g_hash_table_size(change_file_attr_maps);
}

void sendto_all_varnish(const char* url)
{
	char* bind_net_varnish_ip = config_get_strval("bind_net_varnish_ip");
	char* bind_tele_varnish_ip = config_get_strval("bind_tele_varnish_ip");
	int net_fd,tele_fd;
	char buff[512] = {0};
	int bufflen =0;
	DEBUG_LOG("SEND varnish url[%s]",url);	
	if(bind_net_varnish_ip &&bind_tele_varnish_ip){
		net_fd = safe_tcp_connect(bind_net_varnish_ip, VARNISH_SERV_PORT, 1, 0);
		tele_fd = safe_tcp_connect(bind_tele_varnish_ip, VARNISH_SERV_PORT, 1, 0);
		if(net_fd && tele_fd){
			bufflen = snprintf(buff,sizeof(buff),"PURGE %s HTTP/1.1\r\nHost:%s\r\nConnection: close\r\n\r\n",
		(char*)url,VARNISH_SERV_HOST);
			DEBUG_LOG("SEND varnish buf[%s]",buff);
			if(safe_tcp_send_n(net_fd, buff, bufflen) == bufflen 
			&&safe_tcp_send_n(tele_fd, buff, bufflen) == bufflen ){
				DEBUG_LOG("SEND VARNISH SUCC");
				close(net_fd);
				close(tele_fd);
				return ;
			}
			ERROR_LOG("SENDTO VARNISH ERROR len error bufflen[%d]",bufflen);
		}
			ERROR_LOG("CONNECTTO VARNISH ERROR netfd[%u] telefd[%u]",net_fd,tele_fd);
	}
	ERROR_LOG("READ varnish ip error");
	return ;
}

int change_file_attr_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen) 
{
	CHECK_BODY_LEN(pkglen, 8);
	int ret = 0,j = 0,user_fd = 0;
	UNPKG_H_UINT32(pkg, user_fd, j);
	UNPKG_H_UINT32(pkg, ret, j);
	DEBUG_LOG("RECV FROM FILESERV [user_fd = %u ret = %u]",user_fd,ret);
	change_file_attr_t* lpdm  = g_hash_table_lookup(change_file_attr_maps, &user_fd);
	if (!lpdm) {
		ERROR_RETURN(("not find fd\t[%u %u %u]", userid, cmd, user_fd), -1);
	}
	if(ret){
		send_err_to_php(lpdm->fd, cmd, userid, ret);
	}else{
		send_ok_to_php(lpdm->fd,cmd,userid);
		sendto_all_varnish(lpdm->url);
	}
	DEBUG_LOG("lpdm->fd = %u",lpdm->fd);
	do_del_conn(lpdm->fd);
	g_hash_table_remove(change_file_attr_maps, &lpdm->fd);
	return 0;
}

int change_file_attr(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen) 
{
	CHECK_BODY_LEN_GE(pkglen, 4+LLOCCODE_LEN+GET_PIC_URL_LEN);
	change_file_attr_t* lptm = (change_file_attr_t*)g_slice_alloc0(sizeof(change_file_attr_t));
	if (!lptm) {
		ERROR_RETURN(("not find fd\t[%u %u %u]", userid, cmd, fd), -1);
	}
	lptm->fd = fd;
	lptm->userid = userid;
	lptm->cmd = cmd;
	int j = 0;
	UNPKG_H_UINT32(pkg, lptm->action, j);
	UNPKG_STR(pkg, lptm->lloccode, j, LLOCCODE_LEN);
	UNPKG_STR(pkg, lptm->url, j, GET_PIC_URL_LEN);	
	change_file_info_t *lpdf = &lptm->file_info;
	if (analyse_lloccode_ex(lptm->lloccode, &lpdf->hostid, lpdf->thumb_id, 
		&lpdf->thumb_cnt, lpdf->file_path, &lpdf->path_len, &lpdf->f_type, 
		&lpdf->b_type) == -1) {
		ERROR_LOG("cannot chomd file\t[%u %u %s]", userid, lptm->action, lptm->lloccode);
		g_slice_free1(sizeof(change_file_attr_t), lptm);
		return -1;
	}

	char buff[256] = {0};
	int k = sizeof(protocol_t);
	PKG_H_UINT32(buff, lptm->fd, k);
	PKG_H_UINT32(buff, lptm->action, k);
	PKG_STR(buff, lpdf->file_path, k, lpdf->path_len);
	DEBUG_LOG("ADMIN SEND FILESERV[fd=%u  action=%u path=%s,len = %u]",lptm->fd,lptm->action,lpdf->file_path,lpdf->path_len);
	init_proto_head(buff, userid, proto_fs_change_file_attr, k);

	if(-1 == send_to_fileserv(buff, k, lpdf->hostid)){
		send_err_to_php(lptm->fd, lptm->cmd, lptm->userid, err_fileserv_net_err);
		g_slice_free1(sizeof(change_file_attr_t), lptm);
		return -1;
	}
	g_hash_table_replace(change_file_attr_maps, &(lptm->fd), lptm);
	return 0;
}

