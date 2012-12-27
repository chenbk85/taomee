#include "pop_db.h"

get_server_version_out::get_server_version_out(){
	this->init();
}
void get_server_version_out::init(){

}
bool  get_server_version_out::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->version,255)) return false;
	return true;
}

bool get_server_version_out::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->version,255)) return false;
	return true;
}
	
pop_add_game_info_in::pop_add_game_info_in(){
	this->init();
}
void pop_add_game_info_in::init(){
	this->gameid=0;
	this->win_flag=0;

}
bool  pop_add_game_info_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;
	if (!ba.read_uint32(this->win_flag)) return false;
	return true;
}

bool pop_add_game_info_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->win_flag)) return false;
	return true;
}
	
pop_copy_user_in::pop_copy_user_in(){
	this->init();
}
void pop_copy_user_in::init(){
	this->dsc_userid=0;

}
bool  pop_copy_user_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->dsc_userid)) return false;
	return true;
}

bool pop_copy_user_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->dsc_userid)) return false;
	return true;
}
	
pop_exchange_in::pop_exchange_in(){
	this->init();
}
void pop_exchange_in::init(){
	this->opt_flag=0;
	this->add_item_list.clear();
	this->del_item_list.clear();

}
bool  pop_exchange_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->opt_flag)) return false;

	uint32_t add_item_list_count ;
	if (!ba.read_uint32( add_item_list_count )) return false;
	item_exchange_t  add_item_list_item;
	this->add_item_list.clear();
	{for(uint32_t i=0; i<add_item_list_count;i++){
		if (!add_item_list_item.read_from_buf(ba)) return false;
		this->add_item_list.push_back(add_item_list_item);
	}}

	uint32_t del_item_list_count ;
	if (!ba.read_uint32( del_item_list_count )) return false;
	item_exchange_t  del_item_list_item;
	this->del_item_list.clear();
	{for(uint32_t i=0; i<del_item_list_count;i++){
		if (!del_item_list_item.read_from_buf(ba)) return false;
		this->del_item_list.push_back(del_item_list_item);
	}}
	return true;
}

bool pop_exchange_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->opt_flag)) return false;
	if (!ba.write_uint32(this->add_item_list.size())) return false;
	{for(uint32_t i=0; i<this->add_item_list.size() ;i++){
		if (!this->add_item_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->del_item_list.size())) return false;
	{for(uint32_t i=0; i<this->del_item_list.size() ;i++){
		if (!this->del_item_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
pop_find_map_add_in::pop_find_map_add_in(){
	this->init();
}
void pop_find_map_add_in::init(){
	this->islandid=0;
	this->mapid=0;

}
bool  pop_find_map_add_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	if (!ba.read_uint32(this->mapid)) return false;
	return true;
}

bool pop_find_map_add_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	if (!ba.write_uint32(this->mapid)) return false;
	return true;
}
	
user_base_info_t::user_base_info_t(){
	this->init();
}
void user_base_info_t::init(){
	this->register_time=0;
	this->flag=0;
	this->xiaomee=0;
	this->color=0;
	this->age=0;
	this->last_islandid=0;
	this->last_mapid=0;
	this->last_x=0;
	this->last_y=0;
	this->last_login=0;
	this->online_time=0;

}
bool  user_base_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->register_time)) return false;
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->xiaomee)) return false;
	if (!ba.read_uint32(this->color)) return false;
	if (!ba.read_uint32(this->age)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->last_islandid)) return false;
	if (!ba.read_uint32(this->last_mapid)) return false;
	if (!ba.read_uint32(this->last_x)) return false;
	if (!ba.read_uint32(this->last_y)) return false;
	if (!ba.read_uint32(this->last_login)) return false;
	if (!ba.read_uint32(this->online_time)) return false;
	return true;
}

bool user_base_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->register_time)) return false;
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->xiaomee)) return false;
	if (!ba.write_uint32(this->color)) return false;
	if (!ba.write_uint32(this->age)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->last_islandid)) return false;
	if (!ba.write_uint32(this->last_mapid)) return false;
	if (!ba.write_uint32(this->last_x)) return false;
	if (!ba.write_uint32(this->last_y)) return false;
	if (!ba.write_uint32(this->last_login)) return false;
	if (!ba.write_uint32(this->online_time)) return false;
	return true;
}
	
pop_get_all_info_out::pop_get_all_info_out(){
	this->init();
}
void pop_get_all_info_out::init(){
	this->user_base_info.init();
	this->item_list.clear();
	this->item_day_list.clear();
	this->task_list.clear();
	this->find_map_list.clear();
	this->user_log_list.clear();
	this->game_info_list.clear();

}
bool  pop_get_all_info_out::read_from_buf(byte_array_t & ba ){
	if (!this->user_base_info.read_from_buf(ba)) return false;

	uint32_t item_list_count ;
	if (!ba.read_uint32( item_list_count )) return false;
	item_ex_t  item_list_item;
	this->item_list.clear();
	{for(uint32_t i=0; i<item_list_count;i++){
		if (!item_list_item.read_from_buf(ba)) return false;
		this->item_list.push_back(item_list_item);
	}}

	uint32_t item_day_list_count ;
	if (!ba.read_uint32( item_day_list_count )) return false;
	item_day_limit_t  item_day_list_item;
	this->item_day_list.clear();
	{for(uint32_t i=0; i<item_day_list_count;i++){
		if (!item_day_list_item.read_from_buf(ba)) return false;
		this->item_day_list.push_back(item_day_list_item);
	}}

	uint32_t task_list_count ;
	if (!ba.read_uint32( task_list_count )) return false;
	task_t  task_list_item;
	this->task_list.clear();
	{for(uint32_t i=0; i<task_list_count;i++){
		if (!task_list_item.read_from_buf(ba)) return false;
		this->task_list.push_back(task_list_item);
	}}

	uint32_t find_map_list_count ;
	if (!ba.read_uint32( find_map_list_count )) return false;
	find_map_t  find_map_list_item;
	this->find_map_list.clear();
	{for(uint32_t i=0; i<find_map_list_count;i++){
		if (!find_map_list_item.read_from_buf(ba)) return false;
		this->find_map_list.push_back(find_map_list_item);
	}}

	uint32_t user_log_list_count ;
	if (!ba.read_uint32( user_log_list_count )) return false;
	user_log_t  user_log_list_item;
	this->user_log_list.clear();
	{for(uint32_t i=0; i<user_log_list_count;i++){
		if (!user_log_list_item.read_from_buf(ba)) return false;
		this->user_log_list.push_back(user_log_list_item);
	}}

	uint32_t game_info_list_count ;
	if (!ba.read_uint32( game_info_list_count )) return false;
	game_info_t  game_info_list_item;
	this->game_info_list.clear();
	{for(uint32_t i=0; i<game_info_list_count;i++){
		if (!game_info_list_item.read_from_buf(ba)) return false;
		this->game_info_list.push_back(game_info_list_item);
	}}
	return true;
}

bool pop_get_all_info_out::write_to_buf(byte_array_t & ba ){
	if (!this->user_base_info.write_to_buf(ba)) return false;
	if (!ba.write_uint32(this->item_list.size())) return false;
	{for(uint32_t i=0; i<this->item_list.size() ;i++){
		if (!this->item_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->item_day_list.size())) return false;
	{for(uint32_t i=0; i<this->item_day_list.size() ;i++){
		if (!this->item_day_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->task_list.size())) return false;
	{for(uint32_t i=0; i<this->task_list.size() ;i++){
		if (!this->task_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->find_map_list.size())) return false;
	{for(uint32_t i=0; i<this->find_map_list.size() ;i++){
		if (!this->find_map_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->user_log_list.size())) return false;
	{for(uint32_t i=0; i<this->user_log_list.size() ;i++){
		if (!this->user_log_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->game_info_list.size())) return false;
	{for(uint32_t i=0; i<this->game_info_list.size() ;i++){
		if (!this->game_info_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
pop_get_base_info_out::pop_get_base_info_out(){
	this->init();
}
void pop_get_base_info_out::init(){
	this->last_online_id=0;

}
bool  pop_get_base_info_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->last_online_id)) return false;
	return true;
}

bool pop_get_base_info_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->last_online_id)) return false;
	return true;
}
	
pop_login_out::pop_login_out(){
	this->init();
}
void pop_login_out::init(){
	this->user_base_info.init();
	this->item_list.clear();
	this->item_day_list.clear();
	this->task_list.clear();
	this->find_map_list.clear();
	this->user_log_list.clear();
	this->game_info_list.clear();

}
bool  pop_login_out::read_from_buf(byte_array_t & ba ){
	if (!this->user_base_info.read_from_buf(ba)) return false;

	uint32_t item_list_count ;
	if (!ba.read_uint32( item_list_count )) return false;
	item_ex_t  item_list_item;
	this->item_list.clear();
	{for(uint32_t i=0; i<item_list_count;i++){
		if (!item_list_item.read_from_buf(ba)) return false;
		this->item_list.push_back(item_list_item);
	}}

	uint32_t item_day_list_count ;
	if (!ba.read_uint32( item_day_list_count )) return false;
	item_day_limit_t  item_day_list_item;
	this->item_day_list.clear();
	{for(uint32_t i=0; i<item_day_list_count;i++){
		if (!item_day_list_item.read_from_buf(ba)) return false;
		this->item_day_list.push_back(item_day_list_item);
	}}

	uint32_t task_list_count ;
	if (!ba.read_uint32( task_list_count )) return false;
	task_t  task_list_item;
	this->task_list.clear();
	{for(uint32_t i=0; i<task_list_count;i++){
		if (!task_list_item.read_from_buf(ba)) return false;
		this->task_list.push_back(task_list_item);
	}}

	uint32_t find_map_list_count ;
	if (!ba.read_uint32( find_map_list_count )) return false;
	find_map_t  find_map_list_item;
	this->find_map_list.clear();
	{for(uint32_t i=0; i<find_map_list_count;i++){
		if (!find_map_list_item.read_from_buf(ba)) return false;
		this->find_map_list.push_back(find_map_list_item);
	}}

	uint32_t user_log_list_count ;
	if (!ba.read_uint32( user_log_list_count )) return false;
	user_log_t  user_log_list_item;
	this->user_log_list.clear();
	{for(uint32_t i=0; i<user_log_list_count;i++){
		if (!user_log_list_item.read_from_buf(ba)) return false;
		this->user_log_list.push_back(user_log_list_item);
	}}

	uint32_t game_info_list_count ;
	if (!ba.read_uint32( game_info_list_count )) return false;
	game_info_t  game_info_list_item;
	this->game_info_list.clear();
	{for(uint32_t i=0; i<game_info_list_count;i++){
		if (!game_info_list_item.read_from_buf(ba)) return false;
		this->game_info_list.push_back(game_info_list_item);
	}}
	return true;
}

bool pop_login_out::write_to_buf(byte_array_t & ba ){
	if (!this->user_base_info.write_to_buf(ba)) return false;
	if (!ba.write_uint32(this->item_list.size())) return false;
	{for(uint32_t i=0; i<this->item_list.size() ;i++){
		if (!this->item_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->item_day_list.size())) return false;
	{for(uint32_t i=0; i<this->item_day_list.size() ;i++){
		if (!this->item_day_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->task_list.size())) return false;
	{for(uint32_t i=0; i<this->task_list.size() ;i++){
		if (!this->task_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->find_map_list.size())) return false;
	{for(uint32_t i=0; i<this->find_map_list.size() ;i++){
		if (!this->find_map_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->user_log_list.size())) return false;
	{for(uint32_t i=0; i<this->user_log_list.size() ;i++){
		if (!this->user_log_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->game_info_list.size())) return false;
	{for(uint32_t i=0; i<this->game_info_list.size() ;i++){
		if (!this->game_info_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
pop_logout_in::pop_logout_in(){
	this->init();
}
void pop_logout_in::init(){
	this->last_islandid=0;
	this->last_mapid=0;
	this->last_x=0;
	this->last_y=0;
	this->last_login=0;
	this->online_time=0;
	this->last_online_id=0;

}
bool  pop_logout_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->last_islandid)) return false;
	if (!ba.read_uint32(this->last_mapid)) return false;
	if (!ba.read_uint32(this->last_x)) return false;
	if (!ba.read_uint32(this->last_y)) return false;
	if (!ba.read_uint32(this->last_login)) return false;
	if (!ba.read_uint32(this->online_time)) return false;
	if (!ba.read_uint32(this->last_online_id)) return false;
	return true;
}

bool pop_logout_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->last_islandid)) return false;
	if (!ba.write_uint32(this->last_mapid)) return false;
	if (!ba.write_uint32(this->last_x)) return false;
	if (!ba.write_uint32(this->last_y)) return false;
	if (!ba.write_uint32(this->last_login)) return false;
	if (!ba.write_uint32(this->online_time)) return false;
	if (!ba.write_uint32(this->last_online_id)) return false;
	return true;
}
	
pop_opt_in::pop_opt_in(){
	this->init();
}
void pop_opt_in::init(){
	this->opt_groupid=0;
	this->opt_type=0;
	this->optid=0;
	this->count=0;
	this->v1=0;
	this->v2=0;

}
bool  pop_opt_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->opt_groupid)) return false;
	if (!ba.read_uint32(this->opt_type)) return false;
	if (!ba.read_uint32(this->optid)) return false;
	if (!ba.read_int32(this->count)) return false;
	if (!ba.read_uint32(this->v1)) return false;
	if (!ba.read_uint32(this->v2)) return false;
	return true;
}

bool pop_opt_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->opt_groupid)) return false;
	if (!ba.write_uint32(this->opt_type)) return false;
	if (!ba.write_uint32(this->optid)) return false;
	if (!ba.write_int32(this->count)) return false;
	if (!ba.write_uint32(this->v1)) return false;
	if (!ba.write_uint32(this->v2)) return false;
	return true;
}
	
pop_reg_in::pop_reg_in(){
	this->init();
}
void pop_reg_in::init(){
	this->color=0;
	this->age=0;
	this->flag=0;
	this->add_item_list.clear();

}
bool  pop_reg_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->color)) return false;
	if (!ba.read_uint32(this->age)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->flag)) return false;

	uint32_t add_item_list_count ;
	if (!ba.read_uint32( add_item_list_count )) return false;
	item_exchange_t  add_item_list_item;
	this->add_item_list.clear();
	{for(uint32_t i=0; i<add_item_list_count;i++){
		if (!add_item_list_item.read_from_buf(ba)) return false;
		this->add_item_list.push_back(add_item_list_item);
	}}
	return true;
}

bool pop_reg_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->color)) return false;
	if (!ba.write_uint32(this->age)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->add_item_list.size())) return false;
	{for(uint32_t i=0; i<this->add_item_list.size() ;i++){
		if (!this->add_item_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
pop_set_color_in::pop_set_color_in(){
	this->init();
}
void pop_set_color_in::init(){
	this->color=0;

}
bool  pop_set_color_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->color)) return false;
	return true;
}

bool pop_set_color_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->color)) return false;
	return true;
}
	
pop_set_flag_in::pop_set_flag_in(){
	this->init();
}
void pop_set_flag_in::init(){
	this->flag=0;
	this->mask=0;

}
bool  pop_set_flag_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->mask)) return false;
	return true;
}

bool pop_set_flag_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->mask)) return false;
	return true;
}
	
pop_set_item_used_list_in::pop_set_item_used_list_in(){
	this->init();
}
void pop_set_item_used_list_in::init(){
	this->del_can_change_list.clear();
	this->set_noused_list.clear();
	this->add_can_change_list.clear();
	this->set_used_list.clear();

}
bool  pop_set_item_used_list_in::read_from_buf(byte_array_t & ba ){

	uint32_t del_can_change_list_count ;
	if (!ba.read_uint32( del_can_change_list_count )) return false;
	uint32_t  del_can_change_list_item;
	this->del_can_change_list.clear();
	{for(uint32_t i=0; i<del_can_change_list_count;i++){
		if (!ba.read_uint32(del_can_change_list_item)) return false;
		this->del_can_change_list.push_back(del_can_change_list_item);
	}}

	uint32_t set_noused_list_count ;
	if (!ba.read_uint32( set_noused_list_count )) return false;
	uint32_t  set_noused_list_item;
	this->set_noused_list.clear();
	{for(uint32_t i=0; i<set_noused_list_count;i++){
		if (!ba.read_uint32(set_noused_list_item)) return false;
		this->set_noused_list.push_back(set_noused_list_item);
	}}

	uint32_t add_can_change_list_count ;
	if (!ba.read_uint32( add_can_change_list_count )) return false;
	uint32_t  add_can_change_list_item;
	this->add_can_change_list.clear();
	{for(uint32_t i=0; i<add_can_change_list_count;i++){
		if (!ba.read_uint32(add_can_change_list_item)) return false;
		this->add_can_change_list.push_back(add_can_change_list_item);
	}}

	uint32_t set_used_list_count ;
	if (!ba.read_uint32( set_used_list_count )) return false;
	uint32_t  set_used_list_item;
	this->set_used_list.clear();
	{for(uint32_t i=0; i<set_used_list_count;i++){
		if (!ba.read_uint32(set_used_list_item)) return false;
		this->set_used_list.push_back(set_used_list_item);
	}}
	return true;
}

bool pop_set_item_used_list_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->del_can_change_list.size())) return false;
	{for(uint32_t i=0; i<this->del_can_change_list.size() ;i++){
		if (!ba.write_uint32(this->del_can_change_list[i])) return false;
	}}
	if (!ba.write_uint32(this->set_noused_list.size())) return false;
	{for(uint32_t i=0; i<this->set_noused_list.size() ;i++){
		if (!ba.write_uint32(this->set_noused_list[i])) return false;
	}}
	if (!ba.write_uint32(this->add_can_change_list.size())) return false;
	{for(uint32_t i=0; i<this->add_can_change_list.size() ;i++){
		if (!ba.write_uint32(this->add_can_change_list[i])) return false;
	}}
	if (!ba.write_uint32(this->set_used_list.size())) return false;
	{for(uint32_t i=0; i<this->set_used_list.size() ;i++){
		if (!ba.write_uint32(this->set_used_list[i])) return false;
	}}
	return true;
}
	
pop_set_nick_in::pop_set_nick_in(){
	this->init();
}
void pop_set_nick_in::init(){

}
bool  pop_set_nick_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool pop_set_nick_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}
	
pop_task_complete_node_in::pop_task_complete_node_in(){
	this->init();
}
void pop_task_complete_node_in::init(){
	this->taskid=0;
	this->task_nodeid=0;

}
bool  pop_task_complete_node_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	if (!ba.read_uint32(this->task_nodeid)) return false;
	return true;
}

bool pop_task_complete_node_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	if (!ba.write_uint32(this->task_nodeid)) return false;
	return true;
}
	
pop_task_del_in::pop_task_del_in(){
	this->init();
}
void pop_task_del_in::init(){
	this->taskid=0;
	this->task_nodeid=0;

}
bool  pop_task_del_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	if (!ba.read_uint32(this->task_nodeid)) return false;
	return true;
}

bool pop_task_del_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	if (!ba.write_uint32(this->task_nodeid)) return false;
	return true;
}
	
pop_user_log_add_in::pop_user_log_add_in(){
	this->init();
}
void pop_user_log_add_in::init(){
	this->user_log.init();

}
bool  pop_user_log_add_in::read_from_buf(byte_array_t & ba ){
	if (!this->user_log.read_from_buf(ba)) return false;
	return true;
}

bool pop_user_log_add_in::write_to_buf(byte_array_t & ba ){
	if (!this->user_log.write_to_buf(ba)) return false;
	return true;
}
	
pop_user_set_field_value_in::pop_user_set_field_value_in(){
	this->init();
}
void pop_user_set_field_value_in::init(){

}
bool  pop_user_set_field_value_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->field_name,64)) return false;
	if(!ba.read_buf(this->field_value,255)) return false;
	return true;
}

bool pop_user_set_field_value_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->field_name,64)) return false;
	if(!ba.write_buf(this->field_value,255)) return false;
	return true;
}
	
udp_post_msg_in::udp_post_msg_in(){
	this->init();
}
void udp_post_msg_in::init(){
	this->gameid=0;
	this->nouse=0;
	this->type=0;
	this->_msg_len=0;

}
bool  udp_post_msg_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;
	if (!ba.read_uint32(this->nouse)) return false;
	if (!ba.read_uint32(this->type)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if(!ba.read_buf(this->title,60)) return false;

	if (!ba.read_uint32(this->_msg_len )) return false;
	if (this->_msg_len>4096) return false;
 	if (!ba.read_buf(this->msg,this->_msg_len)) return false;
	return true;
}

bool udp_post_msg_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->nouse)) return false;
	if (!ba.write_uint32(this->type)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if(!ba.write_buf(this->title,60)) return false;
	if (this->_msg_len>4096) return false;
	if (!ba.write_uint32(this->_msg_len))return false;
	if (!ba.write_buf(this->msg,this->_msg_len)) return false;
	return true;
}
	