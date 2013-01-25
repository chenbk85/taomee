#ifndef BOX_H_
#define BOX_H_

#include "benchapi.h"
#include "dbproxy.h"
#include "cli_proto.h"
#include "svr_proto.h"
#include "sprite.h"

box_t* get_box(uint32_t boxid);

int load_box(xmlNodePtr cur);
int activate_boxes();
int activate_one_box(void* owner, void* data);
void activate_grasses();

int open_box_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_box_active_cmd(sprite_t *p, uint8_t *body, uint32_t len);
beast_group_t* get_box_beast_group(sprite_t* p);
void noti_user_box_item(sprite_t* p, box_t* pb);

#endif
