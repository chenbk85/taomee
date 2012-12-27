/**
 *    *============================================================
 *       @file      roll.hpp
 *       @brief     roll related functions are declared here
 *             
 *       compiler   gcc4.1.2
 *       platform   Linux
 *              
 *       copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *          
 *     *============================================================
**/



#ifndef  _ROLL_H
#define  _ROLL_H
#include <stdio.h>
#include <string.h>
#include <list>
#include <map>
#include <vector>

#include <boost/intrusive/list.hpp>

#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>
#include <libtaomee++/random/random.hpp>
extern "C" {

#include <libtaomee/list.h>
#include <libtaomee/timer.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>

}

#include "player.hpp"
class Battle;

using namespace std;
using namespace taomee;

//the max player count join roll item
#define MAX_ROLL_PLAYER_COUNT 4
//the roll item expire time (seconds)
#define ROLL_EXPIRE_TIME  10

//roll data struct
typedef struct roll_data
{
	//constructor function
	roll_data()
	{
		roll_id = 0;
		item_id = 0;
		item_count = 0;
		memset(roll_points, 0, sizeof(roll_points));
	}
	//destructor function
	roll_data(uint32_t id, uint32_t itm_id, uint32_t itm_count)
	{
		roll_id = id;
		item_id = itm_id;
		item_count = itm_count;
		memset(roll_points, 0, sizeof(roll_points));
	}	
	uint32_t roll_id;                                   //roll_id unique
	uint32_t item_id;                                   //item_id include item and equipment
	uint32_t item_count;                                //item_count default 0, maybe > 0 in future
	int32_t  roll_points[MAX_ROLL_PLAYER_COUNT];        //record every  point which the player roll
}roll_data;



//class battle_roll_data
class battle_roll_data: public MemPool, public taomee::EventableObject
{
public:
	//constructor function
	battle_roll_data()
	{
		btl = NULL;
		memset(player_ids, 0, sizeof(player_ids));
	}
	//destructor function
	~battle_roll_data()
	{
		btl = NULL;
		memset(player_ids, 0, sizeof(player_ids));
	}
	//set battle	
	void set_battle(Battle* pBattle)
	{	
		btl = pBattle;
	}
	//add all player in map to data
	void add_map_player(map_t* map);
	//del player id from data
	bool del_player_id(uint32_t player_id)
	{
		for(uint32_t i =0; i < MAX_ROLL_PLAYER_COUNT; i++)
		{
			if( player_ids[i] == player_id){
				player_ids[i] = 0;
				return true;
			}
		}
		return false;
	}
	//add player id to data
	bool add_player_id(uint32_t player_id)
	{
		for(uint32_t i =0; i < MAX_ROLL_PLAYER_COUNT; i++)
		{
			if( player_ids[i] ==0){
				player_ids[i] = player_id;
				return true;
			}
		}
		return false;
	}
	//add roll data
	bool add_player_roll_data(uint32_t roll_id, uint32_t roll_item_id, uint32_t roll_item_count)
	{
		roll_data data(roll_id, roll_item_id, roll_item_count);
		roll_datas.push_back(data);
		return true;
	}
	//check roll is finish
	bool is_roll_finish( uint32_t roll_id)
	{
		int cur_index = -1;
		int max_roll_point = -1;
		for( uint32_t i =0; i< roll_datas.size(); i++)
		{
			if( roll_datas[i].roll_id != roll_id ) continue;
			for( uint32_t j =0; j< MAX_ROLL_PLAYER_COUNT; j++)
			{
				if(player_ids[j] == 0)continue;
				if(roll_datas[i].roll_points[j] == 0)return false;
				if(roll_datas[i].roll_points[j] > max_roll_point)
				{
					cur_index = j;
					max_roll_point = roll_datas[i].roll_points[j];
				}	
			}	
		}
		if(cur_index == -1 && max_roll_point == -1)
		{
			player_refuse_roll_result(roll_id);
		}
		else 
		{
			player_roll_result( roll_id, player_ids[cur_index]);
		}
		return true;
	}
	//check roll is finish and then delete the roll
	bool check_delete();
	

	//player begin roll, refuse_flag = 1 means player refuse roll, or agree roll
	bool player_roll_point(uint32_t roll_id, uint32_t player_id, int32_t& roll_point, int refuse_flag = 0)
	{
		roll_point = 0;
		int index = -1;
		for(uint32_t i =0; i < MAX_ROLL_PLAYER_COUNT; i++)
		{
			if( player_ids[i] == player_id ){
				index = i;
				break;
			}
		}
		if( index == -1){
			return false;
		}
		

		for(uint32_t i =0; i < roll_datas.size(); i++)
		{
			if(roll_datas[i].roll_id == roll_id){
				if( roll_datas[i].roll_points[index] != 0){
					return true;
				}
				roll_point = refuse_flag == 1 ? -1 : ranged_random(1, 100);
				roll_datas[i].roll_points[index] = roll_point;				
			}
		}
		return true;
	}
	
	//该函数在定时器到时后调用	
	int timer_roll_result();
	//该函数在所有玩家ROLL后调用
	int player_roll_result(uint32_t roll_id, uint32_t player_id);
	//该函数在所有玩家放弃ROLL后调用
	int player_refuse_roll_result(uint32_t roll_id);	
public:
	Battle*  btl;                                           //btl point
	std::vector<roll_data> roll_datas;		                //roll data array
	uint32_t player_ids[MAX_ROLL_PLAYER_COUNT];             //player who join roll
};


/**
 *   @brief set player roll point
 *   @param Player*, uint32_t, uint32_t , uint32_t, int32_t&
 *   @return  true sucess,  false otherwirse fail
**/
bool set_player_roll_point(Player* p, uint32_t battle_id, uint32_t roll_id, int32_t roll_flag, int32_t& roll_point);


/**
 *   @brief player roll item cmd
 *   @param Player*
 *   @return  
**/
int player_roll_item_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
 *    @brief send roll point
 *    @param Player*, uint32_t, uint32_t roll_point
 *    @return  
**/
int send_roll_point(Player* p, uint32_t roll_id, uint32_t roll_point);

/**
 *   @brief create battle roll data
 *   @param  Battle*
 *   @return  battle_roll_data*
**/
battle_roll_data* create_battle_roll_data(Battle* btl);

/**
 *   @brief  destroy battle roll data
 *   @param  battle_roll_data*
 *   @return void
**/
void destroy_battle_roll_data(battle_roll_data* pData);

/**
 *   @brief send db request to roll item
 *   @param  Player*,  roll_data&
 *   @return  int
**/
int db_pick_roll_item(Player* p,  roll_data& data);

/**
 *   @brief db player pick roll item callback
 *   @param  Battle*
 *   @return  battle_roll_data*
**/
int db_player_pick_roll_item_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

#endif
