#include <string.h>
#include <libxml/tree.h>

#include "benchapi.h"
#include "util.h"
#include "sprite.h"
#include "map.h"

#include "npc.h"

#include "event.h"
#include "cli_proto.h"

#include "maze.h"
#include "mail.h"

enum {
	point_back = 1,
	point_next = 2,
	point_shop = 4,
};

maze_t* alloc_maze()
{
	maze_t *mz = maze_all + maze_next;

	if(maze_next == sizeof(maze_all) / sizeof(maze_all[0]))
		return NULL;

	if(mz->index == 0xFFFFFFFF)
		maze_next++;
	else
		maze_next = mz->index;

	mz->index = (mz - maze_all);

	uint32_t cookie = get_now_tv()->tv_sec << 16;
	if(!cookie) cookie = 1 + rand() % 0xFFFE;

	mz->index |= cookie;

	return mz;
}

void free_maze(maze_t *mz)
{
	int current = mz->index & 0x0000FFFF;
	maze_map_t *mz_map = &maze_config.pmaze_map[mz->layers[0].index];
	mz_map->useds = 0;//clear entry used flag
	memset(mz,0,sizeof(maze_t));
	mz->index = maze_next;
	maze_next = current;
}

maze_t* get_maze(uint32_t maze_idx)
{
	uint32_t index  = maze_idx & 0x0000FFFF;

	if(index < 0 || index >= sizeof(maze_all) / sizeof(maze_all[0]))
		return NULL;

	maze_t *mz = maze_all + index;
	if(mz->p_type == NULL || maze_idx != mz->index)
		return NULL;
	return mz;
}

maze_map_t* get_maze_map(int index)
{
	if(index < 0 || index >= maze_config.map_count)
		return NULL;
	return maze_config.pmaze_map + index;
}

int get_maze_map_index(uint32_t mapid)
{
	int index = 0;
	while(index < maze_config.map_count) {
		if(maze_config.pmaze_map[index].mapid == mapid) {
			return index;
		}
		index++;
	}
	return -1;
}

maze_tpl_t* get_maze_template(int index)
{
	if(index < 0 || index >= maze_config.tpl_count)
		return NULL;
	return (maze_config.pmaze_tpl + index);
}

int load_maze_map(maze_map_t *map,xmlNodePtr cur)
{
	DECODE_XML_PROP_INT(map->mapid, cur, "MapID");
	if(!get_map(map->mapid)) {
		ERROR_RETURN(("load_maze_map error,map=%u not exsit!", map->mapid), -1);
	}
	cur = cur->xmlChildrenNode;
	while(cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Point")) {
			if(map->count == sizeof(map->point)/sizeof(map->point[0])) {
				ERROR_RETURN(("load_maze_map map=%u error,points too much!", map->mapid), -1);
			}
			int flag = 0;
			DECODE_XML_PROP_INT(map->point[map->count].x, cur, "X");
			DECODE_XML_PROP_INT(map->point[map->count].y, cur, "Y");
			DECODE_XML_PROP_INT_DEFAULT(flag, cur, "Back",1);
			if(flag) map->point[map->count].flag |= point_back;
			DECODE_XML_PROP_INT_DEFAULT(flag, cur, "Next",1);
			if(flag) map->point[map->count].flag |= point_next;
			DECODE_XML_PROP_INT_DEFAULT(flag, cur, "Shop",0);
			if(flag) map->point[map->count].flag = point_shop;
			map->count++;
		}
		cur = cur->next;
	}
	if(map->count == 0) {
		ERROR_RETURN(("load_maze_map map=%u error,no points!", map->mapid), -1);
	}
	return 0;
}

int load_maze_layer(layer_tpl_t *layer,xmlNodePtr cur)
{
	int map_count = 0;
	int shop_count = 0;
	uint32_t mapid = 0;

	cur = cur->xmlChildrenNode; 
	while(cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Shop")) {
			if(shop_count == sizeof(layer->shop_id) / sizeof(layer->shop_id[0])) {
				ERROR_RETURN(("load_maze_layer error,too much shops!"), -1);
			}
			DECODE_XML_PROP_INT_DEFAULT(layer->shop_id[shop_count],cur, "ShopID",0);
			shop_t *shop = get_shop_no_chk(layer->shop_id[shop_count]);
			if(!shop || shop->shop_id != layer->shop_id[shop_count]) {
				ERROR_RETURN(("load_maze_layer error,shop(%d) not exsit!",layer->shop_id[shop_count]), -1);
			}
			DECODE_XML_PROP_INT_DEFAULT(layer->shop_rate[shop_count],cur, "Rate",0);
			shop_count++;
		} else if (!xmlStrcmp(cur->name, (const xmlChar *)"Map")) {
			if(map_count == sizeof(layer->map_idx) / sizeof(layer->map_idx[0])) {
				ERROR_RETURN(("load_maze_layer error,too much maps!"), -1);
			}
			DECODE_XML_PROP_UINT32(mapid, cur, "MapID");
			int index = get_maze_map_index(mapid);
			if(index < 0) {
				ERROR_RETURN(("load_maze_layer error,map(%d) not exsit!",mapid), -1);
			} else {
				layer->map_idx[map_count] = index;
			}
			DECODE_XML_PROP_UINT32(layer->map_rate[map_count], cur, "Rate");
			map_count++;
		}
		cur = cur->next;
	}

	if(map_count == 0) {
		ERROR_RETURN(("load_maze_layer error,not maps in layer!"), -1);
	}

	return 0;
}

int load_maze(maze_tpl_t *maze,xmlNodePtr cur)
{
	INIT_LIST_HEAD(&maze->mazes_head);

	DECODE_XML_PROP_UINT32(maze->limits, 	cur, "Limits");
	DECODE_XML_PROP_UINT32(maze->start_time,cur, "StartTime");
	DECODE_XML_PROP_UINT32(maze->close_time,cur, "CloseTime");
	DECODE_XML_PROP_UINT32(maze->refresh,   cur, "Refresh");
	
	cur = cur->xmlChildrenNode; 
	while(cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"MazeEntrance")) {
			if(maze->layers != 0) {
				ERROR_RETURN(("load_maze_xml layer=%u error,MazeEntrance Not First!",maze->layers), -1);
			}
			if(load_maze_layer(&maze->layers_tpl[maze->layers++],cur)) {
				ERROR_RETURN(("load_maze error layer=%u",maze->layers), -1);
			}
		} else if (!xmlStrcmp(cur->name, (const xmlChar *)"MazeLayer")) {
			if(maze->layers == sizeof(maze->layers_tpl) / sizeof(maze->layers_tpl[0])) {
				ERROR_RETURN(("load_maze_xml layer=%u error,layer too much!",maze->layers), -1);
			}
			if(load_maze_layer(&maze->layers_tpl[maze->layers++],cur)) {
				ERROR_RETURN(("load_maze error layer=%u",maze->layers), -1);
			}
		}
		cur = cur->next;
	}

	if(maze->layers < 2) {
		ERROR_RETURN(("load_maze error,no maze layers"), -1);
	}

	return 0;
}

int load_maze_xml(xmlNodePtr cur)
{
	memset(maze_all,0,sizeof(maze_all));
	memset(&maze_config, 0, sizeof(maze_config_t));

	uint32_t i = 0;
	while(i < sizeof(maze_all) / sizeof(maze_all[0])) {
		maze_all[i++].index = 0xFFFFFFFF;
	}

	cur = cur->xmlChildrenNode; 
	while(cur) {
		if(!xmlStrcmp(cur->name,(const xmlChar *)"MapConfig")) {
			if(maze_config.map_count == sizeof(maze_config.pmaze_map) / sizeof(maze_config.pmaze_map[0])) {
				ERROR_RETURN(("load_maze_xml maps=%u error,maps too much!",maze_config.map_count), -1);
			}
			if(load_maze_map(&maze_config.pmaze_map[maze_config.map_count++], cur)) {
				return -1;
			}
		} else if(!xmlStrcmp(cur->name,(const xmlChar *)"MazeTemplate")) {
			if(maze_config.map_count == 0) {
				ERROR_RETURN(("error,no maze maps"), -1);
			}
			if(maze_config.tpl_count == sizeof(maze_config.pmaze_tpl) / sizeof(maze_config.pmaze_tpl[0])) {
				ERROR_RETURN(("load_maze_xml tpl_count=%u error,tpl_count too much!",maze_config.tpl_count), -1);
			}
			if(load_maze(&maze_config.pmaze_tpl[maze_config.tpl_count++], cur)) {
				ERROR_RETURN(("load_maze_xml tpl_count=%u error",maze_config.tpl_count), -1);
			}
		}
		cur = cur->next;
	}
	if(maze_config.tpl_count == 0) {
		ERROR_RETURN(("load_maze_xml error,no maze templates"), -1);
	}

	return 0;
}

map_copy_config_t* get_map_copy_config(uint32_t copyid)
{
	return (copyid && copyid <= MAX_COPY_CONFIG_CNT) ? &map_copy_configs[copyid - 1] : NULL;
}

int load_copy_layer_beast(copy_layer_t* layer, xmlNodePtr cur)
{
	while(cur) {
		if(!xmlStrcmp(cur->name,(const xmlChar *)"Beast")) {
			uint32_t order;
			DECODE_XML_PROP_UINT32(order, cur, "Order");
			if (!order || order >= COPY_BEAST_MAX_ORDER || layer->beasts[order - 1].beast_type) {
				ERROR_LOG("order max\t[%u]", layer->layerid);
				return -1;
			}
			copy_beast_t* pbeast = &layer->beasts[order - 1];
			layer->beast_orders ++;
			pbeast->order = order;
			pbeast->beast_type = decode_xml_prop_arr_int_default((int *)pbeast->grpids, COPY_BEAST_MAX_TYPE, cur, "GrpID", 0);
			if (pbeast->beast_type != decode_xml_prop_arr_int_default((int *)pbeast->validcnts, COPY_BEAST_MAX_TYPE, cur, "ValidCnt", 0))
				return -1;
			if (pbeast->beast_type != decode_xml_prop_arr_int_default((int *)pbeast->needkillcnts, COPY_BEAST_MAX_TYPE, cur, "NeedKillCnt", 0))
				return -1;
		}
		cur = cur->next;
	}
	return 0;
}

int load_map_copy_layer(map_copy_config_t* pmcc, xmlNodePtr cur)
{
	while(cur) {
		if(!xmlStrcmp(cur->name,(const xmlChar *)"Layer")) {
			uint32_t layerid;
			DECODE_XML_PROP_UINT32(layerid, cur, "ID");
			if (pmcc->layer_cnt >= MAX_COPY_LAYER) {
				ERROR_LOG("layer max\t[%u]", pmcc->copyid);
				return -1;
			}
			copy_layer_t* layer = &pmcc->layers[pmcc->layer_cnt];
			pmcc->layer_cnt ++;
			layer->layerid = layerid;
			DECODE_XML_PROP_INT_DEFAULT(layer->parentlayer, cur, "ParentLayer", 0);
			DECODE_XML_PROP_UINT32(layer->templatemap, cur, "Template");
			int tmp[2];
			decode_xml_prop_arr_int_default(tmp, 2, cur, "NeedItem", 0);
			layer->needitem = tmp[0];
			layer->needitemcnt = tmp[1];

			if (load_copy_layer_beast(layer, cur->xmlChildrenNode) == -1)
				return -1;
		}
		cur = cur->next;
	}
	return 0;
}

int load_map_copy(xmlNodePtr cur)
{
	memset(map_copy_configs,0,sizeof(map_copy_configs));

	for (uint32_t loop = 0; loop < MAX_COPY_CONFIG_CNT; loop ++) {
		for(uint32_t i = 0; i < MAX_MAP_COPY - 1; i++)
			map_copy_configs[loop].instanceid[i] = i + 1;
		map_copy_configs[loop].instanceid[MAX_MAP_COPY - 1] = 0;
	}

	uint32_t i = 0;
	while(i < sizeof(maze_all) / sizeof(maze_all[0])) {
		maze_all[i++].index = 0xFFFFFFFF;
	}

	cur = cur->xmlChildrenNode; 
	while(cur) {
		if(!xmlStrcmp(cur->name,(const xmlChar *)"MapCopy")) {
			uint32_t copyid;
			DECODE_XML_PROP_UINT32(copyid, cur, "ID");
			map_copy_config_t* pmcc = get_map_copy_config(copyid);
			if (!pmcc || pmcc->copyid) {
				ERROR_LOG("invalid or duplicate copy id\t[%u]", copyid);
				return -1;
			}
			pmcc->copyid = copyid;
			decode_xml_prop_uint32_default(&pmcc->day_limit, cur, "Day", 0);
			decode_xml_prop_uint32_default(&pmcc->minilv, cur, "MiniLV", 0);
			if (load_map_copy_layer(pmcc, cur->xmlChildrenNode) == -1)
				return -1;
		}
		cur = cur->next;
	}

	return 0;
}


void response_player_go_out(maze_t *mz)
{
	int n;
	uint8_t out[64]={0};

	for(n = 1; n <= mz->p_type->layers; n++) {
		map_t *p_map = get_map(mz->layers[n].mapid);
		if(p_map) {
			int bytes = sizeof(protocol_t);
			map_id_t mapid = mz->layers[0].mapid;

			PKG_UINT32(out, high32_val(mapid), bytes);
			PKG_UINT32(out, low32_val(mapid),  bytes);
			init_proto_head(out, proto_cli_maze_goto_map, bytes);

			list_head_t* node;
			list_for_each(node, &p_map->sprite_list_head) {
				sprite_t *p = list_entry(node, sprite_t, map_list);
				p->portal = mapid;
				KDEBUG_LOG(p->id,"go_out maze map(%lu)==>map(%lu)", p->tiles->id, mapid);
				send_to_self(p, out, bytes, 0);
			}
		}
	}
}

void response_maze_state(maze_t *mz,int active)
{
	uint8_t out[64]={0};
	map_t *tiles = get_map(mz->layers[0].mapid);

	if(tiles) {
		maze_map_t *map = maze_config.pmaze_map + mz->layers[0].index;
		maze_point_t entry = map->point[mz->layers[0].inext];

		int i = sizeof(protocol_t);
		PKG_UINT32(out, mz->index, i);
		PKG_UINT32(out, active, i);
		PKG_UINT32(out, entry.x, i);
		PKG_UINT32(out, entry.y, i);
		PKG_UINT32(out, mz->layers[0].mapid, i);
		init_proto_head(out, proto_cli_maze_state_change, i);
		send_to_map2(tiles, out, i);

		if(active)
			tiles->maze_id = mz->index;
		else
			tiles->maze_id = 0xFFFFFFFF;
		//DEBUG_LOG("change_maze_state mazeid=%u active=%d x=%d,y=%d,mapid=%u",mz->index,active,entry.x,entry.y,mz->layers[0].mapid);
	}
}

void try_close_maze(maze_tpl_t *tpl)
{
	maze_t *mz;
	list_head_t *node;
	list_head_t *next;
	list_for_each_safe(node,next,&tpl->mazes_head) {
		mz = list_entry(node, maze_t, mzlist);
		list_del(&mz->mzlist);
		response_maze_state(mz,0);
		response_player_go_out(mz);
		free_maze(mz);
		tpl->actives--;
	}
}

int rand_maze_map(layer_tpl_t *layer,int entry)
{
	uint32_t i = 0;

	if(entry) {
		int mapcnt = 0;
		int mapidx[64];

		while(i < sizeof(layer->map_idx)/sizeof(layer->map_idx[0]) && layer->map_idx[i]) {
			if(get_maze_map(layer->map_idx[i])->useds == 0) {
				mapidx[mapcnt++] = layer->map_idx[i];
			}
			i++;
		}
		if(mapcnt == 0) return -1;
		return mapidx[rand() % mapcnt];
	} else {
		int sum = 0;
		int val = rand() % 1000;

		while(i < sizeof(layer->map_idx)/sizeof(layer->map_idx[0]) && layer->map_idx[i]) {
			sum += layer->map_rate[i];
			if(sum > val) return layer->map_idx[i];
			i++;
		}
	}

	return layer->map_idx[rand() % i];
}

int rand_maze_map_point(maze_map_t *map,int noteq,uint32_t flag)
{
	int count = 0;
	int buff[8] = {0};

	int i;
	for(i=0;i<map->count;i++) {
		if(i != noteq && (map->point[i].flag & flag)) {
			buff[count++] = i;
		}
	}

	if(count == 0) return 0;

	return buff[rand() % count];
}

int rand_layer_shop(layer_tpl_t *layer)
{
	uint32_t i = 0;
	int sum = 0;
	int val = rand() % 1000;

	while(i < sizeof(layer->shop_id)/sizeof(layer->shop_id[0]) && layer->shop_id[i]) {
		sum += layer->shop_rate[i];
		if(sum > val) return layer->shop_id[i];
		i++;
	}

	return 0;
}

void try_start_maze(maze_tpl_t *tpl)
{
	int maze_noti_len = sizeof(protocol_t);
	PKG_UINT32(msg, 0, maze_noti_len);
	PKG_UINT32(msg, sys_info_maze, maze_noti_len);
	PKG_UINT32(msg, 0, maze_noti_len);
	int count_len_pos = maze_noti_len;
	PKG_UINT32(msg, 0, maze_noti_len);
	int map_cnt = 0;
	while(tpl->actives < tpl->limits)
	{
		int i;
		int index = 0;
		maze_t *mz = NULL;
		map_id_t mapid = 0;
		maze_map_t *mz_map = NULL;

		for(i = 0; i < tpl->layers; i++) {
			if(i == 0) {
				index = rand_maze_map(tpl->layers_tpl,1);
				if(index == -1) goto end_start;
				mz = alloc_maze();
				if(!mz) goto end_start;

				mz->layers[0].index = index;
				mz_map = &maze_config.pmaze_map[index];
				mz->layers[0].mapid = mz_map->mapid;
				mz->layers[0].iback = 0;
				mz->layers[0].inext = rand_maze_map_point(mz_map,-1,1);
				mz_map->useds = 1;
			} else {
				index = rand_maze_map(tpl->layers_tpl + i,0);

				mz->layers[i].index = index;
				mz_map = &maze_config.pmaze_map[index];
				mapid = ((0xFF00 | i) << 16) | (mz_map->useds);
				mz->layers[i].mapid = (mapid << 32) | mz_map->mapid;
				mz_map->useds++;

				//DEBUG_LOG("mazeid=%u layer=%d mapid=(%lu,%lu)",mz->index,i,mz->layers[i].mapid >> 32,mz->layers[i].mapid);

				mz->layers[i].iback = rand_maze_map_point(mz_map,-1,point_back);
				mz->layers[i].inext = rand_maze_map_point(mz_map,mz->layers[i].iback,point_next);
				mz->layers[i].shopid = rand_layer_shop(tpl->layers_tpl + i);
				if(mz->layers[i].shopid) {
					mz->layers[i].ishopxy = rand_maze_map_point(mz_map,-1,point_shop);
					if((mz_map->point[mz->layers[i].ishopxy].flag & point_shop) == 0) {
						mz->layers[i].shopid = 0;
					} else {
						//DEBUG_LOG("shop=%u at mapid(%lu.%lu).point(%d,%d) in mazeid=%u layer=%d",
							//mz->layers[i].shopid,mz->layers[i].mapid >> 32,mz->layers[i].mapid,
							//mz_map->point[mz->layers[i].ishopxy].x,mz_map->point[mz->layers[i].ishopxy].y,mz->index,i);
					}
				}
			}
		}

		tpl->actives++;
		mz->p_type = tpl;
		list_add_tail(&mz->mzlist,&tpl->mazes_head);
		response_maze_state(mz,1);
		PKG_UINT32(msg, mz->layers[0].mapid, maze_noti_len);
		map_cnt ++;
	}
end_start:
	if (map_cnt) {
		PKG_UINT32(msg, map_cnt, count_len_pos);
		PKG_UINT32(msg, 0, maze_noti_len);
		init_proto_head(msg, proto_cli_sys_info, maze_noti_len);
		send_to_all(msg, maze_noti_len);
	}
	return;
}

void start_maze_timer(void)
{
	int i;
	maze_tpl_t *tpl = maze_config.pmaze_tpl;
	
	for(i=0; i < maze_config.tpl_count; i++,tpl++) {
		//seconds passed from 00:00 of today
		uint32_t seconds = get_now_sec();
		if(seconds >= tpl->start_time) {
			if(seconds < tpl->close_time - tpl->refresh / 2) {
				seconds = 2;//start time have pass,start now
			} else {
				seconds = 3600 * 24 + tpl->start_time - seconds;//start tomorrow 
			}
		} else {
			seconds = tpl->start_time - seconds;//start today
		}

		ADD_ONLINE_TIMER(&g_events, n_maze_state_change, tpl, seconds);
	}
}

int maze_state_change(void* owner, void* data)
{
	maze_tpl_t *tpl = (maze_tpl_t*)data;
	//seconds passed from 00:00 of today
	uint32_t seconds = get_now_sec();

	if(seconds >= tpl->close_time) {
		//tomorrow start delay
		seconds = 24 * 3600 + tpl->start_time - seconds;
		ADD_ONLINE_TIMER(&g_events, n_maze_state_change, tpl, seconds);
		try_close_maze(tpl);
	} else {
		try_close_maze(tpl);
		seconds = (seconds - tpl->start_time) % tpl->refresh;
		ADD_ONLINE_TIMER(&g_events, n_maze_state_change, tpl, tpl->refresh - seconds);
		try_start_maze(tpl);
	}

	return 0;
}

inline int nearly(sprite_t *p,int x,int y)
{
	if(x - 250 <= p->posX && x + 250 >= p->posX) {
		if(y - 250 <= p->posY && y + 250 >= p->posY) {
			return 1;
		}
	}
	return 0;
}

maze_t* find_maze_layer(sprite_t *p,map_id_t newmap, int *idx)
{
	int i;
	if(!p->tiles) return NULL;
	maze_t *mz = get_maze(p->tiles->maze_id);//can be NULL
	if(!mz) return NULL;

	for(i=0;i < mz->p_type->layers;i++) {
		if(mz->layers[i].mapid == newmap) {
			if(idx) *idx = i;
			return mz;
		}
	}

	return NULL;
}

int get_maze_portal_mapid(sprite_t *p, maze_t *mz, map_id_t *mapid)
{
	int layer = 0;
	for(; layer < mz->p_type->layers; layer++) {
		if(p->tiles->id == mz->layers[layer].mapid) {
			break;
		}
	}

	if(layer == mz->p_type->layers) {
		return 0;
	}

	maze_layer_t *layer_in = mz->layers + layer;
	maze_map_t *map_in = maze_config.pmaze_map + layer_in->index;
	maze_point_t back = map_in->point[layer_in->iback];
	maze_point_t next = map_in->point[layer_in->inext];

	if(nearly(p,next.x,next.y)) {
		layer++;
		if(layer >= mz->p_type->layers) layer = 0;
	} else if(nearly(p,back.x,back.y)) {
		if(layer > 0) layer--;
	} else {
		KDEBUG_LOG(p->id,"far from maze portal,back=(%d,%d),p(%d,%d),next(%d,%d)",back.x,back.y,p->posX,p->posY,next.x,next.y);
		return 0;
	}

	maze_layer_t *layer_go = mz->layers + layer;
	maze_map_t *map_go = maze_config.pmaze_map + layer_go->index;

	KDEBUG_LOG(p->id,"layer[%d<%d].mapid=(%lu.%lu) map.mapid(%u)",layer, mz->p_type->layers,
		layer_go->mapid <<32,layer_go->mapid,map_go->mapid);

	map_t *tile = get_map(map_go->mapid);//src map id
	if(!tile) {
		KERROR_LOG(p->id,"maze map(%u) not exist",map_go->mapid);
		return 0;
	}
	if(mapid) *mapid = layer_go->mapid;

	return 1;
}

int open_maze_portal_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	int j = 0;
	int maze_id = -1;
	map_id_t mapid = 0;
	CHECK_BODY_LEN(len, sizeof(int));

	if(!p->tiles) {
		KERROR_LOG(p->id,"get_maze_portal_cmd p->tiles=%p",p->tiles);
		return 0;
	}

	KDEBUG_LOG(p->id,"get_maze_portal_cmd p->tiles->id=%lu.%lu",p->tiles->id >> 32,p->tiles->id);

	maze_id = p->tiles->maze_id;
	maze_t *maze = get_maze(maze_id);//can be NULL
	if(!maze) {
		KERROR_LOG(p->id,"get_maze_portal_cmd maze(%u) have been closed",maze_id);
		return send_to_self_error(p, p->waitcmd, cli_err_portal_have_closed, 1);
	}

	if(get_maze_portal_mapid(p,maze,&mapid)) {
		KDEBUG_LOG(p->id,"get_maze_portal_cmd map=%lu.%lu",mapid >> 32,mapid);
		if(p->btr_team) {
			for(j=0;j < p->btr_team->count;j++) {
				p->btr_team->players[j]->portal = mapid;
			}
		} else {
			p->portal = mapid;
		}
		*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(high32_val(mapid));
		*(uint32_t*)(msg + sizeof(protocol_t) + 4) = htonl(low32_val(mapid));
		init_proto_head(msg, p->waitcmd, sizeof(protocol_t) + 8);
		return send_to_self(p, msg, sizeof(protocol_t) + 8, 1);
	}
	
	KDEBUG_LOG(p->id,"get_maze_portal_cmd portal for maze(%u) not useable",maze_id);
	return send_to_self_error(p, p->waitcmd, cli_err_portal_unuseable, 1);
}

int get_maze_in_map_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint8_t out[64]={0};
	CHECK_BODY_LEN(len, 0);
	int bytes = sizeof(protocol_t);

	if(p->tiles && !IS_MAZE_MAP(p->tiles->id)) {
		maze_t *mz = get_maze(p->tiles->maze_id);//can be NULL
		KDEBUG_LOG(p->id,"get_maze_in_map map(%u,%u)",high32_val(p->tiles->id),low32_val(p->tiles->id));
		if(mz) {
			maze_map_t *map = maze_config.pmaze_map + mz->layers[0].index;
			maze_point_t entry = map->point[mz->layers[0].inext];

			PKG_UINT32(out, 1, bytes);
			PKG_UINT32(out, mz->index, bytes);
			PKG_UINT32(out, entry.x, bytes);
			PKG_UINT32(out, entry.y, bytes);
			init_proto_head(out, proto_cli_get_maze_in_map, bytes);
			//KDEBUG_LOG(p->id,"get_maze_in_map mazeid=%u at (%d,%d)",mz->index,entry.x,entry.y);
			return send_to_self(p, out, bytes, 1);
		}
	}

	PKG_UINT32(out, 0, bytes);
	KDEBUG_LOG(p->id,"get_maze_in_map no maze in map(%u.%u)",high32_val(p->tiles->id),low32_val(p->tiles->id));
	init_proto_head(out, proto_cli_get_maze_in_map, bytes);
	return send_to_self(p, out, bytes, 1);
}

int get_shop_in_map_cmd(sprite_t *p, uint8_t *body, uint32_t len)
{
	uint8_t out[64]={0};
	CHECK_BODY_LEN(len, 0);
	int bytes = sizeof(protocol_t);

	if(p->tiles && IS_MAZE_MAP(p->tiles->id) && p->tiles->shop_id) {
		int index = 0;
		maze_t *mz = find_maze_layer(p,p->tiles->id,&index);//can be NULL
		if(mz) {
			maze_map_t *map = maze_config.pmaze_map + mz->layers[index].index;
			maze_point_t entry = map->point[mz->layers[index].ishopxy];

			PKG_UINT32(out, p->tiles->shop_id, bytes);
			PKG_UINT32(out, entry.x, bytes);
			PKG_UINT32(out, entry.y, bytes);
			KDEBUG_LOG(p->id,"found shop=%u at map(%u.%u).point=(%d,%d) in maze=(%u:%d)",
				p->tiles->shop_id,high32_val(p->tiles->id),low32_val(p->tiles->id),entry.x,entry.y,mz->index,index);
			init_proto_head(out, proto_cli_get_shop_in_map, bytes);
			return send_to_self(p, out, bytes, 1);
		}
	}

	PKG_UINT32(out, 0, bytes);
	PKG_UINT32(out, 0, bytes);
	PKG_UINT32(out, 0, bytes);
	KDEBUG_LOG(p->id,"shop=%u at map(%u.%u)",p->tiles->shop_id,high32_val(p->tiles->id),low32_val(p->tiles->id));
	init_proto_head(out, proto_cli_get_shop_in_map, bytes);
	return send_to_self(p, out, bytes, 1);
}


