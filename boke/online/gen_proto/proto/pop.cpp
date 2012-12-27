#include "pop.h"

online_item_t::online_item_t(){
	this->init();
}
void online_item_t::init(){
	this->online_id=0;
	this->user_num=0;
	this->online_port=0;
	this->friend_count=0;

}
bool  online_item_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->online_id)) return false;
	if (!ba.read_uint32(this->user_num)) return false;
	if(!ba.read_buf(this->online_ip,16)) return false;
	if (!ba.read_uint16(this->online_port)) return false;
	if (!ba.read_uint32(this->friend_count)) return false;
	return true;
}

bool online_item_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->online_id)) return false;
	if (!ba.write_uint32(this->user_num)) return false;
	if(!ba.write_buf(this->online_ip,16)) return false;
	if (!ba.write_uint16(this->online_port)) return false;
	if (!ba.write_uint32(this->friend_count)) return false;
	return true;
}
	
find_map_t::find_map_t(){
	this->init();
}
void find_map_t::init(){
	this->islandid=0;
	this->mapid=0;

}
bool  find_map_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	if (!ba.read_uint32(this->mapid)) return false;
	return true;
}

bool find_map_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	if (!ba.write_uint32(this->mapid)) return false;
	return true;
}
	
game_info_t::game_info_t(){
	this->init();
}
void game_info_t::init(){
	this->gameid=0;
	this->win_time=0;
	this->lose_time=0;
	this->draw_time=0;

}
bool  game_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;
	if (!ba.read_uint32(this->win_time)) return false;
	if (!ba.read_uint32(this->lose_time)) return false;
	if (!ba.read_uint32(this->draw_time)) return false;
	return true;
}

bool game_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->win_time)) return false;
	if (!ba.write_uint32(this->lose_time)) return false;
	if (!ba.write_uint32(this->draw_time)) return false;
	return true;
}
	
item_day_limit_t::item_day_limit_t(){
	this->init();
}
void item_day_limit_t::init(){
	this->itemid=0;
	this->total=0;
	this->daycnt=0;

}
bool  item_day_limit_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->total)) return false;
	if (!ba.read_uint32(this->daycnt)) return false;
	return true;
}

bool item_day_limit_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->total)) return false;
	if (!ba.write_uint32(this->daycnt)) return false;
	return true;
}
	
item_exchange_t::item_exchange_t(){
	this->init();
}
void item_exchange_t::init(){
	this->itemid=0;
	this->count=0;
	this->max_count=0;
	this->day_add_cnt=0;

}
bool  item_exchange_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	if (!ba.read_uint32(this->max_count)) return false;
	if (!ba.read_uint32(this->day_add_cnt)) return false;
	return true;
}

bool item_exchange_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	if (!ba.write_uint32(this->max_count)) return false;
	if (!ba.write_uint32(this->day_add_cnt)) return false;
	return true;
}
	
item_ex_t::item_ex_t(){
	this->init();
}
void item_ex_t::init(){
	this->itemid=0;
	this->use_count=0;
	this->count=0;

}
bool  item_ex_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->use_count)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool item_ex_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->use_count)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
item_t::item_t(){
	this->init();
}
void item_t::init(){
	this->itemid=0;
	this->count=0;

}
bool  item_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool item_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
task_t::task_t(){
	this->init();
}
void task_t::init(){
	this->taskid=0;
	this->task_nodeid=0;

}
bool  task_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	if (!ba.read_uint32(this->task_nodeid)) return false;
	return true;
}

bool task_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	if (!ba.write_uint32(this->task_nodeid)) return false;
	return true;
}
	
user_log_t::user_log_t(){
	this->init();
}
void user_log_t::init(){
	this->logtime=0;
	this->v1=0;
	this->v2=0;

}
bool  user_log_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->v1)) return false;
	if (!ba.read_uint32(this->v2)) return false;
	return true;
}

bool user_log_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->v1)) return false;
	if (!ba.write_uint32(this->v2)) return false;
	return true;
}
	