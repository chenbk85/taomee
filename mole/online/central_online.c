#include <libtaomee/conf_parser/config.h>

#include "protoheaders.h"
#include "service.h"
#include "sprite.h"
#include "util.h"
#include "logic.h"
#include "mcast.h"
#include "central_online.h"
#include "charitable_party.h"



int central_online_fd = -1;

static uint8_t cobuf[PAGESIZE];

// send an init pkg to central online
static inline void send_init_pkg();

//--------------------------------------------------------
/**
 * send_to_central_online - uniform interface to send request to central online
 *
 */
int send_to_central_online(int cmd, sprite_t* p, int body_len, void* body_buf, uint32_t id)
{
	static uint8_t copkg[PAGESIZE];

	if (central_online_fd == -1) {
		connect_to_central_online();
	}

	int len = sizeof(server_proto_t) + body_len;
	if ( (central_online_fd) == -1 || (central_online_fd > epi.maxfd) ||
		(epi.fds[central_online_fd].cb.sndbufsz < epi.fds[central_online_fd].cb.sendlen + len) ||
		(body_len > sizeof(copkg) - sizeof(server_proto_t)) ) {
		ERROR_RETURN(("Failed to Send Pkg to Central Online, buflen=%d, fd=%d", epi.fds[central_online_fd].cb.sendlen, central_online_fd), -1);
	}

	server_proto_t* pkg = (void*)copkg;
	pkg->len = len;
	pkg->seq = (p ? (sprite_fd(p) << 16) | p->waitcmd : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	memcpy(pkg->body, body_buf, body_len);

	return net_send(central_online_fd, copkg, len, 0);
}
//--------------------------------------------------------

//-------------------------- Utils --------------------------
/**
 * connect_to_central_online - connect to central online and send an init pkg
 *
 */
void connect_to_central_online()
{
	//central_online_fd = connect_to_svr(config_get_strval("central_online_ip"), config_get_intval("central_online_port", 0), 65535, 1);
	central_online_fd = connect_to_service(config_get_strval("central_online_ip"), 0, 65535, 1);
	if (central_online_fd != -1) {
		//DEBUG_LOG("CONNECTED TO CENTRAL ONLINE AND SEND INIT PKG");
		send_init_pkg();
	}
}


//------------ Form Pkg and Send to Central Online ------------
/**
 * send_init_pkg - send an init pkg to central online
 *
 */
static inline void send_init_pkg()
{
	static uint32_t seqno = 0;

	int i = 0;
	uint8_t pkg[48];
	PKG_H_UINT16(pkg, config_get_intval("domain", 0), i);
	PKG_H_UINT32(pkg, config_cache.bc_elem->online_id, i);
	PKG_STR(pkg, config_cache.bc_elem->online_name, i, 16);
	PKG_STR(pkg, config_cache.bc_elem->bind_ip, i, 16);
	PKG_H_UINT16(pkg, config_cache.bc_elem->bind_port, i);
	PKG_H_UINT32(pkg, sprites_count, i);
	PKG_H_UINT32(pkg, seqno, i);
	++seqno;

	send_to_central_online(COCMD_init_pkg, 0, sizeof pkg, pkg, 0);
}
/**
 * send_attr_update_noti - notify online and client to update a given user's attr
 *
 */
void send_attr_update_noti(uint32_t sender, sprite_t* p_rcver, uint32_t rcver, uint32_t type)
{
	int i;
	sprite_t* rcv = (p_rcver ? p_rcver : get_sprite(rcver));

	if (rcv) {
		i = sizeof(protocol_t);
		PKG_UINT32(cobuf, type, i);
		init_proto_head(cobuf, PROTO_UPD_ATTR_NOTI, i);
		send_to_self(rcv, cobuf, i, 0);
	} else {
		i = 0;
		PKG_H_UINT32(cobuf, sender, i);
		PKG_H_UINT32(cobuf, rcver, i);
		PKG_H_UINT32(cobuf, type, i);
		send_to_central_online(COCMD_attr_update_noti, 0, i, cobuf, rcver);
	}
}
//----------------------------------------------------------

//------------------------------ callbacks ----------------------------------------
int chk_online_across_svr_callback(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	if (len != 0) {
		CHECK_BODY_LEN_GE(len, 2);

		uint16_t olcnt = (initor->sess_len > (sizeof(protocol_t) + 2));

		int i = 0, domain_cnt;
		UNPKG_H_UINT16(buf, domain_cnt, i);
		//DEBUG_LOG("DOMAIN CNT %d", domain_cnt);

		int j = 0, domain_id, k, l;
		uint32_t online_cnt, online_id, friend_cnt, friend_id;
		for (; j != domain_cnt; ++j) {
			UNPKG_H_UINT16(buf, domain_id, i);
			UNPKG_H_UINT32(buf, online_cnt, i);
			//DEBUG_LOG("DOMAIN ID %d ONLINE CNT %u", domain_id, online_cnt);
			olcnt += online_cnt;

			for ( k = 0; k != online_cnt; ++k ) {
				UNPKG_H_UINT32(buf, online_id, i);
				UNPKG_H_UINT32(buf, friend_cnt, i);
				//DEBUG_LOG("ONLINE ID %u FRIEND CNT %u", online_id, friend_cnt);
				PKG_UINT16(initor->session, online_id, initor->sess_len);
				PKG_UINT32(initor->session, friend_cnt, initor->sess_len);
				for ( l = 0; l != friend_cnt; ++l ) {
					UNPKG_H_UINT32(buf, friend_id, i);
					PKG_UINT32(initor->session, friend_id, initor->sess_len);
					PKG_UINT8(initor->session, 1, initor->sess_len);
					//DEBUG_LOG("FRIEND ID %u", friend_id);
				}
			}
		}

		i = sizeof(protocol_t);
		PKG_UINT16(initor->session, olcnt, i);
	}

	init_proto_head(initor->session, initor->waitcmd, initor->sess_len);
	send_to_self(initor, initor->session, initor->sess_len, 1);
	initor->sess_len = 0;
	return 0;
}

int sync_vip_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	int i = 0;
	uint32_t vipflag;

	UNPKG_H_UINT32(buf, vipflag, i);

	int ret = 0;
	sprite_t* p = get_sprite(rcverid);
	if (p) {
		if (vipflag)
			p->flag |= 1u;
		else
			p->flag &= ~1u;
		response_proto_get_sprite2(p, PROTO_SET_STAGE, p, 0);
	} else {
		ret = COERR_user_offline;
	}

	DEBUG_LOG("SYNC VIP\t[recvid=%u offline=%d]", rcverid, ret);
	return ret;
}

void tell_flash_some_msg_across_svr(sprite_t* p, uint32_t msg_type, int msg_len, char* txt)
{
	uint8_t pkg[1024] = {0};
	int len = 0;
	PKG_H_UINT32(pkg, msg_type, len);
	PKG_H_UINT32(pkg, msg_len, len);
	PKG_STR(pkg, txt, len, msg_len);

	uint32_t user_id = 0;
	if( p )
	{
		user_id = p->id;
	}
	send_to_central_online(COCMD_tell_flash_some_msg, p, len, pkg, user_id);
}

int tell_flash_some_msg_across_svr_callback(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);
	int i = 0;
	uint32_t msg_type = 0;
	int32_t msg_len = 0;
	UNPKG_H_UINT32((uint8_t*)buf, msg_type, i);
	UNPKG_H_UINT32((uint8_t*)buf, msg_len, i);
	CHECK_BODY_LEN(len, 8 + msg_len);

	switch( msg_type )
	{
	case CBMT_LONGZU_EXPLORE:
		{
			explor_time_begin = get_now_tv()->tv_sec;
		}
		break;
	case CBMT_CHP_DONATE_ITEM:
		{
			return charparty_broadcast_donate_info( (uint8_t*)(buf + 8), msg_len );
		}
		break;
	case CBMT_UPDATE_MONEY:
		{
			uint32_t user_id;
			uint32_t add_money;
			UNPKG_H_UINT32((uint8_t*)buf, user_id, i);
			UNPKG_H_UINT32((uint8_t*)buf, add_money, i);
			//DEBUG_LOG( "===================Notice Player:[%u] Update Money===================", user_id );
			sprite_t* p = get_sprite(user_id);
			if( p )
			{
				p->yxb += add_money;
				//DEBUG_LOG( "===Notice Player:[%u] Update Money===", user_id );
				response_proto_uint32( p, PROTO_UPDATE_MONEY, p->yxb, 0 );
			}
			return 0;
		}
		break;
	default:
		break;
	}

	uint8_t n_msg[1024] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, msg_type, l);
	PKG_UINT32(n_msg, msg_len, l);
	PKG_STR(n_msg, (uint8_t*)buf + 8, l, msg_len);
	init_proto_head(n_msg, PROTO_TELL_FLASH_SOME_MSG, l);
	send_to_all_players(initor, n_msg, l, 0);
	return 0;
}
//--------------------------------------------------------------------------
