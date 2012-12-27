#ifndef SWITCH_PROTO_HPP_
#define SWITCH_PROTO_HPP_

extern "C" {
#include <stdint.h>
}

enum {
	pkg_size	= 8192
};

typedef uint32_t userid_t;


/* Define the communication protocol between the switch and online 
   server(or adminer server, login server) */
struct svr_proto_t {
        /* package head */
        uint32_t	len; // package length, no more than 4k bytes
        uint32_t	seq; // must be 0
        uint16_t	cmd; // cmd id, >OxEA60
        uint32_t	ret; // Result, must be 0
        userid_t	id; // Userid
        /* package body, variable length */
        uint8_t		body[];
} __attribute__((packed));

inline void
init_proto_head(void* buf, userid_t uid, uint32_t len, uint32_t seq, uint16_t cmd, uint32_t ret)
{
	svr_proto_t* hdr = reinterpret_cast<svr_proto_t*>(buf);
	hdr->len = len;
	hdr->seq = seq;
	hdr->cmd = cmd;
	hdr->ret = ret;
	hdr->id  = uid;
}

#endif // SWITCH_PROTO_HPP_
