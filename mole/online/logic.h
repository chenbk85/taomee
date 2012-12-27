#ifndef LOGIC_H_
#define LOGIC_H_

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <libtaomee/timer.h>

#include "dbproxy.h"
#include "item.h"
#include <libtaomee/log.h>
#include "map.h"
#include "proto.h"
#include <libtaomee/crypt/qdes.h>
#include "sprite.h"
#include "service.h"
#include "dll.h"
#include "npc.h"
#include "central_online.h"

int do_action(sprite_t* p, uint32_t action, uint8_t direction);
int do_buy_item(sprite_t* p, uint32_t itemid, int nitems, int isfree, int ret_needed);
int do_enter_map(sprite_t* p, map_id_t newmap,map_id_t oldmap,uint32_t gdst,uint32_t gsrc);
int do_game_score(sprite_t* p, uint32_t rate, uint32_t score);
int do_get_group(sprite_t* p, uint32_t groupid);
int do_get_user(sprite_t* p, uint32_t uid);
int do_list_items(sprite_t* p, uint32_t userid, uint32_t itemtype, uint8_t flag, uint8_t kind_flag);
int do_list_user(sprite_t* p, map_id_t mapid, uint32_t grid);
int do_map_info(sprite_t *p, map_id_t  mapid);
int do_maps_user(sprite_t* p);
int do_paopao(sprite_t *p);
int do_invite_friend(sprite_t* p, uint32_t id, map_id_t map_id);
int do_reply_invite(sprite_t* p, uint32_t id, map_id_t map_id, uint8_t accept);
int do_talk(sprite_t* p, const uint8_t mesg[], int msglen, uint32_t recvrid);
int do_walk(sprite_t* p, uint32_t xpos, uint32_t ypos, uint32_t item);
int do_throw_item (sprite_t *p, uint32_t item, uint32_t x, uint32_t y);
int do_users_online (sprite_t* p, int count, uint32_t* uin, int return_all);
int do_show_item_use_callback(sprite_t* p, uint32_t itemid);
int do_find_item (sprite_t *p, uint32_t itemid);
int do_get_task (sprite_t *p, uint32_t userid);
int do_exchange_item (sprite_t *p, int idx, uint32_t item_num);
int do_leave_map(sprite_t* p);
int do_get_home_list (sprite_t *p);
int do_post_blackboard(sprite_t *p, uint32_t board, uint32_t msg_len, uint8_t *content);
int do_promotion_tips(sprite_t* p, uint16_t level);

int show_item_use_expired(void* owner, void* data);

int pkg_items(int count, const uint32_t *p, uint8_t *buf, int detail);
int pack_activity_info(sprite_t* p, uint8_t* buf);
int pkg_sprite_detail (sprite_t *p, uint8_t *buf);
int pkg_sprite_simply (sprite_t *p, uint8_t *buf);
int pkg_useritem(sprite_t* p, struct user_itm_use_buf* itm, uint32_t itemid, uint8_t is_used);
void req_add_friend_notifier(sprite_t* p, sprite_t* p2, uint32_t id);

int text_notify_pkg_pic(uint8_t buf[], int cmd, uint32_t id, map_id_t map, uint32_t grid,
			const char nick[], int accept, uint32_t pic, int len, const char txt[]);

int del_mul_friends_cmd(sprite_t* p, const uint8_t* body, int len);
int del_mul_friends_callback(sprite_t* p, uint32_t id, char* buf, int len);

#define text_notify_pkg(buf_, cmd_, uid_, mapid_, grid_, nick_, accept_, len_, txt_) \
	text_notify_pkg_pic((buf_), (cmd_), (uid_), (mapid_), (grid_), (nick_), (accept_), 0, (len_), (txt_))

#define new_text_notify_pkg(buf_, cmd_, uid_, mapid_, grid_, nick_, accept_, pic, len_, txt_) \
	text_notify_pkg_pic((buf_), (cmd_), (uid_), (mapid_), (grid_), (nick_), (accept_), pic, (len_), (txt_))


static inline void
reset_common_action(sprite_t *p)
{
	p->direction = 0;
	if (p->action < ACTION_MAX)
		p->action = 0;
	p->action2 = 0;
}

static inline int
do_add_blacklist(sprite_t* p, uint32_t uid)
{
	if ( IS_GUEST_ID(uid) )
		ERROR_RETURN(("error request: add black list, id=%u, %u", p->id, uid), -1);

	send_request_to_db(SVR_PROTO_DEL_FRIEND, 0, 4, &p->id, uid);
	send_request_to_db(SVR_PROTO_DEL_FRIEND, 0, 4, &uid, p->id);
	uint32_t  buff[2] = {};
	buff[0] = uid;
	buff[1] = p->id;
	notify_join_blacklist_across_svr(p, buff, 8);
	return db_add_bklist(p, uid);
}

static inline int
do_add_friend_req(sprite_t* p, uint32_t friend_id)
{
	sprite_t* p2 = get_sprite(friend_id);
	if (p2) {
		req_add_friend_notifier(p, p2, friend_id);
		response_proto_head(p, PROTO_FRIEND_ADD, 0);
	} else {
		db_chk_uid(p, friend_id);
	}

	DEBUG_LOG("FRIEND REQUEST\t[%u %u]", p->id, friend_id);
	return 0;
}

static inline int
do_add_friend_rsp(sprite_t* p, uint32_t initor, uint8_t accept)
{
	if (!accept) {
		response_proto_head(p, PROTO_RSP_FRIEND_ADD, 0);
		notify_add_friend_response(initor, p, 0);
	} else {
		db_add_friend(0, p->id, initor);
		return db_add_friend(p, initor, p->id);
	}

	return 0;
}

static inline int
do_del_blacklist(sprite_t* p, uint32_t id)
{
	if ( IS_GUEST_ID(id) )
		ERROR_RETURN(("error request: del black list, id=%u, %u", p->id, id), -1);

	return db_del_bklist(p, id);
}

static inline int
do_del_friend(sprite_t* p, uint32_t id)
{
	return db_del_friend(p, id);
}

static inline int
do_get_friend(sprite_t* p, uint32_t uid)
{
	sprite_t* who = get_sprite(uid);
	if (who) {
		response_proto_get_sprite2(who, PROTO_FRIEND_INFO, p, 1);
	} else {
		return db_get_sprite(p, uid);
	}

	return 0;
}

static inline int
do_modify_color(sprite_t* p, uint32_t color, uint32_t item)
{
	p->color = color;
	notify_stamp_svr(p->id);

	//FIXME: players maybe use the bug, client plugin
	if (item == 0)
		return db_set_color(p);

	return db_single_item_op(p, p->id, item, 1, 0);
}

static inline int
do_use_items(sprite_t* p, int nitem_used)
{
	//check total number items with body
	if (nitem_used < 0 || nitem_used > MAX_ITEMS_WITH_BODY) {
		ERROR_RETURN( ("too many items with body, count=%d,%d, id=%u",
						p->item_cnt, nitem_used, p->id), -1 );
	}
	return db_items_op(p);
}

static inline uint8_t
get_sprite_status(const sprite_t* p)
{
	//not online
	if (!p->login)
		return SPRITE_STATUS_NOT_ONLINE;

	//not in game
	if (!p->group)
		return SPRITE_STATUS_LOGIN;
	//in game
	if (GAME_STARTED(p->group))
		return SPRITE_STATUS_IN_GAME;
	//waitting for game
	else
		return SPRITE_STATUS_WAIT_GAME;

}

static inline int
do_jail_user(sprite_t *p, uint32_t userid, int reason, int count)
{
	CHECK_VALID_ID(userid);
	return db_jail_user(p, userid, reason, count);
}

static inline int
do_action2(sprite_t* p, uint32_t action)
{
	p->action2 = action;
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, action, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

#endif // LOGIC_H_
