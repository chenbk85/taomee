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
#include "battle_switch.hpp"
#include "room.hpp"
#include "item_bag.hpp"
#include "item.hpp"
#include "mail.hpp"
#include "task.hpp"

uint32_t player_num;
GHashTable*  all_players;
std::map<uint32_t, player_t*> all_uid_players;

player_t* alloc_player(uint32_t uid, fdsession_t* fdsess)
{
	player_t* p 	= reinterpret_cast<player_t*>(g_slice_alloc0(sizeof(player_t)));	
	p->fd       	= fdsess->fd;
	p->fdsess   	= fdsess;
	p->id       	= uid;
	p->pkg_queue  	= g_queue_new();

	p->exp2level_calc  = exp2level_factory::getInstance()->get_calculator(PLAYER_CALC_TYPE);

	INIT_LIST_HEAD(&p->wait_cmd_player_hook);
	INIT_LIST_HEAD(&p->timer_list);
	INIT_LIST_HEAD(&p->maphook);

	p->cur_map = NULL;
	p->x_pos = 0;
	p->y_pos = 0;
	p->cur_map_line_index = -1;

	p->battle_grp = NULL;
	memset(&(p->room), 0, sizeof(p->room));

	p->init_attr();
    p->init_extra_info();
	p->init_player_equip();
	p->init_player_map();
    init_player_pet(p);
	init_player_mail(p);
	init_player_task(p);
	
    p->friends.clear();
	p->blacklist.clear();
    p->player_prize.clear();

	g_hash_table_insert(all_players, &(p->fd), p);
	all_uid_players[uid] = p;

	++player_num;
	return p;
}

void dealloc_player(player_t* p)
{
	cached_pkg_t* pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(p->pkg_queue));
	while(pkg) {
		g_slice_free1(pkg->len, pkg);
		pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(p->pkg_queue));
	}
	list_del_init(&(p->wait_cmd_player_hook));
	list_del_init(&p->timer_list);
	if(p->check_in_map()) {
		broadcast_player_leave_map(p, p->cur_map, p->cur_map_line_index);
		leave_map(p);
	}
	g_queue_free(p->pkg_queue);

	p->cur_map = NULL;
	p->x_pos = 0;
	p->y_pos = 0;
	p->cur_map_line_index = -1;
	battle_grp_t* grp = p->battle_grp;
	if (grp) {
		btlsvr_player_cancel_battle(p);
		free_battle_grp(p->battle_grp);
		p->battle_grp = NULL;
	}
	
	if (is_player_in_hall(p)) {
		send_to_battle_switch(p, btlsw_player_exit_hall_cmd, 0);
	}

	final_player_item_bag(p);
    final_player_pet(p);
	final_player_mail(p);
	final_player_task(p);

	p->final_attr();
    p->final_extra_info();
	p->final_player_equip();
	p->final_player_map();
    final_player_pet(p);

	g_hash_table_remove(all_players, &(p->fd));
	all_uid_players.erase(p->id);

	--player_num;
}

bool player_t::init_attr()
{
	merge_attr = new c_player_attr();
	return true;
}

bool player_t::final_attr()
{
	if (merge_attr)
	{
		delete merge_attr;
		merge_attr = 0;
	}
	return true;
}

uint32_t player_t::get_player_attr_value(uint16_t attr_type)
{
	return merge_attr->get_player_attr_by_type(attr_type);
}

uint32_t player_t::get_merge_attr_value(uint16_t attr_type)
{
	//return get_player_attr_value(attr_type) + fight_pet->get_attr_value(attr_type);
	return merge_attr->get_merge_attr_by_type(attr_type);
}

void	player_t::set_player_attr_value(uint32_t attr_type, uint32_t value)
{
	merge_attr->set_player_attr_by_type(attr_type, value);
}

void	player_t::add_player_attr(uint32_t attr_type, int value)
{
	merge_attr->add_player_attr(attr_type, value);
}

bool player_t::init_extra_info()
{
    extra_info = new extra_info_t();
    return true;
}


bool player_t::final_extra_info()
{
    if (extra_info)
    {
        delete extra_info;
        extra_info = NULL;
    }

    return true;

}


player_t* get_player_by_fd(int fd)
{
	return reinterpret_cast<player_t*>(g_hash_table_lookup(all_players, &fd));	
}

player_t* get_player(userid_t uid)
{
    std::map<uint32_t, player_t*>::iterator pItr = all_uid_players.find(uid);
	if(pItr == all_uid_players.end())return NULL;
	return pItr->second;
	// player_t* p;	
	// list_for_each_entry (p, &uid_buckets[uid % c_bucket_size], playerhook)
	// {
		// if (p->id == uid) {
			// return p;
		// }	
	// }
	// return NULL;
}

player_t* add_player(player_t* player)
{
	player_t* p = alloc_player(player->id, player->fdsess);
	p->server_id = player->server_id;
	p->seqno = player->seqno;
	p->waitcmd = player->waitcmd;
	return p;
}

void del_player(player_t* p)
{	
	dealloc_player(p);	
}

void init_players()
{
	all_players = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_player);
    all_uid_players.clear();

}

void final_players()
{
	g_hash_table_destroy(all_players);
    all_uid_players.clear();
}

void free_player(void* p)
{
    if (NULL != p)
    {
        g_slice_free1(sizeof(player_t), p);	
    }
}


uint32_t player_t::get_cur_map_id()
{
	return cur_map ? cur_map->id : 0;	
}


map_line_players* player_t::get_cur_map_line()
{
	if (cur_map == NULL)
    {
        return NULL;
    }

	if ( ! VALID_MAP_LINE(cur_map_line_index) )
    {
        return NULL;

    }

	return &(cur_map->line_players[cur_map_line_index]);
}


bool player_t::init_player_map()
{
	cur_map = NULL;
	cur_map_line_index = -1;
	x_pos = 0;
	y_pos = 0;

	last_map_id = 0;
	last_map_x = 0;
	last_map_y = 0;

	return true;
}

bool player_t::final_player_map()
{
	cur_map = NULL;
	cur_map_line_index = -1;
	x_pos = 0;
	y_pos = 0;

	last_map_id = 0;
	last_map_x = 0;
	last_map_y = 0;

	return true;	
}


void player_t::reset_map()
{
	cur_map = NULL;
	cur_map_line_index = -1;
	x_pos = 0;
	y_pos = 0;
}

bool player_t::check_in_map()
{
	return (cur_map != NULL && VALID_MAP_LINE(cur_map_line_index));
}


void player_t::export_db_user_id(db_user_id_t * p_db_user_id)
{
    p_db_user_id->user_id = id;
    p_db_user_id->role_tm = role_tm;
    p_db_user_id->server_id = server_id;

}


void player_t::init_player_attr()
{
	attr_config_data* p_attr = 
		attr_config_data_mgr::instance().get_object(get_player_attr_value(OBJ_ATTR_LEVEL));
	//不设置经验，金币等
    merge_attr->set_player_attr(*p_attr);

	if (fight_pet) {
		merge_attr->set_pet_attr(fight_pet->m_attr);
	}
	merge_attr->calc_merge_attr();
}
