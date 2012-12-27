/*
 * =====================================================================================
 *
 *       Filename:  im_message.c
 *
 *    	Description: 在用户登入或者退出的时候，发送消息到IM
 *
 *        Version:  1.0
 *        Created:  08/17/2010 04:56:32 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <libtaomee/conf_parser/config.h>

#include "util.h"
#include "sprite.h"
#include "protoheaders.h"
#include "proto.h"
#include "dbproxy.h"
#include "im_message.h"

int im_fd = -1;

/* @brief 发包到IM
 */
int send_to_im(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];

	if (im_fd == -1) {
		im_fd = connect_to_svr(config_get_strval("im_ip"), config_get_intval("im_port", 0), 65535, 1);
	}

	int len = sizeof (server_proto_t) + body_len;
	im_msg_header_t *pkg = (im_msg_header_t *)dbbuf;

	if (im_fd == -1 || im_fd > epi.maxfd
		|| epi.fds[im_fd].cb.sndbufsz < epi.fds[im_fd].cb.sendlen + len
		|| body_len > sizeof(dbbuf) - sizeof(*pkg)) {
			ERROR_LOG ("send to im failed, buflen=%d, fd=%d", epi.fds[im_fd].cb.sendlen, im_fd);
			if (p) {
				return send_to_self_error(p, p->waitcmd, -ERR_system_error, 1);
			}
		return -1;
	}

	pkg->proto_length = len;
	pkg->cmd_id = cmd;
	pkg->id = id;
	pkg->proto_id = 0;
	pkg->result = 0;
	memcpy (pkg->body, body_buf, body_len);
	return net_send (im_fd, dbbuf, len, 0);
}

/* @brief 用户登入的时候，发包到IM
 */
int send_im_pkg_user_login(sprite_t *p)
{
	struct {
		uint32_t game_id;
		uint32_t online_id;
	}__attribute__((packed)) login_info_t;

	login_info_t.game_id = 1;
	login_info_t.online_id = config_cache.bc_elem->online_id ;
	return send_to_im(IM_USER_LOGIN_CMD, p, sizeof(login_info_t), &login_info_t, p->id);
}

/* @brief 用户退出的时候发送
 */
int send_im_pkg_user_quit(sprite_t *p)
{
	uint32_t game_id = 1;
	return send_to_im(IM_USER_QUIT_CMD, p, sizeof(game_id), &game_id, p->id);
}
