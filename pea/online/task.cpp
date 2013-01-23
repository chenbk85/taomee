extern "C" {
#include <libtaomee/log.h>
}

#include "cli_proto.hpp"
#include "player.hpp"
#include "task.hpp"
#include "prize.hpp"
#include "db_task.hpp"
#include "task_funcs.hpp"



//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------

bool is_valid_tskid(uint32_t tskid)
{
    return (task_config_map.find(tskid) != task_config_map.end());
}

bool is_valid_task_flag(uint32_t flag)
{
	return (flag < max_task_flag);
}

bool is_valid_query_able_type(uint32_t query_type)
{
	return (query_type == tsk_query_able_obtain);
}

void set_task_flag(task_t *tsk, uint32_t flag)
{
	tsk->flag = flag;
}

void dealloc_task_node(task_t *tsk)
{
	g_slice_free1(sizeof(*tsk), tsk);
}

bool init_player_task(player_t *p)
{
	p->tskmap = new tskmap_t();
	return true;
}

bool final_player_task(player_t *p)
{
	tskiter_t iter = p->tskmap->begin();
	for (; iter != p->tskmap->end(); iter++) {
		dealloc_task_node(iter->second);
	}
	p->tskmap->clear();
    delete p->tskmap;
    p->tskmap = NULL;
	return true;
}

task_t *alloc_task_node(player_t *p, uint32_t tskid, uint32_t flag)
{
	if (!is_valid_tskid(tskid)) return NULL;

	task_t *tsk = reinterpret_cast<task_t*>(g_slice_alloc0(sizeof(task_t)));
	if (!tsk) return NULL;
	tsk->tskid = tskid;
	set_task_flag(tsk, flag);
	return tsk;
}


//------------------------------------------------------------------
// implement functions
//------------------------------------------------------------------

/**
 * @brief 是否可获得任务
 * @return 0: 可以, !0 错误码 (表示不可行的原因)
 */
int task_obtain_able(const player_t *p, uint32_t tskid)
{
	if (!is_valid_tskid(tskid)) {
		return task_err_invalid_tskid;
	}

	/* 1. 是否没有接过该任务 */
	tskiter_t iter = p->tskmap->find(tskid);
	if (iter != p->tskmap->end()) {
		return task_err_taken_yet;
	}

	/* 2. 是否满足获得该任务的各种条件 */
	return task_able_iface(p, tskid, tsk_op_obtain);
}

/**
 * @brief 得到任务
 * @return 0: 成功, !0: 错误码 (表示不可行的原因)
 */
int task_obtain(player_t *p, uint32_t tskid)
{
	int err = task_obtain_able(p, tskid);
	if (err) {
TRACE_TLOG("task_obtain err=%d, tskid=%u, u=%u", err, tskid, p->id);
		return err;
	}

	/* 到此, 可获得该任务 */

TRACE_TLOG("task_obtain tskid=%u, u=%u", tskid, p->id);
	return db_obtain_task(p, tskid);
}

/**
 * @brief 是否可放弃任务
 * @return 0: 可以, !0 错误码 (表示不可行的原因)
 */
int task_cancel_able(const player_t *p, uint32_t tskid)
{
	if (!is_valid_tskid(tskid)) {
		return task_err_invalid_tskid;
	}

	/* 1. 没有接过该任务就不能放弃该任务 */
	tskiter_t iter = p->tskmap->find(tskid);
	if (iter == p->tskmap->end()) {
		return task_err_untaken; 
	}

	/* 是否可取消 tskid 的任务 */
	return task_able_iface(p, tskid, tsk_op_cancel);
}

/**
 * @brief 放弃任务
 * @return 0: 成功, !0: 错误码 (表示不可行的原因)
 */
int task_cancel(player_t *p, uint32_t tskid)
{
	int err = task_cancel_able(p, tskid);
	if (err) {
		return err;
	}

	/* 满足放弃任务的条件, 到 db 设置取消任务 */
	return db_cancel_task(p, tskid);
}

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
		uint32_t setter, bool &need_update_db)
{
	assert(p); // 非客户端协议触发调用时有必要判断一下;

	int err = task_setbuff_iface(p, tskid, step, setter, need_update_db);
	if (err) {
		return err;
	}

	if (!need_update_db) {
		/* 设置过程没有错误, 但不用与db同步
		 * (可能是因为tskid没有注册设置函数, 或者经检查没做任何修改) */
		return 0;
	}

	/* 至此, p->buff_cache 中已是更新过后的 buff, 把它同步到db */
	/* 注意: 此处是把 buff_cache 发到db, 而不是 tsk->buff */
    
    /*由于发送到DB后就返回处理其他流程 而DB不记录step 客户端又需要step，所以缓存起来*/
    p->cache_step = step;
	return db_setbuff_task(p, tskid, p->buff_cache, sizeof(p->buff_cache));
}

/**
 * @brief 是否可完成任务
 * @return 0: 可以, !0 错误码 (表示不可行的原因)
 */
int task_finish_able(const player_t *p, uint32_t tskid)
{
	if (!is_valid_tskid(tskid)) {
		return task_err_invalid_tskid;
	}

	/* 1. p 是否接过该任务? */
	tskiter_t iter = p->tskmap->find(tskid);
	if (iter == p->tskmap->end()) {
		return task_err_untaken; 
	}
    
    /* 2.是否已经完成过了*/
    if (iter->second->flag == task_finished) {
        return task_err_already_finished;
    }

	/* 3. 是否可完成 tskid 任务 */
	return task_able_iface(p, tskid, tsk_op_finish);
}

/**
 * @brief 完成任务
 * @return 0: 可以, !0 错误码 (表示不可行的原因)
 */
int task_finish(player_t *p, uint32_t tskid, uint32_t optional_pos)
{
	int err = task_finish_able(p, tskid);
	if (err) {
		return err;
	}

    /*! 如果可完成任务，先判断可选的奖励是否合法*/
    std::map<uint32_t, task_info_t *>::iterator it;
    it = task_config_map.find(tskid);
    uint32_t prize_id = it->second->prize_id;
    if (prize_id != 0) {//有奖励
        std::map<uint32_t, prize_info_t*>::iterator itp;
        itp = prize_config_map.find(prize_id);
        if (optional_pos > itp->second->prizes->size()) {
            return task_err_prize_illegal;
        }
        prize_t *prize = &((*(itp->second->prizes))[optional_pos - 1]);
        if (prize->fixed != prize_optional) {
            return task_err_prize_illegal;
        }
    }

	/* 满足完成任务的条件, 先到 db 设置完成 tskid 任务 */
    p->cache_prize_id = prize_id;
    p->cache_optional_prize_pos = optional_pos;
	return db_finish_task(p, tskid);
}


//------------------------------------------------------------------
// cli_proto_XXX functions
//------------------------------------------------------------------
/* DEFAULT_ARG = player_t * p, Cmessage * c_in, Cmessage * c_out, void * param */
int cli_proto_obtain_task(DEFAULT_ARG)
{
    cli_proto_obtain_task_in *p_in = P_IN;
TRACE_TLOG("obtain_task, tskid=%u, u=%u", p_in->tskid, p->id);
	int err = task_obtain(p, p_in->tskid);
	if (err) {
        return send_error_to_player(p, err + TASK_ERRCODE_BASE);
	}
	return 0;
}

int cli_proto_cancel_task(DEFAULT_ARG)
{
    cli_proto_cancel_task_in *p_in = P_IN;
TRACE_TLOG("cancel_task, tskid=%u, u=%u", p_in->tskid, p->id);
	int err = task_cancel(p, p_in->tskid);
	if (err) {
        return send_error_to_player(p, err + TASK_ERRCODE_BASE);
	}
	return 0;
}

/** @brief 注意: 客户端不直接设置tskbuff,
 * svr需要解析该协议的subcmd来判断cli是想设置tskbuff的哪些数值;
 * 比如: 与npc聊过天, 打死某只怪,
 *
 * 通用设置协议,
 * 任何客户端所需的设置任务状态的请求都走这条协议
 */
int cli_proto_setbuff_task(DEFAULT_ARG)
{
    cli_proto_setbuff_task_in *p_in = P_IN;
	uint32_t tskid = p_in->tskid;
	uint32_t step = p_in->step;
	uint32_t setter = tsk_setter_client;
	bool need_update_db = false;

	TRACE_TLOG("setbuff_task, tskid=%u, step=%u, setter=%u, u=%u",
			tskid, step, setter, p->id);

	/* 尝试在 buff_cache 中设置 step, 若成功, 则更新db */
	int err = task_setbuff(p, tskid, step, setter, need_update_db);
	if (err) {
        return send_error_to_player(p, err + TASK_ERRCODE_BASE);
	}

	/* 如果不需要更新db, 就直接返回客户端说成功 */
	if (!need_update_db) {
        cli_proto_setbuff_task_out out;
        out.tskid = p_in->tskid;
        out.step = p_in->step;
        return send_to_player(p, &out, p->waitcmd, 1);
	}

	/* 否则就返回 0, 并等待 db 的返回 */
	return 0;
}

int cli_proto_finish_task(DEFAULT_ARG)
{
    cli_proto_finish_task_in *p_in = P_IN;
TRACE_TLOG("finish_task, tskid=%u, u=%u", p_in->tskid, p->id);

	int err = task_finish(p, p_in->tskid, p_in->optional_pos);
	if (err) {
        return send_error_to_player(p, err + TASK_ERRCODE_BASE);
	}
	return 0;
}

int cli_proto_get_task_flag_list(DEFAULT_ARG)
{
    cli_proto_get_task_flag_list_in *p_in = P_IN;

TRACE_TLOG("get_task_flag_list, begin_tskid=%u, end_tskid=%u, u=%u",
		p_in->begin_tskid, p_in->end_tskid, p->id);

	if (!p->tskmap) {
		return send_error_to_player(p, cli_err_system_error);
	}
	uint32_t begin_tskid = p_in->begin_tskid;
	uint32_t end_tskid = p_in->end_tskid;
	if (begin_tskid > end_tskid) {
		task_swap_uint32(begin_tskid, end_tskid);
	}
	if (begin_tskid < task_id_min) {
		begin_tskid = task_id_min;
	}
	if (end_tskid > task_id_max) {
		end_tskid = task_id_max;
	}
	tskiter_t iter = p->tskmap->lower_bound(begin_tskid);
	cli_proto_get_task_flag_list_out out;
	task_flag_t one;

	uint32_t count = 0;
	for (; iter != p->tskmap->end(); iter++) {
		if (count++ > MAX_GET_TSKFLAG_NUM) {
			WARN_TLOG("get_task_flag_list too many once time, u=%u, ip=0x%X",
					p->id, p->fdsess ? p->fdsess->remote_ip : 0);
			break;
		}

		task_t *tsk = iter->second;
		uint32_t tskid = iter->first;
		if (tskid > end_tskid) {
			break;
		}
		one.tskid = tsk->tskid;
		one.flag = tsk->flag;
		out.task_flag_list.push_back(one);
	}

	return send_to_player(p, &out, p->waitcmd, 1);
}

int cli_proto_get_task_full_list(DEFAULT_ARG)
{
    cli_proto_get_task_full_list_in *p_in = P_IN;

TRACE_TLOG("get_task_full_list, begin_tskid=%u, end_tskid=%u, u=%u",
		p_in->begin_tskid, p_in->end_tskid, p->id);

	if (!p->tskmap) {
		return send_error_to_player(p, cli_err_system_error);
	}
	uint32_t begin_tskid = p_in->begin_tskid;
	uint32_t end_tskid = p_in->end_tskid;
	if (begin_tskid > end_tskid) {
		task_swap_uint32(begin_tskid, end_tskid);
	}
	if (begin_tskid < task_id_min) {
		begin_tskid = task_id_min;
	}
	if (end_tskid > task_id_max) {
		end_tskid = task_id_max;
	}
	tskiter_t iter = p->tskmap->lower_bound(begin_tskid);
	cli_proto_get_task_full_list_out out;
	task_full_t one;

	uint32_t count = 0;
	for (; iter != p->tskmap->end(); iter++) {
		if (count++ > MAX_GET_TSKBUFF_NUM) {
			WARN_TLOG("get_task_full_list too many once time, u=%u, ip=0x%X",
					p->id, p->fdsess ? p->fdsess->remote_ip : 0);
			break;
		}

		task_t *tsk = iter->second;
		uint32_t tskid = iter->first;
		if (tskid > end_tskid) {
			break;
		}
		one.tskid = tsk->tskid;
		one.flag = tsk->flag;
		memcpy(one.buff, tsk->buff, sizeof(one.buff));
		out.task_full_list.push_back(one);
	}

	return send_to_player(p, &out, p->waitcmd, 1);
}

int cli_proto_get_task_buff_list(DEFAULT_ARG)
{
    cli_proto_get_task_buff_list_in *p_in = P_IN;
	cli_proto_get_task_buff_list_out out;

	uint32_t tskid = 0;
	uint32_t count = 0;
	task_idbuff_t one;
	for (uint32_t i = 0; i < p_in->tskid_list.size(); i++) {
		if (count++ > MAX_GET_TSKBUFF_NUM) {
			WARN_TLOG("get_task_buff_list too many once time, u=%u, ip=0x%X",
					p->id, p->fdsess ? p->fdsess->remote_ip : 0);
			break;
		}
		tskid = p_in->tskid_list[i];
		if (!is_valid_tskid(tskid)) {
			ERROR_TLOG("ill_tskid: %u, task=%u, uid=%u, ip=0x%X",
					tskid, p->id, p->fdsess ? p->fdsess->remote_ip : 0);
			continue;
		}
		tskiter_t iter = p->tskmap->find(tskid);
		if (iter == p->tskmap->end()) {
			continue;
		}
		task_t *tsk = iter->second;
		one.tskid = tskid;
		memcpy(one.buff, tsk->buff, sizeof(one.buff));
		out.task_buff_list.push_back(one);
	}

	return send_to_player(p, &out, p->waitcmd, 1);
}

int cli_proto_task_query_able(DEFAULT_ARG)
{
    cli_proto_task_query_able_in *p_in = P_IN;
	cli_proto_task_query_able_out out;

	int err = 0;
	uint32_t tskid = 0;
	uint32_t count = 0;
	uint32_t query_type = p_in->query_type;
	if (!is_valid_query_able_type(query_type)) {
		err = task_err_inv_query_able_type;
        return send_error_to_player(p, err + TASK_ERRCODE_BASE);
	}

	task_able_t one;
	for (uint32_t i = 0; i < p_in->tskid_list.size(); i++) {
		if (count++ > MAX_QUERY_ABLE_NUM) {
			WARN_TLOG("task_query_able too many once time, u=%u, ip=0x%X",
					p->id, p->fdsess ? p->fdsess->remote_ip : 0);
			break;
		}
		tskid = p_in->tskid_list[i];
		switch (query_type) {
		case tsk_query_able_obtain:
			err = task_obtain_able(p, tskid);
			break;

		default:
			break;
		}

		one.tskid = tskid;
		one.able = err;
		out.task_able_list.push_back(one);
	}

	return send_to_player(p, &out, p->waitcmd, 1);
}
