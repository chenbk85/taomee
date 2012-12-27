/*
 * =====================================================================================
 *
 *       Filename:  summon.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/12/2011 02:31:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include "summon.hpp"
#include "player.hpp"
#include "home.hpp"
#include "utils.hpp"


int db_get_summon_list_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
 	CHECK_DBERR(p, ret);
	db_get_summon_list_rsp_t* rsp = reinterpret_cast<db_get_summon_list_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(db_get_summon_list_rsp_t) + rsp->mon_cnt * sizeof(summon_mon_info_t));

	p->CurHome->init_owner_monster_info(rsp);
	
 	int idx = sizeof(home_proto_t);
    idx += sizeof(cli_proto_t);
	p->CurHome->pack_all_home_pet_info(pkgbuf, idx);
    init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
    init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_list_home_pet, 0, idx-sizeof(home_proto_t));
	 return  send_to_player(p, pkgbuf, idx, 1);
}


