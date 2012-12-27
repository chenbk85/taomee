
#ifndef ONLINE_CACHE_SVR_H
#define ONLINE_CACHE_SVR_H

#include "sprite.h"
#include "item.h"

#define CS_SET_JY_AND_PET_STATE		9
#define CS_GET_JY_AND_PET_STATE		10
#define CS_SET_FRIEND_HOME_HOT_STATE		11
#define CS_GET_FRIEND_HOME_HOT_STATE		12
#define CS_SET_FRIEND_SHOP_EXP_MONEY		13
#define CS_GET_FRIEND_SHOP_EXP_MONEY		14


// For Cache server
typedef struct cache_server_proto {
	uint32_t	len;
	uint8_t		ver;
	uint32_t	cmd;
	uint32_t	uid;
	uint32_t	seq;
	char		body[];
} __attribute__((packed)) cs_proto_t;

extern int stampsvr_tcp_fd;

int send_request_to_cache_server(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id);

static inline int cs_set_jy_and_pet_state(sprite_t* p, uint32_t type, uint32_t state, uint32_t uid)
{
	uint32_t buf[2] = {0};
	buf[0] = type;
	buf[1] = state;
	return send_request_to_cache_server(CS_SET_JY_AND_PET_STATE, p, 8, buf, uid);
}

#define cs_set_friend_home_hot(p_, buf, len, uid) \
		send_request_to_cache_server(CS_SET_FRIEND_HOME_HOT_STATE, p_, len, buf, uid);

int handle_cs_server_return(int fd, server_proto_t* cs_pkg, int len, sprite_t** sp);
int chk_jy_and_pet_state_cmd(sprite_t* p, const uint8_t* body, int len);
int get_friend_home_hot_cmd(sprite_t* p, const uint8_t* body, int len);

int cs_set_shop_info(uint32_t level, uint32_t money, uint32_t uid);
int cs_get_shops_info_cmd(sprite_t* p, const uint8_t* body, int len);
int cs_get_shops_info_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code);

#endif

