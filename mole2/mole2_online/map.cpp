#include <assert.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include <glib.h>
#include <libxml/tree.h>
#include <async_serv/async_serv.h>

#include "benchapi.h"
#include "util.h"
#include "sprite.h"
#include "battle.h"
#include "map.h"
#include "cli_proto.h"
#include "sns.h"
#include "pet.h"
#include "items.h"
#include "global.h"
#include "homemap.h"
#include "beast.h"
#include "task_new.h"

#include "maze.h"
#include "./proto/mole2_home.h"
#include "./proto/mole2_home_enum.h"

static int load_map_beasts_config(xmlNodePtr cur, struct map *p)
{
	uint32_t lvcnt, pecent_cnt, id;
	int lvs[MAX_BEAST_TYPE_PER_MAP];
	int pecents[MAX_BEAST_TYPE_PER_MAP];
	DECODE_XML_PROP_UINT32(id, cur, "ID");
	lvcnt = decode_xml_prop_arr_int_default(lvs, MAX_BEAST_TYPE_PER_MAP, cur, "Lv", 0);
	pecent_cnt = decode_xml_prop_arr_int_default(pecents, MAX_BEAST_TYPE_PER_MAP, cur, "Percent", 0);
	KTRACE_LOG(0, "BEAST CNT\t[%lu %u %u]", p->id, lvcnt, pecent_cnt);
	if (lvcnt != pecent_cnt) {
		ERROR_RETURN(("beast pecent not match\t[%lu %u %u]", p->id, lvcnt, pecent_cnt), -1);
	}
	if (p->beastcnt + lvcnt > MAX_BEAST_TYPE_PER_MAP) {
		ERROR_RETURN(("beast cnt over max\t[%lu %u %u %u]", p->id, p->beastcnt, lvcnt, pecent_cnt), -1);
	}

	for (uint32_t loop = 0; loop < lvcnt; loop ++) {
		beast_lvinfo_t* pbl = get_beast_lvinfo(id, lvs[loop]);
		if (!pbl || !pbl->id) {
			ERROR_RETURN(("beast id invalid\t[%lu %u %u %u]", p->id, id, lvs[loop],pbl==NULL), -1);
		}
		p->apbl[p->beastcnt + loop] = pbl;
		p->beastrate[p->beastcnt + loop] = pecents[loop];
	}
	p->beastcnt += lvcnt;

	if (!p->check)
		p->ebeasts[p->elite_cnt ++].beastid = id;

	return 0;
}

static int load_map_task_beasts_config(xmlNodePtr cur, struct map *p)
{
	if (p->task_beast_cnt >= MAX_SPEC_BEAST_PER_MAP) {
		ERROR_RETURN(("too much task beasts config\t[%lu]", p->id), -1);
	}
	task_beast_t* ptb = &p->task_beasts[p->task_beast_cnt];
	DECODE_XML_PROP_UINT32(ptb->grpid, cur, "GrpID");
	ptb->task_cnt = decode_xml_prop_arr_int_default((int *)ptb->tasks, MAX_TASK_BEAST_NUM, cur, "TaskID", 0);
	if (!ptb->task_cnt) {
		ERROR_RETURN(("no task beasts config\t[%lu]", p->id), -1);
	}
	if (ptb->task_cnt != decode_xml_prop_arr_int_default((int *)ptb->startns, MAX_TASK_BEAST_NUM, cur, "StartNodes", 0))
		ERROR_RETURN(("task start node not match[%lu]", p->id), -1);
	if (ptb->task_cnt != decode_xml_prop_arr_int_default((int *)ptb->endns, MAX_TASK_BEAST_NUM, cur, "EndNodes", 0))
		ERROR_RETURN(("task end node not match[%lu]", p->id), -1);
	DECODE_XML_PROP_UINT32(ptb->probability, cur, "Probability");
	KTRACE_LOG(0, "TASK BEAST\t[%lu %u %u %u %u %u %u]",p->id, p->task_beast_cnt, ptb->task_cnt, ptb->tasks[0], ptb->tasks[1], ptb->tasks[2], ptb->probability);
	p->task_beast_cnt ++;
	return 0;
}

static int load_map_spec_beasts_config(xmlNodePtr cur, struct map *p)
{
	if (p->spec_beast_cnt >= MAX_SPEC_BEAST_PER_MAP) {
		ERROR_RETURN(("too much task beasts config\t[%lu]", p->id), -1);
	}
	spec_beast_t* psb = &p->spec_beasts[p->spec_beast_cnt];
	DECODE_XML_PROP_UINT32(psb->grpid, cur, "GrpID");
	DECODE_XML_PROP_UINT32(psb->probability, cur, "Probability");
	decode_xml_prop_uint32_default(&psb->clothid, cur, "ClothesID", 0);
	decode_xml_prop_uint32_default(&psb->start_time, cur, "StartHour", 0);
	psb->start_time *= 3600;
	decode_xml_prop_uint32_default(&psb->end_time, cur, "EndHour", 0);
	psb->end_time *= 3600;
	if (psb->start_time > psb->end_time)
		return -1;
	p->spec_beast_cnt ++;
	return 0;
}

static int load_return_map(xmlNodePtr cur, struct map *p)
{
	if (p->retmap_cnt >= MAX_RETMAP_PER_MAP) {
		ERROR_RETURN(("too much return maps\t[%lu %u]", p->id, p->retmap_cnt), -1);
	}
	DECODE_XML_PROP_UINT32(p->ret_maps[p->retmap_cnt].mapid, cur, "ID");
	DECODE_XML_PROP_UINT32(p->ret_maps[p->retmap_cnt].des_x, cur, "X");
	DECODE_XML_PROP_UINT32(p->ret_maps[p->retmap_cnt].des_y, cur, "Y");
	KTRACE_LOG(0, "RETURNMAP[%d %lu %u %u]", p->retmap_cnt, p->ret_maps[p->retmap_cnt].mapid, p->ret_maps[p->retmap_cnt].des_x, p->ret_maps[p->retmap_cnt].des_y);
	p->retmap_cnt ++;
	return 0;
}

static int load_adjacent_map(xmlNodePtr cur, map_t* p)
{
	if (p->adjacent_cnt >= MAX_ADJACENTMAP_PER_MAP) {
		ERROR_RETURN(("too much adjacent maps\t[%lu %u]", p->id, p->adjacent_cnt), -1);
	}
	int mapid;
	DECODE_XML_PROP_INT(mapid, cur, "ID");
	p->adjacent_maps[p->adjacent_cnt].mapid = mapid;
	DECODE_XML_PROP_UINT32(p->adjacent_maps[p->adjacent_cnt].cur_x, cur, "CurX");
	DECODE_XML_PROP_UINT32(p->adjacent_maps[p->adjacent_cnt].cur_y, cur, "CurY");
	DECODE_XML_PROP_UINT32(p->adjacent_maps[p->adjacent_cnt].des_x, cur, "DesX");
	DECODE_XML_PROP_UINT32(p->adjacent_maps[p->adjacent_cnt].des_y, cur, "DesY");
	decode_xml_prop_uint32_default(&p->adjacent_maps[p->adjacent_cnt].grpid, cur, "KillBeastGrp", 0);
	KTRACE_LOG(0, "ADJACENT[%d %lu %d %d %d %d]", p->adjacent_cnt, p->adjacent_maps[p->adjacent_cnt].mapid, \
		p->adjacent_maps[p->adjacent_cnt].des_x, p->adjacent_maps[p->retmap_cnt].des_y, \
		p->adjacent_maps[p->adjacent_cnt].cur_x, p->adjacent_maps[p->retmap_cnt].cur_y);
	p->adjacent_cnt ++;
	return 0;
}

static int load_maze_beast(xmlNodePtr cur, map_t* p)
{
	if (p->maze_beastcnt)
		ERROR_RETURN(("maze beast duplicate\t[%lu]", p->id), -1);
	p->maze_beastcnt = decode_xml_prop_arr_int_default((int *)p->maze_beasts, MAX_BEAST_TYPE_PER_MAP, cur, "ID", 0);
	if (p->maze_beastcnt != decode_xml_prop_arr_int_default((int *)p->maze_beastrates, MAX_BEAST_TYPE_PER_MAP, cur, "BeastPercent", 0))
		ERROR_RETURN(("beast pecent not match\t[%lu %u]", p->id, p->maze_beastcnt), -1);

	int rate = 0;
	for (int  loop = 0; loop < p->maze_beastcnt; loop ++) {
		beast_t* pba = get_beast(p->maze_beasts[loop]);
		if (!pba || !pba->id) {
			ERROR_RETURN(("beast id invalid\t[%lu %u]", p->id, p->maze_beasts[loop]), -1);
		}
		rate += p->maze_beastrates[loop];
	}

	if (rate != RAND_COMMON_RAND_BASE) {
		ERROR_RETURN(("beast total percent err\t[%lu %u]", p->id, rate), -1);
	}

	return 0;
}
static int load_map_object(xmlNodePtr cur, map_t* p)
{
	if (p->state_cnt >= STATE_CNT_PER_MAP)
		ERROR_RETURN(("too much object"), -1);
	map_state_t* pms = &p->states[p->state_cnt];
	DECODE_XML_PROP_UINT32(pms->id, cur, "ID");
	DECODE_XML_PROP_UINT32(pms->state, cur, "Default");
	pms->to_state = 0;
	int loopi, loopj;
	for (loopi = 0; loopi < MAX_STATE_CNT; loopi ++) {
		for (loopj = 0; loopj < MAX_STATE_CNT; loopj ++) {
			pms->trans[loopi][loopj] = -1;
		}
	}
	int states[MAX_STATE_CNT * MAX_STATE_CNT], to_states[MAX_STATE_CNT * MAX_STATE_CNT], events[MAX_STATE_CNT * MAX_STATE_CNT];
	uint32_t count = decode_xml_prop_arr_int_default(states, MAX_STATE_CNT * MAX_STATE_CNT, cur, "States", 0);
	if (count != decode_xml_prop_arr_int_default(to_states, MAX_STATE_CNT * MAX_STATE_CNT, cur, "ToState", 0))
		return -1;
	if (count != decode_xml_prop_arr_int_default(events, MAX_STATE_CNT * MAX_STATE_CNT, cur, "Events", 0))
		return -1;

	for (uint32_t loop = 0; loop < count; loop ++) {
		if (events[loop] < 0 \
			|| states[loop] < 0 || states[loop] >= MAX_STATE_CNT \
			|| to_states[loop] < 0 || to_states[loop] >= MAX_STATE_CNT \
			|| states[loop] == to_states[loop])
			ERROR_RETURN(("map state error\t[%u %u %u]", states[loop], to_states[loop], events[loop]), -1);
		if (pms->trans[states[loop]][to_states[loop]] != -1)
			ERROR_RETURN(("map state cur error\t[%u %u %u %u]", states[loop], to_states[loop], events[loop], pms->trans[states[loop]][to_states[loop]]), -1);
		pms->trans[states[loop]][to_states[loop]] = events[loop];
	}
	p->state_cnt ++;

	KTRACE_LOG(0, "MAP OBJ\t[%lu %u %u]", p->id, pms->id, p->state_cnt);

	return 0;
}


int load_maps(xmlNodePtr cur)
{
	int map_idx = 0, i;
	for (i = 0; i < MAP_HASH_SLOT_NUM; i++)
		INIT_LIST_HEAD (&map_hash_slots[i]);
	
	srand(time(0));
	cur = cur->xmlChildrenNode;
	uint32_t chocoboid = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Map")){ 
			DECODE_XML_PROP_INT (map_idx, cur, "ID");
			map_t *p = &normal_map[map_idx];
			p->id = map_idx;
			DECODE_XML_PROP_INT (p->flag, cur, "Type");
			uint16_t tmp;
			decode_xml_prop_uint16_default(&tmp,cur,"Team",0);
			p->team=tmp;
			DECODE_XML_PROP_UINT32(p->area, cur, "Area");
			decode_xml_prop_uint32_default(&p->chocobo, cur, "Chocobo", 0);
			if (p->chocobo && p->chocobo <= chocoboid)
				ERROR_RETURN(("chocobo id invalid\t[%lu %u %u]", p->id, chocoboid, p->chocobo), -1);
			chocoboid = p->chocobo;
			decode_xml_prop_uint32_default(&p->recv_map, cur, "Receive", 0);
			decode_xml_prop_uint32_default(&p->base_step, cur, "BasicValue", 0);
			decode_xml_prop_uint32_default(&p->rand_step, cur, "RandomValue", 0);
			decode_xml_prop_uint32_default(&p->fin_task, cur, "CompTaskID", 0);
			decode_xml_prop_uint32_default(&p->fin_task_node, cur, "CompTaskNode", 0);
			decode_xml_prop_uint32_default(&p->on_task, cur, "AnimationTask", 0);
			decode_xml_prop_uint32_default(&p->on_task_node, cur, "ANode", 0);
			decode_xml_prop_uint16_default(&p->home_ret, cur, "HomeRet", 0);
			decode_xml_prop_uint32_default(&p->transmap, cur, "TransMap", 0);
			decode_xml_prop_uint32_default(&p->leaderlv, cur, "TeamLeaderLv", 0);
			decode_xml_prop_uint16_default(&p->check, cur, "Check", 1);
			if (p->transmap)
				DECODE_XML_PROP_UINT32(p->transid, cur, "TransId");
			decode_xml_prop_arr_int_default((int *)p->tasks, 2, cur, "TaskGo", 0);
			decode_xml_prop_arr_int_default((int *)p->nodes, 2, cur, "TaskGoNode", 0);
			KTRACE_LOG(0, "MAP TASK INFO\t[%lu %u %u %u %u]", p->id, p->tasks[0], p->tasks[1], p->nodes[0], p->nodes[1]);

			DECODE_XML_PROP_INT_DEFAULT(p->min_beast_cnt, cur, "MinBeastCnt", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->max_beast_cnt, cur, "MaxBeastCnt", 0);
			if ((p->max_beast_cnt && !p->min_beast_cnt) || p->min_beast_cnt > p->max_beast_cnt || p->max_beast_cnt > 2 * MAX_PLAYERS_PER_TEAM) {
				ERROR_RETURN(("BEAST CNT ERR %lu %u %u", p->id, p->min_beast_cnt, p->max_beast_cnt), -1);
			}

			INIT_LIST_HEAD (&p->sprite_list_head);
			INIT_LIST_HEAD (&p->hash_list);
			INIT_LIST_HEAD(&p->timer_list);
			
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar*)"Type")) {
					if (load_map_beasts_config (chl, p))
						ERROR_RETURN(("load map beasts config error\t[%lu]", p->id), -1);
				}
				if (!xmlStrcmp(chl->name, (const xmlChar*)"SpecBeasts")) {
					if (load_map_spec_beasts_config (chl, p))
						ERROR_RETURN(("load map beasts config error\t[%lu]", p->id), -1);
				}
				if (!xmlStrcmp(chl->name, (const xmlChar*)"TaskBeasts")) {
					if (load_map_task_beasts_config (chl, p))
						ERROR_RETURN(("load map beasts config error\t[%lu]", p->id), -1);
				}
				if (!xmlStrcmp(chl->name, (const xmlChar*)"ReturnMap")) {
					if (load_return_map (chl, p)) 
						ERROR_RETURN(("load return map err\t[%lu]", p->id), -1);
				}
				if (!xmlStrcmp(chl->name, (const xmlChar*)"AdjacentMap")) {
					if (load_adjacent_map (chl, p)) 
						ERROR_RETURN(("load adjacent map err\t[%lu]", p->id), -1);
				}

				if (!xmlStrcmp(chl->name, (const xmlChar*)"MazeBeast")) {
					if (load_maze_beast (chl, p)) 
						ERROR_RETURN(("load adjacent map err\t[%lu]", p->id), -1);
				}

				if (!xmlStrcmp(chl->name, (const xmlChar*)"Object")) {
					if (load_map_object (chl, p)) 
						ERROR_RETURN(("load adjacent object err\t[%lu]", p->id), -1);
				}
				chl = chl->next;
			}

			if (p->max_beast_cnt && !p->beastcnt){
				ERROR_RETURN(("load error \t[%lu]max_beast_cnt  p->beastcnt[%u][%u]", p->id,p->max_beast_cnt ,p->beastcnt ), -1);
			}
			if (p->beastcnt) {
				int loop, rate = 0;
				for (loop = 0; loop < p->beastcnt; loop ++) {
					rate += p->beastrate[loop];
				}
				if (rate != BEAST_COMMON_RAND_BASE)
					ERROR_RETURN(("rate not invalid\t[%lu %u]", p->id, rate), -1);
			}
			p->sprite_num = 0;

			ADD_ONLINE_TIMER(p, n_refresh_elite_beast, 0, 3);
			ADD_ONLINE_TIMER(p, n_refresh_elite_beast, (void *)1, 3);
		}
		cur = cur->next;
	}

	return 0;
}

int reload_maps(xmlNodePtr cur)
{
	int map_idx = 0;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Map")){ 
			DECODE_XML_PROP_INT (map_idx, cur, "ID");
			map_t *p = &normal_map[map_idx];
			DECODE_XML_PROP_INT (p->flag, cur, "Type");
			DECODE_XML_PROP_UINT32(p->area, cur, "Area");
			decode_xml_prop_uint32_default(&p->chocobo, cur, "Chocobo", 0);
			decode_xml_prop_uint32_default(&p->recv_map, cur, "Receive", 0);
			//decode_xml_prop_uint32_default(&p->key, cur, "Key", 0);
			decode_xml_prop_uint32_default(&p->base_step, cur, "BasicValue", 0);
			decode_xml_prop_uint32_default(&p->rand_step, cur, "RandomValue", 0);
			decode_xml_prop_uint32_default(&p->fin_task, cur, "CompTaskID", 0);
			decode_xml_prop_uint32_default(&p->fin_task_node, cur, "CompTaskNode", 0);
			decode_xml_prop_uint32_default(&p->on_task, cur, "AnimationTask", 0);
			decode_xml_prop_uint32_default(&p->on_task_node, cur, "ANode", 0);
			decode_xml_prop_uint16_default(&p->home_ret, cur, "HomeRet", 0);
			decode_xml_prop_uint32_default(&p->transmap, cur, "TransMap", 0);
			decode_xml_prop_uint32_default(&p->leaderlv, cur, "TeamLeaderLv", 0);
			if (p->transmap)
				DECODE_XML_PROP_UINT32(p->transid, cur, "TransId");
			decode_xml_prop_arr_int_default((int *)p->tasks, 2, cur, "TaskGo", 0);
			decode_xml_prop_arr_int_default((int *)p->nodes, 2, cur, "TaskGoNode", 0);
			KTRACE_LOG(0, "MAP TASK INFO\t[%lu %u %u %u %u]", p->id, p->tasks[0], p->tasks[1], p->nodes[0], p->nodes[1]);

			DECODE_XML_PROP_INT_DEFAULT(p->min_beast_cnt, cur, "MinBeastCnt", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->max_beast_cnt, cur, "MaxBeastCnt", 0);


			p->beastcnt = 0;
			for (int loop = 0; loop < MAX_BEAST_TYPE_PER_MAP; loop ++) {
				p->apbl[loop] = NULL;
				p->beastrate[loop] = 0;
			}
			p->elite_cnt = 0;
			memset(&p->ebeasts, 0, sizeof(p->ebeasts));
			p->spec_beast_cnt = 0;
			memset(p->spec_beasts, 0, sizeof(p->spec_beasts));
			p->task_beast_cnt = 0;
			memset(p->task_beasts, 0, sizeof(p->task_beasts));
			p->retmap_cnt = 0;
			memset(p->ret_maps, 0, sizeof(p->ret_maps));
			p->adjacent_cnt = 0;
			memset(p->adjacent_maps, 0, sizeof(p->adjacent_maps));
			p->maze_beastcnt = 0;
			memset(p->maze_beasts, 0, sizeof(p->maze_beasts));
			memset(p->maze_beastrates, 0, sizeof(p->maze_beastrates));
			p->state_cnt = 0;
			memset(p->states, 0, sizeof(p->states));
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar*)"Type")) {
					if (load_map_beasts_config (chl, p))
						ERROR_RETURN(("load map beasts config error\t[%lu]", p->id), -1);
				}
				if (!xmlStrcmp(chl->name, (const xmlChar*)"SpecialBeasts")) {
					if (load_map_spec_beasts_config (chl, p))
						ERROR_RETURN(("load map beasts config error\t[%lu]", p->id), -1);
				}
				if (!xmlStrcmp(chl->name, (const xmlChar*)"TaskBeasts")) {
					if (load_map_task_beasts_config (chl, p))
						ERROR_RETURN(("load map beasts config error\t[%lu]", p->id), -1);
				}
				if (!xmlStrcmp(chl->name, (const xmlChar*)"ReturnMap")) {
					if (load_return_map (chl, p)) 
						ERROR_RETURN(("load return map err\t[%lu]", p->id), -1);
				}
				if (!xmlStrcmp(chl->name, (const xmlChar*)"AdjacentMap")) {
					if (load_adjacent_map (chl, p)) 
						ERROR_RETURN(("load adjacent map err\t[%lu]", p->id), -1);
				}

				if (!xmlStrcmp(chl->name, (const xmlChar*)"MazeBeast")) {
					if (load_maze_beast (chl, p)) 
						ERROR_RETURN(("load adjacent map err\t[%lu]", p->id), -1);
				}

				if (!xmlStrcmp(chl->name, (const xmlChar*)"Object")) {
					if (load_map_object (chl, p)) 
						ERROR_RETURN(("load adjacent object err\t[%lu]", p->id), -1);
				}
				chl = chl->next;
			}
		}
		cur = cur->next;
	}

	return 0;
}


void unload_maps ()
{
	list_head_t *l, *p;

	for (int i = 0; i < MAP_HASH_SLOT_NUM; i++) {
		list_for_each_safe (l, p, &map_hash_slots[i]) {
			map_t *m = list_entry (l, struct map, hash_list);
			REMOVE_TIMERS(m);
			list_del (&m->hash_list);
			KDEBUG_LOG(0, "FREE MAP(UNLOAD MAP)\t[%lu]", m->id);
			g_slice_free1(sizeof *m, m);
		}
	}
}

static void broadcast_msg(sprite_t* p, userid_t uid, map_id_t mapid, int cmd, Cmessage * c_in  )
{
	if (IS_HOME_MAP(mapid)){
		char head_buf[sizeof(protocol_t)];
		protocol_t* proto = (protocol_t*)head_buf;
		proto->len = htonl(sizeof(protocol_t));
		proto->cmd = htons(cmd);
		proto->ret = 0;
		send_msg_to_homeserv_ex(p, MCAST_home_msg,head_buf,c_in , uid, mapid, ntohs(proto->cmd));
	}
}

static void broadcast_pkg(sprite_t* p, userid_t uid, map_id_t mapid, const void* buf, int len)
{
	if (IS_HOME_MAP(mapid)){
		const protocol_t* proto = (const protocol_t*)buf;
		send_to_homeserv(p, MCAST_home_msg, len, buf, uid, mapid, ntohs(proto->cmd));
	}
}
void send_msg_to_map(sprite_t* s, int cmd, Cmessage * c_in,  int completed,int self)
{
	list_head_t* p;
	map_t* tile = s->tiles;

	if (!tile || !IS_SPRITE_VISIBLE(s)) {
		if (IS_NORMAL_ID(s->id) && self) {
			send_msg_to_self(s, cmd,c_in, 0);
		}
	} else {	
		list_for_each(p, &tile->sprite_list_head) {
			sprite_t* l = list_entry(p, sprite_t, map_list);
			if ( IS_NORMAL_ID(l->id) ) {
				if(self || l->id != s->id) {
					send_msg_to_self(l, cmd,c_in, 0);
				}
			}
		}
		broadcast_msg(s, s->id, tile->id, cmd,c_in );
	}

	if (completed)
		s->waitcmd = 0;
}

void send_msg_to_map2(const map_t* tile, int cmd, Cmessage * c_in)
{
	list_head_t* p;
	list_for_each(p, &tile->sprite_list_head) {
		sprite_t* l = list_entry(p, sprite_t, map_list);
		if (IS_NORMAL_ID(l->id) && IS_SPRITE_VISIBLE(l)) {
			send_msg_to_self(l, cmd, c_in, 0);
		}
	}
}
void send_msg_to_map3(map_id_t mapid, int cmd, Cmessage * c_in )
{
	const map_t* tile = get_map(mapid);
	if (tile) {
		send_msg_to_map2(tile,cmd,c_in ); 
	}
}



void send_to_map(sprite_t* s, uint8_t* buffer, int len, int completed,int self)
{
	list_head_t* p;
	map_t* tile = s->tiles;

	if (!tile || !IS_SPRITE_VISIBLE(s)) {
		if (IS_NORMAL_ID(s->id) && self) {
			send_to_self(s, buffer, len, 0);
		}
	} else {	
		list_for_each(p, &tile->sprite_list_head) {
			sprite_t* l = list_entry(p, sprite_t, map_list);
			if ( IS_NORMAL_ID(l->id) ) {
				if(self || l->id != s->id) {
					send_to_self(l, buffer, len, 0);
				}
			}
		}
		broadcast_pkg(s, s->id, tile->id, buffer, len);
	}

	if (completed)
		s->waitcmd = 0;
}

void send_to_map2(const map_t* tile, uint8_t* buffer, int len)
{
	list_head_t* p;
	list_for_each(p, &tile->sprite_list_head) {
		sprite_t* l = list_entry(p, sprite_t, map_list);
		if (IS_NORMAL_ID(l->id) && IS_SPRITE_VISIBLE(l)) {
			send_to_self(l, buffer, len, 0);
		}
	}
}

void send_to_map3(map_id_t mapid, uint8_t* buffer, int len)
{
	const map_t* tile = get_map(mapid);

	if (tile) {
		list_head_t* p;
		list_for_each(p, &tile->sprite_list_head) {
			sprite_t *l = list_entry(p, sprite_t, map_list);
			if (IS_NORMAL_ID(l->id) && IS_SPRITE_VISIBLE(l)) {
				send_to_self(l, buffer, len, 0);
			}
		}
	}
}

map_t* alloc_map(map_id_t mapid)
{
	map_t* tile = (map_t *)g_slice_alloc0(sizeof *tile);
	tile->id = mapid;
	tile->sprite_num = 0;
	
	INIT_LIST_HEAD (&tile->sprite_list_head);
	INIT_LIST_HEAD (&tile->hash_list);
	INIT_LIST_HEAD (&tile->timer_list);
	list_add_tail (&tile->hash_list, &map_hash_slots[mapid % MAP_HASH_SLOT_NUM]);
	KDEBUG_LOG (low32_val(mapid), "ALLOC HOME MAP [%u]", high32_val(mapid));
	return tile;
}

void response_proto_leave_map(sprite_t* p, map_id_t newmap, int complet,int self)
{
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, p->id, i);
	PKG_MAP_ID(msg, p->tiles->id, i);
	PKG_MAP_ID(msg, newmap, i);
	init_proto_head (msg, proto_cli_leave_map, i);
	send_to_map (p, msg, i, complet,self);	
}

void try_free_map(map_t* p_map)
{
	if (IS_NORMAL_MAP(p_map->id) || p_map->sprite_num)
		return;
	map_id_t mapid = p_map->id;
	KDEBUG_LOG (low32_val(mapid), "FREE HOME MAP [%u]", high32_val(mapid));
	list_del_init(&p_map->hash_list);
	g_slice_free1(sizeof *p_map, p_map);
	try_del_home_by_id(mapid);
}

void leave_map_copy(map_copy_instance_t* pmci, sprite_t* p)
{
	for (uint32_t loop = 0; loop < pmci->usercnt; loop ++) {
		if (p->id == pmci->uids[loop]) {
			pmci->ustate[loop] = 0;
			/*
			pmci->usercnt --;
			pmci->uids[loop] = pmci->uids[pmci->usercnt];
			pmci->uids[pmci->usercnt] = 0;
			*/
			break;
		}
	}

	bool all_leave = true;
	for (uint32_t loop = 0; loop < pmci->usercnt; loop ++) {
		if (pmci->ustate[loop])
			all_leave = false;
	}
	if (all_leave) {
		free_map_copy(pmci->p_copy_config, pmci->mapid >> 32);
		REMOVE_TIMERS(pmci);
		//TODO
		g_hash_table_remove(map_copys, &pmci->mapid);
		g_slice_free1(sizeof(map_copy_instance_t), pmci);
	}
}

void leave_map(sprite_t *p, map_id_t newmap)
{
	//KDEBUG_LOG(p->id, "11111 pvp_pk %u %u",p->is_in_war_map(),p->pvp_pk_can_get_item  );
	if (!p->tiles)
		return;
	KDEBUG_LOG(p->id, "pvp_pk %u %u",p->is_in_war_map(),p->pvp_pk_can_get_item  );
	if(p->is_in_war_map() && p->pvp_pk_can_get_item>0){
		p->pvp_pk_fail_times[p->pvp_type]+= 1;
		uint32_t ssid=p->pvp_type ? ssid_pvp_pk_fail_times_2:ssid_pvp_pk_fail_times;
		db_day_add_ssid_cnt(NULL, p->id, ssid, 1, -1);
		if(p->pvp_pk_can_get_item>1){
			p->pvp_pk_can_get_item++;
			if(p->pvp_pk_can_get_item>3){
				p->pvp_pk_can_get_item=0;
			}
		}else{
			p->pvp_pk_can_get_item=0;
		}
		p->pvp_type=0;
	//	KDEBUG_LOG(p->id, "22222 pvp_pk  %u", p->pvp_pk_can_get_item  );
	}

	if( IS_TRADE_MAP(p->tiles->id) && !IS_TRADE_MAP(newmap)){
		p->trade_cloth_info->clear();
		p->trade_item_info->clear();
		//KDEBUG_LOG(p->id,"llllll%u",p->shop_state);
		if( p->shop_state > shop_init ){
			home_trade_change_shop_in h_in;
			h_in.cmd=3;
			h_in.roomid=p->roomid;
			h_in.sid=p->shop_id;
			send_msg_to_homeserv(p->id, p->tiles->id, home_trade_change_shop_cmd ,&h_in);
		}
		KF_LOG("trade_leave_map",p->id,"xiaomee=%u roomid=%u",p->xiaomee, p->roomid);
		p->shop_state=shop_init;
		p->roomid=0;
		Cmessage out;
		send_msg_to_homeserv(p->id, p->tiles->id, home_trade_leave_map_cmd ,&out);
	}
	if (IS_COPY_MAP(p->tiles->id) && !IS_COPY_MAP(newmap)) {
		map_copy_instance_t* pmci = get_map_copy_with_mapid(p->tiles->id);
		if (pmci) {
			leave_map_copy(pmci, p);
		}
	}

	if( IS_HOME_MAP(p->tiles->id)){
		sprite_where_t* spwhere    = (sprite_where_t *)g_hash_table_lookup(maps, &(p->tiles->id));
		if( spwhere ){
			home_del_a_user(spwhere,p->id);	
		}
	}
	map_t* m = p->tiles;
	assert(m->sprite_num > 0);
	response_proto_leave_map(p, newmap, 0,1);
	--(m->sprite_num);
	list_del(&p->map_list);

	try_free_map(m);
	p->tiles = NULL;
}

map_t* get_map(map_id_t map_id)
{
	map_t *p_map;
	if (IS_NORMAL_MAP(map_id)) {
		return normal_map[map_id].id ? &normal_map[map_id] : NULL;
	}
	list_for_each_entry(p_map, &map_hash_slots[map_id % MAP_HASH_SLOT_NUM], hash_list) {
		if (p_map->id == map_id)
			return p_map;
	}
	return NULL;
}

int enter_map(sprite_t *p, map_id_t new_map_id)
{
	map_t* tile = get_map(new_map_id);
	reset_common_action(p);
	p->direction = 0;
	if (!tile && IS_NORMAL_MAP(new_map_id)) {
		KERROR_LOG(p->id, "invalid map\t[%lu]", new_map_id);
		return -1;
	}

	if (!tile && !IS_NORMAL_MAP (new_map_id)){
		tile = alloc_map (new_map_id);
	}

	KDEBUG_LOG(p->id, "USER MAPID [ map_type=%u mapid-low=%u]", high32_val(new_map_id), low32_val(new_map_id));

	p->tiles = tile;
	list_add_tail (&p->map_list, &tile->sprite_list_head);
	p->tiles->sprite_num++;
	return 0;
}

void response_proto_enter_map(sprite_t* p, int complet,int self)
{
	int i = sizeof (protocol_t);
	i += pkg_sprite_simply(p, msg + i);
			
	init_proto_head (msg, cli_enter_map_cmd, i);
	KDEBUG_LOG(p->id, "RESPONSE TO ENTER MAP\t[%u %u]", high32_val(p->tiles->id), low32_val(p->tiles->id));
	send_to_map (p, msg, i, complet,self);
}

static inline int 
get_map_normal_entry(sprite_t* p, map_t* pm_new, entry_pos_t* pep)
{
	if (!p->tiles) 
	{	//KDEBUG_LOG(p->id,"yyyyy");
		return cli_err_havnot_map_entry;
	}
	if (!pm_new->flag \
		|| MAP_AREA_OPEN(p->tiles->area, pm_new->area) \
		|| team_boss_map_registered(p, pm_new)) {
		return 0;
	}

	if(p->portal == pep->mapid)
		return 0;

	for (int loop = 0; loop < p->tiles->adjacent_cnt; loop ++) {
		entry_pos_t* p_entry = &p->tiles->adjacent_maps[loop];
		KDEBUG_LOG(p->id, "ENTRY INFO\t[%lu %lu %u %u %u %u]", \
			p_entry->mapid, pep->mapid, p_entry->des_x, p_entry->des_y, pep->des_x, pep->des_y);
		if (p_entry->mapid == pep->mapid && p_entry->des_x == pep->des_x \
			&& p_entry->des_y == pep->des_y && near_pos(p, p_entry->cur_x, p_entry->cur_y)) {
			if (p_entry->grpid && p_entry->grpid != team_last_kill_beastgrp(p))
				return cli_err_have_not_win_boss;
			return 0;
		}
	}
	return cli_err_havnot_map_entry;
	
}

static inline int
get_team_leader_map_entry(sprite_t* p, map_t* pm, uint32_t jumptype, entry_pos_t* pep)
{
	if (jumptype == jump_type_bird_cost || jumptype == jump_type_bird_free) {
		KDEBUG_LOG(p->id, "JUMP USE BIRD MAP\t[%u]", jumptype);
		if (!team_chocobo_registered(p, pm))
			return cli_err_chocobo_not_existed;
		if (jumptype == jump_type_bird_free)
			return 0;
		if (p->xiaomee < 200)
			return cli_err_xiaomee_not_enough;
		p->xiaomee -= 200;
		db_add_xiaomee(0, p->id, -200);
		monitor_sprite(p, "BIRD MAP");
		return 0;
	}

	if (jumptype == jump_type_with_item) {
		if (!cache_item_have_cnt(p, 230024, 1))
			return cli_err_havnot_this_item;
		db_add_item(NULL, p->id, 230024, -1);
		cache_reduce_kind_item(p, 230024, 1);
		return 0;
	}
	
	if ((pm->id == p->recv_map) \
		|| (p->tiles && !IS_NORMAL_MAP(p->tiles->id) && pm->home_ret)) {
		return 0;
	}

	if (pm->fin_task && !cache_check_task_node_ever_fin(p, pm->fin_task, pm->fin_task_node)) {
		KERROR_LOG(p->id, "task not fin can not enter\t[%lu %u %u]", pm->id, pm->fin_task, pm->fin_task_node);
		return cli_err_task_not_finished;
	}
	
	KDEBUG_LOG(p->id, "LEADER MAP TASK\t[%u %u]", pm->tasks[0], pm->tasks[1]);
	if (pm->tasks[0] && cache_check_task_node_ever_fin(p, pm->tasks[0], pm->nodes[0]) \
		&& pm->tasks[1] && !cache_check_task_node_ever_fin(p, pm->tasks[1], pm->nodes[1]))
		return 0;
	
	return get_map_normal_entry(p, pm, pep);
}

void kick_teamate_invalid(batter_team_t* pbt, uint32_t taskid, uint32_t nodeid)
{
	int loop = 0, tcnt = pbt->count;
	for (loop = 0; loop < tcnt && tcnt > 1; loop ++) {
		sprite_t* s = pbt->players[loop];
		if (s && !cache_check_task_node_ever_fin(s, taskid, nodeid) \
			&& !del_mbr_from_bt(pbt, s->id)) {
			tcnt --;
			loop --;
		}
	}
}

int cli_enter_map(sprite_t *p, Cmessage* c_in)
{
	cli_enter_map_in* p_in = P_IN;
	map_id_t newmap = p_in->maptype;
	newmap = p_in->mapid + (newmap << 32);

	CHECK_POS_VALID(p, p_in->posx, p_in->posy);

	KDEBUG_LOG(p->id, "ENTER MAP\t[%u %u %u %u %u]", p_in->mapid, p_in->maptype, p_in->posx, p_in->posy, p_in->jumptype);

	if (p->tiles && p->tiles->id == newmap)
		return send_to_self_error(p, p->waitcmd, cli_err_same_map, 1);
	CHECK_TEAM_WAIT_ANIMATION(p);

	int ret = 0;
	entry_pos_t entry = {newmap, 0, 0, p_in->posx, p_in->posy};
	
	if (p_in->jumptype == jump_type_no_check && idc_type >= idc_type_internal) {
		goto gogogo;
	} else if (p_in->jumptype == jump_type_no_check) {
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}
	if (IS_NORMAL_MAP(newmap)) {
		map_t* p_map = get_map(newmap);
		if (!p_map || !p_map->id || (p_in->jumptype >= jump_type_max))
			ERROR_RETURN(("Invalid map id or jumptype"), -1);

		if(newmap < 30201 || newmap > 30299) {
			p->flag &= ~(1 << flag_hide_bit);
		}

		p->recv_map = p_map->recv_map ? newmap : p->recv_map;

		if ((p->btr_team && TEAM_LEADER(p)) || !p->btr_team) {
			ret = get_team_leader_map_entry(p, p_map, p_in->jumptype, &entry);
			if (ret) {
				KERROR_LOG(p->id, "get map entry fail\t[%u %lu]", ret, newmap);
				return send_to_self_error(p, p->waitcmd, ret, 1);
			}
			if (p->btr_team && p_map->fin_task) {
				kick_teamate_invalid(p->btr_team, p_map->fin_task, p_map->fin_task_node);
			}
		} else if (p->btr_team) {
			sprite_t* lp = get_sprite(p->btr_team->teamid);
			if (!lp || !lp->tiles || lp->tiles->id != p_map->id) {
				KERROR_LOG(p->id, "teamate map entry fail\t[%u %lu]", p->btr_team->teamid, newmap);
				return send_to_self_error(p, p->waitcmd, cli_err_not_in_same_map, 1);
			}
		}
		p->team_state = 0;
	}
	else if(IS_HOME_MAP(newmap)) { 
		KDEBUG_LOG(p->id, "ENTER HOME MAP YET\t[%lu]", newmap);
		/*
		if (IS_WAR_MAP(newmap) && high32_val(newmap) != p->team_state) {
			KERROR_LOG(p->id, "tttt\t[%u %u]", p->team_state, high32_val(newmap));
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
		*/
	} 
	else if(IS_MAZE_MAP(newmap)){
		if(!p->portal || p->portal != newmap) {
			KDEBUG_LOG(p->id, "NOT FOUND PORTAL p->portal=(%u,%u),newmap=(%u,%u)",high32_val(p->portal), low32_val(p->portal),high32_val(newmap), low32_val(newmap));
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}

		map_t *p_map = get_map(newmap & 0xFFFFFFFF);//generate map id
		if(!p_map) {
			KDEBUG_LOG(p->id, "NOT FOUND PORTAL");
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}
		
		int index = 0;
		maze_t *mz = find_maze_layer(p,newmap,&index);
		if(!mz) {
			KDEBUG_LOG(p->id,"maze closed,current maze_id(%u)",p->tiles->maze_id);
			return send_to_self_error(p, p->waitcmd, cli_err_portal_have_closed, 1);
		}

		if(!get_map(newmap)) {
			map_t *tile = (map_t *)g_slice_alloc0(sizeof(map_t));
			memcpy(tile,p_map,sizeof(map_t));
			tile->id = newmap;
			tile->sprite_num = 0;
			tile->maze_id = p->tiles->maze_id;
			tile->shop_id = mz->layers[index].shopid;
			INIT_LIST_HEAD(&tile->hash_list);
			INIT_LIST_HEAD(&tile->sprite_list_head);
			list_add_tail(&tile->hash_list,&map_hash_slots[tile->id % MAP_HASH_SLOT_NUM]);
		}
	} 
	else if (IS_COPY_MAP(newmap)) {
		map_copy_instance_t* pmci = get_map_copy_with_mapid(newmap);
		if (!pmci) {
			KERROR_LOG(p->id, "invalid copy map\t[%lx]", newmap);
			return send_to_self_error(p, p->waitcmd, cli_err_havnot_map_entry, 1);
		}
		
		int layeridx = get_copy_layer_idx(pmci, newmap & 0xFFFFFFFF);
		if (layeridx == -1) {
			KERROR_LOG(p->id, "invalid copy map\t[%lx]", newmap);
			return send_to_self_error(p, p->waitcmd, cli_err_havnot_map_entry, 1);
		}

		bool enter_copy_map = false;
		for (uint32_t loop = 0; loop < pmci->usercnt; loop ++) {
			if (pmci->uids[loop] == p->id) {
				enter_copy_map = true;
				break;
			}
		}
		
		if (!enter_copy_map) {
			KERROR_LOG(p->id, "invalid copy map\t[%lx]", newmap);
			return send_to_self_error(p, p->waitcmd, cli_err_havnot_map_entry, 1);
		}

		if (!p->btr_team || TEAM_LEADER(p)) {
			copy_layer_t* layer = &pmci->p_copy_config->layers[layeridx];
			if (!IS_COPY_MAP(p->tiles->id) && layer->parentlayer) {
				KERROR_LOG(p->id, "not first layer\t[%u]", layer->parentlayer);
				return send_to_self_error(p, p->waitcmd, cli_err_havnot_map_entry, 1);
			}
			int idx=get_copy_layer_idx(pmci, p->tiles->id& 0xFFFFFFFF);
			if (layer->parentlayer !=0 && layer->parentlayer ==idx + 1) {
				copy_layer_t* p_layer=&pmci->p_copy_config->layers[idx];
				int i = p_layer->beast_orders - 1;
				for(uint32_t j=0; j<p_layer->beasts[i].beast_type; j++){
					if(pmci->killedcnts[idx][i][j]<p_layer->beasts[i].needkillcnts[j]){
						KERROR_LOG(p->id, "need kill beasts");
						return send_to_self_error(p, p->waitcmd, cli_err_havnot_map_entry, 1);
					}
				}
			}
			if (layer->needitem && layer->needitemcnt && !pmci->itemdel[layeridx]) {
				CHECK_ITEM_EXIST(p, layer->needitem, layer->needitemcnt);
				cache_reduce_kind_item(p, layer->needitem, layer->needitemcnt);
				db_add_item(NULL, p->id, layer->needitem, -layer->needitemcnt);
				pmci->itemdel[layeridx] = 1;
			}
			
		}
		else {
			sprite_t* lp = get_sprite(p->btr_team->teamid);
			if (!lp || !lp->tiles || lp->tiles->id != newmap) {
				KERROR_LOG(p->id, "teamate map entry fail\t[%u %lu]", p->btr_team->teamid, newmap);
				return send_to_self_error(p, p->waitcmd, cli_err_not_in_same_map, 1);
			}
		}

		if (!IS_COPY_MAP(p->tiles->id)) {
			item_t* pday = cache_get_day_limits(p, ssid_enter_copy_map_cnt);
			pday->count ++;
			db_day_add_ssid_cnt(NULL, p->id, ssid_enter_copy_map_cnt, 1, -1);
			p->team_state = 0;
			REMOVE_TIMERS(pmci);
		}
	} else {
		KERROR_LOG(p->id,"!!!wrong map!!!");
		return -1;
	}
	
gogogo:
	KDEBUG_LOG(p->id, "CDMAP gogogo");
	p->portal = 0;
	if (p->seatid) {
		clean_hangup_info(p);
		rsp_stop_hangup(p, 0, 0);
	}
	leave_map(p, entry.mapid);
	ret = enter_map(p, entry.mapid);

	if (!ret) {	
		if (p->btr_team && p->tiles->on_task) {
			task_db_t* ptr = cache_get_task_info(p, p->tiles->on_task);
			if (ptr && ptr->nodeid == p->tiles->on_task_node) {
				KDEBUG_LOG(p->id, "DEL FROM TEAM\t[%u %u]", ptr->taskid, ptr->nodeid);
				del_mbr_from_bt(p->btr_team, p->id);
			}
		}
		//if (!p->btr_team) {
			//memset(p->team_nick, 0, TEAM_NICK_LEN);
			//p->team_registerd = 0;
		//}
		p->posX = p->startX = entry.des_x;
		p->posY = p->startY = entry.des_y;
		broadcast_sprite_info(p);
		notify_next_fight_step(p);
		response_proto_enter_map(p, 1,1);

		if(p->last_beastgrp && (p->tiles->id <  21635 || p->tiles->id > 21640)) {
			p->last_beastgrp = 0;
			db_set_winbossid(NULL, p->id, 0);
		}
		notify_teamers_enter_map(p, entry.mapid, entry.des_x, entry.des_y);
		g_hash_table_foreach_remove(p->fireworks, free_fireworks, 0);
	}
	return ret;
}

int cli_get_map_usercnt(sprite_t* p, Cmessage* c_in)
{
	cli_get_map_usercnt_out cli_out;
	for (uint32_t grpid = BEGIN_CLEAN_BOSS; grpid <= END_CLEAN_BOSS; grpid ++) {
		beast_group_t* pbg = get_beast_grp(grpid);
		if (pbg && pbg->mapid) {
			map_t* pm = get_map(pbg->mapid);
			if (pm) {
				stru_id_cnt tmp;
				tmp.id = grpid;
				tmp.count = pm->sprite_num;
				cli_out.cntlist.push_back(tmp);
			}
		}
	}

	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int chk_teamate_enter_map_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	map_id_t mapid;
	int j = 0;
	unpkg_mapid(body, &mapid, &j);
	KDEBUG_LOG(p->id, "CHK TEAMATE ENTER MAP\t[%u %u]", high32_val(mapid), low32_val(mapid));
	CHECK_SELF_TEAM_LEADER(p, p->id, 1);
	if (!p->btr_team) {
		response_proto_uint32(p, p->waitcmd, 0, 1, 0);
		return 0;
	}

	map_t* pm = get_map(mapid);
	if (!IS_NORMAL_MAP(mapid) || !pm) {
		return send_to_self_error(p, p->waitcmd, cli_err_mapid_invalid, 1);
	}

	int i = sizeof(protocol_t) + 4;
	uint32_t user_cnt = 0;
	for (int loop = 0; loop < p->btr_team->count; loop ++) {
		sprite_t* s = p->btr_team->players[loop];
		if (s && pm->fin_task && !cache_check_task_node_ever_fin(s, pm->fin_task, pm->fin_task_node)) {
			PKG_UINT32(msg, s->id, i);
			user_cnt ++;
		}
	}

	j = sizeof(protocol_t);
	PKG_UINT32(msg, user_cnt, j);

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

typedef struct PackUserBuf {
	uint8_t** buf;
	int*      idx;
} pack_user_buf_t;


static void pack_across_svr_user(void* key, void* sinfo, void* data)
{
	pack_user_buf_t* pbuf = (pack_user_buf_t *)data;

	sprite_ol_t* p = (sprite_ol_t*)sinfo;
	//DEBUG_LOG("xxxxxxx %u",p->sprite_info.userid);
	int pkgsize = pkg_sprite_ol_simply(p, *(pbuf->buf));
	*(pbuf->buf) += pkgsize;
	++(*(pbuf->idx));
}

int do_list_user(sprite_t* p, map_id_t mapid)
{
	if (mapid != p->tiles->id || !IS_SPRITE_VISIBLE(p)) {
		KWARN_LOG(p->id, "request map=%lu while in map=%lu", mapid, p->tiles->id);
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, 1, i);
		i += pkg_sprite_simply(p, msg + i);
		init_proto_head(msg, p->waitcmd, i);
		return send_to_self(p, msg, i, 1);
	}

	list_head_t* t;
	int i = 0, pkgsize;
	uint8_t* tmp = msg + sizeof(protocol_t) + 4;
	list_for_each(t, &p->tiles->sprite_list_head) {
		sprite_t* l = list_entry(t, sprite_t, map_list);
		//DEBUG_LOG("xxxx %u %u",l->id,(IS_SPRITE_VISIBLE(l)));
		if (IS_SPRITE_VISIBLE(l)) {
			pkgsize = pkg_sprite_simply(l, tmp);
			tmp += pkgsize;
			++i;
		}
	}

	pack_user_buf_t pbuf = { &tmp, &i };
	traverse_across_svr_sprites(mapid, pack_across_svr_user, &pbuf);

	pkgsize = tmp - msg;
	*(uint32_t *)(msg + sizeof(protocol_t)) = htonl(i);
	init_proto_head(msg, proto_cli_map_users, pkgsize);
	send_to_self(p, msg, pkgsize, 1);

	return 0;
}

int get_map_users_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	map_id_t mapid;
    int i = 0;
	if (unpkg_mapid(body, &mapid, &i) == -1) return -1;
	KDEBUG_LOG(p->id, "LIST USER [mapid=%u %u]", high32_val(mapid), low32_val(mapid));
	do_list_user(p, mapid);
	
	notify_teamers_enter_map_done(p);
	return 0;
}

void notify_next_fight_step(sprite_t* p)
{
	if (p->tiles && MAP_HAS_BEAST(p->tiles)) {
		p->left_step = rand() % (p->tiles->rand_step + 1) + p->tiles->base_step;
		p->last_fight_step = p->left_step;
		p->start_walk_time = get_now_tv()->tv_sec;
		KDEBUG_LOG(p->id, "NEXT FIGHT STEP\t[%u]", p->left_step);
		response_proto_uint32(p, proto_cli_next_fight_step, p->left_step, 0, 0);
	}
}

int notify_user_reset_walk(sprite_t* p, int complet)
{
	uint8_t out[64];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, p->left_step, i);
	PKG_UINT32(out, p->startX, i);
	PKG_UINT32(out, p->startY, i);
	PKG_UINT32(out, p->posX, i);
	PKG_UINT32(out, p->posY, i);
	init_proto_head(out, proto_cli_reset_user_walk, i);
	if (complet) p->waitcmd = 0;
	return send_to_self(p, out, i, 0);
}

void set_teamate_pos(sprite_t* p)
{
	int count = p->btr_team ? p->btr_team->count : 0;
	for (int loop = 0; loop < count; loop ++) {
		sprite_t* s = p->btr_team->players[loop];
		if (s && s != p && s->tiles == p->tiles) {
			s->startX = s->posX = p->posX;
			s->startY = s->posY = p->posY;
		}
	}
}

int cli_user_walk(sprite_t *p,  Cmessage * c_in )
{	
	cli_user_walk_in *p_in = P_IN;
 
	uint32_t step=p_in->step;

	//KDEBUG_LOG(p->id, "WALK\t[%u %u %u %u %u %u %u %u]", p_in->type, p_in->direction, p->left_step, step, p_in->sx, p_in->sy, p_in->ex, p_in->ey);

	CHECK_POS_VALID(p, p_in->sx, p_in->sy);
	CHECK_POS_VALID(p, p_in->ex, p_in->ey);
	CHECK_SELF_TEAM_LEADER(p, p->id, 1);
	CHECK_USER_IN_MAP(p, 0);

	uint32_t walkstep = distance(p_in->sx, p_in->sy, p->startX, p->startY) / 5.5;
	if (p->tiles->check && MAP_HAS_BEAST(p->tiles)) {	
		if (walkstep > step + 55 || p->left_step < step) {
			KERROR_LOG(p->id, "INvalid walk info\t[%u %u %u]", p->left_step, step, walkstep);
			return notify_user_reset_walk(p, 1);
		}
	}

	uint32_t timespan = get_now_tv()->tv_sec - p->walktime;
	if (timespan * 33 + 22 < walkstep) {
		KERROR_LOG(p->id, "walk often\t[t=%u s=%u]", timespan, walkstep);
	}

	if (p->seatid) {
		clean_hangup_info(p);
		rsp_stop_hangup(p, 0, 0);
	}

	reset_common_action (p);	
	p->direction = p_in->direction;
	p->startX = p_in->sx;
	p->startY = p_in->sy;
	p->posX = p_in->ex;
	p->posY = p_in->ey;
	p->left_step = p->left_step > step ? (p->left_step - step) : 0;

	set_teamate_pos(p);

	cli_user_walk_out out;
	out.id=p->id;
	out.posX=p->posX;
	out.posY=p->posY;
	out.left_step=p->left_step;
	out.direction=p->direction;
	out.type=p_in->type;
	send_msg_to_map(p, cli_user_walk_cmd, &out, 1,1);
	return 0;
}

int register_boss_map_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	KDEBUG_LOG(p->id, "REGISTER BOSS MAP\t[%lu]", p->tiles->id);
	CHECK_USER_IN_MAP(p, 0);
	if (!IS_NORMAL_MAP(p->tiles->id) || !p->tiles->transmap) {
		return send_to_self_error(p, p->waitcmd, cli_err_mapid_invalid, 1);
	}
	
	uint32_t mapid = p->tiles->id;
	return send_request_to_db(p, p->id, proto_db_register_boss_map, &mapid, 4);
}

int register_boss_map_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret)
{
	uint32_t result = ret ? 1 : 0;
	cache_add_boss_map(p, p->tiles->id);
	response_proto_uint32(p, p->waitcmd, result, 1, 0);
	return 0;
}

static void pkg_boss_map(gpointer key, gpointer value, gpointer data)
{
	map_info_t* pm = (map_info_t *)value;
	if ((pm->state & MAP_INFO_BOSS_MAP_BIT) == 0)
		return;
	PKG_UINT32(msg, pm->mapid, (*(int *)data));
}

int get_boss_maps_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int k = sizeof(protocol_t) + 4;
	g_hash_table_foreach(p->map_infos, pkg_boss_map, &k);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, ((k - sizeof(protocol_t) - 4) / 4), i);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int check_team_boss_map_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t mapid;
	int j = 0;
	UNPKG_UINT32(body, mapid, j);
	map_t* pm = get_map(mapid);
	if (!pm) {
		KERROR_LOG(p->id, "mapid invalid\t[%u]", mapid);
		return send_to_self_error(p, p->waitcmd, cli_err_mapid_invalid, 1);
	}

	CHECK_SELF_TEAM_LEADER(p, mapid, 1);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, mapid, i);
	if (!p->btr_team) {
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, p->id, i);
		PKG_UINT32(msg, cache_boss_map_registerd(p, pm), i);
	} else {
		PKG_UINT32(msg, p->btr_team->count, i);
		int loop;
		for (loop = 0; loop < p->btr_team->count; loop ++) {
			sprite_t* s = p->btr_team->players[loop];
			if (s) {
				PKG_UINT32(msg, s->id, i);
				PKG_UINT32(msg, cache_boss_map_registerd(s, pm), i);
			}
		}
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
		
}

int team_chocobo_registered(sprite_t* p, map_t* pm)
{
	if (!p->tiles || !p->tiles->chocobo || !pm->chocobo)
		return 0;
	if (!p->btr_team)
		return cache_bird_map_registerd(p, pm);
	for (int loop = 0; loop < p->btr_team->count; loop ++) {
		sprite_t* s = p->btr_team->players[loop];
		if (s && !cache_bird_map_registerd(s, pm)) {
			return 0;
		}
	}
	return 1;
}

int team_boss_map_registered(sprite_t* p, map_t* pm)
{
	if (!p->tiles || !pm->transmap || p->tiles->id != pm->transmap)
		return 0;
	if (!p->btr_team)
		return cache_boss_map_registerd(p, pm);
	for (int loop = 0; loop < p->btr_team->count; loop ++) {
		sprite_t* s = p->btr_team->players[loop];
		if (s && !cache_boss_map_registerd(s, pm)) {
			return 0;
		}
	}
	return 1;
}


int cache_bird_map_registerd(sprite_t* p, map_t* pm)
{
	if (pm->chocobo) {
		uint32_t mapid = pm->id;
		map_info_t* mi = cache_get_map_info(p, mapid);
		return mi && (mi->state & MAP_INFO_BIRD_MAP_BIT);
	}
	return 0;
}

int cache_boss_map_registerd(sprite_t* p, map_t* pm)
{
	if (pm->transid) {
		uint32_t mapid = pm->id;
		map_info_t* mi = cache_get_map_info(p, mapid);
		return mi && (mi->state & MAP_INFO_BOSS_MAP_BIT);
	}
	return 0;
}

int get_map_state_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	CHECK_USER_IN_MAP(p, 0);
	int i = sizeof(protocol_t);
	PKG_MAP_ID(msg, p->tiles->id, i);
	PKG_UINT32(msg, p->tiles->state_cnt, i);
	for (int loop = 0; loop < p->tiles->state_cnt; loop ++) {
		PKG_UINT32(msg, p->tiles->states[loop].id, i);
		PKG_UINT32(msg, p->tiles->states[loop].state, i);
		KDEBUG_LOG(p->id, "GET MAP STATE\t[%u %u]", p->tiles->states[loop].id, p->tiles->states[loop].state);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

map_state_t* get_map_state(map_t* mp, uint32_t stateid)
{
	if (!stateid)
		return NULL;

	for (int loop = 0; loop < mp->state_cnt; loop ++) {
		if (mp->states[loop].id == stateid)
			return &mp->states[loop];
	}

	return NULL;
}

void response_map_state_change(map_t* mp, map_state_t* pms)
{
	uint8_t out[64];
	int i = sizeof(protocol_t);
	PKG_MAP_ID(out, mp->id, i);
	PKG_UINT32(out, pms->id, i);
	PKG_UINT32(out, pms->state, i);

	init_proto_head(out, proto_cli_set_map_state, i);
	send_to_map2(mp, out, i);
}

int set_map_state_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	CHECK_USER_IN_MAP(p, 0);
	uint32_t stateid, state;
	int j = 0;
	UNPKG_UINT32(body, stateid, j);
	UNPKG_UINT32(body, state, j);

	map_state_t* pms = get_map_state(p->tiles, stateid);
	if (!pms || state >= MAX_STATE_CNT || pms->trans[pms->state][state]) {
		KERROR_LOG(p->id, "invalid state\t[%u %u]", stateid, state);
		return -1;
	}

	KDEBUG_LOG(p->id, "SET MAP STATE\t[%u %u %u %u]", stateid, state, pms->state, pms->trans[state][pms->state]);

	if (pms->state == state) {
		p->waitcmd = 0;
		return 0;
	}

	pms->state = state;

	int change_time = 0;
	for (uint32_t loop = 0; loop < MAX_STATE_CNT; loop ++) {
		if (pms->trans[state][loop] > 0 && (!change_time || pms->trans[state][loop] < change_time)) {
			change_time = pms->trans[state][loop];
			pms->to_state = loop;
		}
	}
	if (change_time) {
		ADD_ONLINE_TIMER(p->tiles, n_map_state_change, pms, 30);
	}
	response_map_state_change(p->tiles, pms);
	p->waitcmd = 0;
	return 0;
}

int map_state_change(void* owner, void* data)
{
	map_t* mp = (map_t*)owner;
	map_state_t* pms = (map_state_t *)data;
	pms->state = pms->to_state;

	response_map_state_change(mp, pms);
	KDEBUG_LOG(0, "MAP STATE\t[%u %u]", pms->id, pms->state);
	return 0;
}

void rsp_elite_beast(sprite_t* p, map_t* mp)
{
	cli_get_elite_beast_out cli_out;
	cli_out.mapid = mp->id;
	for (uint32_t loop = 0; loop < mp->elite_cnt; loop ++) {
		stru_elite_beast tmpbeast;
		tmpbeast.beastid = mp->ebeasts[loop].beastid;
		tmpbeast.count1 = mp->ebeasts[loop].count[0];
		tmpbeast.count2 = mp->ebeasts[loop].count[1];
		cli_out.elite_beasts.push_back(tmpbeast);
	}

	if (p)
		send_msg_to_self(p, cli_get_elite_beast_cmd, &cli_out, 1);
	else
		send_msg_to_map2(mp, cli_get_elite_beast_cmd, &cli_out);
}

int cli_get_elite_beast(sprite_t* p, Cmessage* c_in)
{
	CHECK_USER_IN_MAP(p, 0);
	rsp_elite_beast(p, p->tiles);
	return 0;
}

int refresh_elite_beast(void* owner, void* data)
{
	map_t* mp = (map_t*)owner;
	if (mp->check || !mp->elite_cnt || mp->id == 11503)
		return 0;

	for (uint32_t loop = 0; loop < mp->elite_cnt; loop ++) {
		if (!data) {
			mp->ebeasts[loop].count[0] = 1;
		} else {
			mp->ebeasts[loop].count[1] = 1;
		}
	}
	rsp_elite_beast(NULL, mp);
	return 0;
}

int set_play_mode_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	uint32_t single;
	UNPKG_UINT32(body, single, j);

	if(single) {
		if(IS_SPRITE_VISIBLE(p)) {
			response_proto_leave_map(p, p->tiles ? p->tiles->id : 0, 0, 0);
			p->flag |= (1 << flag_hide_bit);
		}
	} else {
		if(!IS_SPRITE_VISIBLE(p)) {
			p->flag &= ~(1 << flag_hide_bit);
			response_proto_enter_map(p, 0, 0);
		}
	}

	response_proto_head(p,p->waitcmd,1,0);
	return 0;
}

void send_trade_pkg_ex(void* key, void* spri, void* userdata)
{
	var_pkg_t* vpkg = (var_pkg_t *)userdata;
	sprite_t* p = (sprite_t *)spri;
	KDEBUG_LOG(p->id,"send_trade_pkg1111");
	if(p->tiles && IS_TRADE_MAP(p->tiles->id)){
		KDEBUG_LOG(p->id,"send_trade_pkg_ex");
		send_to_self(p, (uint8_t *)vpkg->pkg, vpkg->pkg_len, 0);
	}
}

int send_to_trade_map(void* buf, int len)
{
	var_pkg_t vpkg = { buf, len, -1 };
	g_hash_table_foreach(all_sprites, send_trade_pkg_ex, &vpkg);
	return 0;
}

