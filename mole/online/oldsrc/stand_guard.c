
#include <ant/bitmanip/bitmanip.h>
#include "proto.h"
#include "exclu_things.h"
#include "login.h"

#include "stand_guard.h"



int db_get_stand_guard_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(guard_info_t));

	const guard_info_t* ret = (void*)buf;
	
	p->tmpinfo.count = ret->count;
	p->tmpinfo.task  = ret->task;
	DEBUG_LOG("STAND GUARD INFO\t[id=%u count=%u task=%u]", p->id, ret->count, ret->task);

	return proc_final_login_step(p);
}

int submit_once_stand_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

	if(!test_bit_on(p->tasks, sizeof(p->tasks), 18))
		ERROR_RETURN(("%u not policeman", p->id), -1);
	if(p->tmpinfo.task != 1)
		ERROR_RETURN(("%u not get guard task or have done", p->id), -1);
	return db_set_sth_done(p, 102, 1);
}

int set_guard_task_status_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	int j = 0;
	uint32_t task_status;

	if(!test_bit_on(p->tasks, sizeof(p->tasks), 18))
		ERROR_RETURN(("%u not policeman", p->id), -1);

	UNPKG_UINT32(body, task_status, j);	
	if(p->tmpinfo.task == 2)
		ERROR_RETURN(("%u already submitted guard task", p->id), -1);
	if(task_status == 1 && p->tmpinfo.task == 0)
		DEBUG_LOG("%u GET GUARD TASK", p->id);
	else if( task_status == 2 && p->tmpinfo.task == 1 && p->tmpinfo.count >= 3 ){
		do_buy_item(p, 12275, 1, 1, 0);
		DEBUG_LOG("%u DONE GUARD TASK", p->id);
	}
	else 
		ERROR_RETURN(("%u bad guard task status, oldstatus=%u, newstatus=%u, count=%u", p->id, p->tmpinfo.task, task_status, p->tmpinfo.count), -1);
	
	db_set_guard_task_status(p, task_status);
	p->tmpinfo.task = task_status;

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

