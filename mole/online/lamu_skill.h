
#ifndef LAMU_SKILL_ITM_H
#define LAMU_SKILL_ITM_H

// C89 headers
 #include <time.h>
// C99 headers
#include <stdint.h>

#include <libxml/tree.h>

#include "map.h"
#include <libtaomee/timer.h>

#define MAX_SKILL_NUM 40
#define MAX_ITEM_NUM 10

struct skill_item {
	uint32_t id;
	uint32_t item_id[MAX_ITEM_NUM];
	uint32_t vip_limit;
	uint32_t pl_limit;
	uint32_t sub_attr;
	uint32_t add_change;
	uint32_t need_change;
};

int load_skill_config(xmlNodePtr cur_node);
struct skill_item*  get_lahm_skill(uint32_t type, uint32_t bits);
int check_itemid(sprite_t *p, uint32_t id, int skill_type, int *item_skill_type);
int check_skill(sprite_t *p, uint32_t skill_type);
int get_item_by_skill_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_item_by_skill_callback(sprite_t *p, uint32_t id, char *buf, int len);
int lamu_end_action(void* owner, void* data);
int lamu_do_action_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int sub_lamu_basic_attire(sprite_t *p, pet_t *pet, int change_value);
int remove_lamu_action_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int check_five_stage_accord_skill(sprite_t *p, uint32_t skill_type);
int enter_five_stage(pet_t *pet);
uint32_t get_change_level(uint32_t change_value);
uint32_t get_day_limit(pet_t *pet);
int check_three_basic_skill(pet_t *pet);

int lahm_use_skill_do_work_cmd(sprite_t *p, const uint8_t *body, int bodylen);

#endif

