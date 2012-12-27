#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/random/random.hpp>

using namespace taomee;

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/project/stat_agent/msglog.h>
}

#include "utils.hpp"
#include "item.hpp"
#include "login.hpp"
#include "task.hpp"
#include "cli_proto.hpp"
#include "coupons.hpp"
#include "ambassador.hpp"
#include "apothecary.hpp"
#include "player.hpp"
#include "global_data.hpp"
#include "common_op.hpp"
#include "mcast_proto.hpp"


//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------
inline bool can_put_package(player_t* p, uint32_t id)
{
    uint32_t item_cnt = 0, attire_cnt = 0;
    exchange_t* p_res = &(g_all_coupons[id - 1]);

    for (uint32_t i = 0; i < p_res->rew_count; i++) {
        if (p_res->rewards[i].give_type == give_type_normal_item) {
            item_cnt++;
        } else if (p_res->rewards[i].give_type == give_type_clothes) {
            attire_cnt++;
        }
    }

    uint32_t max_bag = get_player_total_item_bag_grid_count(p);

    if ( (item_cnt + p->my_packs->all_items_cnt() > max_bag) 
        || (attire_cnt + p->my_packs->all_clothes_cnt() > max_bag)) {
        ERROR_LOG("bag no space: %u [%u %u] [%u %u]", max_bag, item_cnt, p->my_packs->all_items_cnt(), attire_cnt, p->my_packs->all_clothes_cnt());
        return false;
    }
    return true;
}

uint32_t get_feedback_coin_cnt(uint32_t id)
{
    uint32_t count = 0;
    std::map<uint32_t, store_item_t>::iterator setIt = store_item_map_.find(id);

    if (setIt != store_item_map_.end()) {
	    count = 1;
    }

    std::map<uint32_t, store_feedback_t>::iterator mapIt = store_feedback_map_.find(id);

    if (mapIt != store_feedback_map_.end()) {
        count += mapIt->second.fb_cnt;
    }

    return count;
}

uint32_t get_item_gold_coin_price(uint32_t id)
{
    std::map<uint32_t, store_item_t>::iterator setIt = store_item_map_.find(id);
    if (setIt != store_item_map_.end()) {
        return (setIt->second.price * 5);
    }
    return 0;
}
bool is_product_vip_box(uint32_t id)
{
	std::map<uint32_t, coupon_product_item_t>::iterator setIt = product_item_map_.find(id);
	if (setIt != product_item_map_.end()) {
		return (setIt->second.is_box);
	}
	return 0;
}

void broadcast_coupons_event(player_t* p, uint32_t cost, exchange_t* p_event)
{
    if (p_event->is_broadcast == 0) return;

    char words[max_trade_mcast_size] = {0};
    snprintf(words, max_trade_mcast_size, "%s %d %s:%s", char_content[30].content, cost, char_content[31].content, p_event->name);

    TRACE_LOG("BROADCAST: %s", words);

    player_mcast_t info = {0};
    memset(&info, 0x00, sizeof(player_mcast_t));
    info.type = 2; //:player_market 2:player_world
    info.front_id = 2;
    memcpy(&(info.nick), char_content[12].content, sizeof(info.nick));
    memcpy(&(info.info), words, max_trade_mcast_size);

    notify_player_mcast_info(p, mcast_trade_message, &info);
}


//---------------------------------------------------------------------------------------------
// db_XXX function
//---------------------------------------------------------------------------------------------
int db_coupons_exchange_item(player_t* p, uint32_t tradeid, exchange_t* p_res)
{
	int idx = 0;
	pack_h(dbpkgbuf, tradeid, idx);
    pack_h(dbpkgbuf, static_cast<uint32_t>(gold_coin_item_id), idx);
   	pack_h(dbpkgbuf, p_res->price, idx);
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);
	pack_h(dbpkgbuf, p_res->rew_count, idx);

	for (uint32_t i = 0; i < p_res->rew_count; i++) {
		uint32_t duration = 0;
		uint32_t lifetime = 0;

		if (p_res->rewards[i].give_type == give_type_clothes) {
			const GfItem* itm = items->get_item(p_res->rewards[i].give_id);
			duration = itm->duration * clothes_duration_ratio;
            lifetime = itm->lifetime();
		} /*else if (p_res->rewards[i].give_type == give_type_player_attr && 
					p_res->rewards[i].give_id == give_type_apothecary_exp) {
			secondary_profession_data* p_sec_data;
			p_sec_data = get_player_secondary_profession_by_type(p, apothecary_profession_type);
			uint32_t cur_exp = p_sec_data->get_profession_exp();
			uint32_t max_exp = p_sec_data->m_pCalc->get_max_exp();
			uint32_t add_exp = p_res->rewards[i].count;
			if (cur_exp < max_exp) {
				p_res->rewards[i].count = (add_exp + cur_exp) > max_exp ? (max_exp - cur_exp) : add_exp;
			} else {
				p_res->rewards[i].count = 0;
			}
		}*/
		
		pack_h(dbpkgbuf, p_res->rewards[i].give_type, idx);
		pack_h(dbpkgbuf, p_res->rewards[i].give_id, idx);
		pack_h(dbpkgbuf, p_res->rewards[i].count, idx);
		pack_h(dbpkgbuf, duration, idx);
		pack_h(dbpkgbuf, lifetime, idx);
        TRACE_LOG("reward item :[%u %u %u %u %u %u]",
            p->id, p_res->rewards[i].give_type, p_res->rewards[i].give_id,
			p_res->rewards[i].count, duration, lifetime);
	}
	return send_request_to_db(p, p->id, p->role_tm, dbproto_coupons_exchange, dbpkgbuf, idx);
}


//---------------------------------------------------------------------------------------------
// Cmds
//---------------------------------------------------------------------------------------------

int get_coupons_exchange_limit_seq_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    //global_limit_data_t* p_limit_data = g_limit_data_mrg.get_limit_data(g_all_coupons[i].limit_seq);
    return 0;
}
/**
  * @brief player performs periodical action
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int coupons_exchange_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t id;
	unpack(body, id, idx);

	if (p->battle_grp) {
		return send_header_to_player(p, p->waitcmd, cli_err_unable_in_battle, 1);
	}
	
	int i = id - 1;
	if ((i < 0) || (static_cast<uint32_t>(i) >= max_coupons_exchange) || (g_all_coupons[i].id != id)) {
		ERROR_LOG("Invalid action type\t[id=%u action_type=%u]", p->id, id);
		p->waitcmd = 0;
		return 0;
	}
	
    if (g_all_coupons[i].uselv[0] > p->lv) {
        return send_header_to_player(p, p->waitcmd, cli_err_less_lv, 1);
    }
    if (g_all_coupons[i].uselv[1] <= p->lv) {
        return send_header_to_player(p, p->waitcmd, cli_err_large_lv, 1);
    }

	if (g_all_coupons[i].vip && (g_all_coupons[i].vip > p->vip_level || !(is_vip_player(p)))) {	
		return send_header_to_player(p, p->waitcmd, cli_err_nonvip_operation_forbidden, 1);
	}

    //if (id == 16 && get_swap_action_times(p, 1289) >= 1) {
    //    return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    //}

    // coupons id = gold_coin_item_id
    if (p->my_packs->get_item_cnt(gold_coin_item_id) < g_all_coupons[i].price) {
        return send_header_to_player(p, p->waitcmd, cli_err_notenough_coupons, 1);
    }

	KDEBUG_LOG(p->id, "ADD ACTION CNT\t[uid=%u id=%u rew_count=%u]", p->id, id, g_all_coupons[i].rew_count);

    if ( !can_put_package(p, id)) {
        return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
    }

    if (g_all_coupons[i].limit_seq) {
        global_limit_data_t* p_limit_data = g_limit_data_mrg.get_limit_data(g_all_coupons[i].limit_seq);
        if (p_limit_data && p_limit_data->left_cnt == 0) {
            return send_header_to_player(p, p->waitcmd, cli_err_action_global_limited, 1);
        }

        if (p_limit_data) {
            g_limit_data_mrg.reduce_item_cnt(g_all_coupons[i].limit_seq, 1, true);
        }
    }

    db_coupons_exchange_item(p, id, &(g_all_coupons[i]));
	return 0;
}

//---------------------------------------------------------------------------------------------
// Callbacks
//---------------------------------------------------------------------------------------------
/**
  * @brief callback for handling adding periodical action returned from dbproxy
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_coupons_exchange_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
    db_coupons_exchange_t *rsp = reinterpret_cast<db_coupons_exchange_t *>(body);
    CHECK_VAL_EQ(bodylen, rsp->item_cnt * sizeof(db_exchange_item_t) + sizeof(db_coupons_exchange_t));

    KDEBUG_LOG(p->id, "COUPONS EXCHANGE CALLBACK\t[trade id =%u left coupons=%u]",rsp->trade_id,rsp->left_coupons);
    //reduce player's coupons
    uint32_t old_cnt = p->my_packs->get_item_cnt(gold_coin_item_id);
    p->my_packs->del_item(p, gold_coin_item_id, old_cnt - rsp->left_coupons, channel_string_other);

    db_exchange_item_t* item = reinterpret_cast<db_exchange_item_t*>((uint8_t*)body + sizeof(db_coupons_exchange_t));

#ifdef VERSION_KAIXIN
	do_stat_log_universal_interface_1(kaixin_stat_log_gongfuquan_shop, rsp->trade_id, 1);
#endif
    broadcast_coupons_event(p, old_cnt - rsp->left_coupons, &(g_all_coupons[rsp->trade_id - 1]));

    //if (rsp->trade_id == 16) {
    //    add_swap_action_times(p, 1289);
    //}

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->trade_id, idx);
    pack(pkgbuf, rsp->left_coupons, idx);
    pack(pkgbuf, rsp->item_cnt, idx);
    for (uint32_t i = 0; i < rsp->item_cnt; i++) {
        TRACE_LOG("%u %u %u %u %u",item[i].id,item[i].cnt,item[i].uniqueid,item[i].gettime,item[i].endtime);
        if (item[i].type == give_type_clothes) {
            p->my_packs->add_clothes(p,item[i].id, item[i].uniqueid,0, channel_string_other,item[i].gettime,item[i].endtime,0);
        } else if (item[i].type == give_type_normal_item) {
            p->my_packs->add_item(p, item[i].id, item[i].cnt, channel_string_other);
        }
        pack(pkgbuf, item[i].type, idx);
        pack(pkgbuf, item[i].id, idx);
        pack(pkgbuf, item[i].cnt, idx);
        pack(pkgbuf, item[i].uniqueid, idx);
        pack(pkgbuf, item[i].gettime, idx);
        pack(pkgbuf, item[i].endtime, idx);
    }
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}


//---------------------------Parse Xml File-------------------------------------------------
/**
  * @brief parse the "rewards" sub node
  * @param cur sub node 
  * @param idx index to indicate certain exchange action
  * @return true on success, false on error
  */
bool parse_exchange_reward(xmlNodePtr cur, uint32_t idx)
{
	uint32_t cnt = 0;

	while (cur) {
		if ((!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("Rewards")))) {
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if ((!xmlStrcmp(chl->name, reinterpret_cast<const xmlChar *>("Reward")))) {
					get_xml_prop(g_all_coupons[idx].rewards[cnt].give_type, chl, "give_type");
					get_xml_prop(g_all_coupons[idx].rewards[cnt].give_id, chl, "give_id");
                    //KDEBUG_LOG(p->id, "coupons exchange give: [%u ---- %u]",g_all_coupons[idx].rewards[cnt].give_type,g_all_coupons[idx].rewards[cnt].give_id);
                    if ( g_all_coupons[idx].rewards[cnt].give_type == 1 ) {
                        const GfItem* itm = items->get_item(g_all_coupons[idx].rewards[cnt].give_id);
                        if ( !itm || !items->is_clothes(itm->category()) ) {
                            ERROR_LOG("coupons.xml parameter err.[give_type=1 - give_id=%u]",
                                g_all_coupons[idx].rewards[cnt].give_id);
                            return false;
                        }
                    } else if ( g_all_coupons[idx].rewards[cnt].give_type == 2 ) {
                        const GfItem* itm = items->get_item(g_all_coupons[idx].rewards[cnt].give_id);
                        if ( !itm || items->is_clothes(itm->category()) ) {
                            ERROR_LOG("coupons.xml parameter err.[give_type=2 - give_id=%u]",
                                g_all_coupons[idx].rewards[cnt].give_id);
                            return false;
                        }
                    } else {
                            ERROR_LOG("coupons.xml parameter err.[give_type=3 - give_id=%u]",
                                g_all_coupons[idx].rewards[cnt].give_id);
                            return false;
                    }
					get_xml_prop(g_all_coupons[idx].rewards[cnt].count, chl, "count");
					TRACE_LOG("Reward:[%u %u %u]",g_all_coupons[idx].rewards[cnt].give_type, 
                        g_all_coupons[idx].rewards[cnt].give_id, g_all_coupons[idx].rewards[cnt].count);
					cnt++;
				}
				chl = chl->next;
			}
		}
	
		cur = cur->next;
	} //end while(cur)

	(g_all_coupons[idx]).rew_count = cnt;
	TRACE_LOG("coupons exchange cnt[%u]",(g_all_coupons[idx]).rew_count);
	return true;
}

/**
  * @brief load exchange configs from an xml file
  * @param cur xml node 
  * @return 0 on success, -1 on error
  */
int load_coupons_exchange(xmlNodePtr cur)
{
    uint32_t id_test = 0;
	uint32_t id = 0, type = 0, restr_flag = 0, toplimit = 0, vip = 0, cost = 0;
    uint32_t price = 0, is_broadcast = 0, limit_seq =0;
	cur = cur->xmlChildrenNode; 
	memset(g_all_coupons, 0, sizeof(g_all_coupons));
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Exchange"))) {
			id_test++;
			get_xml_prop(id, cur, "ID");
			if ((id != id_test) || (id < 1) || (id >= max_coupons_exchange)) {
				ERROR_LOG("Id invalid/duplicated restriction: id=%u", id);
				return -1;
			}
		
            uint32_t idx = id - 1;
            get_xml_prop_raw_str_def(g_all_coupons[idx].name, cur, "Name", "神秘金币物品");
			get_xml_prop(restr_flag, cur, "Restr_Flag");
			get_xml_prop_def(toplimit, cur, "Toplimit", 999999);
			get_xml_prop_def(vip, cur, "Vip", 0);
			get_xml_prop_def(cost, cur, "Costs", 9999);
			get_xml_prop_def(price, cur, "Price", cost);
			get_xml_prop_def(is_broadcast, cur, "IsBroadcast", 0);
			get_xml_prop_def(limit_seq, cur, "LimitSeq", 0);
            if (get_xml_prop_arr_def(g_all_coupons[idx].uselv, cur, "UseLv", 0) != 2) {
                ERROR_LOG("invalid appearlevel in parsing exchange UserLv: type=%u id=%u", type, id);
                return -1;
            }

			(g_all_coupons[idx]).id = id;
			(g_all_coupons[idx]).restr_flag = restr_flag;
			(g_all_coupons[idx]).toplimit = toplimit;
			(g_all_coupons[idx]).vip = vip;
			(g_all_coupons[idx]).cost_count = cost;
			(g_all_coupons[idx]).price = price;
			(g_all_coupons[idx]).is_broadcast = is_broadcast;
			(g_all_coupons[idx]).limit_seq = limit_seq;
			
            TRACE_LOG("coupons exchange:[%u %u %u %u [%u %u] %u]",
                (g_all_coupons[idx]).id, (g_all_coupons[idx]).restr_flag, (g_all_coupons[idx]).toplimit, 
                (g_all_coupons[idx]).vip, (g_all_coupons[idx]).uselv[0],(g_all_coupons[idx]).uselv[1],
                (g_all_coupons[idx]).cost_count);
            if (!parse_exchange_reward(cur->xmlChildrenNode, idx)) {
                ERROR_LOG("Parse exchange rewards error!");
				return -1;
			}
		}
		cur = cur->next;
	}

	return 0;
}
//-----------------------------------------------------------------------------------------
/**
  * @brief load exchange configs from an xml file
  * @param cur xml node 
  * @return 0 on success, -1 on error
  */
int load_store_sales(xmlNodePtr cur)
{
	uint32_t id = 0;
    uint32_t price = 0;
    uint32_t group_num = 1;
    uint32_t pkg_count = 0;
	cur = cur->xmlChildrenNode; 
	while (cur) {
        store_item_t store_item = {0};
        id = 0;
        price = 0;
        //memset(&store_item, 0x00, sizeof(store_item_t));
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Product"))) {
			uint32_t is_box = 0;
			get_xml_prop_def(is_box, cur, "isBox", 0);

            xmlNodePtr child_node = cur->xmlChildrenNode;
            while ( child_node ) {

                if (!xmlStrcmp(child_node->name, reinterpret_cast<const xmlChar*>("ProductItem"))) {
                    xmlNodePtr grandson_node = child_node->xmlChildrenNode;

                    pkg_count = 0;
                    while (grandson_node) {
                        if (!xmlStrcmp(grandson_node->name, reinterpret_cast<const xmlChar*>("Item"))) {
                            get_xml_prop(id, grandson_node, "ID");
                            if ( (id < 100000) || (id >= 9000000) ) {
                                ERROR_LOG("Id invalid storeSales: id=%u", id);
                                return -1;
                            }
                            TRACE_LOG("storeSales item ID: %u", id);
                            //store_item_set_.insert(id);
                        }
                        pkg_count++;
                        grandson_node = grandson_node->next;
                    }
                } // end ProductItem

                //if (pkg_count > 1) {
                //    break;
                //}

                if (!xmlStrcmp(child_node->name, reinterpret_cast<const xmlChar*>("ProductStore"))) {
                    xmlNodePtr grandson_node_2 = child_node->xmlChildrenNode;

                    //while (grandson_node_2) {
                    if (!xmlStrcmp(grandson_node_2->name, reinterpret_cast<const xmlChar*>("Store"))) {
                        if (pkg_count == 1) {
                            get_xml_prop_def(group_num, grandson_node_2, "GroupNum", 1);
                            get_xml_prop_def(price, grandson_node_2, "SalePrice", 0);
                        }

                        uint32_t icon = 0;
                        get_xml_prop_def(icon, grandson_node_2, "StoreId", 0);
                        coupon_product_item_t tmp_item = { icon, is_box };
                        TRACE_LOG("store product [%u %u]", icon, is_box);
                        product_item_map_[icon] = tmp_item;
                    }
                    TRACE_LOG("storeSales Store Info: %u %u", price, group_num);
                    //grandson_node_2 = grandson_node_2->next;
                    //}
                } // end ProductStore
                child_node = child_node->next;
            }

            if (pkg_count > 1) {
                TRACE_LOG("storeSales drop product ----");
                cur = cur->next;
                continue;
            }


            std::map<uint32_t, store_item_t>::iterator it = store_item_map_.find(id);
            if (it != store_item_map_.end()) {
                ERROR_LOG("storeSales item have exist!!! [%u %u %u]", id, price, group_num);
                cur = cur->next;
                continue;
            }

            store_item.price = price / group_num;
            TRACE_LOG("store item %u | %u", id, store_item.price);
            store_item_map_[id] = store_item;

        } // end Product

		cur = cur->next;
	}

	return 0;
}

/**
  * @brief load exchange configs from an xml file
  * @param cur xml node 
  * @return 0 on success, -1 on error
  */
int load_store_feedback(xmlNodePtr cur)
{
	uint32_t id = 0, feedback = 0;;
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Item"))) {
			get_xml_prop(id, cur, "ID");
            if ( (id < 1000000) || (id >= 9000000)) {
                ERROR_LOG("Id invalid storeFeedback: id=%u", id);
                return -1;
            }
            std::map<uint32_t, store_feedback_t>::iterator it = store_feedback_map_.find(id);
            if (it != store_feedback_map_.end()) {
                ERROR_LOG("Id duplicated storeFeedback: id=%u", id);
                return -1;
            }

            get_xml_prop_def(feedback, cur, "FeedbackCount", 1);
            store_feedback_t sf = {0};
            sf.fb_cnt = feedback;

            TRACE_LOG("storeFeedback %u | %u", id, sf.fb_cnt);
            store_feedback_map_[id] = sf;
        }
        /*
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Feedback"))) {
			get_xml_prop(id, cur, "ID");
        }*/
		cur = cur->next;
	}

	return 0;
}

//---------------------------------------------------------------------------------------------
