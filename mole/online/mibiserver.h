
#ifndef MIBI_SERVER_H
#define MIBI_SERVER_H

#include "sprite.h"
#include "item.h"

#define MIBI_PROTO_INQUIRE_ONE_ITEM_PRICE		1
#define MIBI_PROTO_INQUIRE_MULTI_ITEMS_PRICE	2
#define MIBI_PROTO_INQUIRE_ONE_ITEM_DETAIL		3
#define MIBI_PROTO_BUY_ITEM_USE_MIBI			4
#define MIBI_PROTO_BUY_ITEM_USE_MIBI_COUPON	    5
#define MIBI_PROTO_GET_MIBI_COUPON_INFO         6
#define COUPON_PROTO_ADD                        2001
#define MIBI_PROTO_BUY_GOLD_BEAN				1004

extern int mibiserver_fd;
extern int couponserver_fd;
extern int mibi_buy_bean_fd;

int is_set_pay_passwd_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int is_set_pay_passwd_callback(sprite_t* p, uint32_t id, char* buf, int len);

int inquire_one_item_price_cmd(sprite_t* p, const uint8_t* body, int len);
int inquire_multi_items_price_cmd(sprite_t* p, const uint8_t* body, int len);
int inquire_one_item_detail_cmd(sprite_t* p, const uint8_t* body, int len);
int buy_item_use_mibi_cmd(sprite_t* p, const uint8_t* body, int len);
int buy_item_use_mibi_coupon_cmd(sprite_t* p, const uint8_t* body, int len);
int get_mibi_coupon_info_cmd(sprite_t* p, const uint8_t* body, int len);

int send_request_to_mibiserver(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id);
int handle_mibiserver_return(int fd, server_proto_t* dbpkg, int len, sprite_t** sp);
int handle_couponserver_return(int fd, server_proto_t* mibipkg, int len, sprite_t** sp);
int add_mibi_consume_coupon(sprite_t* p, int cnt);

int handle_mibi_buy_bean_return(int fd, server_proto_t* mibipkg, int len, sprite_t** sp);
int send_request_to_mibi_buy_bean_serv(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id);
int buy_gold_bean_use_mibi_cmd(sprite_t* p, const uint8_t* body, int len);
#endif
