#include "central_online.h"
#include "mole_messenger.h"

#include "npc_event.h"

void msnger_credit_inviter(sprite_t* p, uint32_t inviter_id, uint32_t invitee_num)
{
	int bytes = 0;
	char buf[2048] = {0};
	sprite_t* inviter = get_sprite(inviter_id);
#ifndef TW_VER
	bytes = sprintf(buf, "你邀请的%.16s（米米号:%u）将成为你的好友，快去看看能获得什么奖励吧。",p->nick,p->id);
#else
	bytes = sprintf(buf, "你邀請的%.16s（米米號:%u）將成為你的好友，快去看看能獲得什麼獎勵吧。",p->nick,p->id);
#endif
	bytes = text_notify_pkg_pic(msg + 4, 0, 0, 0, 0, admin, 0, 15000, bytes, buf);

	if (inviter) {
		send_to_self(inviter, msg + 4, bytes, 0);
	} else {
		*(uint32_t*)msg = 0;
		chat_across_svr(inviter_id, msg, bytes + 4);
	}
}

