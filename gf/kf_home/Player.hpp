/**
* =====================================================================================
*       @file  Player.hpp
*      @brief
*
*  Detailed description starts here.
*
*   @internal
*     Created  3/17/2010 11:13:56 AM
*    Revision  1.0.0.0
*    Compiler  gcc/g++
*     Company  TaoMee.Inc, ShangHai.
*   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
*
*     @author  mark (), mark@taomee.com
* This source code is wrote for TaoMee,Inc. ShangHai CN.
* =====================================================================================
*/
#ifndef KF_PLAYER_HPP_
#define KF_PLAYER_HPP_

extern "C" {
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>
}
#include <map>
#include <queue>
#include "fwd_decl.hpp"
//#include "cli_proto.hpp"

class Map;
class PlayerManage;
class Player;
class Online;
struct cached_pkg_t;

typedef std::map<userid_t, Player*> PlayerMap;

struct player_clothes_info_t {
	uint32_t clothes_id;
	uint32_t unique_id;
};

struct player_base_t {
	/*! player's id */
	userid_t	id;
	/*! the create time of the player's role */
	uint32_t	role_tm;
	/*! type of a player's role */
	uint8_t		role_type;
	/*! player's nick name */
	char		nick[max_nick_size];
	/*! vip flag */
	uint32_t	vip;
	/*! flag, currently unused */
	uint32_t	flag;
	/*! player's money */
	uint32_t	coins;

	/*! number of clothes on */
	uint32_t	clothes_num;
	/*! clothes' info */
	player_clothes_info_t	clothes[max_clothes_on];

	/*! fd session */
	fdsession_t*	fdsess;
	/*! save cmd id that is currently under processing */
	uint16_t	waitcmd;
	/*! hold pending processing packages */
	std::queue<cached_pkg_t*> waitcmd_queue;
};

class Player {
public:
	//~Player();

	//PlayerManage* getManage() const { return manage_; }
	int setManage(PlayerManage* manage);
	int setOnline(Online* online);

	int enter_map(map_id_t mapid, uint32_t x, uint32_t y);
	int leave_map(int self_flag);
	int list_players_in_map();
	int logout(uint32_t reason);

	void send_to_client(void* buf, int len, int complete);
	void send_to_player(void* buf, int len, int complete);
	void send_header_to_client(uint16_t cmd, uint32_t err, int complete);
	void send_header_to_player(uint16_t cmd, uint32_t err, int complete);
public:
	player_base_t base_;
	home_id_t homeid;
private:
	friend class GlobalPlayer;
	Player(userid_t id);
	~Player();
	int pack_entermap_rsp(uint8_t* buf);
private:
	//home_id_t homeid;
	PlayerManage* m_pManage_;
	Online* m_pOnline_;
};

class PlayerManage {
public:
	PlayerManage(Map* map);
	~PlayerManage();

	void addPlayer(Player* p);
	void delPlayer(Player* p);

	Player* getPlayer(userid_t id);
	Map* getMap() const { return map_; }
	void send_to_all_players(Player* p, void* buf, int len, int complete);
	void send_to_all_players_except_self(Player* p, void* buf, int len);
public:
	PlayerMap player_map_;
private:
	Map* map_;
};

class GlobalPlayer {
public:
	GlobalPlayer();
	~GlobalPlayer();
	
	Player* allocPlayer(userid_t id);
	void deallocPlayer(Player* p);
	
	Player* getPlayer(userid_t id);
private:
	PlayerMap player_map_;
};

#endif //KF_PLAYER_HPP_
