#ifdef __cplusplus
extern "C"
{
#endif

#include <fcntl.h>
#include <sys/mman.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
#include "libtaomee/project/stat_agent/msglog.h"
#include "utils.h"
/**************lua include file**********************/
#include <stdio.h>
#include <math.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
/**************lua include file**********************/

#ifdef __cplusplus
}
#endif

#include <map>
#include <vector>
#include "bt_skill.h"
#include "bt_bat_check.h"
#include "global.h"
#include "bt_beastai.h"
#include "beast_ai_api.h"
//HP＞50%，物理型宠根据是否拥有以下技能、是否MP足够、是否遗忘技能或指令，决定使用技能的优先级为：狂击（5106）、毁伤（5108）、追击（5102）、普通攻击（10）、防御、待机（5008）
//HP≤50%，物理型宠根据是否拥有以下技能、是否MP足够、是否遗忘技能或指令，决定使用技能的优先级为：吸血攻击（8701）、狂击（5106）、毁伤（5108）、追击（5102）、普通攻击（10）、防御、待机（5008）
//HP＞50%，魔法型宠根据是否拥有以下技能、是否MP足够、是否遗忘技能或指令，决定使用技能的优先级为：陨石术（7001）、霜冻术（7002）、流火术（7003）、旋风术（7004）、普通攻击（5010）、防御、待机（5008）
//HP≤50%，魔法型宠根据是否拥有以下技能、是否MP足够、是否遗忘技能或指令，决定使用技能的优先级为：冥思（6001）、陨石术（7001）、霜冻术（7002）、流火术（7003）、旋风术（7004）、普通攻击（5010）、防御、待机（5008）

skill_id_level_t get_auto_pk_atk_for_pet(warrior_t * ppet)
{
//p->mp
//p->check_mp_enough	
//get_beast_skill_by_atktype
	uint32_t skill_arr[][5]={
		{5106,5108,5102,0,0},
		{8701,5106,5108,5102,0},
		{7001,7002,7003,7004,0 },
		{6001,7001,7002,7003,7004}
	};	
	uint8_t type=0;
	type += ppet->hp_max/5 < ppet->mp_max ? 2:0;
	type += ppet->hp_max >= ppet->hp*2 ? 1:0;
	skill_id_level_t tmp={5010,1};	
	for( uint32_t loop=0 ; loop < 5 ; loop++ ){
		if(skill_arr[type][loop]){
			skill_info_t *psi=get_beast_skill_by_atktype(ppet,skill_arr[type][loop]);
			KDEBUG_LOG(ppet->userid,"petid=%u",ppet->petid);
			KDEBUG_LOG(ppet->userid,"pet222[skillid=%u type=%u psi=%u check=%u]",skill_arr[type][loop],type,psi==NULL,psi?ppet->check_mp_enough(psi):1000);
			if( psi && ppet->check_mp_enough(psi)){
				tmp.skill_id=psi->skill_id;
				tmp.skill_level=psi->skill_level;
				KDEBUG_LOG(ppet->userid,"pet auto pk atk[skillid=%u typeid=%u]",tmp.skill_id,ppet->type_id);
				return tmp;
			}	
		}
	}
	return tmp;
}

//剑士根据是否拥有以下技能、是否MP足够、是否遗忘技能或指令，决定使用技能的优先级为：追击（102）、狂击（106）、毁伤（108）、普通攻击（10）、防御、待机（8）
//弓术士根据是否拥有以下技能、是否MP足够、是否遗忘技能或指令，决定使用技能的优先级为：双星箭（110）、多重箭（109）、凝神箭（111）、普通攻击（10）、防御、待机（8）
//魔法师根据是否拥有以下技能、是否MP足够、是否遗忘技能或指令，决定使用技能的优先级为：陨石术（2001）、霜冻术（2002）、流火术（2003）、旋风术（2004）、普通攻击（10）、防御、待机（8）
//传教士根据是否拥有以下技能、是否MP足够、是否遗忘技能或指令，决定使用技能的优先级为：陨石术（2001）、霜冻术（2002）、流火术（2003）、旋风术（2004）、普通攻击（10）、防御、待机（8）
//忍者根据是否拥有以下技能、是否MP足够、是否遗忘技能或指令，决定使用技能的优先级为：瞬杀（116）、飞叶剑（114）、木叶遁（115）、三叶矢（113）、普通攻击（10）、防御、待机（8）
//skill_id_level_t  get_auto_pk_atk_for_user(warrior_t * p)
//{
	//KDEBUG_LOG(p->userid,"user auto pk atk111[petid=%u]",p->petid);
	//uint32_t skill_arr[][4]={
		//{102,106,108,0},
		//{110,109,111,0},
		//{2001,2002,2003,2004},
		//{2001,2002,2003,2004},
		//{116,114,115,113}
	//};	
	//uint8_t type=p->prof-1;
	//skill_id_level_t tmp={10,1};	
	//if(type > 5)
		//return tmp;
	//for( uint32_t loop=0 ; loop < 4 ; loop++ ){
		//if(skill_arr[type][loop]){
			//skill_info_t *psi=get_beast_skill_by_atktype(p,skill_arr[type][loop]);
			//KDEBUG_LOG(p->userid,"user auto pk atk111222[skillid=%u type=%u psi=%u check=%u]",skill_arr[type][loop],type,psi==NULL,psi?p->check_mp_enough(psi):1000);
			//if( psi && p->check_mp_enough(psi)){
				//tmp.skill_id=psi->skill_id;
				//tmp.skill_level=psi->skill_level;
				//KDEBUG_LOG(p->userid,"user auto pk atk[petid=%u skillid=%u typeid=%u type=%u prof=%u]",p->petid,tmp.skill_id,p->type_id,type,p->prof);
				//return tmp;
			//}	
		//}
	//}
	//KDEBUG_LOG(p->userid,"user auto pk atk222[petid=%u skillid=%u ]",p->petid,tmp.skill_id);
	//return tmp;
//}

#define  MEDICAL_SKILL 9999
auto_skill_info_t get_atk_for_swordman(warrior_t *p,bool can_use_medical)
{
	uint32_t skill_arr[][5]={{1001,9999,106,108,102},{9999,102,106,108,0},{106,108,102,0,0},{102,106,108,0,0}};
	auto_skill_info_t  tmp={10,1,1,1,0};	
	tmp.pos=p->get_weakest_warrior_pos();
	if(tmp.pos==-1){
		KDEBUG_LOG(p->userid,"no enermy");
		return tmp;
	}
	uint32_t type = p->hp*2>=p->hp_max ? 2:0;
	type+=(p->enemy_team->alive_count <=3)?0:1;
	KDEBUG_LOG(p->userid,"get atk for swordman[type=%u]",type);
	for( uint32_t loop=0 ; loop < 5 ; loop++ ){
		if(skill_arr[type][loop]){
			if(skill_arr[type][loop]==MEDICAL_SKILL ){
				if(can_use_medical){
					tmp.use_medical=1;
					tmp.mak=0;
					tmp.pos=get_warrior_pos(p->self_team,p->userid,p->petid);
					KDEBUG_LOG(p->userid,"use medical[pos=%u]",tmp.pos);
					return tmp;	
				}
			}else{
				skill_info_t *psi=get_beast_skill_by_atktype(p,skill_arr[type][loop]);
				KDEBUG_LOG(p->userid,"user auto pk atk[skillid=%u psi=%u check=%u]",
							skill_arr[type][loop],psi!=NULL,psi?p->check_mp_enough(psi):1000);
				if( psi && p->check_mp_enough(psi)){
					tmp.skill_id=psi->skill_id;
					tmp.skill_level=psi->skill_level;
					return tmp;
				}	
			}
		}
	}
	return tmp;
}

auto_skill_info_t  get_atk_for_archer(warrior_t *p,bool can_use_medical)
{
	uint32_t skill_arr[][6]={{1001,9999,110,109,111,112},{9999,109,110,112,0,0},{110,109,111,112,0,0},{109,110,112,0,0,0}};
	auto_skill_info_t  tmp={10,1,1,1,0};	
	tmp.pos=p->get_weakest_warrior_pos();
	if(tmp.pos==-1)
		return tmp;
	uint32_t type = p->hp*2>=p->hp_max ? 2:0;
	type+=(p->enemy_team->alive_count <=3)?0:1;
	KDEBUG_LOG(p->userid,"get atk for archer[type=%u]",type);
	for( uint32_t loop=0 ; loop < 6 ; loop++ ){
		if(skill_arr[type][loop]){
			if(skill_arr[type][loop]==MEDICAL_SKILL ){
				if(can_use_medical){
					tmp.use_medical=1;
					tmp.mak=0;
					tmp.pos=get_warrior_pos(p->self_team,p->userid,p->petid);
					KDEBUG_LOG(p->userid,"use medical[pos=%u]",tmp.pos);
					return tmp;	
				}
			}else{
				skill_info_t *psi=get_beast_skill_by_atktype(p,skill_arr[type][loop]);
				KDEBUG_LOG(p->userid,"user auto pk atk[skillid=%u psi=%u check=%u]",
							skill_arr[type][loop],psi!=NULL,psi?p->check_mp_enough(psi):1000);
				if( psi && p->check_mp_enough(psi)){
					tmp.skill_id=psi->skill_id;
					tmp.skill_level=psi->skill_level;
					return tmp;
				}
			}		
		}
	}
	return tmp;
}

static inline uint8_t get_restricted_nature(warrior_t* p)
{
	if (p==NULL) return 0;

	uint8_t max=0,loop=0;
	if(max<p->earth){
		loop=3;
		max=p->earth;
	}
	if(max<p->water){
		loop=0;
		max=p->water;
	}
	if(max<p->fire){
		loop=1;
		max=p->fire;
	}
	if(max<p->wind){
		loop=2;
	}
	KDEBUG_LOG(p->userid,"get restricted nature uid=%u nature=%u",p->userid,loop);
	return loop;
}
auto_skill_info_t  get_atk_for_magician(warrior_t *p,bool can_use_medical)
{
	uint32_t skill_arr[][5]={{1801,9999,2001,2005,0},
							{1801,9999,2005,2001,0},
							{1801,9999,2009,2005,2001},
							{2001,2005,1801,0,0},
							{2005,2001,1801,0,0},
							{2009,2005,2001,1801,0}};
	auto_skill_info_t  tmp={10,1,1,1,0};	
	uint32_t type = p->hp*2>=p->hp_max ? 3:0;
	if(p->enemy_team->alive_count <= 3){
		tmp.pos=p->get_weakest_warrior_pos();
		type+=0;
	}
	else if(p->enemy_team->alive_count > 3 && p->enemy_team->alive_count < 8){
		tmp.pos=p->get_most_warrior_pos();
		type+=1;
	}
	else{
		tmp.pos=p->get_most_warrior_pos();
		type+=2;
	}
	if(tmp.pos==-1)
		return tmp;
	KDEBUG_LOG(p->userid,"get atk for magician[type=%u]",type);
	for( uint32_t loop=0 ; loop < 5 ; loop++ ){
		uint32_t skillid=skill_arr[type][loop];
		if(skillid){
			if(skillid==MEDICAL_SKILL){
				if(can_use_medical){
					tmp.use_medical=1;
					tmp.mak=0;
					tmp.pos=get_warrior_pos(p->self_team,p->userid,p->petid);
					KDEBUG_LOG(p->userid,"use medical[pos=%u]",tmp.pos);
					return tmp;	
				}
			}else{
				if(skillid > 2000 && skillid < 2020)
					skillid+=get_restricted_nature(p->enemy_team->players_pos[tmp.pos]);
				skill_info_t *psi=get_beast_skill_by_atktype(p,skillid);
				KDEBUG_LOG(p->userid,"user auto pk atk[skillid=%u psi=%u check=%u]",
							skill_arr[type][loop],psi==NULL,psi?p->check_mp_enough(psi):1000);
				if( psi && p->check_mp_enough(psi)){
					tmp.skill_id=psi->skill_id;
					tmp.skill_level=psi->skill_level;
					return tmp;
				}	
			}			
			
		}
	}
	for( uint32_t loop=0 ; loop<p->skill_cnt ; loop++ ){
		if(p->skills[loop].skill_id>2000 && p->check_mp_enough(&p->skills[loop])){
			tmp.skill_id=p->skills[loop].skill_id;
			tmp.skill_level=p->skills[loop].skill_level;
			tmp.pos=p->get_weakest_warrior_pos();
			return tmp;
		}	
	}
	return tmp;
}

auto_skill_info_t  get_atk_for_churchman(warrior_t *p,bool can_use_medical)
{
	uint32_t skill_arr[][3]={{1002,9999,1003},
							{1002,9999,1003},
							{2001,2005,0},
							{2005,2001,0}};
	auto_skill_info_t  tmp={10,1,1,1,0};	
	uint32_t type = 0;
	if(p->hp*2>=p->hp_max){
		type=1;
		int32_t pos=p->check_self_team_hp_enough();
		if(pos == -1){
			if(p->enemy_team->alive_count <= 3){
				tmp.pos=p->get_weakest_warrior_pos();
				type+=1;
			}
			else{
				tmp.pos=p->get_most_warrior_pos();
				type+=2;
			}
		}else{
			tmp.mak=0;
			tmp.pos=pos;
		}
	}else{
		tmp.mak=0;
		tmp.pos=get_warrior_pos(p->self_team,p->userid,p->petid);
	}
	if(tmp.pos==-1)
		return tmp;
	KDEBUG_LOG(p->userid,"get atk for churchman[type=%u]",type);
	for( uint32_t loop=0 ; loop < 3 ; loop++ ){
		uint32_t skillid=skill_arr[type][loop];
		if(skillid){
			if(skillid == MEDICAL_SKILL){
			   	if(can_use_medical){
					tmp.use_medical=1;
					tmp.mak=0;
					KDEBUG_LOG(p->userid,"use medical[pos=%u]",tmp.pos);
					return tmp;	
				}
			}else{
				if(skillid >2000 && skillid <2010)
					skillid+=get_restricted_nature(p->enemy_team->players_pos[tmp.pos]);
				skill_info_t *psi=get_beast_skill_by_atktype(p,skillid);
				KDEBUG_LOG(p->userid,"user auto pk atk[skillid=%u  psi=%u check=%u]",
							skill_arr[type][loop],psi==NULL,psi?p->check_mp_enough(psi):1000);
				if( psi && p->check_mp_enough(psi)){
					tmp.skill_id=psi->skill_id;
					tmp.skill_level=psi->skill_level;
					return tmp;
				}
			}
		}
	}
	tmp.mak=1;
	tmp.pos=p->get_weakest_warrior_pos();
	for( uint32_t loop=0 ; loop<p->skill_cnt ; loop++ ){
		if(p->skills[loop].skill_id>2000 && p->check_mp_enough(&p->skills[loop])){
			tmp.skill_id=p->skills[loop].skill_id;
			tmp.skill_level=p->skills[loop].skill_level;
			return tmp;
		}	
	}
	return tmp;
}

auto_skill_info_t  get_atk_for_ninja(warrior_t *p, bool can_use_medical)
{
	uint32_t skill_arr[][7]={{1001,9999,116,114,112,115,113},
							{9999,113,116,115,114,0,0},
							{116,114,112,115,113,0,0},
							{113,116,115,114,0,0,0}};
	auto_skill_info_t  tmp={10,1,1,1,0};	
	tmp.pos=p->get_weakest_warrior_pos();
	if(tmp.pos==-1){
		return tmp;
	}
	uint32_t type = p->hp*2>=p->hp_max ? 2:0;
	type+=(p->enemy_team->alive_count <=3)?0:1;
	KDEBUG_LOG(p->userid,"get atk for ninja[type=%u]",type);
	for( uint32_t loop=0 ; loop < 7 ; loop++ ){
		if(skill_arr[type][loop]){
			if(skill_arr[type][loop]==MEDICAL_SKILL){
				if(can_use_medical){
					tmp.use_medical=1;
					tmp.mak=0;
					tmp.pos=get_warrior_pos(p->self_team,p->userid,p->petid);
					KDEBUG_LOG(p->userid,"use medical[pos=%u]",tmp.pos);
					return tmp;	
				}
			}else{
				skill_info_t *psi=get_beast_skill_by_atktype(p,skill_arr[type][loop]);
				KDEBUG_LOG(p->userid,"user auto pk atk[skillid=%u psi=%u check=%u]",
							skill_arr[type][loop],psi!=NULL,psi?p->check_mp_enough(psi):1000);
				if( psi && p->check_mp_enough(psi)){
					tmp.skill_id=psi->skill_id;
					tmp.skill_level=psi->skill_level;
					return tmp;
				}	
			}		
		}	
	}
	return tmp;
}
/*
狂战士：
怪物数＞3：
HP＞30%：剑气（4003）、绝气斩（4006）、惊魂斩（4001）、普通攻击（10）、防御（701）、待机
HP≤30%：气疗术（4004）、使用药剂、剑气（4003）、绝气斩（4006）、惊魂斩（4001）、普通攻击（10）、防御（701）、待机
怪物数≤3
HP＞30%：绝气斩（4006）、惊魂斩（4001）、普通攻击（10）、防御（701）、待机
HP≤30%：气疗术（4004）、使用药剂、绝气斩（4006）、惊魂斩（4001）、普通攻击（10）、防御（701）、待机
*/
auto_skill_info_t  get_atk_for_gedoushi(warrior_t *p, bool can_use_medical)
{
	uint32_t skill_arr[][6]={{4003,4006,4001,10,0,0},
							{4004,9999,4003,4006,4001,10},
							{4006,4001,10,0,0,0},
							{4004,9999,4006,4001,10,0}};
	auto_skill_info_t  tmp={701,1,1,1,0};	
	uint32_t type =(p->enemy_team->alive_count <=3)?2:0;
	type += p->hp>p->hp_max*0.3 ? 0:1;
	if(p->enemy_team->alive_count <= 3){
		tmp.pos=p->get_weakest_warrior_pos();
	}
	else{
		tmp.pos=p->get_most_warrior_pos();
	}
	if(tmp.pos==-1)
		return tmp;
	KDEBUG_LOG(p->userid,"get atk for gedoushi[type=%u]",type);
	for( uint32_t loop=0 ; loop < 6 ; loop++ ){
		uint32_t skillid=skill_arr[type][loop];
		if(skillid){
			if(skillid==MEDICAL_SKILL){
				if(can_use_medical){
					tmp.use_medical=1;
					tmp.mak=0;
					tmp.pos=get_warrior_pos(p->self_team,p->userid,p->petid);
					KDEBUG_LOG(p->userid,"use medical[pos=%u]",tmp.pos);
					return tmp;	
				}
			}else{
				skill_info_t *psi=get_beast_skill_by_atktype(p,skillid);
				KDEBUG_LOG(p->userid,"user auto pk atk[skillid=%u psi=%u check=%u]",
							skill_arr[type][loop],psi==NULL,psi?p->check_mp_enough(psi):1000);
				if( psi && p->check_mp_enough(psi)){
					tmp.skill_id=psi->skill_id;
					tmp.skill_level=psi->skill_level;
					return tmp;
				}	
			}			
		}
	}
	return tmp;
}
/*
 * 黑魔导士：
怪物数＞7：
 * HP＞30%：暗云裂解（4015）、大地刺枪（4013）、轮转风暴（4014）、炎蝶之舞（4011）、晶棱封杀（4012）、生命契约（4016）、普通攻击（10）、防御（701）、待机
 * HP≤30%：生命契约（4016）、使用药剂、暗云裂解（4015）、大地刺枪（4013）、轮转风暴（4014）、炎蝶之舞（4011）、晶棱封杀（4012）、普通攻击（10）、防御（701）、待机
7≥怪物数＞3，相邻怪物最多的怪物为中心：
 * HP＞30%：大地刺枪（4013）、轮转风暴（4014）、炎蝶之舞（4011）、晶棱封杀（4012）、生命契约（4016）、普通攻击（10）、防御（701）、待机
 * HP≤30%：生命契约（4016）、使用药剂、大地刺枪（4013）、轮转风暴（4014）、炎蝶之舞（4011）、晶棱封杀（4012）、普通攻击（10）、防御（701）、待机
怪物数≤3：
 * HP＞30%：炎蝶之舞（4011）、晶棱封杀（4012）、生命契约（4016）、普通攻击（10）、防御（701）、待机
 * HP≤30%：生命契约（4016）、使用药剂、炎蝶之舞（4011）、晶棱封杀（4012）、普通攻击（10）、防御（701）、待机*/
auto_skill_info_t  get_atk_for_heimodaoshi(warrior_t *p,bool can_use_medical)
{
	uint32_t skill_arr[][8]={{4016,9999,4011,4012,10,0,0,0},
							{4016,9999,4013,4014,4011,4012,10,0},
							{4016,9999,4015,4013,4014,4011,4012,10},
							{4011,4012,4016,10,0,0,0,0},
							{4013,4014,4011,4012,4016,10,0,0},
							{4015,4013,4014,4011,4012,4016,10,0}};
	auto_skill_info_t  tmp={701,1,1,1,0};	
	uint32_t type = p->hp>p->hp_max*0.3 ? 3:0;
	if(p->enemy_team->alive_count <= 3){
		tmp.pos=p->get_weakest_warrior_pos();
		type+=0;
	} else if(p->enemy_team->alive_count > 3 && p->enemy_team->alive_count < 7){
		tmp.pos=p->get_most_warrior_pos();
		type+=1;
	} else{
		tmp.pos=p->get_most_warrior_pos();
		type+=2;
	}
	if(tmp.pos==-1)
		return tmp;
	KDEBUG_LOG(p->userid,"get atk for heimo[type=%u]",type);
	for( uint32_t loop=0 ; loop < 8 ; loop++ ){
		uint32_t skillid=skill_arr[type][loop];
		if(skillid){
			if(skillid==MEDICAL_SKILL){
				if(can_use_medical){
					tmp.use_medical=1;
					tmp.mak=0;
					tmp.pos=get_warrior_pos(p->self_team,p->userid,p->petid);
					KDEBUG_LOG(p->userid,"use medical[pos=%u]",tmp.pos);
					return tmp;	
				}
			}else{
				if(skillid > 2000 && skillid < 2020)
					skillid+=get_restricted_nature(p->enemy_team->players_pos[tmp.pos]);
				skill_info_t *psi=get_beast_skill_by_atktype(p,skillid);
				KDEBUG_LOG(p->userid,"user auto pk atk[skillid=%u psi=%u check=%u]",
							skill_arr[type][loop],psi==NULL,psi?p->check_mp_enough(psi):1000);
				if( psi && p->check_mp_enough(psi)){
					tmp.skill_id=psi->skill_id;
					tmp.skill_level=psi->skill_level;
					return tmp;
				}	
			}			
			
		}
	}
	return tmp;
}

/*
圣言使：
怪物数＞3，相邻怪物最多的怪物为中心：
HP＞30%：圣灵召唤（4022）、圣枪投射（4021）、普通攻击（10）、防御（701）、待机
HP≤30%：圣堂领域（4023）、使用药剂、圣灵召唤（4022）、圣枪投射（4021）、普通攻击（10）、防御（701）、待机
怪物数≤3：
HP＞30%：圣枪投射（4021）、普通攻击（10）、防御（701）、待机
HP≤30%：圣堂领域（4023）、使用药剂、圣枪投射（4021）、普通攻击（10）、防御（701）、待机
*/
auto_skill_info_t  get_atk_for_shengyanshi(warrior_t *p,bool can_use_medical)
{
	uint32_t skill_arr[][5]={{4022,4021,10,0,0},
							{4023,9999,4022,4021,10},
							{4021,10,0,0,0},
							{4023,9999,4021,10,0}};
	auto_skill_info_t  tmp={701,1,1,1,0};	
	uint32_t type = p->enemy_team->alive_count<=3?2:0;
	type+=p->hp<=p->hp_max*0.3?1:0;
	KDEBUG_LOG(p->userid,"get atk for shengyanshi[type=%u]",type);
	for( uint32_t loop=0 ; loop < 5 ; loop++ ){
		uint32_t skillid=skill_arr[type][loop];
		if(skillid){
			if(skillid == MEDICAL_SKILL){
			   	if(can_use_medical){
					tmp.use_medical=1;
					tmp.mak=0;
					KDEBUG_LOG(p->userid,"use medical[pos=%u]",tmp.pos);
					return tmp;	
				}
			}else{
				skill_info_t *psi=get_beast_skill_by_atktype(p,skillid);
				KDEBUG_LOG(p->userid,"user auto pk atk[skillid=%u  psi=%u check=%u]",
							skill_arr[type][loop],psi==NULL,psi?p->check_mp_enough(psi):1000);
				int pos=-1;
				if(skillid == 4023){
					pos=p->get_self_weakest_warrior_pos();
				}else if(skillid == 4022){
					pos=p->get_most_warrior_pos();
				}else{
					pos=p->get_weakest_warrior_pos();
				}
				if( pos==-1 ){
					return tmp;
				}
				if( psi && p->check_mp_enough(psi)){
					tmp.skill_id=psi->skill_id;
					tmp.skill_level=psi->skill_level;
					if( skillid == 4023 ){
						tmp.mak=0;
					}
					return tmp;
				}
			}
		}
	}
	return tmp;
}
/*
巫术士：
怪物数＞3，相邻怪物最多的怪物为中心：
HP≤30%：使用药剂、审判之轮（4033）、灾厄之爪（4031）、混沌之刃（4032）、普通攻击（10）、防御（701）、待机
HP＞30%：审判之轮（4033）、灾厄之爪（4031）、混沌之刃（4032）、普通攻击（10）、防御（701）、待机
怪物数≤3：
HP≤30%：使用药剂、灾厄之爪（4031）、混沌之刃（4032）、普通攻击（10）、防御（701）、待机
HP＞30%：灾厄之爪（4031）、混沌之刃（4032）、普通攻击（10）、防御（701）、待机
*/
auto_skill_info_t  get_atk_for_wushushi(warrior_t *p, bool can_use_medical)
{
	uint32_t skill_arr[][4]={{9999,4031,4032,10},
							{4033,4031,4032,10},
							{9999,4031,10,0},
							{4031,4032,10,0}};
	auto_skill_info_t  tmp={701,1,1,1,0};	
	uint32_t type =(p->enemy_team->alive_count <=3)?2:0;
	type += p->hp>p->hp_max*0.3 ? 1:0;
	if(p->enemy_team->alive_count <= 3){
		tmp.pos=p->get_weakest_warrior_pos();
	}
	else{
		tmp.pos=p->get_most_warrior_pos();
	}
	if(tmp.pos==-1)
		return tmp;
	KDEBUG_LOG(p->userid,"get atk for wushushi[type=%u]",type);
	for( uint32_t loop=0 ; loop < 4 ; loop++ ){
		uint32_t skillid=skill_arr[type][loop];
		if(skillid){
			if(skillid==MEDICAL_SKILL){
				if(can_use_medical){
					tmp.use_medical=1;
					tmp.mak=0;
					tmp.pos=get_warrior_pos(p->self_team,p->userid,p->petid);
					KDEBUG_LOG(p->userid,"use medical[pos=%u]",tmp.pos);
					return tmp;	
				}
			}else{
				skill_info_t *psi=get_beast_skill_by_atktype(p,skillid);
				KDEBUG_LOG(p->userid,"user auto pk atk[skillid=%u psi=%u check=%u]",
							skill_arr[type][loop],psi==NULL,psi?p->check_mp_enough(psi):1000);
				if( psi && p->check_mp_enough(psi)){
					tmp.skill_id=psi->skill_id;
					tmp.skill_level=psi->skill_level;
					return tmp;
				}	
			}			
		}
	}
	return tmp;
}

auto_skill_info_t  get_auto_pk_atk_for_user(warrior_t * p, bool can_use_medical)
{
	auto_skill_info_t  skill={10,1,1,1,0};	
	//DEBUG_LOG("pppppppprof[%u]",p->prof);
	switch ( p->prof ){
		case 1 :
			skill=get_atk_for_swordman(p,can_use_medical);
			break;
		case 2 :
			skill=get_atk_for_archer(p,can_use_medical);
			break;
		case 3 :
			skill=get_atk_for_magician(p,can_use_medical);
			break;
		case 4 :
			skill=get_atk_for_churchman(p,can_use_medical);
			break;
		case 5 :
			skill=get_atk_for_ninja(p,can_use_medical);
			break;
		case 6 :
			skill=get_atk_for_gedoushi(p,can_use_medical);
			break;
		case 7 :
			skill=get_atk_for_heimodaoshi(p,can_use_medical);
			break;
		case 8 :
			skill=get_atk_for_shengyanshi(p,can_use_medical);
			break;
		case 9 :
			skill=get_atk_for_wushushi(p,can_use_medical);
			break;
		default :
			break;
			DEBUG_LOG("prof error");
	}
	KDEBUG_LOG(p->userid,"auto use skill [skillid=%u level=%u pos=%u mak=%u tag=%u]",
				skill.skill_id,skill.skill_level,skill.pos,skill.mak,skill.use_medical);
	return skill;
}

bool deal_typeid_2005_ai(battle_info_t* abi,  warrior_t * p  )
{
	atk_info_t * p_atk_info=&(p->atk_info[0]);
	if ( get_no_boss_count(&(abi->challgee),TYPEID_2005 )<3){
		p_atk_info->atk_mark=0;
		p_atk_info->atk_type=skill_rl_pet_jiushu_all_only_svr;
		p_atk_info->atk_level=1;
		p_atk_info->atk_pos=0;
		p_atk_info->wrior=p;
		check_user_step_attack_and_insert_atk_list(abi, p, p_atk_info);

		return true;
	}
	
	return false;
}

void deal_typeid_2004_ai(battle_info_t* abi,  warrior_t * p  )
{
	abi->set_is_no_boss_lt_3_in_pk_beast_2004 ( get_no_boss_count(&(abi->challgee),TYPEID_2004 )<3 );
	atk_info_t * p_atk_info=&(p->atk_info[0]);
	if (abi->is_no_boss_lt_3_in_pk_beast_2004() ){
		p_atk_info->atk_mark=0;
		p_atk_info->atk_type=skill_rl_pet_jiushu_all_only_svr;
		p_atk_info->atk_level=1;
		p_atk_info->atk_pos=0;
		p_atk_info->wrior=p;
	}else {
		p_atk_info->atk_mark=1;
		p_atk_info->atk_type=5107;
		p_atk_info->atk_level=10;
		p_atk_info->atk_pos=get_rand_alive_warrior_pos(&(abi->challger) ,p->pet_state);
		p_atk_info->wrior=p;
	}
	check_user_step_attack_and_insert_atk_list(abi, p, p_atk_info);

}

void deal_typeid_2004_ai_ex(battle_info_t* abi, warrior_t* p)
{
	uint32_t skill_type, atk_mark, atk_pos;
	if (p->hp > p->hp_max / 5) {
		skill_type =  skill_pa_pet_douqi;
		atk_mark = 1;
		atk_pos = get_rand_alive_warrior_pos(&(abi->challger) ,p->pet_state);
	} else {
		skill_type =  skill_rh_pet_mingsi;
		atk_mark = 0;
		atk_pos = get_warrior_pos(p->self_team, p->userid, p->petid);
	}
	
	atk_info_t * p_atk_info=&(p->atk_info[0]);
	p_atk_info->atk_mark = atk_mark;
	p_atk_info->atk_type = skill_type;
	p_atk_info->atk_level=10;
	p_atk_info->atk_pos = atk_pos;
	p_atk_info->wrior=p;
	check_user_step_attack_and_insert_atk_list(abi, p, p_atk_info);

	p_atk_info=&(p->atk_info[1]);
	p_atk_info->atk_mark = atk_mark;
	p_atk_info->atk_type = skill_type;
	p_atk_info->atk_level=10;
	p_atk_info->atk_pos = atk_pos;
	p_atk_info->wrior=p;
	check_user_step_attack_and_insert_atk_list(abi, p, p_atk_info);
}

int deal_typeid_2013_ai(battle_info_t* abi, warrior_t* p)
{
	abi->set_is_no_boss_lt_3_in_pk_beast_2013 ( get_no_boss_count(&(abi->challgee),TYPEID_2013 )<3 );
	int atk_seq = 0;
	atk_info_t * p_atk_info = &(p->atk_info[atk_seq]);
	if (abi->is_no_boss_lt_3_in_pk_beast_2013() ){
		p_atk_info->atk_mark=0;
		p_atk_info->atk_type=skill_rl_pet_jiushu_all_only_svr;
		p_atk_info->atk_level=1;
		p_atk_info->atk_pos=0;
		p_atk_info->wrior=p;
		atk_seq ++;
		check_user_step_attack_and_insert_atk_list(abi, p, p_atk_info);
	}
	
	p_atk_info = &(p->atk_info[atk_seq]);
	if (p->hp < p->hp_max / 5 && rand() % 100 < 30) {
		p_atk_info->atk_mark=0;
		p_atk_info->atk_type = skill_rh_pet_zhiyujiejie;
		p_atk_info->atk_level=6;
		p_atk_info->atk_pos = 0;
		p_atk_info->wrior=p;
		atk_seq ++;
		check_user_step_attack_and_insert_atk_list(abi, p, p_atk_info);
	}
	return atk_seq;
}

void deal_typeid_2008_ai( battle_info_t* abi)
{
	if (abi->is_pk_beast_2008() ) {//章界王
		bool is_2008_dead=false;
		bool is_only_2008=true;
		warrior_t* p_boss=NULL;//大怪
		warrior_t* p_alive_beast=NULL;//小怪

		list_head_t* next;
		list_head_t* cur;
		list_for_each_safe(cur, next, &abi->warrior_list){
			warrior_t* p = list_entry(cur, warrior_t, waor_list);

			if (!IS_BEAST_ID(p->userid)) {
				continue;
			}

			bool is_need_jiushu=p->check_state( dead_bit);

			if (p->type_id==TYPEID_2008){//是章界王
				p_boss=p;
	 			if (is_need_jiushu ){//死了
					is_2008_dead=true;
				}
			} else if (!is_need_jiushu) { //不是章界王 没死	
				is_only_2008=false;
				p_alive_beast=p;
			}
		}

		if  ( is_2008_dead ){//boss 死了，小弟招回
			//怎么会==NULL!!!
			if (p_alive_beast==NULL ) return;
			atk_info_t * p_atk_info=&(p_alive_beast->atk_info[0]);
			p_atk_info->atk_mark=0;
			p_atk_info->atk_type=skill_rl_pet_jiushu_all_only_svr;
			p_atk_info->atk_level=1;
			p_atk_info->atk_pos=0;//
			p_atk_info->wrior= p_alive_beast ;
			p_alive_beast->set_is_beast_deal_attacked(true);
			check_user_step_attack_and_insert_atk_list(abi, p_alive_beast ,  p_atk_info);
		}
		if  ( is_only_2008 ){//小弟全死了，boss 全招回它们
			atk_info_t * p_atk_info=&(p_boss->atk_info[0]);
			p_atk_info->atk_mark=0;
			p_atk_info->atk_type=skill_rl_pet_jiushu_all_only_svr;//所有的都招回
			p_atk_info->atk_level=1;
			p_atk_info->atk_pos=0;
			p_atk_info->wrior= p_boss;
			p_boss->set_is_beast_deal_attacked(true);
			check_user_step_attack_and_insert_atk_list(abi, p_boss, p_atk_info);
		}

	}

}

bool  set_fuzhu_pet_atk_info( battle_info_t*  abi, warrior_t* p_pet , std::map<int, skill_info_t> &skill_map,uint32_t skill_id  )
{
	if (skill_id==skill_pd_pet_fangyu ||  skill_id == skill_pa_pet_base_fight){
		if (abi->is_pets_pk_mode() ){//宠物pk时,无防御
			skill_id=skill_pa_pet_base_fight;	
		}
		atk_info_t *p_atk_info=&(p_pet->atk_info[0]);
//		p_atk_info->atk_pos = 0;
		p_atk_info->atk_pos = get_rand_alive_warrior_pos(p_pet->enemy_team, p_pet->pet_state );
		p_atk_info->atk_mark = 1; // enemy
		p_atk_info->atk_type =skill_id ; 
		p_atk_info->atk_level = 1;
		p_atk_info->wrior= p_pet ;
		check_user_step_attack_and_insert_atk_list(abi, p_pet, p_atk_info);
		return true;
	}

	skill_info_t skill_info = skill_map[skill_id]; 	 
	if (skill_info.skill_id==skill_id ){//找到了
		atk_info_t *p_atk_info=&(p_pet->atk_info[0]);
		p_atk_info->atk_type=skill_info.skill_id;
		p_atk_info->atk_level=skill_info.skill_level ;
		if (!check_mp_enough( p_pet,p_atk_info  )) return false;
		p_atk_info->atk_mark=1;
//		p_atk_info->atk_pos = 0;
		p_atk_info->atk_pos= get_rand_alive_warrior_pos(p_pet->enemy_team, p_pet->pet_state );
		p_atk_info->wrior= p_pet ;
		check_user_step_attack_and_insert_atk_list(abi, p_pet, p_atk_info);
		return true;
	}else{
		return false;
	}
}
//处理辅助宠物的ai
void  deal_fuzhu_pet_ai( battle_info_t* abi,  warrior_t* p_pet )
{
	if (abi->touxi==touxi_by_challgee){//被偷袭了
		return;
	}
	skill_id_level_t skill_item=get_auto_pk_atk_for_pet(p_pet);
	atk_info_t *p_atk_info=&(p_pet->atk_info[0]);
	p_atk_info->atk_type=skill_item.skill_id;
	p_atk_info->atk_level=skill_item.skill_level ;
	p_atk_info->atk_mark=1;
	p_atk_info->atk_pos= -1; 
	p_atk_info->wrior= p_pet ;
	check_user_step_attack_and_insert_atk_list(abi, p_pet, p_atk_info);
	

}

void deal_renzhe_ai(battle_info_t* abi,  warrior_t * p)
{
	int rand_prob = rand() % 100;
	atk_info_t *p_atk=&(p->atk_info[0]);

	if (p->hp * 10 > p->hp_max * 3) {
		if (p->enemy_team->alive_count >= 3) {
			if(rand_prob < 60) {
				p_atk->atk_mark = 1;
				p_atk->atk_type = skill_pet_sanyeshi;
				p_atk->atk_pos  = get_rand_alive_warrior_pos(p->enemy_team, 0);
			} else {
				p_atk->atk_mark = 1;
				p_atk->atk_type = skill_pa_pet_douqi;
				p_atk->atk_pos  = get_rand_alive_warrior_pos(p->enemy_team, 0);
			}
		} else {
			if(rand_prob < 60) {
				p_atk->atk_mark = 1;
				p_atk->atk_type = skill_pet_feiyejian;
				p_atk->atk_pos  = get_rand_alive_warrior_pos(p->enemy_team, 0);
			} else {
				p_atk->atk_mark = 1;
				p_atk->atk_type = skill_pa_pet_meiying;
				p_atk->atk_pos  = get_rand_alive_warrior_pos(p->enemy_team, 0);
			}
		}
	} else {
		if (p->enemy_team->alive_count >= 3) {
			if(rand_prob < 50) {
				p_atk->atk_mark = 0;
				p_atk->atk_type = skill_rh_pet_mingsi;
				p_atk->atk_pos  = get_warrior_pos(p->self_team,p->userid,0);
			} else {
				p_atk->atk_mark = 1;
				p_atk->atk_type = skill_pa_pet_douqi;
				p_atk->atk_pos  = get_rand_alive_warrior_pos(p->enemy_team, 0);
			}
		} else {
			warrior_t *lp_pet =  get_rand_alive_warrior_pet(p->enemy_team);
			if(lp_pet && rand_prob < 30) {
				p_atk->atk_mark = 1;
				p_atk->atk_type = skill_pet_shunsha;
				p_atk->atk_pos  = get_warrior_pos(p->enemy_team, lp_pet->userid,lp_pet->petid);
			} else {
				if(!lp_pet) rand_prob = rand_prob * 7 / 10 + 30;
				if(rand_prob < 60) {
					p_atk->atk_mark = 0;
					p_atk->atk_type = skill_rh_pet_mingsi;
					p_atk->atk_pos  = get_warrior_pos(p->self_team,p->userid,0);
				} else if(rand_prob < 80) {
					p_atk->atk_mark = 1;
					p_atk->atk_type = skill_pet_muyedun;
					p_atk->atk_pos  = get_rand_alive_warrior_pos(p->enemy_team, 0);
				} else {
					p_atk->atk_mark = 1;
					p_atk->atk_type = skill_pa_pet_meiying;
					p_atk->atk_pos  = get_rand_alive_warrior_pos(p->enemy_team, 0);
				}
			}
		}
	}

	p_atk->wrior = p;
	p_atk->atk_level = (p->level + 9) / 10;

	check_user_step_attack_and_insert_atk_list(abi, p, p_atk);
}

void deal_2012_ai(battle_info_t* abi,  warrior_t * p)
{
	if (p->self_team->alive_count < 4) {
		p->atk_info[0].atk_mark=0;
		p->atk_info[0].atk_type=skill_rl_pet_jiushu_all_only_svr;
		p->atk_info[0].atk_level=1;
		p->atk_info[0].atk_pos = 0;
		p->atk_info[0].wrior  = p;
	} else {
		beast_intel_attack(abi,p,&p->atk_info[0],0);
	}
	check_user_step_attack_and_insert_atk_list(abi, p, &p->atk_info[0]);
	beast_intel_attack(abi,p,&p->atk_info[1],0);
	check_user_step_attack_and_insert_atk_list(abi, p, &p->atk_info[1]);
}

void new_round_battle_beast(battle_info_t* abi)
{	
	DEBUG_LOG("new_round_battle_beast START===={");
	//处理章界王特别的ai
	deal_typeid_2008_ai(abi);

	list_head_t* next;
	list_head_t* cur;
	list_for_each_safe(cur, next, &abi->warrior_list){
		warrior_t* p = list_entry(cur, warrior_t, waor_list);
		uint64_t ab_state = (p->p_waor_state->state & abnormal_states);
		DEBUG_LOG("new_round_battle_beast(): warrior: %p, is pet: %d, is user: %d abstate:%lx %u %u",
			   	p, p->is_pet(), p->is_user(),ab_state & (shihua|hunshui),p->p_waor_state->check_state(shihua_bit),p->p_waor_state->check_state(hunshui_bit));

		if(ab_state & (shihua|hunshui)) {
			if(ab_state == shihua) {
				clean_one_state(p,shihua_bit,1);
				if(rand() % 100 < p->r_shihua) {
					clean_one_state(p,shihua_bit,10);
				}
				KDEBUG_LOG(p->userid,"new round:petid=%u shihua state=%lx",p->petid,p->p_waor_state->state);
			} else {
				clean_one_state(p,hunshui_bit,1);
				if(rand() % 100 < p->r_hunshui) {
					clean_one_state(p,hunshui_bit,10);
				}
				KDEBUG_LOG(p->userid,"new round:petid=%u sleeps state=%lx",p->petid,p->p_waor_state->state);
			}
			continue;
		}

		if (p->pet_state != on_fuzhu && !IS_BEAST_ID(p->userid))
			continue;

		if (p->is_beast_deal_attacked()){//已经处理了
			p->set_is_beast_deal_attacked(false);
			continue;
		}

		if (CANNOT_ATTACK(p))
			continue;

		if (abi->is_pk_beast_2004() && p->type_id==TYPEID_2004 ){
			deal_typeid_2004_ai(abi, p);
			continue;
		}

		if (abi->is_pk_beast_2906() && p->type_id == TYPEID_2004) {
			deal_typeid_2004_ai_ex(abi, p);
			continue;
		}

		if (p->type_id == 3018){
			deal_renzhe_ai(abi, p);
			continue;
		}
		
		if (abi->is_pk_beast_2012() && p->type_id == TYPEID_2012){
			deal_2012_ai(abi, p);
			continue;
		}

		//辅助宠物处理
		if (p->pet_state == on_fuzhu){
			deal_fuzhu_pet_ai(abi,p );
			continue;	
		}
		

		int loop = 1;
		if ( (abi->is_pk_beast_3017() && p->type_id==TYPEID_3017 )){
			loop=2;
		}

		if(p->self_team->teamid==43 ||p->self_team->teamid==44){
			loop=2;
		}

		if(p->self_team->teamid==46 && p->type_id==2017){
			loop=2;
		}

	
		//可以攻击两次
		if (p->is_double_ack()){
			DEBUG_LOG("IS_DOUBLE_ACK");
			loop=2;
		}
		

		int i  = 0;
		//处理2005，要两次技能,并且第一次可能会使用召唤
		if (abi->is_pk_beast_2005() && p->type_id==TYPEID_2005 ){
			loop=2;
			if (deal_typeid_2005_ai(abi, p)){//已经处理一个了
				i=1;
			}
		}

		if (abi->is_pk_beast_2013() && p->type_id == TYPEID_2013) {
			i = deal_typeid_2013_ai(abi, p);
			loop = 2;
		}

		for (; i < loop; i++){
			get_beast_attack_skill_by_lua(abi, p, &p->atk_info[i], i);

			check_user_step_attack_and_insert_atk_list(abi, p, &p->atk_info[i]);
			if (i == 0){//第一招
				set_fangyu_state_before_round_attack(abi, p, &p->atk_info[i]);
			}

		}
	}
	DEBUG_LOG("}====new_round_battle_beast END ");
}

//计算得到怪物的发招
void get_beast_attack_skill_by_lua(battle_info_t* abi, warrior_t* lp, atk_info_t* aai, int seq)
{
	if (abi->touxi == touxi_by_challger){
		aai->atk_type = skill_pa_pet_dai_ji;
		aai->atk_level = 1;
		return;
	}
	
	if (lp->skill_cnt == 0){
		aai->atk_type = skill_pa_pet_dai_ji;
		aai->atk_level = 1;
		KDEBUG_LOG(lp->userid, "ERR BEAST HAS NO SKILL\t[uid=%u petid=%u]", lp->userid, lp->petid);
		return;
	}

	// 检查lua堆栈空间是否足够，
	// 注意堆栈空间不足时lua_checkstack 会保证空间，
	// 当接口无法保证时，我们需要重新加载一次lua脚本，释放之前的lua全局变量
	if (lua_checkstack(g_lua_state, 7) == 0){
		if (load_lua_script() == -1){
			KERROR_LOG(lp->userid, "function[%s] reload script failed", BATRSERV_LUA_FOR_C_BEAST_AI);
			beast_intel_attack(abi, lp, aai, seq);
			return;
		}
	}

	/* the function name */
	lua_getglobal(g_lua_state, BATRSERV_LUA_FOR_C_BEAST_AI);
	if (!lua_isfunction(g_lua_state, -1)){	//确认一下是个函数 
		lua_pop(g_lua_state,1);
		KERROR_LOG(lp->userid, "function[%s] is not exist", BATRSERV_LUA_FOR_C_BEAST_AI);
		beast_intel_attack(abi, lp, aai, seq);
		return;
	}

	uint32_t pos = get_warrior_pos(lp->self_team, lp->userid, lp->petid);

	char int64id[256] = {0};
	i64tostring(abi->batid, int64id);
	
	/* the 1 argument */
	lua_pushstring(g_lua_state, int64id);

	/* the 2 argument */
	if (lp->self_team == &abi->challgee){
		lua_pushnumber(g_lua_state, mark_challengee);
	}else{
		lua_pushnumber(g_lua_state, mark_challenger);
	}
	
	/* the 3 argument */
	lua_pushnumber(g_lua_state, pos);

	/* the 4 argument */
	lua_pushnumber(g_lua_state, lp->type_id);

	/* the 5 argument */
	lua_pushnumber(g_lua_state, seq);

	if (abi->touxi == touxi_by_challgee){
		lua_pushnumber(g_lua_state, 1);
	}else{
		lua_pushnumber(g_lua_state, 0);
	}

	KDEBUG_LOG(lp->userid, "LUA SCRIPT\t[batid=%lu %u %d %u %u]", abi->batid, 1, pos, lp->type_id, seq);
	
	// 调用,我们有6个参数，要得到1个结果 
	// 你可能注意到BATRSERV_LUA_FOR_C_BEAST_AI函数返回了多个或者是没有，不过我们只要1个，这没有问题 
	// 但是在lua接口中我们还是希望只返回1个值，多余的值我们根本用不到。
	// 有一种可能是我们的lua接口中，也调用了该接口，建议不要这么做。
	// 没有使用错误处理回调，所以lua_pcall最后一个参数是0 
	if(0 != lua_pcall(g_lua_state, 6, 1, 0)) {
		//如果错误，显示 
		const char* errmsg = lua_tostring(g_lua_state, -1); 
        lua_pop(g_lua_state, 1);

		if (errmsg == NULL || errmsg[0] == 0x00){
			KERROR_LOG(lp->userid, "something error in function[%s],error=[no error message]", BATRSERV_LUA_FOR_C_BEAST_AI);
		}else{
			KERROR_LOG(lp->userid, "something error in function[%s],error=[%s]", BATRSERV_LUA_FOR_C_BEAST_AI, errmsg);
		}

		beast_intel_attack(abi, lp, aai, seq);
		return;
	}
	
	/* get the result */
	uint32_t ret = (int)lua_tonumber(g_lua_state, -1);
	lua_pop(g_lua_state, 1);

	KDEBUG_LOG(lp->userid, "LUA SCRIPT RET\t[batid=%lu %u]", abi->batid, ret);

	if (!ret){//lua还没设置招术 
		beast_intel_attack(abi, lp, aai, seq);
	}
}

void register_beast_ai_function()
{
	/*****************************************************************************************************/
	REGISTER_LUA_FUN(g_lua_state, "lua_chk_player_has_prop", lua_chk_player_has_prop);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_prop", lua_get_player_prop);
	REGISTER_LUA_FUN(g_lua_state, "lua_set_player_prop", lua_set_player_prop);
	REGISTER_LUA_FUN(g_lua_state, "lua_chk_player_state", lua_chk_player_state);
	REGISTER_LUA_FUN(g_lua_state, "lua_reset_player_state", lua_reset_player_state);

	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_highspeed", lua_get_player_highspeed);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_least_hp", lua_get_player_least_hp);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_most_hp", lua_get_player_most_hp);
	
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_by_prof_rand", lua_get_player_by_prof_rand);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_except_prof_rand", lua_get_player_except_prof_rand);
	
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_by_state_rand", lua_get_player_by_state_rand);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_beast_by_typeid", lua_get_beast_by_typeid);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_by_rand", lua_get_player_by_rand);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_by_rand_distance", lua_get_player_by_rand_distance);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_fangyu_rand_distance", lua_get_player_fangyu_rand_distance);
	REGISTER_LUA_FUN(g_lua_state, "lua_can_reach_distance", lua_can_reach_distance);

	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_skill_prop", lua_get_player_skill_prop);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_equip_prop", lua_get_player_equip_prop);
	REGISTER_LUA_FUN(g_lua_state, "lua_set_player_equip_prop", lua_set_player_equip_prop);

	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_count", lua_get_player_count);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_of_prof_count", lua_get_player_of_prof_count);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_count_alive", lua_get_player_count_alive);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_player_of_prof_count_alive", lua_get_player_of_prof_count_alive);
	REGISTER_LUA_FUN(g_lua_state, "lua_chk_player_can_attack", lua_chk_player_can_attack);
	REGISTER_LUA_FUN(g_lua_state, "lua_chk_plater_need_attack", lua_chk_plater_need_attack);
	REGISTER_LUA_FUN(g_lua_state, "lua_get_play_atk_target", lua_get_play_atk_target);
	REGISTER_LUA_FUN(g_lua_state, "lua_calc_attack_hit_rate", lua_calc_attack_hit_rate);
	REGISTER_LUA_FUN(g_lua_state, "lua_calc_phy_atk_hurthp_1", lua_calc_phy_atk_hurthp_1);
	REGISTER_LUA_FUN(g_lua_state, "lua_calc_phy_atk_hurthp_2", lua_calc_phy_atk_hurthp_2);
	REGISTER_LUA_FUN(g_lua_state, "lua_calc_mp_atk_hurthp_1", lua_calc_mp_atk_hurthp_1);
	REGISTER_LUA_FUN(g_lua_state, "lua_calc_mp_atk_hurthp_2", lua_calc_mp_atk_hurthp_2);
	REGISTER_LUA_FUN(g_lua_state, "lua_set_atk_skill", lua_set_atk_skill);
	REGISTER_LUA_FUN(g_lua_state, "lua_has_atk_type", lua_has_atk_type);
	REGISTER_LUA_FUN(g_lua_state, "lua_calc_skill_cost_mp_1", lua_calc_skill_cost_mp_1);
	REGISTER_LUA_FUN(g_lua_state, "lua_calc_skill_cost_mp_2", lua_calc_skill_cost_mp_2);
	REGISTER_LUA_FUN(g_lua_state, "lua_calc_skill_cost_mp_3", lua_calc_skill_cost_mp_3);
	REGISTER_LUA_FUN(g_lua_state, "lua_chk_skill_by_weapon", lua_chk_skill_by_weapon);
	REGISTER_LUA_FUN(g_lua_state, "lua_calc_rand_rate", lua_calc_rand_rate);
	REGISTER_LUA_FUN(g_lua_state, "lua_cert_rand_num", lua_cert_rand_num);
}


