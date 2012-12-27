/**
 * =====================================================================================
 *       @file  paopao.cpp
 *      @brief  抛抛球游戏源文件
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  02/16/2009 10:34:45 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  sylar (), sylar@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include "paopao.hpp"
#include <libtaomee++/random/random.hpp>
#include <ant/inet/pdumanip.hpp>
extern "C" {
#include <gameserv/proto.h>
#include <gameserv/timer.h>
}

Paopao::Paopao(game_group_t *grp): bout_status(0), bout_cnt(0),owner(0),winner(0),is_game_over(false)
{
	m_grp = grp;
}
void
Paopao::init(sprite_t* p)
{

}

int
Paopao::handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code)
{
	return 0;
}

int
Paopao::handle_timeout(void* data)
{
	return 0;
}

int
Paopao::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	if(p->pet_id == 0) {
		DEBUG_LOG("%lu\t%d does not have lamn", m_grp->id, p->id);
		return -1;
	}
	switch(cmd)
	{
		case proto_player_leave:
			{
				if(!is_game_over) {
					game_score_t score;
					memset(&score, 0, sizeof(score));
					score.strong = -2;
					submit_game_score(p, &score);
					DEBUG_LOG("%lu\t%d leave game, minus str", m_grp->id, p->id);

				}
				return GER_player_offline;
			}
		case game_start:
			bout_status ++;
	//		DEBUG_LOG("%d client ready", p->id);
			if (is_bout_ready() && !GAME_STARTED(m_grp)) {
//				DEBUG_LOG("%lu paopaoball start", m_grp->id);
				SET_GAME_START(m_grp);
				for (int i = 0; i < m_grp->count; i++) {
					ADD_TIMER_EVENT(m_grp->players[i], on_timer_expire, 0, now.tv_sec + 120);
				}
				notify_serv_ready();
			}
			break;
		case item_choose:
			{
				if(!GAME_STARTED(m_grp)) {
					DEBUG_LOG("%lu %d game not started %d", m_grp->id, p->id, cmd);
					return GER_end_of_game;
				}
	//			DEBUG_LOG("%lu %d item_choose len %d", m_grp->id, p->id, len);
				uint8_t data[4096];
				for(int i = 0; i < len; i++) {
					data[i] = body[i];
				}
				do_data_transfer(p, data, len, item_choose);
				return 0;
			}
		case data_transfer:
			{
				//DEBUG_LOG("%lu %d 's turn", m_grp->id, m_grp->players[owner]->id);
				if(!GAME_STARTED(m_grp)) {
					DEBUG_LOG("%lu %d game not started %d", m_grp->id, p->id, cmd);
					return GER_end_of_game;
				}
				if(p->id != m_grp->players[owner]->id) {
					DEBUG_LOG("%lu %d not your turn, should %d", m_grp->id, p->id, m_grp->players[owner]->id);
					return GER_end_of_game;
				}
				owner = (owner + 1)%2;
				bout_status ++;
				uint8_t data[4096];
				for(int i = 0; i < len; i++) {
					data[i] = body[i];
				}
				do_data_transfer(p, data, len, data_transfer);
				break;
			}
		case game_over:
			if(!GAME_STARTED(m_grp) ) {
				DEBUG_LOG("%lu %d game not started %d,data len %d", m_grp->id, p->id, cmd, len);
				return GER_end_of_game;
			} else {
				is_game_over = true;
			}

			if(bout_cnt < 3) {
				DEBUG_LOG("%lu bout_cnt error %d", m_grp->id, bout_cnt);
				return GER_end_of_game;
			}

			//clean status
			bout_status = 0;
			//DEBUG_LOG("%d game over, len %d", p->id, len);
			if (!winner){
				winner = ntohl(*(uint32_t*)body);
				return 0;
			} else {
				uint32_t winner_check = ntohl(*(uint32_t*)body);
				if (winner != winner_check) {
					DEBUG_LOG("%lu error winner %d %d", m_grp->id, winner, winner_check);
					return GER_end_of_game;
				} else {
					DEBUG_LOG("%lu winner is %d, round_cnt %d", m_grp->id, winner, bout_cnt);
				}
			}

			if (winner != m_grp->players[0]->id
					&& winner != m_grp->players[1]->id) {
				DEBUG_LOG("%lu winner not exist %d", m_grp->id, winner);
				return GER_end_of_game;
			}
			for(int i = 0; i < m_grp->count; i++) {
				if(winner == m_grp->players[i]->id) {
				//	DEBUG_LOG("%d win", m_grp->players[i]->id);
					game_score_t score;
					memset(&score, 0, sizeof(score));
					//decide score;
					score.exp= m_grp->game->exp;
					score.score = taomee::ranged_random(5000, 6000);
					score.coins= 150;
					score.strong= 5;
					if(HAS_SUPER_LAMN(m_grp->players[i]) && rand()) {
						//DEBUG_LOG("%d has super lamn", m_grp->players[i]->id);
						score.itmid = 1200005;
						score.itmkind = 11;
						score.itm_max = 1;
						score.pet_id = m_grp->players[i]->pet_id;
					}
					//score.itmid = get_fire_medal(m_grp->players[i], 0);
					submit_game_score(m_grp->players[i], &score);
				} else {
				//	DEBUG_LOG("%d lose", m_grp->players[i]->id);
					game_score_t score;
					memset(&score, 0, sizeof(score));
					//decide score;
					score.score = taomee::ranged_random(3000, 4000);
					score.exp= 5;
					score.coins= 50;
					score.strong= 2;
					//score.itmid = get_fire_medal(m_grp->players[i], 1);
					submit_game_score(m_grp->players[i], &score);
				}
			}
			return GER_end_of_game;
		default:
			DEBUG_LOG("%lu %d undef cmd %d", m_grp->id, p->id, cmd);
			return GER_end_of_game;
	}

	if (is_bout_ready()) {
		bout_status = 0;
		notify_wind_direction();
		if(GAME_STARTED(m_grp)) {
			for (int i = 0; i < m_grp->count; i++) {
				MOD_EVENT_EXPIRE_TIME(m_grp->players[i], on_timer_expire, now.tv_sec + 120);
			}
		}
		bout_cnt ++;
	}
	return 0;
}

int
Paopao::do_data_transfer(sprite_t *p, const uint8_t body[], int body_len, int proto_num)
{
	for (int i = 0; i < m_grp->count; i++) {
		if (m_grp->players[i] != p) {
			int len = sizeof(protocol_t);
			for(int k = 0; k < body_len; k ++) {
				ant::pack(pkg, body[k], len);
			}
			init_proto_head(pkg, proto_num, len);
			send_to_self(m_grp->players[i], pkg, len, 1);
		}
	}
	return 0;
}
int
Paopao::notify_wind_direction()
{
	int num = taomee::ranged_random(1, 10);
	int len = sizeof (protocol_t);
	ant::pack(pkg, num, len);
	init_proto_head(pkg, wind_direction, len);
	for (int i = 0; i < m_grp->count; i++) {
		send_to_self(m_grp->players[i], pkg, len, 1);
	}
	return 0;
}

int
Paopao::notify_serv_ready()
{
	int len = sizeof (protocol_t);
	//who play first
	ant::pack(pkg, m_grp->players[0]->id, len);
	init_proto_head(pkg, serv_ready, len);
	for (int i = 0; i < m_grp->count; i++) {
		send_to_self(m_grp->players[i], pkg, len, 1);
	}
	return 0;
}
bool
Paopao::is_bout_ready()
{
	if(bout_status == 2) {
		return true;
	} else {
		return false;
	}
}
