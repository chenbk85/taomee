/** 
 * ========================================================================
 * @file update_notice.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-31
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "update_notice.h"
#include "db.h"
#include "itl_common.h"


timer_head_t g_update_event;

int init_update_timely(void * owner, void * data)
{
    ADD_TIMER_EVENT(&g_update_event, init_update_timely, NULL, get_now_tv()->tv_sec + GET_CONFIG_UPDATE_INTERVAL);
    db_get_update_notice();

    return 0;
}


int init_update_notice()
{
    init_update_timely(0, 0);

    DEBUG_LOG("finish init_update_notice, GET_CONFIG_UPDATE_INTERVAL: %d", GET_CONFIG_UPDATE_INTERVAL);
    return 0;
}

