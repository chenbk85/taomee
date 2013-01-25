#ifndef MOLE2_ACTIVITY_H
#define MOLE2_ACTIVITY_H

#include "benchapi.h"
#include "util.h"
#include "mail.h"

#define CHECK_FISHING_STARTED(p_) \
	do { \
		if (p->cur_fish_cnt == -1) { \
			KERROR_LOG(p->id, "not start fish"); \
			return send_to_self_error(p, p->waitcmd, cli_err_not_start_fish, 1); \
		} \
	} while (0)


#define CHECK_FISHING_CNT(p_) \
	do { \
		if (p->cur_fish_cnt >= 10) { \
			KERROR_LOG(p->id, "fish cnt max"); \
			return send_to_self_error(p, p->waitcmd, cli_err_rand_all_max, 1); \
		} \
	} while (0)

int get_water_cnt_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int reset_water_cnt(void* owner, void* data);
int use_water_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int try_get_water_cnt(void* owner, void* data);
int load_fish_info(xmlNodePtr cur);

int set_cli_buff_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_cli_buff_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int lucky_star_add_time(void*owner, void *data);
inline bool check_anti_indulgence(sprite_t *p);
int get_luck_star_reward(sprite_t *p);

static inline void noti_all_online_get_item(sprite_t* p, uint32_t itemid)
{
	uint8_t buf[1024];
	int j = 0;
	//发往switch的广播包所需的数据
	PKG_H_UINT32(buf, 0, j);//onlineid
	PKG_H_UINT32(buf, 0, j);//uid
	PKG_H_UINT32(buf, sys_info_get_item, j);//type
//发给客户端的数据，在broadcast_msg中将通过send_to_all直接发给客户端，所以要把包头封装好
	uint8_t out[256];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, 1, i);
	PKG_UINT32(out, p->id, i);
	PKG_STR(out, p->nick, i, USER_NICK_LEN);
	PKG_UINT32(out, sys_info_get_item, i);
	PKG_UINT32(out, itemid, i);
	PKG_UINT32(out, 0, i);
	PKG_UINT32(out, 0, i);
	init_proto_head(out, proto_cli_sys_info, i);

	PKG_H_UINT16(buf, i, j);//msg len
	memcpy(buf+j,out,i);//copy msg
	j += i;
	PKG_H_UINT32(buf, sizeof(uint32_t), j);//for dbmsg
	send_to_switch(p, COCMD_broadcast_msg, j, buf, p->id);
	KDEBUG_LOG(p->id, "SEND ALL ONLINE GET ITEM:%u",j);
}
#endif
