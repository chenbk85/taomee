#include <glib.h>
#include <math.h>
#include <async_serv/async_serv.h>


#include "cli_proto.h"
#include "center.h"
#include "sns.h"
#include "pet.h"
#include "items.h"
#include "beast.h"
#include "battle.h"
#include "skill.h"
#include "honor.h"
#include "homemap.h"
#include "box.h"
#include "mail.h"
#include "task_new.h"
#include "mail.h"
#include "activity.h"
#include "buff.h"
#include "npc.h"
#include "./proto/mole2_home.h"
#include "./proto/mole2_home_enum.h"


int load_holiday_factor(xmlNodePtr cur)
{
	int loop = 0;
	for (loop = 0; loop < DAYS_PER_WEEK; loop ++) {
		holidy_factor[loop].timetype = 0;
		holidy_factor[loop].half_hour_ratio = 200;
		holidy_factor[loop].two_hour_ratio = 100;
		holidy_factor[loop].five_hour_ratio = 50;
	}
	
	uint32_t weekday, holiday;
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Day")) {
			DECODE_XML_PROP_UINT32(weekday, cur, "ID");
			DECODE_XML_PROP_UINT32(holiday, cur, "TimeType");
			if (weekday < 1 || weekday > DAYS_PER_WEEK) {
				ERROR_RETURN(("invalid weekday\t[%u %u]", weekday, holiday), -1);
			}
			if (holiday != 0 && holiday != 1) {
				ERROR_RETURN(("invalid weekday\t[%u %u]", weekday, holiday), -1);
			}
			holidy_factor[weekday % 7].timetype = holiday;
			decode_xml_prop_uint32_default(&holidy_factor[weekday % 7].half_hour_ratio, cur, "HalfHourRatio", 200);
			decode_xml_prop_uint32_default(&holidy_factor[weekday % 7].two_hour_ratio, cur, "TwoHourRatio", 100);
			decode_xml_prop_uint32_default(&holidy_factor[weekday % 7].five_hour_ratio, cur, "FiveHourRatio", 50);

			KTRACE_LOG(12, "HOLIDAY FACTOR\t[%u %u %u %u]", \
				holidy_factor[weekday % 7].timetype, \
				holidy_factor[weekday % 7].half_hour_ratio, \
				holidy_factor[weekday % 7].two_hour_ratio, \
				holidy_factor[weekday % 7].five_hour_ratio);
		}
		cur = cur->next;
	}

	return 0;

}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 获得经因子
 * 周一～周四 2小时内（前0.5小时x倍 后1.5小时1倍）后3小时0.5倍
 * 			  x=2
 * 周五～周日 3小时内（前2小时x倍 后1小时1倍）后2小时0.5倍
 * 			  13~15 19~21 x=3 其他时间段x=2
 *
 * @param  p
 *
 * @return 经验因子
 */
/* ----------------------------------------------------------------------------*/
uint32_t get_exp_factor(sprite_t* p)
{
	int day_idx = get_now_tm()->tm_wday;
	if (IS_ADMIN(p)) return 300;
	uint32_t online_time = p->online_time + (get_now_tv()->tv_sec - p->login_time);
	double factor=1.0;//周末三倍经验系数，三倍时为1.5
	uint32_t now_time=get_now_sec();
	//if( (day_idx==5 || day_idx==6 || day_idx==0) &&//周五 六 日 
			//((now_time > 13*3600 && now_time < 15*3600) || (now_time > 19*3600 && now_time < 21*3600))){//13~15 17~21
		//KDEBUG_LOG(p->id,"周末三倍经验开启");
		//factor=1.5;	
	//}
	if((now_time > 13*3600 && now_time < 15*3600) || (now_time > 19*3600 && now_time < 21*3600)){//13~15 17~21
		factor=1.5;	
	}
	//得到剩余的多倍经验时间
	uint32_t left_dexp_time = get_left_double_exp_time(p, online_time);
	//KDEBUG_LOG(p->id,"get exp factor left_dexp_time=%X online_time= %u %u",left_dexp_time,online_time,p->double_exptime);
	if (left_dexp_time == 0xFFFFFFFF || left_dexp_time == 0) {//没有多倍经验时间
		if (online_time < 2 * 60 * 60) 
			return holidy_factor[day_idx].two_hour_ratio;
		else if (online_time < 3 * 60 * 60) 
			return (uint32_t)(holidy_factor[day_idx].five_hour_ratio*factor);
		else if (online_time < 5 * 60 * 60)
			return holidy_factor[day_idx].timetype == 1 \
			? (uint32_t)(holidy_factor[day_idx].five_hour_ratio*factor)/2 \
			: (uint32_t)(holidy_factor[day_idx].five_hour_ratio*factor);
	} else {//有
			uint32_t ret=online_time < 3 * 60 * 60 \
			? holidy_factor[day_idx].half_hour_ratio \
			: holidy_factor[day_idx].half_hour_ratio / 2;
			//DEBUG_LOG("get exp factor %u %f",ret,factor);
			return  (uint32_t)(ret*factor);
	}
	return 0;
}

uint32_t get_user_exp_factor(sprite_t* p, uint32_t exp_factor)
{
	for (uint32_t loop = 0; loop < p->vip_buffs[buffid_exp][0]; loop ++) {
		uint32_t exp_buff = p->vip_buffs[buffid_exp][1 + 2 * loop];
		exp_factor = exp_factor * exp_buff / 100;
		if (exp_factor > 300) 
			exp_factor = 300;
	}

	KDEBUG_LOG(p->id, "EXP FACTOR\t[%u]", exp_factor);

	return exp_factor;
}

uint32_t get_pet_exp_factor(sprite_t* p, pet_t* pp, uint32_t exp_factor)
{
	for (uint32_t loop = 0; loop < p->vip_buffs[buffid_pet_exp][0]; loop ++) {
		uint32_t exp_buff = p->vip_buffs[buffid_pet_exp][1 + 2 * loop];
		exp_factor = exp_factor * exp_buff / 100;
		if (exp_factor > 300)
			return 300;
	}
	
	return exp_factor > 300 ? 300 : exp_factor;
}

uint32_t get_skill_exp_factor(sprite_t* p, uint32_t exp_factor)
{
	for (uint32_t loop = 0; loop < p->vip_buffs[buffid_skill_exp][0]; loop ++) {
		uint32_t exp_buff = p->vip_buffs[buffid_skill_exp][1 + 2 * loop];

		exp_factor = exp_factor * exp_buff / 100;
		if (exp_factor > 300)
			return 300;
	}

	return exp_factor > 300 ? 300 : exp_factor;
}

void free_batter_team(void* pt)
{
	batter_team_t* battle = (batter_team_t *)pt;
	int loop;
	for (loop = 0; loop < battle->count; loop++) {
		sprite_t* p = battle->players[loop];
		if (p) {
			KDEBUG_LOG(battle->teamid, "RELEASE TEAM USER\t[%u]", p->id);
			p->btr_team = NULL;
		}
	}
	REMOVE_TIMERS(battle);

	KDEBUG_LOG(battle->teamid, "FREE BATTLE TEAM");
	g_slice_free1(sizeof(batter_team_t), pt);
	--batter_team_cnt;
}

void init_batter_teams()
{
	batter_teams = g_hash_table_new(g_int_hash, g_int_equal);
	batter_team_cnt = 0;
}

static gboolean free_bt_by_key(gpointer key, gpointer bt, gpointer userdata)
{
	free_batter_team(bt);
	return TRUE;
}

void fini_batter_teams()
{
	g_hash_table_foreach_remove(batter_teams, free_bt_by_key, 0);
	g_hash_table_destroy(batter_teams);
	batter_team_cnt = 0;
}

void free_batter_team_by_id(uint32_t teamid)
{
	g_hash_table_remove(batter_teams, &(teamid));
}

batter_team_t* alloc_batter_team(sprite_t* p)
{
	batter_team_t* pt = (batter_team_t*)g_slice_alloc0(sizeof(batter_team_t));
	if (!pt) {
		KERROR_LOG(p->id, "alloc batter_team_t fail");
		return pt;
	}

	INIT_LIST_HEAD(&pt->timer_list);
	KDEBUG_LOG(p->id, "ALLOC BATTLE TEAM");
	pt->teamid = p->id;
	pt->count = 1;
	pt->players[0] = p;
	
	p->btr_team = pt;
	g_hash_table_replace(batter_teams, &(pt->teamid), pt);
	++batter_team_cnt;

	return pt;
}

void change_batter_team_id(batter_team_t* pt, uint32_t new_temid)
{
	g_hash_table_steal(batter_teams, &(pt->teamid));
	pt->teamid = new_temid;
	g_hash_table_replace(batter_teams, &(pt->teamid), pt);
}

void free_batter_info(void* pbi)
{
	battle_t* battle = (battle_t*)pbi;
	for (int loop = 0; loop < battle->count; loop++) {
		sprite_t* p = battle->players[loop];
		if (p) {
			p->btr_info = NULL;
			KDEBUG_LOG(p->id, "EXIT BATTER\t[%u %u %u]", \
				high32_val(battle->battle_id), low32_val(battle->battle_id), low32_val(battle->team_id));
			KDEBUG_LOG(p->id, "team_state=%u busy_state=%u",p->team_state,p->busy_state);
			if (p->team_state == load_bt_succ || (!p->team_state && p->busy_state == be_fighting)) {
				p->team_state = 0;
				KDEBUG_LOG(p->id, "CANCEL BATTLE STATE WHEN FREE BATTLE");
				p->busy_state = be_idle;
				notify_busy_state_update(p, sns_state, p->busy_state);
			}
			switch (p->waitcmd) {
				case proto_cli_challenge:
				case proto_cli_challenge_npc:
				case proto_cli_accept_challenge:
				case proto_cli_load_battle:
				case proto_cli_battle_attack:
				case proto_cli_use_medical:
				case proto_cli_load_ready:
				case proto_cli_reject_challenge:
				case proto_cli_use_beast_card:
				case proto_cli_battle_swap_pet:
					KDEBUG_LOG(p->id, "WAIT CMD WHEN FREE BATTLE\t[%u]", p->waitcmd);
					p->waitcmd = 0;
					break;
			}
		}
	}
	//clear watchers's busy_state
    for(std::vector<uint32_t>::iterator it =battle->watchers->begin(); it != battle->watchers->end(); it++){
		sprite_t *s=get_sprite(*it);
		if(s){
			s->busy_state=be_idle;
		}
	}
	KDEBUG_LOG(low32_val(battle->team_id), "FREE BATTLE INFO\t[%u %u]", high32_val(battle->battle_id), low32_val(battle->battle_id));
	delete battle->watchers;	
	g_slice_free1(sizeof(battle_t), pbi);
	--batter_info_cnt;
}

void init_batter_infos()
{
	batter_infos = g_hash_table_new(g_int64_hash, g_int64_equal);
	batter_info_cnt = 0;
}

static gboolean free_bi_by_key(gpointer key, gpointer bi, gpointer userdata)
{
	free_batter_info(bi);
	return TRUE;
}

void fini_batter_infos()
{
	//DEBUG_LOG(" BT fini_batter_infos");
	g_hash_table_foreach_remove(batter_infos, free_bi_by_key, 0);
	g_hash_table_destroy(batter_infos);
	batter_info_cnt = 0;
}

battle_t* get_batter_info(uint32_t teamid, battle_id_t btid)
{
	//DEBUG_LOG("BT get_batter_info btid");
	battle_id_t tid = GET_BATTLE_ID(teamid, btid);
	//DEBUG_LOG("BT get_batter_info btid:%lu",tid );
	return (battle_t*)g_hash_table_lookup(batter_infos, &tid);
}

void free_batter_info_by_leader(battle_t* abi)
{
	battle_id_t teamid = abi->team_id;
	DEBUG_LOG("BT free btid:%lu",teamid );
	g_hash_table_remove(batter_infos, &teamid);
	free_batter_info(abi);
}

battle_t* alloc_batter_info(sprite_t* p, battle_id_t btid)
{
	battle_t* pbi = (battle_t*)g_slice_alloc0(sizeof(battle_t));
	pbi->battle_id = btid;
	pbi->team_id = GET_BATTLE_ID(p->id, btid);
	pbi->state = bt_state_init;
	KDEBUG_LOG(p->id, "ALLOC BATTLE INFO\t[%u %u]", high32_val(pbi->battle_id), low32_val(pbi->battle_id));
	p->btr_info = pbi;

	//DEBUG_LOG("BT add btid:%lu",pbi->team_id );
	g_hash_table_replace(batter_infos, &(pbi->team_id), pbi);
	++batter_info_cnt;
	pbi->watchers=new std::vector<uint32_t>;
	return pbi;
}

static inline gboolean do_kick_user_out_battle(gpointer key, gpointer spri, gpointer userdata)
{
	battle_t* pbt = (battle_t*) spri;
	if(pbt && ((BATTLE_USER_ID(pbt->battle_id) % MAX_BATRSERV_NUM) == *(uint32_t *)userdata)) {
		KDEBUG_LOG(low32_val(pbt->team_id), "KICK USER OUT BATTLE\t[%u %u svrid=%u]", \
			high32_val(pbt->battle_id), low32_val(pbt->battle_id), *(int *)userdata);
		uint8_t buff[64];
		int i = sizeof(protocol_t);
		init_proto_head(buff, proto_cli_btrsvr_conn_closed, i);
		send_to_battle(pbt, buff, i);
		free_batter_info(pbt);
		return TRUE;
	}
	return FALSE;
}

int kick_users_out_battle(int btidx)
{
	DEBUG_LOG(" BT clear  all btidx=%d",btidx);
	g_hash_table_foreach_remove(batter_infos, do_kick_user_out_battle, &btidx);
	return 0;
}

void response_team_info(sprite_t* p)
{
	int j = sizeof(protocol_t);
	if (p->btr_team) {
		PKG_UINT32(msg, p->btr_team->teamid, j);
		PKG_STR(msg, p->btr_team->players[0]->team_nick, j, TEAM_NICK_LEN);
		PKG_UINT32(msg, p->btr_team->count, j);
		int loop;
		for (loop = 0; loop < p->btr_team->count; loop++) {
			sprite_t* s = p->btr_team->players[loop];
			PKG_UINT32(msg, s->id, j);
			PKG_MAP_ID(msg, (s->tiles ? s->tiles->id : 0), j);
			PKG_UINT32(msg, s->level, j);
			PKG_UINT32(msg, s->hp, j);
			PKG_UINT32(msg, s->hp_max, j);
			PKG_UINT32(msg, s->mp, j);
			PKG_UINT32(msg, s->mp_max, j);
			PKG_UINT32(msg, s->injury_lv, j);
			/*
			PKG_UINT32(msg, p->npc_cnt, j);
			int l = 0;
			for (l = 0; l < s->npc_cnt; l ++ ) {
				PKG_UINT32(msg, s->battle_npc[l]->id, j);
			}
			*/
		}

		KDEBUG_LOG(p->id, "RSP TEAM INFO");
		init_proto_head(msg, proto_cli_rsp_team_info, j);
		send_to_map(p, msg, j, 0,1);
	}/* else {
		PKG_UINT32(msg, p->id, j);
		PKG_UINT32(msg, 1, j);
		PKG_UINT32(msg, p->id, j);
		PKG_MAP_ID(msg, (p->tiles ? p->tiles->id : 0), j);
		PKG_UINT32(msg, p->level, j);
		PKG_UINT32(msg, p->hp, j);
		PKG_UINT32(msg, p->hp_max, j);
		PKG_UINT32(msg, p->mp, j);
		PKG_UINT32(msg, p->mp_max, j);
		PKG_UINT32(msg, p->injury_lv, j);
		PKG_UINT32(msg, p->npc_cnt, j);
		int l = 0;
		for (l = 0; l < p->npc_cnt; l ++ ) {
			PKG_UINT32(msg, p->battle_npc[l]->id, j);
		}
	} */
}
int cli_accept_add_team(sprite_t * p,Cmessage* c_in)
{
	cli_accept_add_team_in *p_in=P_IN;
	sprite_t* s = get_sprite(p_in->uid);
	if (s) {
		if (s->id == p->id) {
			KERROR_LOG(s->id, "add self");
			return -1;
		}

		CHECK_LV_FIT(s, s, 8, MAX_SPRITE_LEVEL);
		CHECK_LV_FIT(s, p, 8, MAX_SPRITE_LEVEL);
		
		if (p->btr_team) {
			KERROR_LOG(p->id, "already join team\t[%u]", p->id);
			return send_to_self_error(p, p->waitcmd, cli_err_alrdy_join_team, 1);
		}

		CHECK_USER_APPLY_PK(p, p);
		CHECK_USER_APPLY_PK(s, p);
		
		CHECK_SELF_BATTLE_INFO(p, p->id);
		CHECK_OTHER_BATTLE_INFO(p, s);
		CHECK_SELF_TEAM_LEADER(s, p->id, 1);
		
		CHECK_TEAM_FULL(s, p, p->npc_cnt + 1);
		if (!p->tiles || !s->tiles || p->tiles->id != s->tiles->id || IS_TRADE_MAP(p->tiles->id) 
				|| IS_TRADE_MAP(p->tiles->id) || IS_TRADE_MAP(s->tiles->id)) {
			KERROR_LOG(s->id, "not same map\t[%u %u %u]", p->id, p->tiles!=NULL, s->tiles!=NULL);
			return send_to_self_error(p, p->waitcmd, cli_err_not_in_same_map, 1);
		}

		if (p->busy_state != be_idle || s->busy_state == hangup) {
			KERROR_LOG(s->id, "busy\t[%u]", p->id);
			return send_to_self_error(p, p->waitcmd, cli_err_user_busy, 1);
		}

		if (team_switch_state(p) == team_refuse) {
			KERROR_LOG(s->id, "refuse all\t[%u]", p->id);
			return send_to_self_error(p, p->waitcmd, cli_err_refuse_team_invite, 1);
		}

		if (add_mbr_to_bt(s, p) == -1) {
			KERROR_LOG(s->id, "not invite you\t[%u]", p->id);
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
		KDEBUG_LOG(p->id, "ADD MBR CCC\t[teamid=%u]", s->id);
		response_proto_head(p, p->waitcmd, 1, 0);
	}else {
		send_to_self_error(p,p->waitcmd,cli_err_user_offline,1);
	}
	return 0;
}

#define text_notify_pkg(buf_, cmd_, uid_, mapid_, nick_, accept_, len_, txt_) \
	text_notify_pkg_pic((buf_), (cmd_), (uid_), (mapid_), (nick_), (accept_), 0, (len_), (txt_))
int req_add_mbr_notifier(sprite_t * p,uint32_t uid)
{
	char txt[256];
	sprite_t* s = get_sprite(uid);
	int j = sprintf(txt, "%s请求将您加入战队，您是否同意？", p->nick);
	j = text_notify_pkg(msg + 4, proto_cli_btrtem_add_mbr, p->id, 0, p->nick, 0, j, txt);
	if (s) {
		return send_to_self(s, msg + 4, j, 0);
	}else {
		return cli_err_user_offline;
	}
}

int do_add_mbr_to_team(sprite_t * p,uint32_t uid)
{
	if(req_add_mbr_notifier(p, uid)){
		return send_to_self_error(p,p->waitcmd,cli_err_user_offline,0);
	}else{
		response_proto_head(p, p->waitcmd, 1, 0);
	}
	KDEBUG_LOG(p->id, "ADD MBR REQUEST\t[%u]", uid);
	return 0;
}

int add_mbr_to_btr_team_cmd(sprite_t * p, uint8_t * body, uint32_t len)
{
	int j = 0;
	userid_t uid;
	UNPKG_UINT32(body, uid, j);
	KDEBUG_LOG(p->id, "ADD MBR\t[%u]", uid);
	if (p->id == uid) {
		KERROR_LOG(p->id, "add self");
		return -1;
	}

	sprite_t* lp = get_sprite(uid);
	if (!lp) {
		KERROR_LOG(p->id, "user not exist\t[%u]", uid);
		return send_to_self_error(p, p->waitcmd, cli_err_user_offline, 1);
	}

	CHECK_LV_FIT(p, p, 8, MAX_SPRITE_LEVEL);
	CHECK_LV_FIT(p, lp, 8, MAX_SPRITE_LEVEL);
	
	if (lp->btr_team) {
		KERROR_LOG(p->id, "already join team\t[%u]", uid);
		return send_to_self_error(p, p->waitcmd, cli_err_alrdy_join_team, 1);
	}

	CHECK_USER_APPLY_PK(lp, p);
	CHECK_USER_APPLY_PK(p, p);
	
	CHECK_SELF_BATTLE_INFO(p, uid);
	CHECK_OTHER_BATTLE_INFO(p, lp);
	CHECK_SELF_TEAM_LEADER(p, uid, 1);
	
	CHECK_TEAM_FULL(p, p, lp->npc_cnt + 1);
	if (!lp->tiles || !p->tiles || lp->tiles != p->tiles) {
		KERROR_LOG(p->id, "not same map\t[%u %p %p]", lp->id, lp->tiles, p->tiles);
		return send_to_self_error(p, p->waitcmd, cli_err_not_in_same_map, 1);
	}

	if (lp->busy_state != be_idle || p->busy_state == hangup) {
		KERROR_LOG(p->id, "busy\t[%u]", lp->id);
		return send_to_self_error(p, p->waitcmd, cli_err_user_busy, 1);
	}

	//if (team_switch_state(lp) == team_refuse) {
		//KERROR_LOG(p->id, "refuse all\t[%u]", lp->id);
		//return send_to_self_error(p, p->waitcmd, cli_err_refuse_team_invite, 1);
	//}

	//if (add_mbr_to_bt(p, lp) == -1) {
		//KERROR_LOG(p->id, "not invite you\t[%u]", lp->id);
		//return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	//}
	
	KDEBUG_LOG(p->id, "ADD MBR CCC\t[%u]", uid);
	//response_proto_head(p, p->waitcmd, 1, 0);
	return do_add_mbr_to_team(p,uid);

}

int apply_for_join_btr_team_new(sprite_t *p, Cmessage* c_in)
{
	apply_for_join_btr_team_new_in *p_in=P_IN;
	apply_for_join_btr_team_new_out cli_out; 
	if (p->id == p_in->teamid) {
		KERROR_LOG(p->id, "self");
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	CHECK_SELF_JOIN_TEAM(p);
	
	sprite_t* lp = get_sprite(p_in->teamid);
	if (!lp) {
		KERROR_LOG(p->id, "leader not exist\t[%u]", p_in->teamid);
		return send_to_self_error(p, p->waitcmd, cli_err_user_offline, 1);
	}

	CHECK_USER_APPLY_PK(lp, p);
	CHECK_USER_APPLY_PK(p, p);
	CHECK_LV_FIT(p, p, 8, MAX_SPRITE_LEVEL);
	CHECK_LV_FIT(p, lp, 8, MAX_SPRITE_LEVEL);
	
	if (lp->btr_info || p->btr_info) {
		KERROR_LOG(p->id, "be fighting\t[%u]", p_in->teamid);
		return send_to_self_error(p, p->waitcmd, cli_err_self_be_fighting, 1);
	}
	batter_team_t* lt = lp->btr_team;
	if (lt && !TEAM_LEADER(lp)) {
		KERROR_LOG(p->id, "not team leader\t[%u]", p_in->teamid);
		return send_to_self_error(p, p->waitcmd, cli_err_other_not_leader, 1);
	}
	CHECK_TEAM_FULL(lp, p, p->npc_cnt + 1);

	if (!lp->tiles || !p->tiles) {
		KERROR_LOG(p->id, "not right map\t[%u]", lp->id);
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
	}

	if (lp->busy_state != be_idle || p->busy_state == hangup) {
		KERROR_LOG(p->id, "busy\t[%u]", lp->id);
		return send_to_self_error(p, p->waitcmd, cli_err_user_busy, 1);
	}

	if (team_switch_state(lp) == team_refuse) {
		KERROR_LOG(p->id, "refuse all\t[%u]", lp->id);
		return send_to_self_error(p, p->waitcmd, cli_err_refuse_team_apply, 1);
	}

	if (add_mbr_to_bt(lp, p) == -1) {
		KERROR_LOG(p->id, "alrdy in team\t[%u]", p_in->teamid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	KDEBUG_LOG(p->id, "APPLY JOIN TEAM NEW SUCC\t[%u cnt=%u]", p_in->teamid, lp->btr_team->count);
	cli_out.mapid=lp->tiles->id;
//	KDEBUG_LOG(p->id, "mapid\t[%lu]",lp->tiles->id);
	return send_msg_to_self(p,p->waitcmd,&cli_out,1);
}

int apply_for_join_btr_team_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t teamid;
	UNPKG_UINT32(body, teamid, j);
	
	if (p->id == teamid) {
		KERROR_LOG(p->id, "self");
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	CHECK_SELF_JOIN_TEAM(p);
	
	sprite_t* lp = get_sprite(teamid);
	if (!lp) {
		KERROR_LOG(p->id, "leader not exist\t[%u]", teamid);
		return send_to_self_error(p, p->waitcmd, cli_err_user_offline, 1);
	}

	CHECK_USER_APPLY_PK(lp, p);
	CHECK_USER_APPLY_PK(p, p);
	CHECK_LV_FIT(p, p, 8, MAX_SPRITE_LEVEL);
	CHECK_LV_FIT(p, lp, 8, MAX_SPRITE_LEVEL);
	
	if (lp->btr_info || p->btr_info) {
		KERROR_LOG(p->id, "be fighting\t[%u]", teamid);
		return send_to_self_error(p, p->waitcmd, cli_err_self_be_fighting, 1);
	}
	batter_team_t* lt = lp->btr_team;
	if (lt && !TEAM_LEADER(lp)) {
		KERROR_LOG(p->id, "not team leader\t[%u]", teamid);
		return send_to_self_error(p, p->waitcmd, cli_err_other_not_leader, 1);
	}
	CHECK_TEAM_FULL(lp, p, p->npc_cnt + 1);

	if (!lp->tiles || !p->tiles || lp->tiles->id != p->tiles->id) {
		KERROR_LOG(p->id, "not same map\t[%u %lu %lu]", lp->id, lp->tiles->id, p->tiles->id);
		return send_to_self_error(p, p->waitcmd, cli_err_not_in_same_map, 1);
	}

	if (lp->busy_state != be_idle || p->busy_state == hangup) {
		KERROR_LOG(p->id, "busy\t[%u]", lp->id);
		return send_to_self_error(p, p->waitcmd, cli_err_user_busy, 1);
	}

	if (team_switch_state(lp) == team_refuse) {
		KERROR_LOG(p->id, "refuse all\t[%u]", lp->id);
		return send_to_self_error(p, p->waitcmd, cli_err_refuse_team_apply, 1);
	}

	if (add_mbr_to_bt(lp, p) == -1) {
		KERROR_LOG(p->id, "alrdy in team\t[%u]", teamid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	KDEBUG_LOG(p->id, "APPLY JOIN TEAM SUCC\t[%u cnt=%u]", teamid, lp->btr_team->count);
	
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

void noti_batsvr_teamate_leave_team(sprite_t* p)
{
	for (int loop = 0; loop < p->btr_info->count; loop ++) {
		sprite_t* s = p->btr_team->players[loop];
		if (s && s != p) {
			notify_batsvr_leave_team(s);
		}
	}
}

int leave_btr_team_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	if (!p->btr_team) {
		KERROR_LOG(p->id, "not join team");
		response_proto_head(p, p->waitcmd, 1, 0);
		return 0;
	}

	if (p->btr_info) {
		if (p->btr_info->state != bt_state_init)
			return send_to_self_error(p, p->waitcmd, cli_err_self_be_fighting, 1);
		noti_batsvr_teamate_leave_team(p);
		del_user_battle_info(p);
	}
	
	del_mbr_from_bt(p->btr_team, p->id);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int kick_outof_btr_team_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	userid_t  uid;
	UNPKG_UINT32(body, uid, j);
	if (p->id == uid) {
		KERROR_LOG(p->id, "self");
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	
	if (!p->btr_team || !TEAM_LEADER(p)) {
		KERROR_LOG(p->id, "not team leader\t[%u]", uid);
		return send_to_self_error(p, p->waitcmd, cli_err_self_not_leader, 1);
	}
	if (get_user_in_bt(p->btr_team, uid) == -1) {
		response_proto_head(p, p->waitcmd, 1, 0);
		return 0;
	}

	sprite_t* lp = get_sprite(uid);
	if (lp && lp->btr_info) {
		if (lp->btr_info->state != bt_state_init)
			return send_to_self_error(p, p->waitcmd, cli_err_self_be_fighting, 1);
		notify_batsvr_leave_team(lp);
		del_user_battle_info(lp);
	}
	
	del_mbr_from_bt(p->btr_team, uid);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

void notify_teamers_enter_map(sprite_t* p, map_id_t mapid, uint16_t x, uint16_t y)
{
	if (!p->btr_team || !TEAM_LEADER(p)) 
		return;
	
	int j = sizeof(protocol_t);
	PKG_MAP_ID(msg, mapid, j);
	PKG_UINT16(msg, x, j);
	PKG_UINT16(msg, y, j);
	init_proto_head(msg, proto_cli_btrtem_enter_map, j);
	send_to_btrtem_except_uid(p->btr_team, msg, j, p->id);
}

void notify_teamers_enter_map_done(sprite_t* p)
{
	if (!p->btr_team || TEAM_LEADER(p)) 
		return;

	for (int loop = 0; loop < p->btr_team->count; loop++) {
		sprite_t* lp = p->btr_team->players[loop];
		if (lp->id == p->btr_team->teamid) {
			if (lp->tiles == p->tiles)
				break;
			else return;
		}
	}

	response_team_info(p);
}

/*******************************************
 	next source for battle team fighting 
********************************************/

void init_challenger_battle(sprite_t* team_leader, int pkapply, int battletype)
{
	battle_t* pba = alloc_batter_info(team_leader, MAKE_BATTLE_ID(team_leader->id));
	pba->battletype = battletype;
	pba->pkapply = pkapply;
	batter_team_t* pbt = team_leader->btr_team;
	if (!pbt) {  // single battle
		pba->count = 1;
		pba->players[0] = team_leader;
		pba->average_lv = team_leader->level;
		pba->toplv = team_leader->level;
		pba->floorlv = team_leader->level;
		team_leader->busy_state = pkapply ? pk_hide : be_fighting;
		notify_busy_state_update(team_leader, sns_state, team_leader->busy_state);
		return;
	}

	// team battle
	uint32_t total_lv = 0;
	pba->toplv = 1;
	pba->floorlv = MAX_SPRITE_LEVEL;
	for (int loop = 0; loop < pbt->count; loop++) {
		sprite_t* lp = pbt->players[loop];
		pba->players[pba->count++] = lp;
		total_lv += lp->level;
		if (pba->toplv < lp->level)
			pba->toplv = lp->level;
		if (pba->floorlv > lp->level)
			pba->floorlv = lp->level;
		lp->btr_info = pba;
		lp->busy_state = pkapply ? pk_hide : be_fighting;
		notify_busy_state_update(lp, sns_state, lp->busy_state);
	}	
	pba->average_lv = total_lv / pba->count;
}

void init_challengee_battle(sprite_t* team_leader, battle_id_t btid, int pkapply)
{
	battle_t* pba = alloc_batter_info(team_leader, btid);
	pba->battletype = be_pk;
	pba->pkapply = pkapply;
	pba->state = bt_state_accept;
	if (pkapply) pba->pkteam = BATTLE_USER_ID(btid);
	batter_team_t* pbt = team_leader->btr_team;
	if (!pbt) {
		pba->count = 1;
		pba->players[0] = team_leader;
		pba->average_lv = team_leader->level;
		pba->toplv = team_leader->level;
		pba->floorlv = team_leader->level;
		team_leader->busy_state = pkapply ? pk_hide : be_fighting;
		notify_busy_state_update(team_leader, sns_state, team_leader->busy_state);
		return;
	}

	uint32_t total_lv = 0;
	pba->toplv = 1;
	pba->floorlv = MAX_SPRITE_LEVEL;
	// team battle
	for (int loop = 0; loop < pbt->count; loop++) {
		sprite_t* lp = pbt->players[loop];
		pba->players[pba->count++] = lp;
		total_lv += lp->level;
		if (pba->toplv < lp->level)
			pba->toplv = lp->level;
		if (pba->floorlv > lp->level)
			pba->floorlv = lp->level;
		lp->btr_info = pba;
		lp->busy_state = pkapply ? pk_hide : be_fighting;
		notify_busy_state_update(lp, sns_state, lp->busy_state);
	}	
	pba->average_lv = total_lv / pba->count;
}

void notify_self_team_challenging(sprite_t *challenger, userid_t challengee_id)
{
	uint8_t buff[256];
	int len = sizeof(protocol_t);
	PKG_UINT32(buff, challengee_id, len);
	init_proto_head(buff, proto_cli_noti_team_challeg, len);
	send_to_self(challenger, buff, len, 0);
}

void notify_other_leader_challenging_state(sprite_t* challengee, battle_id_t btid, uint32_t state)
{
	uint8_t buff[256];
	int len = sizeof(protocol_t);
	PKG_BATTLE_ID(buff, btid, len);
	PKG_UINT32(buff, state, len);
	init_proto_head(buff, proto_cli_noti_team_challeged, len);
	send_to_self(challengee, buff, len, 0);
}

void notify_challger_accept_state(sprite_t* challenger, userid_t challengee, uint32_t state)
{
	uint8_t buff[256];
	int len = sizeof(protocol_t);
	PKG_UINT32(buff, challengee, len);
	PKG_UINT32(buff, state, len);
	init_proto_head(buff, proto_cli_noti_battle_accepted, len);
	send_to_self(challenger, buff, len, 0);
}

static int challenge_user_on_other_online(sprite_t* challenger, uint32_t challengee)
{
	uint8_t buf[12];
	int i = 0;
	PKG_H_UINT64(buf, challenger->btr_info->battle_id, i);
	PKG_H_UINT32(buf, challengee, i);
	return send_to_switch(challenger, COCMD_onli_challenge_battle, sizeof buf, buf, challenger->id);
}

static int auto_challenge_on_other_online(sprite_t* challenger)
{
	uint8_t buf[12];
	int i = 0;
	PKG_H_UINT64(buf, challenger->btr_info->battle_id, i);
	PKG_H_UINT32(buf, challenger->btr_info->pkteam, i);
	return send_to_switch(challenger, COCMD_onli_auto_accept, sizeof buf, buf, challenger->id);
}

body_cloth_t* get_body_show_cloth(sprite_t* p, uint32_t grid)
{
	body_cloth_t* pc = NULL;
	if (p->flag & (1 << flag_fashion_bit)) {
		pc = cache_get_body_cloth(p, grid + BASE_BODY_TYPE + 100);
		if (!pc)
			pc = cache_get_body_cloth(p, grid + BASE_BODY_TYPE);
	} else {
		pc = cache_get_body_cloth(p, grid + BASE_BODY_TYPE);
		if (!pc)
			pc = cache_get_body_cloth(p, grid + BASE_BODY_TYPE + 100);
	}
	return pc;
}

static void pkg_cloth_warrior(sprite_t* p, warrior_t* aw)
{
	aw->cloth_cnt = 0;
	for (uint32_t grid = 0; grid < MAX_ITEMS_WITH_BODY; grid ++) {
		warrior_cloth_t* pwc = &aw->clothes[aw->cloth_cnt];
		body_cloth_t* pc = get_body_show_cloth(p, grid);
		if (pc) {
			pwc->cloth_id = pc->clothid;
			pwc->type_id = pc->clothtype;
			pwc->clevel = pc->clothlv;
			pwc->duration = 0;
			
			pwc->equip_pos = (pc->grid - BASE_BODY_TYPE) % 100;
			aw->cloth_cnt ++;
		}
	}
}

static void pkg_skill_warrior(warrior_skill_t* apws, skill_t* aps, uint32_t skill_cnt) 
{
	for (uint32_t loop = 0; loop < skill_cnt; loop ++) {
		memcpy(&apws[loop].skill_info, &aps[loop], sizeof(skill_t));
		apws[loop].percent = aps[loop].skill_exp;
		if (apws[loop].skill_info.default_lv == 0xFF)
			apws[loop].skill_info.default_lv = apws[loop].skill_info.skill_level;
	}
}

void weapon_type_on_body(sprite_t* p, warrior_t* pw)
{
	body_cloth_t* pc = cache_get_body_cloth(p, BASE_BODY_TYPE + part_shield);
	pw->shield = pc ? 1 : 0;
	pc = cache_get_body_cloth(p, BASE_BODY_TYPE + part_weapon);
	cloth_t* psc = pc ? get_cloth(pc->clothtype) : NULL;
	pw->weapon_type = psc ? psc->weapon_type : 0;
}
static int change_sprite_to_warrior(sprite_t* p, uint32_t uid, warrior_t* aw, int battle_type)
{
	aw->id = 			uid;
	aw->petid = 		0;
	aw->pettype = 		p->pettype;
	memcpy(aw->nick, p->nick, USER_NICK_LEN);
	aw->flag = 			p->flag;
	aw->viplv =			ISVIP(p->flag) ? p->viplv : 0;
	aw->color = 		p->color;
	aw->race = 			p->race;
	aw->prof = 			p->prof;
 	aw->level = 		p->level;
	
	aw->physique = 		p->physique;
	aw->strength = 		p->strength;
	aw->endurance = 	p->endurance;
	aw->quick = 		p->quick;
	aw->intelligence = 	p->intelligence;
	aw->attr_addition = p->attr_addition;
	aw->hp = 			p->hp;
	aw->mp = 			p->mp;
	aw->injury_lv = 	p->injury_lv;
	aw->in_front = 		p->in_front;
	aw->earth = 		p->earth;
	aw->water = 		p->water;
	aw->fire = 			p->fire;
	aw->wind = 			p->wind;
	//second level attr
	aw->hp_max = 		p->hp_max;
	aw->mp_max = 		p->mp_max;
	aw->attack =		p->attack_for_cal;
	aw->mattack = 		p->mattack;
	aw->defense = 		p->defense_for_cal;
	aw->mdefense =		p->mdefense;
	aw->speed = 		p->speed_for_cal;
	aw->spirit = 		p->spirit;
	aw->resume = 		p->resume;
	aw->hit_rate =	 	p->hit_rate;
	aw->avoid_rate = 	p->avoid_rate;
	aw->bisha = 		p->bisha;
	aw->fight_back = 	p->fight_back;
	beast_t* pb=get_beast(p->shapeshifting.pettype);
	aw->change_race = pb?pb->race:0;
	aw->change_petid = pb?p->shapeshifting.pettype:0;
	if( p->challenge_3061 ){//变身任务时 特定变身吉普豆叶
		aw->change_race = 2;
		aw->change_petid = 1;
	}
	//KDEBUG_LOG(p->id,"xxxxx %u %u",pb!=NULL,p->shapeshifting.pettype);
	aw->rpoison =		p->rpoison;
	aw->rconfusion =	p->rconfusion;
	aw->rinebriation =	p->rinebriation;
	aw->rlethargy =		p->rlethargy;
	aw->rlithification = p->rlithification;
	aw->roblivion =		p->roblivion;
	KDEBUG_LOG(p->id,"monitor_sprite_new:%u %u %u %u",aw->hp_max,aw->attack,aw->defense,aw->speed);	
	aw->need_protect_exp = 0;
	if (battle_type == be_pk) {
		//if (p->injury_lv) {
			//update_base_second_attr((base_second_attr_t *)&aw->hp_max, (base_5_attr_t *)&p->physique, (base_5_attr_t *)&p->physique, 0, p->injury_lv);
			//aw->injury_lv = INJURY_NONE;
		//}
		aw->hp = aw->hp_max;
		aw->mp = aw->mp_max;	
	}
	pkg_cloth_warrior(p, aw);
	aw->skill_cnt =		p->skill_cnt;
	pkg_skill_warrior(aw->skills, p->skills, aw->skill_cnt);
	weapon_type_on_body(p, aw);
	aw->pet_cnt_actual = p->pet_cnt_actual;
	aw->catchable = 0;
	aw->handbooklv = 0;
	skill_t* p_si = cache_get_skill(p, skill_life_pet_contact);
	aw->pet_contact_lv = p_si ? p_si->skill_level : 0;
	memcpy(aw->default_atk, p->default_skills, sizeof(aw->default_atk));
	aw->default_atklv[0] = p->default_levels[0];
	aw->default_atklv[1] = p->default_levels[1];
	KDEBUG_LOG(p->id, "PKG SPRITE WARRIOR\t[%u %s %u %u %u %u]", aw->id, p->nick, aw->hp, aw->mp, aw->default_atk[0], aw->default_atk[1]);
	aw->autofight_count = get_auto_fight_count(p);
	return 0;
}

void pkg_beast_skill_warrior(beast_t* pb, warrior_t* aw, uint32_t pkteam)
{
	warrior_skill_t* pskill_run = NULL;
	aw->skill_cnt =		pb->skill_cnt_all;
	for (int loop = 0; loop < aw->skill_cnt; loop ++) {
		uint32_t skill_toplv = get_pet_skill_toplv(pb->skills[loop].skillid);
		skill_toplv = skill_toplv ? skill_toplv : 1;
		uint32_t phaselv = MAX_SPRITE_LEVEL / skill_toplv;
		uint32_t skill_lv = (aw->level + phaselv - 1) / phaselv;
		skill_lv = skill_lv > skill_toplv ? skill_toplv : skill_lv;
		aw->skills[loop].skill_info.skill_id = pb->skills[loop].skillid;
		aw->skills[loop].skill_info.skill_level = skill_lv;
		aw->skills[loop].percent = pb->skills[loop].percent;

		if (pkteam >= 2401 && pkteam <= 2430 && pb->skills[loop].skillid == 5001)
			pskill_run = &aw->skills[loop];

		/*
		KDEBUG_LOG(aw->id, "BEAST SKILL\t[%u %u %u %u]", \
			aw->skill_cnt, aw->skills[loop].skill_info.skill_id, \
			aw->skills[loop].skill_info.skill_level, \
			aw->skills[loop].percent);
		*/
	}

	if (pskill_run == &aw->skills[aw->skill_cnt - 1]) {
		aw->skill_cnt --;
		aw->skills[rand() % aw->skill_cnt].percent += aw->skills[aw->skill_cnt].percent;
	} else if (pskill_run) {
		uint32_t percent = pskill_run->percent;
		aw->skill_cnt --;
		memcpy(pskill_run, &aw->skills[aw->skill_cnt], sizeof(warrior_skill_t));
		aw->skills[rand() % aw->skill_cnt].percent += percent;
	}
}

static int change_beast_to_warrior(beast_t* pb, base_attr_t* pba, warrior_t* aw, uint32_t lv, uint32_t pkteam, uint32_t elite_type)
{
	aw->petid = 			0;
	aw->pettype = 			pb->id;
	memcpy(aw->nick, pb->nick, USER_NICK_LEN);
	aw->race = 				pb->race;
 	aw->level = 			pba->maxlv ? (pba->minlv + rand() % (pba->maxlv - pba->minlv + 1)) : lv;
	
	aw->earth = 			pb->earth;
	aw->water = 			pb->water;
	aw->fire = 				pb->fire;
	aw->wind = 				pb->wind;
	aw->change_race = 0;
	aw->change_petid = 0;
	//DEBUG_LOG("double %u %u",pba->id,pba->double_atk_flag );
	if( pba->double_atk_flag ){
		//DEBUG_LOG("XXXX double_ack_flag " );
		aw->flag |= 0x1;//第二次攻击标记位
	}

	int physi = calc_beast_attr_para10(pb->physi_factor);
	int stren = calc_beast_attr_para10(pb->stren_factor);
	int endur = calc_beast_attr_para10(pb->endur_factor);
	int quick = calc_beast_attr_para10(pb->quick_factor);
	int intel = calc_beast_attr_para10(pb->intel_factor);

	physi = physi * aw->level * 10 / MAGIC_GROW_RATE + physi * 2;
	stren = stren * aw->level * 10 / MAGIC_GROW_RATE + stren * 2;
	endur = endur * aw->level * 10 / MAGIC_GROW_RATE + endur * 2;
	quick = quick * aw->level * 10 / MAGIC_GROW_RATE + quick * 2;
	intel = intel * aw->level * 10 / MAGIC_GROW_RATE + intel * 2;

	int diff[5] = {0, 0, 0, 0, 0};
	for (uint32_t loop = 0; loop < aw->level; loop ++) {
		diff[rand() % 5] += 100;
	}
	physi += diff[0];
	stren += diff[1];
	endur += diff[2];
	quick += diff[3];
	intel += diff[4];

	aw->physique = 			ROUND_100(physi);
	aw->strength = 			ROUND_100(stren);
	aw->endurance = 		ROUND_100(endur);
	aw->quick = 			ROUND_100(quick);
	aw->intelligence =		ROUND_100(intel);

	aw->hp_max 		 = 		ROUND_100(8 * physi + 2 * stren + 3 * endur + 3 * quick + intel);
	aw->mp_max 		 = 		ROUND_100(physi + 2 * stren + 2 * endur + 2 * quick + 10 * intel) + 20;

	switch (elite_type) {
		case 1:
			aw->hp_max *= 2;
			aw->mp_max *= 2;
			aw->viplv |= beast_elite_flag;
			break;
		case 2:
			aw->hp_max *= 3;
			aw->mp_max *= 3;
			aw->viplv |= beast_boss_flag;
			break;
	}
	
	aw->attack = 	ROUND_1000(2 * physi + 27 * stren + 3 * endur + 3 * quick + 2 * intel);
	aw->defense =	ROUND_1000(2 * physi + 3 *stren + 30 * endur + 3 * quick + intel);
	aw->speed = 	ROUND_1000(physi + 2 * stren + 2 * endur + 20 * quick + intel);
	aw->spirit = 	ROUND_1000(-3 * physi - stren + 2 * endur - quick + 8 * intel) + 100;
	aw->resume = 	ROUND_1000(8 * physi - stren - endur + 2 * quick - 3 * intel) + 100;

	if (pba->hp_max) aw->hp_max = aw->hp_max * pba->hp_max / BEAST_COMMON_RAND_BASE;
	if (pba->mp_max) aw->mp_max = aw->mp_max * pba->mp_max / BEAST_COMMON_RAND_BASE;
	if (pba->attack) aw->attack = aw->attack * pba->attack / BEAST_COMMON_RAND_BASE;
	if (pba->mattack) aw->mattack = aw->mattack * pba->mattack / BEAST_COMMON_RAND_BASE;
	if (pba->defense) aw->defense = aw->defense * pba->defense / BEAST_COMMON_RAND_BASE;
	if (pba->speed) aw->speed = aw->speed * pba->speed / BEAST_COMMON_RAND_BASE;

	aw->hp_max *= 5;// for change_prof 2012.3.20

	aw->mdefense +=			pba->mdefense;
	aw->spirit +=			pba->spirit;
	aw->resume +=			pba->resume;
	aw->hit_rate +=			pba->hit;
	aw->avoid_rate +=		pba->dodge;
	aw->bisha +=			pba->crit;
	aw->fight_back +=		pba->fightback;

	aw->need_protect_exp =	0;

	aw->spirit = 			(aw->spirit > 0) ? aw->spirit : 0;
	aw->resume = 			(aw->resume > 0) ? aw->resume : 0;
	aw->hit_rate = 			0;
	aw->avoid_rate = 		0;

	aw->hp = 				aw->hp_max;
	aw->mp = 				aw->mp_max;
	
	pkg_beast_skill_warrior(pb, aw, pkteam);
	int ctype = pba->id / SPEC_BEAST_BASE;
	if (!ctype) {
		beast_lvinfo_t* pbl = get_beast_lvinfo(pb->id, aw->level);
		aw->catchable = pbl ? pbl->catchable : 0;
	} else {
		aw->catchable = ctype < pb->spec_ccnt ? pb->spec_catch[ctype - 1] : 0;
	}
	
	aw->handbooklv = pb->handbooklv;

	//DEBUG_LOG(" double_ack_flag aw->flag:%u", aw->flag);
	return 0;
}


void change_pet_second_attr_warrior(pet_t* p, warrior_t* aw)
{
	aw->hp_max	= ROUND_100(8 * p->physi100 + 2 * p->stren100 + 3 * p->endur100 + 3 * p->quick100 + p->intel100) + 20;
	aw->mp_max	= ROUND_100(p->physi100 + 2 * p->stren100 + 2 * p->endur100 + 2 * p->quick100 + 10* p->intel100) + 20;
	aw->attack	= ROUND_1000(2 * p->physi100 + 27 * p->stren100 + 3 * p->endur100 + 3 * p->quick100 + 2 * p->intel100) + 20;
	aw->defense	= ROUND_1000(2 * p->physi100 + 3 * p->stren100 + 30 * p->endur100 + 3 * p->quick100 + 2 * p->intel100) + 20;
	aw->speed	= ROUND_1000(p->physi100 + 2 * p->stren100 + 2 * p->endur100 + 20 * p->quick100 + p->intel100) + 20;
	aw->spirit	= ROUND_1000(-3 * p->physi100 - p->stren100 + 2 * p->endur100 - p->quick100 + 8 * p->intel100) + 100;
	aw->resume	= ROUND_1000(8 * p->physi100 - p->stren100 - p->endur100 + 2 * p->quick100 - 3 * p->intel100) + 100;

	aw->spirit = (p->spirit > 0) ? p->spirit : 1;
	aw->resume = (p->resume > 0) ? p->resume : 1;

	aw->hit_rate = 0;
	aw->avoid_rate = 0;
}


static int change_pet_to_warrior(sprite_t* ps, pet_t* p, warrior_t* aw, uint32_t uid, int battle_type)
{
	aw->id = 			uid;
	aw->petid = 		p->petid;
	aw->pettype = 		p->pettype;
	
	memcpy(aw->nick, p->nick, USER_NICK_LEN);
	aw->flag = 			p->flag;
	aw->race = 			p->race;
 	aw->level = 		p->level;
	
	aw->change_race = 0;
	aw->change_petid = 0;
	aw->physique = 		ROUND_100(p->physi100);
	aw->strength = 		ROUND_100(p->stren100);
	aw->endurance = 	ROUND_100(p->endur100);
	aw->quick = 		ROUND_100(p->quick100);
	aw->intelligence = 	ROUND_100(p->intel100);
	aw->attr_addition = p->attr_addition;
	aw->hp = 			p->hp > 0 ? p->hp : 1;
	aw->mp = 			p->mp;
	aw->injury_lv = 	p->injury_lv;
	aw->in_front = 		!ps->in_front;
	aw->earth = 		p->earth;
	aw->water = 		p->water;
	aw->fire = 			p->fire;
	aw->wind = 			p->wind;
	//second level attr
	aw->hp_max = 		p->hp_max;
	aw->mp_max = 		p->mp_max;
	aw->attack = 		p->attack/5;
	aw->defense = 		p->defense/5;
	aw->speed = 		p->speed/5;
	aw->spirit = 		p->spirit;
	aw->resume = 		p->resume;
	aw->hit_rate =		p->hit_rate;
	aw->avoid_rate =	p->avoid_rate;
	aw->bisha = 		p->bisha;
	aw->fight_back =	p->fight_back;
	//KDEBUG_LOG(p->petid,"monitor_pet_new:%u %u %u %u %u %u %u",
			//aw->hp_max,aw->attack,aw->defense,aw->speed,p->attack,p->defense,p->speed);	
	if (battle_type == be_pk) {
		if (p->injury_lv) {
			change_pet_second_attr_warrior(p, aw);
		}
		aw->hp = aw->hp_max;
		aw->mp = aw->mp_max;
	}

	aw->cloth_cnt = 0;

	aw->skill_cnt =		p->skill_cnt;
	pkg_skill_warrior(aw->skills, p->skills, aw->skill_cnt);

	aw->action_state = p->action_state;
	memcpy(aw->default_atk, p->default_skills, sizeof(aw->default_atk));
	aw->default_atklv[0] = p->default_levels[0];
	aw->default_atklv[1] = p->default_levels[1];
	return 0;
}

uint32_t get_beast_level(battle_t* pb, uint32_t lvtype, uint32_t minlv)
{
	uint32_t level = 0;
	switch (lvtype) {
		case beast_group_lvtype_min:
			level = pb->floorlv;
			break;
		case beast_group_lvtype_ave:
			level = pb->average_lv;
			break;
		case beast_group_lvtype_m3_ex:
			if (pb->count == 1) {
				level = minlv;
				break;
			}
		case beast_group_lvtype_m3:
			level = pb->toplv > 3 ? (pb->toplv - 3) : 1;
			break;
		case beast_group_lvtype_max:
		default:
			level = pb->toplv;
			break;
	}
	return level < minlv ? minlv : level;
}

static int change_npc_to_warrior(sprite_t* npc, warrior_t* aw, uint32_t lv)
{
	aw->id =				npc->id;
	aw->petid = 			0;
	aw->pettype = 			npc->pettype;
	memcpy(aw->nick, npc->nick, USER_NICK_LEN);
	aw->race = 				npc->race;
 	aw->level = 			lv;
	
	aw->change_race = 0;
	aw->change_petid = 0;
	aw->earth = 			npc->earth;
	aw->water = 			npc->water;
	aw->fire = 				npc->fire;
	aw->wind = 				npc->wind;

	int physi = calc_beast_attr_para10(npc->physique);
	int stren = calc_beast_attr_para10(npc->strength);
	int endur = calc_beast_attr_para10(npc->endurance);
	int quick = calc_beast_attr_para10(npc->quick);
	int intel = calc_beast_attr_para10(npc->intelligence);

	physi = physi * aw->level * 10 / MAGIC_GROW_RATE + physi * 2;
	stren = stren * aw->level * 10 / MAGIC_GROW_RATE + stren * 2;
	endur = endur * aw->level * 10 / MAGIC_GROW_RATE + endur * 2;
	quick = quick * aw->level * 10 / MAGIC_GROW_RATE + quick * 2;
	intel = intel * aw->level * 10 / MAGIC_GROW_RATE + intel * 2;

	int diff[5] = {0, 0, 0, 0, 0};
	for (uint32_t loop = 0; loop < aw->level; loop ++) {
		diff[rand() % 5] += 100;
	}
	physi += diff[0];
	stren += diff[1];
	endur += diff[2];
	quick += diff[3];
	intel += diff[4];

	aw->physique = 		ROUND_100(physi);
	aw->strength = 		ROUND_100(stren);
	aw->endurance = 	ROUND_100(endur);
	aw->quick = 		ROUND_100(quick);
	aw->intelligence =	ROUND_100(intel);

	aw->hp_max = 		ROUND_100(8 * physi + 2 * stren + 3 * endur + 3 * quick + intel);
	aw->mp_max = 		ROUND_100(physi + 2 * stren + 2 * endur + 2 * quick + 10 * intel) + 20;
	aw->attack = 		ROUND_1000(2 * physi + 27 * stren + 3 * endur + 3 * quick + 2 * intel);
	aw->defense =		ROUND_1000(2 * physi + 3 *stren + 30 * endur + 3 * quick + intel);
	aw->speed = 		ROUND_1000(physi + 2 * stren + 2 * endur + 20 * quick + intel);
	aw->spirit = 		ROUND_1000(-3 * physi - stren + 2 * endur - quick + 8 * intel) + 100;
	aw->resume = 		ROUND_1000(8 * physi - stren - endur + 2 * quick - 3 * intel) + 100;

	aw->hp_max = aw->hp_max * npc->hp_max / BEAST_COMMON_RAND_BASE;
	aw->mp_max = aw->mp_max * npc->mp_max / BEAST_COMMON_RAND_BASE;
	aw->attack = aw->attack * npc->attack / BEAST_COMMON_RAND_BASE;
	aw->mattack = aw->mattack * npc->mattack / BEAST_COMMON_RAND_BASE;
	aw->defense = aw->defense * npc->defense / BEAST_COMMON_RAND_BASE;
	aw->speed = aw->speed * npc->speed / BEAST_COMMON_RAND_BASE;
	aw->mdefense +=			npc->mdefense;
	aw->spirit +=			npc->spirit;
	aw->resume +=			npc->resume;
	aw->hit_rate +=			npc->hit_rate;
	aw->avoid_rate +=		npc->avoid_rate;
	aw->bisha +=			npc->bisha;
	aw->fight_back +=		npc->fight_back;

	aw->hp_max *= 5;//for change_prof

	aw->spirit = 			(aw->spirit > 0) ? aw->spirit : 0;
	aw->resume = 			(aw->resume > 0) ? aw->resume : 0;
	aw->hit_rate = 			0;
	aw->avoid_rate = 		0;
	aw->hp = 				aw->hp_max;
	aw->mp = 				aw->mp_max;

	aw->skill_cnt = npc->skill_cnt;
	pkg_skill_warrior(aw->skills, npc->skills, npc->skill_cnt);
	for (uint32_t loop = 0; loop < npc->skill_cnt; loop ++) {
		aw->skills[loop].skill_info.skill_level = 10;
	}
	aw->catchable = 0;
	return 0;
}


void batrserv_challenge_battle(sprite_t* challenger, userid_t challengee_id, uint32_t type, int cmd)
{
	uint8_t buff[10240] = {0};
	battle_t* lpa = challenger->btr_info;
	if (!lpa) {
		KERROR_LOG(challenger->id, "not in battle\t[%u]", challengee_id);
		return;
	}
	*(uint32_t *)(buff) = challengee_id;
	*(uint32_t *)(buff+4) = 0;
	*(uint32_t *)(buff + 8) = type;//0 normal;1 pet pk;2 users pk with 3 pets ;3 team with 2 users can carry 6 pet
	int cnt = lpa->count;
	int len = 16;
	for (int loop = 0; loop < lpa->count; loop++) {
		sprite_t* lp = lpa->players[loop];
		change_sprite_to_warrior(lp, lp->id, (warrior_t*)(buff + len), lpa->battletype);
		KDEBUG_LOG(lp->id, "PLAYER TO BATTLE\t[uid=%u]",lp->id);
		len += sizeof(warrior_t);
		for (int loopi = 0; loopi < lp->pet_cnt; loopi ++) {
			if (lp->pets_inbag[loopi].action_state >= pet_state_max || lp->pets_inbag[loopi].action_state <= just_in_bag)
				continue;
			//DEBUG_LOG("ppppppp %u %u %u",type,lpa->count,lp->pets_inbag[loopi].action_state );
			if (IS_PET_RDY_FIGHT(lp->pets_inbag[loopi].action_state) 
				|| ((type==3||type==0)&& IS_PET_STANDBY(lp->pets_inbag[loopi].action_state)) 
				|| (type==3 && lpa->count==2 && lp->pets_inbag[loopi].action_state== for_assist) //双人组队时可带附宠
				|| (lpa->count == 1 && lp->pets_inbag[loopi].action_state == for_assist //多精灵挑战赛 可带3个宠物 
				&& (type != 0 || lpa->battletype == be_fight_monster))) {
				change_pet_to_warrior(lp, &lp->pets_inbag[loopi], (warrior_t*)(buff + len), lp->id, lpa->battletype);
				len += sizeof(warrior_t);
				cnt++;
				KDEBUG_LOG(lp->id, "PET TO BATTLE\t[%u %u %u]",lp->id, lp->pets_inbag[loopi].petid, lp->pets_inbag[loopi].action_state);
			}
		}
	}
//	KDEBUG_LOG(challenger->id,"challenge beastgrp id[%u]",challengee_id);
	beast_group_t *pbg=get_beast_grp(challengee_id);
	if (lpa->count < MAX_PLAYERS_PER_TEAM && pbg && pbg->npcid != 0) {
		sprite_t* npc = get_npc(pbg->npcid);
		KDEBUG_LOG(challenger->id,"npc join in[id=%u right=%u?]",challengee_id,npc==NULL);
		if (npc) {
			change_npc_to_warrior(npc, (warrior_t*)(buff + len), get_beast_level(lpa, npc->level, 1));
			len += sizeof(warrior_t);
			cnt ++;
		}
		
	}
	*(uint32_t*)(buff + 12) = cnt;
	KDEBUG_LOG(challenger->id,"batrserv_challenge_battle id=%u type=%u cnt=%u",challengee_id,type,cnt);
	send_to_batrserv(challenger, bt_battle_info_msg, len, buff, challenger->id, lpa->battle_id, cmd);
}

void batrserv_accept_battle(sprite_t* challengee )
{
	uint8_t buff[10240] = {0};
	battle_t* lpa = challengee->btr_info;
	int cnt = lpa->count;
	
	int len = 20;
	uint32_t type=0;
	if(challengee->pgrp){
		type=1;
	}else if(challengee->pvp_type){
		type=2;
	}else if(challengee->btr_team && challengee->btr_team->count==2){
		type=3;
	}
	for (int loop = 0; loop < lpa->count; loop++) {
		sprite_t* lp = lpa->players[loop];
		change_sprite_to_warrior(lpa->players[loop], lpa->players[loop]->id, (warrior_t*)(buff + len), be_pk);
		len += sizeof(warrior_t);
		for (int loopi = 0; loopi < lp->pet_cnt; loopi ++) {
			if (lp->pets_inbag[loopi].action_state >= pet_state_max || lp->pets_inbag[loopi].action_state <= just_in_bag)
				continue;
			
			DEBUG_LOG("accept battle %u",type);
			if (IS_PET_RDY_FIGHT(lp->pets_inbag[loopi].action_state) \
				|| (IS_PET_STANDBY(lp->pets_inbag[loopi].action_state) && lp->team_state != apply_for_pk_pet) \
				|| (type==3 && lpa->count==2 && lp->pets_inbag[loopi].action_state==for_assist) 
				|| (lpa->count == 1 && lp->team_state == apply_for_pk_pet 
					&& lp->pets_inbag[loopi].action_state == for_assist)
				|| (lpa->count == 1 && lp->pets_inbag[loopi].action_state == for_assist //辅助宠物参加战斗
					&& (type != 0 || lpa->battletype == be_fight_monster))) {
				change_pet_to_warrior(lp, &lp->pets_inbag[loopi], (warrior_t*)(buff + len), lp->id, be_pk);
				len += sizeof(warrior_t);
				cnt++;
			}
		}
		/*
		change_sprite_to_warrior(lpa->players[loop]->battle_npc[0], lpa->players[loop]->battle_npc[0]->id + cnt * MAX_NPC_TYPE, (warrior_t*)(buff + len), be_pk);
		len += sizeof(warrior_t);
		cnt ++;
		*/
	}

	*(uint32_t *)buff = 0;
	*(uint32_t *)(buff+4) = type;
	*(uint32_t *)(buff + 8) = 100;
	*(uint32_t *)(buff + 12) = be_pk;
	*(uint32_t *)(buff + 16) = cnt;
	
	DEBUG_LOG("cnt:%u,be_pk=%u ",cnt,be_pk );
	send_to_batrserv(challengee, bt_battle_info_msg, len, buff, challengee->id, lpa->battle_id, proto_cli_accept_challenge);
}


static void beast_accept_battle(sprite_t* p, beast_group_t* pbg, battle_t* pb, uint32_t pkteam)
{
	uint8_t buff[10240] = {0};
	*(uint32_t *)buff = pkteam;
	*(uint32_t *)(buff + 4) =4; 
	*(uint32_t *)(buff + 8) = (pkteam || p->pgrp) ? 100 : 70;
	*(uint32_t *)(buff + 12) = be_fight_monster;
	*(uint32_t *)(buff + 16) = BATTLE_POS_CNT;
	
	int len = 20;
	for (int loop = 0; loop < BATTLE_POS_CNT; loop ++) {
		beast_t* pba = get_beast(pbg->saset[loop].id);
		if (pba) {
			warrior_t* pw = (warrior_t*)(buff + len);
			pw->id = pba->id + loop * MAX_NPC_TYPE;
			change_beast_to_warrior(pba, &pbg->saset[loop], pw, 
					get_beast_level(pb, pbg->saset[loop].lvtype, pbg->saset[loop].minlv), pkteam, pbg->elite_type[loop]);
			switch (pbg->elite_type[loop]) {
				case 0:
					pb->beasts[pb->beast_cnt].beast_id = pbg->saset[loop].id;
					break;
				case 1:
					pb->beasts[pb->beast_cnt].beast_id = pbg->saset[loop].id % SPEC_BEAST_BASE + 8 * SPEC_BEAST_BASE;
					break;
				case 2:
					pb->beasts[pb->beast_cnt].beast_id = pbg->saset[loop].id % SPEC_BEAST_BASE + 9 * SPEC_BEAST_BASE;
					break;
			}
			pb->beasts[pb->beast_cnt].beast_lv = pw->level;
			pb->beast_cnt ++;
			//DEBUG_LOG("double_ack_flag %u",pw->flag );
		}
		len += sizeof(warrior_t);
	}

	KDEBUG_LOG(low32_val(pb->battle_id), "BEAST ACCEPT BATTLE[grpid=%u pkteam=%u len=%u]", pbg->grpid,pkteam,len);
	send_to_batrserv(NULL, bt_battle_info_msg, len, buff, pbg->grpid, pb->battle_id, proto_cli_accept_challenge);
}

int challenge_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t  uid, grpid, pos = 0;
	UNPKG_UINT32(body, uid, j);
	UNPKG_UINT32(body, grpid, j);
	UNPKG_UINT32(body, pos, j);

	KDEBUG_LOG(p->id, "CHALLENGE BATTLE[%u %u]", uid,grpid);
	CHECK_SELF_TEAM_LEADER(p, uid, 1);
	CHECK_USER_APPLY_PK(p, p);
	CHECK_USER_IN_MAP(p, 0);	
	CHECK_TEAM_WAIT_ANIMATION(p);
	CHECK_SELF_BATTLE_INFO(p, uid);
	if(!p->tiles || IS_TRADE_MAP(p->tiles->id)){
		return send_to_self_error(p, p->waitcmd,cli_err_not_right_map , 1);
	}
	CHECK_USER_HANGUP(p);

	if (uid == 0xFFFFFFFF) {
		CHECK_SELF_JOIN_TEAM(p);
		if (!get_pet_rdy_fight(p)) {
			KERROR_LOG(p->id, "pet need rdy");
			return send_to_self_error(p, p->waitcmd, cli_err_pet_not_rdy_fight, 1);
		}
		
		if (grpid >= LOOP_GRP_CNT || pos >= LOOP_GRP_BEAST_CNT) {
			KERROR_LOG(p->id, "invalid grp pos\t[%u %u]", grpid, pos);
			return -1;
		}

		if (p->loop_grp[grpid].beasts[pos].count > 0 \
			|| !check_same_period(p->loop_grp[grpid].uptime, repeat_day, 1) \
			|| p->loop_grp[grpid].version != LOOP_GRP_VERSION \
			|| p->loop_grp[grpid].day_draw_cnt >= LOOP_GRP_MAX_DAY) {
			KERROR_LOG(p->id, "challenged\t[%u %u %u]", grpid, pos, p->loop_grp[grpid].uptime);
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}

		msg_log_loop_grp(p->id, 1, 0, grpid);
		
		p->pgrp = &p->loop_grp[grpid].beasts[pos];
		p->grpid = grpid;
		uid = p->loop_grp[grpid].beasts[pos].id;
	} else if(!uid) {
		CHECK_TEAM_FIGHT_TIME(p);	
		uint32_t timespan = get_now_tv()->tv_sec - p->start_walk_time;
		if (p->left_step > 10 || (timespan + 1) * 40 < p->last_fight_step) {
			KERROR_LOG(p->id, "have not happen monster\t[%u %u %u]", timespan, p->last_fight_step, p->left_step);
			return send_to_self_error(p, p->waitcmd, cli_err_not_happen_monster, 1);
		}
		
		if (!MAP_HAS_BEAST(p->tiles)) {
			KERROR_LOG(p->id, "not beast map\t[%u]", uid);
			return send_to_self_error(p, p->waitcmd, cli_err_not_beast_map, 1);
		}
		p->last_fight_time = get_now_tv()->tv_sec;
		notify_next_fight_step(p);
		uid = rand() % MAX_BEAST_TYPE;
		grpid = 0;
	} else if (uid < BEGIN_USER_ID) {
		CHECK_MAP_BEAST_DARK(p);
		if (grpid) {
			CHECK_INT_LE(grpid, 2);
			map_t* mp = p->tiles;
			bool has_elite_beast = false, need_refresh = true;
			for (uint32_t loop = 0; loop < mp->elite_cnt; loop ++) {
				if (mp->ebeasts[loop].beastid == uid && !has_elite_beast) {
					/*
					if (mp->ebeasts[loop].count[grpid - 1] == 0)
						return send_to_self_error(p, p->waitcmd, cli_err_beast_challenged, 1);
					mp->ebeasts[loop].count[grpid - 1] --;
					*/
					has_elite_beast = true;
					
				}

				if (mp->ebeasts[loop].count[grpid - 1])
					need_refresh = false;
			}

			if (!has_elite_beast)
				return send_to_self_error(p, p->waitcmd, cli_err_beast_challenged, 1);
			/*
			if (need_refresh)
				ADD_ONLINE_TIMER(mp, n_refresh_elite_beast, (void *)(grpid - 1), grpid == 1 ? 5 : 10);
			*/
			rsp_elite_beast(NULL, mp);
		}
	} else if  (IS_NORMAL_ID(uid)) {
		sprite_t* lp = get_sprite(uid);
		if (lp) {
			CHECK_OTHER_BATTLE_INFO(p, lp);
			CHECK_LV_FIT(p, p, 15, MAX_SPRITE_LEVEL);
			CHECK_LV_FIT(p, lp, 15, MAX_SPRITE_LEVEL);
		}
	} else {
		KERROR_LOG(p->id, "invalid id\t[%u]", uid);
	}

	*(uint32_t*)p->session = uid;
	*(uint32_t*)(p->session + 4) = grpid;

	uint32_t type=0;//精灵挑战type=1,多精灵挑战type=2,双人组队可带辅助宠物 type=3
	//KDEBUG_LOG(p->id,"xxx %u",(p->btr_team?p->btr_team->count:0));
	if(p->pgrp){
		type=1;
	}else if(p->pvp_type){
		type=2;
	}else if(p->btr_team && p->btr_team->count==2){
		type=3;
	}
	init_challenger_battle(p, 0, uid > BEGIN_USER_ID ? be_pk : be_fight_monster);
	p->btr_info->mapid = p->tiles->id;
	batrserv_challenge_battle(p, uid, type , p->waitcmd);
	return 0;
}

static inline void challenge_kuro_msg_log(sprite_t* p, uint32_t grpid)
{
	if (!p->btr_team) {
		msg_log_kuro_chanllenge(p->id, grpid, 1);
		return;
	}

	for (uint32_t loop = 0; loop < p->btr_team->count; loop ++) {
		sprite_t* s = p->btr_team->players[loop];
		if (p == s) {
			msg_log_kuro_chanllenge(p->id, grpid, 1);
		} else {
			msg_log_kuro_chanllenge(s->id, grpid, 0);
		}
	}
}

int check_for_copy_map(sprite_t* p, uint32_t grpid)
{
	map_copy_instance_t* pmci = get_map_copy_with_mapid(p->tiles->id);
	if (!pmci) {
		KERROR_LOG(p->id, "impossible\t[%lx]", p->tiles->id);
		return -1;
	}

	int layeridx = get_copy_layer_idx(pmci, p->tiles->id);
	if (layeridx == -1) {
		KERROR_LOG(p->id, "impossible layer\t[%lx]", p->tiles->id);
		return -1;
	}

	copy_layer_t* layer = &pmci->p_copy_config->layers[layeridx];
	bool order_valid = true;
	for (uint32_t loop = 0; loop < layer->beast_orders; loop ++) {
		if (!order_valid) {
			KERROR_LOG(p->id, "order not fit");
			return -1;
		}
		copy_beast_t* pbeast = &layer->beasts[loop];
		for (uint32_t loopi = 0; loopi < pbeast->beast_type; loopi ++) {
			if (pbeast->needkillcnts[loopi] > pmci->killedcnts[layeridx][loop][loopi])
				order_valid = false;
			if (grpid == pbeast->grpids[loopi]) {
				if (pmci->killedcnts[layeridx][loop][loopi] >= pbeast->validcnts[loopi]) {
					KERROR_LOG(p->id, "cnt max\t[%u %u]", pmci->killedcnts[layeridx][loop][loopi], pbeast->validcnts[loopi]);
					return -1;
				}
				return 0;
			}
		}
	}
	KERROR_LOG(p->id, "no such grp\t[%u]", grpid);
	return -1;
}

//#define  IS_PUMPKIN_BEASTGRP_TYPE(gid_) (gid_)==39

int challenge_npc_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t  grp_id, refreshid;
	UNPKG_UINT32(body, grp_id, j);
	UNPKG_UINT32(body, refreshid, j);
	CHECK_USER_APPLY_PK(p, p);
	CHECK_USER_HANGUP(p);
	KDEBUG_LOG(p->id,"CHALLENGE_NPC_BATTLE_CMD:grpid=%u refreshid=%u",grp_id,refreshid);
	/*南瓜灯活动  已经下架
	if(IS_PUMPKIN_BEASTGRP_TYPE(grp_id)){
			if(p->pumpkin_lantern_activity_type != 2){
				KDEBUG_LOG(p->id,"-----------lalala");
				p->pumpkin_lantern_activity_type = 0; 
				return send_to_self_error(p, p->waitcmd, cli_err_invalid_grpid, 1);
			}
			p->pumpkin_lantern_activity_type = 0; 
	}
*/
	beast_group_t* pbt = grp_id ? get_beast_grp(grp_id) : get_box_beast_group(p);
	if (!pbt) {
		KERROR_LOG(p->id, "invalid beast group id\t[%u]", grp_id);
		return send_to_self_error(p, p->waitcmd, cli_err_invalid_grpid, 1);
	}

	if (grp_id) {
		CHECK_LV_FIT(p, p, pbt->minlv, pbt->maxlv);
		if (!refreshid && !(pbt->time_valid[get_now_tm()->tm_wday] & (1 << get_now_tm()->tm_hour))) {
			KERROR_LOG(p->id, "time invalid\t[%x %u]", pbt->time_valid[get_now_tm()->tm_wday], get_now_tm()->tm_hour);
			return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
		}
	}

	if (pbt->for_fish) {
		CHECK_FISHING_STARTED(p);
		CHECK_FISHING_CNT(p);
		CHECK_SELF_JOIN_TEAM(p);
	}
	CHECK_SELF_TEAM_LEADER(p, grp_id, 1);
	CHECK_SELF_BATTLE_INFO(p, grp_id);
	CHECK_TEAM_WAIT_ANIMATION(p);
	CHECK_HONOR_SATISFY(p, pbt->honor_id);
	CHECK_USER_IN_MAP(p, 0);
	if (pbt->allowteam)
		CHECK_SELF_JOIN_TEAM(p);
	if (pbt->needitem && pbt->needcnt) {
		if (!p->btr_team)
			CHECK_ITEM_EXIST(p, pbt->needitem, pbt->needcnt);
		else {
			for (uint32_t loop = 0; loop < p->btr_team->count; loop ++) {
				CHECK_ITEM_EXIST(p->btr_team->players[loop], pbt->needitem, pbt->needcnt);
			}
		}
	}

	if (IS_COPY_MAP(p->tiles->id)) {
		if (check_for_copy_map(p, grp_id)) {
			KERROR_LOG(p->id, "invalid copy map\t[%lx, %u]", p->tiles->id, grp_id);
			return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
		}
	} else if (!refreshid && ((pbt->type == group_type_maze && !IS_MAZE_MAP(p->tiles->id)) \
		|| (pbt->mapid && pbt->mapid!=p->tiles->id))) {
		KERROR_LOG(p->id, "not right map\t[%u %u %u %u %u]", \
			grp_id, pbt->type, high32_val(p->tiles->id), low32_val(p->tiles->id), pbt->mapid);
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
	}
	if (pbt->type == group_type_firework) {	
		item_t* pi = (item_t*)g_hash_table_lookup(p->fireworks, &refreshid);
		if (!pi || !pi->count) {
			KERROR_LOG(p->id, "firework npc battle\t[%u %u]", grp_id, refreshid);
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
		pi->count --;
		p->firework_beast = refreshid;
	} else if (refreshid && (reduce_map_grp(p,refreshid, p->tiles, grp_id) == -1)) {
		DEBUG_LOG("err refreshid %d ",refreshid );
		return send_to_self_error(p, p->waitcmd, cli_err_invalid_grpid, 1);
	}
	if (grp_id) {
		CHECK_TEAM_FIGHT_TIME(p);
		if (pbt->repeatcnt) {//检查挑战怪物组的挑战上线
			//if(p->btr_team){//队友打怪次数检查并扣除
				//for( uint8_t loop=1 ; loop < p->btr_team->count ; loop++ ){
					//item_t* pday = cache_get_day_limits(p->btr_team->players[loop], grp_id);
					//if (pday->count >= pbt->repeatcnt) {
						//KERROR_LOG(p->id, "day max\t[%u %u %u]", grp_id, pday->count, pbt->repeatcnt);
						//return send_to_self_error(p, p->waitcmd, cli_err_rand_day_max, 1);
					//}
					//db_day_add_ssid_cnt(NULL, p->btr_team->players[loop]->id, pbt->grpid, 1, pbt->repeatcnt);
					//pday->count ++;
					//KERROR_LOG(p->id, "repeatcnt++\t[%u]",grp_id );
				//}
			//}
			item_t* pday = cache_get_day_limits(p, grp_id);
			if (pday->count >= pbt->repeatcnt) {
				KERROR_LOG(p->id, "day max\t[%u %u %u]", grp_id, pday->count, pbt->repeatcnt);
				return send_to_self_error(p, p->waitcmd, cli_err_rand_day_max, 1);
			}
			db_day_add_ssid_cnt(NULL, p->id, pbt->grpid, 1, pbt->repeatcnt);
			pday->count ++;
		}
	}
	//check end
	p->last_fight_time = get_now_tv()->tv_sec;

	if (pbt->for_fish) {
		p->cur_fish_cnt ++;
	}

	if (grp_id == 2907) {
		msg_log_challenge_total();	
		if (!p->btr_team) {
			msg_log_challenge_singel();
			msg_log_challenge_user(p->id);
		} else {
			for (uint32_t loop = 0; loop < p->btr_team->count; loop ++) {
				sprite_t* s = p->btr_team->players[loop];
				msg_log_challenge_user(s->id);
			}
		}
	}

	if (pbt->needitem && pbt->needcnt) {
		if (!p->btr_team) {
			cache_reduce_kind_item(p, pbt->needitem, pbt->needcnt);
			db_add_item(NULL, p->id, pbt->needitem, -pbt->needcnt);
		} else {
			for (uint32_t loop = 0; loop < p->btr_team->count; loop ++) {
				sprite_t* s = p->btr_team->players[loop];
				cache_reduce_kind_item(s, pbt->needitem, pbt->needcnt);
				db_add_item(NULL, s->id, pbt->needitem, -pbt->needcnt);
			}
		}
	}
	if( grp_id == 3061 ){//变身任务特殊处理
		p->challenge_3061=1;
	}

	*(beast_group_t**)p->session = pbt;
	KDEBUG_LOG(p->id, "================BT START=============[grpid=%u lv=%u]", grp_id,p->level);

	if (grp_id >= 2501 && grp_id < 2900)
		msg_log_clean_boss_road(p->id, grp_id);
	if( p->level >= 10 ){
		msg_log_challenge_user_by_level(grp_id,(p->level-10)/5,p->id);//按等级进行统计，添加时再函数内增加
	}
	msg_log_grp_spec_challenge(p, grp_id);
	if (grp_id >= 2908 && grp_id <= 2910)
		challenge_kuro_msg_log(p, grp_id);
	init_challenger_battle(p, 0, be_fight_monster);
	p->btr_info->pkteam = grp_id;
	p->btr_info->mapid = p->tiles->id;
	uint32_t type=0;
	if(p->btr_team && p->btr_team->count == 2){//双人组队时 可以带6个宠物 type==3
		type=3;
	}
	batrserv_challenge_battle(p, grp_id, type, p->waitcmd);
	return 0;
}

int accept_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	battle_id_t  btid;
	unpkg_battle_id(body, &btid, &j);
	userid_t uid = BATTLE_USER_ID(btid);

	KDEBUG_LOG(p->id, "ACCEPT BATTLE\t[%u %u %u]", high32_val(btid), low32_val(btid), uid);
	CHECK_SELF_TEAM_LEADER(p, uid, 1);
	CHECK_SELF_BATTLE_INFO(p, uid);
	CHECK_USER_APPLY_PK(p, p);
	CHECK_LV_FIT(p, p, 15, MAX_SPRITE_LEVEL);
	CHECK_USER_HANGUP(p);

	if(!p->tiles || IS_TRADE_MAP(p->tiles->id)){
		return send_to_self_error(p, p->waitcmd,cli_err_not_right_map , 1);
	}
	sprite_t* lp = get_sprite(uid);
	if (lp) {
		if (!lp->btr_info) {
			KERROR_LOG(p->id, "not fighting now\t[%u]", uid);
			return send_to_self_error(p, p->waitcmd, cli_err_battle_canceled, 1);
		}
		lp->btr_info->state = bt_state_accept;
	}
	
	init_challengee_battle(p, btid, 0);
	batrserv_accept_battle(p);
	return 0;
}

int reject_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	battle_id_t btid;
	unpkg_battle_id(body, &btid, &j);

	battle_t* pbt = get_batter_info(BATTLE_USER_ID(btid), btid);
	if (!pbt) {
		KERROR_LOG(p->id, "invalid battle id\t[%u %u]", high32_val(btid), low32_val(btid));
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	if (p->btr_info) {
		KERROR_LOG(p->id, "fighting now\t[%u %u]", high32_val(btid), low32_val(btid));
		return send_to_self_error(p, p->waitcmd, cli_err_self_be_fighting, 1);
	}

	send_to_batrserv(p, bt_battle_info_msg, 0, NULL, p->id, btid, proto_cli_reject_challenge);
	response_proto_uint32(p, p->waitcmd, BATTLE_USER_ID(btid), 1, 0);
	return 0;
}

int cancel_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	CHECK_SELF_TEAM_LEADER(p, 0, 1);
	CHECK_USER_IN_BATTLE(p, 0);
	KDEBUG_LOG(p->id, "CANCEL BT\t[%u %u]", high32_val(p->btr_info->battle_id), low32_val(p->btr_info->battle_id));
	return batrserv_cancel_battle(p);
}

void batrserv_battle_attack(sprite_t* p, uint32_t sorp, uint32_t team, uint32_t pos, uint32_t aatype, uint32_t alevel, uint32_t itemid, uint32_t add_hp )
{
	uint8_t buff[28];
	battle_t* lpa = p->btr_info;
	*(uint32_t*)buff = sorp;
	*(uint32_t*)(buff + 4) = team;
	*(uint32_t*)(buff + 8) = pos;
	*(uint32_t*)(buff + 12) = aatype;
	*(uint32_t*)(buff + 16) = alevel;
	*(uint32_t*)(buff + 20) = itemid;
	*(uint32_t*)(buff + 24) = add_hp;
	
	send_to_batrserv(p, bt_battle_info_msg, 28, buff, p->id, lpa->battle_id, p->waitcmd);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief   战斗攻击
 * @return  
 */
/* ----------------------------------------------------------------------------*/
int battle_attack_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t  team, sorp, pos, attack_type, attack_level,itemid,add_hp ;
	UNPKG_UINT32(body, sorp, j);
	UNPKG_UINT32(body, team, j);
	UNPKG_UINT32(body, pos, j);
	UNPKG_UINT32(body, attack_type, j);
	UNPKG_UINT32(body, attack_level, j);

	CHECK_USER_IN_BATTLE(p, sorp);


	
	KDEBUG_LOG(p->id, "ATK\t[%u %u %u %u %u %u %u]", \
		p->round_attack_cnt, p->last_attack_time, sorp, team, pos, attack_type, attack_level);


	p->round_attack_cnt ++;
	if (p->round_attack_cnt > 2) {
		KERROR_LOG(p->id, "round attack too much\t[%u]", p->round_attack_cnt);
		return -1;
	}
	itemid=0;
	add_hp=0;
	if(attack_type==1000000 ) {//自动
		//找到可用的药品	210005-210001
		for (int i=210005;i>=210001;i--){
			if (cache_item_have_cnt(p, i, 1)){
				itemid=i;	
				normal_item_t* pni = get_item(itemid);
				add_hp=pni->medinfo.hp_up;
				break;
			}
		}	
	}

	
	p->last_attack_time = get_now_tv()->tv_sec;
	batrserv_battle_attack(p, sorp, team, pos, attack_type, attack_level,itemid,add_hp );
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

void batrserv_battle_use_medical(sprite_t* p, uint32_t team, uint32_t pos, normal_item_t* pni)
{
	uint8_t buff[20];
	battle_t* lpa = p->btr_info;
	*(uint32_t*)buff = 0;
	*(uint32_t*)(buff + 4) = team;
	*(uint32_t*)(buff + 8) = pos;
	*(uint32_t*)(buff + 12) = pni->item_id;
	*(uint32_t*)(buff + 16) = pni->medinfo.hp_up;
	
	send_to_batrserv(p, bt_battle_info_msg, 20, buff, p->id, lpa->battle_id, p->waitcmd);
}

int battle_use_medical_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t  team, pos, itemid;
	UNPKG_UINT32(body, team, j);
	UNPKG_UINT32(body, pos, j);
	UNPKG_UINT32(body, itemid, j);
	
	CHECK_TEAM_POS_VALID(p, team, pos);
	CHECK_USER_IN_BATTLE(p, itemid);
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	CHECK_ITEM_FUNC(p, pni, item_for_hpmp);
	if(p->last_use_medical_id==itemid){
		CHECK_ITEM_EXIST(p, itemid, 2);
	}else{
		CHECK_ITEM_EXIST(p, itemid, 1);
		if(p->last_use_medical_id==0){//只保存每回合第一次
			p->last_use_medical_id=itemid;
		}
	}

	KDEBUG_LOG(p->id, "USE MEDICAL\t[%u %u %u]", team, pos, itemid);
	batrserv_battle_use_medical(p, team, pos, pni);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

void batrserv_battle_use_beast_card(sprite_t* p, uint32_t teamid, uint32_t pos, normal_item_t* p_ni)
{
	uint8_t buff[64];
	battle_t* lpa = p->btr_info;
	int j = 0;
	PKG_H_UINT32(buff, teamid, j);
	PKG_H_UINT32(buff, pos, j);
	j += pkg_card_race(buff + j, p_ni);
	send_to_batrserv(p, bt_battle_info_msg, j, buff, p->id, lpa->battle_id, p->waitcmd);
}

int battle_use_beast_card_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t teamid, pos, itemid;
	UNPKG_UINT32(body, teamid, j);
	UNPKG_UINT32(body, pos, j);
	UNPKG_UINT32(body, itemid, j);

	if (p->all_petcnt > p->pet_cnt_actual && p->all_petcnt - p->pet_cnt_actual >= 2000)
		return send_to_self_error(p, p->waitcmd, cli_err_pet_cnt_max, 1);

	CHECK_USER_IN_BATTLE(p, itemid);
	CHECK_TEAM_POS_VALID(p, teamid, pos);

	normal_item_t* p_ni = get_item(itemid);
	CHECK_ITEM_VALID(p, p_ni, itemid);
	CHECK_ITEM_FUNC(p, p_ni, item_for_pet_catch);
	CHECK_ITEM_EXIST(p, itemid, 1);

	*(uint32_t *)p->session = itemid;

	KDEBUG_LOG(p->id, "USE BEAST CARD\t[%u %u %u]", teamid, pos, itemid);
	batrserv_battle_use_beast_card(p, teamid, pos, p_ni);
	response_proto_head(p, p->waitcmd, 1, 0);
	
	return 0;
}

int battle_swap_pet_cmd(sprite_t *p, uint8_t* body, uint32_t len)
{
	int j = 0;
	uint32_t petid;
	UNPKG_UINT32(body, petid, j);

	KDEBUG_LOG(p->id, "SWAP PET\t[%u]", petid);
	CHECK_USER_IN_BATTLE(p, petid);

	if (!petid && !get_pet_rdy_fight(p)) {
		KERROR_LOG(p->id, "no pet rdy fight\t[%u]", petid);
		return send_to_self_error(p, p->waitcmd, cli_err_pet_not_rdy_fight, 1);
	} else if (petid) {
		pet_t *p_p = get_pet_inbag(p, petid);
		if (!p_p || p_p->action_state != on_standby)
			return send_to_self_error(p, p->waitcmd, cli_err_pet_not_standby, 1);

		CHECK_PET_LV_FIT(p, p_p, 1, p->level + 5);
	}

	send_to_batrserv(p, bt_battle_info_msg, 4, &petid, p->id, p->btr_info->battle_id, p->waitcmd);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int notify_teamleader_action_cmd(sprite_t *p, uint8_t *body, uint32_t bodylen)
{
	if (!p->btr_team) {
		p->waitcmd = 0;
		return 0;
	}
	CHECK_SELF_TEAM_LEADER(p, p->id, 0);
	CHECK_BODY_LEN_LE(bodylen, 1040);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	memcpy(msg + i, body, bodylen);
	i += bodylen;

	init_proto_head(msg, p->waitcmd, i);
	send_to_btrtem(p->btr_team, msg, i);
	p->waitcmd = 0;
	return 0;
}
int animation_finish_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	KDEBUG_LOG(p->id, "ANIMAT FIN\t[%u %u]", p->team_state, p->busy_state);
	if (p->team_state == wait_animation) {
		p->team_state = 0;
		KDEBUG_LOG(p->id, "CANCEL BATTLE STATE");
		p->busy_state = be_idle;
		notify_busy_state_update(p, sns_state, p->busy_state);
	}

	if (p->btr_team) {
		REMOVE_TIMERS(p->btr_team);
		for (int loop = 0; loop < p->btr_team->count; loop ++) {
			sprite_t* s = p->btr_team->players[loop];
			if (s && s->team_state == wait_animation) {
				ADD_ONLINE_TIMER(p->btr_team, n_kick_out_team_animation, 0, 30);
				break;
			}
		}
	}
	//need for watchers
	if( p->watchid ){
		cancel_watch_ex(p);
	}
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int change_team_leader_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint32_t uid;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	CHECK_SELF_TEAM_LEADER(p, uid, 0);
	if (p->id == uid) {
		response_proto_head(p, p->waitcmd, 1, 0);
		return 0;
	}

	for (int loop = 0; loop < p->btr_team->count; loop ++) {
		if (p->btr_team->players[loop]->id == uid) {
			response_proto_uint32(p->btr_team->players[0], proto_cli_delete_batr_team, p->btr_team->teamid, 0, 1);
			sprite_t* s = p->btr_team->players[loop];
			p->btr_team->players[loop] = p->btr_team->players[0];
			p->btr_team->players[0] = s;
			change_batter_team_id(p->btr_team, p->id);
			response_proto_head(p, p->waitcmd, 1, 0);
			response_team_info(p);
			return 0;
		}
	}

	return send_to_self_error(p, p->waitcmd, cli_err_not_in_same_team, 1);
}

int load_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t  rate;
	UNPKG_UINT32(body, rate, j);
	rate = (rate > 100) ? 98 : rate;
	CHECK_USER_IN_BATTLE(p, rate);

	p->btr_info->state = bt_state_load;
	
	send_to_batrserv(p, bt_battle_info_msg, 4, &rate, p->id, p->btr_info->battle_id, p->waitcmd);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int round_info_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t round_cnt, round_idx;
	UNPKG_UINT16(body, round_cnt, j);
	UNPKG_UINT16(body, round_idx, j);

	CHECK_USER_IN_BATTLE(p, 0);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT16(msg, round_cnt, i);
	PKG_UINT16(msg, round_idx, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_battle(p->btr_info, msg, i);
	return 0;
}

int rsp_watcher_load_result(sprite_t *p)//通知观战者 战斗是否创建成功 返回1307
{
	sprite_t* lp=NULL;
	if(p->watchid){//用户正在观战
		//send to watchers 1307
		if((lp=get_sprite(p->watchid)) && lp->btr_info){
			if(lp->btr_info->load_result){
			//如果创建成功 战斗者已经发1307（load_resul!=0 则直接返回1307 否则等战斗者发1307时再发给观战者
				response_proto_uint32(p, proto_cli_noti_battle_started, lp->btr_info->load_result-1,0,0);
			}
		}else{//用户已经退出,返回1307结果0
			response_proto_uint32(p, proto_cli_noti_battle_started,0, 0, 0);
			cancel_watch_ex(p);
		}
	}
	return 0;
}

int load_battle_result_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t  load_ret;
	UNPKG_UINT32(body, load_ret, j);
	p->team_state = load_ret ? load_bt_succ : load_bt_fail;

	KDEBUG_LOG(p->id, "LOAD RESULT\t[%u %u]", load_ret, p->team_state);
	if(p->watchid){//观战模式
		if( p->team_state==load_bt_fail ){//观战时加载资源失败则取消观战
			cancel_watch_ex(p);
		}
	}else{//正常模式
		CHECK_USER_IN_BATTLE(p, load_ret);
		send_to_batrserv(p, bt_battle_info_msg, 4, &load_ret, p->id, p->btr_info->battle_id, proto_cli_load_ready);
	}		
	response_proto_head(p, p->waitcmd, 1, 0);
	return rsp_watcher_load_result(p);
}

uint32_t get_avg_level(sprite_t* p) 
{
	uint32_t totallv = p->btr_team->count;
	for (int loop = 0; loop < p->btr_team->count; loop ++) {
		sprite_t* s = p->btr_team->players[loop];
		if (s) totallv += s->level;
	}
	return totallv / p->btr_team->count;
}

uint32_t get_pet_avg_level(sprite_t *p, uint32_t count)
{
	uint32_t total_lv = count;
	for (uint32_t loop = 0; loop < p->pet_cnt; loop ++) {
		if (p->pets_inbag[loop].action_state == for_assist \
			|| p->pets_inbag[loop].action_state == rdy_fight)
			total_lv += p->pets_inbag[loop].level;
	}
	return total_lv / count;
}
/*
int check_gvg_again(sprite_t * p)
{
	KDEBUG_LOG(p->id,"check gvg again");
	struct batter_team* pbt= p->btr_team;
	for(uint32_t loop = 0; loop < pbt->count; loop ++)
   	{
		item_t* pday = cache_get_day_limits(pbt->players[loop], ssid_gvg_challenge_times);
		if (pday->count >= MAX_GVG_CHALLENGE )
	   	{
			p->gvg_team_win_cnt=0;
			KDEBUG_LOG(p->id,"del a teammate for limits[id=%u count=%u]",pbt->players[loop]->id,pday->count);
			rm_from_gvg_group(pbt->players[loop]);
		}
	}
	return 0;
}
*/
int apply_pk_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint32_t type;
	int j = 0;
	UNPKG_UINT32(body, type, j);
	KDEBUG_LOG(p->id, "APPLY FOR PK\t[%u]", type);
	CHECK_SELF_TEAM_LEADER(p, type, 1);
	CHECK_SELF_BATTLE_INFO(p, type);

	uint32_t count = 1;
	uint32_t level = p->level;
	switch (type) {
	case 0:
		CHECK_SELF_JOIN_TEAM(p);
		p->team_state = apply_for_pk;
		break;
	case 1:
		if (p->btr_team) {
			count = p->btr_team->count;
			level = get_avg_level(p);
		}
		p->team_state = apply_for_pk;
		break;
	case 2:
		CHECK_SELF_JOIN_TEAM(p);
		if (!get_pet_rdy_fight(p))
			return send_to_self_error(p, p->waitcmd, cli_err_pet_not_rdy_fight, 1);
		count = 1 + p->pet_cnt_assist;
		level = get_pet_avg_level(p, count);
		p->team_state = apply_for_pk_pet;
	}
	
	uint8_t buf[32];
	int i = 0;
	PKG_H_UINT32(buf, type, i);
	PKG_H_UINT32(buf, level, i);
	PKG_H_UINT32(buf, count, i);
	return send_to_switch(p, COCMD_onli_apply_pk, i, buf, p->id);
}

int cancel_pk_apply_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	CHECK_SELF_TEAM_LEADER(p, p->id, 1);
	CHECK_SELF_BATTLE_INFO(p, p->id);
	return send_to_switch(p, COCMD_onli_cancel_pk_apply, 0, NULL, p->id);
}

void rand_pos(int * apos, int max_cnt)
{
	int idxa = rand() % max_cnt;
	int idxb = rand() % 10;
	int tmp = apos[idxa];
	apos[idxa] = apos[idxb];
	apos[idxb] = tmp;
}

int get_group_valid_idx(beast_group_t* pbg)
{
	int posidx = rand() % (BATTLE_POS_CNT);
	for (int loopi = 0; loopi < BATTLE_POS_CNT; loopi ++) {
		if (!pbg->saset[posidx].id)
			return posidx;
	
		posidx = (posidx + 1) % (BATTLE_POS_CNT);
	}
	return -1;
}

int get_pos_valid_idx(beast_group_t* pbt, int pos[4][3])
{
	int posidx = rand() % 4;
	KDEBUG_LOG(0, "GET POST\t[%u]", posidx);
	for (int loop = 0; loop < 4; loop ++) {
		if (!pbt->cntlist[pos[posidx][0]] || !pbt->cntlist[pos[posidx][1]])
			pos[posidx][2] = 0;
		if (pos[posidx][2])
			return posidx;
		posidx = (posidx + 1) % 4;
	}
	return -1;
}

void init_group_for_clean(beast_group_t* pbt, uint32_t count)
{
	int pos[4][3] = {{1, 2, 1}, {3, 4, 1}, {6, 7, 1}, {8, 9, 1}};
	for (uint32_t loop = 0; loop < count; loop ++) {
		if (loop == 4) {
			if (pbt->cntlist[0] && !pbt->saset[0].id) {
				int idx = rand() % pbt->cntlist[0];
				memcpy(&pbt->saset[0], &pbt->idlist[pbt->posinfo[0][idx]], sizeof(base_attr_t));
				pbt->count ++;
			}

			if (pbt->cntlist[5] && !pbt->saset[5].id) {
				int idx = rand() % pbt->cntlist[5];
				memcpy(&pbt->saset[5], &pbt->idlist[pbt->posinfo[5][idx]], sizeof(base_attr_t));
				pbt->count ++;
			}
		} else {
			int idx = get_pos_valid_idx(pbt, pos);
			if (idx == -1)
				break;
			pos[idx][2] = 0;
			KDEBUG_LOG(0, "GROUP CLEAN\t[%u %u %u]", idx, pos[idx][0], pbt->posinfo[pos[idx][0]][idx]);
			int bidx = rand() % pbt->cntlist[pos[idx][0]];
			memcpy(&pbt->saset[pos[idx][0]], &pbt->idlist[pbt->posinfo[pos[idx][0]][bidx]], sizeof(base_attr_t));
			pbt->count ++;
			bidx = rand() % pbt->cntlist[pos[idx][1]];
			memcpy(&pbt->saset[pos[idx][1]], &pbt->idlist[pbt->posinfo[pos[idx][1]][bidx]], sizeof(base_attr_t));
			pbt->count ++;
		}
	}
}

void init_beast_group(sprite_t* p, beast_group_t* pbt, beast_group_t* pbt_src)
{
	memcpy(pbt, pbt_src, sizeof(beast_group_t));
	if (pbt->type == group_type_firework) {
		for (int loop = 0; loop < pbt->idcnt; loop ++) {
			if (pbt->idlist[loop].id % SPEC_BEAST_BASE == p->firework_beast) {
				memcpy(&pbt->saset[rand() % BATTLE_POS_CNT], &pbt->idlist[loop], sizeof(base_attr_t));
				pbt->count ++;
				return;
			}
		}
		memcpy(&pbt->saset[rand() % BATTLE_POS_CNT], &pbt->idlist[0], sizeof(base_attr_t));
		pbt->count ++;
	} else if (pbt->type == group_type_refresh && pbt->idcnt) {
		for (int loop = 0; loop < pbt->idcnt; loop ++) {
			for (uint32_t loopi = 0; loopi < pbt->minlist[loop]; loopi ++) {
				int idx = get_group_valid_idx(pbt);
				if (idx == -1) return;
				memcpy(&pbt->saset[idx], &pbt->idlist[loop], sizeof(base_attr_t));
				KDEBUG_LOG(p->id, "GEN BEAST IN GRP 1\t[%d %u]", idx, pbt->saset[idx].id);
				pbt->count ++;
			}
		}

		int randcnt = 0;
		if (pbt->maxcnt) {
			int randmincnt = pbt->mincnt < pbt->count ? 0 : (pbt->mincnt - pbt->count);
			int randmaxcnt = pbt->maxcnt - pbt->count;
			randmaxcnt = randmaxcnt < randmincnt ? randmincnt : randmaxcnt;
			randcnt = rand() % (randmaxcnt - randmincnt + 1) + randmincnt;
		} else {
			randcnt = (2 * p->btr_info->count <= pbt->count) ? 0 : (2 * p->btr_info->count - pbt->count);
		}

		for (int loop = 0; loop < randcnt; loop ++) {
			int idx = rand() % pbt->idcnt;
			int posidx = get_group_valid_idx(pbt);
			if (posidx == -1) return;
			memcpy(&pbt->saset[posidx], &pbt->idlist[idx], sizeof(base_attr_t));
			KDEBUG_LOG(p->id, "GEN BEAST IN GRP 2\t[%d %u]", posidx, pbt->saset[posidx].id);
			pbt->count ++;
		}
	} else if (pbt->type == group_type_clean) {
		if (pbt->cntlist[5] && pbt->idlist[pbt->posinfo[5][0]].id) {
			if(p)	
			memcpy(&pbt->saset[5], &pbt->idlist[pbt->posinfo[5][0]], sizeof(base_attr_t));
		}
		if (pbt->cntlist[0] && pbt->idlist[pbt->posinfo[0][0]].id % SPEC_BEAST_BASE > 2000) {
			memcpy(&pbt->saset[0], &pbt->idlist[pbt->posinfo[0][0]], sizeof(base_attr_t));
		}
		init_group_for_clean(pbt, p->btr_info->count);
	} else if(pbt->type == group_type_rand){
		if (pbt->cntlist[5] && pbt->idlist[pbt->posinfo[5][0]].id){
			uint32_t rate=rand()%RAND_COMMON_RAND_BASE;
			base_attr_t *pba=&pbt->idlist[pbt->posinfo[5][0]];
			if(p->level>=pba->leaderlv[0] && p->level <= pba->leaderlv[1] && rate < pba->percent){
				KDEBUG_LOG(p->id,"init beast grp:add boss[%u %u %u]",pba->id,rate,pba->percent);
				memcpy(&pbt->saset[5],pba , sizeof(base_attr_t));
			}
		}
		if (pbt->cntlist[0] && pbt->idlist[pbt->posinfo[0][0]].id % SPEC_BEAST_BASE > 2000) {
			memcpy(&pbt->saset[0], &pbt->idlist[pbt->posinfo[0][0]], sizeof(base_attr_t));
		}
		init_group_for_clean(pbt, p->btr_info->count);
		//for( uint32_t loop=0;loop<10 ; loop++ ){
			//if(pbt->saset[loop].id)	
				//KDEBUG_LOG(p->id,"has beast =%u after",pbt->saset[loop].id);
		//}
		return ;
	}

}

static uint32_t create_beast_tmp_grp(sprite_t* p, beast_group_t* abg, uint32_t elite_type)
{
	static int randpos[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	if (p->pgrp) {
		rand_pos(randpos, 3);
		uint32_t minlv, maxlv;
		switch (p->grpid) {
		case 0:
			minlv = p->level > 7 ? p->level - 7 : 1;
			maxlv = p->level > 3 ? p->level - 3 : 1;
			break;
		case 1:
			minlv = p->level > 2 ? p->level - 2 : 1;
			maxlv = p->level + 2;
			break;
		case 2:
			minlv = p->level + 3;
			maxlv = p->level + 7;
			break;
		}
			
		abg->saset[randpos[0]].id = p->pgrp->id;
		abg->saset[randpos[0]].minlv = minlv;
		abg->saset[randpos[0]].maxlv = maxlv;

		abg->saset[randpos[1]].id = valid_beasts[p->grpid][rand() % valid_beast_cnt[p->grpid]];
		abg->saset[randpos[1]].minlv = minlv;
		abg->saset[randpos[1]].maxlv = maxlv;

		abg->saset[randpos[2]].id = valid_beasts[p->grpid][rand() % valid_beast_cnt[p->grpid]];
		abg->saset[randpos[2]].minlv = minlv;
		abg->saset[randpos[2]].maxlv = maxlv;
		abg->count = 3;
		return 0;
	}
	map_t* map = p->tiles;
	if (!map) return 0;
	for (int loop = 0; loop < map->task_beast_cnt; loop ++) {
		task_beast_t* ptb = &map->task_beasts[loop];
		uint32_t l;
		for (l = 0; l < ptb->task_cnt; l ++) {
			beast_group_t* pbg = get_beast_grp(ptb->grpid);
			task_db_t* ptr = cache_get_task_info(p, ptb->tasks[l]);
			KDEBUG_LOG(p->id, "TASK BEAST GRP\t[%u %u %p]", ptb->grpid, ptb->tasks[l], pbg);
			if (pbg && ptr && ptr->nodeid >= ptb->startns[l] && ptr->nodeid < ptb->endns[l] \
				&& rand() % BEAST_COMMON_RAND_BASE < ptb->probability) {
				init_beast_group(p, abg, pbg);
				return abg->grpid;
			}
		}
	}

	for (int loop = 0; loop < map->spec_beast_cnt; loop ++) {
		spec_beast_t* psb = &map->spec_beasts[loop];
		beast_group_t* pbg = get_beast_grp(psb->grpid);

		uint32_t seconds = get_now_sec();
		KDEBUG_LOG(p->id, "SPEC BEAST\t[%u %u %u]", psb->start_time, psb->end_time, seconds);
		if (psb->start_time < psb->end_time && (seconds < psb->start_time || seconds > psb->end_time)) {
			continue;
		}

		uint32_t probability;
		if (psb->clothid) {
			cloth_t* pca = get_cloth(psb->clothid);
			body_cloth_t* pbc = cache_get_body_cloth(p, pca->equip_part + BASE_BODY_TYPE);
			probability = pca && (!pbc || psb->clothid != pbc->clothtype) ? psb->probability : 0;
		} else {
			probability = psb->probability;
		}
		
		if (pbg && rand() % BEAST_COMMON_RAND_BASE < probability) {
			init_beast_group(p, abg, pbg);
			return abg->grpid;
		}
	}

	if (IS_MAZE_MAP(map->id) && map->maze_beastcnt) {
		uint32_t count = p->btr_team ? p->btr_team->count : 1;
		count *= 2;
		abg->count = count;

		KDEBUG_LOG(p->id, "BOX BEAST COUNT\t[%u]", count);
		for (uint32_t loop = 0; loop < count; loop ++) {	
			int idx = rand_type_idx(map->maze_beastcnt, map->maze_beastrates, RAND_COMMON_RAND_BASE);
			abg->saset[loop].id = map->maze_beasts[idx];
			abg->saset[loop].minlv = p->level;
			abg->saset[loop].maxlv = 0;
			abg->saset[loop].lvtype = beast_group_lvtype_min;
		}

		for (uint32_t loop = count; loop < BATTLE_POS_CNT; loop ++) {
			abg->saset[loop].id = 0;
		}
	} else if (map->min_beast_cnt) {
		int minmax[6][2] = {{0, 0}, {1, 2}, {3, 5}, {5, 7}, {7, 9}, {9, 10}};
		int max_beast_cnt = map->max_beast_cnt > minmax[p->btr_info->count][1] ? minmax[p->btr_info->count][1] : map->max_beast_cnt;
		int min_beast_cnt = (minmax[p->btr_info->count][0] > max_beast_cnt) ? max_beast_cnt : \
			(map->min_beast_cnt < minmax[p->btr_info->count][0] ? minmax[p->btr_info->count][0] : map->min_beast_cnt);
		max_beast_cnt = (max_beast_cnt < min_beast_cnt) ? min_beast_cnt : max_beast_cnt;
		int user_cnt = min_beast_cnt + rand() % (max_beast_cnt - min_beast_cnt + 1);
		rand_pos(randpos, user_cnt);
		beast_group_t* tmp=get_beast_grp(abg->grpid);
		for (int loop = 0; loop < user_cnt; loop++) {
			int idx = rand_type_idx(map->beastcnt, map->beastrate, BEAST_COMMON_RAND_BASE);
			beast_lvinfo_t* pbl = map->apbl[idx];
			if (!pbl) continue;

			if (loop == 0 && pbl->id != abg->grpid) {
				for (int i = 0; i < map->beastcnt; i ++) {
					if (map->apbl[i]->id == abg->grpid && map->apbl[i]->level != 1) {
						pbl = map->apbl[i];
						break;
					}
				}
			}
			
			abg->saset[randpos[loop]].id = pbl->id;
			abg->saset[randpos[loop]].minlv = abg->saset[randpos[loop]].maxlv = pbl->level;
			if(tmp){
				//KDEBUG_LOG(p->id,"start to find double attack info beastid=%u grpid=%u",pbl->id,abg->grpid);
				for( uint32_t i=0 ; i < tmp->beastcnt ; i++ ){
					if( tmp->datk_info[loop].beastid%SPEC_BEAST_BASE == pbl->id ){
						//KDEBUG_LOG(p->id,"find double attack info id=%u %u datk=%u",pbl->id,randpos[loop],tmp->datk_info[i].double_atk);
						abg->saset[randpos[loop]].double_atk_flag=tmp->datk_info[i].double_atk;
						break;
					}	
				}	
			}	
			
			if (elite_type && pbl->id == abg->grpid && (loop == 0 || rand() % 2))
				abg->elite_type[randpos[loop]] = elite_type;
			abg->count++;
		}
	}
	return 0;
}

int team_busy(sprite_t* p)
{
	if (!p->btr_team)
		return p->busy_state != be_idle;
	
	for (int loop = 0; loop < p->btr_team->count; loop ++) {
		if (p->btr_team->players[loop] && p->btr_team->players[loop]->busy_state != be_idle)
			return 1;
	}
	return 0;
}

static inline void init_load_attack_time(battle_t* pbt)
{
	for (int loop = 0; loop < pbt->count; loop ++) {
		if (pbt->players[loop]) {
			pbt->players[loop]->round_attack_cnt = 0;
			pbt->players[loop]->last_attack_time = get_now_tv()->tv_sec;
		}
	}
}

int batrserv_challenge_battle_callback(batrserv_proto_t* btrpkg, int len)
{
	if (btrpkg->ret)
		ERROR_RETURN(("batsvr callback\t[%u %u]", btrpkg->id, btrpkg->ret), -1);
	sprite_t* p = get_sprite(btrpkg->id);
	if (!p || !p->btr_info) {
		ERROR_RETURN(("user alrdy offline\t[%u]", btrpkg->id), 0);
	}
	init_load_attack_time(p->btr_info);

	if (p->btr_info->pkteam && IS_NORMAL_ID(p->btr_info->pkteam)) {
		auto_challenge_on_other_online(p);
		return 0;
	}
	
	userid_t challengee = *(uint32_t *)p->session;
	uint32_t elite_type = *(uint32_t *)(p->session + 4);
	*(uint32_t *)(p->session + 8)= btrpkg->btid;
	KDEBUG_LOG(p->id, "BATRSERV CHALLENGE CALLBACK\t[%u]", challengee);

	// challenge the beast
	if (IS_BEAST_ID(challengee)) {
		response_proto_uint32(p, p->waitcmd, 2, 1, 0);
		beast_group_t bg;
		memset(&bg, 0, sizeof bg);
		bg.grpid = challengee;
		DEBUG_LOG("XXXXXX OPT grpid=%u ", bg.grpid );
		p->btr_info->pkteam = create_beast_tmp_grp(p, &bg, elite_type);
		beast_accept_battle(p, &bg, p->btr_info, p->btr_info->pkteam);
		return 0;
	}
	// challengee in the same online server
	sprite_t* lp = get_sprite(challengee);	
	if (lp) {
		if (lp->btr_info) {
			KERROR_LOG(p->id, "need cancel batsrv battle\t[%u]", lp->id);
			return batrserv_cancel_battle(p);
		}
		
		if (lp->pk_switch_is_on() ) {
			if (lp->btr_team && !TEAM_LEADER(lp)) {
				KERROR_LOG(p->id, "not team leader\t[%u]", lp->id);
				return send_to_self_error(p, p->waitcmd, cli_err_other_not_leader, 1);
			}
			if (team_busy(lp)) {
				KERROR_LOG(p->id, "fighting now or busy\t[%u %u]", lp->id, lp->busy_state);
				return send_to_self_error(p, p->waitcmd, cli_err_user_busy, 1);
			} 

			KDEBUG_LOG(p->id, "AUTO ACCEPT BATTLE\t[%u]", lp->id);
			init_challengee_battle(lp, btrpkg->btid, 0);
			batrserv_accept_battle(lp);
		} else {
			notify_other_leader_challenging_state(lp, p->btr_info->battle_id, 1);
			KDEBUG_LOG(p->id, "CHALLENGE NOTIFY OTHER LEADER\t[%u %lu]", lp->id, p->btr_info->battle_id);
			response_proto_uint32(p, p->waitcmd, 1, 1, 0);
		}
		//notify_self_team_challenging(p, challengee);
		return 0;
	}
	challenge_user_on_other_online(p, challengee);
	return 0;
}

int batrserv_challenge_npc_battle_callback(batrserv_proto_t* btrpkg, int len)
{
	if (btrpkg->ret)
		ERROR_RETURN(("batsvr callback\t[%u %u]", btrpkg->id, btrpkg->ret), -1);
	sprite_t* p = get_sprite(btrpkg->id);
	if (!p || !p->btr_info) {
		ERROR_RETURN(("user alrdy offline\t[%u]", btrpkg->id), 0);
	}

	init_load_attack_time(p->btr_info);

	// challenge the beast
	beast_group_t* pbt = *(beast_group_t* *)p->session;

	if (pbt) {
		KDEBUG_LOG(p->id, "BATRSERV CHALLENGE NPC CALLBACK\t[%u]", pbt->grpid);
		response_proto_head(p, p->waitcmd, 1, 0);	

		beast_group_t bg;
		
		init_beast_group(p, &bg, pbt);
	
		
		beast_accept_battle(p, &bg, p->btr_info, pbt->grpid);
		return 0;
	}

	p->waitcmd = 0;
	return 0;
}

int batrserv_accept_battle_callback(batrserv_proto_t* btrpkg, int len)
{
	sprite_t* lp = get_sprite(btrpkg->id);	
	if (btrpkg->ret) {
		KERROR_LOG(btrpkg->id, "accept battle btsvr err\t[%u %u]", btrpkg->ret, low32_val(btrpkg->btid));
		if (lp) {
			return send_to_self_error(lp, lp->waitcmd, cli_err_self_be_fighting, 1);
		}
	}

	if (lp && lp->btr_info && lp->btr_info->pkteam && IS_NORMAL_ID(lp->btr_info->pkteam)) {
		return 0;
	}
	
	sprite_t* lop = get_sprite(BATTLE_USER_ID(btrpkg->btid));
	if (lp && !lp->pk_switch_is_on()) {
		response_proto_head(lp, lp->waitcmd, 1, 0);
		KDEBUG_LOG(lp->id, "ACCEPT BATTLE CALLBACK");
	}
	if (lop && lp && lp->pk_switch_is_on()) {
		response_proto_uint32(lop, lop->waitcmd, 3, 1, 0);
	} else if (lop && !IS_BEAST_ID(btrpkg->id)) {
		notify_challger_accept_state(lop, btrpkg->id, 1);
	}
	return 0;
}

int sync_user_cloth_duration(sprite_t* p, cloth_duration_t* p_cd)
{
	for (int loop = 0; loop < MAX_ITEMS_WITH_BODY; loop ++) {
		if (!p_cd[loop].duration) continue;
		KDEBUG_LOG(p->id, "CLOTH DURATION\t[%u %u]", p_cd[loop].equip_pos, p_cd[loop].duration);
		
		body_cloth_t* pbc = cache_get_body_cloth(p, p_cd[loop].equip_pos + BASE_BODY_TYPE);
		if (pbc && pbc->duration_max != 0xFFFF) {
			uint32_t old_duration = pbc->duration;
			pbc->duration = pbc->duration < p_cd[loop].duration ? 0 : pbc->duration - p_cd[loop].duration;	
			//try_to_send_for_clothes(p, pbc->clothtype, old_duration, pbc->duration, pbc->duration_max);
			if (pbc->duration != old_duration) {
				g_hash_table_insert(p->cloth_uplist, &pbc->clothid, pbc);
			}
		}

		pbc = cache_get_body_cloth(p, p_cd[loop].equip_pos + 100 + BASE_BODY_TYPE);
		if (pbc && pbc->duration_max != 0xFFFF) {
			uint32_t old_duration = pbc->duration;
			pbc->duration = pbc->duration < p_cd[loop].duration ? 0 : pbc->duration - p_cd[loop].duration;	
			//try_to_send_for_clothes(p, pbc->clothtype, old_duration, pbc->duration, pbc->duration_max);
			if (pbc->duration != old_duration) {
				g_hash_table_insert(p->cloth_uplist, &pbc->clothid, pbc);
			}
		}
	}
	
	return 0;
}

void noti_cli_update_pkpt(sprite_t* p, int max_pt, int npt, int pkapply)
{
	uint8_t out[64];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, pkapply, i);
	PKG_UINT32(out, p->pkpoint_total, i);
	PKG_UINT32(out, max_pt - p->pkpoint_day, i);
	PKG_UINT32(out, npt, i);
	init_proto_head(out, proto_cli_sync_pkinfo, i);
	send_to_self(p, out, i, 0);
	//send to watchers
	if( p->btr_info ){
		send_to_battle_watchers(p->btr_info,out,i);
	}
}

void sync_user_pkinfo(sprite_t* p, battle_t* pbt, double enemy_lv)
{
	int npt = 0;
    enemy_lv = enemy_lv >= 1 ? enemy_lv : 1;
	if ((pbt->average_lv > enemy_lv && (pbt->average_lv - enemy_lv) > 5)) {
		double dpt = pow(pbt->average_lv, 5.0/6) / (pbt->average_lv - enemy_lv - 4) * (1 + 0.2 * (pbt->count - 1));
		npt = dpt + 0.5;
	} else {
		double dpt = pow(pbt->average_lv, 5.0/6) * (1 + 0.2 * (pbt->count - 1));
		npt = dpt + 0.5;	
	}

	int32_t max_pt = MAX_PKPT_DAY(p) >= p->pkpoint_day ?  MAX_PKPT_DAY(p) : p->pkpoint_day;	
	npt = (p->pkpoint_day + npt) > max_pt ? (max_pt - p->pkpoint_day) : npt;

	if (IS_ADMIN(p)) {
		npt = npt + 1000;
	}

	p->pkpoint_total += npt;
	p->pkpoint_day += npt;

	noti_cli_update_pkpt(p, max_pt, npt, pbt->pkapply);

	KDEBUG_LOG(p->id, "PK PT POINT\t[%d %d]", p->pkpoint_day, p->pkpoint_total);
	
	if (npt) {
		msg_log_pk_point(p->id, p->pkpoint_total);
		db_update_pkinfo(NULL, p->id, p->pkpoint_total, p->pkpoint_day, (pbt->count == 1));
	}
}

void rsp_out_battle_end(sprite_t* p, uint32_t fly_out)
{
	battle_t* lpbt = p->btr_info;
	KDEBUG_LOG(p->id, "OUT END\t[%u %u]", p->recv_map, lpbt->battletype);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, lpbt->battletype, i);
	PKG_UINT32(msg, fail, i);
	PKG_UINT32(msg, 1, i);
	PKG_UINT32(msg, (fly_out && lpbt->battletype != be_pk) ? p->recv_map : 0, i);
	PKG_UINT32(msg, 0, i);
	i += pkg_ratio_buff(p->vip_buffs[0], msg + i);
	i += pkg_ratio_buff(p->vip_buffs[1], msg + i);
	i += pkg_ratio_buff(p->vip_buffs[2], msg + i);
	PKG_UINT32(msg, get_auto_fight_count(p), i);
	PKG_UINT32(msg, p->vip_buffs[buffid_hpmp][0], i);
	PKG_UINT32(msg, p->vip_buffs[buffid_hpmp][1], i);
	init_proto_head(msg, proto_cli_battle_end, i);
	send_to_self(p, msg, i, 0);
	if( p->btr_info ){
		//send to watchers
 		send_to_battle_watchers(p->btr_info,msg,i);
	}
}

int batrserv_noti_user_out_callback(batrserv_proto_t* btrpkg, uint32_t len)
{
	DEBUG_LOG("batr noti user out callback");
	const uint32_t fix_len = sizeof(batrserv_proto_t) + 44 + MAX_ITEMS_WITH_BODY * sizeof(cloth_duration_t);
	CHECK_BODY_LEN_GE(len, fix_len);
	int j = 0;
	uint32_t uid;
	UNPKG_H_UINT32(btrpkg->body, uid, j);

	sprite_t* lp = get_sprite(uid);	
	if (lp) {
		lp->pgrp = NULL;
		lp->grpid = 0;
		uint32_t petid, hp, mp, injury_lv, autofight, fly_out, pet_cnt;
		UNPKG_H_UINT32(btrpkg->body, hp, j);
		UNPKG_H_UINT32(btrpkg->body, mp, j);
		UNPKG_H_UINT32(btrpkg->body, injury_lv, j);
		UNPKG_H_UINT32(btrpkg->body, lp->default_skills[0], j);
		UNPKG_H_UINT32(btrpkg->body, lp->default_skills[1], j);
		UNPKG_H_UINT32(btrpkg->body, lp->default_levels[0], j);
		UNPKG_H_UINT32(btrpkg->body, lp->default_levels[1], j);
		UNPKG_H_UINT32(btrpkg->body, autofight, j);
		UNPKG_H_UINT32(btrpkg->body, fly_out, j);
		injury_lv = INJURY_NONE;//injury_lv >= INJURY_MAX ? INJURY_RED : injury_lv;
		sync_user_cloth_duration(lp, (cloth_duration_t *)(btrpkg->body + j));
		cache_set_cloth_attr(lp);
		j += MAX_ITEMS_WITH_BODY * sizeof(cloth_duration_t);

		battle_t* lpbt = lp->btr_info;
		if (lpbt && lpbt->battle_id == btrpkg->btid) {
			update_fight_buff(lp, autofight);
			if (lpbt->battletype != be_pk) {
				lp->hp = hp ? hp : 1;
				lp->mp = mp;
				//update_user_second_attr(lp, (base_5_attr_t *)&lp->physique, injury_lv);
				db_add_exp_level(lp);
				db_set_user_injury(NULL, lp->id, lp->injury_lv);
				lp->update_flag = set_bit_off32(lp->update_flag, update_flag_hp_mp_bit);
	            UNPKG_H_UINT32(btrpkg->body, pet_cnt, j);
		        CHECK_BODY_LEN(len, 36 * pet_cnt + fix_len);
				for (uint32_t loop = 0; loop < pet_cnt; loop ++) {
					UNPKG_H_UINT32(btrpkg->body, petid, j);
					pet_t* pp = get_pet_inbag(lp, petid);
					if (!pp) {
						j += 32;
						continue;
					}
					UNPKG_H_UINT32(btrpkg->body, pp->hp, j);
					pp->hp = pp->hp ? pp->hp : 1;
					UNPKG_H_UINT32(btrpkg->body, pp->mp, j);
					UNPKG_H_UINT32(btrpkg->body, pp->injury_lv, j);
					pp->injury_lv = INJURY_NONE;//pp->injury_lv >= INJURY_MAX ? INJURY_RED : pp->injury_lv;
					UNPKG_H_UINT32(btrpkg->body, pp->default_skills[0], j);
					UNPKG_H_UINT32(btrpkg->body, pp->default_skills[1], j);
					UNPKG_H_UINT32(btrpkg->body, pp->default_levels[0], j);
					UNPKG_H_UINT32(btrpkg->body, pp->default_levels[1], j);
					j += 4;
					calc_pet_second_level_attr(pp);
					db_add_pet_exp_level(lp, pp);
					db_set_pet_injury(NULL, lp->id, pp->petid, pp->injury_lv);
				}
			} else {
				noti_cli_update_pkpt(lp, MAX_PKPT_DAY(lp), 0, lpbt->pkapply);
				if (lpbt->count == 1) {
					msg_log_single_pk();
				} else {
					msg_log_team_pk();
				}
			}

			if (lp->team_state == load_bt_fail) {
				KDEBUG_LOG(lp->id, "OUT CANCEL BUSY STATE");
				cancel_battle_state(lp);
			} else {
				KDEBUG_LOG(lp->id, "OUT WAIT ANIMATION");
				lp->team_state = wait_animation;
			}

			rsp_out_battle_end(lp, fly_out);
			del_user_battle_team(lp);
			lp->boxid = 0;
		}
	}
	return 0;
}

int noti_reject_battle_callback(batrserv_proto_t* btrpkg, int len)
{
	KDEBUG_LOG(btrpkg->id, "BATTLE SVR REJECT CHALLENGE");
	sprite_t* lp = get_sprite(btrpkg->id);	

	if (lp && lp->btr_info) {
		uint8_t buff[32];
		int j = sizeof(protocol_t);
		PKG_UINT32(buff, lp->id, j);
		init_proto_head(buff, proto_cli_reject_challenge, j);
		send_to_battle(lp->btr_info, buff, j);
		free_batter_info_by_leader(lp->btr_info);
		
		KDEBUG_LOG(lp->id, "REJECT BATTLE");
		notify_challger_accept_state(lp, btrpkg->id, 2);
	}
	
	return 0;
}

int noti_cancel_battle_callback(batrserv_proto_t* btrpkg, uint32_t len)
{
	CHECK_BODY_LEN_GE(len, sizeof(batrserv_proto_t));
	sprite_t* lp = get_sprite(btrpkg->id);	
	
	if (btrpkg->ret) {
		KERROR_LOG(btrpkg->id, "BATTLE SVR CANCEL RET\t[%u %u %u]", btrpkg->ret, high32_val(btrpkg->btid), low32_val(btrpkg->btid));
		if (lp)
			return send_to_self_error(lp, proto_cli_cancel_challenge, cli_err_client_not_proc, 1);
		return 0;
	}
	CHECK_BODY_LEN(len, sizeof(batrserv_proto_t) + 4);

	if (lp && lp->btr_info) {
		uint32_t uid = *(uint32_t *)btrpkg->body;
		sprite_t* lop = get_sprite(uid);
		KDEBUG_LOG(lp->id, "CANCEL BATTLE BTRSVR BACK\t[%u]", uid);
		if (lop) {
			notify_other_leader_challenging_state(lop, lp->btr_info->battle_id, 2);
		}
		
		uint8_t buff[32];
		int j = sizeof(protocol_t);
		PKG_UINT32(buff, lp->id, j);
		init_proto_head(buff, proto_cli_cancel_challenge, j);
		send_to_battle_except_uid(lp->btr_info, buff, j, lp->id);
		free_batter_info_by_leader(lp->btr_info);
		response_proto_uint32(lp, proto_cli_cancel_challenge, lp->id, 1, 0);
	}
	
	return 0;
}

int noti_battle_created_callback(batrserv_proto_t* btrpkg, int len)
{
	battle_t* lpbt = get_batter_info(btrpkg->id, btrpkg->btid);	
	if (lpbt) {
		uint8_t out[8192];
		memcpy(out, btrpkg->body, sizeof(protocol_t));
		protocol_t* proto = (protocol_t *)out;
		proto->len = htonl(ntohl(proto->len) + 8);
		*(uint32_t *)(out + sizeof(protocol_t)) = htonl(lpbt->pkteam);
		*(uint32_t *)(out + sizeof(protocol_t)+4) = htonl(0);
		memcpy(out + sizeof(protocol_t) + 8, btrpkg->body + sizeof(protocol_t), btrpkg->len - sizeof(batrserv_proto_t) - sizeof(protocol_t));
		send_to_battle(lpbt, out, btrpkg->len - sizeof(batrserv_proto_t) + 8);
		//cache data for watching
		//send to watchers ,第二个参数用来表示是否是观战模式
		*(uint32_t *)(out + sizeof(protocol_t)+4) = htonl(1);
		memcpy(lpbt->cachedata,out,sizeof(out));
		lpbt->datalen=btrpkg->len - sizeof(batrserv_proto_t) + 8;
		
		KDEBUG_LOG(btrpkg->id, "BATTLE CREATE\t[%u %u]", high32_val(btrpkg->btid), low32_val(btrpkg->btid));
		lpbt->state = bt_state_create;
	}
	return 0;
}

int noti_battle_started_callback(batrserv_proto_t* btrpkg, uint32_t len)
{
	battle_t* lpbt = get_batter_info(btrpkg->id, btrpkg->btid);	
	if (lpbt) {
		int i = sizeof(protocol_t);
		CHECK_BODY_LEN_GE(len, sizeof(batrserv_proto_t) + i + 8);
		UNPKG_UINT32(btrpkg->body, lpbt->enemy_lv, i);
		uint32_t count;
		UNPKG_UINT32(btrpkg->body, count, i);
		CHECK_BODY_LEN(len, sizeof(batrserv_proto_t) + i + count * 8);
		for (uint32_t loop = 0; loop < count; loop ++) {
			uint8_t out[64];
			int j = sizeof(protocol_t);
			uint32_t uid, result;
			UNPKG_UINT32(btrpkg->body, uid, i);
			UNPKG_UINT32(btrpkg->body, result, i);
			sprite_t* s = get_sprite(uid);
			if (!s || s->btr_info != lpbt)
				continue;
			PKG_UINT32(out, result, j);
			s->team_state = result ? load_bt_succ : load_bt_fail;
			init_proto_head(out, proto_cli_noti_battle_started, j);
			send_to_self(s, out, j, 0);
			//send to watchers
			s->btr_info->load_result=result+1;//缓存战斗结果用于观战者1317协议，判断战斗者是否发了1307协议
			send_to_battle_watchers(s->btr_info,out,j);//发给观战者1307协议
		}
		KDEBUG_LOG(btrpkg->id, "BATTLE START\t[%u %u]", high32_val(lpbt->battle_id), low32_val(lpbt->battle_id));
		lpbt->state = bt_state_fight;
	}
	return 0;
}

int noti_user_fighted_callback(batrserv_proto_t* btrpkg, int len)
{
	battle_t* lpb = get_batter_info(btrpkg->id, btrpkg->btid);	
	if (lpb) {
		send_to_battle(lpb, btrpkg->body, btrpkg->len - sizeof(batrserv_proto_t));
		//send to watchers
		send_to_battle_watchers(lpb,btrpkg->body,btrpkg->len- sizeof(batrserv_proto_t));
	}
	return 0;
}

int noti_user_load_rate_callback(batrserv_proto_t* btrpkg, int len)
{
	battle_t* lpb = get_batter_info(btrpkg->id, btrpkg->btid);	
	if (lpb) {
		send_to_battle(lpb, btrpkg->body, btrpkg->len - sizeof(batrserv_proto_t));
	}
	return 0;
}

uint32_t alter_pet_exp(sprite_t *p, uint32_t factor)
{
	if(ISVIP(p->flag) && p->viplv >=3){
			factor *=2;
			factor = factor>=500 ? 500:factor;
	}else{
			factor *=1.5;
			factor = factor>=400 ? 400:factor;
	}
	return factor;
}

int sync_pet_base_attr(sprite_t* p, atk_attr_t* auaa, pet_t* pp, uint8_t* pbuff, uint32_t expfactor)
{
	uint32_t add_exp = 0, level = 0, add_addition = 0;
	skill_t* ps = cache_get_skill(p, skill_life_pet_raise);
	uint32_t pet_raise_lv = ps ? ps->skill_level : 0;
	int i = 0;
	pp->hp = auaa->hp ? auaa->hp : 1;
	pp->mp = auaa->mp;
	KDEBUG_LOG(p->id, "PET END\t[%u %u %u %d]", auaa->hp, auaa->mp, auaa->injury_lv, auaa->expri);
	//if (pp->injury_lv != auaa->injury_lv) {
		//if (auaa->injury_lv >= INJURY_MAX) {
			//KERROR_LOG(p->id, "batsvr injury invalid\t[%u %u]", pp->petid, auaa->injury_lv);
			//auaa->injury_lv = INJURY_RED;
		//}
		//pp->injury_lv = auaa->injury_lv;
		//db_set_pet_injury(0, p->id, pp->petid, pp->injury_lv);
	//}

	if (auaa->expri == -1) {
		add_exp = -1;
	} else if (expfactor) {
		uint32_t pet_exp_factor=get_pet_exp_factor(p, pp, expfactor);
		KDEBUG_LOG(p->id,"pet exp factor111=%u",pet_exp_factor);
		//changed by francisco 2012-5-18  普通用户获得1.5倍  vip加速2倍 
		pet_exp_factor= alter_pet_exp(p, pet_exp_factor);	
		KDEBUG_LOG(p->id,"pet exp factor222=%u",pet_exp_factor);
		add_exp = auaa->expri * pet_exp_factor* (100 + 5 * pet_raise_lv) / 10000;
		
		LIMIT_BATTER_EXP(add_exp, p->id);
		pp->experience += add_exp;
		LIMIT_MAX_EXP(pp->experience, p->id);
		LIMIT_PET_MAX_EXP(pp->experience, p->id);
		level = calc_pet_level_from_exp(pp->experience, pp->level);
		level = level - pp->level;
		pp->level += level;
		calc_pet_5attr(pp);
		calc_pet_second_level_attr(pp);
		add_addition = level;
		pp->attr_addition += add_addition;
		//KDEBUG_LOG(pp->petid,"111 %u %u",pp->level,level);
		if (level) {
			if (pp->level > p->level + 5) {
				pp->action_state = just_in_bag;
				db_set_pet_state(0, p->id, pp->petid, just_in_bag);
			}
			pp->hp = pp->hp_max;
			pp->mp = pp->mp_max;
			msg_log_pet_level(pp->level, pp->level - level);
			update_pet_skill_level(p, pp);
		}
	}

	cache_auto_hpmp_for_pet(p, pp);
	db_add_pet_exp_level(p, pp);

	PKG_UINT32 (pbuff, pp->petid, i);
	PKG_UINT32 (pbuff, level, i);
	PKG_UINT32 (pbuff, add_exp, i);
	PKG_UINT16 (pbuff, add_addition, i);
	return i;

}

sprite_t* get_user_in_battle(battle_t* pb, uint32_t uid)
{
	for (uint32_t loop = 0; loop < pb->count; loop ++) {
		if (pb->players[loop] && pb->players[loop]->id == uid)
			return pb->players[loop];
	}
	return NULL;
}


int sync_user_base_attr(sprite_t* p, atk_attr_t* auaa, uint8_t* pbuff, beast_group_t* pbg, uint32_t expfactor, battle_t* pb, int taskgrp)
{
	p->hp = auaa->hp ? auaa->hp : 1;
	p->mp = auaa->mp;
	//if (p->injury_lv != auaa->injury_lv) {
		//if (auaa->injury_lv >= INJURY_MAX)
			//auaa->injury_lv = INJURY_RED;
		//update_user_second_attr(p, (base_5_attr_t *)&p->physique, auaa->injury_lv);
		//db_set_user_injury(0, p->id, p->injury_lv);
	//}

	uint32_t add_exp = (pbg && (pbg->limit_exp >= 0) ? pbg->limit_exp : auaa->expri) * get_user_exp_factor(p, expfactor) / 100;
	KDEBUG_LOG(p->id, "SYNC BASE ATTR\t[%u %u %u %u %u]", p->hp, p->mp, p->injury_lv, auaa->expri, add_exp);
	int needlog = add_exp ? 1 : 0;

	auaa->protect_exp = 0;
	if (p->level >= MAX_USER_LEVEL)
		add_exp = 0;

	LIMIT_BATTER_EXP(add_exp, p->id);
	if (p->relationship.flag == relation_pupil) {
		uint32_t master_cnt = 0;
		sprite_t* masters[MAX_RELATION_USER];
		for (uint32_t loop = 0; loop < p->relationship.count; loop ++) {
			sprite_t* s = get_user_in_battle(pb, p->relationship.users[loop].uid);
			if (s) {
				masters[master_cnt ++] = s;
				KDEBUG_LOG(p->id, "FIND MASTER\t[%u %u]", p->relationship.flag, s->id);
			}
		}

		if (master_cnt) {
			add_exp = add_exp * 120 / 100;
			uint32_t add_intimacy = (add_exp / 100 + master_cnt / 2) / master_cnt + 100 * taskgrp;
			KDEBUG_LOG(p->id, "add_intimacy\t[%u %u]", p->relationship.flag, add_intimacy);
			for (uint32_t loop = 0; loop < master_cnt && add_intimacy; loop ++) {
				mole2_add_relation_val_in db_in;
				db_in.uid = p->id;
				db_in.addval = add_intimacy;
				send_msg_to_db(NULL, masters[loop]->id, mole2_add_relation_val_cmd, &db_in);
				for (uint32_t loopi = 0; loopi < masters[loop]->relationship.count; loopi ++) {
					if (masters[loop]->relationship.users[loopi].uid == p->id) {
						masters[loop]->relationship.users[loopi].intimacy += add_intimacy;
					}
				}
			}
		}
	}
	p->experience += add_exp;

	LIMIT_MAX_EXP(p->experience, p->id);
	LIMIT_USER_MAX_EXP(p->experience, p->id);
	uint32_t level = calc_level_from_exp(p->experience, p->level);
	level -= p->level;
	
	uint32_t add_addition = 4 * level;
	p->attr_addition += add_addition;
	if (level) {
		p->hp = p->hp_max;
		p->mp = p->mp_max;		
	}

	cache_auto_hpmp_for_user(p);
	update_when_level_change(p, p->level, p->level + level);
	db_add_exp_level(p);
	monitor_sprite(p, "BATTLE END");

	int i = 0;
	PKG_UINT32 (pbuff, 0, i);
	PKG_UINT32 (pbuff, level, i);
	PKG_UINT32 (pbuff, add_exp, i);
	PKG_UINT32 (pbuff, auaa->protect_exp, i);
	PKG_UINT16 (pbuff, add_addition, i);

	if (needlog)
		KDEBUG_LOG(p->id, "PVE END\t[lv=%u exp=%u %u]", p->level, p->experience, add_exp);

	return i;
}

void gen_task_useskill_target(sprite_t* p, uint32_t skillid, uint32_t count)
{
	GList* ptasks = g_hash_table_get_values(p->recvtasks);
	GList* head = ptasks;
	while (ptasks) {
		task_db_t* pt = (task_db_t *)ptasks->data;
		task_node_t* ptn = get_task_node(pt->taskid, pt->nodeid + 1);
		if (ptn && ptn->skilltarget.itemid == skillid) {
			if (pt->svr_buf[svr_buf_skill] + count > ptn->skilltarget.count)
				pt->svr_buf[svr_buf_skill] = ptn->skilltarget.count;
			else
				pt->svr_buf[svr_buf_skill] += count;
			db_set_task_svr_buf(NULL, p->id, pt);
		}
		ptasks = ptasks->next;
	}
	g_list_free(head);
}

int sync_user_skills(sprite_t* p, skill_exp_t* p_exp, uint8_t* pbuff, uint32_t expfactor)
{
	uint8_t out[1024];
	uint8_t* p_count = pbuff;
	uint32_t* p_db_cnt = (uint32_t *)out;
	*p_count = 0;
	*p_db_cnt = 0;
	int i = 1, dbi = 4;
	for (int j = 0; j < p->skill_cnt; j ++) {
		skill_info_t* psi = get_skill_info(p_exp[j].skill_id);
		if (!psi || (p_exp[j].skill_id != p->skills[j].skill_id)) {
			KERROR_LOG(p->id, "add skill exp err: %p %u", psi, p->skills[j].skill_id);
			continue;
		}

		if (p_exp[j].skill_use_count) {
			gen_task_useskill_target(p, p_exp[j].skill_id, p_exp[j].skill_use_count);
		}

		if (p_exp[j].skill_exp == 0) {
			PKG_UINT32 (pbuff, p->skills[j].skill_id, i);
			PKG_UINT8 (pbuff, 0, i);
			PKG_UINT32 (pbuff, 0, i);
			(*p_count) ++;
		} else {
			uint32_t skill_exp = p->skills[j].skill_exp;
			int skill_lv = p->skills[j].skill_level;
			if (p_exp[j].skill_exp - p->skills[j].skill_exp > 10000) {
				KERROR_LOG(p->id, "skill exp over max\t[%u %u]", p->skills[j].skill_id, p_exp[j].skill_exp);
				p_exp[j].skill_exp = p->skills[j].skill_exp;
			}
			uint32_t add_exp = (p_exp[j].skill_exp - p->skills[j].skill_exp) * get_skill_exp_factor(p, expfactor) / 100;
			add_exp = IS_ADMIN(p) ? 10000 : add_exp;
			cache_update_skill_level(p, &p->skills[j], psi, add_exp + p->skills[j].skill_exp);
			KDEBUG_LOG(p->id, "ADD SKILL EXP\t[%u %u %u]", skill_exp, add_exp, p->skills[j].skill_exp);
			
			PKG_UINT32(pbuff, p->skills[j].skill_id, i);
			PKG_UINT8(pbuff, p->skills[j].skill_level - skill_lv, i);
			PKG_UINT32(pbuff, p->skills[j].skill_exp - skill_exp, i);
			(*p_count) ++;
			if (p->skills[j].skill_exp != skill_exp) {
				PKG_H_UINT32(out, p->skills[j].skill_id, dbi);
				PKG_H_UINT32(out, p->skills[j].skill_level, dbi);
				PKG_H_UINT32(out, p->skills[j].skill_exp, dbi);
				(*p_db_cnt) ++;
			}
		}
	}
	send_request_to_db(0, p->id, proto_db_set_skill_lv, out, dbi);

	return i;
}

int rand_out_item(item_t* items, uint32_t item_cnt, item_t* out_items, int out_cnt)
{	
	if (item_cnt > MAX_BATTLE_ITEM) {
		for (int j = 0; j < MAX_BATTLE_ITEM; j ++) {
			item_t temp;
			int cidx = rand() % item_cnt;
			memcpy(&temp, &items[cidx], sizeof(item_t));
			memcpy(&items[cidx], &items[j], sizeof(item_t));
			memcpy(&items[j], &temp, sizeof(item_t));
		}
		item_cnt = MAX_BATTLE_ITEM;
	}

	for (uint32_t loop = 0; loop < item_cnt; loop ++) {
		int add_type = 1;
		for (int j = 0; j < out_cnt; j ++) {
			if (out_items[j].itemid == items[loop].itemid && get_item(out_items[j].itemid)) {
				out_items[j].count += items[loop].count;
				add_type = 0;
				break;
			}
		}

		if (add_type && out_cnt < MAX_BATTLE_ITEM) {
			out_items[out_cnt].itemid = items[loop].itemid;
			out_items[out_cnt].count = items[loop].count;
			out_cnt ++;
		}
	}

	return out_cnt;
}

int sync_normal_items (sprite_t* p, beast_lvinfo_t* pbl, item_t* items, uint32_t item_cnt)
{
	for (int i = 0; i < pbl->item_cnt && item_cnt < MAX_TEMP_ITEM; i ++) {
		uint32_t rate = get_ol_time(p) > 3 * 60 * 60 ? pbl->items[i].rate / 2 : pbl->items[i].rate;
		if (rand() % BEAST_COMMON_RAND_BASE < rate) {
			items[item_cnt].itemid = pbl->items[i].itemid;
			items[item_cnt].count = 1;
			item_cnt ++;
		}
	}

	int idx = rand_type_idx(PRODUCT_LV_CNT + 1, cloth_drop_rate[(pbl->level + 4) / 5], CLOTH_COMMON_RAND_BASE);
	KDEBUG_LOG(p->id, "DROP CLOTH\t[%d %u %u %u %u %u]", idx, \
		cloth_drop_rate[(pbl->level + 4) / 5][0], 
		cloth_drop_rate[(pbl->level + 4) / 5][1], 
		cloth_drop_rate[(pbl->level + 4) / 5][2], 
		cloth_drop_rate[(pbl->level + 4) / 5][3], 
		cloth_drop_rate[(pbl->level + 4) / 5][4]);
	if (item_cnt < MAX_TEMP_ITEM && idx < PRODUCT_LV_CNT) {
		uint32_t clothid = drop_clothes[(pbl->level + 4) / 5][rand() % 100];
		cloth_t* pc = get_cloth(clothid);
		if (pc && pc->pro_attr[idx].valid) {
			items[item_cnt].itemid = clothid;
			items[item_cnt].count = idx;
			item_cnt ++;
		}
	}

	return item_cnt;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  战斗结束后同步任务中获得的物品 杀怪的数量
 * 杀怪没有计数时 注意检查节点是否正确 可能前一节点没有提交
 */
/* ----------------------------------------------------------------------------*/
int sync_task_items (sprite_t* p, battle_t* pb, uint32_t beasttype, item_t* items, uint32_t item_cnt)
{
	GList* ptasks = g_hash_table_get_values(p->recvtasks);
	GList* head = ptasks;
	while (ptasks) {
		task_db_t* ptd = (task_db_t *)ptasks->data;
		task_node_t* ptn = get_task_node(ptd->taskid, ptd->nodeid + 1);
		//DEBUG_LOG("aaaaaaaa node=%u id=%u",ptd->nodeid + 1,ptd->taskid);
		if (ptn && ptn->itarget.beastid == beasttype) {
			uint32_t dropodds = get_ol_time(p) > 3 * 60 * 60 && ptd->taskid != 39001 ? ptn->itarget.dropodds / 2 : ptn->itarget.dropodds;
			if (ptn->itarget.cntlimit != 0xFFFF && ptn->itarget.cntlimit <= ptd->svr_buf[svr_buf_item_gen])
				dropodds = 0;

			if ((rand() % BEAST_COMMON_RAND_BASE < dropodds) && item_cnt < MAX_TEMP_TASK_ITEM) {
				KDEBUG_LOG(p->id, "GET TASK ITEM\t[%u]", ptn->itarget.itemid);
				items[item_cnt].itemid = ptn->itarget.itemid;
				items[item_cnt].count = 1;
				if (items[item_cnt].itemid == 290012) {
					int add_cnt = rand_type_idx(3, ptn->itarget.dropratio, RAND_COMMON_RAND_BASE);
					KDEBUG_LOG(p->id, "ADD CNT\t[%u]", add_cnt);
					items[item_cnt].count += add_cnt;
				}
				
				if (ptn->itarget.cntlimit != 0xFFFF) {
					ptd->svr_buf[svr_buf_item_gen] += 1;
					db_set_task_svr_buf(NULL, p->id, ptd);
				}
				item_cnt ++;
			}
		}

		//KDEBUG_LOG(p->id, "BBBBBBBB\t[%u %u %u %u %u %u %u]", beasttype, ptn->btarget.beastid,
				//ptn->btarget.mapid, pb->mapid,ptn->btarget.count,ptn->btarget.type,ptd->svr_buf[svr_buf_beast]);
		//检查是否杀怪计数
		if (ptn && ptn->btarget.type == beast_target_beast && (!ptn->btarget.mapid || ptn->btarget.mapid == pb->mapid) \
			&& beasttype == ptn->btarget.beastid && ptd->svr_buf[svr_buf_beast] < ptn->btarget.count) {
			ptd->svr_buf[svr_buf_beast] += 1;
			db_set_task_svr_buf(NULL, p->id, ptd);
			//KDEBUG_LOG(p->id, "AAAAAA");
		}
		
		ptasks = ptasks->next;
	}
	g_list_free(head);
	return item_cnt;
}
//净化之路boss id
#define  IS_CLEAN_BOSS(grpid)  ((grpid)>= BEGIN_CLEAN_BOSS && (grpid <= END_CLEAN_BOSS))
/* ----------------------------------------------------------------------------*/
/**
 * @brief  同步战斗后获得的物品，随机选出3个
 */
/* ----------------------------------------------------------------------------*/
int sync_user_items(sprite_t* p, battle_t* pb, uint8_t* buff, beast_group_t* pbg)
{
	uint32_t beast_item_cnt = 0, task_item_cnt = 0, out_cnt = 0;
	item_t beast_item[MAX_TEMP_ITEM], task_item[MAX_TEMP_TASK_ITEM], out_item[MAX_BATTLE_ITEM];

	if (p->pgrp) {
		uint32_t percents[LOOP_GRP_CNT][3] = {{600, 400, 0}, {400, 400, 200}, {0, 400, 600}};
		beast_item[0].itemid = ITEM_CHIP;
		beast_item[0].count = 1 + rand_type_idx(3, percents[p->grpid], RAND_COMMON_RAND_BASE);
		beast_item_cnt ++;
	}
	if (pbg) {
		if (pbg->certain_drop_item && pbg->certain_drop_cnt) {
			beast_item[beast_item_cnt].itemid = pbg->certain_drop_item;
			beast_item[beast_item_cnt].count = pbg->certain_drop_cnt;
			beast_item_cnt ++;
		}
		
		if (pbg->drop_item_cnt) {
			//TODO
			std::list<uint32_t > idx_list;
			rand_type_list(pbg->drop_item_cnt, pbg->rates, RAND_COMMON_RAND_BASE,idx_list  );
			std::list<uint32_t >::iterator it;
			for (it=idx_list.begin();it!=idx_list.end();++it) {
				uint32_t idx=*it;
				if (pbg->drop_items[idx]) {
					beast_item[beast_item_cnt].itemid = pbg->drop_items[idx];
					beast_item[beast_item_cnt].count = pbg->counts[idx];
					if (pbg->grpid >= 2908 && pbg->grpid <= 2910) {
						msg_log_beast_item_get(idx, pbg->counts[idx]);
					}

					beast_item_cnt ++;
					if( beast_item_cnt >=MAX_TEMP_ITEM ) {
						break;	
					}
				}
			}
		}
	}
	base_attr_t *pba=NULL;
	bool valid=false;
	if( pbg && pbg->type==5 ){
		for( uint32_t loop=0 ; loop<pbg->idcnt ; loop++ ){
			if(pbg->idlist[loop].itemid && (rand()%RAND_COMMON_RAND_BASE) < pbg->idlist[loop].rate){
				pba=&pbg->idlist[loop];
				break;
			}
		}
	}	
	for (uint32_t loop = 0; loop < pb->beast_cnt; loop ++) {
		if(pba && pba->id==(pb->beasts[loop].beast_id%SPEC_BEAST_BASE)){
			valid=true;
		}
		uint32_t beasttype = pb->beasts[loop].beast_id % SPEC_BEAST_BASE;
		uint32_t beastlv = pb->beasts[loop].beast_lv;
		task_item_cnt = sync_task_items(p, pb, pb->beasts[loop].beast_id, task_item, task_item_cnt);
		beast_lvinfo_t* pbl = get_beast_lvinfo(beasttype, beastlv);
		if (pb->beasts[loop].beast_id / SPEC_BEAST_BASE || !pbl) 
			continue;
		beast_item_cnt = sync_normal_items(p, pbl, beast_item, beast_item_cnt);
	}
	out_cnt = rand_out_item (task_item, task_item_cnt, out_item, out_cnt);
	out_cnt = rand_out_item (beast_item, beast_item_cnt, out_item, out_cnt);
	if(valid) {
		KDEBUG_LOG(p->id,"add item[id=%u cnt=%u] from boss with rate[%u]",pba->itemid,pba->itemcnt,pba->rate);
		if(out_cnt != MAX_BATTLE_ITEM){
			out_item[out_cnt].itemid=pba->itemid;
			out_item[out_cnt].count=pba->itemcnt;
			out_cnt++;
		}else{
			out_item[out_cnt-1].itemid=pba->itemid;
			out_item[out_cnt-1].count=pba->itemcnt;
		}	
	}
	/**********临时用于净化之路怪物掉时装**************/
	uint32_t rd=rand()%10000;
	if(pbg && IS_CLEAN_BOSS(pbg->grpid)) {
		if((pbg->grpid>=2510 && rd<=500) 
				|| (pbg->grpid<2510 && pbg->grpid>=2506 && rd<=300)
				|| (pbg->grpid<2506 && rd<=200)){
			uint32_t clothid=rand()%4 + (rd%2?80074:80070);
			KDEBUG_LOG(p->id,"add cloth[id=%u %u] ",clothid,rd);
			if(out_cnt != MAX_BATTLE_ITEM){
				out_item[out_cnt].itemid=clothid;
				out_item[out_cnt].count=0;
				out_cnt++;
			}else{
				out_item[out_cnt-1].itemid=clothid;
				out_item[out_cnt-1].count=0;
			}
		}
	}
	/************************/
	if(rd<=30){
		//战斗中掉落小吆喝喇叭
		uint32_t itemid=350037;
		if(out_cnt != MAX_BATTLE_ITEM){
			out_item[out_cnt].itemid=itemid;
			out_item[out_cnt].count=1;
			out_cnt++;
		}else{
			out_item[out_cnt-1].itemid=itemid;
			out_item[out_cnt-1].count=1;
		}
	}
	*(buff + 1) = 0;
	int i = 2;
	uint32_t clothcnt = 0;
	for (uint32_t loop = 0; loop < out_cnt; loop ++) {
		normal_item_t *pni=get_item(out_item[loop].itemid);
		if (pni){
			if(pni->day_max > 0){
				//KDEBUG_LOG(p->id, "GGGG item limit1\t[%u  %u]",pni->day_max, out_item[loop].count);
				out_item[loop].count=cache_item_limit_add(p, out_item[loop].itemid, out_item[loop].count);
				//KDEBUG_LOG(p->id, "GGGG item limit2\t[%u  %u]",pni->day_max, out_item[loop].count);
			}
			out_item[loop].count = cache_add_kind_item(p, out_item[loop].itemid, out_item[loop].count);
			if (out_item[loop].count) {
				PKG_UINT32 (buff, out_item[loop].itemid, i);
				PKG_UINT32 (buff, out_item[loop].count, i);
				KDEBUG_LOG(p->id, "BATTLE GET ITEM\t[%u %u]", out_item[loop].itemid, out_item[loop].count);
				(*(buff + 1)) ++;
				db_add_item(NULL, p->id, out_item[loop].itemid, out_item[loop].count);
				if(pni->day_max > 0){
					//KDEBUG_LOG(p->id, "GGGG item limit update\t[%u  %u]",pni->day_max, out_item[loop].count);
					item_limit_update(p, out_item[loop].itemid, out_item[loop].count);
					//KDEBUG_LOG(p->id, "GGGG item limit update\t[%u  %u]",pni->day_max, out_item[loop].count);
				}
			}
		} else if (!clothcnt && p->waitcmd == 0 && get_bag_cloth_cnt(p) < get_max_grid(p)) {
			cloth_t* psc = get_cloth(out_item[loop].itemid);
			//DEBUG_LOG("jjjjjjj %u %u ",psc!=NULL,psc->clothlvs[out_item[loop].count].valid);
			if (psc){
				cloth_lvinfo_t	*pcl=NULL;
				if(psc->pro_attr[out_item[loop].count].valid){
					pcl=&psc->pro_attr[out_item[loop].count];
				}else if( psc->clothlvs[out_item[loop].count].valid){
					pcl=&psc->clothlvs[out_item[loop].count];
				}
				if(pcl){
					PKG_UINT32 (buff, out_item[loop].itemid, i);
					PKG_UINT32 (buff, 1, i);
					KDEBUG_LOG(p->id, "BATTLE GET CLOTH\t[%u %u]", out_item[loop].itemid, out_item[loop].count);
					(*(buff + 1)) ++;
					clothcnt ++;
					p->waitcmd = proto_cli_just_for_online;
					db_add_cloth(p, p->id, 0, psc, 0, pcl);
				}
			}
		}
	}
	
	*buff = p->collection_grid >= get_max_grid(p) || p->chemical_grid >= get_max_grid(p);	
	return i;
}

int sync_user_beast_handbook(sprite_t* p, beast_info_t* apbi,beast_group_t* pbg)
{
	uint32_t beast_id[BATTLE_POS_CNT] = { 0 };
	uint32_t count[BATTLE_POS_CNT] = { 0 };
	uint8_t out[128];
	uint32_t* p_count = (uint32_t *)out;
	*p_count = 0;
	int j = 4;

	for (int loop = 0; loop < BATTLE_POS_CNT; loop ++) {
		beast_t* p_ba = get_beast(apbi[loop].beast_id % MAX_NPC_TYPE);
		if (!IS_BEAST_ID(apbi[loop].beast_id) || !p_ba) 
			continue;

		msg_log_beast_type_kill(apbi[loop].beast_id % MAX_NPC_TYPE);
		msg_log_beast_lv_kill(apbi[loop].beast_lv);

		for (int loopi = 0; loopi < BATTLE_POS_CNT; loopi ++) {
			if (!beast_id[loopi] || beast_id[loopi] == p_ba->id) {
				beast_id[loopi] = p_ba->id;
				count[loopi] ++;
				break;
			} 
		}
	}

	for (int loop = 0; loop < BATTLE_POS_CNT; loop ++) {
		if (!beast_id[loop])
			break;
		
		beast_kill_t* pbk = cache_get_beast_kill_info(p, beast_id[loop]);
		if (pbk->count)
			continue;
		
		pbk->count += count[loop];
		
		PKG_H_UINT32(out, beast_id[loop], j);
		PKG_H_UINT32(out, count[loop], j);
		(*p_count) ++;
	}

	if(pbg) {
		uint32_t grpid = pbg->grpid + 100000;
		if(grpid == 102504) grpid = 102503;
		beast_kill_t* pbk = cache_get_beast_kill_info(p, grpid);
		pbk->count ++;
		PKG_H_UINT32(out, grpid, j);
		PKG_H_UINT32(out, 1, j);
		(*p_count) ++;
	}

	if (*p_count)
		return send_request_to_db(0, p->id, proto_db_set_beast_handbook, out, j);
	return 0;
}

int sync_user_pet_raise_skill(sprite_t* p)
{
	skill_t* ps = cache_get_skill(p, skill_life_pet_raise);
	skill_info_t* psi = get_skill_info(skill_life_pet_raise);
	if (ps && psi) {
		//KDEBUG_LOG(p->id,"xxx %u %u ",ps->skill_exp , psi->skill_prof[p->prof].add_exp);
		cache_update_skill_level(p, ps, psi, ps->skill_exp + psi->skill_prof[p->prof].add_exp);
		db_update_skill_info(0, p->id, ps);
	}
	return 0;
}

int gen_task_pktarget(sprite_t* p, battle_t* pb, task_db_t* ptr, int result, int battletype)
{
	task_node_t* ptn = get_task_node(ptr->taskid, ptr->nodeid + 1);
	if (!ptn) return 0;
	/*
	KDEBUG_LOG(p->id, "GEN TASK PK TARGET\t[%u %u %u %u %u %u]", \
		ptr->taskid, ptr->nodeid, result, battletype, pb->pkteam, ptn->btarget.beastid);
	*/
	int taskgrp = 0;
	if (battletype == be_pk && ptn->ptarget.count && !(ptn->ptarget.win && result == fail) \
		&& ptr->svr_buf[svr_buf_pk] < ptn->ptarget.count) {
		ptr->svr_buf[svr_buf_pk] += 1;
		db_set_task_svr_buf(NULL, p->id, ptr);
	}
	
	if ((ptn->btarget.type == beast_target_group && ptn->btarget.beastid == pb->pkteam) \
		&& !(ptn->btarget.win && (result == fail)) && battletype == be_fight_monster \
		&& ptr->svr_buf[svr_buf_beast] < ptn->btarget.count) {
		ptr->svr_buf[svr_buf_beast] += 1;
		if (ptn->btarget.help)
			taskgrp += 1;
		KDEBUG_LOG(p->id, "ADD KILL BEAST GRP\t[%u %u %u %u]", ptr->taskid, ptr->nodeid, ptr->svr_buf[svr_buf_beast], ptn->ptarget.win);
		db_set_task_svr_buf(NULL, p->id, ptr);
	}
	return taskgrp;
}

int sync_task_pktarget(sprite_t* p, battle_t* pb, int result, int battletype)
{
	int intimacy = 0;
	GList* ptasks = g_hash_table_get_values(p->recvtasks);
	GList* head = ptasks;
	while (ptasks) {
		task_db_t* pt = (task_db_t *)ptasks->data;
		intimacy += gen_task_pktarget(p, pb, pt, result, battletype);
		ptasks = ptasks->next;
	}
	g_list_free(head);
	return intimacy;
}

void sync_one_user_pvp_end(sprite_t* p, battle_t* abi, int result, uint32_t expfactor)
{
	KDEBUG_LOG(p->id, "PK INFO\t[%lu %u %u %u team=%u ip=%u]", \
		abi->battle_id, high32_val(abi->battle_id), low32_val(abi->battle_id), result, low32_val(abi->team_id), get_cli_ip(p->fdsess));
	if (abi->pkapply && result == victory && expfactor) {
		sync_user_pkinfo(p, abi, abi->enemy_lv);
	} else {
		noti_cli_update_pkpt(p, MAX_PKPT_DAY(p), 0, abi->pkapply);
	}
	
	if (abi->count == 1) {
		msg_log_single_pk();
	} else {
		msg_log_team_pk();
	}
	
	return;
}

void db_set_handbook_beastgrp(sprite_t* p, beast_group_t* pbg)
{
	int bytes = 0;
	uint8_t buf[128]={0};
	uint32_t grpid = pbg->grpid;

	if(grpid == 2504) grpid = 2503;
	beast_t* p_ba = get_beast(pbg->beasttype);
	if(p_ba && grpid >= BEGIN_CLEAN_BOSS &&  grpid <= END_CLEAN_BOSS) {
		uint32_t beastid = grpid + 100000;
		beast_kill_t* pbk = cache_get_beast_kill_info(p, beastid);
		if(pbk->count == 0) {
			KDEBUG_LOG(p->id,"get beast type=%u grpid=%d",pbg->beasttype,pbg->grpid);
			bytes = sizeof(protocol_t);
			PKG_UINT32(buf,1,bytes);
			PKG_UINT32(buf,pbg->beasttype,bytes);
			PKG_UINT32(buf,1,bytes);
			init_proto_head(buf,proto_cli_notify_get_boss_egg,bytes);
			send_to_self(p,buf,bytes,0);

			user_add_pet(p, pbg->beasttype, 1, 0, 0, rand_diff_default);
			msg_log_pet_activate_get(pbg->beasttype);
			msg_log_bus_mole2_killbeast(p->id,p->nick,pbg->bossid,pbg->beasttype);
		}
		db_set_monster_handbook(NULL, p->id, grpid + 100000, 2, 0, 0);
	}
}

void sync_one_user_pve_honor(sprite_t* p, beast_group_t* pbg)
{
	if (pbg->gethonor && !check_honor_exist(p, pbg->gethonor)) {
		honor_attr_t* pha = get_honor_attr(pbg->gethonor);
		if (pha) {
			uint32_t pre_honor = get_pre_honor(p, pha);
			update_one_honor(p, pha);
			set_user_honor(p, pha->honor_id);
			notify_user_honor_up(p, 1, 0, 1);
			db_set_user_honor(0, p->id, p->honor);

			uint32_t joblv = 0;
			if (p->prof_lv < pha->proflv) {
				p->prof_lv = pha->proflv;
				joblv = p->prof_lv;
			}
			db_add_user_honor(NULL, p->id, joblv, pre_honor, pha->honor_id);
		}
	}
}

static inline void noti_all_pkboss_win(sprite_t* p, sysinfo_t* psys)
{
	uint8_t out[256];
	int i = sizeof(protocol_t);
	if (p->btr_team) {
		int j = i, cnt = 0;
		i += 4;
		for (int loop = 0; loop < p->btr_team->count; loop ++) {
			sprite_t* s = p->btr_team->players[loop];
			if (s) {
				PKG_UINT32(out, s->id, i);
				PKG_STR(out, s->nick, i, USER_NICK_LEN);
				cnt ++;
			}
		}
		PKG_UINT32(out, cnt, j);
	} else {
		PKG_UINT32(out, 1, i);
		PKG_UINT32(out, p->id, i);
		PKG_STR(out, p->nick, i, USER_NICK_LEN);
	}

	PKG_UINT32(out, sys_info_kill_boss, i);
	PKG_UINT32(out, psys->val1, i);
	PKG_UINT32(out, 0, i);
	PKG_UINT32(out, 0, i);

	init_proto_head(out, proto_cli_sys_info, i);
	send_to_all(out, i);
}

static inline void noti_all_get_pet(sprite_t* p, sysinfo_t* psys)
{
	uint8_t out[256];
	int i = sizeof(protocol_t);

	PKG_UINT32(out, 1, i);
	PKG_UINT32(out, p->id, i);
	PKG_STR(out, p->nick, i, USER_NICK_LEN);

	PKG_UINT32(out, sys_info_get_pet, i);
	PKG_UINT32(out, psys->val1, i);
	PKG_UINT32(out, 0, i);
	PKG_UINT32(out, 0, i);

	KDEBUG_LOG(p->id, "SEND ALL GET PET");

	init_proto_head(out, proto_cli_sys_info, i);
	send_to_all(out, i);
}

void update_copy_map(sprite_t* p, beast_group_t* pbg)
{	
	map_copy_instance_t* pmci = get_map_copy_with_mapid(p->tiles->id);
	if (pmci) {
		int layeridx = get_copy_layer_idx(pmci, p->tiles->id);
		if (layeridx == -1)
			return;
		cli_get_copy_info_out cli_out;
		map_copy_config_t* pmcc = pmci->p_copy_config;
		cli_out.copyid = pmcc->copyid;
		copy_layer_t* layer = &pmci->p_copy_config->layers[layeridx];
		stru_copy_layer tmplayer;
		tmplayer.layerid = layer->layerid;
		tmplayer.mapid = layer->templatemap;
		tmplayer.needitem = layer->needitem;
		tmplayer.needcnt = layer->needitemcnt;
		tmplayer.parentlayer = layer->parentlayer;
		tmplayer.itemdel = pmci->itemdel[layeridx];
		for (uint32_t loop = 0; loop < layer->beast_orders; loop ++) {
			copy_beast_t* pbeast = &layer->beasts[loop];
			stru_copy_beast tmpbeast;
			tmpbeast.order = pbeast->order;
			for (uint32_t loopi = 0; loopi < pbeast->beast_type; loopi ++) {
				if (pbg->grpid == pbeast->grpids[loopi]) {
					pmci->killedcnts[layeridx][loop][loopi] ++;
				}
				stru_copy_beastgrp tmpgrp;
				tmpgrp.grpid = pbeast->grpids[loopi];
				tmpgrp.validcnt = pbeast->validcnts[loopi];
				tmpgrp.needkillcnt = pbeast->needkillcnts[loopi];
				tmpgrp.killedcnt = pmci->killedcnts[layeridx][loop][loopi];
				tmpbeast.grps.push_back(tmpgrp);
			}
			tmplayer.beast.push_back(tmpbeast);
		}
		cli_out.layers.push_back(tmplayer);
		send_msg_to_map(p, cli_get_copy_info_cmd, &cli_out, 0, 1);
	}
}

void sync_one_user_pve_end(sprite_t* p, int result, battle_t* abi, beast_info_t* apbi, atk_attr_t* puaa, uint32_t expfactor, int taskgrp)
{
	int i = sizeof(protocol_t);

	p->challenge_3061=0;
	if (puaa->sorp) {
		pet_t* pp = get_pet_inbag(p, puaa->sorp);
		if (!pp) return;
		if (puaa->expri && pp->action_state == rdy_fight)
			sync_user_pet_raise_skill(p);
		i += sync_pet_base_attr(p, puaa, pp, msg + i, expfactor);
		PKG_UINT8(msg, 0, i);
		PKG_UINT8(msg, 0, i);
		PKG_UINT32(msg, 0, i);	
	} else {
		beast_group_t* pbg = abi->pkteam ? get_beast_grp(abi->pkteam) : NULL;
		i += sync_user_base_attr(p, puaa, msg + i, pbg, expfactor, abi, taskgrp);
		i += sync_user_skills (p, puaa->skill_expri, msg + i, expfactor);
		if(pbg)
			msg_log_beast_grp_kill(pbg->grpid);
		if (result == victory) {			
			if (pbg) {
				p->last_beastgrp = pbg->grpid;
				db_set_handbook_beastgrp(p, pbg);
				sync_one_user_pve_honor(p, pbg);
				db_set_winbossid(NULL, p->id, pbg->grpid);
				if (!p->btr_team || TEAM_LEADER(p))
					msg_log_kuro_win(p->id, pbg->grpid, 1);
				else
					msg_log_kuro_win(p->id, pbg->grpid, 0);

				if ((!p->btr_team || TEAM_LEADER(p))) {
					sysinfo_t* psys = get_sys_info(sys_info_kill_boss, pbg->grpid);
					if (psys) {
						KDEBUG_LOG(p->id, "SYS INFO\t[%u %u]", pbg->grpid, psys->type);
						noti_all_pkboss_win(p, psys);
					}
					if (IS_COPY_MAP(p->tiles->id)) {
						update_copy_map(p, pbg);
					}
				}

				if (pbg->grpid == 2907) {
					msg_log_challenge_uwin(p->id);
					if (!p->btr_team) {
						msg_log_challenge_swin();
						msg_log_challenge_win();
					} else if (p->id == p->btr_team->teamid) {
						msg_log_challenge_win();
					}
				}

				if (pbg->grpid == 2605)
					msg_log_copy_fin(p->id, p->level);
			}

			if (p->pgrp) {
				p->pgrp->count ++;
				db_set_loopgrp(p, p->grpid);
			}
			
			if (expfactor)
				i += sync_user_items(p, abi, msg + i, pbg);
			else {
				PKG_UINT8(msg, 0, i);
				PKG_UINT32(msg, 0, i);
			}
			sync_user_beast_handbook(p, apbi,pbg);
		} else {
			PKG_UINT8(msg, 0, i);
			PKG_UINT32(msg, 0, i);
		}
	}

	init_proto_head(msg, proto_cli_sync_battle_end, i);
	send_to_self(p, msg, i, 0);
	p->update_flag = set_bit_off32(p->update_flag, update_flag_hp_mp_bit);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 战斗后同步属性 物品等 
 */
/* ----------------------------------------------------------------------------*/
int sync_user_attr_battle_end(int result,uint32_t other_userid, atk_attr_t* auaa, int acnt, beast_info_t* apbi, battle_t* abi)
{
	//DEBUG_LOG("sync_user_attr_battle_end");
	for (int loop = 0; loop < abi->count; loop++) {//对所有组员
		sprite_t* p= abi->players[loop];
		if (!p || IS_BEAST_ID(p->id)) continue;
		
		p->team_state = wait_animation;
		/*
		if(p->gvg_challenge_tag && p->btr_team && TEAM_LEADER(p))
		{
			p->gvg_team_win_cnt += result==victory?1:0;
			check_gvg_again(p);
		}
		*/
		uint32_t expfactor = get_exp_factor(p);//获得经验的倍率
		DEBUG_LOG("exp factor=%u",expfactor);
		if ((!p->btr_team || TEAM_LEADER(p)) && p->boxid) {
			box_t* pb = get_box(p->boxid);
			p->boxid = 0;
			if (pb && result == victory && expfactor)
				noti_user_box_item(p, pb);
		}
		KDEBUG_LOG(p->id, "PK INFO\t[%lu %u %u %u team=%u ip=%u type=%d]", \
			abi->battle_id, high32_val(abi->battle_id), low32_val(abi->battle_id), \
			result, low32_val(abi->team_id), get_cli_ip(p->fdsess), abi->battletype);
		
		for (int j = 0; j < acnt; j++) {
			int taskgrp = 0;
			if (p->id == auaa[j].uid) {
				if (!auaa[j].sorp) {
					taskgrp = sync_task_pktarget(p, abi, result, abi->battletype);
					sync_user_cloth_duration(p, auaa[j].cloth_duration);
					cache_set_cloth_attr(p);
					memcpy(p->default_skills, auaa[j].default_skills, sizeof(p->default_skills));
					memcpy(p->default_levels, auaa[j].default_levels, sizeof(p->default_levels));
					update_fight_buff(p, auaa[j].autofight);
				} else {
					pet_t* pp = get_pet_inbag(p, auaa[j].uid);
					if (pp) {
						memcpy(pp->default_skills, auaa[j].default_skills, sizeof(pp->default_skills));
						memcpy(pp->default_levels, auaa[j].default_levels, sizeof(pp->default_levels));
					}
				}
				
				if (abi->battletype == be_pk && !auaa[j].sorp) {
					//if(p->gvg_challenge_tag)
					//{
						//item_t* pday = cache_get_day_limits(p, ssid_gvg_challenge_times);
						//if(result==fail)
						//{
							//pday->count+=pday->count<MAX_GVG_CHALLENGE?1:0;
							//db_day_add_ssid_cnt(0, p->id, ssid_gvg_challenge_times, 1, MAX_GVG_CHALLENGE);
						//}
						//else 
						//{
							//p->gvg_challenge_scores+=2;
							//db_day_add_ssid_cnt(0, p->id, ssid_gvg_challenge_scores, 2, -1);
						//}
						//KDEBUG_LOG(p->id,"gvg battle info  [result=%u times=%u scores=%u]",result,pday->count,p->gvg_challenge_scores);
						//uint8_t out[64];
						//int i = sizeof(protocol_t);
						//PKG_UINT32(out, MAX_GVG_CHALLENGE-pday->count, i);
						//PKG_UINT32(out, p->gvg_challenge_scores, i);
						//init_proto_head(out, proto_cli_sync_pkinfo, i);
						//send_to_self(p, out, i, 0);
					//}
					//else
					sync_one_user_pvp_end(p, abi, result, expfactor);//人与人pk
				} else if (abi->battletype == be_fight_monster) {//打怪时
					sync_one_user_pve_end(p, result, abi, apbi, auaa + j, expfactor, taskgrp);
				}
			}
		}

		p->pgrp = NULL;
		p->grpid = 0;
		//p->gvg_challenge_tag=0;
		if (abi->battletype == be_fight_monster) {
			if (expfactor)
				update_ratio_buff(p);
			db_set_vip_buff(p, buffid_hpmp);
		}
	}
	return 0;
}

int noti_battle_use_medical_callback(batrserv_proto_t* btrpkg, int len)
{
	CHECK_BODY_LEN(len, sizeof(batrserv_proto_t) + 8);
	battle_t* lpb = get_batter_info(btrpkg->id, btrpkg->btid);	
	if (lpb) {
		uint32_t uid = *(uint32_t*)btrpkg->body;
		uint32_t itemid = *(uint32_t*)(btrpkg->body + 4);
		sprite_t* lp = get_sprite(uid);
		if (lp) {
			cache_reduce_kind_item(lp, itemid, 1);
			db_add_item(0, lp->id, itemid, -1);
			db_set_item_handbook(0, lp->id, itemid, 1);
		}
	}
	return 0;
}

int noti_battle_use_beast_card_callback(batrserv_proto_t* btrpkg, int len)
{
	CHECK_BODY_LEN(len, sizeof(batrserv_proto_t) + 20);
	
	battle_t* lpb = get_batter_info(btrpkg->id, btrpkg->btid);	
	if (lpb) {
		uint32_t uid = *(uint32_t*)btrpkg->body;
		uint32_t beast_id = *(uint32_t *)(btrpkg->body + 4) % MAX_NPC_TYPE;
		uint32_t itemid = *(uint32_t *)(btrpkg->body + 8);
		uint32_t succ = *(uint32_t *)(btrpkg->body + 12);
		uint32_t level = *(uint32_t *)(btrpkg->body + 16);
		sprite_t* lp = get_sprite(uid);
		if (!lp || succ == 2) {
			ERROR_RETURN(("usr offline or catch by other[%u %u]", uid, succ), 0);
		}

		if (!cache_reduce_kind_item(lp, itemid, 1)) {
			db_add_item(0, lp->id, itemid, -1);
		}

		skill_t* ps = cache_get_skill(lp, skill_life_pet_contact);
		skill_info_t* psi = get_skill_info(skill_life_pet_contact);
		if (ps && psi) {
			uint32_t add_exp = IS_ADMIN(lp) ? 100 : psi->skill_prof[lp->prof].add_exp;
			cache_update_skill_level(lp, ps, psi, ps->skill_exp + add_exp);
			db_update_skill_info(0, lp->id, ps);
		}

		if (!succ) {
			KDEBUG_LOG(uid, "CATCH FAIL");
			return 0;
		}
		msg_log_pet_spec_catch(lp->id, beast_id);
		sysinfo_t* psys = get_sys_info(sys_info_get_pet, beast_id);
		if (psys)
			noti_all_get_pet(lp, psys);
			
		return user_add_pet(lp, beast_id, level, 0, 0, rand_diff_default);
	}
	return 0;
}

int noti_battle_swap_pet_callback(batrserv_proto_t* btrpkg, int len)
{
	if (btrpkg->ret) {
		ERROR_RETURN(("BATTLE SWAP PET CALLBACK ERR %u %d", btrpkg->id, btrpkg->ret), -1);
	}
	CHECK_BODY_LEN(len, sizeof(batrserv_proto_t) + 12);
	battle_t* lpb = get_batter_info(btrpkg->id, btrpkg->btid);	
	if (lpb) {
		uint32_t uid = *(uint32_t*)btrpkg->body;
		uint32_t petid_new = *(uint32_t *)(btrpkg->body + 4);
		uint32_t petid_old = *(uint32_t *)(btrpkg->body + 8);
		sprite_t* lp = get_sprite(uid);
		if (!lp || petid_new == petid_old) {
			ERROR_RETURN(("swap pet batsvr ret no sprite[%u %u %u]", uid, petid_old, petid_new), 0);
		}

		pet_t* p_p = get_pet_rdy_fight(lp);
		if(p_p) {
			p_p->action_state = on_standby;
			db_set_pet_state(0, lp->id, p_p->petid, on_standby);
		}

		if (petid_new) {
			p_p = get_pet_inbag(lp, petid_new);
			if (!p_p)
				ERROR_RETURN(("swap pet batsvr err[%u %u]", uid, petid_new), 0);
			p_p->action_state = rdy_fight;
			db_set_pet_state(0, lp->id, p_p->petid, rdy_fight);
		}
	}
	return 0;
}

int noti_user_change_pos_callback(batrserv_proto_t* btrpkg, int len)
{
	CHECK_BODY_LEN(len, sizeof(batrserv_proto_t) + 4);
	battle_t* lpb = get_batter_info(btrpkg->id, btrpkg->btid);	
	if (lpb) {
		uint32_t uid = *(uint32_t*)btrpkg->body;
		sprite_t* lp = get_sprite(uid);
		if (!lp) {
			ERROR_RETURN(("user not existed when battle change pos[%u]", uid), 0);
		}
		lp->in_front = lp->in_front ? 0 : 1;
		db_set_user_position(0, lp->id, lp->in_front);
	}
	return 0;
}

int noti_battle_attacks_info_callback(batrserv_proto_t* btrpkg, int len)
{
	battle_t* lpb = get_batter_info(btrpkg->id, btrpkg->btid);	
	if (lpb) {
		//DEBUG_LOG("1310 callback %u",btrpkg->id);
		send_to_battle(lpb, btrpkg->body, btrpkg->len - sizeof(batrserv_proto_t));
		//send to watchers
		send_to_battle_watchers(lpb, btrpkg->body, btrpkg->len - sizeof(batrserv_proto_t));
	}
	return 0;
}

void set_round_changed(battle_t* pb)
{
	for (int loop = 0; loop < pb->count; loop ++) {
		if (pb->players[loop]) {
			pb->players[loop]->round_attack_cnt = 0;
		}
	}
}

/**
 * @brief  一轮战斗结果返回，batlserv那边增加字段时，本函数也要相应更改
 */
int noti_battle_round_result_callback(batrserv_proto_t* btrpkg, int len)
{
	battle_t* lpb = get_batter_info(btrpkg->id, btrpkg->btid);	
	//DEBUG_LOG("ROUND RESULT11111ll%u",lpb);
	if (lpb) {
		set_round_changed(lpb);
		uint32_t i = sizeof(protocol_t);
		CHECK_BODY_LEN_GE(btrpkg->len, i+22);
		i += 13;	// btid teamid touxi
		uint32_t bttype, challeger_cnt;
		
		UNPKG_UINT8(btrpkg->body, bttype, i);
		UNPKG_UINT32(btrpkg->body, challeger_cnt, i);
		if (bttype == 0) {
			for (uint32_t loop = 1; loop <= challeger_cnt; loop ++) {
				CHECK_BODY_LEN_GE(btrpkg->len, i + 72);
				uint32_t uid, petid, hp, mp, injurylv, cloth_cnt, skill_cnt;	
				UNPKG_UINT32(btrpkg->body, uid, i);
				UNPKG_UINT32(btrpkg->body, petid, i);
				i += 36;	// typeid pos nick vip color level
				UNPKG_UINT32(btrpkg->body, hp, i);
				i += 4;		// hpmax
				UNPKG_UINT32(btrpkg->body, mp, i);
				i += 4;		// mpmax
				UNPKG_UINT32(btrpkg->body, injurylv, i);
				i += 4;     // change petid
				i += 4;		// auto round cnt
				uint32_t skillcnt;
				UNPKG_UINT32(btrpkg->body, skillcnt, i);
				i += 4 * skillcnt;	
				i += 9;		// pet state ... earth ....prof
				CHECK_BODY_LEN_GE(btrpkg->len, i + 1);
				UNPKG_UINT8(btrpkg->body, cloth_cnt, i);
				i += cloth_cnt * 10;
				CHECK_BODY_LEN_GE(btrpkg->len, i + 1);
				UNPKG_UINT8(btrpkg->body, skill_cnt, i);
				i += skill_cnt * 7;
				sprite_t* s = get_sprite(uid);
				if (!s || !s->btr_info || s->btr_info != lpb) continue;
				//每个回合完成，重置
				s->last_use_medical_id=0;
				s->update_flag = set_bit_on32(s->update_flag, update_flag_hp_mp_bit);
				if (!petid) {
					s->hp = hp ? hp : 1;
					s->mp = mp;
					//if (s->injury_lv != injurylv) {
						//s->injury_lv=INJURY_NONE;
						//update_user_second_attr(s, (base_5_attr_t *)&s->physique, injurylv);
					//}
				} else {
					pet_t* p_p = get_pet_inbag(s, petid);
					if (!p_p) continue;
					p_p->hp = hp ? hp : 1;
					p_p->mp = mp;
					if (p_p->injury_lv != injurylv) {
						p_p->injury_lv = INJURY_NONE ;
						calc_pet_second_level_attr(p_p);
					}
				}
			}
		}
		send_to_battle(lpb, btrpkg->body, btrpkg->len - sizeof(batrserv_proto_t));
		//send to watchers
		send_to_battle_watchers(lpb, btrpkg->body, btrpkg->len - sizeof(batrserv_proto_t));
		KDEBUG_LOG(btrpkg->id, "ROUND RESULT\t[%u %u]", high32_val(lpb->battle_id), low32_val(lpb->battle_id));
	}
	return 0;
}

void notify_btrteam_state(battle_t* pb)
{
	uint8_t buff[256];
	int i = sizeof(protocol_t), len = i + 8;
	uint32_t player_cnt = 0;
	PKG_UINT32(buff, pb->team_id, i);
	for (int loop = 0; loop < pb->count; loop ++) {
		sprite_t* lp = pb->players[loop];
		if (lp) {
			PKG_UINT32(buff, lp->id, len);
			PKG_UINT32(buff, lp->level, len);
			PKG_UINT32(buff, lp->hp, len);
			PKG_UINT32(buff, lp->hp_max, len);
			PKG_UINT32(buff, lp->mp, len);
			PKG_UINT32(buff, lp->mp_max, len);
			PKG_UINT32(buff, lp->injury_lv, len);
			player_cnt ++;
		}
	}
	if (!player_cnt)
		return;

	PKG_UINT32(buff, player_cnt, i);
	init_proto_head(buff, proto_cli_noti_team_state, len);
	send_to_battle(pb, buff, len);
}

void notify_team_state(batter_team_t* pbt)
{
	uint8_t buff[256];
	int i = sizeof(protocol_t), len = i + 8;
	uint32_t player_cnt = 0;
	PKG_UINT32(buff, pbt->teamid, i);
	for (int loop = 0; loop < pbt->count; loop ++) {
		sprite_t* lp = pbt->players[loop];
		if (lp) {
			PKG_UINT32(buff, lp->id, len);
			PKG_UINT32(buff, lp->level, len);
			PKG_UINT32(buff, lp->hp, len);
			PKG_UINT32(buff, lp->hp_max, len);
			PKG_UINT32(buff, lp->mp, len);
			PKG_UINT32(buff, lp->mp_max, len);
			PKG_UINT32(buff, lp->injury_lv, len);
			player_cnt ++;
		}
	}
	
	if (player_cnt) {
		PKG_UINT32(buff, player_cnt, i);
		init_proto_head(buff, proto_cli_noti_team_state, len);
		send_to_btrtem(pbt, buff, len);
	}
}

int pkg_one_task_svrbuf(task_db_t* ptr, uint8_t* buf)
{
	uint8_t cli_node = *(ptr->cli_buf);
	uint8_t canceled = cli_node < ptr->nodeid;
	task_node_t* ptn = canceled ? get_task_node(ptr->taskid, cli_node + 1) : get_task_node(ptr->taskid, ptr->nodeid + 1);
	int i = 0;
	PKG_UINT32(buf, ptr->taskid, i);
	PKG_UINT32(buf, ptr->nodeid, i);

	if (ptn && ptn->btarget.beastid) {
		PKG_UINT32(buf, ptn->btarget.beastid, i);
		uint16_t beastcnt = ptr->svr_buf[svr_buf_beast];
		beastcnt = (canceled || beastcnt > ptn->btarget.count) ? ptn->btarget.count : beastcnt;
		PKG_UINT16(buf, beastcnt, i);
		PKG_UINT16(buf, ptn->btarget.count, i);
	} else {
		PKG_UINT32(buf, 0, i);
		PKG_UINT16(buf, 0, i);
		PKG_UINT16(buf, 0, i);
	}

	if (ptn && ptn->ptarget.count) {
		uint16_t pkcnt = ptr->svr_buf[svr_buf_pk];
		pkcnt = (canceled || pkcnt > ptn->ptarget.count) ? ptn->ptarget.count : pkcnt;
		PKG_UINT16(buf, pkcnt, i);
		PKG_UINT16(buf, ptn->ptarget.count, i);
	} else {
		PKG_UINT16(buf, 0, i);
		PKG_UINT16(buf, 0, i);
	}

	PKG_UINT16(buf, ptr->svr_buf[svr_buf_skill], i);
	return i;
}

int pkg_task_svrbuf(sprite_t* p, uint8_t* buf)
{
	int i = 0;
	PKG_UINT32(buf, g_hash_table_size(p->recvtasks), i);
	GList* ptasks = g_hash_table_get_values(p->recvtasks);
	GList* head = ptasks;
	while (ptasks) {
		task_db_t* pt = (task_db_t *)ptasks->data;
		i += pkg_one_task_svrbuf(pt, buf + i);
		ptasks = ptasks->next;
	}
	g_list_free(head);
	return i;
}
void notify_cli_battle_end(battle_t* pb, int result)
{
	uint8_t buff[4096] = {0};
	int i = sizeof(protocol_t);
	PKG_UINT32(buff, pb->battletype, i);
	PKG_UINT32(buff, result, i);
	PKG_UINT32(buff, 0, i);
	PKG_UINT32(buff, 0, i);

	for (int loop = 0; loop < pb->count; loop ++) {
		int j = i;
		if (pb->players[loop] && pb->players[loop]->id) {
			j += pkg_task_svrbuf(pb->players[loop], buff + j);
			j += pkg_ratio_buff(pb->players[loop]->vip_buffs[0], buff + j);
			j += pkg_ratio_buff(pb->players[loop]->vip_buffs[1], buff + j);
			j += pkg_ratio_buff(pb->players[loop]->vip_buffs[2], buff + j);
			PKG_UINT32(buff, get_auto_fight_count(pb->players[loop]), j);
			PKG_UINT32(buff, pb->players[loop]->vip_buffs[buffid_hpmp][0], j);
			PKG_UINT32(buff, pb->players[loop]->vip_buffs[buffid_hpmp][1], j);
			init_proto_head(buff, proto_cli_battle_end, j);
			send_to_self(pb->players[loop], buff, j, 0);
			if (pb->battletype == be_fight_monster && !pb->pkteam)
				pb->players[loop]->start_walk_time = get_now_tv()->tv_sec;
		}
	}
	//send to watchers
	PKG_UINT32(buff, 0, i);
	i+=4*9;
	init_proto_head(buff, proto_cli_battle_end, i);
 	send_to_battle_watchers(pb,buff,i);
}

void do_pvp_pk_for_victory(uint32_t  userid ,uint32_t other_userid ){
 	sprite_t* p=get_sprite(userid);
	if (p && user_is_in_war_map(p)) {
		if (p->tiles){
			home_game_del_user_in home_in;
			home_in.other_userid=p->id;
			send_msg_to_homeserv(other_userid,p->tiles->id,home_game_del_user_cmd,&home_in);
		}
		p->pvp_scores+= 1;
		db_day_add_ssid_cnt(NULL, p->id, ssid_week_pvp_scores, 1, -1);

	}
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  战斗结束，收到batrserv的结束通知被调用
 *
 * @param  btrpkg
 * @param  len
 *
 * @return  
 */
/* ----------------------------------------------------------------------------*/
int noti_battle_end_callback(batrserv_proto_t* btrpkg, uint32_t len)
{
	CHECK_BODY_LEN_GE(len, sizeof(batrserv_proto_t) + 12);
	int result, battle_type, cnt, i = 0;
	uint32_t other_userid;
	UNPKG_H_UINT32(btrpkg->body, result, i);
	UNPKG_H_UINT32(btrpkg->body, other_userid , i);
	UNPKG_H_UINT32(btrpkg->body, battle_type, i);
	UNPKG_H_UINT32(btrpkg->body, cnt, i);
	KDEBUG_LOG(btrpkg->id, "BATTLE END CALLBACK\t[%u %u %u %u %u]", \
		high32_val(btrpkg->btid), low32_val(btrpkg->btid), result, battle_type, cnt);
	CHECK_BODY_LEN(len, sizeof(batrserv_proto_t) + i + cnt * sizeof(atk_attr_t) \
		+ sizeof(beast_info_t) * BATTLE_POS_CNT);
	if (result==victory){
		do_pvp_pk_for_victory(btrpkg->id  ,other_userid );
	}

	battle_t* lpb = get_batter_info(btrpkg->id, btrpkg->btid);	
	if (lpb) {
		//KDEBUG_LOG(btrpkg->id ,"====do===");
		sync_user_attr_battle_end(result, other_userid, (atk_attr_t*)(btrpkg->body + i), \
			cnt, (beast_info_t*)(btrpkg->body + i + cnt * sizeof(atk_attr_t)), lpb);//战斗后属性 物品 伤害等的计算
		notify_cli_battle_end(lpb, result);
		notify_btrteam_state(lpb);
		free_batter_info_by_leader(lpb);
		KDEBUG_LOG(low32_val(lpb->team_id), "============================BT END==================\t[%u]", lpb->battletype);
	} 

	return 0;
}

int handle_batrserv_pkg(batrserv_proto_t* btrpkg, int len)
{
	KDEBUG_LOG(btrpkg->id, "BTLSVR CMD\t[(%u %u) %u %u]", \
		high32_val(btrpkg->btid), low32_val(btrpkg->btid), btrpkg->opid, len);
	
	switch (btrpkg->opid) {
		case proto_cli_challenge:
			batrserv_challenge_battle_callback(btrpkg, len);
			break;
		case proto_cli_challenge_npc:
			batrserv_challenge_npc_battle_callback(btrpkg, len);
			break;
		case proto_cli_accept_challenge:
			batrserv_accept_battle_callback(btrpkg, len);
			break;
		case proto_cli_use_medical:
			noti_battle_use_medical_callback(btrpkg, len);
			break;
		case proto_cli_noti_user_out://1315 
			batrserv_noti_user_out_callback(btrpkg, len);
			break;
		case proto_cli_noti_battle_created://1305
			noti_battle_created_callback(btrpkg, len);
			break;
		case proto_cli_noti_battle_started://1307
			noti_battle_started_callback(btrpkg, len);
			break;
		case proto_cli_battle_attacks_info://1310
			noti_battle_attacks_info_callback(btrpkg, len);
			break;
		case proto_cli_battle_round_result://1311
			noti_battle_round_result_callback(btrpkg, len);
			break;
		case proto_cli_sync_battle_end:
			noti_battle_end_callback(btrpkg, len);
			break;
		case proto_cli_noti_user_fighted://1313
			noti_user_fighted_callback(btrpkg, len);
			break;
		case proto_cli_noti_load_rate:
			noti_user_load_rate_callback(btrpkg, len);
			break;
		case proto_cli_reject_challenge:
			noti_reject_battle_callback(btrpkg, len);
			break;
		case proto_cli_cancel_challenge:
			noti_cancel_battle_callback(btrpkg, len);
			break;
		case proto_cli_noti_user_change_pos:
			noti_user_change_pos_callback(btrpkg, len);
			break;
		case proto_cli_use_beast_card:
			noti_battle_use_beast_card_callback(btrpkg, len);
			break;
		case proto_cli_battle_swap_pet:
			noti_battle_swap_pet_callback(btrpkg, len);
			break;
		default:
			break;
	}
	return 0;
}

void noti_batsvr_user_offline(sprite_t* p)
{
	battle_t* lpbi = p->btr_info;
	if (lpbi && lpbi->state == bt_state_init) {
		notify_batsvr_leave_team(p);
	} else if (lpbi) {
		KDEBUG_LOG(p->id, "SEND 1340 TO BTSVR\t[%u %u]", high32_val(lpbi->battle_id), low32_val(lpbi->battle_id));
		send_to_batrserv(p, bt_battle_info_msg, 0, NULL, p->id, lpbi->battle_id, proto_cli_user_offline);
	}
}

void del_self_battle_info(sprite_t* p)
{
	battle_t* lpbi = p->btr_info;
	if (lpbi) {
		if (lpbi->battletype == be_pk) {
			KDEBUG_LOG(p->id, "PK INFO\t[%lu %u %u %u team=%u ip=%u]", \
				lpbi->battle_id, high32_val(lpbi->battle_id), low32_val(lpbi->battle_id), fail, low32_val(lpbi->team_id), get_cli_ip(p->fdsess));
		}
		for (int loop = 0; loop < lpbi->count; loop++) {
			if (lpbi->players[loop] == p) {
				lpbi->players[loop] = NULL;
				p->btr_info = NULL;	
				if (!p->team_state && p->busy_state == be_fighting)
					cancel_battle_state(p);
			}

			if (BATTLE_USER_ID(lpbi->team_id) == p->id && lpbi->count == 1) {
				KDEBUG_LOG(p->id, "OUT SINGLE FREE BATTLE INFO\t[%u %u]", \
					high32_val(lpbi->battle_id), low32_val(lpbi->battle_id));
				free_batter_info_by_leader(lpbi);
			}
		}
	}
}



void del_user_battle_team(sprite_t* p)
{
	KDEBUG_LOG(p->id, "DEL USER BATTLE TEAM\t[%p]", p->btr_info);
	del_self_battle_info(p);

	if (p->btr_team) {
		del_mbr_from_bt(p->btr_team, p->id);
	}
}

void del_user_battle_info(sprite_t* p)
{
	battle_t* lpbi = p->btr_info;
	if (p->btr_team && p->btr_team->teamid == p->id) {
		for (int loop = 0; loop < lpbi->count; loop ++) {
			sprite_t* s = lpbi->players[loop];
			if (s && s != p) {
				lpbi->players[loop] = NULL;
				s->btr_info = NULL;	
				if (!s->team_state && s->busy_state == be_fighting)
					cancel_battle_state(s);
			}
		}
	} else {
		for (int loop = 0; loop < lpbi->count; loop++) {
			if (lpbi->players[loop] == p) {
				lpbi->players[loop] = NULL;
				p->btr_info = NULL;	
				if (!p->team_state && p->busy_state == be_fighting)
					cancel_battle_state(p);
			}

			if (BATTLE_USER_ID(lpbi->team_id) == p->id && lpbi->count == 1) {
				KDEBUG_LOG(p->id, "OUT SINGLE FREE BATTLE INFO\t[%u %u]", \
					high32_val(lpbi->battle_id), low32_val(lpbi->battle_id));
				free_batter_info_by_leader(lpbi);
			}
		}
	}
}

int kick_out_team_animation(void* owner, void* data)
{
	batter_team_t* pt = (batter_team_t*)owner;
	if (pt) {
		KDEBUG_LOG(pt->teamid, "KICK USERS ANIMATION OUT TEAM");
		int loop, tcnt = pt->count;
		for (loop = 0; loop < tcnt && tcnt > 1; loop ++) {
			sprite_t* s = pt->players[loop];
			if (s && s->team_state == wait_animation && !del_mbr_from_bt(pt, s->id)) {
				tcnt --;
				loop --;
			}
		}
	}
	return 0;
}

uint32_t last_kill_beastgrp(sprite_t* p)
{
	if (p->last_beastgrp && p->tiles) {
		beast_group_t* pbg = get_beast_grp(p->last_beastgrp);
		if (pbg && pbg->mapid == p->tiles->id)
			return p->last_beastgrp;
	}
	return 0;
}

uint32_t team_last_kill_beastgrp(sprite_t* p)
{
	if (!p->btr_team)
		return last_kill_beastgrp(p);

	for (int loop = 0; loop < p->btr_team->count; loop ++) {
		sprite_t* s = p->btr_team->players[loop];
		if (s && !last_kill_beastgrp(s))
			return 0;
	}
	return last_kill_beastgrp(p);
}

int get_last_kill_beastgrp_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int i = sizeof(protocol_t);
	if (!p->btr_team) {
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, p->id, i);
		PKG_UINT32(msg, last_kill_beastgrp(p), i);
	} else {
		PKG_UINT32(msg, p->btr_team->count, i);
		for (int loop = 0; loop < p->btr_team->count; loop ++) {
			sprite_t* s = p->btr_team->players[loop];
			if (s) {
				PKG_UINT32(msg, s->id, i);
				PKG_UINT32(msg, last_kill_beastgrp(s), i);
			}
		}
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_team(p, msg, i, 1);
}

int register_for_team_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	CHECK_SELF_TEAM_LEADER(p, p->id, 1);
	((char *)body)[TEAM_NICK_LEN - 1] = '\0';
	CHECK_DIRTYWORD(p, body);
	memcpy(p->team_nick, body, TEAM_NICK_LEN);
	p->team_registerd = 1;
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_STR(msg, p->team_nick, i, TEAM_NICK_LEN);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1,1);
	return 0;
}

void add_user_for_team(gpointer key, gpointer value, gpointer user_data)
{
	cli_get_valid_team_out *out=(cli_get_valid_team_out*)user_data;
	sprite_t *p=(sprite_t*)value;
	if(!p->btr_team && p->team_registerd==1 && p->tiles && p->tiles->team==0 && 
				!IS_COPY_MAP(p->tiles->id) && !p->btr_info && p->busy_state == be_idle && !p->is_in_war_map()){
		//KDEBUG_LOG(p->id,"ddddddd %u",p->tiles->team);
		stru_team_info tmp;
		tmp.teamid=p->id;
		tmp.level=p->level;
		tmp.count=1;
		memcpy(tmp.team_nick,p->team_nick,sizeof(tmp.team_nick));
		out->team_list.push_back(tmp);	
	}
}

void add_team_for_team(gpointer key, gpointer value, gpointer user_data)
{
	cli_get_valid_team_out *out=(cli_get_valid_team_out*)user_data;
	batter_team_t *pbt=(batter_team_t *)value;
	sprite_t *p=get_sprite(pbt->teamid);
	if( p->btr_info || p->busy_state != be_idle || !p->tiles || p->tiles->team == 1 || IS_COPY_MAP(p->tiles->id)
		   	|| team_switch_state(p) == team_refuse || pbt->count == MAX_PLAYERS_PER_TEAM || p->is_in_war_map()){
		return ;	
	}
	stru_team_info tmp;
	tmp.teamid=pbt->teamid;
	tmp.level=p->level;
	tmp.count=pbt->count;
	memcpy(tmp.team_nick,p->team_nick,sizeof(tmp.team_nick));
	out->team_list.push_back(tmp);	
}

int cli_get_valid_team(sprite_t* p, Cmessage* c_in)
{
	cli_get_valid_team_out cli_out; 
	g_hash_table_foreach(batter_teams,(GHFunc)add_team_for_team,&cli_out);
	g_hash_table_foreach(all_sprites,(GHFunc)add_user_for_team,&cli_out); 
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_get_team_details(sprite_t* p, Cmessage* c_in)
{
	cli_get_team_details_in *p_in=P_IN;
	cli_get_team_details_out cli_out; 
	sprite_t *lp=get_sprite(p_in->teamid);
	if( !lp ){
		return send_to_self_error(p,p->waitcmd,cli_err_user_offline,1);	
	}
	if(lp->btr_team ){
		for( uint8_t loop=0 ; loop<lp->btr_team->count ; loop++ ){
			sprite_t *s=lp->btr_team->players[loop];
			stru_team_details tmp;	
			tmp.uid=s->id;
			tmp.level=s->level;
			tmp.prof=s->prof;
			memcpy(tmp.nick,s->nick,sizeof(tmp.nick));
			cli_out.list.push_back(tmp);
		}
	}else{
		stru_team_details tmp;	
		tmp.uid=lp->id;
		tmp.level=lp->level;
		tmp.prof=lp->prof;
		memcpy(tmp.nick,lp->nick,sizeof(tmp.nick));
		cli_out.list.push_back(tmp);
	}	
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_get_grp_day_challenge(sprite_t* p, Cmessage* c_in)
{
	cli_get_grp_day_challenge_out cli_out;
	GList* pdays = g_hash_table_get_values(p->day_limits);
	GList* head = pdays;
	while (pdays) {
		item_t* pday = (item_t *)pdays->data;
		if (pday->itemid < MAX_BEAST_TYPE) {
			stru_id_cnt idcnt;
			idcnt.id = pday->itemid;
			idcnt.count = pday->count;
			cli_out.daycnts.push_back(idcnt);
		}
		pdays = pdays->next;
	}
	g_list_free(head);
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_get_loop_grp(sprite_t* p, Cmessage* c_in)
{
	cli_get_loop_grp_out cli_out;
	for (int loop = 0; loop < LOOP_GRP_CNT; loop ++) {
		grp_loop_t* pgl = &p->loop_grp[loop];
		if (!check_same_period(pgl->uptime, repeat_day, 5) || pgl->version != LOOP_GRP_VERSION) {
			pgl->uptime = get_now_tv()->tv_sec;
			pgl->day_draw_cnt = 0;
			pgl->version = LOOP_GRP_VERSION;
			pgl->beastcnt = LOOP_GRP_BEAST_CNT;
			for (int loopi = 0; loopi < LOOP_GRP_BEAST_CNT; loopi ++) {
				pgl->beasts[loopi].id = valid_beasts[loop][rand() % valid_beast_cnt[loop]];
				pgl->beasts[loopi].count = 0;
			}
			db_set_loopgrp(p, loop);
		}

		stru_loop_grp tmpgrp;
		tmpgrp.grpid = loop;
		tmpgrp.draw_total = pgl->draw_total;
		tmpgrp.isactivate = (loop == 0 ? 1 : p->loop_grp[loop - 1].day_draw_cnt > 0) && p->loop_grp[loop].day_draw_cnt < LOOP_GRP_MAX_DAY;
		for (uint32_t loopi = 0; loopi < pgl->beastcnt; loopi ++) {
			stru_id_cnt tmp;
			tmp.id = pgl->beasts[loopi].id;
			tmp.count = pgl->beasts[loopi].count;
			tmpgrp.beastlist.push_back(tmp);
		}
		cli_out.grplist.push_back(tmpgrp);
	}

	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_refresh_loopgrp(sprite_t* p, Cmessage* c_in)
{
	cli_refresh_loopgrp_in* p_in = P_IN;
	CHECK_INT_LE(p_in->grpid, LOOP_GRP_CNT - 1);
	grp_loop_t* pgl = &p->loop_grp[p_in->grpid];
	for (uint32_t loop = 0; loop < pgl->beastcnt; loop ++) {
		if (pgl->beasts[loop].count == 0) {
			KERROR_LOG(p->id, "cannot get\t[%u %u]", p_in->grpid, pgl->beasts[loop].id);
			return send_to_self_error(p, p->waitcmd, cli_err_not_win_beastgrp, 1);
		}
	}

	uint32_t items[10] = {210004, 210005, 230004, 230005, 270011, 270022, 300002, 300003, 300001, 310001};
	uint32_t percent[LOOP_GRP_CNT][10] = {
		{250, 60, 250, 60, 250, 60, 10, 10, 50, 0},
		{160, 130, 160, 130, 160, 130, 40, 40, 50, 0}, 
		{50, 210, 50, 210, 50, 210, 80, 80, 50, 10}};
	uint32_t count_ex[LOOP_GRP_CNT] = {5, 10, 15};

	cli_refresh_loopgrp_out cli_out;

	if (pgl->day_draw_cnt < LOOP_GRP_MAX_DAY) {
		if (p->chemical_grid >= get_max_grid(p) || uint32_t(p->collection_grid + 1) >= get_max_grid(p)) {
			KERROR_LOG(p->id, "bag full");
			return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
		}
		
		msg_log_loop_grp(p->id, 0, 1, p_in->grpid);
		uint32_t itemid = items[rand_type_idx(10, percent[p_in->grpid], RAND_COMMON_RAND_BASE)];
		uint32_t count = cache_add_kind_item(p, itemid, 1);
		
		db_add_item(NULL, p->id, itemid, count);
		cache_add_kind_item(p, ITEM_CHIP, count_ex[p_in->grpid]);
		db_add_item(NULL, p->id, ITEM_CHIP, count_ex[p_in->grpid]);
		stru_item tmpitem;
		tmpitem.itemid = itemid;
		tmpitem.count = count;
		cli_out.itemlist.push_back(tmpitem);
		tmpitem.itemid = ITEM_CHIP;
		tmpitem.count = count_ex[p_in->grpid];
		cli_out.itemlist.push_back(tmpitem);
	}

	pgl->day_draw_cnt ++;
	pgl->draw_total ++;
	pgl->uptime = get_now_tv()->tv_sec;
	pgl->beastcnt = LOOP_GRP_BEAST_CNT;
	for (int loop = 0; loop < LOOP_GRP_BEAST_CNT; loop ++) {
		pgl->beasts[loop].id = valid_beasts[p_in->grpid][rand() % valid_beast_cnt[p_in->grpid]];
		pgl->beasts[loop].count = 0;
	}
	db_set_loopgrp(p, p_in->grpid);

	for (int loop = 0; loop < LOOP_GRP_CNT; loop ++) {
		grp_loop_t* pgl = &p->loop_grp[loop];
		stru_loop_grp tmpgrp;
		tmpgrp.grpid = loop;
		tmpgrp.draw_total = pgl->draw_total;
		tmpgrp.isactivate = (loop == 0 ? 1 : p->loop_grp[loop - 1].day_draw_cnt > 0) && p->loop_grp[loop].day_draw_cnt < LOOP_GRP_MAX_DAY;
		for (uint32_t loopi = 0; loopi < pgl->beastcnt; loopi ++) {
			stru_id_cnt tmp;
			tmp.id = pgl->beasts[loopi].id;
			tmp.count = pgl->beasts[loopi].count;
			tmpgrp.beastlist.push_back(tmp);
		}
		cli_out.grplist.push_back(tmpgrp);
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_sync_beast_kill(sprite_t* p,Cmessage* c_in)
{
	cli_sync_beast_kill_out cli_out;
	if (beast_cnt == -1) {
		cli_out.beastcnt = MAX_KILL_BEAST_CNT;
		//report_global_beast_kill(0);
	} else {
		cli_out.beastcnt = beast_cnt;
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_check_engaged_beast_kill(sprite_t* p, Cmessage* c_in)
{
	item_t* pday = cache_get_day_limits(p, ssid_global_beast_kill);
	cli_check_engaged_beast_kill_out cli_out;
	cli_out.engaged = !!pday->count;
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  获得挑战怪物组每日的次数上限 
 *
 * @param  p
 * @param  c_in
 *
 * @return  
 */
/* ----------------------------------------------------------------------------*/
int cli_get_grp_challenge_cnt(sprite_t* p, Cmessage* c_in)
{
	cli_get_grp_challenge_cnt_in* p_in = P_IN;
	cli_get_grp_challenge_cnt_out cli_out;
	cli_out.grpid = p_in->grpid;
	beast_group_t* pbg = get_beast_grp(p_in->grpid);
	if (!pbg || !pbg->repeatcnt) {
		cli_out.count = 0;
	} else {
		item_t* pday = cache_get_day_limits(p, p_in->grpid);
		cli_out.count = pday->count;
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  挑战有次数限制的怪物时检查队友的挑战次数是否已达上线
 *
 * @param  p
 * @param  c_in
 *
 * @return  不能继续挑战队友的id
 */
/* ----------------------------------------------------------------------------*/
int cli_check_team_challenge_npc(sprite_t* p, Cmessage* c_in)
{
	cli_check_team_challenge_npc_in * p_in = P_IN;
	cli_check_team_challenge_npc_out  cli_out;
	beast_group_t* pbg = get_beast_grp(p_in->grpid);
	if(pbg && pbg->repeatcnt && p->btr_team){
		for( uint8_t loop=0 ; loop < p->btr_team->count ; loop++ ){
			item_t* pday = cache_get_day_limits(p->btr_team->players[loop], pbg->grpid);
			if (pday->count >= pbg->repeatcnt) {
				cli_out.uids.push_back(p->btr_team->players[loop]->id);
			}
		}
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}
//加入到观战队列
int add_user_to_watchers(sprite_t *p,uint32_t watcher)
{
	std::vector<uint32_t>::iterator pos;
	pos = find(p->btr_info->watchers->begin(), p->btr_info->watchers->end(), watcher);
	if (pos == p->btr_info->watchers->end()) {
		p->btr_info->watchers->push_back(watcher);
		KDEBUG_LOG(p->id,"add watcher[userid=%u]",watcher);
	}
	return 0;
}
//从观战队列中删除
int rm_user_from_watchers(sprite_t *p,uint32_t watcher)
{
	std::vector<uint32_t>::iterator pos;
	KDEBUG_LOG(p->id,"remove watcher[userid=%u]",watcher);
	pos = find(p->btr_info->watchers->begin(), p->btr_info->watchers->end(), watcher);
	if (pos != p->btr_info->watchers->end()) {
	 p->btr_info->watchers->erase(pos);
	}
	return 0;
}
//加入观战
int cli_join_watchers(sprite_t* p, Cmessage* c_in)
{
	cli_join_watchers_in *p_in=P_IN;
	sprite_t *lp=get_sprite(p_in->uid);
	if (p->busy_state != be_idle || p->busy_state == hangup || p->watchid) {
		KERROR_LOG(p->id, "busy,can't watch'\t[%u] state=%u p->watchid=%u", p_in->uid,p->busy_state,p->watchid);
		return send_to_self_error(p, p->waitcmd, cli_err_user_busy, 1);
	}
	CHECK_USER_APPLY_PK(p, p);
	CHECK_SELF_JOIN_TEAM(p);
	CHECK_SELF_BATTLE_INFO(p, p_in->uid);
	if(!lp || !lp->btr_info || lp->btr_info->state == bt_state_init) { 
		KERROR_LOG(p->id, "challeger not fighting now\t[%u]", p_in->uid); 
		return send_to_self_error(p, p->waitcmd, cli_err_not_be_fighting, 1); 
	}
	KDEBUG_LOG(p->id,"add watcher");
	add_user_to_watchers(lp,p->id);
	p->watchid=p_in->uid;
	response_proto_head(p,p->waitcmd,1,0);
	//通知用户加载战斗资源 主动发1305
	send_to_self(p,lp->btr_info->cachedata,lp->btr_info->datalen,0);
	//send_to_battle_watchers(lp->btr_info,lp->btr_info->cachedata,lp->btr_info->datalen);
	return 0;
}

int cancel_watch_ex(sprite_t* p)
{
	sprite_t *lp;
	if(p->watchid && (lp=get_sprite(p->watchid)) && lp->btr_info) { 
		rm_user_from_watchers(lp,p->id);
	}
	p->watchid=0;
	return 0;
}
//取消观战
int cli_cancel_watchers(sprite_t* p, Cmessage* c_in)
{
	cli_cancel_watchers_in *p_in=P_IN;
	cancel_watch_ex(p);
	response_proto_head(p,p->waitcmd,1,0);
	return 0;
}
