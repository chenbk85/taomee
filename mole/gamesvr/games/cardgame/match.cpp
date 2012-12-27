#include <cstdlib>
#include <cstring>

extern "C" {
#include <time.h>
#include <libtaomee/log.h>
#include <libxml/tree.h>
#include <gameserv/dll.h>
#include <gameserv/timer.h>
#include <gameserv/proto.h>
#include <gameserv/config.h>
#include <gameserv/dbproxy.h>
}

#include "match.hpp"

/**
 * @brief 每次玩家进入，先请求用户卡牌库信息
 * @param sprite_t* p 玩家指针
 * @return 无
 */
void
Match::init (sprite_t* p)
{
	p->group->card_count = 0;
	for (int i = 0; i < p->group->count; i++) {
		p->group->players[i]->waitcmd = proto_card_getinfo;
		if(send_request_to_db(db_card_getinfo, p->group->players[i], 0, 0, p->group->players[i]->id) != 0) {
			ERROR_LOG("Get Player Cards info Error ");
		} else {
		//	DEBUG_LOG("request player card info %d", p->group->players[i]->id);
		}
	}
}

/**
 * @brief 用户中途或者意外离开时，竞赛模式退出队列，游戏组的游戏指针置空，防止Match被析构
 * @param
 * @return GER_end_of_game 结束游戏
 */
int
Match::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	switch (cmd) {
		case proto_player_leave:
			//DEBUG_LOG("match: handle_data, player %d leave, %d", p->id, cmd);
			break;
		case card_player_quit:
			//DEBUG_LOG("match: handle_data, player %d quit, %d", p->id, cmd);
			break;
		default:
			//DEBUG_LOG("match: handle_data, player %d undef cmd %d",p->id, cmd);
            break;
	}
	//离开竞赛游戏队列
	if (COMPETE_MODE_GAME(p->group->game)) {
        if(ADVANCE_GAME(p->group->game)) {
            std::deque<player_game_info_t*>::iterator it;
            int position = -1;
            bool is_break = false;
            for (int i = 0; i < 10; i++) {
                for (it = advance_compete[p->itemid - 9].begin(); it != advance_compete[p->itemid - 9].end(); it++) {
                    if((*it)->player == p) {
                        delete (*it)->p_cards_info;
                        delete *it;
                        position = i;
                        advance_compete[p->itemid - 9].erase(it);
                        is_break = true;
                        DEBUG_LOG("%lu ADVANCE COMPETE LEAVE %d", p->group->id, p->id);
                        break;
                    }
                }
                if (is_break) {
                    break;
                }
            }
        }
		std::deque<player_game_info_t*>::iterator it;
		int position = -1;
		bool is_break = false;
		for (int i = 0; i < 10; i++) {
			for (it = competition[i].begin(); it != competition[i].end(); it++) {
				if((*it)->player == p) {
					delete (*it)->p_cards_info;
					delete *it;
					position = i;
					competition[i].erase(it);
					is_break = true;
			        DEBUG_LOG("%lu COMPETE LEAVE %d", p->group->id, p->id);
					break;
				}
			}
			if (is_break) {
				break;
			}
		}
		if ( position == -1) {
			ERROR_LOG("compete mode: player %d leave ,but not found", p->id);
		}

	//离开自由对战队列
	} else if (MULTIPLAYER_GAME(p->group->game)) {
		std::map<game_group_t*, Card*>::iterator it = free_mode.find (p->group);
		if (it != free_mode.end()) {
			DEBUG_LOG("%lu MULTIGAME LEAVE %d", p->group->id, p->id);
			free_mode.erase(it);
		} else {
			ERROR_LOG("%lu MULTIGAME leave, but not found %d", p->group->id, p->id);
		}
	}

	p->group->game_handler = NULL;
	for (int i = 0; i < p->group->count; i++) {
		p->group->players[i]->waitcmd = 0;
	}
	return GER_end_of_game;
}

/**
 * @brief  告诉玩家服务器端已经获得了玩家卡牌库信息，可以开始请求卡牌队列了
 * @param  sprite_t* p 玩家指针
 * @return －1 发送失败
 * @return  0 发送成功
 */
int
Match::notify_server_ready(sprite_t *p)
{
	int l = sizeof (protocol_t);
	init_proto_head(pkg, card_server_ready, l);
	if (send_to_self (p, pkg, l, 1 ) == 0) {
	} else {
		ERROR_LOG("notify server ready error");
		return -1;
	}
	return 0;
}

/**
 * @brief 找不到配对的同等级玩家，超时返回处理函数
 * @param
 * @return
 * @return 0 正常处理结束
 */
int
Match::handle_timeout(void* data)
{
	wait_usr_info *u_info = (wait_usr_info*)data;
	sprite_t *p = u_info->info->player;
	int rank = u_info->rank;
	bool is_match = false;
	sprite_t *rival;
	for (int i = rank - 1; i >= 0; i--) {
		if (competition[i].size() != 0) {
			is_match = true;
			player_game_info_t *rival_info = competition[i].front();
			rival = rival_info->player;
			//DEBUG_LOG("%d find %d at rank %d", p->id, rival->id, i);
			process(p, rival, u_info->info, competition[rank], competition[i]);
			break;
		}
	}
	if (is_match) {
		//clear players tmp info
		wait_usr_info* p_wait_usr_info = NULL;
		std::deque<wait_usr_info*>::iterator it;
		for (it = wait_q[rank-5].begin(); it!= wait_q[rank-5].end(); it++) {
			p_wait_usr_info = *it;
			if(p_wait_usr_info->info->player == p) {
				delete *it;
				wait_q[rank-5].erase(it);
				break;
			}
		}
		for (it = wait_q[rank-5].begin(); it!= wait_q[rank-5].end(); it++) {
			p_wait_usr_info = *it;
			if(p_wait_usr_info->info->player == rival) {
				delete *it;
				wait_q[rank-5].erase(it);
				break;
			}
		}
	} else {
		ADD_TIMER_EVENT(p->group, on_game_timer_expire, u_info, now.tv_sec + 10);
	}
	return 0;
}

/**
 * @brief  处理数据库返回的玩家卡牌库信息
 * @param
 * @return GER_end_of_game 玩家卡牌没有激活时结束游戏
 * @return 0 正常处理结束
 */
int
Match::handle_db_return(sprite_t *p, uint32_t id, const void *buf, int len, uint32_t ret_code)
{
	if(p->waitcmd != proto_card_getinfo)
	{
		ERROR_LOG("cardgame Match::handle_db_return Error %d groupid %ld userid %d len %d", p->waitcmd, 
			p->group->id, p->id, len);
		return GER_end_of_game;
	}

	int total_cnt = *((uint32_t*)buf + 5);	
	CHECK_BODY_LEN(len, 24+total_cnt*4);
	
	player_cards_info_t *my_cards_info = new player_cards_info_t;
	memcpy(my_cards_info, buf, len);
	if (my_cards_info->basic_card_cnt == 0) {
		//this player has never been init;
		ERROR_LOG("%d card not inited", p->id);
		delete my_cards_info;
		p->group->game_handler = NULL;
		for (int i = 0; i < p->group->count; i++) {
			p->group->players[i]->waitcmd = 0;
		}
		return GER_end_of_game;
	}

	DEBUG_LOG("%d rank %d win %d, lost %d", p->id, calculate_rank(my_cards_info), my_cards_info->win_cnt, my_cards_info->lost_cnt);
	player_game_info_t *my_game_info = new player_game_info_t;
	my_game_info->player = p;
	my_game_info->p_cards_info = my_cards_info;
	my_game_info->rank = calculate_rank(my_cards_info);
	int rank = my_game_info->rank;

	//高级竞赛模式
	if(ADVANCE_GAME(p->group->game) && (COMPETE_MODE_GAME(p->group->game))) {
		if(rank != 9 || p->itemid>13 || p->itemid<9) {
			ERROR_LOG("%lu advance card game %d error level %d posid %d", p->group->id, p->id, rank, p->itemid);
			p->group->game_handler = NULL;
			for (int i = 0; i < p->group->count; i++) {
				p->group->players[i]->waitcmd = 0;
			}
			//clear player info
			delete my_cards_info;
			delete my_game_info;
			return GER_end_of_game;
		}
		DEBUG_LOG("%lu advance card game %d level %d posid %d", p->group->id, p->id, rank, p->itemid);
	}


	if (COMPETE_MODE_GAME(p->group->game)) {
		//DEBUG_LOG("%lu\tCOMPETE_MODE_GAME %d", p->group->id, p->id);

		if(ADVANCE_GAME(p->group->game)) {
			DEBUG_LOG("%lu\tADVANCE_COMPETE_MODE_GAME %d", p->group->id, p->id);
			p->waitcmd = 0;
			//test......
			int posid = p->itemid - 9;
			advance_compete[posid].push_back(my_game_info);
			sprite_t *rival = (advance_compete[posid].front())->player;
			if(rival != p) {
				process(p, rival, my_game_info, advance_compete[posid], advance_compete[posid]);
			}
			return 0;
		}
		//player can quit the waiting queue
		p->waitcmd = 0;
		competition[rank].push_back(my_game_info);
		sprite_t *rival = (competition[rank].front())->player;

		//if find a rival, creat new game
		if (rival == p) {
			if (rank >= 5) {
				wait_usr_info *m_info = new wait_usr_info;
				m_info->rank = rank;
				m_info->info = my_game_info;
				wait_q[rank-5].push_back(m_info);
				ADD_TIMER_EVENT(p->group, on_game_timer_expire, m_info, now.tv_sec + 10);
			}
		} else {
			if (rank >=5) {
				std::deque<wait_usr_info*>::iterator it;
				for (it = wait_q[rank-5].begin(); it!= wait_q[rank-5].end(); it++) {
					wait_usr_info* p_usr_info = *it;
					if(p_usr_info->info->player == rival) {
						delete *it;
						wait_q[rank-5].erase(it);
						break;
					}
				}
			}
			process(p, rival, my_game_info, competition[rank], competition[rank]);
		}

	} else if (CHALLENGE_GAME(p->group->game)){
		//it's a challenge game
		//DEBUG_LOG("%lu\tCHALLENGE_MODE_GAME %d", p->group->id, p->id);
		Card *challenge_game = new Card (p->group);
		assert(challenge_game);
		challenge_game->setup (my_game_info, NULL, &all_cards);
		notify_server_ready(my_game_info->player);
		p->waitcmd = 0;
		p->group->game_handler = challenge_game;

	} else if (MULTIPLAYER_GAME(p->group->game)) {
		//DEBUG_LOG("%lu\tMULTIPLAYER %d", p->group->id, p->id);
		p->group->card_count ++;
		if (p->group->card_count == 1) {
			Card *free_game = new Card(p->group);
			assert(free_game);
			std::map<game_group_t*, Card*>::iterator it = free_mode.find (p->group);
			if (it != free_mode.end()) {
				ERROR_LOG("%lu MULTIGAME exist %d", p->group->id, p->id);
			}
			free_mode[p->group] = free_game;
			free_game->setup (my_game_info, NULL, &all_cards);
		} else if (p->group->card_count == 2){
			std::map<game_group_t*, Card*>::iterator it = free_mode.find (p->group);
			it->second->setup (NULL, my_game_info, NULL);
			p->group->game_handler = it->second;
			notify_server_ready(p->group->players[0]);
			notify_server_ready(p->group->players[1]);
			p->group->players[0]->waitcmd = 0;
			p->group->players[1]->waitcmd = 0;
			free_mode.erase(it);
		}
	}
	return 0;
}

/**
 * @brief  计算玩家卡牌等级
 * @param  player_cards_info_t *p_info 玩家卡牌信息指针
 * @return 0－9 卡牌等级
 */
int
Match::calculate_rank(player_cards_info_t *p_info)
{
    if(p_info->flag&0x1) {
        return 9;
    }
	int scores = WIN_BONUS*(p_info->win_cnt) + LOST_BONUS*(p_info->lost_cnt);
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
 * @brief 两个队列玩家配对
 * @param sprite_t *p 要求配对的人
 * @param sprite_t *rival 被配对的人
 * @param player_game_info *rival 要求配对玩家的游戏信息
 * @param my_queue 玩家一所在队列
 * @param rival_queue 玩家二所在队列
 * @return 0 成功
 */
int
Match::process(sprite_t *p, sprite_t *rival, player_game_info_t *my_game_info, std::deque<player_game_info_t*>& my_queue, std::deque<player_game_info_t*>& rival_queue)
{
	free_game_group(rival->group);
	rival->group = p->group;
	Card *card_game= new Card(p->group);
	assert (card_game);
	player_game_info_t *rival_info = rival_queue.front();
	card_game->setup(my_game_info, rival_info, &all_cards);
	notify_server_ready(p);
	notify_server_ready(rival);
	MOD_EVENT_EXPIRE_TIME(p->group, on_timer_expire, now.tv_sec + 6000);
	p->group->game_handler = card_game;
	rival->group->game_handler = card_game;
	p->group->count = 2;
	p->group->players[1] = rival;
	p->waitcmd = 0;
	rival->waitcmd = 0;
	//pop rival
	my_queue.pop_front();
	//pop myself
	rival_queue.pop_front();
	return 0;
}

/**
 * @brief  游戏服务器启动时读取卡牌配置信息
 * @param  const char *file 卡牌文件地址
 * @return 0 正常 -1 错误
 */
int
Match::load_cards_conf (const char *file)
{
	int i, err = -1;
	int cards_num = 0;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile (file);
	if (!doc)
		ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	DECODE_XML_PROP_INT (cards_num, cur, "Count");
	if (cards_num < 0 || cards_num > CARD_ID_MAX) {
		ERROR_LOG ("error cards_num: %d", cards_num);
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Card"))){
			DECODE_XML_PROP_INT(all_cards[i].id, cur, "ID");
			if (all_cards[i].id != i)
				ERROR_RETURN(("%s parse error: id=%u, num=%d",
					config_get_strval("cards_conf"), all_cards[i].id, cards_num), -1);

			DECODE_XML_PROP_INT(all_cards[i].type, cur, "Type");
			DECODE_XML_PROP_INT(all_cards[i].colour, cur, "Colour");
			DECODE_XML_PROP_INT(all_cards[i].value, cur, "Value");
			DECODE_XML_PROP_INT(all_cards[i].event, cur, "Event");
			DECODE_XML_PROP_INT(all_cards[i].star, cur, "Star");
			if ( all_cards[i].type > CARD_TYPE_MAX) {
				ERROR_LOG("Card[%d] Type configuration Error", i);
				goto exit;
			}
			if (all_cards[i].colour > CARD_COLOUR_MAX) {
				ERROR_LOG("Card[%d] Colour configuration Error", i);
				goto exit;
			}
			if (all_cards[i].value > CARD_VALUE_MAX) {
				ERROR_LOG("Card[%d] Value configuration Error", i);
				goto exit;
			}
			if (all_cards[i].event > CARD_EVENT_MAX) {
				ERROR_LOG("Card[%d] Event configuration Error", i);
				goto exit;
			}
			if (all_cards[i].star> CARD_STAR_MAX) {
				ERROR_LOG("Card[%d] star configuration Error", i);
				goto exit;
			}
			++i;
		}
		if (i == cards_num)
			break;
		cur = cur->next;
	}

	if (i != cards_num) {
		ERROR_LOG ("parse %s failed, cards Count=%d, get Count=%d",
				file, cards_num, i);
		goto exit;
	}
	err = 0;
exit:
	xmlFreeDoc (doc);
	return err;
}

