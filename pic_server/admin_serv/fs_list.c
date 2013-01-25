#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <libtaomee/conf_parser/config.h>

#include <iter_serv/net_if.h>
#include "proto.h"
#include "util.h"
#include "fs_list.h"

static int max_fsrv_id = 0;
static file_serv_t g_file_servs[MAX_FILE_SERV_ID];

static int max_tsrv_id = 0;
static thumb_serv_t g_thumb_servs[MAX_THUMB_SERV_ID];



int load_fslist_config(xmlNodePtr cur_node)
{
	int srvid = 0;
	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar*)"Srv")) {
			DECODE_XML_PROP_INT(srvid, cur_node, "ID");
			if (srvid > MAX_FILE_SERV_ID) {
				ERROR_RETURN(("server id too large\t[%u]", srvid), -1);
			}
			if (g_file_servs[srvid - 1].id) {
				ERROR_RETURN(("server id dup\t[%u]", srvid), -1);
			}
			g_file_servs[srvid - 1].id = srvid;
			DECODE_XML_PROP_STR(g_file_servs[srvid - 1].eip, cur_node, "Eip");
			DECODE_XML_PROP_STR(g_file_servs[srvid - 1].iip, cur_node, "Iip");
			g_file_servs[srvid - 1].fd = -1;
			max_fsrv_id = (max_fsrv_id < srvid) ? srvid : max_fsrv_id;
		}
		cur_node = cur_node->next;
	}
	return 0;
}

int get_fs_id(char* ip)
{
	int lp;
	for (lp = 0; lp < MAX_FILE_SERV_ID; lp++) {
		if (!strcmp(g_file_servs[lp].iip, ip)) {
			return g_file_servs[lp].id;
		}
	}
	return -1;
}

int load_tslist_config(xmlNodePtr cur_node)
{
	int srvid = 0;
	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar*)"Srv")) {
			DECODE_XML_PROP_INT(srvid, cur_node, "ID");
			if (srvid > MAX_FILE_SERV_ID) {
				ERROR_RETURN(("server id too large\t[%u]", srvid), -1);
			}
			if (g_thumb_servs[srvid - 1].id) {
				ERROR_RETURN(("server id dup\t[%u]", srvid), -1);
			}
			g_thumb_servs[srvid - 1].id = srvid;
			DECODE_XML_PROP_STR(g_thumb_servs[srvid - 1].eip, cur_node, "Eip");
			DECODE_XML_PROP_STR(g_thumb_servs[srvid - 1].iip, cur_node, "Iip");
			DECODE_XML_PROP_INT(g_thumb_servs[srvid - 1].start_albumid, cur_node, "StartID");
			DECODE_XML_PROP_INT(g_thumb_servs[srvid - 1].end_albumid, cur_node, "EndID");
			g_thumb_servs[srvid - 1].fd = -1;
			max_tsrv_id = (max_tsrv_id < srvid) ? srvid : max_tsrv_id;
		}
		cur_node = cur_node->next;
	}
	return 0;
}





int send_to_fileserv(void* buf, int len, int hostid)
{
	if (hostid < 1 || hostid > MAX_FILE_SERV_ID) {
		ERROR_RETURN(("bad hostid\t[%u]", hostid), -1);
	}
	int idx = hostid - 1;
	if (g_file_servs[idx].fd == -1) {
		g_file_servs[idx].fd = connect_to_svr(g_file_servs[idx].iip, FILE_SERV_PORT, 8*1024, 1);
		if (g_file_servs[idx].fd == -1) {
			ERROR_RETURN(("cannot connect to fileserv\t[%u]", hostid), -1);
		}
	}
	net_send(g_file_servs[idx].fd, buf, len);
	return 0;
}

int send_to_thumbserv(void* buf, int len, uint32_t key_al_id)
{
	int idx = -1;
	int lp;
	for (lp = 0; lp < MAX_THUMB_SERV_ID; lp++) {
		if (key_al_id < g_thumb_servs[lp].end_albumid) {
			idx = lp;
			break;
		}
	}
	if (idx == -1) {
		ERROR_RETURN(("please add thumb servers\t[key=%u]", key_al_id), -1);
	}
	if (g_thumb_servs[idx].fd == -1) {
		g_thumb_servs[idx].fd = connect_to_svr(g_thumb_servs[idx].iip, THUMB_SERV_PORT, 8*1024, 1);
		if (g_thumb_servs[idx].fd == -1) {
			ERROR_RETURN(("cannot connect to thumb\t[%u %u]", idx, key_al_id), -1);
		}
	}
	return net_send(g_thumb_servs[idx].fd, buf, len);
}

int check_fileserv_fd(int sockfd) 
{
	int lp;
	for (lp = 0; lp < MAX_FILE_SERV_ID; lp++) {
		if (sockfd == g_file_servs[lp].fd) {
			g_file_servs[lp].fd = -1;
			return 1;
		}
	}
	return 0;
}

int check_thumbserv_fd(int sockfd) 
{
	int lp;
	for (lp = 0; lp < MAX_THUMB_SERV_ID; lp++) {
		if (sockfd == g_thumb_servs[lp].fd) {
			g_thumb_servs[lp].fd = -1;
			return 1;
		}
	}
	return 0;
}


void connect_to_all_fileserv()
{
	int lop;
	for (lop = 0; lop < MAX_FILE_SERV_ID; lop++) {
		if (g_file_servs[lop].id) {
			g_file_servs[lop].fd = connect_to_svr(g_file_servs[lop].iip, FILE_SERV_PORT, 8*1024, 1);
		}
	}
}

void connect_to_all_thumbserv()
{
	int lop;
	for (lop = 0; lop < MAX_THUMB_SERV_ID; lop++) {
		if (g_thumb_servs[lop].id) {
			g_thumb_servs[lop].fd = connect_to_svr(g_thumb_servs[lop].iip, THUMB_SERV_PORT, 8*1024, 1);
		}
	}
}


