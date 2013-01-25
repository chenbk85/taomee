#ifndef MOLE2_NPC_H_
#define MOLE2_NPC_H_

#include <stdint.h>

#include "benchapi.h"
#include "util.h"
#include "map.h"
#include "cli_proto.h"
#include "battle.h"

#include "global.h"


void init_npcs();
void fini_npcs(int destroy);
sprite_t* get_npc(uint32_t npcid);
int load_npc(xmlNodePtr cur);

void init_shops();
void fini_shops();
int load_shop_item(xmlNodePtr cur);
int load_shop_skill(xmlNodePtr cur);

shop_t* get_shop_no_chk(uint32_t shopid);

int item_shop_existed(uint32_t shopid, uint32_t itemid, map_id_t mapid);
int skill_shop_existed(uint32_t shopid, uint32_t skillid, map_id_t mapid);
int pet_skill_shop_existed(uint32_t shopid, uint32_t itemid, uint32_t lv, map_id_t mapid);

int add_npc_to_team_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int dismiss_npc_cmd(sprite_t *p, uint8_t *body, uint32_t len);

#endif // ONLINE_NPC_H_

