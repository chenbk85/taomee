/**
 *============================================================
 *  @file      fwd_decl.hpp
 *  @brief    forward declarations
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef __FWD_DECL_HPP__
#define __FWD_DECL_HPP__

#include <ctime>
#include <map>

extern "C" {
#include <glib.h>

#include <libtaomee/list.h>
#include <libtaomee/project/types.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
}

#include <libtaomee++/proto/proto_util.h>

struct timer_head_t 
{
    list_head_t timer_list;
};


extern timer_head_t g_events;

class player_t;



#endif // __FWD_DECL_HPP__

