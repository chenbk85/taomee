#ifndef ANDY_MIMIC_SHOW_HPP_
#define ANDY_MIMIC_SHOW_HPP_

extern "C" {
#include <libtaomee/timer.h>
#include <libtaomee/log.h>

#include "benchapi.h"
#include "map.h"
#include "npc.h"
#include "proto.h"
}

class MimicShow {
public:
	//
	static void check_dress(game_group_t* grp);
	//
	static int  create_model();
	//
	static int  load_clothes();
	//
	static int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
	//
	static void rsp_mimic_show_info(sprite_t* p = 0, bool tomap = false, bool p_included = true);
private:
	// CMD ID
	enum {
		mimic_show_bonus_noti   = 60007
	};

	// Const
	enum {
		clothes_max_kind        = 6,
		clothes_max_each_kind   = 200
	};

	struct Clothes {
		int        probability;
		int        cnt;
		uint32_t   clothes[clothes_max_each_kind];
	};

	//
	static int  clothes_parser(xmlNodePtr cur_node);
	//
	static int  change_dress_event(void* owner = 0, void* data = 0);
	static int  next_show(void* owner = 0, void* data = reinterpret_cast<void*>(2));
	//
	static bool is_dress_matched(const game_group_t* grp);
	//
	static void change_npc_dress();
	static void set_npc_init_pos();
	//--------------------------------------------------------------
	// Data Members
	//
	// current show:
	//    0 for clothing mimic game;
	//    10000 for no game nor shows;
	//    1 -- N for shows at daytime;
	//    10001 - N for shows at night.
	static uint32_t  cur_show_;
	static uint32_t  cur_show_no_;
	// model for clothing mimic game
	static sprite_t* npc_;
	// clothes to put on
	static Clothes   clothes_[clothes_max_kind];
	//
	static const int dress_chg_intvl  = 60 * 7;
	static const int mimic_game_intvl = 10;
	static const int game_mapid       = 35;
};

//----------------------------------------------------------------------
// Public Methods
//
inline int
MimicShow::create_model()
{
	npc_ = new_npc("", 0xFEB4DA, 0, 0);
	if ( npc_ ) {
		enter_map(npc_, game_mapid, 0,0);

		set_npc_init_pos();
		change_dress_event(npc_);
		return 0;
	}

	ERROR_RETURN( ("MimicShow: Failed to Create Model NPC"), -1);
}

//
inline int
MimicShow::load_clothes()
{
	static const char* file = "./games/clothes.xml";

	return load_xmlconf(file, clothes_parser);
}

//
inline int
MimicShow::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	int err = 0;
	switch (cmd) {
	case PROTO_MIMIC_SHOW_INFO:
		//response_proto_uint32(p, PROTO_MIMIC_SHOW_INFO, cur_show_, 0);
		rsp_mimic_show_info(p);
		break;
	case PROTO_GAME_LEAVE:
		// TODO
		rsp_mimic_show_info(p, true, false);
		break;
	default:
		ERROR_RETURN( ("%s: Unrecognized Command ID %d from uid=%u",
						p->group->game->name, cmd, p->id), -1 );
	}

	return err;
}


//----------------------------------------------------------------------
// Private Methods
//
inline int
MimicShow::change_dress_event(void* owner, void* data)
{
	change_npc_dress();

	time_t expire = get_now_tv()->tv_sec + dress_chg_intvl;
	if (owner) {
		ADD_TIMER_EVENT(npc_, change_dress_event, 0, expire);
		DEBUG_LOG("CHG DRESS EVENT\t[now=%ld expire=%ld]", get_now_tv()->tv_sec, expire);
	}

	return 0;
}

//
inline void
MimicShow::set_npc_init_pos()
{
	npc_->posX    = 496;
	npc_->posY    = 137;
	npc_->action  = ACTION_DANCE;
}

#endif // ANDY_MIMIC_SHOW_HPP_
