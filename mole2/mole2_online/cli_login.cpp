
#include <fcntl.h>
#include <sys/mman.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/project/utilities.h>
#include <async_serv/async_serv.h>

#ifdef __cplusplus
}
#endif
#include <libtaomee++/utils/md5.h>

#include "./proto/mole2_db.h"

#include "cli_proto.h"
#include "center.h"
#include "pet.h"
#include "map.h"
#include "items.h"
#include "skill.h"
#include "honor.h"
#include "battle.h"
#include "sns.h"
#include "mail.h"
#include "statistic.h"
#include "task_new.h"
#include "beast.h"
#include "npc.h"
#include "buff.h"

#include "cli_login.h"


void rsp_proto_login(sprite_t* p)
{
	int i = sizeof(protocol_t);
	KDEBUG_LOG(p->id,"flag2=%u",p->flag2);
	PKG_UINT32(msg, p->id, i);
	PKG_STR(msg, p->nick, i, 16);
	PKG_UINT32(msg, p->flag, i);
	PKG_UINT32(msg, p->flag2, i);
	PKG_UINT32(msg, p->viplv, i);
	PKG_UINT32(msg, p->vipexp, i);
	PKG_UINT32(msg, p->vip_begin, i);
	PKG_UINT32(msg ,p->vip_end, i);
	PKG_UINT32(msg, p->hero_team, i);
	PKG_UINT32(msg, p->color, i);
	PKG_UINT32(msg, p->register_time, i);
	PKG_UINT8(msg, p->race, i);
	PKG_UINT8(msg, p->prof, i);
	PKG_UINT32(msg, p->prof_lv, i);
	PKG_UINT32(msg, p->honor, i);
	PKG_UINT32(msg, p->xiaomee, i);
	PKG_UINT32(msg, p->pkpoint_total, i);
	PKG_UINT32(msg, p->energy, i);
	PKG_UINT32(msg, p->level, i);
	PKG_UINT32(msg, p->experience, i);
	PKG_UINT16(msg, p->physique, i);
	PKG_UINT16(msg, p->strength, i);
	PKG_UINT16(msg, p->endurance, i);
	PKG_UINT16(msg, p->quick, i);
	PKG_UINT16(msg, p->intelligence, i);
	PKG_UINT16(msg, p->attr_addition, i);
	PKG_UINT32(msg, p->hp, i);
	PKG_UINT32(msg, p->mp, i);
	PKG_UINT8(msg, p->earth, i);
	PKG_UINT8(msg, p->water, i);
	PKG_UINT8(msg, p->fire, i);
	PKG_UINT8(msg, p->wind, i);

	PKG_UINT32(msg, p->injury_lv, i);
	PKG_UINT32(msg, p->shapeshifting.pettype, i);

	PKG_MAP_ID(msg, (p->tiles ? p->tiles->id : 0), i);
	PKG_UINT32(msg, p->posX, i);
	PKG_UINT32(msg, p->posY, i);
	PKG_UINT32(msg, p->base_action, i);
	PKG_UINT32(msg, p->advance_action, i);
	PKG_UINT8(msg, p->direction, i);
	PKG_UINT8(msg, p->in_front, i);

	PKG_UINT32(msg, p->vip_buffs[buffid_hpmp][0], i);
	PKG_UINT32(msg, p->vip_buffs[buffid_hpmp][1], i);
	PKG_UINT32(msg, p->hp_max, i);
	PKG_UINT32(msg, p->mp_max, i);
	PKG_UINT16(msg, p->attack, i);
	PKG_UINT16(msg, p->defense, i);
	PKG_UINT16(msg, p->mdefense, i);
	PKG_UINT16(msg, p->speed, i);
	PKG_UINT16(msg, p->spirit, i);
	PKG_UINT16(msg, p->resume, i);
	PKG_UINT16(msg, p->hit_rate < 0 ? 0 : p->hit_rate, i);
	PKG_UINT16(msg, p->avoid_rate < 0 ? 0 : p->avoid_rate, i);
	PKG_UINT16(msg, p->bisha < 0 ? 0 : p->bisha, i);
	PKG_UINT16(msg, p->fight_back < 0 ? 0 : p->fight_back, i);
	PKG_UINT16(msg, p->rpoison, i);
	PKG_UINT16(msg, p->rlithification, i);
	PKG_UINT16(msg, p->rlethargy, i);
	PKG_UINT16(msg, p->rinebriation, i);
	PKG_UINT16(msg, p->rconfusion, i);
	PKG_UINT16(msg, p->roblivion, i);
	p->injury_lv=INJURY_NONE;	
	i += pack_activity_info(p, msg + i);	
	i += pkg_body_clothes_simple(p, msg + i);
	i += pkg_pet_follow(p->p_pet_follow, msg + i);
	i += pkg_ol_time(p, msg + i);
	i += pkg_ratio_buff(p->vip_buffs[0], msg + i);
	i += pkg_ratio_buff(p->vip_buffs[1], msg + i);
	i += pkg_ratio_buff(p->vip_buffs[2], msg + i);
	PKG_UINT32(msg, get_auto_fight_count(p), i);


	PKG_UINT32(msg, p->relationship.flag, i);
	if (p->relationship.flag == relation_master) {
		uint32_t masterlv = calc_relation_level(p->relationship.exp);
		PKG_UINT32(msg, relation_level[masterlv - 1][1], i);
		PKG_UINT32(msg, masterlv, i);
	} else {
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, 0, i);
	}
	PKG_UINT32(msg, p->relationship.exp, i);
	PKG_UINT32(msg, p->relationship.graduation, i);
	PKG_UINT32(msg, p->relationship.intimacy, i);
	PKG_UINT32(msg, p->relationship.count, i);
	for (uint32_t loop = 0; loop < p->relationship.count; loop ++) {
		PKG_UINT32(msg, p->relationship.users[loop].uid, i);
		PKG_UINT32(msg, p->relationship.users[loop].day, i);
		PKG_UINT32(msg, p->relationship.users[loop].intimacy, i);
	}

	init_proto_head(msg, proto_cli_login, i);
	protocol_t* pkg = (protocol_t *)msg;

	pkg->seq = htonl(p->seq);
	//KERROR_LOG(p->id, "ret seq\t[%u %u]", pkg->seq, p->seq);
	send_to_self(p, msg, i, 0);
}

/**
  * @brief handle login protocol
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int login_cmd(sprite_t* p, uint8_t* body, uint32_t bodylen)
{
	// registered user login
	if (p->id) {
		sprite_t* lsp = add_sprite(p);
		encrypt_login_session(p->session, sizeof(login_session_t), p->id, get_cli_ip(p->fdsess));

		uint8_t	out[32];
		int j = 0;

		/* verify session through session server ( dbproxy) */
		PKG_H_UINT32(out, MOLE2_GAME_FLAG, j);
		memcpy(out + j, body+4, 16);
		j += 16;
		PKG_H_UINT32(out, 0, j);
		KDEBUG_LOG(p->id, "LOGIN CMD CHECK SESSION");
		return send_request_to_db(lsp, lsp->id, proto_db_check_session, out, j);
	}

	KERROR_LOG(p->id, "not support visitor");
	return -1;
}

int add_session_cmd(sprite_t* p, uint8_t* body, uint32_t bodylen)
{
	
	typedef struct _add_session {
	   uint16_t channel_id;
	   char verify_info[32];
	   uint32_t gameid;
	   uint32_t ipaddr;
   } __attribute__ ((packed)) add_session_t;

	uint32_t type,gameid;
	int i = 0;
	UNPKG_UINT32(body, type, i);
	UNPKG_UINT32(body, gameid, i);
    add_session_t session_req;
	session_req.gameid=gameid;
	//switch (type) {
	//case 1:
		//session_req.gameid = 10000;
		//break;
	//case 2:
		//session_req.gameid = 9;
		//break;
	//default:
		//return -1;
	//}

	*(uint32_t *)p->session = type;

    unsigned char src[100];
    uint16_t channel_id = config_get_intval("channel_id", 0);
    uint32_t security_code = config_get_intval("security_code", 0);
    DEBUG_LOG("userid %d channel_id %d security_code %d", p->id, channel_id, security_code);

    session_req.channel_id = channel_id;
    session_req.ipaddr = p->fdsess->remote_ip;
    
	int len = snprintf((char*)src, sizeof src, "channelId=%d&securityCode=%u&data=", channel_id, security_code);
    *(uint32_t*)(src + len) = session_req.gameid; 
    *(uint32_t*)(src + len + 4) = p->fdsess->remote_ip; 
	utils::MD5 md5;

	md5.reset();
	md5.update ( (void*)src, len + 8 ); 
    
    memcpy(session_req.verify_info, md5.toString().c_str() , 32);
	return send_request_to_db(p, p->id, proto_db_add_session, &session_req, sizeof(session_req));
}



int add_session_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret)
{
	char buff_hex[33];
	bin2hex_frm(buff_hex, (char *)buf, 16, 0);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, *(uint32_t *)p->session, i);
	PKG_STR(msg, buff_hex, i, 32);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}


int check_session_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret)
{
	if (ret == dberr_session_err) {
		KERROR_LOG(p->id, "check session fail [%u]", id);
		del_sprite(p, 1);
		return -1;
	} else if (ret == dberr_net_error || ret == dberr_net_timeout) {	
		KERROR_LOG(p->id, "session svr timeout [%u %d]", id, ret);
		if (verify_login_session(p->id, p->session) == -1) {
			del_sprite(p, 1);
			return -1;
		}
	}
	
	return db_get_sprite(p, p->id);
}

int auto_add_energy(void*owner, void *data)
{
	sprite_t *p=(sprite_t *)owner;
	uint32_t time=180;
	p->p_add_energy=ADD_ONLINE_TIMER(p, n_auto_add_energy, NULL, time);
	if(p->energy<max_user_energy(p) && p->recover_energy < MAX_RECOVER_ERNERGY){
		p->energy+=time/60;
		p->recover_energy+=time/60;
		db_set_energy(NULL, p->id, p->energy);
		db_day_add_ssid_cnt(NULL, p->id, ssid_add_energy, time/60,-1);
	}
	if(p->shapeshifting.itemid && p->shapeshifting.lefttime){
		if(p->shapeshifting.lefttime <= time){
			db_day_set_ssid_cnt(NULL, p->id, item_id_shape_shifting , 0,0);
			db_day_set_ssid_cnt(NULL, p->id, item_id_shapeshifting_time ,0,0);
			shapeshifting_lose_effect(p);
		}else{
		   	p->shapeshifting.lefttime -= time; 
			db_sub_ssid_count(NULL, p->id, item_id_shapeshifting_time ,time);
		}
	}
	return 0;
}

//---------------------------------------------------------------------
int db_get_sprite_callback(sprite_t* p, userid_t id, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	

	CHECK_DBERR(p, ret);
	//KDEBUG_LOG(p->id, "AAAA1\t%u", ret);
	if (p->id != id) return 0;
	//KDEBUG_LOG(p->id, "AAAA2\t%u", ret);

	uint32_t pet_buf_len, fix_len;
	uint32_t logout_time, pk_uptime;
	uint32_t frcnt, skill_cnt, pet_cnt, cloth_cnt, cloth_inbag_cnt, item_cnt, task_cnt, honor_cnt;
	int len = sizeof(get_sprite_rsp_t);
	get_sprite_rsp_t* rsp = (get_sprite_rsp_t *)body;
	memcpy(&p->nick, rsp, len);
	
	p->flag2=2000000000;
	UNPKG_H_UINT32(body, logout_time, len);
	memcpy(p->session, body + len, 16);			// posx, posy, mapid
	len += 16;
	UNPKG_H_UINT32(body, p->online_time, len);
	UNPKG_H_UINT32(body, p->recv_map, len);
	p->recv_map = p->recv_map ? p->recv_map : 11102;

	UNPKG_H_UINT32(body, p->expbox, len);
	UNPKG_H_UINT32(body, p->energy, len);
	UNPKG_H_UINT32(body, p->skill_expbox, len);
	UNPKG_H_UINT32(body, p->uiflag, len);
	UNPKG_H_UINT32(body, p->last_beastgrp, len);
	UNPKG_H_UINT32(body, p->parent_id, len);
	UNPKG_H_UINT32(body, p->total_time, len);
	UNPKG_H_UINT32(body, p->levelup_time, len);
	UNPKG_H_UINT32(body, p->login_day, len);
	if (p->level < 10 || !p->login_day)
		p->login_day = 1;
	p->login_day %= 5;
	if (!p->login_day) p->login_day += 5;
	UNPKG_H_UINT32(body, p->vip_auto, len);
	UNPKG_H_UINT32(body, p->vip_exp_ex, len);
	UNPKG_H_UINT32(body, p->vip_base_exp, len);
	UNPKG_H_UINT32(body, p->vip_end, len);
	UNPKG_H_UINT32(body, p->vip_begin, len);
	UNPKG_H_UINT32(body, p->vip_time_1 ,len);
	UNPKG_H_UINT32(body, p->vip_time_2, len);
	UNPKG_H_UINT32(body, p->draw_cnt_gift, len);
	UNPKG_H_UINT32(body, p->vip_time_3, len);
	p->draw_cnt_gift = p->draw_cnt_gift / 86400 / 2;
	//KDEBUG_LOG(p->id, "AAAA\t%u", p->draw_cnt_gift);

	//KDEBUG_LOG(p->id, "AAAA1\tisvip=%u",ISVIP(p->flag) );
	recalc_vip_exp_level(p);
	//KDEBUG_LOG(p->id, "AAAA2\tisvip=%u",ISVIP(p->flag) );

	if (!p->online_time) {
		uint32_t max_energy = max_user_energy(p);
		KDEBUG_LOG(p->id, "ENERY RESET\t[%u %u %u %u]", p->energy, max_energy, logout_time, p->login_time);
		p->energy += max_energy * get_day_interval(p->login_time, logout_time) / 5;
		p->energy = p->energy > max_energy ? max_energy : p->energy;
		db_set_energy(NULL, p->id, p->energy);
	}

	p->p_add_energy=ADD_ONLINE_TIMER(p, n_auto_add_energy, NULL, 180);
	p->p_lucky_star=ADD_ONLINE_TIMER(p, n_lucky_star_timer, NULL, 60);
	UNPKG_H_UINT32(body, p->pkpoint_total, len);
	UNPKG_H_UINT32(body, p->pkpoint_day, len);
	UNPKG_H_UINT32(body, pk_uptime, len);
	p->pkpoint_day = check_same_period(pk_uptime, repeat_day, 0) ? p->pkpoint_day : 0;

	memcpy(&p->relationship, body + len, sizeof(p->relationship));
	len += sizeof(p->relationship);

	UNPKG_H_UINT32(body, frcnt, len);
	UNPKG_H_UINT32(body, p->all_petcnt, len);
	
	UNPKG_H_UINT32(body, skill_cnt, len);
	UNPKG_H_UINT32(body, pet_cnt, len);
	UNPKG_H_UINT32(body, cloth_cnt, len);
	UNPKG_H_UINT32(body, cloth_inbag_cnt, len);
	UNPKG_H_UINT32(body, item_cnt, len);
	UNPKG_H_UINT32(body, task_cnt, len);
	UNPKG_H_UINT32(body, honor_cnt, len);

	fix_len = len + pet_cnt * sizeof(pet_db_t) + cloth_cnt * sizeof(body_cloth_t) 
		+ skill_cnt * sizeof(skill_t) + cloth_inbag_cnt * sizeof(body_cloth_t) 
		+ item_cnt * sizeof (item_t) + task_cnt * 4 + honor_cnt * 4;

	CHECK_BODY_LEN_GE(bodylen, fix_len);

	len += cache_update_skills(p, (skill_t*)(body + len), skill_cnt);
	pet_buf_len = bodylen - fix_len + sizeof(pet_db_t) * pet_cnt;
	if(update_user_pets(p, body + len, pet_buf_len, pet_cnt)) return -1;
	len += pet_buf_len;
	len += cache_update_body_cloths(p, (body_cloth_t *)(body + len), cloth_cnt);
	cache_set_cloth_attr(p);
	len += add_user_bag_cloths(p, (body_cloth_t *)(body + len), cloth_inbag_cnt);
	len += cache_init_bag_items(p, (item_t *)(body + len), item_cnt);
	len += task_cnt * 4;
	len += update_user_honor(p, (uint32_t *)(body + len), honor_cnt);
	
	/*add by francisco 2012-5-24 vip可以获得超时空勇士称号*/
	honor_attr_t* pha = get_honor_attr(VIP_TITLE);//超时空勇士称号
	if (ISVIP(p->flag) && pha && !check_honor_exist(p, VIP_TITLE)) {
		DEBUG_LOG( "ADD HONOR id=%u", VIP_TITLE);
		update_one_honor(p, pha);
		set_user_honor(p, VIP_TITLE);
		notify_user_honor_up(p, 1, 0, 1);
		db_add_user_honor(NULL, p->id, 0, 0, VIP_TITLE);
	}
	if(!ISVIP(p->flag) && p->honor==VIP_TITLE){
		//not vip and has viptitle  卸掉vip称号
		set_user_honor(p, 0);
		db_set_user_honor(NULL, p->id, 0);
	}

	p->pettype = 0;
	p->cur_fish_cnt = -1;
	p->busy_state = be_idle;
	p->pk_switch = pk_off;
	modify_sprite_second_attr(p);
	p->last_fight_time = 0;
	p->double_exptime = 0xFFFFFFFF;
	p->game_type = -1;
	//p->seq = rand() * p->id;
	//p->seq = p->seq < 0 ? 1 : p->seq;
	p->pos_in_line=-1;
	p->draw_max_cnt=DRAW_GIFT_CNT;
	log_sprite(p);

	KDEBUG_LOG(p->id, "LOGIN SUCC\t[%u %u %u %u\t%u %u %u %u %u %u %u\t%u %u %u %u %u %u %u %u]", \
		get_cli_ip(p->fdsess), logout_time, p->online_time, p->recv_map, \
		p->expbox, p->energy, p->pkpoint_total, p->pkpoint_day, pk_uptime, frcnt, p->all_petcnt, \
		skill_cnt, pet_cnt, cloth_cnt, cloth_inbag_cnt, item_cnt, task_cnt, honor_cnt, p->honor);
	
	msg_log_user_on(p->id);
	msg_log_user_xiaomee(p->id, p->xiaomee);
	msg_log_user_friend(p->id, frcnt);
	msg_log_pet_cnt(p->id, p->all_petcnt);
	msg_log_pk_point(p->id, p->pkpoint_total);
	try_to_send_for_login(p, logout_time, (uint32_t)time(0));

	if (!IS_SPRITE_VISIBLE(p)) {
		KDEBUG_LOG(p->id, "SET VISIBLE");
		p->flag &= ~(1 << flag_hide_bit);
	}

	if(ISVIP(p->flag)) {
		int diff = (int)p->vip_end - (int)time(NULL);
		if(diff > 0) {
			if(!(p->flag & (1 << flag_vip_1mail))) {
				send_dbmail(p,23);
				p->flag |= (1 << flag_vip_1mail);
				db_set_flag(NULL, p->id, flag_vip_1mail, 1);
			} else if(!(p->flag & (1 << flag_vip_3mail)) && diff <= 7 * 3600 * 24) {
				send_dbmail(p,25);
				send_dbmail(p,27);
				p->flag |= (1 << flag_vip_3mail);
				db_set_flag(NULL, p->id, flag_vip_3mail, 1);
			} else if(!(p->flag & (1 << flag_vip_2mail)) && diff <= 14 * 3600 * 24) {
				send_dbmail(p,24);
				p->flag |= (1 << flag_vip_2mail);
				db_set_flag(NULL, p->id, flag_vip_2mail, 1);
			}
		}
	} else if(IS_EVER_VIP(p)) {
		if(!(p->flag & (1 << flag_vip_4mail))) {
			send_dbmail(p,26);
			p->flag |= (1 << flag_vip_4mail);
			db_set_flag(NULL, p->id, flag_vip_4mail, 1);
		}
	}

	if(IS_NEW_PROF(p->prof) && p->prof_lv == 1 ){
		p->prof_lv=2;
		db_add_user_honor(NULL, p->id, 2, p->honor, p->honor);
	}
	monitor_sprite(p, "LOG IN");
	return send_request_to_db(p, p->id, proto_db_get_task_all, NULL, 0);
}

int proc_final_login_step(sprite_t* p)
{
	uint16_t newposX = *(uint32_t *)p->session;
	uint16_t newposY = *(uint32_t *)(p->session + 4);
	map_id_t newmap = *(uint64_t *)(p->session + 8);
	if (!BE_INITED(p->flag)) {
		newmap = 11101; newposX = 395; newposY = 320;
	} else if (!cache_check_task_fin(p, 1)) {
		newmap = 11102; newposX = 395; newposY = 320;
	} else if (IS_COPY_MAP(newmap)) {
		bool back_to_copy = false;
		map_copy_instance_t* pmci = get_map_copy_with_mapid(newmap);
		if (pmci) {
			for (uint32_t loop = 0; loop < pmci->usercnt; loop ++) {
				if (pmci->uids[loop] == p->id) {
					back_to_copy = true;
					pmci->ustate[loop] = 1;
				}
			}
		} 
		if (!back_to_copy) {
			newmap = 11202; newposX = 337; newposY = 997;
		}
	} else if (!newmap || !get_map(newmap)){
		newmap = 11202; newposX = 337; newposY = 997;
	}
	
	if (enter_map(p, newmap) < 0) {
		KERROR_LOG(p->id, "enter online map failed");
		return -1;
	}

	p->startX = p->posX = newposX;
	p->startY = p->posY = newposY;

//	KDEBUG_LOG(p->id, "profffffffffffff from final\t%u", p->prof);
	return send_request_to_db(p, p->id, proto_db_get_bird_maps, NULL, 0);
}

//-----------------------------------------------------------
// static function definations
//-----------------------------------------------------------
int update_pet(sprite_t* p, pet_t *p_p, stru_pet_info *db)
{
	uint32_t i;
	int compensation = 1, talent_skill_cnt = 0;

	memset(p_p, 0, sizeof(pet_t));
	p_p->petid = db->gettime;
	p_p->pettype = db->pettype;
	p_p->race = db->race;
	p_p->flag = db->flag;
	memcpy(p_p->nick, db->nick, USER_NICK_LEN);
	p_p->level = db->level;
	p_p->experience = db->exp;
	p_p->base_attr_db.physi_init100 = db->physiqueinit;
	p_p->base_attr_db.stren_init100 = db->strengthinit;
	p_p->base_attr_db.endur_init100 = db->enduranceinit;
	p_p->base_attr_db.quick_init100 = db->quickinit;
	p_p->base_attr_db.intel_init100 = db->iqinit;
	p_p->base_attr_db.physi_para10 = db->physiqueparam;
	p_p->base_attr_db.stren_para10 = db->strengthparam;
	p_p->base_attr_db.endur_para10 = db->enduranceparam;
	p_p->base_attr_db.quick_para10 = db->quickparam;
	p_p->base_attr_db.intel_para10 = db->iqparam;
	p_p->base_attr_db.physi_add = db->physique_add;
	p_p->base_attr_db.stren_add = db->strength_add;
	p_p->base_attr_db.endur_add = db->endurance_add;
	p_p->base_attr_db.quick_add = db->quick_add;
	p_p->base_attr_db.intel_add = db->iq_add;
	p_p->attr_addition = db->attr_addition;
	p_p->earth = db->earth;
	p_p->water = db->water;
	p_p->fire = db->fire;
	p_p->wind = db->wind;
	p_p->injury_lv = 0 ;//db->injury_state;
	p_p->action_state = db->location;

	p_p->skill_cnt = db->skills.size();
	if(p_p->skill_cnt > MAX_SKILL_BAR) {
		p_p->skill_cnt = MAX_SKILL_BAR;
	}

	for(i = 0; i < p_p->skill_cnt; i++) {
		p_p->skills[i].skill_id = db->skills[i].skillid;
		p_p->skills[i].skill_level = db->skills[i].level;
		if(p_p->skills[i].skill_level == 0xFF) {
			p_p->skills[i].skill_level = p_p->level / 10 + 1;
			p_p->skills[i].skill_exp = 1;
			if (talent_skill_cnt >= 2) {
				p_p->skills[i].skill_exp = 0;
			}
			compensation = 0;
			talent_skill_cnt ++;
		}
	}

	calc_pet_5attr(p_p);
	calc_pet_second_level_attr(p_p);
	p_p->hp = db->hp > p_p->hp_max ? p_p->hp_max : db->hp;
	p_p->mp = db->mp > p_p->mp_max ? p_p->mp_max : db->mp;
	return 0;
}

int update_cloths(body_cloth_t* pc, stru_cloth_info* db)
{
	pc->clothid = db->gettime;
	pc->clothtype = db->clothid;
	pc->grid = db->gridid;
	pc->hp_max = db->hpmax;
	pc->mp_max = db->mpmax;
	pc->clothlv = db->level;
	pc->duration = db->duration;
	pc->duration_max = db->mduration;
	pc->attack = db->atk;
	pc->mattack = db->matk;
	pc->defense = db->defense;
	pc->mdefense = db->mdef;
	pc->speed = db->speed;
	pc->spirit = db->spirit;
	pc->resume = db->resume;
	pc->hit = db->hit;
	pc->dodge = db->dodge;
	pc->crit = db->crit;
	pc->fightback = db->fightback;	
	pc->rpoison = db->rpoison;	
	pc->rlithification = db->rlithification;	
	pc->rlethargy = db->rlethargy;	
	pc->rinebriation = db->rinebriation;	
	pc->rconfusion = db->rconfusion;	
	pc->roblivion= db->roblivion;
	pc->quality = 0;
	return 0;
}
int mole2_online_login(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	return 0;
}
/*
int mole2_online_login(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	uint32_t i;
	CHECK_DBERR(p, ret);
	mole2_online_login_out *p_out=P_OUT;

	memcpy(p->nick,p_out->nick,USER_NICK_LEN);
    p->flag = p_out->flag;
    p->color = p_out->color;
    p->register_time = p_out->register_time;
    p->race = p_out->race;
    p->prof = p_out->professtion;
	p->prof_lv = p_out->joblevel;
	p->honor = p_out->honor;
	p->xiaomee = p_out->xiaomee;
    p->level = p_out->level;
    p->experience = p_out->experience;
	p->physique = p_out->physique;
	p->strength = p_out->strength;
	p->endurance = p_out->endurance;
	p->quick = p_out->quick;
	p->intelligence = p_out->intelligence;
	p->attr_addition = p_out->attr_addition;
	p->hp = p_out->hp;
	p->mp = p_out->mp;
	p->injury_lv = p_out->injury_state;
	p->in_front = p_out->in_front;
	p->max_cloth_cnt = p_out->max_attire;
	p->max_m_cnt = p_out->max_medicine;
	p->max_c_cnt = p_out->max_stuff;

	p->online_time = p_out->daytime;
	p->recv_map = p_out->fly_mapid ? p_out->fly_mapid : 11102;

	p->expbox = p_out->expbox;
	p->energy = p_out->energy;
	p->skill_expbox = p_out->skill_expbox;
	p->uiflag = p_out->flag_ex;
	p->last_beastgrp = p_out->winbossid;
	p->parent_id = p_out->parent;
	p->vip_auto = p_out->vipargs.type;
	p->vip_exp_ex = p_out->vipargs.ex_val;
	p->vip_base_exp = p_out->vipargs.base_val;
	p->vip_end = p_out->vipargs.end_time;
	p->vip_begin = p_out->vipargs.begin_time;
	
	recalc_vip_exp_level(p);

	p->pkpoint_total = p_out->battle.all_score;
	p->pkpoint_day = check_same_period(p_out->battle.update_time, repeat_day, 0) ? p_out->battle.day_score : 0;

	if (!p->online_time) {
		uint32_t max_energy = max_user_energy(p);
		p->energy += max_energy * get_day_interval(p->login_time, p_out->time) / 5;
		p->energy = p->energy > max_energy ? max_energy : p->energy;
		db_set_energy(NULL, p->id, p->energy);
	}
	
	log_sprite(p);

	p->all_petcnt = p_out->all_pet_cnt;

	p->skill_cnt = 0;
	p->life_skill_cnt = 0;
	for(i = 0; i < p_out->skills.size(); i++) {
		if(IS_LIFE_SKILL(p_out->skills[i].skillid)) {
			p->life_skills[p->life_skill_cnt].skill_id = p_out->skills[i].skillid;
			p->life_skills[p->life_skill_cnt].skill_level = p_out->skills[i].level;
			p->life_skills[p->life_skill_cnt].skill_exp = p_out->skills[i].exp;
			p->life_skill_cnt++;
		} else if(IS_BATTLE_SKILL(p_out->skills[i].skillid)) {
			p->skills[p->skill_cnt].skill_id = p_out->skills[i].skillid;
			p->skills[p->skill_cnt].skill_level = p_out->skills[i].level;
			p->skills[p->skill_cnt].skill_exp = p_out->skills[i].exp;
			p->skill_cnt++;
		} else {
			
		}
	}

	p->pet_cnt = 0;
	p->pet_cnt_assist = 0;
	p->pet_cnt_standby = 0;
	for(i = 0; i < p_out->pets.size(); i++) {
		if (p_out->pets[i].flag & pet_follow_flag)
			p->p_pet_follow = p->pets_inbag + p->pet_cnt;
		switch(p_out->pets[i].location) {
			case for_assist:
				p->pet_cnt++;
				p->pet_cnt_assist++;
				break;
			case rdy_fight:
			case on_standby:
				p->pet_cnt++;
				p->pet_cnt_standby++;
				break;
		}

		update_pet(p, p->pets_inbag + p->pet_cnt, &p_out->pets[i]);
	}

	g_hash_table_remove_all(p->body_cloths);
	g_hash_table_foreach_remove(p->all_cloths, free_body_cloth, 0);
	for (i = 0; i < p_out->cloths.size(); i++) {
		cloth_t* p_sc = get_cloth(p_out->cloths[i].clothid);
		if (!p_sc || p_out->cloths[i].level >= MAX_CLOTH_LEVL_CNT || !p_sc->clothlvs[p_out->cloths[i].level].valid) {
			KERROR_LOG(p->id, "Invalid cloth id\t[%u %u]", p_out->cloths[i].clothid, p_out->cloths[i].level);
			continue;
		}
		body_cloth_t* pc = (body_cloth_t *)g_slice_alloc(sizeof(body_cloth_t));
		update_cloths(pc,&p_out->cloths[i]);
		g_hash_table_insert(p->all_cloths, &pc->clothid, pc);
		g_hash_table_replace(p->body_cloths, &pc->grid, pc);
	}
	for (i = 0; i < p_out->cloths_in_bag.size(); i ++) {
		body_cloth_t* pc = (body_cloth_t *)g_slice_alloc(sizeof(body_cloth_t));
		update_cloths(pc,&p_out->cloths_in_bag[i]);
		g_hash_table_insert(p->all_cloths, &pc->clothid, pc);
	}
	
	p->card_grid = 0;
	p->chemical_grid = 0;
	p->collection_grid = 0;
	for (i = 0; i < p_out->items.size(); i++) {
		normal_item_t* pni = get_item(p_out->items[i].itemid);
		if (!p_out->items[i].count || !pni)
			continue;

		item_t* pitem = (item_t *)g_slice_alloc(sizeof(item_t));
		pitem->itemid = p_out->items[i].itemid;
		pitem->count  = p_out->items[i].count;
		g_hash_table_insert(p->bag_items, &pitem->itemid, pitem);

		uint32_t need_grid = get_need_grid(pni, 0, pitem->count);

		switch (pni->type) {
			case normal_item_medical:			
				p->chemical_grid += need_grid;
				break;
			case normal_item_collection:
				p->collection_grid += need_grid;
				break;
			case normal_item_card:
				p->card_grid += need_grid;
				break;
		}
	}
	KDEBUG_LOG(p->id, "USER ITEM\t[%u %u %u]", p->collection_grid, p->chemical_grid, p->card_grid);
	for (i = 0; i < p_out->titles.size(); i++) {
		honor_attr_t* pha = get_honor_attr(p_out->titles[i]);
		if (pha) {
			g_hash_table_insert(p->user_honors, &pha->honor_id, pha);
		}
	}

	cache_set_cloth_attr(p);
	
	p->pettype = 0;
	p->busy_state = be_idle;
	p->pk_switch = pk_off;
	modify_sprite_second_attr(p);
	p->last_fight_time = 0;
	p->double_exptime = 0xFFFFFFFF;
	p->seq = rand() * p->id;
	p->seq = p->seq < 0 ? 1 : p->seq;
	if (!IS_SPRITE_VISIBLE(p)) {
		p->flag &= ~(1 << flag_hide_bit);
	}
	try_to_send_for_login(p, p_out->time, (uint32_t)time(0));

	proc_final_login_step(p);
	
	log_sprite(p);

	return 0;
}
*/


