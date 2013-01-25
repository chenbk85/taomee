#include "activity.h"
#include "cli_proto.h"
#include "items.h"
#include "event.h"
#include "beast.h"
#include "homemap.h"
#include "honor.h"
#include <async_serv/async_serv.h>
#include "./proto/mole2_home.h"
#include "./proto/mole2_home_enum.h"
#include "./proto/mole2_switch.h"
#include "./proto/mole2_switch_enum.h"
#define PVP_MAX_FAIL_COUNT 3 

int get_water_cnt_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	response_proto_uint32(p, p->waitcmd, total_water_cnt, 1, 0);
	return 0;
}

int reset_water_cnt(void* owner, void* data)
{
	refresh_group_t* prg = (refresh_group_t*)data;
	KDEBUG_LOG(0, "RESET WATER");
	if (prg) {
		try_end_group(prg);
	}
	
	total_water_cnt = 0;
	response_proto_uint32_mapid(21302, proto_cli_get_water_cnt, total_water_cnt);
	return 0;
}

int use_water_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	const uint32_t itemid = 150001;
	uint32_t count;
	int j = 0;
	UNPKG_UINT32(body, count, j);
	KDEBUG_LOG(p->id, "USE WATER\t[%u]", count);
	CHECK_USER_IN_MAP(p, 21302);
	
	normal_item_t* pni = get_item(itemid);
	CHECK_ITEM_VALID(p, pni, itemid);
	CHECK_ITEM_EXIST(p, itemid, count);

	uint32_t time = get_now_sec();
	if (total_water_cnt + count > 1) {
		count = 1 >= total_water_cnt ? (1 - total_water_cnt) : 0;
	}

	int day_idx = get_now_tm()->tm_wday;
	if (day_idx != 6 && day_idx != 0) {
		KERROR_LOG(p->id, "not right time\t[%u %u]", day_idx, time);
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
	}

	if (!count) {
		response_proto_uint32_uint32(p, p->waitcmd, total_water_cnt, count, 1, 0);
		return 0;
	}

	total_water_cnt += count;
	last_min_water_cnt += count;
	if (total_water_cnt == 1) {
		refresh_group_t* prg = get_refresh_grp(1);
		if (prg) {
			try_start_group(prg);
		}
		
		ADD_ONLINE_TIMER(&g_events, n_reset_water_cnt, prg, 60);
	}

	KDEBUG_LOG(p->id, "WATER CNT\t[%u %u]", total_water_cnt, count);
	db_add_item(NULL, p->id, itemid, -count);
	cache_reduce_kind_item(p, itemid, count);
	response_proto_uint32_mapid(21302, proto_cli_get_water_cnt, total_water_cnt);
	response_proto_uint32_uint32(p, p->waitcmd, total_water_cnt, count, 1, 0);
	return 0;
}

int cli_get_buff_range(sprite_t *p, Cmessage* c_in)
{
	cli_get_buff_range_in* p_in = P_IN;
	mole2_get_user_cli_buff_in db_in;
	db_in.min_id = p_in->min_buff_id;
	db_in.max_id = p_in->max_buff_id;

	return send_msg_to_db(p, p->id, mole2_get_user_cli_buff_cmd, &db_in);
}

int mole2_get_user_cli_buff(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	mole2_get_user_cli_buff_out* p_out = P_OUT;
	cli_get_buff_range_out cli_out;
	for(uint32_t loop = 0; loop < p_out->item_list.size(); loop ++) {
		cli_out.buff_list.push_back(p_out->item_list[loop]);
	}

	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int set_cli_buff_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	struct {
		uint32_t type;
		char	 buff[512];
	}__attribute__((packed)) req;

	UNPKG_UINT32(body, req.type, j);//min
	UNPKG_STR(body, req.buff, j,sizeof(req.buff));//max
	
	return send_request_to_db(p,p->id,proto_db_set_cli_buff,&req,sizeof(req));
}

int set_cli_buff_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	response_proto_head(p,p->waitcmd,1,0);
	return 0;
}

int cli_start_fishing (sprite_t* p, Cmessage * c_in)
{
	CHECK_USER_TIRED(p);
	item_t* pday = cache_get_day_limits(p, ssid_fishing_cnt);
	if (pday->count >= 10) {
		KERROR_LOG(p->id, "fishing day max\t[%u]", pday->count);
		return send_to_self_error(p, p->waitcmd, cli_err_rand_day_max, 1);
	}

	p->cur_fish_cnt = 0;
	pday->count ++;
	db_day_add_ssid_cnt(NULL, p->id, ssid_fishing_cnt, 1, 3);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int cli_fishing_get_item (sprite_t* p, Cmessage * c_in)
{
	CHECK_FISHING_STARTED(p);	
	CHECK_FISHING_CNT(p);	
	cli_fishing_get_item_in* p_in = P_IN;
	cli_fishing_get_item_out out;
	out.itemid = p_in->itemid;
	if (p_in->itemid) {
		normal_item_t* pni = get_item(p_in->itemid);
		CHECK_ITEM_VALID(p, pni, p_in->itemid);
		if (!pni->for_fish) {
			KERROR_LOG(p->id, "not for fish\t[%u]", p_in->itemid);
			return send_to_self_error(p, p->waitcmd, cli_err_not_for_fish, 1);
		}

		if (cache_add_kind_item(p, p_in->itemid, 1)) {
			db_add_item(NULL, p->id, p_in->itemid, 1);
			out.pos = 0;
		} else {
			db_add_item_to_house(NULL, p->id, p_in->itemid, 1);
			out.pos = 1;
		}
	}
	p->cur_fish_cnt ++;	

	return send_msg_to_self(p, p->waitcmd, &out, 1);
}

int cli_finish_fishing (sprite_t* p, Cmessage * c_in)
{
	p->cur_fish_cnt = -1;
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int load_fish_info(xmlNodePtr cur)
{
	GList* pitems = g_hash_table_get_values(all_items);
	GList* head = pitems;
	while (pitems) {
		normal_item_t* pni = (normal_item_t *)pitems->data;
		pni->for_fish = 0;
		pitems = pitems->next;
	}
	g_list_free(head);

	GList* pgrps = g_hash_table_get_values(beast_grp);
	head = pgrps;
	while (pgrps) {
		beast_group_t* pbg = (beast_group_t *)pgrps->data;
		pbg->for_fish = 0;
		pgrps = pgrps->next;
	}
	g_list_free(head);

	cur = cur->xmlChildrenNode; 
	while (cur) {
		uint32_t itemid, grpid;
		if (!xmlStrcmp(cur->name, (const xmlChar*)"title")) {
			decode_xml_prop_uint32_default(&itemid, cur, "ItemID", 0);		
			decode_xml_prop_uint32_default(&grpid, cur, "GrpID", 0);		
			normal_item_t* pni = get_item(itemid);
			if (pni) {
				TRACE_LOG("FISH ITEM\t[%u]", itemid);
				pni->for_fish = 1;
			}

			beast_group_t* pbg = get_beast_grp(grpid);
			if (pbg) {
				TRACE_LOG("FISH GRP\t[%u]", grpid);
				pbg->for_fish = 1;
			}
		}

		cur = cur->next;
	}
	return 0;
}

int get_vacancy_cnt(sprite_t * p, Cmessage * c_in)
{
	response_proto_uint32(p, p->waitcmd, p->vacancy_cnt, 1, 0);
	return 0;
}

int mole2_get_hero_team(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_get_hero_team_out* p_out = P_OUT;
	p->hero_team = p_out->teamid;
	p->medal_devote = p_out->medals;
	return send_request_to_db(p, p->id, proto_db_get_offline_msg, NULL, 0);
}

int cli_get_hero_team_rank(sprite_t* p, Cmessage* c_in)
{
	return send_msg_to_db(p, p->id, mole2_get_team_rank_cmd, NULL);
}


int cli_get_hero_team_reward(sprite_t* p, Cmessage* c_in)
{
	if (!p->hero_team) {
		return send_to_self_error(p, p->waitcmd, cli_err_not_registerd, 1);
	}

	if (p->flag & (1 << flag_herocup_reward)) {
		KERROR_LOG(p->id, "have reward");
		return send_to_self_error(p, p->waitcmd, cli_err_already_get_this, 1);
	}

	if (get_bag_cloth_cnt(p) >= get_max_grid(p))
		return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
	return send_msg_to_db(p, p->id, mole2_get_team_rank_cmd, NULL);
}

int rsp_get_hero_team_rank(sprite_t* p, mole2_get_team_rank_out* p_out)
{
	cli_get_hero_team_rank_out cli_out;
	for (uint32_t loop = 0; loop < p_out->ranks.size(); loop ++) {
		stru_team_rank_t rank;
		rank.teamid = p_out->ranks[loop].teamid;
		rank.count = p_out->ranks[loop].count;
		rank.medals = p_out->ranks[loop].medals;
		cli_out.ranks.push_back(rank);
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int rsp_get_hero_team_reward(sprite_t* p, mole2_get_team_rank_out* p_out)
{
	uint32_t clothids[4] = {80021, 80023, 80025, 80027};
	for (uint32_t loop = 0; loop < p_out->ranks.size() && loop < 4; loop ++) {
		if (p->hero_team == p_out->ranks[loop].teamid) {
			if (loop == 0) {
				normal_item_t* pni = get_item(310008);
				if (!pni)
					return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
				CHECK_CAN_HAVE_MORE_ITEM(p, pni, 1);
			}
			cloth_t* pc = get_cloth(clothids[loop]);
			if (!pc || !pc->clothlvs[0].valid)
				return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
			*(uint32_t *)p->session = loop + 1;
			return db_add_cloth(p, p->id, 0, pc, 0, &pc->clothlvs[0]);
		}
	}
	cli_get_hero_team_reward_out cli_out;
	cli_out.teamid = p->hero_team;
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int mole2_get_team_rank(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_get_team_rank_out* p_out = P_OUT;
	switch (p->waitcmd) {
	case cli_get_hero_team_rank_cmd:
		return rsp_get_hero_team_rank(p, p_out);
	case cli_get_hero_team_reward_cmd:
		return rsp_get_hero_team_reward(p, p_out);
	default:
		return -1;
	}
}

int cli_devote_medals(sprite_t* p, Cmessage* c_in)
{
	normal_item_t* pni = get_item(290006);
	CHECK_ITEM_VALID(p, pni, 290006);
	uint32_t count = get_item_cnt(p, pni);
	CHECK_INT_GE(count, 1);
	if (!p->hero_team) {
		KERROR_LOG(p->id, "not reg");
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	cache_reduce_kind_item(p, 290006, count);
	db_add_item(NULL, p->id, 290006, -count);
	uint32_t exp = add_exp_to_sprite(p, count * 100);
	response_proto_uint32_uint32(p, p->waitcmd, count, exp, 1, 0);
	return 0;
}

int cli_get_hero_cup_reward(sprite_t* p, Cmessage* c_in)
{
	if (p->flag & 1 << flag_herocup_item) {
		KERROR_LOG(p->id, "have reward");
		return send_to_self_error(p, p->waitcmd, cli_err_already_get_this, 1);
	}

	if (p->flag & 1 << flag_herocup_new) {
		KERROR_LOG(p->id, "have reward");
		return send_to_self_error(p, p->waitcmd, cli_err_hero_cup_new, 1);
	}

	if (!p->hero_team) {
		KERROR_LOG(p->id, "not reg");
		return send_to_self_error(p, p->waitcmd, cli_err_not_registerd, 1);
	}

	return send_msg_to_db(p, p->id, mole2_get_user_rank_cmd, NULL);
}

int cli_get_hero_user_rank(sprite_t* p, Cmessage* c_in)
{
	return send_msg_to_db(p, p->id, mole2_get_user_rank_cmd, NULL);
}

int get_user_rank(sprite_t* p, mole2_get_user_rank_out* p_out)
{
	cli_get_hero_user_rank_out cli_out;
	for (uint32_t loop = 0; loop < p_out->ranks.size(); loop ++) {
		if (p_out->ranks[loop].userid == p->id) {
			cli_out.rank = loop + 1;
			return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
		}
	}
	cli_out.rank = -1;
	
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int give_hero_cup_reward(sprite_t* p, mole2_get_user_rank_out* p_out)
{
	cli_get_hero_cup_reward_out cli_out;
	cli_out.itemid = ITEM_ID_ROCK_CHIP;
	normal_item_t* pni = get_item(cli_out.itemid);
	if (!pni) {
		KERROR_LOG(p->id, "item id invalid\t[%u]", cli_out.itemid);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	
	const uint32_t honor_base = 3031;
	const uint32_t item_cnt_base = 50;
	for (uint32_t loop = 0; loop < p_out->ranks.size(); loop ++) {
		if (p_out->ranks[loop].userid == p->id) {
			KDEBUG_LOG(p->id, "GET HERO CUP REWARD\t[%u]", loop);

			cli_out.rank = loop + 1;
			cli_out.count = item_cnt_base - loop / 20 * 10;

			CHECK_CAN_HAVE_MORE_ITEM(p, pni, cli_out.count);

			honor_attr_t* pha = get_honor_attr(honor_base + loop / 20);
			if (pha) {
				update_one_honor(p, pha);
				set_user_honor(p, pha->honor_id);
				db_add_user_honor(NULL, p->id, p->prof_lv, 0, pha->honor_id);
				notify_user_honor_up(p, 1, 0, 1);
				db_set_user_honor(0, p->id, pha->honor_id);
			}
			
			cache_add_kind_item(p, cli_out.itemid, cli_out.count);
			db_add_item(NULL, p->id, cli_out.itemid, cli_out.count);
			p->flag |= 1 << flag_herocup_item;
			db_set_flag(NULL, p->id, flag_herocup_item, 1);
			return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
		}
	}

	cli_out.count = 5;
	cli_out.rank = -1;
	CHECK_CAN_HAVE_MORE_ITEM(p, pni, cli_out.count);
	
	cache_add_kind_item(p, cli_out.itemid, cli_out.count);
	db_add_item(NULL, p->id, cli_out.itemid, cli_out.count);
	p->flag |= 1 << flag_herocup_item;
	db_set_flag(NULL, p->id, flag_herocup_item, 1);
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}


int mole2_get_user_rank(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_get_user_rank_out* p_out = P_OUT;
	switch (p->waitcmd) {
	case cli_get_hero_cup_reward_cmd:
		return give_hero_cup_reward(p, p_out);
	case cli_get_hero_user_rank_cmd:
		return get_user_rank(p, p_out);
	default:
		return -1;
	}
}

int cli_apply_megalith_war(sprite_t* p, Cmessage* c_in)
{
	CHECK_USER_IN_MAP(p, 0);
	CHECK_USER_APPLY_PK(p, p);
	CHECK_SELF_BATTLE_INFO(p, p->id);
	CHECK_SELF_TEAM_LEADER(p, p->id, 1);

	if (IS_WAR_MAP(p->tiles->id)) {
		KERROR_LOG(p->id, "already in war map\t[%lu]", p->tiles->id);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	uint8_t out[256];
	int i =0; 
	if (!p->btr_team) {
		p->uiflag = 0;
	} else {
		for (uint32_t loop = 0; loop < p->btr_team->count; loop ++) {
			sprite_t* s = p->btr_team->players[loop];
			s->uiflag = 0;
		}
	}

	PKG_UINT32(out,p->level , i);
	send_to_homeserv(NULL, MCAST_home_msg, i, out, p->id, 0, cli_apply_megalith_war_cmd);

	p->team_state = apply_for_war;

	return send_msg_to_self(p, p->waitcmd, NULL, 1);
}

int cli_quit_megalith_war(sprite_t* p, Cmessage* c_in)
{
	CHECK_SELF_TEAM_LEADER(p, p->id, 1);
	protocol_t proto;
	init_proto_head(&proto, cli_quit_megalith_war_cmd, sizeof(protocol_t));
	if (!p->btr_team) {
		response_proto_head(p, p->waitcmd, 1, 0);
		send_to_homeserv(NULL, MCAST_home_msg, 0, NULL, p->id, 0, cli_quit_megalith_war_cmd);
	} else {
		for (uint32_t loop = 0; loop < p->btr_team->count; loop ++) {
			response_proto_head(p->btr_team->players[loop], p->waitcmd, 0, 0);
			send_to_homeserv(NULL, MCAST_home_msg, 0, NULL, p->btr_team->players[loop]->id, 0, cli_quit_megalith_war_cmd);
		}
		p->waitcmd = 0;
	}

	KDEBUG_LOG(p->id, "CANCEL STATE");
	p->team_state = 0;
	return 0;
}

int cli_war_challenge_pos(sprite_t* p, Cmessage* c_in)
{
	CHECK_USER_IN_MAP(p, 0);
	if (!IS_WAR_MAP(p->tiles->id)) {
		KERROR_LOG(p->id, "not in war map\t[%lu]", p->tiles->id);
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
	}
	cli_war_challenge_pos_in* p_in = P_IN;
	uint8_t out[64];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, p_in->pos, i);
	init_proto_head(out, cli_war_challenge_pos_cmd, i);
	send_to_homeserv(NULL, MCAST_home_msg, i, out, p->id, p->tiles->id, cli_war_challenge_pos_cmd);
	p->waitcmd = 0;
	return 0;
}

int cli_war_play_card(sprite_t* p, Cmessage* c_in)
{
	CHECK_USER_IN_MAP(p, 0);
	if (!IS_WAR_MAP(p->tiles->id)) {
		KERROR_LOG(p->id, "not in war map\t[%lu]", p->tiles->id);
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
	}
	cli_war_play_card_in* p_in = P_IN;
	uint8_t out[64];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, p_in->card, i);
	init_proto_head(out, cli_war_play_card_cmd, i);
	send_to_homeserv(NULL, MCAST_home_msg, i, out, p->id, p->tiles->id, cli_war_play_card_cmd);
	p->waitcmd = 0;
	return 0;
}

int cli_war_attack_boss(sprite_t* p, Cmessage* c_in)
{
	CHECK_USER_IN_MAP(p, 0);
	if (!IS_WAR_MAP(p->tiles->id)) {
		KERROR_LOG(p->id, "not in war map\t[%lu]", p->tiles->id);
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
	}

	protocol_t proto;
	init_proto_head(&proto, cli_war_attack_boss_cmd, sizeof(protocol_t));
	send_to_homeserv(NULL, MCAST_home_msg, sizeof(protocol_t), &proto, p->id, p->tiles->id, cli_war_attack_boss_cmd);
	p->waitcmd = 0;
	return 0;
}

int cli_get_positions_info(sprite_t* p, Cmessage* c_in)
{
	
	CHECK_USER_IN_MAP(p, 0);
	if (!IS_WAR_MAP(p->tiles->id)) {
		KERROR_LOG(p->id, "not in war map\t[%lu]", p->tiles->id);
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
	}

	protocol_t proto;
	init_proto_head(&proto, cli_get_positions_info_cmd, sizeof(protocol_t));
	send_to_homeserv(NULL, MCAST_home_msg, sizeof(protocol_t), &proto, p->id, p->tiles->id, cli_get_positions_info_cmd);
	p->waitcmd = 0;
	return 0;
}

int cli_leave_position(sprite_t* p, Cmessage* c_in)
{
	CHECK_USER_IN_MAP(p, 0);
	if (!IS_WAR_MAP(p->tiles->id)) {
		KERROR_LOG(p->id, "not in war map\t[%lu]", p->tiles->id);
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_map, 1);
	}
	protocol_t proto;
	init_proto_head(&proto, cli_leave_position_cmd, sizeof(protocol_t));
	send_to_homeserv(NULL, MCAST_home_msg, sizeof(protocol_t), &proto, p->id, p->tiles->id, cli_leave_position_cmd);
	p->waitcmd = 0;
	return 0;
}

int cli_start_game(sprite_t* p, Cmessage* c_in)
{
	if (p->illegal_times >= 15) {
		p->waitcmd = 0;
		return 0;
	}
	cli_start_game_in* p_in = P_IN;
	p->game_start_time = get_now_tv()->tv_sec;
	p->game_type = p_in->type;
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int cli_game_pvp_get_map_id(sprite_t* p, Cmessage* c_in)
{
	//return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	cli_game_pvp_get_map_id_in *p_in=P_IN;
	bool check_time_flag=true;
	//检查人是不是合法的
	switch ( p->id ){
		case 821108:
		case 224478144:
		case 272824558:
		case 231805151:
		case 229908064:
		case 222669200:
		case 16649012:
		case 238716866:
		case 185200997:
		case 284916247:
		case 88693:
		case 5004699:
			check_time_flag=false;
			break;
		default :
			break;
	}
	KDEBUG_LOG(p->id,"CHECK_TIME_FLAG:%u" ,check_time_flag);
	if (check_time_flag) {
		//检查时间对不对
		time_t nowtime = get_now_tv()->tv_sec;
		struct tm tm_tmp;
		localtime_r(&nowtime, &tm_tmp);
		if (!(tm_tmp.tm_wday==0 && 
					((tm_tmp.tm_hour>=14 && tm_tmp.tm_hour<15)|| 
					(tm_tmp.tm_hour>=19 && tm_tmp.tm_hour<20)))){
			KDEBUG_LOG(p->id, "tm_tmp.tm_wday=%u tm_tmp.tm_hour:%u", tm_tmp.tm_wday,tm_tmp.tm_hour );
			return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
		}

	}
	
			
	if (user_is_in_war_map(p) || p_in->pvp_type > 1){
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	if (p->btr_team){//在队伍
		return send_to_self_error(p, p->waitcmd, cli_err_alrdy_join_team, 1);
	}

	p->pvp_type=p_in->pvp_type;
	if ( p->pvp_pk_fail_times[p->pvp_type]>=PVP_MAX_FAIL_COUNT){
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	CHECK_LV_FIT(p,p,20,MAX_SPRITE_LEVEL);
	p->pvp_pk_can_get_item=0;//默认
	home_game_get_map_id_in h_in;
	h_in.type=p->pvp_type;
	h_in.level=p->level;
	memcpy(h_in.nick,p->nick,sizeof(h_in.nick));
	send_msg_to_homeserv(p->id, 0, 	home_game_get_map_id_cmd ,&h_in);
	//统计
	//msg_log_uint32(0x0B5316d7, p->id);
	return 0;
}

int cli_game_get_pvp_info(sprite_t* p, Cmessage* c_in)
{
	cli_game_get_pvp_info_out cli_out;
	//p->pvp_pk_fail_times=0;
	cli_out.scores=p->pvp_scores;
	cli_out.left_pk_count=PVP_MAX_FAIL_COUNT-p->pvp_pk_fail_times[0];
	cli_out.left_pk2_count=PVP_MAX_FAIL_COUNT-p->pvp_pk_fail_times[1];
	if (cli_out.left_pk_count>PVP_MAX_FAIL_COUNT) cli_out.left_pk_count=0;
	if (cli_out.left_pk2_count>PVP_MAX_FAIL_COUNT) cli_out.left_pk2_count=0;
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_pvp_get_honor(sprite_t* p, Cmessage* c_in)
{
	cli_pvp_get_honor_in * p_in = P_IN;
	cli_pvp_get_honor_out cli_out;
	uint32_t new_honor=p_in->honorid;
	cli_out.honorid=p_in->honorid;
	stru_item item;
		
	//检查是不是已经有了
	if(check_honor_exist(p,new_honor) ){
		DEBUG_LOG("check_honor_exist(p,new_honor) err: honorid=%u  ",new_honor );
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
#define ADD_ITEM_TO_LIST(item_list,_itemid,_count )  {\
		item.itemid=_itemid;\
		item.count=_count;\
		item_list.push_back(item);\
}while(0)


	DEBUG_LOG("pvp_scores =%u  ",p->pvp_scores );
	
	switch ( new_honor ){
		case 3036:
			if (p->pvp_scores>=1){
				ADD_ITEM_TO_LIST(cli_out.item_list,300071,1 );
			}
			break;
		case 3037:
			if (p->pvp_scores>=2){
				ADD_ITEM_TO_LIST(cli_out.item_list,300070,1 );
			}
	
			break;
		case 3038:
			if (p->pvp_scores>=6){
				ADD_ITEM_TO_LIST(cli_out.item_list,300069,1 );
			}
	
			break;
		case 3039:
			if (p->pvp_scores>=25){
				ADD_ITEM_TO_LIST(cli_out.item_list,300068,1 );
			}
			break;
		default :
			break;
	}

	if (cli_out.item_list.size() ==0) {
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}


	//加上honorid
	honor_attr_t* pha = get_honor_attr(new_honor);
	if (pha) {
		//DEBUG_LOG( "ADD HONOR id=%u",new_honor );
		update_one_honor(p, pha);
		set_user_honor(p, new_honor);
		notify_user_honor_up(p, 1, 0, 1);
		db_add_user_honor(NULL, p->id, p->prof_lv, 0, new_honor);
	}
	for (uint32_t i=0;i<cli_out.item_list.size();i++){
		//加物品
		stru_item *p_item=&(cli_out.item_list[i]);
		//重置实际加上的个数
		p_item->count=p->add_item(p_item->itemid,p_item->count );
			
	}
		
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);

}
#define     LUCKYSTAR_START_TIME  10*3600
#define     LUCKYSTAR_END_TIME  22*3600
#define     MAX_LUCKYSTAR_HANGUP_TIME  9*1800
#define     LUCKYSTAR_DOUBLE_TIME  5*1800
/**
 * @brief 检查是否可以获得幸运星  
 * @param hangup 挂机时间
 */

inline bool is_in_lucky_star_time()
{
	uint32_t nowtime = get_now_sec();
	int day_idx = get_now_tm()->tm_wday;
	if((nowtime < 10*3600 || nowtime > 22*3600 ) || //time wrong
			(day_idx!=5 && day_idx!=6 && day_idx!=0)){//date wrong
		return false;
	}
	return true;

}
/**
 * @brief 检查当前挂机时间是否能获得幸运星
 */
inline bool check_get_luckystar(uint32_t hangup)
{
	if(hangup <= LUCKYSTAR_DOUBLE_TIME ){
		//挂机小于2.5小时 每半个小时可以获得一个
		return (hangup>=1800 && (hangup%1800==0)) ; 
	}else{
		//挂机大于2.5小时 每个小时可以获得一个
		return ((hangup-LUCKYSTAR_DOUBLE_TIME)>=3600 && ((hangup-LUCKYSTAR_DOUBLE_TIME)%3600==0)); 
	}
	/* 测试
	if(hangup <= 600 ){
		return (hangup>=120 && (hangup%120==0)) ; 
	}else{
		return ((hangup-600)>=240 && ((hangup-600)%240==0)); 
	}
	*/
}

/**
 * @brief  检查是否到了防尘迷时间 即在线超过5小时
 * @return 到了防沉迷时间 返回true 
 */
inline bool check_anti_indulgence(sprite_t *p)
{
	uint32_t online_time = p->online_time + (get_now_tv()->tv_sec - p->login_time);
	return (online_time>=5*3600);
}

/**
 * @brief  幸运星活动定时器  每分钟检查一次
 */
int lucky_star_add_time(void*owner, void *data)
{
	sprite_t *p=(sprite_t *)owner;
	p->p_lucky_star=ADD_ONLINE_TIMER(p, n_lucky_star_timer, NULL, 60);
	if(is_in_lucky_star_time() && !check_anti_indulgence(p)){
		item_t* ptime = cache_get_day_limits(p, ssid_luckystar_hangup_time);
		ptime->count+=60;
		if( ptime->count%600){
			//每十分钟写入数据库
			db_day_set_ssid_cnt(NULL,p->id, ssid_luckystar_hangup_time,ptime->count,ptime->count);
		}
		if(check_get_luckystar(ptime->count)){
			p->luckystar_count++;
			db_day_add_ssid_cnt(NULL,p->id, item_id_lucky_star ,1,-1);
		}
		KDEBUG_LOG(p->id,"lucky star add time:hangup:%u star_cnt:%u",ptime->count,p->luckystar_count);
	}
	return 0;	
}

/**
 * @brief 拉取剩余时间 
 * @param hangup 今日已经挂机时间
 * @return 下一幸运星剩余时间 不再活动时间内返回-1
 */
inline uint32_t get_luckystar_lefttime(sprite_t *p, uint32_t hangup)
{
	uint32_t lefttime;
	/* 测试	
	if(hangup <= 600 ){
		lefttime= (hangup/120 + 1)*120 - hangup; 
	}else{
		lefttime = ((hangup-600)/240+1)*240-(hangup-600); 
	}
	*/
	if(!is_in_lucky_star_time() || hangup >= MAX_LUCKYSTAR_HANGUP_TIME){
		return -1;
	}
	if(hangup <= LUCKYSTAR_DOUBLE_TIME ){
		//挂机小于2.5小时 每半个小时可以获得一个
		lefttime= (hangup/1800 + 1)*1800 - hangup; 
	}else{
		//挂机大于2.5小时 每个小时可以获得一个
		lefttime = ((hangup-LUCKYSTAR_DOUBLE_TIME)/3600+1)*3600-(hangup-LUCKYSTAR_DOUBLE_TIME); 
	}
	return lefttime;
}

/**
 * @brief 拉取幸运星活动的相关信息
 * @return 下一幸运星获得的剩余时间和幸运星总数 
 */
int cli_get_lucky_star_info(sprite_t* p, Cmessage* c_in)
{
	cli_get_lucky_star_info_out out;
	item_t* ptime = cache_get_day_limits(p, ssid_luckystar_hangup_time);
	out.lefttime = get_luckystar_lefttime(p, ptime->count);
	out.star_cnt=p->luckystar_count;
	KDEBUG_LOG(p->id,"cli_get_lucky_star_info:lefttime:%u star_cnt:%u",out.lefttime,out.star_cnt);
	return send_msg_to_self(p,p->waitcmd,&out,1);
}

int get_luck_star_reward(sprite_t *p)
{
	KDEBUG_LOG(p->id,"get_lucky_star_reward");
	uint32_t check_item[3]={50017,160004,150001};
	int32_t check_cnt[3]={70,20,20};
	uint32_t reward=310027;
	//扣物品
	for (uint8_t loop = 0; loop < 3; loop ++) {
		if(check_item[loop] ==  item_id_lucky_star){
			p->luckystar_count -= check_cnt[loop];
			db_sub_ssid_totalcnt(NULL, p->id, item_id_lucky_star, check_cnt[loop]); 
		}else if (check_item[loop] != item_id_lucky_star ){
			cache_reduce_kind_item(p, check_item[loop], check_cnt[loop]);
			db_add_item(NULL, p->id, check_item[loop], check_cnt[loop]*(-1));
		}
	}
	cache_add_kind_item(p, reward, 1);
	db_add_item(NULL, p->id, reward, 1);
	msg_log_lucky_star_exchange(2, p->id);
	response_proto_uint32(p, p->waitcmd, reward, 1, 0);
	//broadcast msg
	noti_all_online_get_item(p, reward);
	return 0;
}
/**
 * @brief 幸运星兑换
 *6个幸运星 +  20个面粉  +  5个至纯之水     兑换 星锤兔精灵蛋 1个
 *40个幸运星+  20个糖    +  10个至纯之水    兑换 金羊狼精灵蛋 1个
 *135个幸运星+ 20个鸡蛋  +  20个至纯之水    兑换 伊瑟拉精灵蛋  1个（999个限量）"

 * @param type:0,1,2 
 * @return  精灵蛋id 
 */
#define     MAX_STAR_EXCHANGE_CNT  999 
int cli_exchange_lucky_star(sprite_t* p, Cmessage* c_in)
{
	cli_exchange_lucky_star_in *p_in=P_IN;
	if(p_in->type>2 ){
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	KDEBUG_LOG(p->id,"cli_exchange_lucky_star:%u %u %u",p->luckystar_count,p_in->type,g_luckystar_exchange);
	if(IS_IN_BATTLE(p)){
		return send_to_self_error(p, p->waitcmd, cli_err_self_be_fighting, 1);
	}
	uint32_t check_item[][3]={ {50017,160001,150001},{50017,160002,150001},{50017,160004,150001}};
	int32_t check_cnt[][3]={{6,20,5},{20,20,10},{70,20,20}};
	uint32_t reward[]={310026,310055,310027};
	uint32_t ret=0;
	//check item count enough
	for (uint8_t loop = 0; loop < 3; loop ++) {
		if ((ret=item_enough_check(p, check_item[p_in->type][loop], check_cnt[p_in->type][loop]))){
			KERROR_LOG(p->id,"item not enough:%u %u",check_item[p_in->type][loop],(uint32_t)check_cnt[p_in->type][loop]);
			return send_to_self_error(p, p->waitcmd, cli_err_havnot_this_item, 1);
		}
	}

	if( p_in->type==2 ){
		//发往switch 全局累加
		switch_add_luckystar_exchange_cnt_in sw;	
		sw.uid=p->id;
		sw.count=1;
		sw.max=MAX_STAR_EXCHANGE_CNT;
		return send_msg_to_switch(p, p->id, switch_add_luckystar_exchange_cnt_cmd, &sw);
	}
	//扣物品
	for (uint8_t loop = 0; loop < 3; loop ++) {
		if(check_item[p_in->type][loop] ==  item_id_lucky_star){
			p->luckystar_count -= check_cnt[p_in->type][loop];
			db_sub_ssid_totalcnt(NULL, p->id, item_id_lucky_star, check_cnt[p_in->type][loop]); 
		}else if (check_item[p_in->type][loop] != item_id_lucky_star ){
			cache_reduce_kind_item(p, check_item[p_in->type][loop], check_cnt[p_in->type][loop]);
			db_add_item(NULL, p->id, check_item[p_in->type][loop], check_cnt[p_in->type][loop]*(-1));
		}
	}
	if( p_in->type == 1){
		noti_all_online_get_item(p, reward[p_in->type]);
	}
	cache_add_kind_item(p, reward[p_in->type], 1);
	db_add_item(NULL, p->id, reward[p_in->type], 1);
	msg_log_lucky_star_exchange(p_in->type, p->id);
	response_proto_uint32(p, p->waitcmd, reward[p_in->type], 1, 0);
	return 0;
}
