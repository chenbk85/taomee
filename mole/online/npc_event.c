#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "homeinfo.h"

#include "race_car.h"
#include "npc_event.h"
#include "exclu_things.h"
#include "school_bar_cmd.h"
#include "foot_print.h"


//#define NPC_TASK_TABLE_SIZE 	4096
#define NPC_TASK_TABLE_SIZE 	50
static npc_task_t npc_tasks[NPC_TASK_TABLE_SIZE];

int get_npc_lovely_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t indx;
	int i = 0;
	UNPKG_UINT32(body, indx, i);
	if (indx >= MAX_NPC || indx == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_npc_type, 1);
	}
	const struct timeval* tv = get_now_tv();
	DEBUG_LOG("GET NPC LOVELY\t[%u %d %d]", p->id, p->npc_lovely[indx-1], (uint32_t)tv->tv_sec);
	//response_proto_uint32_uint32(p, p->waitcmd, p->npc_lovely[indx-1], tv->tv_sec, 0);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, indx, j);
	PKG_UINT16(msg, p->npc_lovely[indx-1], j);
	PKG_UINT32(msg, tv->tv_sec, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int update_npc_lovely_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t indx;
	int	delta, j = 0;
	UNPKG_UINT32(body, indx, j);
	UNPKG_UINT32(body, delta, j);
	if (indx >= MAX_NPC || indx == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_npc_type, 1);
	}
	if (delta > 3) {
		return send_to_self_error(p, p->waitcmd, -ERR_you_are_too_greedy, 1);
	}

	if (delta <= 0) {
		int buf[] = {delta, indx};
		send_request_to_db(SVR_UPDATE_NPC_LOVELY, 0, 8, buf, p->id);
		p->npc_lovely[indx-1] += delta;
		int k = sizeof(protocol_t);
		PKG_UINT32(msg, indx, k);
		PKG_UINT16(msg, p->npc_lovely[indx-1], k);
		PKG_UINT32(msg, get_now_tv()->tv_sec, k);
		init_proto_head(msg, p->waitcmd, k);
		return send_to_self(p, msg, k, 1);
	}
	*(int*)p->session = delta;
	*(uint32_t*)(p->session + 4) = indx;
	return db_set_sth_done(p, 209, 10, p->id);
}



static int parse_npc_task_single_item(item_unit_t* iut, int* cnt, xmlNodePtr cur)
{
	int id, j = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item"))) {
			if (j == MAX_EXCHANGE_ITEMS) {
				ERROR_RETURN(("too many items"), -1);
			}

			DECODE_XML_PROP_INT (id, cur, "ID");
			if (!(iut[j].itm = get_item_prop(id))) {
				ERROR_RETURN(("can't find item=%d", id), -1);
			}
			DECODE_XML_PROP_INT (iut[j].count, cur, "Count");
			if (iut[j].count <= 0) {
				ERROR_RETURN(("error count=%d, item=%d", iut[j].count, id), -1);
			}
			j++;
		}
		cur = cur->next;
	}
	*cnt = j;

	return 0;
}

static int parse_npc_task_items(npc_task_t *eit, xmlNodePtr chl)
{
	xmlNodePtr cur;

	while (chl) {
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemIn")){
			cur = chl->xmlChildrenNode;
			if (parse_npc_task_single_item(eit->in, &(eit->incnt), cur) != 0)
				return -1;
		}
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemOut")){
			cur = chl->xmlChildrenNode;
			if (parse_npc_task_single_item(eit->out[eit->outkind_cnt], &(eit->outcnt[eit->outkind_cnt]), cur) != 0)
				return -1;
			++(eit->outkind_cnt);
		}
		chl = chl->next;
	}

	return 0;
}

int load_npc_tasks(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int ex_count;
	int tmp_type;

	ex_count = 0;
	memset (npc_tasks, 0, sizeof (npc_tasks));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Entry"))) {
			DECODE_XML_PROP_INT(i, cur, "ID");
			if (ex_count >= NPC_TASK_TABLE_SIZE - 1 || i <= 0) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}
//			DEBUG_LOG("PARSE ID OK");
			npc_tasks[i-1].npc_taskid = i;
			DECODE_XML_PROP_INT(npc_tasks[i-1].npc_id, cur, "NPC");
			if (npc_tasks[i-1].npc_id > MAX_NPC) {
				ERROR_LOG ("parse %s failed, Count=%d, npc_id=%d", file, ex_count, npc_tasks[i-1].npc_id);
				goto exit;
			}
//			DEBUG_LOG("PARSE NPC OK");
			DECODE_XML_PROP_INT(npc_tasks[i].npc_lovely, cur, "NpcLovely");
			if (npc_tasks[i-1].npc_lovely > 300 || npc_tasks[i-1].npc_lovely < -200) {
				ERROR_LOG ("parse %s failed, Count=%d, npc_lovely=%d", file, ex_count, npc_tasks[i-1].npc_lovely);
				goto exit;
			}
//			DEBUG_LOG("PARSE NpcLovely OK");
			DECODE_XML_PROP_INT(npc_tasks[i-1].max_num, cur, "MaxNum");
//			DEBUG_LOG("PARSE MaxNum OK");
			DECODE_XML_PROP_INT_DEFAULT(npc_tasks[i-1].vip_task, cur, "VIP", 0);
			if (npc_tasks[i-1].vip_task != 0 && npc_tasks[i-1].vip_task != 1) {
				ERROR_LOG ("parse %s failed, Count=%d, vip=%d", file, ex_count, npc_tasks[i-1].vip_task);
				goto exit;
			}
//			DEBUG_LOG("PARSE VIP OK\t[vip_task=%u]", npc_tasks[i-1].vip_task);
			DECODE_XML_PROP_INT_DEFAULT(tmp_type, cur, "Daily", 0);
			if (tmp_type != 0 && tmp_type != 1) {
				ERROR_LOG ("parse %s failed, Count=%d, Daily=%d", file, ex_count, tmp_type);
				goto exit;
			}
			npc_tasks[i-1].task_type = tmp_type;
//			DEBUG_LOG("PARSE Daily OK\t[task_type=%u]", npc_tasks[i-1].task_type);
			DECODE_XML_PROP_INT_DEFAULT(tmp_type, cur, "Weekly", 0);
			if ((tmp_type != 0 && tmp_type != 1)
				|| (tmp_type == 1 && npc_tasks[i-1].task_type & 1)){
				ERROR_LOG ("parse %s failed, Count=%d, Weekly=%d", file, ex_count, tmp_type);
				goto exit;
			}
//			DEBUG_LOG("PARSE Weekly OK\t[task_type=%u]", npc_tasks[i-1].task_type);
			npc_tasks[i-1].task_type |= tmp_type << 1;
			DECODE_XML_PROP_INT_DEFAULT(tmp_type, cur, "Monthly", 0);
			if ((tmp_type != 0 && tmp_type != 1)
				|| (tmp_type == 1 && (npc_tasks[i-1].task_type & 1 || npc_tasks[i-1].task_type & 2))) {
				ERROR_LOG ("parse %s failed, Count=%d, monthly=%d", file, ex_count, tmp_type);
				goto exit;
			}
//			DEBUG_LOG("PARSE Monthly OK\t[task_type=%u]", npc_tasks[i-1].task_type);
			npc_tasks[i-1].task_type |= tmp_type << 2;

			DECODE_XML_PROP_INT_DEFAULT(npc_tasks[i - 1].smc, cur, "SMC", 0);
			if (npc_tasks[i - 1].smc > 50) {
				ERROR_LOG ("parse %s failed, smc %u", file, npc_tasks[i - 1].smc);
				goto exit;
			}

			chl = cur->xmlChildrenNode;
			if ( (parse_npc_task_items(&npc_tasks[i-1], chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load exchange item file %s", file);

}

int take_npc_task_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t ntaskid;
	int	j = 0;
	UNPKG_UINT32(body, ntaskid, j);
	if (ntaskid == 0 || ntaskid > NPC_TASK_TABLE_SIZE || npc_tasks[ntaskid - 1].npc_taskid != ntaskid) {
		ERROR_RETURN(("npc task id too big or task not exist\t[%u %u]", p->id, ntaskid), -1);
	}
	DEBUG_LOG("TAKE NPC TASK\t[%u %u]", p->id, ntaskid);
	npc_task_t* ntt = &npc_tasks[ntaskid - 1];
	if (VIP_NPC_TASK(ntt->vip_task) && !ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_vip_npc_task, 1);
	}
	if ((ntt->smc != 0) && (p->profession[ntt->smc - 1] == 0)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_fin_your_profession, 1);
	}
	uint32_t buff[5];
	buff[0] = ntt->npc_id;
	buff[1] = ntt->npc_taskid;
	buff[2] = ntt->npc_lovely;
	buff[3] = ntt->max_num;
	buff[4] = ntt->task_type;
	return send_request_to_db(SVR_PROTO_TAKE_NPC_TASK, p, 20, buff, p->id);
}

int take_npc_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);
	uint32_t nt_status, nt_times, ntaskid;
	ntaskid = *(uint32_t*)buf;
	nt_status = *(uint32_t*)(buf + 4);
	nt_times = *(uint32_t*)(buf + 8);

	if (ntaskid == 0 || ntaskid > NPC_TASK_TABLE_SIZE) {
		ERROR_RETURN(("npc task id too big or task not exist\t[%u %u]", p->id, ntaskid), -1);
	}
	uint32_t data[] = {1, 0};
	msglog(statistic_logfile, 0x02051000 + ntaskid, now.tv_sec, data, 8);
	npc_task_t* ntt = &npc_tasks[ntaskid - 1];
	DEBUG_LOG("TAKE NPC TASK\t[%u %u %u %u %u %u %u %u %u]", p->id, ntt->npc_taskid, ntt->npc_id, ntt->npc_lovely, ntt->vip_task, ntt->task_type, ntt->max_num, nt_status, nt_times);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, ntt->npc_taskid, j);
	PKG_UINT32(msg, ntt->npc_id, j);
	PKG_UINT32(msg, ntt->npc_lovely, j);
	PKG_UINT32(msg, ntt->vip_task, j);
	PKG_UINT32(msg, ntt->task_type, j);
	PKG_UINT32(msg, ntt->max_num, j);
	PKG_UINT32(msg, nt_status, j);
	PKG_UINT32(msg, nt_times, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int do_fin_npc_task(sprite_t* p)
{
	return send_request_to_db(SVR_PROTO_FIN_NPC_TASK, p, 8, p->session, p->id);
}

int fin_npc_task_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t ntaskid, select_type;
	int	k = 0;
	UNPKG_UINT32(body, ntaskid, k);
	UNPKG_UINT32(body, select_type, k);
	if (ntaskid == 0 || ntaskid > NPC_TASK_TABLE_SIZE || npc_tasks[ntaskid - 1].npc_taskid != ntaskid) {
		ERROR_RETURN(("npc task id too big or task no exist\t[%u %u]", p->id, ntaskid), -1);
	}
	npc_task_t* ntt = &npc_tasks[ntaskid - 1];
	if (VIP_NPC_TASK(ntt->vip_task) && !ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_vip_npc_task, 1);
	}
	DEBUG_LOG("FIN NPC TASK\t[%u %u %u]", p->id, ntaskid, select_type);


	uint32_t out_kind = select_type % ntt->outkind_cnt;
	*(uint32_t*)p->session 		 = ntt->npc_id;
	*(uint32_t*)(p->session + 4) = ntt->npc_taskid;
	*(uint32_t*)(p->session + 8) = out_kind;

	return do_fin_npc_task(p);

}


int fin_npc_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t nt_status, nt_times;
	nt_status = *(uint32_t*)buf;
	nt_times = *(uint32_t*)(buf + 4);

	uint32_t ntaskid = *(uint32_t*)(p->session + 4);
	uint32_t out_kind = *(uint32_t*)(p->session + 8);
	npc_task_t *ntt = &npc_tasks[ntaskid -1];

	uint32_t data[] = {0, 1};
	msglog(statistic_logfile, 0x02051000 + ntaskid, now.tv_sec, data, 8);

	*(uint32_t *)(p->session + 12) = nt_status;
	*(uint32_t *)(p->session + 16) = nt_times;
	int i, j = 0;
	// Package For DB
	uint8_t buff[1024];
	PKG_H_UINT32(buff, ntt->incnt, j);
	PKG_H_UINT32(buff, ntt->outcnt[out_kind], j);
	//PKG_H_UINT32(buff, ATTR_CHG_task_fin, j);
	PKG_H_UINT32(buff, NOT_INCLUDE_MONEY_DAY_LIMIT, j);
	PKG_H_UINT32(buff, ntt->npc_taskid, j);
	for (i = 0; i != ntt->incnt; ++i) {
		if (pkg_item_kind(p, buff, ntt->in[i].itm->id, &j) == -1)
			return -1;
		PKG_H_UINT32(buff, ntt->in[i].itm->id, j);
		PKG_H_UINT32(buff, ntt->in[i].count, j);
	}
	for (i = 0; i != ntt->outcnt[out_kind]; i++) {
		if (pkg_item_kind(p, buff, ntt->out[out_kind][i].itm->id, &j) == -1)
			return -1;
		PKG_H_UINT32(buff, ntt->out[out_kind][i].itm->id, j);
		PKG_H_UINT32(buff, ntt->out[out_kind][i].count, j);
		PKG_H_UINT32(buff, ntt->out[out_kind][i].itm->max, j);
	}

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);
}

int fin_npc_task_to_self(sprite_t* p)
{
	uint32_t nt_status, nt_times;
	nt_status = *(uint32_t *)(p->session + 12);
	nt_times = *(uint32_t*)(p->session + 16);

	uint32_t ntaskid  = *(uint32_t*)(p->session + 4);
	uint32_t out_kind = *(uint32_t*)(p->session + 8);
	npc_task_t* ntt = &npc_tasks[ntaskid - 1];

	DEBUG_LOG("FIN NPC TASK STATUS\t[%u %u %u %u %u %u %u %u %u]", p->id, ntt->npc_taskid, ntt->npc_id, ntt->npc_lovely, ntt->vip_task, ntt->task_type, ntt->max_num, nt_status, nt_times);
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, ntt->npc_taskid, j);
	PKG_UINT32(msg, ntt->npc_id, j);
	PKG_UINT32(msg, ntt->npc_lovely, j);
	PKG_UINT32(msg, ntt->vip_task, j);
	PKG_UINT32(msg, ntt->task_type, j);
	PKG_UINT32(msg, ntt->max_num, j);
	PKG_UINT32(msg, nt_status, j);
	PKG_UINT32(msg, nt_times, j);
	PKG_UINT32(msg, ntt->outcnt[out_kind], j);
	int i;
	for (i = 0; i != ntt->outcnt[out_kind]; i++) {
		PKG_UINT32(msg, ntt->out[out_kind][i].itm->id, j);
		PKG_UINT32(msg, ntt->out[out_kind][i].count, j);
		grow_profession(p, ntt->out[out_kind][i].itm->id, ntt->out[out_kind][i].count);
		add_foot_print_count(p, ntt->out[out_kind][i].itm->id, ntt->out[out_kind][i].count);
	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int get_nt_client_data_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t ntaskid;
	int	j = 0;
	UNPKG_UINT32(body, ntaskid, j);
	if (ntaskid == 0 || ntaskid > NPC_TASK_TABLE_SIZE || npc_tasks[ntaskid - 1].npc_taskid != ntaskid) {
		ERROR_RETURN(("npc task id too big or task no exist\t[%u %u]", p->id, ntaskid), -1);
	}
	DEBUG_LOG("GET NPC TASK DATA\t[%u %u]", p->id, ntaskid);
	npc_task_t* ntt = &npc_tasks[ntaskid - 1];
	if (VIP_NPC_TASK(ntt->vip_task) && !ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_vip_npc_task, 1);
	}

	*(uint32_t*)p->session = ntaskid;
	uint32_t buff[2];
	buff[0] = ntt->npc_id;
	buff[1] = ntt->npc_taskid;
	return send_request_to_db(SVR_PROTO_GET_NT_CLIENT_DATA, p, 8, buff, p->id);
}

int get_nt_client_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 50);
	char tmp_buf[54];
	*(uint32_t*)tmp_buf = htonl(*(uint32_t*)p->session);
	memcpy(tmp_buf + 4, buf, 50);
	response_proto_str(p, p->waitcmd, 54, tmp_buf, 0);
	return 0;
}

int set_nt_client_data_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 54);
	uint32_t ntaskid;
	int	j = 0;
	UNPKG_UINT32(body, ntaskid, j);
	if (ntaskid == 0 || ntaskid > NPC_TASK_TABLE_SIZE || npc_tasks[ntaskid - 1].npc_taskid != ntaskid) {
		ERROR_RETURN(("npc task id too big or task no exist\t[%u %u]", p->id, ntaskid), -1);
	}
	DEBUG_LOG("SET NPC TASK DATA\t[%u %u]", p->id, ntaskid);
	npc_task_t* ntt = &npc_tasks[ntaskid - 1];
	if (VIP_NPC_TASK(ntt->vip_task) && !ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_vip_npc_task, 1);
	}

	int i = 0;
	char buff[58];
	PKG_H_UINT32(buff, ntt->npc_id, i);
	PKG_H_UINT32(buff, ntt->npc_taskid, i);
	PKG_STR(buff, body+4, i, 50);
	return send_request_to_db(SVR_PROTO_SET_NT_CLIENT_DATA, p, 58, buff, p->id);
}

int set_nt_client_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	//CHECK_BODY_LEN(len, 0);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}
int get_npc_task_status_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	uint32_t ntaskid;
	int	j = 0;
	UNPKG_UINT32(body, ntaskid, j);
	if (ntaskid == 0 || ntaskid > NPC_TASK_TABLE_SIZE || npc_tasks[ntaskid - 1].npc_taskid != ntaskid) {
		ERROR_RETURN(("npc task id too big or task no exist\t[%u %u]", p->id, ntaskid), -1);
	}
	DEBUG_LOG("GET NPC TASK STATUS\t[%u %u]", p->id, ntaskid);
	npc_task_t* ntt = &npc_tasks[ntaskid - 1];
	if (VIP_NPC_TASK(ntt->vip_task) && !ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_vip_npc_task, 1);
	}
	*(uint32_t*)p->session = ntaskid;
	uint32_t buff[2];
	buff[0] = ntt->npc_id;
	buff[1] = ntt->npc_taskid;
	return send_request_to_db(SVR_PROTO_GET_NPC_TASK_STATUS, p, 8, buff, p->id);
}

int get_npc_task_status_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t nt_status, nt_times, ntaskid;
	nt_status = *(uint32_t*)buf;
	nt_times = *(uint32_t*)(buf + 4);

	ntaskid = *(uint32_t*)p->session;
	npc_task_t* ntt = &npc_tasks[ntaskid - 1];

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, ntt->npc_taskid, j);
	PKG_UINT32(msg, ntt->npc_id, j);
	PKG_UINT32(msg, ntt->npc_lovely, j);
	PKG_UINT32(msg, ntt->vip_task, j);
	PKG_UINT32(msg, ntt->task_type, j);
	PKG_UINT32(msg, ntt->max_num, j);
	PKG_UINT32(msg, nt_status, j);
	PKG_UINT32(msg, nt_times, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int get_npc_tasks_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t npc_idx;
	int	j = 0;
	UNPKG_UINT32(body, npc_idx, j);
	DEBUG_LOG("GET NPC TASKS\t[%u %u]", p->id, npc_idx);
	if (npc_idx >= MAX_NPC) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_npc_type, 1);
	}

	return send_request_to_db(SVR_PROTO_GET_NPC_TASKS, p, 4, &npc_idx, p->id);
}

int get_npc_tasks_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct {
	uint32_t npc_taskid;
	uint32_t nt_status;
	uint32_t nt_times;
	uint32_t task_type;
	uint32_t recv_time;
}npc_task_status_t;

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t nt_count;
	nt_count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + nt_count*sizeof(npc_task_status_t));

	int j = sizeof(protocol_t);
	uint32_t temp_pos = j;
	uint32_t temp_count = nt_count;
	PKG_UINT32(msg, nt_count, j);
	int loop;
	for (loop = 0; loop < nt_count; loop++) {
		npc_task_status_t* nts = (npc_task_status_t*)(buf + 4 + 20 * loop);
		if (nts->npc_taskid == 0 || nts->npc_taskid > NPC_TASK_TABLE_SIZE || npc_tasks[nts->npc_taskid - 1].npc_taskid != nts->npc_taskid) {
			temp_count -= 1;
			PKG_UINT32(msg, temp_count, temp_pos);
			temp_pos -= 4;
			continue;
		}
		npc_task_t* ntt = &npc_tasks[nts->npc_taskid - 1];
		PKG_UINT32(msg, ntt->npc_taskid, j);
		PKG_UINT32(msg, ntt->npc_id, j);
		PKG_UINT32(msg, ntt->npc_lovely, j);
		PKG_UINT32(msg, ntt->vip_task, j);
		PKG_UINT32(msg, ntt->task_type, j);
		PKG_UINT32(msg, ntt->max_num, j);
		PKG_UINT32(msg, nts->nt_status, j);
		PKG_UINT32(msg, nts->nt_times, j);
	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int cancel_npc_task_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t ntaskid;
	int	j = 0;
	UNPKG_UINT32(body, ntaskid, j);
	if (ntaskid == 0 || ntaskid > NPC_TASK_TABLE_SIZE || npc_tasks[ntaskid - 1].npc_taskid != ntaskid) {
		ERROR_RETURN(("npc task id too big or task not exist\t[%u %u]", p->id, ntaskid), -1);
	}
	DEBUG_LOG("CANCEL NPC TASK\t[%u %u]", p->id, ntaskid);
	npc_task_t* ntt = &npc_tasks[ntaskid - 1];
	if (VIP_NPC_TASK(ntt->vip_task) && !ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_vip_npc_task, 1);
	}

	*(uint32_t*)p->session = ntaskid;
	uint32_t buff[2];
	buff[0] = ntt->npc_id;
	buff[1] = ntt->npc_taskid;
	return send_request_to_db(SVR_PROTO_CANCEL_NPC_TASK, p, 8, buff, p->id);
}

int cancel_npc_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	response_proto_uint32(p, p->waitcmd, *(uint32_t*)p->session, 0);
	return 0;
}

