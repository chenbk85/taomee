#include "home.h"
#include "cli_proto.h"

int get_home_info_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
    uint32_t uid;
    int j = 0;
    UNPKG_UINT32(body, uid, j);
    return send_request_to_db(p, uid, proto_db_get_home_info, &uid, 4);
}

int get_home_info_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    int i = sizeof(protocol_t);
    i += pkg_home_info(msg + i, (home_t *)buf);
    init_proto_head(msg, p->waitcmd, i);
    return send_to_self(p, msg, i, 1);
}

int update_home_flag_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t bit, flag;
	int j = 0;
	UNPKG_UINT32(body, bit, j);
	UNPKG_UINT32(body, flag, j);

	if (flag != 1)
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);

	switch (bit) {
		case home_flag_tutorial:
			break;
		default:
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	return db_set_home_flag(p, p->id, bit, flag);
}


