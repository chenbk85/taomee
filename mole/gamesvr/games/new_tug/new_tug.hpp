///=====================================================================================
///      @file  new_tug.hpp
///     @brief  游戏服务器端执行类
///
///  执行游戏逻辑，提交分数
///
///  @internal
///    Created  07/28/2010 10:37:01 AM
///   Revision  1.0.0.0
///   Compiler  gcc/g++
///    Company  TaoMee .Inc
///  Copyright  Copyright (c) 2009,
///
///    @author  tommychen
/// This source code is wrote for TaoMee.Inc, ShangHai CN.
///=====================================================================================

#ifndef NEW_TUG_H
#define NEW_TUG_H
#include <vector>
#include <cstdlib>
extern "C" {
#include  <gameserv/game.h>
}
#include "../mpog.hpp"
#include "player.hpp"


#define CARD_ID_MAX				60
#define CARD_TYPE_MAX			3
#define CARD_VALUE_MAX			20
#define CARD_VALUE_MIN			1

const int GAME_READY = 1;


/**
 *	用于记录每张卡牌的具体信息
 */
typedef struct card
{
	int id;
	uint8_t type;
	uint8_t value;
} __attribute__((packed)) card_t;

typedef struct score_info
{
	int exp;
	int coins;
	int medal;
} __attribute__((packed)) score_info_t;


/**
 * 游戏每回合状态，都准备好才能开始
 */
enum game_stat
{
	turn_ready = 0,
	turn_start = 1,
};

enum player_waitcmd
{
	uncompleted,
	completed
};

/**
 *	卡牌属性相克信息数组索引
 */
const int WIN  = 0;
const int LOST = 1;
const int DRAW = 2;


/**
 * 卡牌类型值
 */
enum card_type
{
	water = 0,
	fire = 1,
	wood = 2,
	none_type = 3,
};
/**
 * 卡牌游戏通讯所有使用到的协议号码
 */
enum proto_num
{
	NEW_TUG_SERVER_READY	= 30500,
	NEW_TUG_GET_CARDS		= 30501,
	NEW_TUG_ACT_CARD		= 30502,
	NEW_TUG_GAME_RESULT		= 30503,
	NEW_TUG_CLIENT_READY	= 30504,
	NEW_TUG_USER_INFO		= 30505,
	NEW_TUG_USER_MISS		= 30506,
};


/**
 * @class Cnew_tug
 * @brief 执行游戏逻辑
 */
class Cnew_tug: public mpog
{
	public:
		Cnew_tug (game_group_t *grp);

		int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
		int  handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code);
		int  handle_timeout(void* data) ;

		~Cnew_tug()
		{
			for (uint32_t i = 0; i < players.size(); i++)
			{
				if (players[i] != NULL)
				{
					delete players[i]->get_cards_info();
					delete players[i];
				}
			}
		}

	public:

		//玩家数量
		int players_cnt;
		game_group_t* m_grp;

		/** 记录每个玩家的卡牌，等级，游戏状态*/
		std::vector<Cplayer*> players;

		/** record all cards definition */
		card_t (*cards_info)[CARD_ID_MAX];

		/**  用于得到卡牌属性比较胜负 */
		static int result[3][3];
		int send_cnt;
		int recv_cnt;
		int game_start;
		int time_start;

		//forbidden this funtion
		void init(sprite_t* p){	}
		int compare(card_t p1, card_t p2);
		int send_player_cards(sprite_t* p);
		int notify_game_result();
        int update_db_player_info(sprite_t *p, int exp_in, int xiaomee_in, int medal_in);
		int update_db_players_info( );
		int set_all_card_info(card_t (*all_cards)[CARD_ID_MAX]);
		int set_player(Cplayer* p_player);
		Cplayer* get_player(sprite_t* p);
		void notify_act_card(uint32_t userid, uint32_t type, uint32_t pos);
		int notify_client_ready();
		int process_game_result();
		int on_player_leave(sprite_t* p);
		int update_db_player_info_callback(sprite_t *p, uint32_t ret_code);
		bool is_game_ready();
		int on_card_act(sprite_t* p, int type, int pos);
		int get_score_info(int time, score_info_t& time_score_info);

};

#endif


