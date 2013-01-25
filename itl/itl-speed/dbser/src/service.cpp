#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "service.h"
#include "libtaomee++/utils/strings.hpp"
#include "mysql_iface.h"
#include "Croute_func.h"
#include "version.h"

uint32_t log_hex_data_flag;
static  mysql_interface *g_db ;
static  Croute_func 	*route_func ;



uint32_t ip_history_update_count_per_second=0;
int handle_init (int argc, char **argv, int pid_type)
{

	route_func=NULL;
	switch (pid_type) {
		case PROC_MAIN:
			boot_log(0,0,"DuDu cur  VERSION:%s",version_str );
		return 0;
		case PROC_WORK:
			srandom(getpid());
			log_hex_data_flag=config_get_intval("LOG_HEX_DATA_FLAG",0 );
			g_db=new mysql_interface(config_get_strval("DB_IP"),
				config_get_strval("DB_USER"),
				config_get_strval("DB_PASSWD"),
				config_get_intval("DB_PORT",3306 ),
				config_get_strval("DB_UNIX_SOCKET"),
                "utf8");
			g_db->set_is_log_debug_sql( config_get_intval("IS_LOG_DEBUG_SQL",0 )  );
			g_db->set_is_only_exec_select_sql( config_get_intval("IS_ONLY_EXEC_SELECT_SQL",0 )  );
			route_func=new Croute_func(g_db);
			
			return 0;
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
	//DEBUG_LOG("XXXXX-----recv-len--[%u]",length);
	if (length>=4 )	
	{
		reallen=*((uint32_t* )buffer);
		if (reallen<=8192) {
			return reallen;
		}else{
			return -1;
		}
	}else return 0;

}

int handle_process (char *recvbuf, int rcvlen, char **sendbuf, int *sndlen, const skinfo_t *sk)
{

	if (log_hex_data_flag==1){
		char outbuf[13000];
        bin2hex(outbuf,(char*)recvbuf,rcvlen,500);
        DEBUG_LOG("I[%s]", outbuf );
	}



	int ret= route_func->deal(recvbuf, rcvlen, sendbuf, sndlen);
	if (ret!=SUCC){
		//当处理出错时,标准返回  没有私有域
		SET_STD_ERR_BUF (sendbuf, sndlen,(PROTO_HEADER *)recvbuf,ret);
	}

	if ( log_hex_data_flag ==1 )
	{
		char outbuf[13000];
		bin2hex(outbuf,(char*)*sendbuf,*sndlen  ,500 );
		DEBUG_LOG("O[%s]", outbuf );
	}

	DEBUG_LOG("O:%d:%d",ret, *sndlen);
	return SUCC;
}

void handle_fini(int pid_type)
{
	if (pid_type== PROC_WORK){
		delete route_func; 	
		delete g_db;
	}
}

int handle_timer(int* intvl)
{
    return 0;
}


