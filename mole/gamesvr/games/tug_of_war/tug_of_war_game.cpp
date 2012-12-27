#include <new>

#include "tug_of_war.hpp"

/**
 * @brief 创建一个游戏服务对象
 * @param grp 游戏服务对象需要服务的游戏组
 * @return 游戏服务对象的地址，它是一个 @ref mpog 的派生类对象
 */
extern "C" void* create_game(game_group_t* grp)
{
	return new (std::nothrow) TugOfWar(grp);
}

extern "C" int game_init()
{
	return 0;
}

extern "C" void game_destroy()
{
}

