/*
 * =====================================================================================
 *
 *       Filename:  player.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/29/2011 05:24:47 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  ianyang (ianyang), ianyang@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "player.hpp"
#include "chat.hpp"
#include "group.hpp"

Player::Player(uint32_t uid, ChatMrg* p_chat, fdsession_t* in_fdsess)
{
	uid_ = uid;
	p_chat_mrg = p_chat;
	fdsess = in_fdsess;
	fd = in_fdsess->fd;
}

Player::~Player()
{
	if (get_fight_team_id()) {
		p_chat_mrg->p_talk_group_mrg->del_talk_obj(talk_type_fight_team, get_fight_team_id(), uid());
	}
}

int Player::recv_msg(Player* sender, void* data)
{
	return 0;
}

void Player::set_player_base_attr(user_onoff_info_t* p_attr)
{
	user_tm = p_attr->user_tm;	
	memcpy(nick, p_attr->nick, sizeof(nick));
	fight_team_id = p_attr->fight_team_id;
}

PlayerMrg::PlayerMrg()
{
}

PlayerMrg::~PlayerMrg()
{
	PlayerMap::iterator it = player_map.begin();
	PlayerMap::iterator end = player_map.end();
	for (; it != end; ++it) {
		delete (it->second);
	}
	player_map.clear();
}

Player* PlayerMrg::alloc_player(uint32_t uid, ChatMrg* p_chat_mrg, fdsession_t* fdsess)
{
	del_player(uid);
	Player* p = new Player(uid, p_chat_mrg, fdsess);
	player_map.insert(PlayerMap::value_type(uid, p));
	DEBUG_LOG("alloc uid=%u", p->uid());
	return p;
}

Player* PlayerMrg::get_player(uint32_t uid)
{
	PlayerMap::iterator it = player_map.find(uid);
	if (it != player_map.end()) {
		DEBUG_LOG("alloc uid=%u", uid);
		return it->second;
	}
	ERROR_LOG("NOT FIND PLAYER %u", uid);
	return 0;
}


void PlayerMrg::del_player(uint32_t uid)
{
	PlayerMap::iterator it = player_map.find(uid);
	if (it != player_map.end()) {
		DEBUG_LOG("del uid=%u", it->second->uid());
		delete (it->second);
		player_map.erase(uid);
	}
}

/**
  * @brief clear players' info whose online server is down
  * @param fd online fd
  */
void PlayerMrg::clear_players(int fd)
{
	DEBUG_LOG("B4\t[player_num=%lld]", static_cast<long long>(player_map.size()));

	PlayerMap::iterator it = player_map.begin();
	while (it != player_map.end()) {
		PlayerMap::iterator t_it = it++;
		Player* p = t_it->second;
		if ((p->fd == fd) || (fd == -1)) {
			player_map.erase(p->uid());
			delete p;
		}
	}
	DEBUG_LOG("AF\t[player_num=%lld]", static_cast<long long>(player_map.size()));
}



