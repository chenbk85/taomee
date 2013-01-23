#include "proto.hpp"
#include "dispatch.hpp"
#include "common.hpp"
#include "battle.hpp"


/* 返回编译时间 */
int btlsw_version(DEFAULT_ARG)
{
	btlsw_version_out * p_out = P_OUT;

    snprintf(p_out->version_string, 256,
            "%s %s", __DATE__, __TIME__);

    send_to_player(p, p_out);
	return 0;
}
