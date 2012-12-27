///=====================================================================================
///      @file  match.h
///     @brief  卡牌游戏服务器端配对类
///
///  获取玩家卡牌信息，竞赛模式进行赛前配对
///
///  @internal
///    Created  05/25/2010 10:37:01 AM
///   Revision  1.0.0.0
///   Compiler  gcc/g++
///    Company  TaoMee .Inc
///  Copyright  Copyright (c) 2009
///
///    @author  tommychen
/// This source code is wrote for TaoMee.Inc, ShangHai CN.
///=====================================================================================

#ifndef MATCH_H
#define MATCH_H
#include <queue>
#include <map>
extern "C" {
#include  <libtaomee/log.h>
#include  <gameserv/game.h>
}
#include "../mpog.hpp"
#include "new_card.hpp"


/**
 * @class Cmatch
 * @brief 卡牌游戏初始化类，用于获得每个用户的卡牌信息，竞赛模式负责找到配对玩家
 */
class Cmatch: public mpog
{
	public:
		//this class construct only once
		Cmatch(game_group_t *grp) {
			DEBUG_LOG("Card Game's manage: Match CONSTRUCTOR");
		}
		void init(sprite_t* p);
		int  handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code);
		int  handle_timeout(void* data);
		int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
		int  load_cards_conf(const char *file);
		~Cmatch() {
			DEBUG_LOG("Card Game's manage: Match DESTRUCTOR");
		}

	private:
		/** 保存从配置表中读取的所有卡牌的信息*/
		card_t all_cards[CARD_ID_MAX];

		/** 竞赛模式玩家在对应等级的队列中等候配对*/
		std::vector<Cplayer*> competition[5];

		//等待玩家队列
		std::vector<Cplayer*> wait_players;

		/** 4人自由游戏时保存4人所在的游戏组*/
		std::map<game_group_t*, Cnew_card*> free_mode;

		/** 通知客户端服务器端准备完毕*/
		int notify_server_ready(sprite_t *p);

		int notify_user_info(Cnew_card* cardgame);

		int notify_user_info(uint32_t rank);

		int notify_wait_user_info( );

		/** 竞赛模式4个玩家配对*/
		int process_compete_mode(sprite_t *p, Cplayer *p_player);

		void remove_wait_player(Cplayer* p_player);

		void remove_compet_player(Cplayer* p_player);

		bool is_in_wait_player(Cplayer* p_player);

};

#endif


