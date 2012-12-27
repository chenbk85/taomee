#include <sys/types.h>
#include <sys/time.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>

#include <libtaomee/log.h>
#include <libtaomee/crypt/qdes.h>

#include <ant/utils.h>
#include "net.h"
#include "shmq.h"
#include "proto.h"
#include "config.h"
#include "util.h"
#include "service.h"
#include "dll.h"
#include "timer.h"
#include "dbproxy.h"

#define DES_KEY             ">ta:mee<"
static uint8_t msg[4096];


void game_end_response (game_group_t* ggp, sprite_t* p, uint8_t reason)
{
	int i = sizeof(protocol_t);
	PKG_UINT8(msg, reason, i);
	PKG_UINT32(msg, p ? p->id : 0, i);
	init_proto_head(msg, proto_game_end, i);

	if ( reason != GER_player_offline )
		send_to_group(ggp, msg, i);
	else
		send_to_group_except_self(ggp, p->id, msg, i);
}

typedef struct gamesession {
//	time_t       tm1;
	uint32_t       tm1;
	int          gameid;
	gamegrpid_t  grpid;
	uint16_t     flag;
	uint16_t     posid;
//	time_t       tm2;
	uint32_t       tm2;
	uint32_t     uid;
//	in_addr_t    ip;
	uint32_t	 pet_id;
	uint32_t     itemid;
	uint32_t     no_used;
	uint32_t     exp;
	uint32_t     strength;
	uint32_t     iq;
	uint32_t     charm;
	uint32_t	 oltoday;
	uint32_t	 stamp;
	uint32_t     items[MAX_ITEMS_WITH_BODY];

} __attribute__((packed)) gamesession_t;

static inline int
auth_cmd(const sprite_t* p, const gamesession_t* gamesess)
{
#ifndef STANDALONE_GAMESERVER
//	int fd = (int)p->priv;
	// verify user id
	if ( gamesess->uid != p->id )
		ERROR_RETURN(("verify user id failed: id=%u %u", p->id, gamesess->uid), -1);
	// verify ip
//	if ( gamesess->ip != fds.cn[fd].remote_ip )
//		ERROR_RETURN(("verify remote address failed: ip=%X, %X", gamesess->ip, fds.cn[fd].remote_ip), -1);
	// verify time
	if ( gamesess->tm1 != gamesess->tm2 )
		ERROR_RETURN(("verify time failed: time=%d %d uid=%u", gamesess->tm1, gamesess->tm2, p->id), -1);

	// verify game id
	if ( (gamesess->gameid <= 0) || (gamesess->gameid >= GAMES_NUM_MAX) )
		ERROR_RETURN(("verify gameid failed: uid=%u gid=%d", p->id, gamesess->gameid), -1);

	// verify time
	time_t diff = now.tv_sec - gamesess->tm1;
	if (diff > 1800 || diff < -1800)
		ERROR_RETURN(("verify time failed: tm=%u %ld tmdiff=%lu uid=%u", gamesess->tm1, now.tv_sec, diff, p->id), -1);
#endif

	return add_sprite(p);
}

/* tell the group that the game start */
static inline void
notify_game_ready(sprite_t* p)
{
	game_group_t* grp = p->group;
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, grp->id, i);
	PKG_UINT32(msg, grp->game->id, i);
	PKG_UINT32(msg, grp->count, i);
	int j = 0;
	for (; j < grp->count; j++) {
		PKG_UINT32(msg, grp->players[j]->id, i);
	}
	init_proto_head(msg, proto_game_ready, i);
	if ( IS_GAME_PLAYER(p) ) send_to_group(grp, msg, i);
	else send_to_self(p, msg, i, 1);
}

static inline int
unpkg_gamesession(const uint8_t body[], int bodylen, gamesession_t* gamesess)
{
	int expectLen = sizeof (*gamesess);
	CHECK_BODY_LEN(bodylen, expectLen);

#ifndef STANDALONE_GAMESERVER
	des_decrypt( DES_KEY, body, &(gamesess->tm1) );
	des_decrypt( DES_KEY, (body + 8), &(gamesess->grpid) );
	des_decrypt( DES_KEY, (body + 16), &(gamesess->flag) );
	des_decrypt( DES_KEY, (body + 24), &(gamesess->uid) );
	des_decrypt( DES_KEY, (body + 32), &(gamesess->itemid) );
	des_decrypt( DES_KEY, (body + 40), &(gamesess->exp) );
	des_decrypt( DES_KEY, (body + 48), &(gamesess->iq) );
	des_decrypt( DES_KEY, (body + 56), &(gamesess->oltoday) );

	des_decrypt_n( DES_KEY, (body + 64), gamesess->items, 6);
	DEBUG_LOG("GAME SESSION\t GROUPID:%lu [%u %u %u %u %u]", gamesess->grpid, gamesess->uid, gamesess->iq, gamesess->oltoday, gamesess->items[0], gamesess->items[2]);
#else
	int i = 4;
	UNPKG_UINT32(body, gamesess->gameid, i);
	UNPKG_UINT64(body, gamesess->grpid, i);
	UNPKG_UINT32(body, gamesess->flag, i);
	i += 4;
	UNPKG_UINT32(body, gamesess->uid, i);
#endif

	return 0;
}

//  tell the player just entering the other players' info
static inline void
enter_game_rsp(sprite_t* p)
{
	int i = sizeof(protocol_t);
	game_group_t* gamegrp = p->group;
	PKG_UINT32(msg, gamegrp->game->id, i);
	PKG_UINT32(msg, IS_GAME_PLAYER(p), i);
	PKG_UINT32(msg, gamegrp->count, i);
	int idx = 0;
	while ( idx != gamegrp->count) {
		PKG_UINT32(msg, gamegrp->players[idx++]->id, i);
	}
	init_proto_head(msg, proto_player_enter, i);
	send_to_self(p, msg, i, 1);

	DEBUG_LOG( "JOIN GAME RESPONSE\t[userid: %u gameid: %d groupid: %lu]",
			p->id, gamegrp->game->id, gamegrp->id );
}

// notify all the players that a new player has entered the game
static inline void
notify_player_enter(sprite_t* p)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	init_proto_head(msg, proto_notify_player_entered, i);
	send_to_group_except_self(p->group, p->id, msg, i);
}

static inline void
notify_watcher_number(const game_group_t* grp)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, grp->nwatchers, i);
	init_proto_head(msg, proto_watcher_number, i);
	send_to_players(grp, msg, i);
}

static int add_player_cmd(sprite_t** pp, const uint8_t body[], int bodylen)
{
	gamesession_t gamesess;
#ifndef GAME_TEST

	if ( unpkg_gamesession(body, bodylen, &gamesess) == -1 ) {
		return -1;
	}

	// verify and add sprite
	if ( auth_cmd(*pp, &gamesess) == -1 ) {
		return -1;
	}
	*pp = get_sprite_by_fd((*pp)->priv);
	sprite_t *p = *pp;
	assert(p->group == 0);
	// Set Sprite Attr
	p->exp       = gamesess.exp;
	p->strength  = gamesess.strength;
	p->iq        = gamesess.iq;
	p->charm     = gamesess.charm;
	p->pet_id	 = gamesess.pet_id;
	p->itemid    = gamesess.itemid;
	p->oltoday = gamesess.oltoday;
	p->stamp = gamesess.stamp;

	for ( p->item_cnt = 0; p->item_cnt != MAX_ITEMS_WITH_BODY; ++(p->item_cnt) ) {
		if( gamesess.items[p->item_cnt] != 0 ) {
			p->items[p->item_cnt] = gamesess.items[p->item_cnt];
		} else {
			break;
		}
	}
//---------- temp ---------------------------------
//	if ( gamesess.flag & 1 ) {
//		p->status = gamesess.flag;
//------------------------------------------------
	if ( gamesess.flag & 1) {
		SET_GAME_PLAYER(p);
		p->pos_id = gamesess.posid;
	} else {
		SET_GAME_WATCHER(p);
	}

	if (gamesess.flag & (1<<7)) {
		SET_HAS_SUPER_LAMN(p);
	}
/*
	if (gamesess.flag & (1<<8)) {
		SET_PLAYEER_ENABLE_YUANBAO(p);
	}
	*/

#else
	add_sprite(*pp);
	*pp = get_sprite_by_fd((*pp)->priv);
	sprite_t *p = *pp;
	int i = 0;
	uint32_t grpid = 0;
	UNPKG_UINT32 (body, gamesess.gameid, i);
	UNPKG_UINT32(body, grpid, i);
	gamesess.grpid = grpid;
	SET_GAME_PLAYER(p);
#endif

	if ( join_game(p, gamesess.gameid, gamesess.grpid) == -1 ) {
		send_to_self_error(p, proto_player_enter, -ERR_OCCUPIED_POS, 1);
		// close connection if fail to join game
		return -1;
	}
	// send response to client if enter game successfully
	enter_game_rsp(p);
	// broadcast everyone that some one has joined the game group
	if ( IS_GAME_PLAYER(p) ) {
		notify_player_enter(p);
	}

	if ( GAME_WATCHABLE(p->group->game) ) {
		notify_watcher_number(p->group);
	}

	if ( GAME_READY(p->group) ) {
		// tell the group that the game is ready to start
		notify_game_ready(p);

		DEBUG_LOG( "GAME READY\t[userid: %u gameid: %d groupid: %lu]",
					p->id, p->group->game->id, p->group->id );

		if ( on_game_begin(p) == -1 ) {
			ERROR_LOG("Failed to Create Game");
			end_multiplayer_game(p->group, p, GER_game_system_err);
		}
	}
	return 0;
}

/**
 * @通知一个组的所有用户其中一个用户离开了
 * @param const game_group_t* grp  哪个组
 * @param const sprite_t* sp 哪个用户
 * @param uint8_t reason 离开的原因 @ref game_end_reason_t
 */
static inline void
notify_player_leave(const game_group_t* grp, const sprite_t* sp, uint8_t reason)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, sp->id, i);
	PKG_UINT8(msg, reason, i);
	init_proto_head(msg, proto_player_leave, i);
	if (reason != GER_timer_expire) {
		send_to_group_except_self(grp, sp->id, msg, i);
	} else {
		send_to_group(grp, msg, i);
	}

	TRACE_LOG("Notify Player Leave. uid=%u, grpid=%lu", sp->id, grp->id);
}

/**
 * @brief 当一个用户由于某种原因离开了游戏时调用
 * @param sprite_t* p 哪个用户
 * @param const uint8_t* body 什么原因
 * @param int bodylen body的长度，只能是0或1,否则无效
 * @return 0, 正常返回
 */
int leave_game_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	DEBUG_LOG("----------cmd: ---------");
	game_group_t* ggp = p->group;
	if ( !ggp ) ERROR_RETURN(("not in game, id=%u", p->id), -1);

	uint8_t reason;
	if ( bodylen == 0 ) {
		reason = GER_player_request;
		SET_SPRITE_ESCAPE(p);
	} else if ( bodylen == 1 ) {
		reason = body[0];
		if ( (!IS_SPRITE_OFFLINE(p) && (reason == GER_player_offline)) ||
			(!IS_SPRITE_TIMEOUT(p) && (reason == GER_timer_expire)) ) {
			reason = GER_invalid_data;
		}
	} else {
		reason = GER_invalid_data;
	}
	bodylen = 1;

	DEBUG_LOG("LEAVE GAME\t[uid=%u reason=%u grpid=%lu]", p->id, reason, ggp->id);

	if ( GAME_READY(ggp) ) {
		// Leave game when playing. No punishment currently.
		// > 0 for end multiplayer game, otherwise close only the peer's connection
		if ( on_game_data(p, proto_player_leave, body, bodylen) > 0 ) {
			end_multiplayer_game(ggp, p, reason);
		} else {
			// TODO - if it's needed to notify watcher leaves?
			uint8_t is_player = IS_GAME_PLAYER(p);
			//DEBUG_LOG("IS PLAYER [%d %d]", p->id, is_player);
			if ( is_player ) notify_player_leave(ggp, p, reason);
			// sprite is deleted and connection is also closed when
			// exit game group
			one_exit_group(p);
			if ( !is_player ) notify_watcher_number(ggp);
		}
	} else {
		// online redirects all the players within the same group at once,
		// so if any player leaves before game starting, end the game.
		end_multiplayer_game(ggp, p, reason);
	}

	return 0;
}

static inline void
reset_waitcmd(sprite_t* p)
{
	if ( p->waitcmd && p->waitcmd != proto_game_score
			&& p->waitcmd != proto_get_items && p->waitcmd != proto_card_add_win_lost && p->waitcmd != proto_card_getinfo && p->waitcmd != proto_set_redclothes && p->waitcmd != proto_new_card_get_card_info && p->waitcmd != proto_new_card_add_exp && p->waitcmd != proto_new_card_add_monster && p->waitcmd != proto_new_card_dec_item) {
		p->waitcmd = 0;
	}
}

static int dispatch_protocol(sprite_t *p, int cmd, const uint8_t *body, int len)
{
	int err = -1;

	if (p->waitcmd != 0) {
		send_to_self_error (p, cmd, -ERR_SYSTEM_BUSY, 0);
		ERROR_RETURN(("wait for cmd=%d, id=%u, new cmd=%d", p->waitcmd, p->id, cmd), 0);
	}

	p->waitcmd = cmd;
	switch (cmd) {
	case proto_player_enter:
		err = add_player_cmd(&p, body, len); break;
	case proto_player_leave:
		err = leave_game_cmd(p, body, len); break;
	default:
		if ( p->group ) {
			if ( GAME_READY(p->group) ) {
				// > 0 for end multiplayer game, -1 for close only the peer's connection
				//err = p->group->game->on_game_data(p, cmd, body, len);
				err = on_game_data(p, cmd, body, len);
				if ( err > 0 ) {
					end_multiplayer_game(p->group, p, err);
					err = 0;
				} else if ( err < 0 ) {
					uint8_t reason = GER_invalid_data;
					err = leave_game_cmd(p, &reason, 1);
				}
			} else {
				uint8_t reason = GER_invalid_data;
				err = leave_game_cmd(p, &reason, 1);
			}
		} else {
			ERROR_LOG("error cmd=%d, id=%u", cmd, p->id);
			err = -1;
		}
		break;
	}

	reset_waitcmd(p);
	//if error, connection will be closed
	return err;
}

int response_version( int fd )
{
	char msg[1024] = {0};
	protocol_t *proto = (protocol_t *)msg;

	struct shm_block mb;
	int send_bytes, cur_len;

	sprintf((char*)proto->body, "GameServer-201203301");
	int len = 220;
	init_proto_head(msg, proto_version, len);
	proto->id = 0;

	mb.id	= fds.cn[fd].id;
	mb.fd	= fd;
	mb.type	= DATA_BLOCK;

	for (send_bytes = 0; send_bytes < len; send_bytes += cur_len) {
		if (len - send_bytes > PAGESIZE - sizeof (shm_block_t))
			cur_len = PAGESIZE - sizeof (shm_block_t);
		else
			cur_len = len - send_bytes;

		mb.length = cur_len + sizeof (shm_block_t);

		if (shmq_push (&sendq, &mb, (uint8_t*)(msg + send_bytes)) == -1)
			ERROR_RETURN (("message is lost: cmd %d", proto_version), -1);
	}
	return 0;
}

int parse_protocol(const uint8_t *data, int rcvlen, int fd)
{
	protocol_t pkg;
	sprite_t *p, tmp;
	int i;

	i = 0;
	UNPKG_UINT32 (data, pkg.len, i);
	UNPKG_UINT8 (data, pkg.ver, i);
	UNPKG_UINT32 (data, pkg.cmd, i);
	UNPKG_UINT32 (data, pkg.id, i);
	UNPKG_UINT32 (data, pkg.ret, i);

	
	//非法的ID
	if( 0 == pkg.id )
	{
		return 0;
	}
	// obtain sprite if exist
	p = get_sprite_by_fd(fd);
	//client bug
//	if (pkg.ver != 0x01 || pkg.ret != 0 || pkg.len < sizeof (pkg))
	if ( pkg.len < sizeof (pkg)) {
		ERROR_LOG("recv: len=%d,ver=%d,cmd=%d,id=%d,err=%d",
				pkg.len, pkg.ver, pkg.cmd, pkg.id, pkg.ret);
		if (p) {
			handle_invalid_data(p, GER_invalid_data);
			return 0;
		}
		return -1;
	}
	if( pkg.cmd == proto_version ) {
		return response_version(fd);
	}

	if(pkg.cmd == proto_test_alive) {
		return response_test_alive(fd);
	}

	// if not yet logined or fd already used or userid not equal
	if ((pkg.cmd != proto_player_enter && !p) || (pkg.cmd == proto_player_enter && p)
			|| (p && p->id != pkg.id)) {
		ERROR_LOG("online status error, fd=%d,uid=%d,cmd=%d,p=%p",
				fd, pkg.id, pkg.cmd, p);
		if (p) {
			handle_invalid_data(p, GER_invalid_data);
			return 0;
		}
		return -1;
	}
	// if new player enters game
	if ( pkg.cmd == proto_player_enter ) {
		// check if multiple login
		sprite_t* old = get_sprite(pkg.id);
		if ( old ) {
			ERROR_LOG("multiple login, id=%u cmd:%u", pkg.id, pkg.cmd);
			handle_invalid_data(old, GER_multi_login);
		}
		p = &tmp;
		memset( p, 0, sizeof(*p) );
		p->id = pkg.id;
		p->priv = fd;
	}
	
	return dispatch_protocol( p, pkg.cmd, data + sizeof(pkg), pkg.len - sizeof(pkg) );
}

