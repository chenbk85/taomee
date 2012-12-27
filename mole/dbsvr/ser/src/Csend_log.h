/*
 * =====================================================================================
 *
 *       Filename:  Csend_log.h
 *
 *    Description: 发送米米和密码修改记录 
 *
 *        Version:  1.0
 *        Created:  04/08/2009 10:03:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef  CSEND_LOG_INCL
#define  CSEND_LOG_INCL

#include "Ctable.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "common.h"
#include "tcpip.h"
struct stru_udp {
	int fd;
	struct  sockaddr_in addr;
};
	
/* 
 * @brief 发送修改记录的类
 */
class Csend_log {
private:
	/* 米米修改记录FD */
	int log_fd;
	/* 密码修改记录FD */
	int passwd_fd;
	/* IP历史记录 */
	int ip_fd;
	stru_udp dv_udp;

	/* 保存米米修改记录发送的地址 */
	struct  sockaddr_in log_addr;
	/* 保存密码修改记录发送的地址 */
	struct sockaddr_in passwd_addr;
	/* 保存IP历史记录发送的地址 */
	struct sockaddr_in ip_addr;


public:
	Csend_log();
	/* 发送米米修改记录 */
	int send(change_log_item *p_logmesg);
	/* 发送密码修改记录 */
	int send_passwd(passwd_change_item *p_in);
	/* 历史记录 */
	int send_ip(ip_history_item *p_in);
	int dv_register(userid_t userid, dv_register_in *p_in);
	int dv_change_passwd(userid_t userid, dv_change_passwd_in *p_in );
	int dv_change_nick(userid_t userid, dv_change_nick_in *p_in );
};

#endif
