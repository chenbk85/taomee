/*
 * =====================================================================================
 *
 *       Filename:  Croute.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年01月12日 12时32分48秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:  set tabstop=4 
 * 		
 * =====================================================================================
 */
#include "Croute.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Croute
 *      Method:  Croute
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Croute::Croute ( char * configfile)
{
	this->init_route(configfile);	
}  /* -----  end of method Croute::Croute  (constructor)  ----- */

int Croute:: 
GetRecode(FILE *fpload, enum enum_route_type * route_type ,  
		int* start_index, int * end_index, char* update_ip, int * update_port, 
	   char* query_ip, int * query_port)
{
	//读取一行记录的数据保存在此
	int   nLineLen;
	char sLine[250]={ };
	char sstart_index[50]={ };
	char send_index[50]={ };
	char update_sport[50]={ };
	char query_sport[50]={ };
	char sroute_type[100]={ };

	//是否已读取完毕
	if ( feof(fpload))
		return FAIL;

	//读取一行记录
	fgets(sLine,250,fpload);
	nLineLen=strlen(sLine);

	//是否已读取完毕（行为空）
	if (nLineLen==0)
		return FAIL;

	sscanf(sLine,"%s%s%s%s%s%s%s", sroute_type, 
			sstart_index, send_index, update_ip, update_sport, query_ip, query_sport);
	DEBUG_LOG("%s==%s==%s==%s==%s==%s==%s",sroute_type,
		              sstart_index, send_index, update_ip, update_sport, 
					  query_ip, query_sport	);

	if (strcmp(sroute_type,"USER")==0 ){		
		*route_type=ROUTE_USER;
	}else if (strcmp(sroute_type,"EMAIL")==0 ){		
		*route_type=ROUTE_EMAIL;
	}else if (strcmp(sroute_type,"USERINFO")==0 ){		
		*route_type=ROUTE_USERINFO;
	}else if (strcmp(sroute_type,"GAME_SCORE")==0 ){		
		*route_type=ROUTE_GAME_SCORE;
	}else if (strcmp(sroute_type,"SERIAL")==0 ){		
		*route_type=ROUTE_SERIAL;
	}else if (strcmp(sroute_type,"VIP")==0 ){		
		*route_type=ROUTE_VIP;
	}else if (strcmp(sroute_type,"MSGBOARD")==0 ){		
		*route_type=ROUTE_MSGBOARD;
	}else if (strcmp(sroute_type,"REGISTER_SERIAL")==0 ){		
		*route_type=ROUTE_REGISTER_SERIAL ;
	}else if (strcmp(sroute_type,"USERMSG")==0 ){		
		*route_type=ROUTE_USERMSG;
	}else if (strcmp(sroute_type,"SU")==0 ){		
		*route_type=ROUTE_SU;
	}else if (strcmp(sroute_type,"DV")==0 ){		
		*route_type=ROUTE_DV;
	}else if (strcmp(sroute_type,"SYSARG")==0 ){		
		*route_type=ROUTE_SYSARG;
	}else if (strcmp(sroute_type,"EMAIL_SYS")==0 ){		
		*route_type=ROUTE_EMAIL_SYS;
	}else if (strcmp(sroute_type,"SALE")==0 ){		
		*route_type=ROUTE_SALE;
	}else if (strcmp(sroute_type,"TEMP")==0 ){		
		*route_type=ROUTE_TEMP;
	}else if (strcmp(sroute_type,"HOPE")==0 ){		
		*route_type=ROUTE_HOPE;
	}else if (strcmp(sroute_type,"PRODUCE")==0 ){		
		*route_type=ROUTE_PRODUCE;
	}else if (strcmp(sroute_type,"ROOM")==0 ){		
		*route_type=ROUTE_ROOM;
	}else if (strcmp(sroute_type,"ROOMMSG")==0 ){		
		*route_type=ROUTE_ROOMMSG;
	}else if (strcmp(sroute_type,"GROUP")==0 ){		
		*route_type=ROUTE_GROUP;
	}else if (strcmp(sroute_type,"GROUPMAIN")==0 ){		
		*route_type=ROUTE_GROUPMAIN;
	}else if (strcmp(sroute_type,"MMS")==0 ){		
		*route_type=ROUTE_MMS;
	}else if (strcmp(sroute_type,"PARTY")==0 ){		
		*route_type=ROUTE_PARTY;
	}else if (strcmp(sroute_type,"ADMIN")==0 ){		
		*route_type=ROUTE_ADMIN;
	}else if (strcmp(sroute_type,"PICTURE")==0 ){		
		*route_type=ROUTE_PICTURE;
	}else if (strcmp(sroute_type,"APPEAL")==0 ){		
		*route_type=ROUTE_APPEAL;
	}else if (strcmp(sroute_type,"NOROUTE")==0 ){		
		*route_type=ROUTE_NOROUTE;
	}else {
		*route_type=ROUTE_NULL;
	}

	if (*route_type!=ROUTE_NULL){	
		// 无法识别的不处理  包括注释
		*start_index=atoi(sstart_index);
		*end_index=atoi(send_index);
		*update_port=atoi(update_sport);
		*query_port=atoi(query_sport);
	}
	return SUCC;
}

int  Croute::init_route( char * configfile)
{
	FILE *fp;
	char update_ip[16]={},query_ip[16]={};
	int  i,start_index,end_index,route_index, update_port,query_port ;
	enum enum_route_type  route_type;
	memset(route_cfg,0,sizeof(route_cfg) );
	fp=fopen(configfile,"r");	
	if (fp==NULL) {
		return FAIL;	
	}
	while (GetRecode(fp,&route_type,&start_index,&end_index,update_ip,&update_port,
			   query_ip,&query_port 	)==SUCC){
		//DEBUG_LOG("%s %d",ip,port );
		//当是注释或不认识的类型
		if (route_type==ROUTE_NULL){
			continue;
		}
			
		if (start_index<0||end_index>=this->conn_info_count){
			continue;
		}

		//update	
		for (i=start_index;i<=end_index;i++) {
			route_index=get_route_conf_index(route_type,i);
			if (route_index==FAIL) {
				DEBUG_LOG("config route ip port err index [%d][%s][%d]",i, update_ip,update_port);
				break;
			}

			strcpy(this->route_cfg[route_index].update_conn.ip,update_ip);
			this->route_cfg[route_index].update_conn.port=update_port;
			this->route_cfg[route_index].update_conn.socketfd=-1;
		}

		//query
		if ( query_ip[0]=='\0'){//没有配置
			DEBUG_LOG("====set_same");
			strcpy(query_ip,update_ip );
			query_port=update_port;
		}

		for (i=start_index;i<=end_index;i++) {
			route_index=get_route_conf_index(route_type,i);
			if (route_index==FAIL) {
				DEBUG_LOG("config route ip port err index [%d][%s][%d]",i,query_ip,query_port);
				break;
			}

			strcpy(this->route_cfg[route_index].query_conn.ip,query_ip);
			this->route_cfg[route_index].query_conn.port=query_port;
			this->route_cfg[route_index].query_conn.socketfd=-1;
		}
		//
		memset(update_ip,0,sizeof(update_ip) );
		memset(query_ip,0,sizeof(query_ip) );
	}

	fclose(fp);	
	for(i=0;i<this->conn_info_count;i++){
		DEBUG_LOG("config route [%d][%s][%d][%s][%d]",i,
				route_cfg[i].update_conn .ip,
			   route_cfg[i].update_conn.port,
				route_cfg[i].query_conn .ip,
			   route_cfg[i].query_conn.port
			   );
	}

	return SUCC;
	
}
int  Croute::
get_route_conf_index(enum enum_route_type  route_type,uint32_t id )
{
	int index=0;
	if (route_type >=100)return FAIL;

	/**
	 *0-99:没有分库
	 *100-199:分10个库的
	 *200以上:分100个库的
	 **/
			
	switch (route_type ){
		case	ROUTE_EMAIL_SYS:index= 100+id%10;    break;
		case	ROUTE_SALE: 	index= 110+id%10;    break;
		case	ROUTE_GROUP: 	index= 120+id%10;    break;
		case	ROUTE_MMS: 		index= 130+id%10;    break;

		case	ROUTE_ROOM: 	index= 140+id%10;    break;
		case	ROUTE_ROOMMSG: 	index= 150+id%10;    break;

		case	ROUTE_USER: 	index= 200+id%100;   break; 
		case	ROUTE_USERINFO: index= 300+id%100; 	break; 
		case	ROUTE_PICTURE: index= 400+id%100; 	break; 

		default:  index=route_type;
	}
	return index;
}

//重置一个特定的连接
int  Croute::
reset_socket( enum enum_route_type  route_type ,bool needupdate , int id )
{
	int i,socketfd;		
	char ip[16];
	u_short port;
	struct conn_info *p_sock_info; 
	time_t fail_connect_time;
		

	int route_index;
	if ((route_index=get_route_conf_index(route_type,id ) )==FAIL	){
			return FAIL;
	}

	if (needupdate ){
			p_sock_info=&( this->route_cfg[route_index].update_conn);
	}else{
			p_sock_info=&( this->route_cfg[route_index].query_conn);
	}

	if( p_sock_info->socketfd==-1 && time(NULL)-p_sock_info->failtime<60 ){
		//连接失败时间小于设定时间,直接返回,不重连
		DEBUG_LOG("failtime <60 :fail,return -1[%d][%d] ", time(NULL),p_sock_info->failtime );
		return -1;
	}

	//不管怎样,关闭原有连接
	close(p_sock_info->socketfd);

	/*得到该连接的 IP ,port*/
	strcpy(ip,p_sock_info->ip);	
	port=p_sock_info->port;
	DEBUG_LOG("reset socket :open socket[%s][%d]\n", ip,port);
	uint32_t open_time=time(NULL);
		
	socketfd=open_socket(ip,port);	
	if (socketfd == -1){
		if (time(NULL)-open_time>=1){
			//连接超时
			fail_connect_time=time(NULL);
		} else{
			fail_connect_time=0;
		}
		DEBUG_LOG("reset socket error: open socket[%s][%d]\n", ip,port);
	}else{//连接成功
		fail_connect_time=0;

	}
	//设置所有相同的连接	
	for (i=0;i<this->conn_info_count ;i++){
		if (strcmp(ip ,this->route_cfg[i].update_conn.ip) ==0 && 
			port==this->route_cfg[i].update_conn.port){		
			this->route_cfg[i].update_conn.socketfd=socketfd;
			this->route_cfg[i].update_conn.failtime=fail_connect_time;
		}

		if (strcmp(ip ,this->route_cfg[i].query_conn.ip) ==0 && 
			port==this->route_cfg[i].query_conn.port){		
			this->route_cfg[i].query_conn.socketfd=socketfd;
			this->route_cfg[i].query_conn.failtime=fail_connect_time;
		}
	}
	return socketfd;
}

//return socketfd 
int  Croute::
get_socketfd( enum enum_route_type  route_type ,bool needupdate , int id ) 
{
	int route_index;
	if ((route_index=get_route_conf_index(route_type,id ) )==FAIL){
			return FAIL;
	}else{
		if (needupdate )
			return this->route_cfg[route_index].update_conn.socketfd;
		else
			return this->route_cfg[route_index].query_conn.socketfd;
	}
}

int  Croute:: close_all_socket()
{
	int i;
	//程序退出时关闭:错误忽略
	for (i=0;i<this->conn_info_count ;i++){
		close(this->route_cfg[i].update_conn.socketfd);
	}
	return SUCC;
}
