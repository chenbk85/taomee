#ifndef _USE_ITEM_HISTORY_H_
#define _USE_ITEM_HISTORY_H_


#include"fwd_decl.hpp"
#include <vector>
#include <map>
using namespace std;


struct player_t;

typedef struct use_item_data
{
	use_item_data()
	{
		item_id = 0;
		item_count = 0;
		last_tm = 0;
	}
	uint32_t item_id;
	uint32_t item_count;
	uint32_t last_tm;
}use_item_data;

typedef struct get_use_item_list_rsp_t
{
	uint32_t count;
	use_item_data datas[];
}get_use_item_list_rsp_t;


bool init_player_use_item_data(player_t* p);
bool final_player_use_item_data(player_t* p);
bool add_player_use_item_data(player_t*p,  uint32_t item_id, uint32_t item_count = 0);
bool is_player_item_data_exist(player_t*p, uint32_t item_id);
use_item_data* get_player_use_item_data(player_t* p, uint32_t item_id);
uint32_t   get_player_use_item_data_times(player_t* p, uint32_t item_id);
int  load_player_use_item_data(player_t* p, get_use_item_list_rsp_t* rsp);

int db_get_use_item_list(player_t* p);
int db_replace_use_item(player_t* p, uint32_t item_id, uint32_t item_count, uint32_t last_tm);
int db_get_use_item_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);





#endif
