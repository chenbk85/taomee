#ifndef  GF_CGF_MARKET_LOG_H
#define GF_CGF_MARKET_LOG_H

#include <libtaomee++/utils/tcpip.h>
#include "proto.h"
#include "benchapi.h"

class Cgf_market_log : public Cudp_sender
{
	public:
		Cgf_market_log(const char* a_ipport);
	
		int notify_use_item(userid_t uid, uint32_t role_regtime, uint32_t item_id, uint32_t cnt = 1);

		int send_to_other_db(userid_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, uint32_t body_len);
};

#endif //~GF_CGF_MARKET_LOG_H
