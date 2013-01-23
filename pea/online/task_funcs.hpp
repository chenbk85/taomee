#ifndef __TASK_FUNCS_HPP__
#define __TASK_FUNCS_HPP__


enum tsk_op_t {
	tsk_op_obtain		= 0,
	tsk_op_cancel		= 1,
	tsk_op_finish		= 2,
};



typedef int (*able_fn_t) (const player_t *p, uint32_t task_id);
typedef int (*setbuff_fn_t) (const player_t *p, uint32_t task_id, uint32_t step,
		uint32_t setter, bool &need_update_db, char *buff, int bufflen);

struct tsk_fn_t {
	able_fn_t		obtain_able;
	able_fn_t		cancel_able;
	able_fn_t		finish_able;
	setbuff_fn_t	setbuff;
};

typedef map<uint32_t, tsk_fn_t*>::iterator tsk_fn_iter_t;

typedef map<uint32_t, able_fn_t> tsk_able_map_t;
typedef map<uint32_t, setbuff_fn_t> tsk_setbuff_map_t;


void init_obtain_able_fn_map(void);
void init_cancel_able_fn_map(void);
void init_finish_able_fn_map(void);
void init_setbuff_fn_map(void);
void final_obtain_able_fn_map(void);
void final_cancel_able_fn_map(void);
void final_finish_able_fn_map(void);
void final_setbuff_fn_map(void);


int init_task_fn_map(void);
void final_task_fn_map(void);

int task_setbuff_iface(player_t *p, uint32_t tskid, uint32_t step,
		uint32_t setter, bool &need_update_db);
int task_able_iface(const player_t *p, uint32_t tskid, uint32_t op);


//------------------------------------------------------------------
// inline utils
//------------------------------------------------------------------
//

enum {
    task_type_begin             = 0,
    task_type_main              = 1,
    task_type_branch            = 2,
    task_type_daily             = 3,
    task_type_once              = 4,
    task_type_end           
};

/*
enum {
    task_step_cond_type_bool    = 0,    //01条件
    task_step_cond_type_kv      = 1,    //key val条件
    task_step_cond_type_val     = 2,    //量值条件
}
*/

enum {
    task_step_type_begin        = 0,
    task_step_type_1            = 1,    //某件事情是否完成 1个字节
    task_step_type_2            = 2,    //收集某物品(打死多少怪 收集多少道具)N个 2个字节
    task_step_type_3            = 3,    //级别达到多少 2个字节
    task_step_type_4            = 4,    //经验达到多少 个4字节
    task_step_type_5            = 5,    //钱达到多少 4个字节
    task_step_type_end
};

enum {
    pre_condition_type_or       = 0,    //或类型的前置条件
    pre_condition_type_and      = 1,    //与类型的前置条件
    max_task_pre_condition      = 4,    //接收某个任务时候必须达到的前置条件
    max_step_pre_condition      = 4,    //设置某个步骤的时候必须达到的前置条件
};

struct step_cond_t {
    /*! 参数key 只有key val参数有key 其他为0*/
    uint32_t key;
    /*! 参数值 如果不为0 表示完成该步骤需要的item数量*/
    uint32_t val;
};

struct task_step_t {
    /*! 步骤类型 */
    uint32_t step_type;
    /*! 任务步骤对应的完成条件*/
    step_cond_t condition;
    /*! 设置任务步骤的前置条件*/
    uint32_t pre_cond[max_step_pre_condition];
    /*! 任务步骤的前置步骤类型 与 或*/
    uint32_t pre_cond_type;
};

typedef std::vector<task_step_t> step_vec_t;
struct task_info_t {
    /*! 任务ID */
    uint32_t task_id;
    /*! 任务类型 */
    uint32_t task_type;
    /*! 接受任务的前置任务*/
    uint32_t pre_cond[max_task_pre_condition];
    /*! 前置任务的类型与还是或*/
    uint32_t pre_cond_type;
    /*! 接受任务需要的等级*/
    uint32_t need_level;
    /*! 接受任务时玩家所需要在的地图ID*/
    uint32_t bind_map_id;
    /*! 任务完成时的奖励ID*/
    uint32_t prize_id;
    /*! 任务的步骤 */
    step_vec_t *steps;
};

struct task_buffer_head_t {
    int32_t taken_time;
    int32_t finish_time;
} __attribute__((packed));

/*! 从配置文件读取的任务map */
extern std::map<uint32_t, task_info_t*> task_config_map;

/**
 * @brief 初始化任务列表
 * @return 0 ok -1 失败
 */
int init_task_config_data(xmlNodePtr task_node);

/**
 * @brief 销毁任务列表
 */
void final_task_config_data();

//=====================task buffer check helper function===========
static inline bool buff_8_eq_val(const char* buf, uint32_t val)
{
    return (*((uint8_t*)buf) == val);
}
static inline bool buff_8_ge_val(const char* buf, uint32_t val)
{
    return (*((uint8_t*)buf) >= val);
}
static inline bool buff_16_eq_val(const char* buf, uint32_t val)
{
    return (*((uint16_t*)buf) == val);
}
static inline bool buff_16_ge_val(const char* buf, uint32_t val)
{
    return (*((uint16_t*)buf) >= val);
}
static inline bool buff_32_eq_val(const char* buf, uint32_t val)
{
    return (*((uint32_t*)buf) >= val);
}
static inline bool buff_32_ge_val(const char* buf, uint32_t val)
{
    return (*((uint32_t*)buf) >= val);
}
//=========================================================
#endif // __TASK_FUNCS_HPP__
