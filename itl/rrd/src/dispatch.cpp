/** 
 * ========================================================================
 * @file dispatch.cpp
 * @brief 
 * @version 1.0
 * @date 2012-7-11
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "proto.h"
#include "dispatch.h"


int dispatch(int fd, const char * buf, uint32_t len)
{
	const rrd_proto_t * pkg = reinterpret_cast<const rrd_proto_t *>(buf);

    uint16_t cmd = pkg->cmd;
    uint32_t seq = pkg->seq;
    uint32_t uid = pkg->id;

    TRACE_LOG("dispatch[%u] sender=%u, fd=%u, seq=%u, len=%u",
			cmd, uid, fd, seq, len);


    const cmd_proto_t * p_cmd = find_rrd_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        return 0;
    }

    uint32_t body_len = len - sizeof(rrd_proto_t);

    bool read_ret = p_cmd->p_in->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret)
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, uid);
        return -1;
    }

    int cmd_ret = p_cmd->func(p_cmd->p_in, p_cmd->p_out);

    return cmd_ret;
}
