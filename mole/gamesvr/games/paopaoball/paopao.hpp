///
///=====================================================================================
///      @file  paopao.hpp
///     @brief 抛抛球游戏执行类
///
/// 转发游戏消息包，决定风向
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

#ifndef PAOPAO_HPP
#define PAOPAO_HPP
extern "C" {
#include  <libtaomee/log.h>
#include  <gameserv/game.h>
}
#include "../mpog.hpp"

enum paopao_proto
{
	game_start		= 32200,
	serv_ready		= 32201,
	wind_direction	= 32202,
	item_choose		= 32203,
	data_transfer	= 32204,
	game_over		= 32205,
};
/**
 * @class Paopao
 * @brief 处理各种游戏信息
 */
class Paopao: public mpog
{
	public:
		//this class construct only once
		Paopao(game_group_t *grp);
		void init(sprite_t* p);
		int  handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code);
		int  handle_timeout(void* data);
		int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);

	private:
		int notify_wind_direction();
		int notify_serv_ready();
		int do_data_transfer(sprite_t *p, const uint8_t body[], int body_len, int proto_num);
		bool is_bout_ready();
		int bout_status;
		int bout_cnt;
		int owner;
		uint32_t winner;
		bool is_game_over;
		game_group_t *m_grp;
};
#endif
