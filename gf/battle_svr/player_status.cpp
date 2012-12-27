#include "player.hpp"
#include "buff.hpp"
#include "aura.hpp"
#include "player_status.hpp"
#include "stage.hpp"
#include "battle_impl.hpp"
/*----------------------------internal function----------------------------------------*/
//init player buff list
static bool init_player_bufflist(Player* p);
//final player buff list
static bool final_player_bufflist(Player* p);
//init player aura list
static bool init_player_auralist(Player* p);
//final player aura list
static bool final_player_auralist(Player* p);

//get player atk value by buff
static int get_player_additional_atk_by_buff(Player*p, buff* pBuff);
//get player atk value by aura
static int get_player_additional_atk_by_aura(Player*p, aura* pAura);
//get player atk value by effect
static int get_player_additional_atk_by_effect(Player* p,  base_effect* pEffect);

//get player def rate by buff
static float get_player_additional_def_rate_by_buff(Player*p, buff* pBuff);
//get player def value by buff
int get_player_additional_def_value_by_buff(Player*p, buff* pBuff);

//get player def rate by aura
static float get_player_additional_def_rate_by_aura(Player*p, aura* pAura);
//get player def rate by effect
static float get_player_additional_def_rate_by_effect(Player* p,  base_effect* pEffect);
//get player def value by effect
int get_player_additional_def_value_by_effect(Player* p,  base_effect* pEffect);


//get player ciritical rate by buff
static int get_player_additional_critical_rate_by_buff(Player*p, buff* pBuff);
//get player critical rate by aura
static int get_player_additional_critical_rate_by_aura(Player*p, aura* pAura);
//get player critical rate by effect
static int get_player_additional_critical_rate_by_effect(Player* p,  base_effect* pEffect);

//get player hit rate by buff
static float get_player_additional_hit_rate_by_buff(Player*p, buff* pBuff);
//get player hit rate by aura
static float get_player_additional_hit_rate_by_aura(Player*p, aura* pAura);
//get player hit rate by effect
static float get_player_additional_hit_rate_by_effect(Player* p,  base_effect* pEffect);


//get player hit rate by buff
static float get_player_additional_dodge_rate_by_buff(Player*p, buff* pBuff);
//get player hit rate by aura
static float get_player_additional_dodge_rate_by_aura(Player*p, aura* pAura);
//get player hit rate by effect
static float get_player_additional_dodge_rate_by_effect(Player* p,  base_effect* pEffect);

//get player exp factor by buff
static float get_player_additional_exp_factor_by_buff(Player*p, buff* pBuff);
//get player exp factor by aura
static float get_player_additional_exp_factor_by_aura(Player*p, aura* pAura);
//get player exp factor by effect
static float get_player_additional_exp_factor_by_effect(Player* p,  base_effect* pEffect);
//get player's summon mon exp factor by  buff
//static float get_summon_mon_additional_exp_factor_by_buff(Player* p,  base_effect* pEffect);
//get player's summon mon exp factor by effect
static float get_summon_mon_additional_exp_factor_by_effect(Player* p,  base_effect* pEffect);
//get player only exp factor by  buff
static float get_player_only_additional_exp_factor_by_buff(Player*p, buff* pBuff);
//get player only exp factor by effect
static float get_player_only_additional_exp_factor_by_effect(Player* p,  base_effect* pEffect);

//get player max hp by buff
static int get_player_additional_max_hp_by_buff(Player* p, buff* pBuff);
//get player max hp by buff
static int get_player_additional_max_hp_by_aura(Player* p, aura* pAura);
//get player max hp by effect
static int get_player_additional_max_hp_by_effect( Player* p,  base_effect* pEffect);

//get player max mp by buff
static int get_player_additional_max_mp_by_buff(Player* p, buff* pBuff);
//get player max mp by aura
static int get_player_additional_max_mp_by_aura(Player* p, aura* pAura);
//get player max mp by effect
static int get_player_additional_max_mp_by_effect( Player* p,  base_effect* pEffect);


//get player skill cool down time factor by buff
static float get_player_skill_cd_time_factor_by_buff(Player*p, buff* pBuff);
//get player skill cool down time factor by aura
static float get_player_skill_cd_time_factor_by_aura(Player*p, aura* pAura);
//get player skill cool down time factor by effect
static float get_player_skill_cd_time_factor_by_effect(Player* p,  base_effect* pEffect);



//get player skill mana factor by buff
static float get_player_skill_mana_factor_by_buff(Player*p, buff* pBuff);
//get player skill mana factor by aura
static float get_player_skill_mana_factor_by_aura(Player*p, aura* pAura);
//get player skill mana factor by effect
static float get_player_skill_mana_factor_by_effect(Player* p,  base_effect* pEffect);

//get player atk damage change factor by buff
static float get_player_atk_damage_change_factor_by_buff(Player*p, buff* pBuff);
//get player agility change factor by buff
static int get_player_agility_change_factor_by_buff(Player*p, buff* pBuff);
//get player body quality change factor by buff
static int get_player_body_quality_change_factor_by_buff(Player*p, buff* pBuff);

static int get_player_agility_change_factor_by_aura(Player*p, aura* pAura);

static int get_player_body_quality_change_factor_by_aura(Player*p, aura* pAura);

//get player atk damage change factor by aura
static float get_player_atk_damage_change_factor_by_aura(Player*p, aura* pAura);
//get player atk damage change factor by effect
static float get_player_atk_damage_change_factor_by_effect(Player* p,  base_effect* pEffect);
//get player agility change factor by effect
static int get_player_agility_change_factor_by_effect(Player* p,  base_effect* pEffect);
//get player body quality change factor by effect
static int get_player_body_quality_change_factor_by_effect(Player* p,  base_effect* pEffect);

//get player sustain damage change factor by buff
static float get_player_sustain_damage_change_factor_by_buff(Player*p, buff* pBuff);
//get player sustain damage change factor by aura
static float get_player_sustain_damage_change_factor_by_aura(Player*p, aura* pAura);
//get player sustain damage change factor by effect
static float get_player_sustain_damage_change_factor_by_effect(Player* p,  base_effect* pEffect);


//get player speed change factor by buff
static float get_player_speed_change_factor_by_buff(Player*p, buff* pBuff);
//get player speed change factor by aura
static float get_player_speed_change_factor_by_aura(Player*p, aura* pAura);
//get player speed change factor by effect
static float get_player_speed_change_factor_by_effect(Player*p, base_effect* pEffect);

//get player greed rate factor
static int get_player_greed_rate_factor_by_effect(Player * p, base_effect * pEffect);
static int get_player_greed_rate_factor_by_aura(Player *p, aura * pAura);
static int get_player_greed_rate_factor_by_buff(Player * p, buff * pBuff);

static int get_player_sustain_damage_value_factor_by_effect(Player * p, base_effect * pEffect);
static int get_player_sustain_damage_value_factor_by_aura(Player * p, aura * pEffect);
static int get_player_sustain_damage_value_factor_by_buff(Player * p, buff * pEffect);

static int get_player_critical_max_damage_factor_by_buff(Player * p, buff * pBuff);

static int get_player_critical_max_damage_factor_by_aura(Player * p, aura* pAura);

static int get_player_critical_max_damage_factor_by_effect(Player * p, base_effect * pEffect);



//is effect exist in buff
bool is_effect_exist_in_buff(buff* pBuff, uint32_t effect_type);
//is effect exist in aura
bool is_effect_exist_in_aura(aura* pAura, uint32_t effect_type);

//get effect in buff
effect_data* get_effect_data_in_buff(buff* pBuff, uint32_t effect_type);
//get effect in aura
effect_data* get_effect_data_in_aura(aura* pAura, uint32_t effect_type);






////////////////////////////////////////////////////////////////////////////////


/**
 *   @brief get player additional max mp by buff
 *   @param Player*,  buff*
 *   @return  int
**/

int get_player_additional_max_mp_by_buff(Player* p, buff* pBuff)
{
	if(p == NULL || pBuff == NULL)return 0;
	int value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_max_mp_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *   @brief get player additional max mp by aura
 *   @param Player*, aura*
 *   @return  int
**/

int get_player_additional_max_mp_by_aura(Player* p, aura* pAura)
{
	if(p == NULL || pAura == NULL)return 0;
	int value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_max_mp_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *   @brief get player additional max mp by effect
 *   @param Player*, base_effect*
 *   @return  int
**/

int get_player_additional_max_mp_by_effect( Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0;
	}
	int value = 0;
	int type = (int)pData->effect_type;
	int maxmp = 0;
	switch(type)
	{
		case player_add_max_mp_effect_type:
		{
			if (is_summon_mon(p->role_type)) {
				maxmp = p->summon_info.max_fight_value;
			} else {
				maxmp = p->maxmp;
			}
			value += maxmp * pData->trigger_percent/100;
			value += pData->trigger_value;
		}
	}
	return value;
}

/**
 *   @brief get player additional hp by buff
 *   @param Player*, buff*
 *   @return  int
**/

int get_player_additional_max_hp_by_buff(Player* p, buff* pBuff)
{
	if(p == NULL || pBuff == NULL)return 0;
	int value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_max_hp_by_effect(p, pItr->second);
	}
	return value;
}


/**
 *   @brief get player additional max_hp  by aura
 *   @param Player*, aura*
 *   @return  int
**/

int get_player_additional_max_hp_by_aura(Player* p, aura* pAura)
{
	if(p == NULL || pAura == NULL)return 0;
	int value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_max_hp_by_effect(p, pItr->second);
	}
	return value;
}


/**
 *   @brief get player additional exp factor rate by base_effect
 *   @param Player*, base_effect*
 *   @return  float
**/

int get_player_additional_max_hp_by_effect( Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0;
	}
	int value = 0;
	int type = (int)pData->effect_type;
	switch(type)
	{
		case player_add_max_hp_effect_type:
		{
			value += p->maxhp * pData->trigger_percent/100;
			value += pData->trigger_value;
		}
	}
	return value;
}

/**
 *   @brief get player additional exp factor rate by buff
 *   @param Player*, buff*
 *   @return  float
**/

float get_player_additional_exp_factor_by_buff(Player*p, buff* pBuff)
{
	if(p == NULL || pBuff == NULL){
		return 0.0;
	}
	float value = 0.0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_exp_factor_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *   @brief get player's summon mon additional exp factor rate by buff
 *   @param Player*, buff*
 *   @return  float
**/

float get_summon_mon_additional_exp_factor_by_buff(Player*p, buff* pBuff)
{
	if(p == NULL || pBuff == NULL){
		return 0.0;
	}
	float value = 0.0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_summon_mon_additional_exp_factor_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *   @brief get player's summon mon additional exp factor rate by buff
 *   @param Player*, buff*
 *   @return  float
**/
float get_player_only_additional_exp_factor_by_buff(Player*p, buff* pBuff)
{
	if(p == NULL || pBuff == NULL){
		return 0.0;
	}
	float value = 0.0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_only_additional_exp_factor_by_effect(p, pItr->second);
	}
	return value;

}

/**
 *   @brief get player additional exp factor rate by aura
 *   @param Player*, aura*
 *   @return  float
**/

float get_player_additional_exp_factor_by_aura(Player*p, aura* pAura)
{
	if(p == NULL || pAura == NULL){
		return 0.0;
	}
	float value = 0.0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_exp_factor_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *   @brief get player additional exp factor rate by base_effect
 *   @param Player*, base_effect*
 *   @return  float
**/

float get_player_additional_exp_factor_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	float value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case player_get_exp_factor_effect_type:
		{
			value += 1 * pData->trigger_percent/100.0;
		}
		break;
	}
	return value;
}

/**
 *   @brief get player's summon mon additional exp factor rate by base_effect
 *   @param Player*, base_effect*
 *   @return  float
**/

float get_summon_mon_additional_exp_factor_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	float value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case summon_mon_get_exp_factor_effect_type:
		{
			value += 1.0 * pData->trigger_percent/100.0;
		}
		break;
	}
	return value;
}

/**
 *   @brief get player only  additional exp factor rate by base_effect
 *   @param Player*, base_effect*
 *   @return  float
**/

float get_player_only_additional_exp_factor_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	float value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case player_only_get_exp_factor_effect_type:
		{
			value += 1.0 * pData->trigger_percent/100.0;
		}
		break;
	}
	return value;

}

/**
 *   @brief get player additional dodge rate rate by buff
 *   @param Player*, buff*
 *   @return  float
**/

float get_player_additional_dodge_rate_by_buff(Player*p, buff* pBuff)
{
	if(p == NULL || pBuff == NULL){
		return 0.0;
	}
	float value = 0.0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_dodge_rate_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *   @brief get player additional dodge rate rate by aura
 *   @param Player*, aura*
 *   @return  float
**/

float get_player_additional_dodge_rate_by_aura(Player*p, aura* pAura)
{
	 if(p == NULL || pAura == NULL){
		return 0.0;
	 }
	 float value = 0.0;
	 std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();	
	 for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	 {
 		 value += get_player_additional_dodge_rate_by_effect(p, pItr->second);							     
	 }
	 return value;
}



/**
 *   @brief get player additional dodge rate rate by effect
 *   @param Player*, base_effect*
 *   @return  float
**/

float get_player_additional_dodge_rate_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	float value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case add_player_dodge_rate_percent_effect_type:
		{
			value += p->dodge_rate* pData->trigger_percent/100.0;
		}
		break;

		case reduce_player_dodge_rate_percent_effect_type:
		{
			value -= p->dodge_rate* pData->trigger_percent/100.0;
		}
		break;
	}
	return value;
}





/**
 *   @brief get player additional hit rate by buff
 *   @piaram Player*, buff*
 *   @return  float
**/

float get_player_additional_hit_rate_by_buff(Player*p, buff* pBuff)
{
	if(p == NULL || pBuff == NULL){
		return 0.0;
	}
	float value = 0.0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_hit_rate_by_effect(p, pItr->second);
	}
	return value;
}


/**
 *   @brief get player additional hit rate by aura
 *   @param Player*, buff*
 *   @return  float
**/
float get_player_additional_hit_rate_by_aura(Player*p, aura* pAura)
{
	if(p == NULL || pAura == NULL){
		return 0.0;			    
	}
	float value = 0.0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_hit_rate_by_effect(p, pItr->second);									    
	}
	return value;
}



/**
 *   @brief get player additional hit rate by effect
 *   @param Player*, buff*
 *   @return  float
**/
float get_player_additional_hit_rate_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	float value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case add_player_hit_rate_percent_effect_type:
		{
			value += p->hit_rate* pData->trigger_percent/100.0;
		}
		break;

		case reduce_player_hit_rate_percent_effect_type:
		{
			value -= p->hit_rate* pData->trigger_percent/100.0;
		}
		break;
	}
	return value;
}






/**
 *    @brief get player critical rate by buff
 *    @param Player*, buff*
 *    @return  float
**/
int get_player_additional_critical_rate_by_buff(Player*p, buff* pBuff)
{
	if(p == NULL || pBuff == NULL){
		return 0;
	}
	int value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_critical_rate_by_effect(p, pItr->second);
	}
	return value;	
}

/**
 *    @brief get player critical rate by aura
 *    @param Player*, aura*
 *    @return  int
**/
int get_player_additional_critical_rate_by_aura(Player*p, aura* pAura)
{
	if(p == NULL || pAura == NULL){
		return 0;
	}
	int value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_critical_rate_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *    @brief get player critical rate by effect
 *    @param Player*, base_effect*
 *    @return  int
**/
int get_player_additional_critical_rate_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0;
	}
	int value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case add_critical_atk_percent_effect_type:
		{
			value += p->crit_rate* pData->trigger_percent/100;
		}
		break;

		case reduce_critical_atk_percent_effect_type:
		{
			value -= p->crit_rate* pData->trigger_percent/100;
		}
		break;
	}
	return value;
}

/**
 *   @brief get player def rate by buff
 *   @param Player*, buff*
 *   @return  float
**/
float get_player_additional_def_rate_by_buff(Player*p, buff* pBuff)
{
	if(pBuff == NULL){
		return 0.0;
	}
	float value = 0;

	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_def_rate_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *   @brief get player def value by buff
 *   @param Player*, buff*
 *   @return  int
**/
int get_player_additional_def_value_by_buff(Player*p, buff* pBuff)
{
	if(pBuff == NULL){
		return 0;
	}
	int value = 0;

	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_def_value_by_effect(p, pItr->second);
	}
	return value;
}


/**
 *   @brief get player def rate by aura
 *   @param Player*, aura*
 *   @return  float
**/
float get_player_additional_def_rate_by_aura(Player*p, aura* pAura)
{
	if(pAura == NULL){
		return 0.0;
	}
	float value = 0.0;

	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_def_rate_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *   @brief get player def rate by effect
 *   @param Player*, aura*
 *   @return  float
**/
float get_player_additional_def_rate_by_effect(Player* p,  base_effect* pEffect)
{
	if( pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	float value = 0.0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case add_defence_percent_effect_type:
		{
			value += p->def_rate* pData->trigger_percent/100.0;
		}
		break;

		case reduce_defence_percent_effect_type:
		{
			value -= p->def_rate* pData->trigger_percent/100.0;
		}
		break;
	}
	return value;
}

/**
 *   @brief get player def value by effect
 *   @param Player*, aura*
 *   @return  float
**/
int get_player_additional_def_value_by_aura(Player*p, aura* pAura)
{
	if(pAura == NULL){
		return 0.0;
	}
	float value = 0.0;

	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_def_value_by_effect(p, pItr->second);
	}
	return value;
}


/**
 *   @brief get player def value by effect
 *   @param Player*, aura*
 *   @return  float
**/
int get_player_additional_def_value_by_effect(Player* p,  base_effect* pEffect)
{
	if( pEffect == NULL){
		return 0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0;
	}
	int value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case add_defence_value_effect_type:
		{
			value += p->def_value * pData->trigger_percent/100.0;
		}
		break;

		case reduce_defence_value_effect_type:
		{
			value -= p->def_value * pData->trigger_percent/100.0;
		}
		break;
	}
	return value;
}


/**
 *    @brief get player  atk by buff
 *    @param Player*, aura*
 *    @return  float
**/
int get_player_additional_atk_by_buff(Player*p, buff* pBuff)
{
	if(pBuff == NULL){
		return 0;
	}
	int value = 0;

	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_atk_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *    @brief get player  atk by aura
 *    @param Player*, aura*
 *    @return  float
**/
int get_player_additional_atk_by_aura(Player*p, aura* pAura)
{
	if(pAura == NULL){
		return 0;
	}
	int value = 0;

	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_additional_atk_by_effect(p, pItr->second);
	}
	return value;
}

/**
 *   @brief get player atk by effect
 *   @param Player*, base_effect*
 *   @return  int
**/
int get_player_additional_atk_by_effect(Player* p,  base_effect* pEffect)
{
	int value = 0;
	if(pEffect == NULL){
		return 0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0;
	}
	int type = (int)pData->effect_type;
	switch(type)
	{
		case add_atk_effect_type:
		{
			if (pData->trigger_value != 0) {
				value += pData->trigger_value;
			} else {
				value += p->atk * pData->trigger_percent / 100;
			}
		}
		break;


		case reduce_atk_effect_type:
		{
			if (pData->trigger_value != 0) {
				value -= pData->trigger_value;
			} else {
				value -= p->atk * pData->trigger_percent / 100;
			}
		}
		break;

	}
	return value;
}


/**
 *    @brief init player bufflist
 *    @param Player*
 *    @return  true sucess,  false otherwirse fail
**/
bool init_player_bufflist(Player* p)
{
	p->m_bufflist.clear();
	return true;
}

/**
 *    @brief final player bufflist
 *    @param Player*
 *    @return  true sucess,  false otherwirse fail
**/
bool final_player_bufflist(Player* p)
{
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	while(pItr != p->m_bufflist.end())
	{
		buff* pBuff = *pItr;
		if(pBuff != NULL)
		{
			destroy_buff(pBuff);
			pBuff = NULL;
		}
		pItr = p->m_bufflist.erase(pItr);
	}
	return true;
}


/**
 *    @brief init player auralist
 *    @param Player*
 *    @return  true sucess,  false otherwirse fail
**/
bool init_player_auralist(Player* p)
{
	p->m_auralist.clear();
	return true;
}

/**
 *    @brief final player auralist
 *    @param Player*
 *    @return  true sucess,  false otherwirse fail
**/
bool final_player_auralist(Player* p)
{
        std::list<aura*>::iterator pItr = p->m_auralist.begin();
        while(pItr != p->m_auralist.end())
        {
                aura* pAura = *pItr;
                if(pAura != NULL)
                {
                        destroy_aura(pAura);
                        pAura = NULL;
                }
                pItr = p->m_auralist.erase(pItr);
        }
        return true;
}


//////////////////////////////////////////////////////////////////////////

/**
 *   @brief init player status
 *   @param Player*
 *   @return  true sucess,  false otherwirse fail
**/
bool init_player_status(Player* p)
{
	bool ret = init_player_bufflist(p);
	ret = init_player_auralist(p);
	return ret;
}


/**
 *   @brief final player status
 *   @param Player*
 *   @return  true sucess,  false otherwirse fail
**/
bool final_player_status(Player* p)
{
	bool ret = final_player_bufflist(p);	
	ret = final_player_auralist(p);
	return ret;
}



/////////////////////////////buff interface/////////////////////////////////

/**
 *   @brief check the buff is exist to player
 *   @param Player*, uint32_t
 *   @return  true sucess,  false otherwirse fail
**/
bool is_player_buff_exist(Player* p, uint32_t buff_id)
{
        std::list<buff*>::iterator pItr = p->m_bufflist.begin();
        while(pItr != p->m_bufflist.end())
        {
                buff* pBuff = *pItr;
                if(pBuff != NULL && pBuff->get_buff_id() == buff_id)return true;
		++pItr;
        }
        return false;
}

/**
 *    @brief get buff from player by id
 *    @param Player*, uint32_t
 *    @return  buff* sucess,  NULL otherwirse fail
**/
buff* get_player_buff(Player* p, uint32_t buff_id)
{
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for(; pItr != p->m_bufflist.end(); ++pItr)
	{
		buff* pBuff = *pItr;
		if (pBuff && pBuff->get_buff_id() == buff_id) {
			return pBuff;
		}
	}
	return NULL;
}

bool check_player_immunity_this_effect(Player* p, uint32_t effect_id)
{
	if (effect_id == 21063) {
		return is_player_buff_exist(p, 1222);
	}
	return false;
}
/**
 *   @brief add buff to player with para duration time 
 *   @param Player*  if duration time = 0 the buff duration time will set by xml file, else will set by parameters 
 *   @return  true sucess,  false otherwirse fail
**/
bool add_player_buff(Player* p, uint32_t buff_id, uint32_t creater_id, uint32_t duration_time, bool notify, bool include_invincible)
{
	buff_data* pBuffdata = buff_data_mgr::getInstance()->get_buff_data_by_id(buff_id);
	//该效果免疫一切BUFF
	if(is_effect_exist_in_player(p,  immunity_buff_type) || 
		(( p->invincible_time || (p->invincible && !include_invincible)) && 
			pBuffdata->buff_type != 1)){
		return false;
	}

	TRACE_LOG("Player add buff[%u %u %u]", p->id, buff_id, (uint32_t)time(NULL));
	
	buff* pBuff = get_player_buff(p, buff_id);
	if(pBuff){
		return reset_buff(pBuff);
	}

	pBuff = create_buff(buff_id);
	if(pBuff == NULL){
		return false;
	}

	//该效果免疫一切DEBUFF
	if(is_effect_exist_in_player(p,  immunity_debuff_type) && pBuff->get_buff_type() == 2 ){
		return false;
	}	
	
	if(duration_time){
		pBuff->set_duration_time(duration_time);
	}
	pBuff->set_creater_id(creater_id);
	//添加BUFF需要遵循一定的规则,遍历所有的BUFF，然后匹配规则
	
	if( pBuff->get_buff_category() != 0){
		std::list<buff*>::iterator pItr = p->m_bufflist.begin();
		for( ; pItr != p->m_bufflist.end(); ++pItr)
		{
			buff* pPlayerBuff = *pItr;
			if(pPlayerBuff == NULL)continue;
			if(pPlayerBuff->get_buff_category() == pBuff->get_buff_category() ){
					if( pBuff->get_buff_category_level() > pPlayerBuff->get_buff_category_level()){
						//删除这个BUFF
						TRACE_LOG("buff replace player_id[%u], old_buff[%u], new_buff[%u], time[%u]", 
								          pPlayerBuff->get_buff_id(), p->id, buff_id, (uint32_t)time(NULL));
						
						del_player_buff(p, pPlayerBuff->get_buff_id(), true);
						break;	
					}//if
					if( pBuff->get_buff_category_level() == pPlayerBuff->get_buff_category_level()){
						//重置这个BUFF
						TRACE_LOG("buff reset player_id[%u], old_buff[%u], new_buff[%u], time[%u]", 
								        pPlayerBuff->get_buff_id(), p->id, buff_id, (uint32_t)time(NULL));
						
						reset_buff(pPlayerBuff);
						destroy_buff(pBuff);
						pBuff = NULL;
						return true;
					}
					if( pBuff->get_buff_category_level() < pPlayerBuff->get_buff_category_level())
					{
						//不能添加这个BUFF
						TRACE_LOG("buff can not add player_id[%u], old_buff[%u], new_buff[%u], time[%u]",
										pPlayerBuff->get_buff_id(), p->id, buff_id, (uint32_t)time(NULL));

						destroy_buff(pBuff);
						pBuff = NULL;
						return true;
					}
			}//if
		}//for		
	}//if
	

	TRACE_LOG("Player get buff[%u %u %u]", p->id, buff_id, (uint32_t)time(NULL));
	return add_player_buff(p, pBuff, notify);	 	
}

/**
 *    @brief add buff to player
 *    @param Player*  
 *    @return  true sucess,  false otherwirse fail
**/
bool add_player_buff(Player* p, buff* pBuff, bool notify)
{	
	TRACE_LOG("Add buff[%u %u]", p->id, pBuff->get_buff_id()); 
	if(pBuff == NULL)return false;
	p->m_bufflist.push_back(pBuff);
	if (calc_player_additional_basic_attribute(p) && is_player(p->role_type)) {
		p->calc_player_attr(false);
	}
	calc_player_additional_attribute(p);
	if(notify ){
		notify_add_buff_to_player(p, pBuff);
	}
	if (is_effect_exist_in_player(p, faint_effect_type)) {
		if (!is_valid_uid(p->id)) {
			p->monster_stand_by_faint();
		}
	}
	int max_hp_buff_trim = get_player_additional_max_hp_by_buff(p, pBuff);
	int max_mp_buff_trim = get_player_additional_max_mp_by_buff(p, pBuff);

	p->add_additional_hp(max_hp_buff_trim);

	if(is_summon_mon(p->role_type)){
		p->summon_info.fight_value += max_mp_buff_trim;
	} else {
		p->add_additional_mp(max_mp_buff_trim);
	}
	
	p->noti_hpmp_to_btl();
	return true;
}


/**
 *   @brief delete all buff from player
 *   @param Player*  
 *   @return  true sucess,  false otherwirse fail
**/
bool del_player_all_buff(Player* p)
{
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for( ; pItr != p->m_bufflist.end(); ++pItr)
	{
		buff* pBuff = *pItr;
		if(pBuff != NULL){
			destroy_buff(pBuff);
			pBuff = NULL;
		}
	}
	p->m_bufflist.clear();
	return true;
}

bool del_player_all_buff(Player* p, uint32_t buff_type)
{
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	while( pItr != p->m_bufflist.end())
	{
		buff* pBuff = *pItr;
		if(pBuff == NULL || pBuff->get_buff_type() != buff_type){
			++pItr;
			continue;
		}
		notify_del_buff_to_player(p, pBuff->get_buff_id());
		destroy_buff(pBuff);
		pBuff = NULL;
		pItr = p->m_bufflist.erase(pItr);
	}
	return true;
}


bool del_random_player_buff(Player* p, uint32_t buff_type, uint32_t max_count)
{
	uint32_t cur_count = 0;
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	while( pItr != p->m_bufflist.end() && (cur_count < max_count))
	{
		buff* pBuff = *pItr;
		if(pBuff == NULL || pBuff->get_buff_type() != buff_type){
			++pItr;
			continue;						        
		}
		notify_del_buff_to_player(p, pBuff->get_buff_id());
		destroy_buff(pBuff);
		pBuff = NULL;
		pItr = p->m_bufflist.erase(pItr);
		cur_count++;
	}
	return true;
}




bool del_buff_when_condition(Player* p, uint32_t reason)
{
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	while( pItr != p->m_bufflist.end())
	{
		buff* pBuff = *pItr;
		if(pBuff == NULL){
			++pItr;
			continue;
		}
		if( pBuff->get_dead_delete()& reason){
			notify_del_buff_to_player(p, pBuff->get_buff_id());
			destroy_buff(pBuff);
			pBuff = NULL;
			pItr = p->m_bufflist.erase(pItr);
		}
		else
		{
			++pItr;
		}
	}
	return true;
}
/**
 *   @brief delete buff from player
 *   @param Player*, uint32_t  
 *   @return  true sucess,  false otherwirse fail
**/
bool del_player_buff(Player* p,  uint32_t buff_id, bool notify)
{
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for( ; pItr != p->m_bufflist.end(); ++pItr)
	{
		buff* pBuff = *pItr;
		if(pBuff == NULL)continue;
		if(pBuff->get_buff_id() == buff_id)
        {
			if(notify){
				notify_del_buff_to_player(p, buff_id);
			}
			destroy_buff(pBuff);
			pBuff = NULL;
            p->m_bufflist.erase(pItr);
            return true;
        }
	}
	return false;
}

/**
 *   @brief reset buff 
 *   @param Player*, uint32_t
 *   @return  true sucess,  false otherwirse fail
**/
bool reset_player_buff(Player* p, uint32_t buff_id)
{
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
    for( ; pItr != p->m_bufflist.end(); ++pItr)
    {
    	buff* pBuff = *pItr;
        if(pBuff == NULL)continue;
        if(pBuff->get_buff_id() == buff_id)
		{
			TRACE_LOG("reset buff[%u %u %u]",p->id, buff_id, (uint32_t)time(NULL));
			return reset_buff(pBuff);
		}
	}	
	return false;	
}



/**
 *   @brief player buff list routing
 *   @param Player*, struct timeval
 *   @return  void
**/
void player_bufflist_routing(Player* p, struct timeval cur_time)
{
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	while(pItr != p->m_bufflist.end())
	{
		buff* pBuff = *pItr;
		if(pBuff == NULL){
			++pItr;
			continue;
		}
		if( pBuff->check_buff_finish(cur_time)){
			TRACE_LOG("buff[%d] finish time = %u, player=%u", pBuff->get_buff_id(), (uint32_t)time(NULL), p->id );
			notify_del_buff_to_player(p, pBuff);	
			
			bool ret = pBuff->is_effect_type_exist(suicide_effect_type);
			pItr = p->m_bufflist.erase(pItr);
			destroy_buff(pBuff);
			pBuff = NULL;

			if (calc_player_additional_basic_attribute(p) && is_player(p->role_type)) {
				p->calc_player_attr(false);
			}
			calc_player_additional_attribute(p);
			p->noti_hpmp_to_btl();

			if(ret == true)//如果该BUFF带有自杀效果则，清掉BUFF的同时让玩家死亡
			{
				p->suicide();
				return;
			}
			continue;
		}
		uint32_t creater_id = pBuff->get_creater_id();
		Player* creater = NULL;
		if(creater_id != 0){
		   	creater = p->btl->get_player_in_btl(creater_id);
		}
			
		pBuff->effect_routing(p, cur_time, creater);
				
		++pItr;
	}
}



/////////////////////////aura interface///////////////////////////////////
void player_aura_infect(Player* p, aura* pAura, std::set<Player*>& m)
{
	if( pAura->get_aura_type() == team_aura_type)
	{
		for( std::set<Player*>::iterator it = m.begin(); it != m.end(); ++it)
		{
			Player* p1 = *it;
			if( p1 == NULL || p == p1)continue;
			if( p->team == p1->team && pAura->partnet_infect_datas.size() >0 )
			{
				if( p->check_distance(p1, pAura->get_aura_radius()) )//感染光环
				{
					for( uint32_t i = 0; i < pAura->partnet_infect_datas.size(); i++)
					{
						if( is_player_buff_exist(p, pAura->partnet_infect_datas[i])){
							add_player_buff(p1, pAura->partnet_infect_datas[i],  p->id, 0, false);
						}else{
							add_player_buff(p1, pAura->partnet_infect_datas[i], p->id);
						}
					}
				}
				else
				{
					for( uint32_t i = 0; i < pAura->partnet_infect_datas.size(); i++)
					{
						del_player_buff(p1, pAura->partnet_infect_datas[i]);
					}
				}
			}
			else if( p->team != p1->team && p1->team != neutral_team_1 && pAura->oppose_infect_datas.size() > 0)
			{
				if( p->check_distance(p1, pAura->get_aura_radius()) )//感染光环
				{
					for( uint32_t i = 0; i < pAura->oppose_infect_datas.size(); i++)
					{
						if( is_player_buff_exist(p,  pAura->oppose_infect_datas[i]) ){
							add_player_buff(p1, pAura->oppose_infect_datas[i], p->id, 0, false);		
						}else{
							add_player_buff(p1, pAura->oppose_infect_datas[i], p->id);
						}
					}
				}
				else
				{
					for( uint32_t i = 0; i < pAura->oppose_infect_datas.size(); i++)
					{
						del_player_buff(p1, pAura->oppose_infect_datas[i]);
					}
				}
			}
			else if( p->team == player_team_1 && p1->team == neutral_team_1 && pAura->neutral_infect_datas.size() > 0)
			{
				if( p->check_distance(p1, pAura->get_aura_radius()) )//感染光环
				{
					for( uint32_t i = 0; i < pAura->neutral_infect_datas.size(); i++)
					{
						if( is_player_buff_exist(p,  pAura->neutral_infect_datas[i]) ){
							add_player_buff(p1, pAura->neutral_infect_datas[i], p->id, 0, false);
						}else {
							add_player_buff(p1, pAura->neutral_infect_datas[i], p->id);
						}
					}
				}
				else
				{
					for( uint32_t i = 0; i < pAura->neutral_infect_datas.size(); i++)
					{
						del_player_buff(p1, pAura->neutral_infect_datas[i]);
					}
				}
			}
		}		
	}	
}

void player_auralist_routing(Player* p, struct timeval cur_time)
{
	std::list<aura*>::iterator pItr = p->m_auralist.begin();
	while(pItr != p->m_auralist.end())
	{
		aura* pAura = *pItr;
		if(pAura == NULL){
			++pItr;
			continue;
		}
		player_aura_infect(p, pAura, p->cur_map->monsters);
		player_aura_infect(p, pAura, p->cur_map->players);
		pAura->effect_routing(p, cur_time, NULL);
		++pItr;
	}
}


/**
 *    @brief add aura to player
 *    @param Player*, uint32_t
 *    @return  true sucess,  false otherwirse fail
**/
bool add_player_aura(Player* p,  uint32_t aura_id, uint32_t create_id, bool notify)
{
	if( is_player_aura_exist(p, aura_id))return false;
	aura* pAura = create_aura(aura_id, create_id);
	if(pAura == NULL)return false;
	return add_player_aura(p, pAura, notify);
}


/**
 *    @brief get player aura 
 *    @param Player*, uint32
 *    @return  aura* sucess,  NULL otherwirse fail
**/
aura* get_player_aura(Player* p, uint32_t aura_id)
{
	std::list<aura*>::iterator pItr = p->m_auralist.begin();
    for( ; pItr != p->m_auralist.end(); ++pItr)
	{
		aura* pAura = *pItr;
		return pAura;
	}
	return NULL;
}

/**
 *   @brief add aura to player
 *   @param Player*, aura*
 *   @return  true sucess,  false otherwirse fail
**/
bool add_player_aura(Player* p,  aura* pAura, bool notify)
{
	if(pAura == NULL)return false;
	TRACE_LOG("Add aura %u %u", p->id, pAura->get_aura_id());
	p->m_auralist.push_back(pAura);
	if (calc_player_additional_basic_attribute(p) && is_player(p->role_type)) {
		p->calc_player_attr(false);
	}
	calc_player_additional_attribute(p);
	if(notify){
		notify_add_aura_to_player(p, pAura);
	}
	int max_hp_buff_trim = get_player_additional_max_hp_by_aura(p, pAura);
	int max_mp_buff_trim = get_player_additional_max_mp_by_aura(p, pAura);

	p->add_additional_hp(max_hp_buff_trim);
	if(is_summon_mon(p->role_type)){
		p->summon_info.fight_value += max_mp_buff_trim;
	} else {
		p->add_additional_mp(max_mp_buff_trim);
	}
	p->noti_hpmp_to_btl();	
	return true;
}

/**
 *   @brief delete all aura from player
 *   @param Player*
 *   @return  true sucess,  false otherwirse fail
**/
bool del_player_all_aura(Player* p, bool notify)
{
	std::list<aura*>::iterator pItr = p->m_auralist.begin();
	for( ; pItr != p->m_auralist.end(); ++pItr)
	{
		aura* pAura = *pItr;
		if(pAura != NULL){
			if(notify){
				notify_del_aura_to_player(p, pAura->get_aura_id());				            
			}		
			destroy_aura(pAura);
			pAura = NULL;
		}
	}
	p->m_auralist.clear();
	return true;
}

/**
 *   @brief delete aura by create id
 *   @param Player*  uint32_t bool
 *   @return  true sucess,  false otherwirse fail
 **/

bool de_player_aura_by_creater_id(Player* p, uint32_t create_id, bool notify)
{
	std::list<aura*>::iterator pItr = p->m_auralist.begin();
	while( pItr != p->m_auralist.end())
	{
		aura* pAura = *pItr;
		if(pAura != NULL && pAura->get_create_id() == create_id){
			if(notify){
				notify_del_aura_to_player(p, pAura);
			}
			destroy_aura(pAura);
			pAura = NULL;
			pItr = p->m_auralist.erase(pItr);
		}else{
			++pItr;
		}
	}
	return true;
}


/**
 *   @brief delete aura from player
 *   @param Player*, uint32
 *   @return  true sucess,  false otherwirse fail
**/
bool del_player_aura(Player* p,  uint32_t aura_id, bool notify)
{
	std::list<aura*>::iterator pItr = p->m_auralist.begin();
    for( ; pItr != p->m_auralist.end(); ++pItr)
    {
        aura* pAura = *pItr;
        if(pAura == NULL)continue;
        if(pAura->get_aura_id() == aura_id)
        {
			if(notify){
				notify_del_aura_to_player(p, aura_id);
			}

            destroy_aura(pAura);
            pAura = NULL;
            p->m_auralist.erase(pItr);
            return true;
        }
	}
    return false;
}


/**
 *   @brief del aura when player dead
 *   @param Player*
 *   @return bool
 **/
bool del_infect_aura_when_dead(Player* p)
{
	std::list<aura*>::iterator pItr = p->m_auralist.begin();
	
	for( ; pItr != p->m_auralist.end(); ++pItr)
	{
		aura* pAura = *pItr;
		if(pAura == NULL)continue;

		PlayerSet::iterator it = p->cur_map->players.begin();
		for(;  it != p->cur_map->players.end(); ++ it)
		{
			for( uint32_t i =0; i < pAura->partnet_infect_datas.size(); i++)
			{
				del_player_buff(*it,  pAura->partnet_infect_datas[i]);
			}		
		}
	
		it = p->cur_map->monsters.begin();
		for(;  it != p->cur_map->monsters.end(); ++ it)
		{
			for( uint32_t i =0; i < pAura->oppose_infect_datas.size(); i++)
			{
				del_player_buff(*it,  pAura->oppose_infect_datas[i]);
			}
		}
	}
	return true;
}

/**
 *    @brief check the aura is exist 
 *    @param Player*, uint32
 *    @return  true sucess,  false otherwirse fail
**/
bool is_player_aura_exist(Player* p,  uint32_t aura_id)
{
        std::list<aura*>::iterator pItr = p->m_auralist.begin();
        while(pItr != p->m_auralist.end())
        {
                aura* pAura = *pItr;
                if(pAura != NULL && pAura->get_aura_id() == aura_id)return true;
                ++pItr;
        }
        return false;
}
/**
 *   @brief get player add atk value from buff and aura
 *   @param Player*
 *   @return  int
**/
int get_player_additional_atk(Player* p)
{
	int value = 0;
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for(; pItr != p->m_bufflist.end(); ++pItr )
	{
		value += get_player_additional_atk_by_buff(p, *pItr);
	}

	std::list<aura*>::iterator pItr2 = p->m_auralist.end();
	for(; pItr2 != p->m_auralist.end(); ++pItr2)
	{
		value += get_player_additional_atk_by_aura(p, *pItr2);
	}
	return value;
}

/**
 *     @brief get player add def rate from buff and aura
 *     @param Player*
 *     @return float
**/
float get_player_additional_def_rate(Player* p)
{
	float value = 0.0;

	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for(; pItr != p->m_bufflist.end(); ++pItr )
	{
		value += get_player_additional_def_rate_by_buff(p, *pItr);
	}

	std::list<aura*>::iterator pItr2 = p->m_auralist.end();
	for(; pItr2 != p->m_auralist.end(); ++pItr2)
	{
		value += get_player_additional_def_rate_by_aura(p, *pItr2);
	}
	return value;	
}


/**
 *    @brief get player critical rate from buff and aura
 *    @param Player*
 *    @return  int
**/
int get_player_additional_critical_rate(Player* p)
{
	int value = 0;
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for(; pItr != p->m_bufflist.end(); ++pItr )
	{
		value += get_player_additional_critical_rate_by_buff(p, *pItr);
	}

	std::list<aura*>::iterator pItr2 = p->m_auralist.end();
	for(; pItr2 != p->m_auralist.end(); ++pItr2)
	{
		value += get_player_additional_critical_rate_by_aura(p, *pItr2);
	}
	return value;
}


/**
 *  @brief get player exp_factor from buff abd aura
 *  @param Player*
 *  @return float
**/

float get_player_additional_exp_factor(Player* p)
{
	float value = 0;
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for(; pItr != p->m_bufflist.end(); ++pItr )
	{
		value += get_player_additional_exp_factor_by_buff(p, *pItr);
	}

	std::list<aura*>::iterator pItr2 = p->m_auralist.end();
	for(; pItr2 != p->m_auralist.end(); ++pItr2)
	{
		value += get_player_additional_exp_factor_by_aura(p, *pItr2);
	}
	return value;
}

/**
 *   @brief calc player attribute from buff and aura
 *   @param Player*
 *   @return  void
**/
void calc_player_additional_attribute(Player* p)
{
	
	uint32_t old_speed = p->get_speed();
	
	p->crit_buf_trim = 0;
	p->atk_buf_trim = 0;
	p->def_buf_trim = 0;
	p->def_value_buf_trim = 0;
	p->hit_buf_trim = 0;
	p->dodge_buf_trim = 0;
	p->exp_factor_trim = 0;
	p->mp_max_buf_trim = 0;
	p->hp_max_buf_trim = 0;
	p->summon_mon_exp_factor_trim = 0;
	p->player_only_exp_factor_trim = 0;
	p->skill_cd_time_buf_trim = 1.0;
	p->skill_mana_buf_trim = 1.0;
	p->atk_damage_change_buf_trim = 1.0;
	p->sustain_damage_change_buf_trim  = 1.0;
	p->sustain_damage_value_change_buf_trim = 0;
	p->speed_change_buf_trim = 1.0;
	p->buf_greed_rate = 0;
	p->critical_max_damage_buf_trim = 0;

	//other attr
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for(; pItr != p->m_bufflist.end(); ++pItr )
	{
		p->crit_buf_trim  += get_player_additional_critical_rate_by_buff(p, *pItr);					    
		p->atk_buf_trim   += get_player_additional_atk_by_buff(p, *pItr);
		p->def_buf_trim   += get_player_additional_def_rate_by_buff(p, *pItr);
		p->def_value_buf_trim += get_player_additional_def_value_by_buff(p, *pItr);
		p->hit_buf_trim   += get_player_additional_hit_rate_by_buff(p, *pItr);
		p->dodge_buf_trim += get_player_additional_dodge_rate_by_buff(p, *pItr);
		p->exp_factor_trim += get_player_additional_exp_factor_by_buff(p, *pItr);
		p->mp_max_buf_trim += get_player_additional_max_mp_by_buff(p, *pItr);
		p->hp_max_buf_trim += get_player_additional_max_hp_by_buff(p, *pItr);
		p->summon_mon_exp_factor_trim += get_summon_mon_additional_exp_factor_by_buff(p, *pItr);
		p->player_only_exp_factor_trim += get_player_only_additional_exp_factor_by_buff(p, *pItr);
		p->skill_cd_time_buf_trim += get_player_skill_cd_time_factor_by_buff(p, *pItr);
		p->skill_mana_buf_trim += get_player_skill_mana_factor_by_buff(p, *pItr);
		p->atk_damage_change_buf_trim += get_player_atk_damage_change_factor_by_buff(p, *pItr);
		p->sustain_damage_change_buf_trim += get_player_sustain_damage_change_factor_by_buff(p, *pItr);
		p->speed_change_buf_trim += get_player_speed_change_factor_by_buff(p, *pItr);
		p->buf_greed_rate += get_player_greed_rate_factor_by_buff(p, *pItr);
		p->sustain_damage_value_change_buf_trim += get_player_sustain_damage_value_factor_by_buff(p, *pItr);
		p->critical_max_damage_buf_trim += get_player_critical_max_damage_factor_by_buff(p, *pItr);
	       
	}

	std::list<aura*>::iterator pItr2 = p->m_auralist.begin();
    for(; pItr2 != p->m_auralist.end(); ++pItr2)
	{
		p->crit_buf_trim   += get_player_additional_critical_rate_by_aura(p, *pItr2);
		p->atk_buf_trim    += get_player_additional_atk_by_aura(p, *pItr2);
		p->def_buf_trim    += get_player_additional_def_rate_by_aura(p, *pItr2);
		p->def_value_buf_trim += get_player_additional_def_value_by_aura(p, *pItr2);
		p->hit_buf_trim    += get_player_additional_hit_rate_by_aura(p, *pItr2);
		p->dodge_buf_trim  += get_player_additional_dodge_rate_by_aura(p, *pItr2);
		p->mp_max_buf_trim += get_player_additional_max_mp_by_aura(p, *pItr2);
		p->hp_max_buf_trim += get_player_additional_max_hp_by_aura(p, *pItr2);
		p->skill_cd_time_buf_trim += get_player_skill_cd_time_factor_by_aura(p, *pItr2);
		p->skill_mana_buf_trim += get_player_skill_mana_factor_by_aura(p, *pItr2);
		p->atk_damage_change_buf_trim += get_player_atk_damage_change_factor_by_aura(p, *pItr2);
	    p->sustain_damage_change_buf_trim += get_player_sustain_damage_change_factor_by_aura(p, *pItr2);	
		p->speed_change_buf_trim += get_player_speed_change_factor_by_aura(p, *pItr2);
		p->buf_greed_rate += get_player_greed_rate_factor_by_aura(p, *pItr2);
		p->sustain_damage_value_change_buf_trim += get_player_sustain_damage_value_factor_by_aura(p, *pItr2);
		p->critical_max_damage_buf_trim += get_player_critical_max_damage_factor_by_aura(p, *pItr2);
	}

	if( p->skill_cd_time_buf_trim < 0)  p->skill_cd_time_buf_trim = 0;
	if( p->skill_mana_buf_trim  < 0)    p->skill_mana_buf_trim = 0;
	if( p->atk_damage_change_buf_trim < 0) p->atk_damage_change_buf_trim = 0; 
	if( p->sustain_damage_change_buf_trim < 0) p->sustain_damage_change_buf_trim = 0;
	if( p->speed_change_buf_trim < 0) p->speed_change_buf_trim = 0;

	if(old_speed != p->get_speed())
	{
		if (p->btl) {
			p->btl->notify_player_speed_change(p);
		}
		//if (p->i_ai) {
		//	p->i_ai->reset_velocity();
		//}
	}
	p->adjust_hp();
	p->adjust_mp();
}


bool calc_player_additional_basic_attribute(Player* p)
{	
	p->body_quality_trim = 0;
	p->agility_buf_trim = 0;

	//basic attr
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for(; pItr != p->m_bufflist.end(); ++pItr )
	{
		p->agility_buf_trim += get_player_agility_change_factor_by_buff(p, *pItr);  
		p->body_quality_trim += get_player_body_quality_change_factor_by_buff(p, *pItr); 
		TRACE_LOG("ADD BASICE ATTR %u %u", p->agility_buf_trim, p->body_quality_trim);
	}
	
	std::list<aura*>::iterator pItr2 = p->m_auralist.begin();
    for(; pItr2 != p->m_auralist.end(); ++pItr2)
	{
		p->agility_buf_trim += get_player_agility_change_factor_by_aura(p, *pItr2);  
		p->body_quality_trim += get_player_body_quality_change_factor_by_aura(p, *pItr2); 
		TRACE_LOG("ADD BASICE ATTR %u %u ", p->agility_buf_trim, p->body_quality_trim);
	}
	if (p->body_quality_trim || p->agility_buf_trim) {
		return true;
	}
	return false;
}

/**
 *    @brief notify player add a buff
 *    @param Player* ,  buff*
 *    @return void
**/

void notify_add_buff_to_player(Player* p, buff* pBuff)
{
	if(pBuff == NULL){
		return;
	}
	TRACE_LOG("Add buff %u %u %u", p->id, pBuff->get_buff_id(), pBuff->get_buff_icon());
	p->noti_buf_event_to_map( pBuff->get_buff_id(), pBuff->get_buff_icon(), 1);
}

/**
 *   @brief notify player delete a buff
 *   @param Player* ,  buff*
 *   @return void 
 **/

void notify_del_buff_to_player(Player* p, buff* pBuff)
{
	if(pBuff == NULL){
		return;			    
	}
	p->noti_buf_event_to_map(pBuff->get_buff_id(), 0, 0);	
}

/**
 *   @brief notify player delete a buff
 *   @param Player* ,  uint32_t*
 *   @return void 
 **/
void notify_del_buff_to_player(Player* p, uint32_t buff_id)
{ 
	p->noti_buf_event_to_map(buff_id, 0, 0);
}


void notify_add_aura_to_player(Player* p,  aura* pAura)
{
	if(pAura == NULL){
		return ;
	}
	TRACE_LOG("Add aura %u %u %u", p->id, pAura->get_aura_id(), pAura->get_aura_icon());
	p->noti_aura_event_to_map(pAura->get_aura_id(), pAura->get_aura_icon(), 1);	
}

void notify_del_aura_to_player(Player* p,  aura* pAura)
{
	if(pAura ==  NULL){
		return;
	}
	p->noti_aura_event_to_map(pAura->get_aura_id(), 0, 0);
}

void notify_del_aura_to_player(Player* p,  uint32_t aura_id)
{
	p->noti_aura_event_to_map(aura_id, 0, 0);
}

/**
 *   @brief check is the effect type exist in player
 *   @param Player*, uint32_t effect_type
 *   @return bool
**/
bool is_effect_exist_in_player(Player* p, uint32_t effect_type)
{
	bool ret = false;
	std::list<buff*>::iterator	pItr = p->m_bufflist.begin();
	for(;  pItr != p->m_bufflist.end(); ++pItr)
	{
		ret = is_effect_exist_in_buff( *pItr, effect_type);
		if(ret == true)return ret;
	}

	std::list<aura*>::iterator pItr2 = p->m_auralist.begin();
	for(;  pItr2 != p->m_auralist.end(); ++pItr2)
	{
		ret = is_effect_exist_in_aura( *pItr2, effect_type);
		if(ret == true)return ret;
	}
	return false;
}

effect_data* get_effect_data_in_player(Player* p, uint32_t effect_type)
{
	effect_data* data = NULL;
	std::list<buff*>::iterator  pItr = p->m_bufflist.begin();
	for(;  pItr != p->m_bufflist.end(); ++pItr)
	{
		data = get_effect_data_in_buff(*pItr, effect_type);
		if(data == NULL)continue;
		return data;
	}

	std::list<aura*>::iterator pItr2 = p->m_auralist.begin();
	for(;  pItr2 != p->m_auralist.end(); ++pItr2)
	{
		data = get_effect_data_in_aura(*pItr2, effect_type);
		if(data == NULL)continue;
		return data;
	}
	return data;
}

/**
 *  @brief check the effect type is exist in buff
 *  @param Player*,  uint32_t
 *  @return bool
 **/
bool is_effect_exist_in_buff(buff* pBuff, uint32_t effect_type)
{
	return pBuff->is_effect_type_exist(effect_type);
}

/**
 *  @brief check the effect type is exist in aura
 *  @param Player*, uint32_t
 *  @return bool
 **/
bool is_effect_exist_in_aura(aura* pAura, uint32_t effect_type)
{
	return pAura->is_effect_type_exist(effect_type);
}

effect_data* get_effect_data_in_buff(buff* pBuff, uint32_t effect_type)
{
	base_effect* pEffect =  pBuff->get_base_effect_by_type(effect_type);
	return pEffect == NULL ? NULL :  pEffect->get_effect_data();
}

effect_data* get_effect_data_in_aura(aura* pAura, uint32_t effect_type)
{
	base_effect* pEffect = pAura->get_base_effect_by_type(effect_type);
	return pEffect == NULL ? NULL :  pEffect->get_effect_data();
}



int get_player_additional_max_hp(Player* p)
{
	int value = 0;
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for(; pItr != p->m_bufflist.end(); ++pItr )
	{
		value += get_player_additional_max_hp_by_buff(p, *pItr);
	}

	std::list<aura*>::iterator pItr2 = p->m_auralist.end();
	for(; pItr2 != p->m_auralist.end(); ++pItr2)
	{
		value += get_player_additional_max_hp_by_aura(p, *pItr2);
	}
	return value;
}


int get_player_additional_max_mp(Player* p)
{
	int value = 0;
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for(; pItr != p->m_bufflist.end(); ++pItr )
	{
		value += get_player_additional_max_mp_by_buff(p, *pItr);
	}

	std::list<aura*>::iterator pItr2 = p->m_auralist.end();
	for(; pItr2 != p->m_auralist.end(); ++pItr2)
	{
		value += get_player_additional_max_mp_by_aura(p, *pItr2);
	}
	return value;
}

void player_passive_buff_trigger(Player* p, struct timeval cur_time,  uint32_t trigger_type)
{
	if( !(trigger_type >= on_attack_effect_trigger_type  && trigger_type <= on_damage_effect_trigger_type) ){
		return ;
	}
	std::list<buff*>::iterator pItr = p->m_bufflist.begin();
	for(; pItr != p->m_bufflist.end(); ++pItr )		
	{
		buff* pBuff = *pItr;
		if(pBuff == NULL)continue;

		bool ret = pBuff->special_effect_routing(p, cur_time, p,  trigger_type);
		if( ret && pBuff->get_buff_finish_type() & times_buff_finish_type)	
		{
			pBuff->dec_buff_times();		
		}
	}
}

float get_player_skill_cd_time_factor_by_buff(Player*p, buff* pBuff)
{
	if(pBuff == NULL){
		return 0.0;		    
	}
	float value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_skill_cd_time_factor_by_effect(p, pItr->second);
	}
	return value;
}

float get_player_skill_cd_time_factor_by_aura(Player*p, aura* pAura)
{
	if(pAura == NULL){
		return 0.0;
	}
	float value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_skill_cd_time_factor_by_effect(p, pItr->second);
	}
	return value;
}

float get_player_skill_cd_time_factor_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	float value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case reduce_skill_cd_time_type:
		{
			value -= 1 * pData->trigger_percent/100.0;
		}
		break;
	}
	return value;
}

float get_player_skill_mana_factor_by_buff(Player*p, buff* pBuff)
{
	if(pBuff == NULL){
		return 0.0;
	}
	float value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_skill_mana_factor_by_effect(p, pItr->second);
	}
	return value;
}

float get_player_skill_mana_factor_by_aura(Player*p, aura* pAura)
{
	if(pAura == NULL){
		return 0.0;
	}
	float value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_skill_mana_factor_by_effect(p, pItr->second);
	}
	return value;
}

float get_player_skill_mana_factor_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	float value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case reduce_skill_mana_type:
		{
			value -= 1 * pData->trigger_percent/100.0;
		}
		break;
	}
	return value;
}


float get_player_atk_damage_change_factor_by_buff(Player*p, buff* pBuff)
{
	if(pBuff == NULL){
		return 0.0;
	}
	float value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_atk_damage_change_factor_by_effect(p, pItr->second);
	}
	return value;
}

int get_player_agility_change_factor_by_buff(Player*p, buff* pBuff)
{
	if(pBuff == NULL){
		return 0.0;
	}
	int value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_agility_change_factor_by_effect(p, pItr->second);
	}
	return value;
}

int get_player_body_quality_change_factor_by_buff(Player*p, buff* pBuff)
{
	if(pBuff == NULL){
		return 0.0;
	}
	int value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_body_quality_change_factor_by_effect(p, pItr->second);
	}
	return value;
}

int get_player_agility_change_factor_by_aura(Player*p, aura* pAura)
{
	if(pAura == NULL){
		return 0.0;
	}
	int value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_agility_change_factor_by_effect(p, pItr->second);
	}
	return value;
}

int get_player_body_quality_change_factor_by_aura(Player*p, aura* pAura)
{
	if(pAura == NULL){
		return 0.0;
	}
	int value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_body_quality_change_factor_by_effect(p, pItr->second);
	}
	return value;
}


float get_player_atk_damage_change_factor_by_aura(Player*p, aura* pAura)
{
	if(pAura == NULL){
		return 0.0;
	}
	float value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_atk_damage_change_factor_by_effect(p, pItr->second);
	}
	return value;
}

float get_player_atk_damage_change_factor_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	float value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case atk_damage_change_type:
		{
			value += 1 * pData->trigger_percent/100.0;
		}
		break;
	}
	return value;
}

int get_player_agility_change_factor_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	int value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case add_basic_agility_effect_type:
		{
			value += p->agility_ * pData->trigger_percent/100.0;
			value += pData->trigger_value;
		}
		break;
	}
	return value;
}

int get_player_body_quality_change_factor_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	int value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case add_basic_body_quality_effect_type:
		{
			value += p->agility_ * pData->trigger_percent/100.0;
			value += pData->trigger_value;
		}
		break;
	}
	return value;
}


float get_player_sustain_damage_change_factor_by_effect(Player* p,  base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0; 
	}    
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0; 
	}    
	float value = 0; 

	int type = (int)pData->effect_type;
	switch(type)
	{
		case add_sustain_damage_change_type:
		{
			value += 1 * pData->trigger_percent/100.0;
		}    
		break;

		case reduce_sustain_damage_change_type:
		{
			value -= 1 * pData->trigger_percent/100.0;
		}
		break;
	}    
	return value;	
}

float get_player_sustain_damage_change_factor_by_aura(Player*p, aura* pAura)
{
	if(pAura == NULL){
		return 0.0;
	}
	float value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_sustain_damage_change_factor_by_effect(p, pItr->second);
	}
	return value;
}

float get_player_sustain_damage_change_factor_by_buff(Player*p, buff* pBuff)
{
	if(pBuff == NULL){
		return 0.0;
	}
	float value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_sustain_damage_change_factor_by_effect(p, pItr->second);
	}
	return value;
}



float get_player_speed_change_factor_by_buff(Player*p, buff* pBuff)
{
	if(pBuff == NULL){
		return 0.0;
	}
	float value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
	for(; pItr != pBuff->m_effect_maps.end(); ++pItr)
	{
		value += get_player_speed_change_factor_by_effect(p, pItr->second);
	}
	return value;
}

int get_player_greed_rate_factor_by_buff(Player * p, buff * pBuff)
{
  std::map<uint32_t, base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
  int value = 0;
  for (; pItr != pBuff->m_effect_maps.end(); ++pItr) {
    value += get_player_greed_rate_factor_by_effect(p, pItr->second);
  }
  return value;
}

int get_player_sustain_damage_value_factor_by_buff(Player * p, buff * pBuff)
{
  std::map<uint32_t, base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
  int value = 0;
  for (; pItr != pBuff->m_effect_maps.end(); ++pItr) {
    value += get_player_sustain_damage_value_factor_by_effect(p, pItr->second);
  }
  return value;
}

int get_player_sustain_damage_value_factor_by_aura(Player * p, aura* pAura)
{
  std::map<uint32_t, base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
  int value = 0;
  for (; pItr != pAura->m_effect_maps.end(); ++pItr) {
    value += get_player_sustain_damage_value_factor_by_effect(p, pItr->second);
  }
  return value;
}

int get_player_critical_max_damage_factor_by_buff(Player * p, buff * pBuff)
{
  std::map<uint32_t, base_effect*>::iterator pItr = pBuff->m_effect_maps.begin();
  int value = 0;
  for (; pItr != pBuff->m_effect_maps.end(); ++pItr) {
    value += get_player_critical_max_damage_factor_by_effect(p, pItr->second);
  }
  return value;
}

int get_player_critical_max_damage_factor_by_aura(Player * p, aura* pAura)
{
  std::map<uint32_t, base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
  int value = 0;
  for (; pItr != pAura->m_effect_maps.end(); ++pItr) {
    value += get_player_critical_max_damage_factor_by_effect(p, pItr->second);
  }
  return value;
}

int get_player_critical_max_damage_factor_by_effect(Player * p, base_effect * pEffect)
{
  if (NULL == pEffect) {
    return 0;
  }
  effect_data * pData = pEffect->get_effect_data();
  if (NULL == pData) {
    return 0;
  }
  switch(pData->effect_type) {
    case add_critical_max_damage_effect_type:
      return pData->trigger_percent;
  default:
    break;
  }
  return 0;
}


float get_player_speed_change_factor_by_aura(Player*p, aura* pAura)
{
	if(pAura == NULL){
		return 0.0;
	}	
	float value = 0;
	std::map<uint32_t,  base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
	for(; pItr != pAura->m_effect_maps.end(); ++pItr)
	{
		value += get_player_speed_change_factor_by_effect(p, pItr->second);
	}
	return value;
}

int get_player_greed_rate_factor_by_effect(Player * p, base_effect * pEffect)
{
  if (NULL == pEffect) {
    return 0;
  }
  effect_data * pData = pEffect->get_effect_data();
  if (NULL == pData) {
    return 0;
  }
  switch(pData->effect_type) {
    case add_coins_gain_effect_type:
      return pData->trigger_percent;
  default:
    break;
  }
  return 0;
}

int get_player_sustain_damage_value_factor_by_effect(Player * p, base_effect * pEffect)
{
  if (NULL == pEffect) {
    return 0;
  }
  effect_data * pData = pEffect->get_effect_data();
  if (NULL == pData) {
    return 0;
  }
  switch(pData->effect_type) {
    case add_sustain_damage_value_effect_type:
      return pData->trigger_value;
  default:
    break;
  }
  return 0;
}


float get_player_speed_change_factor_by_effect(Player*p, base_effect* pEffect)
{
	if(pEffect == NULL){
		return 0.0;
	}
	effect_data* pData = pEffect->get_effect_data();
	if(pData == NULL){
		return 0.0;
	}
	float value = 0;

	int type = (int)pData->effect_type;
	switch(type)
	{
		case add_player_speed_percent_effect_type:
		{
			value += 1 * pData->trigger_percent/100.0;
		}
		break;

		case reduce_player_speed_percent_effect_type:
		{
			value -= 1 * pData->trigger_percent/100.0;	
		}
		break;
	}
	return value;
}

int get_player_greed_rate_factor_by_aura(Player * player, aura * pAura)
{
  std::map<uint32_t, base_effect*>::iterator pItr = pAura->m_effect_maps.begin();
  int value = 0;
  for (; pItr != pAura->m_effect_maps.end(); ++pItr) {
    value += get_player_greed_rate_factor_by_effect(player, pItr->second);
  }
  return value;
}



