/*
 * =====================================================================================
 *
 *       Filename:  time_stamp_impl.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2010 11:15:17 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <main_login/common.hpp>
#include "time_stamp_impl.hpp"
#include "mole_common.hpp"


int TimeStampService::fd = -1;

SwitchService TimeStampService::mole_swith;

/* @brief 时间服务器的构造函数
 */
TimeStampService :: TimeStampService()
{
}

/* @brief 初始话时间服务器的包头
 */
void TimeStampService :: init_timestamp_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd)
{
    login_timestamp_pk_header_t* pkg = reinterpret_cast<login_timestamp_pk_header_t*>(buf);

    pkg->length     = htonl(len);
    pkg->version    = 0x01;
    pkg->commandid  = htonl(cmd);
    //keep client fd
    p->counter = (p->counter + 1) % svr_usr_counter;
    pkg->seq        = (p->session->fd << 16) | p->counter;
    pkg->result     = 0;
}

/* @brief 发送数据包到时间服务器
 */
int TimeStampService :: send_to_timestamp(usr_info_t* p, const uint8_t *buf, uint32_t length)
{
    if (fd == -1) {
        fd = connect_to_svr(config_get_strval("time_stamp_ip"), config_get_intval("time_stamp_port", 0), 65535, 1);
    }
    if(p) {
        if(p->timer_list.next && p->timer_list.prev) {
            REMOVE_TIMERS(p);
        }
		INIT_LIST_HEAD(&p->timer_list);
        ADD_TIMER_EVENT(p, timestamp_timeout, p, get_now_tv()->tv_sec + 10); 
    }
	if (fd < 0) {
		return 0;
	}
    return net_send(fd, buf, length);
}

/* @brief 处理时间服务器的返回包
 */
void TimeStampService :: handle_timestamp_return(login_timestamp_pk_header_t* dbpkg, uint32_t pkglen)
{
    if (!dbpkg->seq) {
        ERROR_LOG("timestamp return error");
        return;
    }

    uint32_t counter = dbpkg->seq & 0xFFFF;
    uint32_t connfd  = dbpkg->seq >> 16;
	usr_info_t* p = user_mng.get_user(connfd);
    if(!p || counter != p->counter) {
        //ERROR_LOG("counter error [%d %d %d %d]", p->uid, p->waitcmd, counter, p->counter);
        return;
    }

    uint32_t cmd = ntohl(dbpkg->commandid); 
    DEBUG_LOG("TMSTM RETURN\t[uid=%u cmd=%u fd=%u ret=%u]",
                p->uid, cmd, connfd, dbpkg->result);

    REMOVE_TIMERS(p);
    int err = -1;
    uint32_t bodylen = dbpkg->length - sizeof(login_timestamp_pk_header_t); 
    switch(cmd) {
        case TIMESTAMP_GET_FRIENDTIME:
            err = get_friend_time_callback(p, dbpkg->body, bodylen, dbpkg->result);
            break;

        default:
            ERROR_LOG("err db cmd [%d %d]", cmd, connfd);
    }

    if(err) {
        close_client_conn(connfd);
    }
}

/* @brief 处理时间服务区器返回用户好友时间的数据包
 */
int TimeStampService :: get_friend_time_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
    uint32_t friendcount = *(uint32_t*)(p->tmpinfo + 4);
    uint32_t* friend_timestamp= (uint32_t*)(p->tmpinfo + 8 + sizeof(uint32_t) * max_friend_num);
    int error = 0;
    if(ret || bodylen < (sizeof(login_timestamp_pk_header_t) + 4)) {
        ERROR_LOG("timestamp return data error");
        error = 1;
    }

    uint32_t return_count = ntohl(*reinterpret_cast<uint32_t*>(body +
        sizeof(login_timestamp_pk_header_t))) ;
    if (!error && return_count == friendcount) { //successful      
        for(uint32_t i = 0; i != friendcount; i++) {
            friend_timestamp[i] = ntohl(*(uint32_t *)(body +
                sizeof(login_timestamp_pk_header_t) + 4 + i * sizeof(friend_info_t) + 4));
        }
    } else{ //failed ,return now time to friend time
        TRACE_LOG("get timestamp failed, info is as follows:");
        error = 1;
    }
    if(error) {
        for(uint32_t i = 0; i != max_friend_num; i++) {
            friend_timestamp[i] = 0;
        }
    }
	return mole_swith.get_recommended_svr_list(p);
}

/* @brief 时间服务器超时的回调函数
 */
int TimeStampService :: timestamp_timeout(void* owner, void* data)
{
    usr_info_t* p = reinterpret_cast<usr_info_t*>(data);
    ERROR_LOG("TIMST TIMEOUT [%d %d]", p->uid, p->waitcmd);
	send_warning("cachesvr", p->uid, p->waitcmd, 0, 0);
    if(p->waitcmd) {
        uint32_t* friend_timestamp= (uint32_t*)(p->tmpinfo + 8 + sizeof(uint32_t) * max_friend_num);
        for(uint32_t i = 0; i != max_friend_num; i++) {
            friend_timestamp[i] = 0;
        }
		mole_swith.get_recommended_svr_list(p);
    }
    return 0;
}

/* @brief 得到时间服务器的包长
 */
int TimeStampService ::get_timestamp_pkg_len(const void* avail_data)
{
	uint32_t reallen = ntohl(*reinterpret_cast<const uint32_t*>(avail_data));
	return reallen;
}
