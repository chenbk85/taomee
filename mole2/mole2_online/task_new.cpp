#include "task_new.h"
#include "pet.h"
#include "items.h"
#include "util.h"
#include "sns.h"
#include "skill.h"
#include "honor.h"
#include "mail.h"
#include "beast.h"
#include "cli_login.h"
static inline void cache_add_fin_task(sprite_t* p, uint32_t taskid)
{
	task_fin_t* ptf = (task_fin_t *)g_hash_table_lookup(p->fintasks, &taskid);
	if (ptf) {
		ptf->fin = 1;
		ptf->everfin = 1;
	} else {
		ptf = (task_fin_t *)g_slice_alloc(sizeof(task_fin_t));
		ptf->taskid = taskid;
		ptf->fin = 1;
		ptf->everfin = 1;
		g_hash_table_insert(p->fintasks, &ptf->taskid, ptf);
	}
}

static inline void cache_del_fin_task(sprite_t* p, uint32_t taskid)
{
	task_fin_t* ptf = (task_fin_t *)g_hash_table_lookup(p->fintasks, &taskid);
	if (ptf) {
		ptf->fin = 0;
	}
}

static inline void cache_add_ever_fin_task(sprite_t* p, uint32_t taskid)
{
	task_fin_t* ptf = (task_fin_t *)g_hash_table_lookup(p->fintasks, &taskid);
	if (ptf) {
		ptf->everfin = 1;
	} else {
		ptf = (task_fin_t *)g_slice_alloc(sizeof(task_fin_t));
		ptf->taskid = taskid;
		ptf->fin = 0;
		ptf->everfin = 1;
		g_hash_table_insert(p->fintasks, &ptf->taskid, ptf);
	}
}

int cache_check_task_ever_fin(sprite_t* p, uint32_t taskid)
{
	task_fin_t* ptf = (task_fin_t *)g_hash_table_lookup(p->fintasks, &taskid);
	return ptf ? ptf->everfin : 0;
}

int cache_check_task_node_ever_fin(sprite_t* p, uint32_t taskid, uint32_t nodeid)
{
	if (!cache_check_task_ever_fin(p, taskid)) {
		task_db_t* ptr = cache_get_task_info(p, taskid);
		if (!ptr || ptr->nodeid < nodeid)
			return 0;
	}
	return 1;
}

loop_db_t* cache_get_loop_task(sprite_t* p, uint32_t loopid)
{
	return (loop_db_t*)g_hash_table_lookup(p->looptasks, &loopid);
}

void gen_spec_loop_task(loop_db_t* pld, task_loop_t* ptl)
{
	uint32_t taskcnt = ranged_random(ptl->mincnt, ptl->maxcnt);
	pld->taskcnt = taskcnt;
	int idx = 0;
	if (ptl->startid) {
		pld->ids[0] = ptl->startid;
		idx = 1;
		pld->taskcnt ++;
	}
	
	if (ptl->endid) {
		pld->ids[taskcnt + 1] = ptl->endid;
		pld->taskcnt ++;
	}
	
	int idxa = rand() % ptl->taskcnt;
	int idxb = rand() % ptl->taskcnt;
	uint32_t tasktemp = ptl->tasks[idxa];
	ptl->tasks[idxa] = ptl->tasks[idxb];
	ptl->tasks[idxb] = tasktemp;
	for (uint32_t loop = idx; loop <= taskcnt; loop ++) {
		pld->ids[loop] = ptl->tasks[loop - idx];
	}
	
	pld->finidx = 0;
	pld->optdate = get_now_tv()->tv_sec;

}

void update_loop_task_state(sprite_t* p, loop_db_t* pld, task_loop_t* ptl)
{
	if (!check_same_period(pld->optdate, repeat_day, 5)) {
		gen_spec_loop_task(pld, ptl);
		pld->finnum = 0;
		send_request_to_db(NULL, p->id, proto_db_set_task_db, pld, sizeof(loop_db_t));
	}
}

loop_db_t* cache_add_loop_task(sprite_t* p, loop_db_t* pld, uint32_t loopid)
{
	loop_db_t* ploop = cache_get_loop_task(p, loopid);
	if (!ploop) {
		ploop = (loop_db_t *)g_slice_alloc0(sizeof(loop_db_t));
		if (pld) {
			memcpy(ploop, pld, sizeof(loop_db_t));
		} else {
			ploop->loopid = loopid;
			ploop->state = TASK_STATE_RECVD;	
		}
		g_hash_table_insert(p->looptasks, &ploop->loopid, ploop);
	}
	return ploop;
}


task_t* get_task(uint32_t taskid)
{
	return (task_t *)g_hash_table_lookup(all_tasks, &taskid);
}

task_node_t* get_task_node(uint32_t taskid, uint32_t nodeid)
{
	task_t* pt = get_task(taskid);
	return pt && nodeid && nodeid <= MAX_TASK_NODE_CNT ? &pt->nodes[nodeid - 1] : NULL;
}

task_option_t* get_node_out_opt(task_node_t* ptn, uint32_t outid)
{
	if (!ptn->optcnt || !outid || outid > ptn->optcnt)
		return NULL;
	return &ptn->options[outid];
}

int load_node_outopt(xmlNodePtr cur, task_node_t* ptn)
{
	uint32_t outid;
	DECODE_XML_PROP_UINT32 (outid, cur, "ID");
	if (ptn->optcnt + 1 != outid || ptn->optcnt + 1 > MAX_TASK_OUT_OPT)
		ERROR_RETURN(("invalid option count\t[%u %u]", ptn->nodeid, outid), -1);

	ptn->optcnt ++;
	xmlNodePtr chl = cur->xmlChildrenNode;
	while(chl) {
		if((!xmlStrcmp(chl->name, (const xmlChar *)"Item"))) {
			task_option_t* pto = &ptn->options[outid];
			pto->out_id = outid;
			if (pto->out_cnt >= MAX_TASK_OUT_ITEM_NUM)
				ERROR_RETURN(("out item count max err: id=%u, outid=%u", ptn->nodeid, outid), -1);
			DECODE_XML_PROP_UINT32(pto->items[pto->out_cnt].type, chl, "OutType");
			DECODE_XML_PROP_UINT32(pto->items[pto->out_cnt].itemid, chl, "ItemId");
			if (pto->items[pto->out_cnt].itemid == item_id_skill_exp) {
				DECODE_XML_PROP_UINT32(pto->items[pto->out_cnt].skillid, chl, "SubID");
			}
			decode_xml_prop_uint32_default(&pto->items[pto->out_cnt].count, chl, "Count", 1);
			decode_xml_prop_uint32_default(&pto->items[pto->out_cnt].level, chl, "Level", 1);
			decode_xml_prop_uint32_default(&pto->items[pto->out_cnt].prof, chl, "Prof", -1);
			
			KTRACE_LOG(0, "TASK OUT OPT\t[%u %u %u %u %u %u %u %x]", ptn->nodeid, outid, pto->out_cnt, pto->items[pto->out_cnt].type, \
				pto->items[pto->out_cnt].itemid, pto->items[pto->out_cnt].count, pto->items[pto->out_cnt].level,pto->items[pto->out_cnt].prof);

			switch (pto->items[pto->out_cnt].type) {
				case OUT_TYPE_NORMAL_ITEM:
					if (!get_item(pto->items[pto->out_cnt].itemid))
						ERROR_RETURN(("Invalid out item type or id\t[%u %u %u]", ptn->nodeid, pto->items[pto->out_cnt].type, pto->items[pto->out_cnt].itemid), -1);
					break;
				case OUT_TYPE_CLOTHES:
					if (!get_cloth(pto->items[pto->out_cnt].itemid))
						ERROR_RETURN(("Invalid out item type or id\t[%u %u %u]", ptn->nodeid, pto->items[pto->out_cnt].type, pto->items[pto->out_cnt].itemid), -1);
					decode_xml_prop_uint32_default(&pto->items[pto->out_cnt].level, chl, "Extent", 0);
					break;
			}
			pto->out_cnt ++;
		}
		chl = chl->next;
	}
	
	return 0;
}

int load_task_node(xmlNodePtr cur, task_t* pt)
{
	uint32_t nodeid;
	DECODE_XML_PROP_UINT32(nodeid, cur, "ID");
	task_node_t* ptn = get_task_node(pt->taskid, nodeid);
	if (!ptn || ptn->nodeid) 
		ERROR_RETURN(("invalid or duplicate node id\t[%u %u %u]", pt->taskid, nodeid, ptn->nodeid), -1);
	ptn->nodeid = nodeid;
	decode_xml_prop_uint32_default(&ptn->isend, cur, "IsEnd", 0);
	xmlNodePtr chl = cur->xmlChildrenNode;
	while (chl) {
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemTarget")) {
			DECODE_XML_PROP_UINT32(ptn->itarget.itemid, chl, "ItemId");
			if (!get_item(ptn->itarget.itemid) && !get_cloth(ptn->itarget.itemid))
				ERROR_RETURN(("item id invalid\t[%u %u %u]", pt->taskid, nodeid, ptn->itarget.itemid), -1);
			DECODE_XML_PROP_UINT32(ptn->itarget.count, chl, "Count");
			decode_xml_prop_uint16_default(&ptn->itarget.del, chl, "Del", 1);
			decode_xml_prop_uint32_default(&ptn->itarget.beastid, chl, "BeastType", 0);
			decode_xml_prop_uint16_default(&ptn->itarget.dropodds, chl, "DropOdds", 0);
			decode_xml_prop_uint16_default(&ptn->itarget.check, chl, "Check", 1);
			decode_xml_prop_uint16_default(&ptn->itarget.cntlimit, chl, "CntLimit", -1);
			if (ptn->itarget.beastid && (!get_beast(ptn->itarget.beastid) || get_cloth(ptn->itarget.itemid)))
				ERROR_RETURN(("beast id invalid\t[%u %u %u]", pt->taskid, nodeid, ptn->itarget.beastid), -1);

			if (pt->taskid == 39001) {
				switch (ptn->itarget.beastid % SPEC_BEAST_BASE) {
				case 2009:
					ptn->itarget.dropratio[0] = 600;
					ptn->itarget.dropratio[1] = 300;
					ptn->itarget.dropratio[2] = 100;
					break;
				case 2001:
					ptn->itarget.dropratio[0] = 500;
					ptn->itarget.dropratio[1] = 400;
					ptn->itarget.dropratio[2] = 100;
					break;
				case 2007:
					ptn->itarget.dropratio[0] = 400;
					ptn->itarget.dropratio[1] = 400;
					ptn->itarget.dropratio[2] = 200;
					break;
				case 2005:
					ptn->itarget.dropratio[0] = 200;
					ptn->itarget.dropratio[1] = 500;
					ptn->itarget.dropratio[2] = 300;
					break;
				case 3019:
					ptn->itarget.dropratio[0] = 200;
					ptn->itarget.dropratio[1] = 300;
					ptn->itarget.dropratio[2] = 500;
					break;
				}
			} else {
				ptn->itarget.dropratio[0] = RAND_COMMON_RAND_BASE;
			}
		}

		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemUseTarget")) {
			DECODE_XML_PROP_UINT32(ptn->usetarget.itemid, chl, "ItemId");
			normal_item_t* pni = get_item(!get_item(ptn->usetarget.itemid));
			if (!pni || pni->function != item_for_broadcast)
				ERROR_RETURN(("item id invalid\t[%u %u %u]", pt->taskid, nodeid, ptn->itarget.itemid), -1);

			if (pni->cast_usage.taskid)
				ERROR_RETURN(("not support 2 tasks\t[%u %u %u]", pt->taskid, nodeid, pni->cast_usage.taskid), -1);

			pni->cast_usage.taskid = pt->taskid;
			
			DECODE_XML_PROP_UINT32(ptn->usetarget.count, chl, "Count");
			KDEBUG_LOG(0, "USE TARGET\t[%u %u %u %u]", pt->taskid, ptn->nodeid, ptn->usetarget.itemid, ptn->usetarget.count);
		}

		if (!xmlStrcmp(chl->name, (const xmlChar *)"SkillTarget")) {
			DECODE_XML_PROP_UINT32(ptn->skilltarget.itemid, chl, "SkillID");
			DECODE_XML_PROP_UINT32(ptn->skilltarget.count, chl, "Count");
			if (!get_skill_info(ptn->skilltarget.itemid))
				ERROR_RETURN(("skill id invalid\t[%u]", ptn->skilltarget.itemid), -1);
		}

		if (!xmlStrcmp(chl->name, (const xmlChar *)"BeastTarget")) {
			if (ptn->btarget.beastid) 
				ERROR_RETURN(("duplicate beast id\t[%u %u]", pt->taskid, nodeid), -1);
			DECODE_XML_PROP_UINT32(ptn->btarget.beastid, chl, "BeastType");
			if (!get_beast(ptn->btarget.beastid))
				ERROR_RETURN(("beast id invalid\t[%u %u %u]", pt->taskid, nodeid, ptn->btarget.beastid), -1);
			DECODE_XML_PROP_UINT32(ptn->btarget.count, chl, "Count");
			decode_xml_prop_uint16_default(&ptn->btarget.win, chl, "Win", 1);
			decode_xml_prop_uint32_default(&ptn->btarget.mapid, chl, "MapID", 0);
			ptn->btarget.type = beast_target_beast;
		}

		if (!xmlStrcmp(chl->name, (const xmlChar *)"BeastGrp")) {
			if (ptn->btarget.beastid) 
				ERROR_RETURN(("duplicate beast id\t[%u %u]", pt->taskid, nodeid), -1);
			DECODE_XML_PROP_UINT32(ptn->btarget.beastid, chl, "GrpId");
			if (!get_beast_grp(ptn->btarget.beastid))
				ERROR_RETURN(("grp id invalid\t[%u %u %u]", pt->taskid, nodeid, ptn->btarget.beastid), -1);
			decode_xml_prop_uint32_default(&ptn->btarget.count, chl, "Count", 1);
			decode_xml_prop_uint16_default(&ptn->btarget.help, chl, "Help", 0);
			ptn->btarget.win = 1;
			ptn->btarget.type = beast_target_group;
		}

		if (!xmlStrcmp(chl->name, (const xmlChar *)"PkTarget")) {
			if (ptn->ptarget.count) 
				ERROR_RETURN(("duplicate beast id\t[%u %u]", pt->taskid, nodeid), -1);
			DECODE_XML_PROP_UINT32(ptn->ptarget.count, chl, "Count");
			decode_xml_prop_uint16_default(&ptn->ptarget.win, chl, "Win", 1);
		}

		if (!xmlStrcmp(chl->name, (const xmlChar *)"PetTarget")) {
			if (ptn->pettarget.petcnt) 
				ERROR_RETURN(("duplicate beast id\t[%u %u]", pt->taskid, nodeid), -1);
			ptn->pettarget.petcnt = decode_xml_prop_arr_int_default((int *)ptn->pettarget.pettype, MAX_PETS_PER_SPRITE, chl, "Type", 0);
			if (ptn->pettarget.petcnt != decode_xml_prop_arr_int_default((int *)ptn->pettarget.count, MAX_PETS_PER_SPRITE, chl, "Count", 0))
				ERROR_RETURN(("id cnt not match\t[%u %u]", pt->taskid, nodeid), -1);
		}

		if(!xmlStrcmp(chl->name, (const xmlChar *)"OutItem") && load_node_outopt(chl, ptn)) {
			return -1;
		}	

		if(!xmlStrcmp(chl->name, (const xmlChar *)"TimeCost")) {
			decode_xml_prop_uint32_default(&ptn->needtime, chl, "Time", 0);
		}
		chl = chl->next;
	}

	pt->node_cnt = pt->node_cnt < ptn->nodeid ? ptn->nodeid : pt->node_cnt;
	return 0;
}

void log_task(task_t* pt)
{
	char out[1024];
	int loop, j = 0;
	j += sprintf(out + j, "TASK\t[%u %u %u(", pt->taskid, pt->prelation, pt->ptaskcnt);
	for (loop = 0; loop < pt->ptaskcnt; loop ++) {
		j += sprintf(out + j, "%u ", pt->ptasklist[loop]);
	}
	j += sprintf(out + j, ")]\t[%u %u %u %u %02x %u]", \
		pt->repeat, pt->repeat_cnt, pt->need_lv, pt->max_lv, pt->need_job, pt->need_joblv);
	out[j] = '\0';
	KTRACE_LOG(0, "%s", out);

	j = 0;
	j += sprintf(out + j, "TASK NODE:%u\t", pt->node_cnt);
	for (loop = 0; loop < pt->node_cnt; loop ++) {
		task_node_t* ptn = &pt->nodes[loop];
		j += sprintf(out + j, "[%u %u ", ptn->nodeid, ptn->isend);
		if (ptn->usetarget.itemid)
			j += sprintf(out + j, "ut:(%u %u) ", ptn->usetarget.itemid, ptn->usetarget.count);
		if (ptn->itarget.itemid)
			j += sprintf(out + j, "it:(%u %u %u %u %u) ", ptn->itarget.itemid, ptn->itarget.count, ptn->itarget.beastid, ptn->itarget.dropodds, ptn->itarget.del);
		if (ptn->btarget.beastid)
			j += sprintf(out + j, "bt:(%u %u %u %u) ", ptn->btarget.beastid, ptn->btarget.count, ptn->btarget.win, ptn->btarget.type);
		if (ptn->ptarget.count)
			j += sprintf(out + j, "bt:(%u %u) ", ptn->ptarget.count, ptn->ptarget.win);
		j += sprintf(out + j, "%u]", ptn->optcnt);
	}

	out[j] = '\0';
	KTRACE_LOG(0, "%s", out);
}

void init_all_tasks()
{
	all_tasks = g_hash_table_new(g_int_hash, g_int_equal);
}

static gboolean free_task_by_key(gpointer key, gpointer ptask, gpointer userdata)
{
	g_slice_free1(sizeof(task_t), ptask);
	return TRUE;
}

void fini_all_tasks()
{
	g_hash_table_destroy(all_tasks);
}

int load_tasks(xmlNodePtr cur_task)
{
	g_hash_table_foreach_remove(all_tasks, free_task_by_key, 0);
	cur_task = cur_task->xmlChildrenNode; 
	while (cur_task) {
		if (!xmlStrcmp(cur_task->name, (const xmlChar*)"Tasks")) {
			xmlNodePtr cur = cur_task->xmlChildrenNode;
			while (cur) {
				if (!xmlStrcmp(cur->name, (const xmlChar*)"Task")) {
					uint32_t id;
					DECODE_XML_PROP_UINT32(id, cur, "ID");
					task_t* pt = get_task(id);
					if (pt) ERROR_RETURN(("duplicate task id=%u", id), -1);

					pt = (task_t *)g_slice_alloc0(sizeof(task_t));
					pt->taskid = id;
					g_hash_table_insert(all_tasks, &pt->taskid, pt);
					decode_xml_prop_uint16_default(&pt->prelation, cur, "ParentCost", 0);
					pt->ptaskcnt = decode_xml_prop_arr_int_default((int *)pt->ptasklist, MAX_PARENT_TASK_CNT, cur, "ParentList", 0);
					uint32_t loop = 0;
					for (loop = 0; loop < pt->ptaskcnt; loop ++) {
						if (pt->ptasklist[loop] && !get_task(pt->ptasklist[loop]))
							ERROR_RETURN(("ptaskid invalid\t[%u %u]", pt->taskid, pt->ptasklist[loop]), -1);
					}
					decode_xml_prop_uint32_default(&pt->type, cur, "Kind", TASK_TYPE_MAJOR);
					decode_xml_prop_uint32_default(&pt->repeat, cur, "RepeatType", 0);
					if (pt->repeat) {
						DECODE_XML_PROP_UINT32(pt->repeat_cnt, cur, "RepeatCnt");
						decode_xml_prop_uint32_default(&pt->startday, cur, "StartDay", 5);
					}

					uint32_t time[300];
					uint32_t cnt = decode_xml_prop_arr_time(time, 300, cur, "Time");
					if (!cnt) {
						for (loop = 0; loop < DAYS_PER_WEEK; loop ++) {
							pt->time_valid[loop] = 0xFFFFFFFF;
						}
					} else if (cnt % 3) {
						KERROR_LOG(0, "time cnt err\t[%u]", cnt);
						return -1;
					} else {
						for (loop = 0; loop < cnt / 3; loop ++) {
							if (time[loop * 3 + 0] == 7) time[loop * 3 + 0] = 0;
							if (time[loop * 3 + 0] >= DAYS_PER_WEEK) {
								KERROR_LOG(0, "invalid day\t[%u]", time[loop * 3 + 0]);
								return -1;
							}
							for (uint32_t loopi = time[loop * 3 + 1]; loopi < time[loop * 3 + 2]; loopi ++) {
								if (loopi >= 24) {
									KERROR_LOG(0, "invalid time\t[%u]", loopi);
									return -1;
								}
								pt->time_valid[time[loop * 3 + 0]] |= 1 << loopi;
							}
						}
					}

					int joblist[5];
					uint32_t jobcnt = decode_xml_prop_arr_int_default(joblist, 5, cur, "Job", -1);
					for (loop = 0; loop < jobcnt; loop ++) {
						if (joblist[loop] == -1 || joblist[loop] == -2) {
							pt->need_job = -1;
							break;
						}else if (joblist[loop] == -3 ) {
							pt->need_job |= (1<<6)|(1<<7)|(1<<8)|(1<<9);
							break;
						}
						pt->need_job |= (1 << joblist[loop]);
					}

					int skilllimt[3];
					int scnt = decode_xml_prop_arr_int_default(skilllimt, 3, cur, "SkillLimit", 0);
					if (scnt && !get_skill_info(skilllimt[0]))
						ERROR_RETURN(("invlaid skill\t[%u]", skilllimt[0]), -1);
					pt->need_skill = skilllimt[0];
					pt->min_skill_lv = skilllimt[1];
					pt->max_skill_lv = scnt == 2 ? MAX_SKILL_LEVEL : skilllimt[2];
					
					DECODE_XML_PROP_UINT32(pt->need_lv, cur, "Level");
					decode_xml_prop_uint32_default(&pt->max_lv, cur, "MaxLevel", MAX_SPRITE_LEVEL);
					DECODE_XML_PROP_UINT32(pt->need_joblv, cur, "JobLevel");
					if (pt->need_lv > pt->max_lv)
						ERROR_RETURN(("lv invalid\t[%u %u %u]", pt->taskid, pt->need_lv, pt->max_lv), -1);
					
					xmlNodePtr chl = cur->xmlChildrenNode; 
					//DEBUG_LOG("go in taskid=%u",id);
					while(chl)
					{
						if (!xmlStrcmp(chl->name, (const xmlChar *)"Node") && load_task_node(chl, pt))
							return -1;

						chl = chl->next;	
					}
					//DEBUG_LOG("go out taskid=%u",id);
					for (loop = 0; loop < pt->node_cnt; loop ++) {
						if (!pt->nodes[loop].nodeid)
							ERROR_RETURN(("task lost node info\t[%u %u]", pt->taskid, loop), -1);
					}

					pt->nodes[pt->node_cnt - 1].isend = 1;
					log_task(pt);
				}
				cur = cur->next;
			}
		}
		cur_task = cur_task->next;
	}

	return 0;
}

task_loop_t* get_task_loop_nochk(uint32_t loopid)
{
	if (loopid <= TASK_LOOP_BASE || loopid > TASK_LOOP_BASE + MAX_TASK_LOOP_CNT)
		return NULL;
	return &task_loops[loopid - TASK_LOOP_BASE - 1];
}

task_loop_t* get_task_loop(uint32_t loopid)
{
	task_loop_t* ptl = get_task_loop_nochk(loopid);
	return ptl && ptl->loopid ? ptl : NULL;
}

int load_task_loops(xmlNodePtr cur_task)
{
	memset(task_loops, 0, sizeof(task_loops));
	cur_task = cur_task->xmlChildrenNode; 
	while (cur_task) {
		if (!xmlStrcmp(cur_task->name, (const xmlChar*)"TaskLoops")) {
			xmlNodePtr cur = cur_task->xmlChildrenNode;
			while (cur) {
				if (!xmlStrcmp(cur->name, (const xmlChar*)"TaskLoop")) {
					uint32_t id;
					DECODE_XML_PROP_UINT32(id, cur, "ID");
					id += TASK_LOOP_BASE;
					task_loop_t* ptl = get_task_loop_nochk(id);
					if (!ptl || ptl->loopid) {
						KERROR_LOG(0, "invalid or duplicate loop id\t[%u]", id);
						return -1;
					}

					ptl->loopid = id;

					ptl->taskcnt = decode_xml_prop_arr_int_default((int *)ptl->tasks, MAX_TASK_PER_LOOP, cur, "Tasks", 0);
					for (uint32_t loop = 0; loop < ptl->taskcnt; loop ++) {
						task_t* pt = get_task(ptl->tasks[loop]);
						if (!pt) {
							KERROR_LOG(0, "invalid task id in loop\t[%u %u]", id, ptl->tasks[loop]);
							return -1;
						}
						pt->loopid = id;
					}

					DECODE_XML_PROP_UINT32(ptl->mincnt, cur, "MinTaskCnt");
					DECODE_XML_PROP_UINT32(ptl->maxcnt, cur, "MaxTaskCnt");
					DECODE_XML_PROP_INT_DEFAULT(ptl->maxsame, cur, "MaxSame", 1);
					if (ptl->maxcnt < ptl->mincnt || ptl->maxcnt > ptl->maxsame * ptl->taskcnt) {
						KERROR_LOG(0, "task cnt invalid\t[%u %u %u %u]", ptl->mincnt, ptl->maxcnt, ptl->maxsame, ptl->taskcnt);
						return -1;
					}

					decode_xml_prop_uint32_default(&ptl->startid, cur, "StartTask", 0);
					DECODE_XML_PROP_UINT32(ptl->endid, cur, "EndTask");

					if (ptl->startid) {
						task_t* pstart = get_task(ptl->startid);
						if (!pstart)
							return -1;
						pstart->loopid = id;
					}

					if (ptl->endid) {
						task_t* pend = get_task(ptl->endid);
						if (!pend)
							return -1;
						pend->loopid = id;
					}

					DECODE_XML_PROP_INT_DEFAULT(ptl->loopcnt, cur, "LoopCnt", 1);
				}
				cur = cur->next;
			}
		}
		cur_task = cur_task->next;
	}

	return 0;
}


int get_task_all(sprite_t* p, Cmessage* c_in)
{
	return send_request_to_db(p, p->id, proto_db_get_task_all, NULL, 0);
}

typedef struct task_simple {
	uint32_t		taskid;
	uint32_t		state;
	uint32_t		optdate;
	uint32_t		fintime;
	uint32_t		finnum;
}__attribute__((packed))task_simple_t;

int get_task_all_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t fincnt, on_cnt;
	int j = 0;
	
	UNPKG_H_UINT32(buf, on_cnt, j);
	UNPKG_H_UINT32(buf, fincnt, j);
	KDEBUG_LOG(p->id, "TASK INFO DB\t[%u %u]", fincnt, on_cnt);

	CHECK_BODY_LEN(len, 8 + fincnt * sizeof(task_simple_t) + on_cnt * sizeof(task_db_t));
	get_task_all_out out;
	switch (p->waitcmd) {
		case get_task_all_cmd:
			for (uint32_t loop = 0; loop < on_cnt; loop ++) {
				task_db_t* ptd = (task_db_t*)(buf + 8 + sizeof(task_db_t) * loop);
				if (ptd->taskid > TASK_LOOP_BASE)
					continue;
				task_t* pt = get_task(ptd->taskid);
				if (!pt) continue;
				if ((pt->repeat && !check_same_period(ptd->optdate, pt->repeat, pt->startday))) {
					if (pt->loopid) continue;
					stru_task_other otask;
					otask.taskid = ptd->taskid;
					otask.state = TASK_STATE_FIN_RECV;
					otask.finnum = 0;
					otask.fintime = ptd->fintime;
					out.othertasks.push_back(otask);
				} else {
					task_node_t* ptn = get_task_node(ptd->taskid, ptd->nodeid + 1);
					stru_task_recv rtask;
					rtask.taskid = ptd->taskid;
					rtask.nodeid = ptd->nodeid;
					rtask.finnum = ptd->finnum;
					rtask.fintime = ptd->fintime;
					rtask.itemid = ptn ? ptn->usetarget.itemid : 0;
					rtask.count = ptd->svr_buf[svr_buf_item];
					rtask.optdate=ptd->optdate;
					memcpy(rtask.cli_buff, ptd->cli_buf, sizeof(rtask.cli_buff) );
					out.recvtasks.push_back(rtask);
				}
			}

			for (uint32_t loop = 0; loop < fincnt; loop ++) {	
				task_simple_t* pts = (task_simple_t *)(buf + 8 + on_cnt * sizeof(task_db_t) + loop * sizeof(task_simple_t));
				if (pts->taskid > TASK_LOOP_BASE)
					continue;
				task_t* pt = get_task(pts->taskid);
				if (!pt) continue;
				if (pt->repeat) {
					bool same_period = check_same_period(pts->optdate, pt->repeat, pt->startday);
					if (pt->loopid && !same_period)
						continue;
					pts->finnum = same_period ? pts->finnum : 0;
					if (pts->state == TASK_STATE_FIN && pts->finnum < pt->repeat_cnt) {
						pts->state = TASK_STATE_FIN_RECV;
					}
				}
				stru_task_other otask;
				otask.taskid = pts->taskid;
				otask.state = pts->state;
				otask.finnum = pts->finnum;
				otask.fintime = pts->fintime;
				out.othertasks.push_back(otask);
				//DEBUG_LOG("xxxxxxx %u",pts->taskid);	
			}
			
			return send_msg_to_self(p, p->waitcmd, &out, 1);
		case proto_cli_login:
			cache_init_task_info(p, fincnt, on_cnt, buf + 8);
			if(IS_OLD_PROF(p->prof)&& cache_check_task_fin(p,207)){
				DEBUG_LOG("del 207");
				uint32_t tid=207;
				g_hash_table_remove(p->fintasks, &tid);
				uint8_t out[64];
				int i=0;
				PKG_H_UINT32(out,tid,i);
				send_request_to_db(NULL,p->id,proto_db_del_task,out,i);
			}
			return proc_final_login_step(p);
	}
	return 0;
}

int cli_get_loop_task(sprite_t* p, Cmessage* c_in)
{
	cli_get_loop_task_out cli_out;
	for (uint32_t loop = 0; loop < MAX_TASK_LOOP_CNT; loop ++) {
		if (task_loops[loop].loopid) {
			stru_loop_task_info looptask;
			loop_db_t* pld = cache_get_loop_task(p, task_loops[loop].loopid);
			if (!pld)
				pld = cache_add_loop_task(p, NULL, task_loops[loop].loopid);
			update_loop_task_state(p, pld, &task_loops[loop]);
			looptask.loopid = pld->loopid;
			looptask.fincnt = pld->finnum;
			looptask.taskidx = pld->finidx;
			for (uint32_t loopi = 0; loopi < pld->taskcnt; loopi ++) {
				looptask.tasks.push_back(pld->ids[loopi]);
			}
			cli_out.looptasks.push_back(looptask);
		}
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cache_init_task_info(sprite_t* p, uint32_t fincnt, uint32_t oncnt, uint8_t* buf)
{
	uint32_t loop;
	for (loop = 0; loop < oncnt; loop ++) {
		task_db_t* ptd = (task_db_t*)(buf + sizeof(task_db_t) * loop);
		if (ptd->taskid > TASK_LOOP_BASE && get_task_loop(ptd->taskid)) {
			cache_add_loop_task(p, (loop_db_t *)ptd, 0);
		} else if (ptd->taskid < TASK_LOOP_BASE) {
			if (ptd->fintime)
				cache_add_ever_fin_task(p, ptd->taskid);
			
			task_t* pt = get_task(ptd->taskid);
			if (!pt || (pt->repeat && !check_same_period(ptd->optdate, pt->repeat, pt->startday))) 
				continue;
			cache_add_recvd_task(p, ptd);
		}
	}

	for (loop = 0; loop < fincnt; loop ++) {
		task_simple_t* pts = (task_simple_t *)(buf + oncnt * sizeof(task_db_t) + loop * sizeof(task_simple_t));
		if (pts->state == TASK_STATE_FIN) {
			cache_add_fin_task(p, pts->taskid);
		} else if (pts->fintime) {
			cache_add_ever_fin_task(p, pts->taskid);
		}
	}
	return 0;
}

int cache_ptask_check(sprite_t* p, task_t* pt)
{
	if (pt->loopid) {
		loop_db_t* ploop = cache_get_loop_task(p, pt->loopid);
		if (!ploop || ploop->finidx == ploop->taskcnt || pt->taskid != ploop->ids[ploop->finidx] \
			|| !check_same_period(ploop->optdate, repeat_day, 5)) {
			KERROR_LOG(p->id, "xxx\t[%u %u]", pt->taskid, ploop ? ploop->ids[ploop->finidx] : 0);
			return 1;
		}
		return 0;
	}
	
	if (!pt->ptaskcnt)
		return 0;

	int cnt = 0;
	for (int loop = 0; loop < pt->ptaskcnt; loop ++) {
		if (cache_check_task_fin(p, pt->ptasklist[loop]))
			cnt ++;
	}
	
	return cnt < pt->prelation;
}

int recv_task_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t taskid, clival;
	uint8_t cli_buf[TASK_CLI_BUF_LEN] = {0};
	int j = 0;

	UNPKG_UINT32(body, taskid, j);
	UNPKG_STR(body, cli_buf, j, TASK_CLI_BUF_LEN);
	UNPKG_UINT32(body, clival, j);
	KDEBUG_LOG(p->id, "RECV TASK\t[%u %u]", taskid, clival);
	task_t* pt = get_task(taskid);
	CHECK_TASK_ID_VALID(p, pt, taskid);

	if (!(pt->time_valid[get_now_tm()->tm_wday] & (1 << get_now_tm()->tm_hour))) {
		KERROR_LOG(p->id, "time invalid\t[%x %u]", pt->time_valid[get_now_tm()->tm_wday], get_now_tm()->tm_hour);
		return send_to_self_error(p, p->waitcmd, cli_err_not_right_time, 1);
	}

	CHECK_LV_FIT(p, p, pt->need_lv, pt->max_lv);
	CHECK_PROF_FIT(p, pt->need_job, pt->need_joblv);

	if (pt->need_skill) {
		skill_t* ps = cache_get_skill(p, pt->need_skill);
		if (!ps || ps->skill_level < pt->min_skill_lv || ps->skill_level > pt->max_skill_lv) {
			KERROR_LOG(p->id, "need skill\t[%u %u %u %u]", pt->taskid, pt->need_skill, pt->min_skill_lv, pt->max_skill_lv);
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
	}
	
	task_db_t* ptr = cache_get_task_info(p, taskid);
	if (ptr) {
		KERROR_LOG(p->id, "task recvd\t[%u %u %u]", taskid, ptr->nodeid, clival);
		return rsp_task_update_db(p, taskid, ptr->nodeid, 0, 0, clival);
	} 

	if (g_hash_table_size(p->recvtasks) >= MAX_RECV_TASK_CNT) {
		KERROR_LOG(p->id, "task full");
		return send_to_self_error(p, p->waitcmd, cli_err_task_reach_max, 1);
	}

	if (cache_ptask_check(p, pt)) {
		KERROR_LOG(p->id, "ptask check fail\t[%u]", pt->taskid);
		return send_to_self_error(p, p->waitcmd, cli_err_ptask_not_fin, 1);
	}
	
	if (!pt->repeat && !pt->loopid && cache_check_task_fin(p, pt->taskid)) {
		KERROR_LOG(p->id, "RECV TASK FIN\t[%u]", pt->taskid);
		return send_to_self_error(p, p->waitcmd, cli_err_task_have_finished, 1);
	}
		
	
	*(uint32_t *)(p->session + 4 + TASK_CLI_BUF_LEN) = clival;
	*(uint32_t*)p->session = taskid;
	memcpy(p->session + 4, cli_buf, TASK_CLI_BUF_LEN);
	*(uint32_t *)(p->session + 8 + TASK_CLI_BUF_LEN) = pt->repeat;
	*(uint32_t *)(p->session + 12 + TASK_CLI_BUF_LEN) = pt->repeat_cnt;
	*(uint32_t *)(p->session + 16 + TASK_CLI_BUF_LEN) = pt->startday;
	return send_request_to_db(p, p->id, proto_db_get_task_db, &taskid, 4);
}

int get_task_db_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	task_db_t* ptd = (task_db_t *)buf;
	task_db_t tdb;
	memset(&tdb, 0, sizeof(task_db_t));
	uint32_t repeat;

	switch (p->waitcmd) {
		case proto_cli_recv_task:
			if (ret) {
				ptd = &tdb;
				ptd->taskid = *(uint32_t *)p->session;
				ptd->state = TASK_STATE_NONE;
			}

			repeat = *(uint32_t *)(p->session + 8 + TASK_CLI_BUF_LEN);
			if (repeat) {	
				uint32_t repeat_cnt = *(uint32_t *)(p->session + 12 + TASK_CLI_BUF_LEN);
				uint32_t startday = *(uint32_t *)(p->session + 16 + TASK_CLI_BUF_LEN);
				ptd->finnum = check_same_period(ptd->optdate, repeat, startday) ? ptd->finnum : 0;
				if (ptd->finnum >= repeat_cnt) 
					return send_to_self_error(p, p->waitcmd, cli_err_task_repeat_max, 1);
			}

			if (ptd->state != TASK_STATE_CANCEL) {
				msg_log_task(ptd->taskid, 20);
				if (ptd->taskid == 39001)
					msg_log_task_39001(p->id, 1, 0, p->level);
				ptd->nodeid = 0;
			}

			ptd->state = TASK_STATE_RECVD;
			ptd->optdate = get_now_tv()->tv_sec;
			memcpy(ptd->cli_buf, p->session + 4, TASK_CLI_BUF_LEN);
			memset(ptd->svr_buf, 0, TASK_SVR_BUF_LEN);
			return send_request_to_db(p, p->id, proto_db_set_task_db, ptd, sizeof(task_db_t));
		default:
			return -1;
	}
	return 0;
}

void try_update_loop_state(sprite_t* p, uint32_t loopid)
{
	if (!loopid) 
		return;

	loop_db_t* ploop = cache_get_loop_task(p, loopid);
	if (!ploop)
		return;
	
	ploop->finidx += 1;
	ploop->optdate = get_now_tv()->tv_sec;
	if (ploop->finidx == ploop->taskcnt) {
		ploop->finnum ++;
		task_loop_t* ptl = get_task_loop(loopid);
		if (ptl && ploop->finnum < ptl->loopcnt) {
			gen_spec_loop_task(ploop, ptl);
			noti_cli_task_loop_update_out cli_out;
			cli_out.looptask.loopid = ploop->loopid;
			cli_out.looptask.fincnt = ploop->finnum;
			cli_out.looptask.taskidx = ploop->finidx;
			for (uint32_t loop = 0; loop < ploop->taskcnt; loop ++) {
				cli_out.looptask.tasks.push_back(ploop->ids[loop]);
			}
			send_msg_to_self(p, noti_cli_task_loop_update_cmd, &cli_out, 0);
		}
	}
	send_request_to_db(NULL, p->id, proto_db_set_task_db, ploop, sizeof(loop_db_t));	
}

int finish_task_last_step(sprite_t* p, task_db_t* ptd)
{	
	msg_log_task(ptd->taskid, ptd->nodeid);
	if (ptd->state == TASK_STATE_FIN) {
		uint32_t loopid = *(uint32_t *)(p->session + 8);
		KDEBUG_LOG(p->id, "TASK ALL FIN\t[%u %u %u]", ptd->taskid, ptd->nodeid, loopid);
		cache_del_recvd_task(p, ptd->taskid);
		cache_add_fin_task(p, ptd->taskid);
		try_to_send_for_task(p, ptd->taskid);
		try_update_loop_state(p, loopid);

		if (ptd->taskid == 39001)
			msg_log_task_39001(p->id, 0, 1, p->level);
	} else {
		task_db_t* ptr = *(task_db_t **)(p->session + 12);
		memcpy(ptr, ptd, sizeof(task_db_t));
		KDEBUG_LOG(p->id, "UPDATE TASK NODE ID\t[%u %u]", ptr->taskid, ptr->nodeid);
	}
	
	return 0;
}

int set_task_db_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	task_db_t* ptd = (task_db_t *)buf;
	cache_add_recvd_task(p, ptd);
	KDEBUG_LOG(p->id, "SET TASK DB CALLBACK\t[%u %u %u %u]", ptd->taskid, ptd->nodeid, ptd->state, ptd->optdate);
	int info = 0, outid = 0;
	uint32_t clival = 0;
	switch (p->waitcmd) {
		case proto_cli_recv_task:
			cache_del_fin_task(p, ptd->taskid);
			clival = *(uint32_t *)(p->session + 4 + TASK_CLI_BUF_LEN);
			//勇士的修行
			if(ptd->taskid==TRAIN_TASK_ID){
				p->trainning_start_time=get_now_sec();
				KDEBUG_LOG(p->id,"set_task_db_callback: start a rank task[taskid=%u starttime=%u]",\
						ptd->taskid,p->trainning_start_time);
			}
			break;
		case finish_task_node_cmd:	
			outid = *(uint32_t *)p->session;
			clival = *(uint32_t *)(p->session + 4);		
			break;
		case proto_cli_cancel_task:
			cache_del_recvd_task(p, ptd->taskid);
			response_proto_uint32(p, p->waitcmd, ptd->taskid, 1, 0);
			return 0;
		case proto_cli_set_task_clibuf:
			clival = *(uint32_t *)p->session;
			memcpy(*(task_db_t **)(p->session + 4), ptd, sizeof(task_db_t));
			break;
		default:
			return -1;
	}
	KDEBUG_LOG(p->id, "RESPONSE USER TASK STATE\t[%u %u %u %u]", ptd->taskid, ptd->nodeid, info, clival);
	return rsp_task_update_db(p, ptd->taskid, ptd->nodeid, outid, info, clival);
}

body_cloth_t* cache_get_type_cloth(sprite_t* p, uint32_t clothtype)
{
	cloth_t* pc = get_cloth(clothtype);
	body_cloth_t* pbc = cache_get_body_cloth(p, pc->equip_part + BASE_BODY_TYPE);

	if (!pbc || pbc->clothtype != clothtype) {
		pbc = NULL;
		GList* pcloths = g_hash_table_get_values(p->all_cloths);
		GList* head = pcloths;
		while (pcloths) {
			pbc = (body_cloth_t *)pcloths->data;
			if (pbc->clothtype != clothtype)
				pbc = NULL;
			else
				break;
			
			pcloths = pcloths->next;
		}
		g_list_free(head);
	}

	return pbc;
}

int task_node_fin_check(sprite_t* p, task_node_t* ptn, task_db_t* ptr)
{
	if (ptn->itarget.itemid && ptn->itarget.check) {
		if (get_item(ptn->itarget.itemid) && !cache_item_have_cnt(p, ptn->itarget.itemid, ptn->itarget.count))
			ERROR_RETURN(("item not enough: %u %u %u %u", \
				p->id, ptr->taskid, ptn->itarget.itemid, ptn->itarget.count), -1);

		if (get_cloth(ptn->itarget.itemid) && !cache_get_type_cloth(p, ptn->itarget.itemid))
			ERROR_RETURN(("item not enough: %u %u %u %u", \
				p->id, ptr->taskid, ptn->itarget.itemid, ptn->itarget.count), -1);
	}

	if (ptn->btarget.beastid && ptr->svr_buf[svr_buf_beast] < ptn->btarget.count) {
		ERROR_RETURN(("beast target count\t[%u %u %u %u]", \
			p->id, ptr->taskid, ptr->svr_buf[svr_buf_beast], ptn->btarget.count), -1);
	}

	if (ptn->usetarget.itemid && ptr->svr_buf[svr_buf_item] < ptn->usetarget.count) {
		ERROR_RETURN(("use item count\t[%u %u %u %u]", \
			p->id, ptr->taskid, ptr->svr_buf[svr_buf_item], ptn->btarget.count), -1);
	}

	return 0;
}

int reward_pet_task_exp(sprite_t* p, pet_t* pp, uint32_t add_exp)
{
	pp->experience += add_exp;
	LIMIT_MAX_EXP(pp->experience, p->id);
	LIMIT_PET_MAX_EXP(pp->experience, p->id);
	uint32_t prelv = pp->level;
	pp->level = calc_pet_level_from_exp(pp->experience, pp->level);
	int ret = 0;


	if (pp->level > prelv) {
		calc_pet_5attr(pp);
		calc_pet_second_level_attr(pp);
		pp->attr_addition += pp->level - prelv;
		if (pp->level > p->level + 5) {
			pp->action_state = just_in_bag;
			db_set_pet_state(0, p->id, pp->petid, just_in_bag);
		}
		pp->hp = pp->hp_max;
		pp->mp = pp->mp_max;
		msg_log_pet_level(pp->level, prelv);
		update_pet_skill_level(p, pp);
		ret = 1;
	}
	
	db_add_pet_exp_level(p, pp);
	return ret;
}

void reward_user_skill_exp(sprite_t* p, skill_t* ps, uint32_t exp)
{
	skill_info_t* psi = get_skill_info(ps->skill_id);
	if (psi) {
		cache_update_skill_level(p, ps, psi, ps->skill_exp + exp);
		db_update_skill_info(NULL, p->id, ps);
	}
}

static inline int
reward_user_base_attr(sprite_t* p, task_reward_t* p_tr, task_option_t* p_opt)
{
	uint32_t add_exp = 0, add_lv = 0, add_addition = 0;
	p_tr->xiaomee = p->xiaomee;
	p_tr->level = p->level;
	p_tr->exp = p->experience;
    p_tr->expbox = p->expbox;
	p_tr->skill_expbox = p->skill_expbox;
	p_tr->attr_addition = p->attr_addition;
	p_tr->hp = p->hp;
	p_tr->mp = p->mp;

	int full_reward = IS_ADMIN(p) || get_ol_time(p) < 3 * 60 * 60;
	*(uint32_t *)(p->session + 64) = !full_reward;
	for (uint32_t loop = 0; loop < p_opt->out_cnt; loop ++) {
		if (p_opt->items[loop].type == OUT_TYPE_PLAYER_ATTR) {
			task_t* pt = get_task(p_tr->task.taskid);
			double ratio = 1.0;
			if (pt && (pt->type == TASK_TYPE_DAY || pt->type == TASK_TYPE_STORY)) {
				ratio = pow(p->level, 0.9);
				if (pt->loopid) {
					loop_db_t* ploop = cache_get_loop_task(p, pt->loopid);
					ratio = ploop ? ratio * (ploop->finidx * 0.05 + 0.25) : ratio;
				}
			}

			uint32_t add_count = full_reward ? p_opt->items[loop].count : p_opt->items[loop].count / 2;
			switch (p_opt->items[loop].itemid) {
				case item_id_xiaomee:
					p_tr->xiaomee += add_count;
					break;
				case item_id_exp:
					add_exp += ratio * add_count;
					break;
				case item_id_pet_expbox:
					p_tr->expbox += ratio * add_count;
					break;
				case item_id_skill_expbox:
					p_tr->skill_expbox += add_count;
					break;
				case item_id_skill_exp:
					skill_t* ps = cache_get_skill(p, p_opt->items[loop].skillid);
					if (!ps) {
						KERROR_LOG(p->id, "skill id not have\t[%u %u]", p_opt->items[loop].skillid, add_count);
						return cli_err_skill_not_have;
					}
					reward_user_skill_exp(p, ps, add_count);
					break;
			}
		}
	}

	uint32_t user_add_exp = add_exp;
	if (p->experience >= MAX_TEMP_EXP_EX - 1)
		user_add_exp = 0;

	if (p->experience + user_add_exp >= MAX_TEMP_EXP_EX)
		user_add_exp = MAX_TEMP_EXP_EX - p->experience - 1;

	p_tr->exp += user_add_exp;
	LIMIT_MAX_EXP(p_tr->exp, p->id);
	LIMIT_USER_MAX_EXP(p_tr->exp, p->id);
	p_tr->level = calc_level_from_exp(p_tr->exp, p_tr->level);
	add_lv = p_tr->level - p->level;
	add_addition = 4 * add_lv;
	p_tr->attr_addition += add_addition;
	if (add_lv) {
		p_tr->hp = p->hp_max;
		p_tr->mp = p->mp_max;
	}

	noti_pet_levelup_out cli_out;
	for (uint32_t loop = 0; loop < p->pet_cnt && add_exp; loop ++) {
		if (p->pets_inbag[loop].action_state == rdy_fight \
			|| p->pets_inbag[loop].action_state == for_assist) {
			if (reward_pet_task_exp(p, &p->pets_inbag[loop], add_exp)) {
				stru_pet_lvinfo petlv;
				petlv.petid = p->pets_inbag[loop].petid;
				petlv.level = p->pets_inbag[loop].level;
				petlv.attraddtion = p->pets_inbag[loop].attr_addition;
				cli_out.petlist.push_back(petlv);
			}
		}
	}

	if (cli_out.petlist.size()) {
		send_msg_to_self(p, noti_pet_levelup_cmd, &cli_out, 0);
	}
	return 0;
}

static inline int
reward_user_cloths(sprite_t* p, task_reward_t* p_tr, task_option_t* p_opt, uint8_t* buf)
{
	p_tr->cloth_cnt = 0;
	for (uint32_t loop = 0; loop < p_opt->out_cnt; loop ++) {
		if (p_opt->items[loop].type == OUT_TYPE_CLOTHES){
			for (uint32_t loopin = 0; loopin < p_opt->items[loop].count; loopin ++){	
				if(!IS_PROF_MATCH(p,p_opt->items[loop].prof)) continue;
				cloth_t* pcloth = get_cloth(p_opt->items[loop].itemid);
				cloth_lvinfo_t* pclv = pcloth ? (p_opt->items[loop].level ? &pcloth->pro_attr[p_opt->items[loop].level - 1] : &pcloth->clothlvs[0]) : NULL;
				if (!pclv || !pclv->valid) {
					KERROR_LOG(p->id, "cloth id invalid\t[%u]", p_opt->items[loop].itemid);
					return cli_err_client_not_proc;
				}
				gen_cloth_attr(pcloth, 0, pclv, (cloth_attr_t *)buf + p_tr->cloth_cnt, 0);
				p_tr->cloth_cnt ++;
			}		
		}
	}
	if (p_tr->cloth_cnt + get_bag_cloth_cnt(p) > get_max_grid(p))
		return cli_err_bag_full;
	return 0;
}

static inline int
reward_user_normal_items(sprite_t* p, task_reward_t* p_tr, task_option_t* p_opt, uint8_t* buf)
{
	p_tr->item_cnt = 0;
	int ret = 0;
	item_t add_items[100];
	memset(add_items, 0, sizeof(add_items));
	DEBUG_LOG("reward_user_normal_items %u",p_opt->out_cnt);
	for (uint32_t loop = 0; loop < p_opt->out_cnt; loop ++) {
		DEBUG_LOG("items %u",p_opt->items[loop].type);
		if (p_opt->items[loop].type == OUT_TYPE_NORMAL_ITEM){
			item_t* p_item = (item_t *)buf + p_tr->item_cnt;
			p_item->itemid = p_opt->items[loop].itemid;
			p_item->count = p_opt->items[loop].count;
			normal_item_t* pni = get_item(p_item->itemid);
			if (!pni) {
 				KERROR_LOG(p->id, "itemid invalid\t[%u]", p_opt->items[loop].itemid);
 				ret = cli_err_client_not_proc;
				break;
 			}

			add_items[loop].itemid = p_item->itemid;

			add_items[loop].count = cache_add_kind_item(p, p_item->itemid, p_item->count);
			if (add_items[loop].count != p_item->count) {
 				KERROR_LOG(p->id, "bag_full");
				ret = cli_err_bag_full;
				break;
			}
			p_tr->item_cnt ++;
		}
	}

	DEBUG_LOG("reward_user_normal_items %u",p_tr->item_cnt);
	for (uint32_t loop = 0; loop < p_opt->out_cnt; loop ++) {
		if (add_items[loop].itemid && add_items[loop].count)
			cache_reduce_kind_item(p, add_items[loop].itemid, add_items[loop].count);
	}

	return ret;
}

static inline int
reward_user_pets(sprite_t* p, task_reward_t* p_tr, task_option_t* p_opt, uint8_t* buf)
{
	p_tr->pet_cnt = 0;

	uint8_t pet_cnt_ready = get_pet_rdy_fight(p) ? 1 : 0;
	uint8_t pet_cnt_actual = p->pet_cnt_actual;
	uint8_t pet_cnt_assist = p->pet_cnt_assist;
	uint8_t pet_cnt_standby = p->pet_cnt_standby;

	for (uint32_t loop = 0; loop < p_opt->out_cnt; loop ++) {
		//KDEBUG_LOG(p->id, "TASK OPT ITEM\t[%u %u %u %u %u]", loop, p_opt->out_id, p_opt->out_cnt, p_opt->items[loop].type, p_opt->items[loop].itemid);
		if (p_opt->items[loop].type == OUT_TYPE_PET){
			for (uint32_t loopin = 0; loopin < p_opt->items[loop].count; loopin ++) {
				pet_db_t* pd = (pet_db_t *)(buf + p_tr->pet_cnt * (sizeof(pet_db_t) + MAX_SKILL_BAR * sizeof(skill_t)));
				beast_t* pb = get_beast(p_opt->items[loop].itemid);
				if (!pb) {
					KERROR_LOG(p->id, "pet id invalid\t[%u %u]", p_opt->items[loop].itemid, loop);
					return cli_err_client_not_proc;
				}

				gen_pet_attr(pd, pb, p_opt->items[loop].level, 0, IS_ADMIN(p) ? 0 : rand_diff_default);

				pet_cnt_actual ++;
				if (pet_cnt_actual >= MAX_PETS_PER_SPRITE) {
					pd->action_state = db_warehouse;
				} else if(pet_cnt_ready == 0) {
					pet_cnt_ready = 1;
					pet_cnt_standby++;
					pd->action_state = rdy_fight;
				} else if(pet_cnt_assist < 2) {
					pet_cnt_assist++;
					pd->action_state = for_assist;
				} else if(pet_cnt_standby < MAX_PET_STANDBY){
					pet_cnt_standby++;
					pd->action_state = on_standby;
				}

				init_pet_skills(pd, pb);
				p_tr->pet_cnt ++;
			}
		}
	}

	return 0;
}

static inline int
reward_user_honors(sprite_t* p, task_reward_t* p_tr, task_option_t* p_opt, uint8_t* buf)
{
	p_tr->proflv = p->prof_lv;
	p_tr->honor_cnt = 0;
	for (uint32_t loop = 0; loop < p_opt->out_cnt; loop ++) {
		if (p_opt->items[loop].type == OUT_TYPE_HONOR) {
			if(!IS_PROF_MATCH(p,p_opt->items[loop].prof)) continue;
			honor_attr_t* p_ha = get_honor_attr(p_opt->items[loop].itemid);
			if (!p_ha) {
				return cli_err_honor_id_invalid;
			}
			uint32_t pre_honor = get_pre_honor(p, p_ha);
			p_tr->proflv = p_ha->proflv > p_tr->proflv ? p_ha->proflv : p_tr->proflv;
			if (check_honor_exist(p, p_opt->items[loop].itemid))
				continue;

			*(uint32_t *)(buf + p_tr->honor_cnt * 8) = pre_honor;
			*(uint32_t *)(buf + p_tr->honor_cnt * 8 + 4) = p_opt->items[loop].itemid;
			uint32_t time = check_week_clear_honor(p_opt->items[loop].itemid) ? (get_next_sunday()):0;
			*(uint32_t *)(buf + p_tr->honor_cnt * 8 + 8) = time;
			p_tr->honor_cnt ++;
		}
	}
	return 0;
}

static inline int
reward_user_skills(sprite_t* p, task_reward_t* p_tr, task_option_t* p_opt, uint8_t* buf)
{
	p_tr->skill_cnt = 0;
	uint32_t battle_skill_cnt = 0, life_skill_cnt = 0;
	for (uint32_t loop = 0; loop < p_opt->out_cnt; loop ++) {
		if (p_opt->items[loop].type == OUT_TYPE_SKILL) {
			if (cache_get_skill(p, p_opt->items[loop].itemid))
				continue;
			if (IS_BATTLE_SKILL(p_opt->items[loop].itemid)) {
				battle_skill_cnt ++;
			} else if (IS_LIFE_SKILL(p_opt->items[loop].itemid)) {
				life_skill_cnt ++;
			}
			*(uint32_t *)(buf + p_tr->skill_cnt * sizeof(skill_t)) = p_opt->items[loop].itemid;
			*(uint8_t *)(buf + p_tr->skill_cnt * sizeof(skill_t) + 4) = 1;
			*(uint32_t *)(buf + p_tr->skill_cnt * sizeof(skill_t) + 5) = 0;
			p_tr->skill_cnt ++;
		}
	}
	if (battle_skill_cnt + p->skill_cnt > MAX_SKILL_BAR \
		|| life_skill_cnt + p->life_skill_cnt > MAX_SKILL_BAR)
		return cli_err_skill_cnt_max;
	return 0;
}


static inline int
db_add_task_reward_user(sprite_t* p, task_reward_t* p_reward, task_option_t* pto, finish_task_node_in* p_in)
{

//	|| !(j += p_reward->honor_cnt * 8)) 
	int j = sizeof(task_reward_t), ret = 0;
	uint8_t* out = (uint8_t *)p_reward;
	KDEBUG_LOG(p->id,"db_add_task_reward_user %u %u",pto->out_id,pto->out_cnt);
	if (((ret = reward_user_cloths(p, p_reward, pto, out + j)) \
		|| !(j += p_reward->cloth_cnt * sizeof(cloth_attr_t))) \
		|| ((ret = reward_user_normal_items(p, p_reward, pto, out + j)) \
		|| !(j += p_reward->item_cnt * sizeof(item_t))) \
		|| ((ret = reward_user_pets(p, p_reward, pto, out + j)) \
		|| !(j += p_reward->pet_cnt * (sizeof(pet_db_t) + MAX_SKILL_BAR * sizeof(skill_t)))) \
		|| ((ret = reward_user_honors(p, p_reward, pto, out + j)) \
		|| !(j += p_reward->honor_cnt * 12)) \
		||((ret = reward_user_skills(p, p_reward, pto, out + j)) \
		|| !(j += p_reward->skill_cnt * sizeof(skill_t))) \
		|| (ret = reward_user_base_attr(p, p_reward, pto))) {
		return send_to_self_error(p, p->waitcmd, ret, 1);
	}

	for (uint32_t loop = 0; loop < p_in->delpetlist.size(); loop ++) {
		PKG_H_UINT32(out, p_in->delpetlist[loop].petid, j);
		PKG_H_UINT32(out, p_in->delpetlist[loop].pettype, j);
	}

	return send_request_to_db(p, p->id, proto_db_task_reward, out, j);
}

int chk_delpet(sprite_t* p, task_reward_t* ptr, task_node_t* ptn, finish_task_node_in* p_in)
{
	if (!ptn->pettarget.petcnt)
		return 0;
	
	uint32_t count[MAX_PETS_PER_SPRITE] = {0};
	for (uint32_t loop = 0; loop < p_in->delpetlist.size(); loop ++) {
		for (uint32_t loopi = 0; loopi < ptn->pettarget.petcnt; loopi ++) {
			if (ptn->pettarget.pettype[loopi] == p_in->delpetlist[loop].pettype) {
				count[loopi] ++;
				break;
			}
		}
	}

	for (uint32_t loop = 0; loop < ptn->pettarget.petcnt; loop ++) {
		if (count[loop] != ptn->pettarget.count[loop])
			return -1;
	}

	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief   提交任务节点
 *
 * @param  p
 * @param  c_in
 *
 * @return  需要等待db返回
 */
/* ----------------------------------------------------------------------------*/
int finish_task_node(sprite_t* p, Cmessage* c_in)
{	
	finish_task_node_in* p_in = P_IN;
	KDEBUG_LOG(p->id, "FINISH TASK NODE\t[taskid=%u nodeid=%u outid=%u]", p_in->taskid, p_in->nodeid, p_in->outid);
	CHECK_USER_TIRED(p);

	task_t* pt = get_task(p_in->taskid);;
	CHECK_TASK_ID_VALID(p, pt, p_in->taskid);

	task_db_t* ptr = cache_get_task_info(p, p_in->taskid);
	CHECK_TASK_RECVD(p, ptr, p_in->taskid);

	if (ptr->nodeid >= p_in->nodeid || p_in->nodeid> pt->node_cnt) {
		*(uint32_t *)p->session = 0;
		*(uint32_t *)(p->session + 4) = p_in->clival;
		task_db_t tdb;
		memcpy(&tdb, ptr, sizeof(task_db_t));
		tdb.optdate = get_now_tv()->tv_sec;
		tdb.state = TASK_STATE_RECVD;
		memcpy(tdb.cli_buf, p_in->cli_buf, TASK_CLI_BUF_LEN);

		KDEBUG_LOG(p->id, "SEND TO DB\t[taskid=%u nodeid=%u state=%u]", tdb.taskid, tdb.nodeid, tdb.state);
		return send_request_to_db(p, p->id, proto_db_set_task_db, &tdb, sizeof(task_db_t));
	}

	task_node_t* ptn = &pt->nodes[p_in->nodeid - 1];	
	task_option_t* pto = get_node_out_opt(ptn, p_in->outid);
	if (!pto) {
		KERROR_LOG(p->id, "outid invalid\toutid=%u optcnt=%u", p_in->outid,ptn->optcnt);
		return -1;
	}
//  判断距上次设置buf的时间间隔，不满足则不能提交节点
	if(ptn->needtime && ptn->needtime >= (get_now_tv()->tv_sec - ptr->optdate)){
		KERROR_LOG(p->id, "not up to needtime\tneedtime=%u nowtime=%lu", ptn->needtime,(get_now_tv()->tv_sec - ptr->optdate));
		return send_to_self_error(p, p->waitcmd, cli_err_times_limit, 1);
	}

	//check task finish, if have svr buf to check 
	if (task_node_fin_check(p, ptn, ptr)) {
		return send_to_self_error(p, p->waitcmd, cli_err_task_cannot_fin, 1);
	}

	*(uint32_t *)p->session = ptn->itarget.del ? ptn->itarget.itemid : 0;
	*(uint32_t *)(p->session + 4) = ptn->itarget.count;
	*(uint32_t *)(p->session + 8) = pt->loopid;
	*(task_db_t* *)(p->session + 12) = ptr;

	*(uint32_t *)(p->session + 68) = p_in->clival;
	*(uint32_t *)(p->session + 72) = p_in->outid;

	uint8_t out[4096] = {0};
	task_reward_t* p_reward = (task_reward_t *)out;
	p_reward->task.taskid = ptr->taskid;
	p_reward->task.nodeid = ptn->nodeid;
	p_reward->task.optdate = get_now_tv()->tv_sec;
	p_reward->task.state = ptn->isend ? TASK_STATE_FIN : TASK_STATE_RECVD;
	p_reward->task.finnum = ptn->isend ? ptr->finnum + 1 : ptr->finnum;
	p_reward->task.fintime = !ptr->fintime && ptn->isend ? p_reward->task.optdate : ptr->fintime;
	memcpy(p_reward->task.cli_buf, p_in->cli_buf, TASK_CLI_BUF_LEN);
	if (chk_delpet(p, p_reward, ptn, p_in)) {
		return send_to_self_error(p, p->waitcmd, cli_err_task_cannot_fin, 1);
	}
	//DEBUG_LOG("============3333");
	return db_add_task_reward_user(p, p_reward, pto, p_in);
}

void del_task_need_item(sprite_t* p)
{
	uint32_t itemid = *(uint32_t *)p->session;
	uint32_t count = *(uint32_t *)(p->session + 4);

	if (itemid) {
		KDEBUG_LOG(p->id, "DEL ITEM\t[%u %u]", itemid, count);
		if (get_item(itemid)) {
			cache_reduce_kind_item(p, itemid, count);
			db_add_item(NULL, p->id, itemid, -count);
		} else if (get_cloth(itemid)) {
			body_cloth_t* pbc = cache_get_type_cloth(p, itemid);
			if (pbc) {
				db_del_cloth(NULL, p->id, pbc->clothid);
				cache_del_bag_cloth(p, pbc->clothid);
			}
		}
	}
}

void pkg_cli_cloth_info(body_cloth_t* apbc, uint32_t cloth_cnt, finish_task_node_out* p_out)
{
	for (uint32_t loop = 0; loop < cloth_cnt; loop ++) {
		stru_cloth_detail tmp_cloth;
		tmp_cloth.clothid 		= apbc[loop].clothid;
		tmp_cloth.clothtype 	= apbc[loop].clothtype;
		tmp_cloth.grid 			= apbc[loop].grid;
		tmp_cloth.quality		= apbc[loop].quality;
		tmp_cloth.validday		= apbc[loop].validday;
		tmp_cloth.duration_max	= apbc[loop].duration_max;
		tmp_cloth.duration		= apbc[loop].duration;
		tmp_cloth.hpmax			= apbc[loop].hp_max;
		tmp_cloth.mpmax			= apbc[loop].mp_max;
		tmp_cloth.attack 		= apbc[loop].attack;
		tmp_cloth.mattack 		= apbc[loop].mattack;
		tmp_cloth.defense 		= apbc[loop].defense;
		tmp_cloth.mdef			= apbc[loop].mdefense;
		tmp_cloth.speed 		= apbc[loop].speed;
		tmp_cloth.spirit 		= apbc[loop].spirit;
		tmp_cloth.resume 		= apbc[loop].resume;
		tmp_cloth.hit 			= apbc[loop].hit;
		tmp_cloth.dodge 		= apbc[loop].dodge;
		tmp_cloth.crit 			= apbc[loop].crit;
		tmp_cloth.fightback 	= apbc[loop].fightback;
		tmp_cloth.rpoison 		= apbc[loop].rpoison;
		tmp_cloth.rlithification= apbc[loop].rlithification;
		tmp_cloth.rlethargy 	= apbc[loop].rlethargy;
		tmp_cloth.rcommination	= apbc[loop].rinebriation;
		tmp_cloth.rconfusion 	= apbc[loop].rconfusion;
		tmp_cloth.roblivion 	= apbc[loop].roblivion;
		tmp_cloth.crystal_attr 	= apbc[loop].crystal_attr;

		cloth_t* psc = get_cloth(apbc[loop].clothtype);
		if (psc && apbc[loop].bless_type) {
			tmp_cloth.blesstype		= apbc[loop].bless_type & 0xFFFF;
			tmp_cloth.isactivate	= apbc[loop].bless_type & 0x10000;
			tmp_cloth.addval		= crystal_attr[tmp_cloth.blesstype	- 1][apbc[loop].quality][psc->minlv / 5];
		}
	}
}

int add_task_reward_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	del_task_need_item(p);

	task_reward_t* p_tr = (task_reward_t *)buf;

	finish_task_node_out cli_out;
	cli_out.taskid = p_tr->task.taskid;
	cli_out.nodeid = p_tr->task.nodeid;
	cli_out.ishalf = *(uint32_t *)(p->session + 64);
	cli_out.clival = *(uint32_t *)(p->session + 68);
	cli_out.outid = *(uint32_t *)(p->session + 72);
	if (p_tr->task.state == TASK_STATE_FIN) {
		task_t* pt = get_task(p_tr->task.taskid);
		cli_out.recvable = pt && ((pt->repeat && pt->repeat_cnt > p_tr->task.finnum) && !pt->loopid) ? 1 : 0;
		if(p_tr->task.taskid==TRAIN_TASK_ID){
			//特殊任务排行榜
			int32_t count= get_now_sec() - p->trainning_start_time;
			p->trainning_last_time +=count; 
			db_day_add_ssid_cnt(0,p->id,ssid_trainning_last_time,count,-1);
			KDEBUG_LOG(p->id,"add_task_reward_callback:finish a rank task[lasttime=%u starttime=%u besttime=%u]",
						p->trainning_last_time,p->trainning_start_time,p->trainning_least_time);
			if(p->trainning_least_time==0 || p->trainning_least_time > p->trainning_last_time){
				//更新自己最好成绩
				if( p->trainning_least_time ){
					count=p->trainning_least_time - p->trainning_last_time;
					db_sub_ssid_count(0,p->id,ssid_trainning_least_time,count);
					p->trainning_least_time -= count;
				}else{
					db_day_add_ssid_cnt(0,p->id,ssid_trainning_least_time,p->trainning_last_time,-1);
					p->trainning_least_time=p->trainning_last_time;
				}
				mole2_user_update_rank_in cli_out;
				cli_out.val=p->trainning_least_time;
				memcpy(cli_out.nick,p->nick,sizeof(p->nick));
				send_msg_to_db(0,p->id,mole2_user_update_rank_cmd,&cli_out);
			}
		}
	}
	int j = sizeof(task_reward_t);
	finish_task_last_step(p, &p_tr->task);
	p->attr_addition = p_tr->attr_addition;
	update_when_level_change(p, p->level, p_tr->level);
	
	p->xiaomee = p_tr->xiaomee;
	p->experience = p_tr->exp;
	
	p->hp = p_tr->hp;
	p->mp = p_tr->mp;
	p->expbox = p_tr->expbox;
	p->skill_expbox = p_tr->skill_expbox;
	if (p_tr->proflv > p->prof_lv) {
		msg_log_bus_mole2_jobup(p->id, p->nick, p->prof_lv, p_tr->proflv, p->prof);
		msg_log_profession(p->prof, p_tr->proflv, p->prof, p->prof_lv);
		p->prof_lv = p_tr->proflv;
	}

	monitor_sprite(p, "TASK REWARD");

	CHECK_BODY_LEN(len, sizeof(task_reward_t) + p_tr->cloth_cnt * sizeof(body_cloth_t) \
		+ p_tr->item_cnt * sizeof(item_t) \
		+ p_tr->pet_cnt * (sizeof(pet_db_t) + MAX_SKILL_BAR * sizeof(skill_t)) \
		+ p_tr->honor_cnt * 12 + p_tr->skill_cnt * sizeof(skill_t));

	add_user_bag_cloths(p, (body_cloth_t *)(buf + j), p_tr->cloth_cnt);
	pkg_cli_cloth_info((body_cloth_t *)(buf + j), p_tr->cloth_cnt, &cli_out);
	j += p_tr->cloth_cnt * sizeof(body_cloth_t);
	for (uint32_t loop = 0; loop < p_tr->item_cnt; loop ++) {
		item_t* pi = (item_t *)(buf + j);
		cache_add_kind_item (p, pi->itemid, pi->count);
		stru_item tmp_item;
		tmp_item.itemid = pi->itemid;
		tmp_item.count = pi->count;
		cli_out.itemlist.push_back(tmp_item);
		j += sizeof(item_t);
	}

	noti_get_pets_out pet_out;
	for (uint32_t loop = 0; loop < p_tr->pet_cnt; loop ++) {
		pet_db_t* ppd = (pet_db_t *)(buf + j);
		if (ppd->action_state < db_warehouse) {
			update_pet_from_db(p, &p->pets_inbag[p->pet_cnt], ppd, false);
			msg_log_pet_level(p->pets_inbag[p->pet_cnt].level, 0);
			msg_log_pet_race(p->pets_inbag[p->pet_cnt].race, 1);
			msg_log_pet_task(p->pets_inbag[p->pet_cnt].pettype);
			msg_log_pet_catch(p->pets_inbag[p->pet_cnt].pettype, p->pets_inbag[p->pet_cnt].level);
			p->pet_cnt ++;
			p->pet_cnt_actual ++;
			p->all_petcnt ++;
		}
		stru_pet_simple_info pet_simple;
		pet_simple.petid = ppd->petid;
		pet_simple.pettype = ppd->pettype;
		pet_simple.location = ppd->action_state;
		pet_out.petlist.push_back(pet_simple);
		j += sizeof(pet_db_t) + MAX_SKILL_BAR * sizeof(skill_t);
	}

	if (p_tr->pet_cnt) {
		send_msg_to_self(p, noti_get_pets_cmd, &pet_out, 0);
	}

	for (uint32_t loop = 0; loop < p_tr->honor_cnt; loop ++) {
		uint32_t new_honor = *(uint32_t *)(buf + j + loop * 12 + 4);
		honor_attr_t* pha = get_honor_attr(new_honor);
		if (pha) {
			update_one_honor(p, pha);
			set_user_honor(p, new_honor);
			notify_user_honor_up(p, 1, 0, 1);
			db_set_user_honor(0, p->id, new_honor);
		}
	}
	j += p_tr->honor_cnt * 12;

	for (uint32_t loop = 0; loop < p_tr->skill_cnt; loop ++) {
		skill_t* ps = (skill_t*)(buf + j + sizeof(skill_t) * loop);
		cache_add_skill(p, ps);
		stru_user_skill tmp_skill;
		tmp_skill.skillid = ps->skill_id;
		tmp_skill.skilllv = ps->skill_level;
		tmp_skill.skillexp = ps->skill_exp;
		cli_out.skills.push_back(tmp_skill);
	}

	j += p_tr->skill_cnt * sizeof(skill_t);

	DEBUG_LOG("===============================================\n");

	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int db_cancel_task(sprite_t* p, uint32_t uid, task_db_t* ptr)
{
	task_db_t tdb;
	memset(&tdb, 0, sizeof(task_db_t));
	tdb.taskid = ptr->taskid;
	tdb.nodeid = ptr->nodeid;
	tdb.state = TASK_STATE_CANCEL;
	tdb.optdate = get_now_tv()->tv_sec;
	tdb.finnum = ptr->finnum;
	tdb.fintime = ptr->fintime;
	return send_request_to_db(p, uid, proto_db_set_task_db, &tdb, sizeof(task_db_t));
}

int cancel_task_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t taskid;
	int j = 0;
	UNPKG_UINT32(body, taskid, j);
	KDEBUG_LOG(p->id, "CANCEL TASK\t[%u]", taskid);

	task_t* pt = get_task(taskid);
	CHECK_TASK_ID_VALID(p, pt, taskid);

	task_db_t* ptr = cache_get_task_info(p, taskid);
	CHECK_TASK_RECVD(p, ptr, taskid);
	return db_cancel_task(p, p->id, ptr);
}

int set_task_clibuf_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t taskid, clival;
	int j = 0;
	UNPKG_UINT32(body, taskid, j);

	task_db_t* ptr = cache_get_task_info(p, taskid);
	CHECK_TASK_RECVD(p, ptr, taskid);

	KDEBUG_LOG(p->id, "SET TASK CLI BUF\t[taskid=%u nodeid=%u date=%u]", taskid, ptr->nodeid, ptr->optdate);
	task_db_t tdb;
	memcpy(&tdb, ptr, sizeof(task_db_t));
	tdb.optdate = get_now_tv()->tv_sec;
	memcpy(tdb.cli_buf, body + 4, TASK_CLI_BUF_LEN);
	j += TASK_CLI_BUF_LEN;
	UNPKG_UINT32(body, clival, j);
	*(uint32_t *)p->session = clival;
	*(task_db_t**)(p->session + 4) = ptr;
	
	return send_request_to_db(p, p->id, proto_db_set_task_db, &tdb, sizeof(task_db_t));
}

int get_task_svrbuf_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t taskid;
	int j = 0;
	UNPKG_UINT32(body, taskid, j);
	KDEBUG_LOG(p->id, "GET TASK SVR BUF\t[taskid=%u]", taskid);
	task_db_t* ptr = cache_get_task_info(p, taskid);
	CHECK_TASK_RECVD(p, ptr, taskid);

	task_node_t* ptn = get_task_node(ptr->taskid, ptr->nodeid + 1);
	if (!ptn) {
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, taskid, i);
	PKG_UINT32(msg, ptr->nodeid, i);
	int canfin = 1;

	if (ptn->itarget.itemid) {
		PKG_UINT32(msg, ptn->itarget.itemid, i);
		normal_item_t* pni = get_item(ptn->itarget.itemid);
		uint32_t itemcnt = pni ? get_item_cnt(p, pni) : 0;
		canfin = itemcnt < ptn->itarget.count ? 0 : canfin;
		PKG_UINT32(msg, itemcnt, i);
		PKG_UINT32(msg, ptn->itarget.count, i);
	} else {
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, 0, i);
	}

	if (ptn->btarget.beastid) {
		PKG_UINT32(msg, ptn->btarget.beastid, i);
		uint16_t beastcnt = ptr->svr_buf[svr_buf_beast];
		canfin = beastcnt < ptn->btarget.count ? 0 : canfin;
		PKG_UINT32(msg, beastcnt, i);
		PKG_UINT32(msg, ptn->btarget.count, i);
	} else {
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, 0, i);
	}

	PKG_UINT32(msg, 0, i);
	PKG_UINT32(msg, 0, i);

	PKG_UINT32(msg, canfin, i);

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

task_db_t* cache_get_task_info(sprite_t* p, uint32_t taskid)
{
	return (task_db_t *)g_hash_table_lookup(p->recvtasks, &taskid);;
}

int cache_add_recvd_task(sprite_t* p, task_db_t* ptd)
{
	task_db_t* ptr = cache_get_task_info(p, ptd->taskid);
	if (!ptr) {
		ptr = (task_db_t *)g_slice_alloc(sizeof(task_db_t));
		memcpy(ptr, ptd, sizeof(task_db_t));
		g_hash_table_insert(p->recvtasks, &ptr->taskid, ptr);
	} else if (ptd->state == TASK_STATE_RECVD) {
		memcpy(ptr, ptd, sizeof(task_db_t));
	}
	
	return 0;
}

int cache_del_recvd_task(sprite_t* p, uint32_t taskid)
{
	task_db_t* ptd = cache_get_task_info(p, taskid);
	if (ptd) {
		g_hash_table_remove(p->recvtasks, &ptd->taskid);
		g_slice_free1(sizeof(task_db_t), ptd);
	}
	return 0;
}

int get_task_detail_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t taskid;
	int j = 0;
	UNPKG_UINT32(body, taskid, j);
	KDEBUG_LOG(p->id, "GET TASK DETIAL CMD\t[%u]", taskid);

	if (!cache_check_task_ever_fin(p, taskid))
		return send_to_self_error(p, p->waitcmd, cli_err_task_not_finished, 1);

	return send_request_to_db(p, p->id, proto_db_get_task_detail, &taskid, 4);
}

int get_task_detail_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t taskid, fintime;
	uint8_t diary_str[TASK_DIARY_BUF_LEN];
	int i = sizeof(protocol_t), j = 0;
	UNPKG_H_UINT32(buf, taskid, j);
	UNPKG_H_UINT32(buf, fintime, j);
	UNPKG_STR(buf, diary_str, j, TASK_DIARY_BUF_LEN);
	PKG_UINT32(msg, taskid, i);
	PKG_UINT32(msg, fintime, i);
	PKG_STR(msg, diary_str, i, TASK_DIARY_BUF_LEN);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int write_task_diary_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t taskid;
	uint8_t diary_str[TASK_DIARY_BUF_LEN], out[TASK_DIARY_BUF_LEN + 4];
	int i = 0, j = 0;
	UNPKG_UINT32(body, taskid, j);
	UNPKG_STR(body, diary_str, j, TASK_DIARY_BUF_LEN);
	CHECK_DIRTYWORD(p, diary_str);

	diary_str[TASK_DIARY_BUF_LEN - 1] = '\0';

	KDEBUG_LOG(p->id, "USER WRITE TASK DIARY\t[%u %s]", taskid, diary_str);

	if (!cache_check_task_ever_fin(p, taskid))
		return send_to_self_error(p, p->waitcmd, cli_err_task_not_finished, 1);

	PKG_H_UINT32(out, taskid, i);
	PKG_STR(out, diary_str, i, TASK_DIARY_BUF_LEN);
	return send_request_to_db(p, p->id, proto_db_write_task_diary, out, i);
}

int write_task_diary_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int cli_get_task_rank(sprite_t *p, Cmessage* c_in)
{
	Cmessage cli_out;
	return send_msg_to_db(p,p->id,mole2_get_task_rank_cmd,&cli_out);
}

int cli_get_task_time(sprite_t *p, Cmessage* c_in)
{
	cli_get_task_time_out cli_out;
	if(cache_check_task_fin(p,TRAIN_TASK_ID)){
		cli_out.time=p->trainning_last_time;
	}else if(cache_get_task_info(p, TRAIN_TASK_ID)){
		cli_out.time=p->trainning_last_time + get_now_sec() - p->trainning_start_time;
	}else{
		cli_out.time=0;
	}	
	return send_msg_to_self(p,p->waitcmd,&cli_out,1);
}

int mole2_get_task_rank(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mole2_get_task_rank_out *p_out=P_OUT;
	cli_get_task_rank_out cli_out;
	cli_out.time=p->trainning_least_time;
	cli_out.uids=p_out->uids;
	return send_msg_to_self(p,p->waitcmd,&cli_out,1);
}
