#ifndef ADD_STH_RAND_H
#define	ADD_STH_RAND_H

#include "item.h"

typedef struct {
	item_t*	itm;
	/*物品的数目*/
	int	count;
	/*概率的范围*/
	float rand_start;
	float rand_end;
} item_rand_unit_t;

typedef struct {
	uint32_t		id;
	uint32_t		vip;
	uint32_t		day_type;
	uint32_t		day_count;
	uint32_t		count_start;
	uint32_t		count_end;
	uint32_t		day_begin;
	uint32_t		day_end;
	int				incnt;
	item_rand_unit_t in[MAX_RAND_EXCHANGE_ITEMS];
	int				outcnt[MAX_EXCHG_OUT_KIND];
	item_rand_unit_t		out[MAX_EXCHG_OUT_KIND][MAX_RAND_EXCHANGE_ITEMS];
	uint32_t		outkind_cnt;
}__attribute__((packed)) rand_t;

typedef struct {
	uint32_t entry_id;
	uint32_t itemout_index;
	uint32_t count;
}__attribute__((packed)) add_sth_rand_t;

int load_rand_conf(const char *file);
int get_sth_accord_rand_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int get_sth_accord_rand_callback(sprite_t* p, uint32_t id, char* buf, int len);
int day_limit_callback(sprite_t *p, uint32_t day_cnt);
int get_bean_lottery_count_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_random_gain_times_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_gold_compass_chance_data_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_other_gifts(sprite_t* p, uint32_t entry_id, uint32_t itemid);

#endif
