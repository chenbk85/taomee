/*
 * =====================================================================================
 *
 *       Filename:  common.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2010 05:38:04 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "mole_common.hpp"
#include "time_stamp_impl.hpp"


/* @brief 解压session
 */
int unpkg_auth(const uint8_t body[], int len, login_session_t* sess)
{
	char outbuf[32];
    if ( len != 16 )
        return -1;
		    
    //session: ip + time + userid + time
	des_decrypt(LOGIN_DES_KEY, (char*)(body), outbuf);
    des_decrypt(LOGIN_DES_KEY, (char*)(body + 8), outbuf + 8);
    sess->ip  = *(uint32_t *)outbuf;
    sess->uid = *(uint32_t *)(outbuf + 8);
    sess->tm1 = *(uint32_t*)(outbuf + 4); 
    sess->tm2 = *(uint32_t*)(outbuf + 12);
    return 0;
}

/* @brief 校验客户端传过来的session是否正确
 */
int verify_session(const userid_t uid, const login_session_t* sess)
{
	time_t now_time = time(0);
    if (sess->uid != uid) {
        ERROR_RETURN(("verify userid failed: id=%u %u", uid, sess->uid), -1);
	}
	if (sess->tm1 != sess->tm2) {
        ERROR_RETURN(("verify time failed: tm=%u %u", sess->tm1, sess->tm2), -1);
	}
	int diff = now_time - sess->tm1;
	if ( (diff > 3600) || (diff < -3600) ) {
        ERROR_RETURN(("verify time failed: tm=%u %ld tmdiff=%d uid=%u", sess->tm1, now_time, diff, uid), -1);
	}
	return 0;
}

/* @brief 清楚用户的信息
 */
void clear_usr_info(usr_info_t* p)
{
	p->uid = 0;
	p->waitcmd = 0;
	if(p->timer_list.next && p->timer_list.prev) {
	 	REMOVE_TIMERS(p);
	}
}

/* @breif 初始化发给客户端的包头
 */
void init_proto_head(void* buf, uint32_t len, uint32_t cmd, userid_t uid, uint32_t result)
{
	cli_login_pk_header_t* p = reinterpret_cast<cli_login_pk_header_t*>(buf);

	p->length     = htonl(len);
	p->version    = 0x01;
	p->commandid  = htonl(cmd);
	//keep client fd
	p->userid     = htonl(uid);
	p->result     = htonl(result);
}

/* @brief 发送数据给客户端
 */
int send_to_self(usr_info_t* p, const uint8_t *buf, uint32_t length)
{
	//DEBUG_LOG("SEND TO SELF\t[%d %d %d]", p->uid, p->waitcmd, length);
	clear_usr_info(p);
	return send_pkg_to_client(p->session, buf, length);
}
