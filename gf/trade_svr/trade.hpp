/**
 *============================================================
 *  @file      item.hpp
 *  @brief    item related functions are declared here.
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFTRD_TRADE_HPP_
#define KFTRD_TRADE_HPP_

#include "fwd_decl.hpp"
#include <kf/item_impl.hpp>
#include <libtaomee++/event/event_mgr.hpp>
#include "cli_proto.hpp"
#include "singleton.hpp"

//--------------------------------------------------------------------------------
// struct
//--------------------------------------------------------------------------------
class Player;

enum {
	item_type_normal  = 0,
	item_type_clothes,
};

#pragma pack(1)

struct clothes_base_t {
    uint32_t    id;
    uint32_t    gettime;
    uint32_t    timelag;
    uint32_t    lv;
};


struct summon_t {
	uint32_t mon_type;
	uint32_t mon_tm;
	uint16_t lv;
	uint8_t nick[max_nick_size];
	uint8_t call_flag;
};

struct player_info_t {
	uint32_t market_id;
	uint32_t id;
	uint32_t role_tm;
	uint32_t role_type;
	uint32_t power_user;
	uint32_t player_show_state;
	uint32_t vip;
	uint32_t vip_level;
	uint32_t achieve_title;
	char nick[max_nick_size];
	uint16_t lv;
	uint32_t flag;
	uint32_t app_mon;
	uint32_t honor;
	uint32_t coins;
	uint32_t xpos;
	uint32_t ypos;
	uint32_t clothes_cnt;
	uint16_t summon_cnt;
	uint32_t fight_mon_tm;
	//uint32_t clothes_id[];
    clothes_base_t clothes_arr[];
};

struct update_shop_t {
	uint32_t market_id;
	uint32_t shop_id;
	uint32_t itm_cnt;
	uint32_t attire_cnt;
};

struct update_shop_item_in_t {
	uint32_t grid_idx;
	uint32_t item_id;
	uint32_t item_cnt;
	uint32_t price;
};

struct update_shop_attire_in_t {
	uint32_t grid_idx;
	uint32_t attire_id;
	uint32_t unique_id;
	uint32_t lv;
	uint32_t duration;
	uint32_t price;
};

struct setup_shop_t {
	uint8_t shop_id;
};

struct close_shop_t {
	uint8_t shop_id;
};

struct goods_lock{
	goods_lock() : flg(0), cnt(0) {}
    uint8_t  flg; //0--未锁定; 1--锁定
    uint32_t cnt; //锁定物品的数量
};

struct goods_info_t {
	goods_info_t(): item_type(0), item_id(0), 
						unique_id(0), item_tm(0), 
						item_lv(0), duration(0), 
						price(0), cnt(0)
	{}
	uint8_t  item_type; //物品类型 0:普通物品，1:装备
	uint32_t item_id;   //物品ID
	uint32_t unique_id; // 装备唯一ID
	uint32_t item_tm;	//装备时间,
	uint8_t  item_lv; //装备强化等级， 普通商品都为0 
	uint32_t duration; //装备耐久
	uint32_t price; //单价
	uint32_t cnt; //数量
    goods_lock lock;
};


struct buy_goods_rsp_t {
    uint32_t shop_id;
	uint32_t goods_id;
	uint32_t goods_cnt;
	uint32_t itemid;
	uint32_t uniqueid;
	uint32_t coins;
    uint32_t max_bag;
};

struct db_buy_goods_rsp_t {
    uint32_t type; //1--attire 2--item
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

struct db_sell_goods_rsp_t {
	uint32_t error;
    uint32_t type; //1--attire 2--item
    uint32_t shop_id;
    uint32_t goods_id;
    uint32_t itemid;
    uint32_t uniquekey;
    uint32_t cnt;
    uint32_t leftcoins;
};

struct decorate_rsp_t {
	uint32_t shop_id;
	uint32_t decorate_cnt;
	uint32_t decorate_id[];
};

struct clothes_rsp_t {
	uint32_t clothes_id;
	uint32_t unique_id;
	uint32_t duration;
	uint32_t attirelv;
};

struct wear_clothes_rsp_t {
	uint32_t clothes_cnt;
	clothes_rsp_t clothes[];

};

struct change_summon_rsp_t {
	uint32_t mon_tm;
	uint32_t mon_type;
	uint16_t mon_lv;
	uint8_t  mon_nick[max_nick_size];
};

struct leave_market_rsp_t {
	uint32_t map_id;
};

#pragma pack()

void init_market();

void fini_market();



extern uint8_t pkgbuf[1<<21];

/** 
 * @brief send a pkg to all the player in market a shop closed
 * 
 * @param p
 * @param shopid
 * @param complete 
 * 
 * @return 
 */
int send_drop_shop_rsp(Player* p, uint32_t shopid, uint32_t complete = 1);

//--------------------------------------------------------------------------------
// online cmd
//--------------------------------------------------------------------------------


/**
* @brief enter market
*/
int enter_market_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/** 
 * @brief get usr info in the market
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int list_user_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/** 
 * @brief usr leave the market
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int leave_market_cmd(Player *p, uint8_t* body, uint32_t bodylen);

/** 
 * @brief change market in this trade server
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int change_market_cmd(Player *p, uint8_t *body, uint32_t bodylen);

/** 
 * @brief get the simple info of the shops in this market
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int list_shop_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief cmd for decorate the shop
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int decorate_shop_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief before usr update the goods info, this cmd should be send to pause the shop
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int pause_shop_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief usr list all the goods info of a shop
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int get_shop_detail_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief the shopkeeper drop a shop
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int drop_shop_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief shopkeeper change the shop's name 
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int change_shop_name_cmd(Player *p, uint8_t * body, uint32_t bodylen);



/**
* @brief set up a shop
*/
int setup_shop_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief update shop
*/
int update_shop_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief close a shop
*/
int close_shop_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief player buy goods action
 */
int buy_goods_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief walk
*/
int walk_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief walk keyboard
*/
int walk_keyboard_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief stand
*/
int stand_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief jump
*/
int jump_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief talk
*/
int talk_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/** 
 * @brief wear clothes
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int wear_clothes_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief player change a summon 
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int change_summon_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief player call fight summon , set call_flag 
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int call_summon_cmd(Player *p, uint8_t * body, uint32_t bodylen);


//------------------------------------------------------------------------------
//callback
//------------------------------------------------------------------------------

/**
 * @brief player buy goods db callback
 * @return 0 on success, -1 on error
 */
int db_buy_goods_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
/**
 * @brief player sell goods db callback
 * @return 0 on success, -1 on error
 */
int db_sell_goods_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

//--------------------------------------------------------------------------------
// global 
//--------------------------------------------------------------------------------
//
//TRADE SWITCH
#pragma pack(1)
struct trade_sw_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t sender_id;
	uint8_t body[];
};
#pragma pack()
enum trade_sw_cmd_t {
	tr_sw_init_pkg    = 64001,
	//tr_sw_keep_trade_alive   = 50002,
	tr_sw_player_enter_market = 64002,
	tr_sw_player_leave_market = 64003,
//	tr_sw_player_change_market = 64005,
};

/*init the trade svr info */
void init_trade_svr();

/*fini trade svr info befor close the svr */
void fini_trade_svr();

int connect_to_trade_switch();

int send_to_trade_switch(uint8_t *buf, uint32_t len);
/** 
 * @brief send the trade svr id and player num in each market to the trade switch when the frist time
 * 
 * @return 
 */
int send_init_pkg();
//int trade_sw_keep_trade_alive(){return 0;}
/** 
 * @brief  send to trade switch player  enter a market
 * 
 * @param p
 * @param market_id
 * 
 * @return 
 */
int trade_sw_player_enter_market(Player *p, uint32_t market_id);
/** 
 * @brief send to trade switch player leave a market
 * 
 * @param p
 * @param market_id
 * 
 * @return 
 */
int trade_sw_player_leave_market(Player *p, uint32_t market_id);
//int trade_sw_player_change_market(Player *p, uint32_t market_id);

class TradeSvr : public taomee::EventableObject {
	public:
		TradeSvr(){}
		void init()
		{
			timeval tv = *get_now_tv();
			tv.tv_sec += 1;
			aliveEvent = emgr.add_event(*this, &TradeSvr::keep_trade_alive, tv, 3, -1);
			server_id = get_server_id();
		}
		void fini()
		{
			ev_mgr.remove_event(aliveEvent);
		}

		void proc_events()
		{
			emgr.process_events();
		}

		uint32_t server_id;

		~TradeSvr(){}
	private:
		int keep_trade_alive() {
			timeval tv = *get_now_tv();
			tv.tv_sec += 60;
			//emgr.remove_event(aliveEvent);
			//aliveEvent = emgr.add_event(*this, &TradeSvr::keep_trade_alive, tv);
			TRACE_LOG("SS");
			connect_to_trade_switch();
			return 0;
		}
		taomee::TimedEvent * aliveEvent;
		taomee::EventMgr emgr;
};	

extern TradeSvr sSvr;
extern int  trade_switch_fd;

#endif //KFTRD_TRADE_HPP_
