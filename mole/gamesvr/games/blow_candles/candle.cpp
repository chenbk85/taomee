/*
 * =====================================================================================
 *
 *       Filename:  candle.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/26/2012 02:03:47 PM
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
#include "candle.h"
#include <cmath>

#define CANDLE_WAIT_LOAD_RESOURCE_TIME  35 
#define CANDLE_WAIT_BLOW_ACTION_TIME    35

static coord_candle_t candles_arrage[] = {
	{0,  4, WHILTE_COLORE},
	{-1, 3, WHILTE_COLORE},
	{1, 3,  WHILTE_COLORE},
	{-2, 2, WHILTE_COLORE},
	{0, 2,  WHILTE_COLORE},
	{2, 2,  WHILTE_COLORE},
	{-3, 1, WHILTE_COLORE},
	{-1, 1, WHILTE_COLORE},
	{1,  1, WHILTE_COLORE},
	{3,  1, WHILTE_COLORE},
	{-4, 0, WHILTE_COLORE},
	{-2, 0, WHILTE_COLORE},
	{0,  0, WHILTE_COLORE},
	{2,  0, WHILTE_COLORE},
	{4,  0, WHILTE_COLORE}
};

candle::candle( game_group_t* grp)
{
	INIT_LIST_HEAD( &m_timer );
	m_grp = grp;
	m_round_cnt = FIRST_GAME;
	m_on_offensive = -1;

    m_map = new coord_candle_t[sizeof(candles_arrage)/sizeof(coord_candle_t)];
  
 
}

candle::~candle()
{
	end_timer(true);
	delete m_map;
	m_map = 0;
	m_round_cnt = GAME_END;
}

void candle::init( sprite_t* p )
{

	memset(m_map, 0, sizeof(candles_arrage));
	for(int i = 0; i < m_grp->count; ++i){ // grape team
		//if( m_grp->players[i]->pos_id == 1 ){
			//m_players[0].m_player = m_grp->players[i];
			//m_players[0].m_sport_team = 1;
		//}
		//else{ // nono team
			//m_players[1].m_player = m_grp->players[i];
			//m_players[1].m_sport_team = 2;
		//}
	    m_players[i].m_player = m_grp->players[i];
		m_players[i].m_sport_team = 0;
		m_players[i].xiaomee  = 0;
		m_players[i].badge = 0;   
		m_players[i].mvp_team = 0;
		m_players[i].m_win_count = 0;
		m_players[i].db_step = 2;
		//p->waitcmd = db_proto_get_sport_mvp_team;
		send_request_to_db( db_proto_get_sport_mvp_team, m_players[i].m_player, 0, NULL, m_players[i].m_player->id );

	}
	m_db_ret = m_grp->count;
#ifndef TW_VER
	//blow candle game 
	uint32_t msgbuff[2]= { p->id, 1 };
	msglog( statistic_file, 0x0409C35B, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff) );
#endif
	DEBUG_LOG("blow candle game init end");
}

void candle::notify_client_game_info(){
	
	DEBUG_LOG( "notify client Game Info" );
	start_timer(TE_RESOURCE_LOAD, CANDLE_WAIT_LOAD_RESOURCE_TIME);
	m_game_state = GM_RS_LOAD;
	m_client_cnt = 2;

	int msgLen = sizeof(protocol_t);
	ant::pack( pkg, m_players[0].m_player->id, msgLen );
	ant::pack( pkg, m_players[1].m_player->id, msgLen );

	init_proto_head( pkg, proto_bcg_game_notify_info, msgLen );
	send_to_players( m_grp, pkg, msgLen );
}

int candle::handle_data( sprite_t* p, int cmd, const uint8_t body[], int len )
{
	player_t *pl = get_player(p);
	if(pl == 0){
		ERROR_LOG( "SYSTEM ERR Invalid Sprite ID:[%u]", p->id );
		return GER_game_system_err;
	}

	switch(cmd){
		case proto_player_leave: //player leave from game
		{
			//DEBUG_LOG("------------proto_player_leave");
			notify_game_over(pl, true);
			break;
		 }
		case proto_bcg_game_load_res: //load resource
	    {
			if( m_game_state == GM_RS_LOAD){
				--m_client_cnt;
				if(m_client_cnt == 0){
					end_timer(true);
					notify_game_start();
				}
			}//if
			break;

		}
		case proto_bcg_game_blow:// player blow candle action
		{
			if(m_game_state == GM_UNDER_WAY){
				CHECK_BODY_LEN_GE(len, 4);
				int j = 0;
				int32_t count = 0;
				ant::unpack(body, count, j);
				//DEBUG_LOG("count====: %u", count);
				CHECK_BODY_LEN(len, 8*count + 4);
				end_timer(true);
				if(count == 0){
					m_round_cnt = next_game_round(m_round_cnt);
					//DEBUG_LOG("&&&&&&&now m_round_cnt: %d", m_round_cnt);
					if(m_round_cnt == GAME_END){
						return notify_game_over(&m_players[(current_operator+1)%2]);

					}
					else{
						return notify_round_over();
					}
				}
				else{
					blow_candles(pl, body, count);
				}

			}
			break;
		}

	}//switch
	return 0;
}


int candle::handle_timeout( void* data )
{
	end_timer(true);
	switch(m_timer_event)
	{
	case TE_PLAYER_BLOW:
		{
		   //notify_game_over(&m_players[(current_operator+1)%2]);
			m_round_cnt = next_game_round(m_round_cnt);
			//DEBUG_LOG("!!!!!!!!!!now m_round_cnt: %d", m_round_cnt);
			if(m_round_cnt == GAME_END){
				return notify_game_over(&m_players[(current_operator+1)%2]);

			}
			else{
				return notify_round_over();
			}
		}
		break;
	case TE_RESOURCE_LOAD:
		{
			notify_game_start();
		}
		break;
	default:
		break;
	}
	return 0;
}

int candle::handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code )
{
	player_t *pl = get_player(p);
	switch(pl->db_step)
	{
	case 2:
		{
			--pl->db_step;
			uint32_t mvp_team = *(uint32_t*)buf;
			pl->mvp_team = mvp_team;
			send_request_to_db( db_proto_get_fire_cup_team, p, 0, NULL, p->id );
			break;
		}
	case 1:
		{
			--pl->db_step;
			uint32_t sport_team = *(uint32_t*)buf;
			pl->m_sport_team = sport_team;
			--m_db_ret;
			//if(sport_team == 0){
				//return GER_game_system_err;
			//}
			if(m_db_ret == 0){
				notify_client_game_info();
			}
			break;
		}
	}
	return 0;
}

int candle::check_range_candle_legal(coordinate_t *points, uint8_t count)
{
	uint8_t len = sizeof(candles_arrage)/sizeof(coord_candle_t);
	//check whether succession,only all the points are on the line, which satisfy the condition
	for(uint8_t i = 1; i < count; ++i){
		if(abs(points[i].x - points[i-1].x) != 2 || points[i].y != points[i-1].y){
			return -1;
		}
	}
	//check whether someone extinguish
	for(uint8_t i = 0; i < count; ++i){
		coord_candle_t other = {points[i].x, points[i].y, NOTHING};
		coord_candle_t *iter = std::find(m_map, m_map+len, other);
		if(iter == m_map+len ){
		   return -1;
		}
		else{
			if(iter->state == NOTHING){
				return -1;
			}	
		}
	}

	return 0;
}

int candle::check_blow_candle_over(uint8_t len)
{
	
	//for(uint8_t i = 0; i < len; ++i){
		//DEBUG_LOG("check is over i:[%u], x: [%d], y: [%d], state:[%d]", i, m_map[i].x, m_map[i].y, m_map[i].state);
	//}
	for(uint8_t k = 0; k < len; ++k){
		if((m_map+k)->state != NOTHING){
			return -1;
		}
	}
	return 0;
}

int candle::blow_candles(player_t *p, const uint8_t *body, uint32_t count)
{
	DEBUG_LOG("blow candle game now blowing candles");
    coordinate_t points[5];
	memset(points, 0, sizeof(points));
	const uint8_t* head = body + sizeof(count);
	//DEBUG_LOG("count: [%u]", count);
	int j = 0;
	for(uint8_t k = 0; k < count; ++k){
		ant::unpack(head, points[k].x, j);
		ant::unpack(head, points[k].y, j);
		//DEBUG_LOG("k:[%u], x: [%d], y: [%d]", k, points[k].x, points[k].y);

	}
	int ret = check_range_candle_legal(points, count);
	//DEBUG_LOG("====ret: %u", ret);
	if(ret != 0){
		return -1;
	}
	uint8_t len = sizeof(candles_arrage)/sizeof(coord_candle_t);
	for(uint8_t i = 0; i < count; ++i){
		coord_candle_t other = {points[i].x, points[i].y, NOTHING};
		coord_candle_t *iter = std::find(m_map, m_map+len, other);
		if(iter != m_map + len){
			switch(iter->state)
			{
			case WHILTE_COLORE:
				{
					p->xiaomee += 2*10;
					iter->state = NOTHING;
					break;
				}
			case YELLOW_COLORE:
					{
						p->xiaomee += 10*5;
						iter->state = NOTHING;
						break;
					}
			case RED_COLORE:
				{
					p->xiaomee += 50*2;
					iter->state = NOTHING;
					break;
				}
			case BRIGHT_COLORE:
				{
					iter->state = WHILTE_COLORE;
					break;
				}
			default:
				break;
			}//switch
		}//if
	}

	int is_blow_over = check_blow_candle_over(len);
	//DEBUG_LOG("is_blow_over: %u", is_blow_over);
	if(is_blow_over == 0){
		//int next_round =  m_round_cnt + 1;//
		m_round_cnt = next_game_round(m_round_cnt);
		//DEBUG_LOG("now m_round_cnt: %d", m_round_cnt);
		if(m_round_cnt == GAME_END){
			//player_t *pl = p;
			//if(p->m_player->id == m_players[0].m_player->id){
				//pl = &m_players[1];
			//}
			return notify_game_over(p);
			
		}
		else{
			return notify_round_over();
		}
	}
	current_operator = (current_operator+1)%2;

	int l = sizeof(protocol_t);
	ant::pack( pkg, m_players[current_operator].m_player->id, l );
	ant::pack( pkg, 15, l );
	for(uint8_t i = 0; i < len; ++i){
		ant::pack( pkg, m_map[i].x, l );
		ant::pack( pkg, m_map[i].y, l );
		ant::pack( pkg, (int)m_map[i].state, l );
		//DEBUG_LOG(" blowing now ---x: %d, y: %d, state: %d",m_map[i].x, m_map[i].y, (int)m_map[i].state);
	}
	init_proto_head( pkg, proto_bcg_game_blow, l );
	send_to_players( m_grp, pkg, l );
	start_timer(TE_PLAYER_BLOW, CANDLE_WAIT_BLOW_ACTION_TIME);

	return 0;
}

void candle::random_n_number(uint8_t count, uint8_t *array)
{

	uint8_t arr[sizeof(candles_arrage)/sizeof(coord_candle_t)] = {0};
	for(uint8_t k = 0; k < sizeof(candles_arrage)/sizeof(coord_candle_t); ++k){
		arr[k] = k;	
	}
	std::random_shuffle(arr, arr+sizeof(candles_arrage)/sizeof(coord_candle_t));
	for(uint8_t k = 0; k < count; ++k){
		*(array+k) = arr[k];
	}
	return ;
}

int candle::notify_game_start()
{
	memset(m_map, 0, sizeof(candles_arrage));
	memcpy(m_map, (char*)candles_arrage, sizeof(candles_arrage));
	uint8_t len = sizeof(candles_arrage)/sizeof(coord_candle_t);
	switch(m_round_cnt)
	{
	case FIRST_GAME:
		{
			for(uint8_t i = 0; i < len; ++i){
				if((m_map+i)->y == 0){
					(m_map+i)->state = NOTHING;
				}
			}
			break;
		}
	case SECOND_GAME:
		{
			uint8_t random = rand()%len;
			m_map[random].state = YELLOW_COLORE;
			break;
		}
	case THIRD_GAME:
		{
			uint8_t array[2] = {0};
			random_n_number(2, array);
			m_map[array[0]].state = RED_COLORE;
			m_map[array[1]].state = NOTHING;
			break;
		}
	case FOURTH_GAME:
		{
			uint8_t array[3] = {0};
			random_n_number(3, array);
			m_map[array[0]].state = YELLOW_COLORE;
			m_map[array[1]].state = RED_COLORE;
			m_map[array[2]].state = BRIGHT_COLORE;
			break;
		}
	case FIIFTH_GAME:
		{
			uint8_t array[4] = {0};
			random_n_number(4, array);
			m_map[array[0]].state = YELLOW_COLORE;
			m_map[array[1]].state = RED_COLORE;
			m_map[array[2]].state = BRIGHT_COLORE;
			m_map[array[3]].state = NOTHING;
			break;
		}
	default:
		break;
	}
	/*
	for(uint8_t m = 0; m < len; ++m){
		m_map[m].state = BRIGHT_COLORE;
	}
	*/
	if(m_on_offensive == -1){
		m_on_offensive = rand()% 2;
	}
	else{
		m_on_offensive = (m_on_offensive+1)%2;
	}
	current_operator = m_on_offensive;
	
	int j = sizeof(protocol_t);
	ant::pack( pkg, (int)m_round_cnt, j );
	ant::pack( pkg, m_players[m_on_offensive].m_player->id, j );
	ant::pack( pkg, m_players[0].m_player->id, j );
	ant::pack( pkg, m_players[0].m_sport_team, j );
	ant::pack( pkg, m_players[1].m_player->id, j );
	ant::pack( pkg, m_players[1].m_sport_team, j );
	//DEBUG_LOG("len: [%u], player_1: [%u], team1: [%u], player_2: [%u], team2: [%u], round_cnt: %d",
		//len, m_players[0].m_player->id, m_players[0].m_sport_team, m_players[1].m_player->id, m_players[1].m_sport_team,
		//(int)m_round_cnt);
	
	for(uint8_t k = 0; k < len; ++k){
		ant::pack(pkg, m_map[k].x, j);
		ant::pack(pkg, m_map[k].y, j);
		ant::pack(pkg, (int)m_map[k].state, j);
		DEBUG_LOG(" game start---x: %d, y: %d, state: %d",m_map[k].x, m_map[k].y, (int)m_map[k].state);
	}
	
	init_proto_head( pkg, proto_bcg_game_start, j );
	send_to_players( m_grp, pkg, j );
	start_timer(TE_PLAYER_BLOW, CANDLE_WAIT_BLOW_ACTION_TIME);
	m_game_state = GM_UNDER_WAY;
	//m_on_offensive = (m_on_offensive+1)%2;

	return 0;
}

int candle::notify_game_over(player_t* winner, bool escape)
{
	end_timer(true);
	DEBUG_LOG("blow candle game over now !");
	m_game_state = GM_OVER;
	int l = sizeof(protocol_t);
	if(escape){
		//DEBUG_LOG("blow candle game over now ============!");
		for(uint8_t i = 0; i < m_grp->count; ++i){
			ant::pack(pkg, m_players[i].m_player->id, l);
			if(winner->m_player->id == m_players[i].m_player->id){
				ant::pack(pkg, 0, l);
				m_players[i].badge = 0;
				m_players[i].m_win_count = 0;
				if(m_players[i].xiaomee == 0){
					m_players[i].xiaomee = 10;
				}
			}
			else{
				ant::pack(pkg, 1, l);
				m_players[i].badge = 1;
				m_players[i].xiaomee += 20;
				++m_players[i].m_win_count;
			}
			ant::pack( pkg, m_players[i].m_win_count, l );
			ant::pack( pkg, m_players[i].xiaomee, l );
			ant::pack(pkg, 0, l);
		}
	}
	else{
		for(uint8_t i = 0; i < m_grp->count; ++i){
			ant::pack( pkg, m_players[i].m_player->id, l );
			if(m_players[i].m_player->id == winner->m_player->id){

				ant::pack( pkg, 0, l);// loser 0

			}
			else{
				++m_players[i].m_win_count;

				ant::pack( pkg, 1, l);//winner 1

			}	
			if(m_players[0].m_win_count > m_players[1].m_win_count){
				m_players[0].xiaomee += 100;
				m_players[0].badge = 5;
				m_players[1].badge = 1;
			}
			else{
				m_players[1].xiaomee += 100;
				m_players[1].badge = 5;
				m_players[0].badge = 1;
			}
			ant::pack( pkg, m_players[i].m_win_count, l );
			ant::pack( pkg, m_players[i].xiaomee, l );
			ant::pack(pkg, 0, l);
		}
	}


	init_proto_head( pkg, proto_bcg_game_over, l );
	send_to_players( m_grp, pkg, l );

	for(uint8_t k = 0; k < m_grp->count; ++k){
		
		//set DB USER data
		//uint32_t db_buf[] = {m_players[k].mvp_team, m_players[k].badge};
		//send_request_to_db( db_proto_set_sport_team_badge, NULL, sizeof(db_buf), &db_buf, m_players[k].m_player->id );
		//update global DB data 
		//uint32_t db_buf_2[] = {m_players[k].m_sport_team, m_players[k].mvp_team, m_players[k].badge};
		//send_request_to_db( db_proto_set_sysarg_sport_badge,NULL, sizeof(db_buf_2), &db_buf_2, m_players[k].m_player->id );

		uint8_t buff[1024] = {};
		int j = 0;
		ant::pack_h( buff, (int)0, j );
		ant::pack_h( buff, (int)1, j );
		ant::pack_h( buff, (int)0, j );
		ant::pack_h( buff, (int)0, j );

		ant::pack_h( buff, (int)0, j );
		ant::pack_h( buff, (int)0, j );
		ant::pack_h( buff, m_players[k].xiaomee, j );
		ant::pack_h( buff, (int)99999999999, j );

		//ant::pack_h( buff, (int)99, j );
		//ant::pack_h( buff, (int)1351317, j );
		//ant::pack_h( buff, m_players[k].badge, j );
		//ant::pack_h( buff, (int)99999999999, j );
		send_request_to_db(db_proto_modify_items, NULL, j, buff, m_players[k].m_player->id);
	}

	return GER_end_of_game;
}

int candle::notify_round_over()
{
	DEBUG_LOG("the blow candle round over!");
	int l = sizeof(protocol_t);
	for(uint8_t i = 0; i < m_grp->count; ++i){
		ant::pack( pkg, m_players[i].m_player->id, l );
		if(i == current_operator){
			ant::pack( pkg, 0, l );//lose
		
		}
		else{
			//DEBUG_LOG("m_sport_team: [%u]",m_players[i].m_sport_team); 
			++m_players[i].m_win_count;
			ant::pack(pkg, 1, l);//win
			m_players[i].xiaomee += 50;
		}	
		ant::pack( pkg, m_players[i].m_win_count, l );
	}

	init_proto_head( pkg, proto_bcg_game_round_over, l );
	send_to_players( m_grp, pkg, l );
	
	//current_operator = m_on_offensive;
	m_client_cnt = 2;
	m_game_state = GM_RS_LOAD;

	return 0;
}

game_round_t candle::next_game_round(game_round_t var_round)
{
	game_round_t round[] = {FIRST_GAME, SECOND_GAME, THIRD_GAME, FOURTH_GAME, FIIFTH_GAME, GAME_END};
	for(uint8_t k = 0; k < 6; ++k){
		if(var_round == round[k]){
			return round[k+1];
		}
	}
	return GAME_END;
}

void candle::start_timer(timer_event_t evt, uint32_t time)
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

bool candle::end_timer( bool force )
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
