#include "pea_btlsw.h"

btlsw_chat_msg_transfer_in::btlsw_chat_msg_transfer_in(){
	this->init();
}
void btlsw_chat_msg_transfer_in::init(){
	this->sender.init();
	this->msg_type=0;
	this->_msg_content_len=0;

}
bool  btlsw_chat_msg_transfer_in::read_from_buf(byte_array_t & ba ){
	if (!this->sender.read_from_buf(ba)) return false;
	if(!ba.read_buf(this->sender_nick,16)) return false;
	if (!ba.read_uint32(this->msg_type)) return false;

	if (!ba.read_uint32(this->_msg_content_len )) return false;
	if (this->_msg_content_len>64) return false;
 	if (!ba.read_buf(this->msg_content,this->_msg_content_len)) return false;
	return true;
}

bool btlsw_chat_msg_transfer_in::write_to_buf(byte_array_t & ba ){
	if (!this->sender.write_to_buf(ba)) return false;
	if(!ba.write_buf(this->sender_nick,16)) return false;
	if (!ba.write_uint32(this->msg_type)) return false;
	if (this->_msg_content_len>64) return false;
	if (!ba.write_uint32(this->_msg_content_len))return false;
	if (!ba.write_buf(this->msg_content,this->_msg_content_len)) return false;
	return true;
}

	
btlsw_chat_msg_transfer_out::btlsw_chat_msg_transfer_out(){
	this->init();
}
void btlsw_chat_msg_transfer_out::init(){
	this->sender.init();
	this->msg_type=0;
	this->_msg_content_len=0;

}
bool  btlsw_chat_msg_transfer_out::read_from_buf(byte_array_t & ba ){
	if (!this->sender.read_from_buf(ba)) return false;
	if(!ba.read_buf(this->sender_nick,16)) return false;
	if (!ba.read_uint32(this->msg_type)) return false;

	if (!ba.read_uint32(this->_msg_content_len )) return false;
	if (this->_msg_content_len>64) return false;
 	if (!ba.read_buf(this->msg_content,this->_msg_content_len)) return false;
	return true;
}

bool btlsw_chat_msg_transfer_out::write_to_buf(byte_array_t & ba ){
	if (!this->sender.write_to_buf(ba)) return false;
	if(!ba.write_buf(this->sender_nick,16)) return false;
	if (!ba.write_uint32(this->msg_type)) return false;
	if (this->_msg_content_len>64) return false;
	if (!ba.write_uint32(this->_msg_content_len))return false;
	if (!ba.write_buf(this->msg_content,this->_msg_content_len)) return false;
	return true;
}

	
btlsw_chg_player_status_notify_out::btlsw_chg_player_status_notify_out(){
	this->init();
}
void btlsw_chg_player_status_notify_out::init(){
	this->chg_uid=0;
	this->chg_role_tm=0;
	this->from_status=0;
	this->to_status=0;

}
bool  btlsw_chg_player_status_notify_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->chg_uid)) return false;
	if (!ba.read_int32(this->chg_role_tm)) return false;
	if (!ba.read_uint8(this->from_status)) return false;
	if (!ba.read_uint8(this->to_status)) return false;
	return true;
}

bool btlsw_chg_player_status_notify_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->chg_uid)) return false;
	if (!ba.write_int32(this->chg_role_tm)) return false;
	if (!ba.write_uint8(this->from_status)) return false;
	if (!ba.write_uint8(this->to_status)) return false;
	return true;
}

	
btlsw_get_room_full_info_out::btlsw_get_room_full_info_out(){
	this->init();
}
void btlsw_get_room_full_info_out::init(){
	this->room_full_info.init();

}
bool  btlsw_get_room_full_info_out::read_from_buf(byte_array_t & ba ){
	if (!this->room_full_info.read_from_buf(ba)) return false;
	return true;
}

bool btlsw_get_room_full_info_out::write_to_buf(byte_array_t & ba ){
	if (!this->room_full_info.write_to_buf(ba)) return false;
	return true;
}

	
btlsw_get_room_player_show_info_in::btlsw_get_room_player_show_info_in(){
	this->init();
}
void btlsw_get_room_player_show_info_in::init(){
	this->uid=0;
	this->role_tm=0;

}
bool  btlsw_get_room_player_show_info_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->role_tm)) return false;
	return true;
}

bool btlsw_get_room_player_show_info_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->role_tm)) return false;
	return true;
}

	
btlsw_get_room_player_show_info_out::btlsw_get_room_player_show_info_out(){
	this->init();
}
void btlsw_get_room_player_show_info_out::init(){
	this->show_info.init();

}
bool  btlsw_get_room_player_show_info_out::read_from_buf(byte_array_t & ba ){
	if (!this->show_info.read_from_buf(ba)) return false;
	return true;
}

bool btlsw_get_room_player_show_info_out::write_to_buf(byte_array_t & ba ){
	if (!this->show_info.write_to_buf(ba)) return false;
	return true;
}

	
btlsw_kick_room_player_in::btlsw_kick_room_player_in(){
	this->init();
}
void btlsw_kick_room_player_in::init(){
	this->kickee_uid=0;
	this->kickee_role_tm=0;

}
bool  btlsw_kick_room_player_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->kickee_uid)) return false;
	if (!ba.read_int32(this->kickee_role_tm)) return false;
	return true;
}

bool btlsw_kick_room_player_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->kickee_uid)) return false;
	if (!ba.write_int32(this->kickee_role_tm)) return false;
	return true;
}

	
btlsw_notify_room_seat_onoff_out::btlsw_notify_room_seat_onoff_out(){
	this->init();
}
void btlsw_notify_room_seat_onoff_out::init(){
	this->team_id=0;
	this->seat_id=0;
	this->onoff=0;

}
bool  btlsw_notify_room_seat_onoff_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint8(this->team_id)) return false;
	if (!ba.read_uint8(this->seat_id)) return false;
	if (!ba.read_uint8(this->onoff)) return false;
	return true;
}

bool btlsw_notify_room_seat_onoff_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint8(this->team_id)) return false;
	if (!ba.write_uint8(this->seat_id)) return false;
	if (!ba.write_uint8(this->onoff)) return false;
	return true;
}

	
btlsw_online_register_in::btlsw_online_register_in(){
	this->init();
}
void btlsw_online_register_in::init(){
	this->online_id=0;

}
bool  btlsw_online_register_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->online_id)) return false;
	return true;
}

bool btlsw_online_register_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->online_id)) return false;
	return true;
}

	
btlsw_player_enter_hall_in::btlsw_player_enter_hall_in(){
	this->init();
}
void btlsw_player_enter_hall_in::init(){
	this->player_info.init();

}
bool  btlsw_player_enter_hall_in::read_from_buf(byte_array_t & ba ){
	if (!this->player_info.read_from_buf(ba)) return false;
	return true;
}

bool btlsw_player_enter_hall_in::write_to_buf(byte_array_t & ba ){
	if (!this->player_info.write_to_buf(ba)) return false;
	return true;
}

	
btlsw_player_leave_notify_out::btlsw_player_leave_notify_out(){
	this->init();
}
void btlsw_player_leave_notify_out::init(){
	this->leaver_team=0;
	this->leaver_seat=0;
	this->leaver_uid=0;
	this->leaver_role_tm=0;
	this->is_kicked=0;
	this->owner_team=0;
	this->owner_seat=0;
	this->owner_uid=0;
	this->owner_role_tm=0;

}
bool  btlsw_player_leave_notify_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint8(this->leaver_team)) return false;
	if (!ba.read_uint8(this->leaver_seat)) return false;
	if (!ba.read_uint32(this->leaver_uid)) return false;
	if (!ba.read_int32(this->leaver_role_tm)) return false;
	if (!ba.read_uint8(this->is_kicked)) return false;
	if (!ba.read_uint8(this->owner_team)) return false;
	if (!ba.read_uint8(this->owner_seat)) return false;
	if (!ba.read_uint32(this->owner_uid)) return false;
	if (!ba.read_int32(this->owner_role_tm)) return false;
	return true;
}

bool btlsw_player_leave_notify_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint8(this->leaver_team)) return false;
	if (!ba.write_uint8(this->leaver_seat)) return false;
	if (!ba.write_uint32(this->leaver_uid)) return false;
	if (!ba.write_int32(this->leaver_role_tm)) return false;
	if (!ba.write_uint8(this->is_kicked)) return false;
	if (!ba.write_uint8(this->owner_team)) return false;
	if (!ba.write_uint8(this->owner_seat)) return false;
	if (!ba.write_uint32(this->owner_uid)) return false;
	if (!ba.write_int32(this->owner_role_tm)) return false;
	return true;
}

	
btlsw_player_sitdown_notify_out::btlsw_player_sitdown_notify_out(){
	this->init();
}
void btlsw_player_sitdown_notify_out::init(){
	this->sitdown_uid=0;
	this->sitdown_role_tm=0;
	this->from_team_id=0;
	this->from_seat_id=0;
	this->to_team_id=0;
	this->to_seat_id=0;

}
bool  btlsw_player_sitdown_notify_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->sitdown_uid)) return false;
	if (!ba.read_int32(this->sitdown_role_tm)) return false;
	if (!ba.read_uint8(this->from_team_id)) return false;
	if (!ba.read_uint8(this->from_seat_id)) return false;
	if (!ba.read_uint8(this->to_team_id)) return false;
	if (!ba.read_uint8(this->to_seat_id)) return false;
	return true;
}

bool btlsw_player_sitdown_notify_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->sitdown_uid)) return false;
	if (!ba.write_int32(this->sitdown_role_tm)) return false;
	if (!ba.write_uint8(this->from_team_id)) return false;
	if (!ba.write_uint8(this->from_seat_id)) return false;
	if (!ba.write_uint8(this->to_team_id)) return false;
	if (!ba.write_uint8(this->to_seat_id)) return false;
	return true;
}

	
btlsw_pvp_create_room_in::btlsw_pvp_create_room_in(){
	this->init();
}
void btlsw_pvp_create_room_in::init(){
	this->room_mode=0;
	this->map_id=0;
	this->map_lv=0;

}
bool  btlsw_pvp_create_room_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->room_name,16)) return false;
	if(!ba.read_buf(this->password,16)) return false;
	if (!ba.read_uint8(this->room_mode)) return false;
	if (!ba.read_uint32(this->map_id)) return false;
	if (!ba.read_uint8(this->map_lv)) return false;
	return true;
}

bool btlsw_pvp_create_room_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->room_name,16)) return false;
	if(!ba.write_buf(this->password,16)) return false;
	if (!ba.write_uint8(this->room_mode)) return false;
	if (!ba.write_uint32(this->map_id)) return false;
	if (!ba.write_uint8(this->map_lv)) return false;
	return true;
}

	
btlsw_pvp_create_room_out::btlsw_pvp_create_room_out(){
	this->init();
}
void btlsw_pvp_create_room_out::init(){
	this->team_id=0;
	this->seat_id=0;
	this->room_info.init();
	this->pet_info.init();

}
bool  btlsw_pvp_create_room_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint8(this->team_id)) return false;
	if (!ba.read_uint8(this->seat_id)) return false;
	if (!this->room_info.read_from_buf(ba)) return false;
	if (!this->pet_info.read_from_buf(ba)) return false;
	return true;
}

bool btlsw_pvp_create_room_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint8(this->team_id)) return false;
	if (!ba.write_uint8(this->seat_id)) return false;
	if (!this->room_info.write_to_buf(ba)) return false;
	if (!this->pet_info.write_to_buf(ba)) return false;
	return true;
}

	
btlsw_pvp_end_battle_in::btlsw_pvp_end_battle_in(){
	this->init();
}
void btlsw_pvp_end_battle_in::init(){
	this->online_id=0;
	this->room_id=0;

}
bool  btlsw_pvp_end_battle_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->online_id)) return false;
	if (!ba.read_uint32(this->room_id)) return false;
	return true;
}

bool btlsw_pvp_end_battle_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->online_id)) return false;
	if (!ba.write_uint32(this->room_id)) return false;
	return true;
}

	
btlsw_pvp_join_room_in::btlsw_pvp_join_room_in(){
	this->init();
}
void btlsw_pvp_join_room_in::init(){
	this->room_id=0;

}
bool  btlsw_pvp_join_room_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->room_id)) return false;
	if(!ba.read_buf(this->password,16)) return false;
	return true;
}

bool btlsw_pvp_join_room_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->room_id)) return false;
	if(!ba.write_buf(this->password,16)) return false;
	return true;
}

	
btlsw_pvp_join_room_out::btlsw_pvp_join_room_out(){
	this->init();
}
void btlsw_pvp_join_room_out::init(){
	this->room_id=0;
	this->team_id=0;
	this->seat_id=0;

}
bool  btlsw_pvp_join_room_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->room_id)) return false;
	if (!ba.read_uint8(this->team_id)) return false;
	if (!ba.read_uint8(this->seat_id)) return false;
	return true;
}

bool btlsw_pvp_join_room_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->room_id)) return false;
	if (!ba.write_uint8(this->team_id)) return false;
	if (!ba.write_uint8(this->seat_id)) return false;
	return true;
}

	
btlsw_pvp_query_room_info_in::btlsw_pvp_query_room_info_in(){
	this->init();
}
void btlsw_pvp_query_room_info_in::init(){
	this->room_id=0;

}
bool  btlsw_pvp_query_room_info_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->room_id)) return false;
	return true;
}

bool btlsw_pvp_query_room_info_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->room_id)) return false;
	return true;
}

	
btlsw_pvp_query_room_info_out::btlsw_pvp_query_room_info_out(){
	this->init();
}
void btlsw_pvp_query_room_info_out::init(){
	this->room_info.init();

}
bool  btlsw_pvp_query_room_info_out::read_from_buf(byte_array_t & ba ){
	if (!this->room_info.read_from_buf(ba)) return false;
	return true;
}

bool btlsw_pvp_query_room_info_out::write_to_buf(byte_array_t & ba ){
	if (!this->room_info.write_to_buf(ba)) return false;
	return true;
}

	
btlsw_pvp_room_attr_notify_out::btlsw_pvp_room_attr_notify_out(){
	this->init();
}
void btlsw_pvp_room_attr_notify_out::init(){
	this->room_info.init();

}
bool  btlsw_pvp_room_attr_notify_out::read_from_buf(byte_array_t & ba ){
	if (!this->room_info.read_from_buf(ba)) return false;
	return true;
}

bool btlsw_pvp_room_attr_notify_out::write_to_buf(byte_array_t & ba ){
	if (!this->room_info.write_to_buf(ba)) return false;
	return true;
}

	
btlsw_pvp_room_list_in::btlsw_pvp_room_list_in(){
	this->init();
}
void btlsw_pvp_room_list_in::init(){
	this->room_mode=0;
	this->count=0;
	this->turn=0;
	this->bound_roomid=0;

}
bool  btlsw_pvp_room_list_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint8(this->room_mode)) return false;
	if (!ba.read_uint8(this->count)) return false;
	if (!ba.read_uint8(this->turn)) return false;
	if (!ba.read_uint32(this->bound_roomid)) return false;
	return true;
}

bool btlsw_pvp_room_list_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint8(this->room_mode)) return false;
	if (!ba.write_uint8(this->count)) return false;
	if (!ba.write_uint8(this->turn)) return false;
	if (!ba.write_uint32(this->bound_roomid)) return false;
	return true;
}

	
btlsw_pvp_room_list_out::btlsw_pvp_room_list_out(){
	this->init();
}
void btlsw_pvp_room_list_out::init(){
	this->room_list.clear();

}
bool  btlsw_pvp_room_list_out::read_from_buf(byte_array_t & ba ){

	uint32_t room_list_count ;
	if (!ba.read_uint32( room_list_count )) return false;
	room_list_info_t  room_list_item;
	this->room_list.clear();
	{for(uint32_t i=0; i<room_list_count;i++){
		if (!room_list_item.read_from_buf(ba)) return false;
		this->room_list.push_back(room_list_item);
	}}
	return true;
}

bool btlsw_pvp_room_list_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->room_list.size())) return false;
	{for(uint32_t i=0; i<this->room_list.size() ;i++){
		if (!this->room_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
btlsw_pvp_room_set_attr_in::btlsw_pvp_room_set_attr_in(){
	this->init();
}
void btlsw_pvp_room_set_attr_in::init(){
	this->room_info.init();

}
bool  btlsw_pvp_room_set_attr_in::read_from_buf(byte_array_t & ba ){
	if (!this->room_info.read_from_buf(ba)) return false;
	return true;
}

bool btlsw_pvp_room_set_attr_in::write_to_buf(byte_array_t & ba ){
	if (!this->room_info.write_to_buf(ba)) return false;
	return true;
}

	
btlsw_pvp_room_start_battle_in::btlsw_pvp_room_start_battle_in(){
	this->init();
}
void btlsw_pvp_room_start_battle_in::init(){
	this->btl_id=0;
	this->fd_idx=0;

}
bool  btlsw_pvp_room_start_battle_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->btl_id)) return false;
	if (!ba.read_uint32(this->fd_idx)) return false;
	return true;
}

bool btlsw_pvp_room_start_battle_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->btl_id)) return false;
	if (!ba.write_uint32(this->fd_idx)) return false;
	return true;
}

	
btlsw_pvp_room_start_battle_out::btlsw_pvp_room_start_battle_out(){
	this->init();
}
void btlsw_pvp_room_start_battle_out::init(){
	this->btl_id=0;
	this->fd_idx=0;

}
bool  btlsw_pvp_room_start_battle_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->btl_id)) return false;
	if (!ba.read_uint32(this->fd_idx)) return false;
	return true;
}

bool btlsw_pvp_room_start_battle_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->btl_id)) return false;
	if (!ba.write_uint32(this->fd_idx)) return false;
	return true;
}

	
btlsw_pvp_room_start_out::btlsw_pvp_room_start_out(){
	this->init();
}
void btlsw_pvp_room_start_out::init(){
	this->map_id=0;
	this->player_count=0;

}
bool  btlsw_pvp_room_start_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->map_id)) return false;
	if (!ba.read_uint16(this->player_count)) return false;
	return true;
}

bool btlsw_pvp_room_start_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->map_id)) return false;
	if (!ba.write_uint16(this->player_count)) return false;
	return true;
}

	
btlsw_room_player_set_attr_in::btlsw_room_player_set_attr_in(){
	this->init();
}
void btlsw_room_player_set_attr_in::init(){
	this->which=0;
	this->new_team_id=0;
	this->new_seat_id=0;
	this->new_status=0;

}
bool  btlsw_room_player_set_attr_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint8(this->which)) return false;
	if (!ba.read_uint8(this->new_team_id)) return false;
	if (!ba.read_uint8(this->new_seat_id)) return false;
	if (!ba.read_uint8(this->new_status)) return false;
	return true;
}

bool btlsw_room_player_set_attr_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint8(this->which)) return false;
	if (!ba.write_uint8(this->new_team_id)) return false;
	if (!ba.write_uint8(this->new_seat_id)) return false;
	if (!ba.write_uint8(this->new_status)) return false;
	return true;
}

	
btlsw_set_room_seat_onoff_in::btlsw_set_room_seat_onoff_in(){
	this->init();
}
void btlsw_set_room_seat_onoff_in::init(){
	this->team_id=0;
	this->seat_id=0;
	this->onoff=0;

}
bool  btlsw_set_room_seat_onoff_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint8(this->team_id)) return false;
	if (!ba.read_uint8(this->seat_id)) return false;
	if (!ba.read_uint8(this->onoff)) return false;
	return true;
}

bool btlsw_set_room_seat_onoff_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint8(this->team_id)) return false;
	if (!ba.write_uint8(this->seat_id)) return false;
	if (!ba.write_uint8(this->onoff)) return false;
	return true;
}

	