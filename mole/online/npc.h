#ifndef ONLINE_NPC_H_
#define ONLINE_NPC_H_

#include <stdint.h>

#include "benchapi.h"

#define NPC_NUMBER_MAX		1024
#define NPC_BASE_ID			2000
// color
#define NPC_COLOR_KHAKI		0xBF832D
#define NPC_COLOR_RED		0xD42B14
#define NPC_COLOR_ORANGE	0xFA7F20
#define NPC_COLOR_GREEN		0x009F47
#define NPC_COLOR_BLUE		0x34A6CD
#define	NPC_COLOR_PURPLE	0xFA4BA0
#define NPC_COLOR_GREY		0x6D6D6D
#define NPC_COLOR_BLUE2		0x821792

enum {
	NPC_TYPE_NONE = 0,
	NPC_TYPE_GUIDE,
	NPC_TYPE_WAITER,
	NPC_TYPE_MAX
};

typedef struct {
	uint32_t id;
	uint32_t type;
	uint32_t count;
	char *msg[5];
} choice_struct;

int create_npc_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len);

sprite_t* new_npc(const char* nick, uint32_t color, const uint32_t dress[], uint8_t dress_cnt);
sprite_t* get_npc(uint32_t id);

void free_npc(sprite_t* npc);

int  init_npc_system(const char *file);
void destroy_npc_system();
//extern void check_triggered_task (sprite_t *p, int cmd);

void create_npc_test();






#endif // ONLINE_NPC_H_
