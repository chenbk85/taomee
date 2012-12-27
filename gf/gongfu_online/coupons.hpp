/**
 *=====================================================================
 *  @file     exchange.hpp
 *  @brief    all about player's exchanges of tasks, namely, 
 *            number of times to carry out tasks in one day or one week
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *=====================================================================
 */

#ifndef __KF_COUPONS_HPP__
#define __KF_COUPONS_HPP__

#include "player.hpp"


/*! max rewards count for a single periodical action */
const uint32_t max_coupons_count = 20;

#pragma pack(1)

struct reward_t {
	/*! cost item  */
	uint32_t give_type;
	/*! cost item  */
	uint32_t give_id;
	/*! cost amount */
	uint32_t count;
} ;

struct exchange_t {
	/*! exchange id */
	uint32_t	id;
	/*! exchange name */
	char		name[MAX_MAIL_TITLE_LEN];
	/*! exchange flag: 0 for daily exchange and 1 for weekly one */
	uint32_t	restr_flag;
	/*! exchange top limit in one day or week */
	uint32_t	toplimit;
    /*! limit use level */
    uint32_t    uselv[2];
	/*! exchange vip limit in one day or week */
	uint32_t	vip;
	/*! count of costs */
	uint32_t	cost_count;
	uint32_t	price;
	uint32_t	is_broadcast;
	uint32_t	limit_seq;
    /*! count of reward*/
    uint32_t    rew_count;
	/*! rewards */
	reward_t	rewards[max_coupons_count];
};

struct db_coupons_exchange_t {
    uint32_t    trade_id;
    uint32_t    left_coupons;
    uint32_t    item_cnt;
};

struct db_exchange_item_t {
    uint32_t    type;
    uint32_t    id;
    uint32_t    cnt;
    uint32_t    uniqueid;
    uint32_t    gettime;
    uint32_t    endtime;
};

struct store_item_t {
    uint32_t    price;
};

struct coupon_product_item_t {
	uint32_t    product_id;
	uint32_t 	is_box;
};

struct store_feedback_t {
    uint32_t    fb_cnt;
};

#pragma pack()

/*! global variable to hold all exchange actions  */
extern exchange_t g_all_coupons[max_coupons_exchange];


//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------
/**
  * @brief player performs periodical action
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int coupons_exchange_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

uint32_t get_feedback_coin_cnt(uint32_t id);

uint32_t get_item_gold_coin_price(uint32_t id);

//---------------------------------------------------------------------------------------------
// Callbacks
//---------------------------------------------------------------------------------------------
/**
  * @brief callback for handling adding periodical action returned from dbproxy
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_coupons_exchange_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief parse the "rewards" sub node
  * @param chl sub node 
  * @param idx index to indicate certain exchange action
  * @return true on success, false on error
  */
bool parse_exchange_reward(xmlNodePtr cur, uint32_t idx);

bool is_product_vip_box(uint32_t id);
/**
  * @brief load exchange config from an xml file
  * @param cur xml node 
  * @return 0 on success, -1 on error
  */
int load_coupons_exchange(xmlNodePtr cur);

int load_store_sales(xmlNodePtr cur);
int load_store_feedback(xmlNodePtr cur);

//---------------------------------------------------------------------------------------------
// db_XXX function
//---------------------------------------------------------------------------------------------

#endif  //__KF_COUPONS_HPP__

