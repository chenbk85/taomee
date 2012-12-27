#include <libtaomee++/inet/byteswap.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>

#include <string>
#include <map>
#include "conf_test.hpp"

using namespace taomee;
using namespace std;

//------------------------------------------------------------------
// global vars
//------------------------------------------------------------------
typedef std::map<uint32_t, item_t> ItemsMap;
typedef std::map<uint32_t, task_t> TasksMap;
typedef std::map<uint32_t, unique_item_pos_t> UniqueItemPosMap;
static ItemsMap items;
static TasksMap tasks;
static UniqueItemPosMap unique_item_map;
restriction_t g_all_restr[3000];
//------------------------------------------------------------------
// load function
//------------------------------------------------------------------
/**
 * @brief load xml 
 * @return 0 on success
 */
int load_xml(const char* file, int (*load_fun)(xmlNodePtr cur_node))
{
	int err = -1;
	xmlDocPtr doc = xmlParseFile(file);
	if (!doc) {
		error_log("ERROR!\t[load file %s]", file);
	   	return -1;
   	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		error_log("ERROR!\t[load file %s]", file);
	   	return -1;
   	}
	
	err = load_fun(cur);
	xmlFreeDoc(doc);

	return err;
}

//------------------------------------------------------------------
// load item function
//------------------------------------------------------------------
/**
 * @brief judge if itemid existed
 * @return item type id
 */
uint32_t is_item_existed(uint32_t id) 
{
	ItemsMap::iterator it = items.find(id);
	if (it == items.end()) {
		return noitem;
	}
	if (it->second.catid <= 11) {
		return it_clothes;
	} else {
		return it_item;
	}

	return noitem;
}

/**
 * @brief load item xml 
 * @return 0 on success
 */
int load_item(xmlNodePtr cur)
{
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Cat")))	{
			uint32_t catid = 0, db_catid = 0, max = 0;
			get_xml_prop(catid, cur, "ID");
			get_xml_prop(db_catid, cur, "DbCatID");
			get_xml_prop(max, cur, "Max");

			xmlNodePtr item_cur = cur->xmlChildrenNode;
			while (item_cur) {
				if (!xmlStrcmp(item_cur->name, reinterpret_cast<const xmlChar*>("Item"))) {
					struct item_t itm;
					itm.catid = catid;
					itm.db_catid = db_catid;
					itm.max = max;
					get_xml_prop(itm.id, item_cur, "ID");
					get_xml_prop_def(itm.setid, item_cur, "SetID", 0);
					items.insert(ItemsMap::value_type(itm.id, itm));
				}
				item_cur = item_cur->next;
			}
		}
		cur = cur->next;
	}

	return 0;
}

//------------------------------------------------------------------
// load task function
//------------------------------------------------------------------
/**
 * @brief task task item xml 
 * @return 0 on success
 */
int load_task_items(xmlNodePtr cur, struct task_bound_t* p_task_bound, uint32_t taskid) 
{
	cur = cur->xmlChildrenNode;
	uint32_t cnt = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Item"))) {
			if (cnt >= 10) {
				error_log("task item count than max!\t[taskid=%u]", taskid);
				break;
			}
			get_xml_prop(p_task_bound->items[cnt].give_type, cur, "give_type");
			get_xml_prop(p_task_bound->items[cnt].give_id, cur, "give_id");
			get_xml_prop(p_task_bound->items[cnt].give_cnt, cur, "cnt");

			if (p_task_bound->items[cnt].give_type <= 2) {//attire
				uint32_t item_type = is_item_existed(p_task_bound->items[cnt].give_id);
				if (item_type == noitem) {
					error_log("task item id not existed!\t[taskid=%u itemid=%u]", taskid, p_task_bound->items[cnt].give_id);
				} else if (item_type != p_task_bound->items[cnt].give_type) {
					error_log("task item type error!\t[taskid=%u itemid=%u]", taskid, p_task_bound->items[cnt].give_id);
				}
			}
			cnt++;
		}
		cur = cur->next;
	}

	return 0;
}

/**
 * @brief load task xml 
 * @return 0 on success
 */
int load_tasks(xmlNodePtr cur)
{
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Task")))	{
			struct task_t task;
			get_xml_prop(task.id, cur, "id");
			get_xml_prop(task.type, cur, "type");
			int arr_len = get_xml_prop_arr(task.need_lv, cur, "need_lv");
			if (arr_len != 2) {
				error_log("need lv cnt error!\t[taskid=%u]", task.id);
			}

			uint32_t in_cnt = 0;
			uint32_t out_cnt = 0;
			xmlNodePtr item_cur = cur->xmlChildrenNode;
			while (item_cur) {
				if (!xmlStrcmp(item_cur->name, reinterpret_cast<const xmlChar*>("InItem"))) {
					if (in_cnt >= 1) {
						error_log("InItem cnt than max!\t[taskid=%u]", task.id);
						break;
					}
					get_xml_prop(task.in.id, cur, "id");
					load_task_items(item_cur, &(task.in), task.id);
					in_cnt++;
				}
				if (!xmlStrcmp(item_cur->name, reinterpret_cast<const xmlChar*>("OutItem"))) {
					if (out_cnt >= 5) {
						error_log("OutItem cnt than max!\t[taskid=%u]", task.id);
						break;
					}
					get_xml_prop(task.out[out_cnt].id, cur, "id");
					load_task_items(item_cur, &(task.out[out_cnt]), task.id);
					out_cnt++;
				}
				item_cur = item_cur->next;
			}

			tasks.insert(TasksMap::value_type(task.id, task));
		}
		cur = cur->next;
	}

	return 0;
}

/* 
 * @brief
 * @return 0 on success,
 */
int load_dailyactitivity(xmlNodePtr root)
{
	xmlNodePtr cur = root->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Restriction")))	{
			struct task_t task;
			get_xml_prop(task.id, cur, "id");
			get_xml_prop(task.type, cur, "type");
			int arr_len = get_xml_prop_arr(task.need_lv, cur, "need_lv");
			if (arr_len != 2) {
				error_log("need lv cnt error!\t[taskid=%u]", task.id);
			}

			uint32_t in_cnt = 0;
			uint32_t out_cnt = 0;
			xmlNodePtr item_cur = cur->xmlChildrenNode;
			while (item_cur) {
				if (!xmlStrcmp(item_cur->name, reinterpret_cast<const xmlChar*>("InItem"))) {
					if (in_cnt >= 1) {
						error_log("InItem cnt than max!\t[taskid=%u]", task.id);
						break;
					}
					get_xml_prop(task.in.id, cur, "id");
					load_task_items(item_cur, &(task.in), task.id);
					in_cnt++;
				}
				if (!xmlStrcmp(item_cur->name, reinterpret_cast<const xmlChar*>("OutItem"))) {
					if (out_cnt >= 5) {
						error_log("OutItem cnt than max!\t[taskid=%u]", task.id);
						break;
					}
					get_xml_prop(task.out[out_cnt].id, cur, "id");
					load_task_items(item_cur, &(task.out[out_cnt]), task.id);
					out_cnt++;
				}
				item_cur = item_cur->next;
			}

			tasks.insert(TasksMap::value_type(task.id, task));
		}
		cur = cur->next;

	}
	return 0;
}

/**
  * @brief parse the "costs" and "rewards" sub node
  * @param cur sub node 
  * @param idx index to indicate certain restriction action
  * @return true on success, false on error
  */
bool parse_restriction_cost_reward(xmlNodePtr cur, uint32_t idx)
{
	uint32_t cnt1 = 0, cnt2 = 0;

	while (cur) {
		if ((!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("Costs")))) {
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if ((!xmlStrcmp(chl->name, reinterpret_cast<const xmlChar *>("Cost")))) {
					get_xml_prop(g_all_restr[idx].costs[cnt1].give_type, chl, "give_type");
					get_xml_prop(g_all_restr[idx].costs[cnt1].give_id, chl, "give_id");
					get_xml_prop(g_all_restr[idx].costs[cnt1].count, chl, "count");
					cnt1++;
				} 
				chl = chl->next;
			}
		}

		if ((!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("Rewards")))) {
			get_xml_prop_def(g_all_restr[idx].odds_flg, cur, "OddsFlg", 0);
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if ((!xmlStrcmp(chl->name, reinterpret_cast<const xmlChar *>("Reward")))) {
					get_xml_prop(g_all_restr[idx].rewards[cnt2].give_type, chl, "give_type");
					get_xml_prop_def(g_all_restr[idx].rewards[cnt2].role_type, chl, "role_type", 0);
					get_xml_prop(g_all_restr[idx].rewards[cnt2].give_id, chl, "give_id");
                    //KDEBUG_LOG(p->id, "dailyAction give: [%u ---- %u]",g_all_restr[idx].rewards[cnt2].give_type,g_all_restr[idx].rewards[cnt2].give_id);
                    if ( g_all_restr[idx].rewards[cnt2].give_type == 1 ) {
						if (!is_item_existed(g_all_restr[idx].rewards[cnt2].give_id)) {
							error_log("item not existed :%u ", g_all_restr[idx].rewards[cnt2].give_id);	
						}
                    } else if ( g_all_restr[idx].rewards[cnt2].give_type == 2 ) {
                    	if (!is_item_existed(g_all_restr[idx].rewards[cnt2].give_id)) {
                            error_log("dailyAction.xml parameter err.[give_type=2 - give_id=%u]",
                                g_all_restr[idx].rewards[cnt2].give_id);
                        }
                    } else if ( g_all_restr[idx].rewards[cnt2].give_type == 3 ) {
                        if ( g_all_restr[idx].rewards[cnt2].give_id > 100000 ) {
                            error_log("dailyAction.xml parameter err.[give_type=3 - give_id=%u]",
                                g_all_restr[idx].rewards[cnt2].give_id);
                        }
                    }
					get_xml_prop(g_all_restr[idx].rewards[cnt2].count, chl, "count");
					get_xml_prop_def(g_all_restr[idx].rewards[cnt2].odds, chl, "Odds", 0);
					cnt2++;
				}
				chl = chl->next;
			}
		}
	
		cur = cur->next;
	} //end while(cur)

	(g_all_restr[idx]).cost_count = cnt1;
	(g_all_restr[idx]).rew_count = cnt2;
	return true;
}

/**
  * @brief return bitpos of unique items 
  * @return 0 none
  */
uint32_t get_unique_item_bitpos(uint32_t item_id)
{
	std::map<uint32_t, unique_item_pos_t>::iterator iter = unique_item_map.find(item_id);
	if (iter == unique_item_map.end()) {
		return 0;
	}
	return iter->second.bit_pos;
	
}


/**
  * @brief load restriction configs from an xml file
  * @param cur xml node 
  * @return 0 on success, -1 on error
  */
int load_restrictions(xmlNodePtr cur)
{
	uint32_t id_test = 0, dbpos_test = flag_pos_daily_active_start - 1;
	uint32_t id = 0, type = 0, restr_flag = 0, toplimit = 0, vip = 0, year_vip = 0, userflg = 0, dbpos = 0;
	cur = cur->xmlChildrenNode; 
	memset(g_all_restr, 0, sizeof(g_all_restr));
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Restriction"))) {
			id_test++;
			get_xml_prop(id, cur, "ID");
			//if (id != id_test) {
			//	error_log("Id duplicated restriction: id=%u", id);
			//	return -1;
			//}
		
			get_xml_prop(type, cur, "Type");
			uint32_t idx = id - 1;
			if ((id < 1) || (idx >= max_restr_count) || (g_all_restr[idx]).type) {
				error_log("invalid/duplicated restriction: type=%u max=%u %u", id, max_restr_count, g_all_restr[idx].type);
			}
			get_xml_prop_raw_str_def(g_all_restr[idx].name, cur, "Name", "兑换物品");
			get_xml_prop(restr_flag, cur, "Restr_Flag");
			get_xml_prop_def(toplimit, cur, "Toplimit", 999999);
			get_xml_prop_def(vip, cur, "Vip", 0);
			get_xml_prop_def(year_vip, cur, "YearVip", 0);
			//get_xml_prop_def(uselv, cur, "UseLv", 0);
            if (get_xml_prop_arr_def(g_all_restr[idx].uselv, cur, "UseLv", 0) != 2) {
                error_log("invalid appearlevel in parsing restriction UserLv: type=%u id=%u", type, id);
            }
			get_xml_prop_def(userflg, cur, "UserFlag", 0);
			if (userflg && restr_flag == daily_active_for_ever) {
				get_xml_prop(dbpos, cur, "DbPos");
				if (dbpos != dbpos_test + 1) {
					warning_log("restriction dbpos[%u %u] error!", dbpos, dbpos_test);
				}
				dbpos_test = dbpos;
			}
			if (!userflg && restr_flag == daily_active_for_ever) {
				if (!get_unique_item_bitpos(id)) {
					error_log("unique_item_map not have id %u ! ", id);
				}
			}

			if (get_xml_prop_arr_def(g_all_restr[idx].tm_range, cur, "TimeRange", 0) != 2) {
				error_log("invalid appeartime in parsing restriction TimeRange: type=%u", type);
			}
			(g_all_restr[idx]).id = id;
			(g_all_restr[idx]).type = type;
			(g_all_restr[idx]).restr_flag = restr_flag;
			(g_all_restr[idx]).toplimit = toplimit;
			(g_all_restr[idx]).vip = vip;
			(g_all_restr[idx]).year_vip = year_vip;
			//(g_all_restr[idx]).uselv = uselv;
			(g_all_restr[idx]).userflg = userflg;
			(g_all_restr[idx]).dbpos = dbpos;
			
            if (!parse_restriction_cost_reward(cur->xmlChildrenNode, idx)) {
                error_log("Parse restriction costs and rewards error!");
			}
		}
		cur = cur->next;
	}

	return 0;
}


/**
  * @brief load unique items from an xml file
  * @return 0 on success, -1 on error
  */
int load_unique_items(xmlNodePtr cur)
{
	// load unique items from xml file
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("UniqueDrop"))) {
			uint32_t id;
			uint32_t bit_pos;
			get_xml_prop(id, cur, "ID");
			get_xml_prop(bit_pos, cur, "Bit_Pos");

			if (unique_item_map.size() > 100) {
				error_log("unique items to many %d ", int(unique_item_map.size()));
			}
			if (unique_item_map.find(id) != unique_item_map.end()) {
				error_log("id:%u  defined twice", id);
			}
			unique_item_pos_t item(id, bit_pos);
			unique_item_map.insert(std::map<uint32_t, unique_item_pos_t>::value_type(id, item));
		}
		cur = cur->next;
	}
	return 0;
}



int write_xml(const char* file)
{
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST"root");

	xmlDocSetRootElement(doc, root_node);
	//xmlNewTextChild(root_node, NULL, BAD_CAST"newnode1", BAD_CAST"newnode1context");
	//xmlNewTextChild(root_node, NULL, BAD_CAST"newnode2", BAD_CAST"newnode2context");
	xmlNodePtr node = xmlNewNode(NULL, BAD_CAST"Restriction");
	xmlAddChild(root_node, node);
	xmlNewProp(node, BAD_CAST"ID", BAD_CAST"17");
	xmlNewProp(node, BAD_CAST"Type", BAD_CAST"1");
	xmlNewProp(node, BAD_CAST"Need_lv", BAD_CAST"1 99");

	xmlNodePtr comm = xmlNewComment(BAD_CAST"coooosts");
	xmlAddChild(node, comm);
	xmlNodePtr node2 = xmlNewNode(NULL, BAD_CAST"Costs");
	xmlAddChild(node, node2);

	xmlNodePtr node3 = xmlNewNode(NULL, BAD_CAST"Cost");
	xmlAddChild(node2, node3);
	xmlNewProp(node3, BAD_CAST"give_type", BAD_CAST"3");
	xmlNewProp(node3, BAD_CAST"give_id", BAD_CAST"4");
	xmlNewProp(node3, BAD_CAST"count", BAD_CAST"3000");

	comm = xmlNewComment(BAD_CAST"reeeewards");
	xmlAddChild(node, comm);
	xmlNodePtr node4 = xmlNewNode(NULL, BAD_CAST"Rewards");
	xmlAddChild(node, node4);

	xmlNodePtr node5 = xmlNewNode(NULL, BAD_CAST"Reward");
	xmlAddChild(node4, node5);
	xmlNewProp(node5, BAD_CAST"give_type", BAD_CAST"1");
	xmlNewProp(node5, BAD_CAST"give_id", BAD_CAST"300114");
	xmlNewProp(node5, BAD_CAST"count", BAD_CAST"3");
	
	//write to xml file
	int save_format = 1;
	int ret = xmlSaveFormatFile(file ,doc, save_format);
	if (ret != -1) {
		error_log("write xml ok!");
	}
	xmlFreeDoc(doc);

	return 1;
}

int main()
{
	load_xml("../../conf/items_attire.xml",load_item);
	load_xml("../../conf/items_shop.xml",load_item);
	load_xml("../../conf/items_nor.xml",load_item);
	noti_log("items count:[%u]", (int)(items.size()));

	load_xml("../../conf/unique_item.xml", load_unique_items);
	load_xml("../../conf/tasks.xml", load_tasks);
	load_xml("../../conf/dailyActivity.xml", load_restrictions);
	noti_log("tasks count:[%u]", (int)(tasks.size()));

	//write_xml("mao.xml");
	return 0;
}
