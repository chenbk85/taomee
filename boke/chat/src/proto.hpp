#ifndef SWITCH_PROTO_HPP_
#define SWITCH_PROTO_HPP_

extern "C" {
#include <stdint.h>
}

enum {
	pkg_size	= 8192
};


/**
 * @brief 定义命令号
 */
enum cmd_id_t {
	proto_test_alive            = 30000,
	// Adminer proto
	adm_check_msg				= 0xF133, // 61747
	adm_forbid_chat				= 62015,
	adm_get_dirty_md5			= 65001,
	adm_get_dirty_words			= 65002,
	adm_check_msg_common		= 65003,
};

typedef uint32_t userid_t;
typedef uint64_t battle_id_t;



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
