#include "player.hpp"
#include "skill_imme_effect.hpp"
#include "player_status.hpp"

//static function table 
//static effect_func_point g_table[  skill_end_effect - skill_begin_effect ] = { NULL };

//static bool skill_recover_life_full_process(Player* p, Player* target, base_effect* pEffect);
//static bool skill_recover_life_percent_process(Player* p, Player* target,  base_effect* pEffect);


//-----------------------------------------------------------
//// function implement
////-----------------------------------------------------------



/**
 *    @brief process recover life full effect
 *    @param Player*, Player*
 *    @return  true sucess,  false otherwirse fail
**/
/*
bool skill_recover_life_full_process(Player* p, Player* target,  base_effe)
{
	target->full_hp();
	return true;
}
*/


/*
bool skill_recover_life_percent_process(Player* p, Player* target, uint32_t para)
{
	target->recover_hp(0.2);
	return true;
}
*/
/**
 *    @brief init effect function table
 *    @param void
 *    @return  true sucess,  false otherwirse fail
**/
bool init_effect_func_table()
{
	//g_table[skill_recover_life_full] = skill_recover_life_full_process;
	//g_table[skill_recover_life_percent] = skill_recover_life_percent_process;
	return true;
}


/**
 *   @brief final effect function table
 *   @param void
 *   @return  true sucess,  false otherwirse fail
**/
bool final_effect_func_table()
{
	//for(uint32_t i = 0;  i< sizeof(g_table)/sizeof(g_table[0]); i++)
	//{
	//	g_table[i] = NULL;
	//}
	return true;
}

/**
 *     @brief process effect logic
 *     @param Player*,  skill_effect, Player*
 *     @return  true sucess,  false otherwirse fail
**/
bool process_skill_effect_logic(Player* p, uint32_t effect_id,  Player* target)
{
	if(target == NULL){
		return false;
	}
	/*
	if(  !(effect_type > skill_begin_effect && effect_type < skill_end_effect)){
		return false;
	}
	*/
	/*
	if( g_table[effect_type] != NULL){
		return g_table[effect_type](p, target, para);
	}
	*/	
	if (check_player_immunity_this_effect(target, effect_id)) {
		return false;
	}
	effect_data* pEffectdata = effect_data_mgr::getInstance()->get_effect_data_by_id(effect_id);
	if(pEffectdata == NULL){
		return false;
	}
	base_effect effect_obj;
	const timeval* tv = get_now_tv();
	effect_obj.init_base_effect(pEffectdata, *tv);
	effect_obj.process_effect(target, *tv, p);

	return true;
}
