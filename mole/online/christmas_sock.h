#ifndef CHRISTMAS_SOCK_H
#define CHRISTMAS_SOCK_H
#include "item.h"


int add_christmas_sock_cmd(sprite_t* p, uint8_t* body, int bodylen);
int add_christmas_sock_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int get_christmas_sock_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_christmas_sock_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int christmas_sock_add_count_to_db(sprite_t* p);
int get_item_count(uint32_t sock_count);
int christmas_sock_send_bonus_to_client(sprite_t * p);
int get_christmas_sock_bonus_cmd(sprite_t * p,const uint8_t * body,int len);
int get_rand_itemid( );

//2011圣诞活动
int hang_christmas_sock_cmd(sprite_t* p, uint8_t* body, int bodylen);
int hang_christmas_scok_callback(sprite_t* p, uint32_t id, const char *body, int bodylen);
int check_christmas_sock_cnt_callback(sprite_t* p, uint32_t id, const char *body, int bodylen);

int get_christmas_sock_record_cmd(sprite_t * p,const uint8_t * body,int len);
int get_christmas_sock_record_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_christmas_sock_top10_cmd(sprite_t * p,const uint8_t * body,int len);
int get_christmas_sock_top10_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_christmas_sock_giftbox_cmd(sprite_t* p, const uint8_t * body, int len);
int get_christmas_sock_giftbox_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_christmas_sock_gift_cmd(sprite_t* p, const uint8_t * body, int len);
int check_christmas_sock_gift_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pack_christmas_sock_gift(sprite_t* p, uint8_t* message, uint32_t gift_cnt, int *l);
#endif
