
#include "pet.h"
#include "beast.h"
#include "skill.h"
#include "items.h"


void init_beast_grp()
{
	beast_grp = g_hash_table_new(g_int_hash, g_int_equal);
}

static gboolean free_beastgrp_by_key(gpointer key, gpointer grp, gpointer userdata)
{
	g_slice_free1(sizeof(beast_group_t), grp);
	return TRUE;
}

void fini_beast_grp()
{
	g_hash_table_destroy(beast_grp);
}

beast_group_t* get_beast_grp(uint32_t grpid)
{
	return (beast_group_t *)g_hash_table_lookup(beast_grp, &grpid);
}

void log_beast_grp(beast_group_t* pbg, uint32_t uid)
{
	KTRACE_LOG(uid, "BEAST GRP\t[%u %u %u]", pbg->grpid, pbg->count, pbg->honor_id);
	for (int loop = 0; loop < BATTLE_POS_CNT; loop ++) {
		base_attr_t* pba = &pbg->saset[loop];
		if (pba->id) {
			KTRACE_LOG(uid, "POS INFO\t[%u %u %u %u %u %u]", loop, pba->id, pba->minlv, pba->maxlv, pba->hp_max, pba->mp_max);
		}
	}
}

int load_grp_for_clean(beast_group_t* pt, xmlNodePtr cur, int* baselv, int pos_cnt, int* pos, uint32_t beastid)
{
	if (pt->idcnt >= BATTLE_POS_CNT)
		ERROR_RETURN(("too many beastid"), -1);

	base_attr_t* p_sa = &pt->idlist[pt->idcnt];
	p_sa->id = beastid;
	decode_xml_prop_uint16_default(&p_sa->lvtype, cur, "LvType", 0);
	decode_xml_prop_uint32_default((uint32_t *)&p_sa->hp_max, cur, "HPCoefficient", 100);
	decode_xml_prop_uint32_default((uint32_t *)&p_sa->mp_max, cur, "MPCoefficient", 100);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->attack, cur, "AtkCoefficient", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->mattack, cur, "MatkCoefficient", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->defense, cur, "DefCoefficient", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->mdefense, cur, "MDef", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->speed, cur, "SpeedCoefficient", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->dodge, cur, "Avd", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rpoison, cur, "Rpoison", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rlithification, cur, "Rlithification", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rlethargy, cur, "Rlethargy", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rinebriation, cur, "Rcurse", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rconfusion, cur, "Rconfusion", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->roblivion, cur, "Roblivion", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->double_atk_flag, cur, "DoubleAtk", 0);
	p_sa->minlv = baselv[0];
	p_sa->maxlv = baselv[1];
	for (int loop = 0; loop < pos_cnt; loop ++) {
		pt->posinfo[pos[loop]][pt->cntlist[pos[loop]]] = pt->idcnt;
		KTRACE_LOG(0, "GRP CLEAN INFO\t[%u %u %u]", pos[loop], pt->cntlist[pos[loop]], pt->idcnt);
		pt->cntlist[pos[loop]] ++;
	}
	pt->idcnt ++;
	return 0;
}

int load_grp_for_rand(beast_group_t* pt, xmlNodePtr cur, int* baselv, int* cnts, int pos_cnt, int* pos, uint32_t beastid)
{
	if (pt->idcnt >= BATTLE_POS_CNT)
		ERROR_RETURN(("too many beastid"), -1);
	base_attr_t* p_sa = &pt->idlist[pt->idcnt];
	p_sa->id = beastid;
	decode_xml_prop_uint16_default(&p_sa->lvtype, cur, "LvType", 0);
	decode_xml_prop_uint32_default((uint32_t *)&p_sa->hp_max, cur, "HPCoefficient", 100);
	decode_xml_prop_uint32_default((uint32_t *)&p_sa->mp_max, cur, "MPCoefficient", 100);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->attack, cur, "AtkCoefficient", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->mattack, cur, "MatkCoefficient", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->defense, cur, "DefCoefficient", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->mdefense, cur, "MDef", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->speed, cur, "SpeedCoefficient", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->dodge, cur, "Avd", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rpoison, cur, "Rpoison", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rlithification, cur, "Rlithification", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rlethargy, cur, "Rlethargy", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rinebriation, cur, "Rcurse", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rconfusion, cur, "Rconfusion", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->roblivion, cur, "Roblivion", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->percent, cur, "Percent",1000);
	decode_xml_prop_uint32_default((uint32_t *)&p_sa->itemid, cur, "ItemID", 0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->itemcnt, cur, "Count",0);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->rate, cur, "Rate",1000);
	decode_xml_prop_uint16_default((uint16_t *)&p_sa->double_atk_flag, cur, "DoubleAtk", 0);
	int tmp[2]={0};
	decode_xml_prop_arr_int_default(tmp,2,cur,"LeaderLv",0);
	p_sa->leaderlv[0]=(uint16_t)tmp[0];
	p_sa->leaderlv[1]=(uint16_t)tmp[1];
	p_sa->minlv = baselv[0];
	p_sa->maxlv = baselv[1];
	for (int loop = 0; loop < pos_cnt; loop ++) {
		pt->posinfo[pos[loop]][pt->cntlist[pos[loop]]] = pt->idcnt;
		KTRACE_LOG(0, "GRP RAND INFO\t[%u %u %u]", pos[loop], pt->cntlist[pos[loop]], pt->idcnt);
		pt->cntlist[pos[loop]] ++;
	}
	pt->idcnt ++;
	return 0;

}
int load_grp_for_other(beast_group_t* pt, xmlNodePtr cur, int* baselv, int* cnts, int pos_cnt, int* pos, uint32_t beastid)
{
	for(int loop = 0; loop < pos_cnt; loop ++) {
		base_attr_t* p_sa = NULL;
		if (pos[loop] == -1 && pt->idcnt < BATTLE_POS_CNT) {
			decode_xml_prop_arr_int_default(cnts, 2, cur, "BeastCnt", 0);
			cnts[1] = !cnts[1] ? BATTLE_POS_CNT : cnts[1];
			if (cnts[1] > BATTLE_POS_CNT || cnts[0] > cnts[1])
				ERROR_RETURN(("beast cnt invalid\t[%u %u %u]", pt->grpid, cnts[0], cnts[1]), -1);
			pt->idlist[pt->idcnt].id = beastid;
			pt->minlist[pt->idcnt] = cnts[0];
			pt->maxlist[pt->idcnt] = cnts[1];
			decode_xml_prop_uint32_default(&pt->percents[pt->idcnt], cur, "Percents", 1000);
			p_sa = &pt->idlist[pt->idcnt];
			pt->idcnt ++;
		} else {
			if (pos[loop] >= BATTLE_POS_CNT || pos[loop] < 0 || pt->saset[pos[loop]].id)  {
				ERROR_RETURN(("invalid or duplicate pos\t[%u %u %u]", pt->grpid, beastid, pos[loop]), -1);
			}
			pt->saset[pos[loop]].id = beastid;
			pt->count ++;
			p_sa = &pt->saset[pos[loop]];
		}
		
		decode_xml_prop_uint16_default(&p_sa->lvtype, cur, "LvType", 0);
		decode_xml_prop_uint32_default((uint32_t *)&p_sa->hp_max, cur, "HPCoefficient", 100);
		decode_xml_prop_uint32_default((uint32_t *)&p_sa->mp_max, cur, "MPCoefficient", 100);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->attack, cur, "AtkCoefficient", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->mattack, cur, "MatkCoefficient", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->defense, cur, "DefCoefficient", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->mdefense, cur, "MDef", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->speed, cur, "SpeedCoefficient", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->dodge, cur, "Avd", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->rpoison, cur, "Rpoison", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->rlithification, cur, "Rlithification", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->rlethargy, cur, "Rlethargy", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->rinebriation, cur, "Rcurse", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->rconfusion, cur, "Rconfusion", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->roblivion, cur, "Roblivion", 0);
		decode_xml_prop_uint16_default((uint16_t *)&p_sa->double_atk_flag, cur, "DoubleAtk", 0);
		p_sa->minlv = baselv[0];
		p_sa->maxlv = baselv[1];
		//if(pt->grpid==2002)
			//DEBUG_LOG("111111111 %u %u",beastid,p_sa->double_atk_flag);
	}
	return 0;
}

int load_beast_grp_config(xmlNodePtr cur)
{
	uint32_t grp_id;
	DECODE_XML_PROP_INT (grp_id, cur, "ID");
	beast_group_t* pt = get_beast_grp(grp_id);
	if (grp_id >= BEGIN_USER_ID || pt) 
		ERROR_RETURN(("invalid or duplicate beast grp id\t[%u]", grp_id), -1);

	pt = (beast_group_t *)g_slice_alloc0(sizeof(beast_group_t));
	pt->grpid = grp_id;
	g_hash_table_insert(beast_grp, &pt->grpid, pt);

	decode_xml_prop_uint32_default(&pt->honor_id, cur, "TitleID", 0);
	decode_xml_prop_uint32_default(&pt->allowteam, cur, "AllowTeam", 0);
	decode_xml_prop_uint32_default(&pt->mapid, cur, "MapID", 0);
	decode_xml_prop_uint32_default(&pt->gethonor, cur, "Title", 0);
	DECODE_XML_PROP_INT_DEFAULT(pt->limit_exp, cur, "LimitExp", -1);
	decode_xml_prop_uint32_default(&pt->type, cur, "GrpType", 0);
	decode_xml_prop_uint32_default(&pt->beasttype, cur, "BeastType", 0);
	decode_xml_prop_uint32_default(&pt->repeatcnt, cur, "RepeatCnt", 0);
	decode_xml_prop_uint32_default(&pt->bossid, cur, "BossID", 0);
	decode_xml_prop_uint32_default(&pt->minlv, cur, "MinRoleLv", 1);
	decode_xml_prop_uint32_default(&pt->maxlv, cur, "MaxRoleLv", MAX_SPRITE_LEVEL);
	decode_xml_prop_uint32_default(&pt->needitem, cur, "NeedItem", 0);
	decode_xml_prop_uint32_default(&pt->needcnt, cur, "NeedCount", 0);
	decode_xml_prop_uint32_default(&pt->npcid, cur, "NpcID", 0);
	pt->typeidcnt=decode_xml_prop_arr_int_default((int*)pt->typeids,BATTLE_POS_CNT, cur, "TypeID", 0);
	//DEBUG_LOG("ttttttttttttt %u %u",grp_id,pt->typeidcnt);
	KTRACE_LOG(0, "BEAST GRP TTT\t[%u %u %u %u %u]", pt->grpid, pt->type, pt->beasttype, pt->needitem, pt->needcnt);

	int cnts[2];
	decode_xml_prop_arr_int_default(cnts, 2, cur, "Cnt", 0);
	pt->mincnt = cnts[0];
	pt->maxcnt = cnts[1];
	if ((!pt->mincnt && pt->maxcnt) || (pt->maxcnt && pt->mincnt > pt->maxcnt))
		ERROR_RETURN(("invalid min max cnt\t[%u %u %u]", grp_id, pt->mincnt, pt->maxcnt), -1);

	uint32_t time[300];
	uint32_t cnt = decode_xml_prop_arr_time(time, 300, cur, "Time");
	if (!cnt) {
		for (uint32_t loop = 0; loop < DAYS_PER_WEEK; loop ++) {
			pt->time_valid[loop] = 0xFFFFFFFF;
		}
	} else if (cnt % 3) {
		KERROR_LOG(0, "time cnt err\t[%u]", cnt);
		return -1;
	} else {
		for (uint32_t loop = 0; loop < cnt / 3; loop ++) {
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

	for (uint32_t loop = 0; loop < BATTLE_POS_CNT; loop ++) {
		pt->percents[loop] = RAND_COMMON_RAND_BASE;
	}
			
	xmlNodePtr chl = cur->xmlChildrenNode; 
	pt->beastcnt=0;
	while (chl) {
		if (!xmlStrcmp(chl->name, (const xmlChar *)"Type")) {
			uint32_t beastid, pos_cnt;
			uint32_t max_pos = BATTLE_POS_CNT;
			int pos[BATTLE_POS_CNT];
			DECODE_XML_PROP_UINT32 (beastid, chl, "ID");
			pos_cnt = decode_xml_prop_arr_int_default(pos, max_pos, chl, "Position", max_pos);
			beast_t* pba = get_beast(beastid);
			if (!pba) ERROR_RETURN(("invalid beast id\t[%u %u]", grp_id, beastid), -1);
			
			int baselv[2];
			if (decode_xml_prop_arr_int_default(baselv, 2, chl, "BaseLv", 1) < 2) 
				baselv[1] = 0;
			//读取双击的beast信息
			uint16_t datk;
			decode_xml_prop_uint16_default(&datk, chl, "DoubleAtk", 0);
			if(datk){
				pt->datk_info[pt->beastcnt].beastid=beastid;
				pt->datk_info[pt->beastcnt].double_atk=datk;
				pt->beastcnt++;
				//DEBUG_LOG("1111111 %u %u %u %u",pt->grpid,beastid,pt->beastcnt,datk);
				if( pt->beastcnt > BATTLE_POS_CNT ){
					ERROR_RETURN(("double atk cnt error\t[%u]",pt->beastcnt),-1);
				}
			}
			if (baselv[1] && baselv[0] > baselv[1])
				ERROR_RETURN(("lv min > max\t[%u %u]", baselv[0], baselv[1]), -1);
			if (pt->type == group_type_clean && load_grp_for_clean(pt, chl, baselv,pos_cnt, pos, beastid)) {
				return -1;
			}else if(pt->type == group_type_rand && load_grp_for_rand(pt, chl, baselv, cnts, pos_cnt, pos, beastid)){
				return -1;
			}else if(pt->type != group_type_clean && pt->type != group_type_rand && 
						load_grp_for_other(pt, chl, baselv, cnts, pos_cnt, pos, beastid)) {
				return -1;
			}
			if(!pt->bossid && beastid >= 2000 && beastid < 3000)
				pt->bossid = beastid;
		}

		if (!xmlStrcmp(chl->name, (const xmlChar *)"DropItem")) {
			pt->drop_item_cnt = decode_xml_prop_arr_int_default((int *)pt->drop_items, MAX_DROP_ITEM_TYPE, chl, "ItemID", 0);
			if (pt->drop_item_cnt != decode_xml_prop_arr_int_default((int *)pt->rates, MAX_DROP_ITEM_TYPE, chl, "Rate", 0)) {
				ERROR_LOG("invalid item cnt\t[%u %u]", pt->grpid, pt->drop_item_cnt);
				return -1;
			}
			decode_xml_prop_arr_int_default((int *)pt->counts, MAX_DROP_ITEM_TYPE, chl, "Count", 1);
			uint32_t allrate = 0;
			for (uint32_t loop = 0; loop < pt->drop_item_cnt; loop ++) {
				allrate += pt->rates[loop];
				if (pt->drop_items[loop] && !get_item(pt->drop_items[loop])) {
					ERROR_LOG("invalid item\t[%u %u]", pt->grpid, pt->drop_items[loop]);
					return -1;
				}
			}

			/*
			if (allrate != RAND_COMMON_RAND_BASE) {
				ERROR_LOG("invalid rate\t[%u %u]", pt->grpid, allrate);
				return -1;
			}
			*/

			int tmp[2];
			decode_xml_prop_arr_int_default(tmp, 2, chl, "DropID", 0);
			if (tmp[0] && tmp[1] && get_item(tmp[0])) {
				pt->certain_drop_item = tmp[0];
				pt->certain_drop_cnt = tmp[1];
			}
		}
		log_beast_grp(pt, 0);
		chl = chl->next;
	}

	uint32_t mincnt = pt->count, maxcnt = pt->count;
	for (uint32_t loop = 0; loop < BATTLE_POS_CNT; loop ++) {
		mincnt += pt->minlist[loop];
		maxcnt += pt->maxlist[loop];
	}

	pt->mincnt = pt->mincnt < mincnt ? mincnt : pt->mincnt;
	pt->maxcnt = pt->maxcnt > maxcnt ? maxcnt : pt->maxcnt;
	return 0;
}

int load_beast_grp_refresh(xmlNodePtr cur)
{
	refresh_group_t* prg = &refresh_grps[refresh_group_cnt];
	prg->id = refresh_group_cnt + 1;
	decode_xml_prop_uint32_default(&prg->refresh_type, cur, "RefreshType", refresh_immediately | refresh_time_speed);
	DECODE_XML_PROP_INT (prg->refresh_time, cur, "RefreshTime");
	DECODE_XML_PROP_INT (prg->refresh_speed, cur, "RefreshSpeed");
	DECODE_XML_PROP_INT (prg->start_time, cur, "StartHour");
	prg->start_time *= 3600;
	uint32 tmp=0;
	decode_xml_prop_uint32_default(&tmp, cur, "StartMinute", 0);
	prg->start_time += tmp*60;
	DECODE_XML_PROP_INT (prg->end_time, cur, "EndHour");
	prg->end_time *= 3600;
	decode_xml_prop_uint32_default(&tmp, cur, "EndMinute", 0);
	prg->end_time += tmp*60;
	decode_xml_prop_uint32_default(&prg->disapear, cur, "Disapear", 1);
	decode_xml_prop_uint32_default(&prg->private_refresh_cnt, cur, "RepeatCnt", 0);
	decode_xml_prop_uint32_default(&prg->day, cur, "Day", 0);
	DECODE_XML_PROP_INT (prg->count, cur, "Cnt");
	xmlNodePtr chl = cur->xmlChildrenNode; 
	while (chl) {
		if (!xmlStrcmp(chl->name, (const xmlChar *)"Grp")) {
			prg->grp_cnt = decode_xml_prop_arr_int_default((int *)prg->grpids, MAX_ONE_REFRESH_GRP, chl, "IDs", 0);
			prg->map_cnt = decode_xml_prop_arr_int_default((int *)prg->mapids, MAX_ONE_REFRESH_MAP, chl, "MapIDs", 0);
			if (prg->grp_cnt != decode_xml_prop_arr_int_default((int *)prg->minlist, MAX_ONE_REFRESH_GRP, chl, "MinCnt", 0))
				ERROR_RETURN(("invalid beast grp id"), -1);
			decode_xml_prop_arr_int_default(prg->startx, MAX_ONE_REFRESH_GRP, chl, "StartX", -1);
			decode_xml_prop_arr_int_default(prg->starty, MAX_ONE_REFRESH_GRP, chl, "StartY", -1);
			for (int loop = 0; loop < prg->grp_cnt; loop ++) {
				if (!get_beast_grp(prg->grpids[loop]))
				{
					KERROR_LOG(0,"invalid beast grp id [%u]",prg->grpids[loop]);
					ERROR_RETURN(("invalid beast grp id"), -1);
				}
			}
		}
		chl = chl->next;
	}
	refresh_group_cnt ++;
	return 0;
}

int load_beast_grp(xmlNodePtr cur)
{	
	refresh_group_cnt = 0;
	memset(refresh_grps, 0, sizeof(refresh_grps));
	g_hash_table_foreach_remove(beast_grp, free_beastgrp_by_key, 0);
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Grp") && load_beast_grp_config(cur)) { 
			return -1;			
		}
		
		if (!xmlStrcmp(cur->name, (const xmlChar *)"RefreshGrp") && load_beast_grp_refresh(cur)) { 
			return -1;			
		}
		cur = cur->next;
	}

	refresh_all_map_grp();

	return 0;
}

refresh_group_t* get_refresh_grp(uint32_t refreshid)
{
	return refreshid && refreshid <= refresh_group_cnt ? &refresh_grps[refreshid - 1] : NULL;
}

void rsp_map_grp_challenged(uint32_t refreshid, map_t* tile, uint32_t grpid)
{
	uint8_t out[64];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, refreshid, i);
	PKG_UINT32(out, grpid, i);
	PKG_UINT32(out, 1, i);
	init_proto_head(out, proto_cli_grp_challenged, i);
	send_to_map2(tile, out, i);
}

void rsp_self_grp_challenged(sprite_t *p,uint32_t refreshid, map_t* tile, uint32_t grpid)
{
	uint8_t out[64];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, refreshid, i);
	PKG_UINT32(out, grpid, i);
	PKG_UINT32(out, 1, i);
	init_proto_head(out, proto_cli_grp_challenged, i);
	send_to_self(p, out, i,0);
}

int reduce_map_grp(sprite_t* p,uint32_t refreshid, map_t* tile, uint32_t grpid)
{
	refresh_group_t* prg = get_refresh_grp(refreshid);
	if (!prg) return -1;
	//DEBUG_LOG("1111");	
	int mapidx = 0;
	for (mapidx = 0; mapidx < prg->map_cnt; mapidx ++) {
		if (tile->id == prg->mapids[mapidx])
			break;
	}

	//DEBUG_LOG("2111 %u %u",mapidx,prg->map_cnt);	
	if (mapidx == prg->map_cnt) 
		return -1;

	//DEBUG_LOG("3333333");	
	int grpidx = 0;
	for (grpidx = 0; grpidx < prg->grp_cnt; grpidx ++) {
		if (grpid == prg->grpids[grpidx])
			break;
	}

	//DEBUG_LOG("44444 %u %u  %u",grpidx,prg->grp_cnt,prg->counts[grpidx][mapidx]);	
	if (grpidx == prg->grp_cnt || !prg->counts[grpidx][mapidx]) 
		return -1;
	if (!prg->disapear)
		return 0;
	prg->counts[grpidx][mapidx] --;
	prg->total_cnt --;
	
	if (prg->timeout && !prg->total_cnt) {
		refresh_one_group(prg);
		prg->timeout = 0;
	}
	switch(prg->disapear){
		case 1://正常消失的
			rsp_map_grp_challenged(refreshid, tile, grpid);
			break;
		case 2:{//for 稀有精灵每天限制次数
			rsp_self_grp_challenged(p,refreshid, tile, grpid);
			if(p->times_for_hunting_rarebeast < prg->private_refresh_cnt){
				p->times_for_hunting_rarebeast++;
				db_day_add_ssid_cnt(0,p->id,ssid_hunt_rarebeast_times,1,prg->private_refresh_cnt);
			}
		}
	}
	return prg->counts[grpidx][mapidx];
}

void try_end_group(refresh_group_t* prg)
{
	for (int loop = 0; loop < prg->grp_cnt; loop ++) {
		for (int loopi = 0; loopi < prg->map_cnt; loopi ++) {
			prg->counts[loop][loopi] = 0;
		}
	}
}

void try_start_group(refresh_group_t* prg)
{
	prg->total_cnt = 0;
	memset(prg->counts, 0, sizeof(prg->counts));
	int refresh_cnt = prg->count;
	if (prg->refresh_type | refresh_cnt_speed) {
		refresh_cnt += prg->refresh_speed * sprites_count / 50;
	}
	
	for (int loop = 0; loop < prg->map_cnt; loop ++) {
		int map_beast_cnt = 0;
		for (int loopi = 0; loopi < prg->grp_cnt; loopi ++) {
			prg->counts[loopi][loop] = prg->minlist[loopi];
			map_beast_cnt += prg->counts[loopi][loop];
			//KDEBUG_LOG(0, "START GROUP\t[%u %u i=%u,j=%u %u]", prg->mapids[loop], prg->grpids[loopi],loopi,loop , prg->counts[loopi][loop]);
		}
		
		for (int loopi = 0; loopi < refresh_cnt - map_beast_cnt; loopi ++) {
			int idx = rand() % prg->grp_cnt;
			prg->counts[idx][loop] += 1;
			//KDEBUG_LOG(0, "EX START GROUP\t[%u %u i=%u,j=%u  %u]", prg->mapids[loop], prg->grpids[idx],idx,loop ,prg->counts[idx][loop]);
		}
		
		prg->total_cnt += map_beast_cnt;
	}
}

int refresh_one_group(refresh_group_t* prg)
{
	uint32_t seconds = get_now_sec();

	try_end_group(prg);

	if (seconds < prg->start_time) {
		seconds = prg->start_time - seconds;
		//KDEBUG_LOG(prg->id, "WAIT REFRESH TODAY\t[%u],prg->start_time:[%u] [%u]", seconds,prg->start_time,prg->id);
		ADD_ONLINE_TIMER(&g_events, n_refresh_beast_grp, prg, seconds);
	} else if (seconds < prg->end_time) {
		int real_refresh_time = prg->refresh_time;
		if (prg->refresh_type & refresh_time_speed) {
			real_refresh_time -= prg->refresh_speed * (sprites_count / 50);
			if (real_refresh_time < 200)
				real_refresh_time = 200;
		}
		//KDEBUG_LOG(prg->id, "REFRESH NOW\t[%u %u %u]", prg->start_time,real_refresh_time,prg->id);
		ADD_ONLINE_TIMER(&g_events, n_refresh_beast_grp, prg, real_refresh_time);
		try_start_group(prg);
	} else {
		seconds = 24 * 3600 + prg->start_time - seconds;
		//KDEBUG_LOG(prg->id, "WAIT REFRESH TOMORROW\t[%u %u]", seconds,prg->id);
		ADD_ONLINE_TIMER(&g_events, n_refresh_beast_grp, prg, seconds);
	}
	return 0;
}

int get_grp_type_id(beast_group_t* pbg)
{
	if (pbg->idcnt) {
	//	DEBUG_LOG("get_grp_type_id pbg->idcnt :%d ",pbg->idcnt  );
		return pbg->idlist[rand() % pbg->idcnt].id % SPEC_BEAST_BASE;
	}

	int startidx = rand() % (BATTLE_POS_CNT), loop;
	for (loop = 0; loop < BATTLE_POS_CNT; loop ++) {
	//	DEBUG_LOG("get_grp_type_id pbg->saset[startidx].id=%u ", pbg->saset[startidx].id);
		if (pbg->saset[startidx].id)
			return pbg->saset[startidx].id % SPEC_BEAST_BASE;
		startidx = (startidx + 1) % (BATTLE_POS_CNT);
	}
	return 0;
}
void get_map_grp_list(sprite_t *p,map_id_t mapid,std::vector<map_grp_item_t> &map_grp_list)
{
	for (uint32_t loop = 0; loop < refresh_group_cnt; loop ++) {
		refresh_group_t* prg = &refresh_grps[loop];
		for (int loopi = 0; loopi < prg->map_cnt; loopi ++) {
			if (prg->mapids[loopi] == mapid) {
				for (int loopj = 0; loopj < prg->grp_cnt; loopj ++) {
					beast_group_t* pbg = get_beast_grp(prg->grpids[loopj]);
					if (!pbg) continue; 
					//每日打怪次数限制检查
					int day_idx = get_now_tm()->tm_wday;
				    if(prg->day != 0 && ((prg->day>>day_idx)&0x1)==1){  
						KDEBUG_LOG(p->id,"refresh grp day limit[%u %u] ",prg->day,prg->id);
						continue;
					}	
					//KDEBUG_LOG(p->id,"ggggggggg%u %u",p->times_for_hunting_rarebeast,prg->private_refresh_cnt);
					//打稀有精灵次数检查
					if( prg->private_refresh_cnt > 0 && p != NULL 
							&& p->times_for_hunting_rarebeast >= prg->private_refresh_cnt) {
						continue;				
					}
					//KDEBUG_LOG(p->id,"ggggggggg%u %u",p->times_for_hunting_rarebeast,prg->private_refresh_cnt);
					map_grp_item_t item;
					item.refresh_id=prg->id;
					item.grpid=prg->grpids[loopj];
					if( pbg->typeidcnt ){
						uint32_t loop=rand()%pbg->typeidcnt;
						item.type_id=pbg->typeids[loop]%SPEC_BEAST_BASE;
						//KDEBUG_LOG(p->id,"typeid=%u",item.type_id);
					}else{
						item.type_id= get_grp_type_id(pbg);
					}
					item.posx= prg->startx[loopj];
					item.posy= prg->starty[loopj];
					item.count= prg->counts[loopj][loopi] ;

					map_grp_list.push_back(item);
				}
				break;
			}
		}
	}

}

void noti_map_grp_refreshed(refresh_group_t* prg)
{
	//TODO
	//DEBUG_LOG("11===noti_map_grp_refreshed==== prg:[%u]",prg->id);
	cli_get_map_grp_out  cli_out ;
	for (int loop = 0; loop < prg->map_cnt; loop ++) {
		const map_t* tile = get_map(prg->mapids[loop]);
		if(tile){
			list_head_t* p;
			list_for_each(p, &tile->sprite_list_head) {
				sprite_t* l = list_entry(p, sprite_t, map_list);
				if (IS_NORMAL_ID(l->id) && IS_SPRITE_VISIBLE(l)) {
					cli_out.map_grp_list.clear();
					get_map_grp_list(l,prg->mapids[loop], cli_out.map_grp_list );
					send_msg_to_self(l, cli_get_map_grp_cmd, &cli_out, 0);
				}
			}
		}
	}
	//DEBUG_LOG("11===noti_map_grp_refreshed==== END" );
}

int refresh_beast_grp(void* owner, void* data)
{
	//DEBUG_LOG("refresh_beast_grp BEGIN");
	refresh_group_t* prg = (refresh_group_t *)data;
	if (prg) {
		//DEBUG_LOG("REFRESH_BEAST_GRP id=%u ", prg->id );
		refresh_one_group(prg);
		noti_map_grp_refreshed(prg);
	}
	//DEBUG_LOG("refresh_beast_grp END");

	return 0;
}


void refresh_all_map_grp()
{
	//KDEBUG_LOG(0, "START REFRESH GRP\t[%u]", refresh_group_cnt);
	for (uint32_t loop = 0; loop < refresh_group_cnt; loop ++) {
		refresh_one_group(&refresh_grps[loop]);
		noti_map_grp_refreshed(&refresh_grps[loop]);
	}
}

int set_monster_handbook_info_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int j = 0;
	uint32_t grpid;
	uint16_t state;

	UNPKG_UINT32(body, grpid, j);
	UNPKG_UINT16(body, state, j);

	if(grpid <= 100000) {
		return send_to_self_error(p,p->waitcmd,cli_err_invalid_grpid,1);
	}

	beast_group_t *grp = get_beast_grp(grpid - 100000);

	if(!grp || !grp->beasttype) {
		return send_to_self_error(p,p->waitcmd,cli_err_invalid_grpid,1);
	}

	beast_kill_t* pbk = cache_get_beast_kill_info(p, grpid);
	pbk->state &= 0xFFFF;
	pbk->state |= (state << 16);

	db_set_monster_handbook(NULL, p->id, grpid, (state << 16), 0, 0);
	response_proto_head(p,p->waitcmd,1,0);
	return 0;
}

static void pkg_monster_handbook(gpointer key, gpointer value, gpointer data)
{
	beast_kill_t* pbk = (beast_kill_t *)value;
	uint32_t* pkey = (uint32_t *)data;
	if (pbk->beastid > pkey[1] || pbk->beastid < pkey[0])
		return;

	PKG_UINT32(msg, pbk->beastid, pkey[2]);
	PKG_UINT16(msg, pbk->state >> 16, pkey[2]);
	PKG_UINT16(msg, pbk->state & 0xFFFF, pkey[2]);
	PKG_UINT32(msg, pbk->count, pkey[2]);
}

int get_monster_handbook_info_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t key[3] = {0, 0, sizeof(protocol_t) + 4};
	int j = 0;

	UNPKG_UINT32(body, key[0], j);
	UNPKG_UINT32(body, key[1], j);

	g_hash_table_foreach(p->beast_handbook, pkg_monster_handbook, key);

	KDEBUG_LOG(p->id, "MONSTER HANDBOOK\t[%u %u %u]", key[0], key[1], key[2]);
	
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, ((key[2] - sizeof(protocol_t) - 4) / 12), i);
	//log_buf(p->id, msg, key[2]);
	
	init_proto_head(msg, p->waitcmd, key[2]);
	return send_to_self(p, msg, key[2], 1);
}

int get_monster_handbook_info_callback(sprite_t* p, userid_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	uint32_t records;
	CHECK_DBERR(p,ret);

	int i = 0;
	UNPKG_H_UINT32(buf,records,i);
	CHECK_BODY_LEN(len,4 + records * 12);
	while(records--) {
		beast_kill_t* pbk = (beast_kill_t *)g_slice_alloc(sizeof(beast_kill_t));
		UNPKG_H_UINT32(buf, pbk->beastid, i);
		UNPKG_H_UINT32(buf, pbk->state, i);
		UNPKG_H_UINT32(buf, pbk->count, i);

		g_hash_table_replace(p->beast_handbook, &(pbk->beastid), pbk);
		//KDEBUG_LOG(p->id,"grpid=%d state=%x count=%d", pbk->beastid, pbk->state, pbk->count);
	}

	mole2_get_vip_buff_in db_in;
	for (int i = 0; i < buffid_max_vip; i ++) {
		db_in.petid.push_back(i);
	}

	for (int i = buffid_beast_lv10; i < buffid_max_beast; i ++) {
		db_in.petid.push_back(i);
	}

	return send_msg_to_db(p, p->id, mole2_get_vip_buff_cmd, &db_in);
}

beast_kill_t* cache_get_beast_kill_info(sprite_t* p, uint32_t beastid)
{
	beast_kill_t* pbk = (beast_kill_t *)g_hash_table_lookup(p->beast_handbook, &beastid);
	if (!pbk) {
		pbk = (beast_kill_t *)g_slice_alloc(sizeof(beast_kill_t));
		pbk->beastid = beastid;
		pbk->count = 0;
		pbk->state = 0;
		g_hash_table_insert(p->beast_handbook, &(pbk->beastid), pbk);
	}
	return pbk;
}

int cli_get_map_grp(sprite_t *p, Cmessage* c_in)
{
	cli_get_map_grp_out  cli_out ;

	CHECK_USER_IN_MAP(p, 0);
	KDEBUG_LOG(p->id, "GET MAP GRP\t[%lu]", p->tiles->id);
	get_map_grp_list(p, p->tiles->id, cli_out.map_grp_list );
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}


