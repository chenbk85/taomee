#include "viponly.h"
#include "items.h"
#include "sns.h"

vip_item_t* get_vip_item(uint32_t vid)
{
	return (vip_item_t *)g_hash_table_lookup(vip_items, &vid);
}

static gboolean free_vipitem_by_key(gpointer key, gpointer pvi, gpointer userdata)
{
	g_slice_free1(sizeof(vip_item_t), pvi);
	return TRUE;
}

void init_vip_items()
{
	vip_items = g_hash_table_new(g_int_hash, g_int_equal);
}


int load_vip_item(xmlNodePtr cur)
{
	g_hash_table_foreach_remove(vip_items, free_vipitem_by_key, 0);
	uint32_t id;
	cur = cur->xmlChildrenNode; 
	xmlNodePtr chl;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Rand")) {
			DECODE_XML_PROP_UINT32(id, cur, "ID");
			id += 100000;
			vip_item_t *pvi = get_vip_item(id);
			if (pvi)
				ERROR_RETURN(("duplicate vip item id: %u", id), -1);
			pvi = (vip_item_t *)g_slice_alloc0(sizeof(vip_item_t));
			pvi->vid = id;
			g_hash_table_insert(vip_items, &pvi->vid, pvi);
			DECODE_XML_PROP_UINT32(pvi->repeatcnt, cur, "Time");
			DECODE_XML_PROP_UINT32(pvi->repeattype, cur, "RepeatType");
			decode_xml_prop_uint32_default(&pvi->startday, cur, "StartDay", 5);
			decode_xml_prop_uint16_default(&pvi->minlv, cur, "MinLv", 1);
			decode_xml_prop_uint16_default(&pvi->maxlv, cur, "MaxLv", MAX_SPRITE_LEVEL);
			//decode_xml_prop_uint16_default(&pvi->viplv, cur, "VipLv", 1);

			chl = cur->xmlChildrenNode;
			uint32_t share_rate = 0;
			uint8_t have_cloth = 0, have_item = 0;
			while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar*)"Item")) {
					rand_item_t* pitem = &pvi->items[pvi->type_cnt];
					DECODE_XML_PROP_UINT32(pitem->item_id, chl, "ID");
					if (get_item(pitem->item_id)) {
						DECODE_XML_PROP_UINT32(pitem->item_cnt, chl, "ItemCnt");
						have_item = 1;
					} else {
						DECODE_XML_PROP_UINT32(pitem->item_cnt, chl, "EquipLv");
						cloth_t* pcloth = get_cloth(pitem->item_id);
						if (!pcloth || pitem->item_cnt >= MAX_CLOTH_LEVL_CNT || !pcloth->clothlvs[pitem->item_cnt].valid) {
							ERROR_RETURN(("item id or count invalid\t[%u %u]", pitem->item_id, pitem->item_cnt), -1);
						}
						have_cloth = 1;
					}
					DECODE_XML_PROP_UINT32(pvi->rates[pvi->type_cnt], chl, "ShareRate");
					DECODE_XML_PROP_UINT32(pvi->items[pvi->type_cnt].get_rate, chl, "GetRate");
					share_rate += pvi->rates[pvi->type_cnt];
					pvi->type_cnt ++;
				}
				chl = chl->next;
			}
			if (share_rate != RAND_COMMON_RAND_BASE) {
				ERROR_RETURN(("share rate err\t[%u %u %u]", pvi->vid, pvi->type_cnt, share_rate), -1);
			}

			if (have_cloth && have_item) 
				return -1;
			else if (have_cloth)
				pvi->rand_type = only_cloth;
			else if (have_item)
				pvi->rand_type = only_item;
			else
				return -1;
		}
	
		cur = cur->next;
	}

	return 0;
}


/**
 * @brief vip领取每周礼包
 * @return 
 */
int get_vip_item_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	CHECK_USER_VIP(p);
	uint32_t vid=p->viplv+10;
	//int j = 0;
	//UNPKG_UINT32 (body, vid, j);
	vid += 100000;
	vip_item_t* pvi = get_vip_item(vid);
	KDEBUG_LOG(p->id, "GET VIP ITEM\t[%u %u %u %u]", vid, p->collection_grid, p->card_grid, p->chemical_grid);
	if (!pvi || !pvi->repeatcnt) {
		KERROR_LOG(p->id, "rand id invalid\t[%u]", vid);
		return send_to_self_error (p, p->waitcmd, cli_err_rand_id_invalid, 1);
	}
	
	CHECK_LV_FIT(p, p, pvi->minlv, pvi->maxlv);	
	if(p->viplv < pvi->viplv){
		KERROR_LOG(p->id, "vip lv not enough\t[%u]", vid);
		return send_to_self_error (p, p->waitcmd, cli_err_level_not_fit, 1);
	}
	
	if (pvi->rand_type == only_cloth && get_bag_cloth_cnt(p) >= get_max_grid(p)) {
		return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
	}

	int idx = rand_type_idx(pvi->type_cnt, pvi->rates, RAND_COMMON_RAND_BASE);

	uint32_t itemid = pvi->items[idx].item_id;
	uint32_t count = pvi->items[idx].item_cnt;
	if (get_item(itemid) && !can_have_more_item(p, itemid, count)) {
		return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
	}
	
	*(uint32_t *)p->session = itemid;
	*(uint32_t *)(p->session + 4) = count;
	memcpy(p->session + 8, pvi, sizeof(vip_item_t));

	return db_check_rand_info(p, p->id, pvi->vid, pvi->repeattype, pvi->repeatcnt);
}

int cli_get_vip_item_cnt (sprite_t* p, Cmessage * c_in)
{
	CHECK_USER_VIP(p);
	uint32_t vid=p->viplv+100010;
	get_rand_info_range_in  db_in; 
	db_in.minid=vid;
	db_in.maxid=vid;
	return send_msg_to_db(p, p->id, get_rand_info_range_cmd, &db_in);
}

int cli_get_rand_range (sprite_t* p, Cmessage * c_in)
{
	cli_get_rand_range_in* p_in = P_IN;
	get_rand_info_range_in  db_in; 
	db_in.minid=p_in->min;
	db_in.maxid=p_in->max;
	return send_msg_to_db(p, p->id, get_rand_info_range_cmd, &db_in);
}


int get_rand_info_range(sprite_t* p, userid_t id, Cmessage *c_out, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_rand_info_range_out *p_out=P_OUT;

	cli_get_vip_item_cnt_out cli_out;	
	for (uint32_t i=0;i<p_out->item_list.size();i++ ){
		stru_rand_info *p_item =&(p_out->item_list[i]);
		if (p_item->randid >= 100000) {
			vip_item_t* pvi = get_vip_item(p_item->randid);
			if (pvi){
				vip_item_info_t tmp_item;
				tmp_item.same_period_randid=p_item->randid-100000 ;
				int tmp_count = check_same_period(p_item->time , pvi->repeattype, pvi->startday) ? p_item->count : 0;

				tmp_item.same_period_count = tmp_count;
				cli_out.item_list.push_back(tmp_item);
			}
		} else {
			rand_info_t* pri = get_rand_info(p_item->randid);
			if (pri) {
				vip_item_info_t tmp_item;
				tmp_item.same_period_randid = p_item->randid;
				int tmp_count = check_same_period(p_item->time , pri->repeattype, pri->startday) ? p_item->count : 0;

				tmp_item.same_period_count = tmp_count;
				cli_out.item_list.push_back(tmp_item);
			}
		}
	}
			

	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_get_vip_reward(sprite_t * p,Cmessage * c_in)
{
	return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	cli_get_vip_reward_out cli_out;
	if ((p->vip_day_6 / 30) > 0) {
		//2012-5-24 充值一个月vip送精灵蛋活动 第六次活动
		//增加物品
		do{
			item_t* pday = cache_get_day_limits(p, ITEM_ID_DRAGON_EGG_6);
			if(pday->count>=1){
				break;
			}
			db_add_item(NULL, p->id, VIP_TIME_ID_6,  -1 * p->vip_day_6);
			p->vip_day_6 = 0;

			cache_add_kind_item(p, ITEM_ID_DRAGON_EGG_6, 1);
			db_add_item(NULL, p->id, ITEM_ID_DRAGON_EGG_6 , 1);
			pday->count++;		
			db_day_add_ssid_cnt(NULL,p->id,ITEM_ID_DRAGON_EGG_6,1,-1);
			cli_out.item.itemid = ITEM_ID_DRAGON_EGG_6;
			cli_out.item.count=1;
		}while(0);

	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}
/**
 * @brief 
 */
int cli_get_vip_active_item(sprite_t * p,Cmessage * c_in)
{
	cli_get_vip_active_item_out cli_out;
	stru_item tmpitem;
		
	if ((p->vip_time_1 / 86400 / 90) > p->vip_dragon_cnt) {
		uint32_t count = (p->vip_time_1 / 86400 / 80) - p->vip_dragon_cnt;
		tmpitem.itemid = ITEM_ID_DRAGON_EGG;
		cache_add_kind_item(p, ITEM_ID_DRAGON_EGG, count);
		tmpitem.count = count;
		db_add_item(NULL, p->id, ITEM_ID_DRAGON_EGG, count);

		msg_log_pet_egg(59, pet_egg_damee_buy, count);
		p->vip_dragon_cnt += count;
		db_day_add_ssid_cnt(NULL, p->id, ssid_vip_dragon_active, count, -1);
		cli_out.itemlist.push_back(tmpitem);
	}

	if ((p->vip_time_2 / 86400 / 90) > p->vip_dragon_2) {
		uint32_t count = (p->vip_time_2 / 86400 / 90) - p->vip_dragon_2;
		tmpitem.itemid = ITEM_ID_DRAGON_EGG_2;
		cache_add_kind_item(p, ITEM_ID_DRAGON_EGG_2, count);
		tmpitem.count = count;
		db_add_item(NULL, p->id, ITEM_ID_DRAGON_EGG_2, count);

		p->vip_dragon_2 += count;
		db_day_add_ssid_cnt(NULL, p->id, ssid_vip_dragon_2, count, -1);

		cli_out.itemlist.push_back(tmpitem);
	}

	if ((p->vip_time_3 / 86400 / 30) > 0) {
		//充值一个月  送吉普豆精灵蛋  已下架
		uint32_t count = (p->vip_time_3 / 86400 / 30); 

		cache_add_kind_item(p, ITEM_ID_DRAGON_EGG_3, count);
		mole2_set_vip_activity_in db_in;
		db_in.val = -(count * 86400 * 30);
		send_msg_to_db(NULL, p->id, mole2_set_vip_activity_cmd, &db_in);

		db_add_item(NULL, p->id, ITEM_ID_DRAGON_EGG_3 , count);
	
		tmpitem.itemid = ITEM_ID_DRAGON_EGG_3;
		tmpitem.count=count;
		cli_out.itemlist.push_back(tmpitem);
	}
	if ((p->vip_day_5/  90) > 0) {
		uint32_t count = (p->vip_day_5/ 90); 

		//减去时间
		db_add_item(NULL, p->id, VIP_TIME_ID_5,  -count * 90);
		p->vip_day_5-=count * 90;

		//增加物品
		cloth_t* psc = get_cloth(ITEM_ID_DRAGON_EGG_5);
		cloth_lvinfo_t* pclv;
		pclv = & (psc->clothlvs[0]);
		db_add_cloth(NULL, p->id, 0, psc, 0, pclv);
			
		//cache_add_kind_item(p, ITEM_ID_DRAGON_EGG_5, count);
		//db_add_item(NULL, p->id, ITEM_ID_DRAGON_EGG_5 , count);
	
		tmpitem.itemid = ITEM_ID_DRAGON_EGG_5;
		tmpitem.count=count;
		cli_out.itemlist.push_back(tmpitem);
	}


	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_get_vip_active_2(sprite_t * p, Cmessage * c_in)
{
	if (!(p->flag & (1 << flag_vip_charged))) {
		KERROR_LOG(p->id, "not charged");
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	if (p->flag & (1 << flag_vip_item_got)) {
		KERROR_LOG(p->id, "item got");
		return send_to_self_error(p, p->waitcmd, cli_err_already_get_this, 1);
	}

	cloth_t* pc1 = get_cloth(100013);
	cloth_t* pc2 = get_cloth(88004);

	if (!pc1 || !pc2 || !pc1->clothlvs[0].valid || !pc2->clothlvs[0].valid) {
		KERROR_LOG(p->id, "cloth invalid\t[%p %p]", pc1, pc2);
		return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
	}

	cli_get_vip_active_2_out cli_out;
	if (p->flag & (1 << flag_vip_ever_bit)) {
		cli_out.drawcnt = 45;
		if (get_bag_cloth_cnt(p) + 2 > get_max_grid(p)) {
			KERROR_LOG(p->id, "cloth bag full");
			return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
		}
		db_add_cloth(NULL, p->id, 0, pc1, 0, &pc1->clothlvs[0]);
		db_add_cloth(NULL, p->id, 0, pc2, 0, &pc2->clothlvs[0]);

		stru_cloth_simple_t tmpcloth;
		tmpcloth.clothid = get_now_tv()->tv_sec;
		tmpcloth.clothtype = pc1->id;
		tmpcloth.clothlv = 0;
		cli_out.clothlist.push_back(tmpcloth);

		tmpcloth.clothid = get_now_tv()->tv_sec + 1;
		tmpcloth.clothtype = pc2->id;
		tmpcloth.clothlv = 0;
		cli_out.clothlist.push_back(tmpcloth);
	} else {
		cli_out.drawcnt = 30;
		if (get_bag_cloth_cnt(p) + 1 > get_max_grid(p)) {
			KERROR_LOG(p->id, "cloth bag full");
			return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
		}
		db_add_cloth(NULL, p->id, 0, pc2, 0, &pc2->clothlvs[0]);

		stru_cloth_simple_t tmpcloth;
		tmpcloth.clothid = get_now_tv()->tv_sec;
		tmpcloth.clothtype = pc2->id;
		tmpcloth.clothlv = 0;
		cli_out.clothlist.push_back(tmpcloth);
	}

	p->flag |= (1 << flag_vip_item_got);
	db_set_flag(NULL, p->id, flag_vip_item_got, 1);

	p->draw_cnt_gift += cli_out.drawcnt;
	mole2_set_vip_activity_in db_in;
	db_in.val += cli_out.drawcnt * 86400 * 2;
	send_msg_to_db(NULL, p->id, mole2_set_vip_activity3_cmd, &db_in);
	send_msg_to_self(p, p->waitcmd, &cli_out, 0);
	return db_get_packet_cloth_list(p);
	
}

int noti_cli_can_get_dragon(sprite_t * p,Cmessage * c_in)
{
	noti_cli_can_get_dragon_out cli_out;
	cli_out.show_vip_niti=p->vip_noti_flag_5==0?1:0;
	cli_out.count_old = (p->vip_time_1 / 86400 / 90) - p->vip_dragon_cnt;
	cli_out.count_new = (p->vip_time_2 / 86400 / 90) - p->vip_dragon_2;
	cli_out.count_3= (p->vip_time_3 / 86400 / 30);
	cli_out.count_5= (p->vip_day_5/90);
	item_t* pday = cache_get_day_limits(p, ITEM_ID_DRAGON_EGG_6);
	KDEBUG_LOG(p->id,"noti_cli_can_get_dragon :%u",pday->count);
	//cli_out.count_6= (p->vip_day_6>=30 && pday->count<1)?1:0;
	//cli_out.noti_count_6=(p->vip_noti_flag_6==0 && pday->count<1)?1:0;
	cli_out.count_6=0;
	cli_out.noti_count_6=0;
	if(p->vip_noti_flag_5==0 && p->level>=10)	{//设置已经查看过了
		p->vip_noti_flag_5=1;
		db_add_item(NULL, p->id, VIP_NOTI_FLAGID_5,  1);
	}
	if(p->vip_noti_flag_6==0 && p->level>=10)	{//设置已经查看过了
		p->vip_noti_flag_6=1;
		db_add_item(NULL, p->id, VIP_NOTI_FLAGID_6,  1);
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int cli_get_draw_cnt(sprite_t* p, Cmessage* c_in)
{
	cli_get_draw_cnt_out cli_out;
	KDEBUG_LOG(p->id,"cli_get_novip_draw_cnt[p->draw_cnt_day=%u p->draw_max_cnt=%u]", p->draw_cnt_day,p->draw_max_cnt );
	cli_out.draw_cnt = p->draw_cnt_day >= p->draw_max_cnt? 0 : p->draw_max_cnt - p->draw_cnt_day;
	uint32_t vip_draw_cnt = VIP_DRAW_CNT(p);
	uint32_t vip_draw_cnt_use = VIP_DRAW_CNT_USE(p);
	KDEBUG_LOG(p->id,"cli_get_vip_draw_cnt[vip_draw_cnt_use=%u vip_draw_cnt=%u]",vip_draw_cnt_use,vip_draw_cnt );
	cli_out.vip_draw_cnt = vip_draw_cnt_use >= vip_draw_cnt ? 0 : vip_draw_cnt - vip_draw_cnt_use;
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

/**
 * @brief 翻牌抽奖
 */
int cli_draw_once(sprite_t* p, Cmessage* c_in)
{
	cli_draw_once_in* p_in = P_IN;
	if (p->chemical_grid >= get_max_grid(p)) {
		KERROR_LOG(p->id, "bag full");
		return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
	}
	cli_draw_once_out cli_out;
	stru_item tmpitem;

/*
 * COM
伊影精灵蛋310021	0.001
"侍卫月饼精灵蛋350026"	0.01
种族技能灵药350016	0.05
合成石(中级)350028	0.04
修理模具(初级)350018	0.01
精灵经验笔记300044	0.1
生命之息III210004	0.4
冒险经验手记300002	0.389
310021,350026,350016,350028,350018,300044,210004,300002
1,10,30,20,10,50,400,479
*/
	uint32_t items[8] = {
		310021,350026,350016,350028,350018,300044,210004,300002
	};
	uint32_t percent[8] = {
		1,10,30,20,10,50,400,479
	};

/*
VIP
伊瑟拉精灵蛋310027	0.001
"月饼王精灵蛋310042"	0.01
种族技能仙草350017	0.05
合成石（高级）350029	0.04
修理模具(中级)350019	0.01
技能经验笔记300046	0.1
生命之息III210004	0.4
冒险经验手记300002	0.389
1,10,50,40,10,100,400,389,
310027,310042,350017,350029,350019,300046,210004,300002,
 */

	uint32_t vipitems[8] = { 
		350036,310042,350017,350029,350019,300046,210004,300002
	};

	uint32_t vippercent[8] = {
		1,10,30,20,10,50,400,479
	};

	uint32_t itemid, vipdrawcnt = VIP_DRAW_CNT_USE(p);
	item_t* pday = NULL;
	sysinfo_t* psys = NULL;
	int idx;
	switch (p_in->type) {
	case 1:
		if (p->draw_cnt_day >= p->draw_max_cnt) {
			KERROR_LOG(p->id, "draw cnt max\t[%u]", p->draw_cnt_day);
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}

		idx = rand_type_idx(8, percent, RAND_COMMON_RAND_BASE);
		itemid = items[idx];
		cache_add_kind_item(p, itemid, 1);
		db_add_item(NULL, p->id, itemid, 1);
		tmpitem.itemid = itemid;
		tmpitem.count = 1;
		cli_out.item.push_back(tmpitem);
		p->draw_cnt_day ++;
		p->draw_cnt_total ++;
		db_day_add_ssid_cnt(NULL, p->id, item_id_novip_draw, 1, -1);

		psys = get_sys_info(sys_info_get_item, itemid);
		if (psys)
			noti_all_get_item(p, psys);
		msg_log_draw_once(p->id, p_in->type);
		msg_log_draw_get_item(itemid);
		break;
	case 2:
		if (VIP_DRAW_CNT(p) <= vipdrawcnt) {
			KERROR_LOG(p->id, "draw cnt max\t[%u]", vipdrawcnt);
			return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
		}

		if (get_bag_cloth_cnt(p) >= get_max_grid(p)) {
			KERROR_LOG(p->id, "cloth bag full");
			return send_to_self_error(p, p->waitcmd, cli_err_bag_full, 1);
		}

		pday = cache_get_day_limits(p, ssid_moqi_ride);
		idx = rand_type_idx(8, vippercent, RAND_COMMON_RAND_BASE);

		itemid = vipitems[idx];
		
		psys = get_sys_info(sys_info_get_item, itemid);
		if (psys)
			noti_all_get_item(p, psys);

		msg_log_draw_once(p->id, p_in->type);
		msg_log_draw_get_item(itemid);
		
		if (itemid == 89003){
			cloth_t* pc = get_cloth(itemid);
			if (pc && pc->clothlvs[0].valid) {
				return db_add_cloth(p, p->id, 0, pc, 0, &pc->clothlvs[0]);
			} else {
				KERROR_LOG(p->id, "invalid cloth\t[%u]", itemid);
				return send_to_self_error(p, p->waitcmd, cli_err_client_not_proc, 1);
			}
		} else {
			db_add_item(NULL, p->id, itemid, 1);
			cache_add_kind_item(p, itemid, 1);
			if (p->vip_draw_cnt_day >= DRAW_GIFT_CNT) {
				p->vip_draw_cnt_gift_use ++;
				db_day_add_ssid_cnt(NULL, p->id, item_id_vip_draw_give, 1, -1);
			} else {
				p->vip_draw_cnt_day ++;
				p->vip_draw_cnt_total ++;
				db_day_add_ssid_cnt(NULL, p->id, item_id_vip_draw_day, 1, -1);
			}
			tmpitem.itemid = itemid;
			tmpitem.count = 1;
			cli_out.item.push_back(tmpitem);
		}
		break;
	default:
		KERROR_LOG(p->id, "invalid type\t[%u]", p_in->type);
		return -1;
	}

	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

