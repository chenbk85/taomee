/**
 *============================================================
 *       @file      base_effect.h
 *       @brief     base effect immediately related functions are declared here
 *          
 *       compiler   gcc4.1.2
 *       platform   Linux
 *              
 *       copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *       
 *============================================================
**/

#ifndef GF_BASE_EFFECT_H
#define GF_BASE_EFFECT_H

#include <stdint.h>
#include <map>
#include <libtaomee++/time/time.hpp>
#include <libtaomee++/memory/mempool.hpp>
#include <string.h>
extern "C" {
#include <libtaomee/log.h>
}

class Player;
typedef struct effect_data effect_data;

#define EFFECT_ID_FACTOR 1000

enum  base_effect_type
{
	begin_base_effect_type = 0,
	//每X秒回蓝回血
	restore_life_mana_effect_type = 1,
	//每X秒回蓝
	restore_mana_effect_type = 2,	
	//每X秒回血
	restore_life_effect_type = 3,
	//增加防御%
	add_defence_percent_effect_type = 4,
	//减少防御%
	reduce_defence_percent_effect_type = 5,

	//增加攻击力
	add_atk_effect_type = 6,	
	//攻击力下降
	reduce_atk_effect_type = 7,
	

	//暴击率增加%
	add_critical_atk_percent_effect_type = 8,
	//暴击率较少%
	reduce_critical_atk_percent_effect_type = 9,
	
	//速度增加%
	add_player_speed_percent_effect_type = 10,
	//速度较少%
	reduce_player_speed_percent_effect_type = 11,

	//命中率增加%
	add_player_hit_rate_percent_effect_type = 12,	
	//命中率减少%
	reduce_player_hit_rate_percent_effect_type = 13,

	//闪避增加%
	add_player_dodge_rate_percent_effect_type = 14,
	//闪避减少%
	reduce_player_dodge_rate_percent_effect_type = 15,
	//倒地
	player_fall_down_effect_type = 16,	
	//经验(包括人，宠物)获取增益
	player_get_exp_factor_effect_type = 17,
	//增加玩家HP上限
	player_add_max_hp_effect_type = 18,
	//增加玩家MP上限
	player_add_max_mp_effect_type  =19,
	//宠物经验获益
	summon_mon_get_exp_factor_effect_type  =20,
	//减少血量
	reduce_lift_effect_type = 21,
	//人物经验获益
	player_only_get_exp_factor_effect_type  =22,

	//增加防御%
	add_defence_value_effect_type = 24,
	//减少防御%
	reduce_defence_value_effect_type = 25,
	
	//给周围敌对角色造成XX点伤害每秒
	harm_around_enemy_effect_type = 100,
	//反弹玩家攻击伤害
	rebound_attack_effect_type = 101,
	//给玩家回血x%
	recover_life_effect_type = 102,
	//同时扣除玩家的血和蓝
	reduce_life_mana_effect_type = 103,
	//眩晕
	faint_effect_type = 104,
	//免疫一切BUFF
	immunity_buff_type = 105,
	//减少技能公共CD
	reduce_share_cd_time_type = 106,
	//减少所有技能CD时间
	reduce_skill_cd_time_type = 107,
	//减少技能消耗的MANA
	reduce_skill_mana_type = 108,
	//无敌
	invincibility_type = 109,	
	//攻击伤害变化
	atk_damage_change_type = 110,
	//免疫一切DEBUFF
	immunity_debuff_type = 112,
	//对所有对象造成伤害，无视敌我
	harm_around_all_effect_type = 113,
	//承受的伤害增加
	add_sustain_damage_change_type = 114,
	//承受的伤害减少
	reduce_sustain_damage_change_type = 115,
	//减少血量并且击飞
	reduce_life_and_fly_effect_type = 116,
	//自杀性效果,当包含该效果的BUFF或者AURA被移除的时候，携带者自杀
	suicide_effect_type = 117,
	//防击倒，击飞，僵直的效果。包含该效果的对象不会被击飞，也不会被击倒
	fortitude_effect_type = 118,
	//增加宠物的灵力值//
	add_pet_wakan_value = 119,	
	//攻击附加吸血功能
	vampire_attach_effect_type = 120,
	//有X%的概率对敌人造成x倍伤害
	crit_attach_effect_type = 121,
	//对玩家技能有着X%的抗性
	damage_reduction_effect_type = 122,
	//灵兽有X%的几率施放技能时不消耗灵力
	protect_wakan_effect_type = 123,
	//灵兽的灵力值上限提升X%
	add_max_wakan_effect_type = 124,

       	add_coins_gain_effect_type = 125,
	
	add_drop_rate_effect_type = 126,

	add_basic_agility_effect_type = 127,

	add_basic_body_quality_effect_type = 128,

	add_sustain_damage_value_effect_type = 129,

	add_critical_max_damage_effect_type = 130,

	end_base_effect_type 
};



enum base_effect_trigger_type
{        
	begin_effect_trigger_type    =0,
	//默认触发方式
	normal_effect_trigger_type   =1, 
	//时间间隔触发方式
	time_effect_trigger_type     =2,
	//被攻击触发的触发方式
	on_attack_effect_trigger_type     = 3,
	//受伤触发的触发方式
	on_damage_effect_trigger_type     = 4,

	end_effect_trigger_type 
};


typedef struct effect_data
{
        effect_data()
        {   
                effect_id    = 0;
                effect_type  = 0;
                trigger_type = 0;
                trigger_time = 0;
                trigger_rate = 0;
                trigger_value = 0;
                trigger_percent = 0;
        }   
    
        uint32_t  effect_id;                //效果ID，唯一存在
        uint32_t  effect_type;              //效果类型，见enum  base_effect_type
        uint32_t  trigger_type;             //效果触发类型，见enum trigger_type
        uint32_t  trigger_time;             //效果触发时间间隔
        uint32_t  trigger_rate;             //效果触发概率，默认100
        uint32_t  trigger_value;            //效果触发附件参数1
        uint32_t  trigger_percent;          //效果触发附件参数2
}effect_data;

///////////////////////////this class is base class of dot and  buff////////////////////////////////// 
class base_duration_timer
{
public:
	//constructor function
	base_duration_timer()
	{
		memset(&m_begin_time, 0, sizeof(m_begin_time)) ;
		m_duration_time = 0;
	}
	//destructor function
	virtual ~base_duration_timer(){}
public:
	//init timer function
	void init_base_duration_timer(struct timeval begin_time, uint32_t duration_time)
	{
		m_begin_time = begin_time;
		m_duration_time = duration_time;
	}
public:
	//reset timer 
	virtual void reset_timer(struct timeval begin_time)
	{
		m_begin_time = begin_time;
	}
	//check the timer is finish
	virtual bool is_timer_finish(struct timeval cur_time)
	{
		TRACE_LOG("[%u %u]\n", taomee::timediff2(cur_time, m_begin_time), m_duration_time*1000);
		return taomee::timediff2(cur_time, m_begin_time) >  (int)m_duration_time*1000;
	}
	//set the timer data
	virtual void set_duration_time( uint32_t duration_time)
	{
		m_duration_time = duration_time;
	}	
protected:
	struct timeval 	m_begin_time;         //记录计时器的开始时间戳
	uint32_t 	m_duration_time;	      //记录计时器的持续时间单位秒
};


////////////////////////////this calls is base class of effect///////////////////////////////////////////////////////////////
class base_trigger_timer
{
public:
	//constructor function
	base_trigger_timer()
	{
		memset(&m_begin_time, 0, sizeof(m_begin_time));
		m_interval_time = 0;
	}
	//destructor funtion
	virtual ~base_trigger_timer(){}
public:
	//init timer function
	void init_base_trigger_timer(struct timeval begin_time, uint32_t interval_time)
	{
		m_begin_time = begin_time;
		m_interval_time = interval_time;	
	}
public:
	//check the time is up
	virtual bool to_next_time(struct timeval cur_time)
	{
		if( taomee::timediff2(cur_time, m_begin_time) >(int)m_interval_time*1000)
		{
			m_begin_time = cur_time;
			return true;
		}
		return false;
	}
	//reset the timer
	virtual void reset_timer(struct timeval cur_time)
	{	
		m_begin_time = cur_time;
	}
private:
	struct timeval  m_begin_time;        //计时器开始的时间戳
	uint32_t  	m_interval_time;         //多长时间触发一次，单位秒
};

///////////////////////////////////////////////////////////////////////
class base_effect:public base_trigger_timer
{
public:
	//constructor function
	base_effect()
	{
		m_effect_type  =(base_effect_type)0;	
		m_effect_id = 0;
		m_trigger_type = (base_effect_trigger_type)0;
		m_probability = 0;
		m_pEffectdata  = NULL;
	}
	//destructor function
	virtual ~base_effect()
	{
		//printf("~base_effect\n");
		m_pEffectdata  = NULL;
	}
public:
	//init base effect data
	bool init_base_effect(effect_data* pData, struct timeval begin_time)
	{
		if(pData == NULL)return false; 
		m_pEffectdata = pData;			
		m_effect_id = pData->effect_id;
		m_effect_type = (base_effect_type)pData->effect_type;
		m_trigger_type = (base_effect_trigger_type)pData->trigger_type;
		m_probability  = pData->trigger_rate;
		init_base_trigger_timer(begin_time, pData->trigger_time); 	
		return true;	
	}
	// process effect by timer  may be pOwner == creater
	void process_effect(Player* pOwner, struct timeval cur_time, Player* creater);
public:
	//virtual function to override the base class member function
	bool to_next_time(struct timeval cur_time);
	//virtual function to override the base class member function
	void reset_timer(struct timeval cur_time);	
public:
	//get effect id
	uint32_t 			get_base_effect_id();	
    //get probability
    uint32_t        get_base_effect_trigger_rate();
	//get effect type
	base_effect_type 		get_base_effect_type();
	//get effect trigger type
	base_effect_trigger_type	get_base_effect_trigger_type();
	//effect_data point to xml data
	effect_data*                get_effect_data();
private:
	base_effect_type  		m_effect_type;                       //效果类型
	base_effect_trigger_type 	m_trigger_type;                  //效果触发类型
	uint32_t                        m_effect_id;                 //效果ID 
	uint32_t                        m_probability;               //效果触发概率
private:
	effect_data*                    m_pEffectdata;               //指向XML效果配置数据的指针
};
/////////////////////////////////////////////////////////////////////////


class base_effect_mgr
{
public:
	//constructor function
	base_effect_mgr()
	{
		m_effect_maps.clear();
	}	
	//destructor function
	virtual ~base_effect_mgr()
	{
		std::map<uint32_t,  base_effect*>::iterator pItr = m_effect_maps.begin();
		for(; pItr != m_effect_maps.end(); ++pItr)
		{
			base_effect* pEffect = pItr->second;
			if(pEffect != NULL)
			{
				delete pEffect;
				pEffect = NULL;
			} 
		}
	}
public:
	//get effect count in mgr
	virtual int  get_base_effect_count() 
	{
		return m_effect_maps.size();
	}
	//add an effect to mgr
	virtual bool add_base_effect(base_effect* pEffect)
	{
		if(pEffect == NULL)return false;
		uint32_t effect_id = pEffect->get_base_effect_id();
		if( m_effect_maps.find( effect_id ) == m_effect_maps.end() )
		{
			m_effect_maps[ effect_id ] = pEffect;
			return true;
		}
		return false; 
	}
	//del an effect from mgr by effect_id
	virtual bool del_base_effect(uint32_t effect_id)
	{
		std::map<uint32_t,  base_effect*>::iterator pItr = m_effect_maps.find( effect_id);
		if(pItr != m_effect_maps.end())
		{
			m_effect_maps.erase(pItr);
			return true;
		}
		return false;
	}
	//get effect from mgr by effect id
	virtual base_effect*  get_base_effect_by_id(uint32_t effect_id)
	{
		std::map<uint32_t,  base_effect*>::iterator pItr = m_effect_maps.find( effect_id);
		if(pItr != m_effect_maps.end())
		{
			return pItr->second;
		}
		return NULL;
	}
	virtual uint32_t get_base_effect_trigger_value(uint32_t effect_id)
	{
		std::map<uint32_t,  base_effect*>::iterator pItr = m_effect_maps.find( effect_id);
		if(pItr != m_effect_maps.end())
		{
			base_effect* p_tmp = (pItr->second);
			return p_tmp->get_effect_data()->trigger_value;
		}
		return 0;
	}
	//check the effect is exist in mgr
	virtual bool is_effect_exist(uint32_t effect_id)
	{
		return m_effect_maps.find( effect_id) != m_effect_maps.end();
	}
	//traversal all effect in mgr
	virtual void effect_routing(Player* pOwner, struct timeval time, Player* creater)
	{
		std::map<uint32_t,  base_effect*>::iterator pItr = m_effect_maps.begin();
		for( ;pItr != m_effect_maps.end(); ++pItr)
		{
			base_effect* pEffect = pItr->second;
			if(pEffect == NULL)continue;
			uint32_t type = pEffect->get_base_effect_trigger_type();
			if( !(type == normal_effect_trigger_type || type == time_effect_trigger_type ))continue;
			pEffect->process_effect(pOwner, time, creater);	
		}	
	}
	virtual bool special_effect_routing(Player* pOwner, struct timeval time, Player* creater, uint32_t trigger_type)
	{
		bool ret = false;
		if(trigger_type == normal_effect_trigger_type || trigger_type == time_effect_trigger_type)return false;
		std::map<uint32_t,  base_effect*>::iterator pItr = m_effect_maps.begin();
		for( ;pItr != m_effect_maps.end(); ++pItr)
		{
			base_effect* pEffect = pItr->second;
			if(pEffect == NULL)continue;
			uint32_t type = pEffect->get_base_effect_trigger_type();
			if( type != trigger_type )continue;
			pEffect->process_effect(pOwner, time, creater);	
			ret = true;
		}
		return ret;
	}
	//check the effect type is exist in mgr
	virtual bool is_effect_type_exist(uint32_t effect_type)
	{
		std::map<uint32_t,  base_effect*>::iterator pItr = m_effect_maps.begin();
		for( ;pItr != m_effect_maps.end(); ++pItr)
		{
			base_effect* pEffect = pItr->second;
			if(pEffect == NULL)continue;
			if((uint32_t)pEffect->get_base_effect_type() == effect_type ){
				return true;
			}	
		}
		return false;
	}
	virtual base_effect* get_base_effect_by_type(uint32_t effect_type)
	{
		std::map<uint32_t,  base_effect*>::iterator pItr = m_effect_maps.begin();
		for( ;pItr != m_effect_maps.end(); ++pItr)
		{
			base_effect* pEffect = pItr->second;
			if(pEffect == NULL)continue;

			if((uint32_t)pEffect->get_base_effect_type() == effect_type ){
                //if (pEffect->get_base_effect_trigger_rate() > (rand() % 100))
				return pEffect;
			}
		}
		return NULL;
	}

public:
	std::map<uint32_t,  base_effect*> m_effect_maps;     //效果集合
};





class effect_data_mgr
{
private:
	//constructor function
	effect_data_mgr(){}
	//destructor function
	~effect_data_mgr(){}
public:	
	//get static obj of this class
	static effect_data_mgr* getInstance();
	//init function from xml file
	bool init(const char* xml_name);
	//final function
	bool final();
public:
	//add effect data to mgr
	bool add_effect_data(effect_data* pData);
	//check the effect data is exist in mgr
	bool is_effect_data_exist(uint32_t effect_id);
	//del effect data from mgr
	bool del_effect_data(uint32_t effect_id);
	//get effect data count of mgr 
	uint32_t get_effect_data_count();
	//get effect data from mgr by effect id
	effect_data* get_effect_data_by_id(uint32_t effect_id);
private:
	std::map<uint32_t,  effect_data*>m_effect_data_maps;  //效果数据集合	
};







#endif
