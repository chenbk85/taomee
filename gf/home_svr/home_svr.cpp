/*
 * =====================================================================================
 *
 *       Filename:  home_svr.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/04/2011 05:35:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */

#include "home_svr.hpp"
#include "player.hpp"
#include "home.hpp"
#include "utils.hpp"
#include "cli_proto.hpp"
#include "home_impl.hpp"
#include <kf/home.hpp>
using namespace taomee;

/** 
 * @brief 进入自己的小屋
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int enter_home_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	enter_home_rsp_t * pkg = reinterpret_cast<enter_home_rsp_t *>(body);

	CHECK_VAL_EQ(bodylen, sizeof(enter_home_rsp_t) + 
			(pkg->info.clothes_cnt) * sizeof(clothes_base_t) +  (pkg->info.summon_cnt) * sizeof(summon_t));

	p->init_player(&(pkg->info));

	DEBUG_LOG("PLAYER IN SELF HOME: %u", p->id);

	Home * home = FindOneHome(p->id, p->role_tm);
	if (home) {
		home->EnterHome(p);
	} else {
		Home * home = CreateSelfHome(p);
		home->EnterHome(p);
		db_get_home_data(p);
	}

	int idx = sizeof(home_proto_t);
	pack_h(pkgbuf, pkg->ownerid, idx);
	pack_h(pkgbuf, pkg->role_time, idx);
	init_home_proto_head(pkgbuf, p->waitcmd, idx);
	DEBUG_LOG("SEND TO CLIENT ENTER OK! %u", p->id);
	return send_to_player(p, pkgbuf, idx, 1); 

}

/** 
 * @brief 访问他人小屋
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int enter_friend_home_cmd(Player * p, uint8_t *body, uint32_t bodylen)
{
	enter_home_rsp_t * pkg = reinterpret_cast<enter_home_rsp_t *>(body);

	CHECK_VAL_EQ(bodylen, sizeof(enter_home_rsp_t) + 
			(pkg->info.clothes_cnt) * sizeof(clothes_base_t) +  (pkg->info.summon_cnt) * sizeof(summon_t));

	p->init_player(&(pkg->info));

	DEBUG_LOG("PLAYER IN SELF HOME: %u", p->id);

	int error_no = 0;

	Home * home = FindOneHome(pkg->ownerid, pkg->role_time);
	if (home) {
	/*if (home->IsFull()) {
			error_no = cli_err_home_full;
			int idx = sizeof(home_proto_t);
			taomee::pack_h(pkgbuf, error_no, idx);
			taomee::pack_h(pkgbuf, pkg->ownerid, idx);
			taomee::pack_h(pkgbuf, pkg->role_time, idx);
			init_home_proto_head(pkgbuf, p->waitcmd, idx);
			DEBUG_LOG("SEND TO CLIENT ENTER OK! %u", p->id);
			return send_to_player(p, pkgbuf, idx, 1); 
		} */
		home->EnterHome(p);
	} else {
		home = CreateFriendHome(pkg->ownerid, pkg->role_time);
		home->EnterHome(p);
	}

	int idx = sizeof(home_proto_t);
	taomee::pack_h(pkgbuf, error_no, idx);
	taomee::pack_h(pkgbuf, pkg->ownerid, idx);
	taomee::pack_h(pkgbuf, pkg->role_time, idx);
	init_home_proto_head(pkgbuf, p->waitcmd, idx);
	DEBUG_LOG("SEND TO CLIENT ENTER OK! %u", p->id);
	return send_to_player(p, pkgbuf, idx, 1); 
}

/** 
 * @brief 离开当前的小屋
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int leave_home_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{

	int idx = 0;
	uint32_t ownerid = 0;
	uint32_t role_time = 0;

	unpack_h(body, ownerid, idx);
	unpack_h(body, role_time, idx);

	DEBUG_LOG("PLAYER LEAVE HOME %u", p->id);
	if (p->CurHome && p->CurHome->OwnerUserId == ownerid &&
			p->CurHome->OwnerRoleTime == role_time) {
		Home * home = p->CurHome;
		home->LeaveHome(p);
		if (home->Empty()) {
			delete home;
		}
		p->CurHome = 0;
	}
	delete p;

	return 0;
}

/** 
 * 
 * @return 
 */
int pet_call_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t pet_tm = 0;
	uint32_t flag = 0;
	unpack_h(body, pet_tm, idx);
	unpack_h(body, flag, idx);
	p->set_summon_flag(pet_tm, flag);
	p->waitcmd = 0;
	return 0;	
}

int pet_callback_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint8_t flag = 0;
	unpack_h(body, flag, idx);
	p->fight_summon_callback(flag);
	p->waitcmd = 0;
	return 0;
}

int pet_move_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	pet_move_t * rsp = reinterpret_cast<pet_move_t*>(body);
	p->summon_move(rsp);
	p->waitcmd = 0;
	return 0;
}

int player_walk_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y, flag;  
	unpack_h(body, x, i); 
	unpack_h(body, y, i); 
	unpack_h(body, flag, i);

	if(p->CurHome) {    
		p->walk(x, y, flag);
	}
	p->waitcmd = 0;
	return 0;
}

int walk_keyboard_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y;
	uint8_t dir;
	uint8_t state;
	unpack_h(body, x, i);
	unpack_h(body, y, i);
	unpack_h(body, dir, i);
	unpack_h(body, state, i);

	if (p->CurHome) {
		p->walk_keyboard(x, y, dir, state);
	}
	p->waitcmd = 0;
	return 0;
}

int player_stand_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{

	int i = 0;
	uint32_t x, y;
	uint8_t dir;
	unpack_h(body, x, i);
	unpack_h(body, y, i);
	unpack_h(body, dir, i);

	if (p->CurHome) {
		p->stand(x, y, dir);
	}
	p->waitcmd = 0;
	return 0;

}

int player_jump_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y;
	unpack_h(body, x, i);
	unpack_h(body, y, i);

	if (p->CurHome) {
		p->jump(x, y);
	}
	p->waitcmd = 0;
	return 0;
}

int player_talk_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	int i = 0;
	userid_t id;
	uint32_t len;         
	unpack_h(body, id, i);
	unpack_h(body, len, i);

	if (p->CurHome) {
		p->talk(body + 8, len, id);
	}
	p->waitcmd = 0;
	return 0;
}

int player_enter_battle_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	if (!p->invisible) {
		p->invisible = 1;
		p->CurHome->send_home_player_leave_info(p);
	}
	p->waitcmd = 0;
	return 0;
}

int player_leave_battle_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	enter_home_rsp_t * pkg = reinterpret_cast<enter_home_rsp_t *>(body);

	CHECK_VAL_EQ(bodylen, sizeof(enter_home_rsp_t) + 
			(pkg->info.clothes_cnt) * sizeof(clothes_base_t) +  (pkg->info.summon_cnt) * sizeof(summon_t));

	p->init_player(&(pkg->info));

	//	p->CurHome->syn_player_info_to_home(p);
	if (p->CurHome && p->CurHome->OwnerUserId == p->id 
			&& p->CurHome->OwnerRoleTime == p->role_tm) {	
		p->CurHome->syn_home_pet_info(p);
	}
	
	if (p->invisible) {
		p->invisible = 0;
		p->CurHome->send_home_player_enter_info(p);
	}
	p->waitcmd = 0;
	return 0;
}

int list_user_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
   int idx = sizeof(home_proto_t);
   idx += sizeof(cli_proto_t);
   p->CurHome->pack_all_home_player_info(pkgbuf, idx, p);
   init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
   init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_list_user, 0, idx-sizeof(home_proto_t));
   return	send_to_player(p, pkgbuf, idx, 1);
}

int list_pet_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	 if (p->CurHome->SummonCnt() != 0){
	 	int idx = sizeof(home_proto_t);
     	idx += sizeof(cli_proto_t);
		p->CurHome->pack_all_home_pet_info(pkgbuf, idx);
     	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
     	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_list_home_pet, 0, idx-sizeof(home_proto_t));
	 	return  send_to_player(p, pkgbuf, idx, 1);
	 } else {
		return	send_request_to_db(p, p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, dbproto_get_summon_list, 0, 0);
	 }
}

int syn_player_info_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	struct syn_pet_rsp_t {
		uint32_t owner_id;
		uint32_t owner_tm;
	}__attribute__((packed));

	syn_pet_rsp_t * pkg = reinterpret_cast<syn_pet_rsp_t*>(body);
	Home * home = FindOneHome(pkg->owner_id, pkg->owner_tm);
	int idx = sizeof(home_proto_t);
	if (home) {
		home->pack_pet_syn_info(pkgbuf, idx);
	} else {
		pack_h(pkgbuf, 0, idx);
	}	
	init_home_proto_head_full(pkgbuf, p->waitcmd, idx, 0);
	return  send_to_player(p, pkgbuf, idx, 1);
}


int player_off_line_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	Home * home = p->CurHome;
	p->waitcmd = 0;
	if(home)
	{
		home->LeaveHome(p);
		p->CurHome = NULL;
		home->send_home_player_offline_info(p);
		if (home->Empty()){
			delete home;      
		}
		delete p;
	}
	return 0;
}

int kick_off_player_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{	
	struct kick_off_player_rsp_t{
		uint32_t guest_id;
		uint32_t guest_role_tm;
	}__attribute__((packed));

	p->waitcmd = 0;

	kick_off_player_rsp_t * pkg = reinterpret_cast<kick_off_player_rsp_t*>(body);
	Home * home = FindOneHome(p->id, p->role_tm);
	if(home && home->check_kick_off_player(p, pkg->guest_id, pkg->guest_role_tm))
	{
		home->kick_off_player(p, pkg->guest_id, pkg->guest_role_tm);
	}
	return 0;
}

int get_home_attr_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	Home* home = p->CurHome;
	if(home)
	{
		if (home->check_home_attr_db_flag() && p->CurHome->IsOwner(p)){
			home->send_home_attr_info(p);
		}else {
			db_get_home_data(p);
		}
	}
	return 0;
}

int level_up_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	if(!p->CurHome->IsOwner(p)){
		return send_header_to_player(p, p->waitcmd,  cli_err_not_in_home, 1);	
	}
	p->CurHome->GetHomeDetail()->home_level_up();
	p->waitcmd = 0;
	return 0;
}

int query_player_count(fdsession_t* fdsess, uint8_t * body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t home_owner_id = 0, home_owner_tm = 0;
    unpack_h(body, home_owner_id, idx);
    unpack_h(body, home_owner_tm, idx);

    uint32_t player_cnt = 0;
    Home * home = FindOneHome(home_owner_id, home_owner_tm);
    if (home) {
        player_cnt = home->HomePlayersCnt();
        /*
           for (PlayerMap::iterator it = homeplayer.begin(); it != homeplayer.end(); ++it) { 
           Player * t_p = it->second;
           if (t_p && t_p != p && !(t_p->invisible)) {	
           player_cnt ++;
           }
           }*/

    }

    TRACE_LOG("player [%u|%u] home player count [%u]", home_owner_id, home_owner_tm, player_cnt);
    idx = sizeof(home_proto_t);
    //idx += sizeof(cli_proto_t);
    pack(pkgbuf, player_cnt, idx);
	init_home_proto_head(pkgbuf, home_query_player_count, idx);
    //init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
    //init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), home_owner_id, cli_proto_query_player_count, 0, idx-sizeof(home_proto_t));
    return send_to_player_by_uid(fdsess, home_owner_id, pkgbuf, idx, 1);
}

int query_player_count_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
    uint32_t player_cnt = 0;
    Home * home = FindOneHome(p->id, p->role_tm);
    if (home) {
        player_cnt = home->HomePlayersCnt();
        /*
           for (PlayerMap::iterator it = homeplayer.begin(); it != homeplayer.end(); ++it) { 
           Player * t_p = it->second;
           if (t_p && t_p != p && !(t_p->invisible)) {	
           player_cnt ++;
           }
           }*/

    }

    TRACE_LOG("player [%u] home player count [%u]", p->id, player_cnt);
    int idx = sizeof(home_proto_t);
    idx += sizeof(cli_proto_t);
    pack(pkgbuf, player_cnt, idx);
    init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
    init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_query_player_count, 0, idx-sizeof(home_proto_t));
    return	send_to_player(p, pkgbuf, idx, 1);
}

