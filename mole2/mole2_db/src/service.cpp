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
#include "version.h"
#include <libtaomee++/utils/strings.hpp> 

uint32_t log_hex_data_flag;

static  mysql_interface *g_db ;
static  Croute_func 	*route_func ;
Cudp_sender 	* g_p_change_log_udp_sender=NULL;
bool g_is_test_env;
int handle_init (int argc, char **argv, int pid_type)
{
	switch (pid_type) {
		case PROC_MAIN:
			boot_log(0,0,"MOLE2 DB - %s",version_str );
		return 0;
		case PROC_WORK:{

			srandom(getpid());
			//log_hex_data_flag=config_get_intval("LOG_HEX_DATA_FLAG",0 );
	        const char *ip= get_ip_ex(0x01);
	        if ( strncmp( ip,"10.",3 )==0 ) {
	            g_is_test_env=true;
	            DEBUG_LOG("=============TEST ENV TRUE =============");
	        }else{
	            g_is_test_env=false;
	            DEBUG_LOG("=============TEST ENV FALSE =============");
	        }
	
	        log_hex_data_flag=g_is_test_env?1:0;
	

			g_db=new mysql_interface(config_get_strval("DB_IP"),
				config_get_strval("DB_USER"),
				config_get_strval("DB_PASSWD"));
			g_db->set_is_log_debug_sql( config_get_intval("IS_LOG_DEBUG_SQL",0 )  );
			route_func=new Croute_func(g_db);
			g_p_change_log_udp_sender=new Cudp_sender(config_get_strval("CHANGE_LOG_ADDR") );
			return 0;
		}
		case PROC_CONN:
			DEBUG_LOG( "cur VERSION:%s",version_str );
			return 0;

		case 3:
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

	if(log_hex_data_flag) {
		char out[13000];
		bin2hex(out,(char*)buffer,length);
		DEBUG_LOG("XXXXX-----recv-len--[%u][%s]",length,out);
	}

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
	PROTO_HEADER *h = (PROTO_HEADER *)recvbuf;

	KDEBUG_LOG(h->id,"I:%04X:%d", h->cmd_id, h->id );

	if (log_hex_data_flag==1){
		char outbuf[13000];
        bin2hex(outbuf,(char*)recvbuf,rcvlen,500);
        KDEBUG_LOG(h->id,"I[%s]", outbuf );
	}

	int ret= route_func->deal(recvbuf, rcvlen, sendbuf, sndlen);
	if (ret!=SUCC){
		SET_STD_ERR_BUF (sendbuf, sndlen,(PROTO_HEADER *)recvbuf,ret);
	}

	if ( log_hex_data_flag ==1 )
	{
		char outbuf[13000];
		bin2hex(outbuf,(char*)*sendbuf,*sndlen,2000);
		KDEBUG_LOG(h->id,"O[%s]", outbuf );
	}

	KDEBUG_LOG(h->id,"O:%04X:%d:%d",((PROTO_HEADER*)*sendbuf)->cmd_id,ret, *sndlen);
	return SUCC;
}

void handle_fini(int pid_type)
{
	if (pid_type== PROC_WORK){
		delete g_db; 	
		delete route_func; 	
	}
}
/* 
int handle_timer(int* intvl)
{
    *intvl =  1;
	DEBUG_LOG("pid:%d",getpid());
    return 0;
}

*/
int handle_filter_key (const char* buf, int len, uint32_t* key)
{
//   DEBUG_LOG("handle_filter_key ");
    proto_header_t* p_ph=( proto_header_t* ) buf;
    *key=p_ph->id;
    return 0;
}

