/*
 * =====================================================================================
 *
 *       Filename:  adventure.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/12/2012 04:49:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include <algorithm>
#include <libtaomee++/random/random.hpp>
#include <netinet/in.h>

extern "C"
{
#include "../../proto.h"
#include "../../dbproxy.h"
#include "../../timer.h"
#include "../../game.h"

#ifndef TW_VER
#include  <statistic_agent/msglog.h>
#endif
}

#include "../../ant/inet/pdumanip.hpp"
#include "stdlib.h"
#include <vector>
#include <cmath>
#include "adventure.h"
#include <ctime>

#define PASS_GATE_TIME       60
#define PROCESS_TIME_LIMIT   50

adventure::adventure(game_group_t *m_grp, sprite_t *player1, sprite_t* player2)
{
	this->m_grp = m_grp;
	m_player_count = 0;
	m_game_state = GAME_START;
	m_last_time = 0;
	m_pvp_time = 0;
	m_players[0] = player1;
	if(player1 != NULL){
		++m_player_count;
	}
	m_players[1] = player2;
	if(player2 != NULL){
		++m_player_count;
	}
	
	for(int k = 0; k < 2; ++k){
		m_process[k] = 0;
	}
}

int	adventure::handle_data( sprite_t* p, int cmd, const uint8_t body[], int len )
{
	switch(cmd){
		case proto_player_leave:
			{
				notify_game_over(p, true);
				break;
			}
		case proto_adventure_game_store:
			{
				uint32_t now = time(0);
				if(now - m_last_time >= PASS_GATE_TIME){
					CHECK_BODY_LEN(len, 8);
					int offset = 0;
					uint32_t db_buff[2] = {0};
					ant::unpack(body, db_buff[0], offset);
					ant::unpack(body, db_buff[1], offset);
					return send_request_to_db(db_proto_set_ocean_adventure, p, sizeof(db_buff), db_buff, p->id);
				}
				
				break;
			}
		case proto_adventure_game_process:
			{
				CHECK_BODY_LEN(len, 4);
				uint32_t flag = 0;
				int l = 0;
				ant::unpack(body, flag, l);
				return handle_pass_gate_process(p, flag);
			}
	}
	return 0;
}

int adventure::handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code )
{
	switch(m_game_state){
		case GAME_START:
			{
				CHECK_BODY_LEN_GE(len, 8);
				uint32_t already_pass = *(uint32_t*)buf;
				uint32_t count = *(uint32_t*)((char*)buf+4);

				typedef struct{
					uint32_t prime;
					uint32_t secondary;
				}pass_gate_t;

				CHECK_BODY_LEN(len, (int)(sizeof(pass_gate_t)*count + sizeof(count)));

				pass_gate_t* cur = (pass_gate_t*)((char*)buf+4);

				int j = sizeof(protocol_t);
				{//compatible for pvp model 
					uint32_t state = 0;
					ant::pack(pkg, state, j);
					uint32_t player_1 = 0;
					ant::pack(pkg, player_1, j);
					uint32_t player_2 = 0;
					ant::pack(pkg, player_2, j);
				}
				ant::pack(pkg, already_pass, j);
				ant::pack( pkg, count, j );
				uint32_t k = 0;
				for(; k < count; ++k){
					ant::pack(pkg, cur->prime, j);
					ant::pack(pkg, cur->secondary, j);
				}
				m_game_state = GAME_GOING;

				init_proto_head( pkg, proto_adventure_game_start, j );
				return send_to_self( p, pkg, j, 1);
				
			}
		case GAME_GOING:
			{
				typedef struct{
					uint32_t state;
					uint32_t itemid;
					uint32_t count;
				}award_t;
				CHECK_BODY_LEN(len, (int)(sizeof(award_t)));
				award_t *cur = (award_t*)((char*)buf);
				int j = sizeof(protocol_t);
				ant::pack(pkg, cur->state, j);
				ant::pack(pkg, cur->itemid, j);
				ant::pack(pkg, cur->count, j);

				init_proto_head( pkg, proto_adventure_game_store, j );
				return send_to_self( p, pkg, j, 1);
				break;
			}
	}

	return 0;
}

int	adventure::handle_timeout( void* data )
{
	return 0;
}

void adventure::init( sprite_t* p )
{
	if(m_player_count == 1){
		 sprite_t *db_player = NULL;
		 for(int k = 0; k < 2; ++k){
			 if(m_players[k] != NULL){
				 db_player = m_players[k];
				 break;
			 }
		 }
		 send_request_to_db( db_proto_get_ocean_adventure, db_player, 0, NULL, db_player->id);
	}
	else if(m_player_count == 2){
		notify_game_start();
	}
}

int adventure::notify_game_start()
{
	int j = sizeof(protocol_t);
	uint32_t state = 1;//represent pvp
	ant::pack(pkg, state, j);
	ant::pack(pkg, m_players[0]->id, j);
	ant::pack(pkg, m_players[1]->id, j);
	uint32_t count = 0;
	ant::pack(pkg, count, j);

	init_proto_head( pkg, proto_adventure_game_start, j );
	send_to_players( m_grp, pkg, j );
	return 0;
	
}

int adventure::handle_pass_gate_process(sprite_t *p, uint32_t flag)
{
	if(flag == 1){
		return notify_game_over(p, true);
	}
	uint32_t now = time(0);
	uint32_t state = 0;
	if(now - m_pvp_time > PROCESS_TIME_LIMIT){
		state = 1;
	}
	int k = 0;
	if(state == 1){
		for(; k < PLAYER_COUNT; ++k){
			if(m_players[k] == p){
				m_process[k] = (m_process[k]+1) > 3 ? 3:(m_process[k]+1);
				if(m_process[k] == 3){
					return notify_game_over(p);
				}
				notify_client_broadcast();
				break;
			}
		}//for
	}
	int l = sizeof(protocol_t);
	uint8_t n_msg[256] = {0};
	
	ant::pack( n_msg, state, l );
	ant::pack( n_msg, m_process[k], l );
	init_proto_head( n_msg, proto_adventure_game_process, l );
	send_to_self( p, n_msg, l, 1);
	return 0; 
}

int adventure::notify_client_broadcast()
{
	int msgLen = sizeof(protocol_t);
	ant::pack( pkg, m_players[0]->id, msgLen );
	ant::pack( pkg, m_process[0], msgLen );
	ant::pack( pkg, m_players[1]->id, msgLen );
	ant::pack( pkg, m_process[1], msgLen );

	init_proto_head( pkg, proto_adventure_game_notify, msgLen );
	send_to_players( m_grp, pkg, msgLen );
	return 0;
}

int adventure::notify_game_over(sprite *p, bool flag)
{
	// single represent single game, double represent complete game
	m_winner = 0;
	m_game_state = GAME_OVER  ;
	if(m_player_count == 1){
		int msgLen = sizeof(protocol_t);
		uint32_t single = 1;
		ant::pack( pkg, single, msgLen );
		uint32_t state = 0, itemid = 0;
		ant::pack( pkg, state, msgLen );
		ant::pack( pkg, itemid, msgLen );
		if(m_players[0] != 0){
			m_winner = m_players[0];
		}
		else{
			m_winner = m_players[1];
		}
		init_proto_head( pkg, proto_adventure_game_over, msgLen );
		send_to_self( m_winner, pkg, msgLen, 1);
	}
	else{
		if(flag){
			
			if(m_players[0] == p){
				m_winner = m_players[1];
			}
			else {
				m_winner = m_players[0];
			}
		}
		else{
			m_winner = p;
		}
		uint32_t state = 0, itemid = 0;
		for(int k = 0; k < PLAYER_COUNT; ++k){
			int msgLen = sizeof(protocol_t);
			if(m_players[k] == m_winner){
				state = 1;

				uint8_t buff[128] = {0};
				int j = 0;
				uint32_t del_cnt = 0;
				ant::pack_h( buff, del_cnt, j );
				uint32_t add_cnt = 1;
				ant::pack_h( buff, add_cnt, j );
				uint32_t reason = 202;
				ant::pack_h( buff, reason, j );
				uint32_t reason_ex = 0;
				ant::pack_h( buff, reason_ex, j );

				uint32_t attire_type = 99;
				ant::pack_h( buff, attire_type, j );
				itemid =  1623000;
				ant::pack_h( buff, itemid, j );
				uint32_t count = 1;
				ant::pack_h( buff, count, j );
				uint32_t max_count = 0xFFFFFFFF;
				ant::pack_h( buff, max_count, j );

				send_request_to_db(db_proto_modify_items, NULL, j, buff, m_players[k]->id);	
			}
			uint32_t pairs = 2;
			ant::pack( pkg, pairs, msgLen );
			ant::pack( pkg, state, msgLen );
			ant::pack( pkg, itemid, msgLen );
			DEBUG_LOG("-----userid: %u, state: %u, itemid: %u ----", m_players[k]->id, state, itemid);
			init_proto_head( pkg, proto_adventure_game_over, msgLen );
			send_to_self( m_players[k], pkg, msgLen, 1);
			state = 0;
		}
		
	}

	m_grp = 0;
    return GER_end_of_game; 
}

