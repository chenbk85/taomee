#include "proto.h"

#include "global.h"
#include "home.h"
#include "sprite.h"
#include "dispatcher.h"


#define MAX_PKG_USER_CNT (proto_max_len / (sizeof(sprite_ol_t)) - 1)


typedef struct pack_buf {
	int   		len;
	void* 		buf;	
}	pb_t;

typedef struct pack_home_users{
	int total_cnt;
	int user_cnt;
	int send_cnt;
	int pass_cnt;
	
	int 		len;
	uint16_t	cmd;
	userid_t	userid;
	uint32_t	onlineid;
	mapid_t		homeid;
	uint8_t*	p_buf;
}	phu_t;

static void tranfer_to_sprite(void* key, void* value, void* data)
{
	pb_t* ppb = data;
	online_each_t* p_online = value;
	home_proto_t* p_proto = ppb->buf;	
	if (all_fds[p_online->onlineid] && (p_proto->onlineid != p_online->onlineid)) {
		send_pkg_to_client(all_fds[p_online->onlineid], ppb->buf, ppb->len);
	}
}

inline void tranfer_to_home(home_t* p_home, void* buf, int len)
{
	pb_t pb = {len, buf};
	if (p_home) {
		g_hash_table_foreach(p_home->onlines, tranfer_to_sprite, &pb);
	}
}

inline int send_to_online(void* buf, int len)
{
	home_proto_t* p_proto = buf;
	if (!all_fds[p_proto->onlineid] || (len > proto_max_len) ) {
		KDEBUG_LOG(p_proto->id,"failed to send pkg [fd=%d]", all_fds[p_proto->onlineid]->fd);
		return 0;
	}
	return send_pkg_to_client(all_fds[p_proto->onlineid], buf, len);
}

static void pkg_and_send_online(void* key, void* value, void* data)
{
	phu_t* p_phu = data;
	sprite_ol_t* p = value;

	if (p->onlineid == p_phu->onlineid) {
		++p_phu->pass_cnt;
	} else {
		memcpy(p_phu->p_buf + p_phu->len, &p->sprite_info, sizeof(sprite_base_info_t));
		p_phu->len += sizeof(sprite_base_info_t);
		DEBUG_LOG("PKG LEN[uid=%u len=%u size=%u]", p->sprite_info.userid, p_phu->len, (uint32_t)sizeof(sprite_base_info_t));
		if (p->sprite_info.pet_followed) {
			memcpy(p_phu->p_buf + p_phu->len, &p->pet_sinfo, sizeof(pet_simple_info_t));
			p_phu->len += sizeof(pet_simple_info_t);
		}

		++p_phu->user_cnt;
		++p_phu->send_cnt;
	}

	if (((p_phu->user_cnt + p_phu->pass_cnt == p_phu->total_cnt) && p_phu->send_cnt)
				|| p_phu->send_cnt == MAX_PKG_USER_CNT) {
		pkg_proto_head(p_phu->p_buf, p_phu->len, p_phu->onlineid, p_phu->cmd, p_phu->userid, p_phu->homeid, proto_enter_home);		
		memcpy(p_phu->p_buf + sizeof(home_proto_t), &p_phu->send_cnt, sizeof(int));
		send_to_online(p_phu->p_buf, p_phu->len);

		DEBUG_LOG("PKG USERS [t_cnt=%u uid=%u u_cnt=%u s_cnt=%u p_cnt=%u]", p_phu->total_cnt, p->sprite_info.userid, p_phu->user_cnt, p_phu->send_cnt, p_phu->pass_cnt);

		p_phu->send_cnt = 0;
		p_phu->len	= sizeof(home_proto_t) + 4;
	}
}

inline void send_all_users_to_self(userid_t userid, uint32_t onlineid, home_t* p_home)
{
	int total_cnt	= g_hash_table_size(p_home->sprites);
	if (total_cnt) {
		uint8_t	send_all_buf[proto_max_len];		
		phu_t phu = {total_cnt, 0, 0, 0, sizeof(home_proto_t) + 4, 0, userid, onlineid, p_home->homeid, send_all_buf};
		g_hash_table_foreach(p_home->sprites, pkg_and_send_online, &phu);
	}
}

