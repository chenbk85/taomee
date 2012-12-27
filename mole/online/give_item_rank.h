#ifndef GIVE_ITEM_RANK_H
#define GIVE_ITEM_RANK_H
#include "item.h"


int give_item_to_friend_cmd(sprite_t* p, uint8_t* body, int bodylen);
int check_item_give_friend_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int query_type_rank_cmd(sprite_t* p, uint8_t* body, int bodylen);
int query_type_rank_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int query_get_item_history_cmd(sprite_t* p, uint8_t* body, int bodylen);
int query_get_item_history_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int get_dec_send_count_by_item(uint32_t itemid);
int get_dec_get_count_by_item(uint32_t itemid);
int get_type_count_item_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_type_count_item_bonus_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int get_type_rank_count_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_type_rank_count_info_callback(sprite_t* p, uint32_t id, char* buf, int len);



#endif
