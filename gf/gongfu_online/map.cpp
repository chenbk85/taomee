#include <cassert>

#include <libtaomee++/inet/pdumanip.hpp>

using namespace taomee;

extern "C" {
#include <glib.h>

#include <libtaomee/log.h>
}

#include "cli_proto.hpp"
#include "map.hpp"
#include "trade.hpp"
#include "battle.hpp"
#include "global_data.hpp"
#include "common_op.hpp"
#include "mcast_proto.hpp"
#include "fight_team.hpp"

// hashes maps, all maps are linked together here
//static list_head_t all_maps[c_bucket_size];

//-----------------------------------------------------
// inline function declarations
//-----------------------------------------------------
inline bool is_player_visible(player_t* p);
inline map_t* alloc_map(map_id_t mid);
inline void   dealloc_map(map_t* m);
//-----------------------------------------------------
// static function declarations
//-----------------------------------------------------
static int  pack_entermap_rsp(player_t* p, uint8_t* buf);

//---------------------------------------------------------------------

/**
  * @brief player enters map
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int enter_map_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t map_low_id;
	uint32_t map_type;
	uint32_t x,y;
	unpack(body, map_type, i);
	unpack(body, map_low_id, i);
	unpack(body, x, i);
	unpack(body, y, i);

    KDEBUG_LOG(p->id, "E M\t[uid=%u type=%u mid=%u]", p->id, map_type, map_low_id);
	
	//智力大比拼
	if (map_low_id >= 43 && map_low_id <= 52) {
		if (get_now_active_data_by_active_id(3)) {
			if (p->coins >= 10000) {
				p->coins -= 10000;
				db_set_role_base_info(p);
			} else {
				return -1;
			}
			p->puzzle_data->book_set_flag = 0;
			p->puzzle_data->kick_flag = 0;
			p->puzzle_data->right_cir = 0;
		//	ERROR_LOG("PUZZLE  ENTER ROOM %u", p->id);
			do_stat_log_universal_interface_1(0x09526007, 0, p->id);
		} else {
			p->puzzle_data->kick_flag = 1;
			p->puzzle_data->right_cir = 0;
		}
	}

	if (map_low_id >= 1004 && map_low_id <= 1013) {
		if (can_player_enter_team_contest(p, map_low_id)) {
			 player_enter_fight_team_contest_map(p, map_low_id);
		} else {
			return send_header_to_player(p, p->waitcmd, cli_err_not_contest_enter_time, 1);
		}	
	}

	enter_map(p, make_mapid(map_type , map_low_id), x, y);

	
	//玩家进入小屋地图的时候对于其他玩家都是不可见的
	//所以玩家退出小屋地图还要设置回来
	if(map_low_id == 40){
		p->invisible = 1;
	}
	return 0;
}

/**
  * @brief player leaves map
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int leave_map_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	leave_map(p);
	return 0;
}

/**
  * @brief get info of all the players in this map
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int list_user_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	TRACE_LOG("%u", p->id);
	if (p->trade_grp) {
		return trsvr_player_list_user(p);
	}
	if (p->home_grp){
		p->waitcmd = 0;
		return homesvr_player_home_list_user(p);
	}
	
	int i = sizeof(cli_proto_t) + 4;
	uint32_t cnt = 0;
	list_head_t* l;
	list_for_each (l, &p->cur_map->playerlist) {
		player_t* player = list_entry(l, player_t, maphook);
		if(is_player_visible(player)) {
			TRACE_LOG("player[%u %u]",p->id, player->id);
			i += pack_entermap_rsp(player, pkgbuf + i);
			++cnt;
		}
	}

	*(uint32_t*)(pkgbuf + sizeof(cli_proto_t)) = bswap(cnt);
	TRACE_LOG("player cnt[%u %u]",p->id, cnt);
	init_cli_proto_head(pkgbuf, p, cli_proto_list_user, i);
	send_to_player(p, pkgbuf, i, 1);

    if (get_mapid_low32(p->cur_map->id) == 30) {
        TRACE_LOG("List pvp footprint user");
        int idx = sizeof(cli_proto_t);
        for (int i = 0; i < MAX_ARENA_NUM; i++) {
            pack(pkgbuf, footprint[i].left_id, idx);
            pack(pkgbuf, footprint[i].right_id, idx);
        }
        init_cli_proto_head(pkgbuf, p, cli_proto_list_pvp_point_user, idx);
        return send_to_player(p, pkgbuf, idx, 0);
    }
    return 0;
}

//---------------------------------------------------------------------

/**
  * @brief add player p to map newmap
  * @param p
  * @param newmap
  * @param oldma @return 0 on success, -1 on error
  */
int enter_map(player_t* p, map_id_t mapid, uint32_t x, uint32_t y)
{
	if ( mapid == 0 ) {
		ERROR_LOG("invalid operation: uid=%u curmap=%p maptype=%u mapid=%u",
			   p->id,p->cur_map,get_mapid_high32(mapid),get_mapid_low32(mapid));
		return -1;
	}

	//智力大比拼相关

	if ( p->cur_map ) {
		leave_map(p);
	}

	map_t* m = get_map(mapid);
	reset_common_action(p);
	if (!m) {
		ERROR_LOG("can't find map %lu uid=%u", (uint64_t)mapid, p->id);
		return -1;
	}

	if ( (x==0) && (y==0) ) {
		p->xpos = m->init_x;
		p->ypos = m->init_y;
	} else {
		p->xpos = x;
		p->ypos = y;
	}

	list_add_tail(&(p->maphook), &(m->playerlist));
	++(m->player_num);
	p->cur_map = m;

	send_entermap_rsp(p, 1);

	TRACE_LOG("player %u enters map %lu %u %u, playernum=%u ", p->id, (uint64_t)mapid, p->xpos, p->ypos, m->player_num);
	return 0;
}

/**
  * @brief remove player p from map m
  * @param p
  * @return 0 on success, -1 on error
  */
int leave_map(player_t* p)
{
	if (!p->cur_map) {
		ERROR_LOG("not in map: uid=%u cmd=%u", p->id, p->waitcmd);
		return -1;
	}


	uint32_t cur_map_id = get_mapid_low32(p->cur_map->id);
	if (cur_map_id >= 1004 && cur_map_id <= 1013) {
		player_leave_contest_room(p, cur_map_id);
	}

	map_t* m = p->cur_map;

	assert(m->player_num > 0);

	KDEBUG_LOG(p->id, "L M\t[uid=%u mid=%lu]", p->id, (uint64_t)m->id);

	// send leaving map response to client
	send_leave_map_rsp(p, 1);
	// remove player from the current map
	--(m->player_num);
	list_del(&p->maphook);
	p->cur_map = 0;

	TRACE_LOG("player %u leaves map %lu, playernum=%u", p->id, (uint64_t)m->id, m->player_num);
	return 0;
}

//---------------------------------------------------------------------

/**
  * @brief load map configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_maps
  */
int load_maps(xmlNodePtr cur)
{
	// init all_maps
	for (int i = 0; i != c_bucket_size; ++i) {
		INIT_LIST_HEAD(&all_maps[i]);
	}

	// load maps from xml file
	map_id_t mid;
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("Map"))) {
			// map id
			get_xml_prop(mid, cur, "ID");
			// return -1 if map already exists
			if (get_map(mid)) {
				ERROR_LOG("map %lu already exists", (uint64_t)mid);
				return -1;
			}
			// allocate a map
			map_t* m = alloc_map(mid);
			// map name
			get_xml_prop_raw_str(m->name, cur, "Name");
			get_xml_prop(m->init_x, cur, "InitX");
			get_xml_prop(m->init_y, cur, "InitY");

			// additional info about the map
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				//bool ret = true;
				//if (!xmlStrcmp(chl->name, (const xmlChar*)"Entries")) {
				//	ret = load_entries(m, chl);
				//}
			
				//if (!ret) {
				//	return -1;
				//}
				chl = chl->next;
			}
		}
		cur = cur->next;
	}

	return 0;
}

/**
  * @brief unload all the maps and free memory
  * @see load_maps
  */
void unload_maps()
{
	list_head_t* l;
	list_head_t* p;

	for (int i = 0; i != c_bucket_size; ++i) {
		list_for_each_safe(l, p, &all_maps[i]) {
			map_t* m = list_entry(l, map_t, hash_hook);
			dealloc_map(m);
		}
	}
}

/**
  * @brief return a pointer to a map which id equals to 'mid'
  * @param mid map id
  * @return pointer to the map if map with id equals to 'mid' is found, 0 otherwise
  */
map_t* get_map(map_id_t mid)
{
	map_t* m;

	list_for_each_entry(m, &all_maps[mid % c_bucket_size], hash_hook) {
		if (m->id == mid) {
			return m;
		}
	}

	return 0;
}

/**
  * @brief pack player p's info into buf and send it to other players
  * @param p p's info will be packed into buf
  */
void send_entermap_rsp(player_t* p, int complete)
{
	TRACE_LOG("S E M :%u", p->id);
	int idx = sizeof(cli_proto_t);

	idx += pack_entermap_rsp(p, pkgbuf + idx);

	init_cli_proto_head(pkgbuf, p, cli_proto_enter_map, idx);

	if(p->waitcmd != cli_proto_login) {
		send_to_map(p, pkgbuf, idx, complete);	
	} else {
		send_to_map_except_self(p, pkgbuf, idx);
	}
}

/**
  * @brief pack player p's info into buf and send it to other players
  * @param p p's info will be packed into buf
  */
void send_leave_map_rsp(player_t* p, int complete)
{
	// send leaving map response to client
	TRACE_LOG("S L M :%u", p->id);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_leave_map, idx);
	send_to_map(p, pkgbuf, idx, complete);
}

//-----------------------------------------------
// inline function definations
//-----------------------------------------------
inline bool is_player_visible(player_t* p)
{
	return !p->invisible;
}
// allocate and initialize a map
inline map_t* alloc_map(map_id_t mid)
{
	map_t* m = reinterpret_cast<map_t*>(g_slice_alloc0(sizeof(*m)));

	m->id = mid;
	// init player list
	INIT_LIST_HEAD(&m->playerlist);
	// add the newly created map to all_maps
	list_add_tail(&m->hash_hook, &all_maps[mid % c_bucket_size]);

	//TRACE_LOG("alloc map\t[%d]", mid);
	return m;
}

inline void dealloc_map(map_t* m)
{
	//TRACE_LOG("dealloc map %u %s", m->id, m->name);

	list_del(&m->hash_hook);
	g_slice_free1(sizeof(*m), m);
}

//-----------------------------------------------
// static function definations
//-----------------------------------------------

/**
  * @brief pack player p's info into buf
  * @param p p's info will be packed into buf
  * @param buf
  * @return number of bytes packed into buf
  */
static int pack_entermap_rsp(player_t* p, uint8_t* buf)
{
	int idx = 0;
	uint32_t lv = p->lv;
	pack(buf, p->id, idx);
	pack(buf, p->role_tm, idx);
	pack(buf, p->role_type, idx);
	pack(buf, p->power_user, idx);
	pack(buf, p->player_show_state, idx);
	pack(buf, p->vip, idx);
	pack(buf, is_vip_player(p) ? p->vip_level : 0, idx);
	pack(buf, p->using_achieve_title, idx);
	pack(buf, p->nick, max_nick_size, idx);
	pack(buf, lv, idx);
	pack(buf, p->flag, idx);
	pack(buf, p->honor, idx);
	pack(buf, p->xpos, idx);
	pack(buf, p->ypos, idx);
	pack(buf, p->app_mon, idx);
	
	switch(p->role_type)
	{
		case 1:
			pack(buf, 145, idx);
		break;
		
		case 2:
			pack(buf, 150, idx);
		break;

		case 3:
			pack(buf, 140, idx);
		break;

		case 4:
			pack(buf, 140, idx);
		break;

		default:
			pack(buf, 140, idx);
		break;
	}

	TRACE_LOG("%u %u %u %u", p->id, p->xpos, p->ypos, p->vip_level);
	idx += pack_player_clothes(p, p->waitcmd, buf + idx);
#ifdef DEV_SUMMON
	idx += pack_player_summon(p, buf + idx);
#endif
	return idx;
}

static std::map<uint32_t, intelligence_test_t> all_tests;

int load_all_intelligence_test(xmlNodePtr cur)
{
	all_tests.clear();

	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Question"))) {
			intelligence_test_t  test;
			get_xml_prop(test.test_id, cur, "ID");
			get_xml_prop(test.right_answer, cur, "Answer");
			get_xml_prop_def(test.reward_coins, cur, "Reward_coins", 0);
			get_xml_prop_def(test.reward_exp, cur, "Reward_exp", 0);

			TRACE_LOG("Load intellignece test [%u %u %u %u]", test.test_id, test.right_answer, test.reward_exp, test.reward_coins);

			all_tests.insert(std::map<uint32_t, intelligence_test_t>::value_type(test.test_id, test));

		}
		cur = cur->next;
	}
	return 0;
}

intelligence_test_t * get_intelligence_test_info_by_id(uint32_t test_id)
{
	std::map<uint32_t, intelligence_test_t>::iterator it = all_tests.find(test_id);
	if (it != all_tests.end()) {
		return &(it->second);
	}
	return NULL;
}

//update the intelligence test room info per seconds;
void intelligence_test_room::update(uint32_t now_time)
{
	uint32_t intelligence_prepare_id = 3;
	if (get_now_active_data_by_active_id(intelligence_prepare_id)) {
		this->do_prepare_logic(now_time);
		return;
	} else {
		test_start_prepare_tm = 0;
	} 

	uint32_t intelligence_going_id = 4;
	if (get_now_active_data_by_active_id(intelligence_going_id)) {
		this->do_going_logic(now_time);
		return;
	} else {
		test_start_tm = 0;
	}
}

void intelligence_test_room::do_prepare_logic(uint32_t now_time)
{
    char words[max_trade_mcast_size] = {0};
	uint32_t now = get_now_tv()->tv_sec;

	static uint32_t last_mcast_time = 0;

	if (test_start_prepare_tm == 0) {
		test_start_prepare_tm = now;
		last_mcast_time = 0;
		time_mcast_time = 0;
	}

    /*
	if (last_mcast_time == 0) {
		snprintf(words, max_trade_mcast_size, "%s", char_content[13].content);
    	notify_official_mcast_info(0, mcast_world_notice, words);
		last_mcast_time = now;
	} else if (last_mcast_time + 600 < now) {
		snprintf(words, max_trade_mcast_size, "%s", char_content[14].content);
    	notify_official_mcast_info(0, mcast_world_notice, words);
		last_mcast_time = now;
	}*/

	if (time_mcast_time + 20 < now) {
		time_mcast_time = now;
		uint32_t limit_time = 15 * 60 - (now - test_start_prepare_tm) + 10;
		map_t * m = this->cur_map;
	//	ERROR_LOG("PUZZLE LIMIT TIME %u", limit_time);

		list_head_t* l;
		list_for_each (l, &m->playerlist) { 
			player_t* player = list_entry(l, player_t, maphook);
			if (!player->puzzle_data->kick_flag) {
				int idx = sizeof(cli_proto_t);
				pack(pkgbuf, limit_time, idx);
				
				init_cli_proto_head(pkgbuf, player, cli_proto_10_second_msg, idx);
				send_to_player(player, pkgbuf, idx, 0);
			}
		}

	}

}


void intelligence_test_room::do_going_logic(uint32_t now_time)
{
	if (!test_start_tm) {

		//init puzzle info 
		test_start_tm = now_time;
		cur_question = 0;
		cur_answer = 0;
		this->rand_test_2_going();
		map_t * m = this->cur_map;

		list_head_t* l;
		list_for_each (l, &m->playerlist) { 
			player_t* player = list_entry(l, player_t, maphook);
			if (!player->puzzle_data->kick_flag) {
				int idx = sizeof(cli_proto_t);
				pack(pkgbuf, 10, idx);
				init_cli_proto_head(pkgbuf, player, cli_proto_10_second_msg, idx);
				send_to_player(player, pkgbuf, idx, 0);
			}
		}
	}

	uint32_t pass_time = now_time - test_start_tm;

	if (pass_time <= 10) {
		return;
	} 

	pass_time -= 10;

	uint32_t cir_time = in_cir_per_time_part1 + in_cir_per_time_part2;
	uint32_t cir_cnt = pass_time / cir_time; 

	if (cir_cnt < in_total_cir_count) {
		if (((pass_time % cir_time) >= 1) && (cur_question < (cir_cnt + 1))) { //出题
			cur_question ++;
			int idx = sizeof(cli_proto_t);
			pack(pkgbuf, (cir_cnt + 1), idx);
			pack(pkgbuf, (this->intelligence_tests[cir_cnt])->test_id, idx);
			init_cli_proto_head(pkgbuf, 0, cli_proto_question_info, idx);
			do_send_to_map(this->cur_map, pkgbuf, idx);

		} 

		if (pass_time % cir_time >= (cir_time - 2) && (cur_answer < (cir_cnt+ 1))) { //答题判断


			//ERROR_LOG("PUZZLE CIR CUR_ANSER[%u] ANSWER [%u]", cur_answer, cir_cnt);

			cur_answer++;
			map_t * m = this->cur_map;

			list_head_t* l;
			list_for_each (l, &m->playerlist) {

				player_t* player = list_entry(l, player_t, maphook);
				if (!player->puzzle_data->kick_flag) {
					uint32_t player_answer = 0;
					//get user answer
					if (player->xpos < 1600 && player->xpos > 1000 &&
						 player->ypos < 543 && player->ypos > 200) {
						player_answer = 1;
					} else if (player->xpos < 2500 && player->xpos > 1800 && 
						 player->ypos < 950 && player->ypos > 550) {
						player_answer = 2;
					} else if (player->xpos < 1400 && player->xpos > 700 &&
						player->ypos < 1343 && player->ypos > 900) {
						player_answer = 3;
					} else if (player->xpos < 700 && player->xpos > 50 && 
							player->ypos < 850 && player->ypos > 450) {
						player_answer = 4;
					}

					uint32_t add_coins = 0;
					uint32_t add_exp =0;

					//calc exp and coins reward
					add_coins = player->lv * 20;
					add_exp = player->lv * 300;

					if (cir_cnt == in_total_cir_count - 1) {
						add_coins += 10000;
						add_exp += 10000;
					}
					intelligence_test_t * test = (this->intelligence_tests[cir_cnt]);
					//judge for kick
					if (player_answer != (this->intelligence_tests[cir_cnt])->right_answer) {
						DEBUG_LOG("USER ANSWER ERROR [%u %u %u %u]",
								player->id, test->test_id, test->right_answer, player_answer);

						//book for wrong answer
						if (player->puzzle_data->book_set_flag) {
							uint32_t item = 1500572;
							if (player->my_packs->get_item_cnt(item)) {
								DEBUG_LOG("USER PUZZLE TEST WRONG USE ITEM [%u %u %u %u]",
									player->id, test->test_id, test->right_answer, player_answer);

								db_use_item_ex(0, player->id, player->role_tm, item, 1, true);
								player->my_packs->del_item(player, item, 1, channel_string_other);	
							} else {
								player->puzzle_data->kick_flag = 1;
								player->puzzle_data->book_set_flag = 0;
								add_exp = 0;
								add_coins = 0;
							}
						} else {
								player->puzzle_data->kick_flag = 1;
								player->puzzle_data->book_set_flag = 0;
								add_exp = 0;
								add_coins = 0;
						}
					} else {
						DEBUG_LOG("USER ANSWER OK [%u %u %u %u]",
								player->id, test->test_id, test->right_answer, player_answer);

					}

					

					// add exp and coins then set to db
					if (player->lv >= max_exp_lv) {
						add_exp = 0;
					}
					player->coins += add_coins;
					player->exp += add_exp;
					calc_player_level(player);
					db_set_role_base_info(player);

					//send answer info to client
					int idx = sizeof(cli_proto_t);
					pack(pkgbuf, (cir_cnt + 1), idx);
					pack(pkgbuf, (this->intelligence_tests[cir_cnt])->test_id, idx);
					pack(pkgbuf, (this->intelligence_tests[cir_cnt])->right_answer, idx);
					pack(pkgbuf, player_answer, idx);
					pack(pkgbuf, add_exp, idx);
					pack(pkgbuf, add_coins, idx);
					init_cli_proto_head(pkgbuf, player, cli_proto_cur_question_answer_info, idx);
					send_to_player(player, pkgbuf, idx, 0);

					//all cir over init info
					if (cir_cnt == (in_total_cir_count - 1)) {
						player->puzzle_data->book_set_flag = 0;
						player->puzzle_data->right_cir = cir_cnt + 1;
						player->puzzle_data->kick_flag = 1;
					}

				}

			}
		}
	}
}

//前n-1个数中随机选取m个
void  ran_pos(int * ptr, int n, int m) 
{
	int k = 0;
	int j = 0;
	for (int i = 0; i < n; i++) {
		k = ptr[i];
		j = rand()%(n - i) + i;
		ptr[i] = ptr[j];
		ptr[j] = k;
	}
}

void intelligence_test_room::rand_test_2_going()
{
	int info[500] = {};
	for (int i = 0; i < 500; i++) {
		info[i] = i;
	}

	//ERROR_LOG("PUZZLE %u RAN FROM %u QUESTION ", in_total_cir_count, all_tests.size()); 

	ran_pos(info, all_tests.size(), in_total_cir_count);
	intelligence_tests.clear();
	for (int i = 0; i < in_total_cir_count; i ++) {
	//	ERROR_LOG("RAN PUZZLE FORM INFO i==%u", info[i]);
		intelligence_test_t * test = get_intelligence_test_info_by_id(info[i] + 1);
	//	ERROR_LOG("PUZZLE RAN QUEST %u", test->test_id);
		intelligence_tests.push_back(test);
	}
}

static std::map<uint32_t, intelligence_test_room*> test_rooms;
static int last_update_tm;

void init_intelligence_test_room()
{
	uint32_t base_test_map_id = 43;


	for (int i = 0; i < 10 ; i++) {
		intelligence_test_room * room = new intelligence_test_room();
		room->cur_map = get_map(make_mapid(0, base_test_map_id + i));
		room->room_id = i + 1;
		test_rooms.insert(std::map<uint32_t, intelligence_test_room *>::value_type(room->room_id, room));
	}
}


void proc_intelligence_test_event()
{
	if (last_update_tm == 0) {
		last_update_tm = get_now_tv()->tv_sec;
		init_intelligence_test_room();
	}

	if (last_update_tm == get_now_tv()->tv_sec) { 
		return;
	} else {
		last_update_tm = get_now_tv()->tv_sec;
	}

	for (std::map<uint32_t, intelligence_test_room*>::iterator it = test_rooms.begin();
			it != test_rooms.end(); ++it) {
			(it->second)->update(get_now_tv()->tv_sec);
		//	ERROR_LOG("PUZZLE UPDATE ROOM ID %u", (it->second)->room_id);
	}
}

//list all room status
int list_test_room_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);
	int room_cnt = test_rooms.size();
	pack(pkgbuf, room_cnt, idx);

	uint32_t room_flag1_ = 0;
	uint32_t room_flag2_ = 0;
	if (get_now_active_data_by_active_id(3)) {
		room_flag1_ = 1;
	}

	if (get_now_active_data_by_active_id(4)) {
		room_flag2_ = 1;
	}

	uint32_t room_status;
	for (std::map<uint32_t, intelligence_test_room*>::iterator it = test_rooms.begin();
			it != test_rooms.end(); ++it) {
		intelligence_test_room * room = (it->second);
		pack(pkgbuf, room->room_id, idx);
		if (room_flag2_) {
			room_status = 3;
			pack(pkgbuf, room_status, idx);
			continue;
		} 

		if (!room_flag1_ && !room_flag2_) {
			room_status = 0;
			pack(pkgbuf, room_status, idx);
			continue;
		}

		if (room->cur_map->player_num > 30) {
			room_status = 2;
			pack(pkgbuf, room_status, idx);
			continue;
		}

		room_status = 1;
		pack(pkgbuf, room_status, idx);
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int set_book_for_fault_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t book_item = 1500572;
	uint32_t set_succ = 0;
	if (p->my_packs->get_item_cnt(book_item)) {
	//	ERROR_LOG("PLAYER SET BOOK FOR PUZZLE %u SUCC", p->id);
		p->puzzle_data->book_set_flag = 1;
		set_succ = 1;
	}
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, set_succ, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

//get a single room status for aim enter
int try_enter_test_room_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t room_id = 0;
	int idx = 0;
	unpack(body, room_id, idx);
//	ERROR_LOG("TRY GET PUZZLE ROOM %u %u", p->id, room_id);
	std::map<uint32_t, intelligence_test_room*>::iterator it = test_rooms.find(room_id);
	if (it == test_rooms.end()) {
		return -1;
	}

	uint32_t room_flag1_ = 0; //for prepare time flag
	uint32_t room_flag2_ = 0; //for going time flag
	uint32_t room_status = 0; 

	if (get_now_active_data_by_active_id(3)) {
		room_flag1_ = 1;
	}

	if (get_now_active_data_by_active_id(4)) {
		room_flag2_ = 1;
	}


	idx = sizeof(cli_proto_t);
	intelligence_test_room * room = (it->second);
	pack(pkgbuf, room->room_id, idx);
	if (room_flag2_) { 
		room_status = 3; //starting can't in
	} else if (!room_flag1_ && !room_flag2_) {
		room_status = 0; //not in active time
	} else if (room_flag1_ && room->cur_map->player_num > 30) {
		room_status = 2;
	} else {
		room_status = 1;
	}
	pack(pkgbuf, room_status, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

