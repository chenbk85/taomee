///
///=====================================================================================
///      @file  match.hpp
///     @brief  卡牌游戏服务器端配对类
///
///  获取玩家卡牌信息，竞赛模式进行赛前配对
///
///  @internal
///    Created  01/16/2009 10:37:01 AM
///   Revision  1.0.0.0
///   Compiler  gcc/g++
///    Company  TaoMee .Inc
///  Copyright  Copyright (c) 2009, sylar
///
///    @author  sylar, sylar@taomee.com
/// This source code is wrote for TaoMee.Inc, ShangHai CN.
///=====================================================================================
///

#ifndef MATCH_HPP
#define MATCH_HPP
#include <queue>
#include <map>
extern "C" {
#include  <libtaomee/log.h>
#include  <gameserv/game.h>
}
#include "../mpog.hpp"
#include "card.hpp"

typedef struct wait_info {
	int rank;
	player_game_info_t *info;

} wait_usr_info;

/**
 * @class Match
 * @brief 卡牌游戏初始化类，用于获得每个用户的卡牌信息，竞赛模式负责找到配对玩家
 */
class Match: public mpog
{
	public:
		//this class construct only once
		Match(game_group_t *grp) {
//			ERROR_LOG("Card Game's manage: Match CONSTRUCTOR");
		}
		void init(sprite_t* p);
		int  handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code);
		int  handle_timeout(void* data);
		int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);

		int load_cards_conf(const char *file);
		~Match() {
			ERROR_LOG("Card Game's manage: Match DESTRUCTOR");
		}
	private:
		/** 保存从配置表中读取的所有卡牌的信息*/
		card_t all_cards[CARD_ID_MAX];
		/** 竞赛模式玩家在对应等级的队列中等候配对*/
		std::deque<player_game_info_t*> competition[10];
		std::deque<player_game_info_t*> advance_compete[5];
		/** 竞赛模式中找不到同等级配对的玩家信息, 只保存蓝色训练服以上玩家*/
		std::deque<wait_usr_info*> wait_q[5];
		/** 两人自由游戏时保存两人所在的游戏组*/
		std::map<game_group_t*, Card*> free_mode;
		/** 计算玩家卡牌游戏等级*/
		inline int calculate_rank (player_cards_info_t *p_info);
		/** 通知客户端服务器端准备完毕*/
		int notify_server_ready(sprite_t *p);
		/** 竞赛模式两个玩家配对*/
		int process(sprite_t *p, sprite_t *rival, player_game_info_t *my_game_info, std::deque<player_game_info_t*>& my_queue, std::deque<player_game_info_t*>& rival_queue);
};
#endif
