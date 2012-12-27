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

#include "cheersquad_show.hpp"

uint32_t cut_sheep_cheer[3];
uint32_t fisher_cheer[4];
uint32_t fisher2_cheer[3];
uint32_t tree_status = 0;
uint32_t barber_cheer[3];

void CheerSquadShow::new_rsp_cheer_squad_show_ok(game_group_t* grp)
{
    uint8_t buf[256];
    int len = sizeof(protocol_t);

    int flag = rand() % 2;
    PKG_UINT32(buf, flag, len);
    init_proto_head(buf, PROTO_cheer_squad_show_new_OK, len);

    if (!(grp->players[0])->tiles)
        return;

    send_to_group(grp, buf, len);
}

void CheerSquadShow::rsp_cheer_squad_show_ok(game_group_t* grp)
{
	sprite_t* pp = grp->players[0];

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, grp->count, j);
	int loop;
	for (loop = 0; loop < grp->count; loop++) {
		sprite_t* p = grp->players[loop];
		if (p) {
			PKG_UINT32(msg, p->id, j);
		}
	}
	tree_status = (tree_status + 1) % 3;
	PKG_UINT32(msg, tree_status, j);
	init_proto_head(msg, PROTO_cheer_squad_show_OK, j);
	send_to_map(pp, msg, j, 0);
}

void CheerSquadShow::update_players_info(game_group_t* grp, sprite_t* p, uint32_t in_or_out)
{
	int out_lab;
	int loop;
	if (in_or_out) {  // 1  out; 0, in
		for (loop = 0; loop < 3; loop++) {
			if (cut_sheep_cheer[loop] == p->id) {
				cut_sheep_cheer[loop] = 0;
				out_lab = loop;
			}
		}
	} else {
		if (p->gitem->id < 7 || p->gitem->id > 9) {
			return;
		}
		cut_sheep_cheer[p->gitem->id - 7] = p->id;
	}
	
	int j = sizeof(protocol_t);
	for (loop = 0; loop < 3; loop++) {
		if (in_or_out && loop == out_lab) {
			PKG_UINT32(msg, p->id, j);
			PKG_UINT32(msg, 0, j);  // out
		} else {
			PKG_UINT32(msg, cut_sheep_cheer[loop], j);
			PKG_UINT32(msg, !!(cut_sheep_cheer[loop]), j);  //in 
		}
	}
	DEBUG_LOG("PLAYER INFO\t[%u %u %u %u]", p->id, cut_sheep_cheer[0], cut_sheep_cheer[1], cut_sheep_cheer[2]);
	init_proto_head(msg, PROTO_GET_cut_sheep_cheer, j);
	return send_to_map(p, msg, j, 0);
}

void CheerSquadShow::update_fishers_info(game_group_t* grp, sprite_t* p, uint32_t in_or_out)
{
	uint32_t* fc;
	int fish_count;
	if (grp->game->id == 58) {
		fc = fisher_cheer;
		fish_count = 4;
	}
	else {
		fc = fisher2_cheer;
		fish_count = 3;
	}
	
	int out_lab;
	int loop;
	if (in_or_out) {  // 1  out; 0, in
		for (loop = 0; loop < fish_count; loop++) {
			if (fc[loop] == p->id) {
				fc[loop] = 0;
				out_lab = loop;
			}
		}
	} else {
		if (p->gitem->id < 1 || p->gitem->id > 5) {
			return;
		}
		if (grp->game->id == 58) {
			fc[p->gitem->id - 2] = p->id;
		} else {
			fc[p->gitem->id - 1] = p->id;
		}
	}
	
	int j = sizeof(protocol_t);
	for (loop = 0; loop < fish_count; loop++) {
		if (in_or_out && loop == out_lab) {
			PKG_UINT32(msg, p->id, j);
			PKG_UINT32(msg, 0, j);  // out
		} else {
			PKG_UINT32(msg, fc[loop], j);
			PKG_UINT32(msg, !!(fc[loop]), j);  //in 
		}
	}
	DEBUG_LOG("FISHER INFO\t[%u %u %u %u]", p->id, fc[0], fc[1], fc[2]);
	init_proto_head(msg, PROTO_fisher_item_info, j);
	return send_to_map(p, msg, j, 0);
}

void CheerSquadShow::update_barbers_info(game_group_t* grp, sprite_t* p, uint32_t in_or_out)
{
	uint32_t* fc = barber_cheer;
	int barber_count = 3;
		
	int out_lab;
	int loop;
	if (in_or_out) {  // 1  out; 0, in
		for (loop = 0; loop < barber_count; loop++) {
			if (fc[loop] == p->id) {
				fc[loop] = 0;
				out_lab = loop;
			}
		}
	} else {
		if (p->gitem->id < 1 || p->gitem->id > 3) {
			return;
		}
		fc[p->gitem->id - 1] = p->id;
	}
	
	int j = sizeof(protocol_t);
	for (loop = 0; loop < barber_count; loop++) {
		if (in_or_out && loop == out_lab) {
			PKG_UINT32(msg, p->id, j);
			PKG_UINT32(msg, 0, j);  // out
		} else {
			PKG_UINT32(msg, fc[loop], j);
			PKG_UINT32(msg, !!(fc[loop]), j);  //in 
		}
	}
	DEBUG_LOG("BARBER INFO\t[%u %u %u %u]", p->id, fc[0], fc[1], fc[2]);
	init_proto_head(msg, PROTO_barber_item_info, j);
	return send_to_map(p, msg, j, 0);
}

void CheerSquadShow::broad_question(game_group_t* grp)
{
	char_indx = rand() % 68;
	
	uint8_t buf1[256];
	int len = sizeof(protocol_t);
	PKG_UINT32(buf1, strlen(char_match[char_indx]), len);
	PKG_STR(buf1, char_match[char_indx], len, strlen(char_match[char_indx]));
	init_proto_head(buf1, PROTO_broad_question, len);
	if (grp->players[0])
		send_to_map(grp->players[0], buf1, len, 0);
}


