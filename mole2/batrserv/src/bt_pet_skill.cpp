#ifdef __cplusplus
extern "C"
{
#endif

#include <fcntl.h>
#include <sys/mman.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
//#include  <statistic_agent/msglog.h>
#include "libtaomee/project/stat_agent/msglog.h"

#ifdef __cplusplus
}
#endif

#include "bt_pet_skill.h"
#include "global.h"


int kuangji_pet_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->hit_rate *= (1 - 0.03 * ara->atk_level);
	return 0;
}

int kuangji_pet_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->hit_rate /= (1 - 0.03 * ara->atk_level);
	return 0;
}

int huishang_pet_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->defense_value *= (1 - 0.05 * ara->atk_level);
	p->spirit *= (1 - 0.05 * ara->atk_level);
	p->attack_value *= (1 + 0.07 * ara->atk_level);
	return 0;
}

int huishang_pet_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->defense_value /= (1 - 0.05 * ara->atk_level);
	p->spirit /= (1.0 - 0.05 * ara->atk_level);
	p->attack_value /= (1 + 0.07 * ara->atk_level);
	return 0;
}

resolved_atk_t* base_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* pwar = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!pwar){
		return NULL;
	}
		
	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = pwar->userid;
	pra->atk_petid= pwar->petid;
	pra->hurthp_rate = 100;
	return pra;
}

resolved_atk_t* kuangji_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (lp == NULL){
		return NULL;
	}

	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid= lp->petid;
	pra->hurthp_rate = 100 + 10 * p_atk_info->atk_level;
	return pra;
}

resolved_atk_t* chanshentengman_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (lp == NULL){
		return NULL;
	}

	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid= lp->petid;
	pra->hurthp_rate = 100 + 1 + 3 * (p_atk_info->atk_level - 1);
	return pra;
}


resolved_atk_t* meiying_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
	
	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid = lp->petid;
	pra->hurthp_rate = 100;
	return pra;
}

resolved_atk_t* huishang_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
		
	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid = lp->petid;
	pra->hurthp_rate = 100;
	return pra;
}

resolved_atk_t* zhuiji_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	int cnt = (p_atk_info->atk_level - 1) / 2 + 2;
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (lp == NULL || NONEED_ATTACK(lp)){
		return NULL;
	}
	
	RESET_WARRIOR_STATE(p->p_waor_state->one_loop_state, one_loop_bit_huiji);
	
	int loop;
	for (loop = 0; loop < cnt; loop++) {
		resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
		pra->atk_uid = lp->userid;
		pra->atk_petid = lp->petid;
		pra->hurthp_rate = 100;
		switch(p_atk_info->atk_level){
		case 1:
		case 2:
			pra->hurthp_rate *= 0.7;
			break;
		case 3:
		case 4:
			pra->hurthp_rate *= 0.5;
			break;
		case 5:
		case 6:
			pra->hurthp_rate *= 0.4;
			break;
		case 7:
		case 8:
			pra->hurthp_rate *= 0.34;
			break;
		case 9:
		case 10:
			pra->hurthp_rate *= 0.3;
			break;
		}
			
		calc_warrior_single_step_battle_result(abi, pra, p_atk_info);
		
		if (abi->challger.ren_alive_count <= 0 || abi->challgee.ren_alive_count <= 0){
			break;
		}
		
		if (CANNOT_ATTACK(p) || CHK_WARRIOR_STATE(p->p_waor_state->one_loop_state, one_loop_bit_huiji)){
			break;
		}
	
		if ((loop + 1) < cnt && (p_atk_info->atk_level % 2 || NONEED_ATTACK(lp))) {
			p_atk_info->atk_pos = -1;
			lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
			if (lp == NULL || NONEED_ATTACK(lp)){
				break; // all ememy are died!
			}
		}
	}
	return NULL;

}

resolved_atk_t* bengya_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (lp == NULL){
		return NULL;
	}

	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid = lp->petid;
	
	pra->hurthp_rate = 99 + 20 * p_atk_info->atk_level + p_atk_info->atk_level * p_atk_info->atk_level;
	if (p_atk_info->atk_level == 10){
		pra->hurthp_rate += 1;
	}
	return pra;
}

resolved_atk_t* pohun_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (lp == NULL){
		return NULL;
	}

	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid = lp->petid;
	
	pra->hurtmp_rate = 5 * p_atk_info->atk_level;
	return pra;
}

resolved_atk_t* xixuegongji_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (lp == NULL){
		return NULL;
	}
	
	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid = lp->petid;
	pra->hurthp_rate = 100;
	return pra;
}

resolved_atk_t* shengmingchouqu_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
	
	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid = lp->petid;
	pra->hurthp_rate = 100;
	return pra;
}


resolved_atk_t* mingsi_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
	
	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid = lp->petid;
	
	switch(p_atk_info->atk_level){
		case 10:
			pra->hurthp_rate = (1 + rand() % 6) * 10;
			break;
		default:
			pra->hurthp_rate = 5 + (rand() % 6) * p_atk_info->atk_level;
			break;
	}
	return pra;
}

resolved_atk_t* zhiyuzhizhen_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (lp == NULL){
		return NULL;
	}
	
	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid = lp->petid;
	pra->hurthp_rate = 300 * p_atk_info->atk_level;
	return pra;
}

resolved_atk_t* zhiyujiejie_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	battle_users_t* ally = p->self_team;
	int loop;
	for (loop = 0; loop < MAX_PLAYERS_PER_TEAM * 2; loop++) {
		warrior_t* lp = ally->players_pos[loop];
		if (!lp || NONEED_ATTACK(lp)){
			continue;
		}

		// alloc resolve attack info
		resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
		pra->atk_uid = lp->userid;
		pra->atk_petid = lp->petid;
		pra->hurthp_rate = 120 * p_atk_info->atk_level;
		calc_warrior_single_step_battle_result(abi, pra, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* t_attacked_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	battle_users_t* enemy_team = p_atk_info->atk_mark ? p->enemy_team : p->self_team;
	
	int pos = get_valid_t_attack_cross_pos_ex(p_atk_info->atk_pos, enemy_team); 
	if (pos == -1){
		ERROR_LOG("t_attack cross pos is not valid-pet\t[batid=%lu uid=%u petid=%u atk_type=%u weapon_type=%u]", abi->batid, p->userid, p->petid, p_atk_info->atk_type, p->weapon_type);
		return NULL;
	}
	
	int loop;
	for (loop = 0; loop < T_USER_MAX_NUM; loop++) {
		int lpos = t_users[pos][loop];
		if (lpos == -1){
			continue; 
		}
		
		warrior_t* lp = enemy_team->players_pos[lpos];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}
		
		resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
		pra->atk_uid = lp->userid;
		pra->atk_petid = lp->petid;
		pra->hurthp_rate = get_attack_hurthp_rate(p, p_atk_info, abi, enemy_team);
		calc_warrior_single_step_battle_result(abi, pra, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* all_attacked_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	battle_users_t* enemy_team = p_atk_info->atk_mark ? p->enemy_team : p->self_team;
	for (int loop = 0; loop < MAX_PLAYERS_PER_TEAM * 2; loop++) {
		warrior_t* lp = enemy_team->players_pos[loop];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode() ){
			continue;
		}
		resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
		pra->atk_uid = lp->userid;
		pra->atk_petid = lp->petid;
		pra->hurthp_rate = get_attack_hurthp_rate(p, p_atk_info, abi, enemy_team);
		calc_warrior_single_step_battle_result(abi, pra, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* zaishengzhizhen_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* pwar = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!pwar){
		return NULL;
	}
	
	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = pwar->userid;
	pra->atk_petid = pwar->petid;
	pra->hurthp_rate = 100*p_atk_info->atk_level;
	return pra;

}

resolved_atk_t* zaishenglingyu_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	battle_users_t* ally = p->self_team;
	if(p_atk_info->atk_mark) ally = p->enemy_team;
	int pos = p_atk_info->atk_pos;
	if (pos < 0 || pos >= 10){
		return NULL;
	}

	// DEBUG_LOG("T_USER\t[%u %u %u]", pos, p_atk_info->atk_uid, p_atk_info->atk_petid);
	// if cross player is died, we need to move cross postion
	int loop;
	for (loop = 0; loop < T_USER_MAX_NUM; loop++) {
		int lpos = t_users[pos][loop];
		if (lpos == -1){
			continue;
		}

		warrior_t* lp = ally->players_pos[lpos];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}

		resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
		pra->atk_uid = lp->userid;
		pra->atk_petid = lp->petid;
		pra->hurthp_rate = 60*p_atk_info->atk_level;
		calc_warrior_single_step_battle_result(abi, pra, p_atk_info);
	}
	return NULL;

}

resolved_atk_t* zaishengjiejie_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	battle_users_t* ally = p->self_team;
	int loop;
	if(p_atk_info->atk_mark) ally = p->enemy_team;
	for (loop = 0; loop < MAX_PLAYERS_PER_TEAM * 2; loop++) {
		warrior_t* lp = ally->players_pos[loop];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode() ){
			continue;
		}
	
		// alloc resolve attack info
		resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
		pra->atk_uid = lp->userid;
		pra->atk_petid = lp->petid;
		pra->hurthp_rate = 40*p_atk_info->atk_level;
		calc_warrior_single_step_battle_result(abi, pra, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* duochongjian_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	int counts[MAX_PLAYERS_PER_TEAM * 2] = {0};
	int cnt = p_atk_info->atk_level + 2;
	int loop;
	
	warrior_t* lpememy = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lpememy){
		return NULL;
	}
	
	for (loop = 0; loop < cnt; loop++) {
		warrior_t* lp = (!loop) ? lpememy : get_valid_attack_warrior_ex(abi, p, p_atk_info);
		if (lp == NULL){
			break;
		}

		int idx = get_warrior_pos(enemy, lp->userid, lp->petid);
		
		counts[idx]++;
		resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
		pra->atk_uid = lp->userid;
		pra->atk_petid = lp->petid;
		pra->hurthp_rate = (counts[idx] < 3) ? (35 - counts[idx] * 10) : 5;

		calc_warrior_single_step_battle_result(abi, pra, p_atk_info);
		p_atk_info->atk_pos = -1;
	}
	return NULL;
}

