#ifndef FOOT_PRINT_H
#define FOOT_PRINT_H
#include "item.h"


int get_foot_print_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_foot_print_bonus_mgr(sprite_t* p);
int get_itemid_by_num(uint32_t num);
int foot_print_get_items_cmd(sprite_t * p,const uint8_t * body,int len);
int get_foot_print_count_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_foot_print_count_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int exchange_sth_by_footprint_count_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_foot_print_count(sprite_t *p, uint32_t itemid, uint32_t count);
int exchange_foot_print_msglog(sprite_t *p, uint32_t gameid, uint32_t i);



#endif
