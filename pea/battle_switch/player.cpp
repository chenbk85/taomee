/** 
 * ========================================================================
 * @file player.cpp
 * @brief 
 * @version 1.0
 * @date 2011-12-12
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "player.hpp"
#include "online.hpp"



void init_proto_head(void *buf, c_player *p, uint16_t cmd)
{
	if (!cmd) {
		init_proto_head(buf, p->m_id, sizeof(btlsw_proto_t),
				p->m_waitcmd, p->m_ret, p->m_seq);
		return ;
	}
	init_proto_head(buf, p->m_id, sizeof(btlsw_proto_t), cmd, p->m_ret, p->m_seq);
}

c_player* get_player_from_online(int fd, uint32_t uid)
{
	c_online *p_online = get_online_by_fd(fd);
	if (!p_online) return NULL;

	c_player *p_player = p_online->get_player(uid);
	if (!p_player) return NULL;

	return p_player;
}

int send_to_player(c_player *p, btlsw_proto_t *p_header, Cmessage *p_out, bool complete)
{
	assert(p);

    p_header->id = p->m_id;

    TRACE_TLOG("send_to_player[%u]: u=%u, online_id=%u, seq=%u, ret=%u",
			p_header->cmd, p->m_id, p->get_online()->m_id,
			p_header->seq, p_header->ret);

    if (-1 == send_msg_to_client(p->get_online()->m_fdsess, (char *)p_header, p_out)) {
		ERROR_TLOG("send_to_player failed[%u] u=%u, online_id=%u, seq=%u",
				p_header->cmd, p->m_id, p->get_online()->m_id, p_header->seq);
		return -1;
	}

    if (complete) {
        p->clear_waitcmd();
    }

	return 0;
}

int send_to_player(c_player *p, Cmessage * p_out, bool complete)
{
	assert(p);

    btlsw_proto_t header;
    init_proto_head(&header, p->m_id, sizeof(header), p->m_waitcmd, p->m_ret, p->m_seq);

    return send_to_player(p, &header, p_out, complete);
}

int send_to_player(c_player *p, uint16_t cmd, Cmessage *p_out, bool complete)
{
	assert(p);

    if (!cmd) {
        return send_to_player(p, p_out, complete);
    }

    btlsw_proto_t header;
    init_proto_head(&header, p->m_id, sizeof(header), cmd, p->m_ret, p->m_seq);

    return send_to_player(p, &header, p_out, complete);
}

c_player *alloc_player(c_online *p_online, uint32_t uid,
		uint32_t online_id, uint32_t role_tm)
{
    c_player *p_player = new c_player(p_online, uid, online_id, role_tm);
    if (!p_player) {
		ERROR_TLOG("failed to create player, u=%u, online=%u, role_tm=%u",
				uid, online_id, role_tm);
        return NULL;
    }

    p_online->add_player(p_player);
    return p_player;
}

int destroy_player(c_player *p)
{
	c_online *p_online = p->get_online();
	if (p->is_has_room()) {
        c_room *p_room = p->get_room();
        uint8_t user_seat = p->get_seat();
        p_room->del_player(user_seat);
		p->leave_room();

        DEBUG_TLOG("player leave room, u=%u, olid=%u, room=%u",
				p->m_id, p_online->m_id, p_room->get_room_id());

        if (p_room->is_empty()) {
            p_online->destroy_room(p_room);
            return 0;
        }

        btlsw_pvp_abnormal_leave_room_out out;

        out.online_id = p_online->m_id;
        out.room_id = p_room->get_room_id();
        out.host_seat = p_room->get_host_seat();
        out.host_id = p_room->get_host_id();
        out.user_id = p->m_id;
        out.user_seat = user_seat;

        btlsw_proto_t header;
        init_proto_head(&header, 0, sizeof(header), btlsw_pvp_abnormal_leave_room_cmd, 0, 0);
        send_to_room(p_room, &header, &out);
    }
	p_online->del_player(p);	
    delete p;
    return 0;
}
