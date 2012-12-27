#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>

#include <glib.h>

#include <statistic_agent/msglog.h>

#include "central_online.h"
#include "communicator.h"
#include "sprite.h"
#include "proto.h"
#include "dll.h"
#include "logic.h"
#include "dbproxy.h"
#include "shmq.h"
#include "daemon.h"
#include "npc.h"
#include "small_require.h"
#include "mole_doctor.h"
#include "mole_pasture.h"
#include "mole_candy.h"
#include "thanksgiving.h"
#include "im_message.h"
#include "snowball_war.h"
#include <openssl/md5.h>
#include "hide_and_seek.h"
#include "year_feast.h"
#include "mole_cake.h"

static GHashTable*	all_sprites;
list_head_t	idslots[HASH_SLOT_NUM];
uint32_t		sprites_count;

//list head of the sprites who has pending cmds
#ifdef USE_CMD_QUEUE
list_head_t g_busy_sprite_list;
#endif

#ifdef USE_CMD_QUEUE
static void free_cmd_queue_elem(gpointer data, gpointer user_data);
#endif

void traverse_sprites(void (*action)(void* key, void* spri, void* userdata), void* data)
{
	g_hash_table_foreach(all_sprites, action, data);
}

void traverse_sprites_remove(gboolean (*action)(gpointer key, gpointer spri, gpointer userdata), void* data)
{
	g_hash_table_foreach_remove(all_sprites, action, data);
}

static inline void
do_free_sprite(void* p)
{
	g_slice_free1(SPRITE_STRUCT_LEN, p);
	--sprites_count;
}

void init_sprites()
{
	int i;

	all_sprites = g_hash_table_new_full(g_int_hash, g_int_equal, 0, do_free_sprite);
	sprites_count = 0;

	for (i = 0; i < HASH_SLOT_NUM; i++)
		INIT_LIST_HEAD (&idslots[i]);

#ifdef USE_CMD_QUEUE
	INIT_LIST_HEAD (&g_busy_sprite_list);
#endif
}

void fini_sprites()
{
	g_hash_table_destroy(all_sprites);
	destroy_npc_system();
}

sprite_t* get_sprite_from_gamegrp(uint32_t id, const struct game_group* grp)
{
	uint8_t i = 0;
	for (; i != grp->count; ++i) {
		if (grp->players[i]->id == id) {
			return grp->players[i];
		}
	}
	return 0;
}


sprite_t* get_sprite_by_fd(int fd)
{
	sprite_t* p = g_hash_table_lookup(all_sprites, &fd);
	if ( !p || IS_NPC_ID(p->id) ) {
		return 0;
	}
	return p;
}

static inline void
free_sprite(sprite_t *p, int del_node)
{
	if (del_node) {
		g_hash_table_remove(all_sprites, &(p->fd));
	}
}

static inline sprite_t*
alloc_sprite(int fd)
{
	sprite_t* p = g_slice_alloc(SPRITE_STRUCT_LEN);

	p->fd = fd;
	g_hash_table_insert(all_sprites, &(p->fd), p);
	++sprites_count;

	return p;
}

sprite_t* add_sprite(sprite_t* v)
{
	sprite_t* p = alloc_sprite(v->fd);
	*p = *v;

	p->stamp = now.tv_sec;
	INIT_LIST_HEAD(&p->hash_list);
	INIT_LIST_HEAD(&p->map_list);
	INIT_LIST_HEAD(&p->timer_list);
	INIT_LIST_HEAD(&p->hero_list);

#ifdef USE_CMD_QUEUE
	//make the hook point to it self, convenient to judge empty or not
	INIT_LIST_HEAD(&p->busy_sprite_list_hook);

	//alloc a pending cmd queue head
	p->pending_cmd_queue = g_queue_new(); // must succ, or program dies
#endif

	//guest login
	if (p->id == 0) {
		static uint32_t	guest_baseid = GUEST_SEQ_NUM;
		p->id = ++guest_baseid;
		if (!(p->id)) {
			p->id = guest_baseid = GUEST_SEQ_NUM + 1;
		}

		p->login = 1;
		p->birth = now.tv_sec;
		p->color = NPC_COLOR_GREY;
		p->item_cnt = 0;
#ifdef TW_VER
		sprintf(p->nick, "遊客-%d", p->id - GUEST_SEQ_NUM);
#else
		sprintf(p->nick, "游客-%d", p->id - GUEST_SEQ_NUM);
#endif
	}

	list_add_tail(&p->hash_list, &idslots[p->id % HASH_SLOT_NUM]);
	return p;
}

// notify mother user login info
static void notify_mother_user_info(sprite_t* p)
{
	//uint32_t buff[3] = { 0, p->stamp, get_now_tv()->tv_sec };
	//send_request_to_db(SVR_PROTO_ADD_LOGIN_INFO, 0, 12, buff, p->id);

    typedef struct _add_login_info {
        uint16_t channel_id;
        char verify_info[32];
        uint32_t gameid;
        uint32_t login_time;
        uint32_t logout_time;
    } __attribute__ ((packed)) add_login_info_t;

    add_login_info_t login_info_req;
    uint32_t game_id = 0;

	unsigned char src[100];
	unsigned char md_out[16], md[33];
	uint16_t channel_id = config_get_intval("channel_id", 0);
    uint32_t security_code = config_get_intval("security_code", 0);
    DEBUG_LOG("userid %d channel_id %d security_code %d", p->id, channel_id, security_code);

    login_info_req.channel_id = channel_id;
    login_info_req.gameid = game_id;
    login_info_req.login_time = p->stamp;
    login_info_req.logout_time = get_now_tv()->tv_sec;

	int len = snprintf((char*)src, sizeof src, "channelId=%d&securityCode=%u&data=", channel_id, security_code);
    *(uint32_t*)(src + len) = login_info_req.gameid;
    *(uint32_t*)(src + len + 4) = login_info_req.login_time;
    *(uint32_t*)(src + len + 8) = login_info_req.logout_time;
    MD5(src, len + 12, md_out);
    int i;
    for (i = 0; i != 16; ++i) {
        sprintf((char*)md + i * 2, "%.2x", md_out[i]);
    }

    memcpy(login_info_req.verify_info, md, 32);
    send_request_to_db(SVR_PROTO_ADD_LOGIN_INFO, 0, sizeof(login_info_req), &login_info_req, p->id);
    return ;

}

int del_sprite(sprite_t *p, int update, int del_node)
{
	DEBUG_LOG("DEL USER\t[uid=%u flg=0x%x fd=%d]", p->id, p->flag, p->fd);
	clear_array(p->id);
	clear_rabit_pos(p->id);
	clean_booth(p->id);

	cheer_team_clear_user_info(p);

	kick_off_dacing_user(p);
	year_feast_kill_user(p);
	list_del_init(&p->hero_list);

	rm_usrpets_when_logout(p);

//	clear hide and seek info
	has_clear_my_info(p);

	REMOVE_TIMERS(p);
	if (p->group) {
		uint8_t reason = LEAVE_GAME_BY_OFFLINE;
		leave_game_cmd(p, &reason, 1);
	}

	//set_all_pets_attr(p->id);
	leave_map(p, 0);
	send_login_exit_info(p->id, 1, config_cache.bc_elem->online_id, get_now_tv()->tv_sec);
	notify_user_login(p, 0);
	notify_mother_user_info(p);

	//set db guider state
	if (p->sg_guidering_flag == 1) {
		DEBUG_LOG("db clear guider state: [%u]", p->id);
		uint32_t buf[]={50008};
	    send_request_to_db(SVR_PROTO_CLEAN_STH_DONE, NULL, sizeof(buf), &buf, p->id);
	}

	if (update && p->login && !IS_GUEST_ID(p->id) && !IS_NPC_ID(p->id)) {
		int tmp = now.tv_sec - p->stamp;
		if (tmp > 0) {
			p->waitcmd = PROTO_LOGOUT;

			uint32_t buf[2];
			buf[0] = tmp;
			buf[1] = get_ip2(p);
			db_set_stamp_ip(0, buf, p->id);
		} else if (tmp < 0) {
			ERROR_LOG("Server stamp error,  login time=%u, id=%u",
						 p->stamp, p->id);
		}
	}

	list_del(&p->hash_list);

#ifdef USE_CMD_QUEUE
	//for cmd queue
	if ( ! g_queue_is_empty(p->pending_cmd_queue) ) {
		g_queue_foreach(p->pending_cmd_queue, free_cmd_queue_elem, NULL);
	}
	g_queue_free(p->pending_cmd_queue);
	list_del_init(&p->busy_sprite_list_hook);
#endif
	//send_im_pkg_user_quit(p);
	free_sprite(p, del_node);
	return 0;
}

int del_sprite_by_fd(int fd, int update, int del_node)
{
	sprite_t* p = get_sprite_by_fd(fd);
	if ( !p ) {
		return -1;
	}

	return del_sprite(p, update, del_node);
}

void send_varpkg(void* key, void* spri, void* userdata)
{
	var_pkg_t* vpkg = userdata;

	if (vpkg->idle_time < 0) {
		send_to_self(spri, vpkg->pkg, vpkg->pkg_len, 0);
	} else {
		sprite_t* p = spri;
		const struct timeval* tv = get_now_tv();
		if ( (tv->tv_sec - p->fdsess->last_sent_tm) >= vpkg->idle_time ) {
			send_to_self(spri, vpkg->pkg, vpkg->pkg_len, 0);
		}
	}
}

//--------------------------------------------------------
//
/**
 * pack_dress - pack current dress of @p to @buf
 * @p: pointer of type sprite_t
 * @buf: place to pack the dress of @p
 * @dress_up: indicate if the dress is to be dressed up or taken off
 *
 * returns lenght of the package.
 */
static inline int
pack_dress(sprite_t* p, uint8_t* buf, int dress_up)
{
	int i, len = 0;
	for (i = 0; i != p->item_cnt; ++i) {
		PKG_UINT32(buf, p->items[i], len);
		PKG_UINT8(buf, dress_up, len);
	}

	return len;
}

/**
 * chg_dress - Chg @p's current dress to @dress. Used only by NPC currently
 * @p: pointer of type sprite_t
 * @dress: dress to dress up
 * @cnt: count of the dress to dress up
 *
 */
void chg_dress(sprite_t* p, uint32_t dress[], int cnt)
{
	int len = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, len);
	PKG_UINT32(msg, p->item_cnt + cnt, len);

	len += pack_dress(p, msg + len, 0);
	memcpy(p->items, dress, cnt * sizeof(dress[0]));
	p->item_cnt = cnt;
	len += pack_dress(p, msg + len, 1);

	init_proto_head(msg, PROTO_USER_ITEM_USE, len);
	send_to_map(p, msg, len, 0);
}
//--------------------------------------------------------

//
int sub_sprite_attr(sprite_t* p, int type, int cnt)
{
	uint32_t attr = 0;

#define SUB_ATTR(attr_, cnt_) \
		do { \
			if ( (cnt_) > (attr_) ) { \
				attr = (attr_); \
				break; \
			} \
			(attr_) -= cnt; \
			return 0; \
		} while (0)

	switch (type) {
	case 0:
		SUB_ATTR(p->yxb, cnt);
	case 1:
		SUB_ATTR(p->exp, cnt);
	case 2:
		SUB_ATTR(p->strong, cnt);
	case 3:
		SUB_ATTR(p->iq, cnt);
	case 4:
		SUB_ATTR(p->lovely, cnt);
	}

	ERROR_RETURN( ("attr %d not enough for sub. uid=%u has=%u sub=%d", type, p->id, attr, cnt), -1 );

#undef SUB_ATTR
}

void notify_5basic_attr(sprite_t* p)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->yxb, i);
	PKG_UINT32(msg, p->exp, i);
	PKG_UINT32(msg, p->strong, i);
	PKG_UINT32(msg, p->iq, i);
	PKG_UINT32(msg, p->lovely, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_self(p, msg, i, 1);
}

//
static inline gboolean
do_kick_user(gpointer key, gpointer spri, gpointer userdata)
{
	notify_user_exit(spri, (long)userdata, 0);
	return TRUE;
}
//
int kick_all_users_offline(void* owner, void* data)
{
	traverse_sprites_remove(do_kick_user, data);
	remove_all_across_svr_sprites();
	//send_to_central_online(COCMD_kick_all_offline, 0, 0, 0, 0);
	DEBUG_RETURN( ("KICK ALL USERS OFFLINE"), 0 );
}

int select_sprite(sprite_t* p, sprite_t** val)
{
	int i, j, selected_player;

	uint32_t base = sprites_count;
	if (base > 20)
		base = 20;

	j = 0;
	selected_player = rand() % base;
	sprite_t* tmp;
	for (i = 0; i < HASH_SLOT_NUM; i++){
		list_for_each_entry (tmp, &idslots[i], hash_list) {
			j++;
			if(j >= selected_player && p->id != tmp->id && !IS_GUEST_ID(tmp->id)){
				*val = tmp;
				return 0;
			}
		}
	}
	*val = 0;
	return -1;
}

#ifdef USE_CMD_QUEUE
//for cmd queue free
void free_cmd_queue_elem(gpointer data, gpointer user_data)
{
	uint32_t len = *(uint32_t*)data;
	if ( likely(data != NULL) ) {
		g_slice_free1(len, data);
	}
}
#endif

// no used now
void update_sprite_op(uint32_t opid, const void* body, int len)
{
	CHECK_BODY_LEN_VOID(len, 12);

	uint32_t uid = *(uint32_t*)(body);
	sprite_t* p = get_sprite(uid);
    if (p) {
        //p->tmpinfo.tangguo_num++;
        //DEBUG_LOG("UPDATE USER\t[%u %u]", uid, p->tmpinfo.tangguo_num);
    }
}

int mole_sprite_op(uint32_t uid, void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 12);

	communicator_body_t* cbody = buf;

#define DISPATCH_OP(op_, func_) \
		case (op_): (func_)(op_, cbody->body, len - 12); break

	switch (cbody->opid) {

		DISPATCH_OP(PROTO_CATCH_PLAYER_BY_WORD, update_sprite_op);
	default:
		break;
	}
#undef DISPATCH_OP

	return 0;
}


int cal_mole_level(uint32_t mole_exp)
{
	int lvl = 1;
	int num = 15;
	uint32_t exp_tmp = 108900;

	if(mole_exp > exp_tmp) {
		lvl = 120;
		while(mole_exp >= exp_tmp && lvl <= 200) {
			lvl++;
			exp_tmp += (lvl - 105) * lvl;
		}
	} else {
		while(mole_exp >= (uint32_t)lvl * num) {
		mole_exp -= lvl * num;
		lvl++;
		}
	}
	return lvl - 1;
}

