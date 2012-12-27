#include <stdio.h>
#include <stdint.h>
#include "player.hpp"
#include "stage.hpp"
#include "base_effect.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee/log.h>
#include "player_status.hpp"
using namespace taomee;


/**
 *   @brief check the timer is up
 *   @param struct timeval
 *   @return  true sucess  false otherwise fail
**/
bool base_effect::to_next_time(struct timeval cur_time)
{
	if( m_trigger_type == time_effect_trigger_type)
	{
		return base_trigger_timer::to_next_time(cur_time);
	}
	return true; 
}

/**
 *   @brief reset timer
 *   @param struct timeval
 *   @return  void
**/
void base_effect::reset_timer(struct timeval cur_time)
{
	if(m_trigger_type == time_effect_trigger_type) 
	{
		base_trigger_timer::reset_timer(cur_time);
	}
}

/**
 *   @brief get base effect id
 *   @param void
 *   @return uint32_t
**/
uint32_t base_effect::get_base_effect_id()
{
	return m_effect_id;
}
/**
 * @brief get base trigger rate
 * @return uint32_t
 */
uint32_t base_effect::get_base_effect_trigger_rate()
{
    return m_probability;
}
/**
 *   @brief get effect type
 *   @param void
 *   @return uint32_t
**/
base_effect_type base_effect::get_base_effect_type()
{
	return m_effect_type;
}

/**
 *   @brief get effect trigger type
 *   @param void
 *   @return uint32_t
**/
base_effect_trigger_type base_effect::get_base_effect_trigger_type()
{
	return m_trigger_type;
}

/**
 *   @brief get effect_data 
 *   @param void
 *   @return effect_data*
**/
effect_data*  base_effect::get_effect_data()
{
	return m_pEffectdata;
}


/**
 *   @brief process effect
 *   @param Player*, struct timeval
 *   @return void
**/
void base_effect::process_effect(Player* pOwner, struct timeval cur_time, Player* creater)
{
	if (pOwner->is_dead()) {
		return;
	}
	//如果该效果是时间触发，则在这里检查是否到了触发时间
	if( m_trigger_type == time_effect_trigger_type && !to_next_time(cur_time)){
		return ;
	}

	TRACE_LOG("malong process_effect time = %u, player_id = %u, effect_type = %d", (uint32_t)time(NULL), pOwner->id, (int)m_effect_type );
	switch((int)m_effect_type)
	{
		
		case add_pet_wakan_value://增加灵力值
		{
			/*
			if(is_summon_mon(pOwner->role_type))
			{
				pOwner->summon_info.fight_value += m_pEffectdata->trigger_value;
				pOwner->summon_info.fight_value =pOwner->summon_info.fight_value > 100 ? 100 : pOwner->summon_info.fight_value;
				pOwner->summon_attr_chg();
			}*/
			pOwner->add_pet_fight_value(m_pEffectdata->trigger_value);
		}
		break;
		
		case restore_life_mana_effect_type://回血回蓝
		{	
			player_hpmp_chg_info_t info;
        		info.hp_chg = m_pEffectdata->trigger_value;
        		info.mp_chg = m_pEffectdata->trigger_value;
       	 		pOwner->hpmp_chg(info);					
		}
		break;

		case restore_mana_effect_type: //回蓝
		{
			player_hpmp_chg_info_t info;
			info.hp_chg = 0;
			info.mp_chg = m_pEffectdata->trigger_value;
			pOwner->hpmp_chg(info);
		}
		break;

		case restore_life_effect_type: //回血
		{
			player_hpmp_chg_info_t info;
		    info.hp_chg = m_pEffectdata->trigger_value;
			info.mp_chg = 0;
			pOwner->hpmp_chg(info);	
		}
		break;

		case player_fall_down_effect_type://放倒
		{
			pOwner->fall_down( m_pEffectdata->trigger_value );
		}
		break;

		case recover_life_effect_type://按百分比回血
		{
			pOwner->recover_hp(m_pEffectdata->trigger_percent );
		}
		break;

		case reduce_lift_effect_type://扣血
		{
			if( is_effect_exist_in_player(pOwner, invincibility_type) )
			{
				return;
			}

			int reduce_value = 0 - m_pEffectdata->trigger_value;
			reduce_value +=  0 - m_pEffectdata->trigger_percent * pOwner->max_hp() / 100;
			pOwner->chg_hp(reduce_value);

			if( pOwner->is_dead()){
				pOwner->on_dead( creater );
				if(  !is_valid_uid( pOwner->id) && !is_summon_mon(pOwner->role_type) )
				{
					pOwner->leave_map();						                    
				}
			}
			pOwner->noti_hpmp_to_btl();
		}
		break;

		case reduce_life_and_fly_effect_type://扣血并且击飞
		{
			if( is_effect_exist_in_player(pOwner, invincibility_type) )
			{
				return;
			}
			
			int reduce_value = 0 - m_pEffectdata->trigger_value;
			reduce_value +=  0 - m_pEffectdata->trigger_percent * pOwner->max_hp() / 100;
			pOwner->chg_hp(reduce_value);
			pOwner->notify_damage_by_buff(0, pOwner->hp, 0-reduce_value, 2);

			if( pOwner->is_dead()){
				pOwner->on_dead( creater );
				if(  !is_valid_uid( pOwner->id) && !is_summon_mon(pOwner->role_type) )
				{
					pOwner->leave_map();
				}
			}
		}
		break;

		case reduce_life_mana_effect_type://同时扣除血和蓝
		{
			if( is_effect_exist_in_player(pOwner, invincibility_type) )
			{
				return;
			}
			
			int reduce_value = 0 - m_pEffectdata->trigger_value;
			pOwner->chg_hp(reduce_value);
			pOwner->chg_mp(reduce_value);

			
			if( pOwner->p_cur_skill){
				pOwner->p_cur_skill->set_dead();
				pOwner->on_skill_end( pOwner->p_cur_skill);
			}

			if( pOwner->is_dead()){
				pOwner->on_dead( creater );
				if(  !is_valid_uid( pOwner->id) && !is_summon_mon(pOwner->role_type) )
				{
					pOwner->leave_map();	
				}
			}
			pOwner->noti_hpmp_to_btl();
		}
		break;


		case harm_around_all_effect_type://所有对象范围伤害
		{
			map_t* pMap = pOwner->cur_map;
			PlayerSet::iterator pItr = pMap->monsters.begin();
			for( ; pItr != pMap->monsters.end(); ++pItr)
			{
				if( pOwner->collision(*pItr))
				{
					int damage = (m_pEffectdata->trigger_value);
					if(  (*pItr)->p_cur_skill)
					{
						(*pItr)->p_cur_skill->set_dead();
						(*pItr)->on_skill_end( (*pItr)->p_cur_skill);
					}
					(*pItr)->chg_hp( 0-damage);
					(*pItr)->notify_damage_by_buff(pOwner->id, (*pItr)->hp, damage);
					if( (*pItr)->is_dead())
					{
						(*pItr)->on_dead(pOwner);
						(*pItr)->leave_map();
					}
				}
			}
			pItr = pMap->players.begin();
			for( ; pItr != pMap->players.end(); ++pItr)
			{
				if( pOwner->collision(*pItr))
				{
					int damage = (m_pEffectdata->trigger_value);
					if(  (*pItr)->p_cur_skill)
					{
						(*pItr)->p_cur_skill->set_dead();
						(*pItr)->on_skill_end( (*pItr)->p_cur_skill);
					}
					(*pItr)->chg_hp( 0-damage);
					(*pItr)->notify_damage_by_buff(pOwner->id, (*pItr)->hp, damage);
					if( (*pItr)->is_dead())
					{
						(*pItr)->on_dead(pOwner);
					}
				}
			}
		}
		break;

		case harm_around_enemy_effect_type://敌方范围伤害
		{	
			map_t* pMap = pOwner->cur_map;
			if( is_valid_uid(pOwner->id) || is_summon_mon(pOwner->role_type) )//角色或者召唤兽
			{
				PlayerSet::iterator pItr = pMap->monsters.begin();
				for( ; pItr != pMap->monsters.end(); ++pItr)
				{
					if( pOwner->collision(*pItr))
					{
						int damage = (m_pEffectdata->trigger_value);
						//be carefull before reduce life you should end current skill 
						if(  (*pItr)->p_cur_skill)
						{
							(*pItr)->p_cur_skill->set_dead();
							(*pItr)->on_skill_end( (*pItr)->p_cur_skill);
						}

						(*pItr)->chg_hp( 0-damage);
						(*pItr)->notify_damage_by_buff(pOwner->id, (*pItr)->hp, damage);	
						if( (*pItr)->is_dead())
						{
							(*pItr)->on_dead(pOwner);
							(*pItr)->leave_map();
						}
					}
				}
			}
			else//怪物
			{
				PlayerSet::iterator pItr = pMap->players.begin();
				for( ; pItr != pMap->players.end(); ++pItr)
				{
					if( (*pItr)->is_dead()){
						continue;
					}
					
					if( pOwner->collision(*pItr))
					{
						int damage = (m_pEffectdata->trigger_value);
						//be carefull before reduce life you should end current skill 
						
						if(  (*pItr)->p_cur_skill)
						{
							(*pItr)->p_cur_skill->set_dead();
							(*pItr)->on_skill_end( (*pItr)->p_cur_skill);
						}
						

						(*pItr)->chg_hp( 0-damage);
						(*pItr)->notify_damage_by_buff(pOwner->id, (*pItr)->hp, damage);
						if( (*pItr)->is_dead())
						{
							(*pItr)->on_dead(pOwner);
						}//if
					}//if	
				}//for
			}//else			
		}//case
	}//switch
}



////////////////////////////////////////////////////////////

/**
 *   @brief get static obj of this class
 *   @param  void
 *   @return effect_data_mgr*
**/
effect_data_mgr* effect_data_mgr::getInstance()
{	
	static effect_data_mgr obj;
	return &obj;
}


/**
 *   @brief init from xml file
 *   @param const char* 
 *   @return true sucess, false otherwise fail
**/
bool effect_data_mgr::init(const char* xml_name)
{
	if(xml_name == NULL){
		return false;
	}
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr effect_node = NULL;
	xmlNodePtr effect_son = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml_name);
    if(doc == NULL){
		throw XmlParseError(std::string("the xml file is not exist"));
		return false;
    }		
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the xml file content is empty"));
		return false;
	}
	bool ret = true;
	
	effect_node = root->xmlChildrenNode;
	while(effect_node)
	{
		uint32_t effect_type = 0;	

		get_xml_prop_def(effect_type, effect_node, "effect_type", 0);
		effect_son = effect_node->xmlChildrenNode;
		while(effect_son)
		{
			if(  xmlStrcmp(effect_son->name, BAD_CAST"trigger") == 0){

				effect_data* pData = new effect_data;
				get_xml_prop_def(pData->effect_id,   effect_son, "effect_id", 0);
				get_xml_prop_def(pData->trigger_type, effect_son, "trigger_type", 0);
				get_xml_prop_def(pData->trigger_time, effect_son, "trigger_time", 0);
				get_xml_prop_def(pData->trigger_rate, effect_son, "trigger_rate", 100);
				get_xml_prop_def(pData->trigger_value, effect_son, "trigger_value", 0);
				get_xml_prop_def(pData->trigger_percent, effect_son, "trigger_percent", 0);
				pData->effect_type = effect_type;
				pData->effect_id = pData->effect_id + effect_type * EFFECT_ID_FACTOR;

				if(is_effect_data_exist(pData->effect_id)){
					ret = false;
					delete pData;
					pData = NULL;
					throw XmlParseError(std::string("effect_id is exist"));
					goto ERR_LOGIC;
				}
				add_effect_data(pData);
			}
		    effect_son = effect_son->next;	
		}	
		effect_node = effect_node->next;	
	}

ERR_LOGIC:
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return ret;		
}


/**
 *   @brief final the effect data mgr
 *   @param void
 *   @return true sucess, false otherwise fail
**/
bool effect_data_mgr::final()
{
	std::map<uint32_t, effect_data*>::iterator pItr = m_effect_data_maps.begin();
	for( ; pItr != m_effect_data_maps.end(); ++pItr)
	{
		effect_data* pData = pItr->second;
		if(pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_effect_data_maps.clear();
	return true;
}

/**
 *   @brief add effect data to mgr
 *   @param effect_data*
 *   @return true sucess, false otherwise fail
**/
bool effect_data_mgr::add_effect_data(effect_data* pData)
{
	if(pData == NULL)return false;
	if(is_effect_data_exist(pData->effect_id))return false;
	m_effect_data_maps[ pData->effect_id ] = pData;
	return true;	
}

/**
 *   @brief check the effect is exist in mgr
 *   @param uint32_t
 *   @return true sucess, false otherwise fail
**/
bool effect_data_mgr::is_effect_data_exist(uint32_t effect_id)
{
	return m_effect_data_maps.find(effect_id) != m_effect_data_maps.end();
}

/**
 *   @brief del effect data from mgr
 *   @param uint32_t
 *   @return true sucess, false otherwise fail
**/
bool effect_data_mgr::del_effect_data(uint32_t effect_id)
{
	std::map<uint32_t, effect_data*>::iterator pItr = m_effect_data_maps.find(effect_id);
	if( pItr == m_effect_data_maps.end()) return false;
	effect_data* pData = pItr->second;
	if(pData != NULL)
	{
		delete pData;
		pData = NULL;
	}	
	m_effect_data_maps.erase(pItr);
	return true;
}

/**
 *   @brief get effect data count of mgr
 *   @param void
 *   @return uint32_t
**/
uint32_t effect_data_mgr::get_effect_data_count()
{
	return m_effect_data_maps.size();
}


/**
 *   @brief get effect data by id
 *   @param uint32_t
 *   @return effect_data*
**/
effect_data* effect_data_mgr::get_effect_data_by_id(uint32_t effect_id)
{
	if( is_effect_data_exist(effect_id))
	{
		return m_effect_data_maps[ effect_id ];
	}
	return NULL;
}
