#ifndef PEA_ITEM_H
#define PEA_ITEM_H

#include <vector>
#include "CtableRoute100x10.h"
#include "proto.hpp"
#include "benchapi.h"

class pea_item: public CtableRoute100x10
{
public:
	pea_item(mysql_interface* db);	
	int query_player_items(Cmessage * c_in, Cmessage * c_out);
	
	int query_player_equips(Cmessage * c_in, Cmessage * c_out);

    int get_player_equips(db_user_id_t * db_user, std::vector<db_equip_info_t> & item_vec);

	int add_item(Cmessage * c_in, Cmessage * c_out);

    int add_item(db_user_id_t * db_user_id, db_add_item_request_t * info, db_add_item_reply_t * reply);
	
	int add_item(db_user_id_t * db_user_id, std::vector<db_add_item_request_t>& request);

    int del_item(db_user_id_t * db_user_id, db_del_item_request_t * info);
	
	int split_item(Cmessage* c_in, Cmessage* c_out);

	int wear_equip(Cmessage* c_in, Cmessage* c_out);

	int remove_equip(Cmessage* c_in, Cmessage* c_out);

	int batch_equips_opt(Cmessage* c_in, Cmessage* c_out);
	
	int get_cur_grid_item_count(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t* item_count);
	
	int get_item_count(db_user_id_t* db_user, uint32_t item_id, uint32_t* item_count);
	
	int insert_item_count(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t item_id, uint32_t item_count, uint32_t get_time, uint32_t expire_time, uint32_t* last_insert_id);
	
	int update_item_count(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t item_id, uint32_t item_count);
	
	int inc_item_count(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t item_id, uint32_t item_count);
	
	int dec_item_count(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t item_id, uint32_t item_count);

	int del_grid_item(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t item_id);

	int update_item_grid_index(db_user_id_t* db_user, uint32_t cur_item_index, uint32_t new_item_index, uint32_t hide);
	
	int set_equip_hide(db_user_id_t * db_user, uint32_t grid_index, uint32_t hide);
};






#endif
