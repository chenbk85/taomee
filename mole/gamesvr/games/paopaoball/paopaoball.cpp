#include <new>
#include "paopao.hpp"

/** 抛抛球游戏初始化类 */
/**
 * @brief  每次新抛抛球游戏时，获得处理卡牌初始化类的指针
 * @param  game_group_t* grp 新游戏的组
 * @return void* 卡牌初始化管理类指针
 */
extern "C" void* create_game(game_group_t* grp)
{
    return new (std::nothrow) Paopao(grp);
}


/**
 * @brief  框架调用的三个接口之一。初始化游戏的接口
 * @param  None
 * @return int类型的结果码
 */
extern "C" int game_init()
{
    return 0;
}


/**
 * @brief  框架调用的三个接口之一，游戏结束时调用，清理资源
 * @param  None
 * @return void
 */
extern "C" void game_destroy()
{
}
