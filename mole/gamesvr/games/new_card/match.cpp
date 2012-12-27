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

#ifndef TW_VER
#include  <statistic_agent/msglog.h>
#endif

}

#include "match.hpp"
#include "player.hpp"
#include <ant/inet/pdumanip.hpp>


/**
 * @brief 每次玩家进入，先请求用户卡牌库信息
 * @param sprite_t* p 玩家指针
 * @return 无
 */
void Cmatch::init (sprite_t* p)
{
	p->group->card_count = 0;
	for (int i = 0; i < p->group->count; i++)
	{
		p->group->players[i]->waitcmd = proto_new_card_get_card_info;
		if(send_request_to_db(db_new_card_get_card_info, p->group->players[i], 0, 0, p->group->players[i]->id) != 0)
		{
			ERROR_LOG("Get Player Cards info Error ");
		}

	}

}

/**
 * @brief 用户中途或者意外离开时，竞赛模式退出队列，游戏组的游戏指针置空，防止Match被析构
 * @param
 * @return GER_end_of_game 结束游戏
 */
int Cmatch::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	switch (cmd)
	{
		case proto_player_leave:
			DEBUG_LOG("match: handle_data, player %d leave, %d", p->id, cmd);
			break;
		case NEW_CARD_PLAYER_QUIT:
			DEBUG_LOG("match: handle_data, player %d quit, %d", p->id, cmd);
			break;
		default:
			DEBUG_LOG("match: handle_data, player %d undef cmd %d",p->id, cmd);
            break;
	}

	int rank = -1;
	int pos = -1;
	bool is_break = false;

	//离开竞赛游戏队列
	if (COMPETE_MODE_GAME(p->group->game))
	{
		std::vector<Cplayer*>::iterator it;
		for (int i = 0; i < 5; i++)
		{
			rank = i;
			pos = -1;
			for (it = competition[i].begin(); it != competition[i].end(); it++)
			{
				pos++;
				Cplayer* p_player = *it;
				if (p_player != NULL)
				{
					if(p_player->get_sprite() == p)
					{
						player_cards_info_t* p_cards_info =  p_player->get_cards_info();
						delete p_cards_info;
						p_cards_info = NULL;
						delete p_player;
						p_player = NULL;
						competition[i].erase(competition[i].begin()+pos);
						is_break = true;
			        	DEBUG_LOG("%lu COMPETE LEAVE %d", p->group->id, p->id);

						notify_user_info(rank);

						break;
					}
				}
			}
			if (is_break)
			{
				break;
			}
		}

		//delet wait queue
		for (uint32_t j = 0; j < wait_players.size(); j ++)
		{
			if (wait_players[j] != NULL)
			{
				if (wait_players[j]->get_sprite() == p)
				{
					player_cards_info_t* p_cards_info =  wait_players[j]->get_cards_info();
					delete p_cards_info;
					p_cards_info = NULL;
					delete wait_players[j];
					wait_players[j] = NULL;
					wait_players.erase(wait_players.begin()+j);
			        DEBUG_LOG("%lu player userid %d leave delete wait queue ", p->group->id, p->id);

				}
			}
		}

		if ( rank == -1) {
			ERROR_LOG("Cnew_card: compete mode: player %d leave ,but not found", p->id);
			return GER_end_of_game;
		}
	}
	else if (MULTIPLAYER_GAME(p->group->game))
	{
		std::map<game_group_t*, Cnew_card*>::iterator it = free_mode.find(p->group);
		if (it != free_mode.end())
		{
			DEBUG_LOG("%lu MULTIGAME LEAVE %d", p->group->id, p->id);
			free_mode.erase(it);
		}
		else
		{
			ERROR_LOG("%lu MULTIGAME leave, but not found %d", p->group->id, p->id);
		}
	}

	p->group->game_handler = NULL;
	for (int i = 0; i < p->group->count; i++) {
		p->group->players[i]->waitcmd = 0;
	}

	return  GER_end_of_game;

}

/**
 * @brief  告诉玩家服务器端已经获得了玩家卡牌库信息，可以开始请求卡牌队列了
 * @param  sprite_t* p 玩家指针
 * @return －1 发送失败
 * @return  0 发送成功
 */
int Cmatch::notify_server_ready(sprite_t *p)
{
	int l = sizeof (protocol_t);
	init_proto_head(pkg, NEW_CARD_SERVER_READY, l);
	if (send_to_self (p, pkg, l, 1 ) == 0) {
	} else {
		ERROR_LOG("notify server ready error");
		return 0;
	}
	return 0;
}

int Cmatch::notify_user_info(Cnew_card* cardgame)
{

	int l = sizeof (protocol_t);
	uint32_t gameid = cardgame->m_grp->game->id;
	uint32_t groupid = cardgame->m_grp->id;

	ant::pack(pkg, gameid, l);
	ant::pack(pkg, groupid, l);

	uint32_t count = cardgame->player.size();
	ant::pack(pkg, count, l);
	for (uint32_t i = 0; i < count; i++ )
	{
		ant::pack(pkg, cardgame->player[i]->id(), l);
		DEBUG_LOG(" notify_user_info player userid:%u", cardgame->player[i]->id());
	}

	init_proto_head(pkg, NEW_CARD_USER_INFO, l);
	send_to_players(cardgame->m_grp, pkg, l);

	return 0;
}

int Cmatch::notify_user_info(uint32_t rank)
{

	int l = sizeof (protocol_t);
	uint32_t gameid = 0;
	uint32_t groupid = 0;

	ant::pack(pkg, gameid, l);
	ant::pack(pkg, groupid, l);

	uint32_t count = competition[rank].size();
	if (count >= 4)
	{
		return 0;
	}

	ant::pack(pkg, count, l);
	for (uint32_t i = 0; i < count; i++ )
	{
		ant::pack(pkg, competition[rank][i]->id(), l);
	}

	init_proto_head(pkg, NEW_CARD_USER_INFO, l);

	for (uint32_t i = 0; i < count; i++ )
	{
		sprite_t* p = competition[rank][i]->get_sprite();
		send_to_self(p, pkg, l, 1 );
	}

	return 0;
}

int Cmatch::notify_wait_user_info( )
{

	int l = sizeof (protocol_t);
	uint32_t gameid = 0;
	uint32_t groupid = 0;

	ant::pack(pkg, gameid, l);
	ant::pack(pkg, groupid, l);

	uint32_t count = wait_players.size();
	if (count >= 4)
	{
		return 0;
	}

	ant::pack(pkg, count, l);
	for (uint32_t i = 0; i < count; i++ )
	{
		ant::pack(pkg, wait_players[i]->id(), l);
	}

	init_proto_head(pkg, NEW_CARD_USER_INFO, l);

	for (uint32_t i = 0; i < count; i++ )
	{
		sprite_t* p = wait_players[i]->get_sprite();
		send_to_self(p, pkg, l, 1 );
	}

	return 0;
}


/**
 * @brief  处理数据库返回的玩家卡牌库信息
 * @param
 * @return GER_end_of_game 玩家卡牌没有激活时结束游戏
 * @return 0 正常处理结束
 */
int Cmatch::handle_db_return(sprite_t *p, uint32_t id, const void *buf, int len, uint32_t ret_code)
{
	DEBUG_LOG("%lu\t Cmatch::handle_db_return %d, ret_code:%u,wait_cmd:%u", p->group->id, p->id, ret_code, p->waitcmd);
	if( p->waitcmd != proto_new_card_get_card_info )
	{
		ERROR_LOG( "Group:[%lu],Player:[%u],New Card Game handle_db_return,Len:[%d]", p->group->id, p->id, len );
		return 0;
	}
	player_cards_info_t *my_cards_info = NULL;
	my_cards_info = new player_cards_info_t;
	if (my_cards_info == NULL)
	{
		return GER_end_of_game;
	}

	memcpy(my_cards_info, buf, len);

	if (my_cards_info->total_cnt == 0)
	{
		//this player has never been init;
		ERROR_LOG("%d new card not inited", p->id);
		delete my_cards_info;
		p->group->game_handler = NULL;
		for (int i = 0; i < p->group->count; i++) {
			p->group->players[i]->waitcmd = 0;
		}
		return GER_end_of_game;
	}

/*
	my_cards_info->total_cnt = 11;
	my_cards_info->card_id[0] = 0;
	my_cards_info->card_id[1] = 1;
	my_cards_info->card_id[2] = 2;
	my_cards_info->card_id[3] = 3;
	my_cards_info->card_id[4] = 4;
	my_cards_info->card_id[5] = 5;
	my_cards_info->card_id[6] = 6;
	my_cards_info->card_id[7] = 7;
	my_cards_info->card_id[8] = 8;
	my_cards_info->card_id[9] = 9;
	my_cards_info->card_id[10] = 10;
*/

	Cplayer *p_player = NULL;
	p_player = new Cplayer;
	if (p_player == NULL)
	{
		return GER_end_of_game;
	}

	p_player->set_sprite(p);
	p_player->set_life_value(6);
	p_player->set_cards_info(my_cards_info);
	int  rank = p_player->calculate_rank(my_cards_info);
	p_player->set_rank(rank);
	p_player->create_cards_seq();

	if (COMPETE_MODE_GAME(p->group->game))
	{
		DEBUG_LOG("%lu\t COMPETE_MODE_GAME %d", p->group->id, p->id);
		process_compete_mode(p, p_player);

	}
	else if (CHALLENGE_GAME(p->group->game)){

		//it's a challenge game
        DEBUG_LOG("%lu\tCHALLENGE_MODE_GAME %d", p->group->id, p->id);
        Cnew_card *challenge_game = new Cnew_card (p->group);
        assert(challenge_game);
		challenge_game->set_all_card_info(&all_cards);
        notify_server_ready(p);
        p->waitcmd = 0;
		challenge_game->init_players_cnt = p->group->count;
		challenge_game->set_player(p_player);
        p->group->game_handler = challenge_game;
		notify_user_info(challenge_game);

    }
	else if (MULTIPLAYER_GAME(p->group->game))
	{

		DEBUG_LOG("%lu\tMULTIPLAYER %d", p->group->id, p->id);
		p->group->card_count++;
		if (p->group->card_count == 1)
		{
			Cnew_card *free_game = new Cnew_card(p->group);
			assert(free_game);
			std::map<game_group_t*, Cnew_card*>::iterator it = free_mode.find(p->group);
			if (it != free_mode.end()) {
				ERROR_LOG("%lu MULTIGAME exist %d", p->group->id, p->id);
			}
			free_mode[p->group] = free_game;
			free_game->set_all_card_info(&all_cards);
			free_game->set_player(p_player);
			notify_user_info(free_game);
		}
		else if (p->group->card_count == p->group->game->players)
		{
			std::map<game_group_t*, Cnew_card*>::iterator it = free_mode.find(p->group);
			it->second->set_player(p_player);
			it->second->init_players_cnt = p->group->card_count;
			for (uint32_t i = 0 ; i < p->group->card_count ; i++)
			{
				notify_server_ready(p->group->players[i]);
				p->group->players[i]->waitcmd = 0;
			}

			p->group->game_handler = it->second;
			free_mode.erase(it);
			notify_user_info(it->second);

		}
		else
		{
			std::map<game_group_t*, Cnew_card*>::iterator it = free_mode.find(p->group);
			it->second->set_player(p_player);
			notify_user_info(it->second);
		}
	}

#ifndef TW_VER
    /* statistic log operation when enable statistic  */
    if (statistic_file != NULL) {
        uint32_t statistic_type = 0;
        statistic_type = STATISTIC_TYPE + p->group->game->id;
        uint32_t buff[7] = {};
        buff[0] = 0;
        buff[1] = 0;
        buff[2] = 0;
        buff[3] = 0;
        buff[4] = 0;
        buff[5] = 1;
        buff[6] = p->id;
        int msgret = msglog(statistic_file, statistic_type, get_now_tv()->tv_sec, buff, sizeof(buff));
        if (msgret != 0) {
            ERROR_LOG( "statistic log error: message type(%x)",statistic_type);
        }
    }
#endif

	return 0 ;

}


/**
 * @brief 超时返回处理函数
 * @param
 * @return
 * @return 0 正常处理结束
 */
int Cmatch::handle_timeout(void* data)
{


	Cplayer* p_player = (Cplayer*)data;

	sprite_t* pp = p_player->get_sprite();
	if (wait_players.size() < 2)
	{
		if (!is_in_wait_player(p_player))
		{
			remove_compet_player(p_player);
			wait_players.push_back(p_player);
			notify_wait_user_info();
		}
		ADD_TIMER_EVENT(pp->group, on_game_timer_expire, p_player, now.tv_sec + 10);
	}
	else
	{
	  	Cnew_card* cardgame= new Cnew_card(pp->group);
		assert (cardgame);
	    pp->group->game_handler = cardgame;
		cardgame->set_all_card_info(&all_cards);
		cardgame->set_player(p_player);
		remove_compet_player(p_player);
		remove_wait_player(p_player);
		notify_server_ready(pp);
		cardgame->m_grp->count = 1;
		pp->waitcmd = 0;

		uint32_t i = 0;
		for (i = 0; i < wait_players.size(); i++)
		{
			if (i == 3)
			{
				break ;
			}

			Cplayer* pplayer = NULL;
			pplayer = wait_players[i];
			cardgame->set_player(pplayer);
			sprite_t *p_sprite = pplayer->get_sprite();
			cardgame->m_grp->count += 1;
			uint32_t count = cardgame->m_grp->count;
			free_game_group(p_sprite->group);
			p_sprite->group = cardgame->m_grp;
			pp->group->players[count-1] = p_sprite;
			p_sprite->waitcmd = 0;
			notify_server_ready(p_sprite);
			DEBUG_LOG(" Cmatch::handle_timeout in for i:%d", i);

		}

		REMOVE_TIMERS(pp->group);
		notify_user_info(cardgame);
		cardgame->init_players_cnt = cardgame->m_grp->count;

		DEBUG_LOG(" Cmatch::handle_timeout out for i:%d, init_players_cnt:%d", i, cardgame->init_players_cnt);

		wait_players.erase(wait_players.begin(), wait_players.begin() + i);

	}

	return 0;


}

void Cmatch::remove_wait_player(Cplayer* p_player)
{
	for (uint32_t j = 0; j < wait_players.size(); j++)
	{
		if (p_player == wait_players[j])
		{
			wait_players.erase(wait_players.begin()+j);
		}
	}

	return ;

}


bool Cmatch::is_in_wait_player(Cplayer* p_player)
{
	for (uint32_t j = 0; j < wait_players.size(); j++)
	{
		if (p_player == wait_players[j])
		{
			return true;
		}
	}

	return  false;

}


void Cmatch::remove_compet_player(Cplayer* p_player)
{
	int rank = p_player->get_rank();
	for (uint32_t j = 0; j < competition[rank].size(); j++)
	{
		if (p_player == competition[rank][j])
		{
			competition[rank].erase(competition[rank].begin()+j);
		}
	}

	return ;

}


/**
 * @brief 玩家加入游戏或创建新的游戏
 * @param sprite_t *p
 * @param Cplayer * 玩家的游戏信息
 * @return 0 成功
 */
int Cmatch::process_compete_mode(sprite_t *p, Cplayer *p_player)
{
	uint32_t rank = p_player->get_rank();

	uint32_t game_count = 4;

	competition[rank].push_back(p_player);

	Cnew_card* cardgame = NULL;
	if (competition[rank].size() < game_count)
	{
		//通知玩家加入游戏
		notify_user_info(rank);
		ADD_TIMER_EVENT(p->group, on_game_timer_expire, p_player, now.tv_sec + 10);
	}
	else
	{
		sprite_t* pp = competition[rank][0]->get_sprite();
		cardgame= new Cnew_card(pp->group);
		assert (cardgame);
		DEBUG_LOG(" Cmatch::cardgame 1:%p, grp:%p", cardgame, cardgame->m_grp);
	    pp->group->game_handler = cardgame;
		cardgame->set_all_card_info(&all_cards);
		cardgame->m_grp->count = 1;
		pp->waitcmd = 0;

		for (uint32_t i = 0; i < game_count; i++)
		{
			Cplayer* pplayer = NULL;
			pplayer = competition[rank][i];
			if (pplayer == NULL)
			{
				ERROR_LOG(" Cmatch::cardgame 2:%p, grp:%p", cardgame, cardgame->m_grp);
			}

			cardgame->set_player(pplayer);

			sprite_t *p_sprite = pplayer->get_sprite();
			if (p_sprite != pp)
			{
				cardgame->m_grp->count += 1;
				uint32_t count = cardgame->m_grp->count;
				free_game_group(p_sprite->group);
				p_sprite->group = cardgame->m_grp;
				pp->group->players[count-1] = p_sprite;
				p_sprite->waitcmd = 0;
			}

			notify_server_ready(p_sprite);
		}

		REMOVE_TIMERS(pp->group);

		notify_user_info(cardgame);
		cardgame->init_players_cnt = cardgame->m_grp->count;
		competition[rank].erase(competition[rank].begin(), competition[rank].begin() + game_count);
	}

	return 0;

}


/**
 * @brief  游戏服务器启动时读取卡牌配置信息
 * @param  const char *file 卡牌文件地址
 * @return 0 正常 -1 错误
 */
int Cmatch::load_cards_conf (const char *file)
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
					config_get_strval("new_cards_conf"), all_cards[i].id, cards_num), -1);

			DECODE_XML_PROP_INT(all_cards[i].type, cur, "Type");
			DECODE_XML_PROP_INT(all_cards[i].value, cur, "Value");

			if ( all_cards[i].type > CARD_TYPE_MAX) {
				ERROR_LOG("Card[%d] Type configuration Error", i);
				goto exit;
			}
			if (all_cards[i].value > CARD_VALUE_MAX) {
				ERROR_LOG("Card[%d] Value configuration Error", i);
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


