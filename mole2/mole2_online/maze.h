#ifndef MOLE2_MAZE_H
#define MOLE2_MAZE_H

#include "sprite.h"
#include "global.h"

int load_maze_xml(xmlNodePtr cur);

void start_maze_timer();
int maze_state_change(void* owner, void* data);

maze_t* get_maze(uint32_t maze_idx);
maze_map_t* get_maze_map(int index);
maze_t* find_maze_layer(sprite_t *p,map_id_t newmap, int *idx);

int get_shop_in_map_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_maze_in_map_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int open_maze_portal_cmd(sprite_t *p, uint8_t *body, uint32_t len);
map_copy_config_t* get_map_copy_config(uint32_t copyid);
int load_map_copy(xmlNodePtr cur);

#endif
