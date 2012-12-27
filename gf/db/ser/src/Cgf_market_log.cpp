
#include <libtaomee++/utils/tcpip.h>
#include <libtaomee++/inet/pdumanip.hpp>
#include <netinet/in.h>
#include "Cgf_market_log.h"


static char market_db_buf[PROTO_MAX_SIZE];
Cgf_market_log::Cgf_market_log(const char* a_ipport) : Cudp_sender(a_ipport)
{//config_get_strval("MARKET_LOG_UDP") 
	DEBUG_LOG("Cgf_market_log %s", a_ipport);
}

int Cgf_market_log::notify_use_item(userid_t uid, uint32_t role_regtime, uint32_t item_id, uint32_t cnt)
{
	int idx = 0;
	taomee::pack_h(market_db_buf, item_id, idx);
	taomee::pack_h(market_db_buf, cnt, idx);
	//NOTI_LOG("notify_use_item %u %u %u %u", uid, role_regtime, item_id, cnt);
	//return send_to_other_db(uid, role_regtime, gf_notify_use_item_cmd, market_db_buf, idx);
	return 0;
}

int Cgf_market_log::send_to_other_db(userid_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, uint32_t body_len)
{
	static char dbbuf[PROTO_MAX_SIZE];
	PROTO_HEADER* p_header = reinterpret_cast<PROTO_HEADER*>(dbbuf);
	p_header->id = id;
	p_header->role_tm = role_tm;
	p_header->cmd_id = cmd;
	p_header->result = 0;
	p_header->proto_length = sizeof(PROTO_HEADER) + body_len;
	memcpy(dbbuf + sizeof(PROTO_HEADER), body_buf, body_len);
	return this->send(dbbuf, p_header->proto_length);
}

