/*
 * =====================================================================================
 *
 *       Filename:  player.hpp
 *
 *    Description:  desc class Player
 *
 *        Version:  1.0
 *        Created:  12/29/2011 05:21:31 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ianyang (ianyang), ianyang@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef PLAYER_HPP_
#define PLAYER_HPP_
#include <map>

extern "C" 
{
#include <async_serv/dll.h>
}

#include "object.hpp"

struct user_onoff_info_t;
class ChatMrg;

class Player : public Object {
	public:
		Player(uint32_t uid, ChatMrg*, fdsession_t* );
		~Player();
	public:
		int recv_msg(Player* sender, void* data);
		void set_player_base_attr(user_onoff_info_t* p_attr);
		uint32_t uid() {
			return uid_;
		}

		uint32_t get_fight_team_id() {
			return fight_team_id;
		}
		void set_fight_team_id(uint32_t in_id) {
			fight_team_id = in_id;
		}

	public:
		fdsession_t* fdsess;
		uint16_t waitcmd;
		int fd;
	private:
		uint32_t uid_;
		char nick[16];
		uint32_t user_tm;
		uint32_t fight_team_id;
};


class PlayerMrg : public Object {
	public:
typedef std::map<uint32_t, Player*> PlayerMap;
		PlayerMrg();
		~PlayerMrg();
		Player* alloc_player(uint32_t uid, ChatMrg*, fdsession_t* fdsess);
		Player* get_player(uint32_t uid);
		void del_player(uint32_t uid);
		void clear_players(int fd);
	private:
		PlayerMap player_map;
		ChatMrg* p_chat_mrg;
		fdsession_t* fdsess;
};


#endif //END PLAYER_HPP_
