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
#include "pea_common.hpp"
#include "player.hpp"


using namespace taomee;

int check_session(player_t * p, uint32_t del_flag)
{
    uint32_t game_id = GAME_ID;
    uint32_t session_len = SESSION_LEN;
    c_player * p_player = (c_player *)(p->tmpinfo);

    int idx = 0;


    pack_h(g_pkg, game_id, idx);
    pack(g_pkg, p_player->session, session_len, idx);
    pack_h(g_pkg, del_flag, idx);

    return send_to_dbproxy(p, db_check_session_cmd, g_pkg, idx);
}


int check_session_callback(player_t * p, const uint8_t * body, uint32_t body_len, int ret)
{

    TRACE_TLOG("user: %u, check session callback, waitcmd: %u", p->uid, p->waitcmd);

    switch (p->waitcmd)
    {
        case proto_get_server_list_cmd:
            return do_proto_get_server_list(p, NULL, NULL, NULL);
        case proto_get_role_list_cmd:
            break;
        default:
            break;
    }
    
    return -1;
}
