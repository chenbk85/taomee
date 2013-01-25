#ifndef PROTO_HEAD_HPP
#define PROTO_HEAD_HPP

extern "C" 
{
#include <libtaomee/project/types.h>
}

struct proto_head_t
{
    uint32_t    len;
    uint32_t    seq;
    uint16_t    cmd;
    uint32_t    ret;
    userid_t    id;
}__attribute__((packed));

#define PROTO_HEAD_LEN sizeof(proto_head_t)

#endif // PROTO_HEAD_HPP

