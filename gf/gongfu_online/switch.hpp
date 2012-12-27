/**
 *============================================================
 *  @file	  switch.hpp
 *  @brief	switch server related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_SWITCH_HPP_
#define KF_SWITCH_HPP_

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <stdint.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>
#include <libtaomee/log.h>

#include <libtaomee/project/stat_agent/msglog.h>

#include <async_serv/net_if.h>
}

#include "utils.hpp"

#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "fwd_decl.hpp"
#include "player.hpp"


#pragma pack(1)
/**
  * @brief switch protocol definition
  */
struct switch_proto_t {
	uint32_t	len;
	uint32_t	seq;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	sender_id;
	//uint32_t	sender_role_tm;
	uint8_t		body[];
};

struct across_svr_rltm_t {
	uint32_t type; // : 1 pve , 2 pvp
	uint32_t senduid;
};
#pragma pack()


typedef struct mail_header mail_header;
/**
  * @brief command id for switch protocol
  */
enum switch_cmd {
	sw_cmd_post_msg						= 60001,

	sw_cmd_vip_supplement               = 60003,
	sw_cmd_vip_add_point                = 60004,
	sw_cmd_vip_gold                     = 60005,
	sw_cmd_new_mail                     = 60007,
	sw_cmd_rand_reward                  = 60009,

	sw_cmd_init_pkg						= 61001,
	sw_cmd_user_login_noti				= 61002,
	sw_cmd_chk_if_online				= 61003,
	sw_cmd_chat_across_svr				= 61004,
	sw_cmd_attr_update_noti				= 61005,
	sw_cmd_chk_user_where				= 61006,
	sw_cmd_kick_user					= 61007,
	sw_cmd_keepalive					= 61008,

	sw_cmd_chat_across_svr_2			        = 61010,
	sw_cmd_send_msg                     = 61011,
	sw_cmd_get_users_status             = 61012,
	sw_cmd_update_user_url              = 61013,
	sw_cmd_chat_across_svr_rltm         = 61014,
	sw_cmd_update_paper_count          = 61015,

	sw_cmd_chat_request                 = 61020,
	sw_cmd_get_total_count              = 61021,
	sw_cmd_frd_change_notify            = 61022,

    sw_cmd_seer_SPT_event               = 63103,
	sw_last_cmd
	
	
};

enum {
	sw_rt_nodify_team_pve = 1,
	sw_rt_nodify_pvp = 2,
	sw_rt_nodify_pvp_no_refuse = 3,
	sw_rt_nodify_team_contest = 4,
};

enum {
	sw_rt_nodify_reply_status_refuse = 0,
	sw_rt_nodify_reply_status_accept = 1,
	sw_rt_nodify_reply_status_offline = 2,
	sw_rt_nodify_reply_status_btl	= 3,
	sw_rt_nodify_reply_status_trd 	= 4,
	sw_rt_nodify_reply_status_rookie = 5,
	sw_rt_nodify_reply_status_nor = 6,
	sw_rt_nodify_reply_set_conf_refuse = 7,
};

/**
  * @brief package size
  */
enum {
	swproto_max_len = 8192
};

/*! file descriptor for connection with switch server */
extern int switch_fd;

/**
  * @brief connect to switch server and send an initial pkg 
  */
void connect_to_switch();

/**
  * @brief intitiate the keepalive timer
  */
void init_sw_keepalive_timer();

/*
int sync_vip_op(player_t* initor, uint32_t rcverid, const void* buf, int len);
*/

/**
  * @brief connect to switch server every 30 senconds in case of disconnection
  * @return 0 on success and delete the original timer, -1 on error
  */
inline int connect_to_switch_timely(void* owner, void* data)
{
	if (!data) {
		ADD_TIMER_EVENT_EX(&g_events, connect_to_switch_timely_idx, reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);
	} else if (switch_fd == -1) {
		connect_to_switch();
		if (switch_fd == -1) {
			ADD_TIMER_EVENT_EX(&g_events, connect_to_switch_timely_idx, reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);
		}
	}
	return 0;
}


//---------------package sending and receiving interface with switch----------------------------------
/**
  * @brief uniform interface to send request to switch server
  * @param p sender 
  * @param cmd command id 
  * @param body_len package body length
  * @param body_buf package body
  * @param sender's id
  * @return 0 on success, -1 on error
  */
int send_to_switch(const player_t* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id);

/**
  * @brief uniform handler to process on return by switch server
  * @param data package data
  * @param len package length
  */
void handle_switch_return(switch_proto_t* data, uint32_t len);

//-------------------------------------------------------------------------------------------------


/**
  * @brief send a request to seer db switch
  * @param p the player who launches the request
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_to_seer_switch(const player_t* p, uint16_t cmd, const void* body, uint32_t body_len, userid_t sender_id);



//-------------------- Utils For Sending Pkg to Switch --------------------
//-------------------------------------------------------------------------
/**
  *  @brief send chatting pkg to switch server
  *  @param p sender
  *  @param body package body
  *  @param package body length
  */
inline void
chat_across_svr(const player_t* p, void* body, uint32_t bodylen)
{
	if (send_to_switch(0, sw_cmd_chat_across_svr, bodylen, body, p->id) == -1) {
		userid_t recvid = *(reinterpret_cast<userid_t*>(body));
		//uint32_t role_tm = *(reinterpret_cast<uint32_t*>(body) + 1);
		*(reinterpret_cast<uint32_t*>(body)) = bodylen;
		TRACE_LOG("chat_across_svr:id=[%u]",recvid);
		send_request_to_db(0, recvid, 0, dbproto_add_offline_msg, body, bodylen);
	}
}

/**
  *  @brief send chatting pkg to switch server
  *  @param p sender
  *  @param body package body
  *  @param package body length
  */
inline void
chat_across_svr_rltm(const player_t* p, uint32_t uid, void* body, uint32_t bodylen)
{
	if (send_to_switch(0, sw_cmd_chat_across_svr_rltm, bodylen, body, uid) == -1) {
		userid_t recvid = *(reinterpret_cast<userid_t*>(body));
		WARN_LOG("FAILED TO SW:recvid=[%u] sendid=[%u]", recvid, uid);
	}
}

/**
  *  @brief send msg to switch server, the 2nd version.
  *         in this revision, there is no db operation
  *  @param p the msg sender
  *  @param recv_id msg receiver
  */

inline void
chat_across_svr_2(const player_t* p, void* body, uint32_t bodylen)
{
	if (send_to_switch(0, sw_cmd_chat_across_svr_2, bodylen, body, p->id) == -1) {
		userid_t recvid = *(reinterpret_cast<userid_t*>(body));
		WARN_LOG("FAILED TO SW:recvid=[%u] sendid=[%u]", recvid, p->id);
	}
}




/**
  *  @brief check if users are on or off
  *  @param p sender
  *  @param body package body
  *  @param package body length
*/
inline void
chk_users_onoff_across_svr(player_t* p,  void* body, uint32_t bodylen)
{
	if (send_to_switch(p, sw_cmd_chk_if_online, bodylen, body, p->id) == -1) {
	int idx = sizeof(cli_proto_t);
	taomee::pack(pkgbuf, static_cast<uint32_t>(0), idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);
	}
}

/**
  *  @brief check user location
  *  @param p sender
  *  @param chkee_id user who is to be checked
*/
inline void
chk_user_where(player_t* p, userid_t chkee_id)
{
	//uint8_t buf[4];
	//int i = 0;
	//pack_h(buf, chkee_id, i);
	if (send_to_switch(p, sw_cmd_chk_user_where, sizeof(chkee_id), &chkee_id, p->id) == -1) {
		//rsp_search_user(chker, 0, 0);
	}
}

/**
  *  @brief notify switch when user login/logout
  *  @param p user who is on/off the online server
  *  @param login login flag  
*/
inline void
notify_user_login(player_t* p, uint8_t login)
{	
	TRACE_LOG("login flag : %u", login);
	int idx = 0;
	uint8_t buf[100];
	taomee::pack_h(buf, login, idx);
	taomee::pack_h(buf, p->role_tm, idx);
	taomee::pack_h(buf, p->role_type, idx);
	taomee::pack_h(buf, is_vip_player(p) ? p->vip_level : 0, idx);
	taomee::pack_h(buf, static_cast<uint8_t>(p->lv), idx);
	pack_h(buf, static_cast<uint8_t>(p->get_home_level()), idx);
	pack_h(buf, static_cast<uint8_t>(p->get_home_summon_count()), idx);

	send_to_switch(p, sw_cmd_user_login_noti, idx, buf, p->id);

	uint32_t buf1[4] = { 1, 0, p->id, 0 };
	uint32_t buf2[4] = { 0, 1, p->id, get_now_tv()->tv_sec - p->login_tm };
	//stat log :user on off
#ifdef VERSION_TAIWAN
    msglog(statistic_logfile, stat_log_user_on_off, get_now_tv()->tv_sec, login ? buf1 : buf2, sizeof(buf1));
#elif defined VERSION_KAIXIN
    msglog(statistic_logfile, stat_log_kx_user_on_off, get_now_tv()->tv_sec, login ? buf1 : buf2, sizeof(buf1));
#else
	msglog(statistic_logfile, stat_log_user_on_off, get_now_tv()->tv_sec, login ? buf1 : buf2, sizeof(buf1));

	if (config_get_intval("domain", 0)) {
	// wang tong network
		msglog(statistic_logfile, stat_log_wt_user_on_off, get_now_tv()->tv_sec, login ? buf1 : buf2, sizeof(buf1));
	} else {
		msglog(statistic_logfile, stat_log_dx_user_on_off, get_now_tv()->tv_sec, login ? buf1 : buf2, sizeof(buf1));
	}
#endif
	if (login) {
	//user on
		do_stat_log_universal_interface_2(stat_log_role_login_, p->role_type, p->id, 1);
	}
}

void notify_user_new_mail(player_t* p,  mail_header* header, uint32_t receive_id);

void notify_event_to_seer(player_t* p, uint32_t event_id);

//------------ Utilities for processing  packages received from switch--------------------

/**
  * @brief post system message to client
  * @param buf package body
  * @param len package body length
  */
int post_msg_op(void* buf, uint32_t len);

/**
  * @brief process on attribute update notification package
  * @param data package data
  * @param len package length
  */
int attr_update_op(player_t* p, userid_t rcverid, void* buf, uint32_t len);

/**
  * @brief process on chatting across online servers package 
  * @param buf package body
  * @param len package body length
  */
int chat_across_svr_callback(void* buf, uint32_t len);

/**
  * @brief process on chatting across online servers package 
  * @param buf package body
  * @param len package body length
  */
int chat_across_svr_rltm_callback(void* buf, uint32_t len);

/**
  * @brief process on post msg across online servers package(the 2nd version)
  * @param buf package body
  * @param len package body length
  */
int chat_across_svr_2_callback(void* buf, uint32_t len);

/**
  * @brief process on kickingx-user-offline package 
  * @param buf package body returned by switch
  * @param len package body length
  */
int kick_user_offline_callback(void* buf, uint32_t len);

/**
  * @brief process check users onoff status package returned by switch 
  * @param p sender 
  * @param body package body returned by switch
  * @param bodylen package body length
  */
int chk_users_onoff_across_svr_callback(player_t* p, void* body, uint32_t bodylen);

int user_vip_supplement_callback(player_t* p, void* body, uint32_t bodylen);

int user_vip_add_point_callback(player_t* p, void* body, uint32_t bodylen);

int user_vip_gold_callback(player_t* p, void* body, uint32_t bodylen);

int user_new_mail_callback(player_t* p, void* body, uint32_t bodylen);

int send_sw_keepalive_pkg(void *owner, void *data);

int do_sw_rand_reward_logic(player_t* p, void* body, uint32_t bodylen);
#endif // KF_SWITCH_HPP_

