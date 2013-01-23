#ifndef DB_ITEM_HPP_
#define DB_ITEM_HPP_


#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "dbproxy.hpp"
#include <vector>

using namespace std;

class player_t;

typedef struct parse_item
{
	parse_item()
	{
		item_id = 0;
		item_count = 0;
		expire_time = 0;
	}
	uint32_t item_id;
	uint32_t item_count;
	uint32_t expire_time;
}parse_item;


int db_get_player_items(player_t* p);

int db_proto_get_player_items_callback(DEFAULT_ARG);

int db_get_player_equips(player_t* p);

int db_proto_get_player_equips_callback(DEFAULT_ARG);

int db_add_item(player_t* p, std::vector<db_add_item_request_t>& request);

int db_proto_add_item_callback(DEFAULT_ARG);

int db_del_item(player_t* p, std::vector<db_del_item_request_t>& request);

int db_proto_del_item_callback(DEFAULT_ARG);

int db_proto_split_item_callback(DEFAULT_ARG);

bool parse_db_add_item_request(player_t* p, uint32_t item_id, uint32_t item_count, uint32_t expire_time, std::vector<db_add_item_request_t>& request);

bool parse_db_add_item_request(player_t* p, std::vector<parse_item>& items, std::vector<db_add_item_request_t>& request);

bool parse_db_del_item_request(player_t* p, uint32_t item_id, uint32_t item_count, std::vector<db_del_item_request_t>& request);

int  db_wear_equip(player_t* p, uint32_t bag_index, uint32_t body_index, uint8_t hide);

int  db_proto_wear_equip_callback(DEFAULT_ARG);

int  db_remove_equip(player_t* p, uint32_t equip_pos, uint32_t bag_index);

int  db_proto_remove_equip_callback(DEFAULT_ARG);

int  db_batch_equips_opt(player_t* p, std::vector<equip_change_t>& equip_opts);

int  db_proto_batch_equips_opt_callback(DEFAULT_ARG);



#endif
