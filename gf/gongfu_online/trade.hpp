/**
 *============================================================
 *  @file      trade.hpp
 *  @brief    trade related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_TRADE_HPP_
#define KF_TRADE_HPP_

#include <libtaomee++/bitmanip/bitmanip.hpp>

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/log.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>
}

#include "fwd_decl.hpp"
#include "cli_proto.hpp"
#include "player.hpp"

struct battle_switch_proto_t;

enum {
	max_trade_grid_cnt = 100, //there may not be more than 100 kinds of goods in one shop
	max_shop_name_size = 40,
	setup_shop_cost    = 500,
};
enum trade_cmd_t {
	tr_cmd_start 		    	= 10001,
	tr_player_enter_market  	= 10001,
    tr_player_leave_market      = 10002,	
	tr_player_change_market     = 10003,
	tr_player_setup_shop        = 10004,
	tr_player_drop_shop         = 10005,
	tr_player_update_shop       = 10006,
	//tr_player_add_goods         = 10006,
	//tr_player_delete_goods      = 10007,
	tr_player_buy_goods         = 10007,
	tr_player_decorate_shop     = 10008,
	tr_player_walk              = 10009,
	tr_player_walk_keyboard     = 10010,
	tr_player_stand             = 10011,
	tr_player_jump              = 10012,
    tr_player_sell_goods        = 10013,
    tr_player_list_user         = 10014,
	tr_player_get_shop_list     = 10015,
	tr_player_shop_detail       = 10016,
	tr_player_change_shop_name   = 10017,
	tr_player_pause_shop        = 10018,
	tr_player_talk		        = 10019,
	tr_transmit_only     		= 10020,
	tr_player_wear_clothes      = 10021,
	tr_change_summon            = 10022,
	tr_call_summon              = 10023,

    tr_safe_trade_create_room   = 10030,
    tr_safe_trade_join_room     = 10031,
    tr_safe_trade_cancel        = 10032,
    tr_safe_trade_set_item      = 10033,
    tr_safe_trade_action_agree  = 10034,
};

enum {
	max_nor_user_grid_cnt = 10,
	max_vip_user_grid_cnt = 30,
	max_trd_grid_cnt	  = 30,
	max_one_grid_itm_cnt  = 9999,
};

/** 
 * @brief the info of player's trade svr and market
 */
struct trade_grp_t {
	uint32_t market_id;
	uint32_t server_id;
	uint32_t fd_idx;
	/** 
	 * @brief 0 for normal , 1 for shopKeeper
	 */
	uint8_t  trade_type;
	uint32_t sell_cnt;
};



#pragma pack(1)
struct enter_market_rsp_t {
	uint32_t market_id;
	uint32_t server_id;
};

struct get_coins_rsp_t {
	uint32_t left_coins;
};

/**
  * @brief protocol type for trade server
  */
struct tr_proto_t {
	/*! package length */
	uint32_t	len;
	/*! trade group id */
	uint32_t	seq;
	/*! command id */
	uint16_t	cmd;
	/*! errno */
	uint32_t	ret;
	/*! user id */
	userid_t	id;
	/*! package body */
	uint8_t		body[];
};

struct trade_update_shop_item_in_t {
	trade_update_shop_item_in_t(uint32_t grid_in, uint32_t id_in, uint32_t cnt_in, uint32_t price_in) {
		grid_idx = grid_in;
		item_id = id_in;
		item_cnt = cnt_in;
		price = price_in;
	}
	uint32_t grid_idx;
	uint32_t item_id;
	uint32_t item_cnt;
	uint32_t price;
};

struct trade_update_shop_attire_in_t {
	trade_update_shop_attire_in_t(uint32_t grid_in, uint32_t id_in, uint32_t unique_in, uint32_t duration_in, uint32_t price_in) {
		grid_idx = grid_in;
		attire_id = id_in;
		unique_id = unique_in;
		duration = duration_in;
		price = price_in;
	}
	uint32_t grid_idx;
	uint32_t attire_id;
	uint32_t unique_id;
	uint32_t duration;
	uint32_t price;
};

//follow safe trade server trade_time_t
struct trade_item_unit_t {
    uint32_t    item_id;
    uint32_t    itemcnt;
};

struct trade_attire_unit_t {
    uint32_t    attire_id;
    uint32_t    unique_id;
    uint32_t    item_lv;
};


struct trade_recoder_t {
	uint32_t trade_tm; //交易时间
	uint32_t trade_type;//交易类型 1:买,  2:卖
	uint32_t item_id; //物品ID
	uint32_t unique_id;//装备的唯一ID;
	uint32_t duration;//装备的耐久度
	uint8_t  item_lv;//装备的强化等级
	uint32_t item_cnt;
	uint32_t cost_coins;
};

struct trade_action_t {
    uint32_t shop_id;
    uint32_t grid_id;
    uint32_t item_id;
    uint32_t uniquekey;
    uint32_t cnt;
    uint32_t coins;
};

struct buy_goods_rsp_t {
    uint8_t  update_flag;//0--可信;1--不可信
    uint32_t type; //1--attire; 2--item
    uint32_t grid_id;
    uint32_t itemid;
    uint32_t uniquekey;
    uint32_t cnt;
    uint32_t duration;
    uint32_t attirelv;
    uint32_t gettime;
    uint32_t endtime;
    uint32_t leftcoins;
    uint32_t max_bag;
};
struct sell_goods_rsp_t {
	uint32_t error;
    uint32_t type; //1--attire; 2--item
    uint32_t shop_id;
    uint32_t goods_id;
    uint32_t item_id;
    uint32_t uniquekey;
    uint32_t cnt;
    uint32_t leftcoins;
};

struct shop_simple_info_t {
	uint32_t shop_id;
	uint32_t shopkeeper;

};

struct trade_mcast_info_t {
	uint32_t item_id;
	uint32_t front_id;
	char info[max_trade_mcast_size];
};
struct player_mcast_msg_t {
	uint32_t item_id;
	uint32_t front_id;
	uint8_t msg[max_trade_mcast_size];
};

//--------------------------------------------
// safe trade
//--------------------------------------------
struct safe_trade_set_item_rsp_t {
    uint32_t    room_id;
    uint32_t    peer_id;
    uint32_t    player_action;
    uint32_t    xiaomee;
    uint32_t    itemcnt;
    uint32_t    attirecnt;
    uint8_t     items[];
};


struct safe_trade_action_agree_t {
    uint32_t    flag; // 1-->agree failed
    uint32_t    del_xiaomee;
    uint32_t    del_item_cnt;
    uint32_t    del_attire_cnt;
    uint32_t    add_xiaomee;
    uint32_t    add_item_cnt;
    uint32_t    add_attire_cnt;
    uint8_t     datas[];
};

#pragma pack()

typedef std::map<uint32_t, trade_update_shop_item_in_t> TradeItemMap;
typedef std::map<uint32_t, trade_update_shop_attire_in_t> TradeAttireMap;

extern int trade_fds[max_trade_svr_num];

/**
  * @brief Allocate a trade group
  * @return pointer to the newly allocated trade group
  */
inline trade_grp_t* alloc_trade_grp()
{
	TRACE_LOG("alloc trade group");
	return reinterpret_cast<trade_grp_t*>(g_slice_alloc0(sizeof(trade_grp_t)));
}

/**
  * @brief Free a trade group. If it's a vs_wild mode trade, grp->players[1] will be free too
  * @param grp trade group to free
  */
inline void free_trade_grp(trade_grp_t* grp)
{
	if (!grp) {
		return;
	}
	TRACE_LOG("free trade group");
	g_slice_free1(sizeof(trade_grp_t), grp);
}

/**
  * @brief init header of trade server protocol
  * @param p the player who is going to send a pkg to trade server
  * @param id
  * @param header header to be initialized
  * @param len length of the whole protocol package
  * @param cmd client command id  
  */
inline void init_tr_proto_head(const player_t* p, userid_t id, void* header, int len, uint32_t cmd)
{
	tr_proto_t* pkg = reinterpret_cast<tr_proto_t*>(header);

	pkg->len = len;
	pkg->seq = ((p && p->trade_grp) ? p->trade_grp->market_id : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
}

/** 
 * @brief set all the trade svr fd = -1;
 */
void init_trsvr_fds();

/** 
 * @brief laod trade server id and name from xml config
 * 
 * @param cur
 * 
 * @return 
 */
int load_trade_servers(xmlNodePtr cur);

/** 
 * @brief reload trade server cofig
 */
void do_reload_trade_svr_config();

/** 
 * @brief get the fd by the fd , if not exit return -1
 * 
 * @param fd
 * 
 * @return 
 */
int get_trsvr_fd_idx(int fd);

int get_trsvr_fd_idx_by_id(int server_id);

/** 
 * @brief init trade pkg handleer
 * 
 * @return 
 */
bool init_tr_proto_handles();

/** 
 * @brief proc the pkg from trade svr
 * 
 * @param data
 * @param len
 * @param fd
 */
void handle_trade_svr_return(tr_proto_t * data, int len, int fd);

int send_to_trade_svr(player_t* p, const void* pkgbuf, uint32_t len, int index);        

//void tmp_func(player_t* p);

//pkg from client to online cmd
int trade_enter_market_cmd(player_t *p , uint8_t *body, uint32_t len);

//int trade_change_market_cmd(player_t *p, uint8_t *body, uint32_t len);

/** 
 * @brief leave market
 * 
 * @param p
 * @param body
 * @param len
 * 
 * @return 
 */
int trade_leave_market_cmd(player_t *p, uint8_t * body, uint32_t len);

/** 
 * @brief handle the player's setup shop command
 * 
 * @param p
 * @param body
 * @param len
 * 
 * @return 
 */
int trade_setup_shop_cmd(player_t *p, uint8_t *body, uint32_t len);

/** 
 * @brie
 * 
 * @param p
 * @param body
 * @param len
 * 
 * @return 
 */
int trade_shop_detail_cmd(player_t *p, uint8_t * body, uint32_t len);

int trade_list_shop_cmd(player_t *p, uint8_t * body, uint32_t len);

int trade_decorate_shop_cmd(player_t *p, uint8_t * body, uint32_t len);

//int trade_start_shop_cmd(player_t *p, uint8_t * body, uint32_t len);

int trade_pause_shop_cmd(player_t *p, uint8_t * body, uint32_t len);

int trade_drop_shop_cmd(player_t *p, uint8_t * body, uint32_t len);

int trade_change_shop_name_cmd(player_t *p, uint8_t * body, uint32_t len);


/** 
 * @brief use a mcast item , mcast message to all the player's in trade market
 * 
 * @param p
 * @param body
 * @param len
 * 
 * @return 
 */
int trade_mcast_message_cmd(player_t *p, uint8_t * body, uint32_t len);

/** 
 * @brief use a mcast item , mcast message to all the player's
 * @param p
 * @param body
 * @param len
 * @return 
 */
int player_mcast_message_cmd(player_t *p, uint8_t * body, uint32_t len);


//online to trade svr cmd
int trsvr_player_enter_market(player_t *p, uint32_t market_id, int tr_idx);


/**
 * @brief player trade action cmd
 */
int trade_buy_goods_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int trade_enter_aim_market_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief player leave market and enter map
 * 
 * @param p
 * @param map_id the map player enter
 * 
 * @return 
 */
int trsvr_player_leave_market(player_t *p);

int trade_update_shop_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int trsvr_player_change_market(player_t *p, uint32_t market_id);

int trsvr_player_setup_shop(player_t *p, uint32_t shop_id);

int trsvr_player_get_shop_list(player_t *p);



//trade svr to online 

int trsvr_player_enter_market_callback(player_t *p, tr_proto_t * pkg);
int trsvr_player_update_shop_callback(player_t * p, tr_proto_t * pkg);
int trsvr_player_setup_shop_callback(player_t * p, tr_proto_t * pkg);

int trsvr_player_change_market_callback(player_t *p, tr_proto_t * pkg);
int trsvr_player_leave_market_callback(player_t *p, tr_proto_t * pkg);

int trsvr_player_buy_goods_callback(player_t * p, tr_proto_t * pkg);
int trsvr_player_sell_goods_callback(player_t * p, tr_proto_t * pkg);

int trsvr_player_get_shop_list_callback(player_t *p, tr_proto_t * pkg);

int trsvr_player_shop_detail_callback(player_t *p, tr_proto_t * pkg);

int trsvr_player_pause_shop_callback(player_t *p, tr_proto_t * pkg);

int trsvr_player_drop_shop_callback(player_t *p, tr_proto_t * pkg);

int trsvr_player_decorate_shop_callback(player_t *p, tr_proto_t * pkg);

int trsvr_player_change_shop_name_callback(player_t *p, tr_proto_t *pkg);

int trade_get_sell_log_cmd(player_t *p, uint8_t *body, uint32_t len);

int db_get_sell_log_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
int db_get_coins_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
int db_check_update_trade_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
//common walk and stand 
int trsvr_player_walk(player_t *p, uint32_t x, uint32_t y, uint32_t flag);
int trsvr_player_walk_keyboard(player_t *p, uint32_t x, uint32_t y, uint8_t dir, uint8_t state);
int trsvr_player_stand(player_t *p, uint32_t x, uint32_t y, uint8_t dir);
int trsvr_player_jump(player_t *p, uint32_t x, uint32_t y);
int trsvr_player_talk(player_t* p, uint8_t* msg, uint32_t msg_len, userid_t recvid);

int trsvr_player_list_user(player_t *p);

//form tr switch
/** 
 * @brief the first time player enter trade svr get a ok trade market
 * 
 * @param p player
 * @param data market_id data switch recommend
 * @param len
 * 
 * @return 
 */
int trade_sw_recommend_market_callback(player_t *p, battle_switch_proto_t *data, uint32_t len);

/** 
 * @brief tradeswitch tell player where the aimed market is 
 * 
 * @param p
 * @param data
 * @param len
 * 
 * @return 
 */
int trade_sw_change_market_callback(player_t *p, battle_switch_proto_t *data, uint32_t len);


//--------------------------------------------------------
//  safe trade function
//--------------------------------------------------------
/** 
 * @brief invite safe trade 
 * @param p
 * @param body
 * @param bodylen
 * @return 
 */
int invite_safe_trade_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief  trade svr callback pkg for one drop his shop
 * 
 */
int trsvr_safe_trade_create_room_callback(player_t *p, tr_proto_t * pkg);

int trsvr_safe_trade_join_room_callback(player_t *p, tr_proto_t * pkg);

/** 
 * @brief reply safe trade 
 * @param p
 * @param body
 * @param bodylen
 * @return 
 */
int reply_safe_trade_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief  safe trade set item
 * @param p
 * @param body
 * @param bodylen
 * @return 
 */
int safe_trade_set_item_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

int do_cancel_safe_trade(player_t *p);

/** 
 * @brief create safe trade room 
 * @param p
 * @param body
 * @param bodylen
 * @return 
 */
int cancel_safe_trade_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief  trade svr callback pkg
 * 
 */
int trsvr_safe_trade_cancel_callback(player_t *p, tr_proto_t * pkg);

/** 
 * @brief  trade svr callback pkg
 */
int trsvr_safe_trade_set_item_callback(player_t *p, tr_proto_t * pkg);

/** 
 * @brief  safe trade 
 * @param p
 * @param body
 * @param bodylen
 * @return 
 */
int safe_trade_action_agree_cmd(player_t *p, uint8_t * body, uint32_t bodylen);


/** 
 * @brief  trade svr callback pkg
 */
int trsvr_safe_trade_action_agree_callback(player_t *p, tr_proto_t * pkg);

#endif






