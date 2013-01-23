#include "cli_proto.hpp"



int send_msg_to_player_base(player_t *p, void *head, Cmessage *p_out, bool complete)
{
	btlsw_proto_t *phead = reinterpret_cast<btlsw_proto_t *>(head);
	if (send_msg_to_client(p->online->fdsess, (char *)(head), p_out) == -1) {
		ERROR_LOG("fail_send_msg_to_player: u=%u cmd=%u", p->id, phead->cmd);
		return -1;
	}

	if (complete) {
		p->waitcmd = 0;
	}

	p->last_pkg_time = get_now_tv()->tv_sec;
	return 0;
}


