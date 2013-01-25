#include "mole2_btl.h"

accept_battle_op_in::accept_battle_op_in(){
	this->init();
}
void accept_battle_op_in::init(){
	this->grpid=0;
	this->win=0;
	this->percent=0;
	this->mark=0;
	this->warriors.clear();

}
bool  accept_battle_op_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->grpid)) return false;
	if (!ba.read_uint32(this->win)) return false;
	if (!ba.read_uint32(this->percent)) return false;
	if (!ba.read_uint32(this->mark)) return false;

	uint32_t warriors_count ;
	if (!ba.read_uint32( warriors_count )) return false;
	stru_warrior_t  warriors_item;
	this->warriors.clear();
	{for(uint32_t i=0; i<warriors_count;i++){
		if (!warriors_item.read_from_buf(ba)) return false;
		this->warriors.push_back(warriors_item);
	}}
	return true;
}

bool accept_battle_op_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->grpid)) return false;
	if (!ba.write_uint32(this->win)) return false;
	if (!ba.write_uint32(this->percent)) return false;
	if (!ba.write_uint32(this->mark)) return false;
	if (!ba.write_uint32(this->warriors.size())) return false;
	{for(uint32_t i=0; i<this->warriors.size() ;i++){
		if (!this->warriors[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
nest_atk_info_t::nest_atk_info_t(){
	this->init();
}
void nest_atk_info_t::init(){
	this->uid=0;
	this->petid=0;
	this->fantan_ret=0;
	this->atk_type=0;
	this->atk_level=0;
	this->add_hp=0;
	this->fantan_hp=0;
	this->fantan_mp=0;
	this->use_itemid=0;
	this->atk_uid=0;
	this->atk_petid=0;
	this->huwei_pos=0;
	this->fightee_state=0;
	this->hurt_hp=0;
	this->hurt_mp=0;

}
bool  nest_atk_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint8(this->fantan_ret)) return false;
	if (!ba.read_uint32(this->atk_type)) return false;
	if (!ba.read_uint8(this->atk_level)) return false;
	if (!ba.read_uint16(this->add_hp)) return false;
	if (!ba.read_uint16(this->fantan_hp)) return false;
	if (!ba.read_uint16(this->fantan_mp)) return false;
	if (!ba.read_uint32(this->use_itemid)) return false;
	if (!ba.read_uint32(this->atk_uid)) return false;
	if (!ba.read_uint32(this->atk_petid)) return false;
	if (!ba.read_uint8(this->huwei_pos)) return false;
	if (!ba.read_uint64(this->fightee_state)) return false;
	if (!ba.read_uint16(this->hurt_hp)) return false;
	if (!ba.read_uint16(this->hurt_mp)) return false;
	return true;
}

bool nest_atk_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint8(this->fantan_ret)) return false;
	if (!ba.write_uint32(this->atk_type)) return false;
	if (!ba.write_uint8(this->atk_level)) return false;
	if (!ba.write_uint16(this->add_hp)) return false;
	if (!ba.write_uint16(this->fantan_hp)) return false;
	if (!ba.write_uint16(this->fantan_mp)) return false;
	if (!ba.write_uint32(this->use_itemid)) return false;
	if (!ba.write_uint32(this->atk_uid)) return false;
	if (!ba.write_uint32(this->atk_petid)) return false;
	if (!ba.write_uint8(this->huwei_pos)) return false;
	if (!ba.write_uint64(this->fightee_state)) return false;
	if (!ba.write_uint16(this->hurt_hp)) return false;
	if (!ba.write_uint16(this->hurt_mp)) return false;
	return true;
}

	
attack_info_t::attack_info_t(){
	this->init();
}
void attack_info_t::init(){
	this->seq=0;
	this->fighter=0;
	this->petid=0;
	this->fighter_state=0;
	this->topic_id=0;
	this->resume_hp1=0;
	this->resume_hp2=0;
	this->resume_hp3=0;
	this->poison_hp=0;
	this->host_uid=0;
	this->host_petid=0;
	this->parasitism_hp=0;
	this->zhushang_hp=0;
	this->curse_mp=0;
	this->nest_akt_list.clear();

}
bool  attack_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->seq)) return false;
	if (!ba.read_uint32(this->fighter)) return false;
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint64(this->fighter_state)) return false;
	if (!ba.read_uint32(this->topic_id)) return false;
	if (!ba.read_uint16(this->resume_hp1)) return false;
	if (!ba.read_uint16(this->resume_hp2)) return false;
	if (!ba.read_uint16(this->resume_hp3)) return false;
	if (!ba.read_uint16(this->poison_hp)) return false;
	if (!ba.read_uint32(this->host_uid)) return false;
	if (!ba.read_uint32(this->host_petid)) return false;
	if (!ba.read_uint16(this->parasitism_hp)) return false;
	if (!ba.read_uint16(this->zhushang_hp)) return false;
	if (!ba.read_uint16(this->curse_mp)) return false;

	uint16_t nest_akt_list_count ;
	if (!ba.read_uint16( nest_akt_list_count )) return false;
	nest_atk_info_t  nest_akt_list_item;
	this->nest_akt_list.clear();
	{for(uint32_t i=0; i<nest_akt_list_count;i++){
		if (!nest_akt_list_item.read_from_buf(ba)) return false;
		this->nest_akt_list.push_back(nest_akt_list_item);
	}}
	return true;
}

bool attack_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->seq)) return false;
	if (!ba.write_uint32(this->fighter)) return false;
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint64(this->fighter_state)) return false;
	if (!ba.write_uint32(this->topic_id)) return false;
	if (!ba.write_uint16(this->resume_hp1)) return false;
	if (!ba.write_uint16(this->resume_hp2)) return false;
	if (!ba.write_uint16(this->resume_hp3)) return false;
	if (!ba.write_uint16(this->poison_hp)) return false;
	if (!ba.write_uint32(this->host_uid)) return false;
	if (!ba.write_uint32(this->host_petid)) return false;
	if (!ba.write_uint16(this->parasitism_hp)) return false;
	if (!ba.write_uint16(this->zhushang_hp)) return false;
	if (!ba.write_uint16(this->curse_mp)) return false;
	if (!ba.write_uint32(this->nest_akt_list.size())) return false;
	{for(uint32_t i=0; i<this->nest_akt_list.size() ;i++){
		if (!this->nest_akt_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
bt_battle_attack_in::bt_battle_attack_in(){
	this->init();
}
void bt_battle_attack_in::init(){
	this->petid=0;
	this->enemy_mark=0;
	this->enemy_pos=0;
	this->atk_type=0;
	this->atk_level=0;
	this->itemid=0;
	this->add_hp=0;

}
bool  bt_battle_attack_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->enemy_mark)) return false;
	if (!ba.read_int32(this->enemy_pos)) return false;
	if (!ba.read_uint32(this->atk_type)) return false;
	if (!ba.read_uint32(this->atk_level)) return false;
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->add_hp)) return false;
	return true;
}

bool bt_battle_attack_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->enemy_mark)) return false;
	if (!ba.write_int32(this->enemy_pos)) return false;
	if (!ba.write_uint32(this->atk_type)) return false;
	if (!ba.write_uint32(this->atk_level)) return false;
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->add_hp)) return false;
	return true;
}

	
bt_load_battle_in::bt_load_battle_in(){
	this->init();
}
void bt_load_battle_in::init(){
	this->rate=0;

}
bool  bt_load_battle_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->rate)) return false;
	return true;
}

bool bt_load_battle_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->rate)) return false;
	return true;
}

	
bt_load_battle_ready_in::bt_load_battle_ready_in(){
	this->init();
}
void bt_load_battle_ready_in::init(){
	this->ready=0;

}
bool  bt_load_battle_ready_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->ready)) return false;
	return true;
}

bool bt_load_battle_ready_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->ready)) return false;
	return true;
}

	
bt_noti_battle_attacks_info_out::bt_noti_battle_attacks_info_out(){
	this->init();
}
void bt_noti_battle_attacks_info_out::init(){
	this->attack_info_list.clear();

}
bool  bt_noti_battle_attacks_info_out::read_from_buf(byte_array_t & ba ){

	uint32_t attack_info_list_count ;
	if (!ba.read_uint32( attack_info_list_count )) return false;
	attack_info_t  attack_info_list_item;
	this->attack_info_list.clear();
	{for(uint32_t i=0; i<attack_info_list_count;i++){
		if (!attack_info_list_item.read_from_buf(ba)) return false;
		this->attack_info_list.push_back(attack_info_list_item);
	}}
	return true;
}

bool bt_noti_battle_attacks_info_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->attack_info_list.size())) return false;
	{for(uint32_t i=0; i<this->attack_info_list.size() ;i++){
		if (!this->attack_info_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
bt_recall_pet_in::bt_recall_pet_in(){
	this->init();
}
void bt_recall_pet_in::init(){
	this->petid=0;

}
bool  bt_recall_pet_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	return true;
}

bool bt_recall_pet_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	return true;
}

	
bt_use_chemical_in::bt_use_chemical_in(){
	this->init();
}
void bt_use_chemical_in::init(){
	this->petid=0;
	this->enemy_mark=0;
	this->enemy_pos=0;
	this->itemid=0;
	this->add_hp=0;

}
bool  bt_use_chemical_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->enemy_mark)) return false;
	if (!ba.read_int32(this->enemy_pos)) return false;
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->add_hp)) return false;
	return true;
}

bool bt_use_chemical_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->enemy_mark)) return false;
	if (!ba.write_int32(this->enemy_pos)) return false;
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->add_hp)) return false;
	return true;
}

	
catch_pet_op_in::catch_pet_op_in(){
	this->init();
}
void catch_pet_op_in::init(){
	this->mark=0;
	this->pos=0;
	this->itemid=0;
	this->level=0;
	this->data.clear();

}
bool  catch_pet_op_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->mark)) return false;
	if (!ba.read_uint32(this->pos)) return false;
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->level)) return false;

	uint32_t data_count ;
	if (!ba.read_uint32( data_count )) return false;
	uint32_t  data_item;
	this->data.clear();
	{for(uint32_t i=0; i<data_count;i++){
		if (!ba.read_uint32(data_item)) return false;
		this->data.push_back(data_item);
	}}
	return true;
}

bool catch_pet_op_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->mark)) return false;
	if (!ba.write_uint32(this->pos)) return false;
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->data.size())) return false;
	{for(uint32_t i=0; i<this->data.size() ;i++){
		if (!ba.write_uint32(this->data[i])) return false;
	}}
	return true;
}

	
req_challenge_t::req_challenge_t(){
	this->init();
}
void req_challenge_t::init(){
	this->teamid=0;
	this->win_cnt=0;
	this->mode=0;
	this->warriors.clear();

}
bool  req_challenge_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->teamid)) return false;
	if (!ba.read_uint32(this->win_cnt)) return false;
	if (!ba.read_uint32(this->mode)) return false;

	uint32_t warriors_count ;
	if (!ba.read_uint32( warriors_count )) return false;
	stru_warrior_t  warriors_item;
	this->warriors.clear();
	{for(uint32_t i=0; i<warriors_count;i++){
		if (!warriors_item.read_from_buf(ba)) return false;
		this->warriors.push_back(warriors_item);
	}}
	return true;
}

bool req_challenge_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->teamid)) return false;
	if (!ba.write_uint32(this->win_cnt)) return false;
	if (!ba.write_uint32(this->mode)) return false;
	if (!ba.write_uint32(this->warriors.size())) return false;
	{for(uint32_t i=0; i<this->warriors.size() ;i++){
		if (!this->warriors[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	
challenge_battle_op_in::challenge_battle_op_in(){
	this->init();
}
void challenge_battle_op_in::init(){
	this->req_challenge_info.init();

}
bool  challenge_battle_op_in::read_from_buf(byte_array_t & ba ){
	if (!this->req_challenge_info.read_from_buf(ba)) return false;
	return true;
}

bool challenge_battle_op_in::write_to_buf(byte_array_t & ba ){
	if (!this->req_challenge_info.write_to_buf(ba)) return false;
	return true;
}

	
challenge_npc_battle_op_in::challenge_npc_battle_op_in(){
	this->init();
}
void challenge_npc_battle_op_in::init(){
	this->req_challenge_info.init();

}
bool  challenge_npc_battle_op_in::read_from_buf(byte_array_t & ba ){
	if (!this->req_challenge_info.read_from_buf(ba)) return false;
	return true;
}

bool challenge_npc_battle_op_in::write_to_buf(byte_array_t & ba ){
	if (!this->req_challenge_info.write_to_buf(ba)) return false;
	return true;
}

	
stru_bt_atk_back::stru_bt_atk_back(){
	this->init();
}
void stru_bt_atk_back::init(){
	this->userid=0;
	this->petid=0;
	this->skill_id=0;
	this->skill_lvl=0;
	this->atk_userid=0;
	this->atk_petid=0;

}
bool  stru_bt_atk_back::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->skill_id)) return false;
	if (!ba.read_uint32(this->skill_lvl)) return false;
	if (!ba.read_uint32(this->atk_userid)) return false;
	if (!ba.read_uint32(this->atk_petid)) return false;
	return true;
}

bool stru_bt_atk_back::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->skill_id)) return false;
	if (!ba.write_uint32(this->skill_lvl)) return false;
	if (!ba.write_uint32(this->atk_userid)) return false;
	if (!ba.write_uint32(this->atk_petid)) return false;
	return true;
}

	
stru_bt_atk_list::stru_bt_atk_list(){
	this->init();
}
void stru_bt_atk_list::init(){
	this->seq=0;
	this->userid=0;
	this->petid=0;
	this->topic_id=0;

}
bool  stru_bt_atk_list::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->seq)) return false;
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->topic_id)) return false;
	return true;
}

bool stru_bt_atk_list::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->seq)) return false;
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->topic_id)) return false;
	return true;
}

	
stru_bt_kvalue::stru_bt_kvalue(){
	this->init();
}
void stru_bt_kvalue::init(){
	this->key=0;
	this->value=0;

}
bool  stru_bt_kvalue::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint16(this->key)) return false;
	if (!ba.read_int16(this->value)) return false;
	return true;
}

bool stru_bt_kvalue::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint16(this->key)) return false;
	if (!ba.write_int16(this->value)) return false;
	return true;
}

	
stru_bt_atk_pkt::stru_bt_atk_pkt(){
	this->init();
}
void stru_bt_atk_pkt::init(){
	this->seq=0;
	this->userid=0;
	this->petid=0;
	this->topic=0;
	this->state=0;
	this->changes.clear();

}
bool  stru_bt_atk_pkt::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->seq)) return false;
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->topic)) return false;
	if (!ba.read_uint64(this->state)) return false;

	uint32_t changes_count ;
	if (!ba.read_uint32( changes_count )) return false;
	stru_bt_kvalue  changes_item;
	this->changes.clear();
	{for(uint32_t i=0; i<changes_count;i++){
		if (!changes_item.read_from_buf(ba)) return false;
		this->changes.push_back(changes_item);
	}}
	return true;
}

bool stru_bt_atk_pkt::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->seq)) return false;
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->topic)) return false;
	if (!ba.write_uint64(this->state)) return false;
	if (!ba.write_uint32(this->changes.size())) return false;
	{for(uint32_t i=0; i<this->changes.size() ;i++){
		if (!this->changes[i].write_to_buf(ba)) return false;
	}}
	return true;
}

	