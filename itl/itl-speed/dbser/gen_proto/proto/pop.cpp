#include "pop.h"

item_exchange_t::item_exchange_t(){
	this->init();
}
void item_exchange_t::init(){
	this->itemid=0;
	this->count=0;
	this->max_count=0;

}
bool  item_exchange_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	if (!ba.read_uint32(this->max_count)) return false;
	return true;
}

bool item_exchange_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	if (!ba.write_uint32(this->max_count)) return false;
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
	