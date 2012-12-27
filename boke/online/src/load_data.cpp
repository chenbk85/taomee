/*
 * =========================================================================
 *
 *        Filename: load_data.cpp
 *
 *        Version:  1.0
 *        Created:  2011-05-18 13:33:54
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#include    "load_data.h"
#include    "Csprite.h"
#include	"Cgame.h"
#include    "global.h"
#include    "Cmap_conf.h"

int load_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node))
{
	int err = -1;

	xmlDocPtr doc = xmlReadFile(file,NULL, XML_PARSE_NOBLANKS);
	if (!doc) {
		ERROR_RETURN(("Failed to Load %s", file), -1);
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc); 
	if (!cur) {
		KERROR_LOG(0, "xmlDocGetRootElement error");
		goto fail;
	}

	err = parser(cur);
fail:
	xmlFreeDoc(doc);
	BOOT_LOG(err, "==Load File %s", file);
}

int load_jf_shop(xmlNodePtr cur)
{
  	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"item")) {
			uint32_t itemid, pay;
			DECODE_XML_PROP_INT(itemid, cur, "id");
			DECODE_XML_PROP_INT(pay, cur, "pay");
			source_conf_t* p_source_conf = g_item_conf_map.get_item_info(itemid);
			if (p_source_conf) {
				p_source_conf->gamept = pay;
			}
		}
		cur = cur->next;
	}	
	return 0;
}

int load_item_conf_map(xmlNodePtr cur){
	item_card_map.clear();
  	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"kind")) {
			uint32_t kindid=0;
			DECODE_XML_PROP_INT(kindid, cur, "id");
			xmlNodePtr chl = cur->xmlChildrenNode;
		  	while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar *)"item")) {
					const xmlChar * buf;
					item_conf_t item_conf ;
					DECODE_XML_PROP_INT_DEFAULT(item_conf.itemid, chl , "id",0);
					DECODE_XML_PROP_INT_DEFAULT(item_conf.change, chl , "change",0);
					DECODE_XML_PROP_DEFAULT(chl , "island", buf);
 					init_int_list((const char*)buf,item_conf.islandid_list);
					DECODE_XML_PROP_INT_DEFAULT(item_conf.clothtype,chl,"clothtype",0);
					DECODE_XML_PROP_INT_DEFAULT(item_conf.type,chl,"type",0);
					DECODE_XML_PROP_INT_DEFAULT(item_conf.source,chl,"source",0);
					DECODE_XML_PROP_INT_DEFAULT(item_conf.pay,chl,"pay",0);
					std::vector<uint32_t> contain_list;
					DECODE_XML_PROP_DEFAULT(chl , "contain", buf);
 					init_int_list((const char*)buf, contain_list);
					for (uint32_t loop = 0; loop < contain_list.size(); loop ++) {
						item_card_map[contain_list[loop]].insert(item_conf.itemid);
					}

					DECODE_XML_PROP_DEFAULT(chl , "comprise", buf);
 					init_int_list((const char*)buf,item_conf.comprise_itemid_list);

					if (item_conf.itemid>0 ){
						g_item_conf_map.add(kindid,item_conf);
					}
				}
				chl= chl->next;	
			}
		}
		cur = cur->next;	

		// 特殊物品的一些限制
		item_conf_t item_conf;
		item_conf.itemid = spec_item_game_point;
		item_conf.total_max = MAX_SPEC_ITEM_COUNT;
		item_conf.day_max = 1000;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid= spec_item_day_oltime;
		item_conf.total_max = -1;
		item_conf.day_max = -1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_experience;
		item_conf.total_max = -1;
		item_conf.day_max = -1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_pet_egg_count;
		item_conf.total_max = 5;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_login_count;
		item_conf.total_max = 20;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_chest_get;
		item_conf.total_max = -1;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_login_reward_1;
		item_conf.total_max = 1;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_login_reward_2;
		item_conf.total_max = 1;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_login_reward_3;
		item_conf.total_max = 1;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_login_reward_5;
		item_conf.total_max = 1;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_login_reward_10;
		item_conf.total_max = 1;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_login_reward_15;
		item_conf.total_max = 1;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_login_reward_20;
		item_conf.total_max = 1;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid = spec_item_login_reward_0;
		item_conf.total_max = 1;
		item_conf.day_max = 1;
		g_item_conf_map.add(0, item_conf);

		item_conf.itemid= spec_item_game_point_used;
		item_conf.total_max = -1;
		item_conf.day_max = -1;
		g_item_conf_map.add(0, item_conf);

		for (int i = spec_item_npc_talk_begin; i < spec_item_npc_talk_end; i ++) {
			item_conf.itemid = i;
			item_conf.total_max = -1;
			item_conf.day_max = -1;
			g_item_conf_map.add(0, item_conf);
		}
	}
	return 0;
}

int load_island_map(xmlNodePtr cur){
	DEBUG_LOG("xxxxxx load_island_map ");
	g_map_conf_map.clear_island_time();
  	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"island")) {
			uint32_t islandid=0, medalid;
			DECODE_XML_PROP_INT_DEFAULT(islandid, cur, "id",0);
			DECODE_XML_PROP_INT_DEFAULT(medalid, cur, "medal",0);
			if (medalid) {
				g_item_conf_map.add_medal(medalid);
			}

			std::map<uint32_t, uint32_t> time_map[7];
			const xmlChar * buf;
			DECODE_XML_PROP_DEFAULT(cur, "time", buf);
			if (init_time_list((const char *)buf, time_map)) {
				g_map_conf_map.add_island_time(islandid, time_map);
			}

			xmlNodePtr chl = cur->xmlChildrenNode;
		  	while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar *)"Map")) {
					map_conf_t map_conf;
					DECODE_XML_PROP_INT_DEFAULT(map_conf.mapid , chl , "id",0);
					DECODE_XML_PROP_INT_DEFAULT(map_conf.multi , chl , "multi",0);
					g_map_conf_map.add ( islandid,map_conf );

				}
				chl= chl->next;	
			}
		}
		cur = cur->next;	

	}
	g_map_conf_map.reset_island_timer();

	return 0;
}

int load_task_map(xmlNodePtr cur){
	DEBUG_LOG("xxxxxx load_task_map ");
  	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"island")) {
			uint32_t taskid=0;
			DECODE_XML_PROP_INT_DEFAULT(taskid , cur, "id",0);

			xmlNodePtr chl = cur->xmlChildrenNode;
		  	while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar *)"tastdata")) {
					const xmlChar * buf;
					Ctask_node task_node;
					DECODE_XML_PROP_INT(task_node.task_nodeid , chl , "id");

					DECODE_XML_PROP(chl , "req_id", buf);
 					init_int_list((const char*)buf,task_node.req_taskid_list);
				
					DECODE_XML_PROP(chl , "req_item", buf);
 					init_item_list((const char*)buf,task_node.req_item_list);
					DECODE_XML_PROP(chl , "recycle_item", buf);
 					init_item_list((const char*)buf,task_node.recycle_item_list);
						
					DECODE_XML_PROP(chl , "reward_item", buf);
 					init_item_list((const char*)buf,task_node.reward_item_list);

					uint32_t over_type;
					DECODE_XML_PROP_INT_DEFAULT(over_type, chl, "over_type", 0);
					if (over_type == 8) {
						DECODE_XML_PROP(chl , "over_id", buf);
 						init_item_list((const char*)buf,task_node.reward_item_list);
					}

					g_task_map.add_task_node(taskid,task_node);
				}
				chl= chl->next;	
			}
		}
		cur = cur->next;	

	}

	DEBUG_LOG("xxxxxx load_task_map  end");
	return 0;
}

int load_game_map(xmlNodePtr cur){
	DEBUG_LOG("xxxxxx load_game_conf_map ");
  	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"game")) {
			uint32_t gameid, winpt, losept, min_time;
			DECODE_XML_PROP_INT_DEFAULT(gameid, cur, "id",0);
			DECODE_XML_PROP_INT_DEFAULT(winpt, cur, "win",5);
			DECODE_XML_PROP_INT_DEFAULT(losept, cur, "lose",2);
			DECODE_XML_PROP_INT_DEFAULT(min_time, cur, "time", 60);
			g_game_conf_map.add_game(gameid, winpt, losept, min_time);
		}
		cur = cur->next;	

	}

	return 0;
}

int load_movement(xmlNodePtr cur)
{
	g_item_movement_map.init();
  	cur = cur->xmlChildrenNode;
	while (cur) {
		uint32_t exprid, cardid, math;
		DECODE_XML_PROP_INT_DEFAULT(exprid, cur, "id",0);
		DECODE_XML_PROP_INT_DEFAULT(cardid, cur, "cardid",0);
		DECODE_XML_PROP_INT_DEFAULT(math, cur, "math",0);

		if (math) {
			exprid += 20000;
			g_item_movement_map.add_movement(exprid, cardid);
			DEBUG_LOG("movement\t[%u %u]", exprid, cardid);
		}
		cur = cur->next;	
	}
	return 0;
}

int load_rand_group(xmlNodePtr cur)
{
	g_rand_group_map.clear();
  	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"group")) {
			uint32_t gid = 0;
			DECODE_XML_PROP_INT_DEFAULT(gid , cur, "id", 0);
			g_rand_group_map[gid] = Citem_rand_group();
			DECODE_XML_PROP_INT_DEFAULT(g_rand_group_map[gid].randbase , cur, "randbase", 0);
			xmlNodePtr chl = cur->xmlChildrenNode;
		  	while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar *)"item")) {
					Citem_rand item_rand;
					const xmlChar * buf;
					DECODE_XML_PROP(chl , "idlist", buf);
					init_item_count_list((const char*)buf, item_rand.item_list);
					DECODE_XML_PROP_INT_DEFAULT(item_rand.proability, chl, "proability", 0);
					// 空的不插入
					if (item_rand.item_list.size())
						g_rand_group_map[gid].item_list.push_back(item_rand);
				}
				chl = chl->next;
			}

		}
		cur = cur->next;
	}
	return 0;
}

int load_npc_exp(xmlNodePtr cur)
{
	npc_trans_map.clear();
  	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"npcxp")) {
			uint32_t id = 0;
			DECODE_XML_PROP_INT_DEFAULT(id , cur, "id", 0);
			const xmlChar * buf;
			DECODE_XML_PROP(cur , "npcid", buf);
			std::set<uint32_t> npclist;
			init_int_list((const char*)buf, npclist);
			for (std::set<uint32_t>::iterator it = npclist.begin(); 
					it != npclist.end(); it ++) {
				// 转换为系统内部ID
				npc_trans_map[*it] = id + 10000;
			}

		}
		cur = cur->next;
	}
	return 0;
}
