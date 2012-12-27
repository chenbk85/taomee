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
#include "player.hpp"
#include <ant/inet/pdumanip.hpp>

/**
 * @brief 玩家进入
 * @param sprite_t* p 玩家指针
 * @return 无
 */
void Cmatch::init (sprite_t* p)
{
	DEBUG_LOG("Cmatch::init user:%u", p->id);

	player_cards_info_t *my_cards_info = NULL;
	my_cards_info = new player_cards_info_t;

	my_cards_info->total_cnt = 60;
	for (uint32_t i = 0; i < 60; i++)
	{
		my_cards_info->card_id[i] = i;

	}

	Cplayer *p_player = NULL;
	p_player = new Cplayer;

	p_player->set_sprite(p);
	p_player->set_cards_info(my_cards_info);
	p_player->create_cards_seq();

	if (COMPETE_MODE_GAME(p->group->game))
	{
		DEBUG_LOG("%lu\t COMPETE_MODE_GAME %d", p->group->id, p->id);
		mgr_player_match(p, p_player);

	}

	return ;

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
		default:
			DEBUG_LOG("match: handle_data, player %d undef cmd %d",p->id, cmd);
            break;
	}

	int pos = -1;

	//离开竞赛游戏队列
	if (COMPETE_MODE_GAME(p->group->game))
	{
		std::vector<Cplayer*>::iterator it;
		pos = -1;
		for (it = tug_players.begin(); it != tug_players.end(); it++)
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
					tug_players.erase(tug_players.begin()+pos);
			        DEBUG_LOG("%lu COMPETE LEAVE %d", p->group->id, p->id);
					break;
				}
			}
		}
	}

	if ( pos == -1)
	{
		ERROR_LOG("new_tug:compete mode: player %d leave ,but not found", p->id);
		return GER_end_of_game;
	}

	p->group->game_handler = NULL;
	for (int i = 0; i < p->group->count; i++)
	{
		p->group->players[i]->waitcmd = 0;
	}

	return  GER_end_of_game;

}

/**
 * @brief  告诉玩家服务器端已经获得了玩家信息，可以开始请求卡牌队列了
 * @param  sprite_t* p 玩家指针
 * @return －1 发送失败
 * @return  0 发送成功
 */
int Cmatch::notify_server_ready(sprite_t *p)
{
	int l = sizeof (protocol_t);
	init_proto_head(pkg, NEW_TUG_SERVER_READY, l);
	if (send_to_self (p, pkg, l, 1 ) == 0) {
	} else {
		ERROR_LOG("notify server ready error");
		return 0;
	}
	return 0;
}

int Cmatch::notify_user_info(Cnew_tug* tuggame)
{

	int l = sizeof (protocol_t);
	uint32_t gameid = tuggame->m_grp->game->id;
	uint32_t groupid = tuggame->m_grp->id;

	ant::pack(pkg, gameid, l);
	ant::pack(pkg, groupid, l);

	uint32_t count = tuggame->players.size();
	ant::pack(pkg, count, l);
	for (uint32_t i = 0; i < count; i++ )
	{
		ant::pack(pkg, tuggame->players[i]->id(), l);
		DEBUG_LOG(" notify_user_info player userid:%u", tuggame->players[i]->id());
	}

	init_proto_head(pkg, NEW_TUG_USER_INFO, l);
	send_to_players(tuggame->m_grp, pkg, l);

	return 0;
}

int Cmatch::notify_user_info( )
{

    int l = sizeof (protocol_t);
    uint32_t gameid = 0;
    uint32_t groupid = 0;

    ant::pack(pkg, gameid, l);
    ant::pack(pkg, groupid, l);

    uint32_t count = tug_players.size();
    if (count >= 2)
    {
        return 0;
    }

    ant::pack(pkg, count, l);
    for (uint32_t i = 0; i < count; i++ )
    {
        ant::pack(pkg, tug_players[i]->id(), l);
    }

    init_proto_head(pkg, NEW_TUG_USER_INFO, l);

    for (uint32_t i = 0; i < count; i++ )
    {
        sprite_t* p = tug_players[i]->get_sprite();
        send_to_self(p, pkg, l, 1 );
    }

    return 0;
}



/**
 * @brief  处理数据库返回的玩家信息
 * @param
 * @return GER_end_of_game 玩家卡牌没有激活时结束游戏
 * @return 0 正常处理结束
 */
int Cmatch::handle_db_return(sprite_t *p, uint32_t id, const void *buf, int len, uint32_t ret_code)
{

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

	return 0;

}


/**
 * @brief 玩家加入游戏或创建新的游戏
 * @param sprite_t *p
 * @param Cplayer * 玩家的游戏信息
 * @return 0 成功
 */
int Cmatch::mgr_player_match(sprite_t *p, Cplayer *p_player)
{

	uint32_t game_count = 2;

	tug_players.push_back(p_player);

	Cnew_tug* tuggame = NULL;
	if (tug_players.size() < game_count)
	{
		//通知玩家加入游戏
		notify_user_info();

	}
	else
	{
		sprite_t* pp = tug_players[0]->get_sprite();
		tuggame= new Cnew_tug(pp->group);
		assert (tuggame);
		DEBUG_LOG(" Cmatch::tuggame :%u, grp:%u", tuggame, tuggame->m_grp);
	    pp->group->game_handler = tuggame;
		tuggame->set_all_card_info(&all_cards);
		tuggame->m_grp->count = 1;

		for (uint32_t i = 0; i < game_count; i++)
		{
			Cplayer* pplayer = NULL;
			pplayer = tug_players[i];

			tuggame->set_player(pplayer);

			sprite_t *p_sprite = pplayer->get_sprite();
			if (p_sprite != pp)
			{
				tuggame->m_grp->count += 1;
				uint32_t count = tuggame->m_grp->count;
				free_game_group(p_sprite->group);
				p_sprite->group = tuggame->m_grp;
				pp->group->players[count-1] = p_sprite;

			}

			notify_server_ready(p_sprite);
		}

		notify_user_info(tuggame);
		tuggame->players_cnt = tuggame->m_grp->count;
		tug_players.erase(tug_players.begin(), tug_players.begin() + game_count);

	}


	return 0;

}


/**
 * @brief  游戏服务器启动时读取tug配置信息
 * @param  const char *file 文件地址
 * @return 0 正常 -1 错误
 */
int Cmatch::load_tugs_conf (const char *file)
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

//			DEBUG_LOG("Card[%d] Type:%d", i, all_cards[i].type);

			if ( all_cards[i].type > CARD_TYPE_MAX) {
				ERROR_LOG("Card[%d] Type configuration Error", i);
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



