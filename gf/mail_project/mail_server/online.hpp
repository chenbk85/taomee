// =====================================================================================
//
//       Filename:  online.hpp
// 
//    	 Description: specify the communication between online and switch 
// 
//       Version:  1.0
//       Created:  03/11/2009 08:35:48 PM CST
//       Compiler:  g++
// 
//       Company:  TAOMEE
// 
// =====================================================================================


#ifndef SWITCH_ONLINE_HPP_
#define SWITCH_ONLINE_HPP_

#include <map>
#include <set>
#include <fstream>
#include <list>
#include <libtaomee++/memory/mempool.hpp>

using namespace std;

extern "C" 
{
#include <arpa/inet.h>
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <string.h>
}

#include "mail_server.hpp"

#define MAX_ONLINE_INFO_COUNT 2000


extern  uint8_t       s_pkg_[pkg_size];

typedef struct online_info_t 
{
	online_info_t()
	{
		domain = 0;
		online_id = 0;
		memset(online_ip, 0, sizeof(online_ip));
		online_port = 0;
		fd = 0;
		game_type = 0;
		fdsess = NULL;
	}
	uint16_t	domain;
	uint32_t	online_id;
	uint32_t    game_type;
	char		online_ip[16];
	in_port_t	online_port;
	int         fd;
	fdsession_t*    fdsess;
}online_info_t;



// =====================================================================================
class Online 
{
public:
	static online_info_t*    p_online_infos;
	static std::map<int, int> online_fd_maps; //online fd --->online id

public:
	static void init();
	static void final();

	static online_info_t*  get_online_info_by_fd(int fd);
	static int  get_online_id_by_fd(int fd);	
	static int  report_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  clear_online_info(int fd);
	static int  online_keepalive(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  get_mail_head_list(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int  get_mail_body(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
};


//----------------------------------------------------------------------------------------
#define MAX_MAIL_TITLE_LEN     (40)
#define MAX_MAIL_CONTENT_LEN   (100)


typedef struct mail_header
{
	mail_header()
	{   
		mail_id = 0;
		mail_time = 0;
		mail_state = 0;
		mail_templet = 0;
		mail_type = 0;
		sender_id = 0;
		sender_role_tm = 0;
		memset(mail_title, 0, sizeof(mail_title));
	}   
	uint32_t mail_id;                                  //邮件ID，唯一
	uint32_t mail_time;                                //邮件发送时间 unix time
	uint32_t mail_state;                               //邮件状态 1:已读 2：未读
	uint32_t mail_templet;                             //邮件模板
	uint32_t mail_type;                                //邮件类型
	uint32_t sender_id;                                //发件人米米号，系统邮件为0
	uint32_t sender_role_tm;                           //发件人角色ROLE_TM，系统邮件为0（该字段暂时没有用）
	char     mail_title[ MAX_MAIL_TITLE_LEN+1 ];       //邮件标题
}mail_header;

typedef struct mail_body
{
	mail_body()
	{   
		mail_id = 0;
		memset(mail_content, 0, sizeof(mail_content));
	}   
	uint32_t mail_id;                                  //邮件ID
	char     mail_content[ MAX_MAIL_CONTENT_LEN+1 ];   //邮件内容    
}mail_body;

typedef struct mail_head_list_rsp_t
{
	uint32_t count;
	mail_header   headers[];
}mail_head_list_rsp_t;


typedef struct mail_body_rsp_t
{
	uint32_t mail_id ;
	char     mail_body[MAX_MAIL_CONTENT_LEN + 1]; 
	char     mail_numerical_enclosure[1024];
	char     mail_item_enclosure[1024];
	char     mail_equip_enclosure[1024];
}mail_body_rsp_t;

int db_mail_head_list(online_info_t* info, uint32_t userid, uint32_t role_regtime);
int db_mail_head_list_callback(online_info_t* info, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_mail_body(online_info_t* info, uint32_t userid, uint32_t role_regtime, uint32_t mail_id);
int db_mail_body_callback(online_info_t* info, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

#endif // SWITCH_ONLINE_HPP_

