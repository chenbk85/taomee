extern "C" {
#include "dll.h"
}

#include "mimic_show.hpp"

extern "C"
int on_game_begin(sprite_t* p)
{
	MimicShow::rsp_mimic_show_info(p, true);

	game_group_t* grp = p->group;
	if (grp->count == grp->game->players) {
		MimicShow::check_dress(grp);
		DEBUG_LOG("MimicShow Ended\t[grpid=%u]", grp->id);
		end_game(grp, p, 0);
	}

	return 0;
}

extern "C"
int on_game_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	return MimicShow::handle_data(p, cmd, body, len);
}

extern "C"
void on_game_end(game_group_t* gamegrp)
{
}

extern "C"
int game_init()
{
	int err = MimicShow::load_clothes();
	if (!err) {
		err = MimicShow::create_model();
	}
	return err;
}

extern "C"
void game_destroy()
{
}
