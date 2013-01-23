/** 
 * ========================================================================
 * @file common.hpp
 * @brief 
 * @version 1.0
 * @date 2012-02-09
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_COMMON_H_2012_02_09
#define H_COMMON_H_2012_02_09


#include <string.h>
#include <stdio.h>
#include <list>
#include <map>

extern "C"
{
#include <libtaomee/project/types.h>
#include <async_serv/net_if.h>
#include <libtaomee/timer.h>
}

extern "C"
{
#include <libtaomee/conf_parser/config.h> 

}
#include "pea_common.hpp"

class c_player;
class c_online;
class c_room;
class c_pvp_room;
class c_battle;

#endif
