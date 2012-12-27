/**
 *============================================================
 *     @file      skill_imme_effect.h
 *     @brief     skill immediately related functions are declared here
 *      
 *     compiler   gcc4.1.2
 *     platform   Linux
 *      
 *     copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
**/

#ifndef _SKILL_IMME_EFFECT_H_
#define _SKILL_IMME_EFFECT_H_

/*
enum  skill_effect
{
	skill_begin_effect = 0,
	skill_recover_life_full = 1, //马上回满一个player对象的血量	
	skill_recover_life_percent = 2, //回血30%
	skill_end_effect 
}*/

typedef bool (* effect_func_point)(Player*, Player*,  base_effect* pEffect);


/**
 *     @brief process skill effect
 *     @param Player* , skill_effect Player*
 *     @return  true sucess,  false otherwirse fail
**/
bool process_skill_effect_logic(Player* p, uint32_t effect_id,  Player* target);

/**
 *   @brief init effect function table
 *   @param void
 *   @return  true sucess,  false otherwirse fail
**/
bool init_effect_func_table();


/**
 *    @brief final effect function table
 *    @param void
 *    @return  true sucess,  false otherwirse fail
**/
bool final_effect_func_table();



#endif
