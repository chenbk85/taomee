/**
 *============================================================
 *  @file      warehouse.h
 *  @brief     warehouse related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef _WAREHOUSE_H
#define _WAREHOUSE_H

extern "C" 
{
	#include <libtaomee/project/stat_agent/msglog.h>
}

#include <map>
#include "item.hpp"
using namespace std;
//the default warehouse grid count
#define DEFAULT_WAREHOUSE_GRID_COUNT 12
//the money you should pay for moving item from warehouse to bag
#define MOVE_ITEM_W2B_CONSUME 0
//the money you should pay for moving item from bag to warehouse
#define MOVE_ITEM_B2W_CONSUME 0

/**
  * @brief initialization player's warehouse and set the max warehouse grid count 
  * @param player_t* , max_warehouse_grid_count
  * @return  0 sucess,  -1 otherwirse fail
 */
int init_player_warehouse(player_t* p);

/**
  * @brief uninitialization player's warehouse 
  * @param player_t* ,
  * @return  0 sucess,  -1 otherwirse fail
 */
int final_player_warehouse(player_t* p);

/**
  * @brief add item to player's warehouse 
  * @param player_t* , item_id, item_count
  * @return  0 sucess,  -1 otherwirse fail
 */
int add_warehouse_item(player_t* p,  uint32_t item_id,  int item_count);
int add_warehouse_clothes_item(player_t* p,  uint32_t id, uint32_t attireid, uint32_t get_time, uint32_t attire_rank, uint32_t duration, uint32_t attire_lv, uint32_t end_time);
/**
  * @brief delete item from player's warehouse 
  * @param player_t* , item_id, item_count
  * @return  0 sucess,  -1 otherwirse fail
 */
int del_warehouse_item(player_t* p,  uint32_t item_id,  int item_count);
int del_warehouse_clothes_item(player_t* p, uint32_t id, uint32_t item_id);
/**
  * @brief increase player's warehouse grid count 
  * @param player_t* , grid_count 
  * @return  0 sucess,  -1 otherwirse fail
 */
int  increase_warehouse_grid_count(player_t* p,  uint32_t grid_count);

/**
  * @brief get count of player's warehouse grid 
  * @param player_t* 
  * @return  value be more than or equal to 0
 */
int  get_warehouse_max_grid_count(player_t* p);

/**
  * @brief get current userd count of player's warehouse grid 
  * @param player_t* 
  * @return  value be more than or equal to 0
 */
int  get_warehouse_cur_used_grid_count(player_t* p);
/**
  * @brief check put item to player's warehouse 
  * @param player_t* 
  * @return true ok, otherwise false
 */
bool check_warehouse_add_item(player_t* p, uint32_t item_id, int item_count);
bool check_warehouse_add_clothes_item(player_t*p,  uint32_t id,  uint32_t item_id);
/**
  * @brief check delete item from player's warehouse 
  * @param player_t* 
  * @return true ok, otherwise false
 */
bool check_warehouse_delete_item(player_t* p, uint32_t item_id, int item_count);    
bool check_warehouse_delete_clothes_item(player_t*p, uint32_t id, uint32_t item_id);
/**
  * @brief check the item count in player's warehouse 
  * @param player_t* 
  * @return true ok, otherwise false
 */   
bool check_warehouse_item_count(player_t* p, uint32_t item_id, int item_count);


/**
  * @brief get warehouse item list
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int get_warehouse_item_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief move item from warehouse to bag
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int move_item_warehouse_to_bag_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief move item from bag to warehouse
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int move_item_bag_to_warehouse_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief initialization player's warehouse item data
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int init_player_warehouse_list(player_t* p, get_warehouse_item_list_rsp_t* rsp);

/**
  * @brief callback function for getting warehouse itemlist
  * @param player_t* 
  * @return 0 sucess, unzero otherwise 
 */
int db_get_warehouse_item_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief send db request for moving item from warehouse to bag
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int move_item_warehouse_to_bag_db_request(player_t* p, uint32_t item_id,  int count, int coin);

/**
  * @brief send db request for moving item from bag to warehouse
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int move_item_bag_to_warehouse_db_request(player_t* p, uint32_t item_id,  int count, int coin);

/**
  * @brief callback function for moving item from warehouse to bag
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int db_move_item_warehouse2bag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback function for moving item from  bag to warehouse
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int db_move_item_bag2warehouse_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief send db request for getting warehouse item list
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int db_get_warehouse_item_list(player_t* p);

/**
  * @brief send warehouse item list to player;
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int send_warehouse_item_list_info(player_t* p);

/*----------------------------------------------------*/
int get_warehouse_clothes_item_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int db_get_warehouse_clothes_item_list(player_t* p);
int db_get_warehouse_clothes_item_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
int send_warehouse_clothes_item_list_info(player_t* p);
int init_player_warehouse_clothes_list(player_t* p, get_warehouse_clothes_item_list_rsp_t* rsp);


int move_clothes_item_bag_to_warehouse_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int move_clothes_item_bag_to_warehouse_db_request(player_t* p, uint32_t item_id,  int id, int coin);
int db_move_clothes_item_bag2warehouse_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);



int move_clothes_item_warehouse_to_bag_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int move_clothes_item_warehouse_to_bag_db_request(player_t* p, uint32_t item_id,  int id, int coin);
int db_move_clothes_item_warehouse2bag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);




#endif
