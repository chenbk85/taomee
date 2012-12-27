///
///=====================================================================================
///      @file  card.hpp
///     @brief  卡牌游戏服务器端执行类
///
///  执行游戏逻辑，提交分数
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

#ifndef CARD_HPP
#define CARD_HPP
#include <vector>
#include <cstdlib>
extern "C" {
#include  <gameserv/game.h>
}
#include "../mpog.hpp"
#define CARD_GAME_GROUP_BEGIN 	1
#define INVENTORY_MAX 			80
#define WIN_BONUS	  			50
#define LOST_BONUS	 		 	10
#define CARD_ARRAY_NUM			12
#define CARD_ID_MAX				60
#define CARD_TYPE_MAX			3
#define CARD_COLOUR_MAX			5
#define CARD_VALUE_MAX			20
#define CARD_VALUE_MIN			1
#define CARD_EVENT_MAX			17
#define CARD_STAR_MAX			6

/** 游戏等级对应的经验值 */
enum rank_exp
{
	rank1_exp = 100,
	rank2_exp = 250,
	rank3_exp = 500,
	rank4_exp = 1000,
	rank5_exp = 2000,
	rank6_exp = 3000,
	rank7_exp = 4000,
	rank8_exp = 5000,
	rank9_exp = 7000
};

//single game get this item if first win!

/** 挑战瑞琪成功有机会获得的骑士长枪 */
const uint32_t card_spec_item = 12367;

/**
 * 游戏各个等级对应训练服的物品id, 等级0的训练服在卡牌游戏激活时获得
 */
const uint32_t rank_clothe[10] = {
	0,
	12356,
	12357,
	12358,
	12359,
	12360,
	12361,
	12362,
	12363,
	12364,
};


/**
 *	用于记录每张卡牌的具体信息
 */
typedef struct card
{
	int id;
	uint8_t type;
	uint8_t colour;
	uint8_t value;
	uint8_t event;
	uint8_t star;
} __attribute__((packed)) card_t;


/**
 * 用于保存数据库发送的用户卡牌信息
 */
typedef struct u_card_info
{
    uint32_t flag;
	uint32_t win_cnt;
	uint32_t lost_cnt;
	uint32_t basic_card_cnt;
	uint32_t senior_card_cnt;
	uint32_t total_cnt;
	uint32_t card_id[100];
} __attribute__((packed)) player_cards_info_t;


/**
 * 游戏每回合状态，双方都准备好才能开始
 */
enum game_stat
{
	turn_ready,
	turn_start,
};

/**
 * 保存服务器端每个玩家实际卡牌游戏信息
 */
typedef struct game_info
{
	sprite_t *player;
	player_cards_info_t *p_cards_info;
	int status;
	int rank;
	game_info() {
		status = turn_ready;
	}
} player_game_info_t;


/**
 * 游戏类型：单人游戏，双人游戏。挑战模式是单人游戏
 */
enum game_type
{
	single_player = 1,
	multi_players
};

/**
 *	区分游戏玩家,电脑
 */
enum processing_owner
{
	player1,
	player2,
	computer = 1,
};

/**
 *	卡牌属性相克信息数组索引
 */
const int win  = 0;
const int lost = 1;
const int draw = 2;

/**
 * 完成时服务器端可以继续接收玩家指令，否则不能
 */
enum player_waitcmd
{
	uncompleted,
	completed
};

/**
 * 卡牌类型值
 */
enum card_type
{
	water,
	fire,
	wood,
};

/**
 * 在发送给玩家的卡牌队列中，用于标记是否可以挑战成功瑞琪老师
 */
enum cards_info_flag
{
	single_lost,
	single_win,
	game_cards_flag_default = 0xFF
};

/**
 * 卡牌游戏通讯所有使用到的协议号码
 */
enum proto_num
{
	card_server_ready	= 30100,
	card_game_start		= 30101,
	card_rival_done 	= 30102,
	card_bout_result	= 30103,
	card_final_result	= 30104,
	card_player_quit	= 30105,
	card_rival_ready 	= 30106,
	card_single_over	= 30107,
	card_exp_limited	= 30108,
};

/**
 * 用以记录每个玩家能够致胜的卡牌信息
 */
typedef struct score
{
	int cnt;
	int colour[5];
	score(){
		cnt = 0;
	}
}score_t;

/**
 * @class Card
 * @brief 执行游戏逻辑
 */
class Card: public mpog
{
	public:
		Card (game_group_t *grp);

		int  setup(player_game_info_t *p1, player_game_info_t *p2, card_t (*all_cards)[CARD_ID_MAX]);

		int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
		int  handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code);
		/**  */
		int  handle_timeout(void* data){
			return GER_end_of_game;
		}


		~Card() {
			for (int i = 0; i < m_grp->count; i++) {
				delete player[i]->p_cards_info;
				delete player[i];
			}
			if (CHALLENGE_GAME(m_grp->game)) {
				delete player[computer];
			}
			//DEBUG_LOG("%lu\tcard DESTRUCTOR",m_grp->id);
		}
	private:
		/** 记录这是一个单人还是双人游戏 */
		int player_num;
		/** 记录最后胜利的玩家 */
		int final_winner;
		/** 记录当前游戏逻辑所有者 */
		int owner;
		/** 记录当前卡牌事件所有者 */
		int event_owner;
		/** 记录录当前卡牌事件信息 */
		int event_now;
		/** 记录竞赛游戏模式下，提交游戏结果是数据库返回次数*/
		int return_cnt;
		game_group_t 		*m_grp;
		/** 记录每个玩家的卡牌，等级，游戏状态*/
		player_game_info_t 	*player[2];
		/** used to calculate bout_winner */
		card_t 				card[2];
		/** used to record original bout cards info */
		card_t 				bout_info[2];
		/** record all cards definition */
		card_t 				(*cards_info)[CARD_ID_MAX];
		/** cards array for this game */
		std::vector<int> 	cards_id_array[2];
		/**event pointer array  */
		int (Card::*event_impl[CARD_EVENT_MAX]) (card_t card[2], int owner);
		score_t scores[2][3];
		//forbidden this funtion
		void init(sprite_t* p){	}

		int create_cards_array();
		int	processing (sprite_t *p, int position);
		int referee(card_t p1, card_t p2);
		int figure_final_winner(card_t win_card, int winer);

		int send_cards_array ();
		int notify_rival_ready();
		int notify_rival_done();
		int notify_exp_limited();
		int notify_get_item(sprite_t *p, uint32_t item_id);
		int	notify_bout_result(int bout_winer);
		int notify_final_result(int final_winner);
		int get_item(sprite_t *p, int item_id);
        int get_all_item(sprite_t *p, int rank);
        int update_db_player_info(sprite_t *p, int result);
        int update_redclothes(sprite_t *p);
        int get_items_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret);

		inline void set_me_ready(player_game_info_t *player);
		inline void reset_player_status();
		inline bool is_all_ready();
		inline int calculate_rank(player_cards_info_t *p_info);

		/**  用于得到卡牌属性比较胜负 */
		static int result[3][3];

		//event handler
		//event 0;
		int event_none(card_t card[2],int owner);
		//event 1;
		int self_add_one(card_t card[2],int owner);
		//event 2;
		int rival_minus_one(card_t card[2],int owner);
		//event 3;
		int self_add_three(card_t card[2],int owner);
		//event 4;
		int rival_minus_three(card_t card[2],int owner);
		//event 5;
		int self_add_five(card_t card[2],int owner);
		//event 6;
		int rival_minus_five(card_t card[2],int owner);
		//event 7;
		int self_max(card_t card[2],int owner);
		//event 8;
		int rival_min(card_t card[2],int owner);
		//event 9;
		int rival_wood_ban(card_t card[2],int owner);
		//event 10;
		int rival_fire_ban(card_t card[2],int owner);
		//event 11;
		int rival_water_ban(card_t card[2],int owner);
		//event 12
		int exchange (card_t card[2], int owner);
		//event 13;
		int rival_turn_wood(card_t card[2], int owner);
		//event 14;
		int rival_turn_fire(card_t card[2], int owner);
		//event 15;
		int rival_turn_water(card_t card[2], int owner);
		//event 16;
		int rival_lose(card_t card[2], int owner);

};
#endif
