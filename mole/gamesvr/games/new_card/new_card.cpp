#include <algorithm>
#include <libtaomee++/random/random.hpp>
extern "C" {
#include <gameserv/proto.h>
#include <gameserv/dbproxy.h>
#include <gameserv/timer.h>
#include <gameserv/game.h>

#ifndef TW_VER
#include  <statistic_agent/msglog.h>
#endif

}
#include <ant/inet/pdumanip.hpp>
#include "new_card.hpp"

#ifdef TW_VER
#define TWO_HOUR_SEC    100000
#define FIVE_HOUR_SEC   200000
#else
#define TWO_HOUR_SEC    7200
#define FIVE_HOUR_SEC   18000
#endif

struct score_info score_info_4[] =
{
	{80, 80},
	{50, 50},
	{30, 30},
	{10, 10},
};

struct score_info score_info_3[] =
{
	{60, 60},
	{30, 30},
	{20, 20},
};

struct score_info score_info_2[] =
{
	{50, 50},
	{30, 30},
};

struct monster_score_info monster_bonus[24] =
{
	{0, 0, 0},
	{10, 20, 2},
	{15, 30, 6},
	{12, 24, 4},
	{20, 40, 8},
	{0, 0, 0},
	{30, 60, 12},
	{40, 80, 14},
	{90, 180, 18},
	{60, 120, 16},
	{0, 0, 0},
	{25, 50, 10},
	{120, 240, 20},
	{280, 560, 28},
	{390, 780, 30},
	{130, 260, 22},
	{150, 300, 24},
	{250, 500, 26},
	{400, 800, 32},
	{550, 1100, 34},
	{660, 1320, 36},
	{780, 1560, 38},
	{780, 1560, 38},
	{900, 1800, 40},
};


struct grid_info grid_info_st[16] =
{
	{war_t,		0},{fire_t, 	0},{water_t, 	0},{wood_t, 	0},
	{attr_t, 	0},{fire_t, 	0},{wood_t, 	0},{water_t, 	0},
	{war_t, 	0},{fire_t, 	0},{water_t, 	0},{wood_t, 	0},
	{attr_t, 	0},{fire_t, 	0},{wood_t, 	0},{water_t, 	0},
};



/**
 * @brief 卡牌类构造函数
 * @param  玩家所在用户组
 * @return 永远不会G返回
 */
Cnew_card::Cnew_card (game_group_t *grp):player_num(0),init_players_cnt(0),lost_cnt(0),owner_userid(0),owner_roll(0),m_grp(grp),grid_war(false),send_cnt(0),recv_cnt(0),challenge_win(-1),act_state(ROLL),is_get_item(false),send_item_cnt(0),recv_item_cnt(0),bout_cnt(0),monster_id(0),game_start(0),s_xp(0),m_xp(0),h_xp(0),l_xp(0),itemid(0),mole_exp(0),xiaomee(0),strong(0),mole_exp_ex(0),dragon_growth(0),dragon_growth_ex(0)
{

	grid_info_st[0].type = war_t;
	grid_info_st[0].num  = 0;
	grid_info_st[1].type = fire_t;
	grid_info_st[1].num  = 0;
	grid_info_st[2].type = water_t;
	grid_info_st[2].num  = 0;
	grid_info_st[3].type = wood_t;
	grid_info_st[3].num  = 0;
	grid_info_st[4].type = attr_t;
	grid_info_st[4].num  = 0;
	grid_info_st[5].type = fire_t;
	grid_info_st[5].num  = 0;
	grid_info_st[6].type = wood_t;
	grid_info_st[6].num  = 0;
	grid_info_st[7].type = water_t;
	grid_info_st[7].num  = 0;
	grid_info_st[8].type = war_t;
	grid_info_st[8].num  = 0;
	grid_info_st[9].type = fire_t;
	grid_info_st[9].num  = 0;
	grid_info_st[10].type = water_t;
	grid_info_st[10].num  = 0;
	grid_info_st[11].type = wood_t;
	grid_info_st[11].num  = 0;
	grid_info_st[12].type = attr_t;
	grid_info_st[12].num  = 0;
	grid_info_st[13].type = fire_t;
	grid_info_st[13].num  = 0;
	grid_info_st[14].type = wood_t;
	grid_info_st[14].num  = 0;
	grid_info_st[15].type = water_t;
	grid_info_st[15].num  = 0;


	if (CHALLENGE_GAME(m_grp->game))
	{
        player_num = single_player;
    }
	else
	{
        player_num = multi_players;
    }

}

int Cnew_card::result[CARD_TYPE_MAX][CARD_TYPE_MAX] =
{
	//水，	火， 木
	{DRAW,	  WIN,    LOST,},
	{LOST,    DRAW,   WIN,},
	{WIN,     LOST,   DRAW,},
};


int Cnew_card::init_player_grid[4] =
{
	12,4,8,0,
};

/**
 * @brief  处理玩家用户游戏通讯
 * @param
 * @return 返回GER_end_of_game结束游戏,返回0游戏继续
 */
int Cnew_card::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	Cplayer* p_player = NULL;
	p_player = get_player(p);
	if (p_player == NULL)
	{
		DEBUG_LOG("%lu player uid=%d aready leave , cmd:%u",m_grp->id, p->id, cmd);
		return 0;
	}

    switch (cmd)
    {
        case NEW_CARD_GET_CARDS:
            return send_player_cards(p);

		case NEW_CARD_CLIENT_READY:
			{
				if (CHALLENGE_GAME(m_grp->game))
				{
					CHECK_BODY_LEN(len, 4);
					int m_id = 0;
                	int i = 0;
                	ant::unpack(body, m_id, i);
					monster_id = m_id;

					if (monster_id > 23)
					{
						return GER_end_of_game;
					}

					int mole_level = p_player->get_mole_level();
					int high_card = p_player->get_high_card();
					set_challenge_result(p_player,monster_id, mole_level, high_card);
				}

            	if (!CHALLENGE_GAME(m_grp->game))
				{
                	ADD_TIMER_EVENT(p, on_timer_expire, 0, now.tv_sec + 180);
            	}

				p_player->set_game_status(GAME_READY);

				if (is_game_ready())
				{
					int num = get_game_ready_num();
					if ((num <= 1) && (!CHALLENGE_GAME(m_grp->game)))
					{
						return GER_end_of_game;
					}

					notify_client_ready();
					owner_userid = player[0]->id();
					game_start = 1;
				}

				break ;
			}

		case NEW_CARD_GAME_ROLL:
			{
				if (act_state != ROLL)
				{
					ERROR_LOG("%lu new_card:error act_state:%u \t uid=%d, cmd:%u",m_grp->id, act_state, p->id, cmd);
					break ;
				}

				MOD_EVENT_EXPIRE_TIME(p, on_timer_expire, now.tv_sec + 180);

				int point = p_player->roll();
				if (!point)
				{
					ERROR_LOG("%lu new_card:illegal roll\t uid=%d",m_grp->id, p->id);
					return GER_end_of_game;
				}

                notify_roll_point(p->id, point, player[(owner_roll + 1)%player.size()]->id());
				owner_roll = (owner_roll + 1)%player.size();
				owner_userid = p->id;

				set_act_state(MOVE_GRID);

				break ;
			}

		case NEW_CARD_MOVE_GRID:
			{

				if (act_state != MOVE_GRID)
				{
					ERROR_LOG("%lu new_card:error act_state:%u \t uid=%d, cmd:%u",m_grp->id, act_state, p->id, cmd);
					break ;
				}

				if (!is_my_turn(p))
				{
					ERROR_LOG("%lu new_card:error not move grid user:%u, cmd:%u",m_grp->id, p->id, cmd);
					break ;

				}

				MOD_EVENT_EXPIRE_TIME(p, on_timer_expire, now.tv_sec + 180);

				CHECK_BODY_LEN(len, 1);
                int8_t direct = 0;
                int i = 0;
                ant::unpack(body, direct, i);
				if (direct < 0 || direct > 1)
				{
					ERROR_LOG("%lu Cnew_card: %d error direct %d", m_grp->id, p->id, direct);
                    return GER_end_of_game;
				}

				int roll_point = p_player->get_roll_point();
				int grid_id = p_player->get_grid_id();

				int new_grid_id = move_grid_direct(grid_id, roll_point, direct);
				set_grid_num(grid_id, new_grid_id);
				p_player->set_grid_id(new_grid_id);

				set_bout_status(new_grid_id);

				notify_move_grid(p->id, new_grid_id);
				uint32_t userid = p_player->id();

				int num = grid_info_st[new_grid_id].num;

				if (num > 1)
				{
					set_act_state(ACT_CARD);

					grid_war_userid = userid;
					set_grid_other_userid(new_grid_id, userid);
					uint32_t war_userid = 0;
					if (grid_userids.size() > 0)
					{
						war_userid = grid_userids[0];
						grid_userids.erase(grid_userids.begin());
						notify_select_war_user(grid_war_userid, war_userid);
						set_bout_playerid_by_userid(grid_war_userid, war_userid);
					}

					if (grid_userids.size() > 0)
					{
						grid_war = true;
					}
					else
					{
						grid_war = false;
						grid_war_userid = 0;
						grid_userids.clear();
					}

				}
				else
				{
					set_bout_status(new_grid_id);
					set_bout_playerid(new_grid_id);

					set_act_state_by_grid(new_grid_id);
					set_owner_userid_by_grid(new_grid_id, userid);

				}

				break ;
			}

		case NEW_CARD_ATTR_SELECT:
			{
				if (act_state != ATTR_SELECT)
				{
					ERROR_LOG("%lu new_card:error act_state:%u \t uid=%d, cmd:%u",m_grp->id, act_state, p->id, cmd);
					break ;
				}

				if (!is_my_turn(p))
				{
					ERROR_LOG("%lu new_card:error not attr select user:%u, cmd:%u",m_grp->id, p->id, cmd);
					break ;

				}

				MOD_EVENT_EXPIRE_TIME(p, on_timer_expire, now.tv_sec + 180);

				CHECK_BODY_LEN(len, 1);
                int8_t type = 0;
                int i = 0;
                ant::unpack(body, type, i);
				if (type < 0 || type > 2)
				{
					ERROR_LOG("%lu Cnew_card: %d error type %d", m_grp->id, p->id, type);
                    return GER_end_of_game;
				}

				set_bout_status_by_attr(type);

				set_act_state(ACT_CARD);

				notify_act_card_type(p->id,type);
				break ;

			}

		case NEW_CARD_WAR_USER:
			{
				if (act_state != WAR_SELECT)
				{
					ERROR_LOG("%lu new_card:error act_state:%u \t uid=%d,cmd:%u",m_grp->id, act_state, p->id, cmd);
					break ;
				}

				if (!is_my_turn(p))
				{
					ERROR_LOG("%lu new_card:error not war select user:%u, cmd:%u",m_grp->id, p->id, cmd);
					break ;
				}

				MOD_EVENT_EXPIRE_TIME(p, on_timer_expire, now.tv_sec + 180);

				CHECK_BODY_LEN(len, 4);
                uint32_t war_userid = 0;
                int i = 0;
                ant::unpack(body, war_userid, i);

				Cplayer * war_player = NULL;
				war_player = get_player(war_userid);
				if (war_player == NULL)
				{
					ERROR_LOG("%lu Cnew_card: %d error war user %d", m_grp->id, p->id, war_userid);
                    return GER_end_of_game;
				}

				int num = war_player->get_num();
				int owner_num = p_player->get_num();
				bout_playerid.clear();
				bout_playerid.push_back(owner_num);
				bout_playerid.push_back(num);

				set_act_state(ACT_CARD);

				notify_select_war_user(p->id, war_userid);
				break ;

			}

		case NEW_CARD_ACT_CARD:
			{
				if (act_state != ACT_CARD)
				{
					ERROR_LOG("%lu new_card:error act_state:%u \t uid=%d",m_grp->id, act_state, p->id);
					break ;
				}

				if (!check_bout_userid(p->id))
				{
					ERROR_LOG("%lu new_card:error not bout  act card user:%d",m_grp->id, p->id);
					break ;
				}

				MOD_EVENT_EXPIRE_TIME(p, on_timer_expire, now.tv_sec + 180);

				CHECK_BODY_LEN(len, 1);
				int8_t pos = 0;
                int i = 0;
                ant::unpack(body, pos, i);
				if (pos < 0 || pos > 4)
				{
					ERROR_LOG("%lu Cnew_card: %d error pos %d", m_grp->id, p->id, pos);
                    return GER_end_of_game;
				}
				int ret = on_card_act(p, pos);
				if (ret != 0)
				{
            		return GER_end_of_game;
				}
				break ;
			}
		case NEW_CARD_DEC_ITEM:
			{
				CHECK_BODY_LEN(len, 4);
				itemid = 0;
                int i = 0;
                ant::unpack(body, itemid, i);
				on_dec_item(p, itemid);
				break ;
			}

		case NEW_CARD_MONSTER_OVER:
			{
				CHECK_BODY_LEN(len, 1);
				int8_t win = 0;
                int i = 0;
                ant::unpack(body, win, i);
				if (!win)
				{
					challenge_win = 0;
				}
				return on_monster_over(p_player);
			}

        case proto_player_leave:
        case NEW_CARD_PLAYER_QUIT:
			{
				return on_player_leave(p);
			}
        default:
            ERROR_LOG("%lu\tCard, undef cmd: %d", m_grp->id, cmd);
            return GER_end_of_game;
    }

	return 0;

}

Cplayer* Cnew_card::get_player(sprite_t* p)
{
	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			if (init_players[i]->get_sprite() == p)
			{
				return init_players[i];
			}
		}
	}
	return NULL;
}

Cplayer* Cnew_card::get_player(uint32_t userid)
{
	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			if (init_players[i]->id() == userid)
			{
				return init_players[i];
			}
		}

	}
	return NULL;
}


int Cnew_card::on_card_act(sprite_t* p, int pos)
{
	Cplayer* p_player = NULL;
	p_player = get_player(p);
	if (p_player == NULL)
	{
		return 0;
	}

	p_player->act_bout_card(pos);
	p_player->set_status(turn_start);
	notify_act_card(p->id, pos);

	if (is_bout_all_ready())
	{
		int ret = process_bout_result();
		if (ret != 0)
		{
			return ret ;
		}

		notify_bout_result();
		process_bout_players_lost();
		bout_clear();

		ret = process_game_result();
		if (ret != 0) return ret;
		if (grid_war)
		{
			uint32_t war_userid = 0;
			if (grid_userids.size() > 0)
			{
				set_act_state(ACT_CARD);

				war_userid = grid_userids[0];
				grid_userids.erase(grid_userids.begin());
				notify_select_war_user(grid_war_userid, war_userid);
				set_bout_playerid_by_userid(grid_war_userid, war_userid);
			}

			if (grid_userids.size() > 0)
			{
				grid_war = true;
			}
			else
			{
				grid_war = false;
				grid_war_userid = 0;
				grid_userids.clear();
			}

		}
		else
		{
			set_act_state(ROLL);
		}
	}

	return 0;

}

int Cnew_card::process_bout_result()
{
	if (bout_type != none_type)
	{
		int max_value = 0;
		std::vector<int> win_ids;
		for (uint32_t i = 0; i < bout_playerid.size(); i++)
		{
			int id = bout_playerid[i];
			int cardid =  init_players[id]->get_bout_cardid();
			card_t card_info = (*cards_info)[cardid];

			if (card_info.type != bout_type)
			{
				init_players[id]->update_life_dec(1);
				init_players[id]->set_bout_result(LOST);
			}
			else
			{
				if (card_info.value < max_value)
				{
					init_players[id]->update_life_dec(1);
					init_players[id]->set_bout_result(LOST);
				}
				else if (card_info.value == max_value)
				{
					win_ids.push_back(id);
				}
				else
				{
					max_value = card_info.value;
					for (uint32_t j = 0; j < win_ids.size(); j++)
					{
						int id_lost = win_ids[j];
						init_players[id_lost]->update_life_dec(1);
						init_players[id_lost]->set_bout_result(LOST);
					}
					win_ids.clear();
					win_ids.push_back(id);
				}
			}
		}

		for (uint32_t k = 0; k < win_ids.size(); k++)
		{
			int id_win = win_ids[k];
			init_players[id_win]->set_bout_result(WIN);

		}

	}
	else
	{
		if (bout_playerid.size() == 1)
		{
			int id = bout_playerid[0];
			init_players[id]->update_life_inc(1);
			init_players[id]->set_bout_result(WIN);

			return 0;
		}

		int id1 = bout_playerid[0];
		int id2 = bout_playerid[1];
		int cardid1 = init_players[id1]->get_bout_cardid();
		int cardid2 = init_players[id2]->get_bout_cardid();
		card_t card_info1 = (*cards_info)[cardid1];
		card_t card_info2 = (*cards_info)[cardid2];

		int result = compare(card_info1, card_info2);

		if (result == WIN)
		{
			init_players[id1]->update_life_inc(1);
			init_players[id1]->set_bout_result(WIN);
			init_players[id2]->update_life_dec(1);
			init_players[id2]->set_bout_result(LOST);
		}
		else if (result == DRAW)
		{
			init_players[id1]->set_bout_result(DRAW);
			init_players[id2]->set_bout_result(DRAW);
		}
		else
		{
			init_players[id1]->update_life_dec(1);
			init_players[id1]->set_bout_result(LOST);
			init_players[id2]->update_life_inc(1);
			init_players[id2]->set_bout_result(WIN);
		}

	}

	return 0;

}


int Cnew_card::process_game_result()
{
	int live_players = 0;
	Cplayer* p_player = NULL;
	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			if ( init_players[i]->get_life_value() > 0)
			{
				live_players++;
				p_player = init_players[i];
			}
		}
	}

	if (live_players <= 1)
	{
		if (live_players == 1)
		{
			int grade = init_players_cnt - lost_cnt;
			p_player->set_grade(grade);
		}

		update_db_players_info();

	}

	return 0;

}


/**
 * @brief  裁决双方卡牌胜负
 * @param  card_t p1 要裁决的卡牌1
 * @param  card_t p2 要裁决的卡牌2
 * @return 返回卡牌1的胜负
 */
int Cnew_card::compare(card_t p1, card_t p2) {
	int my_result = Cnew_card::result[p1.type][p2.type];
	if (my_result != DRAW)
	{
		return my_result;
	}
	else
	{
		if (p1.value == p2.value) {
			return DRAW;
		}
		if (p1.value > p2.value) {
			return WIN;
		}
		if (p1.value < p2.value) {
			return LOST;
		}
	}

	return 0;

}

int Cnew_card::set_all_card_info(card_t (*all_cards)[CARD_ID_MAX])
{
	if (all_cards != NULL) {
		cards_info = all_cards;
	}

	return 0;
}


/**
 * @brief 给准备好的玩家发送本次游戏的卡牌队列
 * @param
 * @return
 */
int Cnew_card::send_player_cards(sprite_t* p)
{
	Cplayer * p_player = NULL;
	int l = sizeof (protocol_t);
	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			if (init_players[i]->get_sprite() == p)
			{
				p_player = init_players[i];
				break ;
			}
		}
	}

	if (p_player == NULL)
	{
		return GER_end_of_game;
	}

	int size = p_player->get_card_size();
	ant::pack(pkg, p->id, l);

	int high_card = 0;

	uint8_t rank = p_player->get_rank();
	ant::pack(pkg, rank, l);
	ant::pack(pkg, (uint8_t)size, l);
	for (int j = 0; j< size; j++)
	{
		uint8_t id = p_player->get_card_id(j);
		ant::pack(pkg, id, l);

		if ((*cards_info)[id].value >= 6)
		{
			high_card++;
		}

	}

	p_player->set_high_card(high_card);

	init_proto_head(pkg, NEW_CARD_GET_CARDS, l);
	if (send_to_self(p, pkg, l, completed) != 0)
	{
		ERROR_LOG("Send cards array error");
		return 0;
	}


	DEBUG_LOG("userid:%u Send cards array leave ", p->id);

	return 0;

}


/**
 * @brief  每个回合结束之后重置玩家状态
 * @param
 * @return
 */
inline void Cnew_card::bout_clear()
{
	for (uint32_t i = 0 ; i < bout_playerid.size(); i++ )
	{
		int id = bout_playerid[i];
		init_players[id]->bout_clear() ;
	}

	if ((uint32_t)owner_roll < player.size())
	{
		owner_userid = player[owner_roll]->id();
	}
	else
	{
		owner_userid = 0;
	}

}

/**
 * @brief  判断是否都准备好了
 * @param
 * @return true 准备好了
 * @return false  没有准备好
 */
inline bool Cnew_card::is_bout_all_ready()
{
	if (bout_playerid.size() == 0)
	{
		return false;
	}

	for (uint32_t i = 0; i < bout_playerid.size(); i++)
	{
		int id = bout_playerid[i];
		if (init_players[id]->get_status() != turn_start)
		{
			return false;
		}
	}

	return true;

}


void Cnew_card::notify_roll_point(uint32_t userid, int point, uint32_t uid)
{
	int len = sizeof (protocol_t);
    ant::pack(pkg, userid, len);
	ant::pack(pkg, (uint8_t)point, len);
	ant::pack(pkg, uid, len);
	init_proto_head(pkg, NEW_CARD_GAME_ROLL, len);
    DEBUG_LOG("%lu ROLL [%d %d %d]", m_grp->id, userid, point, uid);
    send_to_players(m_grp, pkg, len);
}


int Cnew_card::move_grid_direct(int grid_id, int roll_point, int direct)
{
	int id = 0;
	if (direct == 1)
	{
		if ( (grid_id + roll_point) < 16 )
		{
			id = grid_id + roll_point;
		}
		else
		{
			id = grid_id + roll_point - 16;
		}
	}
	else
	{
		if (grid_id >= roll_point)
		{
			id = grid_id - roll_point;
		}
		else
		{
			id = 16 - (roll_point - grid_id);
		}
	}

    DEBUG_LOG("%lu move_grid_direct grid_id:%u, direct:%u, roll_point:%u ", m_grp->id, grid_id, direct, roll_point);

	return id;

}

void Cnew_card::notify_move_grid(uint32_t userid, int id)
{
	int len = sizeof (protocol_t);
    ant::pack(pkg, userid, len);
	ant::pack(pkg, (uint8_t)id, len);
	init_proto_head(pkg, NEW_CARD_MOVE_GRID, len);
    DEBUG_LOG("%lu MOVE [%d %d]", m_grp->id, userid, id);
    send_to_players(m_grp, pkg, len);
}

void Cnew_card::notify_act_card_type(uint32_t userid, int type)
{
	int len = sizeof (protocol_t);
    ant::pack(pkg, userid, len);
	ant::pack(pkg, (int8_t)type, len);
	init_proto_head(pkg, NEW_CARD_ATTR_SELECT, len);
    DEBUG_LOG("%lu ATTR SELECT [%d %d]", m_grp->id, userid, type);
    send_to_players(m_grp, pkg, len);
}


void Cnew_card::notify_select_war_user(uint32_t userid, uint32_t war_userid)
{
	int len = sizeof (protocol_t);
    ant::pack(pkg, userid, len);
	ant::pack(pkg, war_userid, len);
	init_proto_head(pkg, NEW_CARD_WAR_USER, len);
    DEBUG_LOG("%lu WAR USER SELECT [%d %d]", m_grp->id, userid, war_userid);
    send_to_players(m_grp, pkg, len);
}


int Cnew_card::set_player(Cplayer* p_player)
{
	player.push_back(p_player);
	init_players.push_back(p_player);
	return 0;
}

void Cnew_card::notify_act_card(uint32_t userid, uint32_t pos)
{
	int len = sizeof (protocol_t);
    ant::pack(pkg, userid, len);
    ant::pack(pkg, uint8_t(pos), len);
	init_proto_head(pkg, NEW_CARD_ACT_CARD, len);
    DEBUG_LOG("%lu ACT CARD [%d]", m_grp->id, userid);
    send_to_players(m_grp, pkg, len);
}

int  Cnew_card::notify_client_ready()
{
	int len = sizeof (protocol_t);

	uint8_t result = challenge_win;
	ant::pack(pkg, result, len);

	uint8_t count = init_players.size();
	ant::pack(pkg, count, len);
	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			uint32_t userid = init_players[i]->id();
			int grid_id = init_player_grid[i];
			init_players[i]->set_grid_id(grid_id);

			init_players[i]->set_num(i);

			grid_info_st[grid_id].num += 1;

			ant::pack(pkg, userid, len);
			ant::pack(pkg, uint8_t(i), len);
		}

	}

	init_proto_head(pkg, NEW_CARD_CLIENT_READY, len);
    DEBUG_LOG("%lu CLIENT READY", m_grp->id);
    send_to_players(m_grp, pkg, len);

	return 0;
}


int Cnew_card::notify_bout_result()
{
	bout_cnt++; //统计回合数

	int len = sizeof (protocol_t);
	uint8_t flag = grid_war;
	ant::pack(pkg, (uint8_t)flag, len);
	uint8_t count = bout_playerid.size();
	ant::pack(pkg, (uint8_t)count, len);

	for (uint32_t i = 0; i < bout_playerid.size(); i++)
	{
		int id = bout_playerid[i];

		Cplayer * pplayer =  init_players[id];

		ant::pack(pkg, (uint32_t)pplayer->id(), len);
		ant::pack(pkg, (uint8_t)pplayer->get_bout_cardid(), len);
		ant::pack(pkg, (uint8_t)pplayer->get_bout_result(), len);
		ant::pack(pkg, (uint8_t)pplayer->get_life_value(), len);

	}

	init_proto_head(pkg, NEW_CARD_BOUT_RESULT, len);
    DEBUG_LOG("%lu BOUT RESULT ", m_grp->id);
    send_to_players(m_grp, pkg, len);

	return 0;
}


int Cnew_card::process_bout_players_lost()
{
	int bout_lost = 0;

	for (uint32_t i = 0; i < bout_playerid.size(); i++)
	{
		int id = bout_playerid[i];
		Cplayer * pplayer =  init_players[id];
		int life_value = pplayer->get_life_value();
		if (life_value <= 0)
		{
			bout_lost++;
		}
	}

	lost_cnt = lost_cnt + bout_lost;

	for (uint32_t i = 0; i < bout_playerid.size(); i++)
	{
		int id = bout_playerid[i];
		Cplayer * pplayer =  init_players[id];
		int life_value = pplayer->get_life_value();
		if (life_value <= 0)
		{
			int grade = init_players_cnt - lost_cnt + 1;
			pplayer->set_grade(grade);
			process_player_lost(pplayer->id());
		}

	}

	return 0;
}


int Cnew_card::notify_game_result()
{
	int len = sizeof (protocol_t);

	uint8_t count = 0;
	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			count = count + 1;
		}

	}
	ant::pack(pkg, (uint8_t)count, len);

	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			int grade = init_players[i]->get_grade();
			int itemid = init_players[i]->get_itemid();
			int cardid = init_players[i]->get_cardid();
			int exp = init_players[i]->get_bout_exp();
			int coins = init_players[i]->get_bout_xiaomee();
			ant::pack(pkg, (uint32_t)init_players[i]->id(), len);
			ant::pack(pkg, (uint8_t)grade, len);
			ant::pack(pkg, (int)exp, len);
			ant::pack(pkg, (int)coins, len);
			ant::pack(pkg, (int)itemid, len);
			ant::pack(pkg, (int)cardid, len);
		}

	}

	init_proto_head(pkg, NEW_CARD_GAME_RESULT, len);
    DEBUG_LOG("%lu GAME RESULT ", m_grp->id);
    send_to_players(m_grp, pkg, len);

	return 0;
}

int Cnew_card::update_db_players_info( )
{

	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			int exp = 0;
			int coins = 0;
			int cardid = 0;
			sprite_t * p = NULL;
			p =	init_players[i]->get_sprite();
			int grade = init_players[i]->get_grade();
			if ( grade < 1 )
			{
    			ERROR_LOG("%lu update_db_players_info userid:%u, grade:%d ", m_grp->id, p->id, grade);
				exp = 0;
				coins = 0;
			}

			if (bout_cnt < 5)
			{
				exp = 5;
				coins = 5;
			}
			else
			{
				if (init_players_cnt == 4)
				{
					if (grade > 4)
					{
    					ERROR_LOG("%lu update_db_players_info userid:%u, grade:%d ", m_grp->id, p->id, grade);
						exp = 0;
						coins = 0;
					}
					else
					{
						exp = score_info_4[grade-1].exp;
						coins = score_info_4[grade-1].coins;
					}

				}
				else if (init_players_cnt == 3)
				{
					if (grade > 3)
					{
    					ERROR_LOG("%lu update_db_players_info userid:%u, grade:%d ", m_grp->id, p->id, grade);
						exp = 0;
						coins = 0;
					}
					else
					{
						exp = score_info_3[grade-1].exp;
						coins = score_info_3[grade-1].coins;
					}

				}
				else if (init_players_cnt == 2)
				{
					if (grade > 2)
					{
    					ERROR_LOG("%lu update_db_players_info userid:%u, grade:%d ", m_grp->id, p->id, grade);
						exp = 0;
						coins = 0;
					}
					else
					{
						exp = score_info_2[grade-1].exp;
						coins = score_info_2[grade-1].coins;
					}

				}

			}

			if (MULTIPLAYER_GAME(p->group->game))
			{
				exp = 0;
			}

			init_players[i]->set_bout_exp(exp);
			init_players[i]->set_bout_xiaomee(coins);


			if (COMPETE_MODE_GAME(p->group->game))
			{
				if (grade == 1)
				{
					int value = taomee::ranged_random(1,100);
					if (value <= 10)
					{
						int id = taomee::ranged_random(0,7);
						cardid = win_card[id];
						if ( !(init_players[i]->is_in_cards(cardid)) )
						{
							init_players[i]->set_cardid(cardid);
						}

					}
				}

			}


			if (init_players[i]->is_up_rank())
			{
				int new_rank = init_players[i]->get_new_rank();
				cardid = rank_card[new_rank];
				init_players[i]->set_cardid(cardid);
			}

			if (p != NULL)
			{
				update_db_player_info(p, exp, coins, cardid);
				send_cnt++;
			}
		}
	}

	return 0;

}



bool Cnew_card::is_my_turn(sprite_t* p)
{
	return (p->id == owner_userid)? true:false;
}


/**
 * @brief 向数据库提交竞赛模式玩家获取经验值
 * @param sprite_t *p 玩家信息
 * @param int exp 对应玩家经验
 * @return
 */
int Cnew_card::update_db_player_info(sprite_t *p, int exp_in, int xiaomee_in, int cardid_in)
{
	score_info s_info = { };
	s_info.exp = exp_in;
	s_info.coins = xiaomee_in;
	s_info.cardid = cardid_in;
	p->waitcmd = proto_new_card_add_exp;
	DEBUG_LOG("Cnew_card::update_db_player_info userid:%u, exp:%d, xiaomee:%d, cardid:%d, waitcmd:%u", p->id, exp_in, xiaomee_in, cardid_in,  p->waitcmd);
	return send_request_to_db(db_new_card_add_exp, p, sizeof(s_info), &s_info, p->id);
}


int Cnew_card::update_db_player_info_callback(sprite_t *p, uint32_t ret_code)
{
	Cplayer* p_player = NULL;
	p_player = get_player(p);
	if (p_player == NULL)
	{
		DEBUG_LOG("%lu player uid=%d aready leave",m_grp->id, p->id);
		return 0;
	}

	if (ret_code == 0)
	{
		if (p_player->is_up_rank())
		{
			int new_rank = p_player->get_new_rank();
			int itemid = rank_item[new_rank];
			p_player->set_itemid(itemid);
			is_get_item = true;
			send_item_cnt++;
			get_item(p, itemid);
			DEBUG_LOG("%lu\t%d RANK %d UP TO %d", m_grp->id, p->id, p_player->get_rank(), new_rank);
		}
		else
		{
			get_items_qs(p_player);
		}

	}
	else
	{
		if (ret_code == 2679)
		{
			p_player->set_bout_exp(0);
			p_player->set_bout_xiaomee(0);

		}
		else if (ret_code == 1116)
		{
			p_player->set_bout_xiaomee(0);
		}
		else if (ret_code == 2675)
		{
			p_player->set_cardid(0);
		}

	}

	recv_cnt++;
    if ((recv_cnt == send_cnt) && (!is_get_item))
	{
		notify_game_result();
		send_cnt = 0;
		recv_cnt = 0;
        return GER_end_of_game;
	}

	return 0;

}

int Cnew_card::update_db_monster_info_callback(sprite_t *p, uint32_t ret_code)
{

	recv_cnt++;
	Cplayer* p_player = NULL;
	p_player = get_player(p);
	if (p_player == NULL)
	{
		DEBUG_LOG("%lu player uid=%d aready leave",m_grp->id, p->id);
		return  GER_end_of_game;
	}

	if ((ret_code == 0) || (monster_id == 10))
	{
		if (p_player->get_v_itemid_size() > 0)
		{
			is_get_item = true;
			send_item_cnt++;
			get_items_max(p_player);
		}

		int itemid = p_player->get_itemid();
		if (itemid > 0)
		{
			is_get_item = true;
			send_item_cnt++;
			get_item(p_player->get_sprite(), itemid);

		}

		if (is_get_item)
		{
			return 0;
		}
	}
	else
	{
		if (ret_code == 2675)
		{
			p_player->set_cardid(0);
		}

		p_player->set_itemid(0);
		p_player->clear_v_itemid();
	}

    if ((recv_cnt == send_cnt) && (!is_get_item))
	{
		notify_monster_over();
		send_cnt = 0;
		recv_cnt = 0;
        return GER_end_of_game;

	}

	return  GER_end_of_game;

}

int Cnew_card::update_db_dec_item_callback(sprite_t *p, uint32_t ret_code)
{
	Cplayer* p_player = NULL;
	p_player = get_player(p);
	if (p_player == NULL)
	{
		DEBUG_LOG("%lu player uid=%d aready leave",m_grp->id, p->id);
		return 0;
	}

	int result = 0;
	int win = 0;
	if (ret_code == 0)
	{
		result = 1;
		switch (itemid)
		{
			case 190672:
				{
					s_xp = s_xp + 1;
					break ;
				}
			case 190684:
				{
					m_xp = m_xp + 1;
					break ;
				}
			case 190706:
				{
					h_xp = h_xp + 1;
					break ;
				}
			case 190675:
				{
					l_xp = l_xp + 1;
					break ;
				}
			default :
				{
					break ;
				}
		}

		int rate = 1*s_xp + 5*m_xp + 10*h_xp + 20*l_xp;
		int value = taomee::ranged_random(1,100);
		if (value <= rate)
		{
			win = 1;
		}

	}

	if (monster_id == 10)
	{
		win = 0;
	}

	if ( win == 1 )
	{
		challenge_win = 1;
	}

	p->waitcmd = 0;

	notify_dec_item_result(p->id, result, win);
	return 0;

}



int Cnew_card::get_item_callback(sprite_t *p, uint32_t ret_code)
{
	Cplayer* p_player = NULL;
	p_player = get_player(p);
	if (p_player == NULL)
	{
		DEBUG_LOG("%lu player uid=%d aready leave",m_grp->id, p->id);
		return 0;
	}

	if (ret_code != 0)
	{
		p_player->set_itemid(0);
	}

	recv_item_cnt++;

	if (player_num == single_player)
	{
		if ((recv_cnt == send_cnt) && (recv_item_cnt == send_item_cnt))
		{
			notify_monster_over();
			send_cnt = 0;
			recv_cnt = 0;
			send_item_cnt = 0;
			recv_item_cnt = 0;
        	return GER_end_of_game;
		}
	}
	else
	{
		if ((recv_cnt == send_cnt) && (recv_item_cnt == send_item_cnt))
		{
			notify_game_result();
			send_cnt = 0;
			recv_cnt = 0;
			send_item_cnt = 0;
			recv_item_cnt = 0;
        	return GER_end_of_game;

		}

	}

	return 0;

}



/**
 * @brief  竞赛模式提交胜负后，处理数据库返回结果
 * @param
 * @return GER_end_of_game 两个玩家都有返回或者错误才能结束
 * @return 0			   两个玩家没有都返回
 */
int Cnew_card::handle_db_return(sprite_t *p, uint32_t id, const void* buf, int len, uint32_t ret_code)
{

	switch(p->waitcmd)
	{
        case proto_get_items:
			{
				return  get_item_callback(p, ret_code);

			}
        case proto_new_card_add_exp:
			{
				return  update_db_player_info_callback(p, ret_code);

			}
		case proto_new_card_add_monster:
			{
				return  update_db_monster_info_callback(p, ret_code);
			}
		case proto_new_card_dec_item:
			{
				return  update_db_dec_item_callback(p, ret_code);
			}
		default:
			ERROR_LOG("%lu\tCnew_card:: handle db return cmd undef %d", m_grp->id, p->waitcmd);
	}

	return 0;
}

void Cnew_card::set_bout_status(int grid_id)
{
	int grid_type =grid_info_st[grid_id].type ;
	int num =grid_info_st[grid_id].num ;
	if (num > 1)
	{
		bout_type = none_type;
		return ;
	}

	switch (grid_type)
	{
		case fire_t:
			{
				bout_type = fire;
				break ;
			}
		case wood_t:
			{
				bout_type = wood;
				break ;
			}
		case water_t:
			{
				bout_type = water;
				break ;
			}
		case war_t:
			{
				bout_type = none_type;
				break ;
			}
		case attr_t:
			{
				bout_type = none_type;
				break ;
			}
		default:
			{
				bout_type = none_type;
			}
	}

	return ;

}



void Cnew_card::set_bout_status_by_attr(int attr_type)
{
	switch (attr_type)
	{
		case fire_t:
			{
				bout_type = fire;
				break ;
			}
		case wood_t:
			{
				bout_type = wood;
				break ;
			}
		case water_t:
			{
				bout_type = water;
				break ;
			}
		default:
			{
				ERROR_LOG("Cnew_card:: select attr type err %d", attr_type);
			}
	}

	return ;

}


void Cnew_card::set_bout_playerid(int grid_id)
{
	bout_playerid.clear();
	int grid_type =grid_info_st[grid_id].type ;
	if (grid_type != war_t)
	{
		for (uint32_t i = 0; i < player.size(); i++)
		{
			uint32_t num = player[i]->get_num();
			bout_playerid.push_back(num);
		}

	}

}

void Cnew_card::set_grid_num(int grid_leave, int grid_enter)
{

	grid_info_st[grid_leave].num = grid_info_st[grid_leave].num - 1 ;
	grid_info_st[grid_enter].num = grid_info_st[grid_enter].num + 1 ;

	return ;

}


void Cnew_card::set_grid_other_userid(int grid_id, uint32_t  userid)
{
	std::vector<Cplayer*>::iterator it;
	for (it = player.begin(); it != player.end(); it++)
	{
		if ( (*it)->get_grid_id() == grid_id)
		{
			uint32_t u_id = (*it)->id();
			if (u_id != userid)
			{
				grid_userids.push_back(u_id);
			}
		}
	}

	return ;

}


void Cnew_card::set_bout_playerid_by_userid(uint32_t grid_war_userid, uint32_t war_userid)
{
	bout_playerid.clear();
	uint32_t num = 0;
	Cplayer* p_player = NULL;
	p_player = get_player(grid_war_userid);
	if (p_player != NULL)
	{
		num = p_player->get_num();
	}
	bout_playerid.push_back(num);

	p_player = get_player(war_userid);
	if (p_player != NULL)
	{
		num = p_player->get_num();
	}
	bout_playerid.push_back(num);

	return ;

}

int Cnew_card::on_player_leave(sprite_t* p)
{
	if (CHALLENGE_GAME(p->group->game))
	{
		DEBUG_LOG("challenge mode leave end game userid:%d", p->id);
		return  GER_end_of_game;
	}

	uint32_t roll_userid = 0;

	if ((uint32_t)owner_roll < player.size())
	{
		if (player[owner_roll] != NULL)
		{
			roll_userid = player[owner_roll]->id();
		}
		else
		{
			DEBUG_LOG("on_player_leave  userid:%d, owner_roll:%d, player size:%lu", p->id, owner_roll, player.size());

		}

	}

	int num = -1;

	uint32_t userid = p->id;

	for (uint32_t i = 0; i < player.size(); i++)
	{
		if(player[i]->get_sprite() == p)
        {
			int grid_id = player[i]->get_grid_id();
			grid_info_st[grid_id].num = grid_info_st[grid_id].num - 1;
			player[i]->set_grid_id(-1);
			player[i]->set_grade(0);

			if (player[i]->get_life_value() > 0)
			{
				lost_cnt++;
				player[i]->set_life_value(0);

			}

            player.erase(player.begin()+i);
        }
	}

	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			if(init_players[i]->get_sprite() == p)
        	{
				num = init_players[i]->get_num();
				init_players[i]->set_sprite(NULL);

// 				player_cards_info_t* p_cards_info = init_players[i]->get_cards_info();
//             	delete p_cards_info;
//             	p_cards_info = NULL;
				if( init_players[i]->p_cards_info != NULL )
				{
					delete init_players[i]->p_cards_info;
					init_players[i]->p_cards_info = NULL;
				}
            	delete init_players[i];
            	init_players[i] = NULL;

            	DEBUG_LOG("%lu player leave game %d", p->group->id, p->id);
        	}
		}

	}

	if (player.size() >= 1)//还剩1个人以上
	{
		uint32_t next_userid = 0;
		if (roll_userid == userid)
		{
        	DEBUG_LOG("on_player_leave:  owner_roll:%d, count:%lu", owner_roll, player.size());
			owner_roll = (owner_roll + 1)%player.size();
			next_userid = player[owner_roll]->id();
		}
		else
		{
			next_userid = roll_userid;
		}
		notify_player_leave(userid, next_userid);
		if (num != -1)
		{
			modify_bout_player_info(num);
			del_grid_userid(userid);

			if (is_bout_all_ready())
			{
				process_bout_result();
				notify_bout_result();
				process_bout_players_lost();
				bout_clear();
				if (grid_war)
				{
					uint32_t war_userid = 0;
					if (grid_userids.size() > 0)
					{
						set_act_state(ACT_CARD);
						war_userid = grid_userids[0];
						grid_userids.erase(grid_userids.begin());
						notify_select_war_user(grid_war_userid, war_userid);
						set_bout_playerid_by_userid(grid_war_userid, war_userid);
					}
					if (grid_userids.size() > 0)
					{
						grid_war = true;
					}
					else
					{
						grid_war = false;
						grid_war_userid = 0;
						grid_userids.clear();
					}
				}
				else
				{
					set_act_state(ROLL);
				}
			}
		}
	}

	process_game_result();

	if (!game_start)
	{
		if (is_game_ready())
		{
			int num = get_game_ready_num();
			if (num <= 1)
			{
				return  GER_end_of_game;
			}

			notify_client_ready();
			owner_userid = player[0]->id();
			game_start = 1;
		}
	}

	return  0;

}

void Cnew_card::notify_player_leave(uint32_t userid, uint32_t next_userid)
{
	int len = sizeof (protocol_t);
	uint8_t flag = 0;
	if((userid == owner_userid) && (act_state != ACT_CARD))
	{
		flag = 1;
		set_act_state(ROLL);
	}

	if (userid == grid_war_userid)
	{
		grid_war = false;
		grid_war_userid = 0;
		grid_userids.clear();
		flag = 1;
		set_act_state(ROLL);
	}

    ant::pack(pkg, userid, len);
    ant::pack(pkg, next_userid, len);
    ant::pack(pkg, flag, len);
	init_proto_head(pkg, NEW_CARD_PLAYER_QUIT, len);
    DEBUG_LOG("%lu PLAYER QUIT user:%u, owner_userid:%u, next_user:%u, flag:%d", m_grp->id, userid, owner_userid, next_userid, flag);
    send_to_players(m_grp, pkg, len);
}

void Cnew_card::modify_bout_player_info(int num)
{
	for (uint32_t i = 0; i < bout_playerid.size(); i++)
	{
		if (num == bout_playerid[i])
		{
			bout_playerid.erase(bout_playerid.begin() + i);
		}
	}

}

void Cnew_card::del_grid_userid(uint32_t userid)
{
	for (uint32_t i = 0; i < grid_userids.size(); i++)
	{
		if (userid == grid_userids[i])
		{
			grid_userids.erase(grid_userids.begin() + i);
		}
	}

}


void Cnew_card::process_player_lost(uint32_t userid)
{
	uint32_t next_userid = 0;
	uint32_t roll_userid = player[owner_roll]->id();

	if (roll_userid == userid)
	{
		owner_roll = (owner_roll + 1)%player.size();
		next_userid = player[owner_roll]->id();
	}
	else
	{
		next_userid = player[owner_roll]->id();
	}

	for (uint32_t i = 0; i < player.size(); i++)
	{
		if (player[i]->id() == userid)
		{
			int grid_id = player[i]->get_grid_id();
			grid_info_st[grid_id].num = grid_info_st[grid_id].num - 1;
			player[i]->set_grid_id(-1);
			player[i]->set_life_value(0);

			player.erase(player.begin() + i);
		}
	}

	for (uint32_t i = 0; i < player.size(); i++)
	{
		if (player[i]->id() == next_userid)
		{
			owner_roll = i;
		}
	}

	notify_player_leave(userid, next_userid);

}

void Cnew_card::set_act_state(int state)
{
	act_state = state;
}

void Cnew_card::set_act_state_by_grid(int grid_id)
{
	int grid_type =grid_info_st[grid_id].type ;
	switch (grid_type)
	{
		case fire_t:
		case wood_t:
		case water_t:
			{
				act_state = ACT_CARD;
				break ;
			}
		case war_t:
			{
				act_state = WAR_SELECT;
				break ;
			}
		case attr_t:
			{
				act_state = ATTR_SELECT;
				break ;
			}
		default:
			{
    			ERROR_LOG("set_act_state_by_grid,error grid_id:%u, grid_type:%u", grid_id, grid_type);
			}
	}

	return ;

}

bool Cnew_card::check_bout_userid(uint32_t userid)
{
	for (uint32_t i = 0; i < bout_playerid.size(); i++)
	{
		int id = bout_playerid[i];
		if (init_players[id]->id() == userid )
		{
			return true;
		}
	}

	return false;

}

void Cnew_card::set_owner_userid_by_grid(int grid_id, uint32_t userid)
{
	int grid_type =grid_info_st[grid_id].type ;
	switch (grid_type)
	{
		case war_t:
		case attr_t:
			{
				owner_userid = userid;
				break ;
			}
		default:
			{
				break ;
			}
	}

	return ;

}

int Cnew_card::get_item(sprite_t *p, int item_id)
{
    game_items_t items;
    items.flag = 0;
    items.newcount = 1;
    items.itm_id[0] = item_id;
    return db_get_items(p, &items);
}

int Cnew_card::get_items_max(Cplayer *p_player)
{
    game_items_t items;
    items.flag = 0;

	uint32_t count =  p_player->get_v_itemid_size();
	for (uint32_t i = 0; i < count; i++)
	{
    	items.itm_id[i] = p_player->get_v_itemid(i);
	}
	uint32_t item_max = 99999;

    items.newcount = count;
	sprite_t* p = p_player->get_sprite();
    return db_get_items_max(p, &items, item_max);
}

int Cnew_card::get_item_without_return(sprite_t *p, int item_id)
{
    game_items_t items;
    items.flag = 0;
    items.newcount = 1;
    items.itm_id[0] = item_id;
    return db_get_items_without_return(p, &items);
}



void Cnew_card::notify_monster_result(uint32_t userid, uint32_t result)
{
	int len = sizeof (protocol_t);
    ant::pack(pkg, uint8_t(result), len);
	init_proto_head(pkg, NEW_CARD_SELECT_MONSTER, len);
    DEBUG_LOG("%lu MONSTER RESULT [%u], result:%d", m_grp->id, userid, result);
    send_to_players(m_grp, pkg, len);
}

int Cnew_card::get_item_by_monster(int monster_id)
{
	int value = taomee::ranged_random(1,100);
	int itemid = 0;
	switch (monster_id)
	{
		case 1:
			 {
				if (value > 50)
				{
					itemid = 190672;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 3;
					return itemid ;
				}

				break ;
			 }
		case 2:
			 {
				if (value > 50)
				{
					itemid = 190672;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 23;
					return itemid ;
				}

				break ;
			 }
		case 3:
			 {
				if (value > 65)
				{
					itemid = 190672;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 190674;
					return itemid ;
				}
				else if (value > 10)
				{
					itemid = 24;
					return itemid ;
				}

				break ;
			 }
		case 4:
			 {
				if (value > 65)
				{
					itemid = 190672;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 190674;
					return itemid ;
				}
				else if (value > 10)
				{
					itemid = 43;
					return itemid ;
				}

				break ;
			 }

		case 5:
			{
				return 0;
			}
		case 6:
			 {
				if (value > 65)
				{
					itemid = 190672;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 190673;
					return itemid ;
				}
				else if (value > 10)
				{
					itemid = 45;
					return itemid ;
				}

				break ;
			 }
		case 7:
			 {
				if (value > 65)
				{
					itemid = 190684;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 190673;
					return itemid ;
				}
				else if (value > 10)
				{
					itemid = 5;
					return itemid ;
				}

				break ;
			 }
		case 8:
			 {
				if (value > 65)
				{
					itemid = 190684;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 190689;
					return itemid ;
				}
				else if (value > 10)
				{
					itemid = 6;
					return itemid ;
				}
				break ;
			 }
		case 9:
			 {
				if (value > 65)
				{
					itemid = 190673;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 190689;
					return itemid ;
				}
				else if (value > 10)
				{
					itemid = 26;
					return itemid ;
				}
				break ;
			 }

		case 10:
			{
				itemid = 13476;
				return itemid;
			}

		case 13:
			 {
				if (value > 65)
				{
					itemid = 190674;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 190684;
					return itemid ;
				}
				else if (value > 10)
				{
					itemid = 27;
					return itemid ;
				}
				break ;
			 }
		case 15:
			 {
				if (value > 65)
				{
					itemid = 190674;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 190673;
					return itemid ;
				}
				else if (value > 10)
				{
					itemid = 7;
					return itemid ;
				}
				break ;
			 }
		case 16:
			 {
				if (value > 65)
				{
					itemid = 190685;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 190689;
					return itemid ;
				}
				else if (value > 10)
				{
					itemid = 47;
					return itemid ;
				}
				break ;
			 }
		case 18:
			 {
				if (value > 65)
				{
					itemid = 190675;
					return itemid ;
				}
				else if (value > 30)
				{
					itemid = 190673;
					return itemid ;
				}
				else if (value > 10)
				{
					itemid = 10;
					return itemid ;
				}

				break ;
			 }

		default :
			{
				itemid = 0;
				return itemid ;
			}
	}

	return itemid ;
}

void Cnew_card::set_challenge_result(Cplayer* p_player, uint32_t monster_id, uint32_t mole_level, uint32_t high_card)
{
	if (monster_id == 10) //turin
	{
		int exp = p_player->get_card_exp();
		if (exp < 5000)
		{
			challenge_win = 0;
			return ;
		}
		else
		{
			challenge_win = 1;
			return ;
		}
	}

	if ((monster_id==11)||(monster_id==12)||(monster_id==14)||(monster_id==17)||(monster_id==19)||(monster_id==20)||(monster_id==21)||(monster_id==22)||(monster_id==23))//boss
	{
		int rate  = taomee::ranged_random(1,100);

		int win = 0;
		if (mole_level > monster_level[monster_id])
		{
			win = (mole_level - monster_level[monster_id])*6 ;

		}

		win = win + high_card*3 ;
		win = win + 10;

		if (rate < win)
		{
			challenge_win = 1;
		}
		else
		{
			challenge_win = 0;
		}

		if (monster_id == 21)
		{
			if (mole_level >= 15)
			{
				challenge_win = 1;
			}
		}

		return ;
	}


	if (mole_level > monster_level[monster_id])
	{
		challenge_win = 1;
	}
	else
	{
		int lost = (monster_level[monster_id] - mole_level)*3;
		int value  = taomee::ranged_random(1,100);
		if (value > lost)
		{
			challenge_win = 1;
		}
		else
		{
			challenge_win = 0;
		}
	}

	if (monster_id < 6)
	{
		if (high_card >= 5)
		{
			challenge_win = 1;
		}
	}

	return ;

}

int Cnew_card::on_monster_over(Cplayer* p_player)
{

#ifndef TW_VER
    uint32_t msg[2];
    msg[0] = p_player->id();
    msg[1] = 1;
    int msglog_type = 0x02100200 + monster_id;

    int msgret = msglog(statistic_file, msglog_type, get_now_tv()->tv_sec, &msg, sizeof (msg));
    if (msgret != 0)
	{
        ERROR_LOG( "statistic log error: message type(%x)",msglog_type);
    }

#endif

    sprite_t * p = p_player->get_sprite();

	int d_flag = 0;

	//const struct tm* cur_time = get_now_tm();
	//if (cur_time->tm_wday == 5 || cur_time->tm_wday == 6 || cur_time->tm_wday == 0)
    {
        d_flag = 1;
    }

    int modulus = 1;

	if (d_flag)
	{
        modulus = modulus * 2;
    }

    if ((get_now_tv()->tv_sec - p->stamp + p->oltoday) >= TWO_HOUR_SEC)
    {
        if (d_flag)
        {
            modulus = modulus / 2;
        }
        else
        {
            modulus = 0;
        }
    }

    if ((get_now_tv()->tv_sec - p->stamp + p->oltoday) >= FIVE_HOUR_SEC) {
        modulus = 0;
    }

	if (!challenge_win)
	{
		notify_monster_over();
		return  GER_end_of_game;
	}

	if ( !modulus && (monster_id != 10))
	{
		notify_monster_over();
		return  GER_end_of_game;
	}

    int cardid = 0;
    int flag = 0;
	int itemid = 0;
	if (challenge_win > 0)
	{
		if (monster_id != 10)
		{
			xiaomee = monster_bonus[monster_id].xiaomee * modulus;
    		mole_exp = monster_bonus[monster_id].exp * modulus;
    		strong = monster_bonus[monster_id].strong * modulus;
		}
		else
		{
			flag = 1;
		}

		if ((monster_id==11)||(monster_id==12)||(monster_id==14)||(monster_id ==17)||(monster_id==19)||(monster_id==20)||(monster_id==21)||(monster_id==22)||(monster_id==23))//boss
		{
			get_items_by_boss(p_player, monster_id);
		}
		else
		{
			itemid = get_item_by_monster(monster_id);
			if (itemid > 0)
			{
				if (itemid <= 59)
				{
					if (p_player->is_in_cards(itemid))
					{
						itemid = 190672;
						p_player->add_v_itemid(itemid);
					}
					else
					{
						p_player->set_cardid(itemid);
					}
				}
				else
				{
					if (itemid == 13476) //changqiang
					{
						p_player->set_itemid(itemid);
					}
					else
					{
						p_player->add_v_itemid(itemid);
					}
				}
			}
		}

    	cardid = p_player->get_cardid();

		DEBUG_LOG("Cnew_card::on_monster_over userid:%u, mole_exp:%d, cardid:%d, win:%d, monster_id:%d, flag:%d", p_player->id(), mole_exp, cardid, challenge_win, monster_id, flag);
		update_db_monster_info(p_player->get_sprite(), mole_exp, xiaomee, strong, cardid, flag);
		send_cnt++;
	}

	return 0;

}

int Cnew_card::update_db_monster_info(sprite_t *p, int mole_exp, int xiaomee, int strong, int cardid, int flag)
{
	monster_info s_info = { };
	s_info.mole_exp = mole_exp;
	s_info.xiaomee = xiaomee;
	s_info.strong = strong;
	s_info.cardid = cardid;
	s_info.flag = flag;
	p->waitcmd = proto_new_card_add_monster;
	DEBUG_LOG("Cnew_card::update_db_monster_info userid:%u, mole_exp:%d, cardid:%d, waitcmd:%u", p->id, mole_exp, cardid, p->waitcmd);
	return send_request_to_db(db_new_card_add_monster, p, sizeof(s_info), &s_info, p->id);
}

void Cnew_card::notify_dec_item_result(uint32_t userid, uint32_t result, uint32_t win)
{
	int len = sizeof (protocol_t);
    ant::pack(pkg, uint8_t(result), len);
    ant::pack(pkg, uint8_t(win), len);
	init_proto_head(pkg, NEW_CARD_DEC_ITEM, len);
    DEBUG_LOG("%lu DEC ITEM [%d]", m_grp->id, userid);
    send_to_players(m_grp, pkg, len);

}

int Cnew_card::update_db_dec_item(sprite_t *p, int itemid)
{
	item_info s_info = { };
	s_info.attireid = itemid;
	s_info.operatetype = 0;
	s_info.value = 1;
	s_info.maxvalue = 99999;
	p->waitcmd = proto_new_card_dec_item;
	DEBUG_LOG("Cnew_card::update_db_dec_itemattireid:%d,  waitcmd:%u", itemid, p->waitcmd);
	return send_request_to_db(db_new_card_dec_item, p, sizeof(s_info), &s_info, p->id);
}

int Cnew_card::on_dec_item(sprite_t* p, int itemid)
{
	update_db_dec_item(p, itemid);

	return 0;

}


bool Cnew_card::is_game_ready()
{
	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			if (init_players[i]->get_game_status() != GAME_READY)
			{
				return false;
			}
		}
	}

	return true;

}

int Cnew_card::get_game_ready_num()
{
	int ready_num = 0;
	for (uint32_t i = 0; i < init_players.size(); i++)
	{
		if (init_players[i] != NULL)
		{
			if (init_players[i]->get_game_status() == GAME_READY)
			{
				ready_num++;
			}
		}
	}

	return ready_num;

}


int Cnew_card::get_items_by_boss(Cplayer* p_player, int id)
{
	int value = taomee::ranged_random(1,100);
	int itemid = 0;
	int cardid = 0;
	switch (id)
	{
		case 11:
			{
				if (value > 75)
				{
					cardid = 50;
				}
				else if (value > 50)
				{
					cardid = 30;
				}
				if (cardid > 0)
				{
					if (!(p_player->is_in_cards(cardid)))
					{
						p_player->set_cardid(cardid);
					}
					cardid = 0;
				}

				if (value > 60)
				{
					itemid = 190684;
				}
				else if (value > 20)
				{
					itemid = 190673;
				}
				if (itemid > 0)
				{
					p_player->add_v_itemid(itemid);
					itemid = 0;
				}
				break ;
			}
		case 12:
			{
				if (value > 88)
				{
					itemid = 13482;
				}
				else if (value > 76)
				{
					itemid = 13497;
				}
				else if (value > 64)
				{
					itemid = 13483;
				}
				else if (value > 50)
				{
					itemid = 13510;
				}
				if (itemid > 0)
				{
					p_player->set_itemid(itemid);
					itemid = 0;
				}

				if (value > 70)
				{
					itemid = 190674;
				}
				else if (value > 40)
				{
					itemid = 190684;
				}
				else if (value > 20)
				{
					itemid = 190688;
				}
				if (itemid > 0)
				{
					p_player->add_v_itemid(itemid);
					itemid = 0;
				}
				break ;
			}

		case 14:
			{
				if (value > 84)
				{
					cardid = 31;
				}
				else if (value > 68)
				{
					cardid = 11;
				}
				else if (value > 50)
				{
					cardid = 51;
				}
				if (cardid > 0)
				{
					if (!(p_player->is_in_cards(cardid)))
					{
						p_player->set_cardid(cardid);
						cardid = 0;
					}
				}

				if (value > 75)
				{
					itemid = 190685;
				}
				else if (value > 50)
				{
					itemid = 190706;
				}
				else if (value > 20)
				{
					itemid = 190688;
				}
				if (itemid > 0)
				{
					p_player->add_v_itemid(itemid);
					itemid = 0;
				}

				break ;
			}

		case 17:
			{
				if (value > 84)
				{
					itemid = 13526;
				}
				else if (value > 68)
				{
					itemid = 13496;
				}
				else if (value > 50)
				{
					itemid = 13495;
				}
				if (itemid > 0)
				{
					p_player->set_itemid(itemid);
					itemid = 0;
				}

				if (value > 70)
				{
					itemid = 190685;
				}
				else if (value > 40)
				{
					itemid = 190675;
				}
				else if (value > 20)
				{
					itemid = 190688;
				}
				if (itemid > 0)
				{
					p_player->add_v_itemid(itemid);
					itemid = 0;
				}

				break ;
			}
		case 19:
			{
				if (value > 84)
				{
					cardid = 16;
				}
				else if (value > 68)
				{
					cardid = 56;
				}
				else if (value > 50)
				{
					cardid = 36;
				}
				if (cardid > 0)
				{
					if (!(p_player->is_in_cards(cardid)))
					{
						p_player->set_cardid(cardid);
					}
				}

				if (value > 84)
				{
					itemid = 190706;
				}
				else if (value > 68)
				{
					itemid = 190675;
				}
				else if (value > 50)
				{
					itemid = 190688;
				}
				if (itemid > 0)
				{
					p_player->add_v_itemid(itemid);
				}

				break ;

			}
		case 20:
			{
				if (value > 84)
				{
					itemid = 13508;
				}
				else if (value > 68)
				{
					itemid = 13506;
				}
				else if (value > 50)
				{
					itemid = 13507;
				}
				if (itemid > 0)
				{
					p_player->set_itemid(itemid);
					itemid = 0;
				}

				if (value > 84)
				{
					itemid = 190706;
				}
				else if (value > 68)
				{
					itemid = 190685;
				}
				else if (value > 50)
				{
					itemid = 190688;
				}
				if (itemid > 0)
				{
					p_player->add_v_itemid(itemid);
				}

				break ;

			}
		case 21:
			{
				if (value > 88)
				{
					itemid = 13540;
				}
				else if (value > 76)
				{
					itemid = 13541;
				}
				else if (value > 64)
				{
					itemid = 13542;
				}
				else if ( value > 50)
				{
					itemid = 13543;
				}
				if (itemid > 0)
				{
					p_player->set_itemid(itemid);
					itemid = 0;
				}

				if (value > 84)
				{
					itemid = 190706;
				}
				else if (value > 68)
				{
					itemid = 190675;
				}
				else if (value > 50)
				{
					itemid = 190688;
				}
				if (itemid > 0)
				{
					p_player->add_v_itemid(itemid);
				}

				break ;
			}
		case 22:
			{
				if (value > 88)
				{
					itemid = 13540;
				}
				else if (value > 76)
				{
					itemid = 13541;
				}
				else if (value > 64)
				{
					itemid = 13542;
				}
				else if ( value > 50)
				{
					itemid = 13543;
				}
				if (itemid > 0)
				{
					p_player->set_itemid(itemid);
					itemid = 0;
				}

				if (value > 84)
				{
					itemid = 190706;
				}
				else if (value > 68)
				{
					itemid = 190675;
				}
				else if (value > 50)
				{
					itemid = 190688;
				}
				if (itemid > 0)
				{
					p_player->add_v_itemid(itemid);
				}

				break ;
			}
			case 23:
			{
				if (value > 84)
				{
					cardid = 38;
				}
				else if (value > 68)
				{
					cardid = 18;
				}
				else if (value > 50)
				{
					cardid = 58;
				}
				if (cardid > 0)
				{
					if (!(p_player->is_in_cards(cardid)))
					{
						p_player->set_cardid(cardid);
					}
				}

				if (value > 84)
				{
					itemid = 190706;
				}
				else if (value > 68)
				{
					itemid = 190685;
				}
				else if (value > 50)
				{
					itemid = 190688;
				}
				if (itemid > 0)
				{
					p_player->add_v_itemid(itemid);
				}

				break ;
			}

		default :
			{
				itemid = 0;
			}
	}

	return 0;

}

int Cnew_card::notify_monster_over()
{
	int len = sizeof (protocol_t);

	if (init_players[0] != NULL)
	{
		int itemid = 0;
		uint32_t count = 0;
		int cardid = 0;

		if (challenge_win && (monster_id != 10) )
		{
			cardid = init_players[0]->get_cardid();
		}

		ant::pack(pkg, (int)mole_exp, len);
		ant::pack(pkg, (int)xiaomee, len);
		ant::pack(pkg, (int)strong, len);
		ant::pack(pkg, (int)0, len);
		ant::pack(pkg, (int)cardid, len);

		itemid = init_players[0]->get_itemid();
		if (itemid > 0)
		{
			init_players[0]->add_v_itemid(itemid);

		}

		count = init_players[0]->get_v_itemid_size();

		ant::pack(pkg, (int)count, len);
		for (uint32_t i = 0; i < count; i++)
		{
			itemid = init_players[0]->get_v_itemid(i);
			ant::pack(pkg, (int)itemid, len);
		}
	}
	else
	{
		ERROR_LOG("%lu MONSTER OVER", m_grp->id);

	}

	DEBUG_LOG("%lu MONSTER OVER CMD:%d", m_grp->id, NEW_CARD_MONSTER_OVER);
	init_proto_head(pkg, NEW_CARD_MONSTER_OVER, len);
    send_to_players(m_grp, pkg, len);

	return 0;

}

int Cnew_card::get_items_qs(Cplayer *p_player)
{

    uint32_t rank  =  p_player->get_new_rank();
	uint32_t i = 1;
    for (i = 1; i <= rank; i++)
    {
        int itemid = rank_item[i];
		get_item_without_return(p_player->get_sprite(), itemid);

    }

	int flag = 0;
	flag = p_player->get_flag();
	if (flag > 0)
	{
		int id = 13476;
		get_item_without_return(p_player->get_sprite(), id);
	}

	return 0;

}

int Cnew_card::card_switch_item(int cardid)
{
	int itemid = 0;
	switch (cardid)
	{
		case 8:
		case 28:
		case 48:
			{
				itemid = 190727;
				break ;
			}
		default :
			{
				itemid = 190672;
				break ;
			}
	}

	return itemid;

}



