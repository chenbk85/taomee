#include <sys/types.h>
#include <sys/time.h>

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <openssl/md5.h>

#include <libtaomee/bitmanip/bitmanip.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>

#include <libtaomee/log.h>
#include <libtaomee/crypt/qdes.h>
#include "util.h"
#include <statistic_agent/msglog.h>

#include "central_online.h"
#include "mole_card.h"
#include "dbproxy.h"
#include "dll.h"
#include "event.h"
#include "exclu_things.h"
#include "item.h"
#include "logic.h"
#include "login.h"
#include "map.h"
#include "message.h"
#include "mole_messenger.h"
#include "npc.h"
#include "poll.h"
#include "wish_pool.h"

#include "market.h"
#include "picsrv.h"

#include "proto.h"
#include "tasks.h"
#include "mole_group.h"
#include "birthday.h"
#include "communicator.h"
#include "mibiserver.h"
#include "gold_bean_serv.h"
#include "magic_code.h"
#include "party.h"
#include "spring.h"
#include "small_require.h"
#include "pet_item.h"
#include "lahm_sport.h"
#include "mole_bank.h"
#include "homeinfo.h"
#include "mole_homeland.h"
#include "mole_question.h"
#include "cache_svr.h"
#include "gift.h"
#include "mole_class.h"
#include "mole_pasture.h"
#include "mole_happy_card.h"
#include "mole_doctor.h"
#include "game_pk.h"
#include "lahm_diary.h"
#include "mole_work.h"
#include "mole_car.h"
#include "race_car.h"
#include "mole_candy.h"
#include "hallowmas.h"
#include "npc_event.h"
#include "thanksgiving.h"
#include "rand_itm.h"
#include "lamu_skill.h"
#include "profession_work.h"
#include "delicous_food.h"
#include "street.h"
#include "mole_dragon.h"
#include "fire_cup.h"
#include "add_sth_rand.h"
#include "swap.h"
#include "game_bonus.h"
#include "friendship_box.h"
#include "race_bonus.h"
#include "lahm_classroom.h"
#include "charity_drive.h"
#include "give_item_rank.h"
#include "snowball_war.h"
#include "christmas_sock.h"
#include "chris_cake.h"
#include "foot_print.h"
#include "mole_angel.h"
#include "mole_dungeon.h"
#include "angry_lahm.h"
#include "hide_and_seek.h"
#include "pasture.h"
#include "tv_question.h"
#include "angel_fight.h"
#include "sand_pool.h"
#include "play_drum.h"
#include "final_boss_2011.h"
#include "moon_cake.h"
#include "lucky_lottery.h"
#include "mole_cutepig.h"
#include "week_activity.h"
#include "xhx_server.h"
#include "mole_avatar.h"
#include "super_guider.h"
#include "svc_ac_api.h"
#include "charitable_party.h"
#include "spring_couplets.h"
#include "winter_vs_spring.h"
#include "year_feast.h"
#include "lanterns_day.h"
#include "flower_party.h"
#include "advanced_class.h"
#include "mole_cake.h"
#include "mole_ocean.h"
uint8_t msg[2<<20];

char admin[16] = "淘米网络";

#define MAX_PROC_MSG_NUM	65000
typedef int (*dispatcher_t)(sprite_t*, const uint8_t*, int);

dispatcher_t dispatch[MAX_PROC_MSG_NUM];

int response_proto_list_items2(sprite_t* p, uint32_t id, uint8_t* buf, int count)
{
	int i, j, k;
	uint32_t itm, cnt;

	assert (count >= 0);

	j = sizeof (protocol_t);
	PKG_UINT32 (msg, id, j);
	PKG_UINT32 (msg, count, j);
	for (i = 0, k = 0; i < count; i++) {
		UNPKG_H_UINT32 (buf, itm, k);
		UNPKG_H_UINT32 (buf, cnt, k);
		PKG_UINT32 (msg, itm, j);
		PKG_UINT32 (msg, cnt, j);
	}

	init_proto_head (msg, p->waitcmd, j);

	DEBUG_LOG ("LIST ITEM RSP\t[%u %d]", p->id, count);
	return send_to_self (p, msg, j, 1);
}

int response_proto_list_items(sprite_t* p, uint32_t id, uint32_t* itms, int count)
{
	int i, j;

	j = sizeof (protocol_t);
	PKG_UINT32 (msg, id, j);
	i = pkg_items (count, itms, msg + j, 1);

	init_proto_head (msg, PROTO_ITEM_LIST, i + j);
	return send_to_self (p, msg, i + j, 1);
}

int pkg_proto_throw_item(uint8_t* buf, sprite_t* p, uint32_t itemid, int x, int y)
{
	int j;
	j = sizeof (protocol_t);
	PKG_UINT32 (buf, p->id, j);
	PKG_UINT32 (buf, itemid, j);
	PKG_UINT32 (buf, x, j);
	PKG_UINT32 (buf, y, j);

	init_proto_head (buf, PROTO_THROW, j);

	return j;
}

void
response_proto_get_sprite2(sprite_t* p, int cmd, sprite_t* req, int completed)
{
	int j;
	uint8_t st;

	j = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, j);
	PKG_STR(msg, p->nick, j, 16);
	PKG_UINT32(msg, p->color, j);
	PKG_UINT32(msg, p->dining_flag, j);
	PKG_UINT32(msg, p->dining_level, j);

	PKG_UINT32(msg, p->flag, j);
	if (p->tiles)
		PKG_MAP_ID(msg, p->tiles->id, j);
	else
		PKG_MAP_ID(msg, 0, j);

	st = get_sprite_status(p);
	PKG_UINT8(msg, st, j);
	PKG_UINT32(msg, p->action, j);
	init_proto_head(msg, cmd, j);

	if (!req)
		send_to_map(p, msg, j, completed);
	else
		send_to_self(req, msg, j, completed);
}

void
response_proto_get_sprite(sprite_t* p, int cmd, int is_detail, sprite_t *req)
{
	int len;
	if (is_detail)
		len = pkg_sprite_detail(p, msg + sizeof (protocol_t));
	else
		len = pkg_sprite_simply(p, msg + sizeof (protocol_t));

	len += sizeof(protocol_t);
	init_proto_head(msg, cmd, len);
	if (!req)
		send_to_map(p, msg, len, 1);
	else
		send_to_self(req, msg, len, 1);
}

int
response_proto_user_home(sprite_t *p, uint32_t id, const char *data, int len)
{
	int count;
	int k, j, i;
	uint32_t itm_id;

	i = sizeof (protocol_t);
	PKG_UINT32 (msg, id, i);
	if (p->id == id) {
		PKG_STR(msg, p->nick, i, sizeof p->nick);
		PKG_UINT32 (msg, 1, i);
	} else {
		sprite_t* op = get_sprite(id);
		if (!op)
			op = get_across_svr_sprite(id, id);

		if (op) {
			PKG_STR(msg, op->nick, i, sizeof p->nick);
			PKG_UINT32 (msg, 1, i);
		} else {
			char buf[sizeof p->nick];
			snprintf(buf, sizeof buf, "%u", id);
			PKG_STR(msg, buf, i, sizeof buf);
			PKG_UINT32 (msg, 0, i);
		}
	}

	j = 0;
	uint16_t tmp;
	// add for getting the first home FangXing
	UNPKG_H_UINT32 (data, itm_id, j);
	PKG_UINT32 (msg, itm_id, i);

	UNPKG_H_UINT16 (data, tmp, j);
	PKG_UINT16 (msg, tmp, i);

	UNPKG_H_UINT16 (data, tmp, j);
	PKG_UINT16 (msg, tmp, i);

	memcpy (msg + i, data + j, 8);
	i += 8;
	j += 8;

	UNPKG_H_UINT32 (data, count, j);
	CHECK_BODY_LEN (len, count * 16 + 20);

	PKG_UINT32 (msg, count, i);
	for (k = 0; k < count; k++) {
		UNPKG_H_UINT32 (data, itm_id, j);
		PKG_UINT32 (msg, itm_id, i);

		UNPKG_H_UINT16 (data, tmp, j);
		PKG_UINT16 (msg, tmp, i);

		UNPKG_H_UINT16 (data, tmp, j);
		PKG_UINT16 (msg, tmp, i);

		memcpy (msg + i, data + j, 8);
		i += 8;
		j += 8;
	}
	init_proto_head (msg, PROTO_USER_HOME, i);

	DEBUG_LOG ("USER HOME CALLBACK\t[%u %u %d]", p->id, id, count);
	return send_to_self (p, msg, i, 1);
}

void response_proto_use_items(sprite_t *p)
{
	int total;
	int j, i = sizeof (protocol_t);

	assert (p->sess_len);
	total = *(int *)p->session;

	PKG_UINT32 (msg, p->id, i);
	PKG_UINT32 (msg, total, i);
	for (j = 0; j < total; j++) {
		struct user_itm_use_buf *itms = (struct user_itm_use_buf *)
			(p->session + 8 + j * sizeof (struct user_itm_use_buf));
		PKG_UINT32 (msg, itms->id, i);
		if (itms->mask & ADD_ITEM_MASK)
			PKG_UINT8 (msg, 1, i);
		else
			PKG_UINT8 (msg, 0, i);
	}

	init_proto_head (msg, PROTO_USER_ITEM_USE, i);
	send_to_map (p, msg, i, 1);
}

void notify_home_users_change(sprite_t *p, uint32_t count, int completed)
{
	int i;

	i = sizeof (protocol_t);
	PKG_UINT32 (msg, p->id, i);
	PKG_UINT32 (msg, count, i);
	init_proto_head (msg, PROTO_HOME_USERS_CHANGE_NOTIFY, i);

	send_to_self (p, msg, i, completed);
}

void notify_add_friend_response(uint32_t id, sprite_t* rep, uint8_t accept)
{
	int j;
	char txt[256];
	sprite_t* initiator;

	if (accept) {
		j = sprintf(txt, "%s已经同意您加他为好友.", rep->nick);
	} else {
		j = sprintf(txt, "%s拒绝了您加他为好友的请求.", rep->nick);
	}

	DEBUG_LOG ("ADD FRIEND\t[%u %u]", rep->id, id);
	j = text_notify_pkg(msg + 4, PROTO_RSP_FRIEND_ADD, rep->id, 0, rep->sub_grid, rep->nick, accept, j, txt);
	if ((initiator = get_sprite(id))) {
		send_to_self(initiator, msg + 4, j, 0);
	} else {
		*(uint32_t*)msg = rep->id;
		chat_across_svr(id, msg, j + 4);
	}

}

void notify_system_message(sprite_t *p, const char *txt, int len)
{
	int i = text_notify_pkg(msg, 0, 0, 0, 0, admin, 0, len, txt);
	if (p) {
		send_to_self(p, msg, i, 0);
	} else {
		send_to_all(msg, i);
	}
}

void new_notify_system_message(sprite_t *p, const char *txt, int len, int cmd)
{
	int i = new_text_notify_pkg(msg, cmd, 0, 0, 0, admin, 0, 0, len, txt);
	if (p) {
		send_to_self(p, msg, i, 0);
	} else {
		send_to_all(msg, i);
	}
}

void notify_user_exit(sprite_t *p, int reason, int del_node)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, reason, i);
	init_proto_head(msg, PROTO_EXIT_NOTIFY, i);
	send_to_self(p, msg, i, 0);

	if (reason != -ERR_dirty_word) {
		del_sprite_conn(p, del_node);
	}
}

void response_proto_leave_map(sprite_t *p, int notify_self)
{
	int i;
	i = sizeof (protocol_t);
	PKG_UINT32 (msg, 1, i);
	PKG_UINT32 (msg, p->id, i);
	init_proto_head (msg, PROTO_MAP_LEAVE, i);
	if (notify_self)
		send_to_map (p, msg, i, 1);
	else
		send_to_map_except_self (p, msg, i, 1);
}
//---------------------------------------------------------------------------------
static int
unpkg_user_items(sprite_t *p, const uint8_t *body, int len, int* nitem_used)
{
	const static int useritem_pkg_minsize	=  9;
	const static int useritem_size			=  5;

	uint8_t flag;
	uint32_t itemid;
	int j, i, count ,k;

	if (len < useritem_pkg_minsize) {
		ERROR_RETURN(("error body len=%d, id=%u", len, p->id), -1);
	}

	i = j = k = 0;
	UNPKG_UINT32(body, count, i);
	CHECK_BODY_LEN(len, count * useritem_size + 4);
	if (count > 20) {
		ERROR_RETURN(("error user item count=%d, id=%u", count, p->id), -1);
	}

	p->sess_len = 8 + count * sizeof(struct user_itm_use_buf);

	PKG_H_UINT32(p->session, count, k);
	PKG_H_UINT32(p->session, 0, k);
	*nitem_used = p->item_cnt;

	struct user_itm_use_buf* itms = (struct user_itm_use_buf*)&p->session[8];
	for (; j < count; ++j) {
		UNPKG_UINT32(body, itemid, i);
		UNPKG_UINT8(body, flag, i);

		int ret = pkg_useritem(p, itms + j, itemid, flag);
		switch (ret) {
		case -1:
			goto err;
		case USED_ITEM_MASK:
			--*nitem_used;
			break;
		case ADD_ITEM_MASK | USED_ITEM_MASK:
			++*nitem_used;
			break;
		default:
			assert(!"impossible return value from pkg_useritem!");
		}
	}

	return 0;
err:
	p->sess_len = 0;
	return -1;
}

static int
unpkg_post_blackboard(uint8_t *body, int bodylen, uint32_t *board, uint32_t *msglen, uint8_t **content)
{
	int i;
	if (bodylen < 8)
		ERROR_RETURN(("error len=%d", bodylen), -1);

	i = 0;
	UNPKG_UINT32 (body, *board, i);
	UNPKG_UINT32 (body, *msglen, i);
	int len = bodylen - 8;
	CHECK_BODY_LEN (len, *msglen);
	if (*msglen > 1024 || *msglen < 2 || (*board > 6 ))
		ERROR_RETURN (("error len=%d, board=%d", *msglen, *board), -1);

	*content = body + i;
	--(*msglen);
	(*content)[*msglen] = '\0';

	return 0;
}

static int
unpkg_post_message (uint8_t *body, int bodylen, uint32_t *type,
		uint32_t *msglen, uint8_t **content)
{
	int i;
	if (bodylen < 128)
		ERROR_RETURN (("error len=%d", bodylen), -1);

	i = 0;
	UNPKG_UINT32 (body, *type, i);
	UNPKG_UINT32 (body, *msglen, i);
	if (*msglen > 3600)
		ERROR_RETURN (("error msglen=%d", *msglen), -1);

	CHECK_BODY_LEN (bodylen, 8 + (*msglen));
	*content = body + i;

	return 0;
}

/*
static int
unpkg_set_home_item(sprite_t* p, uint8_t* body, int len, int num, int *used, int *not_used)
{
	int j, i;
	home_item_t *hi;
	item_t* pitm;
	int  home_nbr;
	if (len < 12)
		ERROR_RETURN (("error len=%d", len), -1);

	i = 0;
	UNPKG_UINT32 (body, home_nbr, i);
	UNPKG_UINT32 (body, *used, i);
	UNPKG_UINT32 (body, *not_used, i);
	j = 0;
	PKG_H_UINT32 (body, home_nbr, j);
	PKG_H_UINT32 (body, *used, j);
	PKG_H_UINT32 (body, *not_used, j);

	if (home_nbr != num) {
		ERROR_RETURN(("not in right map\t[%u %u %u]", p->id, home_nbr, num), -1);
	}

	CHECK_BODY_LEN (len, 12 + (*used) * sizeof (home_item_t) + (*not_used) * 8);
	for (j = 0; j < *not_used; j++) {
		uint32_t * t = (uint32_t*) (body + 8 + j * 8);
		*t = ntohl (*t);
		*(t + 1) = ntohl (*(t + 1));
	}

	for (j = 0; j < *used; j++) {
		hi = (home_item_t*) (body + 8 + j * sizeof (home_item_t) + (*not_used) * 8);
		hi->itemid = ntohl (hi->itemid);
		hi->x = ntohs (hi->x);
		hi->y = ntohs (hi->y);

		if (!(pitm = get_item_prop (hi->itemid)) || pitm->layer != hi->layer)
			ERROR_RETURN (("error item id or layer, id=%d layer=%d", hi->itemid, hi->layer), -1);
		memset (hi->pad, 0, sizeof (hi->pad));
	}

	return 0;
}
*/

static int
unpkg_set_used_home_item(sprite_t* p, uint8_t* body, int len, int num, int *used)
{
	int j, i;
	home_item_t *hi;
	item_t* pitm;
	int  home_nbr;
	if (len < 8)
		ERROR_RETURN (("error len=%d", len), -1);

	i = 0;
	UNPKG_UINT32 (body, home_nbr, i);
	UNPKG_UINT32 (body, *used, i);
	j = 0;
	PKG_H_UINT32 (body, home_nbr, j);
	PKG_H_UINT32 (body, *used, j);

	if (home_nbr != num) {
		ERROR_RETURN(("not in right map\t[%u %u %u]", p->id, home_nbr, num), -1);
	}

	CHECK_BODY_LEN (len, 8 + (*used) * sizeof (home_item_t));

	for (j = 0; j < *used; j++) {
		hi = (home_item_t*) (body + 8 + j * sizeof (home_item_t));
		hi->itemid = ntohl (hi->itemid);
		hi->x = ntohs (hi->x);
		hi->y = ntohs (hi->y);

		if (!(pitm = get_item_prop (hi->itemid)) || pitm->layer != hi->layer)
			ERROR_RETURN (("error item id or layer, id=%d layer=%d", hi->itemid, hi->layer), -1);
		memset (hi->pad, 0, sizeof (hi->pad));
	}

	return 0;
}

static inline int
unpkg_maps_user(const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);

	return 0;
}

static inline int
unpkg_throw_item(const uint8_t body[], int bodylen, uint32_t *item, uint32_t *x, uint32_t *y)
{
	CHECK_BODY_LEN(bodylen, 12);

	int i = 0;
	UNPKG_UINT32(body, *item, i);
	UNPKG_UINT32(body, *x, i);
	UNPKG_UINT32(body, *y, i);

	item_kind_t* ik = find_kind_of_item (*item);
	if ( !ik || (ik->kind != THROW_ITEM_KIND) ) {
		ERROR_RETURN(("wrong itmid=%u", *item), -1);
	}
	if ( (*x > MAP_WIDTH) || (*y > MAP_HEIGHT) ) {
		WARN_LOG("Invalid Coordinate (%u, %u)", *x, *y);
	}

	return 0;
}

static inline int
unpkg_walk(const uint8_t body[], int bodylen, uint32_t* xpos, uint32_t* ypos, uint32_t* item)
{
	CHECK_BODY_LEN(bodylen, 12);

	int i = 0;
	UNPKG_UINT32(body, *xpos, i);
	UNPKG_UINT32(body, *ypos, i);
	UNPKG_UINT32(body, *item, i);

	return 0;
}

static inline int
unpkg_users_online(const uint8_t body[], int bodylen, int* return_all, int* count, uint32_t* uin)
{
	CHECK_BODY_LEN_GE(bodylen, 9);

	int i = 0;
	UNPKG_UINT8(body, *return_all, i);
	if (*return_all) {
		CHECK_BODY_LEN(bodylen, 9);
	}
	UNPKG_UINT32(body, *count, i);
	if (*count > SPRITE_FRIENDS_MAX) {
		ERROR_RETURN(("too many users, count=%d", *count), -1);
	}
	CHECK_BODY_LEN(*count * 4 + 5, bodylen);

	int j = 0;
	for (; j != *count; ++j) {
		UNPKG_UINT32(body, uin[j], i);
		CHECK_VALID_ID(uin[j]);
	}
	return 0;
}

static inline int
unpkg_talk(const uint8_t body[], int bodylen, uint32_t* recvrid, int* msglen)
{
	if (bodylen <= 8) {
		ERROR_RETURN(("error len=%d", bodylen), -1);
	}

	int i = 0;
	UNPKG_UINT32(body, *recvrid, i);
	UNPKG_UINT32(body, *msglen, i);
	CHECK_INT_GE(*msglen, 1);
	CHECK_INT_LE(*msglen, 136);
	//CHECK_INT_LE(*msglen, 200);
	CHECK_BODY_LEN(bodylen, *msglen + 8);
	return 0;
}

static inline int
unpkg_list_items(const uint8_t body[], int len, uint32_t* userid, uint32_t* itemtype, uint8_t* flag, uint8_t* kind_flag)
{
	CHECK_BODY_LEN(len, 10);

	int i = 0;
	UNPKG_UINT32 (body, *userid, i);
	UNPKG_UINT32 (body, *itemtype, i);
	UNPKG_UINT8 (body, *flag, i);
	UNPKG_UINT8 (body, *kind_flag, i);
	return 0;
}

static inline int
unpkg_get_items(const uint8_t body[], int len, uint8_t* itemkind)
{
	CHECK_BODY_LEN(len, 1);
	*itemkind = *body;
	return 0;
}

static inline int
unpkg_get_blacklist_cmd(const uint8_t body[], int len)
{
	CHECK_BODY_LEN(len, 0);
	return 0;
}

static inline int
unpkg_leave_map(const uint8_t body[], int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return 0;
}

static inline int
unpkg_paopao(const uint8_t body[], int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return 0;
}

static inline int
use_item_cmd(sprite_t* p, const uint8_t* body, int len)
{
	int nitem_used;
	if (unpkg_user_items(p, body, len, &nitem_used) == -1) {
		return -1;
	}

	DEBUG_LOG("USE ITEM\t[%u %d]", p->id, nitem_used);
	return do_use_items(p, nitem_used);
}

static int already_have_same_layer(uint32_t layer, uint32_t layers[], uint32_t count)
{
	int loop;
	for (loop = 0; loop < count; loop++) {
		if (layer == layers[loop])
			return 1;
	}
	return 0;
}

static int use_body_item_cmd(sprite_t* p, const uint8_t* body, int len)
{
	int count, lose = 0;
	uint8_t buff[256];
	uint32_t itm_layers[MAX_ITEMS_WITH_BODY] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	CHECK_BODY_LEN_GE(len, 4);

	int j = 0;
	UNPKG_UINT32(body, count, j);
	CHECK_INT_LE(count, MAX_ITEMS_WITH_BODY);
	CHECK_BODY_LEN(len, 4 * (count + 1));

	int i = 4;
	int loop;
	for (loop = 0; loop < count; loop++) {
		uint32_t itmid;
		UNPKG_UINT32(body, itmid, j);
//		DEBUG_LOG("USE BODY ITEM\t[%u %u %d]", p->id, itmid, itm_layers[loop]);
		item_t* pitm = get_item_prop(itmid);
		item_kind_t* kind = find_kind_of_item(itmid);
		if (!pitm || !kind || kind->kind != SUIT_ITEM_KIND
			|| already_have_same_layer(pitm->layer, itm_layers, loop)) {
			lose++ ;
			continue;
		}
		PKG_H_UINT32(buff, itmid, i);
		itm_layers[loop - lose] = pitm->layer;
	}
	*(uint32_t*)buff = count - lose;
	memcpy(p->session, buff, i);
	DEBUG_LOG("USE BODY ITEM\t[%u %d %d]", p->id, count, lose);
	return send_request_to_db(SVR_PROTO_PUT_ON_CLOTHES, p, i, buff, p->id);
}

static inline int
unpkg_reply_invite(const uint8_t body[], int len, uint32_t* uid, map_id_t* mapid, uint8_t* accept)
{
	CHECK_BODY_LEN(len, 13);

	int i = 0;
	UNPKG_UINT32(body, *uid, i);
	unpkg_mapid(body, mapid, &i);
	UNPKG_UINT8(body, *accept, i);
	return 0;
}

static inline int
unpkg_special_action(const uint8_t* body, int len, uint16_t* type, uint16_t* x, uint16_t* y)
{
	CHECK_BODY_LEN(len, 6);

	int i = 0;
	UNPKG_UINT16(body, *type, i);
	UNPKG_UINT16(body, *x, i);
	UNPKG_UINT16(body, *y, i);
	return 0;
}

//------------------------------------------------------------------------------------
static inline int
list_map_cmd(sprite_t* p, const uint8_t* body, int len)
{
	if (unpkg_maps_user(body, len) == -1) return -1;

	//DEBUG_LOG("LIST MAP\t[%u]", p->id);
	return do_maps_user(p);
}

static inline int
get_home_list_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);

	//DEBUG_LOG("HOME ENTRY\t[%u]", p->id);
	return do_get_home_list(p);
}

static inline int
set_home_item_cmd(sprite_t* p, uint8_t* body, int len)
{
	int used;
	if (!p->tiles || (!IS_HOUSE_MAP(p->tiles->id) && !IS_HOUSE1_MAP(p->tiles->id) && !IS_HOUSE2_MAP(p->tiles->id))) {
		ERROR_RETURN(("not in home[%u]", p->id), -1);
	}

	uint32_t home_num = GET_MAP_TYPE(p->tiles->id);
	home_num = (home_num < 11) ? 1 : (home_num - 9);

	if (unpkg_set_used_home_item (p, body, len, home_num, &used) == -1)
		return -1;

	DEBUG_LOG("SET HOME\t[%u %d %d]", p->id, home_num, used);
	cs_set_jy_and_pet_state(0, 1, get_now_tv()->tv_sec, p->id);
	return db_set_home_i (p, body, len);
}

static inline int
reset_home_item_cmd(sprite_t* p, uint8_t* body, int len)
{
	item_t* pitm;
	if (!p->tiles) {
		ERROR_RETURN(("not in home[%u]", p->id), -1);
	}

	uint32_t home_num = GET_MAP_TYPE(p->tiles->id);
	home_num = (home_num < 11) ? 1 : (home_num - 9);

	home_item_t* hi = (home_item_t*) body;
	hi->itemid = ntohl (hi->itemid);
	hi->x = ntohs (hi->x);
	hi->y = ntohs (hi->y);

	if (!(pitm = get_item_prop (hi->itemid)) || pitm->layer != hi->layer)
		ERROR_RETURN (("error item id or layer, id=%d layer=%d", hi->itemid, hi->layer), -1);
	memset (hi->pad, 0, sizeof (hi->pad));

	DEBUG_LOG("RESET HOME\t[%u %d]", p->id, home_num);
	cs_set_jy_and_pet_state(0, 1, get_now_tv()->tv_sec, p->id);
	return db_reset_home_i (p, body, len);
}

static inline int
user_home_cmd(sprite_t* p, const uint8_t* body, int len)
{
	if (!p->tiles || (!IS_HOUSE_MAP(p->tiles->id) && !IS_HOUSE1_MAP(p->tiles->id) && !IS_HOUSE2_MAP(p->tiles->id))) {
		ERROR_RETURN(("not in home[%u]", p->id), -1);
	}

	userid_t uid = GET_UID_IN_HOUSE(p->tiles->id);
	DEBUG_LOG("USER HOME\t[%u %u %lu]", p->id, uid, p->tiles->id);
	int num = 1;
	if (IS_HOUSE_MAP(p->tiles->id)) {
		num = 1;
	}
	if (IS_HOUSE1_MAP(p->tiles->id)) {
		num = 2;
	}
	if (IS_HOUSE2_MAP(p->tiles->id)) {
		num = 3;
	}

	if (p->id == uid)
	{
		msglog(statistic_logfile, 0x04040108,get_now_tv()->tv_sec, &(p->id), 4);
	}

	DEBUG_LOG("USER HOME\t[%u %u %u]", p->id, uid, num);
	return db_get_home_i(p, &num, uid);
}

static inline int
home_guest_num_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);

	map_t* m = get_map(p->id);
	notify_home_users_change(p, m ? m->sprite_num : 0, 1);
	return 0;
}

static inline int
map_info_cmd(sprite_t *p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 8);
	map_id_t mapid;
	int i = 0;
	if (unpkg_mapid(body, &mapid, &i) == -1) return -1;

	DEBUG_LOG("GET MAP\t[%u %lu]", p->id, mapid);
	return do_map_info(p, mapid);
}

static inline int
list_user_cmd(sprite_t *p, const uint8_t *body, int len)
{
	CHECK_BODY_LEN(len, 12);
	int i = 0;
	map_id_t mapid;
	uint32_t grid;
	if (unpkg_mapid(body, &mapid, &i) == -1) return -1;
	UNPKG_UINT32(body,grid,i);

	DEBUG_LOG("LIST USER\t[%u %lu self=%u grid=%u]", p->id, mapid, p->sub_grid,grid);
	return do_list_user(p, mapid, grid);
}

static inline int
action_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint8_t direction;
	uint32_t action;
	if (unpkg_uint32_uint8(body, bodylen, &action, &direction) == -1)
		return -1;

	//DEBUG_LOG("ACTION\t[%u %u]", p->id, action);
	return do_action(p, action, direction);
}

static inline int
action2_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t action;
	if ( unpkg_uint32(body, bodylen, &action) == -1 )
		return -1;

	DEBUG_LOG("ACTION2\t[%u %u]", p->id, action);
	return do_action2(p, action);
}

static inline int
find_blackboard_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	uint32_t board, msgid;
	int i = 0;

	if (unpkg_uint32_uint32(body, bodylen, &board, &msgid) == -1)
		return -1;

	DEBUG_LOG("FIND BOARD\t[%u %u %u]", p->id, board, msgid);
	PKG_H_UINT32(body, board, i);
	PKG_H_UINT32(body, msgid, i);
	return send_request_to_db(SVR_PROTO_FIND_BOARD, p, 8, body, p->id);
}

static inline int
get_blackboard_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	uint32_t board, page;
	int i = 0;
	if (unpkg_uint32_uint32(body, bodylen, &board, &page) == -1)
		return -1;

	DEBUG_LOG("GET BOARD\t[%u %u %u]", p->id, board, page);
	PKG_H_UINT32(body, board, i);
	PKG_H_UINT32(body, page, i);
	return send_request_to_db(SVR_PROTO_GET_BOARD, p, 8, body, p->id);
}

static inline int
get_passed_boardmsg_by_id_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);
	uint32_t buff[3];
	int j = 0;
	UNPKG_UINT32(body, buff[0], j);
	UNPKG_UINT32(body, buff[1], j);
	UNPKG_UINT32(body, buff[2], j);

	DEBUG_LOG("GET PASSED BOARD MSG\t[%u %u %u %u]", p->id, buff[0], buff[1], buff[2]);
	return send_request_to_db(SVR_PROTO_GET_PASSED_BOARD_MSG, p, 12, buff, 0);
}

static inline int
post_blackboard_cmd(sprite_t *p, uint8_t *body, int bodylen)
{
	uint8_t *content;
	uint32_t board, msglen;

	if (unpkg_post_blackboard(body, bodylen, &board, &msglen, &content) == -1)
		return -1;
	CHECK_DIRTYWORD(p, content);
	DEBUG_LOG("POST BOARD\t[%u %u %u %u]", p->id, p->color, board, msglen);
	return do_post_blackboard(p, board, msglen, content);
}

static inline int
add_flower_to_blkboard_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	uint32_t board, msgid;
	if (unpkg_uint32_uint32 (body, bodylen, &board, &msgid) == -1) {
		return -1;
	}

	DEBUG_LOG("ADD FLOWER TO BLKBOARD\t[%u %u %u]", p->id, board, msgid);

	int i = 0;
	PKG_H_UINT32(body, board, i);
	PKG_H_UINT32(body, msgid, i);
	send_request_to_db(SVR_PROTO_BOARD_FLOWER, 0, 8, body, p->id);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

static inline int
post_message_cmd (sprite_t *p, uint8_t *body, int bodylen)
{
	uint8_t *content;
	uint32_t type, msglen;
	int i;

	if (unpkg_post_message (body, bodylen, &type, &msglen, &content) == -1)
		return -1;

	i = 0;
	PKG_H_UINT32 (body, type, i);
	PKG_H_UINT32 (body, msglen, i);

	DEBUG_LOG("POST MSG\t[%u %u %u]", p->id, type, msglen);
//	#ifdef TW_VER
//	db_post_msg_tw(p, body, bodylen);
//	#else
	db_post_msg (p, body, bodylen);
//	#endif
	if (type == 2008) {
		db_set_cnt(p, 135, 1);
	}
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

static inline int
jail_user_cmd (sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t userid, reason;

	if (unpkg_uint32_uint32 (body, bodylen, &userid, &reason) == -1)
		return -1;

	DEBUG_LOG("JAIL USER\t[%u %u %u]", p->id, userid, reason);
	if (p->profession[12] != 1) {
		do_jail_user(p, userid, reason, 0);	
		response_proto_head(p, p->waitcmd, 0);
		uint32_t msgbuf[] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BD9E, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
	} else {
		*(uint32_t*)(p->session) = userid;
		*(uint32_t*)(p->session + 4) = reason;
		uint8_t db_buf[1024] = {0};
		int l = 0;
		PKG_H_UINT32(db_buf, 99, l);
		PKG_H_UINT32(db_buf, 1351131, l);
		PKG_H_UINT32(db_buf, 1351132, l);
		PKG_H_UINT8(db_buf, 0, l);

		uint32_t msgbuf[] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BDA0, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
		return send_request_to_db(SVR_PROTO_CHK_ITEM, p, l, db_buf, p->id);
	}

	return 0;
}

static inline int
exchange_item_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t ex_id;

	if (unpkg_uint32 (body, bodylen, &ex_id) == -1)
		return -1;
	if ( (ex_id > 170 && ex_id <= 220)  && !ISVIP(p->flag))
		ERROR_RETURN( ("Exchg ID=%u, %u not vip", ex_id, p->id), -1 );
	if (ex_id < 600) {
		//if((ex_id != 151) && (!check_recv_not_fini_task(p, ex_id))) {
		if(!exits[ex_id].not_recv && (!check_recv_not_fini_task(p, ex_id))) {
			ERROR_RETURN(("HAVE NOT RECV TASK %u %u", p->id, ex_id), -1);
		}

		if (exits[ex_id].not_recv && check_fini_task(p, ex_id))
		{
			ERROR_RETURN(("AREADY FIN TASK %u %u", p->id, ex_id), -1);
		}
	}
	DEBUG_LOG("EXCHG ITEM\t[%u %u]", p->id, ex_id);
	if (ex_id < 600) {
		if(ex_id == 72) {
			ERROR_RETURN(("can not fin [%d]", p->id), -1);
		}
		return do_exchange_item(p, ex_id, 0);
	}

	ERROR_RETURN( ("Exchg ID=%u out of bound", ex_id), -1 );
}

static inline int
set_task_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t taskid;
	uint8_t  status;

	if (unpkg_uint32_uint8(body, bodylen, &taskid, &status) == -1)
		return -1;

	if (taskid >= 600 || status > 2) {
		ERROR_RETURN(("error taskid=%d, id=%u, status=%d", taskid, p->id, status), -1);
	}
	if ((exits[taskid].id != 0) && (exits[taskid].pre_id != -1)) {
		if (!test_bit_on(p->tasks, sizeof p->tasks, exits[taskid].pre_id * 2 + 2)) {
			ERROR_LOG("not fini pre id taskid=%u, pre_id = %u, id = %u", taskid, exits[taskid].pre_id, p->id);
			return send_to_self_error(p, p->waitcmd, -ERR_not_fini_pre_taskid, 1);
		}
	}

	if (exits[taskid].mole_level_limit  > cal_mole_level(p->exp)) {
		ERROR_LOG("not enough level %u %u", p->id, taskid);
		return send_to_self_error(p, p->waitcmd, -ERR_mole_level_not_enough, 1);
	}

	if((taskid > 170 && taskid < 220) && !ISVIP(p->flag))
		ERROR_RETURN(("error taskid=%d, id=%u, status=%d, not vip", taskid, p->id, status), -1);

	uint8_t old_status = 0;
	if (test_bit_on(p->tasks, sizeof p->tasks, taskid * 2 + 1))
		old_status = 1;
	if (test_bit_on(p->tasks, sizeof p->tasks, taskid * 2 + 2))
		old_status = 2;

	if (taskid >= 24 && taskid != 131 && taskid != 141 && taskid != 161 && taskid != 221 && !exits[taskid].quit_task) {
		if (status == 0 || old_status == 2)
			ERROR_RETURN(("error task status taskid=%d, id=%u, status=%d old_status=%d", taskid, p->id, status, old_status), -1);
	}

	if (exits[taskid].not_limit) {
		if (old_status == 2) {
			ERROR_RETURN(("not limit status taskid=%d, id=%u, status=%d old_status=%d", taskid, p->id, status, old_status), -1);
		}
	}

	if (taskid == 0 || taskid == 8 || taskid == 17 || taskid == 131 || taskid == 141 || taskid == 161 || taskid == 221) {
	if (old_status == 2) {
			ERROR_RETURN(("error task status taskid=%d, id=%u, status=%d old_status=%d", taskid, p->id, status, old_status), -1);
	}
			//ERROR_RETURN(("error task status taskid=%d, id=%u, status=%d old_status=%d", taskid, p->id, status, old_status), -1);
	}
	DEBUG_LOG("SET TASK\t[%u %d %d "
				"0x%X %X %X %X %X %X %X %X %X %X "
				"%X %X %X %X %X %X %X %X %X %X "
				"%X %X %X %X %X %X %X %X %X %X "
				"%X %X %X %X %X %X %X %X %X %X "
				"%X %X %X %X %X %X %X %X %X %X]",
				p->id, taskid, status,
				p->tasks[0], p->tasks[1], p->tasks[2], p->tasks[3], p->tasks[4],
				p->tasks[5], p->tasks[6], p->tasks[7], p->tasks[8], p->tasks[9],
				p->tasks[10], p->tasks[11], p->tasks[12], p->tasks[13], p->tasks[14],
				p->tasks[15], p->tasks[16], p->tasks[17], p->tasks[18], p->tasks[19],
				p->tasks[20], p->tasks[21], p->tasks[22], p->tasks[23], p->tasks[24],
				p->tasks[25], p->tasks[26], p->tasks[27], p->tasks[28], p->tasks[29],
				p->tasks[30], p->tasks[31], p->tasks[32], p->tasks[33], p->tasks[34],
				p->tasks[35], p->tasks[36], p->tasks[37], p->tasks[38], p->tasks[39],
				p->tasks[40], p->tasks[41], p->tasks[42], p->tasks[43], p->tasks[44],
				p->tasks[45], p->tasks[46], p->tasks[47], p->tasks[48], p->tasks[49]);

	switch (status) {
	case 0:
		set_bit_off(p->tasks, sizeof p->tasks, taskid * 2 + 1);
		set_bit_off(p->tasks, sizeof p->tasks, taskid * 2 + 2);
		break;
	case 1:
	{
		msglog(statistic_logfile, 0x04040104,get_now_tv()->tv_sec, &(p->id), 4);

		uint32_t recv_task[2] = {1,0};
		if (taskid == 0){
		    msglog(statistic_logfile, 0x0409BEE5,get_now_tv()->tv_sec, recv_task, sizeof(recv_task));
		}
		else {
		    msglog(statistic_logfile, 0x02022000 + taskid,get_now_tv()->tv_sec, recv_task, sizeof(recv_task));
		}
		set_bit_on(p->tasks, sizeof p->tasks, taskid * 2 + 1);
		set_bit_off(p->tasks, sizeof p->tasks, taskid * 2 + 2);
		break;
	}
	case 2:
	{
		uint32_t fini_task[2] = {0,1};
		if (taskid == 0){
		    msglog(statistic_logfile, 0x0409BEE5,get_now_tv()->tv_sec, fini_task, sizeof(fini_task));
		}
		else {
		    msglog(statistic_logfile, 0x02022000 + taskid,get_now_tv()->tv_sec, fini_task, sizeof(fini_task));
		}
		set_bit_off(p->tasks, sizeof p->tasks, taskid * 2 + 1);
		set_bit_on(p->tasks, sizeof p->tasks, taskid * 2 + 2);
		break;
	}
	default:
		break;
	}

	uint32_t* tempTaskID = ((uint32_t*)p->session);
	*tempTaskID = taskid;
	return db_set_task(p, taskid, status);
}

static inline int
get_task_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t userid;
	if (unpkg_uint32(body, bodylen, &userid) == -1) {
		return -1;
	}
	CHECK_VALID_ID (userid);

	sprite_t* who = get_sprite(userid);
	if (!who) {
		ERROR_RETURN(("unknow sprite, id=%u, %u", p->id, userid), -1);
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, 600, i);
	i += pack_user_tasks(p, msg + i);

	//DEBUG_LOG("GET TASK\t[%u %u 0x%llX]", p->id, userid, p->tasks);
	init_proto_head(msg, PROTO_GET_TASK, i);
	return send_to_self(p, msg, i, 1);
}

static inline int
find_item_cmd (sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t itemid;
	if (unpkg_uint32(body, bodylen, &itemid) == -1)
		return -1;

	DEBUG_LOG("FIND ITEM\t[%u %u]", p->id, itemid);
	return do_find_item(p, itemid);
}

static inline int
throw_item_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t item, x, y;
	if (unpkg_throw_item(body, bodylen, &item, &x, &y) == -1)
		return -1;

	DEBUG_LOG("THROW ITEM\t[%u %d %u %u]", p->id, item, x, y);
	return do_throw_item(p, item, x, y);
}

static inline int
paopao_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	if (unpkg_paopao(body, bodylen) == -1) return -1;

	DEBUG_LOG("PAOPAO\t\t[%u %lu %u %u]", p->id, p->tiles->id, p->posX, p->posY);
	return do_paopao(p);
}

static inline int
walk_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 16);

	int i = 0;
	uint32_t xpos, ypos, show, grid;

	UNPKG_UINT32(body, xpos, i);
	UNPKG_UINT32(body, ypos, i);
	UNPKG_UINT32(body, show, i);
	UNPKG_UINT32(body, grid, i);

	if(p->sub_grid != grid) {
		if(grid_nearly(p->sub_grid,grid)) {
			response_proto_leave_map(p, 0);
			p->posX = (p->sub_grid > grid ? 959:0);
			xpos = (p->sub_grid > grid ? 954:5);
			p->sub_grid = grid;
			response_proto_get_sprite(p, PROTO_MAP_ENTER, 0, NULL);
		}
	}

	return do_walk(p, xpos, ypos, show);
}

static inline int
enter_map_cmd (sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 24);

	int i = 0;
	uint32_t gdst,gsrc;
	map_id_t newmap, oldmap;
	if (unpkg_mapid(body, &newmap, &i) == -1) return -1;
	if (unpkg_mapid(body, &oldmap, &i) == -1) return -1;
	unpkg_uint32_uint32(body + i,bodylen - i,&gdst,&gsrc);

	DEBUG_LOG("ENTER MAP\t[%u %lu %lu %u %u]", p->id, newmap, oldmap, p->posX, p->posY);

	if (IS_NORMAL_MAP(newmap)) {
		map_t* mp = get_map(newmap);
		if (!mp) {
			ERROR_RETURN(("Invalid map id"), -1);
		}
		struct tm tm_tmp = *get_now_tm();
		if (tm_tmp.tm_hour < mp->open_hour || tm_tmp.tm_hour >= mp->close_hour) {
			return send_to_self_error(p, p->waitcmd, -ERR_this_scene_closed, 1);
		}
		if (newmap == 72) {
			if (mp->sprite_num >= 10) {
				return send_to_self_error(p, p->waitcmd, -ERR_max_num_in_ship, 1);
			}
		}
		if (newmap == 102 || newmap == 103 || newmap == 104) {
			if (!ISVIP(p->flag)) {
				return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
			}
		}
	}
	if (p->action >= 10004 && p->action <= 10011) {
		if (p->action == 10010 || p->action == 10011)
			pos_dance[p->action == 10010 ? 0 : 1] = 0;
		p->action = 0;
	}
	if (IS_NORMAL_MAP(newmap))
	{
		return do_enter_map(p, newmap, oldmap,gdst,gsrc);
	}
	else
	{
		*(uint64_t*)p->session       = newmap;
		*(uint64_t*)(p->session + 8) = oldmap;
		*(uint32_t*)(p->session + 16) = gdst;
		*(uint32_t*)(p->session + 20) = gsrc;

		uint32_t userid = 0;
		if (IS_JY_MAP(newmap))
		{
		   userid = GET_UID_IN_JY(newmap);
		}
		else
		{
			userid = ((uint32_t)(newmap));
		}
		if (userid == p->id)
		{
			return do_enter_map(p, newmap, oldmap,gdst,gsrc);
		}

		return send_request_to_db(SVR_PROTO_USER_CHECKIN_BLACK, p, 4, &(p->id), userid);
	}

}

static inline int
leave_map_cmd (sprite_t *p, const uint8_t *body, int bodylen)
{
	if (unpkg_leave_map(body, bodylen) == -1) return -1;

	return do_leave_map(p);
}

static inline int
add_blacklist_cmd (sprite_t *p, const uint8_t *body, int len)
{
	uint32_t id;
	if (unpkg_uint32(body, len, &id) == -1) return -1;

	CHECK_VALID_ID(id);

	DEBUG_LOG ("ADD BKLIST\t[%u %u]", p->id, id);
	return do_add_blacklist(p, id);
}

static inline int
del_blacklist_cmd(sprite_t* p, const uint8_t *body, int len)
{
	uint32_t id;
	if (unpkg_uint32(body, len, &id) == -1)
		return -1;

	CHECK_VALID_ID(id);

	DEBUG_LOG ("DEL BKLIST\t[%u %u]", p->id, id);
	return do_del_blacklist(p, id);
}

static inline int
get_blacklist_cmd(sprite_t* p, const uint8_t* body, int len)
{
	if (unpkg_get_blacklist_cmd(body, len) == -1) return -1;

	DEBUG_LOG ("GET BLACK\t[%u]", p->id);
	if (IS_GUEST_ID (p->id) || IS_NPC_ID (p->id)) {
		int k = sizeof (protocol_t);
		PKG_UINT32 (msg, 0, k);
		init_proto_head (msg, PROTO_BKLIST_GET, k);
		return send_to_self (p, msg, k, 1);
	}
	return db_get_bklist(p);
}

static inline int
del_friend_cmd(sprite_t *p, const uint8_t *body, int len)
{
	uint32_t id;
	if (unpkg_uint32(body, len, &id) == -1) return -1;

	CHECK_VALID_ID(id);
	DEBUG_LOG ("DEL FRIEND\t[%u %u]", p->id, id);
	return do_del_friend(p, id);
}

static inline int
add_friend_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t id;
	if (unpkg_uint32(body, len, &id) == -1) {
		return -1;
	}
	CHECK_VALID_ID(id);

	*(uint32_t*)p->session 	= id;

	return send_request_to_db(SVR_PROTO_USER_CHECKIN_BLACK, p, sizeof(id), &id, p->id);
}

static inline int
reply_add_friend_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t initiator;
	uint8_t  accept;
	if (unpkg_uint32_uint8(body, len, &initiator, &accept) == -1) {
		return -1;
	}
	CHECK_VALID_ID(initiator);
	*(uint8_t*)p->session = accept;
	DEBUG_LOG("REPLY ADD FRIEND\t[addee=%u adder=%u accept=%d]", p->id, initiator, accept);
	return send_request_to_db( SVR_PROTO_ADD_FRIEND_REP, p, sizeof(p->id), &p->id, initiator );
}

int
reply_add_friend_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t result;
	CHECK_BODY_LEN( len, sizeof(result) );
	result = *(uint32_t*)buf;
	if( result == 1 )
	{
		uint8_t accept = *(uint8_t*)p->session;
		return do_add_friend_rsp( p, id, accept );
	}
	else
	{
		return send_to_self_error( p, p->waitcmd, ERR_add_friend_rep_invalid, 1 );
	}
}

static inline int
reply_invite_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t id;
	uint8_t accept;
	map_id_t map_id;
	if (unpkg_reply_invite(body, len, &id, &map_id, &accept) == -1)
		return -1;

	CHECK_VALID_ID(id);
	DEBUG_LOG("INVITE RSP\t[%u %lu %u]", p->id, p->tiles->id, id);
	return do_reply_invite(p, id, map_id, accept);
}

static inline int
invite_friend_cmd(sprite_t *p, const uint8_t *body, int len)
{
	uint32_t id;
	map_id_t map_id;
	CHECK_BODY_LEN(len, 12);
	int i = 0;
	UNPKG_UINT32(body, id, i);
	unpkg_mapid(body, &map_id, &i);

	if (map_id == 72)
		return send_to_self_error(p, p->waitcmd, ERR_cannot_invite_to_this_map, 1);

	CHECK_VALID_ID(id);
	DEBUG_LOG("INVITE REQ\t[%u %lu %u]", p->id, map_id, id);
	return do_invite_friend(p, id, map_id);
}

static inline int
discard_item_cmd(sprite_t *p, const uint8_t *body, int len)
{
	uint32_t id;
	uint16_t used;
	if (unpkg_uint32_uint16(body, len, &id, &used) == -1)
		return -1;

	if ( used && (unwear_item(p, id) != 0) ) {
		return -1;
	}

	DEBUG_LOG("DISCARD ITEM\t[uid=%u itmid=%u used=%d]", p->id, id, used);
	return db_single_item_op(p, p->id, id, 1, used ? 4 : 0);
}

static inline int
show_item_use_cmd (sprite_t *p, const uint8_t *body, int len)
{
	uint32_t id, itemid;
	if (unpkg_uint32_uint32(body, len, &id, &itemid) == -1)
		return -1;

	if (p->tiles && p->tiles->id == 60) {
		return send_to_self_error(p, PROTO_SHOW_ITEM_USE, -ERR_cannot_change_oth_in_this_map, 1);
	}
	sprite_t* ps = get_sprite(id);
	if(!ps)
		ps = get_across_svr_sprite(id, p->tiles->id);
	if (ps && ps->car_status) {
		return send_to_self_error(p, PROTO_SHOW_ITEM_USE, -ERR_client_not_proc, 1);
	}

	item_kind_t *ik;
	ik = find_kind_of_item(itemid);
	//汤圆投掷变身特殊处理
	if (!ik || ( itemid != 150017 && ik->kind != CHG_SHOW_ITEM_KIND ) )
		ERROR_RETURN(("error item=%u", itemid), -1);
	if ( get_now_tv()->tv_sec < p->last_show_item + 2)
		return send_to_self_error(p, PROTO_SHOW_ITEM_USE, -ERR_user_offline, 1);

	//自己不能把自己变成汤圆，变成汤圆后也不能攻击
	if( p->id == id && itemid == 150017 )
	{
		return send_to_self_error(p, PROTO_SHOW_ITEM_USE, -ERR_ld_tar_invalid_attack, 1);
	}

	if (itemid == 17032){
	    uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3E4,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}

	DEBUG_LOG("SHOW ITEM\t[%u %u %u]", p->id, id, itemid);

	p->sess_len = 0;
	PKG_H_UINT32(p->session, id, p->sess_len);
	return db_single_item_op(p, p->id, itemid, 1, 0);
}

static inline int
cancel_transform_cmd (sprite_t *p, const uint8_t *body, int len)
{
	uint32_t id, itemid;

	if(!is_wear_item(p, 12336))
		ERROR_RETURN(("not wear magic stick\t[changer=%u]", p->id), -1);

	if (unpkg_uint32_uint32(body, len, &id, &itemid) == -1)
		return -1;

	DEBUG_LOG("CANCEL TRANSFORM\t[%u %u %d]", p->id, id, itemid);

	sprite_t* changee = get_sprite(id);
	if (!changee && !(changee = get_across_svr_sprite(id, p->tiles->id))) {
		return send_to_self_error(p, p->waitcmd, -ERR_user_offline, 1);
	}
	if (changee)
		changee->action = 0;

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, id, i);
	PKG_UINT32(msg, itemid, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);

	return 0;
}


static inline int
list_item_cmd (sprite_t *p, const uint8_t *body, int len)
{
	uint32_t type, id;
	uint8_t flag;
	uint8_t kind_flag = 0;
	if (unpkg_list_items(body, len, &id, &type, &flag, &kind_flag) == -1)
	{
		return -1;
	}

	if (flag > 2 || id == 0 )
	{
		ERROR_RETURN(("error packet, flag=%d, type=%u, id=%u,%u", flag, type, id, p->id), -1);
	}

	DEBUG_LOG("LIST ITEM\t[%u %d %d]", p->id, flag, kind_flag);
	return do_list_items(p, id, type, flag, kind_flag);
}


static inline int
modify_nick_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN (bodylen, 16);
	notify_stamp_svr (p->id);

	char tmp[17];
	memcpy(tmp, body, 16);
	tmp[16] = '\0';
	CHECK_DIRTYWORD(p, tmp);
	char* bad_char = "\n\v\t\f\r";
	if (strpbrk(tmp, bad_char)) {
		return send_to_self_error(p, p->waitcmd, -ERR_dirty_word, 1);
	}

	memcpy(p->nick, tmp, USER_NICK_LEN);

	DEBUG_LOG("SET NICK\t[%u]", p->id);
	return db_set_user_nick(p, tmp);
}

static inline int
set_stage_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint16_t pos, tobe;
	if ( unpkg_uint16_uint16(body, bodylen, &pos, &tobe) == -1 ) {
		return -1;
	}
	if ( (pos < 2) || (tobe > 1) ) {
		ERROR_RETURN( ("Invalid Req: pos=%d tobe=%d", pos, tobe), -1 );
	}

	p->flag = (tobe ? set_bit_on32(p->flag, pos) : set_bit_off32(p->flag, pos));

	DEBUG_LOG("SET STAGE\t[uid=%u pos=%d tobe=%d flag=0x%X]", p->id, pos, tobe, p->flag);
	return db_set_user_flag(p, set_bit_on32(p->flag, 1), p->id);
}

static inline int
modify_color_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	item_kind_t *ik;

	uint32_t color, item;
	if (unpkg_uint32_uint32(body, bodylen, &item, &color) == -1)
		return -1;

	if (item > 0) {
		ik = find_kind_of_item (item);
		if (!ik || ik->kind != CHG_COLOR_ITEM_KIND)
			ERROR_RETURN (("error item=%u, color=%u", item, color), -1);
	}
	//DEBUG_LOG ("SET COLOR\t[%u %u %u]", p->id, color, item);
	return do_modify_color(p, color, item);
}

static inline int
get_friend_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t id;
	if (unpkg_uint32(body, bodylen, &id) == -1)
		return -1;

	DEBUG_LOG("GET FRIEND\t[chker=%u chkee=%u]", p->id, id);
	CHECK_VALID_ID(id);
	*(uint32_t *)p->session = id;
	
	return do_get_friend(p, id);
}

static inline int
get_user_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t id;
	if (unpkg_uint32(body, bodylen, &id) == -1)
		return -1;

	DEBUG_LOG("USER INFO\t[chker=%u chkee=%u]", p->id, id);
	return do_get_user(p, id);

}

void rsp_search_user(sprite_t* chker, const sprite_t* chkee, uint16_t online_id)
{
	static char mapname[64];

	int i = sizeof(protocol_t);
	PKG_UINT16(msg, online_id, i);
	if (chkee) {
		const map_t* m = chkee->tiles;
		if (m) {
#ifdef TW_VER
			char* loc[] = {"", "的小屋", "的家園", "的班级", "的牧場"};
#else
			char* loc[] = {"", "的小屋", "的家园", "的班级", "的牧场"};
#endif
			int indx = 0;
			if (IS_HOUSE_MAP(m->id))
				indx = 1;
			else if (IS_JY_MAP(m->id))
				indx = 2;
			else if (IS_CLASS_MAP(m->id))
				indx = 3;
			else if (IS_PASTURE_MAP(m->id))
				indx = 4;

			snprintf(mapname, sizeof mapname, "%s%s", m->name, loc[indx]);
		} else {
#ifdef TW_VER
			strncpy(mapname, "他正在切換地圖中，請晚些再尋找他一次", sizeof mapname);
#else
			strncpy(mapname, "三界之外", sizeof mapname);
#endif
		}
		PKG_UINT32(msg, (m ? m->id : 0), i);
		PKG_STR(msg, mapname, i, 64);
	} else if (online_id) {
		i += 68;
	} else {
		send_to_self_error(chker, chker->waitcmd, -ERR_user_offline, 1);
		return;
	}
	init_proto_head(msg, chker->waitcmd, i);
	send_to_self(chker, msg, i, 1);
}

static inline int
search_user_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t chkee_id;
	if (unpkg_uint32(body, bodylen, &chkee_id) == -1) {
		return -1;
	}

	const sprite_t* chkee = get_sprite(chkee_id);
	if (chkee) {
		rsp_search_user(p, chkee, 0);
	} else {
		chk_user_where(p, chkee_id);
	}
	return 0;
}

static inline int
talk_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	int len;
	uint32_t id;
	if (unpkg_talk(body, bodylen, &id, &len) == -1) {
		return -1;
	}
	if ( IS_NPC_ID(id) || IS_GUEST_ID(id) ) {
		ERROR_RETURN(("error id=%u, %u", p->id, id), -1);
	}
	body[bodylen - 1] = '\0';

	send_chatlog(SVR_PROTO_CHATLOG, p, id, body + 8, len);
	CHECK_DIRTYWORD(p, body + 8);

	DEBUG_LOG("TALK\t\t[%u %u]", p->id, id);
	return do_talk(p, body + 8, len, id);
}

static inline int
users_online_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int count, return_all;
	uint32_t uin[SPRITE_FRIENDS_MAX];
	if (unpkg_users_online(body, bodylen, &return_all, &count, uin) == -1)
		return -1;

	DEBUG_LOG("CHK IF USERS ONLINE\t[chker=%u cnt=%d all=%d]", p->id, count, return_all);
	return do_users_online(p, count, uin, return_all);
}

static int special_action_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint16_t type;
	if (unpkg_special_action(body, bodylen, &type, &(p->posX), &(p->posY)) == -1) {
		return -1;
	}
	// do special action
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_STR(msg, body, i, bodylen);
	init_proto_head(msg, PROTO_SPECIAL_ACTION, i);
	send_to_map(p, msg, i, 1);

	if (p->followed) {
		switch (type) {
		case 1: // Sliding
			calc_pet_attr(p->id, p->followed, 0, 0, 0, 10);
			//db_pet_op(0, p->followed, p->id);
			break;
		case 10001: // waterball
			calc_pet_attr(p->id, p->followed, 0, 0, 2, 0);
			//db_pet_op(0, p->followed, p->id);
			break;
		case 10002: // tomato
			calc_pet_attr(p->id, p->followed, 2, 0, 0, 0);
			//db_pet_op(0, p->followed, p->id);
			break;
		default:
			break;
		}
	}
	//DEBUG_LOG("SPECIAL ACTION\t[uid=%u x=%d y=%d]", p->id, p->posX, p->posY);
	return 0;
}

static inline int
promotion_tips_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 2);

	uint16_t level = ntohs( *((uint16_t*)body) );

	do_promotion_tips(p, level);
	p->waitcmd = 0;
	return 0;
}

static inline int
event_status_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t evid;
	int i = 0;
	UNPKG_UINT32(body, evid, i);

	//DEBUG_LOG("EVENT STATUS\t[uid=%u evid=%u]", p->id, evid);
	return event_dispatcher(p, evid);
}

//----------------------- For Collecting Items Begin -----------------------
static inline int
rand_item_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	notify_rand_item_info(p, 0);
	return 0;
}

static inline int
collect_item_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t pos, itemid;
	if (unpkg_uint32_uint32(body, bodylen, &pos, &itemid) == -1) {
		return -1;
	}

	DEBUG_LOG("COLLECT RANDITEM\t[uid=%u pos=%u itmid=%u]", p->id, pos, itemid);
	return do_collect_item(p, pos, itemid);
}

static inline int
exchg_collected_item_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 14);

	uint16_t cnt;
	uint32_t ex_id, item_num, itemid;

	unpkg_uint32_uint16(body, 6, &ex_id, &cnt);
	unpkg_uint32_uint32(body + 6, 8, &item_num, &itemid);

	DEBUG_LOG("EXCHG COLLECTED ITEM\t[uid=%u exid=%u itemnum=%u]", p->id, ex_id, item_num);
	if ( (ex_id > 1000) && (ex_id < 1500) && (item_num > 0) ) {
		return do_exchange_item(p, ex_id, item_num);
	}

	ERROR_RETURN( ("Invalid Exchg ID=%u or Item Num=%u", ex_id, item_num), -1 );
}
//----------------------- For Collecting Items End -----------------------

//--------------------------------------------------------------------
static inline int
upd_5basic_attr_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return do_db_attr_op(p, p->id, 0, 0, 0, 0, 0, ATTR_CHG_chk_attr, 0);
}

static inline int
chk_sth_cnt_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t type;

	if (unpkg_uint32(body, bodylen, &type) == 0) {
		return db_chk_cnt(p, type);
	}

	return -1;
}

static inline int
get_cnt_list_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t start, end;
	if (unpkg_uint32_uint32(body, bodylen, &start, &end) == 0) {
		if (start > end) {
			ERROR_RETURN(("bad start and end\t[%u %u %u]", p->id, start, end), -1);
		}
		db_get_cnt_list(p, start, end);
		return 0;
	}
	return -1;
}

int notify_systime(sprite_t* p, map_id_t* maps)
{
	const struct timeval* tv = get_now_tv();

	int i = sizeof(protocol_t);

	PKG_UINT32(msg, tv->tv_sec, i);
	PKG_UINT32(msg, tv->tv_usec, i);
	init_proto_head(msg, PROTO_GET_SYSTIME, i);

	if (p) {
		return send_to_self(p, msg, i, 1);
	} else if (maps) {
		int j = 0;
		while (maps[j]) {
			send_to_map3(maps[j++], msg, i);
		}
	}

	return 0;
}

static inline int
get_systime_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	return notify_systime(p, 0);
}

static inline int
get_md5_session_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	unsigned char src[100];
	unsigned char md_out[16], md[32];

	if (IS_GUEST_ID(p->id))
		return send_to_self_error(p, p->waitcmd, ERR_client_not_proc, 1);

	time_t t = get_now_tv()->tv_sec;

	int len = snprintf((char*)src, sizeof src, "userid=%u&time=%ld&key=%s",
						p->id, t, signature_key);
	DEBUG_LOG("MD5\t[uid=%u src=%s]", p->id, src);
	MD5(src, len, md_out);

	int i;
	for (i = 0; i != 16; ++i) {
		sprintf((char*)md + i * 2, "%.2x", md_out[i]);
	}

	i = sizeof(protocol_t);
	PKG_UINT32(msg, t, i);
	PKG_STR(msg, md, i, 32);
	init_proto_head(msg, p->waitcmd, i);

	//DEBUG_LOG("MD5\t[uid=%u src=%s md=%s]", p->id, src, md);
	return send_to_self(p, msg, i, 1);
}

static inline int
chk_vip_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	DEBUG_LOG("chk vip %u", p->id);
	return db_chk_vip_info(p);
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
static inline int
submit_personal_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint8_t  gender;
	uint32_t birth;
	if ( unpkg_uint32_uint8(body, bodylen, &birth, &gender) == -1 ) {
		return -1;
	}

	if ( (birth > 19000100) && (gender < 2) ) {

#ifdef TW_VER
		char buf[8];
		int i = 0;
		PKG_H_UINT32(buf, birth, i);
		PKG_H_UINT32(buf, gender, i);
		DEBUG_LOG("PERSONAL INFO\t[uid=%u birth=%u gender=%d]", p->id, birth, gender);
		return db_submit_personal_info(p, buf);
#else
		typedef struct _personal_info{
			uint16_t channel_id;
			char verify_info[32];
			uint32_t birth;
			uint32_t gender;
		} __attribute__ ((packed)) personal_info_t;
		personal_info_t personal_info_req;

		unsigned char src[100];
		unsigned char md_out[16], md[33];
		uint16_t channel_id = config_get_intval("channel_id", 0);
		uint32_t security_code = config_get_intval("security_code", 0);
		DEBUG_LOG("userid %d channel_id %d security_code %d", p->id, channel_id, security_code);

		personal_info_req.channel_id = channel_id;
		personal_info_req.birth = birth;
		personal_info_req.gender = gender;

		int len = snprintf((char*)src, sizeof src, "channelId=%d&securityCode=%u&data=", channel_id, security_code);
		*(uint32_t*)(src + len) = birth;
		*(uint32_t*)(src + len + 4) = gender;
		MD5(src, len + 8, md_out);
		int i;
		for (i = 0; i != 16; ++i) {
			sprintf((char*)md + i * 2, "%.2x", md_out[i]);
		}

		memcpy(personal_info_req.verify_info, md, 32);
		return send_request_to_db(SVR_PROTO_SUBMIT_PERSONAL_INFO, p, sizeof(personal_info_req), &personal_info_req, p->id);
#endif


	}
	ERROR_RETURN( ("Invalid Data: uid=%u birth=%u gender=%d", p->id, birth, gender), -1 );
}

static inline int
chk_if_info_submitted_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	//DEBUG_LOG("CHK IF INFO SUBMITTED\t[uid=%u]", p->id);
	return db_chk_if_info_submitted(p);
}

int init_proc_msg()
{
#define POST_MSG(n,h)	if (dispatch[n]) { return -1; } else dispatch[n] = (dispatcher_t)h
	POST_MSG(PROTO_LOGIN, auth_cmd);
	POST_MSG(PROTO_USER_SIMPLY, get_user_cmd);
	POST_MSG(PROTO_USER_DETAIL, get_user_cmd);
	POST_MSG(PROTO_GROUP_INFO, get_group_cmd);
	POST_MSG(PROTO_FRIEND_INFO, get_friend_cmd);
	POST_MSG(PROTO_NICK_MODIFY, modify_nick_cmd);
	POST_MSG(PROTO_COLOR_MODIFY, modify_color_cmd);
	POST_MSG(PROTO_SET_STAGE, set_stage_cmd);

	POST_MSG(PROTO_BUY_PET, buy_pet_cmd);
	POST_MSG(PROTO_ASK_FOR_A_PET, ask_for_a_pet_cmd);
	POST_MSG(PROTO_GET_PET, get_pet_cmd);
	POST_MSG(PROTO_GET_PET_CNT, get_pet_cnt_cmd);
	POST_MSG(PROTO_GET_OUTSIDE_PETS, get_outside_pets_cmd);
	POST_MSG(PROTO_FOLLOW_PET, follow_pet_cmd);
	POST_MSG(PROTO_FOLLOW_OUTSIDE_PET, follow_outside_pet_cmd);
	POST_MSG(PROTO_SEND_PET_HOME, send_pet_home_cmd);
	POST_MSG(PROTO_FEED_ITEM_USE, feed_item_cmd);
	POST_MSG(PROTO_SET_PET_NICK, set_pet_nick_cmd);
	POST_MSG(PROTO_PLAY_WITH_PET, play_with_pet_cmd);
	POST_MSG(PROTO_SET_PET_POS, set_pets_pos_cmd);
	POST_MSG(PROTO_LEARN_PET_SKILL, learn_pet_skill_cmd);
	POST_MSG(PROTO_MODIFY_PET_COLOR, discolor_pill_modify_pet_color_cmd);
	//
	POST_MSG(PROTO_PET_TRUST, trust_pets_cmd);
	POST_MSG(PROTO_PET_WITHDRAW, withdraw_pets_cmd);
	POST_MSG(PROTO_GET_TRUSTED_PET, get_trusted_pets_cmd);
	//
	POST_MSG(PROTO_PICK_PET_TASK, pick_pet_task_cmd);
	POST_MSG(PROTO_TERMINATE_PET_TASK, terminate_pet_task_cmd);
	POST_MSG(PROTO_SET_PET_TASK_FIN, set_pet_task_fin_cmd);
	POST_MSG(PROTO_GET_PET_TASK_LIST, get_pet_task_list_cmd);
	POST_MSG(PROTO_GET_PETS_ON_TASK, get_pets_on_task_cmd);
	POST_MSG(PROTO_PET_HOUSEWORK, lahm_housework_cmd);
	POST_MSG(PROTO_CALL_SUPER_LAHM, call_super_lahm_cmd);

	POST_MSG(PROTO_PICK_PET_MAGIC_TASK, pick_pet_magic_task_cmd);
	POST_MSG(PROTO_FIN_PET_MAGIC_TASK, fin_pet_magic_task_cmd);
	POST_MSG(PROTO_CANCEL_PET_MAGIC_TASK, cancel_pet_magic_task_cmd);
	POST_MSG(PROTO_SET_MAGIC_TASK_DATA, set_magic_task_data_cmd);
	POST_MSG(PROTO_GET_MAGIC_TASK_DATA, get_magic_task_data_cmd);
	POST_MSG(PROTO_GET_PET_MAGIC_TASK, get_pet_magic_task_list_cmd);
	//
	POST_MSG(PROTO_BECOME_SUPER_LAHM, become_super_lahm_cmd);
	POST_MSG(PROTO_HAS_SUPER_LAHM, has_super_lahm_cmd);

	POST_MSG(PROTO_GET_TASK, get_task_cmd);
	POST_MSG(PROTO_SET_TASK, set_task_cmd);

	POST_MSG(PROTO_SEARCH_USER, search_user_cmd);

	POST_MSG(PROTO_GET_TASK_TMP_INFO, get_task_tmp_info_cmd);
	POST_MSG(PROTO_SET_TASK_TMP_INFO, set_task_tmp_info_cmd);
	POST_MSG(PROTO_GET_TASK_INFO, get_task_info_cmd);
	POST_MSG(PROTO_SET_TASK_INFO, set_task_info_cmd);
	POST_MSG(PROTO_DEL_TASK_INFO, del_task_info_cmd);

	POST_MSG(PROTO_TALK, talk_cmd);
	POST_MSG(PROTO_WALK, walk_cmd);
	POST_MSG(PROTO_PAOPAO, paopao_cmd);
	POST_MSG(PROTO_ACTION, action_cmd);
	POST_MSG(PROTO_ACTION2, action2_cmd);

	POST_MSG(PROTO_THROW, throw_item_cmd);
	POST_MSG(PROTO_FIND_ITEM, find_item_cmd);
	POST_MSG(PROTO_EXCHANGE_ITEM, exchange_item_cmd);
	POST_MSG(PROTO_RAND_ITEM_INFO, rand_item_info_cmd);
	POST_MSG(PROTO_COLLECT_ITEM, collect_item_cmd);
	POST_MSG(PROTO_EXCHG_COLLECTED_ITEM, exchg_collected_item_cmd);
	POST_MSG(PROTO_DEMAND_RANDOM_ITEM, ritm_gen_on_demand_cmd);
	POST_MSG(PROTO_EXCHG_ITEM2, exchg_item2_cmd);

	POST_MSG(PROTO_ITEM_BUY, buy_item_cmd);
	POST_MSG(PROTO_ITEM_LIST, list_item_cmd);
	POST_MSG(PROTO_SHOW_ITEM_USE, show_item_use_cmd);
	POST_MSG(PROTO_ITEM_DISCARD, discard_item_cmd);
	//POST_MSG(PROTO_USER_ITEM_USE, use_item_cmd);
	POST_MSG(PROTO_USER_ITEM_USE, use_body_item_cmd);
	POST_MSG(PROTO_CHK_ITEM_AMOUNT, chk_item_amount_cmd);
	POST_MSG(PROTO_ASK_FOR_ITMES, ask_for_items_cmd);
	POST_MSG(PROTO_BARGAINING, bargaining_cmd);
	POST_MSG(PROTO_SELL_ITMES, sell_items_cmd);
	POST_MSG(PROTO_COMPOUND_ITEM, compound_item_cmd);
	POST_MSG(PROTO_CANCEL_CHANGE_FORM, cancel_transform_cmd);
	POST_MSG(PROTO_GET_ITEM_ARRAY, get_item_array_cmd);

	POST_MSG(PROTO_JAIL_USER, jail_user_cmd);
	POST_MSG(PROTO_POST_MESSAGE, post_message_cmd);
	POST_MSG(PROTO_POST_BLACKBOARD, post_blackboard_cmd);
	POST_MSG(PROTO_GET_BLACKBOARD, get_blackboard_cmd);
	POST_MSG(PROTO_FIND_BLACKBOARD, find_blackboard_cmd);
	POST_MSG(PROTO_ADD_FLOWER_TO_BLKBOARD, add_flower_to_blkboard_cmd);
	POST_MSG(PROTO_SPECIAL_ACTION, special_action_cmd);

	POST_MSG(PROTO_GET_PASSED_BOARD_MSG_BY_ID, get_passed_boardmsg_by_id_cmd);

	POST_MSG(PROTO_MAP_ENTER, enter_map_cmd);
	POST_MSG(PROTO_MAP_LEAVE, leave_map_cmd);
	POST_MSG(PROTO_GAME_ENTER, enter_game_cmd);
	POST_MSG(PROTO_GAME_LEAVE, leave_game_cmd);
	POST_MSG(PROTO_USER_LIST, list_user_cmd);
	POST_MSG(PROTO_MAP_INFO, map_info_cmd);
	POST_MSG(PROTO_MAPGAME_INFO, mapgame_info_cmd);
	POST_MSG(PROTO_MAPS_USER, list_map_cmd);
	POST_MSG(PROTO_GAME_SCORE, game_score_cmd);
	POST_MSG(PROTO_SIGPL_GAME_MSG, singleplayer_game_msg_cmd);
	POST_MSG(PROTO_USER_HOME, user_home_cmd);
	POST_MSG(PROTO_SET_HOME_ITEM, set_home_item_cmd);
	POST_MSG(PROTO_RESET_HOME, reset_home_item_cmd);
	POST_MSG(PROTO_GET_HOME_LIST, get_home_list_cmd);
	POST_MSG(PROTO_GET_VIP_HOME_LIST, get_vip_home_list_cmd);
	POST_MSG(PROTO_GET_RECENT_SPRITE, get_recent_sprite_in_map_cmd);

	POST_MSG(PROTO_HOME_USERS_CHANGE_NOTIFY, home_guest_num_cmd);
	POST_MSG(PROTO_PROMOTION_TIPS, promotion_tips_cmd);
	POST_MSG(PROTO_EVENT_STATUS, event_status_cmd);
	POST_MSG(PROTO_UPD_5BASIC_ATTR, upd_5basic_attr_cmd);

	//
	POST_MSG(PROTO_CHK_IF_STH_DONE, chk_if_sth_done_cmd);
	POST_MSG(PROTO_SET_STH_DONE, set_sth_done_cmd);
	POST_MSG(PROTO_CHK_STH_CNT, chk_sth_cnt_cmd);
	POST_MSG(PROTO_GET_CNT_LIST, get_cnt_list_cmd);

	//
	POST_MSG(PROTO_POLL, poll_cmd);

	POST_MSG(PROTO_FRIEND_INVITE, invite_friend_cmd);
	POST_MSG(PROTO_RSP_FRIEND_INVITE, reply_invite_cmd);
	POST_MSG(PROTO_FRIEND_ADD, add_friend_cmd);
	POST_MSG(PROTO_RSP_FRIEND_ADD, reply_add_friend_cmd);
	POST_MSG(PROTO_FRIEND_DEL, del_friend_cmd);
	POST_MSG(PROTO_BKLIST_ADD, add_blacklist_cmd);
	POST_MSG(PROTO_BKLIST_DEL, del_blacklist_cmd);
	POST_MSG(PROTO_BKLIST_GET, get_blacklist_cmd);
	POST_MSG(PROTO_USERS_ONLINE, users_online_cmd);
	POST_MSG(PROTO_CHK_JY_PET_CHANGE, chk_jy_and_pet_state_cmd);
	POST_MSG(PROTO_DEL_MUL_FRIENDS, del_mul_friends_cmd);

	// For Mole Messenger
	//POST_MSG(PROTO_MSNGER_INVITEE_CHK_IN, msnger_invitee_chk_in_cmd);
	POST_MSG(PROTO_CHK_MSNGER_INFO, chk_msnger_info_cmd);

	// For Wishing Pool
	POST_MSG(PROTO_MAKE_WISH, make_wish_cmd);
	POST_MSG(PROTO_FETCH_REALIZED_WISH, fetch_realized_wish_cmd);
	POST_MSG(PROTO_GET_REALIZED_WISHES, get_realized_wishes_cmd);
	POST_MSG(PROTO_CHK_IF_WISH_MADE, chk_if_wish_made_cmd);

	// For Postcard System
	POST_MSG(PROTO_SEND_POSTCARD, send_postcard_cmd);
	POST_MSG(PROTO_GET_POSTCARDS, get_postcards_cmd);
	POST_MSG(PROTO_READ_POSTCARD, read_postcard_cmd);
	POST_MSG(PROTO_DEL_POSTCARD, del_postcard_cmd);
	POST_MSG(PROTO_GET_UNREAD_CARD_NUM, get_unread_card_num_cmd);
	POST_MSG(PROTO_DEL_ALL_POSTCARD, del_all_postcard_cmd);
	POST_MSG(PROTO_SET_POSTCARD_GIFT_FLAG, set_postcard_gift_flag_cmd);
	POST_MSG(PROTO_GET_POSTCARDS_BASE_INFO, get_postcards_base_info_cmd);
	POST_MSG(PROTO_DEL_POSTCARDS, del_postcards_cmd);

	//For home information, about hot, flower, mud,visitor, and neighbor
	POST_MSG(PROTO_GET_HOME_HOT, get_home_hot_cmd);
	//POST_MSG(PROTO_GET_TOP_HOMES, get_top_hot_homes_cmd);
	POST_MSG(PROTO_VOTE_HOME, vote_home_flower_mud_cmd);
	POST_MSG(PROTO_GET_RECENT_VISITORS, get_recent_visitors_cmd);
	POST_MSG(PROTO_GET_HOME_NEIGHBOR, get_neighbors_cmd);
	POST_MSG(PROTO_ADD_HOME_NEIGHBOR, add_neighbors_cmd);
	POST_MSG(PROTO_LEAVE_HOME_MESSAGE, submit_message_cmd);
	POST_MSG(PROTO_VERIFY_HOME_MESSAGE, verify_message_cmd);
	POST_MSG(PROTO_GET_HOME_MESSAGES, get_messages_cmd);
	POST_MSG(PROTO_DEL_HOME_MESSAGE, del_message_cmd);
	POST_MSG(PROTO_REPLY_HOME_MESSAGE, reply_message_cmd);
	POST_MSG(PROTO_GET_MM_TREE_INFO, get_maomao_tree_info_cmd);
	POST_MSG(PROTO_FERTILIZE_WATER_TREE, fertilize_water_cmd);
	POST_MSG(PROTO_GET_FERTILIZED_MAN_LIST, list_fertilized_watered_cmd);
	POST_MSG(PROTO_PICK_FRUIT, pick_fruit_cmd);
	POST_MSG(PROTO_GET_HOME_TYPE_LIST, get_home_type_list_cmd);
	POST_MSG(PROTO_GET_SMC_INFO, get_smc_info_cmd);
	POST_MSG(PROTO_VOTE_SMC, vote_smc_cmd);

	// for birthday airship
	POST_MSG(PROTO_SET_BIRTHDAY, set_birthday_cmd);
	POST_MSG(PROTO_DRESS_BIRTHDAY_CLOTHES, dress_birthday_clothes_cmd);
	//POST_MSG(PROTO_ISSET_BIRTHDAY, isset_birthday_cmd);

	// for christmas
	//POST_MSG(PROTO_SET_CHRISTMAS_WISH, set_christmas_wish_cmd);
	//POST_MSG(PROTO_IS_SET_CHRISTMAS_WISH, is_set_christmas_wish_cmd);
	//POST_MSG(PROTO_GET_CHRISTMAS_WISH, get_christmas_wish_cmd);
	//POST_MSG(PROTO_IS_GET_CHRISTMAS_WISH, is_get_christmas_wish_cmd);
	//POST_MSG(PROTO_DRESS_CHRISTMAS_HAT, lahm_dress_hat_cmd);

	// fro yuanxiao
	POST_MSG(PROTO_SET_YUANXIAO_WISH, set_yuanxiao_wish_cmd);
	POST_MSG(PROTO_GET_YUANXIAO_WISH, get_yuanxiao_wish_cmd);

	POST_MSG(PROTO_CREATE_MOLEGROUP, create_molegroup_cmd);
	POST_MSG(PROTO_INVITE_JOIN_MOLEGROUP, add_member_to_molegroup_cmd);
	POST_MSG(PROTO_APPLY_FOR_JOINING_MOLEGROUP, apply_for_joining_molegroup_cmd);
	POST_MSG(PROTO_REPLY_TO_JOIN_MOLEGROUP, reply_to_join_molegroup_cmd);
	POST_MSG(PROTO_LEAVE_MOLEGROUP, leave_molegroup_cmd);
	POST_MSG(PROTO_KICK_OUT_OF_MOLEGROUP, kick_outof_molegroup_cmd);
	POST_MSG(PROTO_DEL_MOLEGROUP, delete_molegroup_cmd);
	POST_MSG(PROTO_MOD_MOLEGROUP_INFO, mod_molegroup_info_cmd);
	POST_MSG(PROTO_GET_USER_MOLEGROUP_LIST, get_user_molegroup_list_cmd);
	POST_MSG(PROTO_GET_MOLEGROUP_DETAIL, get_molegroup_detail_cmd);
	POST_MSG(PROTO_CHAT_IN_MOLEGROUP, chat_in_molegroup_cmd);
	POST_MSG(PROTO_SET_MOLEGROUP_FLAG, set_molegroup_flag_cmd);

	// Small requirements
	POST_MSG(PROTO_GET_QUES_ANSWER, get_ques_answer_cmd);
	POST_MSG(PROTO_GET_QUES_NUM, get_ques_num_cmd);
	POST_MSG(PROTO_INC_RUN_TIMES, inc_run_times_cmd);
	POST_MSG(PROTO_GET_RUN_TIMES, get_run_times_cmd);
	POST_MSG(PROTO_LAHM_PLAY, lahm_play_cmd);
	POST_MSG(PROTO_GET_USER_FLAG, get_user_flag_cmd);
	//POST_MSG(PROTO_BECOME_WATER_BUCKET, become_water_bucket_cmd);
	//POST_MSG(PROTO_FINISH_WATER_BUCKET, finish_water_bucket_cmd);
	POST_MSG(PROTO_SEND_RAND_ITEM, send_rand_item_cmd);
	POST_MSG(PROTO_RAND_ITEM_SWAP, rand_item_swap_cmd);
	POST_MSG(PROTO_DISCOVER_RAND_ITEM, discover_random_item_cmd);
	POST_MSG(PROTO_LAHM_FOOD_MACHINE, lahm_food_machine_cmd);
	POST_MSG(PROTO_SET_QUES_NAIRE, set_ques_naire_cmd);
	//POST_MSG(PROTO_GIVE_JIJILE_CARD_FREE, give_jijile_card_cmd);

	// For Profession information
	POST_MSG(PROTO_GET_PROFESSIONS, get_profession_cmd);
	POST_MSG(PROTO_GET_SMC_SALARY, get_smc_salary_cmd);
	POST_MSG(PROTO_CHK_SMC_SALARY, chk_smc_salary_cmd);

	// For Send Gift
	POST_MSG(PROTO_SEND_ONE_GIFT, send_one_gift_cmd);
	POST_MSG(PROTO_GET_ADDITINAL_ITM, get_additional_gift_cmd);
	POST_MSG(PROTO_GET_GIFT_TIMES, get_gift_times_cmd);

	// For Pasture
	POST_MSG(PROTO_GET_PASTURE, get_pasture_cmd);
	POST_MSG(PROTO_GET_STOREHOUSE_ITEM, get_store_items_cmd);
	POST_MSG(PROTO_CAPTURE_ANIMAL, pasture_capture_animal_cmd);
	POST_MSG(PROTO_PASTURE_ADD_FEED, pasture_add_feed_cmd);
	POST_MSG(PROTO_HERD_ANIMAL, pasture_herd_animal_cmd);
	POST_MSG(PROTO_PASTURE_GET_FISH, pasture_get_animal_cmd);
	POST_MSG(PROTO_PASTURE_ADD_WATER, pasture_add_water_cmd);
	POST_MSG(PROTO_PASTURE_GET_RECENT_VISITOR, get_recent_pasture_visitors_cmd);
	POST_MSG(PROTO_PASTURE_GET_FEED_ITEM, get_feed_items_cmd);
	POST_MSG(PROTO_SET_PASTURE_BOX_TIEM, set_pasture_items_cmd);
	POST_MSG(PROTO_SET_PASTURE_LOCK, pasture_lock_cmd);
	POST_MSG(PROTO_GET_SHEEP, get_sheep_from_vip_cmd);
	POST_MSG(PROTO_FOLLOW_ANIMAL, follow_animal_cmd);
	POST_MSG(PROTO_ANIMAL_CHICAO, animal_chicao_cmd);
	POST_MSG(PROTO_RELEASE_ANIMAL, pasture_release_animal_cmd);

	POST_MSG(PROTO_GET_500_XIAOME, get_500_xiaome_cmd);
	POST_MSG(PROTO_PAY_MONEY, pay_money_cmd);
	POST_MSG(PROTO_GET_EGG_HATCHS, get_penguin_egg_hatch_times_cmd);
	POST_MSG(PROTO_SET_EGG_HATCHS, set_penguin_egg_hatch_times_cmd);
	POST_MSG(PROTO_BUY_SOME_SUITS, buy_some_cloths_cmd);
	POST_MSG(PROTO_GET_RAND_TIMES, get_rand_times_cmd);
	POST_MSG(PROTO_WANT_ITEM_RAND, rand_item_want_cmd);
	POST_MSG(PROTO_GET_PET_ATTIRE_CNT, get_pet_attires_count_cmd);
	POST_MSG(PROTO_GET_ITEMS_CNT_IN_BAG, get_items_cnt_in_bag_cmd);
	// For sport

	// For game PK
	POST_MSG(PROTO_GET_FRD_GAME_GRADE_LIST, get_frd_game_grade_list_cmd);
	POST_MSG(PROTO_GET_SELF_GAME_GRADE_LIST, get_game_grade_list_cmd);
	POST_MSG(PROTO_SET_GAME_PK_INFO, set_pk_info_cmd);
	POST_MSG(PROTO_GET_HISTORY_PK_LIST, get_pk_history_list_cmd);
	POST_MSG(PROTO_GET_PK_SCORE, get_pk_score_cmd);
	POST_MSG(PROTO_GET_PK_TRADE, get_pk_trade_cmd);
	//Pet items operate
	POST_MSG(PROTO_GET_PET_ITEM_COUNT, get_pet_item_count_cmd);
	POST_MSG(PROTO_GET_PET_ITEMS_CNT, get_pet_items_cnt_cmd);
	POST_MSG(PROTO_BUY_PET_ITEM, buy_pet_item_cmd);
	POST_MSG(PROTO_PET_CLOTHES_USE, use_pet_cloth_cmd);
	POST_MSG(PROTO_PET_HONOR_USE, use_pet_cloth_cmd);
//	POST_MSG(PROTO_PET_HONOR_USE, use_pet_honor_cmd);

	POST_MSG(PROTO_GET_SYSTIME, get_systime_cmd);
	POST_MSG(PROTO_GET_MD5_SESSION, get_md5_session_cmd);
	POST_MSG(PROTO_CHK_VIP_INFO, chk_vip_info_cmd);

	//for mibi server
	POST_MSG(PROTO_IS_SET_PAY_PASSWD, is_set_pay_passwd_cmd);
#ifndef TW_VER //tai wai mei you di yong quan
	POST_MSG(PROTO_GET_MIBI_COUPON_INFO, get_mibi_coupon_info_cmd);
#endif

	//for gold bean server
	POST_MSG(PROTO_GOLD_BEAN_INQUIER_MUL_ITEM_PRICE, gold_bean_inquire_multi_items_price_cmd);
	POST_MSG(PROTO_GOLD_BEAN_INQUIER_ONE_ITEM_PRICE, gold_bean_inquire_one_item_price_cmd);
	POST_MSG(PROTO_GOLD_BEAN_BUY_ITEM_USE_GOLD_BEAN, buy_item_use_gold_bean_cmd);
	POST_MSG(PROTO_GOLD_BEAN_GET_GOLD_BEAN_INFO, get_gold_bean_count_cmd);

	//for mibi buy gold bean server
	POST_MSG(PROTO_BUY_GOLD_BEAN_USE_MIBI, buy_gold_bean_use_mibi_cmd);

	POST_MSG(PROTO_GET_BEAN_MALL_LOGIN_TIMES, get_bean_mall_login_times_cmd);
	POST_MSG(PROTO_GET_VIP_MONTH_TICKET_INFO, get_vip_month_ticket_info_cmd);

	POST_MSG(PROTO_SET_FREE_VIP_INFO, set_free_vip_info_cmd);
	POST_MSG(PROTO_GET_FREE_VIP_INFO, get_free_vip_info_cmd);

	//for magic code server
	POST_MSG(PROTO_QUERY_MAGIC_CODE_GIFT, query_magic_code_gift_cmd);
	POST_MSG(PROTO_CONSUME_MAGIC_CODE, consume_magic_code_cmd);
	POST_MSG(PROTO_GET_MAGIC_ITEMS, magic_passwd_rand_item_cmd);

	// For mole party
	POST_MSG(PROTO_GET_MOLE_PARTY, get_mole_party_cmd);
	POST_MSG(PROTO_SET_MOLE_PARTY, set_mole_party_cmd);
	POST_MSG(PROTO_GET_OWN_MOLE_PARTY, get_own_mole_party_cmd);
	POST_MSG(PROTO_GET_PARTY_COUNT, get_party_count_cmd);

	// For homeland
	POST_MSG(PROTO_FERTILIZE_FLOWER, homeland_fertilize_plant_cmd);
	POST_MSG(PROTO_POLLINATE_FLOWER, homeland_pollinate_cmd);
	POST_MSG(PROTO_GET_HOMELAND_INFO, enter_homeland_cmd);
	POST_MSG(PROTO_SET_HOMELAND_ITEM, set_homeland_items_cmd);
	POST_MSG(PROTO_GET_HOMELAND_BOX_ITEM, get_homeland_box_items_cmd);
	POST_MSG(PROTO_HOMELAND_PLANT_SEED, homeland_plant_cmd);
	POST_MSG(PROTO_HOMELAND_ROOT_OUT_PLANT, homeland_root_out_plant_cmd);
	POST_MSG(PROTO_HOMELAND_WATER_PLANT, homeland_water_plant_cmd);
	POST_MSG(PROTO_HOMELAND_KILL_BUG, homeland_kill_bug_cmd);
	POST_MSG(PROTO_HOMELAND_GET_ONE_PLANT, homeland_get_one_plant_cmd);
	POST_MSG(PROTO_HOMELAND_HARVEST_FRUITS, homeland_harvest_fruits_cmd);
	POST_MSG(PROTO_SELL_FRUITS, sell_fruits_cmd);
	POST_MSG(PROTO_GET_RECENT_JY_VISITOR, get_recent_jy_visitors_cmd);
	POST_MSG(PROTO_SET_HOMELAND_USED_ITEM, set_homeland_used_itms_cmd);
	POST_MSG(PROTO_CHK_IF_USER_EXIST, chk_if_user_exist_cmd);
	POST_MSG(PROTO_THIEF_FRUIT, homeland_thief_fruit_cmd);

	POST_MSG(PROTO_GET_FRUIT_FREE, get_fruit_free_cmd);
	POST_MSG(PROTO_GET_ITEM_FROM_BAOHE, get_item_from_baohe_cmd);
	POST_MSG(PROTO_GET_BAOHE_OP_LIST, get_baohe_op_list_cmd);

	// For spring festival
//	POST_MSG(PROTO_SET_SPRING_MSG, set_spring_msg_cmd);
	POST_MSG(PROTO_GET_SPRING_MSG, get_spring_msg_cmd);
	POST_MSG(PROTO_SEND_FU, send_fu_cmd);
	POST_MSG(PROTO_LIST_FU_SENT, list_fu_sent_cmd);
	POST_MSG(PROTO_GET_FU_INFO, get_fu_info_cmd);
	POST_MSG(PROTO_PET_CHANGE_FORM, lahm_form_change_cmd);

	// For Temporary Use Only
	POST_MSG(PROTO_PERSONAL_INFO, submit_personal_info_cmd);
	POST_MSG(PROTO_IF_INFO_SUBMITTED, chk_if_info_submitted_cmd);
	POST_MSG(PROTO_SUBMIT_MAGIC_CODE, submit_magic_code_cmd);
	POST_MSG(PROTO_BUY_REAL_ITEM, buy_real_item_cmd);

	// For Scene Game
	POST_MSG(PROTO_SCENEGAME_INFO, scenegame_info_cmd);
	POST_MSG(PROTO_SEESAW_INFO, scenegame_info_cmd);
	POST_MSG(PROTO_MIMIC_SHOW_INFO, scenegame_info_cmd);

	//For cutting picture
	POST_MSG(PROTO_PIC_SRV_IP_PORT, get_pic_srv_ip_port_cmd);
	//For tu ya
	POST_MSG(PROTO_TUYA_SRV_IP_PORT, get_tuya_srv_ip_port_cmd);

	//For card books
	POST_MSG(PROTO_CARD_DO_INIT, card_do_init);
	POST_MSG(PROTO_CARD_GET_INFO, card_get_info);
	POST_MSG(PROTO_CARD_ADD_BASIC, add_basic_card);
	POST_MSG(PROTO_CARD_CHECK_AVAIL_NUM, add_basic_card_avail);

	//For mole bank
	POST_MSG(PROTO_GET_DEPOSIT_INFO, get_deposit_info_cmd);
	POST_MSG(PROTO_DEPOSIT_BEAN, deposit_bean_cmd);
	POST_MSG(PROTO_DRAW_BEAN, draw_bean_cmd);

	//For ask and answer question
	POST_MSG(PROTO_ANSWER_QUESTION, answer_question_cmd);

#ifndef TW_VER
	POST_MSG(PROTO_GET_TV_QUESTION, get_tv_question_cmd);
	POST_MSG(PROTO_ANSWER_TV_QUESTION, answer_tv_question_cmd);
#endif
	//For mole class
	POST_MSG(PROTO_CREATE_CLASS, create_class_cmd);
	POST_MSG(PROTO_MODIFY_CLASS, modify_class_info_cmd);
	POST_MSG(PROTO_GET_CLASS_DETAIL, get_class_info_cmd);
	POST_MSG(PROTO_JOIN_CLASS, join_class_cmd);
	POST_MSG(PROTO_QUIT_CLASS, quit_class_cmd);
	POST_MSG(PROTO_REQ_MONITOR_VERIFY, request_join_class_cmd);
	POST_MSG(PROTO_MONITOR_VERIFY, reply_request_join_cmd);
	POST_MSG(PROTO_DEL_CLASS_MEMBER, delete_class_member_cmd);
	POST_MSG(PROTO_DEL_CLASS, delete_class_cmd);
	POST_MSG(PROTO_GET_CLASS_LOGO_INFO, get_class_logo_info_cmd);
	POST_MSG(PROTO_SET_FIRST_CLASS, set_first_class_cmd);
	POST_MSG(PROTO_GET_FIRST_CLASS, get_first_class_cmd);
	POST_MSG(PROTO_GET_CLASS_ITM_UNUSED, get_class_unused_itm_cmd);
	POST_MSG(PROTO_SET_CLASS_ITEM, set_itm_class_cmd);
	POST_MSG(PROTO_GET_CLASS_LIST, get_usr_class_list_cmd);
	POST_MSG(PROTO_LOCK_CLASS, lock_class_cmd);
	POST_MSG(PROTO_UNLOCK_CLASS, unlock_class_cmd);
	POST_MSG(PROTO_GET_CLASS_SIMPLE, get_class_simple_info_cmd);
	POST_MSG(PROTO_GET_CLASS_MEMBER_LIST, get_member_list_cmd);
	POST_MSG(PROTO_GET_TOP_CLASS, get_top_class_list_cmd);
	POST_MSG(PROTO_GET_CLASS_HONOR, get_class_honor_cmd);
	POST_MSG(PROTO_GET_CLASS_QUES_REWARD, get_class_ques_reward_cmd);
	POST_MSG(PROTO_GET_CLASS_QUES_MEMBER_REWARD, get_class_ques_member_reward_cmd);
	//POST_MSG(PROTO_REQ_TMP_SUPERLAMN, request_tmp_superlamn_cmd);
	//POST_MSG(PROTO_REQ_TMP_SUPERLAMN_STAT, req_tmp_superlamn_stat_cmd);

	POST_MSG(PROTO_GET_LOTTERY, get_lottery_cmd);

	//For mole happy game
	POST_MSG(PROTO_SET_HAPPY_CARD, set_happy_card_cmd);
	POST_MSG(PROTO_GET_HAPPY_CARD, get_happy_card_cmd);
	POST_MSG(PROTO_TRADE_HAPPY_CARD, trade_happy_card_cmd);
	POST_MSG(PROTO_GET_TRADE_HAPPY_CARD_INFO, get_trade_happy_card_info_cmd);
	POST_MSG(PROTO_GET_HAPPY_CARD_CLOTH, get_happy_card_cloth_cmd);
	//POST_MSG(PROTO_GET_FIRE_CUP_CARD, get_fire_cup_card_cmd);
	POST_MSG(PROTO_GET_MAGIC_CARD_REWARD, get_magic_card_reward_cmd);

	POST_MSG(PROTO_BECOME_BIGGER, become_bigger_cmd);
	POST_MSG(PROTO_BECOME_VICTORY, become_victory_cmd);
	POST_MSG(PROTO_BECOME_FAIL, become_fail_cmd);

	//lahm diary
	POST_MSG(PROTO_EDIT_LAHM_DIARY, edit_diary_cmd);
	POST_MSG(PROTO_GET_DIARY_TITLES, get_diarys_title_cmd);
	POST_MSG(PROTO_DELETE_LAHM_DIARY, delete_diary_cmd);
	POST_MSG(PROTO_SET_DIARY_LOCK_STATE, set_diary_lock_state_cmd);
	POST_MSG(PROTO_SEND_DIARY_FLOWER, send_diary_flower_cmd);
	POST_MSG(PROTO_GET_DIARY_CONTENT, get_diary_content_cmd);
	POST_MSG(PROTO_SET_BIG_LOCK, set_big_lock_cmd);
	POST_MSG(PROTO_GET_BIG_LOCK, get_big_lock_cmd);
	POST_MSG(PROTO_GET_DIARY_NUM, get_diary_num_cmd);

	//work system
	POST_MSG(PROTO_SET_WORK_NUM, set_work_num_cmd);
	POST_MSG(PROTO_GET_WORK_NUM, get_work_num_cmd);
	POST_MSG(PROTO_GET_WORK_SALARY, get_work_salary_cmd);
	POST_MSG(PROTO_SET_WORKER, set_worker_cmd);

	//For mole doctor
	POST_MSG(PROTO_GET_SICKBED_INFO, get_sick_house_info_cmd);
	POST_MSG(PROTO_DOCTOR_SIT, doctor_sit_cmd);
	POST_MSG(PROTO_DOCTOR_QUIT, doctor_quit_cmd);
	POST_MSG(PROTO_PATIENT_SIT, patient_sit_cmd);
	POST_MSG(PROTO_PATIENT_QUIT, patient_quit_cmd);
	POST_MSG(PROTO_DOCTOR_CHECK, doctor_check_cmd);
	POST_MSG(PROTO_DOCTOR_CURE, doctor_cure_cmd);
	POST_MSG(PROTO_DOCTOR_DUTY, doctor_duty_cmd);
	POST_MSG(PROTO_MEDICINE_CURE_LAHM, medicine_cure_lahm_cmd);
	POST_MSG(PROTO_SET_CLOSET, set_closet_cmd);
	POST_MSG(PROTO_UNSET_CLOSET, unset_closet_cmd);
	POST_MSG(PROTO_GET_CLOSET, get_closet_cmd);

	//car
	POST_MSG(PROTO_DRIVE_OUT, drive_out_cmd);
	POST_MSG(PROTO_DRIVE_BACK, drive_back_cmd);
	POST_MSG(PROTO_LIST_CAR, list_cars_cmd);
	POST_MSG(PROTO_SET_SHOW_CAR, set_show_car_cmd);
	POST_MSG(PROTO_GET_SHOW_CAR, get_show_car_cmd);
	POST_MSG(PROTO_CAR_REFUEL, refuel_cmd);
	POST_MSG(PROTO_BUY_CAR, buy_car_cmd);
	POST_MSG(PROTO_CHECK_CAR, check_car_cmd);
	POST_MSG(PROTO_RENT_ZHAIJISONG, rent_zhaijisong_cmd);
	POST_MSG(PROTO_ON_OFF_NANGUA, on_off_nangua_cmd);
	POST_MSG(PROTO_NOTIFY_ONLINE_TIME, get_online_time_cmd);

	//tmp: class question
	POST_MSG(PROTO_GET_CLASS_QUESTION_STAT, get_class_ques_stat_cmd);
	POST_MSG(PROTO_GET_CLASS_QUESTION, get_class_ques_cmd);
	POST_MSG(PROTO_ANSWER_CLASS_QUESTION, answer_class_ques_cmd);
	POST_MSG(PROTO_GET_CLASS_QUESTION_INFO, get_class_ques_info_cmd);

	POST_MSG(PROTO_GET_FRIEND_HOME_HOT, get_friend_home_hot_cmd);
	POST_MSG(PROTO_GGJ_EXCHANGE_ITM, ggj_exchg_itm_cmd);

	POST_MSG(PROTO_GET_MATCHED_STRING, get_matched_string_cmd);
	POST_MSG(PROTO_GET_CHAR_CLOTH, get_char_cloth_cmd);
	//POST_MSG(PROTO_CATCH_PLAYER_BY_WORD, catch_player_by_word_cmd);
	POST_MSG(PROTO_EXCHG_CHAR_CLOTH, exchg_char_cloth_cmd);
	POST_MSG(PROTO_GET_CLASS_SCORE, get_class_score_cmd);
	//POST_MSG(PROTO_GET_TANGGUO_SCORE_TOTAL, get_tangguo_score_total_cmd);
	POST_MSG(PROTO_GET_cut_sheep_cheer, scenegame_info_cmd);
	POST_MSG(PROTO_GET_fisher_cheer, scenegame_info_cmd);
	POST_MSG(PROTO_GET_chris_tree_stat, scenegame_info_cmd);
	POST_MSG(PROTO_GET_barber_cheer, scenegame_info_cmd);
	//POST_MSG(PROTO_GET_cow_milk_pos, scenegame_info_cmd);
	POST_MSG(PROTO_ENGLISH_LAHM_GET_CLOTH, english_lahm_get_cloth_and_honor_cmd);

	POST_MSG(PROTO_GET_VIP_LEVEL_LAHM_GIFTS, vip_level_lahm_gifts_cmd);
	POST_MSG(PROTO_GET_VIP_LEVEL_GIFTS, vip_level_mole_gifts_cmd);
	POST_MSG(PROTO_GET_JIAZHAO,  get_jiazhao_cmd);
	POST_MSG(PROTO_EXCHG_XUANFENG_JIJILE,  exchg_xuanfeng_jijile_cmd);
	POST_MSG(PROTO_GET_XUANFENG_CAR,  get_xuanfeng_car_cmd);
	POST_MSG(PROTO_GET_USER_NUM_IN_SHIP,  get_user_num_in_ship_cmd);
	POST_MSG(PROTO_GET_NEXT_FLIGHT_TIME,  get_next_flight_time_cmd);
	POST_MSG(PROTO_CANDY_EXCH, get_candy_cmd);
	POST_MSG(PROTO_CANDY_FROM_RECY, get_candy_from_recy_cmd);
	POST_MSG(PROTO_CANDY_MAKE, make_candy_cmd);
	POST_MSG(PROTO_CANDY_POSITION_EX, get_position_info_cmd);
	POST_MSG(PROTO_CANDY_HAVE_ONE, get_candy_status_cmd);

	POST_MSG(PROTO_PASTURE_GET_NETS, pasture_get_nets_cmd);
	POST_MSG(PROTO_PASTURE_NETS_STATUS, pasture_get_nets_status_cmd);
	POST_MSG(PROTO_PASTURE_CATCH_FISH, pasture_catch_fish_cmd);
	POST_MSG(PROTO_CHANGE_SELF_NANGUA, change_self_nangua_cmd);
	POST_MSG(PROTO_CANDY_PACKET_SEND, send_candy_packet_cmd);
	POST_MSG(PROTO_SUB_CANDY_COUNT, sub_candy_count_cmd);
	POST_MSG(PROTO_EXCHANGE_PROP, exchange_prop_cmd);
	// hallowmas
	POST_MSG(PROTO_GET_BLACK_CAT_STATUS, get_black_car_status_cmd);
	POST_MSG(PROTO_GET_CANDY_FROM_OTHER, get_candy_from_other_home_cmd);
	POST_MSG(PROTO_GET_CANDY_COUNT, get_candy_count_cmd);
	POST_MSG(PROTO_VIP_PATCH_WORK, vip_work_patch_cmd);
	POST_MSG(PROTO_GET_BIBO_INFO, get_bibo_date_info_cmd);
	POST_MSG(PROTO_PASTURE_GET_LEVEL, pasture_get_level_ex_cmd);
	POST_MSG(PROTO_GET_NPC_LOVELY, get_npc_lovely_cmd);
	POST_MSG(PROTO_GIFT_DRAW, gift_draw_cmd);
	POST_MSG(PROTO_UPDATE_NPC_LOVELY, update_npc_lovely_cmd);
	//1915 miss
	POST_MSG(PROTO_GET_ITEMS_COUNT, get_items_count_cmd);
	POST_MSG(PROTO_GET_ITEMS_COUNT_NEW, get_items_count_new_cmd);
	//POST_MSG(PROTO_SEND_ONE_OF_TWO, send_gift_oneoftwo_cmd);

	POST_MSG(PROTO_GET_VIP_INVATATION, get_vip_invitation_cmd);
	POST_MSG(PROTO_SET_EGG_POS, set_egg_pos_cmd);
	POST_MSG(PROTO_PROCESS_FUDAN, process_fudan_cmd);
	POST_MSG(PROTO_GET_FUDAN_TIMES, get_fudan_times_cmd);

	POST_MSG(PROTO_CUT_YANGMAO, cut_yang_mao_cmd);
	POST_MSG(PROTO_GET_FIRE_EGG, get_egg_only_one_cmd);
	POST_MSG(PROTO_RABIT_OCCUPY_POS, occupy_rabit_pos_cmd);
	POST_MSG(PROTO_BROADCAST_RABIT_POS, get_occupy_rabit_pos_cmd);
	POST_MSG(PROTO_BUY_LIMITED_ITEM, buy_limited_item_cmd);
	POST_MSG(PROTO_GET_LIMIT_ITEM_NUM, get_limited_item_num_cmd);
	POST_MSG(PROTO_GET_LITTLE_RABIT_INFO, get_little_rabit_cmd);
	//POST_MSG(PROTO_SEND_GIFT_TWO, send_gift_twooffour_cmd);

	// npc tasks
	POST_MSG(PROTO_TAKE_NPC_TASK, take_npc_task_cmd);
	POST_MSG(PROTO_FIN_NPC_TASK, fin_npc_task_cmd);
	POST_MSG(PROTO_SET_NT_CLIENT_DATA, set_nt_client_data_cmd);
	POST_MSG(PROTO_GET_NT_CLIENT_DATA, get_nt_client_data_cmd);
	POST_MSG(PROTO_GET_NPC_TASK_STATUS, get_npc_task_status_cmd);
	POST_MSG(PROTO_GET_NPC_TASKS, get_npc_tasks_cmd);
	POST_MSG(PROTO_CANCEL_NPC_TASKS, cancel_npc_task_cmd);
	POST_MSG(PROTO_FETCH_LITTLE_TU, fetch_little_rabit_cmd);
	POST_MSG(PROTO_OCCUPY_CUT_POS, cut_occupy_pos_cmd);
	POST_MSG(PROTO_GET_CUT_POS, get_cut_position_cmd);
	POST_MSG(PROTO_BREAK_EGG, break_fire_egg_cmd);
	POST_MSG(PROTO_BEAUTY_COMPETE, beauty_dress_compete_cmd);
	POST_MSG(PROTO_SET_PHOTO_DRESS, set_photo_dress_cmd);
	POST_MSG(PROTO_GET_PHOTO_DRESS, get_photo_dress_cmd);
	POST_MSG(PROTO_GET_SHOW_STATUS, get_beauty_show_stat_cmd);
	POST_MSG(PROTO_GET_ITEM_PRICE, get_items_price_cmd);
	POST_MSG(PROTO_SET_MODEL_WEAR, set_model_wear_cmd);
	POST_MSG(PROTO_GET_MODEL_WEAR, get_model_wear_cmd);
	POST_MSG(PROTO_SET_MOLE_MODEL_WEAR, set_model_wear_to_mole_cmd);
	POST_MSG(PROTO_SET_MODEL_MOLE_WEAR, set_mole_wear_to_model_cmd);
	POST_MSG(PROTO_SET_CAKE, set_cake_cmd);
	POST_MSG(PROTO_GET_CAKE_INFO, get_cake_info_cmd);
	POST_MSG(PROTO_GET_RABIT_MASTER_CLOTH, get_rabit_master_cloth_cmd);
	POST_MSG(PROTO_GET_SHOW_SCORE, get_beauty_show_score_cmd);
	POST_MSG(PROTO_SET_ECHO_NUM_TYPE, echo_set_num_type_cmd);
	//POST_MSG(PROTO_BROADCAST_NUM, broadcast_value_cmd);
	POST_MSG(PROTO_GET_ECHO_NUM_TYPE, echo_get_num_type_cmd);
	POST_MSG(PROTO_GET_TUJIAN, get_all_tujian_cmd);
	POST_MSG(PROTO_WEIGHTING_FISH, weighing_fish_cmd);
	POST_MSG(PROTO_FISHING, finshing_cmd);
	POST_MSG(PROTO_GET_TOTAL_FISH_WEIGHT, get_total_fish_weight_cmd);
	POST_MSG(PROTO_GET_RING_FROM_OTHER, get_ring_from_other_cmd);
	POST_MSG(PROTO_GET_10XIAOMEE_50TIMES, get_10xiaomee_50times_cmd);
	POST_MSG(PROTO_OCCUPY_BOOTH, occupy_booth_cmd);
	POST_MSG(PROTO_GET_ALL_SELLER_INFO, get_boothes_cmd);
	POST_MSG(PROTO_LEAVE_BOOTH, leave_booth_cmd);
	POST_MSG(PROTO_BUY_FURNI_FROM_BOOTH, buy_furni_from_booth_cmd);
	POST_MSG(PROTO_SET_CHRISTMAS_WISH, set_christmas_wish_cmd);
	POST_MSG(PROTO_GET_CHRISTMAS_WISH, get_christmas_wish_cmd);

	POST_MSG(PROTO_GET_CHRIS_GIFTS_NUM, get_christmas_gift_num_cmd);
	POST_MSG(PROTO_GET_CHRISTMAS_GIFTS, get_five_christmas_gifts_cmd);
	POST_MSG(PROTO_SEND_CHRIS_GIFTS_BILU, send_chris_gift_cmd);
	POST_MSG(PROTO_GET_CHRIS_GIFT_FROM_BILU, get_chris_gift_from_bilu_cmd);
	//POST_MSG(PROTO_GET_XIAO_XIONG_CAR, get_xiao_xiong_car_cmd);
	POST_MSG(PROTO_COLLECT_RAIN_GIFT, collect_gift_cmd);
	POST_MSG(PROTO_SEND_CHRIS_GIFTS_TO_NPC, send_chris_gift_to_npc_cmd);
	POST_MSG(PROTO_ADD_NENG_LIANG_XING_CNT, add_nengliangxing_cmd);
	POST_MSG(PROTO_GET_NENG_LIANG_XING_CNT, get_nengliang_cnt_cmd);
	//POST_MSG(PROTO_GET_DAJIAOYIN_REWARD, get_dajiaoyin_reward_cmd);
	POST_MSG(PROTO_SET_PHOTO_DAHEZHAO, set_photo_dahezhao_cmd);
	POST_MSG(PROTO_GET_PHOTO_DAHEZHAO, get_photo_dahezhao_cmd);
	//POST_MSG(PROTO_EXCHG_MIBI_COUPON, exchg_mibi_coupon_cmd);
	POST_MSG(PROTO_GET_ALL_PET_INFO, get_all_pet_info_cmd);
	//POST_MSG(PROTO_FETCH_JINIAN_JIAOYIN, fetch_jinian_jiaoyin_cmd);
	POST_MSG(PROTO_GET_PET_TASK_ATTIRE_INFO,get_pet_task_attire_info_cmd);
	POST_MSG(PROTO_GET_CARRY_ROCK_NUM, mole_get_carry_rock_num_cmd);
	//POST_MSG(PROTO_SET_CARRY_ROCK_NUM, mole_set_carry_rock_num_cmd);
	POST_MSG(PROTO_SUPER_LAMU_GET_ZHANPAO, super_lamu_get_zhanpao_cmd);
	POST_MSG(PROTO_ADD_LAMU_GROWTH, add_lamu_growth_cmd);
	POST_MSG(PROTO_SET_PET_SKILL_EX, set_pet_skill_ex_cmd);
	POST_MSG(PROTO_GET_LAHM_TASK_STATE, get_lahm_task_state_cmd);
	POST_MSG(PROTO_SET_LAHM_TASK_STATE, set_lahm_task_state_cmd);
	POST_MSG(PROTO_GET_LAHM_TASK_DATA, get_lahm_task_data_cmd);
	POST_MSG(PROTO_SET_LAHM_TASK_DATA, set_lahm_task_data_cmd);
	POST_MSG(PROTO_BUILD_LAHM_TEMPLE, build_temple_cmd);
	POST_MSG(PROTO_GET_BUILD_TEMPLE, get_build_temple_cmd);
	POST_MSG(PROTO_GET_PET_TASK_LIST_EX, get_pet_list_by_task_cmd);
	POST_MSG(PROTO_LAMU_ACTION, lamu_do_action_cmd);
	POST_MSG(PROTO_REMOVE_PET_ACTION, remove_lamu_action_cmd);
	POST_MSG(PROTO_EXCHANGE_RAND_ITEM, exchange_item_to_rand_item_cmd);
	POST_MSG(PROTO_GET_ITEM_BY_USE_SKILL, get_item_by_skill_cmd);
	POST_MSG(PROTO_GET_USER_DATA,get_user_data_cmd);
	POST_MSG(PROTO_SET_USER_DATA,set_user_data_cmd);
	POST_MSG(PROTO_EXCHANGE_GOLD_SILVER, exchange_gold_and_silver_cmd);
	POST_MSG(PROTO_SET_PET_SKILL_TYPE, set_pet_skill_type_cmd);
	POST_MSG(PROTO_SET_PET_HOT_SKILL, set_pet_hot_skill_cmd);
	POST_MSG(PROTO_EXCHANGE_ALL_TO_ANOTHER, exchange_thing_to_another);
	//POST_MSG(PROTO_GET_SKILL_ITME_LIBAO, lamu_get_skill_item_libao);
	POST_MSG(PROTO_QUIT_MACHINE_DOG, quit_machine_dog_cmd);
	POST_MSG(PROTO_FEED_MACHINE_DOG, feed_machine_dog_cmd);
	POST_MSG(PROTO_GET_DOG_DO_THING_INFO, get_machine_dog_do_thing_info_cmd);
//	POST_MSG(PROTO_PET_HELP_CIWEI,	lamu_help_ciwei_cmd);
	POST_MSG(PROTO_PET_USE_SKILL_DO_WORK, lahm_use_skill_do_work_cmd);
	POST_MSG(PROTO_GET_ARCHITECT_EXP, get_architect_exp_cmd);
	POST_MSG(PROTO_GET_PIPI_OR_DOUYA, get_pipi_or_douya_cmd);
	POST_MSG(PROTO_GET_MAP_BUILDING_INFO, list_map_building_info_cmd);
	POST_MSG(PROTO_GET_MILK_FROM_COW, get_milk_from_cow_cmd);
	POST_MSG(PROTO_SET_ONLY_ONE_FLAG, set_only_one_flag_cmd);
	POST_MSG(PROTO_ADD_SCORE_FOR_WO_WEI_HUI, add_score_for_wo_wei_hui_cmd);
	//POST_MSG(PROTO_GIVE_YUMI_GET_YINGTAO, feed_rabit_get_seed_cmd);
	POST_MSG(PROTO_FOR_WU_WEI_HUI_SCORE, get_score_for_wo_wei_hui_cmd);
	POST_MSG(PROTO_GET_SUPPORT_NPC, get_support_npc_wo_wei_hui_cmd);
	POST_MSG(PROTO_GET_LOGIN_SESSION, get_login_session_cmd);
	POST_MSG(PROTO_TANTALIZE_CAT, tantalize_cat_cmd);
	POST_MSG(PROTO_SUPER_LAMU_PARTY_GET,super_lamu_party_get_cmd);
	POST_MSG(PROTO_CREATE_BULDING,create_building_cmd);
	POST_MSG(PROTO_GET_BUILDING_CERTIFY, get_certificates_for_building_cmd);
	POST_MSG(PROTO_CHANGE_BUILDING_NAME, set_building_name_cmd);
	POST_MSG(PROTO_CHANGE_BUILDING_STYLE, set_building_style_cmd);
	POST_MSG(PROTO_GET_MAP_LAST_GRID, get_map_last_grid_cmd);
	POST_MSG(PROTO_SET_BUILDING_INNER_STYLE,set_building_inner_style_cmd);
	POST_MSG(PROTO_GET_SHOP_INFO, get_shop_info_cmd);
	POST_MSG(PROTO_GET_SHOP_BOARD_INFO,get_shop_board_info_cmd);
	POST_MSG(PROTO_GET_SHOP_ITEM_INFO, get_shop_item_list_cmd);
//	POST_MSG(PROTO_GET_SHOP_ITEM_LOCK, get_shop_item_lock_cmd);
	POST_MSG(PROTO_ADD_EMPLOYEE, add_employee_cmd);
	POST_MSG(PROTO_DEL_EMPLOYEE, del_employee_cmd);
	POST_MSG(PROTO_COOKING_DISH, cooking_dish_cmd);
	POST_MSG(PROTO_EAT_DISH, eat_dish_cmd);
	POST_MSG(PROTO_CLEAN_DISH, clean_dish_cmd);
	POST_MSG(PROTO_CHANGE_DISH_STATE, change_dish_state_cmd);
	POST_MSG(PROTO_PUT_DISH_IN_BOX,put_dish_in_box_cmd);
	POST_MSG(PROTO_LIST_EMPLOYEE,get_employee_list_cmd);
	POST_MSG(PROTO_SUB_EVALUATE, sub_shop_evaluate_cmd);
	POST_MSG(PROTO_GET_SHOP_COUNT,get_shop_count_cmd);
	POST_MSG(PROTO_GET_PET_EMPLOYED_LIST,get_pet_employed_list_cmd);
	POST_MSG(PROTO_GET_SHOP_HONOR_LIST, get_shop_honor_list_cmd);
	POST_MSG(PROTO_PROFESSION_DO_WORK, do_profession_work_cmd);
	//POST_MSG(PROTO_GET_REPUTATION, get_reputation_cmd);
	POST_MSG(PROTO_GET_pos_info, scenegame_info_cmd);
	POST_MSG(PROTO_GET_SHOP_EVENT,get_restuarant_event_cmd);
	POST_MSG(PROTO_SET_SHOP_EVENT,set_restuarant_event_cmd);
	POST_MSG(PROTO_GET_CLIENT_OBJ,get_client_object_cmd);
	POST_MSG(PROTO_SET_CLIENT_OBJ,set_client_object_cmd);
//	POST_MSG(PROTO_ADD_TEAM_SPORT_SCORE,ding_pigu_add_score_cmd);
	POST_MSG(PROTO_GET_USER_IN_MAP,get_map_user_cnt_cmd);
	POST_MSG(PROTO_GET_FRIEND_SHOP_INFO,cs_get_shops_info_cmd);
	POST_MSG(PROTO_GET_DISH_COUNT,get_dish_count_cmd);
	POST_MSG(PROTO_GIVE_NPC_DISHS,give_npc_shop_dish_cmd);
	POST_MSG(PROTO_GET_DISH_LEVEL_COUNT,get_dish_count_and_level_cmd);
	POST_MSG(PROTO_GET_DISH_LEVEL_COUNT_ALL,get_dish_count_and_level_all_cmd);
	POST_MSG(PROTO_GET_MONEY_BY_GUA_GAU_CARD, get_money_by_gua_gua_card_cmd);
	POST_MSG(PROTO_MAGIC_PASSWD_LOOKUP, magic_passwd_item_lookup_cmd);
	POST_MSG(PROTO_GET_TYPE_DATA,get_type_data_cmd);
	POST_MSG(PROTO_SET_TYPE_DATA,set_type_data_cmd);
	POST_MSG(PROTO_GET_EAT_DISH_RAND,get_magic_man_eat_dish_cmd);
	POST_MSG(PROTO_SET_MAGIC_MAN_EAT_DISH,set_magic_man_eat_dish_cmd);
	POST_MSG(PROTO_GET_SESSION,get_session_cmd);
	POST_MSG(PROTO_GET_NEW_CARD_EXP,get_new_card_exp_cmd);
	POST_MSG(PROTO_FIX_CARD_GAME_BUG,add_red_clothe_accord_exp_lance_cmd);
	POST_MSG(PROTO_GET_SIG_CARD_FROM_NPC,add_new_card_by_npc_talk_cmd);
	POST_MSG(PROTO_CHECK_HAVE_BASIC_CARD,check_if_have_basic_card_cmd);
	POST_MSG(PROTO_GET_BUSINESSMAN_GOODS,get_businessman_goods_cmd);
	POST_MSG(PROTO_BUY_BUSINESSMAN_GOODS,buy_businessman_goods_cmd);
	POST_MSG(PROTO_GET_DRAGON_EGG_BY_TASK,get_dragon_egg_cmd);
	POST_MSG(PROTO_GET_DRAGON_BAG,get_dragon_bag_cmd);
	POST_MSG(PROTO_CALL_DRAGON,call_dragon_cmd);
	POST_MSG(PROTO_RELEASE_DRAGON,release_dragon_cmd);
	POST_MSG(PROTO_SET_DRAGON_NAME,set_dragon_name_cmd);
	POST_MSG(PROTO_FEED_DRAGON,feed_dragon_cmd);
	POST_MSG(PROTO_EXCHANGE_ONE_TO_ANOTHER,exchange_one_thing_to_another_cmd);
	POST_MSG(PROTO_BROOD_DRAGON_EGG,brood_dragon_egg_cmd);
	POST_MSG(PROTO_TAKE_LITTLE_DRAGON,get_baby_dragon_cmd);
	POST_MSG(PROTO_QUICK_BROOD_DRAGON_EGG, accelerate_dragon_brood_cmd);
	POST_MSG(PROTO_GET_BROOD_EGG_TIME, get_brood_dragon_egg_time_cmd);
	POST_MSG(PROTO_GET_CALLED_DRAGON_INFO, get_call_dragon_info_cmd);
	POST_MSG(PROTO_GET_NEW_CARD_FLAG, get_new_card_exp_flag_cmd);

	POST_MSG(PROTO_SIGN_FIRE_CUP, sign_fire_cup_cmd);
	POST_MSG(PROTO_SIGN_GET_TEAM_ID, get_team_id_cmd);
	POST_MSG(PROTO_ADD_STH_RAND, get_sth_accord_rand_cmd);
	POST_MSG(PROTO_EXCHANGE_STH, swap_cmd);
	POST_MSG(PROTO_GET_FIRE_CUP_TEAM_MEDAL,get_team_medal_num_cmd);
	POST_MSG(PROTO_GET_FIRE_CUP_SPRITE_MEDAL_NUM,get_sprite_medal_num_cmd);
	POST_MSG(PROTO_FIRE_CUP_ADD_GAME_BONUS,add_game_bonus_cmd);
	POST_MSG(PROTO_ADD_ITEM_IN_FRIENDSHIP_BOX,add_item_in_friendship_box_cmd);
	POST_MSG(PROTO_GET_ITEM_FROM_FRIENDSHIP_BOX,get_item_from_friendship_box_cmd);
	POST_MSG(PROTO_QUERY_ITEMS_FROM_FRIENDSHIP_BOX,query_items_from_friendship_box_cmd);
	POST_MSG(PROTO_GET_ITEM_HISTORY_FROM_FRIENDSHIP_BOX,get_item_history_from_friendship_box_cmd);
	POST_MSG(PROTO_RACE_GAIN_RACE_BONUS,gain_race_bonus_cmd);
	POST_MSG(PROTO_EXPAND_DRAGON_BAG,expand_dragon_bag_cmd);
	POST_MSG(PROTO_GET_OPEN_SCHOOL_GIFT,get_open_school_gift_cmd);
	POST_MSG(PROTO_GET_LAHM_COUNT_ACCORD_STATE,get_lahm_count_accord_state_cmd);
	//POST_MSG(PROTO_DEL_DEAD_LAHM,del_dead_lahm_cmd);
	POST_MSG(PROTO_BUY_ITEM_BY_BEANS,buy_item_by_beans_cmd);
	POST_MSG(PROTO_DEL_PET_DEAD,del_pet_cmd);
	POST_MSG(PROTO_ADD_TEMP_ITEM,add_temp_item_cmd);
	POST_MSG(PROTO_LOOK_STH_CNT,look_sth_cnt_cmd);
	POST_MSG(PROTO_GET_USE_PROFESSION_EXP_LEVEL,get_profession_exp_level_cmd);
	POST_MSG(PROTO_GUESS_ITEM_PRICE_CHARITY_DRIVE,guess_item_price_charity_drive_cmd);
	POST_MSG(PROTO_QUERY_GUESS_ITEM_CHARITY_DRIVE,query_guess_item_charity_drive_cmd);
	POST_MSG(PROTO_DONATE_ITEM_CHARITY_DRIVE,donate_item_charity_drive_cmd);
	POST_MSG(PROTO_QUERY_CHARITY_DRIVE_TYPE_RANK,query_charity_drive_type_rank_cmd);
	POST_MSG(PROTO_QUERY_CHARITY_DRIVE_INFO,query_charity_drive_info_cmd);
	POST_MSG(PROTO_GET_JY_ANIMAL_ITEMS,get_jy_anmimal_items_cmd);

	//lahm classroom
	POST_MSG(PROTO_BUILD_LAHM_CLASSROOM,create_classroom_cmd);
	POST_MSG(PROTO_GET_LAHM_CLASSROOM_LAST_GRID,get_classroom_last_grid_cmd);
	POST_MSG(PROTO_GET_LAHM_CLASSROOM_GRID_INFO,get_grid_classroom_info_cmd);
	POST_MSG(PROTO_SET_LAHM_CLASSROOM_NAME,set_classroom_name_cmd);
	POST_MSG(PROTO_SET_LAHM_CLASSROOM_INNER_STYLE,set_classroom_inner_style_cmd);
	POST_MSG(PROTO_GET_USER_LAHM_CLASSROOM_INFO,get_classroom_info_cmd);
	POST_MSG(PROTO_QUERY_CLASSROOM_All_STUDENTS_INFO,query_all_students_info_cmd);
	POST_MSG(PROTO_ADD_CLASSROOM_STUDENTS_INFO,add_lahm_students_info_cmd);
	POST_MSG(PROTO_DEL_CLASSROOM_STUDENTS_INFO,del_lahm_students_info_cmd);
	POST_MSG(PROTO_GET_CLASSROOM_STUDENTS_INFO,get_lahm_students_info_cmd);
	POST_MSG(PROTO_GET_CLASSROOM_TEACHER_INFO,get_lahm_teacher_info_cmd);
	POST_MSG(PROTO_GET_CLASSROOM_TEACH_PLAN_INFO,get_teach_plan_info_cmd);
	POST_MSG(PROTO_SET_CLASSROOM_CLASS_BEGIN,set_classroom_class_begin_cmd);
	POST_MSG(PROTO_SET_CLASSROOM_CLASS_MODE,set_classroom_class_mode_cmd);
	POST_MSG(PROTO_SET_CLASSROOM_CLASS_END,set_classroom_class_end_cmd);
	POST_MSG(PROTO_GET_CLASSROOM_COURSE_LEVEL,get_classroom_course_level_cmd);
	POST_MSG(PROTO_LAHM_CLASSROOM_LAHM_EXAM,lahm_classroom_exam_cmd);
	POST_MSG(PROTO_GET_CLASSROOM_TEACHER_EXAM_INFO,get_lahm_teacher_exam_info_cmd);
	POST_MSG(PROTO_GET_USER_CLASSROOM_COUNT,get_lahm_classroom_count_cmd);
	POST_MSG(PROTO_GET_USER_CLASSROOM_RAND_USERID,get_classroom_rand_userid_cmd);
	POST_MSG(PROTO_GET_USER_CLASSROOM_CLASS_FLAG,get_classroom_class_flag_cmd);
	POST_MSG(PROTO_LAHM_CLASSROOM_CLASS_GRADUATE,lahm_classroom_class_graduate_cmd);
	POST_MSG(PROTO_GET_LAHM_CLASSROOM_TEACH_REM,lahm_classroom_teach_remembrance_cmd);
	POST_MSG(PROTO_GET_LAHM_CLASSROOM_ITEMS,lahm_classroom_get_items_cmd);
	POST_MSG(PROTO_USE_LAHM_CLASSROOM_ITEMS,lahm_classroom_use_item_cmd);
	POST_MSG(PROTO_GET_LAHM_CLASSROOM_FRIEND_INFO,get_friend_classroom_info_cmd);
	POST_MSG(PROTO_ADD_LAHM_CLASSROOM_GAME_SCORE,add_lahm_classroom_game_score_cmd);
	POST_MSG(PROTO_ANSWER_LAHM_CLASSROOM_LESSON_QUESTION,answer_lahm_classroom_lesson_question_cmd);
	POST_MSG(PROTO_GET_LAHM_CLASSROOM_HONOR_LIST,get_lahm_classroom_honor_list_cmd);
	POST_MSG(PROTO_GET_LAHM_CLASSROOM_INNER_STYLE_LIST,get_classroom_inner_style_list_cmd);
	POST_MSG(PROTO_GET_EVENT_RECHARGE_MONTHS,get_the_event_recharge_months_cmd);
	POST_MSG(PROTO_GET_EVENT_RECHARGE_REPLAY,set_the_event_recharge_replay_cmd);
	POST_MSG(PROTO_LAHM_CLASSROOM_ON_FELLOWSHIP,on_lahm_classroom_fellowship_cmd);
	//POST_MSG(PROTO_GIVE_ITEM_TO_FRIEND,give_item_to_friend_cmd);
	POST_MSG(PROTO_QUERY_TYPE_RANK,query_type_rank_cmd);
	POST_MSG(PROTO_QUERY_TYPE_HISTORY_RECORD,query_get_item_history_cmd);
	POST_MSG(PROTO_GET_TYPE_COUNT_ITEM_BONUS,get_type_count_item_bonus_cmd);
	POST_MSG(PROTO_GET_TYPE_RANK_COUNT_INFO,get_type_rank_count_info_cmd);
	POST_MSG(PROTO_SNOWBALL_ENTER_QUIT,snowball_enter_quit_cmd);
	POST_MSG(PROTO_SNOWBALL_ATTACK_ENEMY,snowball_attack_enemy_cmd);
	POST_MSG(PROTO_SNOWBALL_QUERY_TEAMS_INFO,snowball_query_teams_info_cmd);
	POST_MSG(PROTO_SNOWBALL_OCUPY_POS,snowball_ocupy_pos_cmd);
	POST_MSG(PROTO_SNOWBALL_GET_GAME_BONUS,snowball_game_bonus_cmd);
	POST_MSG(PROTO_SNOWBALL_GET_LEFT_TIME,snowball_get_left_time_cmd);
	POST_MSG(PROTO_SNOWBALL_LAHM_ATTACK,snowball_lahm_attack_cmd);
	POST_MSG(PROTO_SNOWBALL_GET_GAME_NEW_BONUS,snowball_game_new_bonus_cmd);
	
	POST_MSG(PROTO_GET_USED_MIBI_COUNT, get_used_mibi_count_cmd);
	POST_MSG(PROTO_EXCHANGE_BEANS, exchange_beans_cmd);
	POST_MSG(PROTO_CHRISTMAS_SOCK_ADD_SOCK, add_christmas_sock_cmd);
	POST_MSG(PROTO_CHRISTMAS_SOCK_GET_SOCK_INFO, get_christmas_sock_info_cmd);
	POST_MSG(PROTO_CHRISTMAS_SOCK_GET_BONUS, get_christmas_sock_bonus_cmd);
	POST_MSG(PROTO_GET_CHRIS_CAKE_INFO, get_chris_cake_info_cmd);
	POST_MSG(PROTO_END_MAKE_ONE_CAKE, end_make_one_cake_cmd);
	POST_MSG(PROTO_GIVE_CAKE_TO_NPC, give_cake_to_npc_cmd);
	POST_MSG(PROTO_GET_CHRIS_CAKE_SCORE, get_chris_cake_score_cmd);
	POST_MSG(PROTO_GET_FOOT_PRINT_BONUS, get_foot_print_bonus_cmd);
	POST_MSG(PROTO_GET_FOOT_PRINT_ITEMS, foot_print_get_items_cmd);
	POST_MSG(PROTO_GET_FOOT_PRINT_COUNT_INFO, get_foot_print_count_info_cmd);
	POST_MSG(PROTO_GET_CONTINUE_LOGIN_BONUS_INFO, get_continue_login_bonus_info_cmd);
	POST_MSG(PROTO_GET_QUES_NAIRE_INFO, get_ques_naire_info_cmd);
	POST_MSG(PROTO_GET_30_DAY_LOGIN_BONUS_INFO, get_30_day_login_bonus_info_cmd);
	POST_MSG(PROTO_TRAIN_KAKU_NIAN, train_kaku_nian_cmd);
	POST_MSG(PROTO_GET_TRAIN_KAKU_NIAN_INFO, get_train_kaku_nian_info_cmd);
	POST_MSG(PROTO_LEAVE_HERO_GAME, leave_hero_game_cmd);
	POST_MSG(PROTO_GET_IN_HERO_USERID_LIST, get_in_hero_userid_list_cmd);
	POST_MSG(PROTO_GET_ANIMAL_LVL, get_animal_lvl_cmd);
	POST_MSG(PROTO_FARM_ANIMAL_USE_ITEM, farm_animal_use_item_cmd);
	POST_MSG(PROTO_GET_ANIMAL_USED_SKILL_INFO, get_animal_used_skill_info_cmd);
	POST_MSG(PROTO_FARM_ANIMAL_USED_SKILL, farm_animal_use_skill_cmd);
	POST_MSG(PROTO_GET_MAP_RAIN_EGG_INFO, get_map_rain_egg_info_cmd);
	POST_MSG(PROTO_GAIN_MAP_POS_EGG, gain_map_pos_egg_cmd);
	POST_MSG(PROTO_OCTOPUS_CAR_GAME, get_octopus_car_win_or_lose_cmd);
	POST_MSG(PROTO_GET_GOOD_ANIMAL_NUM, get_very_good_animal_count_cmd);
	POST_MSG(PROTO_GAIN_USE_SKILL_PRIZE_ITEM, gain_use_skill_prize_item_cmd);
	POST_MSG(PROTO_GET_USE_SKILL_PRIZE_INFO, get_use_skill_prize_info_cmd);
	POST_MSG(PROTO_GAIN_GAME_ANGLES, gain_game_angles_cmd);
	POST_MSG(PROTO_GAIN_WEEKEND_BONUS, get_weekend_login_bonus_cmd);
	POST_MSG(PROTO_GET_WEEKEND_CONTINUE_LOGIN_COUNT, get_weekend_continue_login_count_cmd);
	POST_MSG(PROTO_ADD_CLIENT_MSG_LOG, add_client_msg_log_cmd);

	POST_MSG(PROTO_GET_PARADISE, get_paradise_cmd);
	POST_MSG(PROTO_GET_PRARDISE_STORE_ITEMS ,get_paradise_store_items_cmd);
	POST_MSG(PROTO_ADD_NIMSBUS, add_nimsbus_cmd);
	POST_MSG(PROTO_FEED_ANGLE_SEED, feed_angle_seed_cmd);
	POST_MSG(PROTO_BUILD_ANGLE_CONTRACT, build_angle_contract_cmd);
	POST_MSG(PROTO_FREE_ANGLE_IN_PARADISE, free_angle_in_paradise_cmd);
	POST_MSG(PROTO_GET_CONTRACT_ANGLES, get_contract_angles_cmd);
	POST_MSG(PROTO_FREE_CAPTURED_ANGEL, free_captured_angle_in_paradise_cmd);
	POST_MSG(PROTO_MAKE_ANGLE_FALLOWED, make_angle_fallowed_cmd);
	POST_MSG(PROTO_CHANGE_TO_ANGEL, change_to_angle_cmd);
	POST_MSG(PROTO_CHANGE_BACKGROUND, change_background_cmd);
	POST_MSG(PROTO_GET_FRIEND_PARADISE_INFO, get_friend_paradise_info_cmd);
	POST_MSG(PROTO_GET_VISITOR_PARADISE_INFO, get_visitor_paradise_info_cmd);
	POST_MSG(PROTO_GET_HOT_ITEM, get_hot_item_info_cmd);
	POST_MSG(PROTO_USE_PARADISE_PROP, use_prop_info_cmd);
	POST_MSG(PROTO_GET_FIGHT_ANGEL, get_angel_info_for_td_cmd);
	POST_MSG(PROTO_GET_FIGHT_LEVEL, get_fight_level_cmd);
	POST_MSG(PROTO_GET_FIGHT_SCORE, get_fight_score_cmd);
	POST_MSG(PROTO_GET_TD_START, td_game_start_cmd);
	POST_MSG(PROTO_GET_TD_END, td_game_end_cmd);
	POST_MSG(PROTO_CLEAN_ANGEL_GAME_BEGIN, clean_black_angel_start_cmd);
	POST_MSG(PROTO_CLEAN_ANGEL_SUCESS, clean_black_angel_sucess_cmd);
	POST_MSG(PROTO_CLEAN_ANGEL_GAME_END, clean_black_angel_end_cmd);
	POST_MSG(PROTO_KILL_MONSTER, td_game_kill_monster_cmd);
	POST_MSG(PROTO_ADD_ANGEL, add_angel_cmd);
	POST_MSG(PROTO_GET_ANGEL_IN_HOSPITAL, get_angel_in_hospital_cmd);
	POST_MSG(PROTO_USE_TEAR_FOR_ANGEL, use_tear_of_jack_cmd);
	POST_MSG(PROTO_LEAVE_HOSPITAL, angel_leave_hospotal_cmd);
	POST_MSG(PROTO_USE_PROP_FOR_TD, use_prop_for_td_cmd);
	POST_MSG(PROTO_GET_PROP_FOR_TD, get_prop_for_td_cmd);
	//for angel favorite
	POST_MSG(PROTO_ANGEL_SHOW, angel_show_cmd);
	POST_MSG(PROTO_ANGEL_REGAIN, angel_regain_cmd);
	POST_MSG(PROTO_ANGEL_GET_FAVORITE_INFO, angel_get_favorite_info_cmd);
	POST_MSG(PROTO_GET_PARADISE_BACKGROUND_ID, get_angel_paradise_background_cmd);
	POST_MSG(PROTO_EXCHANGE_DAVIS_BEAN, exchange_davis_bean_cmd);
	POST_MSG(PROTO_GET_PARADISE_HONORS, get_angel_paradise_honors_cmd);
	POST_MSG(PROTO_GET_HONOR_ITEM, angel_get_honor_bonus_cmd);
	POST_MSG(PROTO_SAVE_MONSTER_GAME, save_monster_game_cmd);
	POST_MSG(PROTO_GET_ANGEL_COMPOSE_MATERIAL, get_angel_compose_material_cmd);
	POST_MSG(PROTO_USER_COMPOSE_NEW_ANGEL, user_compose_new_angel_cmd);

	POST_MSG(PROTO_ANGEL_CHALLENGE_GET_AWARD, angel_get_game_award_cmd);
	//dugneon explore
	POST_MSG(PROTO_DUNGEON_BEGIN_EXPLORE, dungeon_begin_explore_cmd);
	POST_MSG(PROTO_DUNGEON_END_EXPLORE, dungeon_end_explore_cmd);
	POST_MSG(PROTO_DUNGEON_GET_BAG_INFO, dungeon_get_bag_info_cmd);
	POST_MSG(PROTO_DUNGEON_EXPLORE_AREA, dungeon_explore_area_cmd);
	POST_MSG(PROTO_DUNGEON_PICK_ITEM, dungeon_pick_item_cmd);
	POST_MSG(PROTO_DUNGEON_USE_ITEM, dungeon_use_item_cmd);
	POST_MSG(PROTO_DUNGEON_SYN_HP, dungeon_syn_hp_cmd);

	//for dungeon exhibit
	POST_MSG(PROTO_DUNGEON_GET_EXHIBIT_INFO, dungeon_get_exhibit_info_cmd);
	POST_MSG(PROTO_DUNGEON_GET_STORAGE_INFO, dungeon_get_storage_info_cmd);
	POST_MSG(PROTO_DUNGEON_SET_EXHIBIT_ITEM, dungeon_set_exhibit_item_cmd);
	POST_MSG(PROTO_DUNGEON_SWAP_EXHIBIT_ITEM, dungeon_swap_exhibit_item_cmd);
	POST_MSG(PROTO_DUNGEON_SET_EXHIBIT_STATE, dungeon_set_exhibit_state_cmd);
	POST_MSG(PROTO_DUNGEON_GET_FRIEND_INFO, dungeon_get_friend_info_cmd);
	POST_MSG(PROTO_DUNGEON_GET_VISITOR_INFO, dungeon_get_visitor_info_cmd);
	POST_MSG(PROTO_DUNGEON_SET_BACKGROUND, dungeon_set_background_map_cmd);

	//for angry lahm
	POST_MSG(PROTO_AL_GET_TASK_INFO, al_get_task_info_cmd);
	POST_MSG(PROTO_AL_UPDATE_SCORE, al_update_score_cmd);
	//for hide and seek
	POST_MSG(PROTO_HAS_JOIN_GAME, has_join_game_cmd);
	POST_MSG(PROTO_HAS_QUIT, has_quit_game_cmd);
	POST_MSG(PROTO_HAS_GAME_OVER, has_game_over_cmd);
	POST_MSG(PROTO_HAS_GET_QUEUE_INFO, has_get_queue_info_cmd);

	POST_MSG(PROTO_GET_MONGOLIA_COW_PASTURE, get_mongolia_cow_pasture_cmd);
	POST_MSG(PROTO_PLANT_PASUTURE_GRASS, plant_pasture_grass_cmd);
	POST_MSG(PROTO_PLAY_WITH_MILK_COW, play_with_milk_cow_cmd);
	POST_MSG(PROTO_PURCHASE_PASTURE_MILK, purchase_pasture_milk_cmd);
	//for angel fight
	POST_MSG(PROTO_AF_GET_USER_BASE_INFO, af_get_user_base_info_cmd);
	POST_MSG(PROTO_AF_GET_USER_PACKAGE, af_get_user_package_cmd);
	POST_MSG(PROTO_AF_GET_USER_SKILL, af_get_fight_skill_cmd);
	POST_MSG(PROTO_AF_GET_CARD_BOOK, af_get_fight_card_cmd);
	POST_MSG(PROTO_AF_USE_ITEM, af_use_item_cmd);
	POST_MSG(PROTO_AF_USE_CARD, af_use_card_cmd);
	POST_MSG(PROTO_AF_SKILL_LEVEL_UP, af_user_skill_level_up_cmd);
	POST_MSG(PROTO_AF_CHANGE_EQUIP, af_user_change_equip_cmd);
	POST_MSG(PROTO_AF_GET_ALL_FRIENDS, af_get_all_friends_cmd);
	POST_MSG(PROTO_AF_GET_ALL_FRIENDS_FIGHT, af_get_all_friends_fight_cmd);
	POST_MSG(PROTO_AF_GET_MAP_PASS_POINT, af_get_map_pass_point_cmd);
	POST_MSG(PROTO_AF_GET_ONE_SKILL, af_get_one_skill_level_cmd);
	POST_MSG(PROTO_AF_CHECK_USER_GET_PRIZE, af_check_user_get_prize_cmd);
	POST_MSG(PROTO_AF_USER_GET_PRIZE, af_user_get_prize_cmd);
	POST_MSG(PROTO_AF_USER_SET_WISHING, af_user_set_wishing_cmd);
	POST_MSG(PROTO_AF_DEAL_FRIEND_WISHING, af_deal_friend_wishing_cmd);
	POST_MSG(PROTO_AF_GET_WISHING_STATE, af_user_get_wishing_state_cmd);
	POST_MSG(PROTO_AF_DEAL_FRIEND_EVENT, af_deal_friend_event_cmd);
	POST_MSG(PROTO_AF_GET_SOME_FRIEND_INFO, af_user_get_some_friends_info_cmd);
	POST_MSG(PROTO_AF_USER_GET_FIGHT_RECORD, af_user_get_fight_record_cmd);
	POST_MSG(PROTO_AF_GET_WISHING_COLLECT, af_user_get_wishing_collect_cmd);
	POST_MSG(PROTO_AF_GET_DAILY_TASK, af_user_get_daily_task_cmd);
	POST_MSG(PROTO_AF_FINISH_ONE_TASK, af_user_finish_one_task_cmd);
	POST_MSG(PROTO_AF_REFRESH_ONE_TASK, af_user_refresh_one_task_cmd);
	POST_MSG(PROTO_AF_GET_FIGHT_BOSS_COUNT, af_user_get_fight_boss_count_cmd);
	POST_MSG(PROTO_AF_GET_APPRENTICE_MARKET, af_user_get_apprentice_market_cmd);
	POST_MSG(PROTO_AF_GET_MASTER_APPRENTICE_INFO, af_get_master_apprentice_info_cmd);
	POST_MSG(PROTO_AF_CHECK_GET_APPRENTICE, af_check_get_an_apprentice_cmd);
	POST_MSG(PROTO_AF_GET_APPRENTICE_TRAINING, af_get_apprentice_trainning_info_cmd);
	POST_MSG(PROTO_AF_USER_TRAIN_APPRENTICE, af_user_train_apprentice_cmd);
	POST_MSG(PROTO_AF_USER_EXPEL_APPRENTICE, af_user_expel_apprentice_cmd);
	POST_MSG(PROTO_AF_APPRENTICE_RESPECT_MASTER, af_apprentice_respect_master_cmd);
	POST_MSG(PROTO_AF_GET_RESPECT_CASH, af_user_get_respect_cash_cmd);
	POST_MSG(PROTO_AF_APPRENTICE_RETRAY_MASTER, af_apprentice_retray_master_cmd);
	POST_MSG(PROTO_AF_FINISH_APPRENTICE, af_finish_apprentice_cmd);
	POST_MSG(PROTO_AF_GET_MASTER_RECORD, af_user_get_master_record_cmd);
	POST_MSG(PROTO_AF_USER_ADD_APPRENTICE, af_user_add_apprentice_cmd);
	//as send message log
	//for TW_VER
	POST_MSG(PROTO_AS_SEND_MSGLOG, as_send_msglog_cmd);
	POST_MSG(PROTO_AF_GET_PVP_FIGHT_RECORD, af_get_pvp_fight_record_cmd);
	POST_MSG(PROTO_AF_GET_PVP_WIN_PRIZE, af_get_pvp_win_prize_cmd);

	POST_MSG(PROTO_AF_GET_DONATE_INFO, af_get_donate_info_cmd);
	POST_MSG(PROTO_AF_DONATE_CARD, af_donate_card_cmd);
	POST_MSG(PROTO_AF_EXCHANGE_CARD, af_exchange_card_cmd);

	POST_MSG(PROTO_SEND_TIME_MAIL, send_time_mail_cmd);
	POST_MSG(PROTO_GET_TIME_MAIL, get_time_mail_cmd);
	POST_MSG(PROTO_SEND_TIME_ITEM, send_time_item_cmd);
	//POST_MSG(PROTO_GET_TIME_ITEM, get_time_item_cmd);
	POST_MSG(PROTO_GET_MAIL_ITEM_COUNT, get_time_mail_item_count_cmd);
	POST_MSG(PROTO_USER_NATIONAL_DAY_LOGIN, user_get_national_day_login_cmd);
	POST_MSG(PROTO_USER_GET_NATIONAL_DAY_GIFT, user_get_national_day_gift_cmd);

	POST_MSG(PROTO_FB_GET_BOSS_HP_CMD, fb_get_boss_info_cmd);
	POST_MSG(PROTO_FB_ENTER_TURRET_CMD, fb_enter_turret_cmd);
	POST_MSG(PROTO_FB_LEAVE_TURRET_CMD, fb_leave_turret_cmd);

	POST_MSG(PROTO_GET_BOOK_INFO, get_book_info_cmd);
	POST_MSG(PROTO_SET_BOOK_INFO, set_book_info_cmd);
	POST_MSG(PROTO_GET_BOOK_LIST, get_book_list_cmd);

	POST_MSG(PROTO_CP_GET_GAME_INFO, cutepig_get_game_info_cmd);
	POST_MSG(PROTO_CP_GET_PIG_INFO, cutepig_get_pig_info_cmd);
	POST_MSG(PROTO_CP_GET_KNAPSACK_INFO, cutepig_get_knapsack_info_cmd);
	POST_MSG(PROTO_CP_RAISE_PIG, cutepig_raise_pig_cmd);
	POST_MSG(PROTO_CP_BATHE, cutepig_bathe_cmd);
	POST_MSG(PROTO_CP_TICKLE, cutepig_tickle_cmd);
	POST_MSG(PROTO_CP_TRAINING, cutepig_training_cmd);
	POST_MSG(PROTO_CP_MATE, cutepig_mate_cmd);
	//POST_MSG(PROTO_CP_BUY_ITEM, cutepig_buy_item_cmd);
	POST_MSG(PROTO_CP_SALE, cutepig_sale_cmd);
	POST_MSG(PROTO_CP_SET_NAME, cutepig_set_name_cmd);
	POST_MSG(PROTO_CP_SET_FORMATION, cutepig_set_formation_cmd);
	POST_MSG(PROTO_CP_FEED, cutepig_feed_cmd);
	POST_MSG(PROTO_CP_GET_MALE_PIG_INFO, cutepig_get_players_male_pig_info_cmd);
	POST_MSG(PROTO_CP_GET_FRIEND_INFO, cutepig_get_friend_info_cmd);
	POST_MSG(PROTO_CP_GET_TASK_INFO, cutepig_get_task_list_cmd);
	POST_MSG(PROTO_CP_ACCEPT_TASK, cutepig_accept_task_cmd);
	POST_MSG(PROTO_CP_SUBMIT_TASK, cutepig_submit_task_cmd);
	POST_MSG(PROTO_CP_GET_NOTICE_INFO, cutepig_get_notice_info_cmd);
	POST_MSG(PROTO_CP_PROCESS, cutepig_process_cmd);
	POST_MSG(PROTO_CP_GET_RAND_LUCKY, cutepig_get_rand_lucky_cmd);
	POST_MSG(PROTO_CP_GET_ILLUSTRATE, cutepig_get_illustrate_cmd);
	POST_MSG(PROTO_CP_UPGRADE_BUILDING, cutepig_upgrade_building_cmd);
	POST_MSG(PROTO_CP_USE_ITEM, cutepig_use_item_cmd);
	POST_MSG(PROTO_CP_GET_TICKET, cutepig_get_ticket_cmd);
	POST_MSG(PROTO_CP_USE_CARD, cutepig_use_card_cmd);
	POST_MSG(PROTO_CP_GET_CARD_INFO, cutepig_get_card_info_cmd);
	//POST_MSG(PROTO_GET_FEIFEI_DAREN_PRIZE, get_feifei_daren_prize_cmd); 20111104下架
	POST_MSG(PROTO_CP_EXCHANGE_PIG, cutepig_exchange_pig_with_items_cmd);
	POST_MSG(PROTO_CP_SET_BACKGROUND, cutepig_set_background_cmd);
	//for 2011.11.18
	POST_MSG(PROTO_CP_PIG_FOLLOW, cutepig_set_pig_follow_cmd);
	POST_MSG(PROTO_CP_GET_SHOW_STAGE_INFO, cutepig_get_show_stage_info_cmd);
	POST_MSG(PROTO_CP_SHOW_PIG, cutepig_beauty_show_cmd);
	POST_MSG(PROTO_CP_GET_2ND_GAME_INFO, cutepig_get_2nd_game_info_cmd);
	POST_MSG(PROTO_CP_SWAP_PIG, cutepig_swap_cmd);
	//end for
	POST_MSG(PROTO_CP_GET_BEAUTY_GAME_PRIZE, cutepig_get_beauty_game_prize_cmd);
	POST_MSG(PROTO_CP_BEAUTY_SHOW_WITH_NPC, cutepig_beauty_show_with_npc_cmd);
	POST_MSG(PROTO_CP_BEAUTY_SHOW_CHECK_ACHIEVE, cutepig_get_bs_npc_pk_achieve_info_cmd);
	POST_MSG(PROTO_CP_BEAUTY_SHOW_GET_ACHIEVE, cutepig_get_bs_npc_pk_achieve_cmd);
	POST_MSG(PROTO_CP_NPC_WEIGHT_PK, cutepig_npc_weight_pk_cmd);
	POST_MSG(PROTO_CP_NPC_WEIGHT_PK_CHECK, cutepig_get_npc_weight_pk_info_cmd);
	POST_MSG(PROTO_CP_ILLUSTRATE_EXCHANGE_PIG, cutepig_get_pig_by_illustrate_cmd);
	POST_MSG(PROTO_CP_GET_ALL_HONOR, cutepig_get_all_honor_cmd);
	
	POST_MSG(PROTO_SG_GET_PATROL_TESK, sg_get_patrol_tast_cmd);
	POST_MSG(PROTO_SG_USER_ASK_FOR_HELP, sg_user_ask_for_help_cmd);
	POST_MSG(PROTO_SG_GUIDER_HELP_OTHERS, sg_guider_help_others_cmd);
	POST_MSG(PROTO_SG_GET_GUIDER_PRIZE, sg_get_guider_prize_cmd);
	POST_MSG(PROTO_SG_LOOK_GUIDER_PRIZE, sg_look_guider_prize_cmd);

	POST_MSG(PROTO_GET_CANDY_FREIND_CNT, get_candy_friend_cnt_cmd);
	POST_MSG(PROTO_GET_CANDY_FROM_FREIND, get_candy_from_friend_cmd);
	POST_MSG(PROTO_GET_CANDY_SHARE_BONUS, get_candy_share_bonus_cmd);

	POST_MSG(PROTO_SAVE_AVATAR, save_avatar_cmd);
	POST_MSG(PROTO_REMOVE_AVATAR, remove_avatar_cmd);
	POST_MSG(PROTO_GET_AVATAR_LIST, get_avatar_list_cmd);

	POST_MSG(PROTO_GET_TASK_LIST_INFO, get_task_list_info_cmd);
	POST_MSG(PROTO_AS_ADD_TYPE_INT_DATA, add_as_type_int_data_cmd);
	POST_MSG(PROTO_AS_QUERY_TYPE_INT_DATA, query_as_type_int_data_cmd);
	POST_MSG(PROTO_HANG_CHRISTMAS_SOCK, hang_christmas_sock_cmd);
	POST_MSG(PROTO_GET_CHRISTMAS_SOCR_RECORD, get_christmas_sock_record_cmd);
	POST_MSG(PROTO_GET_CHRISTMAS_SOCK_TOP10, get_christmas_sock_top10_cmd);
	POST_MSG(PROTO_GET_CHRISTMAS_SOCK_GIFTBOX, get_christmas_sock_giftbox_cmd);
	POST_MSG(PROTO_GET_CHRISTMAS_SOCK_GIFT, get_christmas_sock_gift_cmd);
	
	POST_MSG(PROTO_USER_SUBMIT_SPRING_COUPLETS, user_submit_spring_couplets_cmd);
	POST_MSG(PROTO_USER_GET_SPRING_COUPLETS, user_get_spring_couplets_cmd);
	POST_MSG(PROTO_USER_GET_COUPLETS_USERID_LIST, user_get_couplets_userid_list_cmd);
	POST_MSG(PROTO_USER_GET_COUPLETS_PRIZE, user_get_couplets_prize_cmd);
	POST_MSG(PROTO_USER_SET_COUPLETS_PRIZE, user_set_couplets_prize_cmd);

	POST_MSG(PROTO_GET_VIP_ANGEL_INFO, get_vip_angel_info_cmd);
	POST_MSG(PROTO_EXCHANGE_VIP_ANGEL, exchange_vip_angel_cmd);

	//POST_MSG(PROTO_ADD_MINE_MAP_PIECE, add_mine_map_piece_cmd);
	POST_MSG(PROTO_GET_MINE_MAP_PIECES, get_mine_map_pieces_cmd);
	POST_MSG(PROTO_GET_MINE_KEY_CHANCE, get_mine_key_chance_cmd);
	POST_MSG(PROTO_GET_MINE_MAP_KEY, get_mine_map_key_cmd);
	POST_MSG(PROTO_QUERY_MINE_KEY_CHANCE, query_mine_key_chance_cmd);
	POST_MSG(PROTO_QUERY_MINE_MAP_EXPLOR_TIME, query_mine_map_explor_time_cmd);

	POST_MSG(PROTO_GET_DAYTYPE_COUNT_LIST, get_day_type_count_cmd);
	POST_MSG(PROTO_GET_GOLD_COMPASS_USERS_INFO, get_gold_compass_users_info_cmd);
	POST_MSG(PROTO_GUESS_DATE_ITEM_PRICE, guess_date_item_price_cmd);
	POST_MSG(PROTO_GUESS_RIGHT_USERS_INFO, get_guess_right_user_info_cmd);
	POST_MSG(PROTO_GET_SPRINT_FESTIVAL_LOGIN_INFO, get_sprint_festival_login_info_cmd);
	POST_MSG(PROTO_GET_SPRINT_FESTIVAL_LOGIN_BONUS, get_sprint_festival_login_bonus_cmd);
	POST_MSG(PROTO_GET_VIP_CONTINUE_WEEK_LOGIN_INFO, get_vip_continue_week_login_info_cmd);
	POST_MSG(PROTO_GET_VIP_WEEK_LOGIN_AWARD, get_vip_week_login_award_cmd);
	POST_MSG(PROTO_GET_ANOTHER_WEEKEND_LOGIN_INFO,get_another_weekend_login_info_cmd );
	POST_MSG(PROTO_GET_ANOTHER_WEEKEND_LGOIN_BONUS,get_another_weekend_login_bonus_cmd);
	
	POST_MSG(PROTO_ADD_USER_ITEMS_SCORE,add_user_item_score_cmd);
	POST_MSG(PROTO_GET_USER_ITEMS_SCORE,get_user_items_score_cmd);
	POST_MSG(PROTO_CHANGE_USER_ICE_DRAGON,change_ice_dragon_cmd);
	POST_MSG(PROTO_EXCHANGE_USER_PIGLET_HOUSE,exchange_user_piglet_house_cmd);

	POST_MSG(PROTO_CHP_DONATE, charparty_donate_item_cmd);
	POST_MSG(PROTO_CHP_AUCTION, charparty_auction_item_cmd);
	POST_MSG(PROTO_CHP_GET_AUCTION_LIST, charparty_get_auction_list_cmd);
	POST_MSG(PROTO_DONATE_FOOD_ITEM, donate_food_item_cmd);
	POST_MSG(PROTO_YEAR_FEAST_GET_ALL_INFO, year_feast_get_all_info_cmd);
	POST_MSG(PROTO_YEAR_FEAST_WAHS_FOOD, year_feast_wash_food_cmd);
	POST_MSG(PROTO_YEAR_FEAST_COOK_FIRE, year_feast_cook_fire_cmd);
	POST_MSG(PROTO_YEAR_FEAST_COOK_FOOD, year_feast_cook_food_cmd);
	POST_MSG(PROTO_YEAR_FEAST_SET_CAKE_POS, year_feast_set_cake_pos_cmd);
	POST_MSG(PROTO_YEAR_FEAST_SET_TABLE_POS, year_feast_set_table_pos_cmd);
	POST_MSG(PROTO_YEAR_FEAST_GET_PRIZE, year_feast_get_prize_cmd);

	//冬天春天大作战，已经下架
// 	POST_MSG(PROTO_WVS_SWITCH_TEAM, wvs_team_switch_cmd);
	POST_MSG(PROTO_WVS_GET_TEAM_POWER, wvs_get_team_power_cmd);

	POST_MSG(PROTO_LD_GET_SP_LANTERN_INFO, ld_get_sp_lanterns_info_cmd);
	POST_MSG(PROTO_LD_ATTACK_SP_LANTERN, ld_attack_sp_lanterns_cmd);

	POST_MSG(PROTO_FP_GET_FLOWER, fp_get_flower_cmd);
	POST_MSG(PROTO_FP_SET_FLOWER, fp_set_flower_cmd);
	POST_MSG(PROTO_FP_OPEN_VIP_FLOWER, fp_open_vip_flower_cmd);
	POST_MSG(PROTO_FP_GET_GAME_INFO, fp_get_game_info_cmd);
	POST_MSG(PROTO_FP_GET_AWARD, fp_get_award_cmd);
	
	POST_MSG(PROTO_AC_GET_AWARD, ac_get_award_cmd);

	POST_MSG(PROTO_GET_USER_CUTEPIG_GOLD_KEY, get_user_cutepig_gold_key_cmd);
	POST_MSG(PROTO_SPEECH_USER_ENTER_OR_LEAVE, speech_user_enter_or_leave_cmd);
	POST_MSG(PROTO_SPEECH_GET_USER_INFO, speech_user_get_user_info_cmd);

	POST_MSG(PROTO_GET_USER_CUTEPIG_MACHINE_WORK, cutepig_get_piglet_machine_work_cmd);
	POST_MSG(PROTO_USER_CUTEPIG_EXPLOR_MAP_MINE, cutepig_user_pig_explor_mine_cmd);
	POST_MSG(PROTO_GET_USER_CUTEPIG_EXPLOR_MINE_INFO, cutepig_get_explor_mine_info_cmd);
	POST_MSG(PROTO_CP_USER_PIGLET_MELT_ORE, cutepig_user_piglet_melt_ore_cmd);
	POST_MSG(PROTO_CP_USER_PRODUCE_MACHINE_PART, cutepig_user_produce_machine_part_cmd);
	POST_MSG(PROTO_CP_USER_PIGLET_FINISH_WORK, cutepig_user_pig_finish_work_cmd);
	POST_MSG(PROTO_CP_USER_PIGLET_WORK_MACHINE, cutepig_get_user_piglet_work_machine_cmd);
	POST_MSG(PROTO_CP_USER_PIGLET_USE_ACCELERATE_TOOL, cutepiglet_use_accelerate_machine_tool_cmd);
	POST_MSG(PROTO_CP_USER_PIGLET_GUIDER_STEP_GET, cutepiglet_guider_get_work_cmd);
	POST_MSG(PROTO_CP_USER_PIGLET_GUIDER_STEP_SET, cutepiglet_guider_set_work_cmd);
	POST_MSG(PROTO_GET_GREAT_PRODUCE_ORDER, get_majesty_or_urgent_order_cmd);
	POST_MSG(PROTO_PUT_GREAT_PRODUCE_ORDER, put_majesty_or_urgent_order_cmd);
	POST_MSG(PROTO_FINISH_GREAT_PRODUCE_ORDER,finish_majesty_or_urgent_order_cmd);
	POST_MSG(PROTO_CP_USER_PIGLET_PRODUCE_SPECIAL_MACHINE, cutepig_user_produce_special_machine_cmd);
	POST_MSG(PROTO_CP_USER_PIGLET_RANDOM_MACHINE_PRODUCT, cutepig_user_give_random_machine_something_cmd);
	POST_MSG(PROTO_PUT_USER_KFC_DECORATION, put_user_KFC_decoration_cmd);
	POST_MSG(PROTO_GET_USER_KFC_DECORATION,  get_user_KFC_decoration_cmd);
	POST_MSG(PROTO_VOTE_KFC_FRIEND_DECORATION,  vote_KFC_friend_decoration_cmd);
	POST_MSG(PROTO_GET_POLL_FRIEND_DECORATION,  get_poll_friend_decoration_cmd);
	POST_MSG(PROTO_MW_ADD_ITEM_IN_FRIENDSHIP_BOX,mw_add_item_in_friendship_box_cmd);
	POST_MSG(PROTO_MW_GET_ITEM_FROM_FRIENDSHIP_BOX,mw_get_item_from_friendship_box_cmd);
	POST_MSG(PROTO_MW_QUERY_ITEMS_FROM_FRIENDSHIP_BOX,mw_query_items_from_friendship_box_cmd);
	POST_MSG(PROTO_MW_GET_ITEM_HISTORY_FROM_FRIENDSHIP_BOX,mw_get_item_history_from_friendship_box_cmd);
	//POST_MSG(PROTO_USER_CATCH_BUTTERFLY_ANGEL,user_catch_butterfly_angel_cmd);
	POST_MSG(PROTO_USER_LAHM_SPORT_CREATE_MVP_TEAM,create_lahm_sport_mvp_team_cmd);
	POST_MSG(PROTO_USER_LAHM_SPORT_DISSOLVE_MVP_TEAM,dissolve_lahm_sport_mvp_team_cmd);
	POST_MSG(PROTO_USER_LAHM_SPORT_GET_MVP_TEAM_INFO,get_lahm_sport_mvp_team_info_cmd);
	POST_MSG(PROTO_CP_USER_TAKE_BACK_MINING_PIGLET, cutepiglet_take_back_mining_pig_cmd);
	POST_MSG(PROTO_USER_CHANGE_LAHM_SPORT_TEAM,user_change_lahm_sport_team_cmd);
	POST_MSG(PROTO_USER_GET_MVP_TEAM_RANKING,user_get_mvp_team_ranking_cmd);
	POST_MSG(PROTO_USER_CHEER_TEAM_ACT,cheer_team_user_act_cmd);
	POST_MSG(PROTO_GET_USER_CHEER_TEAM_INFO,query_cheer_team_info_cmd);
	POST_MSG(PROTO_GET_LAHM_SPORT_TEAMS_MEDALS,get_two_team_medal_cnt_cmd);
	POST_MSG(PROTO_USER_LAHM_SPORT_GET_SYSARG_MVP_TEAM,get_lahm_sport_sysarg_mvp_team_cmd);

	POST_MSG(PROTO_MAKE_BIRHTHDAY_CAKE_ACTIVIEY, mole_make_birthday_cake_cmd);
	POST_MSG(PROTO_GET_CURRENT_BIRTHDAY_CAKE_INFO, get_bithday_cake_curent_info_cmd);
	POST_MSG(PROTO_GET_MAKE_BIRTHDAY_CAKE_AWARD, get_make_birthday_cake_award_cmd);

	POST_MSG(PROTO_JOIN_CRYSTAL_CELEBRATE_DANCE, mole_join_in_dacing_cmd);
	POST_MSG(PROTO_CRYSTAL_WHOLE_DNANCE_SCENE, get_whole_dancing_scene_cmd);
	POST_MSG(PROTO_GET_RANDOM_DANCING_AWARD , get_dancing_random_item_cmd);
	POST_MSG(PROTO_CHECK_DANCING_THREE_TIMES , check_dancing_three_times_cmd);
	POST_MSG(PROTO_SET_TYPE_PHOTO_HEZHAO, set_type_photo_hezhao_cmd);
	POST_MSG(PROTO_GET_TYPE_PHOTO_HEZHAO, get_type_photo_hezhao_cmd);
	POST_MSG(PROTO_LIGHT_CELEBRATE_FOURYEAR_BOOK, check_four_celebrate_book_light_cmd);
	////
	POST_MSG(PROTO_SET_MOLE_MISS_NOTE, mole_set_miss_note_cmd); 
	POST_MSG(PROTO_MOLE_GET_ENERGY_STONE, mole_get_enery_rock_stones_cmd);
	POST_MSG(PROTO_MOLE_GET_MISS_NOTE, mole_get_miss_note_cmd);
	///
	POST_MSG(PROTO_USER_WATER_JACK_AND_MODOU, user_water_jack_and_modou_cmd);
	POST_MSG(PROTO_GET_WATER_TIME_JACK_AND_MODOU, get_water_time_jack_and_modou_cmd);
	POST_MSG(PROTO_USER_SHAKE_DICE, user_shake_dice_for_vip_cmd);
	POST_MSG(PROTO_USER_GET_SIGNAL_FLARE, user_get_signal_flare_cmd);
	POST_MSG(PROTO_USER_GET_SHAKE_DICE_LEFT_TIME, user_get_shake_dice_left_time_cmd);
	POST_MSG(PROTO_MOLE_CHECK_EXPEDITION_LOCK, mole_check_expedition_lock_cmd);
	POST_MSG(PROTO_MOLE_GET_MAKE_SHIP, mole_get_make_ship_cmd);
	POST_MSG(PROTO_MOLE_FUNFLY_SEAHORSE, mole_funfly_seahorse_cmd);
	//kula celebrate birthday
	POST_MSG(PROTO_USER_GUESS_SSCLOTH, user_guess_stone_scissors_cloth_cmd);
	POST_MSG(PROTO_USER_GET_FREE_VIP_FLAYER, get_free_player_info_cmd);
	POST_MSG(PROTO_MOLE_MERMAN_KINGDOM, mole_merman_kingdom_expedition_cmd);
	POST_MSG(PROTO_USER_GET_LOTTERY_LEFT_TIME,  get_lottery_draw_left_time_cmd);
	POST_MSG(PROTO_USER_CONTEND_FOR_PRAY, mole_contend_for_seat_cmd);
	POST_MSG(PROTO_USER_GET_PRAY_INFO, mole_get_pray_info_cmd);
	POST_MSG(PROTO_USER_SET_PRAY_STEP_INFO, mole_set_pray_activity_cmd);
	POST_MSG(PROTO_USER_GET_PRAY_STEP_INFO, mole_get_pray_activity_cmd);
	POST_MSG(PROTO_MOLE_GET_INVADE_MONSTER, mole_get_invade_monster_cmd);
	POST_MSG(PROTO_MOLE_ATTACK_INVADE_MONSTER, mole_attack_invade_monster_cmd);
	POST_MSG(PROTO_MOLE_GET_SELF_GAMES, get_mole_self_games_cmd);
	POST_MSG(PROTO_MOLE_ENTER_GAME_HALL, mole_enter_game_hall_cmd);


	//2012 summer for ocean system, proto begin with 8800
	POST_MSG(PROTO_USER_GET_OCEAN_INFO, get_user_ocean_info_cmd);
	POST_MSG(PROTO_USER_PUT_ANIMAL_IN_OCEAN, put_user_ocean_animal_in_ocean_cmd);
	POST_MSG(PROTO_USER_FEED_OCEAN_ANIMAL, user_feed_ocean_animal_cmd);
	POST_MSG(PROTO_USER_USE_OCEAN_TOOL, user_use_ocean_tool_cmd);
	POST_MSG(PROTO_USER_EXPAND_OCEAN_CAPACITY, user_expand_ocean_capacity_cmd);
	POST_MSG(PROTO_USER_GET_OCEAN_ITEMS, get_user_ocean_items_cmd);
	POST_MSG(PROTO_USER_DIY_SELF_OCEAN, user_diy_self_ocean_cmd);
	POST_MSG(PROTO_USER_GET_OFFLINE_SHELLS, user_get_offline_shells_cmd);
	POST_MSG(PROTO_USER_GET_ONLINE_SHELLS, user_get_online_shells_cmd);
	POST_MSG(PROTO_USER_PICK_ONLINE_SHELLS, user_pick_online_shells_cmd);
	POST_MSG(PROTO_USER_SALE_OCEAN_FISH, user_sale_ocean_fish_cmd);
	//POST_MSG(PROTO_USER_BUY_OCEAN_THINGS, user_buy_ocean_things_cmd);
	POST_MSG(PROTO_MOLE_GET_OCEAN_ADVENTURE, mole_get_ocean_adventure_cmd);
	POST_MSG(PROTO_MOLE_SET_OCEAN_ADVENTURE, mole_set_ocean_adventure_cmd);
	POST_MSG(PROTO_MOLE_GET_OCEAN_HANDBOOK, mole_get_ocean_handbook_cmd);
	POST_MSG(PROTO_MOLE_GET_OCEAN_FRIEND_LEVEL, mole_get_friend_ocean_info_cmd);
	POST_MSG(PROTO_MOLE_GET_SCROLL_FRAGMENT, mole_get_scroll_fragment_cmd);
	POST_MSG(PROTO_MOLE_GET_SCROLL_MAP_STATE, get_srcoll_map_state_cmd);
	POST_MSG(PROTO_MOLE_DO_WORK_FOR_OTHERS, mole_do_work_for_others_cmd);
	POST_MSG(PROTO_MOLE_MEET_MERMAID_AGAIN, mole_mee_mermaid_again_cmd);
	POST_MSG(PROTO_MOLE_FINISH_MERMAN_KINGDOM, mole_finish_merman_kingdom_expedition_cmd);

	POST_MSG(PROTO_USER_JOIN_COSPLAY_RACE, user_join_cosplay_race_cmd);
	POST_MSG(PROTO_SYSARG_GET_COSPLAY_RANK, sysarg_get_cos_rank_info_cmd);
	POST_MSG(PROTO_SYSARG_GET_COSPLAY_RANK_FALG, get_user_cos_rank_flag_cmd);

#undef POST_MSG
	return 0;
}

//---------------------------------------------------------------------------
static inline int
dispatch_protocol(sprite_t* p, int cmd, uint8_t* body, int len)
{
	int err;
	//DEBUG_LOG("---------------dispatch %u, cmd %d",p->id, cmd);
	if (p->waitcmd == cmd) {
		WARN_RETURN(("wait for cmd=%d, id=%u, new cmd=%d", p->waitcmd, p->id, cmd), 0);
		return 0;
	}
	if (p->waitcmd != 0) {
		send_to_self_error(p, cmd, -ERR_system_busy, 0);
		WARN_RETURN(("wait for cmd=%d, id=%u, new cmd=%d", p->waitcmd, p->id, cmd), 0);
	}
	if (cmd < 0 || cmd >= MAX_PROC_MSG_NUM) {
		ERROR_RETURN(("not supported command, cmd=%d, id=%u", cmd, p->id), -1);
	}
	p->waitcmd = cmd;
	if (dispatch[cmd]) {
		err = dispatch[cmd](p, body, len);
	} else {
		if ( p->group && SCENE_GAME(p->group) && GAME_STARTED(p->group) ) {
			err = (p->group->game->on_game_data
					? p->group->game->on_game_data(p, cmd, body, len) : 0);
		} else {
			p->waitcmd = err = 0;
		}
		if (err) {
			ERROR_RETURN(("not supported command, cmd=%d, id=%u", cmd, p->id), -1);
		}
	}

//	if (!err) check_triggered_task (p, cmd);
	return err;
}

int response_test_alive(fdsession_t* fdsess)
{
	uint8_t msg[sizeof (protocol_t)];
	init_proto_head(msg, PROTO_TEST_ALIVE, sizeof (protocol_t));
	int send_bytes, cur_len;
	int len = sizeof(protocol_t);
	protocol_t *proto;

	struct shm_block mb;

	proto = (protocol_t *) msg;
	proto->id = 0;

	mb.id   = fdsess->id;
	mb.fd   = fdsess->fd;
	mb.type = DATA_BLOCK;

	for (send_bytes = 0; send_bytes < len; send_bytes += cur_len) {
		if (len - send_bytes > PAGESIZE - sizeof (shm_block_t))
			cur_len = PAGESIZE - sizeof (shm_block_t);
		else
			cur_len = len - send_bytes;

		mb.length = cur_len + sizeof (shm_block_t);

		if (shmq_push (&(config_cache.bc_elem->sendq), &mb, (uint8_t*)(msg + send_bytes)) == -1)
			ERROR_RETURN (("message is lost: cmd %d", PROTO_TEST_ALIVE), -1);
	}
	fdsess->last_sent_tm = now.tv_sec;
	return 0;
}

int response_version(fdsession_t* fdsess)
{
	char msg[1024] = {0};
	protocol_t *proto = (protocol_t *)msg;

	struct shm_block mb;
	int send_bytes, cur_len;

	sprintf((char*)proto->body, "online-201206225");

	int len = 220;
	init_proto_head(msg, PROTO_VERSION, len);
	proto->id = 0;

	mb.id   = fdsess->id;
	mb.fd   = fdsess->fd;
	mb.type = DATA_BLOCK;

	for (send_bytes = 0; send_bytes < len; send_bytes += cur_len) {
		if (len - send_bytes > PAGESIZE - sizeof (shm_block_t))
			cur_len = PAGESIZE - sizeof (shm_block_t);
		else
			cur_len = len - send_bytes;

		mb.length = cur_len + sizeof (shm_block_t);

		if (shmq_push (&(config_cache.bc_elem->sendq), &mb, (uint8_t*)(msg + send_bytes)) == -1)
			ERROR_RETURN (("message is lost: cmd %d", PROTO_VERSION), -1);
	}
	fdsess->last_sent_tm = now.tv_sec;
	return 0;
}

int parse_protocol(uint8_t *data, int rcvlen, fdsession_t* fdsess)
{
	//send_request_to_anti_third_soft_server(17, data);

    struct timeval now;
    struct sockaddr_in cliaddr;
	gettimeofday(&now, 0);
	
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(fdsess->remote_port);
	cliaddr.sin_addr.s_addr = fdsess->remote_ip;
    send_cpkg_head_to_svr(config_get_strval("udp_third_soft_ip"), config_get_intval ("udp_third_soft_port", 0), 
        data, sizeof(protocol_t), &cliaddr, &now);
        
	protocol_t pkg;
	sprite_t *p, tmp;
	int i;
	i = 0;
	UNPKG_UINT32 (data, pkg.len, i);
	UNPKG_UINT8 (data, pkg.ver, i);
	UNPKG_UINT32 (data, pkg.cmd, i);
	UNPKG_UINT32 (data, pkg.id, i);
	UNPKG_UINT32 (data, pkg.ret, i);

	if(pkg.cmd == PROTO_VERSION) {
		return response_version(fdsess);
	}

//    DEBUG_LOG("20120626 recv: len=%d,ver=%d,cmd=%d,id=%d",pkg.len, pkg.ver, pkg.cmd, pkg.id);

	//client bug
	if (pkg.len < sizeof(pkg) || IS_NPC_ID(pkg.id))
		ERROR_RETURN (("recv: len=%d,ver=%d,cmd=%d,id=%d,err=%d",
			pkg.len, pkg.ver, pkg.cmd, pkg.id, pkg.ret), -1);

	if(pkg.cmd == PROTO_TEST_ALIVE) {
		return response_test_alive(fdsess);
	}

	p = get_sprite_by_fd(fdsess->fd);
	//this won't delete sprite if return error
	if ((pkg.cmd != PROTO_LOGIN && !p) || (pkg.cmd == PROTO_LOGIN && p)
			|| (p && p->id != pkg.id)) {
		ERROR_RETURN(("pkg error: fd=%d uid=%d cmd=%d p=%p", fdsess->fd, pkg.id, pkg.cmd, p), -1);
	}

	if (pkg.cmd == PROTO_LOGIN && pkg.ver != FIRST_VERSION) {
		ERROR_RETURN(("login cmd pkg version error: need: %u send: %u, cmd: %u, uid: %u",FIRST_VERSION, pkg.ver, pkg.cmd, pkg.id), -1);
	} else if (pkg.cmd != PROTO_LOGIN && p && !p->pend_flag) {
        uint32_t t_ver = 0;
        uint8_t crc_val = calc_crc8(data + sizeof (pkg), pkg.len - sizeof (pkg));
        t_ver = calc_ver_crc(pkg.len, p->ver, pkg.cmd, crc_val);
		p->ver = t_ver%256;
		if (p->ver != pkg.ver) {
			ERROR_RETURN(("pkg version error: %u %u %u %u", p->id, pkg.ver, p->ver, pkg.cmd), -1);
		}
	} else if(p && p->pend_flag) {
		p->pend_flag = 0;
	}

	if (p && !p->tiles && pkg.cmd != PROTO_LOGIN && pkg.cmd != PROTO_MAP_INFO &&
			pkg.cmd != PROTO_MAP_ENTER && pkg.cmd != PROTO_LOGOUT) {
		WARN_RETURN(("map error, id=%u, cmd=%d", pkg.id, pkg.cmd), 0);
		//ERROR_RETURN (("map error, id=%u, cmd=%d", pkg.id, pkg.cmd), -1);
	}

	if (pkg.cmd == PROTO_LOGIN) {
		sprite_t* old = get_sprite(pkg.id);
		if (old) notify_user_exit(old, -ERR_multi_login, 1);
		p = &tmp;
		memset(p, 0, sizeof(*p));
		p->id		= pkg.id;
		p->item_cnt	= 0;
		p->fd		= fdsess->fd;
		p->fdsess	= fdsess;

	}

#ifdef USE_CMD_QUEUE
	else if ( unlikely(p && p->waitcmd != 0) ) { //must have login success
		//for cmd queue
		if ( g_queue_get_length(p->pending_cmd_queue) > CMD_QUEUE_MAX_LENGTH ) { //queue full
			send_to_self_error(p, pkg.cmd, -ERR_system_busy, 0);
			DEBUG_LOG("CMD QUEUE FULL\t[wait for cmd=%d, id=%u, new cmd=%d]",
						p->waitcmd, p->id, pkg.cmd);
			return 0;
		} else { //add the cmd to queue's tail
			gpointer cmdbuf = g_slice_alloc(pkg.len + 4);
			*(uint32_t*)cmdbuf = pkg.len + 4;
			memcpy((char*)cmdbuf + 4, data, pkg.len);
			if ( g_queue_is_empty(p->pending_cmd_queue) ) {
				//add to busy sprite list
				list_add_tail(&(p->busy_sprite_list_hook), &g_busy_sprite_list);
			}
			// already in busy sprite list, then add to pending cmd queue
			g_queue_push_tail(p->pending_cmd_queue, cmdbuf);
			DEBUG_LOG("PENDING CMD\t[uid=%u, cmdid=%u, waitcmd=%u, cmdlen=%u]",
						p->id, pkg.cmd, p->waitcmd, pkg.len);
			return 0;
		}
	}
#endif
	return dispatch_protocol(p, pkg.cmd, data + sizeof (pkg), pkg.len - sizeof (pkg));
}

int send_msglog_to_bus(int cmd, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t msgbuf[PAGESIZE];

	msglog_proto_t *pkg;
	int len;

	len = sizeof (msglog_proto_t) + body_len;

	pkg = (msglog_proto_t *)msgbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	pkg->ver = 1;
	memcpy (pkg->body, body_buf, body_len);
	msglog(statistic_logfile, 0x00000001, get_now_tv()->tv_sec, pkg, pkg->len);
//	DEBUG_LOG("----len:%d--", pkg->len);
	return 0;
}

//for TW_VER
//转发前端发过来的msglog
int as_send_msglog_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 8);
	int j = 0;
	uint32_t msgid = 0;
	uint32_t type = 0;
	UNPKG_UINT32(body, msgid, j);
	UNPKG_UINT32(body, type, j);
	uint32_t log_buf[256];
	uint32_t log_len = 0;
	uint32_t right_type = 1;
	if (type == 1) {
		log_buf[log_len++] = 1;
		log_buf[log_len++] = p->id;
	} else {
		right_type = 0;
	}
	if (right_type) {
		msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, log_buf, log_len * sizeof(log_len));
	}

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}
