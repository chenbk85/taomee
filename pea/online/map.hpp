#ifndef MAP_H_
#define MAP_H_

#include <stdint.h>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee++/proto/proto_base.h>

extern "C"
{
#include <libtaomee/list.h>
}

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "map_data.hpp"

typedef struct player_t player_t;

#define MAX_MAP_NAME_LEN 	 64
#define MAX_MAP_LINE 		 20
#define MAX_MAP_PLAYER_COUNT 400

#define VALID_MAP_LINE(index) \
    (0 <= index && index < MAX_MAP_LINE)


typedef struct map_line_players
{
	list_head_t map_line_playerlist;
	uint32_t    cur_player_count;
}map_line_players;

typedef struct map_t
{
	uint32_t   id;
	map_data*  data;	

	list_head_t hash_hook;

	map_line_players line_players[MAX_MAP_LINE];

	map_line_players* get_line_players(uint32_t line_index)
	{
		if( (line_index >= 0 && line_index < MAX_MAP_LINE) )
		{
			return &line_players[line_index];	
		}
		return NULL;
	}

} map_t;


bool init_maps();

bool final_maps();

map_t* alloc_map(uint32_t map_id);

map_t* get_map(uint32_t map_id);

map_line_players* find_idle_map_line(map_t* m, int32_t& map_line_index);

void   dealloc_map(map_t* m);

int    enter_map(player_t* p, uint32_t map_id, uint32_t x, uint32_t y);

int    leave_map(player_t* p);

int    change_map_line(player_t* p, int map_line);

int    broadcast_player_enter_map(player_t* p, map_t* m, uint32_t cur_line_index);

int    broadcast_player_leave_map(player_t* p, map_t* m, uint32_t cur_line_index);

int    broadcast_player_move_map(player_t* p, map_t* m, uint32_t x, uint32_t y, uint32_t dir);

int    broadcast_player_equips_change(player_t* p);

int broadcast_player_fight_pet_change(player_t * p);

player_t* get_player_in_map(map_t* m,  uint32_t cur_line_index, uint32_t userid);

int send_to_map(player_t *p, char *pkg, uint32_t len, int completed);

int send_to_map(player_t * p, Cmessage * p_out, uint16_t cmd,  uint8_t completed);

#endif
