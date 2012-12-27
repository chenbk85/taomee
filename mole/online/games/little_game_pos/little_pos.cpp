#include <cstdlib>

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/utils.h>

#include "dbproxy.h"
#include "logic.h"
#include "small_require.h"
#include "communicator.h"
#include "mole_class.h"

}

#include "little_pos.hpp"

uint32_t cow_milk_pos[3];


void LittlePos::update_players_info(game_group_t* grp, sprite_t* p, uint32_t in_or_out)
{
	int out_lab;
	int loop;
	if (in_or_out) {  // 1  out; 0, in
		for (loop = 0; loop < 3; loop++) {
			if (cow_milk_pos[loop] == p->id) {
				cow_milk_pos[loop] = 0;
				out_lab = loop;
			}
		}
	} else {
		if (p->gitem->id < 11 || p->gitem->id > 13) {
			
			DEBUG_LOG("update_players_info \t[%u  p->gitem->id:%u]", p->id, p->gitem->id);
			
			return;
		}
		cow_milk_pos[p->gitem->id - 11] = p->id;
	}
	
	int j = sizeof(protocol_t);
	for (loop = 0; loop < 3; loop++) {
		if (in_or_out && loop == out_lab) {
			PKG_UINT32(msg, 0, j);  // out
		} else {
			PKG_UINT32(msg, (cow_milk_pos[loop]), j);  //in 
		}
	}
	DEBUG_LOG("PLAYER INFO\t[%u %u %u %u]", p->id, cow_milk_pos[0], cow_milk_pos[1], cow_milk_pos[2]);
	init_proto_head(msg, PROTO_user_pos_info, j);
	return send_to_map(p, msg, j, 0);
}
