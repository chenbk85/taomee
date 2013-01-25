#include "mole2.h"

stru_relation_item::stru_relation_item(){
	this->init();
}
void stru_relation_item::init(){
	this->uid=0;
	this->day=0;
	this->val=0;

}
bool  stru_relation_item::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->day)) return false;
	if (!ba.read_uint32(this->val)) return false;
	return true;
}

bool stru_relation_item::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->day)) return false;
	if (!ba.write_uint32(this->val)) return false;
	return true;
}

	
stru_cli_buff::stru_cli_buff(){
	this->init();
}
void stru_cli_buff::init(){
	this->type=0;

}
bool  stru_cli_buff::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->type)) return false;
	if(!ba.read_buf(this->buff,512)) return false;
	return true;
}

bool stru_cli_buff::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->type)) return false;
	if(!ba.write_buf(this->buff,512)) return false;
	return true;
}

	
stru_rank_val::stru_rank_val(){
	this->init();
}
void stru_rank_val::init(){
	this->id=0;
	this->val=0;

}
bool  stru_rank_val::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->id)) return false;
	if (!ba.read_uint32(this->val)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool stru_rank_val::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->id)) return false;
	if (!ba.write_uint32(this->val)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}

	
stru_team_rank_t::stru_team_rank_t(){
	this->init();
}
void stru_team_rank_t::init(){
	this->teamid=0;
	this->count=0;
	this->medals=0;

}
bool  stru_team_rank_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->teamid)) return false;
	if (!ba.read_uint32(this->count)) return false;
	if (!ba.read_uint32(this->medals)) return false;
	return true;
}

bool stru_team_rank_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->teamid)) return false;
	if (!ba.write_uint32(this->count)) return false;
	if (!ba.write_uint32(this->medals)) return false;
	return true;
}

	
stru_user_rank_t::stru_user_rank_t(){
	this->init();
}
void stru_user_rank_t::init(){
	this->userid=0;
	this->teamid=0;
	this->medals=0;

}
bool  stru_user_rank_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->teamid)) return false;
	if (!ba.read_uint32(this->medals)) return false;
	return true;
}

bool stru_user_rank_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->teamid)) return false;
	if (!ba.write_uint32(this->medals)) return false;
	return true;
}

	
stru_ex_item::stru_ex_item(){
	this->init();
}
void stru_ex_item::init(){
	this->eid=0;
	this->itemid=0;

}
bool  stru_ex_item::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->eid)) return false;
	if (!ba.read_uint32(this->itemid)) return false;
	return true;
}

bool stru_ex_item::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->eid)) return false;
	if (!ba.write_uint32(this->itemid)) return false;
	return true;
}

	
stru_id_cnt::stru_id_cnt(){
	this->init();
}
void stru_id_cnt::init(){
	this->id=0;
	this->count=0;

}
bool  stru_id_cnt::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->id)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool stru_id_cnt::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->id)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
stru_petfight_rank_t::stru_petfight_rank_t(){
	this->init();
}
void stru_petfight_rank_t::init(){
	this->userid=0;
	this->scores=0;

}
bool  stru_petfight_rank_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->scores)) return false;
	return true;
}

bool stru_petfight_rank_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->scores)) return false;
	return true;
}

	
map_grp_item_t::map_grp_item_t(){
	this->init();
}
void map_grp_item_t::init(){
	this->refresh_id=0;
	this->grpid=0;
	this->type_id=0;
	this->posx=0;
	this->posy=0;
	this->count=0;

}
bool  map_grp_item_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->refresh_id)) return false;
	if (!ba.read_uint32(this->grpid)) return false;
	if (!ba.read_uint32(this->type_id)) return false;
	if (!ba.read_uint16(this->posx)) return false;
	if (!ba.read_uint16(this->posy)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool map_grp_item_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->refresh_id)) return false;
	if (!ba.write_uint32(this->grpid)) return false;
	if (!ba.write_uint32(this->type_id)) return false;
	if (!ba.write_uint16(this->posx)) return false;
	if (!ba.write_uint16(this->posy)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}

	
pvp_score_t::pvp_score_t(){
	this->init();
}
void pvp_score_t::init(){
	this->userid=0;
	this->scores=0;
	this->is_alive=0;

}
bool  pvp_score_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->userid)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->scores)) return false;
	if (!ba.read_uint32(this->is_alive)) return false;
	return true;
}

bool pvp_score_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->userid)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->scores)) return false;
	if (!ba.write_uint32(this->is_alive)) return false;
	return true;
}

	
stru_body_cloth::stru_body_cloth(){
	this->init();
}
void stru_body_cloth::init(){
	this->clothid=0;
	this->clothtype=0;
	this->grid=0;
	this->clothlv=0;
	this->duration_max=0;
	this->duration=0;
	this->hpmax=0;
	this->mpmax=0;
	this->attack=0;
	this->mattack=0;
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
	this->blesstype=0;

}
bool  stru_body_cloth::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->clothid)) return false;
	if (!ba.read_uint32(this->clothtype)) return false;
	if (!ba.read_uint32(this->grid)) return false;
	if (!ba.read_uint16(this->clothlv)) return false;
	if (!ba.read_uint16(this->duration_max)) return false;
	if (!ba.read_uint16(this->duration)) return false;
	if (!ba.read_uint32(this->hpmax)) return false;
	if (!ba.read_uint32(this->mpmax)) return false;
	if (!ba.read_uint16(this->attack)) return false;
	if (!ba.read_uint16(this->mattack)) return false;
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
	if (!ba.read_uint32(this->blesstype)) return false;
	return true;
}

bool stru_body_cloth::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->clothid)) return false;
	if (!ba.write_uint32(this->clothtype)) return false;
	if (!ba.write_uint32(this->grid)) return false;
	if (!ba.write_uint16(this->clothlv)) return false;
	if (!ba.write_uint16(this->duration_max)) return false;
	if (!ba.write_uint16(this->duration)) return false;
	if (!ba.write_uint32(this->hpmax)) return false;
	if (!ba.write_uint32(this->mpmax)) return false;
	if (!ba.write_uint16(this->attack)) return false;
	if (!ba.write_uint16(this->mattack)) return false;
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
	if (!ba.write_uint32(this->blesstype)) return false;
	return true;
}

	
stru_game_change_add::stru_game_change_add(){
	this->init();
}
void stru_game_change_add::init(){
	this->gameid=0;
	this->userid=0;
	this->logtime=0;
	this->opt_type=0;
	this->ex_v1=0;
	this->ex_v2=0;
	this->ex_v3=0;
	this->ex_v4=0;

}
bool  stru_game_change_add::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->opt_type)) return false;
	if (!ba.read_int32(this->ex_v1)) return false;
	if (!ba.read_int32(this->ex_v2)) return false;
	if (!ba.read_int32(this->ex_v3)) return false;
	if (!ba.read_int32(this->ex_v4)) return false;
	return true;
}

bool stru_game_change_add::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->opt_type)) return false;
	if (!ba.write_int32(this->ex_v1)) return false;
	if (!ba.write_int32(this->ex_v2)) return false;
	if (!ba.write_int32(this->ex_v3)) return false;
	if (!ba.write_int32(this->ex_v4)) return false;
	return true;
}

	
stru_trade_cloth::stru_trade_cloth(){
	this->init();
}
void stru_trade_cloth::init(){
	this->tradeid=0;
	this->clothid=0;
	this->clothtype=0;
	this->price=0;
	this->tax=0;
	this->attr.init();

}
bool  stru_trade_cloth::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->tradeid)) return false;
	if (!ba.read_uint32(this->clothid)) return false;
	if (!ba.read_uint32(this->clothtype)) return false;
	if (!ba.read_uint32(this->price)) return false;
	if (!ba.read_uint32(this->tax)) return false;
	if (!this->attr.read_from_buf(ba)) return false;
	return true;
}

bool stru_trade_cloth::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->tradeid)) return false;
	if (!ba.write_uint32(this->clothid)) return false;
	if (!ba.write_uint32(this->clothtype)) return false;
	if (!ba.write_uint32(this->price)) return false;
	if (!ba.write_uint32(this->tax)) return false;
	if (!this->attr.write_to_buf(ba)) return false;
	return true;
}

	
stru_trade_cloth_new::stru_trade_cloth_new(){
	this->init();
}
void stru_trade_cloth_new::init(){
	this->tradeid=0;
	this->clothid=0;
	this->clothtype=0;
	this->price=0;
	this->tax=0;

}
bool  stru_trade_cloth_new::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->tradeid)) return false;
	if (!ba.read_uint32(this->clothid)) return false;
	if (!ba.read_uint32(this->clothtype)) return false;
	if (!ba.read_uint32(this->price)) return false;
	if (!ba.read_uint32(this->tax)) return false;
	return true;
}

bool stru_trade_cloth_new::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->tradeid)) return false;
	if (!ba.write_uint32(this->clothid)) return false;
	if (!ba.write_uint32(this->clothtype)) return false;
	if (!ba.write_uint32(this->price)) return false;
	if (!ba.write_uint32(this->tax)) return false;
	return true;
}

	
stru_trade_item::stru_trade_item(){
	this->init();
}
void stru_trade_item::init(){
	this->tradeid=0;
	this->itemid=0;
	this->itemcnt=0;
	this->price=0;
	this->tax=0;

}
bool  stru_trade_item::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->tradeid)) return false;
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->itemcnt)) return false;
	if (!ba.read_uint32(this->price)) return false;
	if (!ba.read_uint32(this->tax)) return false;
	return true;
}

bool stru_trade_item::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->tradeid)) return false;
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->itemcnt)) return false;
	if (!ba.write_uint32(this->price)) return false;
	if (!ba.write_uint32(this->tax)) return false;
	return true;
}

	
stru_trade_record::stru_trade_record(){
	this->init();
}
void stru_trade_record::init(){
	this->itemid=0;
	this->itemcnt=0;
	this->price=0;
	this->tax=0;
	this->opt_time=0;

}
bool  stru_trade_record::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->itemcnt)) return false;
	if (!ba.read_uint32(this->price)) return false;
	if (!ba.read_uint32(this->tax)) return false;
	if (!ba.read_uint32(this->opt_time)) return false;
	return true;
}

bool stru_trade_record::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->itemcnt)) return false;
	if (!ba.write_uint32(this->price)) return false;
	if (!ba.write_uint32(this->tax)) return false;
	if (!ba.write_uint32(this->opt_time)) return false;
	return true;
}

	
stru_trade_shop::stru_trade_shop(){
	this->init();
}
void stru_trade_shop::init(){
	this->state=0;
	this->uid=0;
	this->prof=0;

}
bool  stru_trade_shop::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->state)) return false;
	if (!ba.read_uint32(this->uid)) return false;
	if (!ba.read_uint32(this->prof)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool stru_trade_shop::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->state)) return false;
	if (!ba.write_uint32(this->uid)) return false;
	if (!ba.write_uint32(this->prof)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}

	
stru_warrior_cloth::stru_warrior_cloth(){
	this->init();
}
void stru_warrior_cloth::init(){
	this->cloth_id=0;
	this->type_id=0;
	this->clevel=0;
	this->duration=0;
	this->equip_pos=0;

}
bool  stru_warrior_cloth::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->cloth_id)) return false;
	if (!ba.read_uint32(this->type_id)) return false;
	if (!ba.read_uint16(this->clevel)) return false;
	if (!ba.read_uint16(this->duration)) return false;
	if (!ba.read_uint8(this->equip_pos)) return false;
	return true;
}

bool stru_warrior_cloth::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->cloth_id)) return false;
	if (!ba.write_uint32(this->type_id)) return false;
	if (!ba.write_uint16(this->clevel)) return false;
	if (!ba.write_uint16(this->duration)) return false;
	if (!ba.write_uint8(this->equip_pos)) return false;
	return true;
}

	
stru_warrior_skill::stru_warrior_skill(){
	this->init();
}
void stru_warrior_skill::init(){
	this->skill_id=0;
	this->skill_level=0;
	this->use_level=0;
	this->skill_exp=0;
	this->rand_rate=0;
	this->cool_round=0;
	this->cool_update=0;

}
bool  stru_warrior_skill::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->skill_id)) return false;
	if (!ba.read_uint8(this->skill_level)) return false;
	if (!ba.read_uint8(this->use_level)) return false;
	if (!ba.read_uint32(this->skill_exp)) return false;
	if (!ba.read_uint16(this->rand_rate)) return false;
	if (!ba.read_uint8(this->cool_round)) return false;
	if (!ba.read_uint8(this->cool_update)) return false;
	return true;
}

bool stru_warrior_skill::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->skill_id)) return false;
	if (!ba.write_uint8(this->skill_level)) return false;
	if (!ba.write_uint8(this->use_level)) return false;
	if (!ba.write_uint32(this->skill_exp)) return false;
	if (!ba.write_uint16(this->rand_rate)) return false;
	if (!ba.write_uint8(this->cool_round)) return false;
	if (!ba.write_uint8(this->cool_update)) return false;
	return true;
}

	
stru_warrior_t::stru_warrior_t(){
	this->init();
}
void stru_warrior_t::init(){
	this->userid=0;
	this->petid=0;
	this->type_id=0;
	this->flag=0;
	this->viplv=0;
	this->color=0;
	this->race=0;
	this->prof=0;
	this->level=0;
	this->physique=0;
	this->strength=0;
	this->endurance=0;
	this->quick=0;
	this->intelligence=0;
	this->attr_addition=0;
	this->hp=0;
	this->mp=0;
	this->injury_lv=0;
	this->in_front=0;
	this->earth=0;
	this->water=0;
	this->fire=0;
	this->wind=0;
	this->hp_max=0;
	this->mp_max=0;
	this->attack=0;
	this->mattack=0;
	this->defense=0;
	this->mdefense=0;
	this->speed=0;
	this->spirit=0;
	this->resume=0;
	this->hit_rate=0;
	this->avoid_rate=0;
	this->bisha=0;
	this->fight_back=0;
	this->rpoison=0;
	this->rlithification=0;
	this->rlethargy=0;
	this->rinebriation=0;
	this->rconfusion=0;
	this->roblivion=0;
	this->need_protect_exp=0;
	this->cloth_cnt=0;
	{for(uint32_t i=0; i<8;i++){
		this->clothes[i].init();
	}}
	this->skill_cnt=0;
	{for(uint32_t i=0; i<10;i++){
		this->skills[i].init();
	}}
	this->weapon_type=0;
	this->shield=0;
	this->pet_cnt_actual=0;
	this->catchable=0;
	this->handbooklv=0;
	this->pet_contact_lv=0;
	this->pet_state=0;
	{for(uint32_t i=0; i<2;i++){
		this->default_atk[i]=0;
	}}
	{for(uint32_t i=0; i<2;i++){
		this->default_atklv[i]=0;
	}}
	this->autofight_count=0;
	this->changerace=0;
	this->changepetid=0;

}
bool  stru_warrior_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->petid)) return false;
	if (!ba.read_uint32(this->type_id)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->flag)) return false;
	if (!ba.read_uint32(this->viplv)) return false;
	if (!ba.read_uint32(this->color)) return false;
	if (!ba.read_uint8(this->race)) return false;
	if (!ba.read_uint8(this->prof)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint16(this->physique)) return false;
	if (!ba.read_uint16(this->strength)) return false;
	if (!ba.read_uint16(this->endurance)) return false;
	if (!ba.read_uint16(this->quick)) return false;
	if (!ba.read_uint16(this->intelligence)) return false;
	if (!ba.read_uint16(this->attr_addition)) return false;
	if (!ba.read_uint32(this->hp)) return false;
	if (!ba.read_uint32(this->mp)) return false;
	if (!ba.read_uint32(this->injury_lv)) return false;
	if (!ba.read_uint8(this->in_front)) return false;
	if (!ba.read_uint8(this->earth)) return false;
	if (!ba.read_uint8(this->water)) return false;
	if (!ba.read_uint8(this->fire)) return false;
	if (!ba.read_uint8(this->wind)) return false;
	if (!ba.read_uint32(this->hp_max)) return false;
	if (!ba.read_uint32(this->mp_max)) return false;
	if (!ba.read_int16(this->attack)) return false;
	if (!ba.read_int16(this->mattack)) return false;
	if (!ba.read_int16(this->defense)) return false;
	if (!ba.read_int16(this->mdefense)) return false;
	if (!ba.read_int16(this->speed)) return false;
	if (!ba.read_int16(this->spirit)) return false;
	if (!ba.read_int16(this->resume)) return false;
	if (!ba.read_int16(this->hit_rate)) return false;
	if (!ba.read_int16(this->avoid_rate)) return false;
	if (!ba.read_int16(this->bisha)) return false;
	if (!ba.read_int16(this->fight_back)) return false;
	if (!ba.read_int16(this->rpoison)) return false;
	if (!ba.read_int16(this->rlithification)) return false;
	if (!ba.read_int16(this->rlethargy)) return false;
	if (!ba.read_int16(this->rinebriation)) return false;
	if (!ba.read_int16(this->rconfusion)) return false;
	if (!ba.read_int16(this->roblivion)) return false;
	if (!ba.read_uint8(this->need_protect_exp)) return false;
	if (!ba.read_uint8(this->cloth_cnt)) return false;
	{for(uint32_t i=0; i<8;i++){
		if (!this->clothes[i].read_from_buf(ba)) return false;
	}}
	if (!ba.read_uint8(this->skill_cnt)) return false;
	{for(uint32_t i=0; i<10;i++){
		if (!this->skills[i].read_from_buf(ba)) return false;
	}}
	if (!ba.read_uint32(this->weapon_type)) return false;
	if (!ba.read_uint32(this->shield)) return false;
	if (!ba.read_uint8(this->pet_cnt_actual)) return false;
	if (!ba.read_uint16(this->catchable)) return false;
	if (!ba.read_uint32(this->handbooklv)) return false;
	if (!ba.read_uint32(this->pet_contact_lv)) return false;
	if (!ba.read_uint8(this->pet_state)) return false;
	{for(uint32_t i=0; i<2;i++){
		if (!ba.read_uint32(this->default_atk[i])) return false;
	}}
	{for(uint32_t i=0; i<2;i++){
		if (!ba.read_uint8(this->default_atklv[i])) return false;
	}}
	if (!ba.read_uint32(this->autofight_count)) return false;
	if (!ba.read_uint32(this->changerace)) return false;
	if (!ba.read_uint32(this->changepetid)) return false;
	return true;
}

bool stru_warrior_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->petid)) return false;
	if (!ba.write_uint32(this->type_id)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->flag)) return false;
	if (!ba.write_uint32(this->viplv)) return false;
	if (!ba.write_uint32(this->color)) return false;
	if (!ba.write_uint8(this->race)) return false;
	if (!ba.write_uint8(this->prof)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint16(this->physique)) return false;
	if (!ba.write_uint16(this->strength)) return false;
	if (!ba.write_uint16(this->endurance)) return false;
	if (!ba.write_uint16(this->quick)) return false;
	if (!ba.write_uint16(this->intelligence)) return false;
	if (!ba.write_uint16(this->attr_addition)) return false;
	if (!ba.write_uint32(this->hp)) return false;
	if (!ba.write_uint32(this->mp)) return false;
	if (!ba.write_uint32(this->injury_lv)) return false;
	if (!ba.write_uint8(this->in_front)) return false;
	if (!ba.write_uint8(this->earth)) return false;
	if (!ba.write_uint8(this->water)) return false;
	if (!ba.write_uint8(this->fire)) return false;
	if (!ba.write_uint8(this->wind)) return false;
	if (!ba.write_uint32(this->hp_max)) return false;
	if (!ba.write_uint32(this->mp_max)) return false;
	if (!ba.write_int16(this->attack)) return false;
	if (!ba.write_int16(this->mattack)) return false;
	if (!ba.write_int16(this->defense)) return false;
	if (!ba.write_int16(this->mdefense)) return false;
	if (!ba.write_int16(this->speed)) return false;
	if (!ba.write_int16(this->spirit)) return false;
	if (!ba.write_int16(this->resume)) return false;
	if (!ba.write_int16(this->hit_rate)) return false;
	if (!ba.write_int16(this->avoid_rate)) return false;
	if (!ba.write_int16(this->bisha)) return false;
	if (!ba.write_int16(this->fight_back)) return false;
	if (!ba.write_int16(this->rpoison)) return false;
	if (!ba.write_int16(this->rlithification)) return false;
	if (!ba.write_int16(this->rlethargy)) return false;
	if (!ba.write_int16(this->rinebriation)) return false;
	if (!ba.write_int16(this->rconfusion)) return false;
	if (!ba.write_int16(this->roblivion)) return false;
	if (!ba.write_uint8(this->need_protect_exp)) return false;
	if (!ba.write_uint8(this->cloth_cnt)) return false;
	{for(uint32_t i=0; i<8;i++){
		if (!this->clothes[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint8(this->skill_cnt)) return false;
	{for(uint32_t i=0; i<10;i++){
		if (!this->skills[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->weapon_type)) return false;
	if (!ba.write_uint32(this->shield)) return false;
	if (!ba.write_uint8(this->pet_cnt_actual)) return false;
	if (!ba.write_uint16(this->catchable)) return false;
	if (!ba.write_uint32(this->handbooklv)) return false;
	if (!ba.write_uint32(this->pet_contact_lv)) return false;
	if (!ba.write_uint8(this->pet_state)) return false;
	{for(uint32_t i=0; i<2;i++){
		if (!ba.write_uint32(this->default_atk[i])) return false;
	}}
	{for(uint32_t i=0; i<2;i++){
		if (!ba.write_uint8(this->default_atklv[i])) return false;
	}}
	if (!ba.write_uint32(this->autofight_count)) return false;
	if (!ba.write_uint32(this->changerace)) return false;
	if (!ba.write_uint32(this->changepetid)) return false;
	return true;
}

	