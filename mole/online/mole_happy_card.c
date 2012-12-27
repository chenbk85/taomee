
#include "item.h"
#include "dbproxy.h"
#include "proto.h"
#include "mole_happy_card.h"
#include "exclu_things.h"
#define NICK_LEN 16

typedef struct _hy_card_t {
    uint32_t id;
    uint32_t cnt;
    uint32_t flag;
    uint32_t card_want;
}usr_hy_card_t;

typedef struct _set_t {
    uint32_t flag;
    uint32_t card;
    uint32_t card_want;
    uint8_t  nick[NICK_LEN];
    uint32_t color;
    uint8_t  is_vip;
    uint32_t time;
} __attribute__((packed)) set_hy_card_t;

typedef struct _trade_o_t {
    uint32_t card;
    uint32_t card_want;
}trade_hy_card_t;

int set_happy_card_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 12);
    set_hy_card_t set_info;
    int i = 0;
    UNPKG_UINT32(body, set_info.flag, i);
    UNPKG_UINT32(body, set_info.card, i);
    UNPKG_UINT32(body, set_info.card_want, i);
    DEBUG_LOG("SET HAPPY CARD [%d %d %d %d]", p->id, set_info.flag, set_info.card, set_info.card_want);
    item_t* itm1 = get_item_prop(set_info.card);
    item_t* itm2 = get_item_prop(set_info.card_want);
    if(!itm1 || !itm2) {
        ERROR_RETURN(("wrong happy card id [%d]", p->id), -1);
    }
    if((set_info.card >=1290001 && set_info.card <= 1290020 && set_info.card_want >=1290056)
            || (set_info.card >=1290056 && set_info.card <= 1290060 && set_info.card_want <=1290055)) {
        return send_to_self_error(p, p->waitcmd, -ERR_happy_card_not_match, 1);
    }
    memcpy(set_info.nick, p->nick, NICK_LEN);
    set_info.color = p->color;
    set_info.is_vip = ISVIP(p->flag);
    set_info.time = get_now_tv()->tv_sec;
    return send_request_to_db(SVR_PROTO_SET_HAPPY_CARD, p, sizeof(set_hy_card_t), &set_info, p->id);
}

int get_happy_card_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t uid;
    UNPKG_UINT32(body, uid, i);
    CHECK_VALID_ID(uid);
    DEBUG_LOG("GET HAPPY CARD [%d]", p->id);
    return send_request_to_db(SVR_PROTO_GET_HAPPY_CARD, p, 0, NULL, uid);
}

int trade_happy_card_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4 + sizeof(trade_hy_card_t));
    trade_hy_card_t *trade_info = (trade_hy_card_t*)p->session;
    trade_hy_card_db_t other_trade_info;
    int i = 0;
    uint32_t other_id;
    UNPKG_UINT32(body, other_id, i);
    CHECK_VALID_ID(other_id);
    UNPKG_UINT32(body, trade_info->card, i);
    UNPKG_UINT32(body, trade_info->card_want, i);
    if((trade_info->card >=1290001 && trade_info->card <= 1290020 && (trade_info->card_want > 1290020 || trade_info->card_want < 1290001))
            || (trade_info->card >= 1290056 && trade_info->card <= 1290060 && (trade_info->card_want > 1290060 || trade_info->card_want < 1290056))
            || (trade_info->card >= 1290066 && trade_info->card <= 1290070 && (trade_info->card_want > 1290070 || trade_info->card_want < 1290066))) {
        return send_to_self_error(p, p->waitcmd, -ERR_happy_card_not_match, 1);

    }
    other_trade_info.card = trade_info->card_want;
    other_trade_info.card_want = trade_info->card;
    DEBUG_LOG("TRADE HAPPY CARD [%d %d %d %d]", p->id, other_id, trade_info->card, trade_info->card_want);
    item_t* itm1 = get_item_prop(trade_info->card);
    item_t* itm2 = get_item_prop(trade_info->card_want);
    if(!itm1 || !itm2) {
        ERROR_RETURN(("wrong happy card id [%d]", p->id), -1);
    }
    *(uint32_t*)(p->session + sizeof(trade_hy_card_t)) = other_id;
    other_trade_info.identify = 0;
    return send_request_to_db(SVR_PROTO_TRADE_HAPPY_CARD, p, sizeof(trade_hy_card_db_t), &other_trade_info, other_id);
}

int get_trade_happy_card_info_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 8);
    trade_hy_card_t trade_info;
    int i = 0;
    UNPKG_UINT32(body, trade_info.card, i);
    UNPKG_UINT32(body, trade_info.card_want, i);
    if((trade_info.card >=1290001 && trade_info.card <= 1290020 && trade_info.card_want >=1290056)
            || (trade_info.card >=1290056 && trade_info.card <= 1290060 && trade_info.card_want <=1290055)) {
        return send_to_self_error(p, p->waitcmd, -ERR_happy_card_not_match, 1);

    }

    DEBUG_LOG("GET TRADE HAPPY CARD INFO [%d %d %d]", p->id, trade_info.card, trade_info.card_want);
    return send_request_to_db(SVR_PROTO_GET_TRADE_HAPPY_CARD_INFO, p, sizeof(trade_hy_card_t), &trade_info, p->id);
}

int get_happy_card_cloth_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    return db_set_sth_done(p, 126, 1, p->id);
}

int set_happy_card_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 0);
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

int get_happy_card_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN_GE(len, 4);
    uint32_t count = *(uint32_t*)buf;
    DEBUG_LOG("GET HAPPY CARD CALLBACK [%d %d]", p->id, count);
    CHECK_BODY_LEN(len, 4 + count * sizeof(usr_hy_card_t));
    usr_hy_card_t* usr_card_info = (usr_hy_card_t*)(buf +4);
    int i;
    int length = sizeof(protocol_t);
    PKG_UINT32(msg, count, length);
    for(i = 0; i < count; i++) {
        PKG_UINT32(msg, usr_card_info[i].id, length);
        PKG_UINT32(msg, usr_card_info[i].cnt, length);
        PKG_UINT32(msg, usr_card_info[i].flag, length);
        PKG_UINT32(msg, usr_card_info[i].card_want, length);
        //DEBUG_LOG("GET HAPPY CARD CALLBACK [%d %d %d %d %d %d]", p->id, count, usr_card_info[i].id, usr_card_info[i].cnt, usr_card_info[i].flag, usr_card_info[i].card_want);
    }
    init_proto_head(msg, p->waitcmd, length);
    return send_to_self(p, msg, length, 1);
}

int trade_happy_card_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    trade_hy_card_db_t trade_info_db;
    trade_info_db.card = *(uint32_t*)p->session;
    trade_info_db.card_want = *(uint32_t*)(p->session + 4);
    if(id != p->id) {
        //DEBUG_LOG("TRADE HAPPY CARD FIRST STEP [%d %d %d %d]", p->id, id, trade_info->card, trade_info->card_want);
        trade_info_db.identify = 1;
        return send_request_to_db(SVR_PROTO_TRADE_HAPPY_CARD, p, sizeof(trade_hy_card_db_t), &trade_info_db, p->id);
    }
    //DEBUG_LOG("TRADE HAPPY CARD SECOND STEP [%d %d %d %d]", p->id, id, trade_info->card, trade_info->card_want);
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

int get_trade_happy_card_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN_GE(len, 4);
    int i = 0;
    uint32_t count;
    count = *(uint32_t*)buf;
    DEBUG_LOG("GET TRADE CARD INFO CALLBACK [%d %d]", p->id, count);
    typedef struct _usr_hy_card_info_t {
        uint32_t uid;
        uint8_t  nick[NICK_LEN];
        uint32_t color;
        uint8_t  is_vip;
        uint32_t time;
    } __attribute__((packed)) usr_hy_card_info_t;
    CHECK_BODY_LEN(len, 4 + count * sizeof(usr_hy_card_info_t));
    usr_hy_card_info_t* usr_hy_info= (usr_hy_card_info_t*)(buf + 4);
    int length = sizeof(protocol_t);
    PKG_UINT32(msg, count, length);
    for(i = 0; i < count; i++) {
        PKG_UINT32(msg, usr_hy_info[i].uid, length);
        PKG_STR(msg, usr_hy_info[i].nick, length, NICK_LEN);
        PKG_UINT32(msg, usr_hy_info[i].color, length);
        PKG_UINT8(msg, usr_hy_info[i].is_vip, length);
        PKG_UINT32(msg, usr_hy_info[i].time, length);
        DEBUG_LOG("GET TRADE CARD INFO CALLBACK [%d %d %d]", p->id, count, usr_hy_info[i].uid);
    }
    init_proto_head(msg, p->waitcmd, length);
    return send_to_self(p, msg, length, 1);
}

int get_happy_card_cloth_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 8);
    uint32_t itmid;
    uint32_t count;
    int i = 0;
    UNPKG_H_UINT32(buf, itmid, i);
    UNPKG_H_UINT32(buf, count, i);
    int length = sizeof(protocol_t);
    PKG_UINT32(msg, itmid, length);
    PKG_UINT32(msg, count, length);
    DEBUG_LOG("GET HAPPY CLOTH [%d %d %d]", p->id, itmid, count);
    init_proto_head(msg, p->waitcmd, length);
    return send_to_self(p, msg, length, 1);
}

int get_magic_card_reward_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    int j = 0;
    uint8_t buff[13];
    PKG_H_UINT32(buff, 0, j);
    PKG_H_UINT32(buff, 12897, j);
    PKG_H_UINT32(buff, 12898, j);
    PKG_UINT8(buff, 2, j);
    //return send_request_to_db(SVR_PROTO_GET_ITEM, p, j, buff, p->id);
	return send_request_to_db(SVR_PROTO_CHK_ITEM, p, j, buff, p->id);
}

int do_get_magic_card_reward(sprite_t* p, uint32_t count)
{
    if(count) {
        do_exchange_item(p, 556, 0);
    } else {
        do_exchange_item(p,555, 0);
    }
    return 0;
}
