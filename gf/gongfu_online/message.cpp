#include <libtaomee++/inet/pdumanip.hpp>
#include "battle.hpp"

using namespace taomee;

#include "cli_proto.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "switch.hpp"
#include "trade.hpp"
#include "home.hpp"
#include "chat.hpp"
//-----------------------------------------------------------
// static function declarations
//-----------------------------------------------------------
static int talk(player_t* p, uint8_t* msg, uint32_t msg_len, userid_t recvid);

//--------------------------------------------------------------------

void report_chat_to_monitor(player_t* p, uint32_t rid, uint32_t msglen, uint8_t* msg)
{
    int idx_udp = 0;
    uint8_t buf[dbproto_max_len] = {0};
    pack_h(buf, static_cast<uint32_t>(6), idx_udp);
    pack_h(buf, p->id, idx_udp);
    pack_h(buf, rid, idx_udp);
    uint32_t online_id = get_server_id();
    pack_h(buf, online_id, idx_udp);
    pack_h(buf, static_cast<uint32_t>(0), idx_udp);
    uint32_t mapid = get_mapid_low32(p->cur_map->id);
    pack_h(buf, mapid, idx_udp);
    pack_h(buf, static_cast<uint32_t>(get_now_tv()->tv_sec), idx_udp);
    pack_h(buf, msglen, idx_udp);
    pack(buf, msg, msglen, idx_udp);
    TRACE_LOG("add item monitor:sender id=%u, receive id=%u online_id=%u mapid=%u len=%u",
        p->id, rid, online_id, mapid, msglen); 
    send_udp_request_to_db(p, p->id, dbproto_report_chat_content, buf, idx_udp, 1);
}

/**
 * @brief player talks
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int talk_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	
	int i = 0;
	userid_t id;
	uint32_t len;
	unpack(body, id, i);
	unpack(body, len, i);
	TRACE_LOG("%u %u", id, len);
	CHECK_VAL_LE(len, 136);
	CHECK_VAL_EQ(bodylen - 8, len);
	body[bodylen - 1] = '\0';

//	CHECK_DIRTYWORD(p, body + 8);
	REPLACE_DIRTYWORD(p, body + 8);

	if (p->trade_grp && !id) {
		return trsvr_player_talk(p, body + 8, len, id);
	}
	if( p->home_grp && !id){
		p->waitcmd = 0;
		return homesvr_player_talk(p, body + 8, len, id);
	}

	if (p->battle_grp && !id) {
		p->waitcmd = 0;
//		return btlsvr_player_talk(p, body + 8, len, id);
	}
	return talk(p, body + 8, len, id);
}

/**
  * @brief pack simple notification into buf
  * @param buf buffer to pack into 
  * @param type message type
  * @param from message sender
  * @param accept give acceptting or not information
  * @param m player's current map
  * @return the length of buf
  */
int pack_simple_notification(uint8_t* buf, uint32_t type, const player_t* from, uint32_t accept, const map_t* m)
{
	int idx = sizeof(cli_proto_t);
	pack(buf, type, idx);
    if (from != NULL) {
        pack(buf, from->id, idx);
        pack(buf, from->role_tm, idx);
        pack(buf, from->role_type, idx);
        pack(buf, from->nick, sizeof(from->nick), idx);
        pack(buf, accept, idx);
        pack(buf, get_server_id(), idx);
        pack(buf, get_mapid_high32(m->id), idx);
        pack(buf, get_mapid_low32(m->id), idx);
        pack(buf, m->name, sizeof(m->name), idx);

        TRACE_LOG("[%u %u, %u %s %d %u %u %u %s]",type,from->id,from->role_tm,from->nick,
            accept,get_server_id(),get_mapid_high32(m->id),get_mapid_low32(m->id),m->name);
    } else {
        idx += 108;
    }
    init_cli_proto_head(buf, from, cli_proto_simple_notify, idx);
	return idx;
}

int pack_simple_notification_ex(uint8_t *buf, uint32_t type, const player_t * from, uint32_t svr)
{
	int idx = sizeof(cli_proto_t);
	pack(buf, type, idx);
    if (from != NULL) {
		map_t * m = from->cur_map;
		pack(buf, from->id, idx);
		pack(buf, from->role_tm, idx);
		pack(buf, from->role_type, idx);
		pack(buf, from->nick, sizeof(from->nick), idx);
		pack(buf, 1, idx);
		pack(buf, svr, idx);
		pack(buf, get_mapid_high32(m->id), idx);
		pack(buf, get_mapid_low32(m->id), idx);
		pack(buf, m->name, sizeof(m->name), idx);
		TRACE_LOG("[%u %u, %u %s %d %u %u %u %s]",type,from->id,from->role_tm,from->nick,
				1, svr, get_mapid_high32(m->id),get_mapid_low32(m->id), m->name);
	} else {
        idx += 108;
    }
    init_cli_proto_head(buf, from, cli_proto_simple_notify, idx);
	return idx;

}

/**
  * @brief pack realtime notification into buf
  * @param buf buffer to pack into 
  * @param type message type
  * @param from message sender
  * @param accept give acceptting or not information
  * @param 
  * @return the length of buf
  */
int pack_rltm_notification(uint8_t* buf, uint16_t cmd, uint32_t type, const player_t* from, uint32_t accept, uint32_t ex_id1, uint32_t ex_id2, uint32_t ex_id3)
{
	int idx = sizeof(cli_proto_t);
	pack(buf, type, idx);
	pack(buf, from->id, idx);
	pack(buf, static_cast<uint32_t>(from->lv), idx);
	pack(buf, from->nick, sizeof(from->nick), idx);
	pack(buf, accept, idx);
	pack(buf, get_server_id(), idx);
	pack(buf, ex_id1, idx);
	pack(buf, ex_id2, idx);
	pack(buf, ex_id3, idx);

	TRACE_LOG("[%u %u, %u, %u %s %d %u %u %u %u]",type, from->id, from->role_tm, from->lv, from->nick,
			accept, get_server_id(), ex_id1, ex_id2, ex_id3);
	init_cli_proto_head(buf, from, cmd, idx);
	return idx;
}

/**
  * @brief send a simple notification to 'to'
  * @param to user to send a notification to
  * @param from user from whom the notification is sent
  * @param type type of the notification
  * @param accept
  * @param m map
  */
void send_simple_notification(userid_t to, const player_t* from, uint32_t type, uint32_t accept, const map_t* m)
{
	int off_len = sizeof(userid_t);
	int pkglen = pack_simple_notification(pkgbuf + off_len, type, from, accept, m);

	player_t* p = get_player(to);
	if (p) {
		send_to_player(p, pkgbuf + off_len, pkglen, 0);
	} else {
		*(reinterpret_cast<userid_t*>(pkgbuf)) = to; // fill with receiver id 
 		chat_across_svr(from, pkgbuf,  pkglen + off_len);
	}
}


void send_simple_notification_with_pre_svr(userid_t to, player_t * from, uint32_t type, uint32_t server_id)
{
	int off_len = sizeof(userid_t);
	int pkglen = pack_simple_notification_ex(pkgbuf + off_len, type, from, server_id);
	player_t* p = get_player(to);
	if (p) {
		send_to_player(p, pkgbuf + off_len, pkglen, 0);
	} else {
		*(reinterpret_cast<userid_t*>(pkgbuf)) = to; // fill with receiver id 
 		chat_across_svr(from, pkgbuf,  pkglen + off_len);
	}

}

/**
 * @brief send system notification on the end of monthly
 * @param to user to send a notification to
 * @param day 
 */
void send_monthly_timeout_notification(player_t* to, int day)
{
    time_t cur_tm = time(NULL);
    struct tm *p_cur_tm = localtime(&cur_tm);
    int cur_year  = p_cur_tm->tm_year;
    int cur_month = p_cur_tm->tm_mon;

    time_t up_tm = cur_tm + day * 3600 * 24;
    struct tm *p_up_tm = localtime( &up_tm );
    int up_year  = p_up_tm->tm_year;
    int up_month = p_up_tm->tm_mon;
    int up_day   = p_up_tm->tm_mday; 
    time_t low_tm = to->ollast + day * 3600 * 24;
    struct tm *p_low_tm = localtime( &low_tm );
    int low_year  = p_low_tm->tm_year;
    int low_month = p_low_tm->tm_mon;
    int low_day   = p_low_tm->tm_mday;
    KDEBUG_LOG(to->id, "COUPONS \tup: %d-%d-%d low: %d-%d-%d",up_year,up_month,up_day,low_year,low_month,low_day);

    if ( ((cur_month < up_month) && (cur_month >= low_month)) ||
        ((cur_year < up_year) && (low_year <= cur_year)) ) {
        send_simple_notification(to->id, NULL, 2143, 0, NULL);
    }
}
/**
  * @brief send a real time notification to 'to'
  * @param to user to send a notification to
  * @param from user from whom the notification is sent
  * @param type type of the notification
  * @param accept
  * @param m map
  */
void send_rltm_notification(uint16_t cmd, userid_t to, const player_t* from, uint32_t type, uint32_t accept, uint32_t ex_id1, uint32_t ex_id2, uint32_t ex_id3)
{
	int off_len = sizeof(userid_t);
	int pkglen = pack_rltm_notification(pkgbuf + off_len, cmd, type, from, accept, ex_id1, ex_id2, ex_id3);

	TRACE_LOG("%u -> %u %u %u", from->id, to, type, accept);
	player_t* p = get_player(to);
	if (p) {
		send_to_player(p, pkgbuf + off_len, pkglen, 0);
	} else {
		*(reinterpret_cast<userid_t*>(pkgbuf)) = to; // fill with receiver id 
 		chat_across_svr_rltm(from, from->id, pkgbuf,  pkglen + off_len);
	}
}


//-----------------------------------------------------------
// static function
//-----------------------------------------------------------

/**
 * @brief player talks
 * @param p the player
 * @param msg what the player talk
 * @param msg_len the lens of the msg
 * @param recvid the id of the player to talk to
 * @return 0 on success, -1 on error
 */
static int talk(player_t* p, uint8_t* msg, uint32_t msg_len, userid_t recvid)
{
	int i = sizeof(cli_proto_t);
	//pack(pkgbuf,p->id,i);
	pack(pkgbuf + 4, p->nick, sizeof(p->nick), i);
	pack(pkgbuf + 4, recvid, i);
	pack(pkgbuf + 4, msg_len, i);
	pack(pkgbuf + 4, msg, msg_len, i);

	init_cli_proto_head(pkgbuf + 4, p, cli_proto_talk, i);

	player_t *receiver;
	if (recvid == 0 ) {
		send_to_map(p, pkgbuf + 4, i, 1);
	} else if ((receiver = get_player(recvid))){
		send_to_player(receiver, pkgbuf + 4, i, 0);
		send_to_player(p, pkgbuf + 4, i, 1);
	} else {
		// TODO: offline msg
		*(reinterpret_cast<userid_t*>(pkgbuf)) = recvid;
		chat_across_svr(p, pkgbuf, i + 4);
		send_to_player(p, pkgbuf + 4, i, 1);
	}

    report_chat_to_monitor(p, recvid, msg_len, msg);
	return 0;
}

/**                                                                           
 * @brief real to send sys-notify to all online player (include home)
 * @param npc the sys-notify sender
 * @param msg the msg to notify
 * @param msglen the length of the notify msg
 */
void sys_notify_to_all(uint32_t npc, const char* msg, int msglen)
{
	int32_t now = get_now_tv()->tv_sec;
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, npc, idx);
	pack(pkgbuf, now, idx);
	pack(pkgbuf, msglen, idx);
	pack(pkgbuf, msg, msglen, idx);
	init_cli_proto_head(pkgbuf, 0, cli_proto_detailed_noti, idx);
	send_to_all(pkgbuf, idx);
}

/**                                                                           
 * @brief real to sys-notify to one player
 * @param p the player
 * @param npc the sys-notify sender
 * @param msg the msg to notify
 * @param msglen the length of the notify msg
 */
int sys_notify_to_player(player_t* p, uint32_t npc, const char* msg, int msglen)
{
	int32_t now = get_now_tv()->tv_sec;
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, npc, idx);
	pack(pkgbuf, now, idx);
	pack(pkgbuf, msglen, idx);
	pack(pkgbuf, msg, msglen, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_detailed_noti, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}



