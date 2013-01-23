#include <cstdio>

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/utils/md5.h>

using namespace taomee;

extern "C"
{
#include <glib.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
#include <async_serv/mcast.h>
	}
#include "player.hpp"
#include "battle.hpp"
#include "room.hpp"
#include "item_bag.hpp"
#include "item.hpp"
#include "player_equip.hpp"

bool player_t::init_player_equip()
{
	head = NULL;
	eye = NULL;
	glass = NULL;
	body = NULL;
	tail = NULL;
	suit = NULL;
	return true;	
}

bool player_t::final_player_equip()
{
	if(head)
	{
		item_factory::get_instance()->destroy_item(head);
		head = NULL;
	}
	if(eye)
	{
		item_factory::get_instance()->destroy_item(eye);
		eye = NULL;
	}
	if(glass)
	{
		item_factory::get_instance()->destroy_item(glass);
		glass = NULL;
	}
	if(body)
	{
		item_factory::get_instance()->destroy_item(body);
		body = NULL;
	}
	if(tail)
	{
		item_factory::get_instance()->destroy_item(tail);
		tail = NULL;
	}
	if(suit)
	{
		item_factory::get_instance()->destroy_item(suit);
		suit = NULL;
	}
	return true;	
}

bool player_t::check_valid_equip_pos(uint32_t pos)
{
	return (pos >= head_equip_pos && pos <= suit_equip_pos);
}

bool player_t::set_player_equip(item* p_item, uint32_t equip_pos)
{
	switch(equip_pos)
	{
		case head_equip_pos:
			head = p_item;
		break;

		case eye_equip_pos:
			eye = p_item;
		break;
		
		case glass_equip_pos:
			glass = p_item;
		break;

		case body_equip_pos:
			body = p_item;
		break;

		case tail_equip_pos:
			tail = p_item;
		break;

		case suit_equip_pos:
			suit = p_item;
		break;

		default:
			return false;
		break;
	}
	if(p_item)
	{
		p_item->set_grid_index(equip_pos);
	}
	return true;
}

item_ptr  player_t::get_equip_by_pos(uint32_t body_index)
{
	switch(body_index)
	{
		case head_equip_pos:
			return head;
		break;

		case eye_equip_pos:
			return eye;
		break;

		case glass_equip_pos:
			return glass;
		break;

		case body_equip_pos:
			return body;
		break;

		case tail_equip_pos:
			return tail;
		break;

		case suit_equip_pos:
			return suit;
		break;

		default:
			return NULL;
		break;
	}
	return NULL;
}

int cli_proto_get_equips(DEFAULT_ARG)
{
	return send_player_equips(p);	
}

int send_player_equips(player_t* p)
{
	item* equips[] = {p->head, p->eye, p->glass, p->body, p->tail, p->suit};

	uint32_t  count = 0;
	int idx = sizeof(cli_proto_t);
	int idx2 = idx;

	pack(pkgbuf, count, idx);

	for( uint32_t i =0; i<  sizeof(equips)/sizeof(equips[0]); i++)
	{
		item* p_item = equips[i];
		if(p_item == NULL)continue;

		pack(pkgbuf, p_item->id_, idx);
		pack(pkgbuf, p_item->p_data->item_id_, idx);
		pack(pkgbuf, p_item->cur_heap_count_, idx);
		pack(pkgbuf, p_item->bag_index_, idx);
		pack(pkgbuf, p_item->get_time_, idx);
		pack(pkgbuf, p_item->expire_time_, idx);
		pack(pkgbuf, p_item->hide, idx);
		count++;
	}

	pack(pkgbuf, count, idx2);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, (char*)pkgbuf, idx, 1);
}

int cli_proto_wear_equip(DEFAULT_ARG)
{
	cli_proto_wear_equip_in* p_in = P_IN;

	item* p_item =  p->bag->get_item_by_grid(p_in->grid_index);

	if(p_item == NULL)return 0;

	if( !p_item->get_item_data()->check_category_flag(equip_item_category) )return 0;	

	return db_wear_equip(p, p_in->grid_index, p_item->equip_pos(), p_in->hide);
}

int cli_proto_remove_equip(DEFAULT_ARG)
{
	cli_proto_remove_equip_in* p_in = P_IN;

	item* p_equip = p->get_equip_by_pos(p_in->equip_pos);
	
	if(p_equip == NULL)return 0;

	if(!p_equip->get_item_data()->check_category_flag(equip_item_category) )return 0;

	if( !p->bag->check_bag_index_empty(p_in->empty_bag_index) )return 0;

	return db_remove_equip(p, p_in->equip_pos, p_in->empty_bag_index);
}

int cli_proto_batch_equips_opt(DEFAULT_ARG)
{
	cli_proto_batch_equips_opt_in* p_in = P_IN;

	if( p_in->equip_opts.size() == 0)return 0;

	for(uint32_t i =0; i< p_in->equip_opts.size(); i++)
	{
		equip_change_t* opt = &(p_in->equip_opts[i]);
		if (opt->empty_bag_index == 0) {//此件装备只有隐藏属性改变
			continue;
		}
		if(opt->body_index == opt->empty_bag_index)return 0;
		if( !p->bag->check_valid_grid_index(opt->empty_bag_index) )return 0;
		if( !p->check_valid_equip_pos(opt->body_index))return 0;
	}

	return db_batch_equips_opt(p, p_in->equip_opts);
}



