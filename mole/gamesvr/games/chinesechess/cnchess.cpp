#include <new>

#include "../chessgame.hpp"
#include "../reqhandler.hpp"
#include "cnchessrule.hpp"

extern "C" void* create_game(game_group_t* grp)
{
	return new (std::nothrow) ChessGame<ChineseChessRule, ReqHandler>(grp);
}

extern "C" int game_init()
{
	return 0;
}

extern "C" void game_destroy()
{
}
