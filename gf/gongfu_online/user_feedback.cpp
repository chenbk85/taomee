#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/project/utilities.h>
}

#include "utils.hpp"
#include "dbproxy.hpp"
#include "user_feedback.hpp"
#include "cli_proto.hpp"

using namespace taomee;

//--------------------------------------------------------------------
/**
  * @brief submit message to DB
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int submit_msg_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;

	uint32_t type, msglen;
	unpack(body, type, idx);
	unpack(body, msglen, idx);

	CHECK_VAL_GE(msglen, c_msg_title_len);
	CHECK_VAL_LE(msglen, c_msg_max_len);
	CHECK_VAL_EQ(bodylen, 8 + msglen);

	*reinterpret_cast<uint32_t*>(body) = type;
	*reinterpret_cast<uint32_t*>(body + 4) = msglen;
	
	submit_msg_t* precv = reinterpret_cast<submit_msg_t*>(body);
	return db_submit_msg(p, precv);
}

inline int db_submit_msg(player_t* p, submit_msg_t* precv)
{
	int idx = 0;
	db_submit_msg_t* pmsg = reinterpret_cast<db_submit_msg_t*>(pkgbuf);
	uint32_t db_msglen = precv->msglen - sizeof(precv->title);

	pack_h(pkgbuf, static_cast<uint32_t>(6), idx);
	pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
	pack_h(pkgbuf, precv->type, idx);
	pack(pkgbuf, p->nick, sizeof(p->nick), idx);
	pack(pkgbuf, precv->title, sizeof(pmsg->title), idx);
	pack_h(pkgbuf, db_msglen, idx);
	pack(pkgbuf, precv->msg, db_msglen, idx);

	send_udp_request_to_db(p, p->id, dbproto_submit_msg, pkgbuf, idx, 0);
	return send_header_to_player(p, p->waitcmd, 0, 1);

}
/**
  * @brief user report message to DB
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int report_msg_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0, idx2 = 0;
	CHECK_VAL_EQ(bodylen, sizeof(appellee_msg_t));
    appellee_msg_t appellee = {0,0,{0},0};
    unpack(body, appellee.id, idx2);
    unpack(body, appellee.role_tm, idx2);
    unpack(body, appellee.nick, sizeof(appellee.nick), idx2);
    unpack(body, appellee.rule_id, idx2);
    KDEBUG_LOG(p->id, "USER REPORT\t[uid=%u appelleeid=%u ruleid=%u]",p->id,appellee.id,appellee.rule_id);

    pack_h(pkgbuf, static_cast<uint32_t>(6), idx);
    pack_h(pkgbuf, p->id, idx);
    pack_h(pkgbuf, appellee.id, idx);
    //pack_h(pkgbuf, appellee->role_tm, idx);
    //pack(pkgbuf, appellee->nick, sizeof(appellee->nick), idx);
    pack_h(pkgbuf, appellee.rule_id, idx);

    send_udp_request_to_db(p, p->id, dbproto_report_msg, pkgbuf, idx, 0);
    return send_header_to_player(p, p->waitcmd, 0, 1);
}

