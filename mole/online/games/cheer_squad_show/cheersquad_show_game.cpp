extern "C" {
#include "dll.h"
#include "hallowmas.h"
}

#include "cheersquad_show.hpp"

extern "C"
int on_game_begin(sprite_t* p)
{
	game_group_t* grp = p->group;
	DEBUG_LOG("CheerSquadShow BEGIN\t[uid=%u gid=%u grpid=%u action=%u]", p->id, grp->game->id, grp->id, p->action);

	if (grp->game->id == 92 && grp->count == p->group->game->players) {
		CheerSquadShow::new_rsp_cheer_squad_show_ok(grp);
		DEBUG_LOG("CheerSquadShow Ended\t[grpid=%u]", grp->id);
		end_game(grp, p, 0);
		return 0;
	}
		
	if (grp->game->id == 57 && grp->count == p->group->game->players) {
		CheerSquadShow::rsp_cheer_squad_show_ok(grp);
		DEBUG_LOG("CheerSquadShow Ended\t[grpid=%u]", grp->id);
		end_game(grp, p, 0);
		return 0;
	}
	
	if (grp->game->id == 45)
		CheerSquadShow::update_players_info(grp, p, 0);
	if (grp->game->id == 58 || grp->game->id == 65)
		CheerSquadShow::update_fishers_info(grp, p, 0);
	if (grp->game->id == 66)
		CheerSquadShow::update_barbers_info(grp, p, 0);
	return 0;	
}

extern "C"
int on_game_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	return CheerSquadShow::handle_data(p, cmd, body, len);
}

extern "C"
void on_game_end(game_group_t* gamegrp)
{
}

extern "C"
int game_init()
{
	return 0;
}

extern "C"
void game_destroy()
{
}
