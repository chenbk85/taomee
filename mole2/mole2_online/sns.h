
#ifndef MOLE2_SNS_H
#define MOLE2_SNS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <async_serv/async_serv.h>

#include <libtaomee/dataformatter/bin_str.h>
#ifdef __cplusplus
}
#endif

#include "benchapi.h"
#include "dbproxy.h"
#include "cli_proto.h"
#include "svr_proto.h"
#include "sprite.h"
#include "mail.h"
#include "proto/mole2_db.h"
#include "proto/bus_server.h"
#include "proto/bus_server_enum.h"
#include "honor.h"
#include "skill.h"

#define OFFLINE_MSG_LEN			2000
#define MAX_TRADE_SHOP_CNT 		6	

enum state_type {
	team_switch		= 0,
	sns_state		= 1,
	pk_switch		= 2,
};

enum {
	shop_init=0,
	shop_register=1,
	shop_open=2,
};

enum relation_type {
	relation_none 	= 0,
	relation_pupil	= 1,
	relation_master = 2,
};

enum {
	be_off			= 0,
	be_idle			= 1,
	be_fighting		= 2,
	read_paper		= 3,
	talk_to_npc		= 4,
	game_story		= 5,
	info_p			= 6,
	pk_hide			= 11,
	hangup			= 15,
};

enum sprite_svr_state {			/* 0-4 for team pos */
	wait_animation		= MAX_PLAYERS_PER_TEAM,
	in_battle			= 6,
	load_bt_fail		= 7,
	load_bt_succ		= 8,
	apply_for_pk		= 9,
	apply_for_pk_pet	= 10,
	apply_for_war		= 11,
	apply_for_copy		= 12,
	hang_for_exp		= 13,
	hang_for_pet_exp	= 14,
	hang_for_skill_exp	= 15,
};

enum relation_up_type {
	relation_add,
	relation_del,
	relation_graduate,
};

#define db_add_friend(p_, adder_, addee_) \
		send_request_to_db((p_), (adder_), proto_db_add_friend,  &(addee_), 4)
#define db_del_friend(p, id, frdid) \
		send_request_to_db(p, id, proto_db_del_friend, &frdid, 4)
#define db_add_bklist(p, who) \
		send_request_to_db( p, p->id, proto_db_bklist_add, &who, 4)
#define db_del_bklist(p, id) \
		send_request_to_db(p, p->id, proto_db_bklist_del, &id, 4)
#define db_get_bklist(p) \
		send_request_to_db(p, p->id, proto_db_get_bklist, NULL, 0)
		
#define db_get_other_user_info(p, id) \
		send_request_to_db(p, id, proto_db_get_other_user_info, NULL, 0)

#define CHECK_USER_TIRED(p_) \
		do { \
			uint32_t oltime = get_ol_time(p_); \
			if (!IS_ADMIN(p_) && oltime > 5 * 60 * 60) { \
				KERROR_LOG((p_)->id, "tired\t[%u]", oltime); \
				return send_to_self_error((p_), (p_)->waitcmd, cli_err_user_tired, 1); \
			} \
		} while (0)

int talk_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int show_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int text_notify_pkg_pic(uint8_t buf[], int cmd, uint32_t id, map_id_t map,
			const char nick[], int accept, uint32_t pic, int len, const char txt[]);

int add_friend_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int add_friend_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int add_base_attr_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int reply_add_friend_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int del_friend_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int add_blacklist_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int del_blacklist_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_blacklist_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_bklist_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_user_info_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_offline_msg_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_other_user_info_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int users_online_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_user_detail_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_user_nick_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_user_nick_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int set_user_color_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_user_color_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int chk_str_valid_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int user_cure_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int add_hp_mp_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int cure_all_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int add_xiaomee_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int set_energy_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret);
int set_position_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_position_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int update_sprite_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int set_prof_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int user_re_birth_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int register_bird_map_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int register_bird_map_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int check_team_bird_map_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_bird_maps_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_maps_state_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int allocate_exp_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int allocate_exp_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_handbook_info_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_handbook_info_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int set_busy_state_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int check_online_id_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int db_add_exp_level(sprite_t* p);
int parent_add_invited(sprite_t* p);
int get_invited_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_invited_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int read_news_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_flag_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int post_msg_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int start_double_exp_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_injury_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret);
int noti_relation_up_cmd(sprite_t* p, uint8_t* body, uint32_t len);

int reset_attr_cmd(sprite_t* p, uint8_t* body, uint32_t len);
uint32_t get_fight_power(sprite_t *p);
int hangup_get_exp(void* owner, void* data);
int release_copy_as_noenter(void*owner, void *data);
void clean_hangup_info(sprite_t* p);
void rsp_stop_hangup(sprite_t* p, uint32_t reason, int completed);
//int rm_from_gvg_group(sprite_t * p);

int try_vip_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret);

static inline uint32_t calc_level_from_exp(uint32_t exp, uint32_t curlevel)
{
	uint32_t level = curlevel;
	if (exp == 0 || curlevel >= MAX_USER_LEVEL)
		return curlevel;
	while(get_lv_exp(level + 1) <= exp) {
		level ++;
		if (level >= MAX_USER_LEVEL)
			return level;
	}
	return level;
}
static inline uint32_t calc_pet_level_from_exp(uint32_t exp, uint32_t curlevel)
{
	uint32_t level = curlevel;
	if (exp == 0 || curlevel >= MAX_PET_LEVEL)
		return curlevel;
	while(get_lv_exp(level + 1) <= exp) {
		level ++;
		if (level >= MAX_PET_LEVEL)
			return level;
	}
	return level;
}
static inline uint32_t calc_relation_level(uint32_t exp)
{
	uint32_t level = 0;
	for (level = 0; level < 10; level ++) {
		if (relation_level[level][0] > exp)
			break;
	}
	return level;
}

static inline void notify_user_levelup(sprite_t* p)
{
	uint8_t buff[64] = {0};
	int j = sizeof(protocol_t);
	PKG_UINT32(buff, p->id, j);
	PKG_UINT32(buff, p->level, j);
	PKG_UINT32(buff, p->attr_addition, j);
	KDEBUG_LOG(p->id, "USER LV UP\t[%u %u]", p->level, p->attr_addition);
	init_proto_head(buff, proto_cli_notify_user_levelup, j);
	send_to_map(p, buff, j, 0,1);
	msg_log_user_level(p->id, p->register_time, p->level);
	monitor_sprite(p, "LEVEL UP");
}

static inline void notify_busy_state_update(sprite_t* p, uint32_t type, uint32_t state)
{
	uint8_t buff[64] = {0};
	int j = sizeof(protocol_t);
	PKG_UINT32(buff, p->id, j);
	PKG_UINT32(buff, type, j);
	PKG_UINT32(buff, state, j);
	init_proto_head(buff, proto_cli_set_busy_state, j);
	send_to_map(p, buff, j, 0,1);
}

static inline void noti_all_sys_info(uint32_t uid, char* nick, sysinfo_t* psys)
{
	uint8_t outbuf[256];
	int i = sizeof(protocol_t);
	PKG_UINT32(outbuf, uid, i);
	PKG_STR(outbuf, nick, i, USER_NICK_LEN);
	PKG_UINT32(outbuf, psys->type, i);
	PKG_UINT32(outbuf, psys->val1, i);
	PKG_UINT32(outbuf, psys->val2, i);
	PKG_STR(outbuf, psys->message, i, SYS_INFO_MSG_LEN);
	init_proto_head(outbuf, proto_cli_sys_info, i);
	send_to_all(outbuf, i);
	
}

static inline void response_user_vip_info(sprite_t* p)
{
	uint8_t out[128];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, p->id, i);
	PKG_UINT32(out, p->flag, i);
	PKG_UINT32(out, p->viplv, i);
	PKG_UINT32(out, p->vipexp, i);
	PKG_UINT32(out, p->vip_begin, i);
	PKG_UINT32(out, p->vip_end, i);
	init_proto_head(out, proto_cli_vip_update, i);
	send_to_map(p, out, i, 0, 1);
}

static inline int db_add_hp_mp(sprite_t* p, uint32_t id, uint32_t hp, uint32_t mp, uint32_t injury_lv)
{
	uint8_t out[64];
	int i = 0;
	PKG_H_UINT32(out, hp, i);
	PKG_H_UINT32(out, mp, i);
	PKG_H_UINT32(out, injury_lv, i);
	return send_request_to_db(p, id, proto_db_add_hp_mp, out, i);
}

static inline int db_set_user_position(sprite_t* p, uint32_t id, uint32_t pos) 
{
	return send_request_to_db(p, id, proto_db_set_position, &pos, 1);
}

static inline int db_set_user_injury(sprite_t* p, uint32_t id, uint32_t injury_lv)
{
	return send_request_to_db(p, id, proto_db_user_set_injury, &injury_lv, 4);
}

static inline int db_set_user_map_info(sprite_t* p, uint32_t id, uint32_t time, map_id_t mapid, uint16_t posX, uint16_t posY, uint32_t recv_map)
{
	uint8_t buff[64];
	int j = 0;
	PKG_H_UINT32(buff, time, j);
	PKG_H_UINT16(buff, posX, j);
	PKG_H_UINT16(buff, posY, j);
	PKG_H_UINT64(buff, mapid, j);
	PKG_H_UINT32(buff, recv_map, j);
	PKG_H_UINT32(buff, get_server_id(), j);
	return send_request_to_db(p, id, proto_db_set_map_info, buff, j);
}

static inline int db_set_levelup_time(sprite_t* p)
{
	return send_request_to_db(NULL, p->id, mole2_set_levelup_time_cmd, &p->levelup_time, 4);
}

static inline int db_set_expbox(sprite_t* p, uint32_t uid, uint32_t expbox)
{
	return send_request_to_db(p, uid, proto_db_set_expbox, &expbox, 4);
}

static inline void add_cmd_pending_across_svr(sprite_t* p, uint32_t uid, uint8_t* body, uint32_t bodylen)
{
	static uint8_t buf[switch_proto_max_len];
	*(uint32_t *)buf = uid;
	memcpy(buf + 4, body, bodylen);
	send_to_switch(NULL, COCMD_add_cmd_across_svr, bodylen + 4, buf, p->id);
}


static inline void noti_other_relation_update(sprite_t* p, uint32_t uid, uint32_t type)
{
	uint8_t out[64];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, p->id, i);
	PKG_STR(out, p->nick, i, USER_NICK_LEN);
	PKG_UINT32(out, type, i);
	init_proto_head(out, proto_cli_noti_relation_up, i);

	protocol_t* proto = reinterpret_cast<protocol_t*>(out);
	proto->len = htonl(i);
	proto->cmd = htons(proto_cli_noti_relation_up);
	proto->ret = 0;
	proto->id = htonl(uid);
	
	sprite_t* s = get_sprite(uid);
	if (s) {
		add_cmd_pending_head(s, out);
	} else {
		add_cmd_pending_across_svr(p, uid, out, i);
	}
}


static inline void try_graduate_when_levelup(sprite_t* p)
{
	if (p->relationship.flag == relation_pupil && p->level >= 30) {
		send_msg_to_db(NULL, p->id, mole2_reset_relation_flag_cmd, NULL);
		for (uint32_t loop = 0; loop < p->relationship.count; loop ++) {
			mole2_add_graduation_uid_in db_in;
			db_in.uid = p->id;
			send_msg_to_db(NULL, p->relationship.users[loop].uid, mole2_add_graduation_uid_cmd, &db_in);
			msg_log_graduation(p->relationship.users[loop].uid);
			noti_other_relation_update(p, db_in.uid, relation_graduate);
		}
		memset(&p->relationship, 0, sizeof(p->relationship));
	}
}

static inline void update_when_level_change(sprite_t* p, uint32_t old_v, uint32_t new_v)
{
	if (old_v >= new_v) {
		return ;
	}
	//角色等级达到一定时，在升级时自动领悟职业技能
	add_skill_for_new_prof(p,new_v);
	if (new_v == 50) {
		honor_attr_t* pha = get_honor_attr(1026);
		if (pha) {
			update_one_honor(p, pha);
			set_user_honor(p, pha->honor_id);
			db_add_user_honor(NULL, p->id, p->prof_lv, 0, pha->honor_id);
			notify_user_honor_up(p, 1, 0, 1);
			db_set_user_honor(0, p->id, pha->honor_id);
		}
	}
	
	uint32_t lvuptime = p->total_time + get_now_tv()->tv_sec - p->login_time - p->levelup_time;
	KDEBUG_LOG(p->id, "LV UP\t[%u %u %u]", old_v, new_v, lvuptime);
	p->levelup_time = get_now_tv()->tv_sec - p->login_time - p->levelup_time;
	db_set_levelup_time(p);
	p->level = new_v;
	try_graduate_when_levelup(p);
	notify_user_levelup(p);
	try_to_send_for_level(p, old_v, new_v);
}

static inline uint32_t add_exp_to_sprite(sprite_t* p, uint32_t count)
{
	if (!count) return 0;
	if (p->experience + count >= MAX_TEMP_EXP_EX)
		count = MAX_TEMP_EXP_EX - p->experience - 1;
	p->experience += count;
	uint32_t level = calc_level_from_exp(p->experience, p->level);
	if (level > p->level) {
		p->attr_addition += 4 * (level - p->level);
		p->hp = p->hp_max;
		p->mp = p->mp_max;
		update_when_level_change(p, p->level, level);
	}
	db_add_exp_level(p);
	return count;
}

static inline int db_add_xiaomee(sprite_t* p, uint32_t id, int xiaomee) 
{
	DEBUG_LOG("DB ADD XIAOMEE[uid=%u xiaomee=%d]",id,xiaomee);
	return send_request_to_db(p, id, proto_db_add_xiaomee, &xiaomee, 4);
}

static inline int db_set_flag(sprite_t* p, uint32_t id, uint32_t bit, uint32_t flag)
{
	mole2_set_flag_bits_in db_in;
	db_in.pos_id=bit;
	db_in.is_set_true=flag;
	return send_msg_to_db(p, id, mole2_set_flag_bits_cmd, &db_in);
}

static inline int db_add_skill_expbox(sprite_t* p, uint32_t uid, int exp)
{
	mole2_add_skill_exp_in db_in;
	db_in.add_exp = exp;
	return send_msg_to_db(p, uid, mole2_add_skill_exp_cmd, &db_in);
}

#define text_notify_pkg(buf_, cmd_, uid_, mapid_, nick_, accept_, len_, txt_) \
	text_notify_pkg_pic((buf_), (cmd_), (uid_), (mapid_), (nick_), (accept_), 0, (len_), (txt_))

static inline int del_item(sprite_t *p, uint32_t itemid, int32_t count)
{
    if(count>0){
		cache_reduce_kind_item(p,itemid,count);
		db_add_item(NULL,p->id,itemid,count*(-1));
    }
	return 0;
}
static inline int32_t alloc_map_copy(map_copy_config_t* pmcc)
{
	uint32_t i=pmcc->instanceid[0];
	if(pmcc->instanceid[0] !=0)
		pmcc->instanceid[0]=pmcc->instanceid[i];
	else
		return 0;
	return i+(0xFD000000 + (pmcc->copyid << 16));
}

static inline int32_t free_map_copy(map_copy_config_t* pmcc, uint32_t pos)
{
	pmcc->instanceid[pos-(0xFD000000 + (pmcc->copyid << 16))]=pmcc->instanceid[0];
	pmcc->instanceid[0]=pos-(0xFD000000 + (pmcc->copyid << 16));
	return 0;
}
static inline uint32_t get_hangup_activity_time(sprite_t *p)
{
	uint32_t add_time=0;
	if(get_now_sec(p->login_time)>= HANGUP_START_TIME && get_now_sec()>= HANGUP_START_TIME)
		add_time=get_now_sec()-get_now_sec(p->login_time);
	else if(get_now_sec(p->login_time) <= HANGUP_START_TIME && get_now_sec()>= HANGUP_START_TIME)
		add_time=get_now_sec()-HANGUP_START_TIME;
	return p->hangup_activity_time+add_time;
}

int is_in(uint32_t *arr, uint32_t val, uint32_t cnt);
int check_users_again(void*owner,void *data);

#define CHECK_SELF_IN_TRADE(p) \
		do { \
			if (!p->tiles || !IS_TRADE_MAP(p->tiles->id)){\
				KERROR_LOG(p->id,"not in market");\
				return send_to_self_error(p, p->waitcmd, cli_err_mapid_invalid, 1);\
			}\
		} while (0)
	
#define CHECK_SHOP_STATE(p,state) \
		do { \
			if(p->shop_state != state){\
				KERROR_LOG(p->id,"shop state error[now=%u need=%u]",p->shop_state,state);\
				return send_to_self_error(p,p->waitcmd, cli_err_shop_state, 1);\
			}\
		} while (0)
#define     MAX_PRICE_VALUE  9999999

#define CHECK_PRICE_VALID(p,price) \
		do { \
			if(price>=MAX_PRICE_VALUE){\
				KERROR_LOG(p->id,"price too high[%u]",price);\
				return send_to_self_error(p, p->waitcmd, cli_err_price_too_high, 1);\
			}\
		} while (0)

void trade_get_items(sprite_t *p, cli_trade_get_items_out &out);
bool check_trade_item_exist(sprite_t *p, uint32_t itemid ,uint32_t tradeid, int32_t itemcnt);
bool check_trade_cloth_exist(sprite_t *p, uint32_t itemid ,uint32_t tradeid, int32_t itemcnt);
trade_item_t trade_del_item_ex(sprite_t *p, uint32_t itemid,int32_t itemcnt, uint32_t tradeid);
trade_cloth_t trade_del_cloth_ex(sprite_t *p, uint32_t clothid, uint32_t tradeid);
void trade_get_records(sprite_t *p, cli_trade_get_records_out &out);
bool check_trade_xiaomee_enough(uint32_t xiaomee, sprite_t *p, uint32_t itemid, int32_t itemcnt,uint32_t tradeid);

static inline void add_to_trade_record(sprite_t* p,uint32_t buyer, trade_item_t *pti)
{
	trade_record_t tmp;
	tmp.itemid=pti->itemid;
	tmp.itemcnt=pti->itemcnt;
	tmp.price=pti->price;
	tmp.tax=pti->tax;
	tmp.trade_time=time(NULL);
	p->trade_record_info->insert(std::make_pair(buyer,tmp));
}

static inline void add_to_trade_record(sprite_t* p, uint32_t buyer, trade_cloth_t *pti)
{
	trade_record_t tmp;
	tmp.itemid=pti->clothtype;
	tmp.itemcnt=1;
	tmp.price=pti->price;
	tmp.tax=pti->tax;
	tmp.trade_time=time(NULL);
	p->trade_record_info->insert(std::make_pair(buyer,tmp));
}

/**
 * @brief 特权检查，添加的帐号将不再需要受限制 
 *
 * @param uid
 *
 * @return true:需要检查   false：不需要检查
 */
static inline bool uid_need_check(uint32_t uid)
{
	switch ( uid ){
		case 88693:
		case 231805151:
		case 136041924:
		case 158303066:
		case 236001116:
		case 5004699:
			return false;
			break;
		default :
			return true;
	}
}
#endif

