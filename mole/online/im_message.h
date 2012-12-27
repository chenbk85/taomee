/*
 * =====================================================================================
 *
 *       Filename:  im_message.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/17/2010 04:56:28 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#ifndef  IM_MESSAGE_INC
#define  IM_MESSAGE_INC

#include <stdint.h>

typedef struct {
	uint32_t proto_length; //报文总长度
	uint32_t proto_id;//序列号，需要原样返回
	uint16_t cmd_id; //命令号
	int32_t result; //返回值
	userid_t id;  /*米米号*/
	uint8_t body[];
} __attribute__((packed)) im_msg_header_t;

extern int im_fd;

/*用户登入的时候，给IM发消息*/
#define IM_USER_LOGIN_CMD	0xF040

/*用户退出的时候，给IM发消息*/
#define IM_USER_QUIT_CMD	0xF041

int send_im_pkg_user_login(sprite_t *p);

int send_im_pkg_user_quit(sprite_t *p);

#endif

/* ----- #ifndef IM_MESSAGE_INC  ----- */

