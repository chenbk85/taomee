#include "gf_db.h"

finish_stage_list_item::finish_stage_list_item(){
	this->init();
}
void finish_stage_list_item::init(){
	this->stage_id=0;
	{for(uint32_t i=0; i<4;i++){
		this->grade[i]=0;
	}}

}
bool  finish_stage_list_item::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->stage_id)) return false;
	{for(uint32_t i=0; i<4;i++){
		if (!ba.read_uint8(this->grade[i])) return false;
	}}
	return true;
}

bool finish_stage_list_item::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->stage_id)) return false;
	{for(uint32_t i=0; i<4;i++){
		if (!ba.write_uint8(this->grade[i])) return false;
	}}
	return true;
}
	
friend_list_item_t::friend_list_item_t(){
	this->init();
}
void friend_list_item_t::init(){
	this->friend_id=0;
	this->friend_type=0;

}
bool  friend_list_item_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->friend_id)) return false;
	if (!ba.read_uint32(this->friend_type)) return false;
	return true;
}

bool friend_list_item_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->friend_id)) return false;
	if (!ba.write_uint32(this->friend_type)) return false;
	return true;
}
	
gf_add_attire_kf_in::gf_add_attire_kf_in(){
	this->init();
}
void gf_add_attire_kf_in::init(){
	this->role_regtime=0;
	this->attireid=0;
	this->gettime=0;
	this->usedflag=0;
	this->duration=0;
	this->attire_lv=0;
	this->max_bag_count=0;

}
bool  gf_add_attire_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	if (!ba.read_uint32(this->attireid)) return false;
	if (!ba.read_uint32(this->gettime)) return false;
	if (!ba.read_uint32(this->usedflag)) return false;
	if (!ba.read_uint32(this->duration)) return false;
	if (!ba.read_uint32(this->attire_lv)) return false;
	if (!ba.read_uint32(this->max_bag_count)) return false;
	return true;
}

bool gf_add_attire_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	if (!ba.write_uint32(this->attireid)) return false;
	if (!ba.write_uint32(this->gettime)) return false;
	if (!ba.write_uint32(this->usedflag)) return false;
	if (!ba.write_uint32(this->duration)) return false;
	if (!ba.write_uint32(this->attire_lv)) return false;
	if (!ba.write_uint32(this->max_bag_count)) return false;
	return true;
}
	
gf_add_friend_kf_in::gf_add_friend_kf_in(){
	this->init();
}
void gf_add_friend_kf_in::init(){
	this->friend_id=0;

}
bool  gf_add_friend_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->friend_id)) return false;
	return true;
}

bool gf_add_friend_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->friend_id)) return false;
	return true;
}
	
gf_add_item_kf_in::gf_add_item_kf_in(){
	this->init();
}
void gf_add_item_kf_in::init(){
	this->role_regtime=0;
	this->item_id=0;
	this->add_num=0;
	this->max_bag_count=0;

}
bool  gf_add_item_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->add_num)) return false;
	if (!ba.read_uint32(this->max_bag_count)) return false;
	return true;
}

bool gf_add_item_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->add_num)) return false;
	if (!ba.write_uint32(this->max_bag_count)) return false;
	return true;
}
	
gf_add_killed_boss_kf_in::gf_add_killed_boss_kf_in(){
	this->init();
}
void gf_add_killed_boss_kf_in::init(){
	this->role_regtime=0;
	this->boss_id=0;

}
bool  gf_add_killed_boss_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	if (!ba.read_uint32(this->boss_id)) return false;
	return true;
}

bool gf_add_killed_boss_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	if (!ba.write_uint32(this->boss_id)) return false;
	return true;
}
	
gf_add_role_kf_in::gf_add_role_kf_in(){
	this->init();
}
void gf_add_role_kf_in::init(){
	this->role_type=0;
	this->level=0;
	this->hp=0;
	this->mp=0;
	this->parentid=0;

}
bool  gf_add_role_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_type)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->hp)) return false;
	if (!ba.read_uint32(this->mp)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->parentid)) return false;
	return true;
}

bool gf_add_role_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_type)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->hp)) return false;
	if (!ba.write_uint32(this->mp)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->parentid)) return false;
	return true;
}
	
gf_add_role_kf_out::gf_add_role_kf_out(){
	this->init();
}
void gf_add_role_kf_out::init(){
	this->role_num=0;
	this->user_existed=0;

}
bool  gf_add_role_kf_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_num)) return false;
	if (!ba.read_uint32(this->user_existed)) return false;
	return true;
}

bool gf_add_role_kf_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_num)) return false;
	if (!ba.write_uint32(this->user_existed)) return false;
	return true;
}
	
gf_add_skill_with_no_book_kf_in::gf_add_skill_with_no_book_kf_in(){
	this->init();
}
void gf_add_skill_with_no_book_kf_in::init(){
	this->skill_id=0;
	this->skill_lv=0;
	this->role_regtime=0;

}
bool  gf_add_skill_with_no_book_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->skill_id)) return false;
	if (!ba.read_uint32(this->skill_lv)) return false;
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_add_skill_with_no_book_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->skill_id)) return false;
	if (!ba.write_uint32(this->skill_lv)) return false;
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
gf_add_skill_with_no_book_kf_out::gf_add_skill_with_no_book_kf_out(){
	this->init();
}
void gf_add_skill_with_no_book_kf_out::init(){
	this->skill_id=0;

}
bool  gf_add_skill_with_no_book_kf_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->skill_id)) return false;
	return true;
}

bool gf_add_skill_with_no_book_kf_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->skill_id)) return false;
	return true;
}
	
gf_del_attire_kf_in::gf_del_attire_kf_in(){
	this->init();
}
void gf_del_attire_kf_in::init(){
	this->role_regtime=0;
	this->attire_id=0;
	this->index_id=0;

}
bool  gf_del_attire_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	if (!ba.read_uint32(this->attire_id)) return false;
	if (!ba.read_uint32(this->index_id)) return false;
	return true;
}

bool gf_del_attire_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	if (!ba.write_uint32(this->attire_id)) return false;
	if (!ba.write_uint32(this->index_id)) return false;
	return true;
}
	
gf_del_attire_kf_out::gf_del_attire_kf_out(){
	this->init();
}
void gf_del_attire_kf_out::init(){
	this->attire_id=0;
	this->index_id=0;

}
bool  gf_del_attire_kf_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->attire_id)) return false;
	if (!ba.read_uint32(this->index_id)) return false;
	return true;
}

bool gf_del_attire_kf_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->attire_id)) return false;
	if (!ba.write_uint32(this->index_id)) return false;
	return true;
}
	
gf_del_friend_kf_in::gf_del_friend_kf_in(){
	this->init();
}
void gf_del_friend_kf_in::init(){
	this->friend_id=0;

}
bool  gf_del_friend_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->friend_id)) return false;
	return true;
}

bool gf_del_friend_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->friend_id)) return false;
	return true;
}
	
gf_del_item_by_num_kf_in::gf_del_item_by_num_kf_in(){
	this->init();
}
void gf_del_item_by_num_kf_in::init(){
	this->role_regtime=0;
	this->item_id=0;
	this->del_num=0;

}
bool  gf_del_item_by_num_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->del_num)) return false;
	return true;
}

bool gf_del_item_by_num_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->del_num)) return false;
	return true;
}
	
gf_del_item_by_num_kf_out::gf_del_item_by_num_kf_out(){
	this->init();
}
void gf_del_item_by_num_kf_out::init(){
	this->item_id=0;
	this->del_num=0;

}
bool  gf_del_item_by_num_kf_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->del_num)) return false;
	return true;
}

bool gf_del_item_by_num_kf_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->del_num)) return false;
	return true;
}
	
gf_del_role_kf_in::gf_del_role_kf_in(){
	this->init();
}
void gf_del_role_kf_in::init(){
	this->role_regtime=0;

}
bool  gf_del_role_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_del_role_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
gf_del_skill_kf_in::gf_del_skill_kf_in(){
	this->init();
}
void gf_del_skill_kf_in::init(){
	this->skill_id=0;
	this->role_regtime=0;

}
bool  gf_del_skill_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->skill_id)) return false;
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_del_skill_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->skill_id)) return false;
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
gf_get_all_task_kf_in::gf_get_all_task_kf_in(){
	this->init();
}
void gf_get_all_task_kf_in::init(){
	this->role_regtime=0;

}
bool  gf_get_all_task_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_get_all_task_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
task_list_t::task_list_t(){
	this->init();
}
void task_list_t::init(){
	this->taskid=0;
	this->task_flag=0;
	this->task_tm=0;

}
bool  task_list_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	if (!ba.read_uint32(this->task_flag)) return false;
	if (!ba.read_uint32(this->task_tm)) return false;
	return true;
}

bool task_list_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	if (!ba.write_uint32(this->task_flag)) return false;
	if (!ba.write_uint32(this->task_tm)) return false;
	return true;
}
	
gf_get_all_task_kf_out::gf_get_all_task_kf_out(){
	this->init();
}
void gf_get_all_task_kf_out::init(){
	this->task_list.clear();

}
bool  gf_get_all_task_kf_out::read_from_buf(byte_array_t & ba ){

	uint32_t task_list_count ;
	if (!ba.read_uint32( task_list_count )) return false;
	if (task_list_count>999999) return false;
	task_list_t  task_list_item;
	this->task_list.clear();
	{for(uint32_t i=0; i<task_list_count;i++){
		if (!task_list_item.read_from_buf(ba)) return false;
		this->task_list.push_back(task_list_item);
	}}
	return true;
}

bool gf_get_all_task_kf_out::write_to_buf(byte_array_t & ba ){
	if (this->task_list.size()>999999 ) return false;
	if (!ba.write_uint32(this->task_list.size())) return false;
	{for(uint32_t i=0; i<this->task_list.size() ;i++){
		if (!this->task_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
gf_get_clothes_list_kf_in::gf_get_clothes_list_kf_in(){
	this->init();
}
void gf_get_clothes_list_kf_in::init(){
	this->role_regtime=0;

}
bool  gf_get_clothes_list_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_get_clothes_list_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
role_clothes_list_item::role_clothes_list_item(){
	this->init();
}
void role_clothes_list_item::init(){
	this->attireid=0;
	this->id=0;
	this->usedflag=0;
	this->duration=0;

}
bool  role_clothes_list_item::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->attireid)) return false;
	if (!ba.read_uint32(this->id)) return false;
	if (!ba.read_uint32(this->usedflag)) return false;
	if (!ba.read_uint32(this->duration)) return false;
	return true;
}

bool role_clothes_list_item::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->attireid)) return false;
	if (!ba.write_uint32(this->id)) return false;
	if (!ba.write_uint32(this->usedflag)) return false;
	if (!ba.write_uint32(this->duration)) return false;
	return true;
}
	
gf_get_clothes_list_kf_out::gf_get_clothes_list_kf_out(){
	this->init();
}
void gf_get_clothes_list_kf_out::init(){
	this->role_clothse_list.clear();

}
bool  gf_get_clothes_list_kf_out::read_from_buf(byte_array_t & ba ){

	uint32_t role_clothse_list_count ;
	if (!ba.read_uint32( role_clothse_list_count )) return false;
	if (role_clothse_list_count>99999) return false;
	role_clothes_list_item  role_clothse_list_item;
	this->role_clothse_list.clear();
	{for(uint32_t i=0; i<role_clothse_list_count;i++){
		if (!role_clothse_list_item.read_from_buf(ba)) return false;
		this->role_clothse_list.push_back(role_clothse_list_item);
	}}
	return true;
}

bool gf_get_clothes_list_kf_out::write_to_buf(byte_array_t & ba ){
	if (this->role_clothse_list.size()>99999 ) return false;
	if (!ba.write_uint32(this->role_clothse_list.size())) return false;
	{for(uint32_t i=0; i<this->role_clothse_list.size() ;i++){
		if (!this->role_clothse_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
gf_get_friend_list_type_kf_in::gf_get_friend_list_type_kf_in(){
	this->init();
}
void gf_get_friend_list_type_kf_in::init(){
	this->friend_type=0;

}
bool  gf_get_friend_list_type_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->friend_type)) return false;
	return true;
}

bool gf_get_friend_list_type_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->friend_type)) return false;
	return true;
}
	
gf_get_friend_list_type_kf_out::gf_get_friend_list_type_kf_out(){
	this->init();
}
void gf_get_friend_list_type_kf_out::init(){
	this->friend_list.clear();

}
bool  gf_get_friend_list_type_kf_out::read_from_buf(byte_array_t & ba ){

	uint32_t friend_list_count ;
	if (!ba.read_uint32( friend_list_count )) return false;
	if (friend_list_count>99999) return false;
	friend_list_item_t  friend_list_item;
	this->friend_list.clear();
	{for(uint32_t i=0; i<friend_list_count;i++){
		if (!friend_list_item.read_from_buf(ba)) return false;
		this->friend_list.push_back(friend_list_item);
	}}
	return true;
}

bool gf_get_friend_list_type_kf_out::write_to_buf(byte_array_t & ba ){
	if (this->friend_list.size()>99999 ) return false;
	if (!ba.write_uint32(this->friend_list.size())) return false;
	{for(uint32_t i=0; i<this->friend_list.size() ;i++){
		if (!this->friend_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
gf_get_info_for_login_kf_in::gf_get_info_for_login_kf_in(){
	this->init();
}
void gf_get_info_for_login_kf_in::init(){
	this->role_regtime=0;

}
bool  gf_get_info_for_login_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_get_info_for_login_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
gf_get_info_for_login_kf_out::gf_get_info_for_login_kf_out(){
	this->init();
}
void gf_get_info_for_login_kf_out::init(){
	this->role_regtime=0;
	this->role_type=0;
	this->flag=0;
	this->regtime=0;
	this->vip=0;
	this->vip_month_cnt=0;
	this->start_time=0;
	this->end_time=0;
	this->x_value=0;
	this->sword_value=0;
	this->vip_exwarehouse=0;
	this->skill_point=0;
	this->map_id=0;
	this->xpos=0;
	this->ypos=0;
	this->level=0;
	this->exp=0;
	this->alloter_exp=0;
	this->hp=0;
	this->mp=0;
	this->xiaomee=0;
	this->honour=0;
	this->fight=0;
	this->win=0;
	this->fail=0;
	this->winning_streak=0;
	this->fumo_points_today=0;
	this->fumo_points_total=0;
	this->fumo_tower_top=0;
	this->fumo_tower_used_tm=0;
	this->Ol_count=0;
	this->Ol_today=0;
	this->Ol_last=0;
	this->Ol_time=0;
	{for(uint32_t i=0; i<20;i++){
		this->uniqueitem[i]=0;
	}}
	{for(uint32_t i=0; i<40;i++){
		this->itembind[i]=0;
	}}
	this->amb_status=0;
	this->parentid=0;
	this->child_cnt=0;
	this->achieve_cnt=0;
	{for(uint32_t i=0; i<40;i++){
		this->flag_bit[i]=0;
	}}
	{for(uint32_t i=0; i<40;i++){
		this->act_record[i]=0;
	}}
	this->double_exp_time=0;
	this->day_flag=0;
	this->max_times_chapter=0;
	this->show_state=0;
	this->strengthen_cnt=0;
	this->achieve_point=0;
	this->last_update_tm=0;
	this->achieve_title=0;
	this->forbiden_add_friend_flag=0;

}
bool  gf_get_info_for_login_kf_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	if (!ba.read_uint32(this->role_type)) return false;
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->regtime)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->vip)) return false;
	if (!ba.read_uint32(this->vip_month_cnt)) return false;
	if (!ba.read_uint32(this->start_time)) return false;
	if (!ba.read_uint32(this->end_time)) return false;
	if (!ba.read_uint32(this->x_value)) return false;
	if (!ba.read_uint32(this->sword_value)) return false;
	if (!ba.read_uint32(this->vip_exwarehouse)) return false;
	if (!ba.read_uint32(this->skill_point)) return false;
	if (!ba.read_uint32(this->map_id)) return false;
	if (!ba.read_uint32(this->xpos)) return false;
	if (!ba.read_uint32(this->ypos)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint32(this->alloter_exp)) return false;
	if (!ba.read_uint32(this->hp)) return false;
	if (!ba.read_uint32(this->mp)) return false;
	if (!ba.read_uint32(this->xiaomee)) return false;
	if (!ba.read_uint32(this->honour)) return false;
	if (!ba.read_uint32(this->fight)) return false;
	if (!ba.read_uint32(this->win)) return false;
	if (!ba.read_uint32(this->fail)) return false;
	if (!ba.read_uint32(this->winning_streak)) return false;
	if (!ba.read_uint32(this->fumo_points_today)) return false;
	if (!ba.read_uint32(this->fumo_points_total)) return false;
	if (!ba.read_uint32(this->fumo_tower_top)) return false;
	if (!ba.read_uint32(this->fumo_tower_used_tm)) return false;
	if (!ba.read_uint32(this->Ol_count)) return false;
	if (!ba.read_uint32(this->Ol_today)) return false;
	if (!ba.read_uint32(this->Ol_last)) return false;
	if (!ba.read_uint32(this->Ol_time)) return false;
	{for(uint32_t i=0; i<20;i++){
		if (!ba.read_uint8(this->uniqueitem[i])) return false;
	}}
	{for(uint32_t i=0; i<40;i++){
		if (!ba.read_uint8(this->itembind[i])) return false;
	}}
	if (!ba.read_uint8(this->amb_status)) return false;
	if (!ba.read_uint32(this->parentid)) return false;
	if (!ba.read_uint32(this->child_cnt)) return false;
	if (!ba.read_uint32(this->achieve_cnt)) return false;
	{for(uint32_t i=0; i<40;i++){
		if (!ba.read_uint8(this->flag_bit[i])) return false;
	}}
	{for(uint32_t i=0; i<40;i++){
		if (!ba.read_uint8(this->act_record[i])) return false;
	}}
	if (!ba.read_uint32(this->double_exp_time)) return false;
	if (!ba.read_uint32(this->day_flag)) return false;
	if (!ba.read_uint32(this->max_times_chapter)) return false;
	if (!ba.read_uint32(this->show_state)) return false;
	if (!ba.read_uint32(this->strengthen_cnt)) return false;
	if (!ba.read_uint32(this->achieve_point)) return false;
	if (!ba.read_uint32(this->last_update_tm)) return false;
	if (!ba.read_uint32(this->achieve_title)) return false;
	if (!ba.read_uint32(this->forbiden_add_friend_flag)) return false;
	return true;
}

bool gf_get_info_for_login_kf_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	if (!ba.write_uint32(this->role_type)) return false;
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->regtime)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->vip)) return false;
	if (!ba.write_uint32(this->vip_month_cnt)) return false;
	if (!ba.write_uint32(this->start_time)) return false;
	if (!ba.write_uint32(this->end_time)) return false;
	if (!ba.write_uint32(this->x_value)) return false;
	if (!ba.write_uint32(this->sword_value)) return false;
	if (!ba.write_uint32(this->vip_exwarehouse)) return false;
	if (!ba.write_uint32(this->skill_point)) return false;
	if (!ba.write_uint32(this->map_id)) return false;
	if (!ba.write_uint32(this->xpos)) return false;
	if (!ba.write_uint32(this->ypos)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint32(this->alloter_exp)) return false;
	if (!ba.write_uint32(this->hp)) return false;
	if (!ba.write_uint32(this->mp)) return false;
	if (!ba.write_uint32(this->xiaomee)) return false;
	if (!ba.write_uint32(this->honour)) return false;
	if (!ba.write_uint32(this->fight)) return false;
	if (!ba.write_uint32(this->win)) return false;
	if (!ba.write_uint32(this->fail)) return false;
	if (!ba.write_uint32(this->winning_streak)) return false;
	if (!ba.write_uint32(this->fumo_points_today)) return false;
	if (!ba.write_uint32(this->fumo_points_total)) return false;
	if (!ba.write_uint32(this->fumo_tower_top)) return false;
	if (!ba.write_uint32(this->fumo_tower_used_tm)) return false;
	if (!ba.write_uint32(this->Ol_count)) return false;
	if (!ba.write_uint32(this->Ol_today)) return false;
	if (!ba.write_uint32(this->Ol_last)) return false;
	if (!ba.write_uint32(this->Ol_time)) return false;
	{for(uint32_t i=0; i<20;i++){
		if (!ba.write_uint8(this->uniqueitem[i])) return false;
	}}
	{for(uint32_t i=0; i<40;i++){
		if (!ba.write_uint8(this->itembind[i])) return false;
	}}
	if (!ba.write_uint8(this->amb_status)) return false;
	if (!ba.write_uint32(this->parentid)) return false;
	if (!ba.write_uint32(this->child_cnt)) return false;
	if (!ba.write_uint32(this->achieve_cnt)) return false;
	{for(uint32_t i=0; i<40;i++){
		if (!ba.write_uint8(this->flag_bit[i])) return false;
	}}
	{for(uint32_t i=0; i<40;i++){
		if (!ba.write_uint8(this->act_record[i])) return false;
	}}
	if (!ba.write_uint32(this->double_exp_time)) return false;
	if (!ba.write_uint32(this->day_flag)) return false;
	if (!ba.write_uint32(this->max_times_chapter)) return false;
	if (!ba.write_uint32(this->show_state)) return false;
	if (!ba.write_uint32(this->strengthen_cnt)) return false;
	if (!ba.write_uint32(this->achieve_point)) return false;
	if (!ba.write_uint32(this->last_update_tm)) return false;
	if (!ba.write_uint32(this->achieve_title)) return false;
	if (!ba.write_uint32(this->forbiden_add_friend_flag)) return false;
	return true;
}
	
gf_get_killed_boss_kf_in::gf_get_killed_boss_kf_in(){
	this->init();
}
void gf_get_killed_boss_kf_in::init(){
	this->role_regtime=0;

}
bool  gf_get_killed_boss_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_get_killed_boss_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
gf_get_killed_boss_kf_out::gf_get_killed_boss_kf_out(){
	this->init();
}
void gf_get_killed_boss_kf_out::init(){
	this->kill_boss_stage_list.clear();

}
bool  gf_get_killed_boss_kf_out::read_from_buf(byte_array_t & ba ){

	uint32_t kill_boss_stage_list_count ;
	if (!ba.read_uint32( kill_boss_stage_list_count )) return false;
	if (kill_boss_stage_list_count>9999) return false;
	finish_stage_list_item  kill_boss_stage_list_item;
	this->kill_boss_stage_list.clear();
	{for(uint32_t i=0; i<kill_boss_stage_list_count;i++){
		if (!kill_boss_stage_list_item.read_from_buf(ba)) return false;
		this->kill_boss_stage_list.push_back(kill_boss_stage_list_item);
	}}
	return true;
}

bool gf_get_killed_boss_kf_out::write_to_buf(byte_array_t & ba ){
	if (this->kill_boss_stage_list.size()>9999 ) return false;
	if (!ba.write_uint32(this->kill_boss_stage_list.size())) return false;
	{for(uint32_t i=0; i<this->kill_boss_stage_list.size() ;i++){
		if (!this->kill_boss_stage_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
gf_get_material_list_kf_in::gf_get_material_list_kf_in(){
	this->init();
}
void gf_get_material_list_kf_in::init(){
	this->role_regtime=0;

}
bool  gf_get_material_list_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_get_material_list_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
item_list_t::item_list_t(){
	this->init();
}
void item_list_t::init(){
	this->item_id=0;
	this->count=0;

}
bool  item_list_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool item_list_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
gf_get_material_list_kf_out::gf_get_material_list_kf_out(){
	this->init();
}
void gf_get_material_list_kf_out::init(){
	this->material_list.clear();

}
bool  gf_get_material_list_kf_out::read_from_buf(byte_array_t & ba ){

	uint32_t material_list_count ;
	if (!ba.read_uint32( material_list_count )) return false;
	if (material_list_count>99999) return false;
	item_list_t  material_list_item;
	this->material_list.clear();
	{for(uint32_t i=0; i<material_list_count;i++){
		if (!material_list_item.read_from_buf(ba)) return false;
		this->material_list.push_back(material_list_item);
	}}
	return true;
}

bool gf_get_material_list_kf_out::write_to_buf(byte_array_t & ba ){
	if (this->material_list.size()>99999 ) return false;
	if (!ba.write_uint32(this->material_list.size())) return false;
	{for(uint32_t i=0; i<this->material_list.size() ;i++){
		if (!this->material_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
role_list_item::role_list_item(){
	this->init();
}
void role_list_item::init(){
	this->role_time=0;
	this->role_type=0;
	this->level=0;
	this->status=0;

}
bool  role_list_item::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_time)) return false;
	if (!ba.read_uint32(this->role_type)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->status)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool role_list_item::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_time)) return false;
	if (!ba.write_uint32(this->role_type)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->status)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}
	
gf_get_role_list_kf_out::gf_get_role_list_kf_out(){
	this->init();
}
void gf_get_role_list_kf_out::init(){
	this->role_list.clear();

}
bool  gf_get_role_list_kf_out::read_from_buf(byte_array_t & ba ){

	uint32_t role_list_count ;
	if (!ba.read_uint32( role_list_count )) return false;
	if (role_list_count>99999) return false;
	role_list_item  role_list_item;
	this->role_list.clear();
	{for(uint32_t i=0; i<role_list_count;i++){
		if (!role_list_item.read_from_buf(ba)) return false;
		this->role_list.push_back(role_list_item);
	}}
	return true;
}

bool gf_get_role_list_kf_out::write_to_buf(byte_array_t & ba ){
	if (this->role_list.size()>99999 ) return false;
	if (!ba.write_uint32(this->role_list.size())) return false;
	{for(uint32_t i=0; i<this->role_list.size() ;i++){
		if (!this->role_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
gf_get_skill_list_kf_in::gf_get_skill_list_kf_in(){
	this->init();
}
void gf_get_skill_list_kf_in::init(){
	this->role_regtime=0;

}
bool  gf_get_skill_list_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_get_skill_list_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
skill_list_t::skill_list_t(){
	this->init();
}
void skill_list_t::init(){
	this->skill_id=0;
	this->skill_lv=0;
	this->skill_point=0;

}
bool  skill_list_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->skill_id)) return false;
	if (!ba.read_uint32(this->skill_lv)) return false;
	if (!ba.read_uint32(this->skill_point)) return false;
	return true;
}

bool skill_list_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->skill_id)) return false;
	if (!ba.write_uint32(this->skill_lv)) return false;
	if (!ba.write_uint32(this->skill_point)) return false;
	return true;
}
	
gf_get_skill_list_kf_out::gf_get_skill_list_kf_out(){
	this->init();
}
void gf_get_skill_list_kf_out::init(){
	this->left_sp=0;
	this->skill_list.clear();

}
bool  gf_get_skill_list_kf_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->left_sp)) return false;

	uint32_t skill_list_count ;
	if (!ba.read_uint32( skill_list_count )) return false;
	if (skill_list_count>999999) return false;
	skill_list_t  skill_list_item;
	this->skill_list.clear();
	{for(uint32_t i=0; i<skill_list_count;i++){
		if (!skill_list_item.read_from_buf(ba)) return false;
		this->skill_list.push_back(skill_list_item);
	}}
	return true;
}

bool gf_get_skill_list_kf_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->left_sp)) return false;
	if (this->skill_list.size()>999999 ) return false;
	if (!ba.write_uint32(this->skill_list.size())) return false;
	{for(uint32_t i=0; i<this->skill_list.size() ;i++){
		if (!this->skill_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
gf_get_summon_list_kf_in::gf_get_summon_list_kf_in(){
	this->init();
}
void gf_get_summon_list_kf_in::init(){
	this->role_regtime=0;

}
bool  gf_get_summon_list_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_get_summon_list_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
summon_list_t::summon_list_t(){
	this->init();
}
void summon_list_t::init(){
	this->mon_tm=0;
	this->mon_type=0;
	this->exp=0;
	this->lv=0;
	this->fight_value=0;
	this->status=0;

}
bool  summon_list_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->mon_tm)) return false;
	if (!ba.read_uint32(this->mon_type)) return false;
	if(!ba.read_buf(this->mon_nick,16)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint32(this->lv)) return false;
	if (!ba.read_uint32(this->fight_value)) return false;
	if (!ba.read_uint32(this->status)) return false;
	return true;
}

bool summon_list_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->mon_tm)) return false;
	if (!ba.write_uint32(this->mon_type)) return false;
	if(!ba.write_buf(this->mon_nick,16)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint32(this->lv)) return false;
	if (!ba.write_uint32(this->fight_value)) return false;
	if (!ba.write_uint32(this->status)) return false;
	return true;
}
	
gf_get_summon_list_kf_out::gf_get_summon_list_kf_out(){
	this->init();
}
void gf_get_summon_list_kf_out::init(){
	this->summon_list.clear();

}
bool  gf_get_summon_list_kf_out::read_from_buf(byte_array_t & ba ){

	uint32_t summon_list_count ;
	if (!ba.read_uint32( summon_list_count )) return false;
	if (summon_list_count>999999) return false;
	summon_list_t  summon_list_item;
	this->summon_list.clear();
	{for(uint32_t i=0; i<summon_list_count;i++){
		if (!summon_list_item.read_from_buf(ba)) return false;
		this->summon_list.push_back(summon_list_item);
	}}
	return true;
}

bool gf_get_summon_list_kf_out::write_to_buf(byte_array_t & ba ){
	if (this->summon_list.size()>999999 ) return false;
	if (!ba.write_uint32(this->summon_list.size())) return false;
	{for(uint32_t i=0; i<this->summon_list.size() ;i++){
		if (!this->summon_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
gf_get_user_base_info_kf_out::gf_get_user_base_info_kf_out(){
	this->init();
}
void gf_get_user_base_info_kf_out::init(){
	this->flag=0;
	this->regtime=0;
	this->forbid_flag=0;
	this->deadline=0;
	this->vip=0;
	this->vip_month_count=0;
	this->start_time=0;
	this->end_time=0;
	this->auto_incr=0;
	this->x_value=0;
	this->Ol_count=0;
	this->Ol_today=0;
	this->Ol_last=0;
	this->Ol_time=0;
	this->amb_status=0;
	this->parentid=0;
	this->child_cnt=0;
	this->achieve_cnt=0;
	this->max_bag=0;

}
bool  gf_get_user_base_info_kf_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->regtime)) return false;
	if (!ba.read_uint32(this->forbid_flag)) return false;
	if (!ba.read_uint32(this->deadline)) return false;
	if (!ba.read_uint32(this->vip)) return false;
	if (!ba.read_uint32(this->vip_month_count)) return false;
	if (!ba.read_uint32(this->start_time)) return false;
	if (!ba.read_uint32(this->end_time)) return false;
	if (!ba.read_uint32(this->auto_incr)) return false;
	if (!ba.read_uint32(this->x_value)) return false;
	if (!ba.read_uint32(this->Ol_count)) return false;
	if (!ba.read_uint32(this->Ol_today)) return false;
	if (!ba.read_uint32(this->Ol_last)) return false;
	if (!ba.read_uint32(this->Ol_time)) return false;
	if (!ba.read_uint32(this->amb_status)) return false;
	if (!ba.read_uint32(this->parentid)) return false;
	if (!ba.read_uint32(this->child_cnt)) return false;
	if (!ba.read_uint32(this->achieve_cnt)) return false;
	if (!ba.read_uint32(this->max_bag)) return false;
	return true;
}

bool gf_get_user_base_info_kf_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->regtime)) return false;
	if (!ba.write_uint32(this->forbid_flag)) return false;
	if (!ba.write_uint32(this->deadline)) return false;
	if (!ba.write_uint32(this->vip)) return false;
	if (!ba.write_uint32(this->vip_month_count)) return false;
	if (!ba.write_uint32(this->start_time)) return false;
	if (!ba.write_uint32(this->end_time)) return false;
	if (!ba.write_uint32(this->auto_incr)) return false;
	if (!ba.write_uint32(this->x_value)) return false;
	if (!ba.write_uint32(this->Ol_count)) return false;
	if (!ba.write_uint32(this->Ol_today)) return false;
	if (!ba.write_uint32(this->Ol_last)) return false;
	if (!ba.write_uint32(this->Ol_time)) return false;
	if (!ba.write_uint32(this->amb_status)) return false;
	if (!ba.write_uint32(this->parentid)) return false;
	if (!ba.write_uint32(this->child_cnt)) return false;
	if (!ba.write_uint32(this->achieve_cnt)) return false;
	if (!ba.write_uint32(this->max_bag)) return false;
	return true;
}
	
gf_get_user_item_list_kf_in::gf_get_user_item_list_kf_in(){
	this->init();
}
void gf_get_user_item_list_kf_in::init(){
	this->role_regtime=0;

}
bool  gf_get_user_item_list_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_get_user_item_list_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
gf_get_user_item_list_kf_out::gf_get_user_item_list_kf_out(){
	this->init();
}
void gf_get_user_item_list_kf_out::init(){
	this->item_list.clear();

}
bool  gf_get_user_item_list_kf_out::read_from_buf(byte_array_t & ba ){

	uint32_t item_list_count ;
	if (!ba.read_uint32( item_list_count )) return false;
	if (item_list_count>99999) return false;
	item_list_t  item_list_item;
	this->item_list.clear();
	{for(uint32_t i=0; i<item_list_count;i++){
		if (!item_list_item.read_from_buf(ba)) return false;
		this->item_list.push_back(item_list_item);
	}}
	return true;
}

bool gf_get_user_item_list_kf_out::write_to_buf(byte_array_t & ba ){
	if (this->item_list.size()>99999 ) return false;
	if (!ba.write_uint32(this->item_list.size())) return false;
	{for(uint32_t i=0; i<this->item_list.size() ;i++){
		if (!this->item_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
gf_get_user_vip_kf_out::gf_get_user_vip_kf_out(){
	this->init();
}
void gf_get_user_vip_kf_out::init(){
	this->vip_is=0;
	this->x_value=0;
	this->vip_lv=0;
	this->vip_month_cnt=0;
	this->start_tm=0;
	this->end_tm=0;
	this->auto_incr=0;
	this->method=0;

}
bool  gf_get_user_vip_kf_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->vip_is)) return false;
	if (!ba.read_uint32(this->x_value)) return false;
	if (!ba.read_uint32(this->vip_lv)) return false;
	if (!ba.read_uint32(this->vip_month_cnt)) return false;
	if (!ba.read_uint32(this->start_tm)) return false;
	if (!ba.read_uint32(this->end_tm)) return false;
	if (!ba.read_uint32(this->auto_incr)) return false;
	if (!ba.read_uint32(this->method)) return false;
	return true;
}

bool gf_get_user_vip_kf_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->vip_is)) return false;
	if (!ba.write_uint32(this->x_value)) return false;
	if (!ba.write_uint32(this->vip_lv)) return false;
	if (!ba.write_uint32(this->vip_month_cnt)) return false;
	if (!ba.write_uint32(this->start_tm)) return false;
	if (!ba.write_uint32(this->end_tm)) return false;
	if (!ba.write_uint32(this->auto_incr)) return false;
	if (!ba.write_uint32(this->method)) return false;
	return true;
}
	
gf_kick_user_offline_kf_in::gf_kick_user_offline_kf_in(){
	this->init();
}
void gf_kick_user_offline_kf_in::init(){
	this->recvid=0;

}
bool  gf_kick_user_offline_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->recvid)) return false;
	return true;
}

bool gf_kick_user_offline_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->recvid)) return false;
	return true;
}
	
gf_official_notice_kf_in::gf_official_notice_kf_in(){
	this->init();
}
void gf_official_notice_kf_in::init(){
	this->index=0;
	this->type=0;
	this->frontid=0;
	{for(uint32_t i=0; i<500;i++){
		this->msg[i]=0;
	}}

}
bool  gf_official_notice_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->index)) return false;
	if (!ba.read_uint32(this->type)) return false;
	if (!ba.read_uint32(this->frontid)) return false;
	{for(uint32_t i=0; i<500;i++){
		if (!ba.read_uint8(this->msg[i])) return false;
	}}
	return true;
}

bool gf_official_notice_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->index)) return false;
	if (!ba.write_uint32(this->type)) return false;
	if (!ba.write_uint32(this->frontid)) return false;
	{for(uint32_t i=0; i<500;i++){
		if (!ba.write_uint8(this->msg[i])) return false;
	}}
	return true;
}
	
gf_post_msg_kf_in::gf_post_msg_kf_in(){
	this->init();
}
void gf_post_msg_kf_in::init(){
	this->recvid=0;
	this->npc_type=0;
	this->msg_tm=0;
	this->msglen=0;
	this->msg.clear();

}
bool  gf_post_msg_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->recvid)) return false;
	if (!ba.read_uint32(this->npc_type)) return false;
	if (!ba.read_uint32(this->msg_tm)) return false;
	if (!ba.read_uint32(this->msglen)) return false;

	uint32_t msg_count ;
	if (!ba.read_uint32( msg_count )) return false;
	if (msg_count>100) return false;
	uint8_t  msg_item;
	this->msg.clear();
	{for(uint32_t i=0; i<msg_count;i++){
		if (!ba.read_uint8(msg_item)) return false;
		this->msg.push_back(msg_item);
	}}
	return true;
}

bool gf_post_msg_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->recvid)) return false;
	if (!ba.write_uint32(this->npc_type)) return false;
	if (!ba.write_uint32(this->msg_tm)) return false;
	if (!ba.write_uint32(this->msglen)) return false;
	if (this->msg.size()>100 ) return false;
	if (!ba.write_uint32(this->msg.size())) return false;
	{for(uint32_t i=0; i<this->msg.size() ;i++){
		if (!ba.write_uint8(this->msg[i])) return false;
	}}
	return true;
}
	
gf_set_account_forbid_kf_in::gf_set_account_forbid_kf_in(){
	this->init();
}
void gf_set_account_forbid_kf_in::init(){
	this->forbid_flag=0;
	this->deadline=0;

}
bool  gf_set_account_forbid_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->forbid_flag)) return false;
	if (!ba.read_uint32(this->deadline)) return false;
	return true;
}

bool gf_set_account_forbid_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->forbid_flag)) return false;
	if (!ba.write_uint32(this->deadline)) return false;
	return true;
}
	
gf_set_item_int_value_kf_in::gf_set_item_int_value_kf_in(){
	this->init();
}
void gf_set_item_int_value_kf_in::init(){
	this->role_regtime=0;
	this->item_id=0;
	this->value=0;

}
bool  gf_set_item_int_value_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	if(!ba.read_buf(this->column_name,32)) return false;
	if (!ba.read_uint32(this->item_id)) return false;
	if (!ba.read_uint32(this->value)) return false;
	return true;
}

bool gf_set_item_int_value_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	if(!ba.write_buf(this->column_name,32)) return false;
	if (!ba.write_uint32(this->item_id)) return false;
	if (!ba.write_uint32(this->value)) return false;
	return true;
}
	
gf_set_nick_kf_in::gf_set_nick_kf_in(){
	this->init();
}
void gf_set_nick_kf_in::init(){
	this->role_regtime=0;
	this->use_item=0;

}
bool  gf_set_nick_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->use_item)) return false;
	return true;
}

bool gf_set_nick_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->use_item)) return false;
	return true;
}
	
gf_set_nick_kf_out::gf_set_nick_kf_out(){
	this->init();
}
void gf_set_nick_kf_out::init(){
	this->use_item=0;

}
bool  gf_set_nick_kf_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->use_item)) return false;
	return true;
}

bool gf_set_nick_kf_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->use_item)) return false;
	return true;
}
	
gf_set_role_delflg_kf_in::gf_set_role_delflg_kf_in(){
	this->init();
}
void gf_set_role_delflg_kf_in::init(){
	this->role_regtime=0;

}
bool  gf_set_role_delflg_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_set_role_delflg_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
gf_set_role_int_value_kf_in::gf_set_role_int_value_kf_in(){
	this->init();
}
void gf_set_role_int_value_kf_in::init(){
	this->role_regtime=0;
	this->value=0;

}
bool  gf_set_role_int_value_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->role_regtime)) return false;
	if(!ba.read_buf(this->column_name,32)) return false;
	if (!ba.read_uint32(this->value)) return false;
	return true;
}

bool gf_set_role_int_value_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->role_regtime)) return false;
	if(!ba.write_buf(this->column_name,32)) return false;
	if (!ba.write_uint32(this->value)) return false;
	return true;
}
	
gf_set_task_flg_kf_in::gf_set_task_flg_kf_in(){
	this->init();
}
void gf_set_task_flg_kf_in::init(){
	this->taskid=0;
	this->task_flag=0;
	this->role_regtime=0;

}
bool  gf_set_task_flg_kf_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	if (!ba.read_uint32(this->task_flag)) return false;
	if (!ba.read_uint32(this->role_regtime)) return false;
	return true;
}

bool gf_set_task_flg_kf_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	if (!ba.write_uint32(this->task_flag)) return false;
	if (!ba.write_uint32(this->role_regtime)) return false;
	return true;
}
	
gf_set_task_flg_kf_out::gf_set_task_flg_kf_out(){
	this->init();
}
void gf_set_task_flg_kf_out::init(){
	this->taskid=0;

}
bool  gf_set_task_flg_kf_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	return true;
}

bool gf_set_task_flg_kf_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	return true;
}
	