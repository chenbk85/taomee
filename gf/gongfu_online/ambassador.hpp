/**
 *============================================================
 *  @file      ambassador.hpp
 *  @brief    
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_AMBASSADOR_HPP_
#define KF_AMBASSADOR_HPP_

#include "player.hpp"
#include "task.hpp"
#include "item.hpp"


enum amb_task_status_t {
	amb_task_cancel = 0,
	amb_task_started = 1,
	amb_task_finished = 2,
};

struct single_reward_t {
	/*! item type : 1 attire, 2 item, 3 player attr*/
	uint32_t give_type;
	/*! item  id*/
	uint32_t give_id;
	/*! item count */
	uint32_t count;
} ;


struct ambassador_reward_t {
	uint32_t cnt;
	single_reward_t reward_arr[max_ambassador_detail_reward_num];
};

struct fin_amb_session_t {
	/*! position of reward bit buf*/
	uint8_t pos;
	/*! number of task_out_clothes */
	uint32_t	clothes_cnt;
	/*! recored the unique id of clothes */
	clothes_info_t clothes[max_ambassador_detail_reward_num];
};

//------------------------------------------------------------------------------------------------
//--- cmd processings
//------------------------------------------------------------------------------------------------
/**
  * @brief get set amb task flg : 0, never got; 1,nomor accept; 2,finished
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int set_amb_task_status_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief get amb task info
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_amb_task_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief get amb reward
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_amb_reward_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
//----- db operation functions
//------------------------------------------------------------------------------------------------
int db_amb_swap_item(player_t* p, uint8_t pos);

int db_set_amb_task_status(player_t* p, uint8_t status);

int db_set_user_flag(player_t* p, uint8_t retflag);

void set_amb_info_after_lv_up(player_t *p);

//------------------------------------------------------------------------------------------------
//--- db operations callback processing 
//------------------------------------------------------------------------------------------------
/**
 * @brief callback for swap item
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */

int db_amb_swap_item_callback(player_t* p, swap_item_rsp_t* p_rsp, uint32_t bodylen);
/**
 * @brief callback for user_flag
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_set_user_flag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
/**
 * @brief callback for set amb task status
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_set_amb_task_status_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

//------------------------------------------------------------------------------------------------
//--- Parse Xml File
//------------------------------------------------------------------------------------------------
/**
  * @brief load ambassador configs from an xml file
  * @param cur xml node 
  * @return 0 on success, -1 on error
  */
int load_ambassador(xmlNodePtr cur);


#endif // KF_AMBASSADOR_HPP_


