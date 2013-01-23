/** 
 * ========================================================================
 * @file dispatch.cpp
 * @brief 
 * @version 1.0
 * @date 2011-12-08
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <errno.h>
#include "dispatch.hpp"
#include "proto.hpp"


typedef boost::intrusive::member_hook<c_player, object_hook_t, &c_player::m_wait_cmd_hook> await_option_t;

typedef boost::intrusive::list< c_player, await_option_t, boost::intrusive::constant_time_size<false> > await_player_list_t;

await_player_list_t g_await_player_list;


void cache_a_pkg(c_player * p_player, const void * buf, uint32_t buf_len);

int dispatch(void * data, fdsession_t * fdsess, bool cache_flag)
{
	btlsw_proto_t * pkg = reinterpret_cast<btlsw_proto_t *>(data);

    uint32_t len = pkg->len;
    uint16_t cmd = pkg->cmd;
    uint32_t seq = pkg->seq;
    int fd = fdsess->fd;
    uint32_t uid = pkg->id;

    TRACE_TLOG("dispatch[%u] sender=%u, fd=%u, seq=%u, len=%u, cache_flag=%d",
			cmd, uid, fd, seq, len, cache_flag);

	c_online *p_online = get_online_by_fd(fd);
	if (cmd == btlsw_online_register_cmd) {
		if (p_online) {
			/* 注册时, 发现通过fd连过来的online, 重复发来注册的包 */
			ERROR_TLOG("dup reg online, fd=%d, olip=0x%X, u=%u",
					fdsess->fd, fdsess->remote_ip, uid);
			return -1;
		}
		uint32_t online_id = *((uint32_t *)pkg->body);
		p_online = new c_online(fdsess, online_id);
		add_online(p_online);
		DEBUG_TLOG("reg_online, fd=%d, olip=0x%X, olid=%u",
				fdsess->fd, fdsess->remote_ip, online_id);
		return 0;
	}

	if (!p_online) {
		/* 到此, 无论是新注册, 还是注册后协议,
		 * 都应该有 online 了, 找不到就是有问题 */
		ERROR_TLOG("nofound p_online by fd=%d, olip=0x%X, cmd=%u, u=%u",
				fd, fdsess->remote_ip, cmd, uid);	
		return -1;
	}

	c_player *p_player = p_online->get_player(uid);
	if (cmd == btlsw_player_enter_hall_cmd) {
		if (p_player) {
			DEBUG_TLOG("player re_enter_hall, lastinfo: u=%u, role_tm=%u, olid=%u",
					p_player->m_role_tm, p_player->m_server_id, p_player->m_server_id);
			destroy_player(p_player);
			p_player = NULL;
		}
		uint32_t online_id = *((uint32_t *)pkg->body);
		uint32_t role_tm = *((uint32_t *)(pkg->body + 4));
		p_player = alloc_player(p_online, uid, online_id, role_tm);
	}
    
	if (!p_player) {
		ERROR_TLOG("nofound c_player, cmd=%u, u=%u, olid=%u",
				cmd, uid, p_online->m_id);
		return -1;
	}
	
	if (cache_flag && p_player->m_waitcmd) {
		if (g_queue_get_length(p_player->m_pkg_queue) < MAX_CACHE_PKG) {
			DEBUG_TLOG("cache a pkg u=%u, cmd=%u, wcmd=%u",
					p_player->m_id, cmd, p_player->m_waitcmd);
			cache_a_pkg(p_player, pkg, len);
			return 0;
		} else {
			WARN_TLOG("too many cache pkg, u=%u, cmd=%u, wcmd=%u",
					p_player->m_id, cmd, p_player->m_waitcmd);
			return 0;
		}
	}

    p_player->m_waitcmd = cmd;
    p_player->m_seq = seq;
    p_player->m_ret = 0;
    p_player->m_last_pkg_time = get_now_tv()->tv_sec;

	bind_proto_cmd_t * p_cmd = NULL;
    if (0 != find_btlsw_cmd_bind(cmd, &p_cmd)) {
        ERROR_TLOG("btl sw cmdid not existed: %u", cmd);
        return 0;
    }

    uint32_t body_len = len - sizeof(btlsw_proto_t);

    bool read_ret = p_cmd->p_in->read_from_buf_ex((char *)data + sizeof(btlsw_proto_t), body_len);
    if (!read_ret) {
        ERROR_TLOG("read_from_buf_ex error cmd=%u, u=%u", cmd, p_player->m_id);
        return -1;
    }

    int cmd_ret = p_cmd->func(p_player, p_cmd->p_in, p_cmd->p_out, NULL);
    return cmd_ret;
}

void cache_a_pkg(c_player * p_player, const void * buf, uint32_t buf_len)
{
    uint16_t len = sizeof(cached_pkg_t) + buf_len;
    cached_pkg_t * cache_buf = reinterpret_cast<cached_pkg_t *>(g_slice_alloc(len));
    cache_buf->len = len;
    memcpy(cache_buf->pkg, buf, buf_len);

    if (!p_player->m_wait_cmd_hook.is_linked()) {
        g_await_player_list.push_back(*p_player);
    }
	g_queue_push_tail(p_player->m_pkg_queue, cache_buf);
}

void proc_cached_pkg()
{
    await_player_list_t::iterator nx = g_await_player_list.begin();
	await_player_list_t::iterator it = nx;
	for(; it != g_await_player_list.end(); it = nx) {
		++nx;
		c_player * p_player = &*it;
		while(p_player->m_waitcmd == 0) {
			int fd = p_player->get_online()->m_fdsess->fd;
			uint32_t uid = p_player->m_id;
			cached_pkg_t *pkg = reinterpret_cast<cached_pkg_t *>(g_queue_pop_head(p_player->m_pkg_queue));
			int err = dispatch(pkg->pkg, p_player->get_online()->m_fdsess, false);
			g_slice_free1(pkg->len, pkg);
			//check p_player is exist
			p_player = get_player_from_online(fd, uid);
			if(p_player == NULL) break;
			if (err) {
				close_client_conn(fd);
				break;
			}

			if (g_queue_is_empty(p_player->m_pkg_queue)) {
				g_await_player_list.erase(it);
				break;
			}
		}
	}
}
