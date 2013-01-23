/** 
 * ========================================================================
 * @file battle.cpp
 * @brief 
 * @version 1.0
 * @date 2012-02-14
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "battle.hpp"

int wrap_send_to_battle(c_room * p_room, void * data)
{
    btlsw_proto_out_t * p_data = (btlsw_proto_out_t *)data;
    btlsw_proto_t * p_header = p_data->p_header;
    Cmessage * p_out = p_data->p_out;

    return send_to_room(p_room, p_header, p_out);
}


int send_to_battle(c_battle * p_battle, btlsw_proto_t * p_header, Cmessage * p_out)
{
    btlsw_proto_out_t out;
    out.p_header = p_header;
    out.p_out = p_out;

    p_battle->traverse_room(wrap_send_to_battle, &out);
    return 0;
}

int send_to_battle(c_battle * p_battle, c_player * p, Cmessage * p_out)
{
    btlsw_proto_t header;
    init_proto_head(&header, p->m_id, sizeof(header), p->m_waitcmd, p->m_ret, p->m_seq);

    return send_to_battle(p_battle, &header, p_out);
}


int new_battle(c_room *p_host, c_battle **pp_battle)
{
    if (!p_host) {
        return -1;
    }

    c_battle * p_battle = new c_battle(p_host);
    if (pp_battle) {
        *pp_battle = p_battle;
    }

    return 0;
}

int wrap_del_battle(c_room * p_room, void * param)
{
    p_room->set_battle(NULL);
    return 0;
}

int del_battle(c_battle * p_battle)
{
    p_battle->traverse_room(wrap_del_battle, NULL);
    delete p_battle;
    return 0;
}
