#include "pop_switch.h"

sw_get_ranged_svrlist_in::sw_get_ranged_svrlist_in(){
	this->init();
}
void sw_get_ranged_svrlist_in::init(){
	this->start_id=0;
	this->end_id=0;
	this->friend_list.clear();

}
bool  sw_get_ranged_svrlist_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->start_id)) return false;
	if (!ba.read_uint32(this->end_id)) return false;

	uint32_t friend_list_count ;
	if (!ba.read_uint32( friend_list_count )) return false;
	if (friend_list_count>1000) return false;
	uint32_t  friend_list_item;
	this->friend_list.clear();
	{for(uint32_t i=0; i<friend_list_count;i++){
		if (!ba.read_uint32(friend_list_item)) return false;
		this->friend_list.push_back(friend_list_item);
	}}
	return true;
}

bool sw_get_ranged_svrlist_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->start_id)) return false;
	if (!ba.write_uint32(this->end_id)) return false;
	if (this->friend_list.size()>1000 ) return false;
	if (!ba.write_uint32(this->friend_list.size())) return false;
	{for(uint32_t i=0; i<this->friend_list.size() ;i++){
		if (!ba.write_uint32(this->friend_list[i])) return false;
	}}
	return true;
}
	
sw_get_ranged_svrlist_out::sw_get_ranged_svrlist_out(){
	this->init();
}
void sw_get_ranged_svrlist_out::init(){
	this->max_online_id=0;
	this->online_list.clear();

}
bool  sw_get_ranged_svrlist_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->max_online_id)) return false;

	uint32_t online_list_count ;
	if (!ba.read_uint32( online_list_count )) return false;
	if (online_list_count>1000) return false;
	online_item_t  online_list_item;
	this->online_list.clear();
	{for(uint32_t i=0; i<online_list_count;i++){
		if (!online_list_item.read_from_buf(ba)) return false;
		this->online_list.push_back(online_list_item);
	}}
	return true;
}

bool sw_get_ranged_svrlist_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->max_online_id)) return false;
	if (this->online_list.size()>1000 ) return false;
	if (!ba.write_uint32(this->online_list.size())) return false;
	{for(uint32_t i=0; i<this->online_list.size() ;i++){
		if (!this->online_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
sw_get_recommend_svr_list_out::sw_get_recommend_svr_list_out(){
	this->init();
}
void sw_get_recommend_svr_list_out::init(){
	this->max_online_id=0;
	this->online_list.clear();

}
bool  sw_get_recommend_svr_list_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->max_online_id)) return false;

	uint32_t online_list_count ;
	if (!ba.read_uint32( online_list_count )) return false;
	if (online_list_count>1000) return false;
	online_item_t  online_list_item;
	this->online_list.clear();
	{for(uint32_t i=0; i<online_list_count;i++){
		if (!online_list_item.read_from_buf(ba)) return false;
		this->online_list.push_back(online_list_item);
	}}
	return true;
}

bool sw_get_recommend_svr_list_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->max_online_id)) return false;
	if (this->online_list.size()>1000 ) return false;
	if (!ba.write_uint32(this->online_list.size())) return false;
	{for(uint32_t i=0; i<this->online_list.size() ;i++){
		if (!this->online_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
sw_get_user_count_out::sw_get_user_count_out(){
	this->init();
}
void sw_get_user_count_out::init(){
	this->online_count=0;

}
bool  sw_get_user_count_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->online_count)) return false;
	return true;
}

bool sw_get_user_count_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->online_count)) return false;
	return true;
}
	
sw_noti_across_svr_in::sw_noti_across_svr_in(){
	this->init();
}
void sw_noti_across_svr_in::init(){
	this->cmdid=0;
	this->_buflist_len=0;

}
bool  sw_noti_across_svr_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->cmdid)) return false;

	if (!ba.read_uint32(this->_buflist_len )) return false;
	if (this->_buflist_len>999) return false;
 	if (!ba.read_buf(this->buflist,this->_buflist_len)) return false;
	return true;
}

bool sw_noti_across_svr_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->cmdid)) return false;
	if (this->_buflist_len>999) return false;
	if (!ba.write_uint32(this->_buflist_len))return false;
	if (!ba.write_buf(this->buflist,this->_buflist_len)) return false;
	return true;
}
	
sw_noti_across_svr_out::sw_noti_across_svr_out(){
	this->init();
}
void sw_noti_across_svr_out::init(){
	this->cmdid=0;
	this->_buflist_len=0;

}
bool  sw_noti_across_svr_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->cmdid)) return false;

	if (!ba.read_uint32(this->_buflist_len )) return false;
	if (this->_buflist_len>999) return false;
 	if (!ba.read_buf(this->buflist,this->_buflist_len)) return false;
	return true;
}

bool sw_noti_across_svr_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->cmdid)) return false;
	if (this->_buflist_len>999) return false;
	if (!ba.write_uint32(this->_buflist_len))return false;
	if (!ba.write_buf(this->buflist,this->_buflist_len)) return false;
	return true;
}
	
sw_report_online_info_in::sw_report_online_info_in(){
	this->init();
}
void sw_report_online_info_in::init(){
	this->domain_id=0;
	this->online_id=0;
	{for(uint32_t i=0; i<16;i++){
		this->online_ip[i]=0;
	}}
	this->online_port=0;
	this->user_num=0;
	this->seqno=0;
	this->userid_list.clear();

}
bool  sw_report_online_info_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint16(this->domain_id)) return false;
	if (!ba.read_uint32(this->online_id)) return false;
	if(!ba.read_buf(this->online_name,16)) return false;
	{for(uint32_t i=0; i<16;i++){
		if (!ba.read_uint16(this->online_ip[i])) return false;
	}}
	if (!ba.read_uint16(this->online_port)) return false;
	if (!ba.read_uint32(this->user_num)) return false;
	if (!ba.read_uint32(this->seqno)) return false;

	uint32_t userid_list_count ;
	if (!ba.read_uint32( userid_list_count )) return false;
	if (userid_list_count>999999) return false;
	uint32_t  userid_list_item;
	this->userid_list.clear();
	{for(uint32_t i=0; i<userid_list_count;i++){
		if (!ba.read_uint32(userid_list_item)) return false;
		this->userid_list.push_back(userid_list_item);
	}}
	return true;
}

bool sw_report_online_info_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint16(this->domain_id)) return false;
	if (!ba.write_uint32(this->online_id)) return false;
	if(!ba.write_buf(this->online_name,16)) return false;
	{for(uint32_t i=0; i<16;i++){
		if (!ba.write_uint16(this->online_ip[i])) return false;
	}}
	if (!ba.write_uint16(this->online_port)) return false;
	if (!ba.write_uint32(this->user_num)) return false;
	if (!ba.write_uint32(this->seqno)) return false;
	if (this->userid_list.size()>999999 ) return false;
	if (!ba.write_uint32(this->userid_list.size())) return false;
	{for(uint32_t i=0; i<this->userid_list.size() ;i++){
		if (!ba.write_uint32(this->userid_list[i])) return false;
	}}
	return true;
}
	
sw_report_user_onoff_in::sw_report_user_onoff_in(){
	this->init();
}
void sw_report_user_onoff_in::init(){
	this->is_on_online=0;

}
bool  sw_report_user_onoff_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->is_on_online)) return false;
	return true;
}

bool sw_report_user_onoff_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->is_on_online)) return false;
	return true;
}
	