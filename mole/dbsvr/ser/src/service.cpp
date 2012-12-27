#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "service.h"
#include "mysql_iface.h"
#include "Croute_func.h"
#include "proto.h"
#include "tcpip.h"
#include "version.h"
#include "Cclientproto.h"

static  mysql_interface *g_db ;
static  Cclientproto 	*g_cp;
static  Croute_func 	*route_func ;
const skinfo_t *g_sk;

int handle_init (int argc, char **argv, int pid_type)
{
	switch (pid_type) {
		case PROC_MAIN:
			boot_log(0,0,"cur VERSION:%s",version_str );
		return 0;
		case PROC_WORK:
			srandom(getpid());
			g_db=new mysql_interface(config_get_strval("DB_IP"),
				config_get_strval("DB_USER"),
				config_get_strval("DB_PASSWD"));
			g_db->set_is_log_debug_sql( config_get_intval("IS_LOG_DEBUG_SQL",0 )  );

			g_cp=new Cclientproto(config_get_strval("ROUTEDB_IP"),
					config_get_intval("ROUTEDB_PORT",0 ) );

			route_func=new Croute_func(g_db,g_cp);
			
			return 0;
		case PROC_CONN:
			DEBUG_LOG( "cur VERSION:%s",version_str );
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
	g_sk=sk;
	*sndlen=0;
	int ret= route_func->deal(recvbuf, rcvlen, sendbuf, sndlen);
	if (ret!=SUCC){
		//当处理出错时,标准返回  没有私有域
		SET_STD_ERR_BUF (sendbuf, sndlen,(PROTO_HEADER *)recvbuf,ret);
	}else{
		if ( *sndlen==0 ){
			SET_STD_ERR_BUF (sendbuf, sndlen,(PROTO_HEADER *)recvbuf,0);
		}
	}

	DEBUG_LOG("O:%04X:%u:%d:%d",((PROTO_HEADER *)recvbuf)->cmd_id, ((PROTO_HEADER *)recvbuf)->id ,ret,*sndlen);
	return SUCC;
}

void handle_fini(int pid_type)
{
	if (pid_type== PROC_WORK){
		delete g_db; 	
		delete route_func; 	
	}
}

int handle_filter_key (const char* buf, int len, uint32_t* key)
{
    DEBUG_LOG("handle_filter_key ");
    proto_header* p_ph=( proto_header* ) buf;
    *key=p_ph->id;

    return 0;
}
