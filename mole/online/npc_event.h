#ifndef NPC_EVENT_H
#define NPC_EVENT_H

#include "item.h"


typedef struct {
	uint32_t		npc_taskid;
	uint32_t		npc_id;
	int32_t			npc_lovely;
	uint8_t			vip_task;
	uint32_t		task_type;
	uint32_t		smc;
	uint32_t		max_num;
	int				incnt;
	item_unit_t		in[MAX_EXCHANGE_ITEMS];
	int				outcnt[MAX_EXCHG_OUT_KIND];
	item_unit_t		out[MAX_EXCHG_OUT_KIND][MAX_EXCHANGE_ITEMS];
	uint32_t		outkind_cnt;
}__attribute__(( packed ))npc_task_t;

#define VIP_NPC_TASK(task_type) (task_type&1)

#define NOT_INCLUDE_MONEY_DAY_LIMIT	202
#define NOT_LIMIT_BY_OTHER_STH_MAX	3000

int get_npc_lovely_cmd(sprite_t* p, uint8_t* body, int bodylen);
int update_npc_lovely_cmd(sprite_t* p, uint8_t* body, int bodylen);
int take_npc_task_cmd(sprite_t* p, uint8_t* body, int bodylen);
int take_npc_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int fin_npc_task_cmd(sprite_t* p, uint8_t* body, int bodylen);
int fin_npc_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_nt_client_data_cmd(sprite_t* p, uint8_t* body, int bodylen);
int set_nt_client_data_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_nt_client_data_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_nt_client_data_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_npc_task_status_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_npc_task_status_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_npc_tasks_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_npc_tasks_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cancel_npc_task_cmd(sprite_t* p, uint8_t* body, int bodylen);
int cancel_npc_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int fin_npc_task_to_self(sprite_t* p);
int do_fin_npc_task(sprite_t* p);
int load_npc_tasks(const char *file);



#endif
