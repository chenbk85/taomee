/**
 * =====================================================================================
 *       @file  memcache_key.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  09/08/2011 02:07:37 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef H_MEM_KEY_H_20110908
#define H_MEM_KEY_H_20110908

enum
{
    //用户role信息
    MEMKEY_ROLE         = 1,
    //房间装饰信息
    MEMKEY_ROOM         = 2,
    //天限制
    MEMKEY_DAY_RESTRICT = 4,
    //小游戏总分
    MEMKEY_GAME_SCORE   = 5,
};

#endif //H_MEM_KEY_H_20110908
