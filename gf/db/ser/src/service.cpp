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
#include "gf_common.h"
#include <libtaomee++/utils/strings.hpp> 
uint32_t log_hex_data_flag;
static  mysql_interface *g_db ;
static  Cclientproto    *g_cp;
static  Croute_func 	*route_func ;

int handle_init (int argc, char **argv, int pid_type)
{
	switch (pid_type) {
		case PROC_MAIN:
			boot_log(0,0,"gf cur  VERSION:%s",version_str );
		return 0;
		case PROC_WORK:
			srandom(getpid());
            log_hex_data_flag=config_get_intval("LOG_HEX_DATA_FLAG",0 );
			g_db=new mysql_interface(config_get_strval("DB_IP"),
				config_get_strval("DB_USER"),
				config_get_strval("DB_PASSWD"),
				config_get_intval("DB_PORT",3306),
                config_get_strval("DB_UNIX_SOCKET")
                );
            g_cp=new Cclientproto(config_get_strval("ROUTEDB_IP"),
                config_get_intval("ROUTEDB_PORT",0 )
                );

			g_db->set_is_log_debug_sql( config_get_intval("IS_LOG_DEBUG_SQL",0 )  );
			route_func=new Croute_func(g_db, g_cp);
			
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
//	DEBUG_LOG("XXXXX-----recv-len--[%u]",length);
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

void hex_printf(char *buf,int len)
{
	
	char tmpbuf[PROTO_MAX_SIZE] = "";
	char *pcur = tmpbuf;
	if (len > (PROTO_MAX_SIZE/3-100))
		return;
	for (int i=0;i<len;i++)
	{
		sprintf(pcur,"%02x ",buf[i]&0xff);
		pcur += 3;
	}
	DEBUG_LOG("hex printf: %s \n",tmpbuf);
	
}


int handle_process (char *recvbuf, int rcvlen, char **sendbuf, int *sndlen, const skinfo_t *sk)
{
	//DEBUG_LOG("\e[1m\e[32m<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<[cmdid]<<<<<<<<<<<<[  userid ]<[ roletime]<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\e[m");
	//hex_printf((recvbuf),rcvlen);
    if (log_hex_data_flag==1){
        char outbuf[13000];
        bin2hex(outbuf,(char*)recvbuf,rcvlen,500);
        DEBUG_LOG("I[%s]", outbuf );
    } 
	int ret= route_func->deal(recvbuf, rcvlen, sendbuf, sndlen);
	
	if (ret!=SUCC){
		if (((PROTO_HEADER*)recvbuf)->cmd_id == gf_report_vesion_cmd) {
			STD_RETURN_REPORT_VERSION(version_str);
		}
		//当处理出错时,标准返回  没有私有域
		SET_STD_ERR_BUF (sendbuf, sndlen,(PROTO_HEADER *)recvbuf,ret);
	}

	DEBUG_LOG("O:%d:%d",ret, *sndlen);
//	hex_printf((*sendbuf),*sndlen);
//	DEBUG_LOG("\e[1m\e[32m>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\e[m\n");
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
	    proto_header_t* p_ph=( proto_header_t* ) buf;
	    *key=p_ph->id;
		return 0;
}
