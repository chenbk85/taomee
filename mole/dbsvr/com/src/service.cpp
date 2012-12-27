#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "service.h"
#include<time.h>
#include<signal.h>
#include "proto.h"

#include "tcpip.h"
#include "Croute.h"
#include "version.h"


//全局路由数据
Croute *route; 
uint32_t log_proto_flag;
/**
 * 0:打印所有报文
 * 1:打印update报文
 * 2:不打印报文
 */

int handle_init (int argc, char **argv, int pid_type)
{
	switch (pid_type) {
		case PROC_MAIN:
			boot_log(0,0,"cur VERSION:%s",version_str );
			return 0;
		case PROC_WORK:
			route=new Croute(config_get_strval("route_file") );
			log_proto_flag=config_get_intval("LOG_PROTO_FLAG",1 );

			return 0;
		case PROC_CONN:
			return 0;
		default:
			ERROR_LOG ("invalid pid_type=%d", pid_type);
			return -1;
	}

	return -1;
}


int handle_input (const char* buffer, int length, const skinfo_t *sk)
{
	int reallen;

	if (length>=4 )	
	{
		reallen=*((uint32_t* )buffer);
		DEBUG_LOG("reallen:%u",reallen);
		if (reallen<=4096) {
			return reallen;
		}else{
			return -1;
		}
	}else return 0;
}

int handle_process (char *recvbuf, int rcvlen, char **sendbuf, int *sndlen, const skinfo_t *sk)
{
	int socketfd=0;
	int ret=0;
	userid_t id;
	enum enum_route_type rt;
	uint16_t cmd_id=0;				
	bool needupdate=false;
	char outbuf[30000];
	int  net_errno=0;
	int  log_proxy_flag=0;
	int  start_time=time(NULL);
	int  end_time;


	cmd_id=((PROTO_HEADER *) recvbuf)->cmd_id;
	rt=enum_route_type ((cmd_id & RROTO_ROUTE_FIELD )>>10);

	
	id=((PROTO_HEADER *) recvbuf)->id;
	needupdate=(cmd_id &  NEED_UPDATE);

	//save log
			

  	if (  log_proto_flag==0 ){
		log_proxy_flag=0;	
    }else if (  log_proto_flag==1)  {
		if (needupdate) log_proxy_flag=0;	
		else log_proxy_flag=1;
	}else if (log_proto_flag==2 ){
		if (needupdate) log_proxy_flag=0;	
		else log_proxy_flag=2;
	}else {
		log_proxy_flag=2;//不打印
	}

	if (log_proxy_flag==0){
        ASC2HEX_3(outbuf,recvbuf,rcvlen);
        INFO_LOG("[%d]I[%04X][%u][%u][%04d][%s]",
				getpid(), cmd_id,rcvlen,id,
				((PROTO_HEADER *) recvbuf)->result,outbuf );
	}else if (log_proxy_flag==1){
			INFO_LOG("[%d]I[%04X][%u]", getpid(), cmd_id,id);
	}

	

	socketfd= route->get_socketfd(  rt,needupdate, id);
	//
	if (socketfd>0){
		ret= net_io(socketfd,recvbuf, rcvlen, sendbuf, sndlen);
		net_errno=errno;
	}else{
		ret=NET_ERR;
	}

	if (ret==SYS_ERR){
	 	DEBUG_LOG("sys err  for net_io\n");	
	}else if (ret== NET_ERR ){
		//防止DB服务器重启，重置连接
	 	DEBUG_LOG("net err fd[%d] errno want[%d]  [%d][%s]re connect \n",
			socketfd,EBADF ,net_errno,strerror(net_errno) );	
		socketfd=route->reset_socket(rt,needupdate,id);
			
		//if (net_errno==EBADF ||net_errno==EAGAIN){//是连接中断，不是超时则重新发送
 		DEBUG_LOG("re send data\n");	
		ret= net_io(socketfd,recvbuf, rcvlen, sendbuf, sndlen);
		net_errno=errno;
        if (ret== NET_ERR){
            socketfd=route->reset_socket(rt,needupdate,id);
            DEBUG_LOG("net  err fasle  fd[%d][%d]\n",net_errno,socketfd);
        }
		//}
	}

	if (ret!=SUCC){
		//返回标准错误报文
        INFO_LOG("[%d]send: err[%d], close socket\n", getpid(),ret ) ;
		SET_STD_ERR_BUF(sendbuf, sndlen,(PROTO_HEADER *)recvbuf,ret );
	}


  	if ( log_proxy_flag==0 )
	{
		end_time=time(NULL);
		ASC2HEX_3(outbuf,*sendbuf,*sndlen );
		INFO_LOG("[%d]O[%04X][%u][%04d][%s][%d]", getpid(),
				((PROTO_HEADER *)*sendbuf)->cmd_id,
				((PROTO_HEADER *)*sendbuf)->id, 
				((PROTO_HEADER *)*sendbuf)->result,outbuf, end_time-start_time );
	}else if (log_proxy_flag==1 ) {
		end_time=time(NULL);
		INFO_LOG("[%d]O[%04X][%u][%04d][%d]", getpid(),
				((PROTO_HEADER *)*sendbuf)->cmd_id,
				((PROTO_HEADER *)*sendbuf)->id, 
				((PROTO_HEADER *)*sendbuf)->result,end_time-start_time );
	}

	if(cmd_id!=((PROTO_HEADER *)*sendbuf)->cmd_id){
		//串包了.
		ERROR_LOG("connect is err pid[%d]",getpid() );
		route->reset_socket(rt,needupdate,id);
	}	

	return SUCC;
}

void handle_fini(int pid_type)
{
	if (pid_type== PROC_WORK)
		delete route;
}

