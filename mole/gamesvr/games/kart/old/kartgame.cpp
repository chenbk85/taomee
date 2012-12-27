#include "kartgame.hpp"

double  KartGame::max_proc_intv = 0.08; // maximun time interval between each call of process

/*---------------------------------------------------------
  *			public methods
  *---------------------------------------------------------*/
// none

/*---------------------------------------------------------
  *			private methods
  *---------------------------------------------------------*/
// none

/*---------------------------------------------------------
  *			global methods
  *---------------------------------------------------------*/
extern "C" void* create_game(game_group_t* grp)
{
	return new (std::nothrow) KartGame(grp);
}

extern "C" int game_init()
{
	return 0;
}

extern "C" void game_destroy()
{
}
