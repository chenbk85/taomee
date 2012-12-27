#ifndef FRIENDSHIP_BOX_H
#define FRIENDSHIP_BOX_H
#include "item.h"

#define MAX_FRIEND_BOX_ITEMS	100
typedef struct friend_box
{
	uint32_t	count;
	uint32_t	itemids[MAX_FRIEND_BOX_ITEMS];
}__attribute__(( packed )) friend_box_t;

int add_item_in_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen);
int add_item_in_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);

int get_item_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen);//self or other
int get_item_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);

int check_item_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);

int query_items_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen);//self or other
int query_items_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);

int get_item_history_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen);//self or other
int get_item_history_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int check_item_from_friendbox_conf(int itemid);
int load_friendbox_config(const char* file);

int mw_add_item_in_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen);
int mw_add_item_in_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);

int mw_get_item_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen);//self or other
int mw_get_item_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);

int mw_check_item_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);

int mw_query_items_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen);//self or other
int mw_query_items_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);

int mw_get_item_history_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen);//self or other
int mw_get_item_history_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int mw_check_item_from_friendbox_conf(int itemid);
int mw_load_friendbox_config(const char* file);


#endif
