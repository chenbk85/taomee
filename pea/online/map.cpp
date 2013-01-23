extern "C" 
{
#include <glib.h>
}
#include "proto.hpp"
#include "cli_proto.hpp"
#include "player.hpp"
#include "map.hpp"

using namespace taomee;


static list_head_t all_maps[c_bucket_size];



void pack_map_pet(player_t * p, int & idx);


bool init_maps()
{
	for(int i =0; i< c_bucket_size; i++)
	{
		INIT_LIST_HEAD(&all_maps[i]);	
	}

	map<uint32_t, map_data*>::iterator pItr = map_data_mgr::get_instance()->datas.begin();
	for(;  pItr != map_data_mgr::get_instance()->datas.end(); ++pItr)
	{
		map_data* p_data = pItr->second;
		if(p_data == NULL)continue;
		map_t* p_map = alloc_map(p_data->map_id);	
		p_map->data = p_data;
	}
	
	return true;	
}

bool final_maps()
{
	list_head_t* l = NULL;
	list_head_t* p = NULL;

	for(int i=0; i < c_bucket_size; ++i)
	{
		list_for_each_safe(l, p, &all_maps[i])
		{
			map_t* m = list_entry(l, map_t, hash_hook);
			dealloc_map(m);
		}
	}
	for(int i=0; i < c_bucket_size; ++i)
	{
		list_del_init(&all_maps[i]);	
	}
	return true;	
}

map_t* alloc_map(uint32_t map_id)
{
	map_t* m = reinterpret_cast<map_t*>(g_slice_alloc0(sizeof(*m)));
	
	m->id = map_id;
	for(int i=0; i< MAX_MAP_LINE; i++)
	{
		INIT_LIST_HEAD(&m->line_players[i].map_line_playerlist);
		m->line_players[i].cur_player_count = 0;
	}
	list_add_tail(&m->hash_hook, &all_maps[map_id % c_bucket_size]);

	return m;
}

void   dealloc_map(map_t* m)
{
	list_del(&m->hash_hook);
	for(int i =0; i< MAX_MAP_LINE; i++)
	{
		list_del_init(&m->line_players[i].map_line_playerlist);
		m->line_players[i].cur_player_count = 0;
	}
	g_slice_free1(sizeof(*m), m);
}

map_t* get_map(uint32_t map_id)
{
	map_t* m = NULL;
	list_for_each_entry(m, &all_maps[map_id % c_bucket_size], hash_hook) 
	{
		if (m->id == map_id) 
		{
			return m;
		}
	}
	return NULL;
}

map_line_players* find_idle_map_line(map_t* m, int32_t& map_line_index)
{
	map_line_index = -1;
	for(int i=0; i< MAX_MAP_LINE; i++)
	{
		if(m->line_players[i].cur_player_count < MAX_MAP_PLAYER_COUNT )
		{
			map_line_index = i;
			return &(m->line_players[i]);
		}
	}
	return NULL;
}

int    enter_map(player_t* p, uint32_t mapid, uint32_t x, uint32_t y)
{
	map_t* m = get_map(mapid);
	if(m == NULL)
	{
		ERROR_TLOG("enter map can not find map_id = %u uid = %u", mapid, p->id);	
		return -1;
	}
	if( p->get_cur_map_id() == mapid)
	{
		ERROR_TLOG("enter the same map again map_id = %u uid = %u", mapid, p->id);
		return -1;
	}

	int map_line_index = -1;
	map_line_players* map_line = find_idle_map_line(m, map_line_index);
	if (map_line == NULL)
	{
		ERROR_TLOG("enter map line full map_id = %u uid = %u", mapid, p->id);
		return -1;
	}

	if( p->cur_map)
	{
		leave_map(p);
	}

	if( x * y == 0)
	{
		p->x_pos = m->data->born_x;
		p->y_pos = m->data->born_y;
	}
	else
	{
		p->x_pos = x;
		p->y_pos = y;
	}

    list_add_tail(&(p->maphook), &map_line->map_line_playerlist);
	map_line->cur_player_count++;
	p->cur_map = m;
	p->cur_map_line_index = map_line_index;
	return 0;
}

int    leave_map(player_t* p)
{
	map_t* m = p->cur_map;
	if( m == NULL)
	{
		ERROR_TLOG("leave map player not in map uid = %u", p->id);
		return -1;
	}
	
	if ( ! VALID_MAP_LINE(p->cur_map_line_index) )
	{
		ERROR_TLOG("leave map invalid map_line_index map_id = %u map_line_index = %d, uid = %u ",
			m->id, p->cur_map_line_index, p->id);
		return -1;
	}
	
	list_del(&p->maphook);
	m->line_players[p->cur_map_line_index].cur_player_count--;
    p->reset_map();
	return 0;
}

int change_map_line(player_t* p, int map_line)
{
	map_t* m = p->cur_map;
	if( m == NULL)
	{
		ERROR_TLOG("change map line player not in map uid = %u", p->id);	
		return -1;
	}	
	if( ! VALID_MAP_LINE(map_line))
	{
		ERROR_TLOG("change map line invalid map line uid = %u, map_line = %d", p->id, map_line);
		return -1;
	}
	
	if( ! VALID_MAP_LINE(p->cur_map_line_index) )
	{
		ERROR_TLOG("change map line invalid current map line uid = %u, map_line = %d", 
				p->id, p->cur_map_line_index );
		return -1;
	}

	if( p->cur_map_line_index == map_line)
	{
		ERROR_TLOG("change map line player in same map line uid = %u, map_line = %d", p->id, map_line);
		return -1;
	}
	list_del(&p->maphook);
	m->line_players[p->cur_map_line_index].cur_player_count --;

    p->cur_map_line_index = map_line;

	list_add_tail(&(p->maphook), &(m->line_players[p->cur_map_line_index].map_line_playerlist));	
	m->line_players[p->cur_map_line_index].cur_player_count ++;
	return 0;
}


int    broadcast_player_enter_map(player_t* p, map_t* m, uint32_t cur_line_index)
{
	int idx = sizeof(cli_proto_t);
	
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, p->role_tm, idx);
    pack(pkgbuf, p->eye_model, idx);
	pack(pkgbuf, p->resource_id, idx);
	pack(pkgbuf, p->nick, sizeof(p->nick), idx);
	pack(pkgbuf, m->id, idx);
	pack(pkgbuf, cur_line_index, idx);
	pack(pkgbuf, p->x_pos, idx);
	pack(pkgbuf, p->y_pos, idx);

	uint32_t equip_count = 0;
	int idx2 = idx;
	pack(pkgbuf, equip_count, idx);

	item* equips[] = {p->head, p->eye, p->glass, p->body, p->tail, p->suit};
	for(uint32_t i =0; i < sizeof(equips)/sizeof(equips[0]); i++)
	{
		item* p_item = equips[i];	
		if(p_item == NULL)continue;

		pack(pkgbuf, p_item->item_id(), idx);
		equip_count++;
	}

    pack_map_pet(p, idx);

	pack(pkgbuf, equip_count, idx2);

	init_cli_proto_head(pkgbuf, p, cli_proto_enter_map_cmd, idx);

	map_line_players* p_line = m->get_line_players(cur_line_index);

	list_head_t * l;
	list_for_each (l, &(p_line->map_line_playerlist))
	{
		player_t * p_player = list_entry(l, player_t, maphook);	
		send_to_player(p_player, (char*)pkgbuf, idx, 0);
	}
	return 0;
}

int    broadcast_player_leave_map(player_t* p, map_t* m, uint32_t cur_line_index)
{
	int idx = sizeof(cli_proto_t);

	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, m->id, idx);
	pack(pkgbuf, cur_line_index, idx);

	init_cli_proto_head(pkgbuf, p, cli_proto_leave_map_cmd, idx);
	
	map_line_players* p_line = m->get_line_players(cur_line_index);

	list_head_t * l;
	list_for_each (l, &(p_line->map_line_playerlist))
	{
		player_t * p_player = list_entry(l, player_t, maphook);
		send_to_player(p_player, (char*)pkgbuf, idx, 0);
	}
	return 0;
}

player_t* get_player_in_map(map_t* m,  uint32_t cur_line_index, uint32_t userid)
{
	map_line_players* p_line = m->get_line_players(cur_line_index);

	list_head_t * l;
	list_for_each (l, &(p_line->map_line_playerlist))
	{
		player_t * p_player = list_entry(l, player_t, maphook);
		if(p_player->id == userid)
		{
			return p_player;	
		}
	}
	return NULL;
}

int cli_proto_map_player_move(DEFAULT_ARG)
{
	cli_proto_map_player_move_in* p_in = P_IN;

	p->clear_waitcmd();
	
	p->x_pos = p_in->x;
	p->y_pos = p_in->y;

	if(p->check_in_battle())return 0;

	if(!p->check_in_map())return 0;

	return broadcast_player_move_map(p, p->cur_map, p_in->x, p_in->y, p_in->dir);
}

int cli_proto_enter_map(DEFAULT_ARG)
{
	cli_proto_enter_map_in* p_in = P_IN;

	p->clear_waitcmd();

	enter_map(p, p_in->map_id, p_in->map_x, p_in->map_y);

	return broadcast_player_enter_map(p, p->cur_map, p->cur_map_line_index);
}


int cli_proto_leave_map(DEFAULT_ARG)
{
	p->clear_waitcmd();

	if(!p->check_in_map())
	{
		ERROR_TLOG("player call leave map but player not in map uid= %u", p->id);
		return -1;
	}	

	broadcast_player_leave_map(p, p->cur_map, p->cur_map_line_index);

	return leave_map(p);
}

int cli_proto_get_map_user_list(DEFAULT_ARG)
{
	p->clear_waitcmd();

	if(!p->check_in_map())
	{
		ERROR_TLOG("player get map user list but not in map uid = %u", p->id);
		return -1;
	}

	int idx = sizeof(cli_proto_t);
	uint32_t count = 0;
	int idx2 = idx;
	pack(pkgbuf, count, idx);

	map_line_players* p_line = p->cur_map->get_line_players(p->cur_map_line_index);
	list_head_t * l;
	list_for_each (l, &(p_line->map_line_playerlist))
	{
		player_t * p_player = list_entry(l, player_t, maphook);
		
		pack(pkgbuf, p_player->id, idx);
		pack(pkgbuf, p_player->role_tm, idx);
        pack(pkgbuf, p_player->eye_model, idx);
		pack(pkgbuf, p_player->resource_id, idx);
		pack(pkgbuf, p_player->nick, sizeof(p_player->nick), idx);
		pack(pkgbuf, p_player->x_pos, idx);
		pack(pkgbuf, p_player->y_pos, idx);

		uint32_t equip_count = 0;
		int idx3 = idx;
		pack(pkgbuf, equip_count, idx);

		item* equips[] = {p->head, p->eye, p->glass, p->body, p->tail, p->suit};
		for(uint32_t i =0; i < sizeof(equips)/sizeof(equips[0]); i++)
		{
			item* p_item = equips[i];
			if(p_item == NULL)continue;

			pack(pkgbuf, p_item->item_id(), idx);
			equip_count++;
		}
		pack(pkgbuf, equip_count, idx3);

        pack_map_pet(p_player, idx);

		count++;
	}
	pack(pkgbuf, count, idx2);
	init_cli_proto_head(pkgbuf, p, cli_proto_get_map_user_list_cmd, idx);

	return send_to_player(p, (char*)pkgbuf, idx, 1);
}

int cli_proto_change_map_line(DEFAULT_ARG)
{
	
	return 0;	
}

int cli_proto_get_map_user_detail_info(DEFAULT_ARG)
{
	cli_proto_get_map_user_detail_info_in* p_in = P_IN;

	p->clear_waitcmd();

	if(!p->check_in_map())
	{
		ERROR_TLOG("player: %u not in map when query one's info", p->id);
		return -1;
	}
		
	player_t* obj = get_player_in_map(p->cur_map, p->cur_map_line_index, p_in->userid);
	
	if(obj == NULL)
	{
		ERROR_TLOG("the uid %u can not find %u in map %u", p->id, p_in->userid, p->cur_map->id);
		return -1;
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, p->nick, sizeof(p->nick), idx);
	pack(pkgbuf, p->get_player_attr_value(OBJ_ATTR_LEVEL), idx);
	pack(pkgbuf, p->get_player_attr_value(OBJ_ATTR_HP), idx);
	pack(pkgbuf, p->get_player_attr_value(OBJ_ATTR_MAGIC), idx);
	pack(pkgbuf, p->get_player_attr_value(OBJ_ATTR_AGILITY), idx);
	pack(pkgbuf, p->get_player_attr_value(OBJ_ATTR_PHYSIQUE), idx);
	pack(pkgbuf, p->get_player_attr_value(OBJ_ATTR_LUCK), idx);
	pack(pkgbuf, p->get_player_attr_value(OBJ_ATTR_ATK), idx);
	pack(pkgbuf, p->get_player_attr_value(OBJ_ATTR_DEF), idx);
	pack(pkgbuf, p->get_player_attr_value(OBJ_ATTR_DOUBLE), idx);


	init_cli_proto_head(pkgbuf, p, cli_proto_get_map_user_detail_info_cmd, idx);
	return send_to_player(p, (char*)pkgbuf, idx, 1);
}

int    broadcast_player_move_map(player_t* p, map_t* m, uint32_t x, uint32_t y, uint32_t dir)
{
	int idx = sizeof(cli_proto_t);

	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, x, idx);
	pack(pkgbuf, y, idx);
	pack(pkgbuf, dir, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_map_player_move_cmd, idx);

	map_line_players* p_line = p->cur_map->get_line_players(p->cur_map_line_index);
	list_head_t * l;
	list_for_each (l, &(p_line->map_line_playerlist))
	{
		player_t * p_player = list_entry(l, player_t, maphook);	
		send_to_player(p_player, (char*)pkgbuf, idx, 0);	
	}
	return 0;
}

int    broadcast_player_equips_change(player_t* p)
{
	uint32_t equip_count = 0;
	int idx = sizeof(cli_proto_t);
	int idx2 = 0;

	pack(pkgbuf, p->id, idx);
	idx2 = idx; 
	pack(pkgbuf, equip_count, idx);

	item* equips[] = {p->head, p->eye, p->glass, p->body, p->tail, p->suit};

	for(uint32_t i =0; i < sizeof(equips)/sizeof(equips[0]); i++)
	{
		item* p_item = equips[i];		
		if(p_item == NULL)continue;
		pack(pkgbuf, p_item->item_id(), idx);
		pack(pkgbuf, p_item->hide, idx);
		equip_count++;
	}
	pack(pkgbuf, equip_count, idx2);
	init_cli_proto_head(pkgbuf, p, cli_proto_equip_change_cmd, idx);

	map_line_players* p_line = p->cur_map->get_line_players(p->cur_map_line_index);
	list_head_t * l;
	list_for_each (l, &(p_line->map_line_playerlist))
	{
		player_t * p_player = list_entry(l, player_t, maphook);
		send_to_player(p_player, (char*)pkgbuf, idx, 0);
	}
	return 0;
}

int broadcast_player_fight_pet_change(player_t * p)
{
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, p->id, idx);
    pack_map_pet(p, idx);


	init_cli_proto_head(pkgbuf, p, cli_proto_set_fight_pet_cmd, idx);

	map_line_players* p_line = p->cur_map->get_line_players(p->cur_map_line_index);
	list_head_t * l;
	list_for_each (l, &(p_line->map_line_playerlist))
	{
		player_t * p_player = list_entry(l, player_t, maphook);
		send_to_player(p_player, (char*)pkgbuf, idx, 0);
	}
	return 0;
}

void pack_map_pet(player_t * p, int & idx)
{
    c_pet * p_pet = p->fight_pet;
    if (NULL == p_pet)
    {
        pack32(pkgbuf, 0, idx);
        pack32(pkgbuf, 0, idx);
        pack32(pkgbuf, 0, idx);
    }
    else
    {
        pack(pkgbuf, p_pet->m_no, idx);
        pack(pkgbuf, p_pet->m_id, idx);
        pack(pkgbuf, p_pet->m_level, idx);
    }

}


int send_to_map(player_t *p, char *pkg, uint32_t len, int completed)
{
    map_line_players *p_line =  p->get_cur_map_line();
    if (p_line == NULL) {
        return send_to_player(p, pkg, len, completed);
    }
    list_head_t * l = NULL;
    list_for_each(l, &(p_line->map_line_playerlist)) {
        player_t *p_dest = list_entry(l, player_t, maphook);
        send_to_player(p_dest, pkg, len, completed);
    }
    return 0;
}

int send_to_map(player_t * p, Cmessage * p_out, uint16_t cmd,  uint8_t completed)
{
    map_line_players *p_line =  p->get_cur_map_line();
    if (p_line == NULL) {
        return send_to_player(p, p_out, cmd, completed);
    }
    list_head_t *l = NULL;
    list_for_each(l, &(p_line->map_line_playerlist)) {
        player_t *p_dest = list_entry(l, player_t, maphook);
        send_to_player(p_dest, p_out, cmd, completed);
    }
    return 0;
}

