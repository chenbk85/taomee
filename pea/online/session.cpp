/** 
 * ========================================================================
 * @file session.cpp
 * @brief 
 * @version 1.0
 * @date 2012-04-25
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

// #include <libtaomee++/inet/pdumanip.hpp>
#include "session.hpp"
#include "dbproxy.hpp"
#include "db_player.hpp"


using namespace taomee;

int check_session(player_t * p, uint32_t del_flag)
{
    uint32_t game_id = GAME_ID;
    uint32_t session_len = SESSION_LEN;

    char pkg[128] = {0};
    int idx = 0;


    pack_h(pkg, game_id, idx);
    pack(pkg, p->session, session_len, idx);
    pack_h(pkg, del_flag, idx);

    return send_to_db(p, p->id, p->role_tm, db_check_session_cmd, pkg, idx);
}


int check_session_callback(player_t * p, const uint8_t * body, uint32_t body_len, int ret)
{

    TRACE_TLOG("user: %u, check session callback, waitcmd: %u", p->id, p->waitcmd);

    switch (p->waitcmd)
    {
        case cli_proto_login_cmd:
            return db_get_player(p);
        default:
            break;
    }
    
    return -1;
}
