/** 
 * ========================================================================
 * @file dispatch.hpp
 * @brief 
 * @version 1.0
 * @date 2011-12-08
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DISPATCH_H_2011_12_08
#define H_DISPATCH_H_2011_12_08

#include <set>

#include <libtaomee++/inet/byteswap.hpp>
#include <libtaomee++/inet/pdumanip.hpp>


extern "C" 
{
#include <assert.h>
#include <glib.h>

#include <libtaomee/timer.h>
#include <libtaomee/project/types.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/utils.h>

#include <async_serv/net_if.h>
}

#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>

#include "pea_common.hpp"
#include "online.hpp"
#include "player.hpp"


#define MAX_CACHE_PKG		(50)



int dispatch(void * data, fdsession_t* fdsess, bool cache_cmd_flag);
void proc_cached_pkg();

#endif
