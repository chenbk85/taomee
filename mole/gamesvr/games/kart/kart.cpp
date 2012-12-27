#include "../client_based_racing_game.hpp"
#include "kart_rule.hpp"

extern "C" void* create_game(game_group_t* grp)
{
	return new (std::nothrow) ClientBasedRacingGame<KartRule, DownCounter1>(grp);
}

extern "C" int game_init()
{
	return 0;
}

extern "C" void game_destroy()
{
}
