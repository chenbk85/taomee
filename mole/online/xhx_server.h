/*
 * =====================================================================================
 *
 *       Filename:  xhx_server.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2011 03:48:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef XHX_SERVER_H
#define XHX_SERVER_H

#include "sprite.h"
#include "item.h"
#include "protoheaders.h"

#define XHX_SWITCH_PROTO_USER_SET_VALUE		62014
extern int xhx_server_fd;

int send_request_to_xhxserver(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id);
int handle_xhxserver_return(int fd, server_proto_t* dbpkg, int len, sprite_t** sp);

int deal_xhx_event(sprite_t* p, uint32_t type);

//小花仙联运活动接取 4110
int xhx_get_tast_cmd(sprite_t* p, const uint8_t* body, int len);
int xhx_get_tast_callback(sprite_t * p, uint32_t id, char * buf, int len);

//查询是否从小花仙时光门过来的用户 4111
int xhx_check_user_cmd(sprite_t* p, const uint8_t* body, int len);
int xhx_check_user_callback(sprite_t * p, uint32_t id, char * buf, int len);
int user_check_entry_xhx_callback(sprite_t* p, uint32_t id, char* buf, int len);

//小花仙增加物品 4112
int xhx_add_items_cmd(sprite_t* p, const uint8_t* body, int len);
int xhx_add_one_item(sprite_t * p, uint32_t state);
int xhx_check_add_items_callback(sprite_t* p, uint32_t id, char* buf, int len);

#endif
