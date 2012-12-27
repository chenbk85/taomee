#ifndef ONLINE_TASKS_H_
#define ONLINE_TASKS_H_

#include <stdint.h>

#include <libxml/tree.h>
#include <libtaomee/bitmanip/bitmanip.h>

#include "benchapi.h"
#include "proto.h"

#define SVR_PROTO_SET_TASK2				0x1171
#define SVR_PROTO_SET_MAGIC_TASK		0x1172
#define SVR_PROTO_FIN_MAGIC_TASK		0x1174
#define SVR_PROTO_CANCEL_MAGIC_TASK		0x117F
#define SVR_PROTO_SET_MAGIC_TASK_DATA	0x1186
#define SVR_PROTO_GET_MAGIC_TASK_DATA	0x1087
#define SVR_PROTO_GET_MAGIC_TASK_LIST	0x1088

#define SVR_PROTO_GET_TASK2_LIST	0x1073

enum {
	TASK_max_name_len	= 25
};

typedef struct Task {
	uint32_t	id;
	char		name[TASK_max_name_len];
	uint32_t		task_time;
	int			task_end_flag;
	int			coins_per_day;
	int			pet_is_vip;
	uint32_t	award_item_id;
} task_t;

typedef struct TaskElemUpd {
	uint32_t	petid;
	uint32_t	taskid;
	uint32_t	flag;     // 1 pending task, 2 ongoing task, 3 completed task
	uint32_t	add_tm;   // time to add to accumulated-time
	uint32_t	start_tm;
	uint32_t	end_tm;
	uint32_t	max_accu_tm;
	uint32_t	max_accu_tm_flag; // flag to set if accumulated-time exceeds max_accu_tm
} __attribute__((packed)) task_elem_upd_t;

typedef struct MagicTaskElemUpd {
	uint32_t	petid;
	uint32_t	taskid;
	uint32_t	flag;     // 1 pending task, 2 ongoing task, 3 completed task
	uint32_t	add_tm;   // time to add to accumulated-time
	uint32_t	start_tm;
	uint32_t	end_tm;
	uint32_t	max_accu_tm;
	uint32_t	max_accu_tm_flag; // flag to set if accumulated-time exceeds max_accu_tm
	uint32_t	stage;
} __attribute__((packed)) magic_task_elem_upd_t;

typedef struct TaskListQuery {
	uint32_t	petid;
	uint32_t	start_taskid;
	uint32_t	end_taskid;
	uint32_t	flag;
} __attribute__((packed)) task_list_query_t;

typedef struct TaskElem {
	uint32_t	petid;
	uint32_t	taskid;
	uint32_t	flag;     // 1 pending task, 2 ongoing task, 3 completed task
	uint32_t	accu_tm;  // accumulate time
	uint32_t	start_tm;
	uint32_t	end_tm;
} __attribute__((packed)) task_elem_t;

typedef struct TaskList {
	uint32_t		cnt;
	task_elem_t		elem[];
} __attribute__((packed)) task_list_t;

typedef struct {
	uint32_t	ID;
} __attribute__((packed)) lahm_task_t;

//---------------- DB-related funcs/macros ------------------
#define db_set_task2(p_, uid_, task_) \
		send_request_to_db(SVR_PROTO_SET_TASK2, (p_), sizeof(task_), &(task_), (uid_))
#define db_set_magic_task(p_, uid_, task_) \
		send_request_to_db(SVR_PROTO_SET_MAGIC_TASK, (p_), sizeof(task_), &(task_), (uid_))
#define db_fin_magic_task(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_FIN_MAGIC_TASK, (p_), 12, buf_, (uid_))
#define db_cancel_magic_task(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_CANCEL_MAGIC_TASK, (p_), 8, buf_, (uid_))
#define db_set_magic_task_data(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_SET_MAGIC_TASK_DATA, (p_), 12, buf_, (uid_))
#define db_get_magic_task_data(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_GET_MAGIC_TASK_DATA, (p_), 8, buf_, (uid_))
#define db_get_magic_task(p_, buf_) \
		send_request_to_db(SVR_PROTO_GET_MAGIC_TASK_LIST, (p_), 12, buf_, (p_)->id)

#define db_get_task2_list(p_, query_) \
		send_request_to_db(SVR_PROTO_GET_TASK2_LIST, (p_), sizeof(query_), &(query_), (p_)->id)

#define db_set_task_info(p_, buf_) \
		send_request_to_db(SVR_PROTO_SET_TASK_INFO, (p_), 54, buf_, (p_)->id)
#define db_get_task_info(p_, buf_) \
		send_request_to_db(SVR_PROTO_GET_TASK_INFO, (p_), 4, buf_, (p_)->id)
#define db_del_task_info(p_, buf_) \
		send_request_to_db(SVR_PROTO_DEL_TASK_INFO, (p_), 4, buf_, (p_)->id)

#define db_set_profession(p_, buf_, uid) \
		send_request_to_db(SVR_PROTO_SET_PROFESSION, (p_), 8, buf_, uid)
#define db_get_profession(p_, uid) \
		send_request_to_db(SVR_PROTO_GET_PROFESSION, (p_), 0, NULL, uid)

//--------------------------------------------------------
static inline int have_profession(sprite_t* p)
{
	if (test_bit_on(p->tasks, sizeof p->tasks, 2)
		|| test_bit_on(p->tasks, sizeof p->tasks, 2 * 8 + 2)
		|| test_bit_on(p->tasks, sizeof p->tasks, 2 * 17 + 2)
		|| test_bit_on(p->tasks, sizeof p->tasks, 2 * 65 + 2)
		|| test_bit_on(p->tasks, sizeof p->tasks, 2 * 131 + 2))
			return 1;

	return 0;
}

static inline int check_recv_task(sprite_t *p, uint32_t taskid)
{
	uint32_t recv = test_bit_on(p->tasks, sizeof(p->tasks), 2 * taskid + 1)
		           + test_bit_on(p->tasks, sizeof(p->tasks), 2 * taskid + 2);
	return recv;
}

static inline int check_fini_task(sprite_t *p, uint32_t taskid)
{
	return test_bit_on(p->tasks, sizeof(p->tasks), 2 * taskid + 2);
}

static inline int check_recv_not_fini_task(sprite_t *p, uint32_t taskid)
{
	return test_bit_on(p->tasks, sizeof(p->tasks), 2 * taskid + 1);
}

//----------------- callbacks for dealing with DB return -----------------
//
int get_task2_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
//
int set_task2_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_task_tmp_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
//----------------------------------------------------------------

//-------------- For proto.h -----------------------
int get_task2_list_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_task_tmp_info_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int set_task_tmp_info_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_task_info_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int set_task_info_cmd(sprite_t* p, uint8_t* body, int bodylen);
int del_task_info_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int task_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int del_task_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_profession_by_taskid(sprite_t* p, uint32_t taskid, uint32_t uid);
int get_profession_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_profession_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_smc_salary_cmd(sprite_t* p, uint8_t* body, int bodylen);
int chk_smc_salary_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_book_info_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int set_book_info_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_book_list_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_book_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_task_list_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_task_list_info_callback(sprite_t* p, uint32_t id, char* buf, int len);


task_t* get_task(uint32_t taskid);
lahm_task_t *get_lahm_task(uint32_t taskid);
int load_lahm_task(xmlNodePtr cur_node);
int load_task_config(xmlNodePtr cur_node);

#endif // ONLINE_TASKS_H_
