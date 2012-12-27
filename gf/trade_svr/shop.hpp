/*
 * =====================================================================================
 *
 *       Filename:  shop.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/15/10 14:40:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef GF_TRADE_SHOP_HPP
#define GF_TRADE_SHOP_HPP
#include <list>
#include <vector>
extern "C" {
#include <libtaomee/timer.h>
}

#include "fwd_decl.hpp"

extern uint8_t trpkgbuf[1<<21];

class Player;
struct goods_info_t;



struct historyShopKeeper {
	uint32_t uid;
	uint32_t history_tm;
};

enum shop_status_t{
	shop_status_null = 0, //空闲中， 
	shop_status_constructing = 1,//建设�?	shop_status_open = 2,//正常营业�?};
	shop_status_open = 2,
};

enum {
	max_shop_sell_item_cnt = 100,
	max_interval_for_setup_shop = 5,
	max_shop_name_size = 32,
	max_shop_keep_time_limit = 3600,
	max_check_keep_time_interval = 10,
};

class Shop : public taomee::EventableObject 
{
	public:
		Shop();

		~Shop(){}
		void leaveShop(Player* p);
		bool setupShop(Player * p);

		void startShop();

		void pauseShop();

		bool check_history_keeper(uint32_t uid);

		void add_keep_limit_timer();

		void delete_keep_limit_timer();

		int proc_keeper_time_limit(uint8_t& type);

        goods_info_t* get_goods(uint32_t grid_idx);

        Player *  get_shopKeeper();
        int delete_goods(uint32_t idx, uint32_t cnt);
		uint32_t get_goods_price(uint32_t idx);
        void lock_goods(uint32_t idx, uint32_t cnt);

        void unlock_goods(uint32_t idx, uint32_t cnt);

        bool is_locked_goods(uint32_t idx, uint32_t cnt) ;

		bool has_locked_goods();

		void clear_all_goods();
		bool set_grid_goods(uint32_t grid_idx, goods_info_t& goods_in);
		void pack_shop_info(uint8_t * body, int &idx);
		void decorate(uint32_t decoration);

		int list_shop_goods_info(Player *p);

		void change_name(uint8_t * buf, uint32_t len);
		
	public:
	
		uint32_t get_shop_id()
		{
			return id_;
		}
		void set_shop_id(uint32_t shopid)
		{
			id_ = shopid;
		}	
		uint8_t get_shop_status()
		{
			return shop_status;
		}
		
		void set_shop_status(uint8_t status)
		{
			shop_status = status;
		}
		
		void set_shop_open() 
		{
			set_shop_status(shop_status_open);
		}
		
		void set_shop_close() 
		{
			set_shop_status(shop_status_null);
		}
		
		void set_shop_constructing() 
		{
			set_shop_status(shop_status_constructing);
		}
		
		bool is_shop_constructing()
		{
			return (shop_status == shop_status_constructing);
		}

		uint32_t shop_status; // 0 for null, 1 for construct, 2 for ok
		uint32_t shop_start_tm; 
	private:
		std::list<historyShopKeeper> oldKeeper;
		typedef std::list<historyShopKeeper>::iterator iterator;
		std::vector<goods_info_t> goods;

		uint32_t lock_num_;

		uint8_t shop_name[max_shop_name_size];

		uint32_t decoration_id;
		uint32_t id_;
		Player *  shopKeeper;
		taomee::TimedEvent* time_limit_ev;
};

#endif


