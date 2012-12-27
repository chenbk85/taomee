extern "C" {
#include <libtaomee/project/constants.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/timer.h>
}
#include "mysvrlist.h"
#include "myproto.h"
#include "mynet.h"
#include "myinit.h"

#define REGISTERED              5002
#define NO_ACTIVED              5004

#define USER_BE_BAN             5006                              
#define USER_BE_24_BAN          5007

#define INVALID_NICK_NAME       5010
#define SERVER_MAINTENANCE      5011
#define USER_BE_7_DAY_BAN       5012
#define USER_BE_14_DAY_BAN      5013
#define USER_OUT_OF_ANXIN       6001

#define MIN_BACKUP_SVR             1
#define MAX_BACKUP_SVR             100
#define MIN_VALID_SVR              1
#define MAX_VALID_SVR              2000

static timer_head_t get_backup_tmr;
static int backup_svrlist_cnt = 0;
static svr_info_t backup_svrlist[MAX_BACKUP_SVR];
static uint8_t login_switch_sendbuf[BUFFER_SIZE];
static uint8_t my_cli_buf[BUFFER_SIZE];

int update_backup_svrlist(void* owner, void* data)
{
    int idx = sizeof(svr_proto_t);
    taomee::pack_h(login_switch_sendbuf, static_cast<uint16_t>(0), idx);
    taomee::pack_h(login_switch_sendbuf, MIN_BACKUP_SVR, idx);
    taomee::pack_h(login_switch_sendbuf, MAX_BACKUP_SVR, idx);
    taomee::pack_h(login_switch_sendbuf, static_cast<uint32_t>(0), idx);
    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(login_switch_sendbuf);

    pkg->len = idx;
    if(switch_fd == -1) {
        //switch_fd = connect_to_svr(my_opt.master_ip, my_opt.master_port, 65535, 1);
		switch_fd = connect_to_service(my_opt.master_ser, 0, 65535, 1);
    }
    if(switch_fd == -1) {
        ADD_TIMER_EVENT(&get_backup_tmr, update_backup_svrlist, NULL, get_now_tv()->tv_sec + 60);
        return 0;
    }
    pkg->seq = (0xFFFF<< 16) | 0;
    pkg->cmd = SWITCH_GET_RANGED_SVR_LIST;
    pkg->ret = 0;
    pkg->id  = 0;
    send_to_switch(NULL, login_switch_sendbuf, idx);
    //TRACE_LOG("update backup svrlist");
    ADD_TIMER_EVENT(&get_backup_tmr, update_backup_svrlist, NULL, get_now_tv()->tv_sec + 60);
    return 0;
}

int timestamp_timeout(void* owner, void* data)
{
    usr_info_t* p = reinterpret_cast<usr_info_t*>(data);
    ERROR_LOG("TIMST TIMEOUT [%d %d]", p->uid, p->waitcmd);
    if(p->waitcmd) {
        uint32_t* friend_timestamp= (uint32_t*)(p->tmpinfo + 8 + sizeof(uint32_t) * MAX_FRIEND_NUM);
        for(uint32_t i = 0; i != MAX_FRIEND_NUM; i++) {
            friend_timestamp[i] = 0;
        }
        get_recommended_svr_list(p);
    }
    return 0;
}

int switch_timeout(void* owner, void* data)
{
    usr_info_t* p = reinterpret_cast<usr_info_t*>(data);
    ERROR_LOG("SWITCH TIMEOUT [%d %d]", p->uid, p->waitcmd);
    if(p->waitcmd) {
        get_backup_svr_list(p);
    }
    return 0;
}

int mydbproxy_timeout(void* owner, void* data)
{
    usr_info_t* p = reinterpret_cast<usr_info_t*>(data);
    ERROR_LOG("DB TIMEOUT [%d %d]", p->uid, p->waitcmd);
    if(p->waitcmd) {
        send_warning(p, 1, p->waitcmd);
        send_to_self_error(p, SYSTEM_ERR);
    }
    return 0;
}

int mydeluser_timeout(void* owner, void* data)
{
    usr_info_t* p = reinterpret_cast<usr_info_t*>(data);
    ERROR_LOG("DELUSER TIMEOUT [%d %d]", p->uid, p->waitcmd);
    if(p->waitcmd) {
        send_warning(p, 1, p->waitcmd);
        send_to_self_error(p, SYSTEM_ERR);
    }
    return 0;
}

int update_backup_svrlist_callback(uint8_t* body, uint32_t bodylen, int ret)
{
    CHECK_VAL_GE(bodylen, sizeof(svr_list_t));

    svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);

    //chk if the pkg len is valid
    CHECK_VAL(bodylen, sizeof(svr_list_t) + svrlist->online_cnt * sizeof(svr_info_t));     
    backup_svrlist_cnt = svrlist->online_cnt;
    memcpy(backup_svrlist, svrlist->svrs, svrlist->online_cnt * sizeof(svr_info_t));
    //for(uint32_t i= 0; i< svrlist->online_cnt; i++) {
    //    DEBUG_LOG("BACKUP svr ID [%d]", backup_svrlist[i].id);
    //    DEBUG_LOG("BACKUP svr usr [%d]", backup_svrlist[i].users);
    //    DEBUG_LOG("BACKUP svr port[%d]", backup_svrlist[i].port);
    //}
    return 0;
}

int init_timer()
{
    INIT_LIST_HEAD(&get_backup_tmr.timer_list);
    ADD_TIMER_EVENT(&get_backup_tmr, update_backup_svrlist, NULL, get_now_tv()->tv_sec + 60);
    return 0;
}

int login_gf_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
    if ( ret) {
        DEBUG_LOG("LOGIN FAILED\t[err=%d uid=%u]", ret, p->uid);
        switch (ret) {
            case USER_ID_NOFIND_ERR:
                send_to_self_error(p, USER_NOT_FIND);
                break;
            case USER_NOUSED_ERR:
                send_to_self_error(p, USER_BE_BAN);
                break;
            case USER_OFFLIN24_ERR:
                send_to_self_error(p, USER_BE_24_BAN);
                break;
            case USER_NOT_ENABLED_ERR:
                send_to_self_error(p, NO_ACTIVED);
                break;
            case USER_OFFLINE_SEVEN_DAY_ERR:
                send_to_self_error(p, USER_BE_7_DAY_BAN);
                break;
            case USER_OFFLINE_FOURTEEN_DAY_ERR:
                send_to_self_error(p, USER_BE_14_DAY_BAN);
			case USER_ANXI_SERVICE_OUT_OF:
                send_to_self_error(p, USER_OUT_OF_ANXIN);
                break;
            default:
                send_to_self_error(p, SYSTEM_ERR);
                ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
                break;
        }
        return SUCCESS;
        //return CLOSE_CONN;
    }
    CHECK_VAL_GE(bodylen, 16);
    gf_login_out* user_login_info = reinterpret_cast<gf_login_out*>(body);
    CHECK_VAL(bodylen, 16 + 4 * user_login_info->friendcount + 4 * user_login_info->blackcount);
    TRACE_LOG("uid=%u, frdcnt=%u, blkcnt=%u",p->uid,user_login_info->friendcount,user_login_info->blackcount);
    if (user_login_info->friendcount > MAX_FRIEND_NUM) {
        ERROR_LOG("severe db error[friendcount]: uid=%u frdcnt=%u", p->uid, user_login_info->friendcount);
        user_login_info->friendcount = MAX_FRIEND_NUM;
    }
    if (user_login_info->blackcount > MAX_FRIEND_NUM) {
        ERROR_LOG("severe db error[blackcount]: uid=%u frdcnt=%u", p->uid, user_login_info->blackcount);
        user_login_info->blackcount = MAX_FRIEND_NUM;
    }

    *(uint32_t*)p->tmpinfo = user_login_info->vip_flag;
    *(uint32_t*)(p->tmpinfo + 4) = user_login_info->friendcount;
    *(uint32_t*)(p->tmpinfo + 8) = user_login_info->blackcount;
    memcpy(p->tmpinfo + 12, user_login_info->frienditem, 4 * (user_login_info->friendcount + user_login_info->blackcount));

    // add the time stamp
    /*if (user_login_info->friendcount != 0) {
        // get the time stamp from time server 

        static uint8_t login_timestamp_sendbuf[BUFFER_SIZE];
        int idx = sizeof(login_timestamp_pk_header_t);
        *(uint32_t*)login_timestamp_sendbuf = p->uid;
        taomee::pack(login_timestamp_sendbuf, user_login_info->friendcount, idx);
        for (uint32_t i = 0; i != user_login_info->friendcount; ++i) {
            taomee::pack(login_timestamp_sendbuf, user_login_info->frienditem[i], idx);
        }
        init_timestamp_head(p, login_timestamp_sendbuf, idx, TIMESTAMP_GET_FRIENDTIME);
        send_to_timestamp(p, login_timestamp_sendbuf, idx);

        static uint32_t friends[MAX_FRIEND_NUM + 2];
        friends[0] = p->uid;
        friends[1] = user_login_info->friendcount;
        memcpy(&(friends[2]), user_login_info->frienditem, sizeof(uint32_t) * user_login_info->friendcount);

        int pkg_len = sizeof(uint32_t) * (user_login_info->friendcount + 2); 
        multicast_player_login((uint8_t*)friends, pkg_len);
        return 0;
    }*/

    return get_recommended_svr_list(p);
}

/**
  * @brierf Send a request package to switch server, then get the respond, but do not parse it.
  * @param  uid. user id.
  * @param    friendcount. haw many friends.
  * @param    allfriendid. store the ids in an array
  * @return     int. 0 on success. less than 0 on error.
  */
int get_recommended_svr_list(usr_info_t* p)
{
    TRACE_LOG("enter get_recommended_svr_list");
    // pack up the request data
    int idx = sizeof(svr_proto_t);
    uint32_t friendcount = *(uint32_t*)(p->tmpinfo + 4);
    taomee::pack_h(login_switch_sendbuf, static_cast<uint16_t>(my_opt.dx_or_wt), idx);
    taomee::pack_h(login_switch_sendbuf, static_cast<uint8_t>(1), idx);
    taomee::pack_h(login_switch_sendbuf, friendcount, idx);
    taomee::pack(login_switch_sendbuf, (void*)(p->tmpinfo + 12), friendcount * sizeof(userid_t), idx);
    init_switch_head(p, login_switch_sendbuf, idx, SWITCH_GET_RECOMMEND_SVR_LIST);
    return send_to_switch(p, login_switch_sendbuf, idx);
}

int send_recommeded_svr_list(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt, int max_online_id)
{
    int idx = sizeof(cli_login_pk_header_t);
    taomee::pack(my_cli_buf, max_online_id, idx);
    taomee::pack(my_cli_buf, *(uint32_t*)p->tmpinfo, idx);
    taomee::pack(my_cli_buf, online_cnt, idx);
    TRACE_LOG("max_online=%u online_cnt=%u",max_online_id,online_cnt);
    for (uint32_t i = 0; i != online_cnt; ++i) {
        taomee::pack(my_cli_buf, svrs[i].id, idx);
        taomee::pack(my_cli_buf, svrs[i].users, idx);
        taomee::pack(my_cli_buf, svrs[i].ip, 16, idx);
        taomee::pack(my_cli_buf, svrs[i].port, idx);
        taomee::pack(my_cli_buf, svrs[i].friends, idx);
    	TRACE_LOG("online=[%u %u %16s %u %u]",svrs[i].id,svrs[i].users,svrs[i].ip,svrs[i].port,svrs[i].friends);
    }

    // pkg the frd id and timestamp
    uint32_t friendcount = *(uint32_t*)(p->tmpinfo + 4);
    uint32_t blackcount = *(uint32_t*)(p->tmpinfo + 8);
    //uint32_t* frd_timestamp = (uint32_t*)(p->tmpinfo + 8 + sizeof(userid_t) * MAX_FRIEND_NUM);
    uint32_t* friend_id = (uint32_t*)(p->tmpinfo + 12);
    uint32_t* black_id = (uint32_t*)(p->tmpinfo + 12 + sizeof(userid_t) * friendcount);

    taomee::pack(my_cli_buf, friendcount, idx);
    TRACE_LOG("friend_cnt=%u",friendcount);
    for (uint32_t i = 0; i < friendcount; i++ ) {
        taomee::pack(my_cli_buf, friend_id[i], idx);
    	TRACE_LOG("friend_id=%u",friend_id[i]);
        //taomee::pack(my_cli_buf, frd_timestamp[i], idx);
    }
    taomee::pack(my_cli_buf, blackcount, idx);
    TRACE_LOG("black_cnt=%u",blackcount);
    for (uint32_t i = 0; i < blackcount; i++ ) {
        taomee::pack(my_cli_buf, black_id[i], idx);
    	TRACE_LOG("black_id=%u",black_id[i]);
        //taomee::pack(my_cli_buf, black_timestamp[i], idx);
    }

    init_proto_head(my_cli_buf, idx, PROTO_GET_RECOMMEND_SVR_LIST, p->uid, SUCCESS);

#ifndef TW_VER  
    statistic_info_t info;
    uint32_t msglog_type;
    info.user_id = p->uid;
    msglog_type = MOLE_LOGIN_OFFSET + p->channel;
	if (p->channel > 56) 
		msglog_type = MOLE_NOT_NORMAL_LOG_CHANNEL;
    statistic_msglog(msglog_type, (void *)&info, sizeof info);
#endif

	return send_to_self(p, my_cli_buf, idx);
}

int get_recommeded_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
    TRACE_LOG("get recommend callback");
    CHECK_VAL_GE(bodylen, sizeof(svr_list_t));
    svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);
    CHECK_VAL(bodylen, sizeof(svr_list_t) + sizeof(svr_info_t) * svrlist->online_cnt);
    uint32_t max_online_id = svrlist->max_online_id;
    return send_recommeded_svr_list(p, svrlist->svrs, svrlist->online_cnt, max_online_id);
}

int get_friend_time_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
    uint32_t friendcount = *(uint32_t*)(p->tmpinfo + 4);
    uint32_t* friend_timestamp= (uint32_t*)(p->tmpinfo + 8 + sizeof(uint32_t) * MAX_FRIEND_NUM);
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
    } else { //failed ,return now time to friend time
        TRACE_LOG("get timestamp failed, info is as follows:");
        error = 1;
    }
    if(error) {
        for(uint32_t i = 0; i != MAX_FRIEND_NUM; i++) {
            friend_timestamp[i] = 0;
        }
    }
    return get_recommended_svr_list(p);
}

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

/** 
 * @brief verify if the DESed session from the user is correct
 * @param const sprite_t*, the user that is currently logging in
 * @param const login_session_t*, UnDESed session
 *
 * @return int, 0 on success, otherwise failure
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

int get_recommeded_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
    CHECK_VAL(bodylen, 24);
    char *sessbody = (char*)body;
    p->channel = ntohl(*(uint32_t*)(body + SESS_LEN));
	uint32_t role_tm = ntohl(*(uint32_t*)(body + SESS_LEN + 4));

    gf_login_out *user_login_info = reinterpret_cast<gf_login_out*>(p->tmpinfo);

    user_login_info->vip_flag    = 0;
    user_login_info->friendcount = 0;
    DEBUG_LOG("RECOMMD SVR\t[uid=%d,role_tm=%u]", p->uid, role_tm);
    if (p->uid != 0) {
        login_session_t sess;
        //varify the session
        unpkg_auth((uint8_t*)sessbody, 16, &sess);
        if ( verify_session(p->uid, &sess) != SUCCESS ) {
            clear_usr_info(p);
            return CLOSE_CONN;
        }

        return send_request_to_db(p, DB_LOGIN_GF, &role_tm, sizeof(role_tm));
    } else {
        return get_recommended_svr_list(p);
    }
}

/**
  * @brierf Send a request package to switch server, then get the respond, but do not parse it.
  * @param  uid. user id.
  * @param    friendcount. haw many friends.
  * @param    allfriendid. store the ids in an array
  * @return     int. 0 on success. less than 0 on error.
  */
int get_ranged_svr_list(usr_info_t* p, int start_id, int end_id)
{
    TRACE_LOG("enter get_ranged_svr_list");
    // pack up the request data
	int friendcount = 0;
    int idx = sizeof(svr_proto_t);
    taomee::pack_h(login_switch_sendbuf, static_cast<uint16_t>(0), idx);
    taomee::pack_h(login_switch_sendbuf, start_id, idx);
    taomee::pack_h(login_switch_sendbuf, end_id, idx);
    taomee::pack_h(login_switch_sendbuf, friendcount, idx);
    //taomee::pack(login_switch_sendbuf, allfriendid, friendcount * sizeof(userid_t), idx);
    init_switch_head(p, login_switch_sendbuf, idx, SWITCH_GET_RANGED_SVR_LIST);
    return send_to_switch(p, login_switch_sendbuf, idx);
}

int get_ranged_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	TRACE_LOG("enter get_ranged_svr_list_cmd");
	//check protocol defined data, to prevent invalid access.
	CHECK_VAL_GE(bodylen, 12); 

	//get the client's request package body
	//uint32_t friendcount;
	uint32_t startid;
	uint32_t endid;
	int idx = 0;
	
	taomee::unpack(body, startid, idx);
	taomee::unpack(body, endid, idx);
	//if id is invalid, then make notification to the flash client.
	if ((startid < MIN_VALID_SVR) || (endid > MAX_VALID_SVR)
            || (startid > endid) || ((endid - startid) > 100)) {
        INFO_LOG("INVALID SVR ID\t[startid=%u endid=%u uid=%u]", startid, endid, p->uid);
        int l = sizeof(cli_login_pk_header_t);
        taomee::pack(my_cli_buf, 0, l);
        init_proto_head(my_cli_buf, l, p->waitcmd, p->uid, SUCCESS);
        return send_to_self(p, my_cli_buf, l);
    }
	DEBUG_LOG("RANGED SVR\t[uid=%u startid=%u endid=%u]", p->uid, startid, endid);
	/*taomee::unpack(body, friendcount, idx);
	if (friendcount > MAX_FRIEND_NUM) {
		ERROR_LOG("[friendcount=%u] is invalid",friendcount) ;
		return CLOSE_CONN;
	}
	// according to the friend count, check the size of the buffer
	CHECK_VAL(static_cast<size_t>(bodylen), 12 + (friendcount * sizeof(userid_t)));
	// unpack friend list
	static uint32_t allfriendid[MAX_FRIEND_NUM];
	for (uint32_t i = 0; i != friendcount; ++i) {
		taomee::unpack(body, allfriendid[i], idx);
	}*/

	//make a request to switch server, and get the response data.
	return get_ranged_svr_list(p, startid, endid);

}

int send_ranged_svrlist(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt)
{
	//make a response package to the flash client
	int idx = sizeof(cli_login_pk_header_t);
	taomee::pack(my_cli_buf, online_cnt, idx);
	for (uint32_t i = 0; i != online_cnt; ++i) {
		taomee::pack(my_cli_buf, svrs[i].id, idx);
		taomee::pack(my_cli_buf, svrs[i].users, idx);
		taomee::pack(my_cli_buf, svrs[i].ip, 16, idx);
		taomee::pack(my_cli_buf, svrs[i].port, idx);
		taomee::pack(my_cli_buf, svrs[i].friends, idx);
    	TRACE_LOG("online=[%u %u %16s %u %u]",svrs[i].id,svrs[i].users,svrs[i].ip,svrs[i].port,svrs[i].friends);
	}
	init_proto_head(my_cli_buf, idx, PROTO_GET_RANGED_SVR_LIST, p->uid, SUCCESS);
    return send_to_self(p, my_cli_buf, idx);
}

int get_ranged_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
	//varify the respond data
    TRACE_LOG("get ranged callback");
	CHECK_VAL_GE(bodylen, sizeof(svr_list_t));

	svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);

    //DEBUG_LOG("svrlist [%d %d]", svrlist->domain_id, svrlist->online_cnt);
	//chk if the pkg len is valid
	CHECK_VAL(bodylen, sizeof(svr_list_t) + svrlist->online_cnt * sizeof(svr_info_t)); 

    return send_ranged_svrlist(p, svrlist->svrs, svrlist->online_cnt);
}

int get_backup_svr_list(usr_info_t* p)
{
	TRACE_LOG("enter get backup svr list");
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
