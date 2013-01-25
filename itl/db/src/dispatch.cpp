/** 
 * ========================================================================
 * @file dispatch.cpp
 * @brief 
 * @version 1.0
 * @date 2011-12-08
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "dispatch.h"
#include "proto.h"
#include "db_interface.h"


int dispatch(int fd, const char * buf, uint32_t len)
{
	const db_proto_t * pkg = reinterpret_cast<const db_proto_t *>(buf);

    uint16_t cmd = pkg->cmd;
    uint32_t seq = pkg->seq;
    uint32_t uid = pkg->id;

    TRACE_LOG("dispatch[%u] sender=%u, fd=%u, seq=%u, len=%u",
			cmd, uid, fd, seq, len);

    char send_buff[ITL_MAX_BUF_LEN] = {0};
    db_proto_t *p_snd_pkg = reinterpret_cast<db_proto_t *>(send_buff);
    p_snd_pkg->cmd = cmd;
    p_snd_pkg->seq = seq;
    p_snd_pkg->id = uid;

    const cmd_proto_t * p_cmd = find_db_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        return 0;
    }

    uint32_t body_len = len - sizeof(db_proto_t);

    bool read_ret = p_cmd->p_in->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret)
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, uid);
        return -1;
    }

    p_cmd->p_out->init();

    int cmd_ret = p_cmd->func(p_cmd->p_in, p_cmd->p_out, NULL);

    p_snd_pkg->ret = cmd_ret;
    if (DB_SUCC == cmd_ret)
    {
        commit();
        uint32_t write_len = 0;
        body_len = sizeof(send_buff) - sizeof(db_proto_t);
        bool write_ret = p_cmd->p_out->write_to_buf_ex(p_snd_pkg->body, body_len, &write_len);
        if (!write_ret)
        {
            ERROR_LOG("write_to_buf_ex error cmd=%u, u=%u", cmd, uid);
            return -1;
        }
        p_snd_pkg->len = sizeof(db_proto_t) + write_len;
    }
    else
    {
        rollback();
        p_snd_pkg->len = sizeof(db_proto_t);
    }


    net_send_cli(fd, send_buff, p_snd_pkg->len);
    TRACE_LOG("send cmd[%u] fd[%u] pkg_len[%u]", p_snd_pkg->cmd, fd, p_snd_pkg->len);

    return cmd_ret;
}
