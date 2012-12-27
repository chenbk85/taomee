/*
 * =====================================================================================
 *
 *       Filename:  home.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/04/2011 03:52:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include "home_attr.hpp"
#include "home.hpp"
#include "home_impl.hpp"
#include "home_manager.hpp"
#include "cli_proto.hpp"


using namespace taomee;

Home::Home()
{
	HomeDetail = 0;
	Owner = 0;
}

Home::~Home()
{
	HomeManager & s_mgr = singleton<HomeManager>::instance();
	s_mgr.RemoveHome(OwnerUserId, OwnerRoleTime);
	if (HomeDetail) {
		delete HomeDetail;
	}
}

void Home::Update()
{
	if (HomeDetail) {
		HomeDetail->Update();
	}
}
void Home::send_to_home(void * pkg, uint32_t len, Player * p, uint8_t complete)
{
  for (PlayerMap::iterator it = HomeDetail->HomePlayers().begin();
		  it != HomeDetail->HomePlayers().end(); ++it) {
	  if (it->second != p && !(it->second->invisible)) {
	     send_to_player(it->second, pkg, len, 0);
	  }
   }

  if (p && complete) {
	  p->waitcmd = 0;
  }
}

void Home::pack_all_home_player_info(void * pkg, int & idx, Player *p)
{
	PlayerMap & homeplayer = HomeDetail->Players_;
	int player_cnt = 0; 
	int ifx = idx + 4;
	for (PlayerMap::iterator it = homeplayer.begin(); 
			it != homeplayer.end(); ++it) { 
		Player * t_p = it->second;
		if (t_p && t_p != p && !(t_p->invisible)) {	
			player_cnt ++;
		   t_p->pack_player_map_info(pkg, ifx);
		}
	}

	pack(pkg, player_cnt, idx);
//	ERROR_LOG("ALL THERE %d player's in home!", player_cnt);
	idx = ifx;
}

void Home::pack_all_home_pet_info(void * buf, int & idx)
{
	pack(buf, this->OwnerUserId, idx);
	pack(buf, this->OwnerRoleTime, idx);
	pack(buf, HomeDetail->summon_cnt, idx);
	for (uint32_t i = 0; i < HomeDetail->summon_cnt; ++i) {
		summon_t & mon = HomeDetail->homesummon[i];
	    pack(buf, mon.mon_type, idx); 
		pack(buf, mon.mon_tm, idx);
	 	pack(buf, mon.lv, idx);
	    pack(buf, mon.nick, max_nick_size, idx);
	    pack(buf, mon.call_flag, idx);
	}
}

void Home::pack_pet_syn_info(void * buf, int & idx)
{
	int mon_cnt = 0;
	int ifx = idx + 4;
	//pack_h(buf, this->HomeDetail->summon_cnt, idx);
	for (uint32_t i = 0; i < this->HomeDetail->summon_cnt; i++) {
		summon_t & mon = this->HomeDetail->homesummon[i];
		if (mon.call_flag == 3) {
			pack_h(buf, mon.mon_type, ifx);
			mon_cnt++;
		}
	}
	pack_h(buf, mon_cnt, idx);
	idx = ifx;
}

void Home::send_home_player_offline_info(Player* p)
{
	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, p->role_tm, idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_home_owner_off_line, 0, idx-sizeof(home_proto_t));
	send_to_home(pkgbuf, idx, p, 0);
}


void Home::send_home_player_enter_info(Player * p)
{
	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	p->pack_player_map_info(pkgbuf, idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_enter_map, 0, idx-sizeof(home_proto_t));
	this->send_to_home(pkgbuf, idx, p, 0);
}

void Home::send_home_attr_info(Player* p)
{
	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	p->pack_home_attr_info(pkgbuf, idx);	
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_get_home_attr, 0, idx-sizeof(home_proto_t));
	send_to_player(p, pkgbuf, idx, 1);
}

void Home::syn_home_pet_info(Player * p)
{
	HomeDetail->syn_pet_info(p);
}

void Home::send_home_player_leave_info(Player *p)
{
	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_leave_map, 0, idx-sizeof(home_proto_t));
	this->send_to_home(pkgbuf, idx, p, 0);
}

void Home::send_home_player_kicked(Player* p)
{
	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, p->role_tm, idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_home_kick_off_guest, 0, idx-sizeof(home_proto_t));
	send_to_player(p, pkgbuf, idx, 0);	
}

void Home::change_home_pet_flag(uint32_t mon_tm, uint8_t flag)
{
	for (uint32_t i = 0; i < this->HomeDetail->summon_cnt; i++) {
		summon_t & mon = this->HomeDetail->homesummon[i];
		if (mon.mon_tm == mon_tm) {
			mon.call_flag = flag;
			return;
		}
	}

}

void Home::init_home_attr_data(home_attr_data_rsp_t* rsp)
{
	if(rsp->home_level == 0)
	{
		HomeDetail->home_data.home_level = 1;
	}
	else
	{
		HomeDetail->home_data.home_level = rsp->home_level;
	}
	HomeDetail->home_data.home_exp   = rsp->home_exp;	
	HomeDetail->home_data.db_flag = 1;
	if (rsp->last_randseed_tm) {
		HomeDetail->home_data.last_randseed_tm = rsp->last_randseed_tm;
	} else {
		HomeDetail->home_data.last_randseed_tm = get_now_tv()->tv_sec;
		db_set_home_update_tm(HomeDetail->p_base_home->OwnerUserId, 
				HomeDetail->p_base_home->OwnerRoleTime, HomeDetail->home_data.last_randseed_tm);
	}
}

void Home::EnterHome(Player * p, uint32_t home_type_)
{
	home_type = home_type_;
	
	if (HomeDetail == 0) {
		if (home_type_ == home_type_summon) {
			HomeDetail = new SummonHome(this);
		}
	}

	p->CurHome = this;

	this->send_home_player_enter_info(p);

	if (IsOwner(p)) {
		this->Owner = p;
	}

	HomeDetail->PlayerEnter(p);
}

void Home::LeaveHome(Player * p)
{

	if (HomeDetail) {
	   HomeDetail->PlayerLeave(p);
	} 

	if (IsOwner(p)) {
		this->Owner = 0;
	}

	this->send_home_player_leave_info(p);

	int idx = sizeof(home_proto_t);
	taomee::pack_h(pkgbuf, this->OwnerUserId, idx);
	taomee::pack_h(pkgbuf, this->OwnerRoleTime, idx);
	init_home_proto_head(pkgbuf, p->waitcmd, idx);
	DEBUG_LOG("SEND TO CLIENT ENTER OK! %u %u", p->id, p->waitcmd);
	send_to_player(p, pkgbuf, idx, 1); 
}

bool Home::check_player_in_home(uint32_t userid, uint32_t role_regtime)
{
	PlayerMap::iterator it = HomeDetail->Players_.begin();
	for(;  it != HomeDetail->Players_.end(); ++it)
	{
		Player *p = it->second;
		if(p->id == userid && p->role_tm == role_regtime)return true;
	}	
	return false;
}

bool Home::check_home_attr_db_flag()
{
	return HomeDetail->home_data.db_flag;
}

Player* Home::get_player_in_home( uint32_t userid, uint32_t role_regtime)
{
	PlayerMap::iterator it = HomeDetail->Players_.begin();
	for(;  it != HomeDetail->Players_.end(); ++it)
	{
		Player *p = it->second;
		if(p->id == userid && p->role_tm == role_regtime)return p;
	}
	return NULL;
}

bool Home::check_kick_off_player(Player * p, uint32_t userid, uint32_t role_regtime)
{
	if( !IsOwner(p))return false;
	if( !check_player_in_home(userid, role_regtime))return false;
	return true;
}

void Home::kick_off_player(Player * p, uint32_t userid, uint32_t role_regtime)
{
	p->waitcmd = 0;
	Player* guest = get_player_in_home(userid, role_regtime);
	if(HomeDetail && guest){
		send_home_player_kicked(guest);
	}	
}


bool Home::Empty()const
{
  return HomeDetail->HomePlayers().empty();
}

bool Home::IsFull()const
{
  return (HomeDetail->HomePlayers().size() >= max_home_player);
}

uint32_t Home::HomePlayersCnt()const
{
  return (uint32_t)(HomeDetail->HomePlayers().size());
}

uint32_t Home::SummonCnt()const
{
	return HomeDetail->summon_cnt;
}

void Home::init_owner_monster_info(db_get_summon_list_rsp_t * rsp)
{
	HomeDetail->summon_cnt = rsp->mon_cnt; 
	for (uint32_t i = 0; i < rsp->mon_cnt; ++i) {
		HomeDetail->homesummon[i].mon_type = rsp->mons[i].mon_type;
		HomeDetail->homesummon[i].mon_tm = rsp->mons[i].mon_tm;
		HomeDetail->homesummon[i].lv = rsp->mons[i].lv;
		HomeDetail->homesummon[i].call_flag = rsp->mons[i].fight_flag;
		memcpy(HomeDetail->homesummon[i].nick, rsp->mons[i].nick, max_nick_size);
	}
}


Home * CreateSelfHome(Player *p)
{
	Home * t_home = new Home();
	t_home->SetHomeIndexInfo(p->id, p->role_tm);

	t_home->Owner = p;

	HomeManager & s_mgr = singleton<HomeManager>::instance();
	s_mgr.AddNewHome(t_home);
	return t_home;
}

Home * CreateFriendHome(uint32_t UserId, uint32_t RoleTime)
{
	Home * t_home = new Home();
	t_home->SetHomeIndexInfo(UserId, RoleTime);

	HomeManager & s_mgr = singleton<HomeManager>::instance();
	s_mgr.AddNewHome(t_home);
	return t_home;
}

Home * FindOneHome(uint32_t UserId, uint32_t RoleTm)
{
	HomeManager & s_mgr = singleton<HomeManager>::instance();
	return  s_mgr.FindHome(UserId, RoleTm);
}


void TryDestroyHome(Player * p, Home * home) 
{
	if (home->Owner == p && home->IsOwner(p)) {
		return;
	}

	if (home->Empty()) {
		delete home;
	}
}


void db_add_home_log(uint32_t uid, uint32_t role_tm, Player* p_operator, uint32_t type, uint32_t access_type, uint32_t tm)
{
	int idx = 0;

	pack_h(dbpkgbuf, p_operator->id, idx);
	pack_h(dbpkgbuf, p_operator->role_tm, idx);
	pack_h(dbpkgbuf, p_operator->role_type, idx);
	pack(dbpkgbuf, p_operator->nick, sizeof(p_operator->nick), idx);
	pack_h(dbpkgbuf, type, idx);
	pack_h(dbpkgbuf, access_type, idx);
	if (tm) {
		pack_h(dbpkgbuf, tm, idx);
	} else {
		pack_h(dbpkgbuf, uint32_t(get_now_tv()->tv_sec), idx);
	}
		
	send_request_to_db(0, uid, role_tm, dbproto_add_home_log, dbpkgbuf, idx);
}

