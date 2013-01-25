#include "pop_db.h"

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
	if (add_item_list_count>9999) return false;
	item_exchange_t  add_item_list_item;
	this->add_item_list.clear();
	{for(uint32_t i=0; i<add_item_list_count;i++){
		if (!add_item_list_item.read_from_buf(ba)) return false;
		this->add_item_list.push_back(add_item_list_item);
	}}

	uint32_t del_item_list_count ;
	if (!ba.read_uint32( del_item_list_count )) return false;
	if (del_item_list_count>9999) return false;
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
	if (this->add_item_list.size()>9999 ) return false;
	if (!ba.write_uint32(this->add_item_list.size())) return false;
	{for(uint32_t i=0; i<this->add_item_list.size() ;i++){
		if (!this->add_item_list[i].write_to_buf(ba)) return false;
	}}
	if (this->del_item_list.size()>9999 ) return false;
	if (!ba.write_uint32(this->del_item_list.size())) return false;
	{for(uint32_t i=0; i<this->del_item_list.size() ;i++){
		if (!this->del_item_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
pop_login_out::pop_login_out(){
	this->init();
}
void pop_login_out::init(){
	this->register_time=0;
	this->sex=0;
	this->age=0;
	this->task_list.clear();

}
bool  pop_login_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->register_time)) return false;
	if (!ba.read_uint32(this->sex)) return false;
	if (!ba.read_uint32(this->age)) return false;
	if(!ba.read_buf(this->nick,16)) return false;

	uint32_t task_list_count ;
	if (!ba.read_uint32( task_list_count )) return false;
	if (task_list_count>9999) return false;
	task_t  task_list_item;
	this->task_list.clear();
	{for(uint32_t i=0; i<task_list_count;i++){
		if (!task_list_item.read_from_buf(ba)) return false;
		this->task_list.push_back(task_list_item);
	}}
	return true;
}

bool pop_login_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->register_time)) return false;
	if (!ba.write_uint32(this->sex)) return false;
	if (!ba.write_uint32(this->age)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (this->task_list.size()>9999 ) return false;
	if (!ba.write_uint32(this->task_list.size())) return false;
	{for(uint32_t i=0; i<this->task_list.size() ;i++){
		if (!this->task_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
pop_reg_in::pop_reg_in(){
	this->init();
}
void pop_reg_in::init(){
	this->sex=0;
	this->age=0;
	this->item_configid=0;

}
bool  pop_reg_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->sex)) return false;
	if (!ba.read_uint32(this->age)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->item_configid)) return false;
	return true;
}

bool pop_reg_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->sex)) return false;
	if (!ba.write_uint32(this->age)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->item_configid)) return false;
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
	
pop_task_get_list_out::pop_task_get_list_out(){
	this->init();
}
void pop_task_get_list_out::init(){
	this->task_list.clear();

}
bool  pop_task_get_list_out::read_from_buf(byte_array_t & ba ){

	uint32_t task_list_count ;
	if (!ba.read_uint32( task_list_count )) return false;
	if (task_list_count>9999) return false;
	task_t  task_list_item;
	this->task_list.clear();
	{for(uint32_t i=0; i<task_list_count;i++){
		if (!task_list_item.read_from_buf(ba)) return false;
		this->task_list.push_back(task_list_item);
	}}
	return true;
}

bool pop_task_get_list_out::write_to_buf(byte_array_t & ba ){
	if (this->task_list.size()>9999 ) return false;
	if (!ba.write_uint32(this->task_list.size())) return false;
	{for(uint32_t i=0; i<this->task_list.size() ;i++){
		if (!this->task_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	