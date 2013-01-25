
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
	return net_send(fd, buff, j);
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
	
	return net_send(fd, buff, j);
}


