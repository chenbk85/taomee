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

#include "utils.h"
#include "bt_skill.h"
#include "bt_bat_check.h"

#include "global.h"

void init_attck_check_func() 
{
	for (uint32_t loop = 0; loop < MAX_FUNC_NUM; loop ++) {
		check_sp_atk_func[loop] = normal_check_warrior_atk;
	}
	
	check_sp_atk_func[skill_run_away]          = run_away_check_atk;
	check_sp_atk_func[skill_pet_run_away]      = run_away_check_atk;
	
	check_sp_atk_func[skill_user_break_off]    = run_away_check_atk;
	check_sp_atk_func[skill_pet_break_off]     = run_away_check_atk;
	// 
	
	check_sp_atk_func[skill_chg_position]      = chg_pos_check_warrior_atk;
	check_sp_atk_func[skill_pet_chg_position]      = pet_chg_pos_check_warrior_atk;
	check_sp_atk_func[skill_catch_pet]		   = catch_pet_check_warrior_atk;
	check_sp_atk_func[skill_recall_pet]		   = recall_pet_check_warrior_atk;
	check_sp_atk_func[skill_pa_dai_ji]		   = daiji_check_warrior_atk;
	check_sp_atk_func[skill_pa_pet_dai_ji]	   = daiji_check_warrior_atk;
	
	
	check_sp_atk_func[skill_pd_fangyu]         = fang_yu_check_atk;
	check_sp_atk_func[skill_pd_pet_fangyu]     = fang_yu_check_atk;
	
	check_sp_atk_func[skill_rl_jiushu]         = jiushu_check_warrior_atk;
	check_sp_atk_func[skill_rl_pet_jiushu]     = jiushu_check_warrior_atk;
	check_sp_atk_func[skill_shenyuhuhuan]     = jiushu_check_warrior_atk;

	check_sp_atk_func[skill_rl_pet_jiushu_all_only_svr] = pet_jiushu_all_only_svr_atk;
	
	check_sp_atk_func[skill_use_chemical]  	   = chemical_check_warrior_atk;
	check_sp_atk_func[skill_pet_use_chemical]  	   = chemical_check_warrior_atk;
	check_sp_atk_func[skill_pa_xianfazhiren]   = xianfazhiren_check_warrior_atk;
	check_sp_atk_func[skill_pa_pet_xianfazhiren]   = xianfazhiren_check_warrior_atk;
	
	check_sp_atk_func[skill_pa_zhuiji]         = zhuiji_check_warrior_atk;
	check_sp_atk_func[skill_pa_pet_zhuiji]     = zhuiji_check_warrior_atk;
	
	check_sp_atk_func[skill_pd_huiji]          = huiji_check_warrior_atk;
	check_sp_atk_func[skill_pd_pet_huiji]      = huiji_check_warrior_atk;
	
	check_sp_atk_func[skill_pa_jizhonggongji]  = jizhonggongji_check_warrior_atk;
	check_sp_atk_func[skill_pa_pet_jizhonggongji]  = jizhonggongji_check_warrior_atk;
	
	check_sp_atk_func[skill_pa_ninshenjian]    = ninshenjian_check_warrior_atk;
	check_sp_atk_func[skill_pa_pet_ninshenjian]    = ninshenjian_check_warrior_atk;

	check_sp_atk_func[skill_mas_zaishengzhizhen]  = zaishengzhizhen_check_warrior_atk;
	check_sp_atk_func[skill_mas_pet_zaishengzhizhen]  = zaishengzhizhen_check_warrior_atk;
	// 

	check_sp_atk_func[skill_hw_huwei]				= huwei_check_warrior_atk;
	check_sp_atk_func[skill_hw_pet_huwei]			= huwei_check_warrior_atk;

	check_sp_atk_func[skill_ph_pet_xixuegongji]		 = xixuegongji_check_warrior_atk;
	
	check_sp_atk_func[skill_fh_chouqu]				 = t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_fh_pet_chouqu]			 = t_attacked_check_warrior_atk;

	check_sp_atk_func[skill_lunzhuanfengbao]		= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_yunshishu]			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_shuangdongshu]		= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_liuhuoshu]			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_xuanfengshu]			= t_attacked_check_warrior_atk;

	check_sp_atk_func[skill_ma_pet_yunshishu]			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_pet_shuangdongshu]		= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_pet_liuhuoshu]			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_pet_xuanfengshu] 		= t_attacked_check_warrior_atk;

	check_sp_atk_func[skill_ma_jushizhou] 			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_hanbingzhou]			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_liehuozhou] 			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_kuangfengzhou]		= t_attacked_check_warrior_atk;
	
	check_sp_atk_func[skill_ma_liuxingyu]			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_youyuanbing]			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_diyuhuo]				= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_longjuanren]			= t_attacked_check_warrior_atk;
	
	check_sp_atk_func[skill_ma_pet_jushizhou] 			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_pet_hanbingzhou]			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_pet_liehuozhou] 			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_pet_kuangfengzhou]		= t_attacked_check_warrior_atk;
	
	check_sp_atk_func[skill_ma_pet_liuxingyu]			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_pet_youyuanbing] 		= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_pet_diyuhuo] 			= t_attacked_check_warrior_atk;
	check_sp_atk_func[skill_ma_pet_longjuanren] 		= t_attacked_check_warrior_atk;
		

}

//检查技能
void check_skill_by_weapon_prof(warrior_t* p, atk_info_t* pai)
{	
	//内部技能不检查
	if (pai->atk_type == skill_rl_pet_jiushu_all_only_svr)
		return ;
	skill_attr_t* psa = get_skill_attr(pai->atk_type, p->weapon_type);
	//DEBUG_LOG("xxxxxxx  %u %u %u",psa!=NULL,(psa!=NULL)?psa->id:0,psa->usr_type);
	if (!psa || !psa->id || (IS_BEAST(p) && pai->atk_type < skill_pet_begin) 
		|| (!IS_BEAST(p) && pai->atk_type > skill_pet_begin)
		|| (p->petid && psa->usr_type != user_pet_beast)) {
		KERROR_LOG(p->userid, "skill invalid\t[%u %u %u]", p->petid, pai->atk_type, p->weapon_type);
		pai->atk_mark = 1; // enemy
		pai->atk_type = (IS_BEAST(p)) ? skill_pa_pet_dai_ji : skill_pa_dai_ji;
		pai->atk_level = 1;
		return;
	} 

	// check skill by prof
	skill_mp_exp_t* pma = get_skill_mp_exp(pai->atk_type, p->prof);
	if (!pma || !pma->id){
		KERROR_LOG(p->userid, "mp info invalid\t[petid=%u atk_type=%u prof=%u]", p->petid, pai->atk_type, p->prof);
		pai->atk_mark = 1; // enemy
		pai->atk_type = (IS_BEAST(p)) ? skill_pd_pet_fangyu : skill_pd_fangyu;
		pai->atk_level = 1;
		return;
	}

	// 检查是否需要盾牌
	if ((psa->needshield == must_have_shield && !p->shield) || (psa->needshield == must_not_shield && p->shield)) {
		ERROR_LOG("shield invalid\t[uid=%u petid=%u atk_type=%u prof=%u]", p->userid, p->petid, pai->atk_type, p->prof);
		pai->atk_mark = 0; // enemy
		pai->atk_type = (IS_BEAST(p)) ? skill_pa_pet_dai_ji : skill_pa_dai_ji;
		pai->atk_level = 1;
		return;
	}

	// ren can only send skill + normal attack
	if (!IS_BEAST_ID(p->userid) && pai->atk_seq){//第二招
		switch(p->atk_info[0].atk_type){
			case skill_run_away:
			case skill_pa_base_fight:
			case skill_pd_fangyu:
			case skill_md_mokang:
			case skill_pd_huandun:
			case skill_pd_huiji:
			case skill_pd_pet_fangyu:
			case skill_md_pet_mokang:
			case skill_pd_pet_huandun:
			case skill_pd_pet_huiji:
			case skill_pa_pet_base_fight:
			case skill_pa_dai_ji:
			case skill_pa_pet_dai_ji:
				break;
			default:
				switch(pai->atk_type){
					case skill_run_away:
					case skill_pa_base_fight:
					case skill_pd_fangyu:
					case skill_md_mokang:
					case skill_pd_huandun:
					case skill_pd_huiji:
					case skill_pd_pet_fangyu:
					case skill_md_pet_mokang:
					case skill_pd_pet_huandun:
					case skill_pd_pet_huiji:
					case skill_pa_pet_base_fight:
					case skill_pa_dai_ji:
					case skill_pa_pet_dai_ji:
						break;
					default:
						pai->atk_type = IS_BEAST(p)?skill_pa_pet_dai_ji:skill_pa_dai_ji;
						pai->atk_level = 1;
						break;
				}
		}
	}
}

atk_info_t* xixuegongji_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	if (!IS_BEAST(p)){
		pai->atk_mark = 0;
		pai->atk_pos  = 0;
		pai->atk_type = skill_pa_pet_dai_ji;
		pai->atk_level = 1;
		return pai;
	}
	
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, pai);
	if (!lp || pai->atk_pos == -1){
		return NULL;
	}

	return pai;
}

atk_info_t* normal_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	//DEBUG_LOG("normal_check_warrior_atk");
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, pai);
 	if (!lp) {
		return NULL;
	}

	return pai;
}




uint8_t check_mp_enough( warrior_t* p, atk_info_t* aai)
{
	if (aai->atk_type == 0){
		return 0;
	}

	// find skill from table by atk_type and prof
	skill_mp_exp_t* psu = get_skill_mp_exp(aai->atk_type, p->prof);
	if (psu && psu->id){
		int res_mp = p->mp;
		res_mp -= (psu->mp_a * aai->atk_level + psu->mp_b);
		// mp is not enough for current attack skill
		return res_mp >= 0;
	}

	// invalid skill
	return 0;
}

void check_one_atk_info_by_mp(warrior_t* p, atk_info_t* aai, int& res_mp)
{
	skill_mp_exp_t* psu = get_skill_mp_exp(aai->atk_type, p->prof);
	if (!psu || !psu->id || aai->confrm_decre_mp != confrm_decre_mp_no_need)
		return;
	
	res_mp -= (psu->mp_a * aai->atk_level + psu->mp_b);
	if (res_mp < 0){
		KERROR_LOG(p->userid, "mp not enough\t[atk_type=%u atk_level=%u]", aai->atk_type, aai->atk_level);
		if (IS_BEAST_ID(p->userid)){
			aai->atk_type = skill_pa_pet_base_fight;
			if (!aai->atk_mark){
				aai->atk_mark = 1;
				aai->atk_pos = get_rand_alive_warrior_pos(p->enemy_team,p->pet_state);
			}
		} else {
			aai->atk_type = IS_BEAST(p)?skill_pa_pet_dai_ji:skill_pd_fangyu;
		}

		aai->atk_level = 1;
	}
}

void check_attack_type_by_mp(warrior_t* p, atk_info_t* aai)
{
	if (aai->atk_type == 0)
		return;

	int res_mp = p->mp;
	if (aai->atk_seq)
		check_one_atk_info_by_mp(p, &p->atk_info[0], res_mp);
	check_one_atk_info_by_mp(p, aai, res_mp);
}
//检查攻击的合法性
void check_user_step_attack_and_insert_atk_list(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{

	KDEBUG_LOG(p->userid,"check_user_step_attack_and_insert_atk_list:skill=%u pos=%u",pai->atk_type,pai->atk_pos);
	pai->atk_speed = p->speed;
	if (pai->atk_type != skill_pet_break_off && pai->atk_type != skill_user_break_off && CANNOT_ATTACK(p)) {
		clear_warrior_atkinfo(abi, p);
		return;
	}
	//KDEBUG_LOG(p->userid,"add pai->atk_type:%u %u %u",WARRIOR_SHIHUA(p),WARRIOR_HUNSHUI(p),p->p_waor_state->check_state(hunshui_bit));

	KDEBUG_LOG(p->userid,"check point(0):skill=%u pos=%u mark=%u",pai->atk_type,pai->atk_pos,pai->atk_mark);
	if(p->p_waor_state->state & yiwang) {
		for(int i=0;i<p->skills_forgot_cnt;i++) {
			if(p->skills_forgot[i] == pai->atk_type) {
				pai->atk_level = 1;
				pai->atk_type = IS_BEAST(p) ? skill_pa_pet_dai_ji : skill_pa_dai_ji;
				break;
			}
		}
	} else if(p->p_waor_state->state & hunluan) {
		if(rand() & 1) {
			pai->atk_level = 1;
			pai->atk_type = IS_BEAST(p) ? skill_pa_pet_base_fight : skill_pa_base_fight;
			if(rand() & 1) {
				pai->atk_mark = 0;
				pai->atk_pos = get_front_rand_alive_warrior_pos(p->self_team);
			} else {
				pai->atk_mark = 1;
				pai->atk_pos = get_front_rand_alive_warrior_pos(p->enemy_team);
			}
			DEBUG_LOG("ATK POS\t[%d]", pai->atk_pos);
		}
	} else if( WARRIOR_SHIHUA(p) || WARRIOR_HUNSHUI(p)) {//shihua:daiji
		pai->atk_level = 1;
		pai->atk_type = IS_BEAST(p) ? skill_pa_pet_dai_ji : skill_pa_dai_ji;
	}


	// check skill by weapon and prof
	KDEBUG_LOG(p->userid,"check point(1):skill=%u pos=%u mark=%u",pai->atk_type,pai->atk_pos,pai->atk_mark);
	check_skill_by_weapon_prof(p, pai);
	KDEBUG_LOG(p->userid,"check point(2):skill=%u pos=%u mark=%u",pai->atk_type,pai->atk_pos,pai->atk_mark);
	check_attack_type_by_mp(p, pai);
	//检查人物等级 TODO
	if((p->level/10+1)<pai->atk_level){
		KERROR_LOG(p->userid, "level invalid\t[%u %u %u]", p->level, pai->atk_type, pai->atk_level);
		pai->atk_level = (p->level/10+1);
	}
		

	KDEBUG_LOG(p->userid,"check point(3):skill=%u pos=%u mark=%u",pai->atk_type,pai->atk_pos,pai->atk_mark);

	atk_info_t* ret_pai = check_sp_atk_func[pai->atk_type](abi, p, pai);

	KDEBUG_LOG(p->userid,"check point(4):skill=%u pos=%u mark=%u",pai->atk_type,pai->atk_pos,pai->atk_mark);

	if (ret_pai){
		if (pai->atk_seq == 1){
		   	//是第二招
			//KDEBUG_LOG(p->userid,"speedddddd %u",p->type_id);
			if(!p->is_boss()){
				pai->atk_speed /= 2;
				KDEBUG_LOG(p->userid,"speed half");
			}
		}
		//插入到攻击列表
		insert_atkinfo_to_list(abi, ret_pai);
	}
}

atk_info_t* run_away_check_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	pai->atk_mark = 0;
	pai->atk_pos = get_warrior_pos(p->self_team, p->userid, p->petid);
	pai->atk_level = 1;

	if (p->petid){
		switch(pai->atk_type){
		case skill_run_away:
		case skill_pet_run_away:
			pai->atk_type = skill_pa_pet_dai_ji;
			break;
		}
	}
	
	return pai;
}

atk_info_t* daiji_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	pai->atk_mark = 0;
	pai->atk_pos = get_warrior_pos(p->self_team, p->userid, p->petid);
	pai->atk_level = 1;
	return pai;
}

atk_info_t* fang_yu_check_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	pai->atk_mark = 0;
	pai->atk_pos = get_warrior_pos(p->self_team, p->userid, p->petid);
	pai->atk_level = 1;
	return pai;
}

atk_info_t* catch_pet_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	pai->atk_mark = 1;
	return pai;
}

atk_info_t* recall_pet_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	pai->atk_mark = 0;
	return pai;
}

atk_info_t* chg_pos_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{

	if (IS_BEAST(p)){//自己是宠物
		pai->atk_pos = get_rand_alive_warrior_pos(p->enemy_team, p->pet_state);
		pai->atk_mark = 1; // enemy
		pai->atk_type = (IS_BEAST(p)) ? skill_pa_pet_base_fight : skill_pa_base_fight;
		pai->atk_level = 1;
		return pai;
	}
	pai->atk_mark = 0;
	pai->atk_pos = get_pet_pos(abi, p->self_team, p->userid);
	return pai;
}
atk_info_t* pet_chg_pos_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	KDEBUG_LOG(p->userid,"pet_chg_pos_check_warrior_atk:petid:%d ", p->petid);
	if (p->petid){
		pai->atk_mark = 0;
		pai->atk_pos = get_warrior_pos(p->self_team, p->userid,0);
	}else{//是人,则使用普通攻击
		pai->atk_pos = get_rand_alive_warrior_pos(p->enemy_team, p->pet_state);
		pai->atk_mark = 1; // enemy
		pai->atk_type = (IS_BEAST(p)) ? skill_pa_pet_base_fight : skill_pa_base_fight;
		pai->atk_level = 1;
		return pai;
	
	}
	return pai;
}



atk_info_t* jiushu_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, pai);
	if (!lp || pai->atk_pos == -1){
		return NULL;
	}
	
	if (lp->check_state( person_break_off) ||
		lp->check_state( attacked_out_bit) ){
		pai->atk_pos= get_rand_alive_warrior_pos(p->enemy_team, p->pet_state);
		pai->atk_mark = 1; // enemy
		pai->atk_type = (IS_BEAST(p)) ? skill_pa_pet_base_fight : skill_pa_base_fight;
		pai->atk_level = 1;
		return pai;
	}
	
	return pai;
}

atk_info_t* pet_jiushu_all_only_svr_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	pai->atk_speed = 32767;
	return pai;
}


atk_info_t* huwei_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	if (pai->atk_mark || pai->atk_pos < 0 || pai->atk_pos >= (MAX_PLAYERS_PER_TEAM * 2)){
		pai->atk_mark = 0;
		pai->atk_type = (IS_BEAST(p))?skill_pa_pet_dai_ji:skill_pa_dai_ji;
		pai->atk_pos = 0;
		return pai;
	}

	warrior_t* lp = p->self_team->players_pos[pai->atk_pos];
	if (!lp || NONEED_ATTACK(lp) || lp == p || p->huwei_pos){
		pai->atk_mark = 0;
		pai->atk_type = (IS_BEAST(p))?skill_pa_pet_dai_ji:skill_pa_dai_ji;
		pai->atk_pos = 0;
	}

	lp->p_waor_state->set_huwei_level ( pai->atk_level);
	return pai;
}

atk_info_t* chemical_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	if (pai->atk_mark){
		pai->atk_pos= get_rand_alive_warrior_pos(p->enemy_team, p->pet_state);
		pai->atk_mark = 1; // enemy
		pai->atk_type = (IS_BEAST(p)) ? skill_pa_pet_base_fight : skill_pa_base_fight;
		pai->atk_level = 1;
		return pai;
	}

	warrior_t* lp = get_warrior_by_pos(p->self_team, pai->atk_pos);
	// if attackee is not exist or can't be use chemcal
	if (!lp || NONEED_ATTACK(lp)){
		if(lp && IS_REVIVING_MEDICAL(p->p_waor_state->get_use_item_itemid())){
		//2012-3-8 增加，如果是使用复活药则可以对死人使用
			return pai;
		}
		pai->atk_pos = get_rand_alive_warrior_pos(p->self_team,p->pet_state);
		//pai->atk_mark = 0;
	}
	return pai;
}

atk_info_t* xianfazhiren_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, pai);
	if (!lp || pai->atk_pos == -1){
		return NULL;
	}
	p->p_waor_state->set_state_info(pause_bit,1,1 );
	SET_WARRIOR_STATE(p->p_waor_state->state, pause_bit);
	pai->atk_speed = p->speed * (1 + 0.05 * pai->atk_level);
	return pai;
}

atk_info_t* zhuiji_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, pai);
	if (!lp || pai->atk_pos == -1){
		return NULL;
	}
	
	return pai;
}

atk_info_t* huiji_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, pai);
	if (!lp || pai->atk_pos == -1){
		return NULL;
	}
	
	return pai;

}

atk_info_t* jizhonggongji_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, pai);
	if (!lp){
		return NULL;
	}
	
	// wan quan xi sheng sudu
	pai->atk_speed = 0;
	return pai;
}

atk_info_t* ninshenjian_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, pai);
	if (!lp){
		return NULL;
	}
	
	// wan quan xi sheng sudu
	pai->atk_speed = 0;
	return pai;
}

atk_info_t* t_attacked_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	skill_attr_t* psa = get_skill_attr(pai->atk_type, p->weapon_type);
	if (!psa || !psa->id){
		ERROR_LOG("invalid t attack skill\t[batid=%lu uid=%u petid=%u atk_type=%u weapon_type=%u]", abi->batid, p->userid, p->petid, pai->atk_type, p->weapon_type);
		return NULL;
	}
	
	warrior_t* lp = NULL;
	if (!pai->atk_mark){
		switch(psa->target_type){
		case enemy_team:
			pai->atk_pos   = get_rand_alive_warrior_pos(p->enemy_team, p->pet_state);
			pai->atk_mark  = 1; // enemy
			break;
		default:
			lp = get_warrior_by_pos(p->self_team, pai->atk_pos);
			if (!lp){
				pai->atk_pos  = get_rand_alive_warrior_pos(p->self_team,p->pet_state);
			}

			if (pai->atk_pos == -1){
				pai->atk_pos   = get_rand_alive_warrior_pos(p->enemy_team, p->pet_state);
				pai->atk_mark  = 1; // enemy
			}
			break;
		}
	}
	else{
		lp = get_warrior_by_pos(p->enemy_team, pai->atk_pos);
		if (!lp) {
			pai->atk_pos  = get_rand_alive_warrior_pos(p->enemy_team, p->pet_state);
			pai->atk_mark = 1;
		}
	}

	return pai;
}

atk_info_t* all_attacked_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	skill_attr_t* psa = get_skill_attr(pai->atk_type, p->weapon_type);
	if (!psa || !psa->id){
		ERROR_LOG("invalid all attack skill\t[batid=%lu uid=%u petid=%u atk_type=%u weapon_type=%u]", abi->batid, p->userid, p->petid, pai->atk_type, p->weapon_type);
		return NULL;
	}

	if (!pai->atk_mark){
		pai->atk_pos   = get_rand_alive_warrior_pos(p->enemy_team, p->pet_state);
		pai->atk_mark  = 1; // enemy
	}
	else{
		warrior_t* lp = get_warrior_by_pos(p->enemy_team, pai->atk_pos);
		if (!lp) {
			pai->atk_pos  = get_rand_alive_warrior_pos(p->enemy_team, p->pet_state);
			pai->atk_mark = 1;
		}
	}

	return pai;
}

atk_info_t* zaishengzhizhen_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai)
{
	if (pai->atk_mark){
		if (IS_BEAST_ID(p->userid)){
			pai->atk_mark = 0;
			pai->atk_pos = get_rand_alive_warrior_pos(p->self_team,p->pet_state);
			if (pai->atk_pos == -1){
				pai->atk_pos = get_rand_alive_warrior_pos(p->enemy_team,p->pet_state );
				pai->atk_mark = 1;
				pai->atk_type = (IS_BEAST(p)) ? skill_pa_pet_base_fight : skill_pa_base_fight;
				pai->atk_level = 1;
				return pai;
			}
		}else{
			pai->atk_type = (IS_BEAST(p)) ? skill_pa_pet_base_fight : skill_pa_base_fight;
			pai->atk_level = 1;
			return pai;
		}
	}
	else{
		warrior_t* lp = get_warrior_by_pos(p->self_team, pai->atk_pos);
		if (!lp){
			pai->atk_pos = get_rand_alive_warrior_pos(p->enemy_team,p->pet_state);
			pai->atk_mark = 1;
			pai->atk_type = (IS_BEAST(p)) ? skill_pa_pet_base_fight : skill_pa_base_fight;
			pai->atk_level = 1;
			return pai;
		}
	}
	
	return pai;
}


