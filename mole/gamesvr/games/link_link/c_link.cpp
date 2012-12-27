/*
 * =====================================================================================
 *
 *       Filename:  c_link.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/08/2012 10:02:09 AM
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
#include "c_link.h"
#include "l_link.h"

#define LINKING_GAME_LOAD_RESOURE_TIME   5*60

c_link::c_link(game_group_t* grp, sprite_t* player1, sprite_t* player2)
{
	m_grp = grp;
	m_players[0] = player1;
	m_players[1] = player2;
	m_winner = 0;
	DEBUG_LOG("===========player_id: %u, player_2: %u========", player1->id, player2->id);
	INIT_LIST_HEAD( &m_timer );

	m_player_link[0] = new l_link();
	m_player_link[1] = new l_link(*m_player_link[0]);
	score[0] = 0;
	score[1] = 0;
	notify_game_start();
}

c_link::~c_link()
{

	for(uint32_t k = 0; k< PLAYER_COUNT; ++k){
		m_players[k] = NULL;
		delete m_player_link[k];
	}
	m_winner = 0;
}

void c_link::init( sprite_t* p )
{
;
}

int c_link::handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code )
{
	return 0;
}

int c_link::handle_timeout( void* data )
{
	
	if(m_timer_event == TE_PLAYER_DOING_LINK){
		return notify_game_over();
	}
	end_timer(true);
	return -1;
}



int	c_link::handle_data( sprite_t* p, int cmd, const uint8_t body[], int len )
{

	if(p == 0){
		ERROR_LOG( "SYSTEM ERR Invalid Sprite ID:[%u]", p->id );
		return GER_game_system_err;
	}
	switch(cmd){

		case proto_player_leave:
			{
				DEBUG_LOG("-------------game over operating now ---------");
				if(m_grp != 0){
					notify_game_over(p, true);
				}
				break;
			}
		case proto_linking_game_operate:
			{
				DEBUG_LOG("-------------operating now ---------");
				do_linking_game(p, body, len);
				break;
			}
	}
	return 0;
}

int c_link::notify_game_start()
{
	DEBUG_LOG( "notify client start linking Game " );

	start_timer(TE_PLAYER_DOING_LINK, LINKING_GAME_LOAD_RESOURE_TIME);
	m_linking_state = GAME_PALYING  ;
	
	//for(uint32_t k = 0; k < PLAYER_COUNT; ++k){

		int msgLen = sizeof(protocol_t);
		ant::pack( pkg, m_players[0]->id, msgLen );
		ant::pack( pkg, m_players[1]->id, msgLen );

		uint32_t count= ROWS * COLS;
		uint32_t rows = ROWS;
		uint32_t cols = COLS;
		ant::pack( pkg, rows, msgLen );
		ant::pack( pkg, cols, msgLen );
		ant::pack( pkg, count, msgLen );
		
		for(uint8_t i = 0; i < ROWS; ++i){
			for(uint8_t j = 0; j <= COLS; ++j){
				ant::pack( pkg, i, msgLen );
				ant::pack( pkg, j, msgLen );
				ant::pack( pkg, m_player_link[0]->map[i][j], msgLen );
				//DEBUG_LOG("---x: %u, y: %u, z: %u ---", i, j, m_player_link[0]->map[i][j]);
			}
		}
		init_proto_head( pkg, proto_linking_game_start, msgLen );
		//send_to_self( m_players[k], pkg, msgLen, 1);
		DEBUG_LOG("player1: [%u], player2: [%u]", m_grp->players[0]->id, m_grp->players[1]->id);
		send_to_players( m_grp, pkg, msgLen );

	//}

	return 0;
}

int c_link::notify_game_over(sprite_t *p, bool flag)
{
	DEBUG_LOG( "notify client over linking Game " );
	end_timer(true);
	m_winner = 0;
	if(flag){
		if(m_players[0] == p){
			m_winner = m_players[1];
		}
		else{
			m_winner = m_players[0];
		}
	}
	else{
		DEBUG_LOG("--- score1: %u, score2: %u ----",score[0], score[1]);
		if(score[0] > score[1]){
			m_winner = m_players[0];
		}
		else if(score[0] < score[1]){
			m_winner = m_players[1];
		}
		else{
			m_winner = 0;
		}
	}

	for(uint32_t k = 0; k < PLAYER_COUNT; ++k){
		int msgLen = sizeof(protocol_t);
		uint32_t win_state = 0;
		uint32_t itemid = 0;
		if(m_players[k] == m_winner){
			win_state = 1;

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
			itemid = 1351355;
			ant::pack_h( buff, itemid, j );
			uint32_t count = 1;
			ant::pack_h( buff, count, j );
			uint32_t max_count = 0xFFFFFFFF;
			ant::pack_h( buff, max_count, j );

			send_request_to_db(db_proto_modify_items, NULL, j, buff, m_players[k]->id);
		}
		ant::pack( pkg, win_state, msgLen );
		ant::pack( pkg, itemid, msgLen );
		DEBUG_LOG("-----userid: %u, win_state: %u, itemid: %u ----", m_players[k]->id, win_state, itemid);
		init_proto_head( pkg, proto_linking_game_over, msgLen );
		send_to_self( m_players[k], pkg, msgLen, 1);
		//send_to_players( m_grp, pkg, msgLen );
	}
	m_linking_state = GAME_OVER  ;
	m_grp = 0;
	return GER_end_of_game;

}

int c_link::do_linking_game(sprite_t* p, const uint8_t body[], int len )
{
	DEBUG_LOG( "notify client doing linking Game " );
	typedef struct{
		uint32_t x;
		uint32_t y;
	} m_point_t;

	CHECK_BODY_LEN(len, sizeof(m_point_t)*2);
		
	m_point_t point_client[2];
	int offset = 0;
	for(uint32_t k = 0; k < 2; ++k){
		ant::unpack(body, point_client[k].x, offset);
		ant::unpack(body, point_client[k].y, offset);
	}
	int ret = 0;
	uint32_t cur_player_pos = 0;
	for(uint32_t k = 0; k < 2; ++k){
		if(m_players[k] == p){
			cur_player_pos = k;
			point_t start(point_client[0].x, point_client[0].y);
			point_t end(point_client[1].x, point_client[1].y);
			ret = m_player_link[k]->find_path(start, end);
			break;
		}
	}


	if(ret != -1 ){
		score[cur_player_pos] += 100;
		if(ret != 2){
			notify_client_broadcast();
		}
	}
	int is_over_ret = ret;
	int l = sizeof(protocol_t);
	uint8_t n_msg[256] = {0};
	if(ret == -1){
		ret = 0; // for client need
	}
	else{
		ret = 1;
	}
	ant::pack( n_msg, ret, l );
	init_proto_head( n_msg, proto_linking_game_operate, l );
	send_to_self( p, n_msg, l, 1);

	if(is_over_ret == 2){
		return notify_game_over(p, false);
	}
	return 0;
}

int c_link::notify_client_broadcast()
{
	DEBUG_LOG( "notify client broadcast linking Game " );
	if(m_grp == 0){
		return 0;
	}

	int l = sizeof(protocol_t);
	for(uint32_t k  = 0; k < PLAYER_COUNT; ++k){
		ant::pack(pkg, m_players[k]->id, l);
		uint32_t remainder = m_player_link[k]->pair_count - m_player_link[k]->find_pairs;
		ant::pack(pkg, remainder, l);
		ant::pack(pkg, score[k], l);
	}

	init_proto_head( pkg, proto_linking_game_notify, l );
	send_to_players( m_grp, pkg, l );

	return 0 ;
}

void c_link::start_timer(timer_event_t evt, uint32_t time)
{
	if( !m_grp )
	{
		return;
	}
	m_timer_event = evt;
	int val;
	m_timer_id = add_timer_event(&m_timer, on_game_timer_expire, m_grp, &val, now.tv_sec + time );
	m_timer_counter = 1;
}

bool c_link::end_timer(bool force )
{
	if( m_timer_counter > 0 )
	{
		m_timer_counter--;
	}
	else
	{
		m_timer_counter = 0;
	}

	if( ( 0 == m_timer_counter || force ) && m_timer_id != 0 )
	{
		remove_timer( &m_timer, m_timer_id );
		m_timer_counter = 0;
		m_timer_id = 0;
	}

	if( 0 == m_timer_id )
	{
		return true;
	}
	else
	{
		return false;
	}
}
