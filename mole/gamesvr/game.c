#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <libtaomee/log.h>
#include <libtaomee/crypt/qdes.h>

#include <ant/utils.h>
#include <ant/bitmanip/bitmanip.h>

#include "dbproxy.h"
#include "proto.h"
#include "service.h"
#include "timer.h"
#include "config.h"
#include "game.h"

#ifndef TW_VER
#include  <statistic_agent/msglog.h>
#endif
char *statistic_file;

typedef struct game_bonus {
    int32_t     exp;
    int32_t     strong;
    int32_t     iq;
    int32_t     charm;
    int32_t     xiaomee;
    uint32_t    reason;
    uint32_t    detailed_reason;
} __attribute__((packed)) game_bonus_t;

/**
 * @class game_score_pkg
 * @brief 保存游戏结果的包结构
 */
typedef struct game_score_pkg
{
	protocol_t header;
	uint16_t gid, ///< 游戏组编号
	  rank; ///< 游戏中所得的分数
	uint32_t strong, ///< 力量值
	  iq,
	  lovely, ///< 亲密度
	  exp, ///< 经验值
	  score,
	  time,
	  coins,
	  itmid;
	uint8_t sess[24];
} __attribute__ ((packed)) game_score_pkg_t;

#ifndef TW_VER
typedef struct statistic {
	int32_t		coins;
	int32_t		exp;
    int32_t     strong;
    int32_t     iq;
    int32_t     charm;
	int			count;
	uint32_t	user_id;
} __attribute__((packed)) statistic_info_t;
#endif

static uint8_t gbuf[4096];

int
db_get_item(sprite_t* p, const game_score_t* gs)
{
	int i = 0;
	PKG_H_UINT32(gbuf, gs->itmkind, i);
	PKG_H_UINT32(gbuf, gs->itmid, i);
	PKG_H_UINT32(gbuf, 1, i);
	PKG_H_UINT32(gbuf, gs->itm_max, i);
	PKG_H_UINT32(gbuf, 0, i);

	return send_request_to_db(db_obtain_item, p, i, gbuf, p->id);
}

int
db_get_item_cnt(sprite_t* p, const game_score_t* gs, int cnt)
{
	int i = 0;
	PKG_H_UINT32(gbuf, 0, i);
	PKG_H_UINT32(gbuf, gs->itmid, i);
	PKG_H_UINT32(gbuf, cnt, i);
	PKG_H_UINT32(gbuf, gs->itm_max, i);
	PKG_H_UINT32(gbuf, 0, i);

	return send_request_to_db(db_obtain_item, p, i, gbuf, p->id);
}
int
db_get_pet_item(sprite_t* p, const game_score_t* gs)
{
	int i = 0;
	PKG_H_UINT32(gbuf, gs->pet_id, i);
	PKG_H_UINT32(gbuf, gs->itmid, i);
	PKG_H_UINT32(gbuf, 1, i);
	PKG_H_UINT32(gbuf, gs->itm_max, i);
	PKG_H_UINT32(gbuf, 0, i);

	return send_request_to_db(db_obtain_pet_item, p, i, gbuf, p->id);
}

int
db_get_items(sprite_t* p, const game_items_t* gt)
{

	uint32_t oldcount = 0;
	uint32_t item_count= 1;
	uint32_t item_max = 1;
	uint32_t newcount = gt->newcount;
	//only can add 10 items one time
	assert(newcount <= 10);
	int i;
	int store_l = 0;
	//store items id
    PKG_UINT8(p->session, (uint8_t)gt->flag, store_l);
    PKG_UINT8(p->session, (uint8_t)newcount, store_l);
	for(i = 0; i < newcount; i++) {
		PKG_H_UINT32(p->session, gt->itm_id[i], store_l);
	}

	//get unique items
	int length = 0;
	int reason = 0;
	PKG_H_UINT32(gbuf, oldcount, length );
	PKG_H_UINT32(gbuf, newcount, length );
	//reason 1 = 0;
	PKG_H_UINT32(gbuf, reason, length );
	//reason 2 = 0;
	PKG_H_UINT32(gbuf, reason, length );

	for(i = 0; i < newcount; i++) {
		PKG_H_UINT32(gbuf, gt->itm_id[i],length );
		PKG_H_UINT32(gbuf, item_count, length );
		PKG_H_UINT32(gbuf, item_max, length );
	}
	p->waitcmd = proto_get_items;
    DEBUG_LOG("SEND REQ GET ITEMS %d %d", p->id, p->waitcmd);
	return send_request_to_db(db_obtain_items, p, length , gbuf, p->id);
}

int
db_get_items_max(sprite_t* p, const game_items_t* gt, uint32_t item_max)
{

	uint32_t oldcount = 0;
	uint32_t item_count= 1;
//	uint32_t item_max = 1;
	uint32_t newcount = gt->newcount;
	//only can add 10 items one time
	assert(newcount <= 10);
	int i;
	int store_l = 0;
	//store items id
    PKG_UINT8(p->session, (uint8_t)gt->flag, store_l);
    PKG_UINT8(p->session, (uint8_t)newcount, store_l);
	for(i = 0; i < newcount; i++) {
		PKG_H_UINT32(p->session, gt->itm_id[i], store_l);
	}

	//get unique items
	int length = 0;
	int reason = 0;
	PKG_H_UINT32(gbuf, oldcount, length );
	PKG_H_UINT32(gbuf, newcount, length );
	//reason 1 = 0;
	PKG_H_UINT32(gbuf, reason, length );
	//reason 2 = 0;
	PKG_H_UINT32(gbuf, reason, length );

	for(i = 0; i < newcount; i++) {
		PKG_H_UINT32(gbuf, gt->itm_id[i],length );
		PKG_H_UINT32(gbuf, item_count, length );
		PKG_H_UINT32(gbuf, item_max, length );
	}
	p->waitcmd = proto_get_items;
    DEBUG_LOG("SEND REQ GET ITEMS %d %d", p->id, p->waitcmd);
	return send_request_to_db(db_obtain_items, p, length , gbuf, p->id);
}

int db_get_items_without_return(sprite_t* p, const game_items_t* gt)
{

	uint32_t oldcount = 0;
	uint32_t item_count= 1;
	uint32_t item_max = 1;
	uint32_t newcount = gt->newcount;
	//only can add 10 items one time
	assert(newcount <= 10);
	int i;
	int store_l = 0;
	//store items id
    PKG_UINT8(p->session, (uint8_t)gt->flag, store_l);
    PKG_UINT8(p->session, (uint8_t)newcount, store_l);
	for(i = 0; i < newcount; i++) {
		PKG_H_UINT32(p->session, gt->itm_id[i], store_l);
	}

	//get unique items
	int length = 0;
	int reason = 0;
	PKG_H_UINT32(gbuf, oldcount, length );
	PKG_H_UINT32(gbuf, newcount, length );
	//reason 1 = 0;
	PKG_H_UINT32(gbuf, reason, length );
	//reason 2 = 0;
	PKG_H_UINT32(gbuf, reason, length );

	for(i = 0; i < newcount; i++) {
		PKG_H_UINT32(gbuf, gt->itm_id[i],length );
		PKG_H_UINT32(gbuf, item_count, length );
		PKG_H_UINT32(gbuf, item_max, length );
	}
	p->waitcmd = proto_get_items;
    DEBUG_LOG("SEND REQ GET ITEMS %d %d", p->id, p->waitcmd);
	return send_request_to_db(db_obtain_items, NULL, length , gbuf, p->id);
}



static inline int
sprite_attr_unchanged(const game_score_pkg_t* score)
{
	return (!score->exp && !score->iq && !score->coins && !score->lovely && !score->strong);
}

static inline void
set_game_bonus(game_bonus_t* gbonus, const game_score_pkg_t* score)
{
	gbonus->charm   = score->lovely;
	gbonus->exp     = score->exp;
	gbonus->iq      = score->iq;
	gbonus->strong  = score->strong;
	gbonus->xiaomee = score->coins;
	gbonus->reason  = 1001;        // attr changed coz of playing game
	gbonus->detailed_reason = score->gid; // game id
}

static inline int
db_upd_game_attr(sprite_t* p, const game_score_pkg_t* score)
{
	set_game_bonus( (void*)&gbuf, score );

	if ( sprite_attr_unchanged(score) ) {
		return upd_game_attr_callback(p, p->id, gbuf, 20, 0);
	}

	return send_request_to_db(db_change_game_attr, p, sizeof(game_bonus_t), gbuf, p->id);
}
#ifdef TW_VER
#define TWO_HOUR_SEC	100000
#define FIVE_HOUR_SEC	200000
#else
#define TWO_HOUR_SEC	7200
#define FIVE_HOUR_SEC	18000
#endif

static inline void
pack_game_score(sprite_t* p, const game_score_t* score)
{
	//const struct tm* cur_time = get_now_tm();
	int modulus = 100;

	int flag = 0;

	//if (cur_time->tm_wday == 5 || cur_time->tm_wday == 6 || cur_time->tm_wday == 0)
	{
		flag = 1;
	}

	if (flag) {
		modulus = modulus * 2;
	}

	if ((get_now_tv()->tv_sec - p->stamp + p->oltoday) >= TWO_HOUR_SEC)
	{
		if (flag)
		{
			modulus = modulus / 2;
		}
		else
		{
			modulus = 0;
		}
	}

	if ((get_now_tv()->tv_sec - p->stamp + p->oltoday) >= FIVE_HOUR_SEC) {
		modulus = 0;
	}

	int i = sizeof(protocol_t);
	const game_t* game = p->group->game;

	PKG_H_UINT16(p->db_data, game->id, i);
	PKG_H_UINT16(p->db_data, score->rank, i);
	PKG_H_UINT32(p->db_data, score->strong * modulus / 100, i);
	PKG_H_UINT32(p->db_data, score->iq * modulus / 100, i);
	PKG_H_UINT32(p->db_data, score->lovely * modulus / 100, i);
	PKG_H_UINT32(p->db_data, score->exp * modulus / 100, i);
	PKG_H_UINT32(p->db_data, score->score, i);
	PKG_H_UINT32(p->db_data, score->time, i);
	PKG_H_UINT32(p->db_data, score->coins * modulus / 100, i);
	PKG_UINT32(p->db_data, score->itmid, i);
	PKG_STR(p->db_data, score->sess, i, sizeof score->sess);

    DEBUG_LOG("gameid:%d, exp:%d, xiaomee:%d", game->id, score->exp, score->coins);

	init_proto_head(p->db_data, proto_game_score, i);
    //if(p->waitcmd != proto_set_redclothes && p->waitcmd != proto_get_items && p->waitcmd != proto_card_add_win_lost) {
    //    p->waitcmd = proto_game_score;
    //}
    //if(p->group->game->id == 54 || p->group->game->id == 40) {
    //    p->waitcmd = proto_game_score;
    //}
    p->waitcmd = proto_game_score;
}

// session = time + ip + time + score + userid + gameid
void pack_score_session(const sprite_t* p, game_score_t* gs, uint32_t gid, uint32_t score)
{
#ifndef TW_VER
	static char des_key[] = ">ta:mee<";
#else
	static char des_key[] = ",.ta0me>";
#endif

	int i = 0;
	char outbuf[8], inbuf[8];

	// time + ip
	memcpy(inbuf, &now, 4); // time
	memcpy(inbuf + 4, &fds.cn[sprite_fd(p)].remote_ip, 4); // ip
	des_encrypt(des_key, inbuf, outbuf);
	PKG_STR(gs->sess, outbuf, i, sizeof outbuf);
	// time + score
	memcpy(inbuf + 4, &score, 4); // score
	des_encrypt(des_key, inbuf, outbuf);
	PKG_STR(gs->sess, outbuf, i, sizeof outbuf);
	// userid + gameid
	memcpy(inbuf, &p->id, 4);    // uid
	memcpy(inbuf + 4, &gid, 4);  // game id
	des_encrypt(des_key, inbuf, outbuf);
	PKG_STR(gs->sess, outbuf, i, sizeof outbuf);
}

// submit game score to db
// update sprite's game attribute
int submit_game_score(sprite_t* p, game_score_t* score)
{
	//double score when Sat&Sun&Fri
//	if(tm_cur.tm_wday == 0 || tm_cur.tm_wday == 5 || tm_cur.tm_wday == 6) {
//        if(p->group->game->id != 39
//                && p->group->game->id != 40
//                && p->group->game->id != 53
//                && p->group->game->id != 54)
//		score->coins = 2 * score->coins;
//	}
	if ( sprite_fd(p) == -1 )
		ERROR_RETURN(("error fd=%d, id=%u", -1, p->id), -1);

	pack_game_score(p, score);
	if (score->itmid == 0) {
		return get_item_callback(p, p->id, 0, 4, 0);
	}
	if (score->itmkind == 11) {
		DEBUG_LOG("GET ITEM\t[id=%u itmid=%u itmkind=%d itm_max=%u]", p->id, score->itmid, score->itmkind, score->itm_max);
		return db_get_pet_item(p, score);
	}
	if (score->itmkind == 12) {
		DEBUG_LOG("GET ITEM\t[id=%u itmid=%u itmkind=%d itm_max=%u]", p->id, score->itmid, score->itmkind, score->itm_max);
		return db_get_item_cnt(p, score, 5);
	}
	/*
	else if ((score->itmid >100) && (score->itmid < 103)) {
		DEBUG_LOG("GET YUANBAO\t[id=%u yuanbao_id=%u]", p->id, score->itmid);
		return db_add_yuanbao(p, score->itmid);
	}
	*/
	//else if ( (score->itmid > 100) && (score->itmid < 110) ) {
		//DEBUG_LOG("GET FIRE MEDAL\t[id=%u medal_id=%u]", p->id, score->itmid);
		//return db_add_fire_medal(p, score->itmid);
	//}

	DEBUG_LOG("GET ITEM\t[id=%u itmid=%u itmkind=%d itm_max=%u]", p->id, score->itmid, score->itmkind, score->itm_max);
	return db_get_item(p, score);
}

int get_item_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret)
{
	game_score_pkg_t* gsp = (void*)p->db_data;
	if (ret) {
		gsp->itmid = 0;
	}

	if (db_upd_game_attr(p, gsp) == -1 ) {
		p->waitcmd = 0;
		ERROR_RETURN(("update game attr error: uid=%u gameid=%u", p->id, p->group->game->id), -1);
	}

	DEBUG_LOG("GAME SCORE\t[id=%u game_id=%d rank=%d score=%d time=%u coins=%d iq=%d strong=%d exp=%d charm=%d]",
			p->id, gsp->gid, gsp->rank, gsp->score, gsp->time, gsp->coins, gsp->iq, gsp->strong, gsp->exp, gsp->lovely);
	return 0;
}

int get_pet_item_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret)
{
	game_score_pkg_t* gsp = (void*)p->db_data;
	if (ret) {
		DEBUG_LOG("%d get pet item error %d", p->id, ret);
		gsp->itmid = 0;
	}

	if (db_upd_game_attr(p, gsp) == -1 ) {
		p->waitcmd = 0;
		ERROR_RETURN(("update game attr error: uid=%u gameid=%u", p->id, p->group->game->id), -1);
	}

	DEBUG_LOG("GAME SCORE\t[id=%u game_id %d rank=%d score=%d time=%u coins=%d iq=%d strong=%d exp=%d charm=%d]",
			p->id, gsp->gid, gsp->rank, gsp->score, gsp->time, gsp->coins, gsp->iq, gsp->strong, gsp->exp, gsp->lovely);
	return 0;
}

int get_items_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret)
{
    DEBUG_LOG("%d get items callback %d", p->id, ret);
    switch (ret) {
        case 0:
            {
                int i;
                int cnt = *(p->session + 1);
                int l = sizeof(protocol_t);
                PKG_UINT8(gbuf, *(p->session), l);
                DEBUG_LOG("%d get items flag %d", p->id, *(p->session));
                PKG_UINT8(gbuf, (uint8_t)cnt, l);
                for (i = 0; i < cnt; i++) {
                    uint32_t item_id = *(uint32_t*)(p->session + 4*i + 2);
                    PKG_UINT32(gbuf, item_id, l);
                    DEBUG_LOG("%d get items callback %d", p->id, item_id);
                }
                init_proto_head(gbuf, proto_get_items, l);
                if (send_to_self(p, gbuf, l, 1) != 0) {
                    DEBUG_LOG("send get_items_callback error");
                }
                /*
                   else {
                   DEBUG_LOG("%d get changqiang", p->id);
                   }
                   */
                break;
            }
        case 1114:
            {
                int i;
                int cnt = *(p->session + 1);
                for (i = 0; i < cnt; i++) {
                    uint32_t item_id = *(uint32_t*)(p->session + 4*i + 2);
                    DEBUG_LOG("%d already has item %d", p->id, item_id);
                }
                break;
            }
        default:
            DEBUG_LOG("get items callback ret %d", ret);
    }
    if (p->group == 0) {
        DEBUG_LOG("GET_ITEMS_CALLBACK delete player %d", p->id);
        del_sprite_conn(p);
    }
    return 0;
}


int upd_game_attr_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN( len, 20 );

	const game_bonus_t* gbonus = buf;
	game_score_pkg_t* gsp = (void*)p->db_data;

#ifndef TW_VER
	/* statistic log operation when enable statistic  */
	if (statistic_file != NULL) {
		uint32_t statistic_type = 0;
		statistic_type = STATISTIC_TYPE + gsp->gid;
		statistic_info_t stat_info;
		stat_info.coins	= gsp->coins;
		stat_info.exp	= gsp->exp;
		stat_info.strong= gsp->strong;
		stat_info.iq 	= gsp->iq;
		stat_info.charm	= gsp->lovely;
		stat_info.count = 1;
		stat_info.user_id = p->id;

		int msgret = msglog(statistic_file, statistic_type, get_now_tv()->tv_sec, &stat_info, sizeof(statistic_info_t));
		if (msgret != 0) {
			ERROR_LOG( "statistic log error: message type(%x)",statistic_type);
		}
	}
#endif
	switch (p->waitcmd) {
	case proto_game_score:
		DEBUG_LOG( "GAME EXP\t[%u %d %d %d %d %d]", id, gbonus->exp,
					gbonus->strong, gbonus->iq, gbonus->charm, gbonus->xiaomee);

		gsp->gid    = htons(gsp->gid);
		gsp->rank   = htons(gsp->rank);
		gsp->strong = htonl(gsp->strong);
		gsp->iq     = htonl(gsp->iq);
		gsp->lovely = htonl(gsp->lovely);
		gsp->exp    = htonl(gsp->exp);
		gsp->score  = htonl(gsp->score);
		gsp->time   = htonl(gsp->time);
		gsp->coins  = htonl(gsp->coins);
		send_to_self(p, p->db_data, ntohl(*(int *)p->db_data), 1);
		break;
	default:
		ERROR_RETURN(("error waitcmd, cmd=%u, id=%u", p->waitcmd, p->id), -1);
	}
	if (p->group == 0) del_sprite_conn(p);
	return 0;
}

void add_grp_timers(const game_group_t* grp, const time_t expiretime)
{
	size_t i = 0;
	for (; i != grp->count; ++i) {
		ADD_TIMER_EVENT(grp->players[i], on_timer_expire, 0, expiretime);
	}
}

void mod_grp_timers(const game_group_t* grp, const time_t expiretime)
{
	size_t i = 0;
	for (; i != grp->count; ++i) {
		MOD_EVENT_EXPIRE_TIME(grp->players[i], on_timer_expire, expiretime);
	}
}

void remove_grp_timers(const game_group_t* grp)
{
	size_t i = 0;
	for (; i != grp->count; ++i) {
		remove_timers(&(grp->players[i]->timer_list));
	}
}

void send_to_group_except_self(const game_group_t* ggp, uint32_t id, void* buffer, int len)
{
	int i = 0;
	// send to players
	for ( ; i != ggp->count; ++i ) {
		if (ggp->players[i]->id != id)
			send_to_self(ggp->players[i], buffer, len, 0);
	}
	// send to watchers
	for ( i = 0; i != ggp->nwatchers; ++i ) {
		if (ggp->watchers[i]->id != id)
			send_to_self(ggp->watchers[i], buffer, len, 0);
	}
}

/*
int get_fire_medal(sprite_t* p, int rank)
{
    int medal_type = 0;
    if(rank < 0 || rank > 3) {
        return medal_type;
    }
    if(p->waitcmd == proto_player_leave) {
        return medal_type;
    }
    static int fire_cup_medal[4] = {101,102,103, 0};
    if ((p->group->game->players >= 2)) {
        medal_type = fire_cup_medal[rank];
    }
    DEBUG_LOG("CAL FIRE MEDAL [%d %d %d]", p->id, medal_type, rank);
    return medal_type;
}

int db_add_fire_medal(sprite_t* p, int medal_type)
{
    int i = 0;
	char buf[12];
	PKG_H_UINT32(buf, medal_type - 100, i);
	PKG_H_UINT32(buf, 1, i);
	PKG_H_UINT32(buf, 1, i);
	return send_request_to_db(SVR_PROTO_ADD_FIRE_MEDAL, p, sizeof buf, buf, p->id);
}

int add_fire_medal_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN(len, 0);

	game_score_pkg_t* gsp = (void*)p->db_data;
	if (ret) {
        DEBUG_LOG("ADD FIRE MEDAL ERROR [%d %d]", p->id, ret);
		gsp->itmid = 0;
	} else {
        uint32_t dbmsg[3];
        dbmsg[0] = ntohl(gsp->itmid) - 100;
        dbmsg[1] = 1;
        dbmsg[2] = 1;
        DEBUG_LOG("GET FIRE MEDAL [%d %d]", p->id, dbmsg[0]);
        send_request_to_db(SVR_PROTO_SPORT_ADD_TEAM_MEDAL, 0, 12, dbmsg, p->id);
    }

    if ( db_upd_game_attr(p, gsp) == -1 ) {
        p->waitcmd = 0;
        ERROR_RETURN(("update game attr error: uid=%u gameid=%u", p->id, p->group->game->id), -1);
    }

	DEBUG_LOG("GAME SCORE\t[id=%u rank=%d score=%d time=%ld coins=%d iq=%d strong=%d exp=%d charm=%d]",
			p->id, gsp->rank, gsp->score, gsp->time, gsp->coins, gsp->iq, gsp->strong, gsp->exp, gsp->lovely);
	return 0;
}
*/

/*
int get_yuanbao(sprite_t* p)
{
	int yuanbao_type = 0;
	int rnd = rand() %100;
	if(rnd < 40) {
		yuanbao_type = 101;
	} else {
		yuanbao_type = 102;
	}
	return yuanbao_type;
}

int db_add_yuanbao(sprite_t* p, int yuanbao_type)
{
	int l = 0;
	//gold
	if (yuanbao_type == 101){
		PKG_H_UINT32(gbuf, 1, l);
		PKG_H_UINT32(gbuf, 0, l);
	} else if (yuanbao_type == 102) {
		//silver
		PKG_H_UINT32(gbuf, 0, l);
		PKG_H_UINT32(gbuf, 1, l);
	}
	return send_request_to_db(db_get_yuanbao, p, l, gbuf, p->id);
}

int add_yuanbao_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret)
{
	game_score_pkg_t* gsp = (void*)p->db_data;
	if (ret) {
		gsp->itmid = 0;
	}
	//DEBUG_LOG("%d yuanbao_callback item %d, ret %d", p->id, ntohl(gsp->itmid), ret);

	if ( db_upd_game_attr(p, gsp) == -1 ) {
		p->waitcmd = 0;
		ERROR_RETURN(("update game attr error: uid=%u gameid=%u", p->id, p->group->game->id), -1);
	}

	DEBUG_LOG("GAME SCORE\t[id=%u rank=%d score=%d time=%ld coins=%d iq=%d strong=%d exp=%d charm=%d]",
			p->id, gsp->rank, gsp->score, gsp->time, gsp->coins, gsp->iq, gsp->strong, gsp->exp, gsp->lovely);
	return 0;


}
*/

/*/-------------------------------------------------------
int get_medal(sprite_t* p)
{
	int medal_type = 0;
	if ((p->status > 1) && (p->group->game->players > 1)) {
		int rnd = rand() % 100;
		if (rnd < 10) {
			medal_type = 101;
		} else if (rnd < 40) {
			medal_type = 102;
		} else {
			medal_type = 103;
		}
	}
	return medal_type;
}

int db_add_medal(sprite_t* p, int medal_type)
{
	char buf[12];

	int  i = 0;
	// pack team id
	if ( test_bit_on32(p->status, 12) ) {
		PKG_H_UINT32(buf, 1, i);
	} else if ( test_bit_on32(p->status, 13) ) {
		PKG_H_UINT32(buf, 2, i);
	} else if ( test_bit_on32(p->status, 14) ) {
		PKG_H_UINT32(buf, 3, i);
	} else if ( test_bit_on32(p->status, 15) ) {
		PKG_H_UINT32(buf, 4, i);
	} else if ( test_bit_on32(p->status, 16) ) {
		PKG_H_UINT32(buf, 5, i);
	} else {
		return -1;
	}
	// pack game id
	PKG_H_UINT32(buf, p->group->game->id, i);
	// pack medal type
	PKG_H_UINT32(buf, medal_type - 100, i);

	return send_request_to_db(SVR_PROTO_ADD_MEDAL, p, sizeof buf, buf, p->id);
}

int add_medal_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret)
{
	CHECK_BODY_LEN(len, 4);

	game_score_pkg_t* gsp = (void*)p->db_data;
	if ( !*((uint32_t*)buf) ) {
		gsp->itmid = 0;
	}

	if ( db_upd_game_attr(p, gsp) == -1 ) {
		p->waitcmd = 0;
		ERROR_RETURN(("update game attr error: uid=%u gameid=%u", p->id, p->group->game->id), -1);
	}

	DEBUG_LOG("GAME SCORE\t[id=%u rank=%d score=%d time=%ld coins=%d iq=%d strong=%d exp=%d charm=%d]",
			p->id, gsp->rank, gsp->score, gsp->time, gsp->coins, gsp->iq, gsp->strong, gsp->exp, gsp->lovely);
	return 0;
}*/
