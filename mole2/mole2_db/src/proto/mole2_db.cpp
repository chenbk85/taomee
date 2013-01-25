#include "mole2_db.h"

stru_cloth_info::stru_cloth_info(){
	this->init();
}
void stru_cloth_info::init(){
	this->gettime=0;
	this->clothid=0;
	this->gridid=0;
	this->hpmax=0;
	this->mpmax=0;
	this->level=0;
	this->duration=0;
	this->mduration=0;
	this->atk=0;
	this->matk=0;
	this->defense=0;
	this->mdef=0;
	this->speed=0;
	this->spirit=0;
	this->resume=0;
	this->hit=0;
	this->dodge=0;
	this->crit=0;
	this->fightback=0;
	this->rpoison=0;
	this->rlithification=0;
	this->rlethargy=0;
	this->rinebriation=0;
	this->rconfusion=0;
	this->roblivion=0;
	this->quality=0;
	this->validday=0;
	this->crystal_attr=0;
	this->bless_type=0;

}
bool  stru_cloth_info::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gettime)) return false;
	if (!ba.read_uint32(this->clothid)) return false;
	if (!ba.read_uint32(this->gridid)) return false;
	if (!ba.read_uint32(this->hpmax)) return false;
	if (!ba.read_uint32(this->mpmax)) return false;
	if (!ba.read_uint16(this->level)) return false;
	if (!ba.read_uint16(this->duration)) return false;
	if (!ba.read_uint16(this->mduration)) return false;
	if (!ba.read_uint16(this->atk)) return false;
	if (!ba.read_uint16(this->matk)) return false;
	if (!ba.read_uint16(this->defense)) return false;
	if (!ba.read_uint16(this->mdef)) return false;
	if (!ba.read_uint16(this->speed)) return false;
	if (!ba.read_uint16(this->spirit)) return false;
	if (!ba.read_uint16(this->resume)) return false;
	if (!ba.read_uint16(this->hit)) return false;
	if (!ba.read_uint16(this->dodge)) return false;
	if (!ba.read_uint16(this->crit)) return false;
	if (!ba.read_uint16(this->fightback)) return false;
	if (!ba.read_uint16(this->rpoison)) return false;
	if (!ba.read_uint16(this->rlithification)) return false;
	if (!ba.read_uint16(this->rlethargy)) return false;
	if (!ba.read_uint16(this->rinebriation)) return false;
	if (!ba.read_uint16(this->rconfusion)) return false;
	if (!ba.read_uint16(this->roblivion)) return false;
	if (!ba.read_uint32(this->quality)) return false;
	if (!ba.read_uint32(this->validday)) return false;
	if (!ba.read_uint32(this->crystal_attr)) return false;
	if (!ba.read_uint32(this->bless_type)) return false;
	return true;
}

bool stru_cloth_info::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gettime)) return false;
	if (!ba.write_uint32(this->clothid)) return false;
	if (!ba.write_uint32(this->gridid)) return false;
	if (!ba.write_uint32(this->hpmax)) return false;
	if (!ba.write_uint32(this->mpmax)) return false;
	if (!ba.write_uint16(this->level)) return false;
	if (!ba.write_uint16(this->duration)) return false;
	if (!ba.write_uint16(this->mduration)) return false;
	if (!ba.write_uint16(this->atk)) return false;
	if (!ba.write_uint16(this->matk)) return false;
	if (!ba.write_uint16(this->defense)) return false;
	if (!ba.write_uint16(this->mdef)) return false;
	if (!ba.write_uint16(this->speed)) return false;
	if (!ba.write_uint16(this->spirit)) return false;
	if (!ba.write_uint16(this->resume)) return false;
	if (!ba.write_uint16(this->hit)) return false;
	if (!ba.write_uint16(this->dodge)) return false;
	if (!ba.write_uint16(this->crit)) return false;
	if (!ba.write_uint16(this->fightback)) return false;
	if (!ba.write_uint16(this->rpoison)) return false;
	if (!ba.write_uint16(this->rlithification)) return false;
	if (!ba.write_uint16(this->rlethargy)) return false;
	if (!ba.write_uint16(this->rinebriation)) return false;
	if (!ba.write_uint16(this->rconfusion)) return false;
	if (!ba.write_uint16(this->roblivion)) return false;
	if (!ba.write_uint32(this->quality)) return false;
	if (!ba.write_uint32(this->validday)) return false;
	if (!ba.write_uint32(this->crystal_attr)) return false;
	if (!ba.write_uint32(this->bless_type)) return false;
	return true;
}

	
buy_cloth_in_mall_in::buy_cloth_in_mall_in(){
	this->init();
}
void buy_cloth_in_mall_in::init(){
	this->cloths.clear();

}
bool  buy_cloth_in_mall_in::read_from_buf(byte_array_t & ba ){

	uint32_t cloths_count ;
	if (!ba.read_uint32( cloths_count )) return false;
	stru_cloth_info  cloths_item;
	this->cloths.clear();
	{for(uint32_t i=0; i<cloths_count;i++){
		if (!cloths_item.read_from_buf(ba)) return false;
		this->cloths.push_back(cloths_item);
	}}
	return true;
}

bool buy_cloth_in_mall_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->cloths.size())) return false;
	{for(uint32_t i=0; i<this->cloths.size() ;i++){
		if (!this->cloths[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
stru_item_count::stru_item_count(){
	this->init();
}
void stru_item_count::init(){
	this->itemid=0;
	this->count=0;

}
bool  stru_item_count::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool stru_item_count::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
buy_item_in_mall_in::buy_item_in_mall_in(){
	this->init();
}
void buy_item_in_mall_in::init(){
	this->items.clear();

}
bool  buy_item_in_mall_in::read_from_buf(byte_array_t & ba ){

	uint32_t items_count ;
	if (!ba.read_uint32( items_count )) return false;
	stru_item_count  items_item;
	this->items.clear();
	{for(uint32_t i=0; i<items_count;i++){
		if (!items_item.read_from_buf(ba)) return false;
		this->items.push_back(items_item);
	}}
	return true;
}

bool buy_item_in_mall_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->items.size())) return false;
	{for(uint32_t i=0; i<this->items.size() ;i++){
		if (!this->items[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
comm_get_dbser_version_out::comm_get_dbser_version_out(){
	this->init();
}
void comm_get_dbser_version_out::init(){

}
bool  comm_get_dbser_version_out::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->msg,255)) return false;
	return true;
}

bool comm_get_dbser_version_out::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->msg,255)) return false;
	return true;
}

	
day_sub_ssid_count_in::day_sub_ssid_count_in(){
	this->init();
}
void day_sub_ssid_count_in::init(){
	this->ssid=0;
	this->count=0;

}
bool  day_sub_ssid_count_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->ssid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool day_sub_ssid_count_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->ssid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
get_all_pet_type_out::get_all_pet_type_out(){
	this->init();
}
void get_all_pet_type_out::init(){
	this->petlist.clear();

}
bool  get_all_pet_type_out::read_from_buf(byte_array_t & ba ){

	uint32_t petlist_count ;
	if (!ba.read_uint32( petlist_count )) return false;
	uint32_t  petlist_item;
	this->petlist.clear();
	{for(uint32_t i=0; i<petlist_count;i++){
		if (!ba.read_uint32(petlist_item)) return false;
		this->petlist.push_back(petlist_item);
	}}
	return true;
}

bool get_all_pet_type_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petlist.size())) return false;
	{for(uint32_t i=0; i<this->petlist.size() ;i++){
		if (!ba.write_uint32(this->petlist[i])) return false;
	}}
	return true;
}

	
get_rand_info_range_in::get_rand_info_range_in(){
	this->init();
}
void get_rand_info_range_in::init(){
	this->minid=0;
	this->maxid=0;

}
bool  get_rand_info_range_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->minid)) return false;
	if (!ba.read_uint32(this->maxid)) return false;
	return true;
}

bool get_rand_info_range_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->minid)) return false;
	if (!ba.write_uint32(this->maxid)) return false;
	return true;
}

	
stru_rand_info::stru_rand_info(){
	this->init();
}
void stru_rand_info::init(){
	this->time=0;
	this->randid=0;
	this->count=0;

}
bool  stru_rand_info::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->time)) return false;
	if (!ba.read_uint32(this->randid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool stru_rand_info::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->time)) return false;
	if (!ba.write_uint32(this->randid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
get_rand_info_range_out::get_rand_info_range_out(){
	this->init();
}
void get_rand_info_range_out::init(){
	this->item_list.clear();

}
bool  get_rand_info_range_out::read_from_buf(byte_array_t & ba ){

	uint32_t item_list_count ;
	if (!ba.read_uint32( item_list_count )) return false;
	stru_rand_info  item_list_item;
	this->item_list.clear();
	{for(uint32_t i=0; i<item_list_count;i++){
		if (!item_list_item.read_from_buf(ba)) return false;
		this->item_list.push_back(item_list_item);
	}}
	return true;
}

bool get_rand_info_range_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->item_list.size())) return false;
	{for(uint32_t i=0; i<this->item_list.size() ;i++){
		if (!this->item_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
item_handbook_t::item_handbook_t(){
	this->init();
}
void item_handbook_t::init(){
	this->itemid=0;
	this->state=0;
	this->count=0;

}
bool  item_handbook_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->state)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool item_handbook_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->state)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
log_vip_item_in::log_vip_item_in(){
	this->init();
}
void log_vip_item_in::init(){
	this->logtype=0;
	this->itemid=0;
	this->count=0;

}
bool  log_vip_item_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->logtype)) return false;
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_int32(this->count)) return false;
	return true;
}

bool log_vip_item_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->logtype)) return false;
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_int32(this->count)) return false;
	return true;
}

	
stru_cloth_limit::stru_cloth_limit(){
	this->init();
}
void stru_cloth_limit::init(){
	this->clothid=0;
	this->count=0;
	this->limit=0;

}
bool  stru_cloth_limit::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->clothid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	if (!ba.read_uint32(this->limit)) return false;
	return true;
}

bool stru_cloth_limit::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->clothid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	if (!ba.write_uint32(this->limit)) return false;
	return true;
}

	
mall_chk_cloths_limits_in::mall_chk_cloths_limits_in(){
	this->init();
}
void mall_chk_cloths_limits_in::init(){
	this->cloths.clear();

}
bool  mall_chk_cloths_limits_in::read_from_buf(byte_array_t & ba ){

	uint32_t cloths_count ;
	if (!ba.read_uint32( cloths_count )) return false;
	stru_cloth_limit  cloths_item;
	this->cloths.clear();
	{for(uint32_t i=0; i<cloths_count;i++){
		if (!cloths_item.read_from_buf(ba)) return false;
		this->cloths.push_back(cloths_item);
	}}
	return true;
}

bool mall_chk_cloths_limits_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->cloths.size())) return false;
	{for(uint32_t i=0; i<this->cloths.size() ;i++){
		if (!this->cloths[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_add_day_count_in::mole2_add_day_count_in(){
	this->init();
}
void mole2_add_day_count_in::init(){
	this->itemid=0;
	this->count=0;

}
bool  mole2_add_day_count_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool mole2_add_day_count_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
mole2_add_graduation_uid_in::mole2_add_graduation_uid_in(){
	this->init();
}
void mole2_add_graduation_uid_in::init(){
	this->uid=0;

}
bool  mole2_add_graduation_uid_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->uid)) return false;
	return true;
}

bool mole2_add_graduation_uid_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uid)) return false;
	return true;
}

	
mole2_add_medals_in::mole2_add_medals_in(){
	this->init();
}
void mole2_add_medals_in::init(){
	this->medals=0;

}
bool  mole2_add_medals_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->medals)) return false;
	return true;
}

bool mole2_add_medals_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->medals)) return false;
	return true;
}

	
mole2_add_medals_out::mole2_add_medals_out(){
	this->init();
}
void mole2_add_medals_out::init(){
	this->medals=0;

}
bool  mole2_add_medals_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->medals)) return false;
	return true;
}

bool mole2_add_medals_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->medals)) return false;
	return true;
}

	
mole2_add_relation_exp_in::mole2_add_relation_exp_in(){
	this->init();
}
void mole2_add_relation_exp_in::init(){
	this->exp=0;

}
bool  mole2_add_relation_exp_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_int32(this->exp)) return false;
	return true;
}

bool mole2_add_relation_exp_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_int32(this->exp)) return false;
	return true;
}

	
mole2_add_relation_uid_in::mole2_add_relation_uid_in(){
	this->init();
}
void mole2_add_relation_uid_in::init(){
	this->flag=0;
	this->uid=0;

}
bool  mole2_add_relation_uid_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->uid)) return false;
	return true;
}

bool mole2_add_relation_uid_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->uid)) return false;
	return true;
}

	
mole2_add_relation_uid_out::mole2_add_relation_uid_out(){
	this->init();
}
void mole2_add_relation_uid_out::init(){
	this->flag=0;
	this->uid=0;

}
bool  mole2_add_relation_uid_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->uid)) return false;
	return true;
}

bool mole2_add_relation_uid_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->uid)) return false;
	return true;
}

	
mole2_add_relation_val_in::mole2_add_relation_val_in(){
	this->init();
}
void mole2_add_relation_val_in::init(){
	this->uid=0;
	this->addval=0;

}
bool  mole2_add_relation_val_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_int32(this->addval)) return false;
	return true;
}

bool mole2_add_relation_val_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_int32(this->addval)) return false;
	return true;
}

	
mole2_add_relation_val_out::mole2_add_relation_val_out(){
	this->init();
}
void mole2_add_relation_val_out::init(){
	this->uid=0;
	this->value=0;

}
bool  mole2_add_relation_val_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->value)) return false;
	return true;
}

bool mole2_add_relation_val_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->value)) return false;
	return true;
}

	
mole2_add_skill_exp_in::mole2_add_skill_exp_in(){
	this->init();
}
void mole2_add_skill_exp_in::init(){
	this->add_exp=0;

}
bool  mole2_add_skill_exp_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_int32(this->add_exp)) return false;
	return true;
}

bool mole2_add_skill_exp_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_int32(this->add_exp)) return false;
	return true;
}

	
mole2_add_skill_exp_out::mole2_add_skill_exp_out(){
	this->init();
}
void mole2_add_skill_exp_out::init(){
	this->cur_exp=0;

}
bool  mole2_add_skill_exp_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->cur_exp)) return false;
	return true;
}

bool mole2_add_skill_exp_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->cur_exp)) return false;
	return true;
}

	
mole2_add_val_in::mole2_add_val_in(){
	this->init();
}
void mole2_add_val_in::init(){
	this->userid=0;
	this->limit=0;

}
bool  mole2_add_val_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->limit)) return false;
	return true;
}

bool mole2_add_val_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->limit)) return false;
	return true;
}

	
mole2_add_val_out::mole2_add_val_out(){
	this->init();
}
void mole2_add_val_out::init(){
	this->userid=0;
	this->val=0;

}
bool  mole2_add_val_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->val)) return false;
	return true;
}

bool mole2_add_val_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->val)) return false;
	return true;
}

	
mole2_day_set_ssid_count_in::mole2_day_set_ssid_count_in(){
	this->init();
}
void mole2_day_set_ssid_count_in::init(){
	this->ssid=0;
	this->count=0;
	this->totalcnt=0;

}
bool  mole2_day_set_ssid_count_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->ssid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	if (!ba.read_uint32(this->totalcnt)) return false;
	return true;
}

bool mole2_day_set_ssid_count_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->ssid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	if (!ba.write_uint32(this->totalcnt)) return false;
	return true;
}

	
mole2_del_relation_uid_in::mole2_del_relation_uid_in(){
	this->init();
}
void mole2_del_relation_uid_in::init(){
	this->flag=0;
	this->uid=0;
	this->op_uid=0;

}
bool  mole2_del_relation_uid_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->op_uid)) return false;
	return true;
}

bool mole2_del_relation_uid_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->op_uid)) return false;
	return true;
}

	
mole2_del_relation_uid_out::mole2_del_relation_uid_out(){
	this->init();
}
void mole2_del_relation_uid_out::init(){
	this->flag=0;
	this->exp=0;
	this->graduation=0;
	this->total_val=0;
	this->relations.clear();

}
bool  mole2_del_relation_uid_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint32(this->graduation)) return false;
	if (!ba.read_uint32(this->total_val)) return false;

	uint32_t relations_count ;
	if (!ba.read_uint32( relations_count )) return false;
	stru_relation_item  relations_item;
	this->relations.clear();
	{for(uint32_t i=0; i<relations_count;i++){
		if (!relations_item.read_from_buf(ba)) return false;
		this->relations.push_back(relations_item);
	}}
	return true;
}

bool mole2_del_relation_uid_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint32(this->graduation)) return false;
	if (!ba.write_uint32(this->total_val)) return false;
	if (!ba.write_uint32(this->relations.size())) return false;
	{for(uint32_t i=0; i<this->relations.size() ;i++){
		if (!this->relations[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_del_vip_buff_in::mole2_del_vip_buff_in(){
	this->init();
}
void mole2_del_vip_buff_in::init(){
	this->petid.clear();

}
bool  mole2_del_vip_buff_in::read_from_buf(byte_array_t & ba ){

	uint32_t petid_count ;
	if (!ba.read_uint32( petid_count )) return false;
	uint32_t  petid_item;
	this->petid.clear();
	{for(uint32_t i=0; i<petid_count;i++){
		if (!ba.read_uint32(petid_item)) return false;
		this->petid.push_back(petid_item);
	}}
	return true;
}

bool mole2_del_vip_buff_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid.size())) return false;
	{for(uint32_t i=0; i<this->petid.size() ;i++){
		if (!ba.write_uint32(this->petid[i])) return false;
	}}
	return true;
}

	
user2_info_t::user2_info_t(){
	this->init();
}
void user2_info_t::init(){
	this->vip_auto=0;
	this->vip_level=0;
	this->vip_ex_val=0;
	this->vip_base_val=0;
	this->vip_end_time=0;
	this->vip_begin_time=0;
	this->vip_activity=0;
	this->vip_activity_2=0;
	this->vip_activity_3=0;
	this->vip_activity_4=0;

}
bool  user2_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->vip_auto)) return false;
	if (!ba.read_uint32(this->vip_level)) return false;
	if (!ba.read_uint32(this->vip_ex_val)) return false;
	if (!ba.read_uint32(this->vip_base_val)) return false;
	if (!ba.read_uint32(this->vip_end_time)) return false;
	if (!ba.read_uint32(this->vip_begin_time)) return false;
	if (!ba.read_uint32(this->vip_activity)) return false;
	if (!ba.read_uint32(this->vip_activity_2)) return false;
	if (!ba.read_uint32(this->vip_activity_3)) return false;
	if (!ba.read_uint32(this->vip_activity_4)) return false;
	return true;
}

bool user2_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->vip_auto)) return false;
	if (!ba.write_uint32(this->vip_level)) return false;
	if (!ba.write_uint32(this->vip_ex_val)) return false;
	if (!ba.write_uint32(this->vip_base_val)) return false;
	if (!ba.write_uint32(this->vip_end_time)) return false;
	if (!ba.write_uint32(this->vip_begin_time)) return false;
	if (!ba.write_uint32(this->vip_activity)) return false;
	if (!ba.write_uint32(this->vip_activity_2)) return false;
	if (!ba.write_uint32(this->vip_activity_3)) return false;
	if (!ba.write_uint32(this->vip_activity_4)) return false;
	return true;
}

	
user_day_t::user_day_t(){
	this->init();
}
void user_day_t::init(){
	this->ssid=0;
	this->total=0;
	this->count=0;
	this->today=0;

}
bool  user_day_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->ssid)) return false;
	if (!ba.read_uint32(this->total)) return false;
	if (!ba.read_uint32(this->count)) return false;
	if (!ba.read_uint32(this->today)) return false;
	return true;
}

bool user_day_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->ssid)) return false;
	if (!ba.write_uint32(this->total)) return false;
	if (!ba.write_uint32(this->count)) return false;
	if (!ba.write_uint32(this->today)) return false;
	return true;
}

	
stru_beast_book::stru_beast_book(){
	this->init();
}
void stru_beast_book::init(){
	this->beastid=0;
	this->state=0;
	this->count=0;

}
bool  stru_beast_book::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->beastid)) return false;
	if (!ba.read_uint32(this->state)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool stru_beast_book::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->beastid)) return false;
	if (!ba.write_uint32(this->state)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
title_t::title_t(){
	this->init();
}
void title_t::init(){
	this->titleid=0;
	this->gen_time=0;

}
bool  title_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->titleid)) return false;
	if (!ba.read_uint32(this->gen_time)) return false;
	return true;
}

bool title_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->titleid)) return false;
	if (!ba.write_uint32(this->gen_time)) return false;
	return true;
}

	
stru_vip_buff::stru_vip_buff(){
	this->init();
}
void stru_vip_buff::init(){
	this->petid=0;

}
bool  stru_vip_buff::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if(!ba.read_buf(this->buff,128)) return false;
	return true;
}

bool stru_vip_buff::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if(!ba.write_buf(this->buff,128)) return false;
	return true;
}

	
mole2_get_all_info_out::mole2_get_all_info_out(){
	this->init();
}
void mole2_get_all_info_out::init(){
	this->user2_info.init();
	this->day_limit_list.clear();
	this->item_handbook_list.clear();
	this->attire_list.clear();
	this->cli_buff_list.clear();
	this->beast_book_list.clear();
	this->user_title_list.clear();
	this->vip_buf_list.clear();
	this->cloths.clear();

}
bool  mole2_get_all_info_out::read_from_buf(byte_array_t & ba ){
	if (!this->user2_info.read_from_buf(ba)) return false;

	uint32_t day_limit_list_count ;
	if (!ba.read_uint32( day_limit_list_count )) return false;
	user_day_t  day_limit_list_item;
	this->day_limit_list.clear();
	{for(uint32_t i=0; i<day_limit_list_count;i++){
		if (!day_limit_list_item.read_from_buf(ba)) return false;
		this->day_limit_list.push_back(day_limit_list_item);
	}}

	uint32_t item_handbook_list_count ;
	if (!ba.read_uint32( item_handbook_list_count )) return false;
	item_handbook_t  item_handbook_list_item;
	this->item_handbook_list.clear();
	{for(uint32_t i=0; i<item_handbook_list_count;i++){
		if (!item_handbook_list_item.read_from_buf(ba)) return false;
		this->item_handbook_list.push_back(item_handbook_list_item);
	}}

	uint32_t attire_list_count ;
	if (!ba.read_uint32( attire_list_count )) return false;
	stru_cloth_info  attire_list_item;
	this->attire_list.clear();
	{for(uint32_t i=0; i<attire_list_count;i++){
		if (!attire_list_item.read_from_buf(ba)) return false;
		this->attire_list.push_back(attire_list_item);
	}}

	uint32_t cli_buff_list_count ;
	if (!ba.read_uint32( cli_buff_list_count )) return false;
	stru_cli_buff  cli_buff_list_item;
	this->cli_buff_list.clear();
	{for(uint32_t i=0; i<cli_buff_list_count;i++){
		if (!cli_buff_list_item.read_from_buf(ba)) return false;
		this->cli_buff_list.push_back(cli_buff_list_item);
	}}

	uint32_t beast_book_list_count ;
	if (!ba.read_uint32( beast_book_list_count )) return false;
	stru_beast_book  beast_book_list_item;
	this->beast_book_list.clear();
	{for(uint32_t i=0; i<beast_book_list_count;i++){
		if (!beast_book_list_item.read_from_buf(ba)) return false;
		this->beast_book_list.push_back(beast_book_list_item);
	}}

	uint32_t user_title_list_count ;
	if (!ba.read_uint32( user_title_list_count )) return false;
	title_t  user_title_list_item;
	this->user_title_list.clear();
	{for(uint32_t i=0; i<user_title_list_count;i++){
		if (!user_title_list_item.read_from_buf(ba)) return false;
		this->user_title_list.push_back(user_title_list_item);
	}}

	uint32_t vip_buf_list_count ;
	if (!ba.read_uint32( vip_buf_list_count )) return false;
	stru_vip_buff  vip_buf_list_item;
	this->vip_buf_list.clear();
	{for(uint32_t i=0; i<vip_buf_list_count;i++){
		if (!vip_buf_list_item.read_from_buf(ba)) return false;
		this->vip_buf_list.push_back(vip_buf_list_item);
	}}

	uint32_t cloths_count ;
	if (!ba.read_uint32( cloths_count )) return false;
	stru_cloth_info  cloths_item;
	this->cloths.clear();
	{for(uint32_t i=0; i<cloths_count;i++){
		if (!cloths_item.read_from_buf(ba)) return false;
		this->cloths.push_back(cloths_item);
	}}
	return true;
}

bool mole2_get_all_info_out::write_to_buf(byte_array_t & ba ){
	if (!this->user2_info.write_to_buf(ba)) return false;
	if (!ba.write_uint32(this->day_limit_list.size())) return false;
	{for(uint32_t i=0; i<this->day_limit_list.size() ;i++){
		if (!this->day_limit_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->item_handbook_list.size())) return false;
	{for(uint32_t i=0; i<this->item_handbook_list.size() ;i++){
		if (!this->item_handbook_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->attire_list.size())) return false;
	{for(uint32_t i=0; i<this->attire_list.size() ;i++){
		if (!this->attire_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->cli_buff_list.size())) return false;
	{for(uint32_t i=0; i<this->cli_buff_list.size() ;i++){
		if (!this->cli_buff_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->beast_book_list.size())) return false;
	{for(uint32_t i=0; i<this->beast_book_list.size() ;i++){
		if (!this->beast_book_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->user_title_list.size())) return false;
	{for(uint32_t i=0; i<this->user_title_list.size() ;i++){
		if (!this->user_title_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->vip_buf_list.size())) return false;
	{for(uint32_t i=0; i<this->vip_buf_list.size() ;i++){
		if (!this->vip_buf_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->cloths.size())) return false;
	{for(uint32_t i=0; i<this->cloths.size() ;i++){
		if (!this->cloths[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_day_count_in::mole2_get_day_count_in(){
	this->init();
}
void mole2_get_day_count_in::init(){
	this->itemid=0;

}
bool  mole2_get_day_count_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	return true;
}

bool mole2_get_day_count_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	return true;
}

	
mole2_get_day_count_out::mole2_get_day_count_out(){
	this->init();
}
void mole2_get_day_count_out::init(){
	this->total=0;
	this->count=0;

}
bool  mole2_get_day_count_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->total)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool mole2_get_day_count_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->total)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
mole2_get_hero_team_out::mole2_get_hero_team_out(){
	this->init();
}
void mole2_get_hero_team_out::init(){
	this->teamid=0;
	this->medals=0;

}
bool  mole2_get_hero_team_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->teamid)) return false;
	if (!ba.read_uint32(this->medals)) return false;
	return true;
}

bool mole2_get_hero_team_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->teamid)) return false;
	if (!ba.write_uint32(this->medals)) return false;
	return true;
}

	
mole2_get_relation_out::mole2_get_relation_out(){
	this->init();
}
void mole2_get_relation_out::init(){
	this->flag=0;
	this->exp=0;
	this->graduation=0;
	this->total_val=0;
	this->relations.clear();

}
bool  mole2_get_relation_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint32(this->graduation)) return false;
	if (!ba.read_uint32(this->total_val)) return false;

	uint32_t relations_count ;
	if (!ba.read_uint32( relations_count )) return false;
	stru_relation_item  relations_item;
	this->relations.clear();
	{for(uint32_t i=0; i<relations_count;i++){
		if (!relations_item.read_from_buf(ba)) return false;
		this->relations.push_back(relations_item);
	}}
	return true;
}

bool mole2_get_relation_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint32(this->graduation)) return false;
	if (!ba.write_uint32(this->total_val)) return false;
	if (!ba.write_uint32(this->relations.size())) return false;
	{for(uint32_t i=0; i<this->relations.size() ;i++){
		if (!this->relations[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_survey_in::mole2_get_survey_in(){
	this->init();
}
void mole2_get_survey_in::init(){
	this->surveyid=0;

}
bool  mole2_get_survey_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->surveyid)) return false;
	return true;
}

bool mole2_get_survey_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->surveyid)) return false;
	return true;
}

	
stru_survey_reply_new::stru_survey_reply_new(){
	this->init();
}
void stru_survey_reply_new::init(){
	this->option_id=0;
	this->answer_id=0;
	this->total=0;

}
bool  stru_survey_reply_new::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->option_id)) return false;
	if (!ba.read_uint32(this->answer_id)) return false;
	if (!ba.read_uint32(this->total)) return false;
	return true;
}

bool stru_survey_reply_new::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->option_id)) return false;
	if (!ba.write_uint32(this->answer_id)) return false;
	if (!ba.write_uint32(this->total)) return false;
	return true;
}

	
mole2_get_survey_out::mole2_get_survey_out(){
	this->init();
}
void mole2_get_survey_out::init(){
	this->replys.clear();

}
bool  mole2_get_survey_out::read_from_buf(byte_array_t & ba ){

	uint32_t replys_count ;
	if (!ba.read_uint32( replys_count )) return false;
	stru_survey_reply_new  replys_item;
	this->replys.clear();
	{for(uint32_t i=0; i<replys_count;i++){
		if (!replys_item.read_from_buf(ba)) return false;
		this->replys.push_back(replys_item);
	}}
	return true;
}

bool mole2_get_survey_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->replys.size())) return false;
	{for(uint32_t i=0; i<this->replys.size() ;i++){
		if (!this->replys[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_task_rank_out::mole2_get_task_rank_out(){
	this->init();
}
void mole2_get_task_rank_out::init(){
	this->uids.clear();

}
bool  mole2_get_task_rank_out::read_from_buf(byte_array_t & ba ){

	uint32_t uids_count ;
	if (!ba.read_uint32( uids_count )) return false;
	stru_rank_val  uids_item;
	this->uids.clear();
	{for(uint32_t i=0; i<uids_count;i++){
		if (!uids_item.read_from_buf(ba)) return false;
		this->uids.push_back(uids_item);
	}}
	return true;
}

bool mole2_get_task_rank_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uids.size())) return false;
	{for(uint32_t i=0; i<this->uids.size() ;i++){
		if (!this->uids[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_team_rank_out::mole2_get_team_rank_out(){
	this->init();
}
void mole2_get_team_rank_out::init(){
	this->ranks.clear();

}
bool  mole2_get_team_rank_out::read_from_buf(byte_array_t & ba ){

	uint32_t ranks_count ;
	if (!ba.read_uint32( ranks_count )) return false;
	stru_team_rank_t  ranks_item;
	this->ranks.clear();
	{for(uint32_t i=0; i<ranks_count;i++){
		if (!ranks_item.read_from_buf(ba)) return false;
		this->ranks.push_back(ranks_item);
	}}
	return true;
}

bool mole2_get_team_rank_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->ranks.size())) return false;
	{for(uint32_t i=0; i<this->ranks.size() ;i++){
		if (!this->ranks[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_type_pets_in::mole2_get_type_pets_in(){
	this->init();
}
void mole2_get_type_pets_in::init(){
	this->pettype=0;
	this->start=0;
	this->limit=0;

}
bool  mole2_get_type_pets_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->pettype)) return false;
	if (!ba.read_uint32(this->start)) return false;
	if (!ba.read_uint32(this->limit)) return false;
	return true;
}

bool mole2_get_type_pets_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->pettype)) return false;
	if (!ba.write_uint32(this->start)) return false;
	if (!ba.write_uint32(this->limit)) return false;
	return true;
}

	
stru_skill_info::stru_skill_info(){
	this->init();
}
void stru_skill_info::init(){
	this->skillid=0;
	this->level=0;
	this->exp=0;

}
bool  stru_skill_info::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->skillid)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	return true;
}

bool stru_skill_info::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->skillid)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	return true;
}

	
stru_pet_info::stru_pet_info(){
	this->init();
}
void stru_pet_info::init(){
	this->gettime=0;
	this->pettype=0;
	this->race=0;
	this->flag=0;
	this->level=0;
	this->exp=0;
	this->physiqueinit=0;
	this->strengthinit=0;
	this->enduranceinit=0;
	this->quickinit=0;
	this->iqinit=0;
	this->physiqueparam=0;
	this->strengthparam=0;
	this->enduranceparam=0;
	this->quickparam=0;
	this->iqparam=0;
	this->physique_add=0;
	this->strength_add=0;
	this->endurance_add=0;
	this->quick_add=0;
	this->iq_add=0;
	this->attr_addition=0;
	this->hp=0;
	this->mp=0;
	this->earth=0;
	this->water=0;
	this->fire=0;
	this->wind=0;
	this->injury_state=0;
	this->location=0;
	this->skills.clear();

}
bool  stru_pet_info::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gettime)) return false;
	if (!ba.read_uint32(this->pettype)) return false;
	if (!ba.read_uint32(this->race)) return false;
	if (!ba.read_uint32(this->flag)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint16(this->physiqueinit)) return false;
	if (!ba.read_uint16(this->strengthinit)) return false;
	if (!ba.read_uint16(this->enduranceinit)) return false;
	if (!ba.read_uint16(this->quickinit)) return false;
	if (!ba.read_uint16(this->iqinit)) return false;
	if (!ba.read_uint16(this->physiqueparam)) return false;
	if (!ba.read_uint16(this->strengthparam)) return false;
	if (!ba.read_uint16(this->enduranceparam)) return false;
	if (!ba.read_uint16(this->quickparam)) return false;
	if (!ba.read_uint16(this->iqparam)) return false;
	if (!ba.read_uint16(this->physique_add)) return false;
	if (!ba.read_uint16(this->strength_add)) return false;
	if (!ba.read_uint16(this->endurance_add)) return false;
	if (!ba.read_uint16(this->quick_add)) return false;
	if (!ba.read_uint16(this->iq_add)) return false;
	if (!ba.read_uint16(this->attr_addition)) return false;
	if (!ba.read_uint32(this->hp)) return false;
	if (!ba.read_uint32(this->mp)) return false;
	if (!ba.read_uint8(this->earth)) return false;
	if (!ba.read_uint8(this->water)) return false;
	if (!ba.read_uint8(this->fire)) return false;
	if (!ba.read_uint8(this->wind)) return false;
	if (!ba.read_uint32(this->injury_state)) return false;
	if (!ba.read_uint32(this->location)) return false;

	uint32_t skills_count ;
	if (!ba.read_uint32( skills_count )) return false;
	stru_skill_info  skills_item;
	this->skills.clear();
	{for(uint32_t i=0; i<skills_count;i++){
		if (!skills_item.read_from_buf(ba)) return false;
		this->skills.push_back(skills_item);
	}}
	return true;
}

bool stru_pet_info::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gettime)) return false;
	if (!ba.write_uint32(this->pettype)) return false;
	if (!ba.write_uint32(this->race)) return false;
	if (!ba.write_uint32(this->flag)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint16(this->physiqueinit)) return false;
	if (!ba.write_uint16(this->strengthinit)) return false;
	if (!ba.write_uint16(this->enduranceinit)) return false;
	if (!ba.write_uint16(this->quickinit)) return false;
	if (!ba.write_uint16(this->iqinit)) return false;
	if (!ba.write_uint16(this->physiqueparam)) return false;
	if (!ba.write_uint16(this->strengthparam)) return false;
	if (!ba.write_uint16(this->enduranceparam)) return false;
	if (!ba.write_uint16(this->quickparam)) return false;
	if (!ba.write_uint16(this->iqparam)) return false;
	if (!ba.write_uint16(this->physique_add)) return false;
	if (!ba.write_uint16(this->strength_add)) return false;
	if (!ba.write_uint16(this->endurance_add)) return false;
	if (!ba.write_uint16(this->quick_add)) return false;
	if (!ba.write_uint16(this->iq_add)) return false;
	if (!ba.write_uint16(this->attr_addition)) return false;
	if (!ba.write_uint32(this->hp)) return false;
	if (!ba.write_uint32(this->mp)) return false;
	if (!ba.write_uint8(this->earth)) return false;
	if (!ba.write_uint8(this->water)) return false;
	if (!ba.write_uint8(this->fire)) return false;
	if (!ba.write_uint8(this->wind)) return false;
	if (!ba.write_uint32(this->injury_state)) return false;
	if (!ba.write_uint32(this->location)) return false;
	if (!ba.write_uint32(this->skills.size())) return false;
	{for(uint32_t i=0; i<this->skills.size() ;i++){
		if (!this->skills[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_type_pets_out::mole2_get_type_pets_out(){
	this->init();
}
void mole2_get_type_pets_out::init(){
	this->total=0;
	this->start=0;
	this->pets.clear();

}
bool  mole2_get_type_pets_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->total)) return false;
	if (!ba.read_uint32(this->start)) return false;

	uint32_t pets_count ;
	if (!ba.read_uint32( pets_count )) return false;
	stru_pet_info  pets_item;
	this->pets.clear();
	{for(uint32_t i=0; i<pets_count;i++){
		if (!pets_item.read_from_buf(ba)) return false;
		this->pets.push_back(pets_item);
	}}
	return true;
}

bool mole2_get_type_pets_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->total)) return false;
	if (!ba.write_uint32(this->start)) return false;
	if (!ba.write_uint32(this->pets.size())) return false;
	{for(uint32_t i=0; i<this->pets.size() ;i++){
		if (!this->pets[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_user_cli_buff_in::mole2_get_user_cli_buff_in(){
	this->init();
}
void mole2_get_user_cli_buff_in::init(){
	this->min_id=0;
	this->max_id=0;

}
bool  mole2_get_user_cli_buff_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->min_id)) return false;
	if (!ba.read_uint32(this->max_id)) return false;
	return true;
}

bool mole2_get_user_cli_buff_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->min_id)) return false;
	if (!ba.write_uint32(this->max_id)) return false;
	return true;
}

	
mole2_get_user_cli_buff_out::mole2_get_user_cli_buff_out(){
	this->init();
}
void mole2_get_user_cli_buff_out::init(){
	this->item_list.clear();

}
bool  mole2_get_user_cli_buff_out::read_from_buf(byte_array_t & ba ){

	uint32_t item_list_count ;
	if (!ba.read_uint32( item_list_count )) return false;
	stru_cli_buff  item_list_item;
	this->item_list.clear();
	{for(uint32_t i=0; i<item_list_count;i++){
		if (!item_list_item.read_from_buf(ba)) return false;
		this->item_list.push_back(item_list_item);
	}}
	return true;
}

bool mole2_get_user_cli_buff_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->item_list.size())) return false;
	{for(uint32_t i=0; i<this->item_list.size() ;i++){
		if (!this->item_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_user_rank_out::mole2_get_user_rank_out(){
	this->init();
}
void mole2_get_user_rank_out::init(){
	this->ranks.clear();

}
bool  mole2_get_user_rank_out::read_from_buf(byte_array_t & ba ){

	uint32_t ranks_count ;
	if (!ba.read_uint32( ranks_count )) return false;
	stru_user_rank_t  ranks_item;
	this->ranks.clear();
	{for(uint32_t i=0; i<ranks_count;i++){
		if (!ranks_item.read_from_buf(ba)) return false;
		this->ranks.push_back(ranks_item);
	}}
	return true;
}

bool mole2_get_user_rank_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->ranks.size())) return false;
	{for(uint32_t i=0; i<this->ranks.size() ;i++){
		if (!this->ranks[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_user_sql_in::mole2_get_user_sql_in(){
	this->init();
}
void mole2_get_user_sql_in::init(){
	this->obj_userid=0;

}
bool  mole2_get_user_sql_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;
	return true;
}

bool mole2_get_user_sql_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	return true;
}

	
mole2_get_user_sql_out::mole2_get_user_sql_out(){
	this->init();
}
void mole2_get_user_sql_out::init(){
	this->_sql_str_len=0;

}
bool  mole2_get_user_sql_out::read_from_buf(byte_array_t & ba ){

	if (!ba.read_uint32(this->_sql_str_len )) return false;
	if (this->_sql_str_len>8000000) return false;
 	if (!ba.read_buf(this->sql_str,this->_sql_str_len)) return false;
	return true;
}

bool mole2_get_user_sql_out::write_to_buf(byte_array_t & ba ){
	if (this->_sql_str_len>8000000) return false;
	if (!ba.write_uint32(this->_sql_str_len))return false;
	if (!ba.write_buf(this->sql_str,this->_sql_str_len)) return false;
	return true;
}

	
mole2_get_val_by_id_in::mole2_get_val_by_id_in(){
	this->init();
}
void mole2_get_val_by_id_in::init(){
	this->list.clear();

}
bool  mole2_get_val_by_id_in::read_from_buf(byte_array_t & ba ){

	uint32_t list_count ;
	if (!ba.read_uint32( list_count )) return false;
	stru_ex_item  list_item;
	this->list.clear();
	{for(uint32_t i=0; i<list_count;i++){
		if (!list_item.read_from_buf(ba)) return false;
		this->list.push_back(list_item);
	}}
	return true;
}

bool mole2_get_val_by_id_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->list.size())) return false;
	{for(uint32_t i=0; i<this->list.size() ;i++){
		if (!this->list[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_val_by_id_out::mole2_get_val_by_id_out(){
	this->init();
}
void mole2_get_val_by_id_out::init(){
	this->vals.clear();

}
bool  mole2_get_val_by_id_out::read_from_buf(byte_array_t & ba ){

	uint32_t vals_count ;
	if (!ba.read_uint32( vals_count )) return false;
	stru_id_cnt  vals_item;
	this->vals.clear();
	{for(uint32_t i=0; i<vals_count;i++){
		if (!vals_item.read_from_buf(ba)) return false;
		this->vals.push_back(vals_item);
	}}
	return true;
}

bool mole2_get_val_by_id_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->vals.size())) return false;
	{for(uint32_t i=0; i<this->vals.size() ;i++){
		if (!this->vals[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
stru_usr_val::stru_usr_val(){
	this->init();
}
void stru_usr_val::init(){
	this->userid=0;
	this->val=0;

}
bool  stru_usr_val::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->val)) return false;
	return true;
}

bool stru_usr_val::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->val)) return false;
	return true;
}

	
mole2_get_val_out::mole2_get_val_out(){
	this->init();
}
void mole2_get_val_out::init(){
	this->vals.clear();

}
bool  mole2_get_val_out::read_from_buf(byte_array_t & ba ){

	uint32_t vals_count ;
	if (!ba.read_uint32( vals_count )) return false;
	stru_usr_val  vals_item;
	this->vals.clear();
	{for(uint32_t i=0; i<vals_count;i++){
		if (!vals_item.read_from_buf(ba)) return false;
		this->vals.push_back(vals_item);
	}}
	return true;
}

bool mole2_get_val_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->vals.size())) return false;
	{for(uint32_t i=0; i<this->vals.size() ;i++){
		if (!this->vals[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_vip_buff_in::mole2_get_vip_buff_in(){
	this->init();
}
void mole2_get_vip_buff_in::init(){
	this->petid.clear();

}
bool  mole2_get_vip_buff_in::read_from_buf(byte_array_t & ba ){

	uint32_t petid_count ;
	if (!ba.read_uint32( petid_count )) return false;
	uint32_t  petid_item;
	this->petid.clear();
	{for(uint32_t i=0; i<petid_count;i++){
		if (!ba.read_uint32(petid_item)) return false;
		this->petid.push_back(petid_item);
	}}
	return true;
}

bool mole2_get_vip_buff_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid.size())) return false;
	{for(uint32_t i=0; i<this->petid.size() ;i++){
		if (!ba.write_uint32(this->petid[i])) return false;
	}}
	return true;
}

	
mole2_get_vip_buff_out::mole2_get_vip_buff_out(){
	this->init();
}
void mole2_get_vip_buff_out::init(){
	this->petlist.clear();

}
bool  mole2_get_vip_buff_out::read_from_buf(byte_array_t & ba ){

	uint32_t petlist_count ;
	if (!ba.read_uint32( petlist_count )) return false;
	stru_vip_buff  petlist_item;
	this->petlist.clear();
	{for(uint32_t i=0; i<petlist_count;i++){
		if (!petlist_item.read_from_buf(ba)) return false;
		this->petlist.push_back(petlist_item);
	}}
	return true;
}

bool mole2_get_vip_buff_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petlist.size())) return false;
	{for(uint32_t i=0; i<this->petlist.size() ;i++){
		if (!this->petlist[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_get_vip_flag_out::mole2_get_vip_flag_out(){
	this->init();
}
void mole2_get_vip_flag_out::init(){
	this->vip_flag=0;

}
bool  mole2_get_vip_flag_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->vip_flag)) return false;
	return true;
}

bool mole2_get_vip_flag_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->vip_flag)) return false;
	return true;
}

	
mole2_init_feed_count_in::mole2_init_feed_count_in(){
	this->init();
}
void mole2_init_feed_count_in::init(){
	this->type=0;
	this->itemid=0;

}
bool  mole2_init_feed_count_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->type)) return false;
	if (!ba.read_uint32(this->itemid)) return false;
	return true;
}

bool mole2_init_feed_count_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->type)) return false;
	if (!ba.write_uint32(this->itemid)) return false;
	return true;
}

	
mole2_login_out::mole2_login_out(){
	this->init();
}
void mole2_login_out::init(){
	this->flag=0;
	this->color=0;
	this->register_time=0;
	this->race=0;
	this->professtion=0;
	this->joblevel=0;
	this->honor=0;
	this->xiaomee=0;
	this->level=0;
	this->experience=0;
	this->physique=0;
	this->strength=0;
	this->endurance=0;
	this->quick=0;
	this->intelligence=0;
	this->attr_addition=0;
	this->hp=0;
	this->mp=0;
	this->injury_state=0;
	this->in_front=0;
	this->max_attire=0;
	this->max_medicine=0;
	this->max_stuff=0;

}
bool  mole2_login_out::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->nick,16)) return false;
	if(!ba.read_buf(this->signature,32)) return false;
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->color)) return false;
	if (!ba.read_uint32(this->register_time)) return false;
	if (!ba.read_uint8(this->race)) return false;
	if (!ba.read_uint8(this->professtion)) return false;
	if (!ba.read_uint32(this->joblevel)) return false;
	if (!ba.read_uint32(this->honor)) return false;
	if (!ba.read_uint32(this->xiaomee)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->experience)) return false;
	if (!ba.read_uint16(this->physique)) return false;
	if (!ba.read_uint16(this->strength)) return false;
	if (!ba.read_uint16(this->endurance)) return false;
	if (!ba.read_uint16(this->quick)) return false;
	if (!ba.read_uint16(this->intelligence)) return false;
	if (!ba.read_uint16(this->attr_addition)) return false;
	if (!ba.read_uint32(this->hp)) return false;
	if (!ba.read_uint32(this->mp)) return false;
	if (!ba.read_uint32(this->injury_state)) return false;
	if (!ba.read_uint8(this->in_front)) return false;
	if (!ba.read_uint32(this->max_attire)) return false;
	if (!ba.read_uint32(this->max_medicine)) return false;
	if (!ba.read_uint32(this->max_stuff)) return false;
	return true;
}

bool mole2_login_out::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->nick,16)) return false;
	if(!ba.write_buf(this->signature,32)) return false;
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->color)) return false;
	if (!ba.write_uint32(this->register_time)) return false;
	if (!ba.write_uint8(this->race)) return false;
	if (!ba.write_uint8(this->professtion)) return false;
	if (!ba.write_uint32(this->joblevel)) return false;
	if (!ba.write_uint32(this->honor)) return false;
	if (!ba.write_uint32(this->xiaomee)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->experience)) return false;
	if (!ba.write_uint16(this->physique)) return false;
	if (!ba.write_uint16(this->strength)) return false;
	if (!ba.write_uint16(this->endurance)) return false;
	if (!ba.write_uint16(this->quick)) return false;
	if (!ba.write_uint16(this->intelligence)) return false;
	if (!ba.write_uint16(this->attr_addition)) return false;
	if (!ba.write_uint32(this->hp)) return false;
	if (!ba.write_uint32(this->mp)) return false;
	if (!ba.write_uint32(this->injury_state)) return false;
	if (!ba.write_uint8(this->in_front)) return false;
	if (!ba.write_uint32(this->max_attire)) return false;
	if (!ba.write_uint32(this->max_medicine)) return false;
	if (!ba.write_uint32(this->max_stuff)) return false;
	return true;
}

	
stru_vip_args::stru_vip_args(){
	this->init();
}
void stru_vip_args::init(){
	this->type=0;
	this->ex_val=0;
	this->base_val=0;
	this->end_time=0;
	this->begin_time=0;

}
bool  stru_vip_args::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->type)) return false;
	if (!ba.read_uint32(this->ex_val)) return false;
	if (!ba.read_uint32(this->base_val)) return false;
	if (!ba.read_uint32(this->end_time)) return false;
	if (!ba.read_uint32(this->begin_time)) return false;
	return true;
}

bool stru_vip_args::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->type)) return false;
	if (!ba.write_uint32(this->ex_val)) return false;
	if (!ba.write_uint32(this->base_val)) return false;
	if (!ba.write_uint32(this->end_time)) return false;
	if (!ba.write_uint32(this->begin_time)) return false;
	return true;
}

	
stru_battle_info::stru_battle_info(){
	this->init();
}
void stru_battle_info::init(){
	this->all_score=0;
	this->day_score=0;
	this->update_time=0;

}
bool  stru_battle_info::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->all_score)) return false;
	if (!ba.read_uint32(this->day_score)) return false;
	if (!ba.read_uint32(this->update_time)) return false;
	return true;
}

bool stru_battle_info::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->all_score)) return false;
	if (!ba.write_uint32(this->day_score)) return false;
	if (!ba.write_uint32(this->update_time)) return false;
	return true;
}

	
stru_item_info::stru_item_info(){
	this->init();
}
void stru_item_info::init(){
	this->itemid=0;
	this->count=0;
	this->storage_cnt=0;

}
bool  stru_item_info::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	if (!ba.read_uint32(this->storage_cnt)) return false;
	return true;
}

bool stru_item_info::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	if (!ba.write_uint32(this->storage_cnt)) return false;
	return true;
}

	
stru_task_done::stru_task_done(){
	this->init();
}
void stru_task_done::init(){
	this->taskid=0;
	this->node=0;
	this->state=0;
	this->optdate=0;
	this->fin_time=0;
	this->fin_num=0;

}
bool  stru_task_done::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	if (!ba.read_uint32(this->node)) return false;
	if (!ba.read_uint32(this->state)) return false;
	if (!ba.read_uint32(this->optdate)) return false;
	if (!ba.read_uint32(this->fin_time)) return false;
	if (!ba.read_uint32(this->fin_num)) return false;
	return true;
}

bool stru_task_done::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	if (!ba.write_uint32(this->node)) return false;
	if (!ba.write_uint32(this->state)) return false;
	if (!ba.write_uint32(this->optdate)) return false;
	if (!ba.write_uint32(this->fin_time)) return false;
	if (!ba.write_uint32(this->fin_num)) return false;
	return true;
}

	
stru_task_doing::stru_task_doing(){
	this->init();
}
void stru_task_doing::init(){
	this->taskid=0;
	this->node=0;
	this->state=0;
	this->optdate=0;
	this->fin_time=0;
	this->fin_num=0;

}
bool  stru_task_doing::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	if (!ba.read_uint32(this->node)) return false;
	if (!ba.read_uint32(this->state)) return false;
	if (!ba.read_uint32(this->optdate)) return false;
	if (!ba.read_uint32(this->fin_time)) return false;
	if (!ba.read_uint32(this->fin_num)) return false;
	if(!ba.read_buf(this->cli_buf,128)) return false;
	if(!ba.read_buf(this->ser_buf,20)) return false;
	return true;
}

bool stru_task_doing::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	if (!ba.write_uint32(this->node)) return false;
	if (!ba.write_uint32(this->state)) return false;
	if (!ba.write_uint32(this->optdate)) return false;
	if (!ba.write_uint32(this->fin_time)) return false;
	if (!ba.write_uint32(this->fin_num)) return false;
	if(!ba.write_buf(this->cli_buf,128)) return false;
	if(!ba.write_buf(this->ser_buf,20)) return false;
	return true;
}

	
stru_map_state::stru_map_state(){
	this->init();
}
void stru_map_state::init(){
	this->mapid=0;
	this->state=0;

}
bool  stru_map_state::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->mapid)) return false;
	if (!ba.read_uint32(this->state)) return false;
	return true;
}

bool stru_map_state::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->mapid)) return false;
	if (!ba.write_uint32(this->state)) return false;
	return true;
}

	
stru_day_limit::stru_day_limit(){
	this->init();
}
void stru_day_limit::init(){
	this->ssid=0;
	this->total=0;
	this->count=0;

}
bool  stru_day_limit::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->ssid)) return false;
	if (!ba.read_uint32(this->total)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool stru_day_limit::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->ssid)) return false;
	if (!ba.write_uint32(this->total)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
mole2_online_login_out::mole2_online_login_out(){
	this->init();
}
void mole2_online_login_out::init(){
	this->flag=0;
	this->color=0;
	this->register_time=0;
	this->race=0;
	this->professtion=0;
	this->joblevel=0;
	this->honor=0;
	this->xiaomee=0;
	this->level=0;
	this->experience=0;
	this->physique=0;
	this->strength=0;
	this->endurance=0;
	this->quick=0;
	this->intelligence=0;
	this->attr_addition=0;
	this->hp=0;
	this->mp=0;
	this->injury_state=0;
	this->in_front=0;
	this->max_attire=0;
	this->max_medicine=0;
	this->max_stuff=0;
	this->time=0;
	this->axis_x=0;
	this->axis_y=0;
	this->mapid=0;
	this->daytime=0;
	this->fly_mapid=0;
	this->expbox=0;
	this->energy=0;
	this->skill_expbox=0;
	this->flag_ex=0;
	this->winbossid=0;
	this->parent=0;
	this->vipargs.init();
	this->battle.init();
	this->all_pet_cnt=0;
	this->skills.clear();
	this->pets.clear();
	this->cloths.clear();
	this->cloths_in_bag.clear();
	this->items.clear();
	this->titles.clear();
	this->task_done.clear();
	this->task_doing.clear();
	this->mapstates.clear();
	this->beastbook.clear();
	this->daylimits.clear();
	this->vipbuffs.clear();
	this->_msglist_len=0;

}
bool  mole2_online_login_out::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->color)) return false;
	if (!ba.read_uint32(this->register_time)) return false;
	if (!ba.read_uint32(this->race)) return false;
	if (!ba.read_uint32(this->professtion)) return false;
	if (!ba.read_uint32(this->joblevel)) return false;
	if (!ba.read_uint32(this->honor)) return false;
	if (!ba.read_uint32(this->xiaomee)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->experience)) return false;
	if (!ba.read_uint16(this->physique)) return false;
	if (!ba.read_uint16(this->strength)) return false;
	if (!ba.read_uint16(this->endurance)) return false;
	if (!ba.read_uint16(this->quick)) return false;
	if (!ba.read_uint16(this->intelligence)) return false;
	if (!ba.read_uint16(this->attr_addition)) return false;
	if (!ba.read_uint32(this->hp)) return false;
	if (!ba.read_uint32(this->mp)) return false;
	if (!ba.read_uint32(this->injury_state)) return false;
	if (!ba.read_uint32(this->in_front)) return false;
	if (!ba.read_uint32(this->max_attire)) return false;
	if (!ba.read_uint32(this->max_medicine)) return false;
	if (!ba.read_uint32(this->max_stuff)) return false;
	if (!ba.read_uint32(this->time)) return false;
	if (!ba.read_uint32(this->axis_x)) return false;
	if (!ba.read_uint32(this->axis_y)) return false;
	if (!ba.read_uint32(this->mapid)) return false;
	if (!ba.read_uint32(this->daytime)) return false;
	if (!ba.read_uint32(this->fly_mapid)) return false;
	if (!ba.read_uint32(this->expbox)) return false;
	if (!ba.read_uint32(this->energy)) return false;
	if (!ba.read_uint32(this->skill_expbox)) return false;
	if (!ba.read_uint32(this->flag_ex)) return false;
	if (!ba.read_uint32(this->winbossid)) return false;
	if (!ba.read_uint32(this->parent)) return false;
	if (!this->vipargs.read_from_buf(ba)) return false;
	if (!this->battle.read_from_buf(ba)) return false;
	if (!ba.read_uint32(this->all_pet_cnt)) return false;

	uint32_t skills_count ;
	if (!ba.read_uint32( skills_count )) return false;
	stru_skill_info  skills_item;
	this->skills.clear();
	{for(uint32_t i=0; i<skills_count;i++){
		if (!skills_item.read_from_buf(ba)) return false;
		this->skills.push_back(skills_item);
	}}

	uint32_t pets_count ;
	if (!ba.read_uint32( pets_count )) return false;
	stru_pet_info  pets_item;
	this->pets.clear();
	{for(uint32_t i=0; i<pets_count;i++){
		if (!pets_item.read_from_buf(ba)) return false;
		this->pets.push_back(pets_item);
	}}

	uint32_t cloths_count ;
	if (!ba.read_uint32( cloths_count )) return false;
	stru_cloth_info  cloths_item;
	this->cloths.clear();
	{for(uint32_t i=0; i<cloths_count;i++){
		if (!cloths_item.read_from_buf(ba)) return false;
		this->cloths.push_back(cloths_item);
	}}

	uint32_t cloths_in_bag_count ;
	if (!ba.read_uint32( cloths_in_bag_count )) return false;
	stru_cloth_info  cloths_in_bag_item;
	this->cloths_in_bag.clear();
	{for(uint32_t i=0; i<cloths_in_bag_count;i++){
		if (!cloths_in_bag_item.read_from_buf(ba)) return false;
		this->cloths_in_bag.push_back(cloths_in_bag_item);
	}}

	uint32_t items_count ;
	if (!ba.read_uint32( items_count )) return false;
	stru_item_info  items_item;
	this->items.clear();
	{for(uint32_t i=0; i<items_count;i++){
		if (!items_item.read_from_buf(ba)) return false;
		this->items.push_back(items_item);
	}}

	uint32_t titles_count ;
	if (!ba.read_uint32( titles_count )) return false;
	uint32_t  titles_item;
	this->titles.clear();
	{for(uint32_t i=0; i<titles_count;i++){
		if (!ba.read_uint32(titles_item)) return false;
		this->titles.push_back(titles_item);
	}}

	uint32_t task_done_count ;
	if (!ba.read_uint32( task_done_count )) return false;
	stru_task_done  task_done_item;
	this->task_done.clear();
	{for(uint32_t i=0; i<task_done_count;i++){
		if (!task_done_item.read_from_buf(ba)) return false;
		this->task_done.push_back(task_done_item);
	}}

	uint32_t task_doing_count ;
	if (!ba.read_uint32( task_doing_count )) return false;
	stru_task_doing  task_doing_item;
	this->task_doing.clear();
	{for(uint32_t i=0; i<task_doing_count;i++){
		if (!task_doing_item.read_from_buf(ba)) return false;
		this->task_doing.push_back(task_doing_item);
	}}

	uint32_t mapstates_count ;
	if (!ba.read_uint32( mapstates_count )) return false;
	stru_map_state  mapstates_item;
	this->mapstates.clear();
	{for(uint32_t i=0; i<mapstates_count;i++){
		if (!mapstates_item.read_from_buf(ba)) return false;
		this->mapstates.push_back(mapstates_item);
	}}

	uint32_t beastbook_count ;
	if (!ba.read_uint32( beastbook_count )) return false;
	stru_beast_book  beastbook_item;
	this->beastbook.clear();
	{for(uint32_t i=0; i<beastbook_count;i++){
		if (!beastbook_item.read_from_buf(ba)) return false;
		this->beastbook.push_back(beastbook_item);
	}}

	uint32_t daylimits_count ;
	if (!ba.read_uint32( daylimits_count )) return false;
	stru_day_limit  daylimits_item;
	this->daylimits.clear();
	{for(uint32_t i=0; i<daylimits_count;i++){
		if (!daylimits_item.read_from_buf(ba)) return false;
		this->daylimits.push_back(daylimits_item);
	}}

	uint32_t vipbuffs_count ;
	if (!ba.read_uint32( vipbuffs_count )) return false;
	stru_vip_buff  vipbuffs_item;
	this->vipbuffs.clear();
	{for(uint32_t i=0; i<vipbuffs_count;i++){
		if (!vipbuffs_item.read_from_buf(ba)) return false;
		this->vipbuffs.push_back(vipbuffs_item);
	}}

	if (!ba.read_uint32(this->_msglist_len )) return false;
	if (this->_msglist_len>2000) return false;
 	if (!ba.read_buf(this->msglist,this->_msglist_len)) return false;
	return true;
}

bool mole2_online_login_out::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->color)) return false;
	if (!ba.write_uint32(this->register_time)) return false;
	if (!ba.write_uint32(this->race)) return false;
	if (!ba.write_uint32(this->professtion)) return false;
	if (!ba.write_uint32(this->joblevel)) return false;
	if (!ba.write_uint32(this->honor)) return false;
	if (!ba.write_uint32(this->xiaomee)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->experience)) return false;
	if (!ba.write_uint16(this->physique)) return false;
	if (!ba.write_uint16(this->strength)) return false;
	if (!ba.write_uint16(this->endurance)) return false;
	if (!ba.write_uint16(this->quick)) return false;
	if (!ba.write_uint16(this->intelligence)) return false;
	if (!ba.write_uint16(this->attr_addition)) return false;
	if (!ba.write_uint32(this->hp)) return false;
	if (!ba.write_uint32(this->mp)) return false;
	if (!ba.write_uint32(this->injury_state)) return false;
	if (!ba.write_uint32(this->in_front)) return false;
	if (!ba.write_uint32(this->max_attire)) return false;
	if (!ba.write_uint32(this->max_medicine)) return false;
	if (!ba.write_uint32(this->max_stuff)) return false;
	if (!ba.write_uint32(this->time)) return false;
	if (!ba.write_uint32(this->axis_x)) return false;
	if (!ba.write_uint32(this->axis_y)) return false;
	if (!ba.write_uint32(this->mapid)) return false;
	if (!ba.write_uint32(this->daytime)) return false;
	if (!ba.write_uint32(this->fly_mapid)) return false;
	if (!ba.write_uint32(this->expbox)) return false;
	if (!ba.write_uint32(this->energy)) return false;
	if (!ba.write_uint32(this->skill_expbox)) return false;
	if (!ba.write_uint32(this->flag_ex)) return false;
	if (!ba.write_uint32(this->winbossid)) return false;
	if (!ba.write_uint32(this->parent)) return false;
	if (!this->vipargs.write_to_buf(ba)) return false;
	if (!this->battle.write_to_buf(ba)) return false;
	if (!ba.write_uint32(this->all_pet_cnt)) return false;
	if (!ba.write_uint32(this->skills.size())) return false;
	{for(uint32_t i=0; i<this->skills.size() ;i++){
		if (!this->skills[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->pets.size())) return false;
	{for(uint32_t i=0; i<this->pets.size() ;i++){
		if (!this->pets[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->cloths.size())) return false;
	{for(uint32_t i=0; i<this->cloths.size() ;i++){
		if (!this->cloths[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->cloths_in_bag.size())) return false;
	{for(uint32_t i=0; i<this->cloths_in_bag.size() ;i++){
		if (!this->cloths_in_bag[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->items.size())) return false;
	{for(uint32_t i=0; i<this->items.size() ;i++){
		if (!this->items[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->titles.size())) return false;
	{for(uint32_t i=0; i<this->titles.size() ;i++){
		if (!ba.write_uint32(this->titles[i])) return false;
	}}
	if (!ba.write_uint32(this->task_done.size())) return false;
	{for(uint32_t i=0; i<this->task_done.size() ;i++){
		if (!this->task_done[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->task_doing.size())) return false;
	{for(uint32_t i=0; i<this->task_doing.size() ;i++){
		if (!this->task_doing[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->mapstates.size())) return false;
	{for(uint32_t i=0; i<this->mapstates.size() ;i++){
		if (!this->mapstates[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->beastbook.size())) return false;
	{for(uint32_t i=0; i<this->beastbook.size() ;i++){
		if (!this->beastbook[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->daylimits.size())) return false;
	{for(uint32_t i=0; i<this->daylimits.size() ;i++){
		if (!this->daylimits[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->vipbuffs.size())) return false;
	{for(uint32_t i=0; i<this->vipbuffs.size() ;i++){
		if (!this->vipbuffs[i].write_to_buf(ba)) return false;
	}}
	if (this->_msglist_len>2000) return false;
	if (!ba.write_uint32(this->_msglist_len))return false;
	if (!ba.write_buf(this->msglist,this->_msglist_len)) return false;
	return true;
}

	
mole2_petfight_get_rank_out::mole2_petfight_get_rank_out(){
	this->init();
}
void mole2_petfight_get_rank_out::init(){
	this->ranks.clear();

}
bool  mole2_petfight_get_rank_out::read_from_buf(byte_array_t & ba ){

	uint32_t ranks_count ;
	if (!ba.read_uint32( ranks_count )) return false;
	stru_petfight_rank_t  ranks_item;
	this->ranks.clear();
	{for(uint32_t i=0; i<ranks_count;i++){
		if (!ranks_item.read_from_buf(ba)) return false;
		this->ranks.push_back(ranks_item);
	}}
	return true;
}

bool mole2_petfight_get_rank_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->ranks.size())) return false;
	{for(uint32_t i=0; i<this->ranks.size() ;i++){
		if (!this->ranks[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_petfight_get_user_rank_out::mole2_petfight_get_user_rank_out(){
	this->init();
}
void mole2_petfight_get_user_rank_out::init(){
	this->rank=0;
	this->last_rank=0;

}
bool  mole2_petfight_get_user_rank_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->rank)) return false;
	if (!ba.read_uint32(this->last_rank)) return false;
	return true;
}

bool mole2_petfight_get_user_rank_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->rank)) return false;
	if (!ba.write_uint32(this->last_rank)) return false;
	return true;
}

	
mole2_petfight_set_scores_in::mole2_petfight_set_scores_in(){
	this->init();
}
void mole2_petfight_set_scores_in::init(){
	this->scores=0;

}
bool  mole2_petfight_set_scores_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->scores)) return false;
	return true;
}

bool mole2_petfight_set_scores_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->scores)) return false;
	return true;
}

	
stru_survey_reply::stru_survey_reply(){
	this->init();
}
void stru_survey_reply::init(){
	this->option_id=0;
	this->value_id=0;

}
bool  stru_survey_reply::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->option_id)) return false;
	if (!ba.read_uint32(this->value_id)) return false;
	return true;
}

bool stru_survey_reply::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->option_id)) return false;
	if (!ba.write_uint32(this->value_id)) return false;
	return true;
}

	
mole2_reply_question_in::mole2_reply_question_in(){
	this->init();
}
void mole2_reply_question_in::init(){
	this->surveyid=0;
	this->replys.clear();

}
bool  mole2_reply_question_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->surveyid)) return false;

	uint32_t replys_count ;
	if (!ba.read_uint32( replys_count )) return false;
	stru_survey_reply  replys_item;
	this->replys.clear();
	{for(uint32_t i=0; i<replys_count;i++){
		if (!replys_item.read_from_buf(ba)) return false;
		this->replys.push_back(replys_item);
	}}
	return true;
}

bool mole2_reply_question_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->surveyid)) return false;
	if (!ba.write_uint32(this->replys.size())) return false;
	{for(uint32_t i=0; i<this->replys.size() ;i++){
		if (!this->replys[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_set_flag_bits_in::mole2_set_flag_bits_in(){
	this->init();
}
void mole2_set_flag_bits_in::init(){
	this->pos_id=0;
	this->is_set_true=0;

}
bool  mole2_set_flag_bits_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->pos_id)) return false;
	if (!ba.read_uint32(this->is_set_true)) return false;
	return true;
}

bool mole2_set_flag_bits_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->pos_id)) return false;
	if (!ba.write_uint32(this->is_set_true)) return false;
	return true;
}

	
mole2_set_flag_bits_out::mole2_set_flag_bits_out(){
	this->init();
}
void mole2_set_flag_bits_out::init(){
	this->flag=0;
	this->pos_id=0;

}
bool  mole2_set_flag_bits_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->pos_id)) return false;
	return true;
}

bool mole2_set_flag_bits_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->pos_id)) return false;
	return true;
}

	
mole2_set_hero_team_in::mole2_set_hero_team_in(){
	this->init();
}
void mole2_set_hero_team_in::init(){
	this->teamid=0;

}
bool  mole2_set_hero_team_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->teamid)) return false;
	return true;
}

bool mole2_set_hero_team_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->teamid)) return false;
	return true;
}

	
mole2_set_levelup_time_in::mole2_set_levelup_time_in(){
	this->init();
}
void mole2_set_levelup_time_in::init(){
	this->time=0;

}
bool  mole2_set_levelup_time_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->time)) return false;
	return true;
}

bool mole2_set_levelup_time_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->time)) return false;
	return true;
}

	
mole2_set_pet_attributes_in::mole2_set_pet_attributes_in(){
	this->init();
}
void mole2_set_pet_attributes_in::init(){
	this->petid=0;
	this->level=0;
	this->exp=0;
	this->physiqueinit=0;
	this->strengthinit=0;
	this->enduranceinit=0;
	this->quickinit=0;
	this->iqinit=0;
	this->physiqueparam=0;
	this->strengthparam=0;
	this->enduranceparam=0;
	this->quickparam=0;
	this->iqparam=0;
	this->physiqueadd=0;
	this->strengthadd=0;
	this->enduranceadd=0;
	this->quickadd=0;
	this->iqadd=0;
	this->attradd=0;
	this->hp=0;
	this->mp=0;

}
bool  mole2_set_pet_attributes_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint16(this->physiqueinit)) return false;
	if (!ba.read_uint16(this->strengthinit)) return false;
	if (!ba.read_uint16(this->enduranceinit)) return false;
	if (!ba.read_uint16(this->quickinit)) return false;
	if (!ba.read_uint16(this->iqinit)) return false;
	if (!ba.read_uint16(this->physiqueparam)) return false;
	if (!ba.read_uint16(this->strengthparam)) return false;
	if (!ba.read_uint16(this->enduranceparam)) return false;
	if (!ba.read_uint16(this->quickparam)) return false;
	if (!ba.read_uint16(this->iqparam)) return false;
	if (!ba.read_uint16(this->physiqueadd)) return false;
	if (!ba.read_uint16(this->strengthadd)) return false;
	if (!ba.read_uint16(this->enduranceadd)) return false;
	if (!ba.read_uint16(this->quickadd)) return false;
	if (!ba.read_uint16(this->iqadd)) return false;
	if (!ba.read_uint16(this->attradd)) return false;
	if (!ba.read_uint32(this->hp)) return false;
	if (!ba.read_uint32(this->mp)) return false;
	return true;
}

bool mole2_set_pet_attributes_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint16(this->physiqueinit)) return false;
	if (!ba.write_uint16(this->strengthinit)) return false;
	if (!ba.write_uint16(this->enduranceinit)) return false;
	if (!ba.write_uint16(this->quickinit)) return false;
	if (!ba.write_uint16(this->iqinit)) return false;
	if (!ba.write_uint16(this->physiqueparam)) return false;
	if (!ba.write_uint16(this->strengthparam)) return false;
	if (!ba.write_uint16(this->enduranceparam)) return false;
	if (!ba.write_uint16(this->quickparam)) return false;
	if (!ba.write_uint16(this->iqparam)) return false;
	if (!ba.write_uint16(this->physiqueadd)) return false;
	if (!ba.write_uint16(this->strengthadd)) return false;
	if (!ba.write_uint16(this->enduranceadd)) return false;
	if (!ba.write_uint16(this->quickadd)) return false;
	if (!ba.write_uint16(this->iqadd)) return false;
	if (!ba.write_uint16(this->attradd)) return false;
	if (!ba.write_uint32(this->hp)) return false;
	if (!ba.write_uint32(this->mp)) return false;
	return true;
}

	
mole2_set_pet_attributes_out::mole2_set_pet_attributes_out(){
	this->init();
}
void mole2_set_pet_attributes_out::init(){
	this->petid=0;
	this->level=0;
	this->exp=0;
	this->physiqueinit=0;
	this->strengthinit=0;
	this->enduranceinit=0;
	this->quickinit=0;
	this->iqinit=0;
	this->physiqueparam=0;
	this->strengthparam=0;
	this->enduranceparam=0;
	this->quickparam=0;
	this->iqparam=0;
	this->physiqueadd=0;
	this->strengthadd=0;
	this->enduranceadd=0;
	this->quickadd=0;
	this->iqadd=0;
	this->attradd=0;
	this->hp=0;
	this->mp=0;

}
bool  mole2_set_pet_attributes_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if (!ba.read_uint16(this->physiqueinit)) return false;
	if (!ba.read_uint16(this->strengthinit)) return false;
	if (!ba.read_uint16(this->enduranceinit)) return false;
	if (!ba.read_uint16(this->quickinit)) return false;
	if (!ba.read_uint16(this->iqinit)) return false;
	if (!ba.read_uint16(this->physiqueparam)) return false;
	if (!ba.read_uint16(this->strengthparam)) return false;
	if (!ba.read_uint16(this->enduranceparam)) return false;
	if (!ba.read_uint16(this->quickparam)) return false;
	if (!ba.read_uint16(this->iqparam)) return false;
	if (!ba.read_uint16(this->physiqueadd)) return false;
	if (!ba.read_uint16(this->strengthadd)) return false;
	if (!ba.read_uint16(this->enduranceadd)) return false;
	if (!ba.read_uint16(this->quickadd)) return false;
	if (!ba.read_uint16(this->iqadd)) return false;
	if (!ba.read_uint16(this->attradd)) return false;
	if (!ba.read_uint32(this->hp)) return false;
	if (!ba.read_uint32(this->mp)) return false;
	return true;
}

bool mole2_set_pet_attributes_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if (!ba.write_uint16(this->physiqueinit)) return false;
	if (!ba.write_uint16(this->strengthinit)) return false;
	if (!ba.write_uint16(this->enduranceinit)) return false;
	if (!ba.write_uint16(this->quickinit)) return false;
	if (!ba.write_uint16(this->iqinit)) return false;
	if (!ba.write_uint16(this->physiqueparam)) return false;
	if (!ba.write_uint16(this->strengthparam)) return false;
	if (!ba.write_uint16(this->enduranceparam)) return false;
	if (!ba.write_uint16(this->quickparam)) return false;
	if (!ba.write_uint16(this->iqparam)) return false;
	if (!ba.write_uint16(this->physiqueadd)) return false;
	if (!ba.write_uint16(this->strengthadd)) return false;
	if (!ba.write_uint16(this->enduranceadd)) return false;
	if (!ba.write_uint16(this->quickadd)) return false;
	if (!ba.write_uint16(this->iqadd)) return false;
	if (!ba.write_uint16(this->attradd)) return false;
	if (!ba.write_uint32(this->hp)) return false;
	if (!ba.write_uint32(this->mp)) return false;
	return true;
}

	
mole2_set_pet_flag_in::mole2_set_pet_flag_in(){
	this->init();
}
void mole2_set_pet_flag_in::init(){
	this->petid=0;
	this->bit=0;
	this->state=0;

}
bool  mole2_set_pet_flag_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->bit)) return false;
	if (!ba.read_uint32(this->state)) return false;
	return true;
}

bool mole2_set_pet_flag_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->bit)) return false;
	if (!ba.write_uint32(this->state)) return false;
	return true;
}

	
mole2_set_skill_def_level_in::mole2_set_skill_def_level_in(){
	this->init();
}
void mole2_set_skill_def_level_in::init(){
	this->petid=0;
	this->skillid=0;
	this->def_level=0;

}
bool  mole2_set_skill_def_level_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->skillid)) return false;
	if (!ba.read_int32(this->def_level)) return false;
	return true;
}

bool mole2_set_skill_def_level_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->skillid)) return false;
	if (!ba.write_int32(this->def_level)) return false;
	return true;
}

	
mole2_set_system_time_in::mole2_set_system_time_in(){
	this->init();
}
void mole2_set_system_time_in::init(){

}
bool  mole2_set_system_time_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->datetime,20)) return false;
	return true;
}

bool mole2_set_system_time_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->datetime,20)) return false;
	return true;
}

	
mole2_set_user_cli_buff_in::mole2_set_user_cli_buff_in(){
	this->init();
}
void mole2_set_user_cli_buff_in::init(){
	this->type=0;

}
bool  mole2_set_user_cli_buff_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->type)) return false;
	if(!ba.read_buf(this->buff,512)) return false;
	return true;
}

bool mole2_set_user_cli_buff_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->type)) return false;
	if(!ba.write_buf(this->buff,512)) return false;
	return true;
}

	
mole2_set_vip_activity3_in::mole2_set_vip_activity3_in(){
	this->init();
}
void mole2_set_vip_activity3_in::init(){
	this->val=0;

}
bool  mole2_set_vip_activity3_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_int32(this->val)) return false;
	return true;
}

bool mole2_set_vip_activity3_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_int32(this->val)) return false;
	return true;
}

	
mole2_set_vip_activity_in::mole2_set_vip_activity_in(){
	this->init();
}
void mole2_set_vip_activity_in::init(){
	this->val=0;

}
bool  mole2_set_vip_activity_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_int32(this->val)) return false;
	return true;
}

bool mole2_set_vip_activity_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_int32(this->val)) return false;
	return true;
}

	
mole2_set_vip_buff_in::mole2_set_vip_buff_in(){
	this->init();
}
void mole2_set_vip_buff_in::init(){
	this->petlist.clear();

}
bool  mole2_set_vip_buff_in::read_from_buf(byte_array_t & ba ){

	uint32_t petlist_count ;
	if (!ba.read_uint32( petlist_count )) return false;
	stru_vip_buff  petlist_item;
	this->petlist.clear();
	{for(uint32_t i=0; i<petlist_count;i++){
		if (!petlist_item.read_from_buf(ba)) return false;
		this->petlist.push_back(petlist_item);
	}}
	return true;
}

bool mole2_set_vip_buff_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petlist.size())) return false;
	{for(uint32_t i=0; i<this->petlist.size() ;i++){
		if (!this->petlist[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
mole2_user_add_vip_ex_val_in::mole2_user_add_vip_ex_val_in(){
	this->init();
}
void mole2_user_add_vip_ex_val_in::init(){
	this->vip_ex_val=0;

}
bool  mole2_user_add_vip_ex_val_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->vip_ex_val)) return false;
	return true;
}

bool mole2_user_add_vip_ex_val_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->vip_ex_val)) return false;
	return true;
}

	
mole2_user_del_title_in::mole2_user_del_title_in(){
	this->init();
}
void mole2_user_del_title_in::init(){
	this->titleid=0;

}
bool  mole2_user_del_title_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->titleid)) return false;
	return true;
}

bool mole2_user_del_title_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->titleid)) return false;
	return true;
}

	
mole2_user_get_shop_name_out::mole2_user_get_shop_name_out(){
	this->init();
}
void mole2_user_get_shop_name_out::init(){

}
bool  mole2_user_get_shop_name_out::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool mole2_user_get_shop_name_out::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}

	
mole2_user_pet_skill_add_in::mole2_user_pet_skill_add_in(){
	this->init();
}
void mole2_user_pet_skill_add_in::init(){
	this->petid=0;
	this->skillid=0;
	this->level=0;

}
bool  mole2_user_pet_skill_add_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->skillid)) return false;
	if (!ba.read_uint8(this->level)) return false;
	return true;
}

bool mole2_user_pet_skill_add_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->skillid)) return false;
	if (!ba.write_uint8(this->level)) return false;
	return true;
}

	
mole2_user_pet_skill_add_out::mole2_user_pet_skill_add_out(){
	this->init();
}
void mole2_user_pet_skill_add_out::init(){
	this->petid=0;
	this->skillid=0;
	this->level=0;

}
bool  mole2_user_pet_skill_add_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->skillid)) return false;
	if (!ba.read_uint8(this->level)) return false;
	return true;
}

bool mole2_user_pet_skill_add_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->skillid)) return false;
	if (!ba.write_uint8(this->level)) return false;
	return true;
}

	
mole2_user_pet_skill_del_in::mole2_user_pet_skill_del_in(){
	this->init();
}
void mole2_user_pet_skill_del_in::init(){
	this->petid=0;
	this->skillid=0;
	this->level=0;

}
bool  mole2_user_pet_skill_del_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->skillid)) return false;
	if (!ba.read_uint8(this->level)) return false;
	return true;
}

bool mole2_user_pet_skill_del_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->skillid)) return false;
	if (!ba.write_uint8(this->level)) return false;
	return true;
}

	
mole2_user_pet_skill_del_out::mole2_user_pet_skill_del_out(){
	this->init();
}
void mole2_user_pet_skill_del_out::init(){
	this->petid=0;
	this->skillid=0;
	this->level=0;

}
bool  mole2_user_pet_skill_del_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->skillid)) return false;
	if (!ba.read_uint8(this->level)) return false;
	return true;
}

bool mole2_user_pet_skill_del_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->skillid)) return false;
	if (!ba.write_uint8(this->level)) return false;
	return true;
}

	
mole2_user_set_shop_name_in::mole2_user_set_shop_name_in(){
	this->init();
}
void mole2_user_set_shop_name_in::init(){

}
bool  mole2_user_set_shop_name_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool mole2_user_set_shop_name_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}

	
mole2_user_set_vip_base_val_in::mole2_user_set_vip_base_val_in(){
	this->init();
}
void mole2_user_set_vip_base_val_in::init(){
	this->growth_base=0;
	this->chnl_id=0;

}
bool  mole2_user_set_vip_base_val_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->growth_base)) return false;
	if (!ba.read_uint16(this->chnl_id)) return false;
	return true;
}

bool mole2_user_set_vip_base_val_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->growth_base)) return false;
	if (!ba.write_uint16(this->chnl_id)) return false;
	return true;
}

	
mole2_user_set_vip_info_in::mole2_user_set_vip_info_in(){
	this->init();
}
void mole2_user_set_vip_info_in::init(){
	this->flag=0;
	this->months=0;
	this->vip_end_time=0;
	this->vip_auto=0;
	this->vip_begin_time=0;
	this->last_charge_chnl_id=0;
	this->vip_type=0;

}
bool  mole2_user_set_vip_info_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->months)) return false;
	if (!ba.read_uint32(this->vip_end_time)) return false;
	if (!ba.read_uint32(this->vip_auto)) return false;
	if (!ba.read_uint32(this->vip_begin_time)) return false;
	if (!ba.read_uint32(this->last_charge_chnl_id)) return false;
	if (!ba.read_uint8(this->vip_type)) return false;
	return true;
}

bool mole2_user_set_vip_info_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->months)) return false;
	if (!ba.write_uint32(this->vip_end_time)) return false;
	if (!ba.write_uint32(this->vip_auto)) return false;
	if (!ba.write_uint32(this->vip_begin_time)) return false;
	if (!ba.write_uint32(this->last_charge_chnl_id)) return false;
	if (!ba.write_uint8(this->vip_type)) return false;
	return true;
}

	
mole2_user_set_vip_level_in::mole2_user_set_vip_level_in(){
	this->init();
}
void mole2_user_set_vip_level_in::init(){
	this->vip_level=0;

}
bool  mole2_user_set_vip_level_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->vip_level)) return false;
	return true;
}

bool mole2_user_set_vip_level_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->vip_level)) return false;
	return true;
}

	
mole2_user_update_rank_in::mole2_user_update_rank_in(){
	this->init();
}
void mole2_user_update_rank_in::init(){
	this->val=0;

}
bool  mole2_user_update_rank_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->val)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool mole2_user_update_rank_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->val)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}

	
mole2_user_user_create_role_in::mole2_user_user_create_role_in(){
	this->init();
}
void mole2_user_user_create_role_in::init(){
	this->parent=0;
	this->color=0;
	this->professtion=0;

}
bool  mole2_user_user_create_role_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->parent)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if(!ba.read_buf(this->signature,32)) return false;
	if (!ba.read_uint32(this->color)) return false;
	if (!ba.read_uint8(this->professtion)) return false;
	return true;
}

bool mole2_user_user_create_role_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->parent)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if(!ba.write_buf(this->signature,32)) return false;
	if (!ba.write_uint32(this->color)) return false;
	if (!ba.write_uint8(this->professtion)) return false;
	return true;
}

	
mole2_user_user_update_nick_in::mole2_user_user_update_nick_in(){
	this->init();
}
void mole2_user_user_update_nick_in::init(){

}
bool  mole2_user_user_update_nick_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool mole2_user_user_update_nick_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}

	
mole2_user_user_update_nick_out::mole2_user_user_update_nick_out(){
	this->init();
}
void mole2_user_user_update_nick_out::init(){

}
bool  mole2_user_user_update_nick_out::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool mole2_user_user_update_nick_out::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}

	
struct_test_info::struct_test_info(){
	this->init();
}
void struct_test_info::init(){
	this->n1=0;
	this->n2=0;
	this->_n3_len=0;

}
bool  struct_test_info::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->n1)) return false;
	if (!ba.read_uint16(this->n2)) return false;

	if (!ba.read_uint32(this->_n3_len )) return false;
	if (this->_n3_len>10) return false;
 	if (!ba.read_buf(this->n3,this->_n3_len)) return false;
	if(!ba.read_buf(this->n4,9)) return false;
	return true;
}

bool struct_test_info::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->n1)) return false;
	if (!ba.write_uint16(this->n2)) return false;
	if (this->_n3_len>10) return false;
	if (!ba.write_uint32(this->_n3_len))return false;
	if (!ba.write_buf(this->n3,this->_n3_len)) return false;
	if(!ba.write_buf(this->n4,9)) return false;
	return true;
}

	
stru_attire_info::stru_attire_info(){
	this->init();
}
void stru_attire_info::init(){
	this->attire_id=0;
	this->gridid=0;
	this->level=0;
	this->mduration=0;
	this->duration=0;
	this->hpmax=0;
	this->mpmax=0;
	this->atk=0;
	this->matk=0;
	this->def=0;
	this->mdef=0;
	this->speed=0;
	this->spirit=0;
	this->resume=0;
	this->hit=0;
	this->dodge=0;
	this->crit=0;
	this->fightback=0;
	this->rpoison=0;
	this->rlithification=0;
	this->rlethargy=0;
	this->rinebriation=0;
	this->rconfusion=0;
	this->roblivion=0;
	this->quality=0;
	this->validday=0;
	this->crystal_attr=0;
	this->bless_type=0;

}
bool  stru_attire_info::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->attire_id)) return false;
	if (!ba.read_uint32(this->gridid)) return false;
	if (!ba.read_uint16(this->level)) return false;
	if (!ba.read_uint16(this->mduration)) return false;
	if (!ba.read_uint16(this->duration)) return false;
	if (!ba.read_uint32(this->hpmax)) return false;
	if (!ba.read_uint32(this->mpmax)) return false;
	if (!ba.read_uint16(this->atk)) return false;
	if (!ba.read_uint16(this->matk)) return false;
	if (!ba.read_uint16(this->def)) return false;
	if (!ba.read_uint16(this->mdef)) return false;
	if (!ba.read_uint16(this->speed)) return false;
	if (!ba.read_uint16(this->spirit)) return false;
	if (!ba.read_uint16(this->resume)) return false;
	if (!ba.read_uint16(this->hit)) return false;
	if (!ba.read_uint16(this->dodge)) return false;
	if (!ba.read_uint16(this->crit)) return false;
	if (!ba.read_uint16(this->fightback)) return false;
	if (!ba.read_uint16(this->rpoison)) return false;
	if (!ba.read_uint16(this->rlithification)) return false;
	if (!ba.read_uint16(this->rlethargy)) return false;
	if (!ba.read_uint16(this->rinebriation)) return false;
	if (!ba.read_uint16(this->rconfusion)) return false;
	if (!ba.read_uint16(this->roblivion)) return false;
	if (!ba.read_uint32(this->quality)) return false;
	if (!ba.read_uint32(this->validday)) return false;
	if (!ba.read_uint32(this->crystal_attr)) return false;
	if (!ba.read_uint32(this->bless_type)) return false;
	return true;
}

bool stru_attire_info::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->attire_id)) return false;
	if (!ba.write_uint32(this->gridid)) return false;
	if (!ba.write_uint16(this->level)) return false;
	if (!ba.write_uint16(this->mduration)) return false;
	if (!ba.write_uint16(this->duration)) return false;
	if (!ba.write_uint32(this->hpmax)) return false;
	if (!ba.write_uint32(this->mpmax)) return false;
	if (!ba.write_uint16(this->atk)) return false;
	if (!ba.write_uint16(this->matk)) return false;
	if (!ba.write_uint16(this->def)) return false;
	if (!ba.write_uint16(this->mdef)) return false;
	if (!ba.write_uint16(this->speed)) return false;
	if (!ba.write_uint16(this->spirit)) return false;
	if (!ba.write_uint16(this->resume)) return false;
	if (!ba.write_uint16(this->hit)) return false;
	if (!ba.write_uint16(this->dodge)) return false;
	if (!ba.write_uint16(this->crit)) return false;
	if (!ba.write_uint16(this->fightback)) return false;
	if (!ba.write_uint16(this->rpoison)) return false;
	if (!ba.write_uint16(this->rlithification)) return false;
	if (!ba.write_uint16(this->rlethargy)) return false;
	if (!ba.write_uint16(this->rinebriation)) return false;
	if (!ba.write_uint16(this->rconfusion)) return false;
	if (!ba.write_uint16(this->roblivion)) return false;
	if (!ba.write_uint32(this->quality)) return false;
	if (!ba.write_uint32(this->validday)) return false;
	if (!ba.write_uint32(this->crystal_attr)) return false;
	if (!ba.write_uint32(this->bless_type)) return false;
	return true;
}

	
stru_limit_item::stru_limit_item(){
	this->init();
}
void stru_limit_item::init(){
	this->eid=0;
	this->itemid=0;
	this->val=0;

}
bool  stru_limit_item::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->eid)) return false;
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->val)) return false;
	return true;
}

bool stru_limit_item::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->eid)) return false;
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->val)) return false;
	return true;
}

	
su_get_last_pet_list_out::su_get_last_pet_list_out(){
	this->init();
}
void su_get_last_pet_list_out::init(){
	this->pet_list.clear();

}
bool  su_get_last_pet_list_out::read_from_buf(byte_array_t & ba ){

	uint32_t pet_list_count ;
	if (!ba.read_uint32( pet_list_count )) return false;
	stru_pet_info  pet_list_item;
	this->pet_list.clear();
	{for(uint32_t i=0; i<pet_list_count;i++){
		if (!pet_list_item.read_from_buf(ba)) return false;
		this->pet_list.push_back(pet_list_item);
	}}
	return true;
}

bool su_get_last_pet_list_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->pet_list.size())) return false;
	{for(uint32_t i=0; i<this->pet_list.size() ;i++){
		if (!this->pet_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
su_mole2_add_black_in::su_mole2_add_black_in(){
	this->init();
}
void su_mole2_add_black_in::init(){
	this->black_userid=0;

}
bool  su_mole2_add_black_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->black_userid)) return false;
	return true;
}

bool su_mole2_add_black_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->black_userid)) return false;
	return true;
}

	
su_mole2_add_cloth_in::su_mole2_add_cloth_in(){
	this->init();
}
void su_mole2_add_cloth_in::init(){
	this->attire_id=0;

}
bool  su_mole2_add_cloth_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->attire_id)) return false;
	return true;
}

bool su_mole2_add_cloth_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->attire_id)) return false;
	return true;
}

	
su_mole2_add_friend_in::su_mole2_add_friend_in(){
	this->init();
}
void su_mole2_add_friend_in::init(){
	this->friendid=0;

}
bool  su_mole2_add_friend_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->friendid)) return false;
	return true;
}

bool su_mole2_add_friend_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->friendid)) return false;
	return true;
}

	
su_mole2_add_item_in::su_mole2_add_item_in(){
	this->init();
}
void su_mole2_add_item_in::init(){
	this->itemid=0;
	this->add_count=0;
	this->add_storage=0;

}
bool  su_mole2_add_item_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_int32(this->add_count)) return false;
	if (!ba.read_int32(this->add_storage)) return false;
	return true;
}

bool su_mole2_add_item_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_int32(this->add_count)) return false;
	if (!ba.write_int32(this->add_storage)) return false;
	return true;
}

	
su_mole2_add_mail_in::su_mole2_add_mail_in(){
	this->init();
}
void su_mole2_add_mail_in::init(){
	this->type=0;
	this->themeid=0;
	this->sender_id=0;

}
bool  su_mole2_add_mail_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->type)) return false;
	if (!ba.read_uint32(this->themeid)) return false;
	if (!ba.read_uint32(this->sender_id)) return false;
	if(!ba.read_buf(this->sender_nick,64)) return false;
	if(!ba.read_buf(this->title,64)) return false;
	if(!ba.read_buf(this->message,256)) return false;
	if(!ba.read_buf(this->items,64)) return false;
	return true;
}

bool su_mole2_add_mail_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->type)) return false;
	if (!ba.write_uint32(this->themeid)) return false;
	if (!ba.write_uint32(this->sender_id)) return false;
	if(!ba.write_buf(this->sender_nick,64)) return false;
	if(!ba.write_buf(this->title,64)) return false;
	if(!ba.write_buf(this->message,256)) return false;
	if(!ba.write_buf(this->items,64)) return false;
	return true;
}

	
su_mole2_add_pet_in::su_mole2_add_pet_in(){
	this->init();
}
void su_mole2_add_pet_in::init(){
	this->pet.init();

}
bool  su_mole2_add_pet_in::read_from_buf(byte_array_t & ba ){
	if (!this->pet.read_from_buf(ba)) return false;
	return true;
}

bool su_mole2_add_pet_in::write_to_buf(byte_array_t & ba ){
	if (!this->pet.write_to_buf(ba)) return false;
	return true;
}

	
su_mole2_add_skill_in::su_mole2_add_skill_in(){
	this->init();
}
void su_mole2_add_skill_in::init(){
	this->skill.init();

}
bool  su_mole2_add_skill_in::read_from_buf(byte_array_t & ba ){
	if (!this->skill.read_from_buf(ba)) return false;
	return true;
}

bool su_mole2_add_skill_in::write_to_buf(byte_array_t & ba ){
	if (!this->skill.write_to_buf(ba)) return false;
	return true;
}

	
su_mole2_add_task_in::su_mole2_add_task_in(){
	this->init();
}
void su_mole2_add_task_in::init(){
	this->taskid=0;

}
bool  su_mole2_add_task_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	return true;
}

bool su_mole2_add_task_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	return true;
}

	
su_mole2_add_title_in::su_mole2_add_title_in(){
	this->init();
}
void su_mole2_add_title_in::init(){
	this->titleid=0;
	this->time=0;

}
bool  su_mole2_add_title_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->titleid)) return false;
	if (!ba.read_uint32(this->time)) return false;
	return true;
}

bool su_mole2_add_title_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->titleid)) return false;
	if (!ba.write_uint32(this->time)) return false;
	return true;
}

	
su_mole2_add_xiaomee_in::su_mole2_add_xiaomee_in(){
	this->init();
}
void su_mole2_add_xiaomee_in::init(){
	this->xiaomee=0;

}
bool  su_mole2_add_xiaomee_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_int32(this->xiaomee)) return false;
	return true;
}

bool su_mole2_add_xiaomee_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_int32(this->xiaomee)) return false;
	return true;
}

	
su_mole2_delete_cloths_in::su_mole2_delete_cloths_in(){
	this->init();
}
void su_mole2_delete_cloths_in::init(){
	this->gettime=0;
	this->clothid=0;

}
bool  su_mole2_delete_cloths_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gettime)) return false;
	if (!ba.read_uint32(this->clothid)) return false;
	return true;
}

bool su_mole2_delete_cloths_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gettime)) return false;
	if (!ba.write_uint32(this->clothid)) return false;
	return true;
}

	
su_mole2_delete_title_in::su_mole2_delete_title_in(){
	this->init();
}
void su_mole2_delete_title_in::init(){
	this->titleid=0;

}
bool  su_mole2_delete_title_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->titleid)) return false;
	return true;
}

bool su_mole2_delete_title_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->titleid)) return false;
	return true;
}

	
su_mole2_del_black_in::su_mole2_del_black_in(){
	this->init();
}
void su_mole2_del_black_in::init(){
	this->black_userid=0;

}
bool  su_mole2_del_black_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->black_userid)) return false;
	return true;
}

bool su_mole2_del_black_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->black_userid)) return false;
	return true;
}

	
su_mole2_del_friend_in::su_mole2_del_friend_in(){
	this->init();
}
void su_mole2_del_friend_in::init(){
	this->friendid=0;

}
bool  su_mole2_del_friend_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->friendid)) return false;
	return true;
}

bool su_mole2_del_friend_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->friendid)) return false;
	return true;
}

	
su_mole2_del_pet_in::su_mole2_del_pet_in(){
	this->init();
}
void su_mole2_del_pet_in::init(){
	this->petid=0;

}
bool  su_mole2_del_pet_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	return true;
}

bool su_mole2_del_pet_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	return true;
}

	
su_mole2_del_skill_in::su_mole2_del_skill_in(){
	this->init();
}
void su_mole2_del_skill_in::init(){
	this->skillid=0;

}
bool  su_mole2_del_skill_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->skillid)) return false;
	return true;
}

bool su_mole2_del_skill_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->skillid)) return false;
	return true;
}

	
su_mole2_del_task_in::su_mole2_del_task_in(){
	this->init();
}
void su_mole2_del_task_in::init(){
	this->taskid=0;

}
bool  su_mole2_del_task_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	return true;
}

bool su_mole2_del_task_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	return true;
}

	
su_mole2_get_ban_flag_out::su_mole2_get_ban_flag_out(){
	this->init();
}
void su_mole2_get_ban_flag_out::init(){
	this->flag=0;

}
bool  su_mole2_get_ban_flag_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	return true;
}

bool su_mole2_get_ban_flag_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	return true;
}

	
su_mole2_get_blacks_out::su_mole2_get_blacks_out(){
	this->init();
}
void su_mole2_get_blacks_out::init(){
	this->blacks.clear();

}
bool  su_mole2_get_blacks_out::read_from_buf(byte_array_t & ba ){

	uint32_t blacks_count ;
	if (!ba.read_uint32( blacks_count )) return false;
	uint32_t  blacks_item;
	this->blacks.clear();
	{for(uint32_t i=0; i<blacks_count;i++){
		if (!ba.read_uint32(blacks_item)) return false;
		this->blacks.push_back(blacks_item);
	}}
	return true;
}

bool su_mole2_get_blacks_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->blacks.size())) return false;
	{for(uint32_t i=0; i<this->blacks.size() ;i++){
		if (!ba.write_uint32(this->blacks[i])) return false;
	}}
	return true;
}

	
su_mole2_get_day_items_out::su_mole2_get_day_items_out(){
	this->init();
}
void su_mole2_get_day_items_out::init(){
	this->day_items.clear();

}
bool  su_mole2_get_day_items_out::read_from_buf(byte_array_t & ba ){

	uint32_t day_items_count ;
	if (!ba.read_uint32( day_items_count )) return false;
	stru_day_limit  day_items_item;
	this->day_items.clear();
	{for(uint32_t i=0; i<day_items_count;i++){
		if (!day_items_item.read_from_buf(ba)) return false;
		this->day_items.push_back(day_items_item);
	}}
	return true;
}

bool su_mole2_get_day_items_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->day_items.size())) return false;
	{for(uint32_t i=0; i<this->day_items.size() ;i++){
		if (!this->day_items[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
su_mole2_get_equips_out::su_mole2_get_equips_out(){
	this->init();
}
void su_mole2_get_equips_out::init(){
	this->cloths.clear();

}
bool  su_mole2_get_equips_out::read_from_buf(byte_array_t & ba ){

	uint32_t cloths_count ;
	if (!ba.read_uint32( cloths_count )) return false;
	stru_cloth_info  cloths_item;
	this->cloths.clear();
	{for(uint32_t i=0; i<cloths_count;i++){
		if (!cloths_item.read_from_buf(ba)) return false;
		this->cloths.push_back(cloths_item);
	}}
	return true;
}

bool su_mole2_get_equips_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->cloths.size())) return false;
	{for(uint32_t i=0; i<this->cloths.size() ;i++){
		if (!this->cloths[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
su_mole2_get_expbox_out::su_mole2_get_expbox_out(){
	this->init();
}
void su_mole2_get_expbox_out::init(){
	this->expbox=0;

}
bool  su_mole2_get_expbox_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->expbox)) return false;
	return true;
}

bool su_mole2_get_expbox_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->expbox)) return false;
	return true;
}

	
su_mole2_get_friends_out::su_mole2_get_friends_out(){
	this->init();
}
void su_mole2_get_friends_out::init(){
	this->friends.clear();

}
bool  su_mole2_get_friends_out::read_from_buf(byte_array_t & ba ){

	uint32_t friends_count ;
	if (!ba.read_uint32( friends_count )) return false;
	uint32_t  friends_item;
	this->friends.clear();
	{for(uint32_t i=0; i<friends_count;i++){
		if (!ba.read_uint32(friends_item)) return false;
		this->friends.push_back(friends_item);
	}}
	return true;
}

bool su_mole2_get_friends_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->friends.size())) return false;
	{for(uint32_t i=0; i<this->friends.size() ;i++){
		if (!ba.write_uint32(this->friends[i])) return false;
	}}
	return true;
}

	
su_mole2_get_items_out::su_mole2_get_items_out(){
	this->init();
}
void su_mole2_get_items_out::init(){
	this->items.clear();

}
bool  su_mole2_get_items_out::read_from_buf(byte_array_t & ba ){

	uint32_t items_count ;
	if (!ba.read_uint32( items_count )) return false;
	stru_item_info  items_item;
	this->items.clear();
	{for(uint32_t i=0; i<items_count;i++){
		if (!items_item.read_from_buf(ba)) return false;
		this->items.push_back(items_item);
	}}
	return true;
}

bool su_mole2_get_items_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->items.size())) return false;
	{for(uint32_t i=0; i<this->items.size() ;i++){
		if (!this->items[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
su_mole2_get_pets_out::su_mole2_get_pets_out(){
	this->init();
}
void su_mole2_get_pets_out::init(){
	this->pets_in_bag.clear();
	this->pets_in_home.clear();

}
bool  su_mole2_get_pets_out::read_from_buf(byte_array_t & ba ){

	uint32_t pets_in_bag_count ;
	if (!ba.read_uint32( pets_in_bag_count )) return false;
	stru_pet_info  pets_in_bag_item;
	this->pets_in_bag.clear();
	{for(uint32_t i=0; i<pets_in_bag_count;i++){
		if (!pets_in_bag_item.read_from_buf(ba)) return false;
		this->pets_in_bag.push_back(pets_in_bag_item);
	}}

	uint32_t pets_in_home_count ;
	if (!ba.read_uint32( pets_in_home_count )) return false;
	stru_pet_info  pets_in_home_item;
	this->pets_in_home.clear();
	{for(uint32_t i=0; i<pets_in_home_count;i++){
		if (!pets_in_home_item.read_from_buf(ba)) return false;
		this->pets_in_home.push_back(pets_in_home_item);
	}}
	return true;
}

bool su_mole2_get_pets_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->pets_in_bag.size())) return false;
	{for(uint32_t i=0; i<this->pets_in_bag.size() ;i++){
		if (!this->pets_in_bag[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->pets_in_home.size())) return false;
	{for(uint32_t i=0; i<this->pets_in_home.size() ;i++){
		if (!this->pets_in_home[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
su_mole2_get_skills_out::su_mole2_get_skills_out(){
	this->init();
}
void su_mole2_get_skills_out::init(){
	this->skills.clear();

}
bool  su_mole2_get_skills_out::read_from_buf(byte_array_t & ba ){

	uint32_t skills_count ;
	if (!ba.read_uint32( skills_count )) return false;
	stru_skill_info  skills_item;
	this->skills.clear();
	{for(uint32_t i=0; i<skills_count;i++){
		if (!skills_item.read_from_buf(ba)) return false;
		this->skills.push_back(skills_item);
	}}
	return true;
}

bool su_mole2_get_skills_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->skills.size())) return false;
	{for(uint32_t i=0; i<this->skills.size() ;i++){
		if (!this->skills[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
su_mole2_get_tasks_out::su_mole2_get_tasks_out(){
	this->init();
}
void su_mole2_get_tasks_out::init(){
	this->tasks_doing.clear();
	this->tasks_done.clear();

}
bool  su_mole2_get_tasks_out::read_from_buf(byte_array_t & ba ){

	uint32_t tasks_doing_count ;
	if (!ba.read_uint32( tasks_doing_count )) return false;
	stru_task_doing  tasks_doing_item;
	this->tasks_doing.clear();
	{for(uint32_t i=0; i<tasks_doing_count;i++){
		if (!tasks_doing_item.read_from_buf(ba)) return false;
		this->tasks_doing.push_back(tasks_doing_item);
	}}

	uint32_t tasks_done_count ;
	if (!ba.read_uint32( tasks_done_count )) return false;
	stru_task_done  tasks_done_item;
	this->tasks_done.clear();
	{for(uint32_t i=0; i<tasks_done_count;i++){
		if (!tasks_done_item.read_from_buf(ba)) return false;
		this->tasks_done.push_back(tasks_done_item);
	}}
	return true;
}

bool su_mole2_get_tasks_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->tasks_doing.size())) return false;
	{for(uint32_t i=0; i<this->tasks_doing.size() ;i++){
		if (!this->tasks_doing[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->tasks_done.size())) return false;
	{for(uint32_t i=0; i<this->tasks_done.size() ;i++){
		if (!this->tasks_done[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
su_mole2_get_user_out::su_mole2_get_user_out(){
	this->init();
}
void su_mole2_get_user_out::init(){
	this->flag=0;
	this->color=0;
	this->regist_time=0;
	this->race=0;
	this->profession=0;
	this->joblevel=0;
	this->honor=0;
	this->xiaomee=0;
	this->level=0;
	this->experience=0;
	this->physique=0;
	this->strength=0;
	this->endurance=0;
	this->quick=0;
	this->intelligence=0;
	this->attr_addition=0;
	this->hp=0;
	this->mp=0;
	this->injury_state=0;
	this->in_front=0;
	this->max_attire=0;
	this->max_medicine=0;
	this->max_stuff=0;
	this->time=0;
	this->axis_x=0;
	this->axis_y=0;
	this->mapid=0;
	this->daytime=0;
	this->fly_mapid=0;
	this->expbox=0;
	this->energy=0;
	this->skill_expbox=0;
	this->flag_ex=0;
	this->winbossid=0;
	this->parent=0;
	this->battle.init();
	this->sign_count=0;

}
bool  su_mole2_get_user_out::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->color)) return false;
	if (!ba.read_uint32(this->regist_time)) return false;
	if (!ba.read_uint32(this->race)) return false;
	if (!ba.read_uint32(this->profession)) return false;
	if (!ba.read_uint32(this->joblevel)) return false;
	if (!ba.read_uint32(this->honor)) return false;
	if (!ba.read_uint32(this->xiaomee)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->experience)) return false;
	if (!ba.read_uint32(this->physique)) return false;
	if (!ba.read_uint32(this->strength)) return false;
	if (!ba.read_uint32(this->endurance)) return false;
	if (!ba.read_uint32(this->quick)) return false;
	if (!ba.read_uint32(this->intelligence)) return false;
	if (!ba.read_uint32(this->attr_addition)) return false;
	if (!ba.read_uint32(this->hp)) return false;
	if (!ba.read_uint32(this->mp)) return false;
	if (!ba.read_uint32(this->injury_state)) return false;
	if (!ba.read_uint32(this->in_front)) return false;
	if (!ba.read_uint32(this->max_attire)) return false;
	if (!ba.read_uint32(this->max_medicine)) return false;
	if (!ba.read_uint32(this->max_stuff)) return false;
	if (!ba.read_uint32(this->time)) return false;
	if (!ba.read_uint32(this->axis_x)) return false;
	if (!ba.read_uint32(this->axis_y)) return false;
	if (!ba.read_uint32(this->mapid)) return false;
	if (!ba.read_uint32(this->daytime)) return false;
	if (!ba.read_uint32(this->fly_mapid)) return false;
	if (!ba.read_uint32(this->expbox)) return false;
	if (!ba.read_uint32(this->energy)) return false;
	if (!ba.read_uint32(this->skill_expbox)) return false;
	if (!ba.read_uint32(this->flag_ex)) return false;
	if (!ba.read_uint32(this->winbossid)) return false;
	if (!ba.read_uint32(this->parent)) return false;
	if (!this->battle.read_from_buf(ba)) return false;
	if (!ba.read_uint32(this->sign_count)) return false;
	return true;
}

bool su_mole2_get_user_out::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->color)) return false;
	if (!ba.write_uint32(this->regist_time)) return false;
	if (!ba.write_uint32(this->race)) return false;
	if (!ba.write_uint32(this->profession)) return false;
	if (!ba.write_uint32(this->joblevel)) return false;
	if (!ba.write_uint32(this->honor)) return false;
	if (!ba.write_uint32(this->xiaomee)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->experience)) return false;
	if (!ba.write_uint32(this->physique)) return false;
	if (!ba.write_uint32(this->strength)) return false;
	if (!ba.write_uint32(this->endurance)) return false;
	if (!ba.write_uint32(this->quick)) return false;
	if (!ba.write_uint32(this->intelligence)) return false;
	if (!ba.write_uint32(this->attr_addition)) return false;
	if (!ba.write_uint32(this->hp)) return false;
	if (!ba.write_uint32(this->mp)) return false;
	if (!ba.write_uint32(this->injury_state)) return false;
	if (!ba.write_uint32(this->in_front)) return false;
	if (!ba.write_uint32(this->max_attire)) return false;
	if (!ba.write_uint32(this->max_medicine)) return false;
	if (!ba.write_uint32(this->max_stuff)) return false;
	if (!ba.write_uint32(this->time)) return false;
	if (!ba.write_uint32(this->axis_x)) return false;
	if (!ba.write_uint32(this->axis_y)) return false;
	if (!ba.write_uint32(this->mapid)) return false;
	if (!ba.write_uint32(this->daytime)) return false;
	if (!ba.write_uint32(this->fly_mapid)) return false;
	if (!ba.write_uint32(this->expbox)) return false;
	if (!ba.write_uint32(this->energy)) return false;
	if (!ba.write_uint32(this->skill_expbox)) return false;
	if (!ba.write_uint32(this->flag_ex)) return false;
	if (!ba.write_uint32(this->winbossid)) return false;
	if (!ba.write_uint32(this->parent)) return false;
	if (!this->battle.write_to_buf(ba)) return false;
	if (!ba.write_uint32(this->sign_count)) return false;
	return true;
}

	
su_mole2_get_vip_flag_out::su_mole2_get_vip_flag_out(){
	this->init();
}
void su_mole2_get_vip_flag_out::init(){
	this->flag=0;

}
bool  su_mole2_get_vip_flag_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	return true;
}

bool su_mole2_get_vip_flag_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	return true;
}

	
su_mole2_get_vip_info_out::su_mole2_get_vip_info_out(){
	this->init();
}
void su_mole2_get_vip_info_out::init(){
	this->vip_auto_charge=0;
	this->vip_level=0;
	this->vip_value=0;
	this->vip_base_value=0;
	this->vip_end_time=0;
	this->vip_begin_time=0;

}
bool  su_mole2_get_vip_info_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->vip_auto_charge)) return false;
	if (!ba.read_uint32(this->vip_level)) return false;
	if (!ba.read_uint32(this->vip_value)) return false;
	if (!ba.read_uint32(this->vip_base_value)) return false;
	if (!ba.read_uint32(this->vip_end_time)) return false;
	if (!ba.read_uint32(this->vip_begin_time)) return false;
	return true;
}

bool su_mole2_get_vip_info_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->vip_auto_charge)) return false;
	if (!ba.write_uint32(this->vip_level)) return false;
	if (!ba.write_uint32(this->vip_value)) return false;
	if (!ba.write_uint32(this->vip_base_value)) return false;
	if (!ba.write_uint32(this->vip_end_time)) return false;
	if (!ba.write_uint32(this->vip_begin_time)) return false;
	return true;
}

	
su_mole2_set_ban_flag_in::su_mole2_set_ban_flag_in(){
	this->init();
}
void su_mole2_set_ban_flag_in::init(){
	this->flag=0;

}
bool  su_mole2_set_ban_flag_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	return true;
}

bool su_mole2_set_ban_flag_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	return true;
}

	
su_mole2_set_base_attr_in::su_mole2_set_base_attr_in(){
	this->init();
}
void su_mole2_set_base_attr_in::init(){
	this->physique=0;
	this->strength=0;
	this->endurance=0;
	this->quick=0;
	this->intelligence=0;
	this->addition=0;

}
bool  su_mole2_set_base_attr_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_int32(this->physique)) return false;
	if (!ba.read_int32(this->strength)) return false;
	if (!ba.read_int32(this->endurance)) return false;
	if (!ba.read_int32(this->quick)) return false;
	if (!ba.read_int32(this->intelligence)) return false;
	if (!ba.read_uint32(this->addition)) return false;
	return true;
}

bool su_mole2_set_base_attr_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_int32(this->physique)) return false;
	if (!ba.write_int32(this->strength)) return false;
	if (!ba.write_int32(this->endurance)) return false;
	if (!ba.write_int32(this->quick)) return false;
	if (!ba.write_int32(this->intelligence)) return false;
	if (!ba.write_uint32(this->addition)) return false;
	return true;
}

	
su_mole2_set_expbox_in::su_mole2_set_expbox_in(){
	this->init();
}
void su_mole2_set_expbox_in::init(){
	this->expbox=0;

}
bool  su_mole2_set_expbox_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->expbox)) return false;
	return true;
}

bool su_mole2_set_expbox_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->expbox)) return false;
	return true;
}

	
su_mole2_set_field_value_in::su_mole2_set_field_value_in(){
	this->init();
}
void su_mole2_set_field_value_in::init(){
	this->opt_type=0;
	this->opt_id=0;

}
bool  su_mole2_set_field_value_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->opt_type)) return false;
	if (!ba.read_uint32(this->opt_id)) return false;
	if(!ba.read_buf(this->field,32)) return false;
	if(!ba.read_buf(this->value,255)) return false;
	return true;
}

bool su_mole2_set_field_value_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->opt_type)) return false;
	if (!ba.write_uint32(this->opt_id)) return false;
	if(!ba.write_buf(this->field,32)) return false;
	if(!ba.write_buf(this->value,255)) return false;
	return true;
}

	
su_mole2_set_nick_in::su_mole2_set_nick_in(){
	this->init();
}
void su_mole2_set_nick_in::init(){

}
bool  su_mole2_set_nick_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool su_mole2_set_nick_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}

	
su_mole2_set_pet_in::su_mole2_set_pet_in(){
	this->init();
}
void su_mole2_set_pet_in::init(){
	this->pet.init();

}
bool  su_mole2_set_pet_in::read_from_buf(byte_array_t & ba ){
	if (!this->pet.read_from_buf(ba)) return false;
	return true;
}

bool su_mole2_set_pet_in::write_to_buf(byte_array_t & ba ){
	if (!this->pet.write_to_buf(ba)) return false;
	return true;
}

	
su_mole2_set_position_in::su_mole2_set_position_in(){
	this->init();
}
void su_mole2_set_position_in::init(){
	this->position=0;

}
bool  su_mole2_set_position_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->position)) return false;
	return true;
}

bool su_mole2_set_position_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->position)) return false;
	return true;
}

	
su_mole2_set_profession_in::su_mole2_set_profession_in(){
	this->init();
}
void su_mole2_set_profession_in::init(){
	this->profession=0;

}
bool  su_mole2_set_profession_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->profession)) return false;
	return true;
}

bool su_mole2_set_profession_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->profession)) return false;
	return true;
}

	
su_mole2_set_task_in::su_mole2_set_task_in(){
	this->init();
}
void su_mole2_set_task_in::init(){
	this->taskid=0;
	this->state=0;

}
bool  su_mole2_set_task_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->taskid)) return false;
	if (!ba.read_uint32(this->state)) return false;
	return true;
}

bool su_mole2_set_task_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->taskid)) return false;
	if (!ba.write_uint32(this->state)) return false;
	return true;
}

	
su_mole2_simple_opt_in::su_mole2_simple_opt_in(){
	this->init();
}
void su_mole2_simple_opt_in::init(){
	this->table_type=0;
	this->opt_type=0;
	this->v1=0;
	this->v2=0;
	this->v3=0;
	this->v4=0;
	this->v5=0;

}
bool  su_mole2_simple_opt_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->table_type)) return false;
	if (!ba.read_uint32(this->opt_type)) return false;
	if (!ba.read_uint32(this->v1)) return false;
	if (!ba.read_uint32(this->v2)) return false;
	if (!ba.read_uint32(this->v3)) return false;
	if (!ba.read_int32(this->v4)) return false;
	if (!ba.read_int32(this->v5)) return false;
	return true;
}

bool su_mole2_simple_opt_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->table_type)) return false;
	if (!ba.write_uint32(this->opt_type)) return false;
	if (!ba.write_uint32(this->v1)) return false;
	if (!ba.write_uint32(this->v2)) return false;
	if (!ba.write_uint32(this->v3)) return false;
	if (!ba.write_int32(this->v4)) return false;
	if (!ba.write_int32(this->v5)) return false;
	return true;
}

	
su_switch_send_msg_in::su_switch_send_msg_in(){
	this->init();
}
void su_switch_send_msg_in::init(){

}
bool  su_switch_send_msg_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->msg,1024)) return false;
	return true;
}

bool su_switch_send_msg_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->msg,1024)) return false;
	return true;
}

	
user_home_t::user_home_t(){
	this->init();
}
void user_home_t::init(){
	this->flag=0;
	this->type=0;
	this->level=0;
	this->exp=0;
	this->photo=0;
	this->blackboard=0;
	this->bookshelf=0;
	this->honorbox=0;
	this->petfun=0;
	this->expbox=0;
	this->effigy=0;
	this->postbox=0;
	this->itembox=0;
	this->petbox=0;
	this->compose1=0;
	this->compose2=0;
	this->compose3=0;
	this->compose4=0;
	this->wall=0;
	this->floor=0;
	this->bed=0;
	this->desk=0;
	this->ladder=0;
	this->petbox_cnt=0;

}
bool  user_home_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->type)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->exp)) return false;
	if(!ba.read_buf(this->name,16)) return false;
	if (!ba.read_uint32(this->photo)) return false;
	if (!ba.read_uint32(this->blackboard)) return false;
	if (!ba.read_uint32(this->bookshelf)) return false;
	if (!ba.read_uint32(this->honorbox)) return false;
	if (!ba.read_uint32(this->petfun)) return false;
	if (!ba.read_uint32(this->expbox)) return false;
	if (!ba.read_uint32(this->effigy)) return false;
	if (!ba.read_uint32(this->postbox)) return false;
	if (!ba.read_uint32(this->itembox)) return false;
	if (!ba.read_uint32(this->petbox)) return false;
	if (!ba.read_uint32(this->compose1)) return false;
	if (!ba.read_uint32(this->compose2)) return false;
	if (!ba.read_uint32(this->compose3)) return false;
	if (!ba.read_uint32(this->compose4)) return false;
	if (!ba.read_uint32(this->wall)) return false;
	if (!ba.read_uint32(this->floor)) return false;
	if (!ba.read_uint32(this->bed)) return false;
	if (!ba.read_uint32(this->desk)) return false;
	if (!ba.read_uint32(this->ladder)) return false;
	if (!ba.read_uint32(this->petbox_cnt)) return false;
	return true;
}

bool user_home_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->type)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->exp)) return false;
	if(!ba.write_buf(this->name,16)) return false;
	if (!ba.write_uint32(this->photo)) return false;
	if (!ba.write_uint32(this->blackboard)) return false;
	if (!ba.write_uint32(this->bookshelf)) return false;
	if (!ba.write_uint32(this->honorbox)) return false;
	if (!ba.write_uint32(this->petfun)) return false;
	if (!ba.write_uint32(this->expbox)) return false;
	if (!ba.write_uint32(this->effigy)) return false;
	if (!ba.write_uint32(this->postbox)) return false;
	if (!ba.write_uint32(this->itembox)) return false;
	if (!ba.write_uint32(this->petbox)) return false;
	if (!ba.write_uint32(this->compose1)) return false;
	if (!ba.write_uint32(this->compose2)) return false;
	if (!ba.write_uint32(this->compose3)) return false;
	if (!ba.write_uint32(this->compose4)) return false;
	if (!ba.write_uint32(this->wall)) return false;
	if (!ba.write_uint32(this->floor)) return false;
	if (!ba.write_uint32(this->bed)) return false;
	if (!ba.write_uint32(this->desk)) return false;
	if (!ba.write_uint32(this->ladder)) return false;
	if (!ba.write_uint32(this->petbox_cnt)) return false;
	return true;
}

	