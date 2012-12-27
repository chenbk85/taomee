#ifndef LITTLE_POS_HPP_
#define LITTLE_POS_HPP_

extern "C" {
#include <libtaomee/timer.h>
#include <libtaomee/log.h>

#include "benchapi.h"
#include "map.h"
#include "npc.h"
#include "proto.h"
}

extern uint32_t cow_milk_pos[3];

class LittlePos{
public:
	//
	static int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
	static void	update_players_info(game_group_t* grp, sprite_t* p, uint32_t in_or_out);
private:
	// CMD ID
	enum {
		PROTO_cow_item_info		  = 60017,
		PROTO_get_user_info		  = 60019,
		PROTO_user_pos_info		  = 60018,
	};
};

//
inline int
LittlePos::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	int err = 0;
	switch (cmd) {
	case PROTO_GAME_LEAVE:
	{	
		if (p->group->game->id == 68) {
			LittlePos::update_players_info(p->group, p, 1);
		}
		return 0;
	}
	case PROTO_cow_item_info:
    {
        if (p->group->game->id == 68) {
			uint32_t pos, i = 0;
			UNPKG_UINT32(body, pos, i);
			uint8_t buf[pagesize];
            int j = sizeof(protocol_t);
			PKG_UINT32(buf, pos, j);
			init_proto_head(buf, p->waitcmd, j);
            send_to_map(p, buf, j, 1);
			return 0;
        }
    }
	case PROTO_get_user_info:
	{
		if (p->group->game->id == 68) {
			uint32_t i = 0;
			uint8_t buf[pagesize];
            int j = sizeof(protocol_t);
			for (i = 0; i < 3; i++) {
				PKG_UINT32(buf, cow_milk_pos[i], j);
			}
			init_proto_head(buf, p->waitcmd, j);
            send_to_self(p, buf, j, 1);
			return 0;
        }
	}
	default:
		ERROR_RETURN( ("%s: Unrecognized Command ID %d from uid=%u",
						p->group->game->name, cmd, p->id), -1 );
	}

	return err;
}


#endif // ANDY_MIMIC_SHOW_HPP_
