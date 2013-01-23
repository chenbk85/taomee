#include "player.hpp"
#include "battle_switch.hpp"
#include "room.hpp"
#include "battle.hpp"
#include "utils.hpp"


int g_battle_switch_fd = -1;
char *g_battle_switch_name = config_get_strval("battle_switch_name");


static void send_init_pkg();

void send_init_pkg()
{
    btlsw_online_register_in in;
    in.online_id = get_server_id();
    send_to_battle_switch(0, btlsw_online_register_cmd, &in);
}

void connect_to_battle_switch()
{
    if (resolve_service_name(g_battle_switch_name, 0)) {
        g_battle_switch_fd = connect_to_service(g_battle_switch_name, 0, 65535, 1);
        if (g_battle_switch_fd != -1) {
            DEBUG_TLOG("reg_self_to_btlsw, olid=%u", get_server_id());
            send_init_pkg();
        }
    }
}

int connect_to_battle_switch_timely(void* owner, void* data)
{
    if (!data) {
        ADD_TIMER_EVENT(&g_events, connect_to_battle_switch_timely,
				reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);	
    } else if (g_battle_switch_fd == -1) {
        connect_to_battle_switch();
        if (g_battle_switch_fd == -1) {
            ADD_TIMER_EVENT(&g_events, connect_to_battle_switch_timely,
					reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);	
        }
    }
    return 0;
}

void init_connect_to_battle_switch()
{
    connect_to_battle_switch();
    if(g_battle_switch_fd == -1) {
        connect_to_battle_switch_timely(0, 0);
    }
}

bool is_battle_switch_fd(int fd)
{
    return ((fd >= 0) && (g_battle_switch_fd == fd));
}

void close_battle_switch_fd()
{
    g_battle_switch_fd = -1;
}

int send_to_battle_switch(const player_t* p, uint16_t cmd, uint32_t body_len, void* body_buf)
{
    static uint8_t bsw_pkg[btlsw_proto_max_len] = {0};	
    if (g_battle_switch_fd == -1) {
        connect_to_battle_switch();
    }

    int len = sizeof(btlsw_proto_t) + body_len;
    if (g_battle_switch_fd == -1
		|| body_len > static_cast<int>(sizeof(bsw_pkg) - sizeof(btlsw_proto_t))) {
        ERROR_TLOG("failed send_to_btlsw, fd=%d, olid=%u",
				g_battle_switch_fd, get_server_id());
        return -1;
    }

    btlsw_proto_t* pkg = reinterpret_cast<btlsw_proto_t*>(bsw_pkg);
    pkg->len = len;
    pkg->seq = (p ? ((p->fd) << 16) | (p->waitcmd) : 0);
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = (p ? p->id : 0);
    pkg->role_tm  = (p ? p->role_tm : 0);

    memcpy(pkg->body, body_buf, body_len);

	DEBUG_TLOG("send_to_btlsw_ori[%u] u=%u, cmd=%hu, wcmd=%hu, fd=%u",
			pkg->seq, p ? p->id : 0, cmd, p ? p->waitcmd : 0, g_battle_switch_fd);
    return net_send(g_battle_switch_fd, bsw_pkg, len);
}

int send_to_battle_switch(const player_t *p, uint16_t cmd, Cmessage *p_out)
{
    if (g_battle_switch_fd == -1) {
        connect_to_battle_switch();
    }

    if (g_battle_switch_fd == -1) {
        ERROR_TLOG("failed connect_to_btlsw, u=%u", p ? p->id : 0);
        return -1;
    }

    btlsw_proto_t head;
    head.len = sizeof(btlsw_proto_t);
    // set higher 16 bytes fd and lower 16 bytes waitcmd
    head.seq = (p ? ((p->fd) << 16) | (p->waitcmd) : 0);
    head.cmd = cmd;
    head.ret = 0;
    head.id = (p ? p->id : 0);
    head.role_tm = (p ? p->role_tm : 0);

	DEBUG_TLOG("send_to_btlsw[%u] u=%u, wcmd=%hu, seq=%u, fd=%u",
			cmd, p ? p->id : 0, p ? p->waitcmd : 0, head.seq, g_battle_switch_fd);

    return net_send_msg(g_battle_switch_fd, (char *)&head, p_out);
}

void handle_battle_switch_return(btlsw_proto_t *pkg, uint32_t len)
{
	TRACE_TLOG("btlsw_ret[%u] pkg_uid=%u, seq=%u, len=%u",
			pkg->cmd, pkg->id, pkg->seq, pkg->len);

	if (!pkg->seq) {
		TRACE_TLOG("no_wait_btlsw_return, cmd=%u, u=%u", pkg->cmd, pkg->id);
		return ;
	}

	/*
	 * 注意:
	 * 由于 btlsw 会产生一些svr主动发起的通知包,
	 * 所以此处不能检查 p->waitcmd == (pkg->seq & 0xFFFF);
	 * 同样的理由, 也不能利用 connfd 获得 player, 因此以下两个语句不能在此使用;
		 int connfd  = dbpkg->seq >> 16;
		 uint32_t waitcmd = dbpkg->seq & 0xFFFF;
	 */

    player_t *p = get_player(pkg->id);
    if (!p) {
        ERROR_TLOG("btlsw_ret_nofound_player, btlsw_u=%u, btlsw_cmd=%u",
				pkg->id, pkg->cmd);	
        return;
    }

    uint16_t cmd = pkg->cmd;
    uint32_t ret = pkg->ret;

    if (ret) {
        DEBUG_TLOG("btlsw_ret_err, u=%u, cmd=%u, ret=%u", p->id, cmd, ret);
        send_error_to_player(p, ret);
        return;
    }

    const bind_proto_cmd_t *p_cmd = find_btlsw_cmd_bind(cmd);
    if (!p_cmd) {
        ERROR_TLOG("btlsw_ret_inv_cmd, cmd=%u, u=%u", cmd, p->id);
        return;
    }

    uint32_t body_len = len - sizeof(btlsw_proto_t);

    bool unpack_ret = p_cmd->p_out->read_from_buf_ex((char *)pkg + sizeof(btlsw_proto_t), body_len);
    if (!unpack_ret) {
        ERROR_TLOG("invalid pkglen, cmd=%u, read_len=%u", cmd, len);
        return;
    }

    int err_code = p_cmd->func(p, p_cmd->p_out, NULL, NULL);
    if(err_code) {
        close_client_conn(p->fd);
    }
}
