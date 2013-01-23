extern "C" {
#include "libtaomee/dataformatter/bin_str.h"
}

#include <time.h>

#include "proto.hpp"
#include "pea_common.hpp"
#include "cli_proto.hpp"

#include "login.hpp"
#include "prize.hpp"
#include "db_task.hpp"


//------------------------------------------------------------------
// obtain_task
//------------------------------------------------------------------

int db_obtain_task(player_t *p, uint32_t tskid)
{
	db_proto_obtain_task_in in;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.user_id = p->id;
	in.tskid = tskid;
    int32_t now = time(NULL);
    //buff的前四个字节设置为任务接取时间
    memcpy(in.buff, &now, sizeof(int32_t)) ;   

TRACE_TLOG("db_obtain_task, tskid=%u, u=%u", tskid, p->id);
	return send_to_db(p, db_proto_obtain_task_cmd, &in);
}

int db_proto_obtain_task_callback(DEFAULT_ARG)
{
	db_proto_obtain_task_out *p_in = P_IN;
TRACE_TLOG("obtain_task_callback, tskid=%u, u=%u", p_in->tskid, p->id);
	uint32_t tskid = p_in->tskid;

	/* 一旦初始化后就是已接受状态 */
	task_t *tsk = alloc_task_node(p, tskid);
	if (!tsk) {
        return send_error_to_player(p, cli_err_system_error);
	}

    //设置前四个字节为任务接取时间
    memcpy(tsk->buff, p_in->buff, sizeof(int32_t));
	p->tskmap->insert(make_pair(tskid, tsk));

	cli_proto_obtain_task_out out;
	out.tskid = tskid;
	return send_to_player(p, &out, p->waitcmd, 1);
}


//------------------------------------------------------------------
//	cancel_task 
//------------------------------------------------------------------

int db_cancel_task(player_t *p, uint32_t tskid)
{
	db_proto_cancel_task_in in;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.user_id = p->id;
	in.tskid = tskid;

TRACE_TLOG("db_cancel_task, tskid=%u, u=%u", tskid, p->id);
	return send_to_db(p, db_proto_cancel_task_cmd, &in);
}

int db_proto_cancel_task_callback(DEFAULT_ARG)
{
	db_proto_cancel_task_out *p_in = P_IN;
TRACE_TLOG("cancel_task_callback, tskid=%u, u=%u", p_in->tskid, p->id);
	uint32_t tskid = p_in->tskid;

	tskiter_t iter = p->tskmap->find(tskid);
	task_t *tsk = iter->second;
	dealloc_task_node(tsk);
	p->tskmap->erase(tskid);

	cli_proto_cancel_task_out out;
	out.tskid = tskid;
	return send_to_player(p, &out, p->waitcmd, 1);
}


//------------------------------------------------------------------
//	setbuff_task 
//------------------------------------------------------------------

int db_setbuff_task(player_t *p, uint32_t tskid, void *buff, int bufflen)
{
	/* 注意: 此函数传入的 bufflen 目前被忽略, 因为DB是定长, 但以后会用lvdb支持变长 */
	assert(bufflen == MAX_TASK_BUFF_LEN);

	db_proto_setbuff_task_in in;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.user_id = p->id;
	in.tskid = tskid;
	memcpy(in.buff, buff, MAX_TASK_BUFF_LEN);

#ifdef ENABLE_TRACE_LOG
char tmpbuf[256] = {0};
hex2str(in.buff, sizeof(in.buff), tmpbuf);
TRACE_TLOG("db_setbuff_task, tskid=%u, u=%u, buff_cache: %s",
		tskid, p->id, tmpbuf);
#endif
	/* TODO(zog): 区分是否需要db返回情况 */
	return send_to_db(p, db_proto_setbuff_task_cmd, &in);
}

int db_proto_setbuff_task_callback(DEFAULT_ARG)
{
	db_proto_setbuff_task_out *p_in = P_IN;
    cli_proto_setbuff_task_out out;
TRACE_TLOG("setbuff_task_callback, tskid=%u, u=%u", p_in->tskid, p->id);

	/* db成功返回, 则正式修改 online 的 tskbuff */
	task_t *tsk = 0;
	uint32_t tskid = p_in->tskid;
	tskiter_t tsk_iter = p->tskmap->find(tskid);
	if (tsk_iter == p->tskmap->end()) {
		WARN_TLOG("setbuff_task_cb return invalid tskid=%u, u=%u",
				tskid, p->id);
		goto end;
	}
	tsk = tsk_iter->second;
	memcpy(tsk->buff, p->buff_cache, sizeof(tsk->buff));

end:
    out.tskid = p_in->tskid;
    out.step = p->cache_step;
	switch (p->waitcmd) {
	case cli_proto_setbuff_task_cmd:
        return send_to_player(p, &out, p->waitcmd, 1);

	default:
		break;
	}
	return 0;
}


//------------------------------------------------------------------
//	finish_task 
//------------------------------------------------------------------

int db_finish_task(player_t *p, uint32_t tskid)
{
	db_proto_finish_task_in in;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.user_id = p->id;
	in.tskid = tskid;
    tskiter_t iter = p->tskmap->find(tskid);
	task_t *tsk = iter->second;
    int32_t now = time(NULL);
    memcpy(in.buff, tsk->buff, sizeof(in.buff));
    //后四个字节设置为任务完成时间
    memcpy(in.buff + 4, &now, sizeof(int32_t)) ;   

TRACE_TLOG("db_finish_task, tskid=%u, u=%u", tskid, p->id);
	return send_to_db(p, db_proto_finish_task_cmd, &in);
}

int db_proto_finish_task_callback(DEFAULT_ARG)
{
	db_proto_finish_task_out *p_in = P_IN;
TRACE_TLOG("finish_task_callback, tskid=%u, u=%u", p_in->tskid, p->id);
	uint32_t tskid = p_in->tskid;

	tskiter_t iter = p->tskmap->find(tskid);
	task_t *tsk = iter->second;
    //回包置后四个字节为任务完成时间
    memcpy(tsk->buff + 4, p_in->buff+4, sizeof(int32_t));
	//设置任务完成标志
    set_task_flag(tsk, task_finished);

    //判断是否有奖励
    if (p->cache_prize_id == 0) {//没有奖励 直接send_to_player;
        cli_proto_finish_task_out out;
        out.tskid = tskid;
        out.optional_pos = p->cache_optional_prize_pos;
        p->cache_tskid = 0;
        p->cache_prize_id = 0;
        return send_to_player(p, &out, p->waitcmd, 1);
    }

    //有奖励则处理奖励
    p->cache_tskid = tskid;
    return get_prize(p, p->cache_prize_id, prize_seq_start);
}


//------------------------------------------------------------------
//	get_task_flag_list
//------------------------------------------------------------------

int db_get_task_flag_list(player_t *p, uint32_t begin_tskid, uint32_t end_tskid)
{
	db_proto_get_task_flag_list_in in;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.user_id = p->id;
	in.begin_tskid = begin_tskid;
	in.end_tskid = end_tskid;

TRACE_TLOG("db_get_task_flag, begin_tskid=%u, end_tskid=%u, u=%u", begin_tskid, end_tskid, p->id);
	return send_to_db(p, db_proto_get_task_flag_list_cmd, &in);
}

int db_proto_get_task_flag_list_callback(DEFAULT_ARG)
{
	task_t *tsk;
	uint32_t tskid, flag;
	db_proto_get_task_flag_list_out *p_in = P_IN;

	for (uint32_t i = 0; i < p_in->task_flag_list.size(); i++) {
		/* 处理每一个task flag信息 */
		tskid = p_in->task_flag_list[i].tskid;
		flag = p_in->task_flag_list[i].flag;
		if (!is_valid_task_flag(flag)) {
			ERROR_TLOG("ill_flag: %u, task=%u, uid=%u", flag, tskid, p->id);
			continue;
		}
		tskiter_t iter = p->tskmap->find(tskid);
		if (iter == p->tskmap->end()) {
			tsk = alloc_task_node(p, tskid, flag);
			if (!tsk) {
				ERROR_TLOG("nomem for task=%u, uid=%u", tskid, p->id);
				return -1;
			}
			p->tskmap->insert(make_pair(tskid, tsk));
		} else {
			tsk = iter->second;
			tsk->flag = flag;
		}
TRACE_TLOG("get_task_flag_callback, tskid=%u, flag=%u, u=%u", tsk->tskid, tsk->flag, p->id);
	}

	/* TODO(zog): anything else? eg: switch (p->waitcmd) {case ...} */
	return 0;
}

//------------------------------------------------------------------
//	get_task_full_list
//------------------------------------------------------------------

int db_get_task_full_list(player_t *p, uint32_t begin_tskid, uint32_t end_tskid)
{
	db_proto_get_task_full_list_in in;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.user_id = p->id;
	in.begin_tskid = begin_tskid;
	in.end_tskid = end_tskid;

TRACE_TLOG("db_get_task_full, begin_tskid=%u, end_tskid=%u, u=%u", begin_tskid, end_tskid, p->id);
	return send_to_db(p, db_proto_get_task_full_list_cmd, &in);
}

int db_proto_get_task_full_list_callback(DEFAULT_ARG)
{
	int bufflen;
	char *buff;
	task_t *tsk;
	uint32_t tskid, flag;

	db_proto_get_task_full_list_out *p_in = P_IN;
	for (uint32_t i = 0; i < p_in->task_full_list.size(); i++) {
		/* 处理每一个task full信息 */
		tskid = p_in->task_full_list[i].tskid;
		flag = p_in->task_full_list[i].flag;
		bufflen = sizeof(p_in->task_full_list[i].buff);
		buff = p_in->task_full_list[i].buff;

		if (!is_valid_task_flag(flag)) {
			ERROR_TLOG("ill_flag(full): %u, task=%u, uid=%u", flag, tskid, p->id);
			continue;
		}

		if (bufflen != MAX_TASK_BUFF_LEN) {
			ERROR_TLOG("error tskbuff len=%d task=%u, uid=%u",
					bufflen, tskid, p->id);
			return -1;
		}

		tskiter_t iter = p->tskmap->find(tskid);
		if (iter == p->tskmap->end()) {
			tsk = alloc_task_node(p, tskid, flag);
			if (!tsk) {
				ERROR_TLOG("nomem(full) for task=%u, uid=%u", tskid, p->id);
				return -1;
			}
			memcpy(tsk->buff, buff, bufflen);
			p->tskmap->insert(make_pair(tskid, tsk));
		} else {
			tsk = iter->second;
			tsk->flag = flag;
		}

TRACE_TLOG("get_task_full_callback, tskid=%u, flag=%u, u=%u", tsk->tskid, tsk->flag, p->id);
	}

	switch (p->waitcmd) {
	case cli_proto_login_cmd:
		p->set_module(MODULE_TASK);
		return process_login(p);

	default:
		break;
	}
	return 0;
}
