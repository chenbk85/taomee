#include  "Cwarrior_state.h"

extern "C"
{
#include <libtaomee/log.h>
}
#include  <stdlib.h>



#define SET_WARRIOR_STATE(state, state_bit)		((state) |= ((uint64_t)1 << (state_bit)))
#define RESET_WARRIOR_STATE(state, state_bit)	((state) &= (~((uint64_t)1 << (state_bit))))
#define CHK_WARRIOR_STATE(state, state_bit)		((state) & ((uint64_t)1 << (state_bit)))


void  Cwarrior_state::set_state_ex_info( uint8_t state_bit, Cwarrior_state_ex_info_base * p_ex_info )
{
	std::map<int8_t,Cwarrior_state_ex_info_base * >::iterator it; 

	it=this->state_info_map.find(state_bit);
	if (it!= this->state_info_map.end()) {//找到
		//释放原有对象
		delete it->second;
		it->second=p_ex_info;
	}else{//没有找到
		this->state_info_map[state_bit]=p_ex_info;
	}


}

Cwarrior_state_ex_info_base * Cwarrior_state::get_state_ex_info (uint8_t state_bit )
{
	std::map<int8_t,Cwarrior_state_ex_info_base * >::iterator it; 
	it=this->state_info_map.find(state_bit);
	if (it!= this->state_info_map.end()) {
		return it->second;
	}else{
		return NULL;
	}


}

//void Cwarrior_state::clear_debuffs_state_info(uint8_t state_bit  )
//{
	//RESET_WARRIOR_STATE(this->state, state_bit );
	//this->effect_cnt[state_bit] = 0;
	//this->round_cnt[state_bit] = 0;
//}

bool Cwarrior_state::set_state_info(uint8_t state_bit,int8_t effect_count,int8_t round_count ){
	if( not_at_time_state_bit & ((uint64_t)1 << state_bit) ){//是"不可叠加状态"
		if (this->state&  not_at_time_state_bit  ){//已经处于"不可叠加状态"
			return false;
		}
	}


	SET_WARRIOR_STATE(this->state, state_bit );
	this->effect_cnt[state_bit] = effect_count;
	this->round_cnt[state_bit] =round_count;
	return true;
}

void Cwarrior_state::unset_state_info(uint8_t state_bit  )
{
	RESET_WARRIOR_STATE(this->state, state_bit );
	this->effect_cnt[state_bit] = 0;
	this->round_cnt[state_bit] = 0;
}



bool Cwarrior_state::check_state(uint8_t state_bit)
{
	return CHK_WARRIOR_STATE( this->state, state_bit ) ; 
}


uint32_t Cwarrior_state::get_state_ex_info_uint32( uint8_t state_bit )
{
	Cwarrior_state_ex_uint32 *p_item;
	p_item=(Cwarrior_state_ex_uint32 * )(this->get_state_ex_info(state_bit ));
	if ( !p_item){
		return  0;
	}else{
		if (p_item->state_bit==state_bit ){
			return  p_item->value;
		}else{
			DEBUG_LOG("ERROR p_item->state_bit!(%u)=state_bit(%u)",p_item->state_bit,state_bit);
			return  0;
		}
	}
}


void  Cwarrior_state::set_state_ex_info_uint32( uint8_t state_bit , uint32_t value )
{
	if (value!=0 ){
		Cwarrior_state_ex_uint32 *p_item=new Cwarrior_state_ex_uint32();
		p_item->state_bit=state_bit;
		p_item->value=value;
		this->set_state_ex_info( state_bit, p_item);
	}else{//清空原有数据
		std::map<int8_t,Cwarrior_state_ex_info_base * >::iterator it; 
		it=this->state_info_map.find(state_bit);
		if (it!= this->state_info_map.end()) {//找到
			//释放原有对象
			delete it->second;
			this->state_info_map.erase(it);
		}
	}
}

Cwarrior_state::~Cwarrior_state(){
	std::map<int8_t,Cwarrior_state_ex_info_base * >::iterator it; 
	for( it=this->state_info_map.begin() ; it!=this->state_info_map.end() ; ++it ){
		delete it->second;
	}
}

uint8_t Cwarrior_state::set_rand_state_info( uint8_t *p_state_bit_list, uint32_t  state_bit_count,
			int rand_value	,int8_t effect_count,int8_t round_count )
{
	if (rand()%1000<=rand_value*10 ){
		uint8_t state_bit=p_state_bit_list[rand()%state_bit_count];
		if(this->set_state_info( state_bit,effect_count, round_count  )){
			return state_bit;
		}else{
			return 0;
		}
	}else{//没有设置
		return 0;
	}
}

uint8_t Cwarrior_state::set_some_state_info( uint8_t state_bit, int rand_value	,int8_t effect_count,int8_t round_count )
{
	if (rand()%1000<=rand_value*10){
		if(this->set_state_info( state_bit,effect_count, round_count)){
			//DEBUG_LOG("set state succ");
			return state_bit;
		}else{
			return 0;
		}
	}else{//没有设置
		return 0;
	}
}

int8_t  Cwarrior_state::change_round_cnt(uint8_t state_bit ,int8_t value){
	if (state_bit >=64) return 0;

	if (value!=0 ){
		this->round_cnt[state_bit]+=value;
	}
	if (this->round_cnt[state_bit]<0 )  {
		this->round_cnt[state_bit]=0;
	}
	return round_cnt[state_bit];
}
int8_t  Cwarrior_state::get_round_cnt(uint8_t state_bit ){
	if (state_bit >=64) return 0;
	return round_cnt[state_bit];
}
int8_t  Cwarrior_state::get_effect_cnt(uint8_t state_bit ){
	if (state_bit >=64) return 0;
	return effect_cnt[state_bit];
}

int8_t  Cwarrior_state::change_effect_cnt(uint8_t state_bit ,int8_t value){
	if (state_bit >=64) return 0;
	if (value!=0 ){
		this->effect_cnt[state_bit]+=value;
	}
	if (this->effect_cnt[state_bit]<0 )  {
		this->effect_cnt[state_bit]=0;
	}
	return effect_cnt[state_bit];
}


uint32_t Cwarrior_state::get_qiliaodun_level ( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_QILIAODUN_LEVEL);
}
void Cwarrior_state::set_qiliaodun_level (uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_QILIAODUN_LEVEL,value);
}

int8_t Cwarrior_state::get_muyedun_level ( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_MUYEDUN_LEVEL);
}
void Cwarrior_state::set_muyedun_level (uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_MUYEDUN_LEVEL,value);
}

int8_t  Cwarrior_state::get_huwei_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_HUWEI_LEVEL);
}
void Cwarrior_state::set_huwei_level(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_HUWEI_LEVEL,value);
}
int8_t  Cwarrior_state::get_hundun_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_HUNDUN_LEVEL );
}
void Cwarrior_state::set_hundun_level(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_HUNDUN_LEVEL,value);
}

int8_t  Cwarrior_state::get_resume_hp1_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_RESUME_HP1_LEVEL);
}
void Cwarrior_state::set_resume_hp1_level(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_RESUME_HP1_LEVEL ,value);
}
int8_t  Cwarrior_state::get_resume_hp2_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_RESUME_HP2_LEVEL);
}
void Cwarrior_state::set_resume_hp2_level(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_RESUME_HP2_LEVEL ,value);
}
int8_t  Cwarrior_state::get_resume_hp3_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_RESUME_HP3_LEVEL);
}
void Cwarrior_state::set_resume_hp3_level(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_RESUME_HP3_LEVEL ,value);
}

int8_t  Cwarrior_state::get_jinghunquan_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_JINGHUNQUAN_LEVEL);
}
void  Cwarrior_state::set_jinghunquan_level(uint32_t value){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_JINGHUNQUAN_LEVEL,value);
}

int8_t  Cwarrior_state::get_jintouquan_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_JINTOUQUAN_LEVEL);
}
void  Cwarrior_state::set_jintouquan_level(uint32_t value){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_JINTOUQUAN_LEVEL,value);
}

uint32_t  Cwarrior_state::get_fanghuzhao_value( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_FANGHUZHAO_VALUE);
}
void Cwarrior_state::set_fanghuzhao_value(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_FANGHUZHAO_VALUE,value);
}


int8_t  Cwarrior_state::get_resume_hp4_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_RESUME_HP4_LEVEL);
}
void Cwarrior_state::set_resume_hp4_level(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_RESUME_HP4_LEVEL ,value);
}


int8_t  Cwarrior_state::get_meiyin_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_MEIYIN_LEVEL);
}
void Cwarrior_state::set_meiyin_level(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_MEIYIN_LEVEL,value);
}





uint32_t  Cwarrior_state::get_host_uid( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_HOST_ID);
}
void Cwarrior_state::set_host_uid(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_HOST_ID,value);
}

uint32_t  Cwarrior_state::get_host_petid( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_HOST_PETID);
}
void Cwarrior_state::set_host_petid(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_HOST_PETID,value);
}

uint32_t  Cwarrior_state::get_use_item_itemid ( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_USE_ITEM_ITEMID );
}
void Cwarrior_state::set_use_item_itemid (uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_USE_ITEM_ITEMID,value);
}

int16_t  Cwarrior_state::get_use_item_add_hp ( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_USE_ITEM_ADD_HP );
}
void Cwarrior_state::set_use_item_add_hp (uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_USE_ITEM_ADD_HP,value);
}

//万兽噬身-野兽
//对目标发动无属性魔法并留下魔法印记，被标记的目标受到野兽系攻击时伤害增加n%，效果持续2回合

uint32_t  Cwarrior_state::get_yeshou_hurt_hp_percent( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_YESHOU_HURT_HP_PERCENT );
}

void Cwarrior_state::set_yeshou_hurt_hp_percent(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_YESHOU_HURT_HP_PERCENT,value);
}
uint32_t  Cwarrior_state::get_mifanquan_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_MIFANQUAN_LEVEL);
}

void Cwarrior_state::set_mifanquan_level(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_MIFANQUAN_LEVEL,value);
}

uint32_t  Cwarrior_state::get_jueqiquan_level( ){
	return this->get_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_JUEQIQUAN_LEVEL);
}

void Cwarrior_state::set_jueqiquan_level(uint32_t value ){
	return this->set_state_ex_info_uint32(WARRIOR_STATE_EX_BIT_JUEQIQUAN_LEVEL,value);
}


Cwarrior_state::Cwarrior_state(){
	//DEBUG_LOG("init state:%lu,one_loop_state:%lu ",state,one_loop_state );
	state=0;
	one_loop_state=0;
}
