#include <statistic_agent/msglog.h>
#include "mole_car.h"
#include "util.h"
#include "logic.h"
#include "proto.h"
#include "dbproxy.h"
#include "hallowmas.h"

#define DRIVING     1
#define NO_DRIVING  0
#define OIL_PRICE   20

int drive_out_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t car_id;
    UNPKG_UINT32(body, car_id, i);
    DEBUG_LOG("DRIVE OUT [%d %d]", p->id, car_id);
    return send_request_to_db(SVR_PROTO_CAR_INFO, p, 4, &car_id, p->id);
}

int buy_car_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    if(!p->driver_time) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_driver, 1);
    }
    int i = 0;
    uint32_t car_id;
    UNPKG_UINT32(body, car_id, i);
    item_t* item = get_item_prop(car_id);
	if (!item|| !itm_buyable(p, item)) {
		ERROR_LOG( "Car %u not found or Unbuyable: uid=%u itmflag=0x%X",
						car_id, p->id, (item? item->tradability : 0));
		return send_to_self_error(p, p->waitcmd, -ERR_cannot_buy_itm, 1);
	}

	item_kind_t* kind = find_kind_of_item(car_id);
    if(!kind || kind->kind != CAR_KIND) {
        ERROR_RETURN(("err car id [%d %d]", p->id, car_id), -1);
    }
    DEBUG_LOG("BUY CAR [%d %d]", p->id, car_id);
    return db_add_car(p, car_id, item->price, p->id);
}

int buy_car_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	DEBUG_LOG("BUY CAR callback [user_id=%d ]", p->id);
	switch (p->waitcmd) {
		case PROTO_BUY_CAR:
		case PROTO_GET_VIP_LEVEL_GIFTS:
		case PROTO_GET_XUANFENG_CAR:
    			response_proto_head(p, p->waitcmd, 0);
    			return 0;
		case PROTO_RACE_AWORD_COMMEMORATE:
			response_proto_uint32(p, p->waitcmd, 1, 0);
			return 0;
		case PROTO_EXCHANGE_PROP:
			return do_send_car_and_prop(p);
		case PROTO_GET_XIAO_XIONG_CAR:
			response_proto_uint32(p, p->waitcmd, 1300010, 0);
			return 0;
		default:
			ERROR_RETURN(("INVAILD CMD[cmd=%u]", p->waitcmd),-1);
	}
	return 0;
}

void pack_car_info(uint8_t* buf, const car_t* mycar, int* index)
{
    PKG_UINT32(buf, mycar->db_id, *index);
    PKG_UINT32(buf, mycar->kindid, *index);
    PKG_UINT32(buf, mycar->oil, *index);
    PKG_UINT32(buf, mycar->oil_consump, *index);
    //PKG_UINT32(buf, carlist[i].oil_total, *index);
    PKG_UINT32(buf, mycar->max_oil, *index);
    PKG_UINT32(buf, mycar->color, *index);
    PKG_UINT32(buf, mycar->oil_time, *index);
    PKG_UINT32(buf, mycar->slot_cnt, *index);
    int k;
    for(k = 0; k < 4; k++) {
        PKG_UINT32(buf, mycar->slot[k], *index);
    }
    PKG_UINT32(buf, mycar->addon[0], *index);
    PKG_UINT32(buf, mycar->addon[1], *index);
}

int car_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, sizeof(car_t));
    car_t* mycar = (car_t*)buf;
    switch(p->waitcmd) {
        case PROTO_DRIVE_OUT:
            p->car_status = DRIVING;
            memcpy(&p->car_driving, mycar, sizeof(car_t));
			db_set_car_show(0, mycar->db_id, 0, p->id);

            int l = sizeof(protocol_t);
            PKG_UINT32(msg, p->id, l);
            pack_car_info(msg, mycar, &l);
            init_proto_head(msg, p->waitcmd, l);
            send_to_map(p, msg, l, 1);
			return 0;
            break;
        default:
            ERROR_RETURN(("err cmd [%d %d]", p->id, p->waitcmd), -1);

    }
        return 0;
}


int drive_back_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    if(p->car_status) {
        p->car_status = NO_DRIVING;
		memset(&(p->car_driving), 0, sizeof(car_t));
        response_proto_uint32(p, p->waitcmd, p->id, 1);
    } else {
        response_proto_uint32(p, p->waitcmd, p->id, 0);
    }
    return 0;
}

int list_cars_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t uid;
    UNPKG_UINT32(body, uid, i);
    CHECK_VALID_ID(uid);
    *(uint32_t*)p->session = uid;
    return send_request_to_db(SVR_PROTO_GARAGE_LIST, p, 0, NULL, uid);

}

int list_cars_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN_GE(len, 4);
    uint32_t car_cnt = *(uint32_t*)buf;
    CHECK_BODY_LEN(len, 4 + car_cnt * sizeof(car_t));
    car_t* carlist = (car_t*)(buf + 4);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, id, l);
    int i;
    if(p->car_status) {
        PKG_UINT32(msg, car_cnt - 1, l);
    } else {
        PKG_UINT32(msg, car_cnt, l);
    }
    for(i = 0; i < car_cnt; i++) {
        if(p->car_status && p->car_driving.db_id == carlist[i].db_id) {
            continue;
        }
        pack_car_info(msg, &carlist[i], &l);
    }
    init_proto_head(msg, p->waitcmd, l);
    DEBUG_LOG("CAR LIST [%d %d]", p->id, car_cnt);
    return send_to_self(p, msg, l, 1);
}

int set_show_car_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t show_car_id;
    UNPKG_UINT32(body, show_car_id, i);
    return db_set_car_show(p, show_car_id, 1, p->id);
}

int set_show_car_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

int get_show_car_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t uid;
    UNPKG_UINT32(body, uid, i);
    CHECK_VALID_ID(uid);
    return send_request_to_db(SVR_PROTO_SHOW_CAR_INFO, p, 0, NULL, uid);
}

int get_show_car_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, sizeof(car_t));
    car_t* mycar= (car_t*)buf;
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, id, l);
    PKG_UINT32(msg, 1, l);
    pack_car_info(msg, mycar, &l);
    init_proto_head(msg, p->waitcmd, l);
    DEBUG_LOG("GET SHOW CAR [%d %d %d]", p->id, id, l);
    return send_to_self(p, msg, l, 1);
}

int refuel_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t car_id;
    UNPKG_UINT32(body, car_id, i);
    *(uint32_t*)p->session = car_id;
    return send_request_to_db(SVR_PROTO_CAR_REFUEL, p, 4, &car_id, p->id);
}

int refuel_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 4);
    uint32_t xiaomee_left = *(uint32_t*)buf;
    msglog(statistic_logfile, 0X020B0101, get_now_tv()->tv_sec, &xiaomee_left, sizeof(uint32_t));
    response_proto_uint32(p, p->waitcmd, xiaomee_left, 0);
    return 0;
}

int get_jiazhao_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_VALID_ID(p->id);

	return db_single_item_op(p, p->id, 190426, 1, 1);
}

int check_car_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t car_id;
    UNPKG_UINT32(body, car_id, i);
    *(uint32_t*)p->session = car_id;
	item_kind_t* kind = find_kind_of_item(car_id);
    item_t* item = get_item_prop(car_id);
    if(!item || kind->kind != CAR_KIND) {
        ERROR_RETURN(("err car id [%d %d]", p->id, car_id), -1);
    }
    return send_request_to_db(SVR_PROTO_CHECK_CAR, p, 4, &car_id, p->id);
}

int check_car_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 4);
    uint32_t car_cnt = *(uint32_t*)buf;
    uint32_t car_id = *(uint32_t*)p->session;
    DEBUG_LOG("CHECK CAR CNT [%d %d %d]", p->id, car_id, car_cnt);
    response_proto_uint32(p, p->waitcmd, car_cnt, 0);
    return 0;
}

int set_drive_time_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	p->driver_time = 1;
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

int rent_zhaijisong_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    DEBUG_LOG("RENT ZJS [%u]", p->id);
	p->car_status = DRIVING;
	memset(&(p->car_driving), 0, sizeof(car_t));
	p->car_driving.kindid = 1300005;

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, l);
	pack_car_info(msg, &(p->car_driving), &l);
	init_proto_head(msg, p->waitcmd, l);
	send_to_map(p, msg, l, 1);
	return 0;
}

int on_off_nangua_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
    int i = 0;
    uint32_t on_or_off, itmid;
    UNPKG_UINT32(body, on_or_off, i);
	UNPKG_UINT32(body, itmid, i);

	item_t* pitm = get_item_prop(itmid);
	if (!pitm) {
		ERROR_RETURN(("PUT ON OFF ITM %u %u %u", p->id, on_or_off, itmid), -1);
	}
    DEBUG_LOG("ON OFF NANGUA [%u %u %u]", p->id, on_or_off, itmid);
	if (p->car_status != DRIVING || p->car_driving.kindid != 1300005) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_drive_zhaijisong, 1);
	}
	if (on_or_off)
		p->car_driving.addon[0] = itmid;
	else
		p->car_driving.addon[0] = 0;

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, l);
	pack_car_info(msg, &(p->car_driving), &l);
	init_proto_head(msg, p->waitcmd, l);
	send_to_map(p, msg, l, 1);
	return 0;
}


