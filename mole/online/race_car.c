
#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "exclu_things.h"
#include "login.h"
#include "small_require.h"
#include "mole_car.h"
#include "mole_class.h"

#include "race_car.h"

#define MAX_RACE_USER		25
#define RACE_NOT_READY		1
#define RACE_REACH_MAX		2
uint32_t 	team_renqi[2] = {0,0};
uint32_t 	team_user[2][MAX_RACE_USER];

int do_exchg_xuanfeng_jijile(sprite_t* p, uint32_t count)
{
    if(count) {
        do_exchange_item(p, 565, 0);
    } else {
        do_exchange_item(p,564, 0);
    }
    return 0;
}

int exchg_xuanfeng_jijile_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	return db_chk_itm_cnt(p, 1220098, 1220099);
}

int do_get_xuanfeng_car(sprite_t* p, uint32_t count, uint8_t* buf)
{
	uint32_t alrdy_get_car = 0;
	uint32_t itm, cnt;
	int loop, k = 0;
	for (loop = 0; loop < count; loop++) {
		UNPKG_H_UINT32 (buf, itm, k);
		UNPKG_H_UINT32 (buf, cnt, k);
		if (itm == 1300001 || itm == 1300003) {
			alrdy_get_car = 1;
			break;
		}
	}
	if (alrdy_get_car) {
		return send_to_self_error(p, p->waitcmd, -ERR_alrdy_get_this_car, 1);
	} else {
		return db_add_car(p, *(uint32_t*)p->session, 0, p->id);
	}
}

int get_xuanfeng_car_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);
	int j = 0;
	uint32_t itmid;
	UNPKG_UINT32(body, itmid, j);

	if (itmid != 1300001 && itmid != 1300003) {
		ERROR_RETURN(("can't get other car\t[%u %u]", p->id, itmid), -1);
	}

	*(uint32_t*)p->session = itmid;
	return db_chk_itm_cnt(p, 1300001, 1300004);
}

int get_user_num_in_ship_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	map_t* mp = get_map(72);

	response_proto_uint32(p, p->waitcmd, mp->sprite_num, 0);
	return 0;
}

int get_next_flight_time_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	if (!p->tiles || p->tiles->id != 83) {
		ERROR_RETURN( ("The map is wrong\t[%u]", p->id), -1 );
	}

	DEBUG_LOG("GET NEXT FLIGHT[%u %u]", p->id, (uint32_t)(p->tiles->next_flight - now.tv_sec));
	response_proto_uint32(p, p->waitcmd, (p->tiles->next_flight - now.tv_sec), 0);
	return 0;
}


