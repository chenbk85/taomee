/*
 * =====================================================================================
 *
 *       Filename:  card.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/23/2011 10:13:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus@taomee.com
 *        Company:  Taomee.sh
 *
 * =====================================================================================
 */

#include "item.hpp"
#include "dbproxy.hpp"
#include "cli_proto.hpp"
#include "player.hpp"
#include "card.hpp"
#include "utils.hpp"
#include "login.hpp"
#include "global_data.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <map>
using namespace taomee;


static std::map<uint32_t, card_set_t*>  all_card_set;

typedef std::map<uint32_t, card_set_t*> SetMapType;
typedef std::map<uint32_t, card_set_t*>::iterator SetMapIterator;

static std::map<uint32_t, card_t> all_cards;

typedef std::map<uint32_t, card_t> CardMapType;
typedef std::map<uint32_t, card_t>::iterator CardMapIterator;

static std::map<uint32_t, set_reward_t> cards_reward;

static std::map<uint32_t, uint32_t> set_swap;

card_t * get_card_by_id(uint32_t id)
{
	CardMapIterator it = all_cards.find(id);
	if (it != all_cards.end()) {
		return &(it->second);
	}
	return 0;
}

set_reward_t * get_reward(uint32_t set_id)
{
	std::map<uint32_t, set_reward_t>::iterator it = cards_reward.find(set_id); 
	if (it != cards_reward.end()) {
		return &(it->second);
	}
	return 0;
}

uint32_t get_set_id_by_swap_id(uint32_t swap_id) 
{
	std::map<uint32_t, uint32_t>::iterator it = set_swap.find(swap_id); 
	if (it != set_swap.end()) {
		return it->second;
	}
	return 0;
}

uint32_t xml_char_2_int(char* srcstr, uint32_t* array)
{
    char* p = srcstr;

    if (!is_digit(*p)) {
        return -1;
    }

    uint32_t tmp = 0;
    uint32_t idx = 0;
    while (*p != '\0') {
        while ( is_digit(*p) ) {
            tmp = tmp * 10 + (*p++ - 0x30);
        }

        array[idx] = tmp;
        tmp = 0;
        if (*p == ',') {
            p++;
            idx++;
        }
    }
    return (idx + 1);
}


int load_cards(xmlNodePtr cur)
{
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("cards"))) {
			xmlNodePtr cards_node = cur->xmlChildrenNode;
			while (cards_node) {
				if (!xmlStrcmp(cards_node->name, reinterpret_cast<const xmlChar*>("card"))) {
					card_t card;

					get_xml_prop(card.card_id, cards_node, "id");

					CardMapIterator it = all_cards.find(card.card_id);

					if (it != all_cards.end()) {
						ERROR_LOG("duplicate card ID %u", card.card_id);
						return -1;
					}

					get_xml_prop(card.star_cnt, cards_node, "rare");

					//check if item exit
					get_xml_prop(card.item_id, cards_node, "costItemId");

					const GfItem* itm = items->get_item(card.item_id);
					if (!itm) {
						ERROR_LOG("NOT EXIST item! %u", card.item_id);
						return -1;
					}
					char set_str[400] = {0};
					get_xml_prop_raw_str(set_str, cards_node, "categotyId");
					int set_num = xml_char_2_int(set_str, card.sets);
					card.set_cnt = set_num;
					all_cards.insert(CardMapType::value_type(card.card_id, card));

					for (uint32_t i = 0; i < card.set_cnt; i++) {
						//ERROR_LOG("card ID----------- %u Set %u", card.card_id, card.sets[i]);
						SetMapIterator it = all_card_set.find(card.sets[i]); 
						if (it != all_card_set.end()) {
							card_set_t *set = it->second;
							set->card_list->push_back(card.card_id);
						} else {
							card_set_t *set = new card_set_t(card.sets[i]);
							set->card_list->push_back(card.card_id);
							all_card_set[(card.sets[i])] = set;
						}
					}
				}
				cards_node = cards_node->next;
			}
		}

		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("categorys"))) {
			xmlNodePtr categorys = cur->xmlChildrenNode; 
			while (categorys) {
				if (!xmlStrcmp(categorys->name, reinterpret_cast<const xmlChar*>("category"))) {
					set_reward_t reward;
					get_xml_prop(reward.set_id, categorys, "id");
					get_xml_prop(reward.swap_id, categorys, "swapId");
					if (!g_swap_action_mrg.get_swap_action(reward.swap_id)) {
						ERROR_LOG("NOT EXIST SWAP ID %u", reward.set_id);
						return -1;
					} else {
						set_swap[reward.swap_id] = reward.set_id;
					}
				}	
				categorys = categorys->next;
			}
		}

		cur = cur->next;
	}

	return 0;
}

card_set_t * get_card_set(uint32_t set) 
{
	SetMapIterator it= all_card_set.find(set);
	if (it != all_card_set.end()) {
		return (it->second);
	}
	return 0;
}

player_card_set_t * get_player_card_set(player_t *p, uint32_t set_id)
{
	PCardSetMapIterator it = p->cardset_info->find(set_id);
	if (it != p->cardset_info->end()) {
		return (it->second);
	}
	return 0;
}

int player_get_card_list_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);
	int ifx = idx + 4;
	int cnt = 0;
	for (PCardSetMapIterator it = p->cardset_info->begin();
			it != p->cardset_info->end(); ++it) {
		player_card_set_t * set = it->second;
		for (PCardMapIterator it = set->set_card_info->begin(); 
				it != set->set_card_info->end(); ++it) {
			player_card_t card = it->second;
			pack(pkgbuf, card.card_id, ifx);
			pack(pkgbuf, card.card_type, ifx);
			pack(pkgbuf, card.card_set, ifx);
			cnt++;
			//ERROR_LOG("CARD SET %u Card [%u %u]", 
			//		  set->set_id, card.card_id, card.card_type);
		}
	}

	pack(pkgbuf, cnt, idx);
	idx = ifx;
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int db_get_card_list(player_t * p)
{

	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_card_list, 0, 0);
}



int db_get_card_list_callback(player_t* p,
	   			       		  userid_t id,
							  void* body,
							  uint32_t bodylen,
							  uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t card_cnt = 0;
	int idx = 0;
	unpack_h(body, card_cnt, idx);
	CHECK_VAL_EQ(bodylen, card_cnt * sizeof(player_card_t) + 4);

	//ERROR_LOG("get card_cnt %u", card_cnt);

	db_get_card_list_rsp_t * pkg = reinterpret_cast<db_get_card_list_rsp_t*>(body);


	for (uint32_t i = 0; i < pkg->card_cnt; i++) {
		player_card_t * card = &pkg->cards[i];
		PCardSetMapIterator it = p->cardset_info->find(card->card_set);
		if (it != p->cardset_info->end()) {
			player_card_set_t *set = (it->second);
			set->set_card_info->insert(PCardMap::value_type(card->card_id, *card));
		} else {
			player_card_set_t * set = new player_card_set_t();
			set->set_id = card->card_set;
			set->set_card_info->insert(PCardMap::value_type(card->card_id, *card));
			p->cardset_info->insert(PCardSetMap::value_type(card->card_set, set));
		}
		//ERROR_LOG("PLAYER %u HAS CARD [%u %u %u]", p->id, card->card_id, card->card_type, card->card_set); 
	}

	if (p->waitcmd == cli_proto_login) {
		return db_get_other_info(p);
	}
	return 0;
}

bool cardset_has_this_card_type(uint32_t set_id, uint32_t type_id)
{
	card_set_t * set = get_card_set(set_id);
	for (std::list<uint32_t>::iterator it = set->card_list->begin();
			it != set->card_list->end(); ++it) {
		if (*it == type_id) {
			return true;
		}
	}
	return false;
}

bool player_has_this_card_already(player_t * p, uint32_t card_type, uint32_t card_set)
{
	PCardSetMapIterator it = p->cardset_info->find(card_set);
	if (it != p->cardset_info->end()) {
		player_card_set_t * set = it->second;

		for (PCardMapIterator t_it = set->set_card_info->begin();
				t_it != set->set_card_info->end(); ++t_it) {
				player_card_t *card = &(t_it->second);
				if (card->card_type == card_type) {
					return true;
				}
		}
	}
	return false;
}


int player_insert_card_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t card_type = 0;
	uint32_t card_set = 0;
	int idx = 0;
	unpack(body, card_type, idx);
	unpack(body, card_set, idx);

	card_t * card = get_card_by_id(card_type);
	if (!card || !cardset_has_this_card_type(card_set, card_type) 
			|| player_has_this_card_already(p, card_type, card_set)) {
		ERROR_LOG("ERROR CARD TYPE %u %u", p->id, card_type);
		return -1;
	}

	//check item_id to card_set
	if (p->my_packs->get_item_cnt(card->item_id)) {
	  return db_insert_card(p, card->item_id, card->card_id, card_set);
	} else {
	  ERROR_LOG("DON HAVE THIS ITEM %u", p->id);
	  return send_header_to_player(p, p->waitcmd, cli_err_not_enough_flower, 1);
	}
}


int db_insert_card(player_t *p,
				   uint32_t item_id,
		           uint32_t card_type,
				   uint32_t card_set)
{
	int idx = 0;
	pack_h(dbpkgbuf, item_id, idx);
	pack_h(dbpkgbuf, card_type, idx);
	pack_h(dbpkgbuf, card_set, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_insert_card, dbpkgbuf, idx);
}


int db_insert_card_callback(player_t * p,
						    userid_t id,
							void * body,
							uint32_t bodylen,
							uint32_t ret)
{
	CHECK_DBERR(p, ret);
	db_insert_card_rsp_t * pkg = reinterpret_cast<db_insert_card_rsp_t*>(body);

	player_card_t card(pkg->card_id, pkg->card_type, pkg->card_set);
	p->my_packs->del_item(p, pkg->item_id, 1, channel_string_other);

//	ERROR_LOG("INSERT CARD CALLBACK %u %u %u Item %u",
//		   	card.card_id, card.card_set, card.card_type, pkg->item_id);

	PCardSetMapIterator it = p->cardset_info->find(card.card_set);

	if (it != p->cardset_info->end()) {
			player_card_set_t *set = (it->second);
			set->set_card_info->insert(PCardMap::value_type(card.card_id, card));
	} else {
		player_card_set_t * set = new player_card_set_t();
		set->set_id = card.card_set;
		set->set_card_info->insert(PCardMap::value_type(card.card_id, card));
		p->cardset_info->insert(PCardSetMap::value_type(card.card_set, set));
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, pkg->item_id, idx);
	pack(pkgbuf, pkg->card_id, idx);
	pack(pkgbuf, pkg->card_type, idx);
	pack(pkgbuf, pkg->card_set, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
//	ERROR_LOG("INSERT CARD CALLBACK %u %u %u %u", pkg->item_id,
//			 pkg->card_id, pkg->card_type, pkg->card_set);

	return send_to_player(p, pkgbuf, idx, 1);
}

bool can_reward_card_set(player_t * p, uint32_t card_set)
{
	PCardSetMapIterator it = p->cardset_info->find(card_set);
	if (it != p->cardset_info->end()) {
		player_card_set_t *p_set = (it->second);
		card_set_t * set = get_card_set(card_set);
		if (set->card_list->size() == p_set->set_card_info->size()) {
			return true;
		}
	}
	return false;
}

int card_set_reward_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t reward_set = 0;
	unpack(body, reward_set, idx);
	set_reward_t * reward = get_reward(reward_set);
	if (reward && can_reward_card_set(p, reward_set)) { 
		return	db_swap_action_empty(p, reward->swap_id);
	} else {
		ERROR_LOG("ERROR CARD_SET REWARD ID %u %u", p->id, reward_set);
      	return -1; 	
	}	
}


int player_erase_card_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	return 0;
}


int db_erase_card(player_t * p,
				  uint32_t item_id,
		          uint32_t card_id,
				  uint32_t card_type,
				  uint32_t card_set)
{
	return 0;
}

int db_erase_card_callback(player_t *p,
		                   userid_t id,
						   void * body,
						   uint32_t bodylen,
						   uint32_t ret)
{
	return 0;
}


void insert_card_to_set(player_t * p, player_card_t * card) 
{

}

void earse_card_drom_set(player_t *p,
						 uint32_t card_id,
						 uint32_t card_type,
						 uint32_t set_id)
{

}


