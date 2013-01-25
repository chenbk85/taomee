#ifndef __FILESERV_LIST_H
#define __FILESERV_LIST_H

#include <libgen.h>
#include <glib.h>
#include <libxml/tree.h>
#include "util.h"

#define FILE_SERV_PORT			6999
#define THUMB_SERV_PORT			7000
#define VARNISH_SERV_PORT			8080
#define VARNISH_SERV_HOST 			"pic1-bus.61.com"

#define MAX_FILE_SERV_ID	256
#define FILE_SERV_LIST		"./file_serv_list.xml"

#define MAX_THUMB_SERV_ID	2
#define THUMB_SERV_LIST		"./thumb_serv_list.xml"



typedef struct file_serv{
	uint32_t	id;
	int 		fd;
	char		eip[16];
	char		iip[16];
	int 		wgh;
}__attribute__ ((packed))file_serv_t;


typedef struct thumb_serv{
	uint32_t	id;
	int 		fd;
	char		eip[16];
	char		iip[16];
	int		start_albumid;
	int 		end_albumid;	
}__attribute__ ((packed))thumb_serv_t;



int load_fslist_config(xmlNodePtr cur_node);
int load_tslist_config(xmlNodePtr cur_node);

int send_to_fileserv(void* buf, int len, int hostid);
int send_to_thumbserv(void* buf, int len, uint32_t key_al_id);

int check_fileserv_fd(int sockfd);
int check_thumbserv_fd(int sockfd);

void connect_to_all_fileserv();
void connect_to_all_thumbserv();


#endif
