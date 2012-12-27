#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/inet/pdumanip.hpp>
#include <kf/player_attr.hpp>

using namespace taomee;

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/project/stat_agent/msglog.h>
}
#include <map>
#include <set>

#include "item.hpp"
//#include "task.hpp"
#include "login.hpp"
#include "cli_proto.hpp"
#include "vip_impl.hpp"
#include "restriction.hpp"
#include "stat_log.hpp"
#include "global_data.hpp"
#include "common_op.hpp"
#include "ring_task.hpp"
#include "task_logic.hpp"
//------------------------------------------------------------------
// global vars
//------------------------------------------------------------------

//------------------------------------------------------------------
// inline helper functions
//------------------------------------------------------------------


inline int is_valid_taskid(uint32_t id)
{
	return ((((id >= 1) && (id <= max_task_id) && (all_tasks[id - 1].id == id)) || 
			(id >= min_nor_daily_task_id && id <= max_nor_daily_task_id )) 
			? 1 : 0);
}

inline int is_valid_bonus_taskid(uint32_t id)
{
	return ((id > bonus_task_base_id && id <= bonus_task_base_id + max_bonus_task_num) ? 1 : 0);
}

inline int is_valid_flag(uint32_t flag)
{
	return ((flag <= max_flag_id) ? 1 : 0);
}

inline int is_valid_itemid(uint32_t id)
{
	return ((id <= max_task_item_num && id >= 1) ? 1 : 0);
}

inline int is_valid_give_type(uint32_t id)
{
	return ((id < give_type_max && id >= 1) ? 1 : 0);
}

inline int is_valid_give_id(uint32_t id, uint32_t type)
{
	const GfItem* itm;
	switch (type) {
	case 1:
	case 2:
		itm = items->get_item(id);
		return (itm ? 1 : 0);
	case 3:
		return (id > 0 && id < give_type_max ? 1 : 0);
	default:
		return 0;
	}
	return 0;
}

inline uint32_t get_player_max_daily_cnt(player_t* p)
{
	return 0;
	//return all_daily_tasks[p->lv].times;****daily task down******
}

inline uint32_t get_going_daily_task_id(player_t* p)
{
	return 0;
}

inline const struct task_t* get_lucky_daily_task(player_t* p)
{
	return 0;
}

inline uint32_t get_going_daily_task_count(player_t* p)
{
	return 0;
}

inline uint32_t get_finished_daily_task_count(player_t* p)
{
	return 0;
}

inline uint32_t get_canceled_daily_task_count(player_t* p)
{
	return p->canceled_tasks_set->size();
}

inline uint32_t get_player_daily_task_count(player_t* p)
{
	return 0;
	//return get_finished_daily_task_count(p) + get_canceled_daily_task_count(p);****daily task down******
}

inline bool is_have_daily_task(player_t* p)
{
	return false;
}

inline bool is_canceled_daily_task(player_t* p, uint32_t task_id)
{
	return false;
	//return (p->canceled_tasks_set->count(task_id) > 0);****daily task down******
}

inline bool can_undertake_daily_task(player_t* p, task_t* p_task)
{
	return false;
}


/**
 * @brief redefine test id is wheather a valid out_id or not
 * @return 0: invalid, 1: valid
 */
inline bool is_daily_taskid(uint32_t task_id)
{
	return false;
	/*****daily task down******
	if (task_id < min_nor_daily_task_id|| task_id > max_nor_daily_task_id) {
		return false;
	}
	return true;
	*/
}

inline bool is_promotions_task(uint32_t task_id)
{
	if(task_id < min_nor_promotions_task_id || task_id > max_nor_promotions_task_id)
	{
		return false;
	}
	return true;
}

inline int get_task_type(uint32_t taskid)
{
	if (is_daily_taskid(taskid)) {
		return daily_task;
	} else {
		return all_tasks[taskid-1].type;
	}
}

inline bool is_too_many_going_task(player_t* p)
{
	if ( p->going_tasks_map->size() >= (max_primary_tasks + max_minor_tasks + max_daily_tasks) ) {
		TRACE_LOG("the number of going tasks error!\t[uid=%u, role_tm=%u, tasks_num=%lu]",
								p->id, p->role_tm, p->going_tasks_map->size());
		return true;
	}
	return false;
}


inline void do_stat_log_after_task_added_items(player_t* p)
{
	fin_tsk_session_t* fin_sess = reinterpret_cast<fin_tsk_session_t *>(p->session);
	task_out_t* p_out = &(fin_sess->out);
	for (uint32_t i = 0; i < p_out->items_cnt; ++i) {
		if (p_out->items[i].give_type == give_type_normal_item
            && p_out->items[i].role_type == p->role_type) {
			do_stat_log_amber(stat_log_got_amber, p_out->items[i].give_id, p->role_type);
		}
	}
}

inline void do_stat_log_task_info(uint32_t id, uint32_t type, uint32_t index, uint32_t cnt)
{
    uint32_t cmd = 0;
    uint32_t buf[3] = { 0};
    buf[index] = cnt;
    if (id > 1000) {
        cmd = stat_log_minor_task;
        id -= 1000;
    } else {
        cmd = stat_log_major_task;
    }
    uint32_t itype = ((type-1) << 12);
	msglog(statistic_logfile, cmd + id + itype, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [taskid=%u] [type=%u %x] buf[%u, %u %u]",
        cmd, id, type, itype, buf[0], buf[1], buf[2]);
}

/**
 * @brief statistics taskid=1269/1270 1296/1297 1311/1312  
 */
inline void do_stat_log_stage_trials(uint32_t taskid, uint32_t uid)
{
    uint32_t buf_1[1] = {0};
    uint32_t buf_2[2] = {0};
    uint8_t flag = 1;
    uint32_t cmd = 0;
    switch (taskid) {
        case 1270:
            cmd = 0x09503001;
            flag = 2;
            break;
        case 1311:
            cmd = 0x09503102;
            break;
        case 1312:
            cmd = 0x09503101;
            break;
        default:
            return;
    }
    if (flag == 1) {
        buf_1[0] = uid;
        msglog(statistic_logfile, cmd, get_now_tv()->tv_sec, buf_1, sizeof(buf_1));
        TRACE_LOG("stat log: [cmd=%x] buf[%u]",cmd, buf_1[0]);
    } else {
        buf_2[0] = uid;
        buf_2[1] = 1;
        msglog(statistic_logfile, cmd, get_now_tv()->tv_sec, buf_2, sizeof(buf_2));
        TRACE_LOG("stat log: [cmd=%x] buf[%u %u]",cmd, buf_2[0], buf_2[1]);
    }
}
//------------------------------------------------------------------
// helper functions 
//------------------------------------------------------------------

/**
 * @brief test id is wheather a valid in_id or not
 * @return 0: invalid, 1: valid
 */
int is_valid_inid(uint32_t id)
{
	return ((id >= 1 && id <= max_task_in) ? 1 : 0);
}

/**
 * @brief test id is wheather a valid out_id or not
 * @return 0: invalid, 1: valid
 */
int is_valid_outid(uint32_t id)
{
	return ((id >= 1 && id <= max_task_out) ? 1 : 0);
}

/**
 * @brief redefine test id is wheather a valid in_id or not
 * @return 0: invalid, 1: valid
 */
int is_valid_inid(const task_t* tsk, uint32_t in_id)
{
	if (in_id < 1 || in_id > max_task_in) {
		return 0;
	}
	return ((tsk && tsk->in[in_id - 1].id == in_id) ? 1 : 0);
}

/**
 * @brief redefine test id is wheather a valid out_id or not
 * @return 0: invalid, 1: valid
 */
int is_valid_outid(const task_t* tsk, uint32_t out_id)
{
	if (out_id < 1 || out_id > max_task_out) {
		return 0;
	}
	return ((tsk && tsk->out[out_id - 1].id == out_id) ? 1 : 0);
}

/**
 * @brief get a task pointer NOTE: include daily task
 * @return pointer to the task or 0 for invalid id
 */
inline const struct task_t* get_daily_task(uint32_t task_id)
{
	
	std::map<uint32_t, task_t>::iterator it = daily_task_map.find(task_id);
	if (it != daily_task_map.end()) {
		return &(it->second);
	}
	return 0;
}

/**
 * @brief get a task pointer NOTE: include daily task
 * @return pointer to the task or 0 for invalid id
 */
const struct task_t* get_task(uint32_t id)
{
	if (is_daily_taskid(id)) {
		return get_daily_task(id);
	} else if ((id >= 1 && id <= max_task_id)
			&& (all_tasks[id - 1].id == id)) {
		return &(all_tasks[id - 1]);
	}

	return 0;
}

inline void add_going_task(player_t *p, uint32_t taskid)
{
	task_going_t task;
	memset(&task, 0, sizeof(task));
	task.id = taskid;
	p->going_tasks_map->insert(std::map<uint32_t, task_going_t>::value_type(taskid, task));
}

/**
 * @brief get a task id NOTE: include daily task
 * @return pointer to the task or 0 for invalid id
 */
inline const struct task_t* get_free_daily_task(player_t* p)
{
	return 0;
	/*
	daily_task_t* p_daily_task = &(all_daily_tasks[p->lv]);
	if (p_daily_task->cnt > 0) {
		uint32_t rand_num = rand() % (p_daily_task->cnt + 1);
		TRACE_LOG("RAND NUM %u", rand_num);
		if (p_daily_task->task_info[rand_num].id &&
			can_undertake_daily_task(p, &(p_daily_task->task_info[rand_num])) &&
			(!p_daily_task->task_info[rand_num].vip_limit || is_vip_player(p)) &&
			judge_task_need(p, &(p_daily_task->task_info[rand_num]))) {
			
			return &(p_daily_task->task_info[rand_num]);
		}
		TRACE_LOG("%u %u %u %u %u", p_daily_task->cnt, rand_num, p_daily_task->task_info[rand_num].id, p_daily_task->task_info[rand_num].need_lv[0], p_daily_task->task_info[rand_num].need_lv[1]);
		for (uint32_t i = (rand_num + 1) % (p_daily_task->cnt + 1); i != rand_num; i = (i + 1) % (p_daily_task->cnt + 1)) {
			if (p_daily_task->task_info[i].id &&
				can_undertake_daily_task(p, &(p_daily_task->task_info[i])) &&
				(!p_daily_task->task_info[i].vip_limit || is_vip_player(p)) &&
				judge_task_need(p, &(p_daily_task->task_info[i]))) {
				
				return &(p_daily_task->task_info[i]);
			}
			TRACE_LOG("%u %u %u %u", i, p_daily_task->task_info[i].id, p_daily_task->task_info[i].need_lv[0], p_daily_task->task_info[i].need_lv[1]);
		}
	}
	return 0;*/
}


/**
 * @brief get a task id NOTE: include daily task
 * @return pointer to the task or 0 for invalid id
 */
inline const struct task_t* get_free_carline_daily_task(player_t* p)
{
	daily_task_t* p_daily_task = &(all_daily_tasks[(p->lv + 4) / 5]);
	if (p_daily_task->cnt > 0) {
		uint32_t rand_num = rand() % (p_daily_task->cnt + 1);
		TRACE_LOG("RAND NUM %u", rand_num);
		if (p_daily_task->task_info[rand_num].id &&
			can_undertake_daily_task(p, &(p_daily_task->task_info[rand_num])) &&
			(!p_daily_task->task_info[rand_num].vip_limit || is_vip_player(p)) &&
			judge_task_need(p, &(p_daily_task->task_info[rand_num]))) {
			
			return &(p_daily_task->task_info[rand_num]);
		}
		TRACE_LOG("%u %u %u %u %u", p_daily_task->cnt, rand_num, p_daily_task->task_info[rand_num].id, p_daily_task->task_info[rand_num].need_lv[0], p_daily_task->task_info[rand_num].need_lv[1]);
		for (uint32_t i = (rand_num + 1) % (p_daily_task->cnt + 1); i != rand_num; i = (i + 1) % (p_daily_task->cnt + 1)) {
			if (p_daily_task->task_info[i].id &&
				can_undertake_daily_task(p, &(p_daily_task->task_info[i])) &&
				(!p_daily_task->task_info[i].vip_limit || is_vip_player(p)) &&
				judge_task_need(p, &(p_daily_task->task_info[i]))) {
				
				return &(p_daily_task->task_info[i]);
			}
			TRACE_LOG("%u %u %u %u", i, p_daily_task->task_info[i].id, p_daily_task->task_info[i].need_lv[0], p_daily_task->task_info[i].need_lv[1]);
		}
	}
	return 0;
}

/**
 * @brief get a bonus task pointer
 * @return pointer to the bonus task or 0 for invalid bonus_id
 */
const struct task_t* get_bonus_task(uint32_t id)
{
	if (is_valid_bonus_taskid(id)
		&& (all_bonus_tasks[id - bonus_task_base_id - 1].id == id)) {
		return &(all_bonus_tasks[id - bonus_task_base_id - 1]);
	}
	return 0;
}

/**
 * @brief get a task->out[out_id - 1] pointer
 * @return pointer to the task->out[out_id - 1] or 0 for invalid out_id
 */

const struct task_out_t* get_task_out(const struct task_t* tsk, uint32_t out_id)
{
	if (tsk && is_valid_outid(out_id)
		&& (tsk->out[out_id - 1].id == out_id)) {
		return &((tsk->out[out_id - 1]));
	}

	return 0;
}

int judge_task_need(player_t* p, const struct task_t* tsk)
{
	if ( tsk->need_lv[0] > p->lv || tsk->need_lv[1] < p->lv ) {
		ERROR_LOG("undertake task error:need_lv[%u %u %u %u]", tsk->id, p->lv, tsk->need_lv[0], tsk->need_lv[1]);
		return 0;
	}
	if ( tsk->need_role != 0 && tsk->need_role != p->role_type ) {
		ERROR_LOG("undertake task error:need_role[%u %u %u]",tsk->id, p->role_type, tsk->need_role);
		return 0;
	}
	if ( (tsk->need_task[0] != 0 && !is_finished_task(p, tsk->need_task[0]))
	  	|| (tsk->need_task[1] != 0 && !is_finished_task(p, tsk->need_task[1]))
	  	|| (tsk->need_task[2] != 0 && !is_finished_task(p, tsk->need_task[2])) ) {
		ERROR_LOG("undertake task error:need_task[%u %u %u %u]",tsk->id, tsk->need_task[0], tsk->need_task[1], tsk->need_task[2]);
		return 0;
	}
	if ( (tsk->need_stage[0] != 0 && !is_passed_stage(p, tsk->need_stage[0]))
	  	|| (tsk->need_stage[1] != 0 && !is_passed_stage(p, tsk->need_stage[1]))
	  	|| (tsk->need_stage[2] != 0 && !is_passed_stage(p, tsk->need_stage[2])) ) {
		ERROR_LOG("undertake task error:need_stage[%u %u %u %u]",tsk->id, tsk->need_stage[0], tsk->need_stage[1], tsk->need_stage[2]);
		return 0;
	}

	return 1;
}


/**
 * @brief Log the config reading from conf/task.xml
 * @return NULL
 */
void log_task_config()
{
	struct task_t* tsk = 0;
	
	for (int t = 0; t < max_task_id; t++) {
		tsk = &all_tasks[t];
		if (tsk && tsk->id > 0) {
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->id=%u]", t+1, tsk->id);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->need_lv=%u %u]", t+1, tsk->need_lv[0], tsk->need_lv[1]);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->need_role=%u]", t+1, tsk->need_role);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->step_cnt=%u]", t+1, tsk->step_cnt);
			for (uint32_t i = 0; i < tsk->step_cnt; i++) {
				TRACE_LOG("LOG_TASK_CONF\t[task->step[%d]->get_id=%u]", i+1, tsk->step[i].get_id);
				TRACE_LOG("LOG_TASK_CONF\t[task->step[%d]->completeness=%u]", i+1, tsk->step[i].completeness);
			}
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->in_cnt=%u]", t+1, tsk->in_cnt);
			for (uint32_t i = 0; i < tsk->in_cnt; i++) {
				TRACE_LOG("LOG_TASK_CONF\t[task->in[%d]->id=%u]", i+1, tsk->in[i].id);
				TRACE_LOG("LOG_TASK_CONF\t[task->in[%d]->items_cnt=%u]", i+1, tsk->in[i].items_cnt);
				for (uint32_t j = 0; j < tsk->in[i].items_cnt; j++) {
				TRACE_LOG("LOG_TASK_CONF\t\t[task->in->item[%d]->give_type=%u]",
						j+1, tsk->in[i].items[j].give_type);
				TRACE_LOG("LOG_TASK_CONF\t\t[task->in->item[%d]->give_id=%u]",
						j+1, tsk->in[i].items[j].give_id);
				TRACE_LOG("LOG_TASK_CONF\t\t[task->in->item[%d]->count=%u]",
						j+1, tsk->in[i].items[j].cnt);
				TRACE_LOG("LOG_TASK_CONF\t\t[task->in->item[%d]->monster=%u]",
						j+1, tsk->in[i].items[j].monster_id);
				TRACE_LOG("LOG_TASK_CONF\t\t[task->in->item[%d]->stage=%u]",
						j+1, tsk->in[i].items[j].stage);
				TRACE_LOG("LOG_TASK_CONF\t\t[task->in->item[%d]->difficulty=%u]",
						j+1, tsk->in[i].items[j].stage_difficulty);
				}
			}
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->out_cnt=%u]", t+1, tsk->out_cnt);
			for (uint32_t i = 0; i < tsk->out_cnt; i++) {
				TRACE_LOG("LOG_TASK_CONF\t[task->out[%d]->id=%u]", i+1, tsk->out[i].id);
				//TRACE_LOG("LOG_TASK_CONF\t[task->out[%d]->all_items_cnt=%u]", i+1, tsk->out[i].all_items_cnt);
				TRACE_LOG("LOG_TASK_CONF\t[task->out[%d]->items_cnt=%u]", i+1, tsk->out[i].items_cnt);
				//for (uint32_t j = 0; j < tsk->out[i].all_items_cnt; j++) {
				for (uint32_t j = 0; j < tsk->out[i].items_cnt; j++) {
					TRACE_LOG("LOG_TASK_CONF\t\t[task->out->item[%d]->give_type=%u]",
							j+1, tsk->out[i].items[j].give_type);
					TRACE_LOG("LOG_TASK_CONF\t\t[task->out->item[%d]->give_id=%u]",
							j+1, tsk->out[i].items[j].give_id);
					TRACE_LOG("LOG_TASK_CONF\t\t[task->out->item[%d]->count=%u]",
							j+1, tsk->out[i].items[j].cnt);
				}
			}
			TRACE_LOG("----------------------------------------");
		}
	}

	for (int t = 0; t < max_bonus_task_num; t++) {
		tsk = &all_bonus_tasks[t];
		if (tsk && tsk->id > 0) {
			TRACE_LOG("LOG_BONUS_TASK_CONF\t[bonus_task[%d]->id=%u]", t+1, tsk->id);
			TRACE_LOG("LOG_BONUS_TASK_CONF\t[bonus_task[%d]->out_cnt=%u]", t+1, tsk->out_cnt);
			for (uint32_t i = 0; i < tsk->out_cnt; i++) {
				TRACE_LOG("LOG_BONUS_TASK_CONF\t[bonus_task->out[%d]->id=%u]", i+1, tsk->out[i].id);
				//TRACE_LOG("LOG_BONUS_TASK_CONF\t[bonus_task->out[%d]->all_items_cnt=%u]", i+1, tsk->out[i].all_items_cnt);
				TRACE_LOG("LOG_BONUS_TASK_CONF\t[bonus_task->out[%d]->items_cnt=%u]", i+1, tsk->out[i].items_cnt);
				//for (uint32_t j = 0; j < tsk->out[i].all_items_cnt; j++) {
				for (uint32_t j = 0; j < tsk->out[i].items_cnt; j++) {
					TRACE_LOG("LOG_BONUS_TASK_CONF\t\t[bonus_task->out->item[%d]->give_type=%u]",
							j+1, tsk->out[i].items[j].give_type);
					TRACE_LOG("LOG_BONUS_TASK_CONF\t\t[bonus_task->out->item[%d]->give_id=%u]",
							j+1, tsk->out[i].items[j].give_id);
					TRACE_LOG("LOG_BONUS_TASK_CONF\t\t[bonus_task->out->item[%d]->count=%u]",
							j+1, tsk->out[i].items[j].cnt);
				}
			}
			TRACE_LOG("----------------------------------------");
		}
	}
}

/**
 * @brief load <Step> level's config of tsk's info from conf/tasks.xml file
 * @param cur the xml_pointer to the node which pointing to "<Task>" level
 * @param tsk the pointer to current task 
 * @param out the pointer to current initem
 * @return 0 on success, -1 on error
 */
int load_one_task_step(xmlNodePtr cur, struct task_t* tsk)
{
	uint32_t count = 0;
	
	cur = cur->xmlChildrenNode; /* goto <Step> */
	while (cur) {
		if (count >= max_task_step) {
			ERROR_LOG("load Step failed: invalid count:%u", count);
			return -1;
		}
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Step")) {
			get_xml_prop_def(tsk->step[count].step_type, cur, "step_type", 1);
			get_xml_prop_def(tsk->step[count].get_id, cur, "get_id", 0);
			get_xml_prop_def(tsk->step[count].completeness, cur, "completeness", 1);
			get_xml_prop_def(tsk->step[count].stage, cur, "stage", 0);
			count++;
		}
		cur = cur->next;
	}
	
	tsk->step_cnt = count;
	
	return 0;
}

/**
 * @brief load <InItem> level's config of tsk's info from conf/tasks.xml file
 * @param cur the xml_pointer to the node which pointing to "<Task>" level
 * @param tsk the pointer to current task 
 * @param out the pointer to current initem
 * @return 0 on success, -1 on error
 */
int load_one_task_in_items(xmlNodePtr cur, struct task_t* tsk, struct task_in_t* in)
{
	/* cur is at <InItem> now */
	uint32_t item_idx = 0;
	uint32_t give_type = 0;
	uint32_t give_id = 0;
	uint32_t count = 0;

	uint32_t monster_id = 0;
	uint32_t stage = 0;
	uint32_t stage_difficulty = 0;

	struct task_item_t* item = 0;

	cur = cur->xmlChildrenNode; /* goto <Item> */
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Item")) {
			double odds = 0.0;
			get_xml_prop(give_type, cur, "give_type");
			get_xml_prop(give_id, cur, "give_id");
			get_xml_prop(count, cur, "cnt");
			get_xml_prop_def(monster_id, cur, "monster_id", 0);
			get_xml_prop_def(stage, cur, "stage", 0);
			get_xml_prop_def(odds, cur, "drop_odds", 0);
			get_xml_prop_def(stage_difficulty, cur, "difficulty", 0);

			if (!is_valid_itemid(item_idx + 1)
				|| !is_valid_give_type(give_type)
				|| !is_valid_give_id(give_id, give_type)) {
				ERROR_LOG("load Item failed: invalid item_id:%u, "
						"or invalid give_type:%u, or invalid give_id:%u",
						item_idx + 1, give_type, give_id);
				return -1;
			}

			item = &(in->items[item_idx]);
			in->items_cnt++;

			item->give_type = give_type;
			item->give_id = give_id;
			item->cnt = count;
			item->monster_id = monster_id;
			item->stage = stage;
			item->drop_odds = static_cast<uint32_t>(odds * 10000);
			item->stage_difficulty = stage_difficulty;
			item_idx++;
		}
		cur = cur->next;
	}
	return 0;
}


/**
 * @brief load <OutItem> level's config of tsk's info from conf/tasks.xml file
 * @param cur the xml_pointer to the node which pointing to "<Task>" level
 * @param tsk the pointer to current task 
 * @param out the pointer to current outitem
 * @return 0 on success, -1 on error
 */
int load_one_task_out_items(xmlNodePtr cur, struct task_t* tsk, struct task_out_t* out)
{
	/* cur is at <OutItem> now */
	uint32_t item_idx = 0;
	uint32_t give_type = 0;
	uint32_t give_id = 0;
	uint32_t count = 0;
	uint32_t role_type = 0;
	struct task_item_t* item = 0;

	cur = cur->xmlChildrenNode; /* goto <Item> */
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Item")) {
			get_xml_prop(give_type, cur, "give_type");
			get_xml_prop(give_id, cur, "give_id");
			get_xml_prop(count, cur, "cnt");
			get_xml_prop_def(role_type, cur, "role_type", 0);
			
			/*
			if (!is_valid_itemid(item_idx + 1)
				|| !is_valid_give_type(give_type)
				|| !is_valid_give_id(give_id, give_type)) {
				ERROR_LOG("load Item failed: invalid item_id:%u, "
						"or invalid give_type:%u, or invalid give_id:%u",
						item_idx + 1, give_type, give_id);
				return -1;
			}
			*/

			item = &(out->items[item_idx]);
			
			out->items_cnt++;

			item->give_type = give_type;
			item->give_id = give_id;
			item->cnt = count;
			item->role_type = role_type;
			item_idx++;
		}
		cur = cur->next;
	}
	return 0;
}

/**
 * @brief load <OutItem> level's config of bonus_tsk's info from conf/tasks.xml file
 * @param cur the xml_pointer to the node which pointing to "<BonusTask>" level
 * @param tsk the pointer to current bonus_task 
 * @param out the pointer to current outitem
 * @return 0 on success, -1 on error
 */
int load_one_bonus_task_out_items(xmlNodePtr cur, struct task_t* tsk, struct task_out_t* out)
{
	/* loading bonus_task_item is the same with the loading task_item */
	return load_one_task_out_items(cur, tsk, out);
}

/**
 * @brief load only one task's info from conf/task.xml
 * @param cur the xml_pointer to the node which pointing to "<Task>" level
 * @param tsk the current task
 * @return 0 on success, -1 on error
 */
int load_one_task(xmlNodePtr cur, struct task_t* tsk)
{
	/* cur is at <Task> level now */
	struct task_in_t* in = 0;
	struct task_out_t* out = 0;
	uint32_t in_id = 0;
	uint32_t out_id = 0;

	cur = cur->xmlChildrenNode; /* goto <OutItem> */
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"TaskStep")) {
			if (load_one_task_step(cur, tsk) < 0) {
				ERROR_LOG("load_one_task_step failed, tsk_id=%u",tsk->id);
				return -1;
			}
		}
		if (!xmlStrcmp(cur->name, (const xmlChar *)"InItem")) {
			get_xml_prop(in_id, cur, "id");
			if (!is_valid_inid(in_id)) {
				ERROR_LOG("Load InItem failed: invalid InID:%d, task_id=%u",
						in_id, tsk->id);
				return -1;
			}
			in = &(tsk->in[in_id - 1]);
			if (in->id != 0) {
				ERROR_LOG("Duplicated in_id:%u, task_id=%u", in_id, tsk->id);
				return -1;
			}
			in->id = in_id;
			tsk->in_cnt++;
			if (load_one_task_in_items(cur, tsk, in) < 0) {
				ERROR_LOG("load_one_task_in_items failed, tsk_id=%u, in_id=%u",
						tsk->id, in_id);
				return -1;
			}
			if (tsk->in_cnt > max_task_in) {
				ERROR_LOG("Too many out_items!");
				return -1;
			}
		}
		if (!xmlStrcmp(cur->name, (const xmlChar *)"OutItem")) {
			get_xml_prop(out_id, cur, "id");
			if (!is_valid_outid(out_id)) {
				ERROR_LOG("Load OutItem failed: invalid outid:%d, task_id=%u",
						out_id, tsk->id);
				return -1;
			}
			out = &(tsk->out[out_id - 1]);
			if (out->id != 0) {
				ERROR_LOG("Duplicated out_id:%u, task_id=%u", out_id, tsk->id);
				return -1;
			}
			out->id = out_id;
			tsk->out_cnt++;
			if (load_one_task_out_items(cur, tsk, out) < 0) {
				ERROR_LOG("load_one_task_out_items failed, tsk_id=%u, out_id=%u",
						tsk->id, out_id);
				return -1;
			}
			if (tsk->out_cnt > max_task_out) {
				ERROR_LOG("Too many out_items!");
				return -1;
			}
		}
		cur = cur->next;
	}
	return 0;
}

/**
 * @brief load only one bonus_task's info from conf/task.xml
 * @param cur the xml_pointer to the node which pointing to "<BonusTask>" level
 * @param tsk the current task
 * @return 0 on success, -1 on error
 */
int load_one_bonus_task(xmlNodePtr cur, struct task_t* tsk)
{
	/* cur is at <BonusTask> level now */
	struct task_out_t* out = 0;
	uint32_t out_id = 0;

	cur = cur->xmlChildrenNode; /* goto <OutItem> */
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"OutItem")) {
			get_xml_prop(out_id, cur, "id");
			if (!is_valid_outid(out_id)) {
				ERROR_LOG("Load OutItem failed: invalid outid:%d", out_id);
				return -1;
			}
			out = &(tsk->out[out_id - 1]);
			if (out->id != 0) {
				ERROR_LOG("Duplicated out_id:%u", out_id);
				return -1;
			}
			out->id = out_id;
			tsk->out_cnt++;
			if (load_one_bonus_task_out_items(cur, tsk, out) < 0) {
				ERROR_LOG("load_one_bonus_task_out_items, tsk_id=%u, out_id:%u",
						tsk->id, out_id);
				return -1;
			}
			if (tsk->out_cnt > max_task_out) {
				ERROR_LOG("Too many out_items!");
				return -1;
			}
		}
		cur = cur->next;
	}
	return 0;
}

/**
 * @brief load tasks' info from conf/tasks.xml file
 * @param p the player
 * @return 0 on success, -1 on error
 */
int load_tasks_config(xmlNodePtr cur)
{
	uint32_t id = 0;
	struct task_t* tsk = 0;
	int limit = 0;

	memset(all_tasks, 0, sizeof(all_tasks));
	memset(all_bonus_tasks, 0, sizeof(all_bonus_tasks));
	memset(all_daily_tasks, 0, sizeof(all_daily_tasks));
	
	if (!daily_task_map.empty()) {
		daily_task_map.clear();
	}
	cur = cur->xmlChildrenNode; /* goto <Task>/<BonusTask> */
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Task"))) {
			get_xml_prop(id, cur, "id");
			if (id < 1 && id > max_task_id) { /* also load daily task */
				ERROR_LOG("Invalid task_id:%d in config file!", id);
				return -1;
			}
			tsk = &all_tasks[id - 1];
			if (tsk->id != 0) {
				ERROR_LOG("Duplicated task_id:%d", id);
				return -1;
			}
			tsk->id = id;
			
			uint32_t tsk_type = 0;
			get_xml_prop(tsk_type, cur, "type");
			tsk->type = tsk_type;
			if ( tsk->type >= max_type && tsk->type <= 0) {
				ERROR_LOG("Invalid type:%d in config file!", tsk->type);
				return -1;
			}
			
			get_xml_prop(tsk->need_role, cur, "need_role");
			if ( tsk->need_role > max_role_type) {
				ERROR_LOG("Invalid need_role:%d in config file!", tsk->need_role);
				return -1;
			}
			
			//get_xml_prop_def(tsk->need_lv, cur, "need_lv", 1);
			int arr_len = get_xml_prop_arr(tsk->need_lv, cur, "need_lv");
			if (arr_len != 2) {
				ERROR_LOG("Invalid need lv arr len:%d in config file, tskid:%u!", arr_len, tsk->id);
				return -1;
			} else if (tsk->need_lv[0] > tsk->need_lv[1]) {
				ERROR_LOG("Invalid need lv:%d %d in config file, tskid:%u!", tsk->need_lv[0], tsk->need_lv[1], tsk->id);
				return -1;
			}

			arr_len = get_xml_prop_arr_def(tsk->need_stage, cur, "stage_finish", 0);
			if (arr_len > 3) {
				ERROR_LOG("Invalid need stage arr len:%d in config file, tskid:%u!", arr_len, tsk->id);
				return -1;
			}

			arr_len = get_xml_prop_arr_def(tsk->need_task, cur, "task_finish", 0);
			if (arr_len > 3) {
				ERROR_LOG("Invalid need task arr len:%d in config file, tskid:%u!", arr_len, tsk->id);
				return -1;
			}

			uint32_t vip_limit;
			get_xml_prop_def(vip_limit, cur, "vip_limit", 0);
			tsk->vip_limit = vip_limit;

			get_xml_prop_def(limit, cur, "limit", 0);
			if (limit != 0 && limit != 1) {
				ERROR_LOG("Invalid task config - Wrong limit=%d", limit);
				return -1;
			}
			tsk->limit = limit;

			uint32_t master_task_id = 0;
			get_xml_prop_def(master_task_id, cur, "master_task_id", 0);
			tsk->master_task_id = master_task_id;

			uint32_t other_limit_id = 0;
			get_xml_prop_def(other_limit_id, cur, "other_limit_id", 0);
			tsk->other_limit_id = other_limit_id;


			uint32_t is_outmoded = 0;
			get_xml_prop_def(is_outmoded, cur, "is_outmoded", 0);
			tsk->is_outmoded = is_outmoded > 0 ? 1 : 0;

			if (load_one_task(cur, tsk) < 0) {
				ERROR_LOG("load_one_task failed, task_id:%d", id);
				return -1;
			}
		}
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("BonusTask"))) {
			get_xml_prop(id, cur, "id");
			if (!is_valid_bonus_taskid(id)) {
				ERROR_LOG("Invalid bonus_task_id:%d in config file!", id);
				return -1;
			}
			tsk = &all_bonus_tasks[id - bonus_task_base_id - 1];
			if (tsk->id != 0) {
				ERROR_LOG("Duplicated bonus_task_id:%d", id);
				return -1;
			}
			tsk->id = id;

			get_xml_prop_def(limit, cur, "limit", 0);
			if (limit != 0 && limit != 1) {
				ERROR_LOG("Invalid bonus task config - Wrong limit=%d", limit);
				return -1;
			}
			tsk->limit = limit;

			if (load_one_bonus_task(cur, tsk) < 0) {
				ERROR_LOG("load_one_bonus_task failed, task_id:%d", id);
				return -1;
			}
		}
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("DailyTasks"))) {
			if (load_daily_tasks_config(cur) < 0) {
				ERROR_LOG("LOAD DAILY TASKS CONFIG ERROR!");
				return -1;
			}
		}

		cur = cur->next;
	}

#ifdef ENABLE_TRACE_LOG
	log_task_config();
#endif

	return 0;
}


int load_one_daily_task_ex(xmlNodePtr cur, daily_task_t* p_daily_task, uint8_t in_type = 0)
{
	uint32_t count = 0;
	if (in_type) {
		count = p_daily_task->lucky_cnt;
	} else {
		count = p_daily_task->cnt;
	}
	
	if (count > max_daily_cnt) {
		ERROR_LOG("Invalid > max_daily_cnt %u ", count);
		return -1;
	}
	uint32_t id = 0;
	get_xml_prop(id, cur, "id");
	
	if (id < min_nor_daily_task_id || id >= max_nor_daily_task_id) {
		ERROR_LOG("Invalid task_id:%d in config file with lv[%u]!", id, p_daily_task->lv);
		return -1;
	}
	task_t* p_task ;
	if (in_type) {
		p_task = &(p_daily_task->lucky_task[count]);
	} else {
		p_task = &(p_daily_task->task_info[count]);
	}
	p_task->id = id;
	
	uint32_t tsk_type = 0;
	get_xml_prop(tsk_type, cur, "type");
	p_task->type = tsk_type;
	if ( p_task->type >= max_type && p_task->type <= 0) {
		ERROR_LOG("Invalid type:%d in config file!", p_task->type);
		return -1;
	}
	
	get_xml_prop(p_task->need_role, cur, "need_role");
	if ( p_task->need_role > max_role_type) {
		ERROR_LOG("Invalid need_role:%d in config file!", p_task->need_role);
		return -1;
	}
	
	//get_xml_prop_def(tsk->need_lv, cur, "need_lv", 1);
	int arr_len = get_xml_prop_arr(p_task->need_lv, cur, "need_lv");
	if (arr_len != 2) {
		ERROR_LOG("Invalid need lv arr len:%d in config file, tskid:%u!", arr_len, p_task->id);
		return -1;
	} else if (p_task->need_lv[0] > p_task->need_lv[1]) {
		ERROR_LOG("Invalid need lv:%d %d in config file, tskid:%u!", p_task->need_lv[0], p_task->need_lv[1], p_task->id);
		return -1;
	}
	p_task->need_lv[0] = 1;
	p_task->need_lv[1] = 999;
	TRACE_LOG("task %u %u %u %u", count, p_task->id, p_task->need_lv[0], p_task->need_lv[1]);
	
	arr_len = get_xml_prop_arr_def(p_task->need_stage, cur, "stage_finish", 0);
	if (arr_len > 3) {
		ERROR_LOG("Invalid need stage arr len:%d in config file, tskid:%u!", arr_len, p_task->id);
		return -1;
	}
	
	arr_len = get_xml_prop_arr_def(p_task->need_task, cur, "task_finish", 0);
	if (arr_len > 3) {
		ERROR_LOG("Invalid need task arr len:%d in config file, tskid:%u!", arr_len, p_task->id);
		return -1;
	}
	
	uint32_t vip_limit;
	get_xml_prop_def(vip_limit, cur, "vip_limit", 0);
	p_task->vip_limit = vip_limit;
	
	int limit = 0;
	get_xml_prop_def(limit, cur, "limit", 0);
	if (limit != 0 && limit != 1) {
		ERROR_LOG("Invalid task config - Wrong limit=%d", limit);
		return -1;
	}
	p_task->limit = limit;

	uint32_t is_outmoded = 0;
	get_xml_prop_def(is_outmoded, cur, "is_outmoded", 0);
	p_task->is_outmoded = is_outmoded > 0 ? 1 : 0;
	
	if (load_one_task(cur, p_task) < 0) {
		ERROR_LOG("load_one_task failed, task_id:%d", id);
		return -1;
	}	
	daily_task_map.insert(std::map<uint32_t, task_t>::value_type(p_task->id, *p_task));
	count ++;
	if (in_type) {
		p_daily_task->lucky_cnt = count;
	} else {
		p_daily_task->cnt = count;
	}
	return 0;

}

int load_one_daily_task(xmlNodePtr cur, daily_task_t* p_daily_task, uint8_t in_type = 0)
{
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Task"))) {
			if (load_one_daily_task_ex(cur, p_daily_task, 0) != 0) {
				ERROR_LOG("LOAD ONE DAILEY TASAK EX ERROR");
				return -1;
			}
		}
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("LuckyTask"))) {
			if (load_one_daily_task_ex(cur, p_daily_task, 1) != 0) {
				ERROR_LOG("LOAD ONE DAILEY TASAK EX ERROR");
				return -1;
			}
		}
		cur = cur->next;
	}
	if (p_daily_task->cnt < p_daily_task->times) {
		ERROR_LOG("load_one_task failed, %u, task_id:%u > %u", p_daily_task->lv, p_daily_task->cnt, p_daily_task->times);
		return -1;
	}	
	return 0;
}

void log_daily_task_config()
{
	for (uint32_t t = 0; t < 101; t++){
		daily_task_t* p_lv_task = &(all_daily_tasks[t]);
		//ERROR_LOG("LOG_ONE LV TASK_CONF\t[%u lv[2]: %u %u ]", t, p_lv_task->level[0], p_lv_task->level[1]);
		for (uint32_t i = 0; i < all_daily_tasks[t].cnt; ++i) {
			task_t* p_task = &(p_lv_task->task_info[i]);
			if (!p_task->id) {
				continue;
			}
			TRACE_LOG("LOG_TASK_CONF\t[level %u task[%d]->id=%u]", t, i, p_task->id);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->need_lv=%u %u]", i, p_task->need_lv[0], p_task->need_lv[1]);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->need_role=%u]", i, p_task->need_role);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->step_cnt=%u]", i, p_task->step_cnt);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->in_cnt=%u]", i, p_task->in_cnt);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->out_cnt=%u]", i, p_task->out_cnt);
			TRACE_LOG("======================================");
		}
		for (uint32_t i = 0; i < all_daily_tasks[t].lucky_cnt; ++i) {
			task_t* p_task = &(p_lv_task->lucky_task[i]);
			if (!p_task->id) {
				continue;
			}
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->id=%u]", i, p_task->id);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->need_lv=%u %u]", i, p_task->need_lv[0], p_task->need_lv[1]);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->need_role=%u]", i, p_task->need_role);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->step_cnt=%u]", i, p_task->step_cnt);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->in_cnt=%u]", i, p_task->in_cnt);
			TRACE_LOG("LOG_TASK_CONF\t[task[%d]->out_cnt=%u]", i, p_task->out_cnt);
			TRACE_LOG("======================================");
		}

	}
}

/**
 * @brief load daily tasks' info from conf/tasks.xml file
 * @param p the player
 * @return 0 on success, -1 on error
 */
int load_daily_tasks_config(xmlNodePtr cur)
{
	uint16_t lv[2] = { 0 };
	uint32_t id = 0;
	get_xml_prop(max_nor_daily_task_times, cur, "times");
	cur = cur->xmlChildrenNode; /* goto <Task>/<BonusTask> */
	while (cur) {
		
		uint16_t lower_idx ;
		uint16_t upper_idx ;
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Level"))) {
			get_xml_prop(id, cur, "id");
			
			TRACE_LOG("lv %u %u", id, max_nor_daily_task_times);

			int arr_len = get_xml_prop_arr(lv, cur, "level");
			if (arr_len != 2) {
				ERROR_LOG("Invalid need lv arr len:%d in config file, id:%u!", arr_len, id);
				return -1;
			} else if (lv[0] > lv[1] || lv[1] > player_max_level) {
				ERROR_LOG("Invalid need lv:%d in config file, id:%u!", lv[0], id);
				return -1;
			}
				
			lower_idx = lv[0];
			upper_idx = lv[1];
			if (all_daily_tasks[lower_idx].lv > 0 || all_daily_tasks[upper_idx].lv > 0) {
				ERROR_LOG("Invalid lv id:%u !", all_daily_tasks[lower_idx].lv);
				return -1;
			}
			all_daily_tasks[lower_idx].lv = id;
			all_daily_tasks[lower_idx].level[0] = lv[0];
			all_daily_tasks[lower_idx].level[1] = lv[1];
			all_daily_tasks[lower_idx].times = max_nor_daily_task_times;
			if (load_one_daily_task(cur, &(all_daily_tasks[lower_idx])) != 0)	{
				return -1;
			}
			TRACE_LOG("dailyidx %u %u %u", lower_idx, upper_idx, all_daily_tasks[lower_idx].times);

			if (lower_idx < upper_idx) {
				for (uint16_t i = lower_idx + 1; i <= upper_idx; i++) {
					memcpy(&all_daily_tasks[i], &all_daily_tasks[lower_idx], sizeof(all_daily_tasks[i]));
					TRACE_LOG("cpy %u %u %u %u", i, lower_idx, all_daily_tasks[i].level[0], all_daily_tasks[i].level[1]);
				}
			}
		}
		
		cur = cur->next;
	}
	log_daily_task_config();
	return 0;
}

inline bool is_stage_task_drop(const task_item_t * p_item, uint32_t stage)
{
	if (p_item->stage && p_item->stage == stage ) {
		return true;
	}
	return false;
}

/**
 * @brief pack the player's task info into battle
 * @param p the player
 * @param pkgbuf
 * @return the idx
 */
int pack_task_to_btl(player_t* p, void* pkgbuf, uint32_t stage, uint32_t difficulty)
{
	uint32_t item_cnt = 0;
	//uint32_t monster_cnt = 0;
	int idx = 4;

	static player_task_item_t tasks_item_info[max_task_items];
	//memset(&(p->tasks_item_info), 0x00, sizeof(p->tasks_item_info));

	std::map<uint32_t, task_going_t>::iterator it;
	TRACE_LOG("task going count[%u %lu]", p->id, (p->going_tasks_map->size()));
	for ( it = p->going_tasks_map->begin(); it != p->going_tasks_map->end(); ++it ) {
		TRACE_LOG("task id[%u %u]", p->id, it->second.id);
		const task_t* p_task = get_task(it->second.id);
		//item
		for ( uint32_t i = 0; i < p_task->in_cnt; i++) {
			const task_in_t* p_in = &(p_task->in[i]);
			for ( uint32_t j = 0; j < p_in->items_cnt; j++ ) {
				const task_item_t* p_item = &(p_in->items[j]);
				if (is_stage_task_drop(p_item, stage) && item_cnt < max_task_items) {
					const GfItem* itm = items->get_item(p_item->give_id);
					if ( items->is_task_item(itm->category()) ) {
						uint32_t item_pack_cnt = p->my_packs->get_item_cnt(p_item->give_id);
						if (item_pack_cnt >= p_item->cnt) {
							continue;
						} else {
							tasks_item_info[item_cnt].rest_cnt = p_item->cnt - item_pack_cnt;
						}
					} else {
						tasks_item_info[item_cnt].rest_cnt = itm->max();
					}

					tasks_item_info[item_cnt].item_id = p_item->give_id;
					tasks_item_info[item_cnt].monster_id = p_item->monster_id;
					tasks_item_info[item_cnt].drop_odds = p_item->drop_odds;
					TRACE_LOG("item:[%u %u %u]", p_item->give_id, p_item->monster_id, p_item->drop_odds);
					item_cnt++;
				}
			}
		}
		//monster
	}

//	//for xunlu active  tmp code
//	tasks_item_info[item_cnt].rest_cnt = calc_can_drop_box_cnt_2_btl(p);
//	tasks_item_info[item_cnt].item_id = 1500576;
//	tasks_item_info[item_cnt].monster_id = 0;
//	tasks_item_info[item_cnt].drop_odds = 50;
//	item_cnt++;
//
	idx = 0;

	//pack item
	pack_h(pkgbuf, item_cnt, idx);
	TRACE_LOG("item_cnt:[%u %u]", p->id, item_cnt);
	pack(pkgbuf, tasks_item_info, sizeof(tasks_item_info), idx);

	if ( item_cnt == max_task_items ) {
		WARN_LOG("count more than max[%u]",item_cnt);
	}

	return idx;
}

/**
 * @brief unpack the player's task info from battle
 * @param p the player
 * @param count the task number
 * @param body the info of task
 */
void unpack_task_buf_info(player_t* p, uint32_t count, uint8_t* body)
{
	player_task_monster_t* p_task = reinterpret_cast<player_task_monster_t*>(body);

	for ( uint32_t i = 0; i < count; i++ ) {
		std::map<uint32_t, task_going_t>::iterator it = p->going_tasks_map->find(p_task[i].task_id);
		if (it == p->going_tasks_map->end()) {
			continue;
		}
		//assert(it != p->going_tasks_map->end());
		memcpy(it->second.serv_buf, p_task[i].serv_buf, serv_buf_len);
		TRACE_LOG("p[%u],tsk_id[%u],count[%u],buf[%*s]",p->id, i, p_task[i].task_id, serv_buf_len, it->second.serv_buf);
	}
}

/**
 * @brief pack the player's task info
 * @param p the player
 * @param pkgbuf
 * @return the idx
 */
int pack_player_task_info(player_t* p, void* buf)
{
	int idx = 0;

	//max daily times
	uint16_t max_daily_get = get_player_max_daily_cnt(p);
	pack(buf, max_daily_get, idx);

	//going daily task
	uint16_t daily_id = get_going_daily_task_id(p);
	pack(buf, daily_id, idx);
	TRACE_LOG("going [%u %u]", daily_id, max_daily_get);
	//finished daily task
	uint16_t finished_daily_cnt = 0;//get_finished_daily_task_count(p);
	pack(buf, finished_daily_cnt, idx);
	TRACE_LOG("finished_cnt[%u]", finished_daily_cnt);
	//std::set<uint32_t>::iterator beg = p->finished_tasks_set->lower_bound(min_nor_daily_task_id);
	//std::set<uint32_t>::iterator end = p->finished_tasks_set->upper_bound(max_nor_daily_task_id);
	//while(beg != end) {
	//	uint16_t tsk_id = *beg;
	//	pack(buf, tsk_id, idx);
	//	TRACE_LOG("--tsk_id[%u]",tsk_id);
	//	++beg;
	//}

	//canceled daily task
	uint16_t cancel_cnt = p->canceled_tasks_set->size();
	pack(buf, cancel_cnt, idx);
	TRACE_LOG("canceleded_cnt[%u]", cancel_cnt);
	std::set<uint32_t>::iterator cancel_it;
	for ( cancel_it = p->canceled_tasks_set->begin(); cancel_it != p->canceled_tasks_set->end(); ++cancel_it ) {
		uint16_t tsk_id = *cancel_it;
		pack(buf, tsk_id, idx);
		TRACE_LOG("--tsk_id[%u]", tsk_id);
	}
	
	uint16_t finished_cnt = p->finished_tasks_set->size() - finished_daily_cnt;
	pack(buf, finished_cnt, idx);
	TRACE_LOG("finished_cnt[%u]",finished_cnt);
	std::set<uint32_t>::iterator it_set;
	std::set<uint32_t>::iterator it_set_end = p->finished_tasks_set->lower_bound(min_nor_daily_task_id);
	for ( it_set = p->finished_tasks_set->begin(); it_set != it_set_end; ++it_set ) {
		uint16_t tsk_id = *it_set;
		pack(buf, tsk_id, idx);
		TRACE_LOG("--tsk_id[%u]",tsk_id);
	}

	uint16_t going_cnt = p->going_tasks_map->size() - (daily_id ? 1 : 0);
	pack(buf, going_cnt, idx);
	TRACE_LOG("going_cnt[%u]", going_cnt);
	std::map<uint32_t, task_going_t>::iterator it_map;
	std::map<uint32_t, task_going_t>::iterator it_map_end = p->going_tasks_map->lower_bound(min_nor_daily_task_id);
	for ( it_map = p->going_tasks_map->begin(); it_map != it_map_end; ++it_map) {
		uint16_t tsk_id = it_map->second.id;
		pack(buf, tsk_id, idx);
		TRACE_LOG("--tsk_id[%u]",tsk_id);
	}

	return idx;
}

int send_undertake_daily_rsp(player_t* p, uint32_t taskid)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, taskid, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	TRACE_LOG("take task %u", taskid);
	return send_to_player(p, pkgbuf, idx, 1);
}

int send_undertake_task_rsp(player_t* p, uint32_t taskid, uint32_t group_id)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, taskid, idx);
	pack(pkgbuf, group_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
 * @brief finish a task step selector
 * @param p the player
 * @param step the Xth step on a finish_task way
 * @return 0 on success, -1 on error
 */
int finish_task_step_over(player_t* p, uint32_t step, uint32_t task_id)
{

	//uint32_t buf[1] = { 1 };
	switch (step) {
	case finish_task_init:
		TRACE_LOG("READY TO ADD_ITEMS\t[uid=%u]", p->id);
		return db_add_items(p, task_id);

	case finish_task_added_items:
		TRACE_LOG("READY TO SET_TASK_FLAG\t[uid=%u]", p->id);
		//stat log :got skill book ,etg. after finish task
		do_stat_log_after_task_added_items(p);
		return set_finish_task_flag(p);

	case finish_task_set_task_flag:
		tmp_log_for_task_bug(p, 4);
		return finish_task_response_to_client(p);

	default:
		ERROR_LOG("unknown finish step: uid=%u, step=%u", p->id, step);
		return -1;
	}

	return 0;
}

/**
 * @brief finish a task
 * @param p the player
 * @param tsk_id task id
 * @param out_id request out item's id
 * @return 0 on success, -1 on error
 */
int do_finish_task(player_t* p, uint32_t tsk_id, uint32_t out_id, const task_t* tsk)
{
	fin_tsk_session_t* fin_sess = reinterpret_cast<fin_tsk_session_t *>(p->session);
	const struct task_in_t* in = 0;
	const struct task_out_t* out = 0;

	in = &(tsk->in[0]);
	out = &(tsk->out[out_id - 1]);
	if (tsk->id != tsk_id || out->id != out_id) {
		ERROR_LOG("invalid task or outitem: uid=%u, task_id(%u) is or out_id(%u)",
				p->id, tsk_id, out_id);
		return -1;
	}

	memset(fin_sess, 0, sizeof(*fin_sess));
	fin_sess->tsk_id = tsk_id;
	fin_sess->tsk_lv = tsk->need_lv[0];
	fin_sess->in = in;
	fin_sess->out = *out;
	p->sesslen = sizeof(fin_tsk_session_t);

	if (finish_task_step_over(p, finish_task_init, tsk->id) < 0) {
		ERROR_LOG("uid=%u, step=%u, tsk_id=%u, out_id=%u, items_count=%u",
				p->id, finish_task_init, tsk_id, out_id, out->items_cnt);
		return -1;
	}

	return 0;
}

//------------------------------------------------------------------
// response to client functions
//------------------------------------------------------------------

/**
 * @brief one of finish a task step: response the finish_task result to client
 * @param p the player
 * @return NULL
 */
void trace_log_finish_task_response_to_client(player_t* p)
{
#ifdef ENABLE_TRACE_LOG 
	fin_tsk_session_t* fin_sess = reinterpret_cast<fin_tsk_session_t *>(p->session);
	const struct task_out_t* out = &(fin_sess->out);
	const struct task_item_t* item = 0;
	int item_seq = 1;

	TRACE_LOG("FINISH TASK STEP COMPLETED SEND RESP TO PLAYER\t");
	TRACE_LOG("[uid=%u, tsk_id=%u, out_id=%u, items_cnt=%u",
			p->id, fin_sess->tsk_id, out->id, out->items_cnt);


	for (uint32_t i = 0; i < out->items_cnt; i++) {
		item = &(out->items[i]);
		switch (item->give_type) {
		case give_type_normal_item:
		case give_type_player_attr:
			switch (item->give_id) {
			case 1: /* coins */
			case 2:
				TRACE_LOG("--->%d.[coins]: give_id=%u, add_count=%u, player->coins=%u",
						item_seq++, item->give_id, item->cnt, p->coins);
				break;
			case 300001 ... 300004: /* monster ball */
				TRACE_LOG("--->%d.[mon_ball]: give_id=%u, add_count=%u",
						item_seq++, item->give_id, item->cnt);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
#endif
}

void tmp_log_for_task_bug(player_t* p, uint32_t flag)
{
	fin_tsk_session_t* fin_sess = reinterpret_cast<fin_tsk_session_t *>(p->session);
	KDEBUG_LOG(p->id, "%u TASK %u %u %u", flag, fin_sess->tsk_id, fin_sess->out_skills_num, fin_sess->out_clothes_num);
}

/**
 * @brief one of finish a task step: response the finish_task result to client
 * @param p the player
 * @return 0 on success, -1 on error
 */
int finish_task_response_to_client(player_t* p)
{
	fin_tsk_session_t* fin_sess = reinterpret_cast<fin_tsk_session_t *>(p->session);
	int idx = sizeof(cli_proto_t);
	const struct task_out_t* out = &(fin_sess->out);
	const struct task_item_t* item = 0;

	if ( fin_sess->out_skills_num > 10 || fin_sess->out_clothes_num > 10 || out->items_cnt > 10) {
		p->waitcmd = 0;
		WARN_LOG("%u TASK %u %u %u %u ", p->id, fin_sess->tsk_id, fin_sess->out_skills_num, fin_sess->out_clothes_num, out->items_cnt);
		return 0;
	}

	//del from my packs
	const struct task_in_t* my_in = fin_sess->in;
	if (my_in) {
		if ( my_in->items_cnt > 10 ) {
			p->waitcmd = 0;
	                WARN_LOG("%u TASK %u %u %u %u %u", p->id, fin_sess->tsk_id, fin_sess->out_skills_num, fin_sess->out_clothes_num, out->items_cnt, my_in->items_cnt);
        	        return 0;
		}
		for (uint32_t i = 0; i < my_in->items_cnt; i++) {
			const struct task_item_t* itm = &(my_in->items[i]);
			if (itm->give_type == give_type_clothes) {
				//p->my_packs->del_clothes();
			} else if (itm->give_type == give_type_normal_item) {
				if (is_need_clear_task_id(fin_sess->tsk_id)) {
					uint32_t itm_cnt = p->my_packs->get_item_cnt(itm->give_id);
					if (itm_cnt < itm->cnt) {
						p->my_packs->del_item(p, itm->give_id, itm_cnt, channel_string_task);
					} else {
						p->my_packs->del_item(p, itm->give_id, itm->cnt, channel_string_task);
					}
				} else {
					p->my_packs->del_item(p, itm->give_id, itm->cnt, channel_string_task);
				}
			}
		}
	}


	/* task id */
	pack(pkgbuf, fin_sess->tsk_id, idx);

	/* items & skills*/
	pack(pkgbuf, fin_sess->out_skills_num, idx);
	int skill_idx = idx;
	idx += 4 * fin_sess->out_skills_num;
	
	int cnt_idx = idx;
	idx += 4;
	uint32_t item_count = 0;
	for (uint32_t i = 0; i < out->items_cnt; i++) {
		item = &(out->items[i]);
		if (item->give_type == give_type_normal_item || item->give_type == give_type_player_attr) {
			if (item->role_type == 0 || item->role_type == p->role_type) {
				pack(pkgbuf, item->give_id, idx);
				pack(pkgbuf, static_cast<uint32_t>(item->cnt), idx);
				item_count++;

				//add to my packs
				if (item->give_type == give_type_normal_item) {
					p->my_packs->add_item(p, item->give_id, item->cnt, channel_string_task, true, task_reward);
					//if the item is relive item then log it
					const GfItem* gitm = items->get_item(item->give_id);
					if (is_storage_relive_item(item->give_id) || gitm->is_item_shop_sale()) {	
						stat_log_relive_item(item->give_id, item->cnt);
					}
				}
			}
		} else if (item->give_type == give_type_skill) {
			pack(pkgbuf, item->give_id, skill_idx);
		}
		KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u taskid=%u channel=%s]", 
			item->give_type, item->give_id, item->cnt, fin_sess->tsk_id, channel_string_task);
	}
	pack(pkgbuf, item_count, cnt_idx);

	/* clothes */
	pack(pkgbuf, fin_sess->out_clothes_num, idx);
	for(uint32_t i = 0; i < fin_sess->out_clothes_num; i++) {
		pack(pkgbuf, fin_sess->clothes[i].clothes_id, idx);
		pack(pkgbuf, fin_sess->clothes[i].unique_id, idx);
		
		//add to my packs
		p->my_packs->add_clothes(p, fin_sess->clothes[i].clothes_id, fin_sess->clothes[i].unique_id, 0, channel_string_task);
	}

	/* update player attr, 2009-6-17: move to db_swap_item_callback */

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);

#ifdef ENABLE_TRACE_LOG 
	trace_log_finish_task_response_to_client(p);
#endif
	
	KDEBUG_LOG(p->id, "FINISH TASK\t[%u %u]", p->id, fin_sess->tsk_id);
	return send_to_player(p, pkgbuf, idx, 1);
}

 /**
  * @brief response to cancel player's task of taskid
  * @param p the requester
  * @param id uid of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @return 0 on success, -1 and p will be deleted
  */
int cancel_task_rsp_to_client(player_t* p, userid_t id, void* body, uint32_t bodylen)
{
	uint32_t taskid = *reinterpret_cast<uint32_t *>(body);
	p->going_tasks_map->erase(taskid);
	if (is_daily_taskid(taskid)) {
		p->canceled_tasks_set->insert(taskid);
	}
	//如果是大环任务
	ring_task_data* p_data = get_ring_task_mgr()->get_ring_task_data_by_id(taskid);
	player_ring_task_data* p_user_data = get_player_ring_task(p, taskid);
	if(p_data && p_user_data && p_user_data->task_type == master_ring_task)
	{
		slave_task_group* p_group = p_data->get_task_group_by_id(p_user_data->task_group_id);
		if(p_group != NULL)
		{
			for(uint32_t i=0;  i < p_group->slave_task_ids.size(); i++)
			{
				uint32_t slave_id = p_group->slave_task_ids[i];
				p->going_tasks_map->erase(slave_id);
				p->finished_tasks_set->erase(slave_id);
				db_set_task_flag(p, p->id, slave_id, 0, set_flag_without_back);
			}
		}
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, taskid, idx);
	TRACE_LOG("player[%u] cancel task[%u]",p->id, taskid);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

//------------------------------------------------------------------
// request to db functions
//------------------------------------------------------------------

/**
 * @brief Send request of setting buf for a task to DB
 * @param p the player
 * @param tsk_id the id of current task
 * @param tobe the start of the buf to set
 * @param tobe_len the length of the buf to set
 * @param buf_type '1' means serv_buf and '2' means client_buf
 * @return 0 on success, -1 on error
 */
int db_set_task_buf(player_t* p, uint32_t tsk_id, char* tobe, int tobe_len, uint32_t buf_type, bool callback)
{
	int idx = 0;
	pack_h(dbpkgbuf, tsk_id, idx);
	pack_h(dbpkgbuf, buf_type, idx);
	pack(dbpkgbuf, tobe, tobe_len, idx);
	TRACE_LOG("[uid=%u tskid=%u buf_type=%d tobe=[%*s]]", p->id, tsk_id, buf_type, tobe_len, tobe);
	return send_request_to_db( (callback ? p : NULL) , p->id, p->role_tm, dbproto_set_task_buf, dbpkgbuf, idx);
}

/**
 * @brief Send request of getting buf for going tasks to DB
 * @param p the player
 * @return 0 on success, -1 on error
 */
int db_get_going_task_buf(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_going_task_buf, 0, 0);
}

/**
 * @brief Send request of getting list for done tasks to DB
 * @param p the player
 * @return 0 on success, -1 on error
 */
int db_get_done_task_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_done_task_list, 0, 0);
}

/**
 * @brief Send request of getting list for all tasks to DB
 * @param p the player
 * @return 0 on success, -1 on error
 */
int db_get_all_task_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_all_task_list, 0, 0);
}

/**
 * @brief one of finish a task step: Set current task's status to finished.
 * @param p the player
 * @return 0 on success, -1 on error
 */
int set_finish_task_flag(player_t* p)
{
	fin_tsk_session_t* fin_sess = reinterpret_cast<fin_tsk_session_t *>(p->session);
	uint32_t tsk_id;

	tsk_id = fin_sess->tsk_id;

	do_extra_tasks_finish_logic(p, tsk_id);

	p->going_tasks_map->erase(tsk_id);
	p->finished_tasks_set->insert(tsk_id);


	KDEBUG_LOG(p->id, "SET FINISH TASK FLAG\t[uid=%u task=%u]",
			p->id, tsk_id);
	tmp_log_for_task_bug(p, 2);
	return db_set_task_flag(p, p->id, tsk_id, 3);
}

/**
 * @brief check if the task be finished
 * @param p the player
 * @param task_id the id of the task to check
 * @return 0 on unfinished, 1 on finished
 */
int check_task_finish(player_t* p, const task_t* tsk)
{	
	std::map<uint32_t, task_going_t>::iterator it = p->going_tasks_map->find(tsk->id);
	if ( it == p->going_tasks_map->end() ) {
		ERROR_LOG("task not in going_task\t[uid=%u tskid=%u]",p->id, tsk->id);
		return 0;
	}
	return 1;
}


/**
 * @brief check the task buf
 * @param p_going_task the point of going task
 * @param p_going_task the point of tasks
 * @return 0 on unfinished, 1 on finished
 */
int check_task_buf(const task_going_t* p_going_task, const task_t* p_task)
{	
	for ( uint32_t i = 0; i < p_task->step_cnt; i++ ) {
		uint8_t cp = p_going_task->serv_buf[i];
		if ( cp != p_task->step[i].completeness ) {
			return 0;
		}
	}
	
	return 1;
}

/**
 * @brief add items for player "p"
 * @param p the player
 * @return 0 on success, -1 on error
 */
int db_add_items(player_t* p, uint32_t task_id)
{
	fin_tsk_session_t* fin_sess = reinterpret_cast<fin_tsk_session_t *>(p->session);
	uint32_t is_add_xiaomee_day = fin_sess->limit;
	const GfItem* cat_item = 0;
	const struct task_in_t* in = fin_sess->in;
	struct task_out_t* out = &(fin_sess->out);
	int idx = 0;
	const uint32_t zero = 0;
	int exp_double_rate = 1;
	bool is_tmp_clear_task_id = is_need_clear_task_id(task_id);
	
	if (is_daily_taskid(task_id) && (get_finished_daily_task_count(p) + get_canceled_daily_task_count(p)) < 5) {
		exp_double_rate = 2;
	}
	bool is_daily_task = is_daily_taskid(task_id);

	/* if not any item, we goto next step directly */
	if ((!in || !in->items_cnt) && (!out || !out->items_cnt)) {
		return finish_task_step_over(p, finish_task_added_items);
	}

	//calc attr loss by player lv & task lv
	uint32_t attr_per = 100;
	uint16_t lv_diff = p->lv - fin_sess->tsk_lv;
	if (lv_diff > 6) {
		attr_per = 50;
	} else if (lv_diff > 5) {
		attr_per = 60;
	} else if (lv_diff > 3) {
		attr_per = 70;
	}
	

	//send to db
	pack_h(dbpkgbuf, is_add_xiaomee_day, idx);
	int in_item_idx = idx;
	uint32_t in_item_cnt = in->items_cnt;
	if (in) {
		pack_h(dbpkgbuf, in_item_cnt, idx);
	} else {
		pack_h(dbpkgbuf, zero, idx);
	}
	
	int cnt_idx = 0;
	//some out item don't need, see by "role_type"
	uint32_t del_out_cnt = 0;
	if (out) {
		cnt_idx = idx;
		idx += 4;
		//pack_h(dbpkgbuf, static_cast<uint32_t>(out->items_cnt), idx);
	} else {
		pack_h(dbpkgbuf, zero, idx);
	}
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);
	

	/* in items */
	if (in) {
		const task_item_t* item = 0;
		for (uint32_t i = 0; i < in->items_cnt; i++) {
			item = &(in->items[i]);

			switch (item->give_type) {
			case give_type_player_attr:
				/* coins or exp of the player */
				pack_h(dbpkgbuf, static_cast<uint32_t>(item->give_type), idx);
				pack_h(dbpkgbuf, item->give_id, idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(item->cnt), idx);
				TRACE_LOG("add coins or exp in: uid=%u, item_catid=%u, item_id=%u, item_cnt=%u",
						p->id, item->give_type, item->give_id, item->cnt);

				break;
			case give_type_clothes:
				/* go through */
			case give_type_normal_item:
				if (is_tmp_clear_task_id) {
					uint32_t itm_cnt = p->my_packs->get_item_cnt(item->give_id);
					if (!itm_cnt) {
						in_item_cnt --;
						break;
					}
				}
				cat_item = items->get_item(item->give_id);
				pack_h(dbpkgbuf, cat_item->db_category(), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(item->give_id), idx);
				if (is_tmp_clear_task_id) {
					uint32_t itm_cnt = p->my_packs->get_item_cnt(item->give_id);
					if (itm_cnt < item->cnt) {
						pack_h(dbpkgbuf, itm_cnt, idx);
					} else {
						pack_h(dbpkgbuf, static_cast<uint32_t>(item->cnt), idx);
					}
				} else {
					pack_h(dbpkgbuf, static_cast<uint32_t>(item->cnt), idx);
				}
				
				TRACE_LOG("add items in: uid=%u, item_catid=%u, item_id=%u, item_cnt=%u",
						p->id, cat_item->db_category(), cat_item->id(), item->cnt);

				break;
			case give_type_skill:
			case give_type_max:
				/* skip */
				break;
			default:
				ERROR_LOG("BUG: uid=%u, unsupported item type:%u", p->id, item->give_type);
				return -1;
			}
		}
		pack_h(dbpkgbuf, in_item_cnt, in_item_idx);
	}

	/* out items */
	if (out) {
		task_item_t* p_out_item = 0;

		for (uint32_t i = 0; i < out->items_cnt; i++) {
            uint32_t opt_type = 0;
            uint32_t exv2 = 0;
            uint32_t exv3 = 0;
			p_out_item = &(out->items[i]);
			cat_item = items->get_item(p_out_item->give_id);
			uint32_t itm_cnt = p_out_item->cnt;

			switch (p_out_item->give_type) {
			case give_type_skill:
				fin_sess->out_skills_num++;
				/* go through */
			case give_type_player_attr:
                if (p_out_item->give_id == 1) 
				{
                    opt_type = opt_xiaomee;
					////前2次大环任务则翻倍
					if( is_master_ring_task(task_id) )
					{
						int count = get_player_ring_task_day_count(p, task_id);
						if( count <= 2)itm_cnt = itm_cnt* 2;
					}
					//根据子任务下标进行加成
					if( is_slave_ring_task(task_id))
					{
						uint32_t factor = get_factor_by_task_id(p, task_id);
						itm_cnt += itm_cnt*factor/100;	
					}
                } 
                else if (p_out_item->give_id == 2) 
				{
                	if (p->lv >= max_exp_lv) 
					{
						del_out_cnt++;
						p_out_item->cnt = 0;
						break;
                	} 
					else 
					{
                		if (is_daily_task) 
						{
	                    	itm_cnt = exp_double_rate * itm_cnt;
                		} 
						else 
						{
                			itm_cnt = exp_double_rate * itm_cnt * attr_per / 100;
                			//前2次大环任务则翻倍
							if( is_master_ring_task(task_id) )
							{
								int count = get_player_ring_task_day_count(p, task_id);
								if( count <= 2)
								{
									itm_cnt = itm_cnt* 2;
								}	
							}
							//根据子任务下标进行加成
							if( is_slave_ring_task(task_id))
							{
								uint32_t factor = get_factor_by_task_id(p, task_id);
								itm_cnt += itm_cnt*factor/100;
							}
						}
						//p_out_item->cnt = itm_cnt;
                	}
                }

				p_out_item->cnt = itm_cnt; 
				exv2 = itm_cnt;
				
				pack_h(dbpkgbuf, static_cast<uint32_t>(p_out_item->give_type), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(p_out_item->give_id), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(itm_cnt), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
				TRACE_LOG("%u add coins or exp out: uid=%u, item_catid=%u, item_id=%u, item_cnt=%u",
						task_id, p->id, p_out_item->give_type, p_out_item->give_id, itm_cnt);
                break;
			case give_type_clothes:
				if (p_out_item->role_type == 0 || p_out_item->role_type == p->role_type) {
					fin_sess->out_clothes_num++;
				}
				
				/* go through */
			case give_type_normal_item:
                opt_type = opt_item;//items
                exv2 = p_out_item->give_id;
                exv3 = itm_cnt;
				if (p_out_item->role_type != 0 && p_out_item->role_type != p->role_type) {
					del_out_cnt++;
					break;
				}
				pack_h(dbpkgbuf, cat_item->db_category(), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(p_out_item->give_id), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(itm_cnt), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(cat_item->duration * clothes_duration_ratio), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(cat_item->lifetime()), idx);
				pack_h(dbpkgbuf, static_cast<uint32_t>(cat_item->max()), idx);
				TRACE_LOG("add items: uid=%u, item_catid=%u, item_id=%u, item_cnt=%u, item_max=%u",
						p->id, cat_item->db_category(), cat_item->id(), p_out_item->cnt, cat_item->max());

				break;
			case give_type_max:
				/* skip */
				break;
			default:
				ERROR_LOG("BUG: uid=%u, unsupported item type:%u", p->id, p_out_item->give_type);
				return -1;
			}
            /* 0xF135 */
            if (opt_type != 0) {
                report_add_to_monitor(p, opt_type, exv2, exv3, 0);
            }
		}
	}
	pack_h(dbpkgbuf, static_cast<uint32_t>(out->items_cnt - del_out_cnt), cnt_idx);
	
	KDEBUG_LOG(p->id, "ADD ITMS\t[uid=%u in_cnt=%u, out_cnt=%u protolen=%u]",
			p->id, in ? in->items_cnt : 0, out ? out->items_cnt - del_out_cnt : 0, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_swap_item, dbpkgbuf, idx);
}

//------------------------------------------------------------------
// recv client's request functions
//------------------------------------------------------------------

/**
 * @brief Get the buf of a task which requested by id
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_task_buf_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0, idx2 = 0;
	uint32_t tsk_cnt = 0;
	unpack(body, tsk_cnt, idx);
	TRACE_LOG("[uid=%u tsk_cnt=%u]",p->id, tsk_cnt);


	CHECK_VAL_EQ(bodylen, 4 + tsk_cnt * 4);

	idx2 = sizeof(cli_proto_t);
	pack(pkgbuf, tsk_cnt, idx2);
	for ( uint32_t i = 0; i < tsk_cnt; i++ ) {
		uint32_t tsk_id;
		unpack(body, tsk_id, idx);
		
		if (!is_valid_taskid(tsk_id)) {
			ERROR_LOG("uid=%u, Invalid param, taskid:%u", p->id, tsk_id);
			return -1;
		}
		
		std::map<uint32_t, task_going_t>::iterator it = p->going_tasks_map->find(tsk_id);
		if ( it == p->going_tasks_map->end() ) {
			ERROR_LOG("uid=%u, task_id=%u is not undertaken", p->id, tsk_id);
			return -1;
		}

		pack(pkgbuf, tsk_id, idx2);
		pack(pkgbuf, it->second.serv_buf, serv_buf_len, idx2);
		pack(pkgbuf, it->second.client_buf, client_buf_len, idx2);
		TRACE_LOG("[uid=%u taskid=%u buf1=[%x] buf2=[%x] ]",p->id, tsk_id, *(uint32_t*)(it->second.serv_buf),  *(uint32_t*)(it->second.client_buf));
	}

	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx2);
	return send_to_player(p, pkgbuf, idx2, 1);
}




/**
 * @brief set client buf for a task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int set_task_buf_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t tsk_id;
	char* tobe = NULL;

	unpack(body, tsk_id, idx);
	tobe = reinterpret_cast<char *>(body) + idx;

	if (!is_valid_taskid(tsk_id)) {
		ERROR_LOG("uid=%u, Invalid param, taskid:%u", p->id, tsk_id);
		return -1;
	}
	
	std::map<uint32_t, task_going_t>::iterator it = p->going_tasks_map->find(tsk_id);
	if ( it == p->going_tasks_map->end() ) {
		ERROR_LOG("uid=%u, task_id=%u is not undertaken", p->id, tsk_id);
		return -1;
	}

	return db_set_task_buf(p, tsk_id, tobe, client_buf_len, client_buf_type);
}

inline bool has_going_treasure_task(player_t * p)
{
	std::map<uint32_t, task_going_t>::iterator it = p->going_tasks_map->begin();
	while (it != p->going_tasks_map->end()) {
		const task_t * tsk = get_task(it->first);
		if (tsk->type == treasure_task) {
			return true;
		}
		++it;
	}
	return false;
}


/**
 * @brief undertake a task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int undertake_task_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t taskid;
	unpack(body, taskid, idx);
	
	if (is_too_many_going_task(p)) {
		TRACE_LOG("uid=%u, task %u too many", p->id, taskid);
		return send_header_to_player(p, p->waitcmd, cli_err_too_many_task, 1);
	}
	
	const task_t* tsk = get_task(taskid);
	if (!tsk) {
		ERROR_LOG("invalid tskid: uid=%u tsk=%u", p->id, taskid);
		return -1;
	}

	if (tsk->is_outmoded) {
		ERROR_LOG("invalid tskid: uid=%u tsk=%u", p->id, taskid);
		return send_header_to_player(p, p->waitcmd, cli_err_undertake_outmoded_task, 1);
	}
  
	if (is_finished_task(p, taskid) || is_going_task(p, taskid)) {
		TRACE_LOG("uid=%u, task %u undertaken already or finished yet", p->id, taskid);
		return send_header_to_player(p, p->waitcmd, cli_err_reundertake_task, 1);
	}

	if (tsk->vip_limit && !is_vip_player(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_nonvip_operation_forbidden, 1);
	}

	if (tsk->type == treasure_task && get_treasure_task_cnt(p)) {
		ERROR_LOG("%u NO TIMES TODY FOR TREASURE TASK! %u", p->id, taskid);
		return send_header_to_player(p, p->waitcmd, cli_err_no_treasure_task_times, 1);
	}

	if (tsk->type == treasure_task && has_going_treasure_task(p)) {
		ERROR_LOG("%u Now HAS Going TREASURE TASK! %u", p->id, taskid);
		return send_header_to_player(p, p->waitcmd, cli_err_has_going_treasure_task, 1);
	}
	//check_other_limit info
	
	if (tsk->other_limit_id && get_swap_action_times(p, tsk->other_limit_id)) {
		ERROR_LOG("%u has FINISH WEEKLY Team Task! %u", p->id, taskid);
		return send_header_to_player(p, p->waitcmd, cli_err_reundertake_task, 1);
	}	


	add_going_task(p, taskid);
	////////////如果是环任务/////////////////
	if(tsk->type == master_ring_task)
	{
		//环任务由大环和小环组成,大环任务随机生成子环任务
		uint32_t group_id = random_gen_slave_group_id(p, tsk->id);
		add_player_ring_task(p, tsk->id, tsk->type, 0, group_id);
		player_ring_task_data data;
		data.task_id = tsk->id;
		data.task_type = tsk->type;
		data.master_task_id = 0;
		data.task_group_id = group_id;
		db_replace_ring_task_data(p, &data);
	}	
	else if(tsk->type == slave_ring_task)
	{
		add_player_ring_task(p, tsk->id, tsk->type, tsk->master_task_id, 0);
		//modify memory
		player_ring_task_data data;
		data.task_id = tsk->id;
		data.task_type = tsk->type;
		data.master_task_id = tsk->master_task_id; 
		data.task_group_id = 0;
		//modify db
		db_replace_ring_task_data(p, &data);
	}
	return db_set_task_flag(p, p->id, taskid, 1);
}


/**
 * @brief finish a task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int finish_task_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t tsk_id = 0;
	uint32_t req_out_id = 0;
	int idx = 0;

	unpack(body, tsk_id, idx);
	unpack(body, req_out_id, idx);

	if (p->offline_data && p->offline_data->player_start_tm)
	{
		ERROR_LOG("PLAYER %u IS IN OFFLINE MODE CANN'T FINISH TASK", p->id);
		return -1;
	}


	if (if_version_kaixin() && is_btl_time_limited(p)) {
		KDEBUG_LOG(p->id, "FINISH TASK TM LIMIT\t[%u %u %u > %u]", p->id, tsk_id, p->oltoday, battle_time_limit);
		return send_header_to_player(p, p->waitcmd, cli_err_limit_time_btl, 1);
	}

	if (!is_valid_taskid(tsk_id) || !is_valid_outid(req_out_id)) {
		ERROR_LOG("uid=%u, invalid task_id(%u) is or req_out_id(%u)",
				p->id, tsk_id, req_out_id);
		return -1;
	}

	if (is_finished_task(p, tsk_id) || is_canceled_daily_task(p, tsk_id) || !is_going_task(p, tsk_id)) {
		ERROR_LOG("uid=%u, task_id(%u) is just a pre_taken task or finished yet", p->id, tsk_id);
		return send_header_to_player(p, p->waitcmd, cli_err_task_had_finished, 1);
	} 

	const task_t* tsk = get_task(tsk_id);
	if (!tsk ) {
		ERROR_LOG("invalid tskid: uid=%u tsk=%u", p->id, tsk_id);
		return -1;
	}

	ring_task_data* p_data = get_ring_task_mgr()->get_ring_task_data_by_id(tsk_id);
	player_ring_task_data* p_user_data = get_player_ring_task(p, tsk_id);
	//如果是大环任务,检查小环是否全部完成
	if(p_data && p_user_data && p_user_data->task_type == master_ring_task)
	{
		slave_task_group* p_group = p_data->get_task_group_by_id(p_user_data->task_group_id);
		if(p_group != NULL)
		{
			for(uint32_t i=0;  i < p_group->slave_task_ids.size(); i++)
			{
				uint32_t slave_id = p_group->slave_task_ids[i];
				if( ! is_finished_task(p, slave_id) )
				{
					return send_header_to_player(p, p->waitcmd, cli_err_task_had_finished, 1);
				}
			}
		}
	}


	if (tsk->vip_limit && !is_vip_player(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_nonvip_operation_forbidden, 1);
	}

  	if (!check_task_finish(p, tsk)) {
  		ERROR_LOG("task hasn't finished: uid=%u tskid=%u", p->id, tsk_id);
		return -1;
	}

	if (tsk->type == treasure_task && get_treasure_task_cnt(p) ){
		ERROR_LOG("%u HAS NO TIMES FINISH TREASURE TASK %u!", p->id, tsk->id);
		return -1;
	}	
	if (!task_step_over(p, tsk)) {
		ERROR_LOG("%u TASK %u STEP IS NOT FINISH !", p->id, tsk->id);
		return -1;
	}
	TRACE_LOG("finish task %u %u %u", p->id, tsk_id, tsk->id);
	return do_finish_task(p, tsk_id, req_out_id, tsk);
}

/**
 * @brief cancel a task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int cancel_task_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t taskid;

	unpack(body, taskid, idx);
	if (!is_valid_taskid(taskid)) {
		ERROR_LOG("uid=%u, invalid taskid=%u", p->id, taskid);
		return -1;
	}
	
	const task_t* tsk = get_task(taskid);
	if (!tsk) {
		ERROR_LOG("invalid tskid: uid=%u tsk=%u", p->id, taskid);
		return -1;
	}
	if (tsk->type == primary_task) {
		TRACE_LOG("cannot cancel primary task.");
		return send_header_to_player(p, p->waitcmd, cli_err_cancel_primary_task, 1);
	}
	
	if (!is_finished_task(p, taskid) && is_going_task(p, taskid)) {
		KDEBUG_LOG(p->id, "CNCL TASK\t[uid=%u taskid=%u]", p->id, taskid);
		if (is_daily_taskid(taskid)) {
			return db_set_task_flag(p, p->id, taskid, 2); /* DB会删除该任务对应的数据 */
		} else {
			return db_set_task_flag(p, p->id, taskid, 0); /* DB会删除该任务对应的数据 */
		}
	} else {
		ERROR_LOG("uid=%u, task %u not undertaken or finished yet", p->id, taskid);
		return -1;
	}
}

/**
 * @brief undertake a daily task
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int undertake_daily_task_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (is_too_many_going_task(p)) {
		TRACE_LOG("uid=%u, too many", p->id);
		return send_header_to_player(p, p->waitcmd, cli_err_too_many_task, 1);
	}

	// if have going daily task	
	if (is_have_daily_task(p)) {
		TRACE_LOG("uid=%u undertaken already", p->id);
		return send_header_to_player(p, p->waitcmd, cli_err_reundertake_task, 1);
	}
	
	const task_t* tsk = 0;
	uint32_t finished_cnt = get_finished_daily_task_count(p);
	if (finished_cnt < max_nor_daily_task_times) {
		// if num of done task >= max
		if (get_canceled_daily_task_count(p) + finished_cnt >= max_nor_daily_task_times) {
			TRACE_LOG("uid=%u undertaken already", p->id);
			return send_header_to_player(p, p->waitcmd, cli_err_reundertake_task, 1);
		}
		
		//get tsk info
		
		tsk = get_free_daily_task(p);
		if (!tsk) {
			TRACE_LOG("uid=%u undertaken already", p->id);
			return send_header_to_player(p, p->waitcmd, cli_err_no_undertake_task, 1);
		}
	} else if (finished_cnt == max_nor_daily_task_times) {
		uint32_t rand_num = rand() % 100;
		if (rand_num < 30) {
			tsk = get_lucky_daily_task(p);
			if (!tsk) {
				TRACE_LOG("uid=%u undertaken already", p->id);
				return send_header_to_player(p, p->waitcmd, cli_err_no_undertake_task, 1);
			}
			do_stat_log(stat_log_daily_task_lucky_take, 1);
			TRACE_LOG("uid=%u undertak lucky task:%u", p->id, tsk->id);
		} else {
			return send_undertake_daily_rsp(p, 0);
		}
	} else {
		return send_header_to_player(p, p->waitcmd, cli_err_reundertake_task, 1);
	}
	KDEBUG_LOG(p->id, "TAKE DAILY TASK\t[uid=%u taskid=%u]", p->id, tsk->id);

	add_going_task(p, tsk->id);
	TRACE_LOG("going daily tasks[%u]", tsk->id);

	return db_set_task_flag(p, p->id, tsk->id, 1);
}

/**
 * @brief set role double experience time
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int double_exp_action_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (p->battle_grp) {
		return send_header_to_player(p, p->waitcmd, cli_err_double_exp_action, 1);
	}
	if (p->daily_restriction_count_list[double_exp_daily_action - 1] 
		>= g_all_restr[double_exp_daily_action - 1].toplimit) {
		TRACE_LOG("%u %u", p->daily_restriction_count_list[double_exp_daily_action - 1], g_all_restr[double_exp_daily_action - 1].toplimit);
		return send_header_to_player(p, p->waitcmd, cli_err_limit_time_act, 1);
	}	
	
	return db_set_double_exp_data(p, 0);

}
/**
 * @brief set role double experience time when use item
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int double_exp_action_use_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t item_id;

	unpack(body, item_id, idx);

	if (p->battle_grp) {
		return send_header_to_player(p, p->waitcmd, cli_err_double_exp_action, 1);
	}
	if (p->daily_restriction_count_list[double_exp_daily_use_item - 1] 
		>= g_all_restr[double_exp_daily_use_item - 1].toplimit) {
		TRACE_LOG("%u %u", p->daily_restriction_count_list[double_exp_daily_use_item - 1], g_all_restr[double_exp_daily_use_item - 1].toplimit);
		return send_header_to_player(p, p->waitcmd, cli_err_limit_time_act, 1);
	}	
	TRACE_LOG("%u %u", p->daily_restriction_count_list[double_exp_daily_use_item - 1], g_all_restr[double_exp_daily_use_item - 1].toplimit);
	return db_use_item(p, item_id, 1);
}

/**
 * @brief get list of boss killed
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_killed_boss_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return 0;//db_get_killed_boss(p, p->id);
}

//------------------------------------------------------------------
// callback for handling package return from dbproxy
//------------------------------------------------------------------

/**
  * @brief callback for setting a player's task list
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_set_task_flag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
    uint32_t taskid = *reinterpret_cast<uint32_t *>(body);
	uint32_t finish_cnt = get_finished_daily_task_count(p);

	KDEBUG_LOG(p->id, "SET TASK CB\t[%u %u]", taskid, finish_cnt);

	switch (p->waitcmd) {
	case cli_proto_undertake_daily_task:
		{
			do_stat_log_task_info(taskid, p->role_type, 0, 1);
			return send_undertake_daily_rsp(p, taskid);
		}
		break;

	case cli_proto_undertake_task:
		{
			std::map<uint32_t, task_going_t>::iterator it = p->going_tasks_map->find(taskid);
			if (it != p->going_tasks_map->end()) {
				init_task_step(p, &(it->second));
			}
			do_stat_log_task_info(taskid, p->role_type, 0, 1);
			uint32_t group_id = 0;
			player_ring_task_data* p_data = get_player_ring_task(p, taskid);
			if(p_data != NULL && p_data->task_type == master_ring_task)
			{
				group_id = p_data->task_group_id;
				//设置环任务groupid
				std::map<uint32_t, task_going_t>::iterator it = p->going_tasks_map->find(taskid);
				if(it != p->going_tasks_map->end())
				{
					memset(it->second.serv_buf, 0, serv_buf_len);	
					int tmp_idx = 0;
					pack(it->second.serv_buf, group_id, tmp_idx);
					db_set_task_buf(p, taskid, (char*)it->second.serv_buf, serv_buf_len, 1, false);
				}
			}
			return send_undertake_task_rsp(p, taskid, group_id);
		}
		break;

	case cli_proto_finish_task:
		{

			tmp_log_for_task_bug(p, 3);


        	do_stat_log_task_info(taskid, p->role_type, 1, 1);
        	do_stat_log_stage_trials(taskid,p->id);
			if (is_promotions_task(taskid))
			{
				do_stat_log_finish_promotions_tast(p, taskid);
			}
			if (is_daily_taskid(taskid)) 
			{
				do_stat_log(stat_log_daily_task_finish, p->id);
				if (finish_cnt == 5) 
				{
					do_stat_log(stat_log_daily_task_5_finish, 1);
				} 
				else if (finish_cnt == 10) 
				{
					do_stat_log(stat_log_daily_task_10_finish, 1);
				} 
				else if (finish_cnt == 11) 
				{
					do_stat_log(stat_log_daily_task_lucky_finish, 1);
				}
			}

			finish_task_step_over(p, finish_task_set_task_flag);
			
			const task_t * tsk = get_task(taskid);
			if (tsk->id == 3004) {
				db_set_task_flag(p, p->id, taskid, 0, set_flag_without_back);
	            p->finished_tasks_set->erase(taskid); 
			}


			if (tsk->type == treasure_task) {
				uint32_t cur_time = get_now_tv()->tv_sec;
				add_player_ring_task_history(p, taskid, cur_time);
				player_ring_task_history_data* p_history_data = get_player_ring_task_history(p, taskid);
				if(p_history_data)
				{
					db_replace_ring_task_history_data(p, p_history_data);
				}
				db_set_task_flag(p, p->id, taskid, 0, set_flag_without_back);
				p->going_tasks_map->erase(taskid);
	            p->finished_tasks_set->erase(taskid); 
			}

			ring_task_data* p_data = get_ring_task_mgr()->get_ring_task_data_by_id(taskid);
			player_ring_task_data* p_user_data = get_player_ring_task(p, taskid);
			if(p_data && p_user_data && p_user_data->task_type == master_ring_task)//如果是大环任务，该任务和子任务全部删除
			{
				slave_task_group* p_group = p_data->get_task_group_by_id(p_user_data->task_group_id);
				if(p_group != NULL)
				{
					for(uint32_t i=0;  i < p_group->slave_task_ids.size(); i++)
					{
						uint32_t slave_id = p_group->slave_task_ids[i];
						p->going_tasks_map->erase(slave_id);
						p->finished_tasks_set->erase(slave_id); 
						db_set_task_flag(p, p->id, slave_id, 0, set_flag_without_back);
					}
				}
				p->going_tasks_map->erase(taskid);
				p->finished_tasks_set->erase(taskid); 
				db_set_task_flag(p, p->id, taskid, 0, set_flag_without_back);
				
				db_delete_ring_task_data(p, p_user_data->task_id, p_user_data->task_type);
				del_player_ring_task(p, p_user_data->task_id, p_user_data->task_type);
				uint32_t cur_time = time(NULL);
				add_player_ring_task_history(p, taskid, cur_time);
				player_ring_task_history_data* p_history_data = get_player_ring_task_history(p, taskid);
				if(p_history_data)
				{
					db_replace_ring_task_history_data(p, p_history_data);
				}
			}
			if(p_user_data != NULL && p_user_data->task_type == slave_ring_task)//如果是子任务
			{

			}

			if (tsk->other_limit_id) {
				add_swap_action_times(p, tsk->other_limit_id);
			}

			proc_task_achieve_logic(p, tsk->type, tsk->id);
			return 0;
		}
		break;

	case cli_proto_cancel_task:
		{
			do_stat_log_task_info(taskid, p->role_type, 2, 1);
			player_ring_task_data* p_data = get_player_ring_task(p, taskid);
			//放弃大环任务的时候，大环任务下所有的完成和未完成子任务都必须删除
			cancel_task_rsp_to_client(p, id, body, bodylen);
			
			if(p_data != NULL && (p_data->task_type == master_ring_task || p_data->task_type == slave_ring_task))
			{
				//modify db
				db_delete_ring_task_data(p, p_data->task_id, p_data->task_type);
				//modify memory
				del_player_ring_task(p, p_data->task_id, p_data->task_type);
			}
			return 0;
		}
		break;

	
	default:
		ERROR_LOG("unsupported cmd=%u, uid=%u", p->waitcmd, p->id);
		return -1;
	}
	return 0;
}

/**
  * @brief callback for get a player's task flag list
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_going_task_buf_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	int idx = 0;
	uint32_t going_tasks_num = 0;
	unpack_h(body, going_tasks_num, idx);
	if ( going_tasks_num > (max_primary_tasks + max_minor_tasks + max_daily_tasks) ) {
		ERROR_LOG("the number of going tasks error!\t[uid=%u, role_tm=%u, tasks_num=%u]",
								p->id, p->role_tm, going_tasks_num);
		return -1;
	}
	TRACE_LOG("going_tasks_num=[%u]",going_tasks_num);
	
	CHECK_VAL_EQ(bodylen, 4 + sizeof(task_going_t) * going_tasks_num);
	
	p->going_tasks_map->clear();
	p->minior_tasks_num = 0;
	for ( uint32_t i = 0; i < going_tasks_num; i++) {
			task_going_t task_info;
			unpack_h(body, task_info, idx);
			p->going_tasks_map->insert(std::map<uint32_t, task_going_t>::value_type(task_info.id, task_info));
			if ( get_task_type(task_info.id) == minor_task || get_task_type(task_info.id) == active_task ) {
				p->minior_tasks_num++;
			}
//			p->player_task_steps->init_task_step(p, task_info);
			TRACE_LOG("going_task_info.id=[%u]",task_info.id);
	}
	
	return 0;
}

/**
  * @brief callback for get a player's done tasks list
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_done_task_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	int idx = 0;
	uint32_t done_tasks_num = 0;
	unpack_h(body, done_tasks_num, idx);
	TRACE_LOG("done_task_num=[%u]",done_tasks_num);
	
	CHECK_VAL_EQ(bodylen, 4 + 4 * done_tasks_num);
	
	p->finished_tasks_set->clear();
	for ( uint32_t i = 0; i < done_tasks_num; i++) {
		uint32_t taskid = 0;
		unpack_h(body, taskid, idx);
		TRACE_LOG("taskid=[%u]",taskid);
		p->finished_tasks_set->insert(taskid);
	}
	
	return 0;
}

int del_clear_done_task(player_t* p)
{
    int idx = 0;
    uint32_t task_num = 0;
    idx += 4;
    std::set<uint32_t>::iterator sit = p->finished_tasks_set->begin();
    for (; sit != p->finished_tasks_set->end(); ) {
        if (is_need_clear_task_id(*sit) || is_outmoded_ring_task(*sit)) {
            TRACE_LOG("del clear task ---> %u", *sit);
            pack_h(dbpkgbuf, *sit, idx);
            task_num++;
            p->finished_tasks_set->erase(sit++);
        } else {
            sit++;
        }
    }

    if (task_num > 0) {
        return send_request_to_db(0, p->id, p->role_tm, dbproto_del_outmoded_task, dbpkgbuf, idx);
    }
    return 0;
}

int del_clear_going_task(player_t* p)
{
    int idx = 0;
    uint32_t task_num = 0;
    idx += 4;
    std::map<uint32_t, task_going_t>::iterator mit = p->going_tasks_map->begin();
    for (; mit != p->going_tasks_map->end(); ) {
        if (is_need_clear_task_id(mit->first) || is_outmoded_ring_task(mit->first)) {
            TRACE_LOG("del clear task ---> %u", mit->first);
            pack_h(dbpkgbuf, mit->first, idx);
            task_num++;
            p->going_tasks_map->erase(mit++);
        } else {
            mit++;
        }
    }

    int tmp_idx = 0;
    pack_h(dbpkgbuf, task_num, tmp_idx);
    
    if (task_num > 0) {
        return send_request_to_db(0, p->id, p->role_tm, dbproto_del_outmoded_task, dbpkgbuf, idx);
    }
    return 0;
}

int del_clear_task_interface(player_t* p)
{
    int idx = 0;
    uint32_t task_num = 0;
    idx += 4;
    std::set<uint32_t>::iterator sit = p->finished_tasks_set->begin();
    for (; sit != p->finished_tasks_set->end(); ) {
        if (is_need_clear_task_id(*sit)) {
            TRACE_LOG("del clear task ---> %u", *sit);
            pack_h(dbpkgbuf, *sit, idx);
            task_num++;
            p->finished_tasks_set->erase(sit++);
        } else {
            sit++;
        }
    }
    std::map<uint32_t, task_going_t>::iterator mit = p->going_tasks_map->begin();
    for (; mit != p->going_tasks_map->end(); ) {
        if (is_need_clear_task_id(mit->first)) {
            TRACE_LOG("del clear task ---> %u", mit->first);
            pack_h(dbpkgbuf, mit->first, idx);
            task_num++;
            p->going_tasks_map->erase(mit++);
        } else {
            mit++;
        }
    }

    int tmp_idx = 0;
    pack_h(dbpkgbuf, task_num, tmp_idx);
    
    if (task_num > 0) {
        return send_request_to_db(0, p->id, p->role_tm, dbproto_del_outmoded_task, dbpkgbuf, idx);
    }
    return 0;
}

 /**
   * @brief callback for get a player's all tasks list
   * @param p the requester
   * @param uid id of the requestee
   * @param body body of the returning package
   * @param bodylen length of body
   * @param ret errno returned from dbproxy
   * @return 0 on success, -1 and p will be deleted
   */
 int db_get_all_task_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
 {
	 CHECK_DBERR(p, ret);
	 
	 int idx = 0;
	 uint32_t done_tasks_num = 0;
	 uint32_t going_tasks_num = 0;
	 uint32_t cancel_daily_tasks_num = 0;

	 unpack_h(body, done_tasks_num, idx);
	 unpack_h(body, going_tasks_num, idx);
	 unpack_h(body, cancel_daily_tasks_num, idx);

	 TRACE_LOG("done_task_num=[%u] going_task_num=[%u]", done_tasks_num, going_tasks_num);
	 
	 CHECK_VAL_EQ(bodylen, 12 + 4 * done_tasks_num + 64 * going_tasks_num + 4 * cancel_daily_tasks_num);
	 
	 p->finished_tasks_set->clear();
	 for ( uint32_t i = 0; i < done_tasks_num; i++) {
		 uint32_t taskid = 0;
		 unpack_h(body, taskid, idx);
         TRACE_LOG("taskid=[%u]",taskid);
		 p->finished_tasks_set->insert(taskid);
	 }

	 if ( going_tasks_num > (max_primary_tasks + max_minor_tasks + max_daily_tasks) ) {
		 ERROR_LOG("the number of going tasks error!\t[uid=%u, role_tm=%u, tasks_num=%u]",
								 p->id, p->role_tm, going_tasks_num);
		 return -1;
	 }
	 
	 p->going_tasks_map->clear();
	 p->minior_tasks_num = 0;
	 for ( uint32_t i = 0; i < going_tasks_num; i++) {
			 task_going_t task_info;
			 unpack_h(body, task_info, idx);
             p->going_tasks_map->insert(std::map<uint32_t, task_going_t>::value_type(task_info.id, task_info));
			 if ( get_task_type(task_info.id) == minor_task || get_task_type(task_info.id) == active_task) {
				 p->minior_tasks_num++;
			 }
			 TRACE_LOG("going_task_info.id=[%u]", task_info.id);
	 }
	 
	 p->canceled_tasks_set->clear();
	 for ( uint32_t i = 0; i < cancel_daily_tasks_num; i++) {
		 uint32_t taskid = 0;
		 unpack_h(body, taskid, idx);
		 TRACE_LOG("taskid=[%u]",taskid);
		 p->canceled_tasks_set->insert(taskid);
	 }
	 
     del_clear_task_interface(p);
	 if ( p->waitcmd == cli_proto_login ) {
         return send_request_to_db(p, p->id, p->role_tm, dbproto_get_packs, 0, 0);
	 }

	 return 0;
 }

 /**
  * @brief callback for set a player's task buf of taskid
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_set_task_buf_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	int idx = 0;
	uint32_t buf_type = 0;
	unpack_h(body, buf_type, idx);
	if (buf_type == 2) {//client_buf
		return send_header_to_player(p, p->waitcmd, 0, 1);
	}
	
	return 0;
}

 /**
  * @brief callback for set a player's boss list killed
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_killed_boss_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	get_killed_boss_rsp_t *p_info = reinterpret_cast<get_killed_boss_rsp_t*>(body);
	TRACE_LOG("boss_cnt=[%u]", p_info->cnt);
	
	CHECK_VAL_EQ(bodylen, 4 + 4 * p_info->cnt);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p_info->cnt, idx);
	for ( uint32_t i = 0; i < p_info->cnt; i++) {
		uint32_t boss_id = p_info->boss_id[i];
		pack(pkgbuf, boss_id, idx);
		TRACE_LOG("get_killed_boss:uid=[%u], boss_id=[%u]",
				p->id, boss_id);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int check_task_extra_logic_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t puzzle_id = 0;
	uint32_t answer = 0;
	unpack(body, puzzle_id, idx);
	unpack(body, answer, idx);
	intelligence_test_t * puzzle = get_intelligence_test_info_by_id(puzzle_id);
	if (puzzle) {
		idx = sizeof(cli_proto_t);
		pack(pkgbuf, puzzle_id, idx);
		pack(pkgbuf, answer, idx);
		pack(pkgbuf, puzzle->right_answer, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	} else {
		ERROR_LOG("%u NO EXIST PUZZLE %u TEST FOR TASK", p->id, puzzle_id);
		return -1;
	}
}


void update_task_step_info_by_kill_monster(player_t * p, uint32_t killed_monster)
{
//	ERROR_LOG("TASK STEP UPDATE %u KILL MONSTER %u", p->id, killed_monster);
 	update_step_by_kill_monster(p, killed_monster);
}

void update_task_step_info_by_pass_stage(player_t *p, uint32_t stage_id) 
{
//	ERROR_LOG("TASK STEP UPDATE %u PASS STAGE %u", p->id, stage_id);
 	update_step_by_pass_stage(p, stage_id);
}



