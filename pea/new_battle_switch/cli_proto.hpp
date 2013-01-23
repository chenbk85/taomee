#ifndef __PROTO_IFACE_HPP__
#define __PROTO_IFACE_HPP__


#include "common.hpp"

#include "proto.hpp"
#include "config.hpp"
#include "dispatch.hpp"
#include "online.hpp"
#include "player.hpp"
#include "room.hpp"
#include "seat.hpp"
#include "team.hpp"




int send_msg_to_player_base(player_t *p, void *head, Cmessage *p_out, bool complete);





inline void init_proto_head_full(void *buf, uint32_t len,
		uint32_t uid, int32_t role_tm, uint32_t cmd, uint32_t seq, uint32_t ret)
{
	btlsw_proto_t *head = reinterpret_cast<btlsw_proto_t *>(buf);
	head->len = len;
	head->seq = seq;
	head->cmd = cmd;
	head->id = uid;
	head->role_tm = role_tm;
	head->ret = ret;
}

inline void init_proto_head_by_player(void *buf, player_t *p, uint32_t ret)
{
	init_proto_head_full(buf, sizeof(btlsw_proto_t),
			p->id, p->role_tm, p->waitcmd, p->seq, ret);
}

inline void init_proto_head_by_player(void *buf, player_t *p, uint32_t cmd, uint32_t ret)
{
	init_proto_head_full(buf, sizeof(btlsw_proto_t),
			p->id, p->role_tm, cmd, p->seq, ret);
}

inline int send_to_player(player_t *p, Cmessage *p_out, bool complete=true)
{
	btlsw_proto_t head;
	init_proto_head_by_player(&head, p, 0);
	return send_msg_to_player_base(p, &head, p_out, complete);
}

inline int send_to_player(player_t *p, uint32_t cmd, Cmessage *p_out, bool complete=true)
{
	btlsw_proto_t head;
	init_proto_head_by_player(&head, p, cmd, 0);
	return send_msg_to_player_base(p, &head, p_out, complete);
}

inline int send_header_to_player(player_t *p, uint32_t err, bool complete=true)
{
	btlsw_proto_t head;
	init_proto_head_by_player(&head, p, err);
	return send_msg_to_player_base(p, &head, 0, complete);
}

inline int send_header_to_player(player_t *p, uint32_t cmd, uint32_t err, bool complete=true)
{
	btlsw_proto_t head;
	init_proto_head_by_player(&head, p, cmd, err);
	return send_msg_to_player_base(p, &head, 0, complete);
}





#endif // __PROTO_IFACE_HPP__
