/**
 * =====================================================================================
 *       @file  tetris_game.cpp
 *      @brief  俄罗斯方块游戏的框架接口调用
 *
 *   @internal
 *     Created  2008年10月22日 13时52分34秒
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee .Inc
 *   Copyright  Copyright (c) 2008, aceway
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <new>
#include "tetris.hpp"


/**
 * @brief  框架调用的三个接口之一。 创建游戏实例， 返回指向实例的指针, 上层已经初始化好了.
 * @param  game_group_t* grp, 指向游戏组的指针
 * @return void*, 实为 new 出来的游戏对象的指针
 */
extern "C" void* create_game(game_group_t* grp)
{
    return new (std::nothrow) Tetris(grp);
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
