#ifndef _K_STREET_H_
#define  _K_STREET_H_

#include "sprite.h"

struct ret_309F {
	uint32_t dish_cnt;
	uint32_t dish_lvl;
};

struct req_3186 {
	uint32_t id;
	uint32_t count;
	uint32_t location;
	uint32_t price;
	uint32_t channel;
	uint32_t condition;
	uint32_t food[6];
	uint32_t addexp;
	uint32_t sell_price;
};

struct req_3187 {
	uint32_t dish_idx;
	uint32_t dish_id;
	uint32_t dish_to;
	uint32_t timer;
	uint32_t addexp;
	uint32_t addcount;
	uint32_t timeout;
	uint32_t dish_local;
};

struct	dish_item{
	uint32_t id;
	uint32_t dish_id;
	uint32_t state;
	uint32_t location;
	uint32_t count;
	uint32_t time;
	uint32_t sell_money;
}__attribute__((packed));


int grid_nearly(uint32_t grid1, uint32_t grid2);
int get_map_last_grid_cmd(sprite_t * p,const uint8_t * body,int len);
int get_map_last_grid_callback(sprite_t* p, uint32_t id, char* buf, int len);

int list_map_building_info_cmd(sprite_t * p,const uint8_t * body,int len);
int list_map_building_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int create_building_cmd(sprite_t * p,const uint8_t * body,int len);
int create_building_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_certificates_for_building_cmd(sprite_t * p,const uint8_t * body,int len);
int db_get_building_certificates(sprite_t *p);

int set_building_name_cmd(sprite_t * p,const uint8_t * body,int len);
int set_building_name_callback(sprite_t* p, uint32_t id, char* buf, int len);

int set_building_style_cmd(sprite_t * p,const uint8_t * body,int len);
int set_building_style_callback(sprite_t* p, uint32_t id, char* buf, int len);

int set_building_inner_style_cmd(sprite_t * p,const uint8_t * body,int len);
int set_building_inner_style_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_shop_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_shop_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_employee_list_cmd(sprite_t * p,const uint8_t * body,int len);
int get_employee_list_callback(sprite_t* p, uint32_t id, char* buf, int len);

int add_employee_cmd(sprite_t * p,const uint8_t * body,int len);
int add_employee_callback(sprite_t* p, uint32_t id, char* buf, int len);

int del_employee_cmd(sprite_t * p,const uint8_t * body,int len);
int del_employee_callback(sprite_t* p, uint32_t id, char* buf, int len);

int cooking_dish_cmd(sprite_t * p,const uint8_t * body,int len);
int cooking_dish_callback(sprite_t* p, uint32_t id, char* buf, int len);

int eat_dish_cmd(sprite_t * p,const uint8_t * body,int len);
int eat_dish_callback(sprite_t* p, uint32_t id, char* buf, int len);

int clean_dish_cmd(sprite_t * p,const uint8_t * body,int len);
int clean_dish_callback(sprite_t* p, uint32_t id, char* buf, int len);

int change_dish_state_cmd(sprite_t * p,const uint8_t * body,int len);
int change_dish_state_callback(sprite_t* p, uint32_t id, char* buf, int len);

int put_dish_in_box_cmd(sprite_t * p,const uint8_t * body,int len);
int put_dish_in_box_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_shop_board_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_shop_board_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_shop_item_lock_cmd(sprite_t * p,const uint8_t * body,int len);
int get_shop_item_list_cmd(sprite_t * p,const uint8_t * body,int len);
int get_shop_honor_list_cmd(sprite_t * p,const uint8_t * body,int len);

int sub_shop_evaluate_cmd(sprite_t * p,const uint8_t * body,int len);
int sub_shop_evaluate_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_shop_count_cmd(sprite_t * p,const uint8_t * body,int len);
int get_shop_count_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_pet_employed_list_cmd(sprite_t * p,const uint8_t * body,int len);
int get_pet_employed_list_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_friend_list_cmd(sprite_t * p,const uint8_t * body,int len);
int get_friend_list_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_restuarant_event_cmd(sprite_t * p,const uint8_t * body,int len);
int get_restuarant_event_callback(sprite_t* p, uint32_t id, char* buf, int len);

int db_change_shop_exp(sprite_t * p,uint32_t exp, uint32_t id);
int db_change_shop_money(sprite_t * p,uint32_t money, uint32_t id);
int db_add_shop_event_count(sprite_t * p, uint32_t id);
int add_shop_event_count_callback(sprite_t* p, uint32_t id, char* buf, int len);

int db_set_dish_level(sprite_t * p,uint32_t dishid,uint32_t level, uint32_t id);
int get_dish_count_and_level_cmd(sprite_t * p,const uint8_t * body,int len);
int get_dish_count_and_level_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_dish_count_and_level_all_cmd(sprite_t * p,const uint8_t * body,int len);
int get_dish_count_and_level_all_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_user_dish_all_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_restuarant_event_cmd(sprite_t * p,const uint8_t * body,int len);
int set_restuarant_event_callback(sprite_t* p, uint32_t id, char* buf, int len);
int magic_man_change_dish(sprite_t* p, struct dish_item *pfood, int count);

int get_magic_man_eat_dish_cmd(sprite_t * p,const uint8_t * body,int len);
int get_magic_man_eat_dish(sprite_t* p, struct dish_item *pfood, int count);

int set_magic_man_eat_dish_cmd(sprite_t * p,const uint8_t * body,int len);
int set_magic_man_eat_dish_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_dish_count_cmd(sprite_t * p,const uint8_t * body,int len);
int get_dish_count_callback(sprite_t* p, uint32_t id, char* buf, int len);

#endif //_K_STREET_H_

