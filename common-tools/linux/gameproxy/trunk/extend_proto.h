
#ifndef _EXTEND_PROTO_H_
#define _EXTEND_PROTO_H_
extern "C" {
#include <libtaomee/project/types.h>
#include <netinet/in.h>
}

#pragma pack(1)

/**
  * @brief client protocol type
  */
struct cli_proto_t {
	/*! package length */
	uint32_t	len;
	/*! protocol version */
	uint8_t		ver;
	/*! protocol command id */
	uint32_t	cmd;
	/*! user id */
	userid_t	id;
	/*! used as an auto-increment sequence number if from client */
	uint32_t	ret;
	/*! body of the package */
	uint8_t		body[];
};

struct new_cli_proto_t {
	/*! package length */
	uint32_t	len;
	/*! protocol command id */
	uint16_t	cmd;
	/*! user id */
	userid_t	id;
	/*! used as an auto-increment sequence number for checking GameCheater */
	uint32_t	seqno;
	/*! errno: 0 for OK */
	uint32_t	ret;
	/*! body of the package */
	uint8_t		body[];
};


enum {
cli_proto_extend_port	= 999,
cli_proto_cmd_max	= 30001,
};


struct online_addr_t {
	/*! IP(net-order) */
	struct in_addr ip;
	/*! Port(net-order) */
	uint16_t port;
};

#pragma pack()







#endif