/*
 * =====================================================================================
 *
 *       Filename:  market.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/15/10 14:59:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef GF_TRADE_MARKET_HPP
#define GF_TRADE_MARKET_HPP

#include "shop.hpp"
#include "player.hpp"


#include <vector>

extern uint8_t trpkgbuf[1<<21];

enum {
	max_shop_cnt = 8,
	max_player_cnt = 30,
};

class Market
{
	public:
		typedef std::map<uint32_t, Player*> PlayerMap;
	public:
		Market(int id);

		~Market(){}

		void Player_Enter(Player * p);

		int list_users(Player* p);

		void Player_Leave(Player *p);
		void player_leave_map(Player *p);

		void player_enter_map(Player *p);

		bool is_full();

		Shop * get_shop(uint32_t shop_id);

		Shop* get_unused_shop(uint32_t shop_id);

		Shop* get_used_shop(uint32_t shop_id);
		void send_to_market(Player* p, uint8_t *buf, uint32_t len, uint32_t completed);

		int list_shops(Player *p);

		void shop_status_change(uint32_t shop_id);
		
	public:
		PlayerMap players;
		uint32_t player_count;
		int   market_id;
	private:
		
		std::vector<Shop> Shopes;
		typedef std::map<uint32_t, Player*>::iterator iterator;
};
#endif


