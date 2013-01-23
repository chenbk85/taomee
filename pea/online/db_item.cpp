#include <libtaomee++/inet/pdumanip.hpp>
extern "C"
{
#include <arpa/inet.h>
#include <libtaomee/timer.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/project/utilities.h>
}

#include "proto.hpp"
#include "pea_common.hpp"
#include "cli_proto.hpp"
#include "db_item.hpp"
#include "item_bag.hpp"
#include "login.hpp"
#include "player_equip.hpp"
#include "db_pet.hpp"
#include "db_mail.hpp"
#include "prize.hpp"
#include "player_attribute.hpp"

using namespace taomee;

bool parse_db_add_item_request(player_t* p, std::vector<parse_item>& items, std::vector<db_add_item_request_t>& request)
{
	uint32_t future_grid_count[1024] = {0};
	int32_t remain_count = 0;

	for(uint32_t m =0; m< items.size(); m++)
	{
		uint32_t item_id = items[m].item_id;
		uint32_t item_count = items[m].item_count;
		item_data* p_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id);
		if(p_data == NULL)
		{	
			request.clear();
			return false;
		}

		item* p_item =  p->bag->get_first_item(item_id);
		remain_count = item_count;

		while(p_item)
		{
			uint32_t count = p_item->get_cur_remain_heap_count() - future_grid_count[ p_item->grid_index()];
			if(count == 0)break;

			db_add_item_request_t info;
			info.grid_index = p_item->grid_index();
			info.item_id =    p_item->item_id();
			info.item_count = count > (uint32_t)remain_count ? remain_count : count;
			info.get_time =   p_item->get_time();
			info.expire_time = items[m].expire_time;
			future_grid_count[ p_item->grid_index() ] += info.item_count;
			request.push_back(info);

			remain_count -= count;
			p_item = p->bag->get_next_item(p_item);
			if(remain_count <= 0)break;
		}

		if(remain_count <= 0)continue;

		for(uint32_t i = 1; i< p->bag->max_grid_count; i++)
		{
			if( !p->bag->check_bag_index_empty(i))continue;

			if( future_grid_count[i] >= p_data->max_heap_num_ )continue;

			uint32_t count = p_data->max_heap_num_ - future_grid_count[i];

			db_add_item_request_t info;
			info.grid_index = i;
			info.item_id =    p_data->item_id_;
			info.item_count = count > (uint32_t)remain_count ? remain_count : count;
			info.get_time =  time(NULL);
			info.expire_time = items[m].expire_time;
			future_grid_count[i] += count;
			request.push_back(info);

			remain_count -= count;
			if(remain_count <= 0)break;
		}
	}

	if(remain_count > 0)
	{
		request.clear();
		return false;
	}
	return true;
}

bool parse_db_add_item_request(player_t* p, uint32_t item_id, uint32_t item_count, uint32_t expire_time, std::vector<db_add_item_request_t>& request)
{
	item_data* p_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id);
	if(p_data == NULL)return false;

	item* p_item =  p->bag->get_first_item(item_id);
	int32_t  remain_count = item_count;
	
	while(p_item)
	{
		uint32_t count = p_item->get_cur_remain_heap_count();
		if(count == 0)break;

		db_add_item_request_t info;
		info.grid_index = p_item->grid_index();
		info.item_id =    p_item->item_id();
		info.item_count = count > (uint32_t)remain_count ? remain_count : count;
		info.get_time =   p_item->get_time();
		info.expire_time = expire_time;	
		request.push_back(info);

		remain_count -= count;
		p_item = p->bag->get_next_item(p_item);
		if(remain_count <= 0)
		{
			return true;
		}
	}

	for(uint32_t i = 1; i< p->bag->max_grid_count; i++)
	{
		if( !p->bag->check_bag_index_empty(i))continue;
		
		db_add_item_request_t info;
		info.grid_index = i;
		info.item_id =    p_data->item_id_;
		info.item_count = p_data->max_heap_num_ > (uint32_t)remain_count ? remain_count : p_data->max_heap_num_;
		info.get_time =  time(NULL);
		info.expire_time = expire_time;
		request.push_back(info);
	
		remain_count -= p_data->max_heap_num_;
		if(remain_count <= 0)
		{
			return true;	
		}
	}

	request.clear();
	return false;
}

/*----------------------------------------------------------------------------*/

bool parse_db_del_item_request(player_t* p, uint32_t item_id, uint32_t item_count, std::vector<db_del_item_request_t>& request)
{
	item_data* p_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id);
	if(p_data == NULL)return false;
	
	item* p_item =  p->bag->get_first_item(item_id);
	int32_t  remain_count = item_count;

	while(p_item)
	{
		db_del_item_request_t info;	

		info.grid_index = p_item->grid_index();
		info.item_id    = p_item->item_id();
		info.item_count = p_item->get_cur_heap_count() < (uint32_t)remain_count ? p_item->get_cur_heap_count() : remain_count;
		request.push_back(info);

		remain_count -= p_item->get_cur_heap_count();
		if(remain_count <= 0)
		{
			return true;	
		}
		p_item = p->bag->get_next_item(p_item);	
	}
	
	request.clear();
	return false;
}

/*--------------------------------------------------------------------------*/

int db_get_player_items(player_t* p)
{
	db_proto_get_player_items_in in;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;
	
	return send_to_db(p, db_proto_get_player_items_cmd, &in);
}

/*----------------------------------------------------------------------*/

int db_proto_get_player_items_callback(DEFAULT_ARG)
{
	db_proto_get_player_items_out* p_in = P_IN;		

	for(uint32_t i =0; i< p_in->items.size(); i++)
	{
		db_item_info_t* p_item_info = &(p_in->items[i]);
		
		item* p_item = item_factory::get_instance()->create_item(p_item_info->id, 
																 p_item_info->item_id, 
																 p_item_info->item_count, 
																 p_item_info->grid_index,
																 p_item_info->get_time,
																 p_item_info->expire_time);
		p->bag->set_grid_item(p_item, p_item->grid_index());	
	}


    p->set_module(MODULE_BAG);

	return process_login(p);
}

/*---------------------------------------------------------------------------*/

int db_add_item(player_t* p, std::vector<db_add_item_request_t>& request)
{
	if( request.size() == 0)return 0;

	db_proto_add_item_in in;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;
	in.add_items = request;	

	return send_to_db(p, db_proto_add_item_cmd, &in);
}

/*---------------------------------------------------------------------------*/


int db_proto_add_item_callback(DEFAULT_ARG)
{
	db_proto_add_item_out* p_in = P_IN;     

    //先增加物品再说
    for(uint32_t i=0; i< p_in->add_items.size(); i++) {
        db_add_item_reply_t* reply = &p_in->add_items[i];	
        p->bag->add_item_by_grid(reply);	
    }

    /*如果是因为奖励而增加item, 需要跳回到增加奖励函数*/
    if (p->process_prize_state == true) {
        return get_prize(p, p->cache_prize_id, p->cache_prize_seq);
    }   

    send_header_to_player(p, cli_proto_add_item_cmd, 0, 1);

    /*如果只是单纯增加item,则通知客户端增加item*/
	for(uint32_t i=0; i< p_in->add_items.size(); i++) {
		db_add_item_reply_t* reply = &p_in->add_items[i];	
		notify_player_add_item(p, reply);
	}
	return 0;
}

/*--------------------------------------------------------------------------*/
int db_del_item(player_t* p, std::vector<db_del_item_request_t>& request)
{
	if( request.size() == 0)return 0;

	db_proto_del_item_in in;
	
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;

	in.del_items = request;

	return send_to_db(p, db_proto_del_item_cmd, &in);
}

/*--------------------------------------------------------------------------*/

int db_proto_del_item_callback(DEFAULT_ARG)
{
	db_proto_del_item_out* p_in = P_IN;

    send_header_to_player(p, cli_proto_del_item_cmd, 0, 1);

	for(uint32_t i=0; i< p_in->del_items.size(); i++)
	{
		db_del_item_reply_t* reply = &p_in->del_items[i];
		p->bag->del_item_by_grid(reply);
		notify_player_del_item(p, reply);
	}
	return 0;	
}

/*--------------------------------------------------------------------------------*/

int db_proto_split_item_callback(DEFAULT_ARG)
{
	return 0;	
}

/*--------------------------------------------------------------------------------*/

int db_get_player_equips(player_t* p)
{
	db_proto_get_player_items_in in;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;

	return send_to_db(p, db_proto_get_player_equips_cmd, &in);
}

/*--------------------------------------------------------------------------------*/

int db_proto_get_player_equips_callback(DEFAULT_ARG)
{
	db_proto_get_player_equips_out* p_in = P_IN;

	for(uint32_t i =0; i< p_in->equips.size(); i++)
	{
		db_equip_info_t* p_item_info = &(p_in->equips[i]);

		item* p_item = item_factory::get_instance()->create_item(p_item_info->id,
				p_item_info->item_id,
				p_item_info->item_count,
				p_item_info->grid_index,
				p_item_info->get_time,
				p_item_info->expire_time);

		p->set_player_equip(p_item, p_item->grid_index());
		p_item->hide = p_item_info->hide;//初始化所有装备都显示
	}

    p->set_module(MODULE_EQUIP);


    return process_login(p);
}

/*--------------------------------------------------------------------------------*/

int  db_wear_equip(player_t* p, uint32_t bag_index, uint32_t body_index, uint8_t hide)
{
	db_proto_wear_equip_in in;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;

	in.bag_index = bag_index;
	in.body_index = body_index;
	in.hide = hide;
	return send_to_db(p, db_proto_wear_equip_cmd, &in);
}

/*--------------------------------------------------------------------------------*/

int  db_proto_wear_equip_callback(DEFAULT_ARG)
{
	db_proto_wear_equip_out *p_in = P_IN;
	item* p_item = p->bag->get_item_by_grid(p_in->bag_index);
	item* p_equip = p->get_equip_by_pos(p_in->body_index);
	p_item->clear_to_item();
	p->bag->set_grid_item(p_equip, p_in->bag_index);
	p->set_player_equip(p_item, p_in->body_index);
	
	int idx = sizeof(cli_proto_t);
	
	pack(pkgbuf, p_in->bag_index, idx);
	pack(pkgbuf, p_in->body_index, idx);
	pack(pkgbuf, p_in->hide, idx);
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, (char*)pkgbuf, idx, 1);
	return broadcast_player_equips_change(p);
}

/*--------------------------------------------------------------------------------*/

int  db_remove_equip(player_t* p, uint32_t equip_pos, uint32_t bag_index)
{
	db_proto_remove_equip_in in;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;

	in.equip_pos = equip_pos;
	in.bag_empty_index = bag_index;
	return send_to_db(p, db_proto_remove_equip_cmd, &in);
}

/*--------------------------------------------------------------------------------*/

int  db_proto_remove_equip_callback(DEFAULT_ARG)
{
	db_proto_remove_equip_out *p_in = P_IN;
	
	item* p_equip = p->get_equip_by_pos(p_in->equip_pos);
	p->bag->set_grid_item(p_equip, p_in->bag_empty_index);
	p->set_player_equip(NULL, p_in->equip_pos);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p_in->equip_pos, idx);
	pack(pkgbuf, p_in->bag_empty_index, idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, (char*)pkgbuf, idx, 1);	
	return broadcast_player_equips_change(p);
}

/*--------------------------------------------------------------------------------*/

int  db_batch_equips_opt(player_t* p, std::vector<equip_change_t>& equip_opts)
{
	db_proto_batch_equips_opt_in in;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;

	in.equip_opts = equip_opts;

	return send_to_db(p, db_proto_batch_equips_opt_cmd, &in);
}

/*--------------------------------------------------------------------------------*/

int  db_proto_batch_equips_opt_callback(DEFAULT_ARG)
{
	db_proto_batch_equips_opt_out *p_in = P_IN;

	for(uint32_t i =0; i< p_in->equip_opts.size(); i++)
	{
		equip_change_t* opt = &p_in->equip_opts[i];	
		item* p_item = p->bag->get_item_by_grid(opt->empty_bag_index);
		item* p_equip = p->get_equip_by_pos( opt->body_index);
	
		if (opt->empty_bag_index == 0) {//只换隐藏属性
			if (p_equip != NULL) {
				p_equip->hide = opt->hide;
			}	
			continue;
		}

		if(p_item == NULL && p_equip != NULL)//脱装备
		{
			p->bag->set_grid_item(p_equip, opt->empty_bag_index);
			p->set_player_equip(NULL, opt->body_index);
		}
		if(p_item != NULL)//穿装备
		{	
			p_item->clear_to_item();
			p->bag->set_grid_item(p_equip, opt->empty_bag_index);
			p->set_player_equip(p_item, opt->body_index);
			p_item->hide = opt->hide;
		}
	}
	

	int count = p_in->equip_opts.size();
	int idx = sizeof(cli_proto_t);
	
	pack(pkgbuf, count, idx);
	for(uint32_t i =0; i< p_in->equip_opts.size(); i++)
	{
		equip_change_t* opt = &p_in->equip_opts[i];
		pack(pkgbuf, opt->body_index, idx);
		pack(pkgbuf, opt->empty_bag_index, idx);
		pack(pkgbuf, opt->hide, idx);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, (char*)pkgbuf, idx, 1);
		
	return broadcast_player_equips_change(p);
}





int db_proto_buy_npc_shop_item_callback(DEFAULT_ARG)
{
    db_proto_buy_npc_shop_item_out * p_in = P_IN;
    uint32_t gold = p_in->gold;

    // 扣减gold
    p->add_player_attr(OBJ_ATTR_GOLD, -gold);
    notify_player_gold(p);


    // 添加
    for (uint32_t i = 0; i < p_in->add_items.size(); i++)
    {
		db_add_item_reply_t* reply = &p_in->add_items[i];
		p->bag->add_item_by_grid(reply);
		notify_player_add_item(p, reply);
    }

    return send_to_player(p, p->bind_cmd->p_out, p->waitcmd, 1);
}



int db_proto_npc_item_callback(DEFAULT_ARG)
{
    db_proto_npc_item_out * p_in = P_IN;

    uint32_t gold = p_in->gold;

    // 增加gold
    p->add_player_attr(OBJ_ATTR_GOLD, gold);;
    notify_player_gold(p);

    // 扣减item
    for (uint32_t i = 0; i < p_in->del_items.size(); i++)
    {
		db_del_item_reply_t* reply = &p_in->del_items[i];

        item_ptr p_item = p->bag->get_item_by_grid(reply->grid_index);
        if (NULL != p_item)
        {
            item_ptr new_item = 
                item_factory::get_instance()->create_item(
                        p_item->id_, 
                        p_item->item_id(), 
                        reply->item_count,
                        p_item->grid_index(),
                        p_item->get_time(),
                        p_item->get_expire_time());
            p->redeem_bag->add_item(new_item);
            item_factory::get_instance()->destroy_item(new_item);

            p->bag->del_item_by_grid(reply);
            notify_player_del_item(p, reply);
        }
    }

    return send_to_player(p, p->bind_cmd->p_out, p->waitcmd, 1);
}


int db_proto_redeem_item_callback(DEFAULT_ARG)
{
    db_proto_redeem_item_out * p_in = P_IN;

    uint32_t redeem_index = p_in->redeem_index;
    uint32_t gold = p_in->gold;

    // 扣减gold
    p->add_player_attr(OBJ_ATTR_GOLD, -gold);;
    notify_player_gold(p);


    // 添加
    for (uint32_t i = 0; i < p_in->add_items.size(); i++)
    {
		db_add_item_reply_t* reply = &p_in->add_items[i];
		p->bag->add_item_by_grid(reply);
		notify_player_add_item(p, reply);
    }


    p->redeem_bag->del_item(redeem_index);

    return send_to_player(p, p->bind_cmd->p_out, p->waitcmd, 1);
}
