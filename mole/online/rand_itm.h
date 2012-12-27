
#ifndef ONLINE_RAND_ITM_H
#define ONLINE_RAND_ITM_H

// C89 headers
 #include <time.h>
// C99 headers
#include <stdint.h>

#include <libxml/tree.h>

#include "map.h"
#include <libtaomee/timer.h>



#define RAND_TYPE_BASE	600
#define MAX_RAND_TYPE	999
#define MAX_RAND_CNT	20
#define MAX_RAND_FLAG	400

typedef struct item_type{
	uint32_t type;
	uint32_t max;
	uint32_t itmid;
	uint32_t itm_cnt;
	uint32_t vip;
}__attribute__((packed)) item_type_t;

typedef struct do_rand_type{
	uint32_t cnt;
	item_type_t  ittitem[MAX_RAND_CNT];
}__attribute__((packed)) do_rand_type_t;

int load_rand_items(const char *file);

int parse_rand_type_items(do_rand_type_t *p_rand_type, xmlNodePtr cur);

int send_rand_item_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int db_add_rand_item(sprite_t* p);

int db_add_rand_item_callback(sprite_t *p);

int rand_item_swap_cmd(sprite_t *p, const uint8_t *body, int bodylen);

#endif

