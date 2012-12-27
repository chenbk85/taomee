#ifndef CENTRAL_ONLINE_H_
#define CENTRAL_ONLINE_H_

// headers since C99
#include <stdint.h>
#include <libtaomee/timer.h>
#include <statistic_agent/msglog.h>
#include "benchapi.h"
#include "dbproxy.h"
#include "proto.h"
#include "sprite.h"
#include "map.h"
#include <libtaomee/conf_parser/config.h>
#include "xhx_server.h"
#include "mole_cutepig.h"
#include "week_activity.h"


enum CentralOnlineCmd {
	COCMD_kick_user 				= 60001,
	COCMD_broadcast_msg				= 60002,
	COCMD_sync_vip					= 60003,
	COCMD_broadcast_bubble_msg		= 60004,

	COCMD_guess_start				= 60005,
	COCMD_guess_end					= 60006,
	COCMD_create_npc				= 60101,
	COCMD_sync_to_user_router		= 61696,

	COCMD_init_pkg					= 62001,
	COCMD_user_login_noti			= 62002,
	COCMD_chk_if_online				= 62003,
	COCMD_chat_across_svr			= 62004,
	// 62005 Used by DirSer
	COCMD_attr_update_noti			= 62006,
	COCMD_chk_user_where			= 62007,
	COCMD_keep_alive				= 62008,
	COCMD_notify_join_blacklist		= 62009,
	COCMD_notify_use_skill_prize	= 62010,
	COCMD_xhx_user_set_value		= 62014,
	COCMD_chat_monitor_warning_user	= 62015,
	COCMD_cp_get_beauty_prize		= 62016,
	COCMD_tell_flash_some_msg		= 62017,
};

enum CentralBroadcastMsgType
{
	CBMT_TAOTAOLE_DRAW			= 1,	//淘淘乐街抽奖
	CBMT_LONGZU_EXPLORE			= 2,	//龙族探险
	CBMT_CHP_DONATE_ITEM		= 3,	//爱心天使捐献公告
	CBMT_UPDATE_MONEY			= 4,	//刷新玩家游戏币
	CBMT_GOLD_COMPASS_MONTH		= 5,	//黄金转盘抽中包月
	CBMT_GOLD_COMPASS_YEAR		= 6,	//黄金转盘抽中包年
	CBMT_GUESS_ITEM_PRICE		= 7,	//我猜猜猜活动
	CBMT_FLOWER_PATY_BLACK_ROSE	= 8,	//春天花花同乐会，黑玫瑰时间
	CBMT_MOLE_GREAT_PRODUCE_TASK = 9,   //摩尔大生产
	CBMT_FREE_SUPER_LAHM         = 10, //lahm
};

enum CentralOnlineErrNo {
	COERR_user_offline	= 1
};

extern int central_online_fd;

// connect to central online and send an init pkg
void connect_to_central_online();
//
int sync_vip_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len);

static inline int
connect_to_central_online_timely(void* owner, void* data)
{
	if (!data) {
		struct timeval tv = *get_now_tv();
		//tv.tv_sec += 30;
		tv.tv_sec += 5;
		add_micro_event(connect_to_central_online_timely, &tv, 0, (void*)1);
	} else if (central_online_fd == -1) {
		connect_to_central_online();
		if (central_online_fd == -1) {
			struct timeval tv = *get_now_tv();
			tv.tv_sec += 30;
			add_micro_event(connect_to_central_online_timely, &tv, 0, (void*)1);
		}
	}
	return 0;
}

// uniform interface to send request to central online
int send_to_central_online(int cmd, sprite_t* p, int body_len, void* body_buf, uint32_t id);


static inline int
chk_user_where_callback(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	rsp_search_user(initor, 0, *((uint32_t*)buf));
	return 0;
}

int chk_online_across_svr_callback(sprite_t* initor, uint32_t rcverid, const void* buf, int len);

//-------------------- Utils For Sending Pkg to Central Online --------------------
// send chatting pkg to central online
static inline void
chat_across_svr(userid_t rcver, void* pkg, int len)
{
	if (send_to_central_online(COCMD_chat_across_svr, 0, len, pkg, rcver) == -1) {
		*(uint32_t*)pkg = len;
		db_send_mail(0, rcver, pkg, len);
	}
}
// check if `p's` friends is online in some other servers
static inline void
chk_online_across_svr(sprite_t* p, void* pkg, int len)
{
	if (send_to_central_online(COCMD_chk_if_online, p, len, pkg, p->id) == -1) {
		chk_online_across_svr_callback(p, p->id, 0, 0);
	}
}
//
static inline void
chk_user_where(sprite_t* chker, uint32_t chkee_id)
{
	uint8_t buf[8];
	int i = 0;
	PKG_H_UINT32(buf, chker->id, i);
	PKG_H_UINT32(buf, chkee_id, i);
	if (send_to_central_online(COCMD_chk_user_where, chker, sizeof buf, buf, chker->id) == -1) {
		rsp_search_user(chker, 0, 0);
	}
}

static inline void
notify_join_blacklist_across_svr(sprite_t* p, void* pkg, int len)
{
	send_to_central_online(COCMD_notify_join_blacklist, p, len, pkg, p->id);
}

static inline void
notify_use_skill_prize_across_svr(sprite_t* p, void* pkg, int len)
{
    DEBUG_LOG("userid %d cmd %d", p->id, COCMD_notify_use_skill_prize);
	send_to_central_online(COCMD_notify_use_skill_prize, p, len, pkg, p->id);
}

static inline void
tell_cp_get_beauty_prize_across_svr(sprite_t* p, void* pkg, int len)
{
	send_to_central_online(COCMD_cp_get_beauty_prize, p, len, pkg, p->id);
}

// notify central online when user login/logout
static inline void
notify_user_login(sprite_t* p, uint8_t login)
{
	uint32_t buf1[4] = { 1, 0, p->id, 0 };
	uint32_t online_time = get_now_tv()->tv_sec - p->stamp;
	if(online_time > 12*60*60){
	    online_time = 12*60*60;
	}
	uint32_t buf2[4] = { 0, 1, 0, online_time};
	msglog(statistic_logfile, 0x0207EEEE, get_now_tv()->tv_sec, login ? buf1 : buf2, sizeof(buf1));

    uint32_t domain = config_get_intval("domain", 0);
    uint32_t msg_type = 0;
	if (domain){
        msg_type = 0x020A0004;
    }
    else{
        msg_type = 0x020A0003;
    }
    msglog(statistic_logfile, msg_type, get_now_tv()->tv_sec, login ? buf1 : buf2, sizeof(buf1));
#ifdef TW_VER 
	msglog(statistic_logfile, 0x0F700002, get_now_tv()->tv_sec, login ? buf1 : buf2, sizeof(buf1));
#endif

	send_to_central_online(COCMD_user_login_noti, 0, 1, &login, p->id);
}
// notify online and client to update a given user's attr
void send_attr_update_noti(uint32_t sender, sprite_t* p_rcver, uint32_t rcver, uint32_t type);
//--------------------------------------------------------------------------

//-------------------- Utils For Receiving Pkg from Central Online --------------------
// attribute update notification
static inline int
attr_update_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN(len, 8);

	int i = 0;
	uint32_t rcvid, type;
	UNPKG_H_UINT32((uint8_t*)buf, rcvid, i);
	sprite_t* rcv = get_sprite(rcvid);
	if (rcv) {
		UNPKG_H_UINT32((uint8_t*)buf, type, i);
		i = sizeof(protocol_t);
		PKG_UINT32(msg, type, i);
		init_proto_head(msg, PROTO_UPD_ATTR_NOTI, i);
		send_to_self(rcv, msg, i, 0);
	}
	return 0;
}
// handle chatting across online servers
static inline int
chat_across_svr_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	int ret = 0;

	uint32_t  rcvid = *((uint32_t*)buf);
	sprite_t* rcver = get_sprite(rcvid);

	if (rcver) {
		send_to_self(rcver, ((uint8_t*)buf) + 4, len - 4, 0);
	} else {
		*((uint32_t*)buf) = len;
		db_send_mail(0, rcvid, buf, len);
		ret = COERR_user_offline;
	}

	return ret;
}

static inline int
notify_join_blacklist_across_svr_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	int ret = 0;
	int i = 0;
	uint32_t rcvid = 0;
	uint32_t userid = 0;

	UNPKG_H_UINT32((uint8_t*)buf, rcvid, i);
	UNPKG_H_UINT32((uint8_t*)buf, userid, i);
	DEBUG_LOG("---rcvid:%u,userid:%u----", rcvid, userid);
	sprite_t* rcver = get_sprite(rcvid);

	if (rcver) {
		i = sizeof(protocol_t);
		PKG_UINT32(msg, userid, i);
		init_proto_head(msg, PROTO_NOTIFY_JOIN_BLACK, i);
		send_to_self(rcver, msg, i, 0);
		DEBUG_LOG("---BLACK----%u", userid);
	} else {
	    DEBUG_LOG("---offline BLACK----%u", userid);
		ret = COERR_user_offline;
	}

	return ret;
}

static inline int
notify_use_skill_prize_across_svr_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	int j = 0;
	uint32_t prize_flag = 0;

	UNPKG_H_UINT32((uint8_t*)buf, prize_flag, j);
	DEBUG_LOG("notify cmd %d prize_flag:%d",PROTO_NOTIFY_ALL_USE_SKILL_PRIZE, prize_flag);

	uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, prize_flag, l);

    init_proto_head(n_msg, PROTO_NOTIFY_ALL_USE_SKILL_PRIZE, l);

    sprite_t* tmp = NULL;
	uint32_t i =0;
	for (i = 0; i < HASH_SLOT_NUM; i++)
	{
		list_for_each_entry (tmp, &idslots[i], hash_list)
		{
		    if (tmp)
		    {
			    if(!IS_GUEST_ID(tmp->id))
			    {
                    send_to_self(tmp, n_msg, l, 0);
			    }
			}
		}
	}

	return 0;
}


static inline int
notify_guess_item_begin_across_svr_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
    DEBUG_LOG("---guess item begin !");

	int i = 0;
	uint32_t itemid = 0;
	uint32_t count = 0;
	uint32_t time_last = 0;

	UNPKG_H_UINT32((uint8_t*)buf, itemid, i);
	UNPKG_H_UINT32((uint8_t*)buf, count, i);
	UNPKG_H_UINT32((uint8_t*)buf, time_last, i);

	i = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, count, i);
	PKG_UINT32(msg, time_last, i);
	init_proto_head(msg, PROTO_QUERY_GUESS_ITEM_CHARITY_DRIVE, i);
	send_to_map3(142, msg, i);

	return 0;
}

static inline int
notify_guess_item_end_across_svr_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
    //DEBUG_LOG("---guess item end !");

    struct guess_info_hdr {
        uint32_t	itemid;
        uint32_t	itemcount;
		uint32_t	count;
	}__attribute__((packed));

    struct guess_info {
        uint32_t	userid;
        uint32_t    xiaomee;
	}__attribute__((packed));

	struct guess_info_hdr *p_hdr_info = (struct guess_info_hdr*)buf;

	CHECK_BODY_LEN_GE((uint32_t)len,sizeof(struct guess_info_hdr));


    CHECK_BODY_LEN((uint32_t)len,sizeof(struct guess_info_hdr) + p_hdr_info->count*sizeof(struct guess_info));
    struct guess_info *p_info = (struct guess_info*)((uint8_t*)buf + sizeof(struct guess_info_hdr));

    sprite_t* rcver = NULL;

	uint32_t i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
	    rcver = get_sprite(p_info->userid);
	    if (rcver)
	    {
	        item_t *itm = get_item_prop(p_hdr_info->itemid);
	        db_buy_items_by_xiaomee(rcver, itm, p_hdr_info->itemcount, find_kind_of_item(p_hdr_info->itemid), p_info->xiaomee, 0);
	        if (rcver->yxb > p_info->xiaomee)
	        {
	            rcver->yxb = rcver->yxb - p_info->xiaomee;
	        }

	        uint32_t l = sizeof(protocol_t);
            PKG_UINT32(msg, p_hdr_info->itemid, l);
            PKG_UINT32(msg, p_hdr_info->itemcount, l);
            PKG_UINT32(msg, 1, l);
	        PKG_UINT32(msg, p_info->userid, l);
	        PKG_UINT32(msg, p_info->xiaomee, l);
	        init_proto_head(msg, PROTO_GUESS_ITEM_END_CHARITY_DRIVE, l);
	        send_to_self(rcver, msg, l, 0);

			char txt[256];
	    	int txtlen;
#ifdef TW_VER
	    	txtlen = snprintf(txt, sizeof txt, "恭喜你以%d摩爾豆在慈善競猜中獲得%s%d個!\n", p_info->xiaomee, itm->name,
					p_hdr_info->itemcount);
#else
	    	txtlen = snprintf(txt, sizeof txt, "恭喜你以%d摩尔豆在慈善竞猜中获得%s%d个!\n", p_info->xiaomee, itm->name,
					p_hdr_info->itemcount);
#endif
	    	notify_system_message(rcver, txt, txtlen);

	    }

	    p_info++;
	}



	DEBUG_LOG("---guess item end succ !");

	return 0;
}

static inline int
notify_syn_to_user_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	int i = 0;
	uint32_t arg = 0;
	uint32_t sync_cmd =  0;
	int bytes = sizeof(protocol_t);
	sprite_t* p = get_sprite(rcverid);

	if(!p) {
		return COERR_user_offline;
	}

	const int bit = (56  - 1) % 32;
	p->only_one_flag[1] &= ~(1 << bit);

	UNPKG_H_UINT32((uint8_t*)buf, sync_cmd, i);//1526 已经使用
    DEBUG_LOG("---notify_syn_to_user_op cmd:%u !", sync_cmd);
	while(i < len) {
		UNPKG_H_UINT32((uint8_t*)buf, arg, i);
		PKG_UINT32(msg, arg, bytes);
	}

	init_proto_head(msg, sync_cmd, bytes);
	return send_to_self(p, msg, bytes,0);
}

static inline int
xhx_user_set_value(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	sprite_t* p= get_sprite(rcverid);
	if (!p) {
		return COERR_user_offline;
	}
	uint32_t type = 0;
	int i = 0;
	UNPKG_H_UINT32((uint8_t*)buf, type, i);
	deal_xhx_event(p, type);
	return 0;
}

static inline int
chat_monitor_warning_user(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	sprite_t* p= get_sprite(rcverid);
	if (!p) {
		return COERR_user_offline;
	}
	uint32_t type = 0;
	int i = 0;
	UNPKG_H_UINT32((uint8_t*)buf, type, i);
	p->user_talk_flag = 1;
	p->ban_talk_time = get_now_tv()->tv_sec;
	DEBUG_LOG("userid %u user_talk_flag %d  ban_talk_time %d", p->id, p->user_talk_flag, p->ban_talk_time);
	return 0;
}

static inline int
cp_get_beauty_prize_across_svr_callback(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	int j = 0;
	uint32_t gift_type = 0;
	UNPKG_H_UINT32((uint8_t*)buf, gift_type, j);

	cutepig_get_beauty_game_limit_gift(gift_type);
	return 0;
}
void tell_flash_some_msg_across_svr(sprite_t* p, uint32_t msg_type, int msg_len, char* txt);
int tell_flash_some_msg_across_svr_callback(sprite_t* initor, uint32_t rcverid, const void* buf, int len);

#endif // CENTRAL_ONLINE_H_
