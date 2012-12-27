#ifndef MC_SERVER_H
#define MC_SERVER_H

#include "sprite.h"
#include "item.h"

#define MC_PROTO_QUERY_MAGIC_CODE_GIFT		2501
#define MC_PROTO_CONSUME_MAGIC_GIFT			2502

enum MAGIC_CMD {
	MAGIC_PROTO_LOOKUP_MAGIC_CODE_ITEMS	= 2601,
	MAGIC_PROTO_GET_MAGIC_CODE_ITEMS	= 2602,
};

#define MAGIC_CODE_LEN		32

extern int mc_server_fd;
extern int magicserver_fd;

int handle_magicserver_return(int fd, server_proto_t* magicpkg, int len, sprite_t** sp);

int magic_passwd_rand_item_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int magic_passwd_item_lookup_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int query_magic_code_gift_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int consume_magic_code_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int send_request_to_mc_server(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id);
int handle_mc_server_return(int fd, server_proto_t* mc_pkg, int len, sprite_t** sp);


#endif

