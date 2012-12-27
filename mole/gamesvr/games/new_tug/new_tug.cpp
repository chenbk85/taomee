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
#include "new_tug.hpp"


struct score_info score_info_7[] =
{
	{300, 150, 6},
	{200, 100, 5},
	{100, 50,  4},
	{80,  40,  3},
	{60,  30,  2},
	{40,  20,  1},
	{0,   0,   0},
};


/**
 * @brief 卡牌类构造函数
 * @param  玩家所在用户组
 * @return 永远不会G返回
 */
Cnew_tug::Cnew_tug(game_group_t *grp):players_cnt(0),m_grp(grp),send_cnt(0),recv_cnt(0),game_start(0),time_start(0)
{


}

int Cnew_tug::result[CARD_TYPE_MAX][CARD_TYPE_MAX] =
{
	//水，	火， 木
	{DRAW,	  WIN,    LOST,},
	{LOST,    DRAW,   WIN,},
	{WIN,     LOST,   DRAW,},
};


/**
 * @brief  处理玩家用户游戏通讯
 * @param
 * @return 返回GER_end_of_game结束游戏,返回0游戏继续
 */
int Cnew_tug::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
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
        case NEW_TUG_GET_CARDS:
			{
            	return send_player_cards(p);
			}

		case NEW_TUG_CLIENT_READY:
			{
				p_player->set_game_status(GAME_READY);
				if (is_game_ready())
				{
					notify_client_ready();
					game_start = 1;
					time_start = time(NULL);

					ADD_TIMER_EVENT(p->group, on_game_timer_expire, p_player, now.tv_sec + 60);

				}

				break ;
			}

		case NEW_TUG_ACT_CARD:
			{
				CHECK_BODY_LEN(len, 2);
				int8_t type = 0;
				int8_t pos = 0;
                int i = 0;
                ant::unpack(body, type, i);
                ant::unpack(body, pos, i);
				if (type < 0 || type > 4)
				{
					ERROR_LOG("%lu Cnew_tug: %d error type %d", m_grp->id, p->id, type);
                    return GER_end_of_game;
				}
				int ret = on_card_act(p, type, pos);
				if (ret != 0)
				{
            		return GER_end_of_game;
				}
				break ;
			}

		case NEW_TUG_USER_MISS:
			{
				p_player->update_life_dec(1);
				process_game_result();
				break ;
			}

        case proto_player_leave:
			{
				DEBUG_LOG("%lu  new tug player user:%d leave , cmd:%u",m_grp->id, p->id, cmd);
				return on_player_leave(p);
			}
        default:
			{
            	ERROR_LOG("%lu\tnew tug, undef cmd: %d", m_grp->id, cmd);
            	return GER_end_of_game;
			}
    }

	return 0;

}

Cplayer* Cnew_tug::get_player(sprite_t* p)
{
	for (uint32_t i = 0; i < players.size(); i++)
	{
		if (players[i] != NULL)
		{
			if (players[i]->get_sprite() == p)
			{
				return  players[i];
			}
		}
	}
	return NULL;
}


int Cnew_tug::on_card_act(sprite_t* p, int type, int pos)
{
	Cplayer* p_player = NULL;
	p_player = get_player(p);
	if (p_player == NULL)
	{
		return 0;
	}

	card_t card_info1 = {};
	card_info1.id = -1;
	card_info1.type = type;
	card_info1.value = 0;

	p_player->act_bout_card(pos);
	int cardid =  p_player->get_bout_cardid();
	card_t card_info2 = (*cards_info)[cardid];

	int result = compare(card_info1, card_info2);
	if (result != WIN)
	{
		p_player->update_life_dec(1);
	}

	notify_act_card(p->id, type, pos);

	process_game_result();

	return 0;

}

int Cnew_tug::process_game_result()
{
	int live_players = 0;
	for (uint32_t i = 0; i < players.size(); i++)
	{
		if (players[i] != NULL)
		{
			if ( players[i]->get_life_value() > 0)
			{
				live_players++;
			}
		}
	}

	if (live_players <= 1)
	{
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
int Cnew_tug::compare(card_t p1, card_t p2)
{
	int my_result = Cnew_tug::result[p1.type][p2.type];
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

int Cnew_tug::set_all_card_info(card_t (*all_cards)[CARD_ID_MAX])
{
	if (all_cards != NULL)
	{
		cards_info = all_cards;
	}

	return 0;
}


/**
 * @brief 给准备好的玩家发送本次游戏的卡牌队列
 * @param
 * @return
 */
int Cnew_tug::send_player_cards(sprite_t* p)
{
	Cplayer * p_player = NULL;
	int l = sizeof (protocol_t);
	for (uint32_t i = 0; i < players.size(); i++)
	{
		if (players[i] != NULL)
		{
			if (players[i]->get_sprite() == p)
			{
				p_player = players[i];
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

	DEBUG_LOG("Cnew_tug::player userid:%u ",p->id);

	ant::pack(pkg, (uint8_t)size, l);
	for (int j = 0; j < size; j++)
	{
		uint8_t id = p_player->get_card_id(j);
		ant::pack(pkg, id, l);
	}

	init_proto_head(pkg, NEW_TUG_GET_CARDS, l);
	if (send_to_self(p, pkg, l, completed) != 0)
	{
		ERROR_LOG("Send cards array error");
		return 0;
	}

	DEBUG_LOG("userid:%u Send cards array leave ", p->id);

	return 0;

}


int Cnew_tug::set_player(Cplayer* p_player)
{
    DEBUG_LOG("set_player [%d %d %d]", players.size(), p_player, p_player->id());
	players.push_back(p_player);
	return 0;
}

void Cnew_tug::notify_act_card(uint32_t userid, uint32_t type, uint32_t pos)
{
	int len = sizeof (protocol_t);
    ant::pack(pkg, userid, len);
    ant::pack(pkg, uint8_t(type), len);
    ant::pack(pkg, uint8_t(pos), len);
	init_proto_head(pkg, NEW_TUG_ACT_CARD, len);
    DEBUG_LOG("%lu ACT CARD [%d]", m_grp->id, userid);
    send_to_players(m_grp, pkg, len);
}

int  Cnew_tug::notify_client_ready()
{
	int len = sizeof (protocol_t);
	uint8_t count = players.size();
	ant::pack(pkg, count, len);
	for (uint32_t i = 0; i < players.size(); i++)
	{
		if (players[i] != NULL)
		{
			uint32_t userid = players[i]->id();
			ant::pack(pkg, userid, len);
		}
	}

	init_proto_head(pkg, NEW_TUG_CLIENT_READY, len);
    DEBUG_LOG("%lu CLIENT READY", m_grp->id);
    send_to_players(m_grp, pkg, len);

	return 0;

}


int Cnew_tug::notify_game_result()
{
	int len = sizeof (protocol_t);

	uint8_t count = 0;
	for (uint32_t i = 0; i < players.size(); i++)
	{
		if (players[i] != NULL)
		{
			count = count + 1;
		}
	}
	ant::pack(pkg, (uint8_t)count, len);

	for (uint32_t i = 0; i < players.size(); i++)
	{
		if (players[i] != NULL)
		{
			uint32_t id = players[i]->id();
			int medal = players[i]->get_medal();
			int exp = players[i]->get_exp();
			int coins = players[i]->get_xiaomee();
			ant::pack(pkg, (uint32_t)id, len);
			ant::pack(pkg, (int)exp, len);
			ant::pack(pkg, (int)coins, len);
			ant::pack(pkg, (int)medal, len);
			DEBUG_LOG("%lu GAME RESULT  userid:%u, exp:%d, medal:%d", m_grp->id, id, exp, medal);
		}
	}

	init_proto_head(pkg, NEW_TUG_GAME_RESULT, len);
    DEBUG_LOG("%lu GAME RESULT ", m_grp->id);
    send_to_players(m_grp, pkg, len);

	return 0;
}

int Cnew_tug::update_db_players_info( )
{
	for (uint32_t i = 0; i < players.size(); i++)
	{
		if (players[i] != NULL)
		{
			int exp = 0;
			int coins = 0;
			int medal = 0;
			int game_time = time(NULL) - time_start;

			int count = 6 - players[i]->get_life_value();
			if (count >= 0 || count <= 6)
			{
				exp = score_info_7[count].exp;
				coins = score_info_7[count].coins;
				medal = score_info_7[count].medal;
			}

			struct score_info time_score_info = { };
			get_score_info(game_time, time_score_info);
			exp = exp + time_score_info.exp;
			coins = coins + time_score_info.coins;
			medal = medal + time_score_info.medal;


			sprite_t * p = NULL;
			p =	players[i]->get_sprite();
			players[i]->set_exp(exp);
			players[i]->set_xiaomee(coins);
			players[i]->set_medal(medal);
			if (p != NULL)
			{
				update_db_player_info(p, exp, coins, medal);
				send_cnt++;
			}
		}
	}

	return 0;

}


/**
 * @brief 向数据库提交竞赛模式玩家获取经验值
 * @param sprite_t *p 玩家信息
 * @param int exp 对应玩家经验
 * @return
 */
int Cnew_tug::update_db_player_info(sprite_t *p, int exp_in, int xiaomee_in, int medal_in)
{
	score_info s_info = { };
	s_info.exp = exp_in;
	s_info.coins = xiaomee_in;
	s_info.medal = medal_in;
	p->waitcmd = proto_new_card_add_exp;
	DEBUG_LOG("Cnew_tug::update_db_player_info userid:%u, exp:%d, xiaomee:%d, medal:%d, waitcmd:%u", p->id, exp_in, xiaomee_in, medal_in,  p->waitcmd);
	return send_request_to_db(db_new_card_add_exp, p, sizeof(s_info), &s_info, p->id);
}


int Cnew_tug::update_db_player_info_callback(sprite_t *p, uint32_t ret_code)
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
		p_player->set_exp(0);
		p_player->set_xiaomee(0);
		p_player->set_medal(0);
	}

	recv_cnt++;
	DEBUG_LOG("%lu\tCnew_tug:: handle db return return_cnt: %d", m_grp->id, recv_cnt);
    if (recv_cnt == send_cnt)
	{
		notify_game_result();
		send_cnt = 0;
		recv_cnt = 0;
        return GER_end_of_game;
	}

	return 0;

}


/**
 * @brief  竞赛模式提交胜负后，处理数据库返回结果
 * @param
 * @return GER_end_of_game 两个玩家都有返回或者错误才能结束
 * @return 0			   两个玩家没有都返回
 */
int Cnew_tug::handle_db_return(sprite_t *p, uint32_t id, const void* buf, int len, uint32_t ret_code)
{
	DEBUG_LOG("%lu\tCnew_card:handle db return %d,%d,ret %d", m_grp->id, p->id, p->waitcmd, ret_code);

	switch(p->waitcmd)
	{
        case proto_new_card_add_exp:
			{
				return  update_db_player_info_callback(p, ret_code);
			}
		default:
			{
				ERROR_LOG("%lu\tCnew_card:: handle db return cmd undef %d", m_grp->id, p->waitcmd);

			}
	}

	return 0;
}

int Cnew_tug::on_player_leave(sprite_t* p)
{
 	for (uint32_t i = 0; i < players.size(); i++)
    {
        if (players[i] != NULL)
        {
            if(players[i]->get_sprite() == p)
            {
                players[i]->set_sprite(NULL);

                player_cards_info_t* p_cards_info =  players[i]->get_cards_info();
                delete p_cards_info;
                p_cards_info = NULL;
                delete players[i];
                players[i] = NULL;

                DEBUG_LOG("%lu player leave game %d", p->group->id, p->id);
            }
        }
    }

	process_game_result();

	return  0;

}

bool Cnew_tug::is_game_ready()
{
	for (uint32_t i = 0; i < players.size(); i++)
	{
		if (players[i] != NULL)
		{
			if (players[i]->get_game_status() != GAME_READY)
			{
				return false;
			}
		}
	}

	return true;

}

int Cnew_tug::get_score_info(int time, score_info_t& time_score_info)
{
	if (time > 56)
	{
		time_score_info.exp = 200;
		time_score_info.coins = 200;
		time_score_info.medal = 3;
	}
	else if (time > 51)
	{
		time_score_info.exp = 150;
		time_score_info.coins = 150;
		time_score_info.medal = 2;
	}
	else if (time > 46)
	{
		time_score_info.exp = 100;
		time_score_info.coins = 100;
		time_score_info.medal = 1;
	}
	else if (time > 36)
	{
		time_score_info.exp = 50;
		time_score_info.coins = 50;
		time_score_info.medal = 0;
	}
	else if (time > 21)
	{
		time_score_info.exp = 20;
		time_score_info.coins = 20;
		time_score_info.medal = 0;
	}
	else
	{
		time_score_info.exp = 0;
		time_score_info.coins = 0;
		time_score_info.medal = 0;
	}

	return 0;

}

int  Cnew_tug::handle_timeout(void* data)
{
	update_db_players_info();

	return 0;

}


