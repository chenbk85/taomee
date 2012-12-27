#include "main_login.h"

main_login_add_game_in::main_login_add_game_in(){
	this->init();
}
void main_login_add_game_in::init(){
	this->channel_id=0;
	this->idczone=0;
	this->gameid=0;

}
bool  main_login_add_game_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint16(this->channel_id)) return false;
	if(!ba.read_buf(this->verify_code,32)) return false;
	if (!ba.read_uint32(this->idczone)) return false;
	if (!ba.read_uint32(this->gameid)) return false;
	return true;
}

bool main_login_add_game_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint16(this->channel_id)) return false;
	if(!ba.write_buf(this->verify_code,32)) return false;
	if (!ba.write_uint32(this->idczone)) return false;
	if (!ba.write_uint32(this->gameid)) return false;
	return true;
}
	
main_login_add_game_with_chanel_in::main_login_add_game_with_chanel_in(){
	this->init();
}
void main_login_add_game_with_chanel_in::init(){
	this->channel_code=0;
	this->idczone=0;
	this->gameid=0;
	this->channel_id=0;

}
bool  main_login_add_game_with_chanel_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint16(this->channel_code)) return false;
	if(!ba.read_buf(this->verify_code,32)) return false;
	if (!ba.read_uint32(this->idczone)) return false;
	if (!ba.read_uint32(this->gameid)) return false;
	if (!ba.read_uint16(this->channel_id)) return false;
	return true;
}

bool main_login_add_game_with_chanel_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint16(this->channel_code)) return false;
	if(!ba.write_buf(this->verify_code,32)) return false;
	if (!ba.write_uint32(this->idczone)) return false;
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint16(this->channel_id)) return false;
	return true;
}
	
main_login_add_session_in::main_login_add_session_in(){
	this->init();
}
void main_login_add_session_in::init(){
	this->channel_id=0;
	this->gameid=0;
	this->ip=0;

}
bool  main_login_add_session_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint16(this->channel_id)) return false;
	if(!ba.read_buf(this->verify_code,32)) return false;
	if (!ba.read_uint32(this->gameid)) return false;
	if (!ba.read_uint32(this->ip)) return false;
	return true;
}

bool main_login_add_session_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint16(this->channel_id)) return false;
	if(!ba.write_buf(this->verify_code,32)) return false;
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->ip)) return false;
	return true;
}
	
main_login_add_session_out::main_login_add_session_out(){
	this->init();
}
void main_login_add_session_out::init(){

}
bool  main_login_add_session_out::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->session,16)) return false;
	return true;
}

bool main_login_add_session_out::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->session,16)) return false;
	return true;
}
	
main_login_check_session_in::main_login_check_session_in(){
	this->init();
}
void main_login_check_session_in::init(){
	this->gameid=0;
	this->del_session_flag=0;

}
bool  main_login_check_session_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;
	if(!ba.read_buf(this->session,16)) return false;
	if (!ba.read_uint32(this->del_session_flag)) return false;
	return true;
}

bool main_login_check_session_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if(!ba.write_buf(this->session,16)) return false;
	if (!ba.write_uint32(this->del_session_flag)) return false;
	return true;
}
	
main_login_check_session_new_in::main_login_check_session_new_in(){
	this->init();
}
void main_login_check_session_new_in::init(){
	this->from_game=0;
	this->del_session_flag=0;
	this->to_game=0;
	this->ip=0;
	this->region=0;
	this->enter_game=0;
	{for(uint32_t i=0; i<128;i++){
		this->tad[i]=0;
	}}

}
bool  main_login_check_session_new_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->from_game)) return false;
	if(!ba.read_buf(this->session,16)) return false;
	if (!ba.read_uint32(this->del_session_flag)) return false;
	if (!ba.read_uint32(this->to_game)) return false;
	if (!ba.read_uint32(this->ip)) return false;
	if (!ba.read_uint16(this->region)) return false;
	if (!ba.read_uint8(this->enter_game)) return false;
	{for(uint32_t i=0; i<128;i++){
		if (!ba.read_uint8(this->tad[i])) return false;
	}}
	return true;
}

bool main_login_check_session_new_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->from_game)) return false;
	if(!ba.write_buf(this->session,16)) return false;
	if (!ba.write_uint32(this->del_session_flag)) return false;
	if (!ba.write_uint32(this->to_game)) return false;
	if (!ba.write_uint32(this->ip)) return false;
	if (!ba.write_uint16(this->region)) return false;
	if (!ba.write_uint8(this->enter_game)) return false;
	{for(uint32_t i=0; i<128;i++){
		if (!ba.write_uint8(this->tad[i])) return false;
	}}
	return true;
}
	
main_login_login_in::main_login_login_in(){
	this->init();
}
void main_login_login_in::init(){
	this->channel_id=0;
	this->region=0;
	this->gameid=0;
	this->ip=0;

}
bool  main_login_login_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->email,64)) return false;
	if(!ba.read_buf(this->passwd_md5_two,16)) return false;
	if (!ba.read_uint16(this->channel_id)) return false;
	if (!ba.read_uint16(this->region)) return false;
	if (!ba.read_uint16(this->gameid)) return false;
	if (!ba.read_uint32(this->ip)) return false;
	return true;
}

bool main_login_login_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->email,64)) return false;
	if(!ba.write_buf(this->passwd_md5_two,16)) return false;
	if (!ba.write_uint16(this->channel_id)) return false;
	if (!ba.write_uint16(this->region)) return false;
	if (!ba.write_uint16(this->gameid)) return false;
	if (!ba.write_uint32(this->ip)) return false;
	return true;
}
	
main_login_login_out::main_login_login_out(){
	this->init();
}
void main_login_login_out::init(){
	this->gameflag=0;

}
bool  main_login_login_out::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->session,16)) return false;
	if (!ba.read_uint32(this->gameflag)) return false;
	return true;
}

bool main_login_login_out::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->session,16)) return false;
	if (!ba.write_uint32(this->gameflag)) return false;
	return true;
}
	
main_login_with_verif_img_in::main_login_with_verif_img_in(){
	this->init();
}
void main_login_with_verif_img_in::init(){
	this->channel_id=0;
	this->region=0;
	this->gameid=0;
	this->ip=0;
	this->channel_code=0;

}
bool  main_login_with_verif_img_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->email,64)) return false;
	if(!ba.read_buf(this->passwd_md5_two,16)) return false;
	if (!ba.read_uint16(this->channel_id)) return false;
	if (!ba.read_uint16(this->region)) return false;
	if (!ba.read_uint16(this->gameid)) return false;
	if (!ba.read_uint32(this->ip)) return false;
	if(!ba.read_buf(this->verif_session,16)) return false;
	if(!ba.read_buf(this->verif_code,6)) return false;
	if (!ba.read_uint16(this->channel_code)) return false;
	return true;
}

bool main_login_with_verif_img_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->email,64)) return false;
	if(!ba.write_buf(this->passwd_md5_two,16)) return false;
	if (!ba.write_uint16(this->channel_id)) return false;
	if (!ba.write_uint16(this->region)) return false;
	if (!ba.write_uint16(this->gameid)) return false;
	if (!ba.write_uint32(this->ip)) return false;
	if(!ba.write_buf(this->verif_session,16)) return false;
	if(!ba.write_buf(this->verif_code,6)) return false;
	if (!ba.write_uint16(this->channel_code)) return false;
	return true;
}
	
pay_become_vip_in::pay_become_vip_in(){
	this->init();
}
void pay_become_vip_in::init(){
	this->channel_id=0;
	this->is_auto_charge=0;
	this->use_coupon=0;
	this->voucher=0;
	this->which_line=0;

}
bool  pay_become_vip_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint16(this->channel_id)) return false;
	if(!ba.read_buf(this->verify_code,32)) return false;
	if (!ba.read_uint8(this->is_auto_charge)) return false;
	if (!ba.read_uint8(this->use_coupon)) return false;
	if(!ba.read_buf(this->coupon,9)) return false;
	if (!ba.read_uint16(this->voucher)) return false;
	if (!ba.read_uint8(this->which_line)) return false;
	if(!ba.read_buf(this->extend,19)) return false;
	return true;
}

bool pay_become_vip_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint16(this->channel_id)) return false;
	if(!ba.write_buf(this->verify_code,32)) return false;
	if (!ba.write_uint8(this->is_auto_charge)) return false;
	if (!ba.write_uint8(this->use_coupon)) return false;
	if(!ba.write_buf(this->coupon,9)) return false;
	if (!ba.write_uint16(this->voucher)) return false;
	if (!ba.write_uint8(this->which_line)) return false;
	if(!ba.write_buf(this->extend,19)) return false;
	return true;
}
	
pay_become_vip_out::pay_become_vip_out(){
	this->init();
}
void pay_become_vip_out::init(){
	this->mb_consumed=0;
	this->mb_balance=0;

}
bool  pay_become_vip_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->mb_consumed)) return false;
	if (!ba.read_uint32(this->mb_balance)) return false;
	return true;
}

bool pay_become_vip_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->mb_consumed)) return false;
	if (!ba.write_uint32(this->mb_balance)) return false;
	return true;
}
	
pay_get_become_vip_price_out::pay_get_become_vip_price_out(){
	this->init();
}
void pay_get_become_vip_price_out::init(){
	this->month1_price=0;
	this->month3_price=0;
	this->month6_price=0;
	this->month12_price=0;

}
bool  pay_get_become_vip_price_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->month1_price)) return false;
	if (!ba.read_uint32(this->month3_price)) return false;
	if (!ba.read_uint32(this->month6_price)) return false;
	if (!ba.read_uint32(this->month12_price)) return false;
	return true;
}

bool pay_get_become_vip_price_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->month1_price)) return false;
	if (!ba.write_uint32(this->month3_price)) return false;
	if (!ba.write_uint32(this->month6_price)) return false;
	if (!ba.write_uint32(this->month12_price)) return false;
	return true;
}
	
pay_get_mb_left_out::pay_get_mb_left_out(){
	this->init();
}
void pay_get_mb_left_out::init(){
	this->mb_balance=0;

}
bool  pay_get_mb_left_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->mb_balance)) return false;
	return true;
}

bool pay_get_mb_left_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->mb_balance)) return false;
	return true;
}
	
userinfo_check_pay_password_in::userinfo_check_pay_password_in(){
	this->init();
}
void userinfo_check_pay_password_in::init(){
	{for(uint32_t i=0; i<16;i++){
		this->password[i]=0;
	}}

}
bool  userinfo_check_pay_password_in::read_from_buf(byte_array_t & ba ){
	{for(uint32_t i=0; i<16;i++){
		if (!ba.read_uint8(this->password[i])) return false;
	}}
	return true;
}

bool userinfo_check_pay_password_in::write_to_buf(byte_array_t & ba ){
	{for(uint32_t i=0; i<16;i++){
		if (!ba.write_uint8(this->password[i])) return false;
	}}
	return true;
}
	
userinfo_get_gameflag_in::userinfo_get_gameflag_in(){
	this->init();
}
void userinfo_get_gameflag_in::init(){
	this->region=0;

}
bool  userinfo_get_gameflag_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->region)) return false;
	return true;
}

bool userinfo_get_gameflag_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->region)) return false;
	return true;
}
	
userinfo_get_gameflag_out::userinfo_get_gameflag_out(){
	this->init();
}
void userinfo_get_gameflag_out::init(){
	this->mee_fans=0;
	this->game_act_flag=0;
	this->dx_gameflag=0;
	this->wt_gameflag=0;

}
bool  userinfo_get_gameflag_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->mee_fans)) return false;
	if (!ba.read_uint32(this->game_act_flag)) return false;
	if (!ba.read_uint32(this->dx_gameflag)) return false;
	if (!ba.read_uint32(this->wt_gameflag)) return false;
	return true;
}

bool userinfo_get_gameflag_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->mee_fans)) return false;
	if (!ba.write_uint32(this->game_act_flag)) return false;
	if (!ba.write_uint32(this->dx_gameflag)) return false;
	if (!ba.write_uint32(this->wt_gameflag)) return false;
	return true;
}
	
userinfo_isset_pay_password_out::userinfo_isset_pay_password_out(){
	this->init();
}
void userinfo_isset_pay_password_out::init(){
	this->is_seted=0;

}
bool  userinfo_isset_pay_password_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->is_seted)) return false;
	return true;
}

bool userinfo_isset_pay_password_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->is_seted)) return false;
	return true;
}
	
userinfo_set_pay_password_in::userinfo_set_pay_password_in(){
	this->init();
}
void userinfo_set_pay_password_in::init(){
	{for(uint32_t i=0; i<16;i++){
		this->password[i]=0;
	}}

}
bool  userinfo_set_pay_password_in::read_from_buf(byte_array_t & ba ){
	{for(uint32_t i=0; i<16;i++){
		if (!ba.read_uint8(this->password[i])) return false;
	}}
	return true;
}

bool userinfo_set_pay_password_in::write_to_buf(byte_array_t & ba ){
	{for(uint32_t i=0; i<16;i++){
		if (!ba.write_uint8(this->password[i])) return false;
	}}
	return true;
}
	
vip_add_days_in::vip_add_days_in(){
	this->init();
}
void vip_add_days_in::init(){
	this->days=0;
	this->add_flag=0;
	this->channel_id=0;

}
bool  vip_add_days_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->days)) return false;
	if (!ba.read_uint32(this->add_flag)) return false;
	if (!ba.read_uint32(this->channel_id)) return false;
	if(!ba.read_buf(this->trade_id,20)) return false;
	return true;
}

bool vip_add_days_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->days)) return false;
	if (!ba.write_uint32(this->add_flag)) return false;
	if (!ba.write_uint32(this->channel_id)) return false;
	if(!ba.write_buf(this->trade_id,20)) return false;
	return true;
}
	
vip_add_days_out::vip_add_days_out(){
	this->init();
}
void vip_add_days_out::init(){
	this->op_id=0;
	this->vip_ret=0;

}
bool  vip_add_days_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->op_id)) return false;
	if (!ba.read_uint32(this->vip_ret)) return false;
	return true;
}

bool vip_add_days_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->op_id)) return false;
	if (!ba.write_uint32(this->vip_ret)) return false;
	return true;
}
	
vip_get_info_out::vip_get_info_out(){
	this->init();
}
void vip_get_info_out::init(){
	this->vip_result=0;
	this->vip_is_auto_charge=0;
	this->vip_begin_time=0;
	this->vip_end_time=0;
	this->vip_channel_id=0;

}
bool  vip_get_info_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->vip_result)) return false;
	if (!ba.read_uint32(this->vip_is_auto_charge)) return false;
	if (!ba.read_uint32(this->vip_begin_time)) return false;
	if (!ba.read_uint32(this->vip_end_time)) return false;
	if (!ba.read_uint32(this->vip_channel_id)) return false;
	return true;
}

bool vip_get_info_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->vip_result)) return false;
	if (!ba.write_uint32(this->vip_is_auto_charge)) return false;
	if (!ba.write_uint32(this->vip_begin_time)) return false;
	if (!ba.write_uint32(this->vip_end_time)) return false;
	if (!ba.write_uint32(this->vip_channel_id)) return false;
	return true;
}
	
vip_sub_days_in::vip_sub_days_in(){
	this->init();
}
void vip_sub_days_in::init(){
	this->days=0;
	this->channel_id=0;

}
bool  vip_sub_days_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->days)) return false;
	if (!ba.read_uint32(this->channel_id)) return false;
	if(!ba.read_buf(this->trade_id,20)) return false;
	return true;
}

bool vip_sub_days_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->days)) return false;
	if (!ba.write_uint32(this->channel_id)) return false;
	if(!ba.write_buf(this->trade_id,20)) return false;
	return true;
}
	
vip_sub_days_out::vip_sub_days_out(){
	this->init();
}
void vip_sub_days_out::init(){
	this->op_id=0;
	this->vip_ret=0;
	this->vip_endtime=0;

}
bool  vip_sub_days_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->op_id)) return false;
	if (!ba.read_uint32(this->vip_ret)) return false;
	if (!ba.read_uint32(this->vip_endtime)) return false;
	return true;
}

bool vip_sub_days_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->op_id)) return false;
	if (!ba.write_uint32(this->vip_ret)) return false;
	if (!ba.write_uint32(this->vip_endtime)) return false;
	return true;
}
	