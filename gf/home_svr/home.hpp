/*
 * =====================================================================================
 *
 *       Filename:  home.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/04/2011 03:12:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef HOME_HPP
#define HOME_HPP

#include <libtaomee++/memory/mempool.hpp>
#include "summon.hpp"
#include "player.hpp"
#include "home_attr.hpp"

class SummonHome;

enum {
	home_type_summon = 1,
};

enum {
	home_log_type_water = 1,
	home_log_type_ferilizer,
	home_log_type_steal,
	home_log_type_btl,
};

enum {
	home_log_access_type_all = 0,
	home_log_access_type_owner,
	home_log_access_type_guest,
};

class Home : public taomee::MemPool {
 public:

  Home();

  ~Home();

  void EnterHome(Player * p, uint32_t home_type_ = home_type_summon);

  void LeaveHome(Player * p);


  bool IsOwner(Player *p)
  {
	  return (OwnerUserId == p->id && OwnerRoleTime == p->role_tm);
  }

  bool IsSummerHome() 
  {
  	return (home_type == home_type_summon);
  }

  bool Empty()const;

  bool IsFull()const;

  uint32_t HomePlayersCnt()const;

  void SetHomeIndexInfo(uint32_t UserId, uint32_t UserRoleTime)  
  {
	  OwnerUserId = UserId;
	  OwnerRoleTime = UserRoleTime;
  }

  bool check_kick_off_player(Player * p, uint32_t userid, uint32_t role_regtime);

  void kick_off_player(Player * p, uint32_t userid, uint32_t role_regtime);

  bool check_player_in_home( uint32_t userid, uint32_t role_regtime);

  bool check_home_attr_db_flag();

  Player* get_player_in_home( uint32_t userid, uint32_t role_regtime);

  void send_to_home(void * pkg, uint32_t len, Player * p, uint8_t complete);

  void pack_all_home_player_info(void * pkg, int & idx, Player * p = 0);

  void pack_all_home_pet_info(void * pkg, int & idx);

  void pack_pet_syn_info(void * pkg, int & idx);

  void send_home_player_enter_info(Player *p);

  void send_home_player_leave_info(Player *p);

  void send_home_player_kicked(Player* p);

  void send_home_player_offline_info(Player* p);

  void send_home_attr_info(Player* p);

  void syn_home_pet_info(Player *p);

  void change_home_pet_flag(uint32_t mon_tm, uint8_t flag);

  void init_owner_monster_info(db_get_summon_list_rsp_t * rsp);

  void init_home_attr_data(home_attr_data_rsp_t* rsp);


  void Update();

  uint32_t OwnerUserId;
  uint32_t OwnerRoleTime;

  Player * Owner;
  
  SummonHome* GetHomeDetail() {
  	return HomeDetail;
  }

  uint32_t SummonCnt()const;

 private:
  uint32_t home_type;
  SummonHome* HomeDetail;
  uint32_t LastActionTm_;
};

Home * CreateSelfHome(Player * p);

Home * CreateFriendHome(uint32_t UserId, uint32_t RoleTm);

Home * FindOneHome(uint32_t UserId, uint32_t RoleTm);

void  TryDestroyHome(Player *p, Home * home);

void db_add_home_log(uint32_t uid, uint32_t role_tm, Player* p_operator, uint32_t type, uint32_t access_type = home_log_access_type_all, uint32_t tm = 0);
#endif


