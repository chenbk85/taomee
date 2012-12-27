#ifndef CHEER_SQUAD_SHOW_HPP_
#define CHEER_SQUAD_SHOW_HPP_

extern "C" {
#include <libtaomee/timer.h>
#include <libtaomee/log.h>

#include "benchapi.h"
#include "map.h"
#include "npc.h"
#include "proto.h"
}

extern uint32_t cut_sheep_cheer[3];
extern uint32_t fisher_cheer[4];
extern uint32_t fisher2_cheer[3];
extern uint32_t tree_status;
extern uint32_t barber_cheer[3];

class CheerSquadShow {
public:
	static void new_rsp_cheer_squad_show_ok(game_group_t* grp);
	static void rsp_cheer_squad_show_ok(game_group_t* grp);
	static int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
	static void	broad_question(game_group_t* grp);
	static void	update_players_info(game_group_t* grp, sprite_t* p, uint32_t in_or_out);
	static void	update_fishers_info(game_group_t* grp, sprite_t* p, uint32_t in_or_out);
	static void update_barbers_info(game_group_t* grp, sprite_t* p, uint32_t in_or_out);
private:
	// CMD ID
	enum {
		PROTO_cheer_squad_show_OK = 60011,
		PROTO_broad_question	  = 60012,
		PROTO_item_info	  		  = 60013,
		PROTO_fisher_item_info	  = 60014,
		PROTO_get_chris_tree_stat = 60015,
		PROTO_barber_item_info	  = 60016,
		PROTO_cheer_squad_show_new_OK = 60020,
	};
};

inline int
CheerSquadShow::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	int err = 0;
	switch (cmd) {
	case PROTO_item_info:
	{	
		if (p->group->game->id == 45) {
			int j = sizeof(protocol_t);
			int loop;
			for (loop = 0; loop < 3; loop++) {
				PKG_UINT32(msg, cut_sheep_cheer[loop], j);
				int in_out = (cut_sheep_cheer[loop]) ? 1 : 0;
				PKG_UINT32(msg, in_out, j);
			}
			init_proto_head(msg, PROTO_item_info, j);
			return send_to_self(p, msg, j, 1);
		}
	}
	case PROTO_fisher_item_info:
	{	
		uint32_t* fc;
		int fish_count;
		if (p->group->game->id == 58) {
			fc = fisher_cheer;
			fish_count = 4;
		} else if (p->group->game->id == 65) {
			fc = fisher2_cheer;
			fish_count = 3;
		} else {
			ERROR_RETURN(("%s: Unknow game id [%u %u]", p->group->game->name, p->id, cmd), -1 );
		}
		
		int j = sizeof(protocol_t);
		int loop;
		for (loop = 0; loop < fish_count; loop++) {
			PKG_UINT32(msg, fc[loop], j);
			int in_out = (fc[loop]) ? 1 : 0;
			PKG_UINT32(msg, in_out, j);
		}
		init_proto_head(msg, PROTO_fisher_item_info, j);
		return send_to_self(p, msg, j, 1);
	}
	case PROTO_GAME_LEAVE:
	{	
		if (p->group->game->id == 45) {
			CheerSquadShow::update_players_info(p->group, p, 1);
		}
		if (p->group->game->id == 58 || p->group->game->id == 65) {
			CheerSquadShow::update_fishers_info(p->group, p, 1);
		}
		if (p->group->game->id == 66) {
			CheerSquadShow::update_barbers_info(p->group, p, 1);
		}
		return 0;
	}
	case PROTO_get_chris_tree_stat:
	{
		response_proto_uint32(p, cmd, tree_status, 0);
		return 0;
	}
	case PROTO_barber_item_info:
	{	
		if (p->group->game->id == 66) {
			int j = sizeof(protocol_t);
			int loop;
			for (loop = 0; loop < 3; loop++) {
				PKG_UINT32(msg, barber_cheer[loop], j);
				int in_out = (barber_cheer[loop]) ? 1 : 0;
				PKG_UINT32(msg, in_out, j);
			}
			init_proto_head(msg, cmd, j);
			return send_to_self(p, msg, j, 1);
		}
	}
	default:
		ERROR_RETURN( ("%s: Unrecognized Command ID %d from uid=%u",
						p->group->game->name, cmd, p->id), -1 );
	}

	return err;
}


#endif // ANDY_MIMIC_SHOW_HPP_
