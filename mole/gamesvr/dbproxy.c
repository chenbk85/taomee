#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#include "dbproxy.h"
#include "tcp.h"
#include "proto.h"
#include "config.h"
#include <libtaomee/log.h>
#include "net.h"
#include "sprite.h"
#include "service.h"
#include "mcast.h"

int proxy_fd = -1;
static uint8_t tmp_buffer[4096];

/**
 * @brief 把一段数据作为给定命令的输入数据
 * @param int cmd 指定命令的编号
 * @param sprite_t* p ???
 * @param int body_len 需要发送的数据长度
 * @param void* body_buf 需要发送的数据
 * @param uint32_t id 用户米米号
 */
int send_request_to_db(int cmd, sprite_t *p, int body_len, void *body_buf, uint32_t id)
{
	server_proto_t *pkg;
	int len;

	if (proxy_fd == -1)
		//proxy_fd = connect_to_svr( config_get_strval ("dbproxy_ip"),
								   //config_get_intval("dbproxy_port", 0), 65535, 1 );
		proxy_fd = connect_to_service( config_get_strval ("dbproxy_ip"), 0, 65535, 1 );

	len = sizeof(server_proto_t) + body_len;
	if ( proxy_fd == -1 || proxy_fd > epi.maxfd ||
			epi.fds[proxy_fd].cb.sndbufsz < epi.fds[proxy_fd].cb.sendlen + len ||
			body_len > sizeof(tmp_buffer) - sizeof(*pkg) )
		ERROR_RETURN( ("send to dbproxy failed, buflen=%d, fd=%d",
						epi.fds[proxy_fd].cb.sendlen, proxy_fd), -1 );

	pkg = (server_proto_t *)tmp_buffer;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
    if(!p) {
        pkg->seq = 0;
    } else {
        pkg->seq = (sprite_fd(p) << 16)| (p->waitcmd & 0xFFFF);
    }
	pkg->ret = 0;
	memcpy(pkg->body, body_buf, body_len);

	TRACE_LOG("send db: len=%d,cmd=0x%x,id=%d,seq=%d", len, pkg->cmd, pkg->id, pkg->seq);

	return net_send(proxy_fd, (const uint8_t *)tmp_buffer, len, 0);
}
