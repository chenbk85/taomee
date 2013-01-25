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
	proto_cross_data			= 65001,
	proto_test_alive            = 65002,
};

// types
enum {
	/*! maximum online id */
	online_num_max	= 2048
};
enum {
	pklv_phase_max	= 20,
	pklv_per_phase	= 5,
	pk_team_cnt_max	= 5,
};

enum {
	apply_fail,
	match_fail,
	match_succ,
};

enum
{
	CROSS_MIN_PROT_COMMID = 0,
	CROSS_COMMID_ONLINE_KEEPALIVE = 1001,
	CROSS_REPORT_ONLINE_INFO = 1002,
	CROSS_COMMID_CHANGEE_OTHER_INLINE = 1003,
	CROSS_COMMID_APPLY_PK = 1004,
	CROSS_COMMID_CANCEL_APPLY_PK = 1005,
	CROSS_COMMID_AUTO_ACCEPT = 1006,
	CROSS_COMMID_AUTO_FIGHT = 1007,
	CROSS_COMMID_ONLI_AUTO_CHALLENGE = 1008,
	
	

	// ϵͳ�ڲ���Ϣid
	CROSS_MAX_PROT_COMMID = 5500,
	CROSS_COMMID_ONLINE_TIMEOUT = 5501,
	CROSS_COMMID_ONLINE_PK_TRYAGAIN = 5502,
	CROSS_COMMID_CLEAN_ONLINE_INFO = 5503,

	
};

enum
{
//	proto_onli_auto_challenge	= 62013,
	proto_challenge_battle_onli = 62010,
//	proto_onli_auto_fight		= 62015,
	proto_onli_cancel_pk_apply	= 62016,
};

typedef uint32_t userid_t;
typedef uint64_t battle_id_t;

// For batrserv
typedef struct mole2cross_proto {
	/* package head */
	uint32_t	len; // package length, no more than 4k bytes
	uint32_t	seq; // must be 0
	uint16_t	cmd; // cmd id, >OxEA60
	uint32_t	ret; // Result, must be 0
	userid_t	id; // Userid

	uint32_t	opid;
	uint8_t		body[];
} __attribute__((packed)) mole2cross_proto_t;

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

inline void init_proto_head(void* buf, userid_t uid, uint32_t len, uint32_t seq, uint16_t cmd, uint32_t ret)
{
	mole2cross_proto* hdr = reinterpret_cast<mole2cross_proto*>(buf);
	hdr->len = len;
	hdr->seq = seq;
	hdr->cmd = proto_cross_data;
	hdr->opid = cmd;
	hdr->ret = ret;
	hdr->id  = uid;
}

#endif // SWITCH_PROTO_HPP_
