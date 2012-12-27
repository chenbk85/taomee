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
#include "card.hpp"


/**
 * @brief 卡牌类构造函数
 * @param  玩家所在用户组
 * @return 永远不会返回
 */
Card::Card (game_group_t *grp): player_num(-1), final_winner(-1), owner(-1),event_owner(0),event_now(0),return_cnt(0), m_grp(grp)
{
	player[0] = NULL;
	player[1] = NULL;
	if ( CHALLENGE_GAME (m_grp->game)) {
		player_num 		 = single_player;
		player[computer] = new player_game_info_t;
		player[computer]->player 	  	= NULL;
		player[computer]->p_cards_info 	= NULL;

		int can_win = (int)(2.0 * rand()/(RAND_MAX + 1.0));
		if (can_win == 1) {
			final_winner = player1;
		}
	} else {
		player_num = multi_players;
	}
	//DEBUG_LOG("%lu\tCARD CONSTRUCT", grp->id);

	//register event handler
	event_impl[0] = &Card::event_none;
	event_impl[1] = &Card::self_add_one;
	event_impl[2] = &Card::rival_minus_one;
	event_impl[3] = &Card::self_add_three;
	event_impl[4] = &Card::rival_minus_three;
	event_impl[5] = &Card::self_add_five;
	event_impl[6] = &Card::rival_minus_five;
	event_impl[7] = &Card::self_max;
	event_impl[8] = &Card::rival_min;
	event_impl[9] = &Card::rival_wood_ban;
	event_impl[10] = &Card::rival_fire_ban;
	event_impl[11] = &Card::rival_water_ban;
	event_impl[12] = &Card::exchange;
	event_impl[13] = &Card::rival_turn_wood;
	event_impl[14] = &Card::rival_turn_fire;
	event_impl[15] = &Card::rival_turn_water;
	event_impl[16] = &Card::rival_lose;
}

int
Card::result[CARD_TYPE_MAX][CARD_TYPE_MAX] =
{
	//水，	火， 木
	//player1 win, result[player1][player2]=win
	{draw,	  win,  lost,},
	{lost,   draw,   win,},
	{ win,   lost,  draw,},
};

/**
 * @brief  处理玩家用户游戏通讯
 * @param
 * @return 返回GER_end_of_game结束游戏,返回0游戏继续
 */
int
Card::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
    if (player[0] && player[1]) {
        if ( p == player[0]->player) {
            owner = player1;
        } else if (player_num == multi_players) {
            if (p == player[1]->player) {
                owner = player2;
            }
        }
    }

    switch (cmd)
    {
        case card_game_start:
            if (!CHALLENGE_GAME(m_grp->game)) {
                ADD_TIMER_EVENT(p, on_timer_expire, 0, now.tv_sec + 120);
            }
            return send_cards_array();
        case proto_player_action:
            CHECK_BODY_LEN(len, 1);
            MOD_EVENT_EXPIRE_TIME(p, on_timer_expire, now.tv_sec + 120);
            set_me_ready(player[owner]);
            return processing(p, (int)body[0]);
        case proto_player_leave:
        case card_player_quit:
            //DEBUG_LOG("%lu\t%d LEAVE GAME %d", m_grp->id, p->id, cmd);
            //中途离开
            return GER_player_request;
        case card_rival_ready:
            return notify_rival_ready();
        case card_single_over:
            {
                if (player[player1]->rank == 9 && final_winner == player1) {
                    get_item (p, card_spec_item);
                    DEBUG_LOG("%lu REQUEST CHANGQIANG %d", m_grp->id, p->id);
                    return 0;
                }
            }
            return GER_end_of_game;
        default:
            ERROR_LOG("%lu\tCard, undef cmd: %d", m_grp->id, cmd);
            return GER_end_of_game;
    }
	//game move on
	return 0;
}

/**
 * @brief 玩家出牌之后，处理游戏逻辑
 * @param  sprite_t *p 出牌的玩家
 * @param  int positio 出的是第几张牌
 * @return GER_end_of_game 结束游戏
 * @return 0		   游戏继续
 */
int
Card::processing (sprite_t *p, int position)
{
	if (position < 0 || position > 4) {
		ERROR_LOG("%lu\t%dPOSITION ERROR: %d",m_grp->id, p->id, position);
		return GER_end_of_game;
	}

	card[owner] = (*cards_info)[cards_id_array[owner][position]];
	bout_info[owner] = card[owner] ;

	//card array move on
	card_t temp = (*cards_info)[cards_id_array[owner][position]];
	cards_id_array[owner][position] = cards_id_array[owner][5];
	cards_id_array[owner].erase(cards_id_array[owner].begin() + 5);
	cards_id_array[owner].push_back(temp.id);


	//client decide computer's action
	if (!is_all_ready()) {
		notify_rival_done();
		return 0;
	} else {
		//two players ready
		int bout_winner;
		//event handle
		int ev_ret = (this->*event_impl[event_now])(card, event_owner);
		if (ev_ret == -1) {
			//it's bot ,end this game
			ERROR_LOG("%lu\tIt's BOT, %d eventid %d event_owner %d cardid %d", m_grp->id, p->id, event_now, event_owner, card->id);
			return GER_invalid_data;
		} else if (ev_ret == 1) {
			//rival_lose event
			bout_winner = event_owner;
		} else {
			bout_winner = referee(card[player1], card[player2]);
		}

		//somebody win this round
		if (bout_winner != draw) {
			DEBUG_LOG("%lu\tBOUT_WINNER %d", m_grp->id, player[bout_winner]->player->id);
			notify_bout_result(bout_winner);
			final_winner = figure_final_winner(bout_info[bout_winner], bout_winner);

			// final winner appear
			if ((final_winner != -1)) {
				DEBUG_LOG("%lu\tFINAL_WINNER %d", m_grp->id, player[final_winner]->player->id);
				if (COMPETE_MODE_GAME(m_grp->game)) {
			//		notify_final_result(final_winner);
					for (int i = 0; i < 2; i++) {
						//I am winner
						if(i == final_winner) {
							player[i]->p_cards_info->win_cnt++;
                            if (player[i]->rank != 9) {
                                if(update_redclothes(player[i]->player)) {
                                    update_db_player_info(player[i]->player, 1);
                                }
                            } else {
                                update_db_player_info(player[i]->player, 1);
                            }
                        //I am loser
						} else {
							player[i]->p_cards_info->lost_cnt++;
							update_db_player_info(player[i]->player, 0);
						}
					}
					//need to wait db return ,can not end this game.
					return 0;
				} else {
					//free mode, do not update db
                    game_score_t win_score;
                    game_score_t lose_score;
                    memset(&win_score, 0, sizeof(win_score));
                    memset(&lose_score, 0, sizeof(lose_score));
                    win_score.coins = m_grp->game->yxb;
                    lose_score.coins = m_grp->game->yxb/5;
					notify_final_result(final_winner);
                    if((rand()%100) == 1) {
                        win_score.itmid = 12635;
                        win_score.itm_max = 1;
                        win_score.itmkind = 0;
                    }
                    submit_game_score(player[final_winner]->player, &win_score);
                    submit_game_score(player[!final_winner]->player, &lose_score);
					return GER_end_of_game;
				}
			}

		} else {
			//draw
			DEBUG_LOG("%lu\tBOUT DRAW", m_grp->id);
			notify_bout_result(bout_winner);
		}

		//decide next event
		int event1,event2;
		event1 = bout_info[0].event;
		event2 = bout_info[1].event;
		if (bout_winner == draw) {
			event_owner = 0;
			event_now 	= 0;
		} else {
			if (event1 && event2) {
				if (bout_info[0].value == bout_info[1].value) {
					event_owner = bout_winner;
					event_now 	= bout_info[bout_winner].event;
				} else if (bout_info[0].value > bout_info[1].value) {
					event_owner = player1;
					event_now 	= event1;
				} else {
					event_owner = player2;
					event_now 	= event2;
				}
			} else if (event1) {
				event_now 	= event1;
				event_owner = player1;
			} else if (event2) {
				event_now 	= event2;
				event_owner = player2;
			} else {
				//everybody does not have event;
				event_owner = 0;
				event_now = 0;
			}
		}
//		DEBUG_LOG("after event_owner %d, event_now %d", event_owner, event_now);
		reset_player_status();
	}
	return 0;
}

int
Card::get_items_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret)
{
    switch (ret) {
        case 0:
            {
                int i;
                int cnt = *(p->session + 1);
                int l = sizeof(protocol_t);
                ant::pack(pkg, (uint8_t)*(p->session), l);
                DEBUG_LOG("%lu GET ITEMS FLAG %d %d", m_grp->id, p->id, *(p->session));
                if(*(p->session)) {
#ifndef TW_VER
                            uint32_t msg[2];
                            msg[0] = 1;
                            msg[1] = p->id;
                            int msglog_type = 0x02080000 + rank_clothe[9];
                            int msgret = msglog(statistic_file, msglog_type, get_now_tv()->tv_sec, &msg, sizeof (msg));
                            if (msgret != 0) {
                                ERROR_LOG( "statistic log error: message type(%x)",msglog_type);
                            }
#endif
                }
                ant::pack(pkg, (uint8_t)cnt, l);
                for (i = 0; i < cnt; i++) {
                    uint32_t item_id = *(uint32_t*)(p->session + 4*i + 2);
                    ant::pack(pkg, item_id, l);
                    DEBUG_LOG("%lu GET ITEMS CALLBACK %d %d", m_grp->id, p->id, item_id);
                }
                init_proto_head(pkg, proto_get_items, l);
                if (send_to_self(p, pkg, l, 1) != 0) {
                    DEBUG_LOG("send get_items_callback error");
                }
                break;
            }
        case 1114:
            {
                int i;
                int cnt = *(p->session + 1);
                for (i = 0; i < cnt; i++) {
                    uint32_t item_id = *(uint32_t*)(p->session + 4*i + 2);
                    DEBUG_LOG("%lu ALREADY HAS ITEM %d %d", m_grp->id, p->id, item_id);
                }
                break;
            }
        default:
            DEBUG_LOG("get items callback ret %d", ret);
    }
    if (p->group == 0) {
        DEBUG_LOG("GET_ITEMS_CALLBACK delete player %d", p->id);
        del_sprite_conn(p);
    }
    return 0;
}


/**
 * @brief  竞赛模式提交胜负后，处理数据库返回结果
 * @param
 * @return GER_end_of_game 两个玩家都有返回或者错误才能结束
 * @return 0			   两个玩家没有都返回
 */
int
Card::handle_db_return(sprite_t *p, uint32_t id, const void* buf, int len, uint32_t ret_code)
{
	if (player[0] && player[1]) {
		if ( p == player[0]->player) {
			owner = player1;
		} else if (player_num == multi_players) {
			if (p == player[1]->player) {
				owner = player2;
			}
        }
    }
	DEBUG_LOG("%lu\tcard:handle db return %d,%d,ret %d", m_grp->id, p->id, p->waitcmd, ret_code);
	switch(p->waitcmd) {
        case proto_get_items:
            get_items_callback(p, id, buf, len, ret_code);
            break;
        case proto_set_redclothes:
            if(!ret_code) {
				get_all_item(p, player[owner]->rank);
                return 0;
            }
            update_db_player_info(p, 1);
            return 0;
        case proto_card_add_win_lost:
            switch (ret_code) {
                case 0:
                    {
                        int rank_new = calculate_rank(player[owner]->p_cards_info);
                        if (rank_new > player[owner]->rank) {
                            get_item(p, rank_clothe[rank_new]);
#ifndef TW_VER
                            uint32_t msg[2];
                            msg[0] = 1;
                            msg[1] = p->id;
                            int msglog_type = 0x02080000 + rank_clothe[rank_new];
                            int msgret = msglog(statistic_file, msglog_type, get_now_tv()->tv_sec, &msg, sizeof (msg));
                            if (msgret != 0) {
                                ERROR_LOG( "statistic log error: message type(%x)",msglog_type);
                            }
#endif
                           //notify_get_item(p, rank_clothe[rank_new]);
                            DEBUG_LOG("%lu\t%d RANK %d UP TO %d", m_grp->id, p->id, player[owner]->rank, rank_new);
                            return 0;
                        }
                    }
                    break;
                case 1147:
                    //experience limited
                    notify_exp_limited();
                    break;
                default:
                    ERROR_LOG("%lu\tcard:handle db return UNDEF %d, ret %d", m_grp->id, p->id, ret_code);
            }
            break;
		default:
			ERROR_LOG("%lu\tCard:: handle db return cmd UNDEF %d", m_grp->id, p->waitcmd);
	}
	if( final_winner == -1 ) {
		ERROR_LOG( "invalid final_winner ID" );
		return GER_end_of_game;
	}
    game_score_t score;
    memset(&score, 0, sizeof(score));
    if(player_num == multi_players) {
        if(p == player[final_winner]->player) {
            if(ADVANCE_GAME(p->group->game)) {
                // DEBUG_LOG("advance compete game");
                static int medal[5] = { 12632, 12629, 12633, 12631, 12630};
                if((rand()%100) < 50) {
                    score.itmid = medal[p->itemid - 9];
                    score.itm_max = 1;
                    score.itmkind = 0;
                    DEBUG_LOG("%lu\tGET ADVANCE KNIGHT ITEM [%d %d]", m_grp->id, p->id, score.itmid);
                }
            }
            score.coins = m_grp->game->yxb;
        } else {
            score.coins = m_grp->game->yxb/5;
        }
    }
    submit_game_score(p, &score);
    return_cnt++;
    //get all return from db, end this game
    if (return_cnt == player_num) {
        notify_final_result(final_winner);
        return GER_end_of_game;
	}
	return 0;
}

/**
 * @brief 通知对方已经出牌
 * @param
 * @return
 */
int
Card::notify_rival_done()
{
	int l = sizeof (protocol_t);
	init_proto_head(pkg, card_rival_done, l);
	if (send_to_self (player[!owner]->player, pkg, l, completed) != 0) {
		ERROR_LOG("Send rival done error");
		return -1;
	}
	return 0;
}

/**
 * @brief  通知玩家升级了或者获得骑士长枪
 * @param
 * @return
 */
int
Card::notify_get_item(sprite_t *p, uint32_t item_id)
{
	int l = sizeof (protocol_t);
	uint8_t count = 1;
	ant::pack(pkg, count, l);
	ant::pack(pkg, item_id, l);
	init_proto_head(pkg, proto_get_items, l);
	if (send_to_self (p, pkg, l, completed) != 0) {
		ERROR_LOG("Send rival done error");
		return -1;
	}
	return 0;
}

/**
 * @brief  通知玩家竞赛模式获得的经验值超过了每天的限制
 * @param
 * @return
 */
int
Card::notify_exp_limited()
{
	DEBUG_LOG("%lu\tEXP LIMITED %d", m_grp->id, player[owner]->player->id);
	int l = sizeof(protocol_t);
	init_proto_head(pkg, card_exp_limited, l);
	if (send_to_self (player[owner]->player, pkg, l, completed) != 0) {
		ERROR_LOG("Send exp limited error");
		return -1;
	}
	return 0;
}

/**
 * @brief 通知对方动画播放完毕，协调双方出牌时间
 * @param
 * @return
 */
int
Card::notify_rival_ready()
{
	int l = sizeof(protocol_t);
	init_proto_head(pkg, card_rival_ready, l);
	if (send_to_self (player[!owner]->player, pkg, l, completed) != 0) {
		ERROR_LOG("Send rival ready error");
		return -1;
	}
	return 0;
}

/**
 * @brief 通知本回合胜负
 * @param  int bout_winner, 玩家1胜利为player1,玩家2胜利为player2
 * @return
 */
int
Card::notify_bout_result(int bout_winner)
{
	for (int i = 0; i < 2; i++) {
		int l = sizeof (protocol_t);
		if (bout_winner != -1) {
			if (bout_winner == draw) {
				ant::pack(pkg, 0, l);
			} else {
				ant::pack(pkg, player[bout_winner]->player->id, l);
			}
			//flash action script need the type plus 1
			ant::pack(pkg, (uint8_t)(bout_info[i].type + 1), l);
			ant::pack(pkg, bout_info[i].value, l);
			ant::pack(pkg, bout_info[i].event, l);
			ant::pack(pkg, bout_info[i].colour, l);
			ant::pack(pkg, bout_info[i].star, l);
		}
		init_proto_head(pkg, card_bout_result, l);
		if (send_to_self (player[!i]->player, pkg, l, completed) != 0) {
			ERROR_LOG("Send cards array error");
			return -1;
		} else {
		//	DEBUG_LOG("send bout_winner to %d", player[!i]->player->id);
		}
	}
	return 0;
}

/**
 * @brief  通知游戏最后胜负结果
 * @param  int final_winner, 玩家1胜利为player1,玩家2胜利为player2
 * @return
 */
int
Card::notify_final_result(int final_winner)
{
	for (int i = 0; i < 2; i++) {
		int l = sizeof (protocol_t);
		ant::pack(pkg, player[final_winner]->player->id, l);

		init_proto_head(pkg, card_final_result, l);
		if (send_to_self (player[i]->player, pkg, l, uncompleted) != 0) {
			ERROR_LOG("Send final result error");
			return -1;
		}
	}
	return 0;
}

int
Card::update_redclothes(sprite_t *p)
{
#ifndef TW_VER
    if(rand()%100 > 5) {
        return 1;
    }
#else
    return 1;
#endif
    uint32_t msg[2];
    msg[0] = 1;
    msg[1] = 1;
    //DEBUG_LOG("SEND SET UPDATE [%d]", p->id);
    p->waitcmd = proto_set_redclothes;
	return send_request_to_db(db_card_up_redclothes, p, sizeof(msg), msg, p->id);
}

/**
 * @brief 向数据库提交竞赛模式玩家胜负
 * @param sprite_t *p 玩家信息
 * @param int result 对应玩家胜负
 * @return
 */
int
Card::update_db_player_info(sprite_t *p, int result)
{
	int res = result;
	p->waitcmd = proto_card_add_win_lost;
//	DEBUG_LOG("update db player info  %d, %d", p->id,p->waitcmd);
	return send_request_to_db(db_card_add_win_lost, p, sizeof(res), &res, p->id);
}

/**
 * @brief 在每回合有玩家胜利之后计算该玩家是否获得最终胜利
 * @param card_t win_card, 该回合此玩家致胜的卡牌信息
 * @param int winner,   胜利者是player1 还是player2
 * @return 0            player1或者最终胜利
 * @return 1            player2或者最终胜利
 * @return -1           没有产生最终赢家
 */
int
Card::figure_final_winner(card_t win_card, int winner)
{
	int type = win_card.type;
	int colour = win_card.colour;
	int count = scores[winner][type].cnt;
	//DEBUG_LOG("scores,%d, type %d, colour %d", winner, type, colour);

	//保存各个属性有多少个不同的颜色
	if (count == 0) {
		scores[winner][type].colour[count] = colour;
		scores[winner][type].cnt++;

	}
	if (count == 1) {
		if (colour != scores[winner][type].colour[0]){
			scores[winner][type].colour[count] = colour;
			scores[winner][type].cnt++;
		}
	}
	if (count == 2) {
		if (colour != scores[winner][type].colour[0]
				&&colour != scores[winner][type].colour[1]){
			scores[winner][type].colour[count] = colour;
			scores[winner][type].cnt++;
		}
	}
	//相同属性有3张不同颜色的卡牌
	for (int player = 0; player < 2; player ++) {
		for (int i = 0; i < 3; i++) {
			if (scores[player][i].cnt == 3) {
				return player;
			}
		}
		int cnt[3];
		cnt[0] = scores[player][0].cnt;
		cnt[1] = scores[player][1].cnt;
		cnt[2] = scores[player][2].cnt;
		//3张不同属性胜利卡牌，且颜色不同
		if (cnt[0] && cnt[1] && cnt[2]) {
			for (int i = 0; i < cnt[0]; i++ ) {
				for (int j = 0; j < cnt[1]; j ++) {
					for (int k = 0; k < cnt[2]; k++) {
						int colours[3];
						colours[0] = scores[player][0].colour[i];
						colours[1] = scores[player][1].colour[j];
						colours[2] = scores[player][2].colour[k];
						if (colours[0] != colours[1]
								&&colours[0] != colours[2]
								&&colours[1] != colours[2]) {
							return player;
						}

					}
				}
			}
		}
	}

	//game is not end
	return -1;

}

/**
 * @brief  裁决事件处理后双方卡牌胜负
 * @param  card_t p1 要裁决的卡牌1
 * @param  card_t p2 要裁决的卡牌2
 * @return 返回卡牌1的胜负
 */
int
Card::referee(card_t p1, card_t p2) {
	int my_result = Card::result[p1.type][p2.type];
	if (my_result != draw) {
		return my_result;
	} else {
		if (p1.value == p2.value) {
			return draw;
		}
		if (p1.value > p2.value) {
			return win;
		}
		if (p1.value < p2.value) {
			return lost;
		}
	}
	return -1;
}

/**
 * @brief 设置进行本次游戏的玩家卡牌信息,设置完成后产生双方卡牌队列各12张
 * @param player_game_info_t *p1 玩家1
 * @param player_game_info_t *p2 玩家2
 * @param card_t (*all_cards)[CARD_ID_MAX] 全局的标准卡牌信息
 * @return
 */
int
Card::setup(player_game_info_t *p1, player_game_info_t *p2, card_t (*all_cards)[CARD_ID_MAX])
{
	if (all_cards != NULL) {
		cards_info = all_cards;
	}
	if ( p1 != NULL) {
		player[0] = p1;
		DEBUG_LOG("%lu\tSETUP P1 %d", m_grp->id, p1->player->id);
	}
	if ( p2 != NULL) {
		player[1] = p2;
		DEBUG_LOG("%lu\tSETUP P2 %d", m_grp->id, p2->player->id);
	}
	if (player[0] && player[1]) {
		return create_cards_array();
	}
	return 0;
}

/**
 * @brief 产生双方进行本次游戏的卡牌队列
 * @param
 * @return 0 正常
 * @return -1 玩家卡牌库里有某个属性卡牌数量不足4张
 */
int
Card::create_cards_array()
{
	srand(time(NULL));
	for (int x = 0; x < 2; x++) {
		std::vector<int> cards[3];
		//setup computer ricky's cards
		if (x == computer && player_num == single_player) {
			cards[0].push_back(2);
			cards[0].push_back(7);
			cards[0].push_back(9);
			cards[0].push_back(16);
			cards[1].push_back(23);
			cards[1].push_back(26);
			cards[1].push_back(28);
			cards[1].push_back(39);
			cards[2].push_back(41);
			cards[2].push_back(45);
			cards[2].push_back(46);
			cards[2].push_back(53);
		} else {
		//sort player's card by type
			for (int i=0; i< (int)player[x]->p_cards_info->total_cnt; i++) {
				int id = player[x]->p_cards_info->card_id[i];
				int type = (*cards_info)[id].type;
				cards[type].push_back(id);
			}
		}
		for (int i = 0; i < 3; i++) {
			if (cards[i].size() < 4) {
				ERROR_LOG("error player card init");
				return -1;
			}
		}
		for (int i = 0; i <3; i++) {
			random_shuffle(cards[i].begin(), cards[i].end());
		}
		/*
		for (int i = 0; i <3 ;i ++) {
			int size = cards[i].size();
			for (int j = 0; j< size; j++) {
				DEBUG_LOG("cards %d", cards[i][j]);
			}
		}
		*/
		for (int i =0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				int id = cards[i][j];
				cards_id_array[x].push_back(id);
			}
		}
		random_shuffle(cards_id_array[x].begin(), cards_id_array[x].end());
	}
	return 0;
}

/**
 * @brief 给准备好的玩家发送本次游戏的卡牌队列
 * @param
 * @return
 */
int
Card::send_cards_array()
{
	int l = sizeof (protocol_t);
	uint8_t flag= game_cards_flag_default;
	if (player_num == single_player) {
		//rank 9 have 1/3 chance to defeat ricky
		if (player[player1]->rank >= 9 &&final_winner == player1) {
			flag = single_win;
		} else {
			flag = single_lost;
		}
	}
	ant::pack(pkg, flag, l);
	for (int i = 0; i < 2; i++) {
		int size = cards_id_array[i].size();
		if ( i == computer && player_num == single_player) {
			uint32_t computer_id = 10000;
			uint8_t computer_rank = 9;
			ant::pack(pkg, computer_id, l);
			ant::pack(pkg, computer_rank , l);
		} else {
			ant::pack(pkg, player[i]->player->id, l);
			uint8_t p_rank = player[i]->rank;
			ant::pack(pkg, p_rank, l);
		}
		ant::pack(pkg, (uint8_t)size, l);
		for (int j = 0; j< size; j++) {
			ant::pack(pkg, (uint8_t) ((*cards_info)[cards_id_array[i][j]].type + 1), l);
			ant::pack(pkg, (*cards_info)[cards_id_array[i][j]].value, l);
			ant::pack(pkg, (*cards_info)[cards_id_array[i][j]].event, l);
			ant::pack(pkg, (*cards_info)[cards_id_array[i][j]].colour, l);
			ant::pack(pkg, (*cards_info)[cards_id_array[i][j]].star, l);
		}
	}
	init_proto_head(pkg, card_game_start, l);
	if (send_to_self (player[owner]->player, pkg, l, completed) != 0) {
		ERROR_LOG("Send cards array error");
		return -1;
	}
	return 0;
}

/**
 * @brief 玩家准备好了
 * @param
 * @return
 */
inline void
Card::set_me_ready(player_game_info_t *player)
{
	player->status = turn_start;
}

/**
 * @brief  每个回合结束之后重置玩家状态
 * @param
 * @return
 */
inline void
Card::reset_player_status()
{
	player[0]->status = turn_ready;
	player[1]->status = turn_ready;
}

/**
 * @brief  判断是否双方都准备好了
 * @param
 * @return true 准备好了
 * @return false  没有准备好
 */
inline bool
Card::is_all_ready()
{
	if (player_num == single_player) {
		if (player[0]->status == turn_start) {
			return true;
		} else {
			return false;
		}
	}
	if (player_num == multi_players) {
		if ((player[0]->status == turn_start) && (player[1]->status == turn_start)) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

/**
 * @brief 升级后请求高等级的训练服以及挑战成功后的骑士长枪
 * @param  sprite_t *p 给谁请求
 * @param  int item_id 拿什么
 * @return
 */
int
Card::get_item(sprite_t *p, int item_id)
{
	game_items_t items;
    items.flag = 0;
	items.newcount = 1;
	items.itm_id[0] = item_id;
	return db_get_items(p, &items);
}

int
Card::get_all_item(sprite_t *p, int rank)
{
    DEBUG_LOG("%lu GET ALL ITEMS [%d]", m_grp->id, p->id);
	game_items_t items;
    items.flag = 1;
	items.newcount = (9 - rank);
    for(int i =0; i < (9 - rank); i++) {
        items.itm_id[i] = rank_clothe[rank + i + 1];
    }
	return db_get_items(p, &items);
}
/**
 * @brief 计算玩家等级
 * @param  player_cards_info_t *p_info
 * @return 返回等级0－9
 */
inline int
Card::calculate_rank(player_cards_info_t *p_info)
{
    if(p_info->flag&0x1) {
        return 9;
    }
	int scores = WIN_BONUS * (p_info->win_cnt) + LOST_BONUS*(p_info->lost_cnt);
	//DEBUG_LOG("scores %d win_cnt %d lost_cnt %d", scores, p_info->win_cnt, p_info->lost_cnt);
	if (scores < rank1_exp) {
		return 0;
	} else if (scores < rank2_exp) {
		return 1;
	} else if (scores < rank3_exp) {
		return 2;
	} else if (scores < rank4_exp) {
		return 3;
	} else if (scores < rank5_exp) {
		return 4;
	} else if (scores < rank6_exp) {
		return 5;
	} else if (scores < rank7_exp) {
		return 6;
	} else if (scores < rank8_exp) {
		return 7;
	} else if (scores < rank9_exp) {
		return 8;
	} else {
		return 9;
	}
}

/**
 * @brief 普通牌，没有事件
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::event_none(card_t card[2],int owner)
{
	return 0;
}

/**
 * @brief  自己卡牌数值加1
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::self_add_one(card_t card[2],int owner)
{
	card[owner].value += 1;
	if (card[owner].value >= CARD_VALUE_MAX) {
		card[owner].value = CARD_VALUE_MAX;
	}
	return 0;
}

/**
 * @brief  自己卡牌加3
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::self_add_three(card_t card[2],int owner)
{
	card[owner].value += 3;
	if (card[owner].value >= CARD_VALUE_MAX) {
		card[owner].value = CARD_VALUE_MAX;
	}
	return 0;
}

/**
 * @brief  自己卡牌加5
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::self_add_five(card_t card[2],int owner)
{
	card[owner].value += 5;
	if (card[owner].value >= CARD_VALUE_MAX) {
		card[owner].value = CARD_VALUE_MAX;
	}
	return 0;
}

/**
 * @brief  自己卡牌数值最大化为20
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::self_max(card_t card[2], int owner)
{
	card[owner].value = CARD_VALUE_MAX;
	return 0;
}

/**
 * @brief  对方卡牌数值最小化为1
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::rival_min(card_t card[2], int owner)
{
	card[!owner].value = CARD_VALUE_MIN;
	return 0;
}

/**
 * @brief 对方数值减1
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::rival_minus_one(card_t card[2],int owner)
{
	card[!owner].value -= 1;
	if (card[!owner].value <= CARD_VALUE_MIN) {
		card[!owner].value = CARD_VALUE_MIN;
	}
	return 0;
}

/**
 * @brief 对方数值减3
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::rival_minus_three(card_t card[2],int owner)
{
	card[!owner].value -= 3;
	if (card[!owner].value <= CARD_VALUE_MIN) {
		card[!owner].value = CARD_VALUE_MIN;
	}
	return 0;
}

/**
 * @brief 对方数值减5
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::rival_minus_five(card_t card[2],int owner)
{
	card[!owner].value -= 5;
	if (card[!owner].value <= CARD_VALUE_MIN) {
		card[!owner].value = CARD_VALUE_MIN;
	}
	return 0;
}

/**
 * @brief 木属性屏蔽
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 * @return -1 如果对方仍然发来被屏蔽的卡牌，判为外挂，结束游戏
 */
int
Card::rival_wood_ban(card_t card[2],int owner)
{
	if (card[!owner].type == wood) {
		return -1;
	} else {
		return 0;
	}
}

/**
 * @brief 火属性屏蔽
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 * @return -1 如果对方仍然发来被屏蔽的卡牌，判为外挂，结束游戏
 */
int
Card::rival_fire_ban(card_t card[2],int owner)
{
	if (card[!owner].type == fire) {
		return -1;
	} else {
		return 0;
	}
}

/**
 * @brief 水属性屏蔽
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 * @return -1 如果对方仍然发来被屏蔽的卡牌，判为外挂，结束游戏
 */
int
Card::rival_water_ban(card_t card[2],int owner)
{
	if (card[!owner].type == water) {
		return -1;
	} else {
		return 0;
	}
}

/**
 * @brief  对方卡牌属性变木
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::rival_turn_wood(card_t card[2],int owner)
{
	card[!owner].type = wood;
	return 0;
}

/**
 * @brief  对方卡牌属性变火
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::rival_turn_fire(card_t card[2],int owner)
{
	card[!owner].type = fire;
	return 0;
}

/**
 * @brief  对方卡牌属性变水
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::rival_turn_water(card_t card[2],int owner)
{
	card[!owner].type = water;
	return 0;
}

/**
 * @brief 必杀卡牌
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 1 无视对方卡牌，直接胜利
 */
int
Card::rival_lose(card_t card[2], int owner)
{
	return 1;
}

/**
 * @brief 转置，交换双方卡牌
 * @param card_t card[2] 需要处理的两张牌
 * @param int owner      处理谁的牌
 * @return 0 正常返回
 */
int
Card::exchange(card_t card[2], int owner)
{
	card_t temp 	= card[owner];
	card[owner]  	= card[!owner];
	card[!owner] 	= temp;
	return 0;
}
