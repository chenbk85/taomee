#include "pea_common.hpp"
#include "cli_proto.hpp"
#include "player.hpp"
#include "battle_impl.hpp"
#include "chat_transfer.hpp"

enum {
	CHAT_MSG_TYPE_TEAM = 6,	//小队聊天
};

int online_proto_btl_chat_msg_transfer(DEFAULT_ARG)
{
	online_proto_btl_chat_msg_transfer_in *p_in = P_IN;

	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, p_in->sender.user_id, idx);
	pack(pkgbuf, p_in->sender.role_tm, idx);
	pack(pkgbuf, p_in->sender_nick, MAX_NICK_SIZE, idx);
	pack(pkgbuf, p_in->msg_type, idx);
	pack(pkgbuf, p_in->_msg_content_len, idx);
	pack(pkgbuf, p_in->msg_content, p_in->_msg_content_len, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_chat_public_msg_cmd, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	
	std::list<Player*>::iterator pItr = p->btl->playerlist.begin();
	for(; pItr != p->btl->playerlist.end(); ++pItr) {
		Player *p_dest = *pItr;
		if (p_dest != p) {//do not clear waitcmd
			if (p_in->msg_type == CHAT_MSG_TYPE_TEAM
				&& p_dest->team != p->team)
				continue;
			send_to_player(p_dest, pkgbuf, idx, 0);	
		}
	}
	return send_to_player(p, pkgbuf, idx, 1);
}

