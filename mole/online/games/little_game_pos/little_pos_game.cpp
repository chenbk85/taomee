extern "C" {
#include <dll.h>
#include "hallowmas.h"
}

#include "little_pos.hpp"

extern "C"
int on_game_begin(sprite_t* p)
{
	game_group_t* grp = p->group;
	DEBUG_LOG("COW MILK BEGIN\t[uid=%u gid=%u grpid=%u action=%u]", p->id, grp->game->id, grp->id, p->action);
	
	if (grp->game->id == 68) {
		LittlePos::update_players_info(grp, p, 0);
	}

	return 0;	
}

extern "C"
int on_game_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	return LittlePos::handle_data(p, cmd, body, len);
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
