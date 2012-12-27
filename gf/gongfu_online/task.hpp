/**
 *============================================================
 *  @file      task.hpp
 *  @brief    all about player's tasks
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_TASK_HPP_
#define KF_TASK_HPP_

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>

#include "utils.hpp"

#include "dbproxy.hpp"
#include "player.hpp"

//------------------------------------------------------------------
// enums
//------------------------------------------------------------------

enum tsk_step_buf_len {
	serv_buf_len = 20,
	client_buf_len = 40,
};

enum task_type {
		primary_task = 1,
		minor_task = 2,
		daily_task = 3,
		active_task = 4,
		master_ring_task = 5,
		slave_ring_task = 6,
		treasure_task = 7,
		prehistoric_task = 8,
		max_type,
};

enum task_buf_type {
  server_buf_type = 1,
  client_buf_type = 2
};

enum max_def_t {
	max_task_id = 9000,
	max_flag_id  = 3,
	max_task_out = 5,
	max_daily_cnt = 100,
	
	min_nor_daily_task_id = 10000,
	max_nor_daily_task_id = 11999,

	min_nor_promotions_task_id = 1267,
	max_nor_promotions_task_id = 1282, 		

	max_carline_daily_task_cnt = 100,
	//max_task_out = 10,
	max_task_in = 1,
	max_task_step = 10,
	max_task_item_num = 10,
	bonus_task_base_id = 5000, /* base_id = first_id - 1 */
	max_bonus_task_num = 400,
	max_primary_tasks = 1,
	max_minor_tasks = 20,
	max_daily_tasks = 1,
};

enum finish_task_step_def_t {
	finish_task_init = 0,
	finish_task_added_monster = 1,
	finish_task_added_items = 2,
	finish_task_set_task_flag = 3,
};

enum{           
    give_type_xiaomee = 1, 
    give_type_exp = 2,
    give_type_skillpoint = 3,
    give_type_fumo_point = 4,
	give_type_expliot	= 5,
	give_type_double_tm = 6,
	give_type_honor 	= 7,
	give_type_apothecary_exp  = 8,
	give_type_summon_exp = 9,
	give_type_sword_value = 10,
	give_type_vip_qualify = 11,

};
enum give_type_t {
	give_type_clothes 	  = 1,
	give_type_normal_item = 2,
	give_type_player_attr = 3,
	give_type_skill 	= 4,

	give_type_max,
};

enum firm_bonus_id_t {
	mon_king_bonus_id = 5003,
};

enum moitor_opt_type {
    opt_xiaomee = 11,
    opt_item    = 12,
};
//------------------------------------------------------------------
// structures
//------------------------------------------------------------------

#pragma pack(1)

struct task_flag_t {
	/*! task id */
	uint32_t task_id;
	/*! flag for one task */
	uint32_t flag;
};

struct clothes_info_t {
	uint32_t	clothes_id;
	uint32_t	unique_id;
};
#pragma pack()

struct task_item_t {
	/*! type of giving of one <Item> */
	uint16_t give_type;
	/*! id of item for one <Item> */
	uint32_t give_id;
	/*! count of item for one <Item> */
	uint32_t cnt;
	/*! item for some role_type for one <Item> */
	uint16_t role_type;
	/*! the item get from monster */
	uint32_t monster_id;
	/*! the odds of droping the item */
	uint32_t drop_odds;
	/*! the item get from which stage */
	uint16_t stage;
	/*! the item get from which difficulty stage */
	uint16_t  stage_difficulty;
};

struct task_in_t {
	/*! in_id for one <InItem> */
	uint16_t id;
	/*! items count for one <InItem> */
	uint16_t items_cnt;
	/*! items info for one <OutItem> */
	struct task_item_t items[max_task_item_num];
};

struct task_out_t {
	/*! out_id for one <OutItem> */
	uint16_t id;
	/*! items(exclude monster) count for one <OutItem> */
	uint16_t items_cnt;
	/*! all items(include monster) count for one <OutItem> */
	//uint16_t all_items_cnt;
	/*! items info for one <OutItem> */
	struct task_item_t items[max_task_item_num];
};


struct task_step_t {
	//1 for mon kill, 2 for stage_win, 3 for other logic check
	int step_type;
	/*! get_id for one <TaskStep> */
	int get_id;
	/*! completeness for one <TaskStep> */
	int completeness;

	int stage;

	uint32_t diffcult;
};

struct task_t {
	/*! is_limit_xiaomee_day */
	uint8_t limit;
	/*! task_id for one <Task> */
	uint32_t id;
	/*! task type for one <Task> */
	uint8_t type;
	/*! vip user only */
	uint8_t vip_limit;
	/*! is outmoded: 0 no,1 yes*/
	uint8_t is_outmoded;
	/*! user's lv needed */
	uint16_t need_lv[2];
	/*! user's role needed */
	uint16_t need_role;
	/*! user's stage finished needed */
	uint16_t need_stage[3];
	/*! user's tsk finished needed */
	uint32_t need_task[3];
	/*! nono func bit needed for task */
	/*uint32_t nono_func;*/
	/*! count of <TaskStep> of one <Task> */
	uint16_t step_cnt;
	/*! <TaskStep> list of one <Task> */
	struct task_step_t step[max_task_step];
	/*! count of <InItem> of one <Task> */
	uint16_t in_cnt;
	/*! <InItem> list of one <Task> */
	struct task_in_t in[max_task_in];
	/*! count of <OutItem> of one <Task> */
	uint16_t out_cnt;
	/*! <OutItem> list of one <Task> */
	struct task_out_t out[max_task_out];
	uint32_t  master_task_id;
	uint32_t  other_limit_id;
};

struct fin_tsk_session_t {
	/*! is_limit_xiaomee_day */
	uint8_t limit;
	/*! task id for one p->session */
	uint32_t tsk_id;
	/*! task lv for one p->session */
	uint16_t tsk_lv;
	/*! number of task_out_items */
	uint32_t	out_skills_num;
	/*! number of task_out_clothes */
	uint32_t	out_clothes_num;
	/*! recored the unique id of clothes */
	clothes_info_t clothes[max_task_item_num];
	/*! in pointer for one p->session */
	const struct task_in_t *in;
	/*! out pointer for one p->session */
	struct task_out_t out;
};

struct lv_daily_task_t {
	uint32_t lv;
	uint32_t times;
	uint32_t cnt;
	task_t* p_task_info[max_daily_cnt];
	
	lv_daily_task_t() {
		cnt = 0;
		lv = 0;
	}
	~lv_daily_task_t() {
		for (uint32_t i = 0; i < cnt; ++i) {
			delete p_task_info[i];
		}
	}	
};

struct daily_task_t {
	uint32_t lv;
	uint16_t level[2];
	uint32_t times;
	uint32_t cnt;
	uint32_t lucky_cnt;
	task_t   task_info[max_daily_cnt];
	task_t	 lucky_task[max_daily_cnt];
};

typedef std::map<uint32_t, lv_daily_task_t*> DailyTaskMap;
typedef std::map<uint32_t, task_t*> TaskMap;


#pragma pack(1)

struct task_going_t {
	/*! task_id for one <Task> */
	uint32_t id;
	/*! buf for server using */
	char serv_buf[serv_buf_len];
	/*! buf for client using */
	char client_buf[client_buf_len];
};

struct player_task_item_t {
	uint32_t	item_id;
	uint32_t	monster_id;
	uint32_t	drop_odds;
	uint32_t	rest_cnt;
};

struct player_task_monster_t {
	uint32_t	task_id;
	uint16_t	step_cnt;
	task_step_t step[max_task_step];
	uint8_t		serv_buf[serv_buf_len];
	uint8_t		change_flag;
};


#pragma pack()

inline int is_valid_mon_level(uint32_t id)
{
	return ((id >= 1 && id <= 100) ? 1 : 0);
}

/**
 * @brief check the task if it's finished or not
 * @param p the player
 * @param tsk_id task id
 * @return true when task(tsk_id) is finished, false otherwise
 */
inline bool is_finished_task(const player_t* p, uint32_t tsk_id)
{
	/*int pos = (tsk_id - 1) * 2 + 1;
	return (taomee::test_bit_on(p->tasklist, pos) && taomee::test_bit_on(p->tasklist, pos + 1));*/
	return (p->finished_tasks_set->count(tsk_id) > 0);
}

inline bool is_treasure_task(uint32_t task_id) 
{
	if (task_id >= 3000 && task_id <= 3003) {
		return true;
	}
	return false;
}

/**
 * @brief check the task if it's taked or not
 * @param p the player
 * @param tsk_id task id
 * @return true when task(tsk_id) is finished, false otherwise
 */
inline bool is_going_task(const player_t* p, uint32_t tsk_id)
{
	return (p->going_tasks_map->count(tsk_id) > 0);
}

//------------------------------------------------------------------
// help functions
//------------------------------------------------------------------

/**
 * @brief test id is wheather a valid in_id or not
 * @return 0: invalid, 1: valid
 */
int is_valid_inid(uint32_t id);

/**
 * @brief test id is wheather a valid out_id or not
 * @return pointer to the task or 0 for invalid id
 */
int is_valid_outid(uint32_t id);

/**
 * @brief redefine test id is wheather a valid in_id or not
 * @return 0: invalid, 1: valid
 */
int is_valid_inid(const task_t* tsk, uint32_t in_id);

/**
 * @brief redefine test id is wheather a valid out_id or not
 * @return 0: invalid, 1: valid
 */
int is_valid_outid(const task_t* tsk, uint32_t out_id);

/**
 * @brief get a task pointer
 * @return pointer to the task or 0 for invalid id
 */
const struct task_t* get_task(uint32_t id);

/**
 * @brief get a task's type
 * @return pointer to the task or 0 for invalid id
 */
int get_task_type(uint32_t taskid);

/**
 * @brief add a daily task to player
 * @return pointer to the task or 0 for invalid id
 */
task_going_t* add_daily_task(player_t *p, uint32_t id);


/**
 * @brief get a bonus task pointer
 * @return pointer to the bonus task or 0 for invalid bonus_id
 */
const struct task_t* get_bonus_task(uint32_t id);

/**
 * @brief get a task->out[out_id - 1] pointer
 * @return pointer to the task->out[out_id - 1] or 0 for invalid out_id
 */
const struct task_out_t* get_task_out(const struct task_t* tsk, uint32_t out_id);

/**
 * @brief judge a player meet the task need
 * @return 0: not, 1: yes
 */
int judge_task_need(player_t* p, const struct task_t* tsk);

/**
 * @brief Log the config reading from conf/task.xml
 * @return NULL
 */
void log_task_config();

/**
 * @brief one of finish a task step: response the finish_task result to client
 * @param p the player
 * @return NULL
 */
void trace_log_finish_task_response_to_client(player_t* p);

/**
 * @brief load <Step> level's config of tsk's info from conf/tasks.xml file
 * @param cur the xml_pointer to the node which pointing to "<Task>" level
 * @param tsk the pointer to current task 
 * @param out the pointer to current initem
 * @return 0 on success, -1 on error
 */
int load_one_task_step(xmlNodePtr cur, struct task_t* tsk);

/**
 * @brief load <InItem> level's config of tsk's info from conf/tasks.xml file
 * @param cur the xml_pointer to the node which pointing to "<Task>" level
 * @param tsk the pointer to current task 
 * @param out the pointer to current initem
 * @return 0 on success, -1 on error
 */
int load_one_task_in_items(xmlNodePtr cur, struct task_t* tsk, struct task_in_t* in);

/**
 * @brief load <OutItem> level's config of tsk's info from conf/tasks.xml file
 * @param cur the xml_pointer to the node which pointing to "<Task>" level
 * @param tsk the pointer to current task 
 * @param out the pointer to current outitem
 * @return 0 on success, -1 on error
 */
int load_one_task_out_items(xmlNodePtr cur, task_t *tsk, struct task_out_t* out);

/**
 * @brief load only one task's info from conf/task.xml
 * @param cur the xml_pointer to the node which pointing to "<Task>" level
 * @param tsk the current task
 * @return 0 on success, -1 on error
 */
int load_one_task(xmlNodePtr cur, struct task_t* tsk);

/**
 * @brief load tasks' info from conf/tasks.xml file
 * @param cur pointer to root(<Tasks>) of conf/tasks.xml
 * @return 0 on success, -1 on error
 */
int load_tasks_config(xmlNodePtr cur);

/**
 * @brief load daily tasks' info from conf/tasks.xml file
 * @param p the player
 * @return 0 on success, -1 on error
 */
int load_daily_tasks_config(xmlNodePtr cur);

/**
 * @brief pack the player's task info into battle
 * @param p the player
 * @param pkgbuf
 * @return the idx
 */
int pack_task_to_btl(player_t* p, void* pkgbuf, uint32_t stage, uint32_t difficulty);

/**
 * @brief unpack the player's task info from battle
 * @param p the player
 * @param count the task number
 * @param body the info of task
 */
void unpack_task_buf_info(player_t* p, uint32_t count, uint8_t* body);

/**
 * @brief pack the player's task info
 * @param p the player
 * @param pkgbuf
 * @return the idx
 */
int pack_player_task_info(player_t* p, void* pkgbuf);

/**
 * @brief finish a task step selector
 * @param p the player
 * @param step the Xth step on a finish_task way
 * @return 0 on success, -1 on error
 */
int finish_task_step_over(player_t* p, uint32_t step, uint32_t task_id = 0);

/**
 * @brief finish a task
 * @param p the player
 * @param tsk_id task id
 * @param out_id request out item's id
 * @return 0 on success, -1 on error
 */
int do_finish_task(player_t* p, uint32_t tsk_id, uint32_t out_id, const task_t* tsk);

/**
 * @brief Add one monster to client
 * @param p the player
 * @return 0 on success, -1 on error
 */
int add_one_monster(player_t* p);

/**
 * @brief one of finish a task step: Set current task's status to finished.
 * @param p the player
 * @return 0 on success, -1 on error
 */
int set_finish_task_flag(player_t* p);

/**
 * @brief check if the task be finished
 * @param p the player
 * @param task_id the id of the task to check
 * @return 0 on success, -1 on error
 */
int check_task_finish(player_t* p, const task_t* tsk);

/**
 * @brief check the task buf
 * @param p_going_task the point of going task
 * @param p_going_task the point of tasks
 * @return 0 on unfinished, 1 on finished
 */
int check_task_buf(const task_going_t* p_going_task, const task_t* p_task);
#if 0	
/**
 * @brief to set the completeness if the monster you killed or the item you get match the step of your going tasks.
 * @param p the player
 * @param get_type "1" means item and "2" means monster
 * @param the id of the monster you kill or the item you get
 * @return 0 on success, -1 on error
 */
int set_step_completeness(player_t* p, uint32_t get_type, uint32_t get_id);
#endif

void tmp_log_for_task_bug(player_t* p, uint32_t flag);

//------------------------------------------------------------------
// response to client functions
//------------------------------------------------------------------

/**
 * @brief Send response to client of getting buf for a task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_task_buf_response_to_client(player_t* p, void* body, uint32_t bodylen);

/**
 * @brief one of finish a task step: response the finish_task result to client
 * @param p the player
 * @return 0 on success, -1 on error
 */
int finish_task_response_to_client(player_t* p);

//------------------------------------------------------------------
// request from client (XXX_cmd)
//------------------------------------------------------------------

/**
 * @brief Get the buf of a task which requested by id
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_task_buf_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief set buf for a task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int set_task_buf_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief undertake a task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int undertake_task_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief finish a task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int finish_task_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief cancel a task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int cancel_task_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief undertake a daily task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int undertake_daily_task_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief cancel a daily task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int cancel_daily_task_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief double experience a daily task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int double_exp_action_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief set role double experience time when use item
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int double_exp_action_use_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief get list of boss killed
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_killed_boss_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int check_task_extra_logic_cmd(player_t *p, uint8_t * body, uint32_t bodylen);
//------------------------------------------------------------------
// request to dbproxy (db_XXX)
//------------------------------------------------------------------

/**
 * @brief Send request of setting buf for a task to DB
 * @param p the player
 * @param tsk_id the id of current task
 * @param tobe the start of the buf to set
 * @param tobe_len the length of the buf to set
 * @return 0 on success, -1 on error
 */
int db_set_task_buf(player_t* p, uint32_t tsk_id, char* tobe, int tobe_len, uint32_t buf_type, bool callback = true);

/**
 * @brief Request all flags of request player's task in a range
 * @param p the player
 * @param start the start_task_id of the range of request
 * @param end the end_task_id of the range of request
 * @return 0 on success, -1 on error
 */
int db_get_task_flag_list(player_t* p, uint32_t start, uint32_t end);

/**
 * @brief Send request of getting buf for going tasks to DB
 * @param p the player
 * @return 0 on success, -1 on error
 */
int db_get_going_task_buf(player_t* p);

/**
 * @brief Send request of getting list for done tasks to DB
 * @param p the player
 * @return 0 on success, -1 on error
 */
int db_get_done_task_list(player_t* p);

/**
 * @brief Send request of getting list for all tasks to DB
 * @param p the player
 * @return 0 on success, -1 on error
 */
int db_get_all_task_list(player_t* p);

/**
 * @brief Do the real work of function "finish_task_step_add_items(...)"
 * @param p the player
 * @return 0 on success, -1 on error
 */
int db_add_items(player_t* p, uint32_t task_id = 0);

/**
  * @brief set player's task's flag from db
  * @param p the requester
  * @param uid id of the requestee
  * @param taskid id of current task
  * @param flag the flag to be set to taskid
  * @return 0 on success, -1 on error
  */
enum back_flag_t {
	set_flag_without_back = 0,
	set_flag_with_back = 1,
};
inline int db_set_task_flag(player_t* p, userid_t uid, uint32_t taskid, uint32_t flag, back_flag_t s_flag=set_flag_with_back)
{
	int idx = 0;
	taomee::pack_h(dbpkgbuf, taskid, idx);
	taomee::pack_h(dbpkgbuf, flag, idx);

	KDEBUG_LOG(p->id, "SET TASK\t[%u %u]", taskid, flag);
	return send_request_to_db( s_flag ? p:NULL, uid, p->role_tm, dbproto_set_task_flag, dbpkgbuf, idx);
}

/**
  * @brief get list of boss killed from db
  * @param p the requester
  * @param uid id of the requestee
  * @param taskid id of current task
  * @param flag the flag to be set to taskid
  * @return 0 on success, -1 on error
  */
inline int db_get_killed_boss(player_t* p, userid_t uid)
{
	return 0;// send_request_to_db(p, uid, p->role_tm, dbproto_get_killed_boss, dbpkgbuf, 0);
}

//------------------------------------------------------------------
// callback for handling pkg return from dbproxy (db_XXX_callback)
//------------------------------------------------------------------

/**
  * @brief callback for setting a player's task list
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_set_task_flag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

 /**
  * @brief callback for get a player's task buf of taskid
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
//int db_get_task_buf_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

 /**
  * @brief callback for set a player's task buf of taskid
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_set_task_buf_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for get a player's task flag list
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_task_flag_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for get a player's task flag list
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_going_task_buf_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for get a player's done tasks list
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_done_task_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
   * @brief callback for get a player's all tasks list
   * @param p the requester
   * @param uid id of the requestee
   * @param body body of the returning package
   * @param bodylen length of body
   * @param ret errno returned from dbproxy
   * @return 0 on success, -1 and p will be deleted
   */
 int db_get_all_task_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
 

/**
  * @brief callback for set a player's boss list killed
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_killed_boss_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

void update_task_step_info_by_kill_monster(player_t * p, uint32_t killed_monster);
void update_task_step_info_by_pass_stage(player_t *p, uint32_t stage_id);

int del_clear_done_task(player_t* p);
int del_clear_going_task(player_t* p);

#endif // KF_TASK_HPP_

