#include "box.h"
#include "beast.h"
#include "event.h"
#include "items.h"
#include "pet.h"
#include "sns.h"
inline bool is_133_134(uint32_t id ){
		return id==133|| id==134;
}

box_t* get_box_no_check(uint32_t boxid)
{
	return boxid && boxid <= MAX_BOX_CNT ? &all_boxes[boxid - 1] : NULL;
}

box_t* get_box(uint32_t boxid)
{
	box_t* pb = get_box_no_check(boxid);
	return pb && pb->id ? pb : NULL;
}

box_pos_t* get_box_pos_no_check(uint32_t pid)
{
	return pid && pid <= MAX_BOX_POS_CNT ? &box_pos[pid - 1] : NULL;
}

box_pos_t* get_box_pos(uint32_t pid)
{
	box_pos_t* pbp = get_box_pos_no_check(pid);
	return pbp && pbp->p_m ? pbp : NULL;
}

beast_group_t* get_box_beast_group(sprite_t* p)
{
	box_t* pb = get_box(p->boxid);
	if (!pb) return NULL;

	uint32_t count = p->btr_team ? p->btr_team->count : 1;
	count *= 2;

	beast_group_t* pbg = &pb->box_beast;
	pbg->count = count;

	KDEBUG_LOG(p->id, "BOX BEAST COUNT\t[%u]", count);
	uint32_t loop;
	for (loop = 0; loop < count; loop ++) {	
		int idx = rand_type_idx(pb->beastcnt, pb->beastrate, BEAST_COMMON_RAND_BASE);
		pbg->saset[loop].id = pb->beastids[idx];
	}

	for (loop = count; loop < BATTLE_POS_CNT; loop ++) {
		pbg->saset[loop].id = 0;
	}
	
	return &pb->box_beast;
}

int load_box_map_config(xmlNodePtr cur, box_t* pb)
{
	int posid[MAX_BOX_POS_CNT];
	pb->pos_cnt = decode_xml_prop_arr_int_default(posid, MAX_BOX_POS_CNT, cur, "ID", 0);
	uint32_t loop;
	for (loop = 0; loop < pb->pos_cnt; loop ++) {
		box_pos_t* pbp = get_box_pos(posid[loop]);
		if (!pbp) ERROR_RETURN(("box map id invalid"), -1);
		pb->pos[loop] = pbp;
	}
	pb->pbpactive = NULL;
	return 0;
}

int load_box_beast_config(xmlNodePtr cur, box_t* pb)
{
	DECODE_XML_PROP_UINT32(pb->beastrand, cur, "BeastRand");
	pb->beastcnt = decode_xml_prop_arr_int_default((int *)pb->beastids, MAX_BEAST_TYPE_PER_MAP, cur, "ID", 0);
	if (pb->beastcnt != decode_xml_prop_arr_int_default((int *)pb->beastrate, MAX_BEAST_TYPE_PER_MAP, cur, "BeastPercent", 0))
		ERROR_RETURN(("beast pecent not match\t[%u %u]", pb->id, pb->beastcnt), -1);

	int loop = 0, rate = 0;
	for (loop = 0; loop < pb->beastcnt; loop ++) {
		beast_t* pba = get_beast(pb->beastids[loop]);
		if (!pba || !pba->id) {
			ERROR_RETURN(("beast id invalid\t[%u %u]", pb->id, pb->beastids[loop]), -1);
		}
		rate += pb->beastrate[loop];
	}

	if (rate != BEAST_COMMON_RAND_BASE) {
		ERROR_RETURN(("beast total percent err\t[%u %u]", pb->id, rate), -1);
	}
	return 0;
}

int load_box_item_config(xmlNodePtr cur, box_t* pb)
{
	pb->itemcnt = decode_xml_prop_arr_int_default((int *)pb->itemid, MAX_BOX_ITEM_CNT, cur, "ID", 0);
	if (pb->itemcnt != decode_xml_prop_arr_int_default((int *)pb->itemrate, MAX_BOX_ITEM_CNT, cur, "ItemPercent", 0)) {
		ERROR_RETURN(("item pecent not match\t[%u %u]", pb->id, pb->itemcnt), -1);
	}
	int loop = 0, rate = 0;
	for (loop = 0; loop < pb->itemcnt; loop ++) {
		normal_item_t* s = get_item(pb->itemid[loop]);
		if (!s) 
			ERROR_RETURN(("item id invalid\t[%u %u]", pb->id, pb->itemid[loop]), -1);
		rate += pb->itemrate[loop];
	}

	if (rate != RAND_COMMON_RAND_BASE) {
		ERROR_RETURN(("item total percent err\t[%u %u]", pb->id, rate), -1);
	}
	DECODE_XML_PROP_UINT32(pb->cntmax, cur, "CountMax");
	DECODE_XML_PROP_UINT32(pb->cntmin, cur, "CountMin");
	if (pb->cntmin > pb->cntmax || pb->cntmax > MAX_BOX_ITEM_CNT) {
		ERROR_RETURN(("item count min max err\t[%u %u %u]", pb->id, pb->cntmax, pb->cntmin), -1);
	}
	return 0;
}

int load_box_config(xmlNodePtr cur)
{
	memset(all_boxes, 0, sizeof(all_boxes));
	uint32_t id;
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Box")) {
			DECODE_XML_PROP_UINT32(id, cur, "ID");
			box_t* pb = get_box_no_check(id);
			if (!pb) ERROR_RETURN(("err box id: %u", id), -1);
			if (pb->id) ERROR_RETURN(("duplicate box id: %u", id), -1);
			pb->id = id;
			INIT_LIST_HEAD(&(pb->active_list));
			DECODE_XML_PROP_UINT32(pb->type, cur, "Type");
			DECODE_XML_PROP_UINT32(pb->refreshtime, cur, "RefreshTime");
			pb->pbpactive = NULL;
			if (pb->id >= 100) {
				pb->refresh_type = box_refresh_type_grass;
				pb->loading_time = 5;
			}

			xmlNodePtr chl = cur->xmlChildrenNode; 
			while(chl)
			{
				if (!xmlStrcmp(chl->name, (const xmlChar *)"BeastType")) {
					load_box_beast_config(chl, pb);
				}
				if (!xmlStrcmp(chl->name, (const xmlChar *)"Item")) {
					load_box_item_config(chl, pb);
				}
				if (!xmlStrcmp(chl->name, (const xmlChar *)"Map")) {
					load_box_map_config(chl, pb);
				}
				chl = chl->next;
			}

			int loop;
			for (loop = 0; loop < BATTLE_POS_CNT; loop ++) {
				pb->box_beast.saset[loop].minlv = 1;
				pb->box_beast.saset[loop].maxlv = 0;
			}
		}
	
		cur = cur->next;
	}
	return 0;
}

int load_box_pos_config(xmlNodePtr cur)
{
	memset(box_pos, 0, sizeof(box_pos));
	uint32_t id, mapid;
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Map")) {
			DECODE_XML_PROP_UINT32(id, cur, "ID");
			box_pos_t* pbp = get_box_pos_no_check(id);
			if (!pbp)
				ERROR_RETURN(("err box pos id: %u", id), -1);
			
			if (pbp->p_m)
				ERROR_RETURN(("duplicate box pos id: %u", id), -1);
			
			DECODE_XML_PROP_UINT32(mapid, cur, "MapID");
			pbp->p_m = get_map(mapid);
			if (!pbp->p_m)
				ERROR_RETURN(("mapid invalid\t[%u %u]", id, mapid), -1);
			
			DECODE_XML_PROP_UINT32(pbp->posx, cur, "X");
			DECODE_XML_PROP_UINT32(pbp->posy, cur, "Y");
		}
	
		cur = cur->next;
	}
	return 0;
}


int load_box(xmlNodePtr cur)
{
	if (load_box_pos_config(cur)) return -1;
	if (load_box_config(cur)) return -1;

	return 0;
}

int activate_boxes()
{
	
	for (int loop = 0; loop < MAX_BOX_CNT; loop ++) {
		box_t* pb = &all_boxes[loop];
		if (!pb->id || pb->refresh_type != box_refresh_type_box)
			continue;
		
		box_pos_t* pbp = pb->pos[rand() % pb->pos_cnt];
		if (rand() % 100 < 30 && active_box_cnt < MAX_ACTIVE_BOX_CNT 
				&& !pbp->hasbox && pbp->p_m 
				)
		{
			active_box[active_box_cnt] = pb->id;
			active_box_cnt ++;
			pb->pbpactive = pbp;
			pbp->hasbox = 1;
		} else {
			if(pb->refreshtime)
				ADD_ONLINE_TIMER(&g_events, n_activate_one_box, &pb->id, pb->refreshtime);
		}
	}

	activate_grasses();
		
	return 0;
}

void noti_online_box_opened(sprite_t* p, uint32_t boxid)
{
	uint8_t out[64];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, p->id, i);
	PKG_UINT32(out, boxid, i);
	init_proto_head(out, proto_cli_box_opened, i);
	send_to_map(p, out, i, 0,1);
}

void noti_online_box_activated(box_t* pb)
{
	uint8_t out[128];
	int i = sizeof(protocol_t);
	PKG_UINT32(out, pb->id, i);
	PKG_UINT32(out, pb->type, i);
	PKG_UINT32(out, pb->pbpactive->p_m->id, i);
	PKG_UINT32(out, pb->pbpactive->posx, i);
	PKG_UINT32(out, pb->pbpactive->posy, i);
	init_proto_head(out, proto_cli_box_activated, i);
	send_to_map2(pb->pbpactive->p_m, out, i);
}

void activate_spec_box(box_t* pb)
{
	if (pb->pbpactive) return;
	box_pos_t* pbp = NULL;
	for (int loop = 0; loop < 3; loop ++) {
		pbp = pb->pos[rand() % pb->pos_cnt];
		if (!pbp->hasbox)
			break;
		pbp = NULL;
	}

	if (pb->refresh_type == box_refresh_type_box) {
		if (active_box_cnt < MAX_ACTIVE_BOX_CNT && pbp && !pbp->hasbox) {
			active_box[active_box_cnt] = pb->id;
			pb->pbpactive = pbp;
			pbp->hasbox = 1;
			active_box_cnt ++;
			noti_online_box_activated(pb);
			DEBUG_LOG("ONE BOX ACTIVATED\t[%u %lu %u %u %u]", \
				pb->id, pb->pbpactive->p_m->id, pb->pbpactive->posx, pb->pbpactive->posy, active_box_cnt);
		} else {
			list_add_tail(&pb->active_list, &active_box_list);
			DEBUG_LOG("BOX ADD TO LIST\t[%u %u %d]", pb->id, active_box_cnt, pbp ? pbp->hasbox : -1);
		}
	} else if (pb->refresh_type == box_refresh_type_grass) {
		if (pbp && !pbp->hasbox) {
			pb->pbpactive = pbp;
			pb->activate_time = get_now_tv()->tv_sec;
			pbp->hasbox = 1;
			noti_online_box_activated(pb);
			DEBUG_LOG("box id =%u,%u ",pb->id, get_now_tm()->tm_hour  );
			if (is_133_134(pb->id) && get_now_tm()->tm_hour>=13 && get_now_tm()->tm_hour<=14 ) {
				noti_global_msg_out noti_out;
 				noti_out.type=9;
				noti_out.value_list.push_back(pb->last_mapid );
				noti_out.value_list.push_back(pb->pbpactive->p_m->id);
				send_msg_to_map3(51201, noti_global_msg_cmd,&noti_out );
				send_msg_to_map3(51202, noti_global_msg_cmd,&noti_out );
				send_msg_to_map3(51203, noti_global_msg_cmd,&noti_out );
				send_msg_to_map3(51205, noti_global_msg_cmd,&noti_out );
				send_msg_to_map3(51301, noti_global_msg_cmd,&noti_out );
				
			}
			DEBUG_LOG("ONE GRASS ACTIVATED\t[%u %lu %u %u]", \
				pb->id, pb->pbpactive->p_m->id, pb->pbpactive->posx, pb->pbpactive->posy);
		}
	}
}

int activate_one_box(void* owner, void* data)
{
	uint32_t boxid = *(uint32_t *)data;
	box_t* pb = get_box(boxid);
	if (pb)
		activate_spec_box(pb);
	return 0;
}

void activate_grasses()
{
	for (int loop = 0; loop < MAX_BOX_CNT; loop ++) {
		box_t* pb = &all_boxes[loop];
		if (!pb->id || pb->refresh_type != box_refresh_type_grass)
			continue;
		
		box_pos_t* pbp = pb->pos[rand() % pb->pos_cnt];
		if (!pbp->hasbox && pbp->p_m) {
			pb->pbpactive = pbp;
			pbp->hasbox = 1;
			pb->activate_time = get_now_tv()->tv_sec;
			noti_online_box_activated(pb);
			DEBUG_LOG("ACTIVATE GRASS\t[%u %lu %u %u]", pb->id, pb->pbpactive->p_m->id, pb->pbpactive->posx, pb->pbpactive->posy);
		}
	}
}

void noti_user_box_item(sprite_t* p, box_t* pb)
{
	KDEBUG_LOG(p->id, "USER GET BOX ITEM\t[%u]", pb->id);
	item_t items[MAX_BOX_ITEM_CNT];
	memset(items, 0, sizeof(items));
	uint32_t loop, count = 0;
	uint32_t item_cnt = pb->cntmin + rand() % (pb->cntmax - pb->cntmin + 1);
	for (loop = 0; loop < item_cnt; loop ++) {
		int idx = rand_type_idx(pb->itemcnt, pb->itemrate, RAND_COMMON_RAND_BASE);
		uint32_t itemid = pb->itemid[idx];
		uint32_t l = 0;
		for (l = 0; l < count; l ++) {
			if (items[l].itemid == itemid)
				break;
		}
		if (l < count) {
			items[l].count ++;
		} else {
			items[count].itemid = itemid;
			items[count].count = 1;
			count ++;
		}
	}

	noti_cli_get_item_out cli_out;
	for (loop = 0; loop < count; loop ++) {
		items[loop].count = cache_add_kind_item(p, items[loop].itemid, items[loop].count);
		if (items[loop].count) {
			stru_item tmp_item;
			tmp_item.itemid = items[loop].itemid;
			tmp_item.count = items[loop].count;
			cli_out.itemlist.push_back(tmp_item);
		}
	}

	if (p->btr_team) {
		for (loop = 0; loop < p->btr_team->count; loop ++) {
			sprite_t* s = p->btr_team->players[loop];
			if (s) {
				db_add_item_list(0, s->id, items, count);
				KDEBUG_LOG(s->id, "SEND TO CLI GET BOX ITEM\t[%u]", count);
				send_msg_to_self(s, noti_cli_get_box_item_cmd, &cli_out, 0);
			}
		}
	} else {
		db_add_item_list(0, p->id, items, count);
		KDEBUG_LOG(p->id, "SEND TO CLI GET BOX ITEM\t[%u]", count);
		send_msg_to_self(p, noti_cli_get_box_item_cmd, &cli_out, 0);
	}
}

void remove_opened_box(box_t* pb)
{
	if (pb->refresh_type == box_refresh_type_grass) {
		pb->pbpactive->hasbox = 0;
 		pb->last_mapid=pb->pbpactive->p_m->id;
		pb->pbpactive = NULL;
		KDEBUG_LOG(0, "REMOVE BOX\t[%u]", pb->id);
		if(pb->refreshtime)
			ADD_ONLINE_TIMER(&g_events, n_activate_one_box, &pb->id, pb->refreshtime);
		return;
	}
	
	for (uint32_t loop = 0; loop < active_box_cnt; loop ++) {
		if (active_box[loop] == pb->id) {
			pb->pbpactive->hasbox = 0;
			pb->pbpactive = NULL;
			KDEBUG_LOG(0, "REMOVE BOX\t[%u]", pb->id);
			active_box_cnt --;
			active_box[loop] = active_box[active_box_cnt];
			active_box[active_box_cnt] = 0;
			if(pb->refreshtime)
				ADD_ONLINE_TIMER(&g_events, n_activate_one_box, &pb->id, pb->refreshtime);
		}
	}
}

void activate_list_box()
{
	box_t *pb = NULL;
	list_for_each_entry (pb, &active_box_list, active_list) {
		if (pb && pb->pbpactive) {
			list_del(&pb->active_list);
			return;
		} else if (pb && pb->id) {
			KDEBUG_LOG(0, "ACTIVATE LIST BOX\t[%u]", pb->id);
			list_del(&pb->active_list);
			activate_spec_box(pb);
			return;
		}
	}
}

int open_box_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t boxid;
	int j = 0;
	UNPKG_UINT32(body, boxid, j);
	box_t* pb = get_box(boxid);
	KDEBUG_LOG(p->id, "USER OPEN BOX\t[%u]", boxid);
	if (!pb) {
		return send_to_self_error(p ,p->waitcmd, cli_err_boxid_invalid, 1);
	}
	CHECK_USER_IN_MAP(p, 0);
	CHECK_SELF_BATTLE_INFO(p, boxid);
	CHECK_USER_TIRED(p);
	
	if (!pb->pbpactive) {
		return send_to_self_error(p, p->waitcmd, cli_err_box_not_active, 1);
	}

	//FOR 133
	if (is_133_134( pb->id) && (get_now_tm()->tm_hour<13 || get_now_tm()->tm_hour>14) ){
		return send_to_self_error(p, p->waitcmd, cli_err_box_not_active, 1);
	}



	if (p->tiles->id != pb->pbpactive->p_m->id \
		|| !near_pos(p, pb->pbpactive->posx, pb->pbpactive->posy)) {
		KERROR_LOG(p->id, "far from box\t[%u %u %u %u]", p->posX, p->posY, pb->pbpactive->posx, pb->pbpactive->posy);
		return send_to_self_error(p, p->waitcmd, cli_err_not_near_box, 1);
	}
	
	CHECK_SELF_TEAM_LEADER(p, boxid, 1);

	if (pb->refresh_type == box_refresh_type_grass) {
		item_t* pday = cache_get_day_limits(p, ssid_grass_open_cnt);
		if (pday->count >= (ISVIP(p->flag) ? 16 : 8)) {
			KERROR_LOG(p->id, "cnt max\t[%u]", pday->count);
			return send_to_self_error(p, p->waitcmd, cli_err_rand_day_max, 1);
		}
		pday->count ++;
		db_day_add_ssid_cnt(NULL, p->id, ssid_grass_open_cnt, 1, 16);
		if (ISVIP(p->flag))
			msg_log_grass_vip(p->id);
		else
			msg_log_grass_normal(p->id);
	}

	uint32_t happon_monster = 0;
	uint32_t beastrand = pb->refresh_type == box_refresh_type_grass ? 100 : pb->beastrand;
	KDEBUG_LOG(p->id, "BEAST RADN\t[%u]", beastrand);
	if (rand() % RAND_COMMON_RAND_BASE < beastrand && !is_133_134( pb->id)) {
		p->boxid = pb->id;
		happon_monster = 1;
	} else if (pb->refresh_type == box_refresh_type_grass && rand() % RAND_COMMON_RAND_BASE < 770 && !is_133_134( pb->id) ) {
		happon_monster = 2;
		p->box_get_rand = 1;
		/*
		if (!p->btr_team)
			p->box_get_rand = 1;
		else {
			for (uint32_t loop = 0; loop < p->btr_team->count; loop ++) {
				sprite_t* s = p->btr_team->players[loop];
				s->box_get_rand = 1;
			}
		}
		*/
	} else {
		noti_user_box_item(p, pb);
	}
	
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, boxid, i);
	PKG_UINT32(msg, happon_monster, i);
	init_proto_head(msg, p->waitcmd, i);

	if (pb->refresh_type == box_refresh_type_grass)
		send_to_self(p, msg, i, 1);
	else
		send_to_team(p, msg, i, 1);

	noti_online_box_opened(p, boxid);
	remove_opened_box(pb);
	if (pb->refresh_type == box_refresh_type_box)
		activate_list_box();
	return 0;
}

int get_box_active_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	CHECK_USER_IN_MAP(p, 0);
	int i = sizeof(protocol_t) + 4;
	uint32_t real_box_cnt = 0;
	
	for (uint32_t loop = 0; loop < MAX_BOX_CNT; loop ++) {
		box_t* pb = &all_boxes[loop];
		if (is_133_134( pb->id) && (get_now_tm()->tm_hour<13 || get_now_tm()->tm_hour>14) ){
				continue;
		}

		if (pb->id && pb->pbpactive && pb->pbpactive->p_m == p->tiles && pb->refreshtime) {
			PKG_UINT32(msg, pb->id, i);
			PKG_UINT32(msg, pb->type, i);
			PKG_UINT32(msg, pb->pbpactive->p_m->id, i);
			PKG_UINT32(msg, pb->pbpactive->posx, i);
			PKG_UINT32(msg, pb->pbpactive->posy, i);
			real_box_cnt ++;
		}
	}

	KDEBUG_LOG(p->id, "GET BOX INFO\t[%u]", real_box_cnt);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, real_box_cnt, j);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int cli_open_precious_box(sprite_t *p, Cmessage* c_in)
{
	cli_open_precious_box_in *p_in=P_IN;
	cli_open_precious_box_out cli_out;
	normal_item_t *pni=NULL;
	box_t* pb = get_box(p_in->boxid);
	uint32_t valid_mapid[]={21615 ,21620 ,21625, 21630,21635,21645,21650},len=7;
	KDEBUG_LOG(p->id, "USER OPEN PRECIOUS BOX\t[boxid=%u] times:[%u]", p_in->boxid,p->open_box_times);
	if (!pb || pb->type != PRECIOUS_BOX_TYPE){
		KERROR_LOG(p->id, "can not open precious box\t[boxid=%u]", p_in->boxid);
		return send_to_self_error(p ,p->waitcmd, cli_err_boxid_invalid, 1);
	} 

	if(p_in->itemid==0){
		if(p->open_box_times >= max_openbox_times){
			KERROR_LOG(p->id, "up to limits of opening_box times \t[boxid=%u]", p_in->boxid);
			return send_to_self_error(p ,p->waitcmd, cli_err_times_limit, 1);
		}
	}else{
		pni=get_item(p_in->itemid);
		CHECK_ITEM_VALID(p, pni, p_in->itemid);
		CHECK_ITEM_EXIST(p, p_in->itemid, 1);
		CHECK_ITEM_FUNC(p, pni,item_for_box_key);
	}
	
	map_t *p_map=get_map(p->tiles->id);
	CHECK_USER_IN_MAP(p, 0);
	if((is_in(valid_mapid, low32_val(p->tiles->id), len) < 0) || !p_map || p_map->adjacent_cnt==0){
		KERROR_LOG(p->id, "invalid mapid  \t[%u]",  low32_val(p->tiles->id));
		return send_to_self_error(p ,p->waitcmd, cli_err_mapid_invalid, 1);
	}
	CHECK_SELF_BATTLE_INFO(p, p_in->boxid);
	if(p_map->adjacent_maps[0].grpid != p->last_beastgrp){
		KERROR_LOG(p->id, "last grpid:[%u] need grpid:[%u]", p->last_beastgrp, p_map->spec_beasts[0].grpid);
		return send_to_self_error(p ,p->waitcmd, cli_err_have_not_win_boss, 1);
	}
	CHECK_USER_TIRED(p);
	if(!pni){
		p->open_box_times++;
		db_day_add_ssid_cnt(NULL, p->id, ssid_precious_box, 1, -1);
	}else{
		del_item(p, p_in->itemid, 1);
	}
	msg_log_precious_box(low32_val(p->tiles->id)-21615, p->id);
//get item list from the box
	item_t items[MAX_BOX_ITEM_CNT];
	memset(items, 0, sizeof(items));
	uint32_t count = 0;
	for (uint32_t loop = 0; loop < pb->itemcnt; loop++) {
		items[loop].itemid = pb->itemid[loop];
		items[loop].count = pb->cntmax;
		if(cache_add_kind_item(p, items[loop].itemid, items[loop].count)>0){
			stru_item tmp_item;
			tmp_item.itemid = items[loop].itemid;
			tmp_item.count = items[loop].count;
			cli_out.item_list.push_back(tmp_item);
		}
		count++;
	} 
	db_add_item_list(0, p->id, items, count);
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
} 

