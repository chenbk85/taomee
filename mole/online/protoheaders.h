#ifndef ONLINE_PROTO_HEADERS_H_
#define ONLINE_PROTO_HEADERS_H_

// headers since C99
#include <stdint.h>
#include "benchapi.h"

// For DB and Central Online
typedef struct server_proto {
	uint32_t	len;
	uint32_t	seq;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	char		body[];
} __attribute__((packed)) server_proto_t;

// For Broadcaster
typedef struct CommunicatorHeader {
	uint32_t	len;
	uint32_t	online_id;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	map_id_t    mapid;
	uint32_t	opid;
	uint8_t		body[];
} __attribute__((packed)) communicator_header_t;

typedef struct msglog_proto {
	uint16_t	len;
	uint16_t	cmd;
	uint32_t	id;
	uint8_t		ver;
	uint8_t		body[];
} __attribute__((packed)) msglog_proto_t;



#endif // ONLINE_PROTO_HEADERS_H_
