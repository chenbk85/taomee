#ifndef __TASK_HPP__
#define __TASK_HPP__


#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "task_funcs.hpp"


#ifdef tsk_likely
#undef tsk_likely
#endif
#define tsk_likely(x) __builtin_expect(!!(x), 1)

#ifdef tsk_unlikely
#undef tsk_unlikely
#endif
#define tsk_unlikely(x) __builtin_expect(!!(x), 0)



/*! 最多 1w 个任务 */
#define MAX_TASK_NUM		(10000)
/*! 最大buffer长度(db字段长度); 单位: 字节 */
#define MAX_TASK_BUFF_LEN	(32)

/*! 任务错误码基数 */
#define TASK_ERRCODE_BASE	(202000)


/*! 一次最多获取任务 仅flag 数量 */
#define MAX_GET_TSKFLAG_NUM		(100)

/*! 一次最多获取任务 buff 数量 */
#define MAX_GET_TSKBUFF_NUM		(50)

/*! 一次最多查询使能的任务数量 */
#define MAX_QUERY_ABLE_NUM		(20)

enum task_id_t {
	task_id_min							= 1,
	task_id_max							= task_id_min + MAX_TASK_NUM - 1,
};

enum tsk_setter_t {
	tsk_setter_server					= 0,
	tsk_setter_client					= 1,
};

enum tsk_query_able_type {
	tsk_query_able_obtain				= 1,
};

enum task_error_t {
	/*! 没有错误 */
	task_err_noerr						= 0,

	/*! 公共错误 (1 - 100) */
	task_err_invalid_tskid				= 1,
	task_err_taken_yet					= 2,
	task_err_untaken					= 3,
	task_err_inv_query_able_type		= 4,
    task_err_already_finished           = 5,

	/*! 不可 获得 任务错误 (101 - 200) */
	/*! 前置任务未完成*/
    task_err_lack_pre_task				= 101,
    /*! 等级不够*/
    task_err_higher_level_required      = 102,
    /*! 玩家地图不对*/
    task_err_at_wrong_map               = 103,

	/*! 不可 取消 任务错误 (201 - 300) */


	/*! 不可 设置 任务错误 (301 - 400) */
	/*! 该 setter 没有权限设置该任务的步骤 */
	task_err_setter_noauth				= 301,
    /*! 任务步骤不合法*/
    task_err_step_invalid               = 302,
    /*! 任务步骤类型不合法*/
    task_err_step_type_invalid          = 303,
    /*! 前置步骤未完成*/
    task_err_lack_pre_step              = 304,
    /*! 步骤已经设置过*/
    task_err_step_already_finished      = 305,

	/*! 不可 完成 任务错误 (401 - 500) */
    task_err_unfinished                 = 401,
    /*! 完成任务时提交的奖励可选位置不合法*/
    task_err_prize_illegal              = 402,

	/*! 测试用 */
	task_err_testerr_obtain_able		= 1300000001,
	task_err_testerr_cancel_able		= 1300000002,
	task_err_testerr_finish_able		= 1300000003,
	task_err_testerr_setbuff_able		= 1300000004,
};

enum {
	task_inprogress						= 0,
	task_finished						= 1,

	max_task_flag,
};

struct task_t {
	/*! 任务id */
	uint32_t	tskid;
	/*! 任务状态 (0: inprogress, 1: finished) */
	uint32_t	flag;
	/*! 任务buffer */
	char		buff[MAX_TASK_BUFF_LEN];
};


typedef std::map<uint32_t, task_t *>::iterator tskiter_t;
typedef std::map<uint32_t, task_t *> tskmap_t;



//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------

bool is_valid_tskid(uint32_t tskid);
bool is_valid_task_flag(uint32_t flag);
void set_task_flag(task_t *tsk, uint32_t flag);
task_t *alloc_task_node(player_t *p, uint32_t tskid, uint32_t f = task_inprogress);
void dealloc_task_node(task_t *tsk);
bool init_player_task(player_t *p);
bool final_player_task(player_t *p);



//------------------------------------------------------------------
// implement functions
//------------------------------------------------------------------

/**
 * @brief 是否可获得任务
 * @return 0: 可以, !0 错误码 (表示不可行的原因)
 */
int task_obtain_able(const player_t *p, uint32_t tskid);

/**
 * @brief 得到任务
 * @return 0: 成功, !0: 错误码 (表示不可行的原因)
 */
int task_obtain(player_t *p, uint32_t tskid);

/**
 * @brief 是否可放弃任务
 * @return 0: 可以, !0 错误码 (表示不可行的原因)
 */
int task_cancel_able(const player_t *p, uint32_t tskid);

/**
 * @brief 放弃任务
 * @return 0: 成功, !0: 错误码 (表示不可行的原因)
 */
int task_cancel(player_t *p, uint32_t tskid);

/**
 * @brief 设置任务buffer
 * 注意: 这个接口会被server很多地方调用, 用来设置各种任务状态
 *
 * tskid 要设置 buff 的任务id
 * step 要设置tskid任务的第几步
 * setter 设置者 (0: server, 1: client)
 * return 0: 成功, !0: 错误码 (表示不可行的原因)
 */
int task_setbuff(player_t *p, uint32_t tskid, uint32_t step,
		uint32_t setter, uint32_t &need_update_db);

/**
 * @brief 是否可完成任务
 * @return 0: 可以, !0 错误码 (表示不可行的原因)
 */
int task_finish_able(const player_t *p, uint32_t tskid);

/**
 * @brief 完成任务
 * @return 0: 可以, !0 错误码 (表示不可行的原因)
 */
int task_finish(player_t *p, uint32_t tskid, uint32_t prize_id);



//------------------------------------------------------------------
// cli_proto_XXX functions
//------------------------------------------------------------------

int cli_proto_obtain_task(DEFAULT_ARG);
int cli_proto_cancel_task(DEFAULT_ARG);

/** @brief 注意: 客户端不直接设置tskbuff,
 * svr需要解析该协议的subcmd来判断cli是想设置tskbuff的哪些数值;
 * 比如: 与npc聊过天, 打死某只怪,
 *
 * 通用设置协议,
 * 任何客户端所需的设置任务状态的请求都走这条协议
 */
int cli_proto_setbuff_task(DEFAULT_ARG);
int cli_proto_finish_task(DEFAULT_ARG);
int cli_proto_get_task_flag_list(DEFAULT_ARG);
int cli_proto_get_task_full_list(DEFAULT_ARG);


//------------------------------------------------------------------
// inline utils
//------------------------------------------------------------------
inline void task_swap_uint32(uint32_t &left, uint32_t &right)
{
	left ^= right;
	right ^= left;
	left ^= right;
}

inline bool is_valid_setter(uint32_t setter)
{
	return (setter == tsk_setter_server || setter == tsk_setter_client);
}

#endif // __TASK_HPP__
