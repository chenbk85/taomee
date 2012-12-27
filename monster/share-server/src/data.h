/*
 * =====================================================================================
 *
 *       Filename:  data.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月18日 14时16分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luis (程龙), luis@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_DATA_H_20110718
#define H_DATA_H_20110718

#include <map>
#include <list>

#include "c_pack.h"
#include "constant.h"
#include "data_structure.h"

#include "user_manager.h"
#include "c_server.h"
#include "cli_proto.h"

typedef int (*PROTO_PROCESS_FUNC)(usr_info_t *p_user, char *p_body, uint16_t body_len);
typedef int (*DB_PROCESS_FUNC)(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

extern std::map<uint16_t, PROTO_PROCESS_FUNC> g_cli_cmd_map;
extern std::map<uint16_t, DB_PROCESS_FUNC> g_db_cmd_map;

extern UserManager g_user_mng;                      //管理所有在线用户的缓存
extern c_server *g_p_db_proxy_svr;                  //管理到db-proxy的连接

extern uint32_t g_errno;                            //协议处理的错误码
extern c_pack g_send_msg;                           //用来打包数据的类
extern char g_send_buffer[MAX_SEND_PKG_SIZE];       //发送数据的缓存
extern std::list<int> g_waiting_user_list;          //保存有请求正在等待处理的用户fd

extern std::list<user_login_info_t> g_login_user_list;          //保存最近登录的用户
extern int g_max_login_user;                   //保存的登录用户的数量

extern int g_health_desc_offline;                   //离线时健康值减少1点需要的时间,分钟
extern int g_health_desc_slow;                       //在线时健康值高时减少1点需要的时间,分钟
extern int g_health_desc_middle;                    //在线时健康值中时减少1点需要的时间,分钟
extern int g_health_desc_high;                      //在线时健康值低时减少1点需要的时间,分钟
extern int g_happy_desc_time;                       //愉悦值减少1点需要的时间,分钟

#endif//H_DATA_H_20110718
