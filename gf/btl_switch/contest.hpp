/*
 * =====================================================================================
 *
 *       Filename:  contest.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/10/2012 02:26:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#ifndef HERO_CONTEST_HPP
#define HERO_CONTEST_HPP
#include "online.hpp"
extern "C" {
#include <assert.h>
#include <stdint.h>
}
#include <vector>

class ContestGroup;
struct contest_t;

struct user_contest_req_t {
 uint32_t user_id;
 uint32_t role_tm;
 uint32_t role_type;
 char     nick_name[MAX_NICK_SIZE];
 uint32_t user_lv;
 uint32_t expliot_val;
}__attribute__((packed));

struct user_contest_info_t {
 uint32_t user_id;
 uint32_t role_tm;
 uint32_t role_type;
 char     nick_name[MAX_NICK_SIZE];
 uint32_t user_lv;
 uint32_t out_flag;
 uint32_t init_index;
 uint32_t win_times;
 uint32_t player_status;
 uint32_t lose_flag;
 uint32_t player_val_;

 user_contest_info_t * guess_champion;

 ContestGroup * group;
 contest_t * cur_contest;
}__attribute__((packed));

user_contest_info_t * alloc_user_contest_info(const user_contest_req_t * req);
void del_user_contest_info(user_contest_info_t * info); 

struct contest_t {
  uint32_t start_tm;
  uint32_t status;
  uint32_t winner;
  user_contest_info_t * first_player;
  user_contest_info_t * second_player;
  contest_t * next_contest;
  contest_t * pre_contest;
  int battle_svr_id;
  int room_index;

//  uint32_t end_reason;

  uint32_t index;

  uint64_t session_id;

//  extra_exploit_t * req;

  uint32_t win_val_;
  uint32_t lose_val_;
  uint32_t monster_val_;
  uint32_t spurprise_val_;
  uint32_t monster_killer_;
  list_head_t timer_list;
};

//struct extra_exploit_t {
//	uint32_t enter_val;
//	uint32_t speal_val;
//	uint32_t win_val;
//	uint32_t lose_val;
//	uint32_t 
//};

contest_t * alloc_new_contest();
void del_contest(void * contest);

void set_player_ready(contest_t * contest, user_contest_info_t * player);

void pack_contest_info(contest_t * contest, void * buf, int & idx);

void start_contest(contest_t * contest);

void check_contest_ended(contest_t * contest);

void player_create_contest_room(contest_t * contest, uint32_t btl_id, uint32_t room_index);

void player_enter_contest_room(contest_t * contest, uint32_t btl_id, uint32_t  room_index); 

void pack_user_passed_contest(user_contest_info_t * user, void * buf, int & idx);

contest_t * get_player_cur_contest_by_id(uint32_t uid);

#define USER_SIZE 16
enum contest_status_t {
	STATUS_WAITING = 0,
	STATUS_PREAPREING = 1,
	STATUS_GOING  = 2,
	STATUS_ENDING = 3,

	STATUS_WAITING_CREATE = 4,
	STATUS_WAITING_ENTER = 5,
};

class ContestGroup 
{
 public:
  ContestGroup() 
	: sessionId(0),
	  groupSize_(USER_SIZE),
	  Lv_(0),
	  status_(STATUS_WAITING),
	  end_tm(0),
	  champion(NULL)
  {
	  for (int i = 0; i < (USER_SIZE - 1); i++) {
		  contest_t * contest = alloc_new_contest(); 
		  assert(contest != NULL);
	          allContests_.push_back(contest);
	  }
	  

  }

  ~ContestGroup();

  void JoinIn(user_contest_info_t * user);
  bool CanJoin(user_contest_info_t * user);

  bool CanGuessChampion()
  {
	  return (status_ == STATUS_PREAPREING);
  }

  bool IsGoing()
  {
	  return status_ == STATUS_GOING;
  }

  void InitsessionID(uint32_t uid, uint32_t role_tm); 

  void pack_group_contest_info(void * buf, int & idx);


  void player_contest_btl(uint32_t uid);

  user_contest_info_t * get_user_contest_info_by_id(uint32_t uid);

  contest_t * get_group_contest_by_index(uint32_t index);

  void notify_all_player_over();

  bool IsFull()
  {
	  return allUsers_.size() == USER_SIZE;
  }

  bool IsWaiting()
  {
	  return status_ == STATUS_WAITING;
  }

  void Leave(uint32_t uid);

  bool need_del()
  {
	  return allUsers_.empty();
  }

  bool OutDate()
  {
	 if (status_ == STATUS_ENDING ) {
		  return (end_tm + 600 < (uint32_t)get_now_tv()->tv_sec); 
	  }
	 return false;
  }

  void SetStart();

  void SetEnded(user_contest_info_t * user);

  void SetStatus(uint32_t status)
  {
	  status_ = status;
  }


  void pack_all_user_info(void * buf, int & idx);
  uint32_t get_user_size()
  {
	  return allUsers_.size();
  }

  uint64_t sessionId;
 private:

  void SetGroupLv(user_contest_info_t * user);
  void InitContests();

  std::vector<contest_t*> allContests_;

 // std::vector<UserInfo*> allUsers_;
  std::list<user_contest_info_t*> allUsers_;
  uint32_t groupSize_;
  uint32_t Lv_;
  uint32_t status_;
  uint32_t end_tm;
  user_contest_info_t * champion;
//  uint32_t champion_uid_;
};

ContestGroup * find_contestgroup_by_session(uint64_t sessionId);

ContestGroup * auto_join_group(user_contest_info_t * info);

void update_all_group(int timediff);
void player_win_contest(contest_t * contest, user_contest_info_t * user);

int64_t get_user_old_session(uint32_t uid, uint32_t role_tm);
void save_player_session(uint32_t uid, uint32_t role_tm, uint64_t session);
void clear_user_old_session(uint32_t uid);

void update_all_end_contest();
void init_contest_groups();
void fini_contest_groups();
#endif
