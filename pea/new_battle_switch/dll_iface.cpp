#include "common.hpp"
#include "config.hpp"
#include "player.hpp"
#include "dispatch.hpp"
#include "online.hpp"


#ifdef USE_HEX_LOG_PROTO
int g_log_send_buf_hex_flag = 1;
#endif


extern "C" int init_service(int isparent)
{
    if (isparent) {
		return 0;
	}

	TRACE_TLOG("servive_start");
	srand(time(0));
	setup_timer();

	/* 初始化所有配置 */
	if (init_config() == -1) {
		BOOT_TLOG("Failed to init config");
		return -1;
	}

	/* 初始化所有相关的数据结构 */
/* TODO(zog): 实现它 */
#if 0
	if (init_battle_switch() == -1) {
		BOOT_TLOG("Failed to init service");
		return -1;
	}
#endif

    return 0;
}

extern "C" int fini_service(int isparent)
{
    if (isparent) {
		return 0;
	}

	/* 销毁所有的数据结构 */
/* TODO(zog): 实现它 */
#if 0
	finish_battle_switch();
#endif

	/* 释放配置数据 */
	release_config();

	return 0;
}

extern "C" void proc_events()
{
    proc_cached_pkg();
	proc_closed_rooms();
}

extern "C" int get_pkg_len(int fd, const void *avail_data, int avail_len, int isparent)
{
    if (avail_len < 4) {
        return 0;
    }

    const btlsw_proto_t *pkg = (const btlsw_proto_t *)avail_data;
    uint32_t pkglen = pkg->len;

    if (pkglen > btlsw_proto_max_len
		|| pkglen < sizeof(btlsw_proto_t)) {
		ERROR_TLOG("invalid pkglen=%u not in [%zd, %u], fd=%d",
				pkglen, sizeof(btlsw_proto_t), btlsw_proto_max_len, fd);
        return -1;
    }

    if (pkglen <= (uint32_t)avail_len) {
        return pkglen;
    } else {
        return 0;
    }
}

extern "C" int proc_pkg_from_client(void *data, int len, fdsession_t *fdsess)
{
#ifdef USE_HEX_LOG_PROTO
	char outbuf[4*btlsw_proto_max_len];
	bin2hex(outbuf, (char *)data, len, btlsw_proto_max_len);
	TRACE_TLOG("[IN] %s", outbuf);
#endif

    return dispatch(data, fdsess, true);
}

extern "C" void on_client_conn_closed(int fd)
{
	online_t *online = get_online_by_fd(fd);
    DEBUG_TLOG("online_closed, olip=0x%X, fd=%d, olid=%u",
			online ? online->fdsess->remote_ip : 0, fd, online ? online->id : 0);

	if (online) {
		del_online(online);
	}
}

extern "C" void proc_pkg_from_serv(int fd, void * data, int len)
{
	/* 暂时没有主动连接 */
}

extern "C" void on_fd_closed(int fd)
{
	/* 暂时没有主动连接, 不需要处理主动连接关闭的情况 */
}
