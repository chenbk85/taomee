#include "../client_based_racing_game.hpp"
#include "surf_riding_rule.hpp"

extern "C" void* create_game(game_group_t* grp)
{
	return new (std::nothrow) ClientBasedRacingGame<SurfRidingRule>(grp);
}

extern "C" int game_init()
{
	return SurfRidingRule::load_tile_grps();
}

extern "C" void game_destroy()
{
}
