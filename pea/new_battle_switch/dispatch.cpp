#include "online.hpp"
#include "player.hpp"
#include "dispatch.hpp"
#include "proto.hpp"



int dispatch(void *data, fdsession_t *fdsess, bool cache_flag)
{
	btlsw_proto_t * pkg = reinterpret_cast<btlsw_proto_t *>(data);

    uint32_t len = pkg->len;
    uint16_t cmd = pkg->cmd;
    uint32_t seq = pkg->seq;
    int fd = fdsess->fd;
    uint32_t uid = pkg->id;
	int32_t role_tm = pkg->role_tm;

    TRACE_TLOG("dispatch[%u] pkg_uid=%u, fd=%u, seq=%u, len=%u, cache_flag=%d",
			cmd, uid, fd, seq, len, cache_flag);

	online_t *online = 0;
	if (unlikely(cmd == btlsw_online_register_cmd)) {
		uint32_t olid = *((uint32_t *)pkg->body);
		online = add_online(fdsess, olid);
		if (unlikely(!online)) {
			ERROR_TLOG("failed add_online, olip=0x%X, fd=%d, olid=%u",
					fdsess->remote_ip, fd, olid);
			return -1;
		}
		return 0; // TODO(zog): 给 online 一个 ack (回包);
	} else {
		online = get_online_by_fd(fd);
	}

	/* 到此, 无论是新注册, 还是注册后协议,
	 * 都应该有 online 了, 找不到就是有问题 */
	if (!online) {
		ERROR_TLOG("nofound_online by fd=%d, olip=0x%X, cmd=%u, u=%u",
				fd, fdsess->remote_ip, cmd, uid);	
		return -1;
	}

	player_t *player = 0;
	if (cmd == btlsw_player_enter_hall_cmd) {
		player = get_player(uid, role_tm);
		if (player) {
			seat_t *seat = player->seat ? player->seat : 0;
			team_t *team = seat ? seat->team : 0;
			room_t *room = team ? team->room : 0;
			uint32_t room_id = room ? room->id : 0;
			uint8_t team_id = team ? team->id : 0;
			uint8_t seat_id = seat ? seat->id : 0;
			DEBUG_TLOG("player re_enter_hall, lastinfo: u=%u, "
					"role_tm=%u, last_alloc_time=%d"
					"roomid=%u, team=%hhu, seat=%hhu, olid=%u", player->id,
					player->role_tm, player->alloc_time,
					room_id, team_id, seat_id, player->online->id);
			del_player(player);
		}
		player = add_player(online, uid, role_tm);
	} else {
		player = get_player(uid, role_tm);
	}
    
	if (!player) {
		ERROR_TLOG("nofound_player, cmd=%u, u=%u, role_tm=%d, olid=%u",
				cmd, uid, role_tm, online->id);
		return -1;
	}
	if (!player->online || player->online != online) {
		ERROR_TLOG("player->online(%p) != online(%p), "
				"cmd=%u, u=%u, role_tm=%d, olid=%u", player->online, online,
				cmd, uid, role_tm, online->id);
		return -1;
	}
	
	if (cache_flag && player->waitcmd) {
		if (player->cache_pkg_queue->size() < MAX_CACHE_PKG) {
			DEBUG_TLOG("cache_a_pkg u=%u, role_tm=%d, cmd=%u, wcmd=%u",
					player->id, player->role_tm, cmd, player->waitcmd);
			cache_a_pkg(player, pkg, len);
			return 0;
		} else {
			WARN_TLOG("too_many_cache_pkg, u=%u, role_tm=%d, cmd=%u, wcmd=%u",
					player->id, player->role_tm, cmd, player->waitcmd);
			return 0;
		}
	}

    player->waitcmd = cmd;
    player->seq = seq;
    player->last_pkg_time = get_now_tv()->tv_sec;

	bind_proto_cmd_t *p_cmd = find_btlsw_cmd_bind(cmd);
    if (!p_cmd) {
        ERROR_TLOG("btlsw_cmdid_not_existed: %u", cmd);
        return 0;
    }

    uint32_t body_len = len - sizeof(btlsw_proto_t);

    bool read_ret = p_cmd->p_in->read_from_buf_ex((char *)data + sizeof(btlsw_proto_t), body_len);
    if (!read_ret) {
        ERROR_TLOG("read_from_buf_ex_error, cmd=%u, u=%u", cmd, player->id);
        return -1;
    }

    return p_cmd->func(player, p_cmd->p_in, p_cmd->p_out, NULL);
}
