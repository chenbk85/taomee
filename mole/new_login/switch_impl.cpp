/*
 * =====================================================================================
 *
 *       Filename:  switch_impl.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2010 11:14:25 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
extern "C" {
#include <libtaomee/timer.h>
}

#include "switch_impl.hpp"
#include "time_stamp_impl.hpp"
#include "mole_common.hpp"

/*保存定时备份online服务器的定时器列表*/
timer_head_t get_backup_tmr;

/*备份的online数目*/
int backup_svrlist_cnt = 0;

/*保存备份的online的信息*/
svr_info_t backup_svrlist[max_backup_svr];

int SwitchService::fd = -1;

uint8_t SwitchService::switch_buf[switch_max_size];

/* @brief 构造函数
 */
SwitchService :: SwitchService()
{
	fd = -1;
}

/* @brief 初始化switch服务器的包头
 */
void SwitchService :: init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd)
{
	switch_head_t *pkg = reinterpret_cast<switch_head_t*>(buf);

	pkg->len = len;
	p->counter = (p->counter + 1) % svr_usr_counter;
	pkg->seq = (p->session->fd<< 16) | p->counter;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = p->uid;
}

/* @brief 发送数据包到switch服务器
 */
int SwitchService :: send_to_switch(usr_info_t* p, uint16_t cmd, const void* buf, uint32_t body_len)
{
	if (fd == -1) {
		fd = connect_to_service(config_get_strval("switch_name"), 0, 65535, 1);
	}

	if (fd == -1 && p) {
		return get_backup_svr_list(p);
	}

    if ((fd == -1) || (body_len > (sizeof(switch_buf) - sizeof(switch_head_t)))) {
		ERROR_LOG("send to switch failed: fd=%d len=%d", fd, body_len);
        return -1;
    }

	if(p) {
		if(p->timer_list.next && p->timer_list.prev) {
			REMOVE_TIMERS(p);
		}
		INIT_LIST_HEAD(&p->timer_list);
		ADD_TIMER_EVENT(p, switch_timeout, p, get_now_tv()->tv_sec + 10); 
	}
	int len = sizeof(switch_head_t) + body_len;
	init_switch_head(p, switch_buf, len, cmd);
	memcpy(switch_buf + sizeof(switch_head_t), buf, body_len);
	return net_send(fd, switch_buf, len);
}

/* @brief 拉取推荐服务器超时，从备份数据中返回
 */
int SwitchService :: switch_timeout(void* owner, void* data)
{
    usr_info_t* p = reinterpret_cast<usr_info_t*>(data);
    ERROR_LOG("SWITCH TIMEOUT [%d %d]", p->uid, p->waitcmd);
	send_warning("switch", p->uid, p->waitcmd, 0, 0);
    if(p->waitcmd) {
        get_backup_svr_list(p);
    }
    return 0;
}

/* @brief 处理从switch服务器返回的数据包
 */
void SwitchService :: handle_switch_return(switch_head_t * dbpkg, uint32_t pkglen)
{
	if (!check_switch_return(dbpkg, pkglen)) {
		return;
	}

    int connfd  = dbpkg->seq >> 16;
    uint32_t bodylen = dbpkg->len - sizeof(switch_head_t); 

    if(connfd == 0xFFFF) {
        update_backup_svrlist_callback(dbpkg->body, bodylen, dbpkg->ret);
        return;
    }

	usr_info_t* p = user_mng.get_user(connfd);
    DEBUG_LOG("SWITCH RETURN\t[uid=%u %u fd=%d cmd=%u ret=%u]",
                p->uid, dbpkg->id, connfd, dbpkg->cmd, dbpkg->ret);
    int err = -1;
    REMOVE_TIMERS(p);
    switch(dbpkg->cmd) {
        case SWITCH_GET_RECOMMEND_SVR_LIST:
            err = get_recommeded_svr_list_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;

        case SWITCH_GET_RANGED_SVR_LIST:
            err = get_ranged_svr_list_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;

        default:
            ERROR_LOG("err db cmd [%d %d]", dbpkg->cmd, dbpkg->id);
    }
    if(err) {
        close_client_conn(connfd);
    } 
}

/* @brief 校验switch返回的包是否正确
 */
int SwitchService :: check_switch_return(switch_head_t *dbpkg, uint32_t pkglen)
{
	if (!dbpkg->seq) {
        ERROR_LOG("Switch return error");
        return 0;
    }

    uint32_t counter = dbpkg->seq & 0xFFFF;
    int connfd  = dbpkg->seq >> 16;
	if (connfd != 0XFFFF) {
		usr_info_t* p = user_mng.get_user(connfd);
    	if(!p || counter != p->counter) {
        	//ERROR_LOG("counter error [%d %d %d %d]", p->uid, p->waitcmd, counter, p->counter);
        	return 0;
    	}
	}
	return 1;
}

/* @brief 向switch服务器发送得到推荐服务器列表的包
 */
int SwitchService :: get_recommended_svr_list(usr_info_t* p)
{
	TRACE_LOG("enter get_recommended_svr_list");
	//pack up the request data
	int idx = 0;
	uint32_t friendcount = *(uint32_t*)(p->tmpinfo + 4);
	uint8_t buf[1024 * 1024 *2];
	taomee::pack_h(buf, static_cast<uint16_t>(config_get_intval("domain", 1)), idx);
	taomee::pack_h(buf, static_cast<uint8_t>(1), idx);
	taomee::pack_h(buf, friendcount, idx);
	taomee::pack(buf, (void*)(p->tmpinfo + 8), friendcount * sizeof(userid_t), idx);
	send_to_switch(p, SWITCH_GET_RECOMMEND_SVR_LIST, buf, idx);
	return 0;
}

/* @brief 处理switch服务器返回的推荐服务器的数据包
 */
int SwitchService :: get_recommeded_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
	TRACE_LOG("get recommend callback");

	struct in_addr cliaddr;
    memcpy(&cliaddr, &(p->session->remote_ip), 4);
	NOTI_LOG("get recommend callback userid:%u,ip:%s", p->uid, inet_ntoa(cliaddr));
	
	CHECK_VAL_GE(bodylen, sizeof(svr_list_t));
	svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);
	CHECK_BODY_LEN(bodylen, 4 + sizeof(svr_list_t) + sizeof(svr_info_t) * svrlist->online_cnt);
	uint32_t max_online_id = *(uint32_t*)(body + sizeof(svr_list_t) + sizeof(svr_info_t) * svrlist->online_cnt);
	return send_recommeded_svr_list(p, svrlist->svrs, svrlist->online_cnt, max_online_id);
}

/* @brief 返回给客户端推荐服务器列表信息
 */
int SwitchService :: send_recommeded_svr_list(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt, int max_online_id)
{
    int idx = sizeof(cli_login_pk_header_t);
	uint8_t client_buf[1024 * 1024 *2];
    taomee::pack(client_buf, online_cnt, idx);
    for (uint32_t i = 0; i != online_cnt; ++i) {
        taomee::pack(client_buf, svrs[i].id, idx);
        taomee::pack(client_buf, svrs[i].users, idx);
        taomee::pack(client_buf, svrs[i].ip, 16, idx);
        taomee::pack(client_buf, svrs[i].port, idx);
        taomee::pack(client_buf, svrs[i].friends, idx);
    }
    taomee::pack(client_buf, max_online_id, idx);

    // pkg the frd id and timestamp
    uint32_t friendcount = *(uint32_t*)(p->tmpinfo + 4);
    uint32_t* frd_timestamp = (uint32_t*)(p->tmpinfo + 8 + sizeof(userid_t) * max_friend_num);
    uint32_t* friend_id = (uint32_t*)(p->tmpinfo + 8);
    taomee::pack(client_buf, *(uint32_t*)p->tmpinfo, idx);
    taomee::pack(client_buf, friendcount, idx);
    for (uint32_t i = 0; i < friendcount; i++ ) {
        taomee::pack(client_buf, friend_id[i], idx);
        taomee::pack(client_buf, frd_timestamp[i], idx);
    }
#ifndef TW_VER  
	send_login_num_to_stat(p);
#endif
    init_proto_head(client_buf, idx, PROTO_GET_RECOMMEND_SVR_LIST, p->uid, SUCCESS);
    return send_to_self(p, client_buf, idx);
}

/* @brief 向统计平台发送登入人数数据包
 */
void SwitchService :: send_login_num_to_stat(usr_info_t *p)
{
	statistic_info_t info;
    uint32_t msglog_type;
    info.user_id = p->uid;
    msglog_type = mole_login_offset + p->channel;
	if (p->channel > 56)  {
		msglog_type = mole_not_normal_log_channel;
	}
    statistic_msglog(msglog_type, (void *)&info, sizeof info);
}

/* @brief 定时的备份服务器列表
 */
int SwitchService :: update_backup_svrlist(void* owner, void* data)
{
    int idx = sizeof(switch_head_t);
    taomee::pack_h(switch_buf, static_cast<uint16_t>(0), idx);
    taomee::pack_h(switch_buf, min_backup_svr, idx);
    taomee::pack_h(switch_buf, max_backup_svr, idx);
    taomee::pack_h(switch_buf, static_cast<uint32_t>(0), idx);
    switch_head_t *pkg = reinterpret_cast<switch_head_t*>(switch_buf);

    pkg->len = idx;
    if(fd == -1) {
		fd = connect_to_service(config_get_strval("switch_name"), 0, 65535, 1);
    }
    if(fd == -1) {
        ADD_TIMER_EVENT(&get_backup_tmr, update_backup_svrlist, NULL, get_now_tv()->tv_sec + 60);
        return 0;
    }
    pkg->seq = (0xFFFF<< 16) | 0;
    pkg->cmd = SWITCH_GET_RANGED_SVR_LIST;
    pkg->ret = 0;
    pkg->id  = 0;
    send_to_switch_without_init_head(NULL, switch_buf, idx);
    ADD_TIMER_EVENT(&get_backup_tmr, update_backup_svrlist, NULL, get_now_tv()->tv_sec + 60);
    return 0;
}

/* @brief 保存定时备份switch列表
 */
int SwitchService :: update_backup_svrlist_callback(uint8_t* body, uint32_t bodylen, int ret)
{
    CHECK_VAL_GE(bodylen, sizeof(svr_list_t));

    svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);

    //chk if the pkg len is valid
    CHECK_BODY_LEN(bodylen, sizeof(svr_list_t) + svrlist->online_cnt * sizeof(svr_info_t));     
    backup_svrlist_cnt = svrlist->online_cnt;
    memcpy(backup_svrlist, svrlist->svrs, svrlist->online_cnt * sizeof(svr_info_t));
    return 0;
}

/* @brief 从备份中得到推荐服务器或者某个范围的服务器列表
 */
int SwitchService :: get_backup_svr_list(usr_info_t* p)
{
    switch(p->waitcmd) {
        case PROTO_GET_RECOMMEND_SVR_LIST:
            return send_recommeded_svr_list(p, backup_svrlist, backup_svrlist_cnt, backup_svrlist_cnt);

        case PROTO_GET_RANGED_SVR_LIST:
            return send_ranged_svrlist(p, backup_svrlist, backup_svrlist_cnt);

    	default:
        	ERROR_LOG("invalid command=%u", p->waitcmd);
    }
    return 0;
}

/* @brief 初始化函数
 */
void SwitchService :: init_switch()
{
	backup_svrlist_cnt = 0;
	memset(backup_svrlist, 0, sizeof(svr_info_t) * max_backup_svr);
	INIT_LIST_HEAD(&get_backup_tmr.timer_list);
	ADD_TIMER_EVENT(&get_backup_tmr, update_backup_svrlist, NULL, get_now_tv()->tv_sec + 60);
}

/* @brief 发包给switch,但不初始化包头
 */
int SwitchService :: send_to_switch_without_init_head(usr_info_t* p, const uint8_t *buf, uint32_t length)
{
    if (fd == -1) {
		fd = connect_to_service(config_get_strval("switch_name"), 0, 65535, 1);
    }
    if (fd == -1 && p) {
        return get_backup_svr_list(p);
		return 0;
    }
    if(p) {
        if(p->timer_list.next && p->timer_list.prev) {
            REMOVE_TIMERS(p);
        }
        INIT_LIST_HEAD(&p->timer_list);
        ADD_TIMER_EVENT(p, switch_timeout, p, get_now_tv()->tv_sec + 10); 
    }
	if (fd < 0) {
		return 0;
	}
    return net_send(fd , buf, length);
}

/* @brief 发包给switch,得到某个范围的推荐服务器
 */
int SwitchService :: get_ranged_svr_list(usr_info_t* p, int start_id, int end_id, int friendcount, userid_t allfriendid[])
{
    // pack up the request data
	uint8_t buf[1024 * 1024 *2];
    int idx = 0;
    taomee::pack_h(buf, static_cast<uint16_t>(0), idx);
    taomee::pack_h(buf, start_id, idx);
    taomee::pack_h(buf, end_id, idx);
    taomee::pack_h(buf, friendcount, idx);
    taomee::pack(buf, allfriendid, friendcount * sizeof(userid_t), idx);
    return send_to_switch(p, SWITCH_GET_RANGED_SVR_LIST, buf, idx);
}

/* @brief 处理switch拉某个范围服务器列表的数据包
 */
int SwitchService :: get_ranged_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
	//varify the respond data
    TRACE_LOG("get ranged callback");

	struct in_addr cliaddr;
    memcpy(&cliaddr, &(p->session->remote_ip), 4);
	NOTI_LOG("get ranged callback userid:%u,ip:%s", p->uid, inet_ntoa(cliaddr));
	
	CHECK_VAL_GE(bodylen, sizeof(svr_list_t));

	svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);

    //DEBUG_LOG("svrlist [%d %d]", svrlist->domain_id, svrlist->online_cnt);
	//chk if the pkg len is valid
	CHECK_BODY_LEN(bodylen, sizeof(svr_list_t) + svrlist->online_cnt * sizeof(svr_info_t)); 
    return send_ranged_svrlist(p, svrlist->svrs, svrlist->online_cnt);
}

/* @brief 返回客户端拉取的某个范围的服务器列表
 */
int SwitchService :: send_ranged_svrlist(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt)
{
	uint8_t my_cli_buf[1024 * 1024 *2];
	//make a response package to the flash client
	int idx = sizeof(cli_login_pk_header_t);
	taomee::pack(my_cli_buf, online_cnt, idx);
	for (uint32_t i = 0; i != online_cnt; ++i) {
		taomee::pack(my_cli_buf, svrs[i].id, idx);
		taomee::pack(my_cli_buf, svrs[i].users, idx);
		taomee::pack(my_cli_buf, svrs[i].ip, 16, idx);
		taomee::pack(my_cli_buf, svrs[i].port, idx);
		DEBUG_LOG("server id [%u] server ip [%s] server port [%u]", svrs[i].id, svrs[i].ip, svrs[i].port);
		taomee::pack(my_cli_buf, svrs[i].friends, idx);
	}
	init_proto_head(my_cli_buf, idx, PROTO_GET_RANGED_SVR_LIST, p->uid, SUCCESS);
    return send_to_self(p, my_cli_buf, idx);
}

/* @brief 得到switch包的长度
 */
int SwitchService :: get_switch_pkg_len(const void *avail_data)
{
	uint32_t reallen = *reinterpret_cast<const uint32_t*>(avail_data);
	if ((reallen <= dbpkg_max_size) && (reallen >= sizeof(dbproto_t))) {
		return reallen;
	}

    ERROR_LOG("invalid switch pkg len: %u", reallen);
    return -1;
}
