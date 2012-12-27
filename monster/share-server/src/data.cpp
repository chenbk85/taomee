/*
 * =====================================================================================
 *
 *       Filename:  data.cpp
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
#include "data.h"
using namespace std;

map<uint16_t, PROTO_PROCESS_FUNC> g_cli_cmd_map;
map<uint16_t, DB_PROCESS_FUNC> g_db_cmd_map;

UserManager g_user_mng;                      //管理所有在线用户的缓存
c_server *g_p_db_proxy_svr;                  //管理到db-proxy的连接

uint32_t g_errno;                            //协议处理的错误码
c_pack g_send_msg;                           //用来打包数据的类
char g_send_buffer[MAX_SEND_PKG_SIZE];       //发送数据的缓存
list<int> g_waiting_user_list;          //保存有请求正在等待处理的用户fd
list<user_login_info_t> g_login_user_list;          //保存最近登录的用户

int g_max_login_user;                   //保存的登录用户的数量

int g_health_desc_offline;                   //离线时健康值减少1点需要的时间
int g_health_desc_slow;                       //在线时健康值高时减少1点需要的时间
int g_health_desc_middle;                    //在线时健康值中时减少1点需要的时间
int g_health_desc_high;                      //在线时健康值低时减少1点需要的时间
int g_happy_desc_time;                       //愉悦值减少1点需要的时间,分钟
