#ifndef CHARITY_DRIVE_H
#define CHARITY_DRIVE_H

#define MAX_DONATE_BOX_ITEMS	100
typedef struct donate_box
{
	uint32_t	count;
	uint32_t	itemids[MAX_DONATE_BOX_ITEMS];
}__attribute__(( packed )) donate_box_t;



int guess_item_price_charity_drive_cmd(sprite_t* p, uint8_t* body, int bodylen);
int guess_item_price_charity_drive_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int query_guess_item_charity_drive_cmd(sprite_t* p, uint8_t* body, int bodylen);
int query_guess_item_charity_drive_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int donate_item_charity_drive_cmd(sprite_t* p, uint8_t* body, int bodylen);
int donate_item_charity_drive_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int query_charity_drive_type_rank_cmd(sprite_t* p, uint8_t* body, int bodylen);
int query_charity_drive_type_rank_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int query_charity_drive_info_cmd(sprite_t* p, uint8_t* body, int bodylen);
int query_charity_drive_info_callback(sprite_t *p, uint32_t id, const char *body, int bodylen);
int load_donate_item_config(const char* file);
int check_item_from_donateitem_conf(int itemid);



#endif
