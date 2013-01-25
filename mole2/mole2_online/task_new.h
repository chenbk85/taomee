#ifndef MOLE2_TASK_NEW_H
#define MOLE2_TASK_NEW_H

#include "benchapi.h"
#include "dbproxy.h"
#include "cli_proto.h"
#include "svr_proto.h"
#include "sprite.h"

#define TASK_DIARY_BUF_LEN		156
#define MAX_RECV_TASK_CNT		200
#define TASK_LOOP_BASE			1000000

/* task type define */
enum {
	TASK_TYPE_MAJOR = 1,
	TASK_TYPE_MINOR,
	TASK_TYPE_DAY,
	TASK_TYPE_STORY,
};

/* task item give type */
enum {
	OUT_TYPE_PLAYER_ATTR	= 1,
	OUT_TYPE_NORMAL_ITEM	= 2,
	OUT_TYPE_PET			= 3,
	OUT_TYPE_CLOTHES		= 4,
	OUT_TYPE_SKILL			= 5,
	OUT_TYPE_HONOR			= 6,
	OUT_TYPE_FUNCTION		= 7,
	OUT_TYPE_MAX
};

enum {
	beast_target_beast,
	beast_target_group,
};

enum {
	svr_buf_beast		= 0,
	svr_buf_pk			= 1,
	svr_buf_item		= 2,
	svr_buf_skill		= 3,
	svr_buf_item_gen	= 4,
};

enum {
	TASK_STATE_CANCEL,
	TASK_STATE_RECVD,
	TASK_STATE_FIN,
	TASK_STATE_FIN_RECV,
	TASK_STATE_NONE,
};

typedef struct task_reward {
	task_db_t		task;
	uint32_t 		xiaomee;
	uint32_t		level;
	uint32_t		exp;
    uint32_t        expbox;
	uint32_t		skill_expbox;
	uint32_t		attr_addition;
	uint32_t		hp;
	uint32_t		mp;
	uint32_t		uiflag;
	uint32_t		proflv;
	uint32_t		cloth_cnt;
	uint32_t		item_cnt;
	uint32_t		pet_cnt;
	uint32_t		honor_cnt;
	uint32_t		skill_cnt;
	uint32_t		del_pet_cnt;
}__attribute__((packed))task_reward_t;

#define CHECK_TASK_RECVD(p_, pt_, taskid_) \
	do { \
		if (!pt_) { \
			KERROR_LOG(p_->id, "no recv\t[%u]", taskid_); \
			return send_to_self_error(p_, p_->waitcmd, cli_err_task_not_recvd, 1); \
		} \
	} while (0)

#define CHECK_TASK_ID_VALID(p_, pt_, taskid_) \
	do { \
		if (!pt_) { \
			KERROR_LOG(p_->id, "invalid\t[%u]", taskid_); \
			return send_to_self_error(p_, p_->waitcmd, cli_err_task_id_invalid, 1); \
		} \
	} while (0)


int load_tasks(xmlNodePtr cur_task);	
int load_task_loops(xmlNodePtr cur_task);
int get_task_all_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int recv_task_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int cancel_task_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_task_clibuf_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_task_svrbuf_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_task_detail_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_task_detail_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int write_task_diary_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int write_task_diary_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int get_task_db_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret);
int set_task_db_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret);

int add_task_reward_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret);


void init_all_tasks();
void fini_all_tasks();

task_t* get_task(uint32_t taskid);
task_node_t* get_task_node(uint32_t taskid, uint32_t nodeid);

task_db_t* cache_get_task_info(sprite_t* p, uint32_t taskid);
int cache_add_recvd_task(sprite_t* p, task_db_t* ptd);
int cache_del_recvd_task(sprite_t* p, uint32_t taskid);
int cache_check_task_ever_fin(sprite_t* p, uint32_t taskid);
int cache_check_task_node_ever_fin(sprite_t* p, uint32_t taskid, uint32_t nodeid);
int cache_init_task_info(sprite_t* p, uint32_t fincnt, uint32_t oncnt, uint8_t* buf);

static inline int
rsp_task_update_db(sprite_t* p, uint32_t taskid, uint32_t nodeid, uint32_t outid, uint32_t info, uint32_t clival)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, taskid, i);
	PKG_UINT32(msg, clival, i);
	PKG_UINT32(msg, nodeid, i);
	PKG_UINT32(msg, outid, i);
	PKG_UINT32(msg, info, i);
	PKG_UINT32(msg, 0, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

static inline int cache_check_task_fin(sprite_t* p, uint32_t taskid)
{
	task_fin_t* ptf = (task_fin_t *)g_hash_table_lookup(p->fintasks, &taskid);
	return ptf ? ptf->fin : 0;
}


#endif
