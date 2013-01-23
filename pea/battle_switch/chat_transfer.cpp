#include "proto.hpp"
#include "common.hpp"
#include "online.hpp"
#include "room.hpp"
#include "chat_transfer.hpp"

int btlsw_chat_msg_transfer(DEFAULT_ARG)
{
	btlsw_chat_msg_transfer_in *p_in = P_IN;

    c_online *p_online = p->get_online();
	c_room *p_room = p->get_room();

    if (!p_online) {
        ERROR_TLOG("chat nofound online, u=%u", p->m_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);

        return 0;
    }

    if (!p_room) {
        ERROR_TLOG("nofound room, olid=%u, u=%u", p_online->m_id, p->m_id);
        p->m_ret = BTLSW_ERR_NO_ROOM_FOUND;
		send_to_player(p);
        return 0;
    }
        
    // 应答
    p->m_waitcmd = btlsw_chat_msg_transfer_cmd;
    send_to_room(p_room, p, p_in);
    TRACE_TLOG("u=%u, send msg to room %u", p->m_id, p_room->get_room_id());

	return 0;
}

