#include "seesaw.hpp"

extern "C"
int on_game_begin(sprite_t* p)
{
	Seesaw::add_player(p);
	return 0;
}

extern "C"
int on_game_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	return Seesaw::handle_data(p, cmd, body, len);
}

extern "C"
void on_game_end(game_group_t* gamegrp)
{
	Seesaw::remove_all_players();
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
