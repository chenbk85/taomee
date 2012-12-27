/**
 *============================================================
 *  @file      common_op.hpp
 *  @brief    common operations such as walk, talk...
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_FIGHT_TEAM_HPP_
#define KF_FIGHT_TEAM_HPP_

#include <libtaomee++/inet/pdumanip.hpp>

#include "cli_proto.hpp"
#include "fwd_decl.hpp"
extern "C" {
#include <libtaomee/dataformatter/bin_str.h>
}

enum team_member_lv {
	member_lv_tmp = 0,
	member_lv_captain = 1,
	member_lv_vice_captain = 2,
	member_lv_team_leader = 3,
	member_lv_normal  = 4,
};

#pragma pack(1)

struct create_fight_team_rsp_t {
	uint32_t team_id;

};

struct set_player_team_flg_rsp_t {
	uint32_t team_id;
};

struct fight_team_member_elem_t {
	uint32_t uid;
	uint32_t role_tm;
	uint32_t join_tm;
	uint32_t level;
	uint32_t t_coins;
	uint32_t t_exp;
};


struct add_fight_team_member_rsp_t {
	uint32_t team_id;
	uint32_t inviter;
	uint32_t member_cnt;
	fight_team_member_elem_t member[];
};


struct team_member_elem_t {
	uint32_t id;
	uint32_t tm;
	uint32_t join_tm;
	uint32_t level;
	uint32_t t_coins;
	uint32_t t_exp;
};
struct get_team_info_rsp_t {
	uint32_t team_id;
	uint32_t captain_uid;
	uint32_t captain_tm;
	char     captain_nick[max_nick_size];
	uint32_t team_coin;
    uint32_t active_flag;
    uint32_t active_score;
	uint32_t score_index;
	char	team_name[max_nick_size];
	uint32_t team_score;
	uint32_t team_exp;
	uint32_t last_tax_tm;
	char    team_mcast[240];
	uint32_t member_cnt;
	team_member_elem_t member[];
};

struct del_team_member_rsp_t {
	uint32_t team_id;
	uint32_t del_uid;
	uint32_t del_role_tm;
};

struct chat_across_team_member_changed_rsp_t {
	uint32_t type;
	uint32_t team_id;
	uint32_t del_uid;
	uint32_t del_role_tm;
};

struct get_team_top10_elem_t {
	uint32_t teamid;
	char team_name[max_nick_size];
	uint32_t captain_uid;
	uint32_t captain_role_tm;
	char captain_name[max_nick_size];
	uint32_t member_cnt;
	uint32_t team_score;
	uint32_t team_exp;
};

struct get_team_top10_rsp_t {
	uint32_t cnt;
	get_team_top10_elem_t elem[];
};


struct team_top10_chache_t {
	uint32_t cnt;
	get_team_top10_elem_t elem[100];
};

struct db_contribute_team_rsp_t {
    uint32_t    team_id;
    uint32_t    team_coin;
    //uint32_t    player_coin;
};

struct db_fetch_team_coin_rsp_t {
    uint32_t    team_id;
    uint32_t    team_coin;
    //uint32_t    player_coin;
};

struct team_active_enter_t {
    uint32_t    team_id;
    uint32_t    need_coin;
};

struct team_active_score_elem_t {
    uint32_t    id;
    char        name[max_nick_size];
    uint32_t    captain_id;
    uint32_t    captain_tm;
    char        captain_nick[max_nick_size];
    uint32_t    member_cnt;
    uint32_t    active_score;
    uint32_t    active_index;
	uint32_t    team_score;
	uint32_t    team_exp;
};

struct team_active_top100_cache_t {
    uint32_t    timestamp;
    uint32_t    type;
    uint32_t    cnt;
    team_active_score_elem_t elem[100];
};

struct team_search_elem_t {
	uint32_t    teamid;
	char        team_name[max_nick_size];
	uint32_t    captain_uid;
	uint32_t    captain_role_tm;
	char        captain_name[max_nick_size];
	uint32_t    member_cnt;
	uint32_t    team_score;
	uint32_t    team_exp;
};

struct team_search_rsp_t {
	uint32_t cnt;
	team_search_elem_t teams[];
};

#pragma pack()

enum {
    create_team_action  = 1,
    join_team_action    = 2
};

//---------------------inline ----------------



//----------------------------------------------------------------
bool is_player_team_captain(player_t* p);

bool is_have_team_member(player_t* p, uint32_t uid, uint32_t role_tm);

bool is_player_have_team(player_t* p);

uint32_t get_team_join_tm(player_t* p);
uint32_t get_team_active_rank(player_t* p);

void send_team_member_changed(player_t* p, uint32_t type, uint32_t team_id, uint32_t uid, uint32_t role_tm);

void send_team_member_onoff(player_t* p, uint32_t type, uint32_t team_id, uint32_t uid, uint32_t role_tm);

int db_chat_across_team_member_changed(player_t* p, uint8_t* body);

void notify_team_info_changed(player_t* p, uint32_t type, uint32_t score);

void notify_team_active_score_change(player_t* p, uint32_t active_flag, uint32_t type, uint32_t score);

bool judge_if_can_get_fight_team_reward(player_t* p, uint32_t swap_id);

void db_set_team_base_info(player_t* p);

void synchro_team_info_to_member(player_t* p, uint32_t is_active_flag, uint32_t add_exp = 0, uint32_t self_flag = 0);

int chat_across_team_info(player_t* p, uint8_t* body);

bool is_player_team_in_top100(player_t* p);

bool is_active_time_member(player_t* p);

int db_set_active_flag(player_t* p, uint32_t coin, uint32_t active_flag, int is_ret = 0);

void set_team_member_exp(player_t * p, uint32_t add_exp);

void set_team_member_coin(player_t * p, uint32_t add_coin);

uint32_t get_team_order_in_100(uint32_t team_id);
//----------------------------------------------------------------


/**
  * @brief 
  */
int create_fight_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief 
  */
int invite_team_member_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief 
  */
int reply_invite_team_member_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief 
  */
int invite_team_member_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief 
  */
int reply_invite_team_member_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
  * @brief 
  */
int get_team_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


int del_team_member_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int del_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int get_team_top10_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief
 */
int contribute_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int fetch_team_coin_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief 
 */
int team_active_enter_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int get_team_active_top100_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int set_team_member_lv_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int change_team_name_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int change_team_mcast_cmd(player_t * p, uint8_t * body, uint32_t bodylen);


bool can_get_team_attr_add(player_t * p);

//-------------------------------------------------------------------------------
// Callbacks: process package from db and send package to client
//-------------------------------------------------------------------------------
/**
  * @brief callback for handling friend adding action returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */  
int db_create_fight_team_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_create_fight_team_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) ;

int db_set_player_team_flg_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) ;

/**
 * @brief
 */
int db_contribute_team_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_fetch_team_coin_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
/**
 * @brief
 */
int db_team_active_enter_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_get_team_active_top100_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
//---------------------------------------------------------------
// little function 
//---------------------------------------------------------------


int pack_rltm_invite_team_member_pkg(uint8_t* buf, uint32_t cli_cmd, player_t* inviter);


int pack_rltm_reply_invite_team_member_pkg(uint8_t* buf, uint32_t cli_cmd, uint32_t accept, player_t* p);


void send_reply_invite_team_member(player_t* p, uint32_t team_id, uint32_t inviter, uint32_t accept);


int db_add_team_member(player_t* p, uint32_t team_id, uint32_t inviter);


int db_add_fight_team_member_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_get_team_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) ;

int db_del_team_member_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_del_team_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) ;

int db_get_team_top10_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) ;


int search_team_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int db_search_team_info_callback(player_t *p, userid_t id, void * body, uint32_t bodylen, uint32_t ret);

int player_add_team_exp_by_pvp(player_t * p, uint32_t continue_win);

int db_team_tax_coin_callback(player_t * p,
	   					userid_t id,
					   	void * body,
					   	uint32_t bodylen,
					   	uint32_t ret);


int player_gain_team_exp_by_swap(player_t * p, uint32_t swap_id);

int check_fight_team_exp_swap(player_t * p, uint32_t swap_id);

struct fight_team_attr_add_t;

void calc_fight_team_attr_add(player_t * p, fight_team_attr_add_t * attr);

class ContestWapper {
 public:
	ContestWapper(player_t * in) 
		: player(in),
		  active_flag(0),
		  enemy_uid(0)
	{

	}

	player_t * player;

	bool can_get_final_reward()
	{
		return true;
	}	
	uint32_t team_id;
	uint32_t active_flag;
	uint32_t enemy_uid;
 private:
	uint32_t  map_id;
	uint32_t  win_flag;
};

enum ContestStatus {
	Contest_NOT_Ready,
	Contest_Ready_Status,
	Contest_Going_Status,
	Contest_Ended_Status,
};

class TeamContestRoom {
  public:
	  TeamContestRoom(uint32_t map)
		  : a_team_id(0),
			b_team_id(0),	
			map_id(map),
		 	room_status(Contest_NOT_Ready),
			win_team(0)
	  {}

	  void update(); 
	  bool isContestOver() { return false;}

	  bool can_join() {
		  return a_team_id == 0 || b_team_id == 0; 
	  }

	  bool CanPlayerJoin(struct player_t * p);

	  void PlayerJoin(struct player_t * p);

	  bool IsTeamContestRoom(uint32_t team_id)
	  {
		  return (team_id == a_team_id || team_id == b_team_id) && team_id;
	  }

	  uint32_t RoomMap()
	  {
		  return map_id;
	  }

	  uint32_t RoomStatus()
	  {
		  return room_status;
	  }
	  uint32_t GetTeamPos(uint32_t team_id)
	  {
		  if (team_id == b_team_id) {
			  return 1;
		  }
		  return 0;
	  }

	  void Init();

	  uint32_t RewardPlayer(player_t * p);

	  bool CanPlayerGetReward(player_t * p);

	  void ContestReady();
	  void ContestStart();
	  void ContestOver();
	  void Contest_Status();
      void ContestTriggerLogic();
	  void ContestMatchFail();

	  bool IsMatched() 
	  {
		  return (a_team_id && b_team_id);
	  }
	  
	  void RewardTeamExp();
	  ContestWapper * GetContestPlayer(uint32_t uid, uint32_t role_tm);

	  void PlayerLeave(struct player_t * p);

	  void team_join_room(uint32_t team_id);
	  //服务器主动通知
	  void SendToRoom(void * buf, uint32_t cmdm, int len); 
  private:
	  bool ContestNeedEnded();
	  void RoomMcast();
	  std::list<ContestWapper*> ATeamMembers;
	  std::list<ContestWapper*> BTeamMembers;
	  std::list<ContestWapper*> GoingMembers;
	  uint32_t a_team_id;
	  uint32_t b_team_id;
	  uint32_t map_id;
	  uint32_t room_status;
	  uint32_t win_team;
	  uint32_t reward_cnt;
	  uint32_t last_mcast_tm;
	  uint32_t next_mcast_tm;
};

ContestWapper * get_player_fight_contest_info(player_t * p);

void player_enter_team_contest_btl(player_t * p);

void player_leave_team_contest_btl(player_t * p, uint32_t win_flag);


//团长报名
int captain_team_contest_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int get_team_contest_room_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

bool can_player_enter_team_contest(player_t * p, uint32_t map_id);

player_t * get_one_team_member_player(uint32_t team_id);

void player_enter_fight_team_contest_map(player_t * p, uint32_t map_id);

bool can_player_get_contest_reward(player_t * p);

uint32_t player_get_contest_reward(player_t * p);

void  reward_player_team_contest_win(player_t * p);

void player_leave_contest_room(player_t * p, uint32_t map_id);

void init_fight_team_contest_info();

void update_contest_room_event();

void player_cancel_contest_team_btl(player_t * p);

void reduce_team_coin(player_t *p, uint32_t coin);

TeamContestRoom *  get_contest_room();
TeamContestRoom *  get_player_team_contest_room(uint32_t team_id);

void add_player_team_active_score(player_t * p, uint32_t active_id,  uint32_t add_score);

int get_player_team_coin_donate(player_t * p);
int get_player_team_exp_donate(player_t * p);


#endif
