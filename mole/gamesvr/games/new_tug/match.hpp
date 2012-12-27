///=====================================================================================
///      @file  match.h
///     @brief  游戏服务器端配对类
///
///  竞赛模式进行赛前配对
///
///  @internal
///    Created  07/27/2010 10:37:01 AM
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
#include "new_tug.hpp"


/**
 * @class Cmatch
 * @brief 游戏初始化类，负责找到配对玩家
 */
class Cmatch: public mpog
{
	public:
		//this class construct only once
		Cmatch(game_group_t *grp)
		{
			DEBUG_LOG("New Tug Game's manage: Match CONSTRUCTOR");
		}
		void init(sprite_t* p);
		int  handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code);
		int  handle_timeout(void* data);
		int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
		int  load_tugs_conf(const char *file);

		~Cmatch()
		{
			DEBUG_LOG("New Tug Game's manage: Match DESTRUCTOR");
		}

	private:

		card_t all_cards[CARD_ID_MAX];

		/** 竞赛模式玩家在对应等级的队列中等候配对*/
		std::vector<Cplayer*> tug_players;

		/** 通知客户端服务器端准备完毕*/
		int notify_server_ready(sprite_t *p);

		int notify_user_info(Cnew_tug* tuggame);

		int notify_user_info();

		/** 竞赛模式2个玩家配对*/
		int mgr_player_match(sprite_t *p, Cplayer *p_player);

};

#endif


