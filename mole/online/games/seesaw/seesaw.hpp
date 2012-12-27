#ifndef ANDY_SEESAW_HPP_
#define ANDY_SEESAW_HPP_

#include <cassert>
#include <map>

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/log.h>

#include "benchapi.h"
#include "proto.h"
}

class Seesaw {
public:
	static void add_player(sprite_t* p);
	static void remove_all_players();
	static int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
private:
	enum {
		seesaw_player_move	= 60002,
		seesaw_player_enter	= 60003
	};

	struct PosType {
		PosType(uint16_t xpos = 0, uint16_t ypos = 0, uint8_t dir = 0)
			{ x = xpos; y = ypos; direction = dir; }
		uint16_t x, y;
		uint8_t  direction;
	};
	// typedefs
	typedef std::map<userid_t, PosType>  PlayerCoordMap;

	//
	static int  handle_player_move(const uint8_t body[], int len);
	//
	static void remove_player();
	//
	static void rsp_player_enter(sprite_t* p);
	static void rsp_seesaw_status();	

	static PlayerCoordMap player_pos_map_;
	static uint8_t        pkg_[1024];
	static sprite_t*      cur_user_;
};

//----------------------------------------------------------------------
// Public Methods
//
inline void
Seesaw::add_player(sprite_t* p)
{
	player_pos_map_[p->id] = PosType(p->posX, p->posY, p->direction);

	p->action = ACTION_SIT;
	rsp_player_enter(p);

	assert(player_pos_map_.size() <= MAX_PLAYERS_PER_GAME);
}

//
inline void
Seesaw::remove_all_players()
{
	assert(player_pos_map_.size() == 0);
}

//
inline int
Seesaw::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	cur_user_ = p;

	int err = 0;
	switch (cmd) {
	case seesaw_player_move:
		err = handle_player_move(body, len);
		break;
	case PROTO_GAME_LEAVE:
		remove_player();
		break;
	case PROTO_SEESAW_INFO:
		rsp_seesaw_status();
		break;
	default:
		ERROR_RETURN(("%s: Unrecognized Command ID %d from uid=%u", p->group->game->name, cmd, p->id), -1);
	}

	return err;
}

//----------------------------------------------------------------------
// Private Methods
//
inline void
Seesaw::rsp_player_enter(sprite_t* p)
{
	int i = sizeof(protocol_t);

	taomee::pack(pkg_, p->id, i);
	taomee::pack(pkg_, p->posX, i);
	taomee::pack(pkg_, p->posY, i);
	taomee::pack(pkg_, p->direction, i);

	init_proto_head(pkg_, seesaw_player_enter, i);
	send_to_map(p, pkg_, i, 1);
}

//
inline void
Seesaw::remove_player()
{
	player_pos_map_.erase(cur_user_->id);
}

#endif // ANDY_SEESAW_HPP_
