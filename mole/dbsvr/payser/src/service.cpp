#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<time.h>
#include "service.h"
#include "Croute_func.h"

static  mysql_interface *db ;
static  Croute_func *route_func ;

int handle_init (int argc, char **argv, int pid_type)
{
	switch (pid_type) {
		case PROC_MAIN:
		return 0;
		case PROC_WORK:
		DEBUG_LOG("work.......");
		db=new mysql_interface(config_get_strval("DB_IP"),
				config_get_strval("DB_USER"),
				config_get_strval("DB_PASSWD"));
		route_func=new Croute_func( config_get_intval("ITEM_SHM_KEY",0)  ,
			   	config_get_intval("SYNC_USER_MSG_KEY",0) ,db ,
				config_get_strval("SERIAL_IP"),
				config_get_intval("SERIAL_PORT",0)
				);
			return 0;
		case PROC_CONN:
			return 0;
		default:
			ERROR_LOG ("invalid pid_type=%d", pid_type);
			return -1;
	}
}


int handle_input (const char* buffer, int length, const skinfo_t *sk)
{
		int reallen;
		if (length>=4 )	
		{
			reallen=*((uint32_t* )buffer);
			if (reallen<=4096) {
				return reallen;
			}else{
				return -1;
			}
		}else return 0;
}

int handle_process (char *recvbuf, int rcvlen, char **sendbuf, int *sndlen, const skinfo_t *sk)
{
	int ret; 
	char outbuf[5000];

	userid_t id;
	short cmd_id=0;				
	cmd_id=((PROTO_HEADER *) recvbuf)->cmd_id;
	id=((PROTO_HEADER *) recvbuf)->id;
	//if (cmd_id &  NEED_SAVE_LOG)
	{
		ASC2HEX_3(outbuf,recvbuf,rcvlen);
		INFO_LOG("recv[%X][%u][%u][%u][%s]",
			cmd_id,rcvlen,id,((PROTO_HEADER *) recvbuf)->result,outbuf );		
	}

	ret=route_func->deal(recvbuf, rcvlen, sendbuf, sndlen);
	if (ret!=SUCC){
		//出错返回
		SET_STD_ERR_BUF(sendbuf, sndlen,(PROTO_HEADER *)recvbuf,ret);
	}

	{
		ASC2HEX_3(outbuf,*sendbuf,*sndlen);
		INFO_LOG("send[%X][%u][%u][%u][%s]\n", cmd_id,*sndlen,id, 
				((PROTO_HEADER *)*sendbuf)->result, outbuf );		
	}

	return SUCC;
}

void handle_fini(int pid_type)
{
	if (pid_type== PROC_WORK)
	{
		delete db; 	
		delete route_func; 	
	}

}
