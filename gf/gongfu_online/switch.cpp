#include <cstring>

#include "dbproxy.hpp"

#include "switch.hpp"
#include <assert.h>
#include "login.hpp"
#include "vip_impl.hpp"
#include "mail.hpp"
#include "stat_log.hpp"
#include "global_data.hpp"
#include "message.hpp"
#include "fight_team.hpp"
#include "chat.hpp"
#include "temporary_team.hpp"


using namespace taomee;

/*! file descripter of connection with switch server */
//int switch_fd = -1;

//------------------------------inline function declaration---------------------------------------
/**
  * @brief send an initialization package to switch server
  */
inline void send_init_pkg();

/**
  * @brief home send initialization package to switch server
  */
inline void send_home_init_pkg();


/**
  * @brief send keepalive package to switch
  * @param owner just ignore it
  * @param data just ignore it
  * @return 0
  */
//inline int send_sw_keepalive_pkg(void* owner, void* data);
//-------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
/**
  * @brief connect to switch and send an init package of online information when online is launched
  */
void connect_to_switch()
{
	switch_fd = connect_to_service(config_get_strval("switch_name"),
										0, 65535, 1);
	if (switch_fd != -1) {
		KDEBUG_LOG(0, "BEGIN CONNECTING TO SWITCH AND SEND INIT PKG");

		send_init_pkg();
	}
}

/**
  * @brief intitiate the keepalive timer
  */
void init_sw_keepalive_timer()
{
	ADD_TIMER_EVENT_EX(&g_events, send_sw_keepalive_pkg_idx, 0, get_now_tv()->tv_sec + 30);
}

/**
  * @brief send an init pkg to switch server 
  */
inline void send_init_pkg()
{
	KDEBUG_LOG(0, "SEND INIT PKG");
	int idx = 0;
	uint8_t pkgbody[28];
	
	pack_h(pkgbody, static_cast<uint16_t>(config_get_intval("domain", 0)), idx);
	pack_h(pkgbody, get_server_id(), idx);
	pack(pkgbody, config_cache.bc_elem->bind_ip, 16, idx);
	pack_h(pkgbody, static_cast<uint16_t>(config_cache.bc_elem->bind_port), idx);
 	pack_h(pkgbody, static_cast<uint32_t>(player_num), idx); //user number of the online server
	
	send_to_switch(0, sw_cmd_init_pkg, sizeof pkgbody, pkgbody, 0);   // sender id 0
}

/**
  * @brief send a keepalive package to switch
  * @param owner just ignore it
  * @param data just ignore it
  * @return 0 on success and delete the original timer
  */
int send_sw_keepalive_pkg(void* owner, void* data)
{
	TRACE_LOG("send a switch to switch keep ...alive");
	send_to_switch(0, sw_cmd_keepalive, 0, 0, 0);   // send keepalive header package to switch
	ADD_TIMER_EVENT_EX(&g_events, send_sw_keepalive_pkg_idx, 0, get_now_tv()->tv_sec + 30);

	return 0;
}

//---------------package sending and receiving interface with switch----------------------------------
/**
  * @brief uniform interface to send request to switch server
  * @param p sender 
  * @param cmd command 
  * @param body_len package body length
  * @param sender_id package sender id
  * @return 0 on success, -1 on ret_valor  
  */
int send_to_switch(const player_t* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id)
{
	//Common buffer for sending package to switch
	static uint8_t sw_pkg[swproto_max_len];

	//Connect to switch and send init package in case of disconnection
	if (switch_fd == -1) {
		connect_to_switch();
	}
	
	int len = sizeof(switch_proto_t) + body_len;
	if ((switch_fd == -1) || (body_len > static_cast<int>(sizeof(sw_pkg) - sizeof(switch_proto_t)))) {	
		ERROR_LOG("FAILED SENDING PACKAGE TO SWITCH\t[switch_fd=%d]", switch_fd);
		return -1;
	}

	//Build the package
	switch_proto_t* pkg = reinterpret_cast<switch_proto_t*>(sw_pkg);
	pkg->len = len;
	pkg->seq = (p ? ((p->fd) << 16) | (p->waitcmd) : 0); // set higher 16 bytes fd and lower 16 bytes waitcmd
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->sender_id  = sender_id;
	//pkg->sender_role_tm  = role_tm;
	memcpy(pkg->body, body_buf, body_len);

	if (p) {
		KDEBUG_LOG(p->id, "SEND REQ TO SWITCH\t[uid=%u cmd=%u waitcmd=%u]", p->id, cmd, p->waitcmd);
	}
	//assert( ntohl(p->waitcmd) != 10042 );
	//TRACE_LOG("SEND TO SWITCH, fd=%u, cmd=%u", switch_fd, pkg->cmd);
	return net_send(switch_fd, sw_pkg, len);
}

/**
  * @brief send a request to seer db switch
  * @param p the player who launches the request
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_to_seer_switch(const player_t* p, uint16_t cmd, uint32_t body_len, const void* body, userid_t sender_id, uint8_t domain)
{
	static uint8_t seerbuf[swproto_max_len];

	if (seer_fd[domain] == -1) {
		/* TODO: 支持 connect_to_service() */
		//vipsvr_id = connect_to_service(config_get_strval("service_vipsvr"), 0, 65535, 1);
		seer_fd[domain] = connect_to_svr(config_get_strval(seer_ip[domain]), config_get_intval(seer_port[domain], 0), 65535, 1);
	}

    int len = sizeof(switch_proto_t) + body_len;
	if ((seer_fd[domain] == -1) || (body_len > (sizeof(seerbuf) - sizeof(switch_proto_t)))) {
		ERROR_LOG("send to seersvr failed: fd=%d len=%d", seer_fd[domain], body_len);
        return 0;
	}

    //Build the package
	switch_proto_t* pkg = reinterpret_cast<switch_proto_t*>(seerbuf);
	pkg->len = len;
	pkg->seq = (p ? ((p->fd) << 16) | (p->waitcmd) : 0); // set higher 16 bytes fd and lower 16 bytes waitcmd
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->sender_id  = sender_id;
	memcpy(pkg->body, body, body_len);

	if (p) {
		KDEBUG_LOG(p->id, "SEND REQ TO SEER SWITCH\t[uid=%u cmd=%u waitcmd=%u fd=%u len=%u]", p->id, cmd, p->waitcmd, seer_fd[domain], pkg->len);
	}

	return net_send(seer_fd[domain], seerbuf, pkg->len);
}


/**
  * @brief uniform handler to process on return by switch server
  * @param data package data
  * @param len package length
  */
void handle_switch_return(switch_proto_t* data, uint32_t len)
{
	KDEBUG_LOG(0, "SWITCH RETURN\t[connfd=%u,id=%u]",data->seq>>16,data->sender_id);
	player_t* p = 0;
	uint32_t waitcmd;
	if (data->seq) {
		waitcmd = data->seq & 0xFFFF; // lower 16 bytes
		int	  connfd  = data->seq >> 16; //higher 16 bytes

		p = get_player_by_fd(connfd);
		if (!p || p->waitcmd != waitcmd) {
			ERROR_LOG("process connection to switch error\t[uid=%u cmd=%u seq=%X]",
				data->sender_id, waitcmd, data->seq);
			return;								
		}
	}

	uint32_t bodylen = data->len - sizeof(switch_proto_t);

	switch (data->cmd) {
	case sw_cmd_post_msg:
		post_msg_op(data->body, bodylen); //process on amdin's posting msg
		break;	
	case sw_cmd_chat_across_svr: 
		chat_across_svr_callback(data->body, bodylen);
		break;
	case sw_cmd_chat_across_svr_2:
		chat_across_svr_2_callback(data->body, bodylen);
		break;
	case sw_cmd_chat_across_svr_rltm: 
		chat_across_svr_rltm_callback(data->body, bodylen);
		break;
	case sw_cmd_kick_user: 
		kick_user_offline_callback(data->body, bodylen);
		break;
	case sw_cmd_chk_if_online:	 
		chk_users_onoff_across_svr_callback(p, data->body, bodylen);
		break;
	case sw_cmd_vip_supplement:
		{
			p = get_player(data->sender_id);
			if(p){
				user_vip_supplement_callback(p, data->body,  bodylen);
			}
		}
		break;

	case sw_cmd_vip_add_point:
		{
			p = get_player(data->sender_id);
			if(p){	
				user_vip_add_point_callback(p, data->body, bodylen);
			}
		}
		break;

	case sw_cmd_vip_gold:
		{
			p = get_player(data->sender_id);
			if(p){
				user_vip_gold_callback(p, data->body, bodylen);
			}
		}
		break;
	
	case sw_cmd_new_mail:
		{
			p = get_player(data->sender_id);
		 	if(p){
				user_new_mail_callback(p, data->body, bodylen);
			}		
		}
		break;
    case sw_cmd_rand_reward:
        {
            p = get_player(data->sender_id);
            if (p) {
                do_sw_rand_reward_logic(p, data->body, bodylen);
            } else {
	            ERROR_LOG("[%u] not found !!!!! ", data->sender_id);
            }
        }
        break;
	
	default:
		ERROR_LOG("Unsupported cmd to switch\t[id=%u cmd=%u]", data->sender_id, data->cmd);
		break;
	}
}
//---------------------------------------------------------------------------------------------------
void notify_event_to_seer(player_t* p, uint32_t event_id)
{
	KDEBUG_LOG(p->id, "SEER EVENT \t %u", event_id);
	int idx = 0;
	uint8_t buf[1024] = {0};
	taomee::pack_h(buf, event_id, idx);
	send_to_seer_switch(p, sw_cmd_seer_SPT_event, idx, buf, p->id, 0);
	send_to_seer_switch(p, sw_cmd_seer_SPT_event, idx, buf, p->id, 1);
}

//---------------------------------------------------------------------------------------------------

void notify_user_new_mail(player_t* p,  mail_header* header, uint32_t receive_id)
{
	int idx = 0;
	uint8_t buf[1024] = {0};
	taomee::pack_h(buf, header->mail_id, idx);
	taomee::pack_h(buf, header->mail_time, idx);
	taomee::pack_h(buf, header->mail_state, idx);
	taomee::pack_h(buf, header->mail_templet, idx);
	taomee::pack_h(buf, header->mail_type, idx);
	taomee::pack_h(buf, header->sender_id, idx);
	taomee::pack_h(buf, header->sender_role_tm, idx);
	taomee::pack(buf, header->mail_title, sizeof(header->mail_title), idx);
	send_to_switch(NULL, sw_cmd_new_mail, idx, buf, receive_id);
}

//------------ Utilities for processing  packages received from switch--------------------

int user_new_mail_callback(player_t* p, void* body, uint32_t bodylen)
{
	mail_header header;	
	memset(&header, 0, sizeof(header));
	int idx = 0;
	unpack_h(body, header.mail_id, idx);
	unpack_h(body, header.mail_time, idx);
	unpack_h(body, header.mail_state, idx);
	unpack_h(body, header.mail_templet, idx);
	unpack_h(body, header.mail_type, idx);
	unpack_h(body, header.sender_id, idx);
	unpack_h(body, header.sender_role_tm, idx);
	unpack(body, header.mail_title, sizeof(header.mail_title), idx);
	add_player_mail(p, header);	
	return send_player_new_mail(p, &header);
}
/*
inline void do_stat_log_user_vip_supplement(player_t *p)
{
	int idx = 0;
	uint32_t tm = time(0);
	pack_h(statlogbuf, tm, idx);
   	pack(statlogbuf, p->nick, NICK_SIZE, idx);	
	msglog(statistic_logfile, stat_log_proto_vip_supplement_cmd, get_now_tv()->tv_sec, statlogbuf, idx);
}
*/
int user_vip_supplement_callback(player_t* p, void* body, uint32_t bodylen)
{
	uint32_t begin_vip_time = 0;
	uint32_t end_vip_time = 0;
	uint32_t is_vip = 0;
	uint32_t auto_pay = 0;
	uint32_t month_count = 0;
	int i = 0;
	unpack_h(body, is_vip, i);
   	unpack_h(body, month_count, i);
	unpack_h(body, auto_pay, i);	
	unpack_h(body, begin_vip_time, i);
	unpack_h(body, end_vip_time, i); 	
	KDEBUG_LOG(p->id, "VIP SUPPLEMENT\t player_id:%u,begin_vip_time:%u,end_vip_time:%u is_vip:%u,auto_pay:%u,month_count:%u", 
				p->id, begin_vip_time, end_vip_time, is_vip, auto_pay, month_count);	
	
	set_player_vip_info(p, is_vip, month_count, begin_vip_time, end_vip_time);
	if (is_vip)
	{
		do_stat_log_user_vip_supplement(p);
	}
	return 0;
}

int user_vip_add_point_callback(player_t* p, void* body, uint32_t bodylen)
{
	uint32_t vip_point = 0;
	uint16_t syn_channel  = 0;
	int i = 0;

	unpack_h(body,  vip_point, i);
	unpack_h(body,  syn_channel, i);
	set_player_vip_point(p,  vip_point);

	KDEBUG_LOG(p->id, "VIP ADD POINT\t player_id:%u, vip_point:%u, syn_cl:%u", p->id, vip_point, syn_channel);

	return 0;
}

int user_vip_gold_callback(player_t* p, void* body, uint32_t bodylen)
{
	return 0;
}


/**
  * @brief post system message to client
  * @param buf package body
  * @param len package body length
  */
int post_msg_op(void* buf, uint32_t len)
{
	userid_t recvid = *reinterpret_cast<userid_t*>(buf); 
	if (recvid == 0) { //broadcast
		send_to_all(reinterpret_cast<uint8_t*>(buf) + sizeof(userid_t), len - sizeof(userid_t));
	} else {
		player_t* p = get_player(recvid);
		if (p) {
			return send_to_player(p,  reinterpret_cast<uint8_t*>(buf) + sizeof(userid_t), len - sizeof(userid_t), 0);
		} else {
			*reinterpret_cast<uint32_t*>(buf) = len;
			return send_request_to_db(0, recvid, 0, dbproto_add_offline_msg, buf, len); //offline msg
		}
	}
	
	return 0;
}

/**
  * @brief process on attribute update notification package
  * @param data package data
  * @param len package length
  */
int attr_update_op(player_t* p, userid_t rcverid, void* buf, uint32_t len)
{
	//CHECK_BODY_LEN(len, 8);
 /*
	int i = 0;
	uint32_t rcvid, type;
	UNPKG_H_UINT32(buf, rcvid, i);
	player_t* rcv = get_player(rcvid);
	if (rcv) {
		UNPKG_H_UINT32(buf, type, i);
		i = sizeof(protocol_t);
		PKG_UINT32(msg, type, i);
		init_proto_head(msg, PROTO_UPD_ATTR_NOTI, i);
		//send_to_self(rcv, msg, i, 0);

	}
*/
	return 0;
}
 
/**
  * @brief process on chatting across online servers package 
  * @param buf package body
  * @param len package body length
  */
int chat_across_svr_callback(void* buf, uint32_t len)
{
	CHECK_VAL_GE(len, sizeof(userid_t));

	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	if (rcver) {
		TRACE_LOG("send to player ,rcvid=%u",rcvid);
		send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
	} else {
		TRACE_LOG("send to db ,rcvid=%u",rcvid);
		*(reinterpret_cast<uint32_t*>(buf)) = len;
		send_request_to_db(0, rcvid, 0, dbproto_add_offline_msg, buf, len);
	}

	return 0;
}

/**
  * @brief process on chatting across online servers package 
  * @param buf package body
  * @param len package body length
  */
int chat_across_svr_rltm_callback(void* buf, uint32_t len)
{
	CHECK_VAL_GE(len, sizeof(userid_t));

	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	TRACE_LOG("send to player ,rcvid=%u", rcvid);
	if (rcver) {
		cli_proto_t* proto = reinterpret_cast<cli_proto_t*>((reinterpret_cast<uint8_t*>(buf)) + 4);
		talk_msg_out_head_t* p_out = reinterpret_cast<talk_msg_out_head_t*>(proto->body);
		across_svr_rltm_t rsp = {0};
		int idx = 0;
		unpack(proto->body, rsp.type, idx);
		unpack(proto->body, rsp.senduid, idx);

		uint8_t* p_tmp = reinterpret_cast<uint8_t*>(buf) + 4;
		idx = 4;
		uint16_t tmp_cmd = 0;
		unpack(p_tmp, tmp_cmd, idx);
		TRACE_LOG("send to player ,rcvid=%u, cmd=%u %s", rcvid, tmp_cmd, p_out->from_nick);
		if (tmp_cmd == cli_proto_tmp_team_invite_msg_2_invitee) {
			return on_invitee_recv_invite_msg(buf, len);	//临时队伍：接收到对战邀请入队时	
		} else if (tmp_cmd == cli_proto_noti_tmp_team_info_changed) {
			return on_tmp_team_member_changed(buf, len);//临时队伍：队伍信息变化时，为了同步成员，只有队长发出
		} else if (tmp_cmd == cli_proto_tmp_team_noti_reply_result) { 
			return on_tmp_team_inviter_recv_reply_msg(buf, len);//临时队伍：邀请入团，队长接收到队员返回时
		} else if (tmp_cmd == cli_proto_tmp_team_member_leave) {
			return on_tmp_team_player_recv_self_leave_msg(buf, len);//临时队伍：队员收到关于自己离开队伍的通知包，有自己离开，被踢，解散队伍等
		} else if (tmp_cmd == cli_proto_tmp_team_member_leave_msg_2_leader) {
			return on_team_leader_recv_member_leave_msg(buf, len);//临时队伍：队长收到队员的离开消息，队长需要同步信息给其它
		} else if (tmp_cmd == cli_proto_send_tmp_team_pvp_invite_2_member) {
			return on_tmp_team_member_recv_pvp_invite(buf, len);
		} else if (tmp_cmd == cli_proto_tmp_send_team_reply_pvp_invite) {
			return on_tmp_team_member_recv_reply_pvp_invite(buf, len);
		} else if (tmp_cmd == cli_proto_tmp_team_pvp_start) {
			return on_tmp_team_recv_pvp_rsp(buf, len);
		} else if (tmp_cmd == cli_proto_tmp_team_status) {
			return on_tmp_team_recv_team_status_rsp(buf, len);;
		} else if (tmp_cmd!= cli_proto_reply_btl_invite_notify) {
			if (rsp.type == sw_rt_nodify_team_pve || rsp.type == sw_rt_nodify_pvp) {
			//reply invite battle
				if (rcver->trade_grp) {
					send_rltm_notification(cli_proto_reply_btl_invite_notify, rsp.senduid, rcver, 
						rsp.type, sw_rt_nodify_reply_status_trd, 0);
					return 0;
				}
				if (rcver->battle_grp) {
					send_rltm_notification(cli_proto_reply_btl_invite_notify, rsp.senduid, rcver, 
						rsp.type, sw_rt_nodify_reply_status_btl, 0);
					return 0;
				}

				if (rcver->client_buf[4] && rsp.type == sw_rt_nodify_pvp && !is_player_have_friend(rcver, rsp.senduid)) {
					send_rltm_notification(cli_proto_reply_btl_invite_notify, rsp.senduid, rcver, 
						rsp.type, sw_rt_nodify_reply_set_conf_refuse, 0);
					return 0;
				}
				send_rltm_notification(cli_proto_reply_btl_invite_notify, rsp.senduid, rcver, 
					rsp.type, sw_rt_nodify_reply_status_nor, 0);
			}
		}
		send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
	} 
	return 0;
}


/**
  * @brief process on post msg across online servers package(the 2nd version)
  * @param buf package body
  * @param len package body length
  */
int chat_across_svr_2_callback(void* buf, uint32_t len)
{
	CHECK_VAL_GE(len, sizeof(userid_t));

	userid_t  rcvid = *(reinterpret_cast<userid_t*>(buf));
	player_t* rcver = get_player(rcvid);
	if (rcver) {
		uint16_t tmp_cmd = 0;
		int idx = 8;
		unpack(buf, tmp_cmd, idx);
		if (tmp_cmd == cli_proto_send_team_member_changed) {
			db_chat_across_team_member_changed(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4 + sizeof(cli_proto_t));
		} else if (tmp_cmd == cli_proto_synchro_team_info) {
            chat_across_team_info( rcver, (reinterpret_cast<uint8_t*>(buf)) + 4 + sizeof(cli_proto_t) );
        } else if (tmp_cmd == cli_proto_invite_prentice) {
            if (is_player_have_master(rcver)) return 0;
        //} else if (tmp_cmd == cli_proto_safe_trade_set_item) {
        //    chat_across_safe_trade_peer_set_item(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4 + sizeof(cli_proto_t));
        }
		KDEBUG_LOG(rcver->id, "%u", tmp_cmd);	
		send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
	} 
	return 0;
}

/**
  * @brief process on kicking-user-offline package 
  * @param buf package body returned by switch
  * @param len package body length
  */
int kick_user_offline_callback(void* buf, uint32_t len)
{
	CHECK_VAL_EQ(len, sizeof(userid_t) + 4);

	userid_t obj_id;
	uint32_t reason;
	int i = 0;
	unpack_h(buf, obj_id, i);
	unpack_h(buf, reason, i);
	player_t* obj = get_player(obj_id); //The user who is to be kicked
	if (obj) {
		// send multiple login notification
		send_header_to_player(obj, cli_proto_login, reason, 0);
		// kick the previous login instance offline
		close_client_conn(obj->fd);
	} 

	return 0;
}

/** 
  * @brief process check users onoff status package returned by switch 
  * @param p sender 
  * @param body package body returned by switch
  * @param bodylen package body length
  */
int chk_users_onoff_across_svr_callback(player_t* p, void* body, uint32_t bodylen)
{
	CHECK_VAL_GE(bodylen, 4);
	
	struct usr_onoff_t {
		userid_t uid;
		uint32_t role_tm;
		uint32_t role_type;
		uint32_t online_id;
		uint32_t  vip_lv;
		uint32_t  lv;
		uint32_t  home_lv;
		uint32_t  home_pet_count;
	} __attribute__((packed));

	int idx = sizeof(cli_proto_t);
	uint32_t count = *reinterpret_cast<uint32_t*>(body);

	CHECK_VAL_EQ(bodylen, 4 + count * (sizeof(usr_onoff_t))); 
	pack(pkgbuf, count, idx);

	usr_onoff_t* usr_onoff = reinterpret_cast<usr_onoff_t*>(reinterpret_cast<uint8_t*>(body) + 4);
	for (uint32_t i = 0; i < count; i++) 
	{
		pack(pkgbuf, usr_onoff->uid, idx);	
		pack(pkgbuf, usr_onoff->role_tm, idx);	
		pack(pkgbuf, usr_onoff->role_type, idx);	
		pack(pkgbuf, usr_onoff->lv, idx);	
		pack(pkgbuf, usr_onoff->vip_lv, idx);
		pack(pkgbuf, usr_onoff->online_id, idx);
		pack(pkgbuf, usr_onoff->home_lv, idx);
		usr_onoff++;
	}
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int do_sw_rand_reward_logic(player_t* p, void* body, uint32_t bodylen)
{
    CHECK_VAL_EQ(bodylen, 8);
    uint32_t role_tm = *reinterpret_cast<uint32_t*>(body);
    uint32_t reward_id = *reinterpret_cast<uint32_t*>((uint8_t*)(body) + 4);

    KDEBUG_LOG(p->id, "SW ---> reward\t[%u %u]", role_tm, reward_id);

    switch (reward_id) {
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
        {
            mail_random_reward(p, p->id, role_tm, reward_id);
        }
        break;
    case 3:
        {
            //broadcast_random_reward_to_world(p, p->id, role_tm, reward_id);
            save_data_to_db(p->id, role_tm, reward_id);
            mail_random_reward(p, p->id, role_tm, reward_id);
        }
        break;
    default:
        break;
    }
    return 0;
}

//---------------------------------------------------------------------------------------------------
