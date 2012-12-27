/*
 * =====================================================================================
 *
 *       Filename:  other_active.cpp
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  12/19/2011 09:41:27 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#include "other_active.hpp"
#include "player.hpp"
#include "item.hpp"
#include "global_data.hpp"
#include "limit.hpp"
#include "rank_top.hpp"
#include "common_op.hpp"

enum {
	xunlu_reward_cnt = 13
};
static int xunlu_reward_item[xunlu_reward_cnt] = {
	1500903, 1500567, 1500564, 1500566, 1500354, 1500565, 1500350,1500351,
	1500353, 1500352, 1540005, 1540006, 1500902
};

static int xulu_reward_odds[xunlu_reward_cnt] = {
	3, 3, 3, 3, 3, 5, 5, 5, 5, 5, 20, 20, 20
};

//+++++++++++++++
//int = player->other_info --->3 取other info ID为3 的字段信息存储
//char info[8] = reinterpret_cast<char*>(&int)
//info[0] info[1] 是本次随机奖励idx ，奖励给了之后清空
//info[2] 是当天还能掉落的空盒子数量
//+++++++++++++++
int get_item_idx_by_rand()
{
	int x = rand() % 100;
	int total = 0;
	for (int i = 0; i < xunlu_reward_cnt; ++i) {
		if (xulu_reward_odds[i] + total > x) {
			return (i + 1);
		}
	}
	
	return xunlu_reward_cnt;
}

int get_item_by_reward_idx(int idx) 
{
	return xunlu_reward_item[idx - 1];
}

int get_player_xunlu_reward_item_1(player_t * p) 
{
	int xunlu_info = get_player_other_info_value(p, 3);
	uint8_t * detail_info = reinterpret_cast<uint8_t*>(&xunlu_info);
	if (detail_info[0] == 0) {
		uint8_t x = get_item_idx_by_rand();
		detail_info[0] = x;
		set_player_other_info(p, 3, xunlu_info);
		DEBUG_LOG("PLAYER %u XUNLU REWARD ONE IDX %u", p->id, x);
	}
	return get_item_by_reward_idx(detail_info[0]);
}

int get_player_xunlu_reward_item_2(player_t * p) 
{
	int xunlu_info = get_player_other_info_value(p, 3);
	uint8_t * detail_info = reinterpret_cast<uint8_t*>(&xunlu_info);
	if (detail_info[1] == 0) {
		uint8_t x = get_item_idx_by_rand();
		detail_info[1] = x;
		set_player_other_info(p, 3, xunlu_info);
		DEBUG_LOG("PLAYER %u XUNLU REWARD TWO IDX %u", p->id, x);
	}
	return get_item_by_reward_idx(detail_info[1]);
}

//int calc_can_drop_box_cnt_2_btl(player_t * p) 
//{
//	int xunlu_info = get_player_other_info_value(p, 3);
//	char * detail_info = reinterpret_cast<char*>(&xunlu_info);
//	return static_cast<int>(detail_info[2]) * 30;
//}

void get_reward_time_when_login(player_t * p)
{
	int xunlu_info = get_player_other_info_value(p, 3);
	char * detail_info = reinterpret_cast<char*>(&xunlu_info);

	time_t cur_time_tm  = get_now_tv()->tv_sec;
	time_t on_last = p->ollast;
	struct tm * cur = localtime(&cur_time_tm);
	struct tm * last_ol = localtime(&(on_last));
	if (cur->tm_mday != last_ol->tm_mday) {	
		detail_info[2] = 0;
		set_player_other_info(p, 3, xunlu_info);
	}
	DEBUG_LOG("PLAYER %u HAS %u REWARD CNT", p->id,  detail_info[2]);
}

bool judge_can_reward(player_t * p)
{
	int xunlu_info = get_player_other_info_value(p, 3);
	char * detail_info = reinterpret_cast<char*>(&xunlu_info);
	DEBUG_LOG("PLAYER %u HAS %u  REWAD CNT", p->id, detail_info[2]);
	return (detail_info[2] < 3);
//	return true;
}

void save_reward_cnt(player_t * p)
{
	int xunlu_info = get_player_other_info_value(p, 3);
	char * detail_info = reinterpret_cast<char*>(&xunlu_info);
	detail_info[2] ++;
	set_player_other_info(p, 3, xunlu_info);
}



void clear_player_xunlu_reward_info(player_t * p)
{
	uint32_t xunlu_info = get_player_other_info_value(p, 3);
	char * detail_info = reinterpret_cast<char*>(&xunlu_info);
	detail_info[0] = 0;
	detail_info[1] = 0;
	set_player_other_info(p, 3, xunlu_info);
}

void add_item_with_out_back(player_t * p, uint32_t item_id, uint32_t cnt)
{
	uint32_t type = 2;
	int ifx = 0;
	uint32_t max_bag_grid_count = get_player_total_item_bag_grid_count(p); 
	if (!p->my_packs->check_enough_bag_item_grid(p, item_id, cnt)) {
		//物品包裹满 通过邮件插送
		char title[MAX_MAIL_TITLE_LEN + 1] = {0};
		char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
		std::vector<mail_item_enclosure> item_vec;
		strncpy(title, char_content[1].content, sizeof(title) - 1);
		strcpy(content, char_content[2].content);
		//ERROR_LOG("mail: %s ===== %s", title, content);
		item_vec.push_back(mail_item_enclosure(item_id, cnt));
		db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, 0);
		item_vec.clear();
	} else {
		pack_h(dbpkgbuf, type, ifx);
		pack_h(dbpkgbuf, item_id, ifx);
		pack_h(dbpkgbuf, cnt, ifx);
		pack_h(dbpkgbuf, max_bag_grid_count, ifx);
		DEBUG_LOG("ADD PLAYER %u ITEM %u CNT %u", p->id, item_id, cnt);
		send_request_to_db(0, p->id, p->role_tm,
				dbproto_add_item, dbpkgbuf, ifx);
	}
}

int get_xunlu_reward_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	int lie_flag = 0;
	unpack(body, lie_flag, idx);
	DEBUG_LOG("PLAYER %u XUNLU REWAED FLAG %u", p->id, lie_flag);
	uint32_t empty_box_id = 1500576;
	uint32_t lie_buff_id = 1312;
	uint32_t need_box_cnt = 30;
//	if (!judge_can_reward(p)) {
//		return send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act, 1);
//	}

	if (is_buff_actived_on_player(p, lie_buff_id)) {
		need_box_cnt = 20;	
	}

	//	uint32_t cur_tm = get_now_tv()->tv_sec;
	if (lie_flag) {
		db_add_buff(p, lie_buff_id, 7200, 0, 0);
		p->player_show_state = 1;
	}

	if (p->my_packs->get_item_cnt(empty_box_id) >= need_box_cnt) {
		//del empty box whatever
		p->my_packs->del_item(p, empty_box_id, need_box_cnt, channel_string_other);
		db_use_item_ex(0, p->id, p->role_tm, empty_box_id, need_box_cnt, false);

		int item_1 = get_player_xunlu_reward_item_1(p);
		int item_2 = get_player_xunlu_reward_item_2(p);
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, empty_box_id, idx);
		pack(pkgbuf, need_box_cnt, idx);
		pack(pkgbuf, lie_flag, idx);
		if (lie_flag) {
			pack(pkgbuf, 0, idx);
		} else {
			pack(pkgbuf, 2, idx);
			pack(pkgbuf, item_1, idx);
			pack(pkgbuf, 1, idx);
			pack(pkgbuf, item_2, idx);
			pack(pkgbuf, 1, idx);
			//clear info xunlu reward info
			clear_player_xunlu_reward_info(p);
			//reward item add 
			add_item_with_out_back(p, item_1, 1);
			add_item_with_out_back(p, item_2, 1);
//			ERROR_LOG("PLAYER %u REWARD %u %u", p->id, item_1, item_2);

			if (is_buff_actived_on_player(p, lie_buff_id)) {
				del_buff_on_player(p, lie_buff_id);
				p->player_show_state = 0;
			}
		}

//		save_reward_cnt(p);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		send_to_player(p, pkgbuf, idx, 1);
		do_stat_log_universal_interface_2(0x09526201, 0, p->id, 1);
		return 0;
	} else{
			ERROR_LOG("PLAYER %u DON'T HAS ENOUGH BOX NEED %u", p->id, need_box_cnt);
			return -1;
	}  
}
	

int get_xunlu_active_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);
	int lie_flag = 0;
	int lie_buff_id = 1312;
	if (is_buff_actived_on_player(p, lie_buff_id)) {
		lie_flag = 1;
	}
	
	int item_1 = get_player_xunlu_reward_item_1(p);
	int item_2 = get_player_xunlu_reward_item_2(p);

	pack(pkgbuf, lie_flag, idx);
	pack(pkgbuf, item_1, idx);
	pack(pkgbuf, item_2, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd,  idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int random_master_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	
	if (0 == get_player_other_info_value(p, 7)) {
		uint32_t rand_team = 1 + rand() % 2;
		KDEBUG_LOG(p->id, "player %u rand a team %u", p->id, rand_team); 
		set_player_other_info(p, 7, rand_team);
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, rand_team, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}
	ERROR_LOG("player %u already has a team", p->id);
	return -1;
}

int change_master_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t team = get_player_other_info_value(p, 7);
	if (team && p->my_packs->get_item_cnt(1500567))  {
		if (team == red_team) {
			team = blue_team;
		} else {
			team = red_team;
		}
		set_player_other_info(p, 7, team);
		p->my_packs->del_item(p, 1500567, 1, channel_string_active);
		db_use_item_ex(0, p->id, p->role_tm, 1500567, 1, false);

		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, team, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}
	ERROR_LOG("player %u don't have any team or enough item", p->id);
	return -1;
}

static int team_pvp_lv[2][3] = {
	{10, 11, 12},
	{12, 11, 10}
};

uint32_t get_and_set_pvp_lv(red_blue_info_t * info)
{
	int idx = info->left_time / 10;
	if (idx == 0) {
		idx += 1;
	}

	if (info->random_master == 0) {
		info->random_master += 1;
	}

	info->pvp_lv = team_pvp_lv[info->random_master - 1][idx - 1];
	return info->pvp_lv;
}

uint32_t init_and_get_banner(red_blue_info_t * info)
{
	info->cur_banner = 3;
	return info->cur_banner;
}

uint32_t init_and_get_left_time(red_blue_info_t * info)
{
	info->left_time = 30;
	return info->left_time;
}

int protect_banner_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{

	uint32_t god_ret = is_in_active_time_section(21);
	if (god_ret) {
	    return send_header_to_player(p, p->waitcmd, god_ret, 1);
	}
	if (0 == p->p_banner_info->random_master) {
		p->p_banner_info->random_master = get_player_other_info_value(p, 7);
	}

	if (p->battle_grp) {
		ERROR_LOG("PLAYER %u ALLREADY IN BTL NOW", p->id); 
		return -1;
	}


	if (!p->p_banner_info->cur_banner && p->p_banner_info->random_master) {
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, init_and_get_left_time(p->p_banner_info), idx);
		pack(pkgbuf, get_and_set_pvp_lv(p->p_banner_info), idx);
		pack(pkgbuf, init_and_get_banner(p->p_banner_info), idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}

	ERROR_LOG("USER %u RE PROTECT WHEN HAS A BANNER %u TEAM %u", p->id, p->p_banner_info->cur_banner, p->p_banner_info->random_master);
	return -1;
}

void player_gain_banner_score(player_t * p, uint32_t score)
{
	p->p_banner_info->cur_score += score;
	if (p->p_banner_info->random_master == red_team) {
		add_red_banner(p, score);
	} else {
		add_blue_banner(p, score);
	}
}


int rob_banner_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t god_ret = is_in_active_time_section(21);
	if (god_ret) {
	    return send_header_to_player(p, p->waitcmd, god_ret, 1);
	}

	if (p->battle_grp) {
		ERROR_LOG("PLAYER %u ALLREADY IN BTL NOW", p->id); 
		return -1;
	}

	if (0 == p->p_banner_info->random_master) {
		p->p_banner_info->random_master = get_player_other_info_value(p, 7);
	}

	if (p->p_banner_info->random_master) {
		int idx = sizeof(cli_proto_t);
		int pvp_init_lv = pvp_red_blue_1 + rand() % 3;
		pack(pkgbuf, pvp_init_lv, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}
	return -1;
}

void notify_banner_btl_over_info_2_player(player_t * p, uint32_t win_lose_flag)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, win_lose_flag, idx);
	pack(pkgbuf, p->p_banner_info->cur_score, idx);
	pack(pkgbuf, p->p_banner_info->cur_banner, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_banner_btl_over, idx);
	send_to_player(p, pkgbuf, idx, 0);
//	ERROR_LOG("NOTI BANNER OVER INFO %u %u %u" , p->id, win_lose_flag, p->p_banner_info->cur_banner);
}


void clac_and_send_banner_score(player_t * p)
{
	uint32_t gain_score = 0;
	switch (p->p_banner_info->cur_banner) {
		case 1:
			gain_score = 3; 
			break;
		case 2:
			gain_score = 8; 
			break;
		case 3:
			gain_score = 15; 
			break;
		default:
			break;
	}

	player_gain_banner_score(p, gain_score);

	p->p_banner_info->protect_banner += p->p_banner_info->cur_banner;

//	ERROR_LOG("ENDED PROTECT BANNER %u %u", p->id, p->p_banner_info->cur_banner);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->p_banner_info->cur_banner, idx);
	pack(pkgbuf, gain_score, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_protect_banner_over, idx);
	send_to_player(p, pkgbuf, idx, 0);
}

void ended_banner_protect(player_t * p)
{

	clac_and_send_banner_score(p);
	p->p_banner_info->cur_banner = 0;
	p->p_banner_info->left_time = 0;
	p->p_banner_info->pvp_lv = 0;

	save_player_red_blue_info(p);
}



void proc_player_banner_logic_after_btl_over(player_t * p, uint32_t  win_lose_flag)
{
	if (p->p_banner_info->cur_banner) {//the player who protects banner
		if (!win_lose_flag) { //lose
			p->p_banner_info->cur_banner --;
			player_gain_banner_score(p, 1);
		} else {
			player_gain_banner_score(p, 2);
		}

		notify_banner_btl_over_info_2_player(p, win_lose_flag);

		if (0 == p->p_banner_info->cur_banner) {
			ended_banner_protect(p);
		}

	} else { //the player who rob banner
		if (win_lose_flag) {
			player_gain_banner_score(p, 4);
			p->p_banner_info->rob_banner ++;
		}
		notify_banner_btl_over_info_2_player(p, win_lose_flag);
	}

	save_player_red_blue_info(p);
}


void proc_player_red_blue_logic(player_t * p)
{
	if (p->battle_grp) {
		if (test_bit_on(p->battle_grp->flag, battle_started)) {
			return;
		}
	}

	if (p->p_banner_info->cur_banner) {
		if (p->p_banner_info->left_time) {
			p->p_banner_info->left_time --;
//			ERROR_LOG("UPDATE BANNER %u %u %u", p->id, p->p_banner_info->cur_banner, p->p_banner_info->left_time);
			if (0 == p->p_banner_info->left_time % 10 && p->p_banner_info->left_time) {

				int idx = sizeof(cli_proto_t);
				pack(pkgbuf, p->p_banner_info->left_time, idx);
				pack(pkgbuf, get_and_set_pvp_lv(p->p_banner_info), idx);
				pack(pkgbuf, p->p_banner_info->cur_banner, idx);
				init_cli_proto_head(pkgbuf, p, cli_proto_banner_info_sync, idx);
				send_to_player(p, pkgbuf, idx, 0);

//				ERROR_LOG("SYNC BANNER %u %u %u %u", p->id, 
//						p->p_banner_info->cur_banner, p->p_banner_info->left_time, p->p_banner_info->pvp_lv);
			}
		} else { 
			ended_banner_protect(p);
		}
	}
}

int send_all_banner_info_to_player(player_t * p)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, get_red_banner_cnt(),  idx);
	pack(pkgbuf, get_blue_banner_cnt(), idx);
	pack(pkgbuf, p->p_banner_info->protect_banner, idx);
	pack(pkgbuf, p->p_banner_info->rob_banner, idx);
	pack(pkgbuf, p->p_banner_info->cur_score, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_list_all_banner_info,  idx);
	return send_to_player(p, pkgbuf, idx, 0);
}

int get_red_blue_info_callback(player_t* p, other_active_store_rsp_t * rsp)
{
	red_blue_rsp_t * pkg = reinterpret_cast<red_blue_rsp_t*>(rsp->info_buf);
	p->p_banner_info->cur_score = pkg->red_blue_score;
	p->p_banner_info->protect_banner = pkg->protect_banner;
	p->p_banner_info->rob_banner = pkg->rob_banner;
	if (pkg->red_blue_team) {
		p->p_banner_info->random_master = pkg->red_blue_team;
	} else if (0 == p->p_banner_info->random_master) {
		p->p_banner_info->random_master = get_player_other_info_value(p, 7);
	}

	if (p->waitcmd == cli_proto_list_all_banner_info) {
		p->p_banner_info->db_flag = 1;
		send_all_banner_info_to_player(p);
		p->waitcmd = 0;
	}
	return 0;
}

void pack_player_active_info(player_t * p, uint32_t active_id, void * buf,  int & idx)
{
	if (active_id == active_red_blue) {
		pack_h(buf, p->p_banner_info->random_master, idx);
		pack_h(buf, p->p_banner_info->protect_banner, idx);
		pack_h(buf, p->p_banner_info->rob_banner, idx);
		pack_h(buf, p->p_banner_info->cur_score, idx);
	}
}

int save_player_active_info(player_t *p, uint32_t active_id)
{
	int idx = 0;
	active_out_date_data_t * info =  get_active_out_date_info(active_red_blue);
	pack_h(dbpkgbuf, active_id, idx);
	if (info) {
		pack_h(dbpkgbuf, info->reset_type, idx);
		pack_h(dbpkgbuf, info->out_tm, idx);
	} else {
		pack_h(dbpkgbuf, 0, idx);
		pack_h(dbpkgbuf, 0, idx);
	}
	pack_player_active_info(p, active_id, dbpkgbuf,  idx);
	assert(idx <= ACTIVE_INFO_LEN + 8);
	return send_request_to_db(NULL, p->id, p->role_tm, dbproto_set_other_active_info, dbpkgbuf, ACTIVE_INFO_LEN + 8);
}

int save_player_red_blue_info(player_t * p)
{
	return save_player_active_info(p, active_red_blue);
}

int list_all_banner_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	if (p->p_banner_info->db_flag) {
		send_all_banner_info_to_player(p);
		p->waitcmd = 0;
		return 0;
	} 
	return db_get_other_active_info(p, active_red_blue);
}

int del_player_red_blue_score(player_t * p, uint32_t del_count)
{
	if (p->p_banner_info->cur_score < del_count) {
		return -1;
	}

	p->p_banner_info->cur_score -= del_count;
	save_player_red_blue_info(p);
	return 0;
}

int cancel_protect_banner_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	p->p_banner_info->cur_banner = 0;
	p->p_banner_info->left_time = 0;
	p->p_banner_info->pvp_lv = 0;
	return send_header_to_player(p, p->waitcmd, 0, 1);	
}


int db_get_other_active_info(player_t * p, uint32_t active_id)
{
	int idx = 0;
	pack_h(dbpkgbuf, active_id, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_other_active_info, dbpkgbuf, idx);
}


void proc_other_active_store_info(player_t * p , other_active_store_rsp_t * rsp)
{
	get_red_blue_info_callback(p, rsp);
}

int db_get_other_active_info_callback(player_t * p, uint32_t id, void * body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	other_active_store_rsp_t * rsp = reinterpret_cast<other_active_store_rsp_t*>(body);

	proc_other_active_store_info(p, rsp);

	return 0;
}

static std::map<uint32_t, active_out_date_data_t> * all_active_date = NULL;

active_out_date_data_t * get_active_out_date_info(uint32_t active_id)
{
	std::map<uint32_t, active_out_date_data_t>::iterator it = all_active_date->find(active_id);
	if (it != all_active_date->end()) {
		return &(it->second);
	}
	return NULL;
}


int load_active_out_date(xmlNodePtr cur)
{
	if (all_active_date == NULL) { 
		all_active_date = new std::map<uint32_t, active_out_date_data_t>;
	} else {
		all_active_date->clear();
	}

	if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Actives"))) {
		xmlNodePtr active_node = cur->xmlChildrenNode;

		while (active_node) {
			if (!xmlStrcmp(active_node->name, reinterpret_cast<const xmlChar*>("active"))) {
				TRACE_LOG("LOAD active!");
				active_out_date_data_t active;
				get_xml_prop(active.active_id, active_node, "id");

				get_xml_prop_def(active.reset_type, active_node, "reset_type", 0);

				char out_tm_str[128] = "";
				get_xml_prop_raw_str_def(out_tm_str, active_node, "out_tm", "\0");
				active.out_tm = get_utc_second_ex(out_tm_str);

				if (active.reset_type == 4 && active.out_tm == 0) {
					ERROR_LOG("REST OUT TIME ERROR ACTIVE %u", active.active_id);
					return -1;
				}

				TRACE_LOG("LOAD Active out date info %u %u %u", active.active_id, active.reset_type, active.out_tm);

				all_active_date->insert(std::map<uint32_t, active_out_date_data_t>::value_type(active.active_id, active));
			}
			active_node = active_node->next;
		}
	}
	return 0;
}



//------------------------------------------------
//------------------------------------------------

int send_active_info_to_player(player_t * p)
{
	int idx = sizeof(cli_proto_t);
    pack(pkgbuf, static_cast<uint32_t>(max_rank_top_type), idx);
    for (uint32_t i = 0; i <= max_rank_top_type; i ++) {
        pack(pkgbuf, p->other_info_2[i], 40, idx);
    }
	init_cli_proto_head(pkgbuf, p, cli_proto_get_player_other_active_info,  idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int get_player_other_active_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    //int idx = 0;
    //uint32_t active_id = 0;
    //unpack(body, active_id, idx);

    if (p->other_info_2_db_flag) {
        return send_active_info_to_player(p);
    }

	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_other_active_info_list, 0, 0);
}

int db_get_other_active_info_list_callback(player_t * p, uint32_t id, void * body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	other_active_info_rsp_t * rsp = reinterpret_cast<other_active_info_rsp_t*>(body);

    TRACE_LOG("other active count: %u", rsp->active_cnt);
    CHECK_VAL_EQ(bodylen, sizeof(other_active_info_rsp_t) + rsp->active_cnt * sizeof(other_active_t));

    other_active_t *p_active = reinterpret_cast<other_active_t*>(rsp->datas);
    for (uint32_t i = 0; i < rsp->active_cnt; i++) {
        TRACE_LOG("other active [%u %s]", p_active[i].active_id, p_active[i].info_buf);
        memcpy(p->other_info_2[p_active[i].active_id - 1], p_active[i].info_buf, sizeof(p_active[i].info_buf));
    }

    p->other_info_2_db_flag = 1;

    return send_active_info_to_player(p);
}

int save_player_active_info_2(player_t *p, uint32_t key, uint8_t *value)
{
	int idx = 0;
	active_out_date_data_t * info =  get_active_out_date_info(key);
	pack_h(dbpkgbuf, key, idx);
	if (info) {
		pack_h(dbpkgbuf, info->reset_type, idx);
		pack_h(dbpkgbuf, info->out_tm, idx);
	} else {
		pack_h(dbpkgbuf, 0, idx);
		pack_h(dbpkgbuf, 0, idx);
	}
    pack(dbpkgbuf, value, ACTIVE_BUF_LEN, idx);
	//assert(idx <= ACTIVE_INFO_LEN + 8);
	return send_request_to_db(NULL, p->id, p->role_tm, dbproto_set_other_active_info, dbpkgbuf, idx);
}

//--------------------------------------------------
//--------------------------------------------------
void print_player_active_info(uint8_t *info)
{
    int idx = 0;
    for (uint32_t i = 0; i < 5; i++) {
        uint32_t tmp_1 = 0;
        unpack(info, tmp_1, idx);
        TRACE_LOG("active daliy fate ----> [%u]", tmp_1);
    }
    for (uint32_t i = 0; i < 7; i++) {
        uint16_t tmp_2 = 0;
        unpack(info, tmp_2, idx);
        TRACE_LOG("active daliy box ----> [%u]", tmp_2);
    }
}

int send_single_active_to_player(player_t * p, uint32_t active_id)
{
	int idx = sizeof(cli_proto_t);
    pack(pkgbuf, p->other_info_2[active_id - 1], 40, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd,  idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int get_player_single_other_active_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t active_id = 0, userid = 0, role_tm = 0;
    unpack(body, active_id, idx);
    unpack(body, userid, idx);
    unpack(body, role_tm, idx);

    /*
    player_t *target = get_player(userid);
    if (target && target->role_tm == role_tm) {
        return send_single_active_to_player(p, active_id);
    }*/

    idx = 0;
    pack_h(dbpkgbuf, active_id, idx);
    pack_h(dbpkgbuf, p->id, idx);
    pack_h(dbpkgbuf, p->role_tm, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_single_other_active, dbpkgbuf, idx);
}

int db_get_single_other_active_callback(player_t * p, uint32_t id, void * body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

    other_active_t *rsp = reinterpret_cast<other_active_t*>(body);

    TRACE_LOG("uid=[%u] other active ID: %u", p->id, rsp->active_id);

    //print_player_active_info(rsp->info_buf);
    memcpy(p->other_info_2[rsp->active_id - 1], rsp->info_buf, ACTIVE_BUF_LEN);

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->active_id, idx);
    pack(pkgbuf, rsp->info_buf, ACTIVE_BUF_LEN, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd,  idx);
    return send_to_player(p, pkgbuf, idx, 1);
}


