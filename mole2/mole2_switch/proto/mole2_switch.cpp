#include "mole2_switch.h"

switch_add_luckystar_exchange_cnt_in::switch_add_luckystar_exchange_cnt_in(){
	this->init();
}
void switch_add_luckystar_exchange_cnt_in::init(){
	this->uid=0;
	this->count=0;
	this->max=0;

}
bool  switch_add_luckystar_exchange_cnt_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	if (!ba.read_uint32(this->max)) return false;
	return true;
}

bool switch_add_luckystar_exchange_cnt_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	if (!ba.write_uint32(this->max)) return false;
	return true;
}
	
switch_add_luckystar_exchange_cnt_out::switch_add_luckystar_exchange_cnt_out(){
	this->init();
}
void switch_add_luckystar_exchange_cnt_out::init(){
	this->ret=0;

}
bool  switch_add_luckystar_exchange_cnt_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->ret)) return false;
	return true;
}

bool switch_add_luckystar_exchange_cnt_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->ret)) return false;
	return true;
}
	
switch_broadcast_msg_in::switch_broadcast_msg_in(){
	this->init();
}
void switch_broadcast_msg_in::init(){
	this->onlineid=0;
	this->uid=0;
	this->type=0;
	this->msglen=0;

}
bool  switch_broadcast_msg_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->onlineid)) return false;
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->type)) return false;
	if (!ba.read_uint16(this->msglen)) return false;
	return true;
}

bool switch_broadcast_msg_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->onlineid)) return false;
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->type)) return false;
	if (!ba.write_uint16(this->msglen)) return false;
	return true;
}
	