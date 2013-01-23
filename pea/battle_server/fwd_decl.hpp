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

#ifndef KFBTL_FWD_DECL_HPP_
#define KFBTL_FWD_DECL_HPP_

#include <ctime>
#include <set>

extern "C" {
#include <glib.h>

#include <libtaomee/list.h>
#include <libtaomee/project/types.h>
}

#include <libtaomee++/pathfinder/astar/astar.hpp>
#include <libtaomee++/event/event_mgr.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>


#define YJ_DEBUG_LOG(fmt, args...) \
    do { INFO_LOG(fmt, ##args); } while(0)

#endif // KF_FWD_DECL_HPP_

