/*
 * =====================================================================================
 * 
 *       Filename:  Croute.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年01月12日 12时32分50秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#ifndef  CROUTE_INC
#define  CROUTE_INC

/*
 * =====================================================================================
 *        Class:  Croute
 *  Description:  
 * =====================================================================================
 */

//#define CONN_INFO_COUNT (sizeof(struct route_conf)/sizeof(struct conn_info) )
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include "tcpip.h"
#include <unistd.h>
#include "common.h" 
#include "benchapi.h"



struct conn_info{
	char ip[16];
	u_short port;
	int socketfd;//>0:正常fd, -1:连接失败
	uint32_t failtime;//连接失败时间(连接失败后设置)
};

struct route_cfg_stru{
	struct conn_info update_conn;
	struct conn_info query_conn;
};

class Croute
{
  public:
    Croute ( char * configfile );  /* constructor */
    virtual ~Croute () {
		this->close_all_socket();
	}	
	static const int conn_info_count=500; 
	int init_route( char * configfile);
	int get_socketfd( enum enum_route_type  route_type , bool needupdate  , int id ); 
	int reset_socket( enum enum_route_type  route_type , bool needupdate,  int id );
	int close_all_socket();
  protected:
	int GetRecode(FILE *fpload, enum enum_route_type * route_type ,  
		int* start_index, int * end_index, char* update_ip, int * update_port, 
	   char* query_ip, int * query_port);

	virtual int get_route_conf_index( 
			enum enum_route_type route_type,uint32_t id );
  private:
 	struct route_cfg_stru route_cfg[conn_info_count];
}; /* -----  end of class  Croute  ----- */
#endif   /* ----- #ifndef CROUTE_INC  ----- */
