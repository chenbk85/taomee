#include <cstdlib>

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/utils.h>

#include "dbproxy.h"
#include "logic.h"
}

#include "mimic_show.hpp"

//----------------------------------------------------------------------
// Private Static Data Members
//
uint32_t  MimicShow::cur_show_     = 0;
uint32_t  MimicShow::cur_show_no_  = 0;
sprite_t* MimicShow::npc_          = 0;
MimicShow::Clothes MimicShow::clothes_[MimicShow::clothes_max_kind];

void MimicShow::check_dress(game_group_t* grp)
{
	static uint8_t buf[sizeof(protocol_t) + 7];

	// not mimic show time
	if (cur_show_ != 0) {
		return;
	}
	int  len; 

	bool match = is_dress_matched(grp);
	// check if clothes match
	if (!match) {
		goto chk_end;
	}	

chk_end:
	// notify player the bonus
	len = sizeof(protocol_t);
	taomee::pack(buf, static_cast<uint8_t>(match), len);
	init_proto_head(buf, mimic_show_bonus_noti, len);
	send_to_group(grp, buf, len);

	if (!match) {
		DEBUG_LOG("MimicShow Mismatched\t[grpid=%u]", grp->id);
		goto end;
	}

	DEBUG_LOG("MimicShow All Matched\t[grpid=%u]", grp->id);
	do_gen_ritm_on_demand(npc_->tiles, 0, 1);
	do_notify_ritm_info(npc_->tiles, 1, &(npc_->tiles->ritem_list), PROTO_RAND_ITEM_INFO);
	// jump to the next show
	next_show();
	if (cur_show_ != 0) {
		// Model Leave Map
		leave_map(npc_, 0);
		ADD_TIMER_EVENT(npc_, next_show, reinterpret_cast<void*>(cur_show_ != 10000),
						get_now_tv()->tv_sec + mimic_game_intvl);
	}

end:
	return;
}

//----------------------------------------------------------------------
// Private Methods
//
int MimicShow::next_show(void* owner, void* data)
{
	if (!data) {
		enter_map(npc_, game_mapid, 0,0);
		set_npc_init_pos();
		change_dress_event();
		response_proto_get_sprite(npc_, PROTO_MAP_ENTER, 0, 0);
		cur_show_ = 0;
	} else if (data == reinterpret_cast<void*>(1)) {
		cur_show_ = 10000;
		ADD_TIMER_EVENT(npc_, next_show, 0,	get_now_tv()->tv_sec + mimic_game_intvl);
	} else {
		cur_show_no_ = (cur_show_no_ % 4) + 1;
		cur_show_ = cur_show_no_;
	}		

	rsp_mimic_show_info();

	return 0;
}

//
void MimicShow::change_npc_dress()
{
	int      itm_cnt = 0;
	uint32_t itms[clothes_max_kind];

	if ((rand() % 100) > 0) {
		for (int i = 0; i != clothes_max_kind; ++i) {
			if ((rand() % 100) < clothes_[i].probability) {
				itms[itm_cnt++] = clothes_[i].clothes[rand() % clothes_[i].cnt];
			}
		}
	}

	chg_dress(npc_, itms, itm_cnt);
}

//
bool MimicShow::is_dress_matched(const game_group_t* grp)
{
	bool match = true;
	// check if clothing match
	for (int i = 0; i != grp->count; ++i) {
		sprite_t* p = grp->players[i];
		if (p->followed && PET_IS_SUPER_LAHM(p->followed)) {
			continue;
		}
		if (p->item_cnt < npc_->item_cnt) {
			match = false;
			break;
		}

		for (int i = 0; i != npc_->item_cnt; ++i) {
			if (!is_wear_item(p, npc_->items[i])) {
				match = false;
				goto ret;
			}
		}
	}

ret:
	return match;
}

void MimicShow::rsp_mimic_show_info(sprite_t* p, bool tomap, bool p_included)
{
	uint8_t buf[256], cnt = 0;

	int idx, len = sizeof(protocol_t);
	taomee::pack(buf, cur_show_, len);
	idx = len + 1;

	if (p) {
		game_group_t* grp = p->group;
		for (int i = 0; i != grp->count; ++i) {
			sprite_t* op = grp->players[i];
			if (op->followed && PET_IS_SUPER_LAHM(op->followed)
					&& ((p != op) || p_included) ) {
				taomee::pack(buf, op->id, idx);
				++cnt;
			}
		}
	}
	*(buf + len) = cnt;
	init_proto_head(buf, PROTO_MIMIC_SHOW_INFO, idx);

	if (p) {
		if (!tomap) {
			send_to_self(p, buf, idx, 1);
		} else {
			send_to_map(p, buf, idx, 0);
		}
	} else {
		send_to_map3(game_mapid, buf, idx);
	}
}


// XML Parser
int MimicShow::clothes_parser(xmlNodePtr cur_node)
{
	int i = 0;

	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar*)"Layer")) {
			DECODE_XML_PROP_INT(clothes_[i].probability, cur_node, "Probability");

			xmlNodePtr children_node = cur_node->xmlChildrenNode;
			while (children_node) {
				if (!xmlStrcmp(children_node->name, (const xmlChar*)"Item")) {
					DECODE_XML_PROP_UINT32(clothes_[i].clothes[clothes_[i].cnt], children_node, "ID");
					if (++(clothes_[i].cnt) > clothes_max_each_kind) {
						ERROR_RETURN(("max item num exceeded: %d", clothes_[i].cnt), -1);
					}
				}
				children_node = children_node->next;
			}

			if (++i > clothes_max_kind) {
				ERROR_RETURN(("max kind num exceeded: %d", i), -1);
			}
		}
		cur_node = cur_node->next;
	}

	return 0;
}
