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
#include "CNetComm.h"

static  mysql_interface *g_db ;
static  Croute_func 	*route_func ;
CNetComm*     net;
uint32_t gf_max_role_deleted=30 * 24 * 60 * 60;


int handle_init (int argc, char **argv, int pid_type)
{
	switch (pid_type) {
		case PROC_MAIN:
			boot_log(0,0,"DuDu cur  VERSION:%s",version_str );
		return 0;
		case PROC_WORK:
			srandom(getpid());
			g_db=new mysql_interface(config_get_strval("DB_IP"),
				config_get_strval("DB_USER"),
				config_get_strval("DB_PASSWD"));
			g_db->set_is_log_debug_sql( config_get_intval("IS_LOG_DEBUG_SQL",0 )  );
			route_func=new Croute_func(g_db);

			net  = new CNetComm(config_get_strval("DBPROXY_IP"), config_get_intval("DBPROXY_PORT",0));

			gf_max_role_deleted = config_get_intval("MAX_ROLE_DELETED",30)*24*60*60;
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

int handle_process (char *recvbuf, int rcvlen, char **sendbuf, int *sndlen, const skinfo_t *sk)
{
	DEBUG_LOG("\e[1m\e[32m<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<[cmdid]<<<<<<<<<<<<[  userid ]<[ roletime]<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\e[m");
	hex_printf((recvbuf),rcvlen);
	
	int ret= route_func->deal(recvbuf, rcvlen, sendbuf, sndlen);
	
	if (ret!=SUCC){
		//当处理出错时,标准返回  没有私有域
		SET_STD_ERR_BUF (sendbuf, sndlen,(PROTO_HEADER *)recvbuf,ret);
	}

	DEBUG_LOG("O:%d:%d",ret, *sndlen);
	hex_printf((*sendbuf),*sndlen);
	DEBUG_LOG("\e[1m\e[32m>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\e[m\n");
	return SUCC;
}

int handle_timer(int *sec)
{
	*sec = 60;
	static uint32_t exec_time = 0;
	time_t now = time(NULL);
	struct tm *yymmddhh = localtime(&now);
	if (yymmddhh->tm_hour >= 3 && yymmddhh->tm_hour <= 4 &&
		yymmddhh->tm_min >= 15) {
		DEBUG_LOG("enter purge uid_file...., time:%u", now);
		if ((now - exec_time) > 36000) {
			exec_time = now;
			static char timer_recvbuf[PROTO_MAX_SIZE]="";
			gf_del_allrole_overload_in pri_part={0};
			int recvlen=0;
			pri_part.time = now;
			 
			recvlen = set_proto_buf(timer_recvbuf,gf_del_allrole_overload_cmd,0,0,(char*)&pri_part,sizeof(gf_del_allrole_overload_in));
			push_a_pkg_to_worker(timer_recvbuf,recvlen);
		}
	}
	return 0;
}


void handle_fini(int pid_type)
{
	if (pid_type== PROC_WORK)
	{
		if (g_db)
		{
			delete g_db; 
			g_db=NULL;	
		}
		if (route_func)
		{
			delete route_func; 	
			route_func=NULL;
		}
		if (net)
		{
			delete net;
			net=NULL;
		}
	}
}
