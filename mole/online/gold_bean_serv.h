
#ifndef GOLD_BEAN_SERVER_H
#define GOLD_BEAN_SERVER_H

#include "sprite.h"
#include "item.h"

#define GOLD_BEAN_PROTO_INQUIRE_MULTI_ITEMS_PRICE 	1002
#define GOLD_BEAN_PROTO_INQUIRE_ONE_ITEM_PRICE		1003
#define GOLD_BEAN_PROTO_BUY_ITEM_USE_GOLD_BEAN		1004
#define GOLD_BEAN_PROTO_GET_GOLD_BEAN_INFO			1005
#define GET_BEAN_PROTO_GET_GOLD_BEAN_COUNT			0x0401

//增加包月抵用券
#define TICKET_SERVER_PROTO_ADD_TICKET_CNT			0x9301
#define TICKET_SERVER_PROTO_GET_TICKET_INFO			0x9202


// free super lahm activity
#define FREE_VIP_SERVER_PROTO_SET_INFO              0x9305
#define FREE_VIP_SERVER_PROTO_GET_INFO              0x9206
#define FREE_VIP_SERVER_PROTO_LEFT_TIME             0x9208

extern int goldbean_serv_fd;
extern int getbean_serv_fd;
extern int ticket_serv_fd;	//增加包月抵用券

int gold_bean_inquire_multi_items_price_cmd(sprite_t* p, const uint8_t* body, int len);
int gold_bean_inquire_one_item_price_cmd(sprite_t* p, const uint8_t* body, int len);
int buy_item_use_gold_bean_cmd(sprite_t* p, const uint8_t* body, int len);
//int get_gold_bean_info_cmd(sprite_t* p, const uint8_t* body, int len);
int give_gift_after_buy_item(sprite_t* p, uint32_t money_1_number);

int send_request_to_goldbean_serv(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id);
int handle_goldbean_serv_return(int fd, server_proto_t* dbpkg, int len, sprite_t** sp);

int get_gold_bean_count_cmd(sprite_t* p, const uint8_t* body, int len);
int send_request_to_getbean_serv(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id);
int handle_getbean_serv_return(int fd, server_proto_t* dbpkg, int len, sprite_t** sp);

//拉取米币数量的协议
int get_gold_bean_info_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code);

int get_bean_mall_login_times_cmd(sprite_t* p, const uint8_t* body, int len);
int get_bean_mall_login_times_callback(sprite_t* p, uint32_t id, char* buf, int len);

//online增加包月抵用券 VIP包月时用 20120113上线
int add_vip_month_ticket(sprite_t* p, uint32_t ticket_type, uint32_t id);
int send_request_to_ticket_serv(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id);
int handle_ticket_serv_return(int fd, server_proto_t* dbpkg, int len, sprite_t** sp);

//得到包月抵用券 20120113上线
int get_vip_month_ticket_info_cmd(sprite_t* p, const uint8_t* body, int len);
int get_vip_month_ticket_info_callback(sprite_t * p, uint32_t id, char * buf, int len, int ret_code);

//free vip activity
int set_free_vip_info_cmd(sprite_t *p, const uint8_t* body, int len);
int set_free_vip_info_callback(sprite_t *p, uint32_t id, char* buf, int len, int ret_code);
int get_free_vip_info_cmd(sprite_t *p, const uint8_t* body, int len);
int get_free_vip_info_callback(sprite_t* p, uint32_t id, char* buf, int len, int ret_code);

int check_prior_dice_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_free_player_info_cmd(sprite_t *p, const uint8_t* body, int len);
int get_free_player_info_callback(sprite_t* p, uint32_t id, char* buf, int len);


int get_lottery_draw_left_time_cmd(sprite_t *p, const uint8_t* body, int len);
int get_lottery_draw_left_time_callback(sprite_t* p, uint32_t id, char* buf, int len, int ret_code);

#endif
