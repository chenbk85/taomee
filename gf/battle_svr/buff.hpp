/**
 *============================================================
 *    @file      buff.h
 *    @brief     buff related functions are declared here
 *              
 *    compiler   gcc4.1.2
 *    platform   Linux
 *                  
 *    copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *              
 *============================================================
**/


#ifndef GF_BUFF_H
#define GF_BUFF_H

#include <stdint.h>
#include <libtaomee++/memory/mempool.hpp>
#include <libtaomee++/time/time.hpp>
#include <libtaomee/timer.h>
#include "base_effect.hpp"
#include <vector>
#include <map>

enum buff_delete_reason
{
	buff_dead_delete_reason = 1<<0,
	buff_stage_restart_delete_reason = 1<<1
};

enum buff_finish_type
{
	begin_buff_finish_type = 0,
	time_buff_finish_type =  1,
	times_buff_finish_type = 1<<1,	
	end_buff_finish_type 
};



typedef struct buff_data
{
	buff_data()
	{
		buff_id = 0;
		show_client = 0;
		duration_time = 0;
		category = 0;
		level = 0;
		dead_delete = 0;
		buff_type = 0;
		buff_trigger_times = 0;
		buff_finish_type = 0;
	}
	uint32_t buff_id;              //buff id 唯一BUFF ID
	int      show_client;          
	uint32_t duration_time;        //buff 持续时间
	uint32_t category;             //buff 种类，默认为0（无种类,添加的时候没有任何限制,有种类的buff,级别高的覆盖级别低）
	uint32_t level;                //buff 种类级别，默认为0
	uint32_t icon;                 //buff 在客户端显示的BUFF图标索引
	int32_t  dead_delete;          //死亡后是否删除
	uint32_t buff_type;            //buff类型 1：普通BUFF,2:debuff
	uint32_t buff_trigger_times;   //buff触发最大次数限制 
	uint32_t buff_finish_type;     //buff的结束类型1:时间结束，2按触发次数结束
	std::vector<uint32_t> effect_ids;
}buff_data;



class buff: public base_effect_mgr, public base_duration_timer, public taomee::MemPool
{
public:
	//constructor function
	buff(uint32_t buff_id,uint32_t category, uint32_t level, uint32_t icon, int32_t dead_delete, bool show)
	{
		m_buff_id = buff_id;
		m_show_client = show;
		m_category  = category;
		m_level = level;
		m_icon = icon;
		m_dead_delete = dead_delete;
		m_creater_id = 0;
		m_buff_trigger_times = 0;
		m_buff_type = 0;
		m_buff_finish_type = 0;
	}
	//destructor function
	~buff(){}
public:
	void set_creater_id(uint32_t id)
	{
		m_creater_id = id;
	}
	//get buff id 
	uint32_t  get_buff_id() 
    {
    	return m_buff_id;
    }
	//check the buff is show client
    bool      is_show_client() 
    {
        return m_show_client;
    }      
    //get buff category
	uint32_t get_buff_category()
	{
		return m_category;
	}
	int32_t get_dead_delete()
	{
		return m_dead_delete;
	}
	uint32_t get_buff_icon()
	{
		return m_icon;
	}	
	//get buff category level
	uint32_t get_buff_category_level()
	{
		return m_level;
	}
	uint32_t get_creater_id()
	{
		return m_creater_id;
	}

	uint32_t get_buff_type()
	{
		return m_buff_type;
	}
	uint32_t get_buff_trigger_times()
	{
		return m_buff_trigger_times;
	}
	void set_buff_type(uint32_t type)
	{
		m_buff_type = type;
	}
	void set_buff_times(int32_t times)
	{
		m_buff_trigger_times = times;
	}
	int32_t dec_buff_times(uint32_t time = 1)
	{
		m_buff_trigger_times -= time;
		return m_buff_trigger_times;
	}
	uint32_t get_buff_finish_type()
	{
		return m_buff_finish_type;
	}
	void set_buff_finish_type(uint32_t type)
	{
		m_buff_finish_type = type;
	}
	bool check_buff_finish( struct timeval cur_time )
	{
		if( m_buff_finish_type & time_buff_finish_type ){
            if (is_timer_finish(cur_time)) return true;
			//return is_timer_finish(cur_time);
		}	
		if( m_buff_finish_type & times_buff_finish_type){
			return m_buff_trigger_times <= 0;
		}
		return false;
	}
private:
	uint32_t m_buff_id;               //buff id 唯一
	uint32_t m_category;              //buff 种类
	uint32_t m_level;                 //buff 种类级别
	uint32_t m_icon;                  //buff 在客户端对应的图标
	int32_t  m_dead_delete;           //死亡后是否删除
	uint32_t m_creater_id;            //谁释放的BUFF,默认为0
	bool     m_show_client;        
 	uint32_t m_buff_type;             //buff类型 1:普通buff, 2:debuff
	int32_t  m_buff_trigger_times;    //buff触发最大次数限制 
	uint32_t m_buff_finish_type;      //buff的结束类型 	
};


class buff_data_mgr
{
private:
	//constructor function
	buff_data_mgr(){}
	//destructor  function
	~buff_data_mgr(){}
public:
	//init from xml file for mgr
	bool init(const char* xml_name);
	//final mgr
	bool final();
public:
	//create static class obj 
	static buff_data_mgr* getInstance();
	//get buff data count in mgr
	uint32_t get_buff_data_count();
	//check the buff is exist in mgr
	bool is_buff_data_exist(uint32_t buff_id);
	//add buff to mgr
	bool add_buff_data(buff_data* pData);
	//del buff from mgr
	bool del_buff_data(uint32_t buff_id);
	//get buff data by id from mgr
	buff_data*  get_buff_data_by_id(uint32_t buff_id);
private:
	std::map<uint32_t, buff_data*> m_buff_data_maps;    //buff data maps
};
/**
 *   @brief create buff by buff id
 *   @param uint32_t
 *   @return buff*
**/
buff* create_buff(uint32_t buff_id);

/**
 *   @brief destroy buff
 *   @param buff*
 *   @return void
**/
void  destroy_buff(buff* pBuff);

/**
 *   @brief reset buff timer
 *   @param buff*
 *   @return true sucess, false otherwise fail
**/
bool  reset_buff(buff* pBuff);

bool buff_has_stuck_effect(uint32_t buff_id);

#endif
