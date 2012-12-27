///=====================================================================================
///      @file  new_card.hpp
///     @brief  卡牌游戏服务器端执行类
///
///  执行游戏逻辑，提交分数
///
///  @internal
///    Created  05/25/2010 10:37:01 AM
///   Revision  1.0.0.0
///   Compiler  gcc/g++
///    Company  TaoMee .Inc
///  Copyright  Copyright (c) 2009,
///
///    @author  tommychen
/// This source code is wrote for TaoMee.Inc, ShangHai CN.
///=====================================================================================

#ifndef NEW_CARD_H
#define NEW_CARD_H
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

/** 挑战土林长老成功有机会获得的森林骑士长枪 */
const uint32_t card_spec_item = 13476;

const int GAME_READY = 1;

/**
 * 游戏各个等级对应训练服的物品id, 等级0的训练服在卡牌游戏激活时获得
 */
const uint32_t rank_item[5] = {
    0,
    13475,
    13474,
    13473,
    13472,
};

const uint32_t rank_card[5] = {
    0,
    43,
    4,
    25,
    46,
};

const uint32_t monster_level[24] = {
    0,
    28,
    34,
    40,
    40,
	3,
	40,
	50,
	60,
	60,
	50,  //turin
	50,
	70,
	65,
	80,
	60,
	60,
	105,
	75,
	90,
	100,
	125,
	125,
	140,
};


//第一名 %10几率掉出的卡牌
const uint32_t win_card[8] = {
    3,
    23,
    44,
    24,
    5,
    45,
    6,
    26,
};



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
	int cardid;
} __attribute__((packed)) score_info_t;

typedef struct monster_score_info
{
	int xiaomee;
	int exp;
	int strong;
} __attribute__((packed)) monster_score_info_t;

typedef struct monster_info
{
	int mole_exp;
	int xiaomee;
	int strong;
	int cardid;
	int flag;
} __attribute__((packed)) monster_info_t;

typedef struct item_info
{
	int attireid;
	int operatetype;
	int value;
	int maxvalue;
} __attribute__((packed)) item_info_t;


typedef struct grid_info
{
	int type;
	int num;
} __attribute__((packed)) grid_info_t;


/**
 * 游戏每回合状态，都准备好才能开始
 */
enum game_stat
{
	turn_ready = 0,
	turn_start = 1,
};


/**
 * 游戏类型：单人游戏，双人游戏。挑战模式是单人游戏
 */
enum game_type
{
	single_player = 1,
	multi_players
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

enum players_state
{
	ROLL 		=  1,
	MOVE_GRID 	=  2,
	ATTR_SELECT =  3,
	WAR_SELECT	=  4,
	ACT_CARD	=  5,

};


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
 * 格子类型值
 */
enum grid_type
{
	water_t = 0,
	fire_t  = 1,
	wood_t  = 2,
	attr_t  = 3,
	war_t   = 4,
};

/**
 * 卡牌游戏通讯所有使用到的协议号码
 */
enum proto_num
{
	NEW_CARD_SERVER_READY	= 30300,
	NEW_CARD_GET_CARDS		= 30301,
	NEW_CARD_GAME_ROLL		= 30302,
	NEW_CARD_MOVE_GRID		= 30303,
	NEW_CARD_ATTR_SELECT	= 30304,
	NEW_CARD_ACT_CARD		= 30305,
	NEW_CARD_BOUT_RESULT	= 30306,
	NEW_CARD_GAME_RESULT	= 30307,
	NEW_CARD_CLIENT_READY	= 30308,
	NEW_CARD_USER_INFO		= 30309,
	NEW_CARD_WAR_USER		= 30310,
	NEW_CARD_PLAYER_QUIT	= 30311,
	NEW_CARD_SELECT_MONSTER	= 30315,
	NEW_CARD_MONSTER_OVER	= 30313,
	NEW_CARD_DEC_ITEM	= 30314,
};


/**
 * @class Cnew_card
 * @brief 执行游戏逻辑
 */
class Cnew_card: public mpog
{
	public:
		Cnew_card (game_group_t *grp);

		int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
		int  handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code);
		int  handle_timeout(void* data){
			return GER_end_of_game;
		}

		~Cnew_card() {
			for (uint32_t i = 0; i < init_players.size(); i++)
			{
				if (init_players[i] != NULL)
				{
					delete init_players[i]->get_cards_info();
					delete init_players[i];
				}
			}
		}

	public:

		/** 记录这是一个单人还是双人游戏 */
		int player_num;

		//玩家数量
		int init_players_cnt;

		int lost_cnt;

		uint32_t owner_userid;//保存将要走格子或选择属性或选择对战玩家的用户id或丢骰子用户id

		int owner_roll;

		game_group_t* m_grp;

		/** 记录每个玩家的卡牌，等级，游戏状态*/
		std::vector<Cplayer*> player;

		std::vector<Cplayer*> init_players;

		//当前回合玩家id 0,1,2,3
		std::vector<int> bout_playerid;

		//同一格子的其他玩家id
		std::vector<uint32_t> grid_userids;
		uint32_t grid_war_userid;
		bool grid_war;

		//当前回合应该出的卡牌类型
		int bout_type;

		/** record all cards definition */
		card_t (*cards_info)[CARD_ID_MAX];

		/**  用于得到卡牌属性比较胜负 */
		static int result[3][3];
		static int init_player_grid[4];

		int send_cnt;
		int recv_cnt;

		int challenge_win;

		int act_state;

		bool is_get_item;
		int send_item_cnt;
		int recv_item_cnt;

		int bout_cnt;
		int monster_id;
		int game_start;

		int s_xp;
		int m_xp;
		int h_xp;
		int l_xp;
		int itemid;

		int mole_exp;
		int xiaomee;
		int strong;
        int mole_exp_ex;
        int dragon_growth;
        int dragon_growth_ex;

		struct grid_info grid_info_st[16];

		//forbidden this funtion
		void init(sprite_t* p){	}
		int compare(card_t p1, card_t p2);
		int send_player_cards(sprite_t* p);
		int	notify_bout_result();
		int notify_game_result();
        int update_db_player_info(sprite_t *p, int exp_in, int xiaomee_in, int cardid_in);
		int update_db_players_info( );
		inline void bout_clear();
		inline bool is_bout_all_ready();
		void notify_roll_point(uint32_t userid,int point, uint32_t uid);
		int move_grid_direct(int grid_id, int roll_point, int direct);
		void notify_move_grid(uint32_t userid, int id);
		int set_all_card_info(card_t (*all_cards)[CARD_ID_MAX]);
		int set_player(Cplayer* p_player);
		int on_card_act(sprite_t* p, int pos);
		Cplayer* get_player(sprite_t* p);
		Cplayer* get_player(uint32_t userid);
		void notify_act_card_type(uint32_t userid, int type);
		void notify_act_card(uint32_t userid, uint32_t pos);
		void notify_select_war_user(uint32_t userid, uint32_t war_userid);
		int notify_client_ready();
		int process_bout_result();
		int process_game_result();
		bool is_my_turn(sprite_t* p);
		void set_bout_status(int grid_id);
		void set_bout_status_by_attr(int attr_type);
		void set_bout_playerid(int);
		void set_grid_num(int grid_leave, int grid_enter);
		void set_grid_other_userid(int grid_id, uint32_t  userid);
		void set_bout_playerid_by_userid(uint32_t grid_war_userid, uint32_t war_userid);
		int on_player_leave(sprite_t* p);
		void notify_player_leave(uint32_t userid, uint32_t next_userid);
		void modify_bout_player_info(int num);
		void process_player_lost(uint32_t userid);
		void set_act_state(int state);
		void set_act_state_by_grid(int grid_id);
		bool check_bout_userid(uint32_t userid);
		void set_owner_userid_by_grid(int grid_id, uint32_t userid);
		int process_bout_players_lost();
		int get_item(sprite_t *p, int item_id);
		int update_db_player_info_callback(sprite_t *p, uint32_t ret_code);
		int get_item_callback(sprite_t *p, uint32_t ret_code);
		void del_grid_userid(uint32_t userid);
		void notify_monster_result(uint32_t userid, uint32_t result);
		int get_item_by_monster(int monster_id);
		void set_challenge_result(Cplayer* p_player, uint32_t monster_id, uint32_t mole_level, uint32_t high_card);
		int notify_monster_over();
		int on_monster_over(Cplayer* p_player);
		int update_db_monster_info(sprite_t *p, int mole_exp, int xiaomee, int power, int cardid, int flag);
		int update_db_monster_info_callback(sprite_t *p, uint32_t ret_code);
		void notify_dec_item_result(uint32_t userid, uint32_t result, uint32_t win);
		int update_db_dec_item(sprite_t *p, int itemid);
		int on_dec_item(sprite_t* p, int itemid);
		int update_db_dec_item_callback(sprite_t *p, uint32_t ret_code);
		bool is_game_ready();
		int get_game_ready_num();
		int get_items_max(Cplayer* p_player);
		int get_items_by_boss(Cplayer* p_player, int id);
		int get_items_qs(Cplayer *p_player);
		int card_switch_item(int cardid);
		int get_item_without_return(sprite_t *p, int item_id);

};

#endif


