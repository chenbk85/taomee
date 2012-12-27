/**
 *============================================================
 *   @file      aura.h
 *   @brief     aura related functions are declared here
 *                   
 *   compiler   gcc4.1.2
 *   platform   Linux
 *                       
 *   copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *                       
 *============================================================
**/


#ifndef GF_AURA_H
#define GF_AURA_H

#include <stdint.h>
#include <libtaomee++/memory/mempool.hpp>
#include <libtaomee++/time/time.hpp>
#include <libtaomee/timer.h>
#include "base_effect.hpp"
#include <vector>

enum aura_type
{
	begin_aura_type = 0,
	myself_aura_type = 1, //只对自己起作用的光环
	team_aura_type = 2,   //允许传染的光环
	end_aura_type
};

typedef struct aura_infect_data
{
	aura_infect_data()
	{
		buff_id = 0;
		target = 0;
	}
	uint32_t buff_id;
	uint32_t target;
}ura_infect_data;


typedef struct aura_data
{
	aura_data()
	{
		aura_id = 0;
		show_client = 0;
		aura_type = 0;
		icon = 0;
		aura_radius= 0;
	}	
	uint32_t aura_id;                    //光环ID
	int      show_client;                //是否在客户端显示
	uint32_t aura_type;                  //光环的类型
	uint32_t icon;                       //光环的图标
	uint32_t aura_radius;                //光环影响的半径
	std::vector<uint32_t> effect_ids;	 //效果ID集合
	std::vector<uint32_t> partnet_infect_datas; //友方感染效果
	std::vector<uint32_t> oppose_infect_datas;  //敌方感染效果
	std::vector<uint32_t> neutral_infect_datas; //中立友方单位的感染效果
}aura_data;

class aura: public base_effect_mgr, public taomee::MemPool
{
public:
	//constructor function
	aura(uint32_t aura_id, uint32_t aura_type, uint32_t icon, uint32_t radius, uint32_t create_id = 0,  bool show = false)
	{
		m_aura_id = aura_id;
		m_show_client = show;
		m_create_id = create_id;
		m_aura_type = aura_type;
		m_icon = icon;
		m_radius = radius;	
	}
	//destructor function
	~aura(){}
public:
	//get aura id 
	uint32_t  get_aura_id()	
	{
		return m_aura_id;
	}
	//check this aura is show client
	bool      is_show_client()
	{
		return m_show_client;
	}
	uint32_t   get_create_id()
	{
		return m_create_id;
	}	
	uint32_t   get_aura_type()
	{
		return m_aura_type;
	}
	uint32_t   get_aura_icon()
	{
		return m_icon;
	}
	uint32_t   get_aura_radius()
	{
		return m_radius;
	}
public:
	std::vector<uint32_t> partnet_infect_datas; //友方感染效果
	std::vector<uint32_t> oppose_infect_datas;  //敌方感染效果
	std::vector<uint32_t> neutral_infect_datas; //中立的友方感染效果
private:
	uint32_t   m_aura_id;             //光环ID，唯一
	uint32_t   m_create_id;           //光环的产生者ID
	uint32_t   m_radius;            //作用距离
	uint32_t   m_aura_type;           //光环类型
	uint32_t   m_icon;                //光环对应的图标
	bool       m_show_client;         //是否在客户端显示

};




class aura_data_mgr
{
private:
	//constructor function
	aura_data_mgr(){}
	//destructor function
	~aura_data_mgr(){}
public:
	//create static class obj
	static aura_data_mgr* getInstance();
	//init from xml file for mgr
	bool   init(const char* xml_name);
	//final mgr
	bool   final();
public:
	//check the aura data is exist in mgr
	bool   is_aura_data_exist(uint32_t aura_id);
	//add aura data to mgr
	bool   add_aura_data(aura_data* pData);
	//del aura data from mgr
	bool   del_aura_data(uint32_t aura_id);
	//get aura data count in mgr
	uint32_t get_aura_data_count();
	//get aura data by id
	aura_data* get_aura_data_by_id(uint32_t aura_id);
private:
	std::map<uint32_t, aura_data*> m_aura_data_maps;   //aura data map
};

/**
 *   @brief create aura by aura id
 *   @param uint32_t
 *   @return aura*
**/
aura* create_aura(uint32_t aura_id, uint32_t create_id);

/**
 *   @brief destroy aura 
 *   @param aura*
 *   @return void
**/
void  destroy_aura(aura* pAura);
#endif





