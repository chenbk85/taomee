/** 
 * ========================================================================
 * @file dispatch.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-24
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "dispatch.h"
#include "proto.h"
#include "itl_common.h"



int dispatch(int fd, const char * buf, uint32_t len)
{
    const node_proto_t * pkg = reinterpret_cast<const node_proto_t *>(buf);


    uint16_t cmd = pkg->cmd;
    uint32_t seq = pkg->seq;
    uint32_t uid = pkg->id;
    uint32_t body_len = len - sizeof(node_proto_t);

    TRACE_LOG("dispatch[%u] sender=%u, fd=%u, seq=%u, len=%u",
			cmd, uid, fd, seq, len);



    const cmd_proto_t * p_cmd = find_switch_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        return 0;
    }


    bool read_ret = p_cmd->p_in->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret) 
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, uid);
        return -1;
    }



    int cmd_ret = p_cmd->func(fd, p_cmd->p_in, p_cmd->p_out, body_len);

    if (SWITCH_SUCC != cmd_ret)
    {
        node_proto_t head;
        init_node_proto_header(&head, sizeof(head), seq, cmd, uid, cmd_ret);
        net_send_cli_ex(fd, &head, sizeof(head));
    }

    return cmd_ret;
}
