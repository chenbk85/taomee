#ifndef ONLINE_POLL_H_
#define ONLINE_POLL_H_

#include <libtaomee/log.h>
#include "util.h"

#include "benchapi.h"
#include "exclu_things.h"

static inline int
poll_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t type, to;
	if (unpkg_uint32_uint32(body, bodylen, &type, &to) == 0) {
		if (to >= 2000 && to <= 2003) {
			*(uint32_t*)(p->session) = to;
			p->sess_len = 4;
			return db_set_sth_done(p, 1000000005, 1, p->id);
		}
		ERROR_RETURN(("Invalid Pollee %u uid=%u", to, p->id), -1);
	}

	return -1;
}

#endif // ONLINE_POLL_H_
