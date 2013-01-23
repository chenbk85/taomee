#include "pea.h"

uid_role_t::uid_role_t(){
	this->init();
}
void uid_role_t::init(){
	this->user_id=0;
	this->role_tm=0;

}
bool  uid_role_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->user_id)) return false;
	if (!ba.read_uint32(this->role_tm)) return false;
	return true;
}

bool uid_role_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->user_id)) return false;
	if (!ba.write_uint32(this->role_tm)) return false;
	return true;
}

	
room_info_t::room_info_t(){
	this->init();
}
void room_info_t::init(){
	this->room_id=0;
	this->room_type=0;
	this->room_mode=0;
	this->map_id=0;
	this->map_lv=0;

}
bool  room_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->room_id)) return false;
	if(!ba.read_buf(this->room_name,16)) return false;
	if(!ba.read_buf(this->password,16)) return false;
	if (!ba.read_uint8(this->room_type)) return false;
	if (!ba.read_uint8(this->room_mode)) return false;
	if (!ba.read_uint32(this->map_id)) return false;
	if (!ba.read_uint8(this->map_lv)) return false;
	return true;
}

bool room_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->room_id)) return false;
	if(!ba.write_buf(this->room_name,16)) return false;
	if(!ba.write_buf(this->password,16)) return false;
	if (!ba.write_uint8(this->room_type)) return false;
	if (!ba.write_uint8(this->room_mode)) return false;
	if (!ba.write_uint32(this->map_id)) return false;
	if (!ba.write_uint8(this->map_lv)) return false;
	return true;
}

	
model_info_t::model_info_t(){
	this->init();
}
void model_info_t::init(){
	this->eye_model=0;
	this->resource_id=0;

}
bool  model_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->eye_model)) return false;
	if (!ba.read_uint32(this->resource_id)) return false;
	return true;
}

bool model_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->eye_model)) return false;
	if (!ba.write_uint32(this->resource_id)) return false;
	return true;
}

	
room_pet_info_t::room_pet_info_t(){
	this->init();
}
void room_pet_info_t::init(){
	this->pet_id=0;
	this->pet_level=0;
	this->skill_1=0;
	this->skill_2=0;
	this->skill_3=0;
	this->uni_skill=0;

}
bool  room_pet_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->pet_id)) return false;
	if (!ba.read_uint32(this->pet_level)) return false;
	if (!ba.read_uint32(this->skill_1)) return false;
	if (!ba.read_uint32(this->skill_2)) return false;
	if (!ba.read_uint32(this->skill_3)) return false;
	if (!ba.read_uint32(this->uni_skill)) return false;
	return true;
}

bool room_pet_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->pet_id)) return false;
	if (!ba.write_uint32(this->pet_level)) return false;
	if (!ba.write_uint32(this->skill_1)) return false;
	if (!ba.write_uint32(this->skill_2)) return false;
	if (!ba.write_uint32(this->skill_3)) return false;
	if (!ba.write_uint32(this->uni_skill)) return false;
	return true;
}

	
simple_equip_info_t::simple_equip_info_t(){
	this->init();
}
void simple_equip_info_t::init(){
	this->equip_id=0;

}
bool  simple_equip_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->equip_id)) return false;
	return true;
}

bool simple_equip_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->equip_id)) return false;
	return true;
}

	
room_player_show_info_t::room_player_show_info_t(){
	this->init();
}
void room_player_show_info_t::init(){
	this->uid=0;
	this->role_tm=0;
	this->team_id=0;
	this->seat_id=0;
	this->player_status=0;
	this->model_info.init();
	this->pet_info.init();
	this->equip_info.clear();

}
bool  room_player_show_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->role_tm)) return false;
	if (!ba.read_uint8(this->team_id)) return false;
	if (!ba.read_uint8(this->seat_id)) return false;
	if (!ba.read_uint8(this->player_status)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!this->model_info.read_from_buf(ba)) return false;
	if (!this->pet_info.read_from_buf(ba)) return false;

	uint32_t equip_info_count ;
	if (!ba.read_uint32( equip_info_count )) return false;
	simple_equip_info_t  equip_info_item;
	this->equip_info.clear();
	{for(uint32_t i=0; i<equip_info_count;i++){
		if (!equip_info_item.read_from_buf(ba)) return false;
		this->equip_info.push_back(equip_info_item);
	}}
	return true;
}

bool room_player_show_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->role_tm)) return false;
	if (!ba.write_uint8(this->team_id)) return false;
	if (!ba.write_uint8(this->seat_id)) return false;
	if (!ba.write_uint8(this->player_status)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!this->model_info.write_to_buf(ba)) return false;
	if (!this->pet_info.write_to_buf(ba)) return false;
	if (!ba.write_uint32(this->equip_info.size())) return false;
	{for(uint32_t i=0; i<this->equip_info.size() ;i++){
		if (!this->equip_info[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
room_full_info_t::room_full_info_t(){
	this->init();
}
void room_full_info_t::init(){
	this->owner_uid=0;
	this->owner_role_tm=0;
	this->room_info.init();
	this->player_info.clear();

}
bool  room_full_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->owner_uid)) return false;
	if (!ba.read_int32(this->owner_role_tm)) return false;
	if (!this->room_info.read_from_buf(ba)) return false;

	uint32_t player_info_count ;
	if (!ba.read_uint32( player_info_count )) return false;
	room_player_show_info_t  player_info_item;
	this->player_info.clear();
	{for(uint32_t i=0; i<player_info_count;i++){
		if (!player_info_item.read_from_buf(ba)) return false;
		this->player_info.push_back(player_info_item);
	}}
	return true;
}

bool room_full_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->owner_uid)) return false;
	if (!ba.write_int32(this->owner_role_tm)) return false;
	if (!this->room_info.write_to_buf(ba)) return false;
	if (!ba.write_uint32(this->player_info.size())) return false;
	{for(uint32_t i=0; i<this->player_info.size() ;i++){
		if (!this->player_info[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
room_list_info_t::room_list_info_t(){
	this->init();
}
void room_list_info_t::init(){
	this->room_id=0;
	this->map_id=0;
	this->room_type=0;
	this->room_mode=0;
	this->encrypted_flag=0;
	this->room_status=0;
	this->player_count=0;
	this->max_count=0;

}
bool  room_list_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->room_id)) return false;
	if (!ba.read_uint32(this->map_id)) return false;
	if(!ba.read_buf(this->room_name,16)) return false;
	if (!ba.read_uint8(this->room_type)) return false;
	if (!ba.read_uint8(this->room_mode)) return false;
	if (!ba.read_uint8(this->encrypted_flag)) return false;
	if (!ba.read_uint8(this->room_status)) return false;
	if (!ba.read_uint8(this->player_count)) return false;
	if (!ba.read_uint8(this->max_count)) return false;
	return true;
}

bool room_list_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->room_id)) return false;
	if (!ba.write_uint32(this->map_id)) return false;
	if(!ba.write_buf(this->room_name,16)) return false;
	if (!ba.write_uint8(this->room_type)) return false;
	if (!ba.write_uint8(this->room_mode)) return false;
	if (!ba.write_uint8(this->encrypted_flag)) return false;
	if (!ba.write_uint8(this->room_status)) return false;
	if (!ba.write_uint8(this->player_count)) return false;
	if (!ba.write_uint8(this->max_count)) return false;
	return true;
}

	
btl_pet_info_t::btl_pet_info_t(){
	this->init();
}
void btl_pet_info_t::init(){
	this->pet_no=0;
	this->pet_id=0;
	this->level=0;
	this->exp=0;
	this->quality=0;
	this->status=0;
	this->width=0;
	this->height=0;

}
bool  btl_pet_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->pet_no)) return false;
	if (!ba.read_uint32(this->pet_id)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint32(this->quality)) return false;
	if (!ba.read_uint32(this->status)) return false;
	if (!ba.read_uint32(this->width)) return false;
	if (!ba.read_uint32(this->height)) return false;
	return true;
}

bool btl_pet_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->pet_no)) return false;
	if (!ba.write_uint32(this->pet_id)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint32(this->quality)) return false;
	if (!ba.write_uint32(this->status)) return false;
	if (!ba.write_uint32(this->width)) return false;
	if (!ba.write_uint32(this->height)) return false;
	return true;
}

	
btl_player_statistical_info_t::btl_player_statistical_info_t(){
	this->init();
}
void btl_player_statistical_info_t::init(){
	this->uid=0;
	this->team=0;
	this->hit_rate=0;
	this->damage=0;
	this->exp=0;
	this->extern_exp=0;

}
bool  btl_player_statistical_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->team)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->hit_rate)) return false;
	if (!ba.read_uint32(this->damage)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint32(this->extern_exp)) return false;
	return true;
}

bool btl_player_statistical_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->team)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->hit_rate)) return false;
	if (!ba.write_uint32(this->damage)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint32(this->extern_exp)) return false;
	return true;
}

	
btl_skill_info_t::btl_skill_info_t(){
	this->init();
}
void btl_skill_info_t::init(){
	this->skill_id=0;
	this->skill_lv=0;

}
bool  btl_skill_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->skill_id)) return false;
	if (!ba.read_uint32(this->skill_lv)) return false;
	return true;
}

bool btl_skill_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->skill_id)) return false;
	if (!ba.write_uint32(this->skill_lv)) return false;
	return true;
}

	
db_add_item_reply_t::db_add_item_reply_t(){
	this->init();
}
void db_add_item_reply_t::init(){
	this->id=0;
	this->grid_index=0;
	this->item_id=0;
	this->item_count=0;
	this->get_time=0;
	this->expire_time=0;

}
bool  db_add_item_reply_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->id)) return false;
	if (!ba.read_uint32(this->grid_index)) return false;
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->item_count)) return false;
	if (!ba.read_uint32(this->get_time)) return false;
	if (!ba.read_uint32(this->expire_time)) return false;
	return true;
}

bool db_add_item_reply_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->id)) return false;
	if (!ba.write_uint32(this->grid_index)) return false;
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->item_count)) return false;
	if (!ba.write_uint32(this->get_time)) return false;
	if (!ba.write_uint32(this->expire_time)) return false;
	return true;
}

	
db_add_item_request_t::db_add_item_request_t(){
	this->init();
}
void db_add_item_request_t::init(){
	this->grid_index=0;
	this->item_id=0;
	this->item_count=0;
	this->get_time=0;
	this->expire_time=0;

}
bool  db_add_item_request_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->grid_index)) return false;
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->item_count)) return false;
	if (!ba.read_uint32(this->get_time)) return false;
	if (!ba.read_uint32(this->expire_time)) return false;
	return true;
}

bool db_add_item_request_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->grid_index)) return false;
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->item_count)) return false;
	if (!ba.write_uint32(this->get_time)) return false;
	if (!ba.write_uint32(this->expire_time)) return false;
	return true;
}

	
db_change_pet_status_t::db_change_pet_status_t(){
	this->init();
}
void db_change_pet_status_t::init(){
	this->status=0;
	this->src_pet_no=0;
	this->des_pet_no=0;

}
bool  db_change_pet_status_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->status)) return false;
	if (!ba.read_uint32(this->src_pet_no)) return false;
	if (!ba.read_uint32(this->des_pet_no)) return false;
	return true;
}

bool db_change_pet_status_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->status)) return false;
	if (!ba.write_uint32(this->src_pet_no)) return false;
	if (!ba.write_uint32(this->des_pet_no)) return false;
	return true;
}

	
db_del_item_reply_t::db_del_item_reply_t(){
	this->init();
}
void db_del_item_reply_t::init(){
	this->grid_index=0;
	this->item_id=0;
	this->item_count=0;

}
bool  db_del_item_reply_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->grid_index)) return false;
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->item_count)) return false;
	return true;
}

bool db_del_item_reply_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->grid_index)) return false;
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->item_count)) return false;
	return true;
}

	
db_del_item_request_t::db_del_item_request_t(){
	this->init();
}
void db_del_item_request_t::init(){
	this->grid_index=0;
	this->item_id=0;
	this->item_count=0;

}
bool  db_del_item_request_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->grid_index)) return false;
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->item_count)) return false;
	return true;
}

bool db_del_item_request_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->grid_index)) return false;
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->item_count)) return false;
	return true;
}

	
db_equip_info_t::db_equip_info_t(){
	this->init();
}
void db_equip_info_t::init(){
	this->id=0;
	this->item_id=0;
	this->item_count=0;
	this->grid_index=0;
	this->get_time=0;
	this->expire_time=0;
	this->hide=0;

}
bool  db_equip_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->id)) return false;
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->item_count)) return false;
	if (!ba.read_uint32(this->grid_index)) return false;
	if (!ba.read_uint32(this->get_time)) return false;
	if (!ba.read_uint32(this->expire_time)) return false;
	if (!ba.read_uint8(this->hide)) return false;
	return true;
}

bool db_equip_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->id)) return false;
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->item_count)) return false;
	if (!ba.write_uint32(this->grid_index)) return false;
	if (!ba.write_uint32(this->get_time)) return false;
	if (!ba.write_uint32(this->expire_time)) return false;
	if (!ba.write_uint8(this->hide)) return false;
	return true;
}

	
db_extra_info_t::db_extra_info_t(){
	this->init();
}
void db_extra_info_t::init(){
	this->info_id=0;
	this->info_value=0;

}
bool  db_extra_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->info_id)) return false;
	if (!ba.read_uint32(this->info_value)) return false;
	return true;
}

bool db_extra_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->info_id)) return false;
	if (!ba.write_uint32(this->info_value)) return false;
	return true;
}

	
db_item_info_t::db_item_info_t(){
	this->init();
}
void db_item_info_t::init(){
	this->id=0;
	this->item_id=0;
	this->item_count=0;
	this->grid_index=0;
	this->get_time=0;
	this->expire_time=0;

}
bool  db_item_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->id)) return false;
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->item_count)) return false;
	if (!ba.read_uint32(this->grid_index)) return false;
	if (!ba.read_uint32(this->get_time)) return false;
	if (!ba.read_uint32(this->expire_time)) return false;
	return true;
}

bool db_item_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->id)) return false;
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->item_count)) return false;
	if (!ba.write_uint32(this->grid_index)) return false;
	if (!ba.write_uint32(this->get_time)) return false;
	if (!ba.write_uint32(this->expire_time)) return false;
	return true;
}

	
db_mail_head_info_t::db_mail_head_info_t(){
	this->init();
}
void db_mail_head_info_t::init(){
	this->mail_id=0;
	this->mail_time=0;
	this->mail_state=0;
	this->mail_templet=0;
	this->mail_type=0;
	this->sender_id=0;
	this->sender_role_tm=0;
	{for(uint32_t i=0; i<16;i++){
		this->sender_nick[i]=0;
	}}
	{for(uint32_t i=0; i<50;i++){
		this->mail_title[i]=0;
	}}

}
bool  db_mail_head_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->mail_id)) return false;
	if (!ba.read_uint32(this->mail_time)) return false;
	if (!ba.read_uint32(this->mail_state)) return false;
	if (!ba.read_uint32(this->mail_templet)) return false;
	if (!ba.read_uint32(this->mail_type)) return false;
	if (!ba.read_uint32(this->sender_id)) return false;
	if (!ba.read_uint32(this->sender_role_tm)) return false;
	{for(uint32_t i=0; i<16;i++){
		if (!ba.read_uint8(this->sender_nick[i])) return false;
	}}
	{for(uint32_t i=0; i<50;i++){
		if (!ba.read_uint8(this->mail_title[i])) return false;
	}}
	return true;
}

bool db_mail_head_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->mail_id)) return false;
	if (!ba.write_uint32(this->mail_time)) return false;
	if (!ba.write_uint32(this->mail_state)) return false;
	if (!ba.write_uint32(this->mail_templet)) return false;
	if (!ba.write_uint32(this->mail_type)) return false;
	if (!ba.write_uint32(this->sender_id)) return false;
	if (!ba.write_uint32(this->sender_role_tm)) return false;
	{for(uint32_t i=0; i<16;i++){
		if (!ba.write_uint8(this->sender_nick[i])) return false;
	}}
	{for(uint32_t i=0; i<50;i++){
		if (!ba.write_uint8(this->mail_title[i])) return false;
	}}
	return true;
}

	
db_pet_gift_info_t::db_pet_gift_info_t(){
	this->init();
}
void db_pet_gift_info_t::init(){
	this->gift_no=0;
	this->gift_id=0;
	this->gift_level=0;

}
bool  db_pet_gift_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gift_no)) return false;
	if (!ba.read_uint32(this->gift_id)) return false;
	if (!ba.read_uint32(this->gift_level)) return false;
	return true;
}

bool db_pet_gift_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gift_no)) return false;
	if (!ba.write_uint32(this->gift_id)) return false;
	if (!ba.write_uint32(this->gift_level)) return false;
	return true;
}

	
db_pet_info_t::db_pet_info_t(){
	this->init();
}
void db_pet_info_t::init(){
	this->pet_no=0;
	this->pet_id=0;
	this->level=0;
	this->exp=0;
	this->quality=0;
	this->iq=0;
	this->status=0;
	{for(uint32_t i=0; i<4;i++){
		this->rand_attr[i]=0;
	}}
	{for(uint32_t i=0; i<4;i++){
		this->train_attr[i]=0;
	}}
	{for(uint32_t i=0; i<4;i++){
		this->try_train_attr[i]=0;
	}}
	this->gift.clear();

}
bool  db_pet_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->pet_no)) return false;
	if (!ba.read_uint32(this->pet_id)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint32(this->quality)) return false;
	if (!ba.read_uint32(this->iq)) return false;
	if (!ba.read_uint32(this->status)) return false;
	{for(uint32_t i=0; i<4;i++){
		if (!ba.read_uint32(this->rand_attr[i])) return false;
	}}
	{for(uint32_t i=0; i<4;i++){
		if (!ba.read_uint32(this->train_attr[i])) return false;
	}}
	{for(uint32_t i=0; i<4;i++){
		if (!ba.read_uint32(this->try_train_attr[i])) return false;
	}}

	uint32_t gift_count ;
	if (!ba.read_uint32( gift_count )) return false;
	db_pet_gift_info_t  gift_item;
	this->gift.clear();
	{for(uint32_t i=0; i<gift_count;i++){
		if (!gift_item.read_from_buf(ba)) return false;
		this->gift.push_back(gift_item);
	}}
	return true;
}

bool db_pet_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->pet_no)) return false;
	if (!ba.write_uint32(this->pet_id)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint32(this->quality)) return false;
	if (!ba.write_uint32(this->iq)) return false;
	if (!ba.write_uint32(this->status)) return false;
	{for(uint32_t i=0; i<4;i++){
		if (!ba.write_uint32(this->rand_attr[i])) return false;
	}}
	{for(uint32_t i=0; i<4;i++){
		if (!ba.write_uint32(this->train_attr[i])) return false;
	}}
	{for(uint32_t i=0; i<4;i++){
		if (!ba.write_uint32(this->try_train_attr[i])) return false;
	}}
	if (!ba.write_uint32(this->gift.size())) return false;
	{for(uint32_t i=0; i<this->gift.size() ;i++){
		if (!this->gift[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
db_player_info::db_player_info(){
	this->init();
}
void db_player_info::init(){
	this->user_id=0;
	this->role_tm=0;
	this->model.init();
	this->server_id=0;
	this->last_login_tm=0;
	this->last_off_line_tm=0;
	this->exp=0;
	this->level=0;
	this->max_bag_grid_count=0;
	this->map_id=0;
	this->map_x=0;
	this->map_y=0;
	this->gold=0;
	this->forbid_friends_me=0;

}
bool  db_player_info::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->user_id)) return false;
	if (!ba.read_uint32(this->role_tm)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!this->model.read_from_buf(ba)) return false;
	if (!ba.read_uint32(this->server_id)) return false;
	if (!ba.read_uint32(this->last_login_tm)) return false;
	if (!ba.read_uint32(this->last_off_line_tm)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->max_bag_grid_count)) return false;
	if (!ba.read_uint32(this->map_id)) return false;
	if (!ba.read_uint32(this->map_x)) return false;
	if (!ba.read_uint32(this->map_y)) return false;
	if (!ba.read_uint32(this->gold)) return false;
	if (!ba.read_uint32(this->forbid_friends_me)) return false;
	return true;
}

bool db_player_info::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->user_id)) return false;
	if (!ba.write_uint32(this->role_tm)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!this->model.write_to_buf(ba)) return false;
	if (!ba.write_uint32(this->server_id)) return false;
	if (!ba.write_uint32(this->last_login_tm)) return false;
	if (!ba.write_uint32(this->last_off_line_tm)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->max_bag_grid_count)) return false;
	if (!ba.write_uint32(this->map_id)) return false;
	if (!ba.write_uint32(this->map_x)) return false;
	if (!ba.write_uint32(this->map_y)) return false;
	if (!ba.write_uint32(this->gold)) return false;
	if (!ba.write_uint32(this->forbid_friends_me)) return false;
	return true;
}

	
db_prize_t::db_prize_t(){
	this->init();
}
void db_prize_t::init(){
	this->prize_id=0;
	this->count=0;

}
bool  db_prize_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->prize_id)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool db_prize_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->prize_id)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
db_update_pet_gift_t::db_update_pet_gift_t(){
	this->init();
}
void db_update_pet_gift_t::init(){
	this->gift_no=0;
	this->gift_id=0;
	this->gift_level=0;

}
bool  db_update_pet_gift_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gift_no)) return false;
	if (!ba.read_uint32(this->gift_id)) return false;
	if (!ba.read_uint32(this->gift_level)) return false;
	return true;
}

bool db_update_pet_gift_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gift_no)) return false;
	if (!ba.write_uint32(this->gift_id)) return false;
	if (!ba.write_uint32(this->gift_level)) return false;
	return true;
}

	
db_user_id_t::db_user_id_t(){
	this->init();
}
void db_user_id_t::init(){
	this->user_id=0;
	this->role_tm=0;
	this->server_id=0;

}
bool  db_user_id_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->user_id)) return false;
	if (!ba.read_uint32(this->role_tm)) return false;
	if (!ba.read_uint32(this->server_id)) return false;
	return true;
}

bool db_user_id_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->user_id)) return false;
	if (!ba.write_uint32(this->role_tm)) return false;
	if (!ba.write_uint32(this->server_id)) return false;
	return true;
}

	
equip_change_t::equip_change_t(){
	this->init();
}
void equip_change_t::init(){
	this->body_index=0;
	this->empty_bag_index=0;
	this->hide=0;

}
bool  equip_change_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->body_index)) return false;
	if (!ba.read_uint32(this->empty_bag_index)) return false;
	if (!ba.read_uint8(this->hide)) return false;
	return true;
}

bool equip_change_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->body_index)) return false;
	if (!ba.write_uint32(this->empty_bag_index)) return false;
	if (!ba.write_uint8(this->hide)) return false;
	return true;
}

	
equip_enclosure_info_t::equip_enclosure_info_t(){
	this->init();
}
void equip_enclosure_info_t::init(){
	this->equip_id=0;
	this->equip_count=0;
	this->duration_time=0;
	this->end_time=0;

}
bool  equip_enclosure_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->equip_id)) return false;
	if (!ba.read_uint32(this->equip_count)) return false;
	if (!ba.read_uint32(this->duration_time)) return false;
	if (!ba.read_uint32(this->end_time)) return false;
	return true;
}

bool equip_enclosure_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->equip_id)) return false;
	if (!ba.write_uint32(this->equip_count)) return false;
	if (!ba.write_uint32(this->duration_time)) return false;
	if (!ba.write_uint32(this->end_time)) return false;
	return true;
}

	
item_enclosure_info_t::item_enclosure_info_t(){
	this->init();
}
void item_enclosure_info_t::init(){
	this->item_id=0;
	this->item_count=0;
	this->duration_time=0;
	this->end_time=0;

}
bool  item_enclosure_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->item_count)) return false;
	if (!ba.read_uint32(this->duration_time)) return false;
	if (!ba.read_uint32(this->end_time)) return false;
	return true;
}

bool item_enclosure_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->item_count)) return false;
	if (!ba.write_uint32(this->duration_time)) return false;
	if (!ba.write_uint32(this->end_time)) return false;
	return true;
}

	
login_player_info_t::login_player_info_t(){
	this->init();
}
void login_player_info_t::init(){
	this->user_id=0;
	this->role_tm=0;
	this->model.init();
	this->server_id=0;
	this->level=0;
	this->equip_info.clear();

}
bool  login_player_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->user_id)) return false;
	if (!ba.read_uint32(this->role_tm)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!this->model.read_from_buf(ba)) return false;
	if (!ba.read_uint32(this->server_id)) return false;
	if (!ba.read_uint32(this->level)) return false;

	uint32_t equip_info_count ;
	if (!ba.read_uint32( equip_info_count )) return false;
	simple_equip_info_t  equip_info_item;
	this->equip_info.clear();
	{for(uint32_t i=0; i<equip_info_count;i++){
		if (!equip_info_item.read_from_buf(ba)) return false;
		this->equip_info.push_back(equip_info_item);
	}}
	return true;
}

bool login_player_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->user_id)) return false;
	if (!ba.write_uint32(this->role_tm)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!this->model.write_to_buf(ba)) return false;
	if (!ba.write_uint32(this->server_id)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->equip_info.size())) return false;
	{for(uint32_t i=0; i<this->equip_info.size() ;i++){
		if (!this->equip_info[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
msg_t::msg_t(){
	this->init();
}
void msg_t::init(){
	this->_msg_len=0;

}
bool  msg_t::read_from_buf(byte_array_t & ba ){

	if (!ba.read_uint32(this->_msg_len )) return false;
	if (this->_msg_len>512) return false;
 	if (!ba.read_buf(this->msg,this->_msg_len)) return false;
	return true;
}

bool msg_t::write_to_buf(byte_array_t & ba ){
	if (this->_msg_len>512) return false;
	if (!ba.write_uint32(this->_msg_len))return false;
	if (!ba.write_buf(this->msg,this->_msg_len)) return false;
	return true;
}

	
msg_list_t::msg_list_t(){
	this->init();
}
void msg_list_t::init(){
	this->msg_list.clear();

}
bool  msg_list_t::read_from_buf(byte_array_t & ba ){

	uint32_t msg_list_count ;
	if (!ba.read_uint32( msg_list_count )) return false;
	msg_t  msg_list_item;
	this->msg_list.clear();
	{for(uint32_t i=0; i<msg_list_count;i++){
		if (!msg_list_item.read_from_buf(ba)) return false;
		this->msg_list.push_back(msg_list_item);
	}}
	return true;
}

bool msg_list_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->msg_list.size())) return false;
	{for(uint32_t i=0; i<this->msg_list.size() ;i++){
		if (!this->msg_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
numerical_enclosure_info_t::numerical_enclosure_info_t(){
	this->init();
}
void numerical_enclosure_info_t::init(){
	this->type=0;
	this->number=0;

}
bool  numerical_enclosure_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->type)) return false;
	if (!ba.read_uint32(this->number)) return false;
	return true;
}

bool numerical_enclosure_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->type)) return false;
	if (!ba.write_uint32(this->number)) return false;
	return true;
}

	
room_player_info_t::room_player_info_t(){
	this->init();
}
void room_player_info_t::init(){
	this->uid=0;
	this->role_tm=0;
	this->online_id=0;
	this->room_id=0;
	this->owner_uid=0;
	this->team_id=0;
	this->seat_id=0;

}
bool  room_player_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->role_tm)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->online_id)) return false;
	if (!ba.read_uint32(this->room_id)) return false;
	if (!ba.read_uint32(this->owner_uid)) return false;
	if (!ba.read_uint8(this->team_id)) return false;
	if (!ba.read_uint8(this->seat_id)) return false;
	return true;
}

bool room_player_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->role_tm)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->online_id)) return false;
	if (!ba.write_uint32(this->room_id)) return false;
	if (!ba.write_uint32(this->owner_uid)) return false;
	if (!ba.write_uint8(this->team_id)) return false;
	if (!ba.write_uint8(this->seat_id)) return false;
	return true;
}

	
simple_union_info_t::simple_union_info_t(){
	this->init();
}
void simple_union_info_t::init(){
	this->union_id=0;

}
bool  simple_union_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->union_id)) return false;
	if(!ba.read_buf(this->union_name,16)) return false;
	return true;
}

bool simple_union_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->union_id)) return false;
	if(!ba.write_buf(this->union_name,16)) return false;
	return true;
}

	
simple_user_info_t::simple_user_info_t(){
	this->init();
}
void simple_user_info_t::init(){
	this->user_id=0;

}
bool  simple_user_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->user_id)) return false;
	if(!ba.read_buf(this->user_nick,16)) return false;
	return true;
}

bool simple_user_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->user_id)) return false;
	if(!ba.write_buf(this->user_nick,16)) return false;
	return true;
}

	