extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/dataformatter/bin_str.h>
}

#include <libtaomee++/conf_parser/xmlparser.hpp>

#include "pea_common.hpp"
#include "player.hpp"
#include "map.hpp"
#include "task.hpp"
#include "db_task.hpp"
#include "utils.hpp"
#include "task_funcs.hpp"

using namespace taomee;

map<uint32_t, tsk_fn_t*> tsk_fn_map;
tsk_able_map_t obtain_able_fn_map;
tsk_able_map_t cancel_able_fn_map;
tsk_able_map_t finish_able_fn_map;
tsk_setbuff_map_t setbuff_fn_map;

/*任务MAP*/
std::map<uint32_t, task_info_t*> task_config_map;

/**
 * @brief 分配一个task_info_t的空间
 * @return 分配的地址
 */
task_info_t *alloc_a_task_info()
{
    task_info_t *p = new task_info_t();
    p->steps = new step_vec_t();
    p->steps->clear();
    return p;
}
/**
 * @brief 释放一个task_info_t的空间
 */
void dealloc_a_task_info(task_info_t *task_info)
{
    delete task_info->steps;
    delete task_info;
}

/**
 * @brief 单个任务项的读取，包括该任务的每一个步骤
 * @para step_node，步骤结点，task_id 任务ID，task_type,上层读取的任务类型(主线，支线等)
 * @return NULL 失败 否则返回新任务项的指针
 */
task_info_t *process_single_task_conf(xmlNodePtr step_node, uint32_t task_id)
{
    task_info_t *atask = alloc_a_task_info();
    task_step_t astep;
    uint32_t step_type;
    uint32_t item_arr[2];
    uint32_t cond;
    uint32_t step_pre_cond[max_step_pre_condition];
    uint32_t step_pre_cond_type;
    uint32_t total_step_buf_len = 0;
    while (step_node) {
        get_xml_prop_def(step_type, step_node, "type", 0);
        if (step_type <= task_step_type_begin || step_type >= task_step_type_end) {
            dealloc_a_task_info(atask);
            ERROR_TLOG("invalid task step type %u(%u-%u) task id=%u",
                    step_type, task_step_type_begin, task_step_type_end, task_id);
            return NULL;
        }
        astep.step_type = step_type;
        
        //buffer长度保护,如果一个任务的步骤太多 可能导致32个字节
        //的buffer(除开8字节头部实际只有24)长度超出
        switch (step_type) {
        case task_step_type_1 :
            total_step_buf_len += 1;
            break;
        case task_step_type_2 :
            total_step_buf_len += 2;
            break;
        case task_step_type_3 :
            total_step_buf_len += 2;
            break;
        case task_step_type_4 :
            total_step_buf_len += 4;
            break;
        case task_step_type_5 :
            total_step_buf_len += 4;
            break;
        }
        if (total_step_buf_len > MAX_TASK_BUFF_LEN - sizeof(task_buffer_head_t)) {
            dealloc_a_task_info(atask);
            ERROR_TLOG("too much task step. 24Bytes buffer is not enough");
            return NULL;
        }

        int count;
        switch (step_type) {
        case task_step_type_1 :
            astep.condition.val = 1;
            break;
        case task_step_type_2 :
            count = get_xml_prop_arr_def(item_arr, step_node, "condition", 0);
            if (count != 2) {
                dealloc_a_task_info(atask);
                ERROR_TLOG("invalid task step condition type %u, t_paras %u[2 needed] id= %u",
                        task_step_type_2, count, task_id);
                return NULL;
            }
            astep.condition.key = item_arr[0];
            astep.condition.val = item_arr[1];
            break;
        default :
            get_xml_prop_def(cond, step_node, "condition", 0);
            if (cond == 0) {
                dealloc_a_task_info(atask);
                ERROR_TLOG("invalid task step condition type %u, para_v 0[must gt 0] id=%u",
                        step_type, task_id);
                return NULL;
            }
            astep.condition.val = cond;
            break;
        }

        memset(step_pre_cond, 0, sizeof(step_pre_cond));
        get_xml_prop_arr_def(step_pre_cond, step_node, "pre", 0);
        memcpy(astep.pre_cond, step_pre_cond, sizeof(astep.pre_cond));
        
        get_xml_prop_def(step_pre_cond_type, step_node, "isAnd", 0);
        if (step_pre_cond_type != pre_condition_type_or 
            && step_pre_cond_type != pre_condition_type_and) {
            dealloc_a_task_info(atask);
            ERROR_TLOG("invalid task pre_cond_type %u[%u-%u]",
                    step_pre_cond_type, pre_condition_type_or, pre_condition_type_and);
            return NULL;
        }
        astep.pre_cond_type = step_pre_cond_type;
        
        atask->steps->push_back(astep);
        step_node = step_node->next;
    }
    return atask;
}

/**
 * @brief 初始化从task.xml中读取所有的任务 遇到错误打印错误日志并立即返回
 * @para task_node, XML头结点
 * @return 0 成功，-1 失败，并由load_xmlconf抛出异常。
 */
int init_task_config_data(xmlNodePtr task_node)
{
    task_config_map.clear();
    task_node = task_node->xmlChildrenNode;
    uint32_t id;
    uint32_t task_type;
    uint32_t pre_cond[max_task_pre_condition];
    uint32_t pre_cond_type;
    uint32_t need_level;
    uint32_t bind_map_id;
    uint32_t prize_id;
    task_info_t *task;
    while(task_node) {
        if (xmlStrEqual(task_node->name, reinterpret_cast<const xmlChar*>("task"))) {
            get_xml_prop_def(id, task_node, "id", 0);
            if (id == 0) {
                RT_ERROR_TLOG(-1, "invalid task id[null or 0]");
            }
            if (task_config_map.find(id) != task_config_map.end()) {
                RT_ERROR_TLOG(-1, "duplicate task id! %u already exist", id);
            }
            get_xml_prop_def(task_type, task_node, "type", 0);
            if (task_type <= task_type_begin && task_type >= task_type_end) {
                RT_ERROR_TLOG(-1, "invalid task type %u[%u-%u], id=%u", 
                        task_type, task_type_begin, task_type_end, id);
            }
            memset(pre_cond, 0, sizeof(pre_cond));
            get_xml_prop_arr_def(pre_cond, task_node, "pre", 0);
            get_xml_prop_def(pre_cond_type, task_node, "isAnd", 0);
            if (pre_cond_type != pre_condition_type_or 
                && pre_cond_type != pre_condition_type_and) {
                RT_ERROR_TLOG(-1, "invalid task pre_cond_type %u[%u-%u]",
                        pre_cond_type, pre_condition_type_or, pre_condition_type_and);
            }
            get_xml_prop_def(need_level, task_node, "need_level", 0);
            get_xml_prop_def(bind_map_id, task_node, "bind_map_id", 10);
            if (get_map(bind_map_id) == NULL) {
                RT_ERROR_TLOG(-1, "bind map id %u doesn't exist for task %u", bind_map_id, id);
            }
            
            get_xml_prop_def(prize_id, task_node, "prize_id", 0);

            if ((task = process_single_task_conf(task_node->xmlChildrenNode, id)) == NULL) {
                RT_ERROR_TLOG(-1, "process task id %u failed", id);
            }
            
            task->task_id = id;
            task->task_type = task_type;
            memcpy(task->pre_cond, pre_cond, sizeof(task->pre_cond));
            task->pre_cond_type = pre_cond_type;
            task->need_level = need_level;
            task->bind_map_id = bind_map_id;
            task->prize_id = prize_id;

            task_config_map[id] = task;
        }
        task_node = task_node->next;
    }
    //检查任务的前置任务 和步骤的前置步骤的存在性
    std::map<uint32_t, task_info_t*>::iterator it;
    std::vector<task_step_t>::iterator sit;
    uint32_t tsk_id, step;
    for (it = task_config_map.begin(); it != task_config_map.end(); it++) {
        //任务
        for (int i = 0; i < max_task_pre_condition; i++) {
            tsk_id = it->second->pre_cond[i];
            if (tsk_id == 0) {
                break;
            }
            if (task_config_map.find(tsk_id) == task_config_map.end()) {
                RT_ERROR_TLOG(-1, "task %u's pre task %u is not exist", 
                        it->first, tsk_id);
            }
        }
        //步骤
        int k = 0;//记录当前步骤
        for (sit = it->second->steps->begin(); sit != it->second->steps->end(); sit++) {
            k++;
            for (int j = 0; j < max_step_pre_condition; j++) {
                step = sit->pre_cond[j];
                if (step == 0) {
                    break;
                }
                if (step > it->second->steps->size()) {
                    RT_ERROR_TLOG(-1, "task %u's step %u's pre step %u is not exist",
                            it->first, k, step);
                }
            }//for step's every pre
        }// for every step
    }//for every task

    return 0;
}

void final_task_config_data()
{
    std::map<uint32_t, task_info_t*>::iterator iter;
    iter = task_config_map.begin();
    for (; iter != task_config_map.end(); iter++) {
        task_info_t *p = iter->second;
        dealloc_a_task_info(p);        
    }
    task_config_map.clear();
}

//=========================task buffer helper function==============

/*判断某用户的任务id是否完成*/
bool is_task_finished(const player_t *p, uint32_t task_id)
{
    tskiter_t it = p->tskmap->find(task_id);
    return (it->second->flag == task_finished);
}

/**
 * @brief 给定步骤和步骤的位置指针,判定步骤是否完成
 * @para step_info,步骤描述结构体,buf步骤的位置
 * @return true 满足 false 不满足
 */
bool step_finished(task_step_t *step_info, const char *buf)
{
    uint32_t type = step_info->step_type;
    uint32_t val = step_info->condition.val;
    switch(type) {
        case task_step_type_1 :
            if (buff_8_eq_val(buf, 1) == false) {//01任务条件没有达到
                return false; 
            }
            break;
        case task_step_type_2 :
            if (buff_16_ge_val(buf, val) == false) {//物品不足
                return false;
            }
            break;
        case task_step_type_3 :
            if (buff_16_ge_val(buf, val) == false) {//等级不够
                return false;
            }
            break;
        case task_step_type_4 :
            if (buff_32_ge_val(buf, val) == false) {//经验不足
                return false;
            }
            break;
        case task_step_type_5 :
            if (buff_32_ge_val(buf, val) == false) {//钱不够
                return false;
            }
            break;
        default :
            return false;
            break;
    }//switch

    return true;
}

/**
 * @brief 根据任务ID 查询到对应的任务结构体指针
 * @para task_id 任务ID
 * @return 如果找到则返回对应的指针，否则返回NULL
 */
task_info_t *get_task_conf_ptr(uint32_t task_id)
{
    std::map<uint32_t, task_info_t*>::iterator conf_it;
    conf_it = task_config_map.find(task_id);
    if (conf_it != task_config_map.end())
        return conf_it->second;
    return NULL;
}

/**
 * @brief 判断step是否某任务的合法step
 * @para task_conf_ptr 任务指针 step 任务步骤
 * @return true or false
 */
bool is_valid_step(task_info_t *task_conf_ptr, uint32_t step)
{
    if (step > 0 && step <= task_conf_ptr->steps->size())
        return true;
    return false;
}

/**
 * @brief 查找某任务的第step步骤的步骤类型
 * @para task_conf_ptr,任务指针 step 任务步骤
 * @return 正常返回step_type 如果不是合法的step则返回0
 */
uint32_t get_step_type(task_info_t *task_conf_ptr, uint32_t step)
{
    if (!is_valid_step(task_conf_ptr, step)) {
        return 0;
    }
    return (*(task_conf_ptr->steps))[step-1].step_type;
}

/*根据buf的当前位置 以及步骤的类型更新buf到新的位置*/
char *increase_buf_pos(char *buf, uint32_t step_type)
{
    char *buf_ptr = buf;
    switch(step_type) {
    case task_step_type_1 :
        buf_ptr += 1;//uint8_t 1个字节
        break;
    case task_step_type_2 :
        buf_ptr += 2; //uint16_t 两个字节
        break;
    case task_step_type_3 :
        buf_ptr += 2; //uint16_t 两个字节
        break;
    case task_step_type_4 :
        buf_ptr += 4; //uint32_t 四个字节
        break;
    case task_step_type_5 :
        buf_ptr += 4; //uint32_t 四个字节
        break;
    default :
        return NULL;
    }//switch

    return buf_ptr;
}

/**
 * @brief 检验task_id对应的条件在p的buff中是否都满足
 * @para p 玩家 task_id任务ID
 * @return 0 表示都满足条件 否则返回相应的错误码
 */
int task_buffer_check(const player_t *p, uint32_t task_id)
{
    //能够到这里,task_id都是有效的，且user接过该任务.
    tskiter_t it = p->tskmap->find(task_id);
    task_t *task = it->second;

    char *buf_ptr = task->buff + sizeof(task_buffer_head_t);

    std::map<uint32_t, task_info_t*>::iterator conf_it;
    conf_it = task_config_map.find(task_id);
    task_info_t *task_conf_info = conf_it->second;
    step_vec_t *step_vec = task_conf_info->steps;

    uint32_t type;
    for (size_t i = 0; i != step_vec->size(); i++) {
        if (!step_finished(&((*step_vec)[i]), buf_ptr))
            return task_err_unfinished;
        
        type = (*step_vec)[i].step_type;
        buf_ptr = increase_buf_pos(buf_ptr, type);
        if (buf_ptr == NULL)
            return task_err_step_type_invalid;
    }//for
    return 0; //buffer的内容完全满足该任务对应的各步骤需要的条件
}

/**
 * @brief 根据任务id以及任务的步骤得到该步骤所在的buf的位置和步骤的类型
 * @para buf任务buf跳过头部8字节, task_id 任务id step 任务步骤
 * @return 正确返回位置 错误返回NULL
 */
char *task_locate_buffer_pos(const char *buf, uint32_t task_id, uint32_t step)
{
    std::map<uint32_t, task_info_t*>::iterator conf_it;
    conf_it = task_config_map.find(task_id);
    task_info_t *task_info = conf_it->second;
    step_vec_t *step_vec = task_info->steps;
    
    char *ret_buf = (char*)buf;
    uint32_t type;
    for (size_t i = 0; i != (step - 1); i++) {
        type = (*step_vec)[i].step_type;
        ret_buf = increase_buf_pos(ret_buf, type);
        if (ret_buf == NULL)
            return NULL;
     }
    return ret_buf;
}

/**
 * @brief 更新任务的buffer
 * @para p player, buf_ptr, buffer要更新的字段指针,step_type更新步骤的类型
 * @return 0更新成功，否则返回错误码步骤类型不对
 */
int task_update_buffer_pos(player_t *p, char *buf_ptr, uint32_t step_type)
{
    switch (step_type) {
    case task_step_type_1 :
        *((uint8_t*)buf_ptr) = 1;
        break;
    case task_step_type_2 :
        (*((uint16_t*)buf_ptr))++;
        break;
    case task_step_type_3 :
        (*((uint16_t*)buf_ptr)) = (uint16_t)p->get_player_attr_value(OBJ_ATTR_LEVEL);
        break;
    case task_step_type_4 :
        *((uint32_t*)buf_ptr) = p->get_player_attr_value(OBJ_ATTR_EXP);
        break;
    case task_step_type_5 :
        *((uint32_t*)buf_ptr) = p->get_player_attr_value(OBJ_ATTR_GOLD);
        break;
    default :
        return task_err_step_type_invalid;
    }
    return 0; 
}

//==========================================================

//=======task function for every task|start=
/* 注意: 所有使能函数和设置的返回值:
 * 0 - 可以/正常;
 * !0: 错误码(没加任务错误码基值);
 */

//common
int obtain_able_fn_common(const player_t *p, uint32_t task_id)
{
    task_info_t *task_conf_ptr = get_task_conf_ptr(task_id);
    if (task_conf_ptr == NULL) {
        return task_err_invalid_tskid; 
    }
    
    if (task_conf_ptr->pre_cond[0] == 0) {
        return 0;//没有前置任务条件
    }

    if (((player_t*)p)->get_player_attr_value(OBJ_ATTR_LEVEL) < task_conf_ptr->need_level) {
        return task_err_higher_level_required;//等级不达标
    }

    if (((player_t*)p)->get_cur_map_id() != task_conf_ptr->bind_map_id) {
        return task_err_at_wrong_map;//玩家所在地图ID不正确
    }

    if (task_conf_ptr->pre_cond_type == pre_condition_type_or) {
        for (int i = 0; i < max_task_pre_condition; i++) {
            if (task_conf_ptr->pre_cond[i] == 0)
                //or情况下已经判断到最后一个条件，都没有完成
                return task_err_lack_pre_task;
            if (is_task_finished(p, task_conf_ptr->pre_cond[i]))
                return 0;
        }
    } else {//与条件
        for (int i = 0; i < max_task_pre_condition; i++) {
            if (task_conf_ptr->pre_cond[i] == 0)
                return 0;//已经判断到最后一个条件了都满足
            if (!is_task_finished(p, task_conf_ptr->pre_cond[i]))
                return task_err_lack_pre_task;
        }
    }
	return 0;
}
int cancel_able_fn_common(const player_t *p, uint32_t task_id)
{
	return 0;
}
int finish_able_fn_common(const player_t *p, uint32_t task_id)
{
    return task_buffer_check(p, task_id);
}
int setbuff_fn_common(const player_t *p, uint32_t task_id, uint32_t step, uint32_t setter,
		bool &need_update_db, char *buff, int bufflen)
{
    task_info_t *task_conf_ptr = get_task_conf_ptr(task_id);
    if (task_conf_ptr == NULL) {
        return task_err_invalid_tskid; 
    }

    uint32_t step_type = get_step_type(task_conf_ptr, step);
    if (step_type == 0) {
        return task_err_step_invalid;
    }

    /*涉及到量值增加的buffer设置,不允许client来设置*/
    if (step_type == task_step_type_2 && setter == tsk_setter_client) {
        return task_err_setter_noauth;
    }
    
    step_vec_t *step_vec = task_conf_ptr->steps;
    task_step_t *step_conf = &((*step_vec)[step-1]);
    char *buf_ptr = task_locate_buffer_pos(buff + sizeof(task_buffer_head_t),
                        task_id, step);    	
    if (step_finished(step_conf, buf_ptr)) {//已经设置过了没必要更新DB
        need_update_db = false;
        return task_err_step_already_finished;
    }

    //判断步骤的前置步骤是否满足
    if (step_conf->pre_cond[0] != 0) {//有前置步骤 则需要判断
        if (step_conf->pre_cond_type == pre_condition_type_or) {
            for (int i = 0; i < max_step_pre_condition; i++) {
                if (step_conf->pre_cond[i] == 0)
                    //or情况下已经判断到最后一个条件，都没有完成
                    return task_err_lack_pre_step;
                buf_ptr = task_locate_buffer_pos(buff + sizeof(task_buffer_head_t),
                        task_id, step_conf->pre_cond[i]);    	
                if (step_finished(step_conf, buf_ptr))
                    break;
            }   
        } else {
            for (int i = 0; i < max_step_pre_condition; i++) {
                if (step_conf->pre_cond[i] == 0)
                    //and情况下已经判断到最后一个条件，都完成
                    break;
                buf_ptr = task_locate_buffer_pos(buff + sizeof(task_buffer_head_t),
                        task_id, step_conf->pre_cond[i]);    	
                if (!step_finished(step_conf, buf_ptr))
                    return task_err_lack_pre_step;
            }   
        }
    }

    //满足
    buf_ptr = task_locate_buffer_pos(buff + sizeof(task_buffer_head_t), task_id, step);    	
    if (!buf_ptr) {
        return task_err_step_type_invalid;
    }
    return task_update_buffer_pos((player_t*)p, buf_ptr, step_type);
}

//=======task function for every task|end====

//==============FUNC REG MACROS=================
#define REG_OBTAIN_FUNC(tskid_) \
	do { \
		obtain_able_fn_map[tskid_] = obtain_able_fn_ ## tskid_; \
	} while(0)

#define REG_CANCEL_FUNC(tskid_) \
	do { \
		cancel_able_fn_map[tskid_] = cancel_able_fn_ ## tskid_; \
	} while(0)

#define REG_FINISH_FUNC(tskid_) \
	do { \
		finish_able_fn_map[tskid_] = finish_able_fn_ ## tskid_; \
	} while(0)

//==================COMMON FUNC REG MACROS================
#define REG_SETBUFF_FUNC_COMMON(tskid_) \
	do { \
		setbuff_fn_map[tskid_] = setbuff_fn_common; \
	} while(0)

#define REG_OBTAIN_FUNC_COMMON(tskid_) \
	do { \
		obtain_able_fn_map[tskid_] = obtain_able_fn_common; \
	} while(0)

#define REG_CANCEL_FUNC_COMMON(tskid_) \
	do { \
		cancel_able_fn_map[tskid_] = cancel_able_fn_common; \
	} while(0)

#define REG_FINISH_FUNC_COMMON(tskid_) \
	do { \
		finish_able_fn_map[tskid_] = finish_able_fn_common; \
	} while(0)

#define REG_SETBUFF_FUNC_COMMON(tskid_) \
	do { \
		setbuff_fn_map[tskid_] = setbuff_fn_common; \
	} while(0)


void init_all_task_fn_map(void)
{
    obtain_able_fn_map.clear();
    cancel_able_fn_map.clear();
    finish_able_fn_map.clear();
    setbuff_fn_map.clear();

    std::map<uint32_t, task_info_t *>::iterator it;
    for (it = task_config_map.begin(); it != task_config_map.end(); it++) {
        REG_OBTAIN_FUNC_COMMON(it->first);
        REG_CANCEL_FUNC_COMMON(it->first);
        REG_FINISH_FUNC_COMMON(it->first);
        REG_SETBUFF_FUNC_COMMON(it->first);
    }

    /*TODO (singku) 如果有任务需要单独的able及setbuff函数的,需要自己完成并注册函数
     * 在这里重新注册，会替换掉之前的COMMON注册
     */
}

void init_obtain_able_fn_map(void)
{
	obtain_able_fn_map.clear();
}

void init_cancel_able_fn_map(void)
{
	cancel_able_fn_map.clear();
}

void init_finish_able_fn_map(void)
{
	finish_able_fn_map.clear();
}

void init_setbuff_fn_map(void)
{
	setbuff_fn_map.clear();
}

void final_obtain_able_fn_map(void)
{
	obtain_able_fn_map.clear();
}
void final_cancel_able_fn_map(void)
{
	cancel_able_fn_map.clear();
}
void final_finish_able_fn_map(void)
{
	finish_able_fn_map.clear();
}
void final_setbuff_fn_map(void)
{
	setbuff_fn_map.clear();
}


/** @brief 初始化各种tsk处理函数
 * return 0: 成功, -1: 失败 (错误日志) */
int init_task_fn_map(void /* TODO(zog): task_config */)
{
    /*!OLD CODE(singku) 使用下面的统一初始化函数注册所有任务的处理函数*/
#if 0
	init_obtain_able_fn_map();
	init_cancel_able_fn_map();
	init_finish_able_fn_map();
	init_setbuff_fn_map();
#endif

    /*NEW METHOD TO KNOW(singku)*/
    init_all_task_fn_map();

    std::map<uint32_t, task_info_t *>::iterator it;
    uint32_t tskid;
	for (it = task_config_map.begin(); it != task_config_map.end(); it++) {
        tskid = it->first;
		
        if (tsk_fn_map.find(tskid) != tsk_fn_map.end()) {
            ERROR_TLOG("dup tskid=%u", tskid);
            return -1;
		}
		tsk_fn_t *tsk_fn = reinterpret_cast<tsk_fn_t*>(malloc(sizeof(tsk_fn_t)));
		tsk_fn->obtain_able = obtain_able_fn_map[tskid];
		tsk_fn->cancel_able = cancel_able_fn_map[tskid];
		tsk_fn->finish_able = finish_able_fn_map[tskid];
		tsk_fn->setbuff = setbuff_fn_map[tskid];
		tsk_fn_map.insert(make_pair(tskid, tsk_fn));
	}

	return 0;
}

void final_task_fn_map(void)
{
	tsk_fn_iter_t iter = tsk_fn_map.begin();
	for (; iter != tsk_fn_map.end(); iter++) {
		tsk_fn_t *tsk_fn = iter->second;
		free(tsk_fn);
	}
	tsk_fn_map.clear();

	final_obtain_able_fn_map();
	final_cancel_able_fn_map();
	final_finish_able_fn_map();
	final_setbuff_fn_map();
}

/** @brief 为玩家p 设置 tskid 的 buff 中的第 step,
 * 注意:
 * (1) 是否能设置buff的这一步, 在具体的设置函数中判断, 并返回错误码表示成功与否;
 * (2) step 对不同的任务有不同的含义, 同时对应buff中的不同的设置点;
 * (3) 保证buff强一致性的更新流程参考 player_t::buff_cache 的注释说明;
 *
 * tskid 要设置 buff 的任务id
 * step 要设置tskid任务的第几步
 * setter 设置者 (0: server, 1: client)
 * need_update_db 是否需要更新db:
 * 		true: 需要(当返回成功时);
 * 		false: 不需要(可能没注册设置函数);
 * 注意: 只有当 return 返回 0 (成功) 时, 这个值才有意义;
 * return 0: 成功, !0: 错误码 (没加上 TASK_ERRCODE_BASE 的值)
 */
int task_setbuff_iface(player_t *p, uint32_t tskid, uint32_t step,
		uint32_t setter, bool &need_update_db)
{
	if (tsk_unlikely(step == 0)) {
		/* step 从 1 开始计数 */
		return task_err_step_invalid;
	}

	if (tsk_unlikely(!is_valid_setter(setter))) {
		/* 不支持该 setter */
		return task_err_setter_noauth;
	}

	tskiter_t tsk_iter = p->tskmap->find(tskid);
	if (tsk_iter == p->tskmap->end()) {
		/* p 没有接 tskid 任务 */
		return task_err_untaken;
	}
	task_t *tsk = tsk_iter->second;

	tsk_fn_iter_t tsk_fn_iter = tsk_fn_map.find(tskid);
	if (tsk_fn_iter == tsk_fn_map.end()) {
		/* tskid 没有注册任何处理函数 */
		need_update_db = false;
		TRACE_TLOG("task_setbuff_iface(no setfn), tskid=%u, step=%u, setter=%u, u=%u",
				tskid, step, setter, p->id);
		return 0;
	}
	tsk_fn_t *tsk_fn = tsk_fn_iter->second;

	if (tsk_fn && tsk_fn->setbuff) {
		/*
		 * 到此, 通常需要更新db了, 因此在此做个保护,
		 * 防止设置函数忘记修改 need_update_db;
		 * 当设置函数发现确实没必要更新db, 可以把 need_update_db 设置成 false;
		*/
		need_update_db = true;
		memcpy(p->buff_cache, tsk->buff, sizeof(p->buff_cache));
		return tsk_fn->setbuff(p, tskid, step, setter,
				need_update_db, p->buff_cache, sizeof(p->buff_cache));
	}

	/* 不需要设置buff */
	need_update_db = false;
	return 0;
}

/** @brief obtain/cancel/finish 使能判断
 * return 0: 成功, !0: 错误码 (没加上 TASK_ERRCODE_BASE 的值)
 */
int task_able_iface(const player_t *p, uint32_t tskid, uint32_t op)
{
	tsk_fn_iter_t iter = tsk_fn_map.find(tskid);
	if (iter == tsk_fn_map.end()) {
		/* tskid 没有注册任何处理函数 */
		return 0;
	}

	tsk_fn_t *tsk_fn = iter->second;
	able_fn_t fn;
	switch (op) {
	case tsk_op_obtain:
		fn = tsk_fn ? tsk_fn->obtain_able : 0;
		break;
	case tsk_op_cancel:
		fn = tsk_fn ? tsk_fn->cancel_able : 0;
		break;
	case tsk_op_finish:
		fn = tsk_fn ? tsk_fn->finish_able : 0;
		break;
	default:
		fn = 0;
		break;
	}

	if (fn) {
		return fn(p, tskid);
	}

	/* 不需要判断使能 (都允许) */
	return 0;
}
