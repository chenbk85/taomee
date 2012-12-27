#include <statistic_agent/msglog.h>
#include "dbproxy.h"
#include "pet.h"
#include "util.h"
#include "exclu_things.h"
#include "login.h"

#include "tasks.h"
#include "lahm_classroom.h"

enum TaskConst {
	TASK_max_tasks	= 128
};

int set_profession_by_taskid(sprite_t* p, uint32_t taskid, uint32_t uid)
{
	uint32_t buff[2];
	switch (taskid) {
	case 0:              // become guider
		buff[0] = 0;
		buff[1] = 1;
		break;
	case 8:				// Becoming A Policeman
		buff[0] = 1;
		buff[1] = 1;
		break;
	case 17:			// Becoming A reporter
		buff[0] = 2;
		buff[1] = 1;
		break;
	case 151:			//Becoming A practical cavalier
		buff[0] = 3;
		buff[1] = 1;
		break;
	case 152:			//Becoming A XunZhang cavalier
		buff[0] = 3;
		buff[1] = 2;
		break;
	case 131:			// Becoming A fireman
		buff[0] = 4;
		buff[1] = 1;
       	break;
   	case 161:           // Becoming A Doctor
		buff[0] = 5;
		buff[1] = 1;
		break;
	case 141:           // Becoming A Driver
		buff[0] = 6;
		buff[1] = 1;
		break;
	case 221:           // Becoming A  architect
		buff[0] = 7;
		buff[1] = 1;
		break;
	case 153:			//Become A FOREST cavalier
		buff[0] = 8;
		buff[1] = 1;
		break;
	case 145:			//Become A smc lahm teacher
		buff[0] = 9;
		buff[1] = 1;
		break;
	case 274:			//Become A smc dietitian
		buff[0] = 10;
		buff[1] = 1;
		break;
	case 365:			//Become A smc small hunt
		buff[0] = 11;
		buff[1] = 1;
		break;
	case 1:			//Become A super guider
		buff[0] = 12;
		buff[1] = 1;
		break;
	default:
		return 0;
	}
	p->profession[buff[0]] = buff[1];
    DEBUG_LOG("SET PROFESS [%d %d]", uid, taskid);
	return db_set_profession(0, buff, uid);
}

static task_t all_tasks[TASK_max_tasks];
static lahm_task_t lahm_task[TASK_max_tasks];
//----------------- callbacks for dealing with DB return -----------------
//
int get_task2_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(task_list_t));

	const task_list_t* tasklist = (void*)buf;

	CHECK_BODY_LEN(len, sizeof(task_list_t) + tasklist->cnt * sizeof(task_elem_t));

	switch (p->waitcmd) {
	case PROTO_GET_PET_TASK_LIST:
	{
		int i = sizeof(protocol_t), j;
		PKG_UINT32(msg, *(uint32_t*)(p->session), i);
		PKG_UINT32(msg, tasklist->cnt, i);
		for (j = 0; j != tasklist->cnt; ++j) {
			PKG_UINT32(msg, tasklist->elem[j].taskid, i);
			PKG_UINT32(msg, tasklist->elem[j].flag, i);
			PKG_UINT16(msg, (tasklist->elem[j].accu_tm / 86400), i);
//			DEBUG_LOG("PET TASK INFO\t[%u %u %u]", p->id, tasklist->elem[j].taskid, tasklist->elem[j].flag);
		}
		init_proto_head(msg, p->waitcmd, i);
		send_to_self(p, msg, i, 1);
		p->sess_len = 0;
		break;
	}
	default:
		ERROR_RETURN( ("Unexpected WaitCmd %d", p->waitcmd), -1 );
	}

	return 0;
}

int set_task2_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct TaskElemUpdRet {
	uint32_t petid;
	uint32_t taskid;
} __attribute__((packed)) task_elem_upd_ret_t;

	CHECK_BODY_LEN(len, sizeof(task_elem_upd_ret_t));

	task_elem_upd_ret_t* task_elem_ret = (void*)buf;

	switch (p->waitcmd) {
	case PROTO_TERMINATE_PET_TASK:
		do_terminate_pet_task(p, p->id, task_elem_ret->petid, 0);
		break;
	case PROTO_SET_PET_TASK_FIN:
	{
		pet_t* pet = get_pet(p->id, task_elem_ret->petid);
		if (pet) {
			int loop;
			for (loop = 0; loop < pet->suppl_info.pti.count; loop++) {
				if (pet->suppl_info.pti.taskinfo[loop].taskid == task_elem_ret->taskid) {
					pet->suppl_info.pti.taskinfo[loop].task_flag = 3;
					DEBUG_LOG("FIN PET TASK\t[%u %u %u]", p->id, pet->id, pet->suppl_info.pti.taskinfo[loop].taskid);
					break;
				}
			}
		}
		response_proto_head(p, p->waitcmd, 0);
		break;
	}
	default:
		ERROR_RETURN( ("Unexpected WaitCmd %d", p->waitcmd), -1 );
	}

	return 0;
}

//-----------------------------------------------------------------

//----------------------- For proto.h -----------------------
int get_task2_list_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 16);

	int i = 0;
	task_list_query_t* query = (void*)(p->session);
	UNPKG_UINT32(body, query->petid, i);
	UNPKG_UINT32(body, query->start_taskid, i);
	UNPKG_UINT32(body, query->end_taskid, i);
	UNPKG_UINT32(body, query->flag, i);

	p->sess_len = 4;
	return db_get_task2_list(p, *query);
}
//--------------------------------------------------------

//-------------------------- Utils -------------------------
task_t* get_task(uint32_t taskid)
{
	return (((taskid <= TASK_max_tasks) && (all_tasks[taskid - 1].id == taskid)) ? &(all_tasks[taskid - 1]) : 0);
}
//--------------------------------------------------------
lahm_task_t *get_lahm_task(uint32_t taskid)
{
	taskid--;
	return taskid < TASK_max_tasks ? lahm_task + taskid : NULL;
}
//------------------ Parse Task Conf File Begin ------------------
//
int load_task_config(xmlNodePtr cur_node)
{
	uint32_t taskid;
	task_t*  task;

	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar*)"Task")) {
			DECODE_XML_PROP_UINT32(taskid, cur_node, "ID");
			if (taskid > TASK_max_tasks) {
				ERROR_RETURN( ("Failed to Parse Task Conf File, TaskID=%u", taskid), -1 );
			}

			task      = &(all_tasks[taskid - 1]);
			task->id  = taskid;

			DECODE_XML_PROP_STR(task->name, cur_node, "Name");
			DECODE_XML_PROP_INT_DEFAULT(task->task_time, cur_node, "TaskTime", 0);
			DECODE_XML_PROP_INT(task->task_end_flag, cur_node, "TaskEndFlag");
			DECODE_XML_PROP_INT_DEFAULT(task->coins_per_day, cur_node, "CoinsPerDay", 0);
			DECODE_XML_PROP_INT_DEFAULT(task->pet_is_vip, cur_node, "PetIsVIP", 0);
			DECODE_XML_PROP_INT_DEFAULT(task->award_item_id, cur_node, "AwardItemID", 0);
			//DEBUG_LOG("ID %u TIME %ld COINS %d", task->id, task->task_time, task->coins_per_day);
		}
		cur_node = cur_node->next;
	}

	return 0;
}

int load_lahm_task(xmlNodePtr cur_node)
{
	uint32_t taskid;

	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar*)"Task")) {
			DECODE_XML_PROP_UINT32(taskid, cur_node, "ID");

			if (taskid - 1 >= TASK_max_tasks) {
				ERROR_RETURN( ("Failed to Parse Lahm Task Conf File, TaskID=%u", taskid), -1 );
			}

			lahm_task[taskid - 1].ID  = taskid;
		}
		cur_node = cur_node->next;
	}

	return 0;
}

int get_task_tmp_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_GET_TASK_TMP_INFO, p, 0, NULL, p->id);
}

int get_task_tmp_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 40);

	int i = sizeof(protocol_t);
	memcpy(msg + sizeof(protocol_t), buf, 40);
	i += 40;

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int set_task_tmp_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 40);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_SET_TASK_TMP_INFO, p, 40, body, p->id);
}

int get_task_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t task_id = ntohl(*(uint32_t*)body);

	return db_get_task_info(p, &task_id);
}


int task_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 54);

	switch (p->waitcmd) {
	case PROTO_GET_TASK_INFO:
	case PROTO_SET_TASK_INFO:
	case PROTO_SET_BOOK_INFO:
	case PROTO_GET_BOOK_INFO:
		*(uint32_t*)buf = htonl(*(uint32_t*)buf);
		response_proto_str(p, p->waitcmd, 54, buf, 0);
		break;
	case PROTO_LOGIN:
	{
		uint32_t step = ntohl(*(uint32_t *)(buf + 4));
		return proc_final_login_step(p, step);
	}
	default:
		ERROR_RETURN(("WRONG WAIT CMD %u %u", p->id, p->waitcmd), -1);
	}
	return 0;
}

int get_task_list_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
    uint32_t taskid = 0;
    int i = 0;
    uint32_t cnt = 0;
    CHECK_BODY_LEN_GE(len, 4);
	UNPKG_UINT32(body, cnt, i);
	CHECK_BODY_LEN(len, 4+cnt*4);
	if ((cnt > 20) || (cnt == 0)){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint8_t dbbuf[1024] = {};
	int l = 0;
	PKG_H_UINT32(dbbuf,cnt,l);

	int j = 0;
	for (j = 0; j < cnt; j++){
	    UNPKG_UINT32(body, taskid, i);
	    PKG_H_UINT32(dbbuf, taskid, l);
	}
    return send_request_to_db(SVR_PROTO_GET_TASK_LIST_INFO, p, l, dbbuf, p->id);

}

int get_task_list_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    struct task_info_hdr {
		uint32_t	count;
	}__attribute__((packed));

    struct task_info {
        uint32_t	taskid;
        uint8_t     data[50];
	}__attribute__((packed));

	struct task_info_hdr *p_hdr_info = (struct task_info_hdr*)buf;

	CHECK_BODY_LEN_GE(len, sizeof(struct task_info_hdr));
    int l = sizeof(protocol_t);;
    PKG_UINT32(msg, p_hdr_info->count, l);

    CHECK_BODY_LEN(len,sizeof(struct task_info_hdr) + p_hdr_info->count*sizeof(struct task_info));
    struct task_info *p_info = (struct task_info*)(buf + sizeof(struct task_info_hdr));

	int i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
	    PKG_UINT32(msg, p_info->taskid, l);
        PKG_STR(msg, p_info->data, l, 50);
	    p_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}


int set_task_info_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 54);
	CHECK_VALID_ID(p->id);

	*(uint32_t*)body = ntohl(*(uint32_t*)body);

	uint32_t taskid = *(uint32_t*)body;
	if (taskid > 1000000)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	return db_set_task_info(p, body);
}


int get_book_list_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t userid = 0;
	uint32_t i = 0;
	UNPKG_UINT32(body, userid, i);
	CHECK_VALID_ID(userid);

	return send_request_to_db(SVR_PROTO_GET_USER_TASK_EX_IDLIST, p, 0, NULL, userid);
}

int get_book_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN_GE(len, 4);
    uint32_t count = 0;
    uint32_t i = 0;
    UNPKG_H_UINT32(buf,count,i);
    if (count > 50)
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
    }

    int l = sizeof(protocol_t);
    PKG_UINT32(msg, count, l);

    uint32_t j = 0;
    for (j = 0; j < count; j++)
    {
        uint32_t taskid = 0;
        UNPKG_H_UINT32(buf,taskid,i);
        PKG_UINT32(msg, taskid, l);
    }

    init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);


}


int get_book_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t task_id = 0;
	uint32_t userid = 0;
	uint32_t i = 0;
	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, task_id, i);
	CHECK_VALID_ID(userid);

	return send_request_to_db(SVR_PROTO_GET_TASK_INFO, p, 4, &task_id, userid);
}


int set_book_info_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 54);
	CHECK_VALID_ID(p->id);

	if (!ISVIP(p->flag))
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	*(uint32_t*)body = ntohl(*(uint32_t*)body);

	uint32_t taskid = *(uint32_t*)body;
	if (taskid != 1000001 && taskid != 1000002)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	return db_set_task_info(p, body);
}


int del_task_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t task_id = ntohl(*(uint32_t*)body);

	return db_del_task_info(p, &task_id);
}

int del_task_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t taskid = *(uint32_t*)buf;
	response_proto_uint32(p, p->waitcmd, taskid, 0);
	return 0;
}

int get_profession_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 4);
	int j = 0;
	uint32_t uid;
	UNPKG_UINT32(body, uid, j);
	CHECK_VALID_ID(uid);

	return db_get_profession(p, uid);
}

int get_smc_salary_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	if (p->month_task[2]) { // check already get smc salary
		return send_to_self_error(p, p->waitcmd, -ERR_already_get_smc_salary, 1);
	}

	return db_set_month_task_done(p, 2, 1, p->id);
}

int chk_smc_salary_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	if (p->month_task[2]) { // check already get smc salary
		return send_to_self_error(p, p->waitcmd, -ERR_already_get_smc_salary, 1);
	}

	return db_get_profession(p, p->id);
}

int get_profession_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t guild_salary[6] = {500, 700, 1000, 1300, 1600, 2000};
	uint32_t qishi_salary[3] = {1000, 2000, 3000};
	uint32_t architect_salary[5] = {500, 600, 700, 800, 1000};
	uint32_t prof_salary[50] = {500, 500, 500, 1000, 500, 500, 500, 500,1500,500,500};
	uint32_t teach_salary[5] = {500, 700, 900, 1100, 1300};
	uint32_t dietitian_salary[5] = {500, 700, 900, 1100, 1300};
	uint32_t hunt_salary[1] = {1000};

	CHECK_BODY_LEN(len, 200);

	switch (p->waitcmd) {
	case 	PROTO_GET_PROFESSIONS:
		response_proto_str(p, p->waitcmd, len, buf, 0);
		return 0;
	case	PROTO_GET_SMC_SALARY:
	{
		p->sess_len = 0;
		uint32_t salary = 0;
		int loop;
		for (loop = 0; loop < 200; loop += 4) {
			uint32_t tmp_salary = 0;
			if (buf[loop] == 0) {
				continue;
			}
			tmp_salary = prof_salary[loop/4];
			if (loop == 0) {        // guild  salary
				if (buf[loop] > 6)
					buf[loop] = 6;
				tmp_salary = guild_salary[buf[loop] - 1];
			}
			if (loop == 12) {		// Qishi  salary
				if (buf[loop] > 3)
					buf[loop] = 3;
				tmp_salary = qishi_salary[buf[loop] - 1];
			}
			if (loop == 28) {		// architect salary
				if (buf[loop] > 5)
					buf[loop] = 5;
				tmp_salary = architect_salary[buf[loop] - 1];
			}
			if (loop == 36) {		// teacher salary
				if (buf[loop] > 5)
					buf[loop] = 5;
				tmp_salary = teach_salary[buf[loop] - 1];
			}
			if (loop == 40){		// dietitian salary
				if (buf[loop] > 5)
					buf[loop] = 5;
				tmp_salary = dietitian_salary[buf[loop] - 1];
			}
			if (loop == 44) {
				if (buf[loop] > 1)
					buf[loop] = 1;
				tmp_salary = hunt_salary[buf[loop] - 1];
			}
			if (loop == 48) {
				if (buf[loop] > 1)
					buf[loop] = 1;
				tmp_salary = 0;
			}
			salary += tmp_salary;
		}
		PKG_UINT32(p->session, salary, p->sess_len);

		uint32_t msglog_salary = salary;
    		msglog(statistic_logfile, 0x020B1102, get_now_tv()->tv_sec, &msglog_salary, sizeof(uint32_t));

		return db_change_xiaome(p, salary, ATTR_CHG_roll_back, 0, p->id);
	}
	case	PROTO_CHK_SMC_SALARY:
	{
		int j = sizeof(protocol_t);
		uint32_t salary = 0;
		int loop;
		for (loop = 0; loop < 200; loop += 4) {
			uint32_t tmp_salary = 0;
			if (buf[loop] == 0) {
				PKG_UINT32(msg, tmp_salary, j);
				continue;
			}
			tmp_salary = prof_salary[loop/4];
			if (loop == 0) {        // guild  salary
				if (buf[loop] > 6)
					buf[loop] = 6;
				tmp_salary = guild_salary[buf[loop] - 1];
			}
			if (loop == 12) {		// Qishi  salary
				if (buf[loop] > 3)
					buf[loop] = 3;
				tmp_salary = qishi_salary[buf[loop] - 1];
			}
			if (loop == 28) {		// architect salary
				if (buf[loop] > 5)
					buf[loop] = 5;
				tmp_salary = architect_salary[buf[loop] - 1];
			}
			if (loop == 36) {		// teacher salary
				if (buf[loop] > 5)
					buf[loop] = 5;
				tmp_salary = teach_salary[buf[loop] - 1];
			}
			if (loop == 40){		// dietitian salary
				if (buf[loop] > 5)
					buf[loop] = 5;
				tmp_salary = dietitian_salary[buf[loop] - 1];
			}
			if (loop == 44) {		// small hunt
				if (buf[loop] > 1)
					buf[loop] = 1;
				tmp_salary = hunt_salary[buf[loop] - 1];
			}
			salary += tmp_salary;
			PKG_UINT32(msg, tmp_salary, j);
			DEBUG_LOG("CHK SALARY[%u %u %u]", p->id, loop, tmp_salary);
		}
		PKG_UINT32(msg, salary, j);
		init_proto_head(msg, p->waitcmd, j);

		return send_to_self(p, msg, j, 1);
	}
	default:
		ERROR_RETURN(("bad cmd id[%u %u]", p->id, p->waitcmd), -1);
	}
}

//------------------ Parse Task Conf File End ------------------
