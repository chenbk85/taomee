/** 
 * ========================================================================
 * @file itl_proto.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "itl_proto.h"



int net_send_ser_msg(int fd, const void * head_buf, Cmessage * msg)
{
    uint32_t head_len = *(uint32_t *)(head_buf);
    if (NULL == msg)
    {
        return net_send_ser(fd, (const char *)head_buf, head_len);
    }
    else
    {
        static byte_array_t ba;
        ba.init_postion();
        ba.write_buf((const char *)head_buf, head_len);
        msg->write_to_buf(ba);
        uint32_t len = ba.get_postion();
        *(uint32_t *)(ba.get_buf()) = len;
        return net_send_ser(fd, ba.get_buf(), len);
    }
}


int net_send_cli_msg(int fd, const void * head_buf, Cmessage * msg)
{
    uint32_t head_len = *(uint32_t *)(head_buf);
    if (NULL == msg)
    {
        return net_send_cli(fd, (const char *)head_buf, head_len);
    }
    else
    {
        static byte_array_t ba;
        ba.init_postion();
        ba.write_buf((const char *)head_buf, head_len);
        msg->write_to_buf(ba);
        uint32_t len = ba.get_postion();
        *(uint32_t *)(ba.get_buf()) = len;
        return net_send_cli(fd, ba.get_buf(), len);
    }

}

#define INIT_PROTO_HEADER(proto_name)    \
    int init_ ## proto_name ## _proto_header(void * buf, uint32_t len, uint32_t seq, uint16_t cmd, uint32_t ret, uint32_t id) \
{   \
    proto_name ## _proto_t * pkg = reinterpret_cast<proto_name ## _proto_t *>(buf);    \
    if (NULL == pkg) return -1; \
    pkg->len = len; \
    pkg->seq = seq; \
    pkg->cmd = cmd; \
    pkg->id = id;   \
    pkg->ret = ret; \
    return 0;   \
}


INIT_PROTO_HEADER(head)
INIT_PROTO_HEADER(db)
INIT_PROTO_HEADER(rrd)
INIT_PROTO_HEADER(alarm)
INIT_PROTO_HEADER(node)
INIT_PROTO_HEADER(update)
INIT_PROTO_HEADER(control)

#undef INIT_PROTO_HEADER

