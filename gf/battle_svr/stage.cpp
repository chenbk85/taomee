#include <libtaomee++/conf_parser/xmlparser.hpp>

extern "C" {
#include <dirent.h>

#include <libtaomee/log.h>
}

using namespace taomee;

#include "stage.hpp"
#include "battle.hpp"

static stage_t all_stages[max_stage_type][stage_max_num];
static punish_info_t punish_stage_arr[stage_max_num + 1];
//--------------------------------------------------------

bool is_punish_stage(uint32_t stageid)
{
	return stageid < stage_max_num && stageid == punish_stage_arr[stageid - 1].stage_id ? true : false;
}

uint32_t get_duration_punish_percent(uint32_t stageid)
{
	return punish_stage_arr[stageid - 1].duration_per;
}

uint32_t get_exp_punish_percent(uint32_t stageid)
{
	return punish_stage_arr[stageid - 1].exp_per;
}


const stage_t* get_stage(uint16_t sid, uint32_t stage_mode)
{
	stage_mode --;
	if ((sid < stage_max_num) && (stage_mode < max_stage_type) && all_stages[stage_mode][sid].id) {
		return &all_stages[stage_mode][sid];
	}
	return 0;
}

void map_t::set_monster_dead(uint32_t mon_type)
{
	PlayerSet::iterator it = monsters.begin();
	while (it != monsters.end()) {
		if ( (*it)->role_type == mon_type) {
			(*it)->suicide();
		}
		++it;
	} 
}


//--------------------------------------------------------

static int load_more_mapinfo(xmlNodePtr cur, map_t* m, stage_t* stg, uint32_t stage_mode)
{
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("ToMaps"))) {
			xmlNodePtr node = cur->xmlChildrenNode;
			while (node) {
				int id;
				uint32_t mid;
				uint16_t stg_id;
				if (!xmlStrcmp(node->name, reinterpret_cast<const xmlChar*>("Map"))) {
					get_xml_prop(mid, node, "ID");

					stg_id = get_stage_id(mid);
					id     = get_stage_map_id(mid);
					if ((id != -1) && (stg_id == stg->id)) {
						m->to[id].id = id + 1;
						
						get_xml_prop(m->to[id].init_x, node, "InitX");
						get_xml_prop(m->to[id].init_y, node, "InitY");
						TRACE_LOG("%u %u %u %u %u", stage_mode, stg_id, id, m->to[id].init_x, m->to[id].init_y);
					} else {
						ERROR_LOG("invalid to map id: %u %u at stage %u", m->id, mid, stg->id);
						return -1;
					}
				}
				node = node->next;
			}
		} else if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Monsters"))) {
			xmlNodePtr node = cur->xmlChildrenNode;
			while (node) {
				if (!xmlStrcmp(node->name, reinterpret_cast<const xmlChar*>("Monster"))) {
					map_monster_t mon;
					get_xml_prop(mon.id, node, "ID");
					const monster_t* moninfo = get_monster(mon.id, stage_mode);
					if (moninfo == 0) {
						ERROR_LOG("invalid monster id: mon=%u map=%u stage_mode=%u", mon.id, m->id, stage_mode);
						return -1;
					}
					if (moninfo->type == 1) {
						stg->boss_id = moninfo->id;
					}
					if (stg->max_lv < moninfo->lv) {
						stg->max_lv = moninfo->lv;
					}
					get_xml_prop(mon.x, node, "X");
					get_xml_prop(mon.y, node, "Y");

					uint32_t birth_flg = 0;
					uint32_t call_flg = 0;
					get_xml_prop_def(birth_flg, node, "RealTimeBirth", 0);
					get_xml_prop_def(call_flg, node, "CallFlag", 0);
					mon.rt_birth_flg = birth_flg;
					mon.call_flg = call_flg;
					if (mon.rt_birth_flg) {
						m->rt_birth_mons->push_back(mon);
						TRACE_LOG("rt_birth_mons in %u %u %u", mon.id, mon.rt_birth_flg, birth_flg);
					} else {
						m->mons->push_back(mon);
					}

					stg->mon_set.insert(moninfo);
				}
				node = node->next;
			}
		} else if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("RMonsters"))) {
			xmlNodePtr node = cur->xmlChildrenNode;
			while (node) {
				if (!xmlStrcmp(node->name, reinterpret_cast<const xmlChar*>("Monster"))) {
					rand_monster_t mon;
					get_xml_prop(mon.id, node, "ID");
					const monster_t* moninfo = get_monster(mon.id, stage_mode);
					if (moninfo == 0) {
						ERROR_LOG("invalid monster id: mon=%u map=%u stage_mode=%u", mon.id, m->id, stage_mode);
						return -1;
					}

					get_xml_prop_arr_def(mon.x_aur, node, "A_X", 0);
					get_xml_prop_arr_def(mon.y_aur, node, "A_Y", 0);

					if ((mon.x_aur[0] > mon.x_aur[1]) || 
							(mon.y_aur[0] > mon.y_aur[1])) {
						ERROR_LOG("invalid rand monster pos %u", mon.id);
						return -1;
					}

					get_xml_prop(mon.odds, node, "Odds");

					/*
					if (moninfo->type == 1) {
						stg->boss_id = moninfo->id;
					}*/

					if (stg->max_lv < moninfo->lv) {
						stg->max_lv = moninfo->lv;
					}

					m->rand_mons->push_back(mon);

				//	stg->mon_set.insert(moninfo);
				}
				node = node->next;
			}

		}
		cur = cur->next;
	}

	return 0;
}

/**
  * @brief load btlmap configs from an xml file
  * @return 0 on success, -1 on error
  */
static int load_maps(xmlNodePtr cur, stage_t* stg, uint32_t stage_mode)
{
	// load maps from xml file
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Map"))) {
			// Map ID
			int mid, id, sid;
			get_xml_prop_def(mid, cur, "ID", 0);
			id  = get_stage_map_id(mid);
			sid = get_stage_id(mid);
			if ((id == -1) || (sid == -1) || (sid != stg->id)) {
				ERROR_LOG("invalid map id: %d %d %d at stage %u", mid, sid, id, stg->id);
				return -1;
			}

			if (id != stg->map_num) {
				//ERROR_LOG("map id should be filled in orderly but now ignor it: %d %d %d %d", mid, sid, id, stg->map_num);
				//return -1;
			}

			map_t* m = &(stg->maps[id]);
			if (m->id) {
				ERROR_LOG("duplicate map id: %d %d %d at stage %u", mid, sid, id, stg->id);
				return -1;
			}
			m->id      = mid;
			m->to      = new tomaps_t[stage_max_map_num];
			m->mons    = new map_t::MapMonVec;
			m->rt_birth_mons    = new map_t::MapMonVec;
			m->rand_mons = new map_t::RanMonVec;

			//load map's path
			char path_filename[30] ={0};
			//sprintf(path_filename, "./data/maps/%7d.txt", m->id);
			snprintf(path_filename, sizeof(path_filename), "./data/maps/%7d.txt", m->id);
			TRACE_LOG("load map path[%u %s]",m->id, path_filename);
			m->path = new KfAstar(path_filename);
			if ( !*(m->path) ) {
				ERROR_LOG("cant load path[%u %s]", m->id, path_filename);
				return -1;
			}
			
			// load more configs
			if (load_more_mapinfo(cur->xmlChildrenNode, m, stg, stage_mode) == -1) {
				return -1;
			}

			++(stg->map_num);
		}
		cur = cur->next;
	}

	return 0;
}


/**
  * @brief load stage configs from an xml file
  * @return 0 on success, -1 on error
  */
static int load_stage(xmlNodePtr cur, uint32_t stage_mode)
{
	if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Stage"))) {
		// Stage ID
		uint32_t id;
		get_xml_prop(id, cur, "ID");
		if (id >= stage_max_num) {
			ERROR_LOG("invalid stage id: %u", id);
			return -1;
		}
		stage_t* s = &(all_stages[stage_mode - 1][id]);
		if (s->id) {
			ERROR_LOG("duplicate stage id: %u", id);
			return -1;
		}
		s->id = id;

		size_t n = get_xml_prop_arr(s->lv, cur, "Lv");
		if (n != 2) {
			ERROR_LOG("invalid level config: sid=%u lv_num=%u", id, (uint32_t)n);
			return -1;
		}

		get_xml_prop(s->init_x, cur, "InitX");
		get_xml_prop(s->init_y, cur, "InitY");
		get_xml_prop_def(s->sec_x, cur, "SecX", 0);
		get_xml_prop_def(s->sec_y, cur, "SecY", 0);
		uint32_t init_mapid = 0;
		get_xml_prop(init_mapid, cur, "InitMapID");
		s->init_map_id  = get_stage_map_id(init_mapid);
		if (s->init_map_id == -1) {
			ERROR_LOG("invalid init map id: %u at stage %u", init_mapid, s->id);
			return -1;
		}

		if (load_maps(cur->xmlChildrenNode, s, stage_mode) == -1) {
			ERROR_LOG("load_maps error");
			return -1;
		}
		TRACE_LOG("max level %u %u", s->id, s->max_lv);
	}

	return 0;
}


static int load_stage_easy(xmlNodePtr cur) {
	return load_stage(cur, stage_mode_easy);
}

static int load_stage_nor(xmlNodePtr cur) {
	return load_stage(cur, stage_mode_nor);
}

static int load_stage_hard(xmlNodePtr cur) {
	return load_stage(cur, stage_mode_hard);
}

static int load_stage_hell(xmlNodePtr cur) {
	return load_stage(cur, stage_mode_hell);
}
static int load_stage_tower(xmlNodePtr cur) {
	return load_stage(cur, stage_mode_tower);
}

static int load_stage_epic(xmlNodePtr cur) {
	return load_stage(cur, stage_mode_epic);
}

static int load_stage_legend(xmlNodePtr cur) {
	return load_stage(cur, stage_mode_legend);
}



void load_all_stages()
{
	// since we need to reload stage.xml at run time, so we need to init 'all_stages'
	for (int i = 0; i != max_stage_type; ++i) {
		for (int j = 0; j != stage_max_num; ++j) {
			all_stages[i][j].init();
		}
	}

	load_stages(stage_mode_easy);
	load_stages(stage_mode_nor);
	load_stages(stage_mode_hard);
	load_stages(stage_mode_hell);
	load_stages(stage_mode_tower);
	load_stages(stage_mode_epic);
	load_stages(stage_mode_legend);

	TRACE_LOG("============= Start Dumping Stages =============\n");
	for (int n = 0; n != max_stage_type; ++n) {
		for (int i = 0; i != stage_max_num; ++i) {
			TRACE_LOG("=stage=%u stage: id=%u lv=%u %u x=[%u %u] y=[%u %u] map_num=%u begin ****", n, i,
                all_stages[n][i].lv[0], all_stages[n][i].lv[1], all_stages[n][i].init_x, all_stages[n][i].sec_x, 
                all_stages[n][i].init_y, all_stages[n][i].sec_y, all_stages[n][i].map_num);
			if (all_stages[n][i].map_num) {
				TRACE_LOG("****diff=%u stage: id=%u lv=%u %u x=%u y=%u map_num=%u begin ****", n, i, all_stages[n][i].lv[0], all_stages[n][i].lv[1],
							all_stages[n][i].init_x, all_stages[n][i].init_y, all_stages[n][i].map_num);
			}

			for (int j = 0; j != stage_max_map_num; ++j) {
				
				map_t* m = &(all_stages[n][i].maps[j]);
				if (m->id) {
					TRACE_LOG("== map id: %u - %u", i, m->id);
					for (int i = 0; i != stage_max_map_num; ++i) {
						tomaps_t* to = &(m->to[i]);
						if (to->id) {
							TRACE_LOG("==== to map: %u %u %u", to->id, to->init_x, to->init_y);
						}
					}
					for (map_t::MapMonVec::iterator it = m->mons->begin(); it != m->mons->end(); ++it) {
						TRACE_LOG("==== mons: %u %u %u", it->id, it->x, it->y);
					}
				}
			}

			if (all_stages[n][i].map_num) {
				TRACE_LOG("**** stage: id=%d  end ****\n", i);
			}
		}
	}
	TRACE_LOG("============= End Dumping Stages =============\n");

}

void load_stages(uint32_t stage_mode)
{
	DIR* dir = 0;
	if (stage_mode == stage_mode_easy) {
		dir = opendir("./conf/stage_easy/stages");

		dirent* dentry;
		while ((dentry = readdir(dir))) {

			if (strstr(dentry->d_name, ".xml") && dentry->d_name[0] != '.') {
				char path[256] = "./conf/stage_easy/stages/";
				strncat(path, dentry->d_name, sizeof(path) - strlen(path) - 1);
				load_xmlconf(path, load_stage_easy);
			}
		}
	} else if (stage_mode == stage_mode_nor) {
		dir = opendir("./conf/stage_normal/stages");

		dirent* dentry;
		while ((dentry = readdir(dir))) {
			if (strstr(dentry->d_name, ".xml") && dentry->d_name[0] != '.') {
				char path[256] = "./conf/stage_normal/stages/";
				strncat(path, dentry->d_name, sizeof(path) - strlen(path) - 1);
				load_xmlconf(path, load_stage_nor);
			}
		}

	} else if (stage_mode == stage_mode_hard) {
		dir = opendir("./conf/stage_hard/stages");

		dirent* dentry;
		while ((dentry = readdir(dir))) {

			if (strstr(dentry->d_name, ".xml") && dentry->d_name[0] != '.') {
				char path[256] = "./conf/stage_hard/stages/";
				strncat(path, dentry->d_name, sizeof(path) - strlen(path) - 1);
				load_xmlconf(path, load_stage_hard);
			}
		}

	} else if (stage_mode == stage_mode_hell) {
		dir = opendir("./conf/stage_hell/stages");

		dirent* dentry;
		while ((dentry = readdir(dir))) {

			if (strstr(dentry->d_name, ".xml") && dentry->d_name[0] != '.') {
				char path[256] = "./conf/stage_hell/stages/";
				strncat(path, dentry->d_name, sizeof(path) - strlen(path) - 1);
				load_xmlconf(path, load_stage_hell);
			}
		}

	} else if (stage_mode == stage_mode_tower) {
		dir = opendir("./conf/stage_tower/stages");

		dirent* dentry;
		while ((dentry = readdir(dir))) {

			if (strstr(dentry->d_name, ".xml") && dentry->d_name[0] != '.') {
				char path[256] = "./conf/stage_tower/stages/";
				strncat(path, dentry->d_name, sizeof(path) - strlen(path) - 1);
				load_xmlconf(path, load_stage_tower);
			}
		}

	} else if (stage_mode == stage_mode_epic) {
		dir = opendir("./conf/stage_epic/stages");

		dirent* dentry;
		while ((dentry = readdir(dir))) {

			if (strstr(dentry->d_name, ".xml") && dentry->d_name[0] != '.') {
				char path[256] = "./conf/stage_epic/stages/";
				strncat(path, dentry->d_name, sizeof(path) - strlen(path) - 1);
				load_xmlconf(path, load_stage_epic);
			}
		}

	} else if (stage_mode == stage_mode_legend) {
		dir = opendir("./conf/stage_legend/stages");

		dirent* dentry;
		while ((dentry = readdir(dir))) {
			if (strstr(dentry->d_name, ".xml") && dentry->d_name[0] != '.') {
				char path[256] = "./conf/stage_legend/stages/";
				strncat(path, dentry->d_name, sizeof(path) - strlen(path) - 1);
				load_xmlconf(path, load_stage_legend);
			}
		}

	}
	closedir(dir);
}

void unload_stages()
{
	for (int i = 0; i != max_stage_type; ++i) {
		for (int j = 0; j != stage_max_num; ++j) {
			stage_t* s = &(all_stages[i][j]);
			if (s->id) {
				// TODO: nothing to do now
			}
		}
	}

}

int load_punish_stage(xmlNodePtr cur)
{
	TRACE_LOG("in");
	if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Punish"))) {
		xmlNodePtr cur_chd = cur->xmlChildrenNode;
		while (cur_chd) {
			if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("Stage"))) {
				// Stage ID
				uint32_t id;
				get_xml_prop(id, cur_chd, "ID");
				if (id >= stage_max_num || punish_stage_arr[id - 1].stage_id) {
					ERROR_LOG("invalid stage id: %u", id);
					return -1;
				}
				punish_stage_arr[id - 1].stage_id = id;

				uint32_t protect_lv, exp_punish, duration_punish;
				get_xml_prop(protect_lv, cur_chd, "ProtectLevel");
				get_xml_prop(exp_punish, cur_chd, "ExpPunish");
				get_xml_prop(duration_punish, cur_chd, "DurationPunish");

				punish_stage_arr[id - 1].protect_lv = protect_lv;
				punish_stage_arr[id - 1].exp_per = exp_punish;
				punish_stage_arr[id - 1].duration_per = duration_punish;

				TRACE_LOG("%u %u %u %u", punish_stage_arr[id - 1].stage_id, punish_stage_arr[id - 1].protect_lv,
					punish_stage_arr[id - 1].exp_per, punish_stage_arr[id - 1].duration_per);
			}
			cur_chd = cur_chd->next;
		}
	}
	return 0;
}


void map_t::send_players_status_info(Player* p, bool except_self)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	int idx2 = idx;
	int total_count = 0;
	pack(pkgbuf, total_count, idx);

	for(PlayerSet::iterator it = players.begin(); it != players.end(); ++it){
		Player* p1 = *it;
		if (except_self) {
			if( p1 == p)continue;
		}
		idx += p1->pack_status_info(pkgbuf + idx);
		total_count++;
		if (p1->my_summon) {
			idx += p1->my_summon->pack_status_info(pkgbuf + idx);
			total_count++;
		}
	}
	for(PlayerSet::iterator it2 = monsters.begin(); it2 != monsters.end(); ++it2){
		Player* p2 = *it2;
		if (except_self) {
			if(p2 == p)continue;
		}
		idx += p2->pack_status_info(pkgbuf + idx);
		total_count++;
	}

	pack(pkgbuf, total_count, idx2);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_players_status_info, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_player(p, pkgbuf, idx, 0);
}


static std::map<uint32_t, challenge_t *> all_challenges;


const challenge_t  * get_challenge(const uint32_t stage_id)
{
	typedef std::map<uint32_t, challenge_t*>::iterator Iter;
	Iter pIter = all_challenges.find(stage_id);
	if (pIter != all_challenges.end()) {
		return pIter->second;
	}

	return 0;
		
}

int load_challenges(xmlNodePtr cur)
{
	if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("DStages"))) {
		cur = cur->xmlChildrenNode;
		while (cur) {
			if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Challenge"))) { 
				challenge_t * challenge = new challenge_t();
				get_xml_prop(challenge->stage_id, cur, "Stage");
				xmlNodePtr diff_node = cur->xmlChildrenNode;
				while (diff_node) {
					if (!xmlStrcmp(diff_node->name, reinterpret_cast<const xmlChar*>("Diff"))) {
						int i = 0;
						get_xml_prop(i, diff_node, "Diff");
						if (i > max_diff_cnt) {
							throw XmlParseError(std::string("diff fata!"));
						}
						get_xml_prop_def(challenge->challenges[i - 1].cool_dec, 
								    	diff_node, 
										"CD_Dec",
									   	0); 
						get_xml_prop_def(challenge->challenges[i - 1].hp_add,
										diff_node,
										"HP_Add",
										0);

						get_xml_prop_def(challenge->challenges[i - 1].atk_add,
										diff_node,
										"Atk_Add",
										0);
						get_xml_prop_def(challenge->challenges[i - 1].drop_add,
								        diff_node,
										"Drop_odds",
										0);
						get_xml_prop_def(challenge->challenges[i - 1].lv_add,
										diff_node,
										"Lv_Add",
										0);
						get_xml_prop_def(challenge->challenges[i - 1].low_cnt,
										diff_node,
										"LowAI",
										0);
						get_xml_prop_def(challenge->challenges[i - 1].mid_cnt,
										diff_node,
										"MiddleAI",
										0);
						get_xml_prop_def(challenge->challenges[i - 1].high_cnt,
										diff_node,
										"HighAI",
										0);
						get_xml_prop_def(challenge->challenges[i - 1].boss_cnt,
										diff_node,
										"BossAI",
										0);
						TRACE_LOG("LOAD Challenges stage %u diff %u hp %u atk %u drop %u",
								challenge->stage_id, i,  challenge->challenges[i - 1].atk_add,
								challenge->challenges[i - 1].hp_add, challenge->challenges[i - 1].drop_add);

					}
					diff_node = diff_node->next;
				}
			
				all_challenges.insert(std::map<uint32_t, challenge_t*>::value_type(challenge->stage_id, challenge));
			}
			cur = cur->next;
		}
	}

//	TRACE_LOG("Load CH OVER!");
	return 0;
}





