#include <assert.h>
#include <string.h>

#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
#include <statistic_agent/msglog.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/log.h>
#include "util.h"
#include "communicator.h"
#include "dbproxy.h"
#include "logic.h"
#include "proto.h"
#include "lahm_sport.h"
#include "exclu_things.h"
#include "mole_question.h"
#include "mole_class.h"
#include "mole_car.h"
#include "gift.h"
#include "picsrv.h"
#include "small_require.h"
#include "item.h"
#include "rand_itm.h"
#include "event.h"
#include "tasks.h"
#include "mole_candy.h"
#include "mole_card.h"
#include "lamu_skill.h"
#include "profession_work.h"
#include "npc_event.h"
#include "swap.h"
#include "add_sth_rand.h"
#include "street.h"
#include "game_bonus.h"
#include "race_bonus.h"
#include "friendship_box.h"
#include "lahm_classroom.h"
#include "charity_drive.h"
#include "mole_angel.h"
#include "mole_dungeon.h"
#include "tv_question.h"
#include "angel_fight.h"
#include "fight_prize.h"
#include "mole_cutepig.h"
#include "charitable_party.h"
#include "mole_cake.h"

static item_t		all_items[ITEMS_NUM_MAX];
static item_kind_t	ikds[KINDS_NUM_MAX];
static int			ikd_count;
static int 			pos = 0;

exchange_item_t exits[EXCHANGE_TABLE_SIZE];

static char itm_deskey[] = "ib,>^W:'";

static int parse_ex_single_item(item_unit_t* iut, int* cnt, xmlNodePtr cur)
{
	int id, j = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item"))) {
			if (j == MAX_EXCHANGE_ITEMS) {
				ERROR_RETURN(("too many items"), -1);
			}

			DECODE_XML_PROP_INT (id, cur, "ID");
			if (!(iut[j].itm = get_item_prop (id))) {
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

static int parse_exchange_items(exchange_item_t *eit, xmlNodePtr chl)
{
	xmlNodePtr cur;

	while (chl) {
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemIn")){
			cur = chl->xmlChildrenNode;
			if (parse_ex_single_item(eit->in, &(eit->incnt), cur) != 0)
				return -1;
		}
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemOut")){
			cur = chl->xmlChildrenNode;
			if (parse_ex_single_item(eit->out[eit->outkind_cnt], &(eit->outcnt[eit->outkind_cnt]), cur) != 0)
				return -1;
			++(eit->outkind_cnt);
		}
		chl = chl->next;
	}

	return 0;
}

int load_exchange_items(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, j, err = -1;
	int ex_count;

	ex_count = 0;
	memset (exits, 0, sizeof (exits));

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
			DECODE_XML_PROP_INT_DEFAULT(j, cur, "PRE_ID", -1);
			if (j > 399 || j < -1) {
				ERROR_RETURN(("pre id is out of max %u", j), -1);
			}
			if (ex_count >= EXCHANGE_TABLE_SIZE - 1) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}

			exits[i].id = i;
			exits[i].pre_id = j;
			DECODE_XML_PROP_INT_DEFAULT(j, cur, "NOT_RECV", 0);
			if (j != 0 && j != 1) {
				ERROR_RETURN(("not recv is out of max %u", j), -1);
			}
			exits[i].not_recv = j;

			DECODE_XML_PROP_INT_DEFAULT(j, cur, "QUIT_TASK", 0);
			if (j != 0 && j != 1) {
				ERROR_RETURN(("quit task is out of max %u", j), -1);
			}
			exits[i].quit_task = j;

			DECODE_XML_PROP_INT_DEFAULT(j, cur, "NOT_LIMIT", 0);
			if (j != 0 && j != 1) {
				ERROR_RETURN(("not limit is out of max %u", j), -1);
			}
			exits[i].not_limit = j;

			DECODE_XML_PROP_INT_DEFAULT(j, cur, "MOLE_LEVEL", 0);
			exits[i].mole_level_limit = j;

			decode_xml_prop_arr_int_default(exits[i].flag_to_set, sizeof exits[i].flag_to_set, cur, "SetFlag", 0);
			chl = cur->xmlChildrenNode;
			if ( (parse_exchange_items(&exits[i], chl) != 0) )
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

static int parse_item(xmlNodePtr chl, item_kind_t *ik)
{
	int old, j;

	int order_max = 0;
	int order_cnt = 0;
	uint8_t order_map[32] = {0};

	old = pos;
	j = 0;
	while (chl) {
		if ((!xmlStrcmp(chl->name, (const xmlChar *)"Item"))){
			int tmp;

			DECODE_XML_PROP_INT(all_items[pos].id, chl, "ID");
			DECODE_XML_PROP_STR(all_items[pos].name, chl, "Name");
			DECODE_XML_PROP_INT_DEFAULT(all_items[pos].tradability, chl, "Tradability", 0);
			DECODE_XML_PROP_INT(all_items[pos].price, chl, "Price");
			DECODE_XML_PROP_INT_DEFAULT(all_items[pos].beans, chl, "SpottedBean",0);
			DECODE_XML_PROP_INT_DEFAULT(all_items[pos].layer, chl, "Layer", 0);
			DECODE_XML_PROP_INT_DEFAULT(all_items[pos].sell_price, chl, "SellPrice", 0);
			decode_xml_prop_uint32_default(&(all_items[pos].max), chl, "Max", ik->max);
			decode_xml_prop_float_default(&(all_items[pos].discount), chl, "Discount", 1.0);
			decode_xml_prop_uint32_default(&(all_items[pos].wishing), chl, "Wishing", 0);
			decode_xml_prop_uint32_default(&(all_items[pos].ggj), chl, "Ggj", 0);
			decode_xml_prop_uint32_default(&(all_items[pos].owner), chl, "Owner", 0);
			decode_xml_prop_uint32_default(&(all_items[pos].vip_gift_level), chl, "VipLevel", 0);
			decode_xml_prop_uint32_default(&(all_items[pos].zhongzi_youzai_level), chl, "buyLevel", 0);
			decode_xml_prop_uint32_default(&(all_items[pos].item_type), chl, "AgriculturalType", 0);
			DECODE_XML_PROP_INT_DEFAULT(all_items[pos].shellprice, chl, "Shellprice", 0);	
/*
			if(all_items[pos].beans == 0) {
				if(all_items[pos].price) {
					DEBUG_LOG(" 1 %u %s",all_items[pos].id,all_items[pos].name);
				} else {
					DEBUG_LOG(" 0 %u %s",all_items[pos].id,all_items[pos].name);
				}
			}
*/
			DECODE_XML_PROP_INT_DEFAULT(tmp, chl, "VipOnly", 0);
			if (tmp) {
				itm_set_vip_only(&(all_items[pos]));
			}
			DECODE_XML_PROP_INT_DEFAULT(tmp, chl, "VipBuyable", 0);
			if (tmp) {
				itm_set_vip_buyable(&(all_items[pos]));
			}

			if (ik->kind == ANIMAL_BABY_ITEM)
			{
			    decode_xml_prop_arr_int_default((int32_t *)all_items[pos].u.animal_tag.levelNum, 3, chl, "LevelNum", 0);
			}
			else if (ik->kind == ANGEL_ITEM_KIND)
			{
			    decode_xml_prop_arr_int_default((int32_t *)all_items[pos].u.angel_tag.abilityNum, 3, chl, "Ability", 0);
			}
			else if (ik->kind == FEED_ITEM_KIND) {
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.feed_attr.hungry, chl, "Hungry", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.feed_attr.thirsty, chl, "Thirsty", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.feed_attr.spirit, chl, "Spirit", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.feed_attr.sanitary, chl, "Sanitary", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.feed_attr.curability, chl, "Curability", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.feed_attr.rebirth, chl, "Rebirth", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.feed_attr.type, chl, "Type", 0);
			}
			else if(ik->kind == STREET_ITEM_KIND) {
				decode_xml_prop_uint32_default(&all_items[pos].u.shop_tag.type, chl, "Type", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.shop_tag.channel, chl, "Channel", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.shop_tag.condition, chl, "Condition", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.shop_tag.stoves, chl, "Stove", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.shop_tag.tables, chl, "Table", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.shop_tag.foodtables, chl, "FoodTable", 0);
				decode_xml_prop_arr_int_default((int32_t *)all_items[pos].u.shop_tag.postion, 2, chl, "Postion", 350);
			}
			else if(ik->kind == RESTAURANT_ITEM_KIND) {
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].layer, chl, "Order", 0);
				int order = all_items[pos].layer;
				if(order != 0) {
					if(order_map[order >> 3] & (1 << (order & 7)) ) {
						ERROR_RETURN(("Order %d CONFICT FOR ITEM:%u",order,all_items[pos].id),-1);
					}
					order_cnt++;
					if(order_max < order) order_max = order;
					order_map[order >> 3] |= (1 << (order & 7));
				}
				decode_xml_prop_uint32_default(&all_items[pos].u.food_tag.type, chl, "Type", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.food_tag.count, chl, "Count", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.food_tag.channel, chl, "Channel", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.food_tag.condition, chl, "Condition", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.food_tag.timer, chl, "NeedTimer", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.food_tag.bad_timer, chl, "Timeout", 0);
//				DEBUG_LOG("%u,%d,%d",all_items[pos].id,all_items[pos].u.food_tag.timer,all_items[pos].u.food_tag.bad_timer);
				decode_xml_prop_uint32_default(&all_items[pos].u.food_tag.addexp, chl, "Exp", 0);
				decode_xml_prop_food_int_default((int32_t *)all_items[pos].u.food_tag.food, 6, chl, "Food", 0);
//				DEBUG_LOG("%u,%d,%d,%d,%d,%d,%d",all_items[pos].id,all_items[pos].u.food_tag.food[0],all_items[pos].u.food_tag.food[1],all_items[pos].u.food_tag.food[2],
//					all_items[pos].u.food_tag.food[3],all_items[pos].u.food_tag.food[4],all_items[pos].u.food_tag.food[5]);
				decode_xml_prop_arr_int_default((int32_t *)all_items[pos].u.food_tag.star, 5, chl, "Star", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.food_tag.ex_exp, chl, "ExtraExp", 0);
				decode_xml_prop_uint32_default(&all_items[pos].u.food_tag.ex_cnt, chl, "ExtraCount", 0);
			}
			else if (ik->kind == FIGHT_ITEM_KIND) {
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_item_tag.type, chl, "Item_type", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_item_tag.flag, chl, "Item_flag", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_item_tag.add_val, chl, "Add_cnt", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_item_tag.round, chl, "Round", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_item_tag.swap_id, chl, "Swap_id", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_item_tag.limit_lvl, chl, "Limit_lvl", 0);
			}
			else if (ik->kind == FIGHT_CARD_KIND) {
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_card_tag.card_type, chl, "Card_type", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_card_tag.add_type, chl, "Add_type", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_card_tag.add_val, chl, "Add_val", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_card_tag.level, chl, "Level", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_card_tag.point, chl, "Point", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_card_tag.max_collect, chl, "Max_collect", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_card_tag.map_item_id, chl, "Mapitem_id", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_card_tag.exchange_val, chl, "ChangePoint", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_card_tag.donate_val, chl, "DonatePoint", 0);
			}
			else if (ik->kind == FIGHT_CLOTHES_KIND) {
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_cloth_tag.power, chl, "Str", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_cloth_tag.iq, chl, "Int", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_cloth_tag.smart, chl, "Ali", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_cloth_tag.strong, chl, "Hab", 0);
			}
			else if (ik->kind == FIGHT_COLLECT_KIND) {
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_collect_tag.wishing_cnt, chl, "Wishing_cnt", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.af_item_tag.limit_lvl, chl, "Limit_lvl", 1);
			}
			else if (ik->kind == CUTE_PIG_KIND ) {
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.breed, chl, "Type", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.sex, chl, "Sex", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.weight, chl, "Weight", 0);
				all_items[pos].u.cp_base_info_tag.weight *= 10;
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.glamour, chl, "Beauty", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.strength, chl, "Strength", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.growth, chl, "GrowSpeed", 0);
				all_items[pos].u.cp_base_info_tag.growth *= 10;
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.lifetime, chl, "Life", 0);
				all_items[pos].u.cp_base_info_tag.lifetime *= 60*60*24;
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.oppsite_id, chl, "OppID", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.mate_cost, chl, "MateCost", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.buy_level, chl, "BuyLevel", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.proc_exp, chl, "ProcExp", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.var_rat, chl, "VarRat", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.var_id, chl, "VarID", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_base_info_tag.price, chl, "PorkPrice", 0);
				all_items[pos].u.cp_base_info_tag.price *= 10;
				float val;
				decode_xml_prop_float_default(&val, chl, "WeightUp", 1);
				all_items[pos].u.cp_base_info_tag.weight_up = val * 10;
				decode_xml_prop_float_default(&val, chl, "GrowUp", 1);
				all_items[pos].u.cp_base_info_tag.grow_up = val * 10;
				decode_xml_prop_float_default(&val, chl, "PorkUp", 1);
				all_items[pos].u.cp_base_info_tag.price_up = val * 10;
				decode_xml_prop_float_default(&val, chl, "StrUp", 1);
				all_items[pos].u.cp_base_info_tag.str_up = val * 10;
				decode_xml_prop_float_default(&val, chl, "BeautyUp", 1);
				all_items[pos].u.cp_base_info_tag.glamour_up = val * 10;
			}
			else if( ik->kind == CUTE_PIG_ITEM_KIND )
			{
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_item_tag.type, chl, "Type", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.cp_item_tag.time_last, chl, "TimeLast", 0);
				all_items[pos].u.cp_item_tag.time_last *= 60 * 60;	
			}
			else if( ik->kind == OCEAN_ANIMAL_ITEM_KIND )
			{
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.ocean_animal_tag.type, chl, "Type", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.ocean_animal_tag.star, chl, "Star", 0);
				decode_xml_prop_arr_int_default((int32_t *)all_items[pos].u.ocean_animal_tag.abilityNum, 3, 
				    chl, "Ability", 0);	
				//DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.ocean_animal_tag.shellprice, chl, "Shellprice", 0);
			}
			else if( ik->kind == OCEAN_ITEM_ITEM_KIND )
			{
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.ocean_item_tag.type, chl, "Type", 0);
				DECODE_XML_PROP_INT_DEFAULT(all_items[pos].u.ocean_item_tag.value, chl, "Value", 0);		
				
			}

			if (ik->start + j != all_items[pos].id) {
				ERROR_RETURN(("error itemid=%d, start=%d", all_items[pos].id, ik->start), -1);
			}
			j++;
			pos++;
		}
		chl = chl->next;

		if (pos >= ITEMS_NUM_MAX) {
			ERROR_LOG("too many items, count=%d", pos);
			return -1;
		}
	}

	if(order_cnt != order_max) {
		for(order_cnt=1;order_cnt < order_max + 1;order_cnt++) {
			if(!(order_map[order_cnt >> 3] & (1 << (order_cnt & 7))) ) {
				ERROR_RETURN(("Order %d Miss,please Check RESTAURANT_ITEM_KIND's Order",order_cnt),-1);
			}
		}
	}

	ik->count = pos - old;
	return old;
}

int load_items (const char *file)
{
    DEBUG_LOG("load_items: size of kind %lu  item %lu", sizeof(ikds), sizeof(all_items));

    
	int j, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur, chl;

	ikd_count = 0;
	pos = 0;
	doc = xmlParseFile (file);
	if (!doc)
		ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Kind"))) {
			DECODE_XML_PROP_INT (ikds[ikd_count].id, cur, "ID");
			DECODE_XML_PROP_INT (ikds[ikd_count].start, cur, "Start");
			DECODE_XML_PROP_INT (ikds[ikd_count].end, cur, "End");

			ikds[ikd_count].kind = ikds[ikd_count].id;

			DECODE_XML_PROP_INT (ikds[ikd_count].max, cur, "Max");
			chl = cur->xmlChildrenNode;
			if ((j = parse_item (chl, &ikds[ikd_count])) == -1) return -1;
			ikds[ikd_count].items = &all_items[j];

			ikd_count++;
		}
		cur = cur->next;
	}

	if (ikd_count >= KINDS_NUM_MAX) {
		ERROR_LOG ("parse %s failed, kind Count=%d", file, ikd_count);
		goto exit;
	}
	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load item file %s", file);
}

const exchange_item_t* get_exchg_entry(int idx)
{
	return &exits[idx];
}

void unload_items ()
{
}

void new_get_item_kind_range (uint32_t type, uint32_t *start, uint32_t *end)
{
	int i;

	*start = -1;
	*end = 0;
	int kind = 1;
	for (i = 0; i < 32; i++) {
        if (ikds[i].kind == 8)
		{
		   kind = 9;
		}
		else if(ikds[i].kind == 9)
	    {
		   kind = 8;
		}
		else
		{
		    kind = ikds[i].kind;
		}

		if ((type & (1 << i)) == (1 << (kind - 1)))
		{
			if (*start > ikds[i].start)
				*start = ikds[i].start;
			if (*end < ikds[i].start)
				*end = ikds[i].end;
		}

		//DEBUG_LOG("ITEM KIND\t[%u start=%u end=%u kind=%u type=%u", i, ikds[i].start, ikds[i].end, ikds[i].kind, type);
	}
}


void get_item_kind_range (uint32_t type, uint32_t *start, uint32_t *end)
{
	int i;

	*start = -1;
	*end = 0;
	for (i = 0; i < ikd_count; i++) {
		if (type == ikds[i].kind)
		{
			if (*start > ikds[i].start)
				*start = ikds[i].start;
			if (*end < ikds[i].start)
				*end = ikds[i].end;
		}
		//DEBUG_LOG("ITEM KIND\t[%u start=%u end=%u", i, ikds[i].start, ikds[i].end);
	}
}


item_kind_t* get_item_kind(uint32_t kind_id)
{
	int i;

	for (i = 0; i < ikd_count; i++)
		if (kind_id == ikds[i].id)
			return &ikds[i];
	ERROR_RETURN (("error item kind, id=%u", kind_id), NULL);
}

item_kind_t* find_kind_of_item(uint32_t itmid)
{
	int i;

	for (i = 0; i != ikd_count; ++i) {
		if ( (ikds[i].start <= itmid) && (ikds[i].end >= itmid) ) {
			return &ikds[i];
		}
	}
	return 0;
}

item_t* get_item_prop(uint32_t itmid)
{
	item_kind_t* kind = find_kind_of_item(itmid);

	if (!kind) {
		ERROR_RETURN(("invalid item id=%u", itmid), 0);
	}

	return get_item(kind, itmid);
}

int is_wear_item(sprite_t* p, uint32_t id)
{
	int i;
	for (i = 0; i < p->item_cnt; i++) {
		if (p->items[i] == id) {
			return 1;
		}
	}
	return 0;
}

int wear_item (sprite_t *p, uint32_t itm)
{
	if (p->item_cnt >= MAX_ITEMS_WITH_BODY)
		ERROR_RETURN (("too many wear item, count=%d, id=%u", p->item_cnt, p->id), -1);
	p->items[p->item_cnt] = itm;
	p->item_cnt++;
	return 0;
}

int unwear_item (sprite_t *p, uint32_t itm)
{
	int i;
	for (i = 0; i < p->item_cnt; i++) {
		if (p->items[i] == itm) {
			p->items[i] = p->items[p->item_cnt - 1];
			p->items[p->item_cnt - 1] = 0;
			p->item_cnt--;
			return 0;
		}
	}

	ERROR_RETURN (("not wear item=%u, id=%u", itm, p->id), -1);
}

void feed_items (sprite_t *p, int count, uint32_t *itms)
{
	int i;

	for (i = 0; i < count; i++) {
		item_t *itmcls = get_item_prop (itms[i]);
		if (!itmcls) {
			ERROR_LOG ("can't find item, id=%d, uin=%u", itms[i], p->id);
			continue;
		}
		p->exp += 100;
	}
}
//--------------------------------------------------------------

//--------------------------------------------------------------
//	Callbacks
//
int item_trading_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	if ( (len == 12) || ((len == 0)&& (p->waitcmd == PROTO_GAME_SCORE))
		  || ((len == 4) && (p->waitcmd == PROTO_SELL_ITMES
				            || p->waitcmd == PROTO_SELL_FRUITS
							|| p->waitcmd == PROTO_FIRE_CUP_ADD_GAME_BONUS))) {
		switch (p->waitcmd) {
		case PROTO_SELL_ITMES:
		case PROTO_SELL_FRUITS:
			p->yxb = *(uint32_t*)buf;
			response_proto_uint32(p, p->waitcmd, p->yxb, 0);
			break;
		case PROTO_ITEM_BUY:
		{
			uint32_t itmid = *(uint32_t*)(buf + 4);
			p->yxb = *(uint32_t*)buf;
			if (itmid == 160193 || itmid == 160194 || itmid == 160195
				|| itmid == 160196 || itmid == 160197 || itmid == 160198
				|| itmid == 12365 || itmid == 12366){
				uint32_t buff[2];
				buff[0] = *(uint32_t*)(buf + 8); //count
				buff[1] = p->id;
				msglog(statistic_logfile, 0x02080000 + itmid, get_now_tv()->tv_sec, buff, sizeof(buff));
			}

			response_proto_uint32(p, p->waitcmd, p->yxb, 0);
			break;
		}
		case PROTO_FETCH_REALIZED_WISH:
			p->yxb = *(uint32_t*)buf;
			response_proto_head(p, p->waitcmd, 0);
			break;
		case PROTO_COMPOUND_ITEM:
			p->yxb = *(uint32_t*)buf;
			send_to_self(p, p->session, p->sess_len, 1);
			p->sess_len = 0;
			break;
        case PROTO_GET_VIP_LEVEL_GIFTS:
        {
            response_proto_head(p, p->waitcmd, 0);
            break;
        }
		case PROTO_COLLECT_RAIN_GIFT:
        {
			gift_disappear(p);
    		break;
        }
		case PROTO_GET_CHRIS_GIFT_FROM_BILU:
		{
			response_chris_gift(p);
			break;
		}
		case PROTO_LAHM_FOOD_MACHINE:
		{
			resp_lahm_food_mechine(p);
			break;
		}
		case PROTO_GET_BUILDING_CERTIFY:
			p->yxb = *(uint32_t*)buf;
			db_get_building_certificates(p);
			break;
		case PROTO_BUY_BUSINESSMAN_GOODS:
			return buy_businessman_add_bag_callback(p);
		case PROTO_FIRE_CUP_ADD_GAME_BONUS:
			return cmd_callback(p, p->id, NULL, 0);
		default:
			ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
		}
		return 0;
	}
	ERROR_RETURN( ("Invalid Len=%d Waitcmd=%d", len, p->waitcmd), -1 );
}


int do_ask_for_items(sprite_t* p, uint32_t incnt, const uint8_t* inbuf, uint32_t outcnt, const uint8_t* outbuf)
{
	item_t*  itm;
	uint32_t itmid, itmcnt;
	int      idx_in = 0, idx_out, i;
	PKG_H_UINT32(msg, incnt, idx_in);   // in count
	PKG_H_UINT32(msg, outcnt, idx_in);  // out count
	PKG_H_UINT32(msg, ATTR_CHG_ask_for, idx_in); // reason
	PKG_H_UINT32(msg, 0, idx_in);    // detailed reason

	idx_out = 0;
	for (i = 0; i != incnt; ++i) {
		// unpack
		UNPKG_UINT32(inbuf, itmid, idx_out);
		UNPKG_UINT32(inbuf, itmcnt, idx_out);
		if ( !(itm = get_item_prop(itmid)) || (itmid > 1200000 && itmid < 1219999) ) {
			ERROR_RETURN( ("Invalid Req Uid=%u ItemID=%u Cnt=%u", p->id, itmid, itmcnt), -1 );
		}
		// sub attr if needed
		if ( (itmid < 5) && (sub_sprite_attr(p, itmid, itmcnt) 	== -1) ) {
			return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
		}
		// Package For DB
		if (pkg_item_kind(p, msg, itmid, &idx_in) == -1)
			return -1;
		PKG_H_UINT32(msg, itmid, idx_in);
		PKG_H_UINT32(msg, itmcnt, idx_in);
		DEBUG_LOG("ASK FOR ITEMS IN\t[uid=%u itmid=%u itmcnt=%u max=%u]", p->id, itmid, itmcnt, itm->max);
	}

	idx_out = 0;
	memset(p->session + 9, 0, 20);
	for (i = 0; i != outcnt; ++i) {
		// unpack
		UNPKG_UINT32(outbuf, itmid, idx_out);
		UNPKG_UINT32(outbuf, itmcnt, idx_out);
		if ((itmid < 5) && (itmid > 0)) {
			ERROR_RETURN( ("Cannot Ask For Item %u: uid=%u", itmid, p->id), -1 );
		}
		item_kind_t* kind = find_kind_of_item(itmid);

		if ( ((itmid < 1) && (itmcnt > 1000))
			|| (!kind || (kind->kind == PET_ITEM_CLOTH_KIND) || (kind->kind == PET_ITEM_HONOR_KIND))
			|| !(itm = get_item_prop(itmid)) || (!itm_buyable(p, itm) && (p->waitcmd != PROTO_COMPOUND_ITEM)) ) {
			ERROR_RETURN( ("Invalid Req Uid=%u ItemID=%u Cnt=%u", p->id, itmid, itmcnt), -1 );
		}

		if (itm->price != 0) {
			if (!kind || (kind->kind != THROW_ITEM_KIND && kind->kind != CHG_SHOW_ITEM_KIND
						  && kind->kind != FEED_ITEM_KIND && kind->kind != CHG_COLOR_ITEM_KIND)) {
				DEBUG_LOG("CAN NOT ASK\t[item=%u, uin=%u]", itmid, p->id);
				return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
			}
		}
		// pack attr for adding on later
		pack_exchg_attr(p->session + 8, itmid, itmcnt);
		// Package For DB
		if (pkg_item_kind(p, msg, itmid, &idx_in) == -1)
			return -1;
		PKG_H_UINT32(msg, itmid, idx_in);
		PKG_H_UINT32(msg, itmcnt, idx_in);
		PKG_H_UINT32(msg, itm->max, idx_in);
		DEBUG_LOG("ASK FOR ITEMS OUT\t[uid=%u itmid=%u itmcnt=%u max=%u]", p->id, itmid, itmcnt, itm->max);
	}

	p->sess_len = sizeof(protocol_t);
	PKG_UINT32(p->session + 29, outcnt, p->sess_len);
	PKG_STR(p->session + 29, outbuf, p->sess_len, outcnt * 8);
	init_proto_head(p->session + 29, p->waitcmd, p->sess_len);

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, idx_in, msg, p->id);
}

/*---------------------------------------------------
  *  Func:  ask_for_items_cmd
  *  Abstr: client asks for some items
  *  Warning: !!Serious Bug For Game Cheater!!
  *---------------------------------------------------*/
int ask_for_items_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 4);

	int idx_out = 0;
	uint16_t incnt, outcnt;

	UNPKG_UINT16(body, incnt, idx_out);
	UNPKG_UINT16(body, outcnt, idx_out);

	if ( incnt || outcnt ) {
		int i = (incnt + outcnt) * 8 + 4;
		CHECK_BODY_LEN(bodylen, i);

		if (outcnt) {
			int avail_len = SPRITE_SESS_AVAIL_LEN - sizeof(protocol_t) - 29;
			i = outcnt * 8 + 4;
			CHECK_BODY_LEN_LE(i, avail_len);
		}

		DEBUG_LOG("ASK FOR ITEMS\t[uid=%u incnt=%d outcnt=%d]", p->id, incnt, outcnt);
		return do_ask_for_items(p, incnt, body + 4, outcnt, body + (incnt * 8) + 4);
	}

	ERROR_RETURN( ("Wrong InCnt=%d OutCnt=%d From Uid=%u", incnt, outcnt, p->id), -1 );
}

/*---------------------------------------------------
  *  func:  chk_item_amount_cmd
  *  abstr: check amount of a given user's given item
  *---------------------------------------------------*/
int chk_item_amount_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 13);

	uint32_t uid, itmid_start, itmid_end;
	uint8_t  flag;

	int i = 0;
	UNPKG_UINT32(body, uid, i);
	UNPKG_UINT32(body, itmid_start, i);
	UNPKG_UINT32(body, itmid_end, i);
	UNPKG_UINT8(body, flag, i);
	DEBUG_LOG("start end %u %u", itmid_start, itmid_end);
	CHECK_VALID_ID(uid);

	if (flag > 2) {
		ERROR_RETURN( ("Wrong Flag=%d From Uid=%u", flag, p->id), -1 );
	}

	item_kind_t* itmk_start = find_kind_of_item(itmid_start);
	item_kind_t* itmk_end = find_kind_of_item(itmid_end);
	if (!itmk_start || !itmk_end || itmid_start >= itmid_end
		|| (itmk_start->kind == HOME_ITEM_KIND && itmk_end->kind == HOME_ITEM_KIND && itmid_end > itmid_start + 1)
		|| (itmk_start->kind == HOME_ITEM_KIND && itmk_end->kind != HOME_ITEM_KIND)
		|| (itmk_start->kind != HOME_ITEM_KIND && itmk_end->kind == HOME_ITEM_KIND)) {
		ERROR_RETURN( ("Invalid ItmID_start=%u ItmID_end=%u Chkee=%u Chker=%u", itmid_start, itmid_end, uid, p->id), -1);
	}
	if ((itmk_start->kind == HOMELAND_ITEM_KIND && itmk_end->kind == HOMELAND_ITEM_KIND && itmid_end > itmid_start + 1)
		|| (itmk_start->kind == HOMELAND_ITEM_KIND && itmk_end->kind != HOMELAND_ITEM_KIND)
		|| (itmk_start->kind != HOMELAND_ITEM_KIND && itmk_end->kind == HOMELAND_ITEM_KIND)) {
		ERROR_RETURN( ("Invalid ItmID_start=%u ItmID_end=%u Chkee=%u Chker=%u", itmid_start, itmid_end, uid, p->id), -1);
	}
	uint32_t kind_type = 0;
	if (itmk_start->kind == HOME_ITEM_KIND) {
		kind_type = 1;
	} else if (itmk_start->kind == HOMELAND_ITEM_KIND) {
		kind_type = 2;
	} else if (itmk_start->kind == HAPPY_CARD_KIND) {
		kind_type = 6;
	} else if (itmk_start->kind == CLASS_ITEM_KIND) {
		kind_type = 7;
	} else if (itmk_start->kind == CAR_KIND) {
		kind_type = 8;
	} else if (itmk_start->kind == TEMP_ITEM_KIND) {
		kind_type = 99;
	} else if (itmk_start->kind == FIGHT_ITEM_KIND
			|| itmk_start->kind == FIGHT_CARD_KIND
			|| itmk_start->kind == FIGHT_COLLECT_KIND) {
		kind_type = 32;
	} else if ( itmk_start->kind == CUTE_PIG_KIND
		||  itmk_start->kind == CUTE_PIG_ITEM_KIND
		||  itmk_start->kind == CUTE_PIG_CLOTHES_KIND )
	{
		kind_type = 36;
	}
	i = 0;
	PKG_H_UINT32(msg, kind_type, i);
	PKG_H_UINT32(msg, itmid_start, i);
	PKG_H_UINT32(msg, itmid_end, i);
	PKG_UINT8(msg, flag, i);

	DEBUG_LOG("CHK ITEM AMT\t[checker=%u checkee=%u itmid=%u %u kind_type=%d, flag=%d]", p->id, uid, itmid_start, itmid_end, kind_type, flag);
	//return send_request_to_db(SVR_PROTO_GET_ITEM, p, i, msg, uid);
	return send_request_to_db(SVR_PROTO_CHK_ITEM, p, i, msg, uid);
}
//--------------------------------------------------------------

//--------------- RandItem Begins -----------------
static inline void
generate_ritem(rand_item_t* ritm)
{
	int pos = (rand() % ritm->maxpoints) + ritm->startpoint;
	while ( ritm->itmpos & (1 << pos) ) {
		pos = (rand() % ritm->maxpoints) + ritm->startpoint;
	}
	ritm->itmpos |= (1 << pos);
	++(ritm->ngenerated);
	++(ritm->cnt);
}

static inline void
generate_ritems(rand_item_t* ritm, int max)
{
	int i = 0;
	for (; (i != max) && (ritm->cnt != ritm->max); ++i) {
		generate_ritem(ritm);
	}
}

static inline int
ritem_party_time(const rand_item_t* ritm)
{
	return (tm_cur.tm_hour > (ritm->start_time - 1)) && (tm_cur.tm_hour < ritm->end_time);
}

//
int do_gen_ritm_on_demand(map_t* m, uint32_t itmid, int force_gen)
{
	list_head_t* cur;
	rand_item_t* ritm;
	list_for_each(cur, &(m->ritem_list)) {
		ritm = list_entry(cur, rand_item_t, entry);
		// if the given ritem of itmid is found
		if (ritm->id == itmid) {
			// if the ritm is generable on user demand or forced to be generated by some other interface of Online
			if (RITM_IS_GEN_ON_DEMAND(ritm) || force_gen) {
				int nitms = RITM_IS_FULLY_GEN(ritm) ? ritm->max : ((rand() % ritm->max) + 1);

				generate_ritems(ritm, nitms);

				if (ritm->disappear_time) {
					time_t disappear_time = get_now_tv()->tv_sec + ritm->disappear_time;
					if ( !(ritm->disappear_tmr) ) {
						ritm->disappear_tmr = ADD_TIMER_EVENT(ritm, notify_rand_item_info, (void*)4, disappear_time);
					} else {
						mod_expire_time(ritm->disappear_tmr, disappear_time);
					}
				}
				break;
			} else {
				ERROR_RETURN(("invalid ritmid=%u mid=%lu", ritm->id, m->id), -1);
			}
		}
	}
	return 0;
}

/**
 * do_notify_ritm_info - notify client the layout of random items.
 *
 * @p: to whom the pkg is sent
 * @tomap: 0 to a given sprite @p, 1 to all sprites of map @p. if 0, p->waitcmd is set to 0
 * @ritmlist: random item list head
 * @cmd: command id
 *
 */
void do_notify_ritm_info(void* p, int tomap, const list_head_t* ritmlist, int cmd)
{
	int cnt = 0;
	int len = sizeof(protocol_t) + 2;
	list_head_t* cur;
	rand_item_t* ritm;
	list_for_each(cur, ritmlist) {
		ritm = list_entry(cur, rand_item_t, entry);
		if (RITM_IS_APPEAR_ANY_TIME(ritm) || ritem_party_time(ritm)) {
			PKG_UINT32(msg, ritm->id, len);
			PKG_UINT32(msg, ritm->itmpos, len);
			++cnt;
		}
	}
	int i = sizeof(protocol_t);
	PKG_UINT16(msg, cnt, i);
	init_proto_head(msg, cmd, len);

	if (tomap) {
		send_to_map2(p, msg, len);
	} else {
		send_to_self(p, msg, len, 1);
	}
}

int load_ritem_config(xmlNodePtr cur_node, map_t* m)
{
	rand_item_t* ritm;

	cur_node = cur_node->xmlChildrenNode;

	int tmp;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar *)"Item")) {
			ritm = g_slice_alloc0(sizeof *ritm);

			INIT_LIST_HEAD(&ritm->timer_list);

			DECODE_XML_PROP_INT(ritm->id, cur_node, "ID");
			DECODE_XML_PROP_INT(ritm->maxpoints, cur_node, "MaxPoints");
			DECODE_XML_PROP_INT(ritm->startpoint, cur_node, "StartPoint");
			DECODE_XML_PROP_INT(ritm->max, cur_node, "Max");
			if (ritm->max > ritm->maxpoints) {
				ERROR_RETURN(("Max must less than MaxPoints"), -1);
			}
			DECODE_XML_PROP_INT_DEFAULT(ritm->item_num_per_point, cur_node, "ItmNumPerPoint", 1);
			DECODE_XML_PROP_INT_DEFAULT(ritm->reborntime, cur_node, "RebornTime", 0);
			DECODE_XML_PROP_INT_DEFAULT(ritm->start_time, cur_node, "StartTime", 0);
			DECODE_XML_PROP_INT_DEFAULT(ritm->end_time, cur_node, "EndTime", 0);
			DECODE_XML_PROP_INT(ritm->multiplicator, cur_node, "Multiplicator");
			DECODE_XML_PROP_INT_DEFAULT(ritm->disappear_time, cur_node, "DisappearTime", 0);

			DECODE_XML_PROP_INT_DEFAULT(tmp, cur_node, "AppearAnyTime", 0);
			if (tmp) {
				RITM_SET_APPEAR_ANY_TIME(ritm);
			}
			DECODE_XML_PROP_INT_DEFAULT(tmp, cur_node, "GenOnDemand", 0);
			if (tmp) {
				RITM_SET_GEN_ON_DEMAND(ritm);
			}
			DECODE_XML_PROP_INT_DEFAULT(tmp, cur_node, "FullyGen", 0);
			if (tmp) {
				RITM_SET_FULLY_GEN(ritm);
			}

			ritm->map = m;
			list_add_tail(&(ritm->entry), &(m->ritem_list));

			if (!RITM_IS_GEN_ON_DEMAND(ritm)) {
				generate_ritems(ritm, ritm->max);
			}

			struct tm tm_tmp = *get_now_tm();
			tm_tmp.tm_hour = (RITM_IS_APPEAR_ANY_TIME(ritm) ? 0 : ritm->start_time);
			tm_tmp.tm_min  = 0;
			tm_tmp.tm_sec  = 0;
			time_t exptm   = mktime(&tm_tmp);
			if (exptm < now.tv_sec) { // If Event Has Begun
				exptm += 86400;
			}

			ADD_TIMER_EVENT(ritm, notify_rand_item_info, (void*)3, exptm);
		}
		cur_node = cur_node->next;
	}

	return 0;
}

void destroy_ritems(list_head_t* head)
{
	list_head_t* cur;
	list_head_t* next;
	rand_item_t* ritm;

	list_for_each_safe(cur, next, head) {
		ritm = list_entry(cur, rand_item_t, entry);
		REMOVE_TIMERS(ritm);
		list_del(&(ritm->entry));
		g_slice_free1(sizeof *ritm, ritm);
	}
}

int do_collect_item(sprite_t* p, uint32_t pos, uint32_t itemid)
{
	int found = 0;
	list_head_t* cur;
	rand_item_t* ritm = 0;

	list_for_each(cur, &(p->tiles->ritem_list)) {
		ritm = list_entry(cur, rand_item_t, entry);
		if ( (pos >= ritm->startpoint) && (pos < (ritm->startpoint + ritm->maxpoints)) ) {
			if ( (RITM_IS_APPEAR_ANY_TIME(ritm) || ritem_party_time(ritm))
					&& (ritm->itmpos & (1 << pos)) ) {
				// If RandItem Found
				ritm->itmpos &= ~(1 << pos);
				++(ritm->ncollected);
				--(ritm->cnt);
				found = 1;
				notify_rand_item_info(p->tiles, (void*)1);
				if (ritm->reborntime) {
					time_t exptime = now.tv_sec + ritm->reborntime;
					ADD_TIMER_EVENT(ritm, notify_rand_item_info, (void*)2, exptime);
				}
			}
			break;
		}
	}

	if (found) {
		if (ritm->id > 4) {
			if (ritm->id == 38) {
				int i = sizeof(protocol_t);
				PKG_UINT32(msg, 38, i);
				PKG_UINT32(msg, 1, i);
				init_proto_head(msg, p->waitcmd, i);
				return send_to_self(p, msg, i, 1);
			}
			return db_single_item_op(p, p->id, ritm->id, 1, 1);
		} else {
			p->sess_len = sizeof(protocol_t);
			PKG_UINT32(p->session, ritm->id, p->sess_len);
			PKG_UINT32(p->session, ritm->item_num_per_point, p->sess_len);
			init_proto_head(p->session, p->waitcmd, p->sess_len);
			return db_attr_op(p, ritm->item_num_per_point, 0, 0, 0, 0, ATTR_CHG_collect_ritm, 0);
		}
	} else {
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, (ritm ? ritm->id : 0), i);
		PKG_UINT32(msg, 0, i);
		init_proto_head(msg, PROTO_COLLECT_ITEM, i);
		return send_to_self(p, msg, i, 1);
	}
}

int notify_rand_item_info(void* owner, void* data)
{
	map_t* m = NULL;
	rand_item_t* ritm;

	if (!data) {
		m = ((sprite_t*)owner)->tiles;
	} else if (data == (void*)1) {
		m = owner;
	} else if (data == (void*)2) {
		ritm = owner;
		m    = ritm->map;

		if (ritem_party_time(ritm)) {
			if (!(ritm->multiplied)) {
				generate_ritems(ritm, ritm->max * (ritm->multiplicator - 1));
				ritm->multiplied = 1;
			}
		} else {
			ritm->multiplied = 0;
		}

		if (ritm->cnt < (ritm->max * (ritm->multiplied ? ritm->multiplicator : 1))) {
			generate_ritem(ritm);
		}
	} else if (data == (void*)3) {
		ritm = owner;
		m    = ritm->map;

		time_t exptm = now.tv_sec + 86400;
		ADD_TIMER_EVENT(ritm, notify_rand_item_info, (void*)3, exptm);
		// Output RandItem Info of the Last Day
		INFO_LOG("RandItem %u Daily Info: %u/%u mapid=%lu", ritm->id, ritm->ncollected, ritm->ngenerated, ritm->map->id);
		ritm->ngenerated = ritm->ncollected = 0;
	} else if (data == (void*)4) { // rand items disappear
		ritm = owner;
		m    = ritm->map;

		memset(&(ritm->itmpos), 0, sizeof ritm->itmpos);
		ritm->disappear_tmr = 0;
		ritm->cnt           = 0;
	}

	if (!data) {
		do_notify_ritm_info(owner, 0, &(m->ritem_list), PROTO_RAND_ITEM_INFO);
	} else {
		do_notify_ritm_info(m, 1, &(m->ritem_list), PROTO_RAND_ITEM_INFO);
	}
	return 0;
}

int ritm_gen_on_demand_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t itmid;
	if (unpkg_uint32(body, bodylen, &itmid) == -1) {
		return -1;
	}

	map_t* m = p->tiles;
	if (do_gen_ritm_on_demand(m, itmid, 0) == 0) {
		do_notify_ritm_info(m, 1, &(m->ritem_list), p->waitcmd);

		DEBUG_LOG("RITM GEN ON DEMAND\t[uid=%u ritm=%u]", p->id, itmid);
		return (p->waitcmd = 0);
	}

	return -1;
}
//--------------- RandItem Ends -----------------


//--------------- Buy Items Begins -----------------
//
int buy_item_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t id;
	int count;
	if (unpkg_uint32_uint32(body, len, &id, (uint32_t*)&count) == -1) return -1;

	DEBUG_LOG("BUY ITEM\t[uid=%u itm=%u cnt=%u]", p->id, id, count);
	return do_buy_item(p, id, count, 0, 1);
}
//---------------- Buy Items Ends ------------------

//--------------- Sell Items Begins -----------------
static inline int
sellable_item(uint32_t itmkind)
{
	return ( ((itmkind != PET_ITEM_KIND) && (itmkind < SPECIAL_ITEM_KIND)) || (itmkind == HOMELAND_ITEM_KIND) );
}

int do_sell_item(sprite_t* p, uint32_t itemid, uint32_t nitems, uint32_t price)
{
	item_t* itm = get_item_prop(itemid);
	if (!itm || !itm_sellable(itm) || (itm->price < price/nitems) ) {
		ERROR_RETURN( ("Item %u not found or Unsellable or Wrong Price: uid=%u itmflag=0x%X price=%d %u",
						itemid, p->id, (itm ? itm->tradability : 0), (itm ? itm->price : 0), price/nitems), -1 );
	}

	item_kind_t* kind = find_kind_of_item(itemid);
	if ( !kind || !sellable_item(kind->kind) ) {
		ERROR_RETURN(("Wrong Item Kind: itmid=%u uid=%u", itemid, p->id), -1);
	}
	int flag = 0;
	if (kind->kind == HOME_ITEM_KIND){
		flag = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND){
		flag = 2;
	}
	char buf[20];
	int i = 0;
	PKG_H_UINT32(buf, flag, i);
	PKG_H_UINT32(buf, itemid, i);
	PKG_H_UINT32(buf, nitems, i);
	//PKG_H_UINT32(buf, itm->max, i);
	PKG_H_UINT32(buf, 99999, i);
	PKG_H_UINT32(buf, price, i);
	DEBUG_LOG("SELL ITEM\t[uid=%u itm=%u cnt=%u price=%u]", p->id, itemid, nitems, price);

	return send_request_to_db(SVR_PROTO_SELL_ITEM, p, 20, buf, p->id);
}

/*---------------------------------------------------
  *  func:  bargaining_cmd
  *  abstr: bargaining item price
  *---------------------------------------------------*/
int bargaining_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t itmid = -1;
	unpkg_uint32(body, bodylen, &itmid);

	item_t* itm = get_item_prop(itmid);
	if (itm) {
		uint32_t price;
		int rate = rand() % 100;
		int rate2 = rand() % 100;
		if (rate < 10) {
			price = itm->price * 0.1;
		} else if (rate < 30) {
			price = itm->price * 0.2;
		} else if (rate < 69) {
			price = itm->price * 0.3;
		} else if (rate < 89) {
			price = itm->price * 0.4;
		} else if (rate < 99) {
			price = itm->price * 0.5;
		} else {
			if (rate2 > 90)
				price = itm->price;
		    else
				price = itm->price * 0.5;
		}

		int i = sizeof(protocol_t);
		PKG_UINT32(msg, price, i);
		*((uint32_t*)(msg + i + 8))  = itmid - 157;
		*((uint32_t*)(msg + i + 12)) = price + 291;
		des_encrypt(itm_deskey, (void*)(msg + i + 8), (void*)(msg + i));
		i += 8;
		init_proto_head(msg, p->waitcmd, i);

		DEBUG_LOG("BARGAINING\t[uid=%u itm=%u price=%u]", p->id, itmid, price);
		return send_to_self(p, msg, i, 1);
	}
	ERROR_RETURN(("wrong itmid=%u", itmid), -1);
}

/*---------------------------------------------------
  *  func:  sell_items_cmd
  *  abstr: users sell their items
  *---------------------------------------------------*/
int sell_items_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 20);

	uint32_t sess[2];
	uint32_t itmid, price, itmcnt;
	int i = 0;
	UNPKG_UINT32(body, itmid, i);
	UNPKG_UINT32(body, price, i);
	UNPKG_UINT32(body, itmcnt, i);
	des_decrypt(itm_deskey, (void*)(body + i), (void*)sess);
	sess[0] += 157; // itmid was subtracted by 157 before DES, so we must plus 157 here
	sess[1] -= 291; // price was increased by 291 before DES, so we must subtract 291 here
	if ( (itmcnt == 1) && (itmid == sess[0]) && (price == sess[1]) ) {
		return do_sell_item(p, itmid, itmcnt, price * itmcnt);
	}
	ERROR_RETURN( ("Invalid Req: uid=%u itmid=%u %u price=%u %u itmcnt=%u",
					p->id, itmid, sess[0], price, sess[1], itmcnt), -1 );
}
//--------------- Sell Items Ends -----------------

//--------------- Compose Item Begins -----------------
//
int compound_item_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	static uint32_t stuffs[][4] //item id must in acsending order
			= { /*{ 3, 190026, 190028, 190029 },
				{ 2, 190022, 190024, 190025 },
				{ 1, 190022, 190023, 190028 },
				{ 3, 190022, 190119, 190120 },
				{ 2, 190024, 190119, 190141 }, // ping guo pai
				{ 2, 190024, 190119, 190142 }, // cao mei pai
				{ 2, 190024, 190029, 190187 }, // xiao man tou
				{ 3, 190027, 190120, 190196 }, // Hei seng lin Guai Wei Quan
				{ 1, 190025, 190029, 190201 }, // Xi Gua Jie Shu ChongYin
				{ 1, 190167, 190350, 190352 },
				{ 1, 190187, 190351, 190390 },
				{ 3, 190119, 190167, 190380 },
				{ 2, 190187, 190202, 190419 },
				{ 1, 190022, 190142, 190388 }*/
				{ 2, 190022, 190187, 190187 },
				{ 1, 190388, 190388, 190388 },
				{ 1, 190380, 190380, 190380 },
				{ 1, 190141, 190141, 190141 },
				{ 1, 190028, 190028, 190028 }, // ping guo pai
				{ 2, 190187, 190202, 190419 }, // cao mei pai
				{ 2, 190022, 190142, 190388 }, // xiao man tou
				{ 3, 190119, 190380, 190380 }, // Hei seng lin Guai Wei Quan
				{ 2, 190167, 190350, 190352 }, // Xi Gua Jie Shu ChongYin
				{ 1, 190201, 190201, 190201 },
				{ 3, 190026, 190216, 190419 },
				{ 2, 190024, 190187, 190187 },
				{ 3, 190022, 190119, 190120 },
				{ 2, 190023, 190023, 190024 },
				{ 2, 190024, 190141, 190187 },
				{ 2, 190024, 190142, 190187 },
				{ 2, 190024, 190024, 190187 },
				{ 3, 190027, 190120, 190196 },
				{ 2, 190141, 190141, 190648 },
				{ 1, 190142, 190667, 190667 },
				{ 1, 190166, 190667, 190667 },
				{ 1, 190667, 190667, 190667 },
				{ 1, 190670, 190670, 190670 },
				{ 3, 190028, 190141, 190142 },
				};

	static uint32_t compound_itms[]
			= /*{ 180015, 180016, 180025, 180041, 180043, 180044, 180045, 180046, 180048, 180054, 180058, 180059, 180062, 180063,*/
				{ 180065, 180061, 180057, 180053, 180021, 180062, 180063, 180059, 180054, 180048, 180015, 180016, 180041, 180025, 180043, 180044, 180045, 180046, 180073, 180089, 180088, 180087,180092,
				180096,180017, 180020, 180001};

	CHECK_BODY_LEN(bodylen, 14);

	int      i = 0, j;
	uint8_t  cnt;
	uint32_t itms[4];

	UNPKG_UINT8(body, itms[0], i);
	UNPKG_UINT8(body, cnt, i);
	for (j = 1; j != 4; ++j) {
		UNPKG_UINT32(body, itms[j], i);
	}

	qsort(itms, array_elem_num(itms), sizeof itms[0], u32cmp);
	for ( j = 0;
			(j != array_elem_num(stuffs)) && memcmp(stuffs[j], itms, sizeof itms);
			++j ) ;

	uint32_t itm = 0;
	if ( j < array_elem_num(stuffs) ) {
		itm = compound_itms[j];
	} else {
		int rno = rand() % 10;
		if (rno < 3) {
			itm = compound_itms[j];
		} else if (rno < 5) {
			itm = compound_itms[j + 1];
		} else if (rno < 9) {
			itm = compound_itms[j + 2];
		}
		/*if ((itms[1] == 190655 || itms[1] == 190656 || itms[1] == 190657 || itms[1] == 190659)*/
		/*|| (itms[2] == 190655 || itms[2] == 190656 || itms[2] == 190657 || itms[2] == 190659)*/
		/*|| (itms[3] == 190655 || itms[3] == 190656 || itms[3] == 190657 || itms[3] == 190659)) {*/
		/*itm = 0;*/
		/*}*/
	}

	DEBUG_LOG("%u %u %u %u", itms[0], itms[1], itms[2], itms[3]);
	DEBUG_LOG("COMPOUND ITM\t[uid=%u itm=%u]", p->id, itm);
	if (itm) {
		uint32_t buf[4 * 2];
		int  i = 2;
		UNPKG_H_UINT32(body, buf[0], i);
		buf[1] = htonl(1);
		UNPKG_H_UINT32(body, buf[2], i);
		buf[3] = htonl(1);
		UNPKG_H_UINT32(body, buf[4], i);
		buf[5] = htonl(1);
		buf[6] = htonl(itm);
		buf[7] = htonl(1);

		return do_ask_for_items(p, 3, (void*)buf, 1, (void*)&(buf[6]));
	} else {
		response_proto_uint32(p, p->waitcmd, 0, 0);
	}

	return 0;
}
//---------------- Compose Item Ends ------------------

//---------------- Exchg Item Related Funcs Begin ----------------
/**
 * exchg_item2_cmd - exchg item but not set task
 *
 */
int exchg_item2_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t exid;
	if (unpkg_uint32(body, bodylen, &exid) == 0) {
		if ((exid > 900 && exid < 1000) && !ISVIP(p->flag))
			ERROR_RETURN(("not vip\t[uid=%u exid=%u]", p->id, exid), -1);
		if ((exid > 600 && exid < 1000) || (exid > 2500 && exid < 2601)) {
			return do_exchange_item(p, exid, 0);
		}
	}

	if (exid > 500 && exid < 600){
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1 );
	}

	return -1;
}

int ggj_exchg_item(sprite_t* p)
{
	uint32_t rand_exchg_itm[7] = {180059, 190028, 190196, 190201, 190232, 1230020, 1230005};
	uint8_t buf[256];

	uint32_t itmid = *(uint32_t*)p->session;

	int j = 0;
	// Package For DB
	PKG_H_UINT32(buf, 1, j);
	PKG_H_UINT32(buf, 1, j);
	PKG_H_UINT32(buf, 0, j);
	PKG_H_UINT32(buf, 0, j);
	if (pkg_item_kind(p, buf, itmid, &j) == -1)
		return -1;
	PKG_H_UINT32(buf, itmid, j);
	PKG_H_UINT32(buf, 1, j);

	uint32_t rand_id = rand_exchg_itm[rand() % (sizeof(rand_exchg_itm) / 4)];
	if (pkg_item_kind(p, buf, rand_id, &j) == -1)
		return -1;

	*(uint32_t*)(p->session + 4) = rand_id;
	item_t* pitm = get_item_prop(rand_id);
	PKG_H_UINT32(buf, rand_id, j);
	PKG_H_UINT32(buf, 1, j);
	PKG_H_UINT32(buf, pitm->max, j);

	DEBUG_LOG("GGJ EXCHANGE\t[%u %u %u]", p->id, itmid, rand_id);
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buf, p->id);
}

int ggj_exchg_itm_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t itmid;
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	UNPKG_UINT32(body, itmid, j);
	*(uint32_t*)p->session = itmid;
	item_t* pitm = get_item_prop(itmid);
	if (!pitm || !pitm->ggj)
		ERROR_RETURN(("bad item id or cannot exchange\t[%u %u]", p->id, itmid), -1);


	return db_set_sth_done(p, 153, 10, p->id);
}

//---------------- Exchg Item Related Funcs End ------------------

void reload_items_cmd(uint32_t opid, const void* body, int len)
{
	if (load_items(config_get_strval("items_conf")) == -1) {
		ERROR_LOG("reload items error");
	}
	DEBUG_LOG("reload items OK");
}

void reload_games_cmd(uint32_t opid, const void* body, int len)
{
	if (load_dll_conf(config_get_strval("games_conf")) == -1) {
		ERROR_LOG("reload games error");
	}
	DEBUG_LOG("reload games OK");
}

void reload_questions_cmd(uint32_t opid, const void* body, int len)
{
	if (load_questions("./conf/questions.xml") == -1) {
		ERROR_LOG("reload questions error");
	}
	if (load_class_questions("./conf/class_questions.xml") == -1) {
		ERROR_LOG("reload questions error");
	}
	DEBUG_LOG("reload questions OK");
}

void reload_gifts_cmd(uint32_t opid, const void* body, int len)
{
	if (load_gifts("./conf/gifts.xml") == -1) {
		ERROR_LOG("reload gifts error");
	}
	DEBUG_LOG("reload gifts OK");
}

void reload_gam_dbp_ip_cmd(uint32_t opid, const void* body, int len)
{
	if (config_init("bench.conf") == -1) {
		ERROR_LOG("reload game and dbproxy ip error");
	}
	if (config_init("common.conf") == -1) {
		ERROR_LOG("reload common file error");
	}

	char*     gameserv_ip   = config_get_strval("gameserv_ip");
	in_port_t gameserv_port = config_get_intval("gameserv_port", 0);
	char*     gameserv_test_ip   = config_get_strval("gameserv_test_ip");
	in_port_t gameserv_test_port = config_get_intval("gameserv_test_port", 0);

	strncpy(config_cache.bc_elem->gameserv_ip, gameserv_ip, sizeof(config_cache.bc_elem->gameserv_ip) - 1); // gameserv ip
	config_cache.bc_elem->gameserv_port = gameserv_port; // gameserv port
	strncpy(config_cache.bc_elem->gameserv_test_ip, gameserv_test_ip, sizeof(config_cache.bc_elem->gameserv_test_ip) - 1); // gameserv ip
	config_cache.bc_elem->gameserv_test_port = gameserv_test_port; // gameserv port
	DEBUG_LOG("reload bench and common file OK");
}

void reload_pic_ip_cmd(uint32_t opid, const void* body, int len)
{
	if (load_xmlconf(PIC_SRV_CONF, load_picsrv_config) == -1) {
		ERROR_LOG("reload picture server ip error");
	}
	DEBUG_LOG("reload picture server ip OK");
}

void reload_tuya_ip_cmd(uint32_t opid, const void* body, int len)
{
	if (load_xmlconf(TUYA_SRV_CONF, load_tuyasrv_config) == -1) {
		ERROR_LOG("reload tuya server ip error");
	}
	DEBUG_LOG("reload tuya server ip OK");
}

void set_db_timeout_cmd(uint32_t opid, const void* body, int len)
{
    uint32_t flag = *(uint32_t*)(body + 4);
    if(flag ==1) {
        db_timeout_flag = 0;
	    DEBUG_LOG("turn off db timeout");
    } else if (flag ==2) {
        db_timeout_flag = 1;
	    DEBUG_LOG("turn on db timeout");
    } else {
        ERROR_LOG("set db timeout err %d", flag);
    }
}

void set_homesvr_switch_cmd(uint32_t opid, const void* body, int len)
{
    uint32_t flag = *(uint32_t*)(body + 4);
    if(flag ==1) {
        use_homeserver = 0;
	    DEBUG_LOG("not use home server");
    } else if (flag ==2) {
        use_homeserver = 1;
	    DEBUG_LOG("use home server");
    } else {
        ERROR_LOG("set home server switch err %d", flag);
    }
}

void reload_holidays_cmd(uint32_t opid, const void* body, int len)
{
	if (load_xml_holidays("./conf/holidays.xml") == -1) {
		ERROR_LOG("reload ./conf/holidays.xml error");
	}

	if(load_events(config_get_strval("events_conf")) == -1) {
		ERROR_LOG("reload ./conf/events.xml error");
	}

	DEBUG_LOG("reload ./conf/holidays.xml OK");
}

void reload_dirty_cmd(uint32_t opid, const void* body, int len)
{
	if (tm_load_dirty("./dirtyFilter/dat/tm_dirty.dat") < 0) {
		ERROR_LOG("reload ./dirtyFilter/dat/tm_dirty.dat error");
	}
	DEBUG_LOG("reload ./dirtyFilter/dat/tm_dirty.dat OK");
}

void reload_rand_type_cmd(uint32_t opid, const void* body, int len)
{
	if(load_rand_items("./conf/rand_type.xml") == -1) {
		ERROR_LOG("reload ./conf/rand_type.xml error");
	}
	DEBUG_LOG("reload ./conf/rand_type.xml OK");
}

void reload_profession_work_cmd(uint32_t opid, const void* body, int len)
{
	if(load_profession_work("./conf/profession_work.xml") == -1){
		ERROR_LOG("reload ./conf/profession_work.xml error");
	}
	DEBUG_LOG("reload ./conf/profession_work.xml OK");
}

void reload_candy_cmd(uint32_t opid, const void* body, int len)
{
	if(load_candy("./conf/candy.xml") == -1){
		ERROR_LOG("reload ./conf/candy.xml error");
	}
	DEBUG_LOG("reload ./conf/candy.xml OK");
}

void reload_cards_cmd(uint32_t opid, const void* body, int len)
{
	if(load_cards("./conf/cards.xml")){
		ERROR_LOG("reload ./conf/cards.xml error");
	}
	DEBUG_LOG("reload ./conf/cards.xml OK");
}

void reload_exchange_cmd(uint32_t opid, const void* body, int len)
{
	if((load_exchange_items(config_get_strval("exchange_conf")) == -1)){
		ERROR_LOG("reload %s error",config_get_strval("exchange_conf"));
	}
	DEBUG_LOG("reload %s OK",config_get_strval("exchange_conf"));
}

void reload_exclu_things_cmd(uint32_t opid, const void* body, int len)
{
	if(load_exclusive_things(config_get_strval("exclusive_things_conf"))) {
		ERROR_LOG("reload %s error",config_get_strval("exclusive_things_conf"));
	}
	DEBUG_LOG("reload %s OK",config_get_strval("exclusive_things_conf"));
}

void reload_tasks_cmd(uint32_t opid, const void* body, int len)
{
	if(load_xmlconf("./conf/tasks.xml", load_task_config)){
		ERROR_LOG("reload ./conf/tasks.xml error");
	}
	DEBUG_LOG("reload ./conf/tasks.xml OK");
}

void reload_npc_tasks_cmd(uint32_t opid, const void* body, int len)
{
	if(load_npc_tasks("./conf/npc_tasks.xml")){
		ERROR_LOG("reload ./conf/npc_tasks.xml error");
	}
	DEBUG_LOG("reload ./conf/npc_tasks.xml OK");
}
void reload_swap_cmd(uint32_t opid, const void* body, int len)
{
	if(load_exchange_conf("./conf/swap.xml")){
		ERROR_LOG("reload ./conf/swap.xml error");
	}
	DEBUG_LOG("reload ./conf/swap.xml OK");
}
void reload_rand_cmd(uint32_t opid, const void* body, int len)
{
	if(load_rand_conf("./conf/rand.xml")){
		ERROR_LOG("reload ./conf/rand.xml error");
	}
	DEBUG_LOG("reload ./conf/rand.xml OK");
}

void reload_game_bonus_cmd(uint32_t opid, const void* body, int len)
{
	if(load_game_bonus("./conf/games_bonus.xml")){
		ERROR_LOG("reload ./conf/games_bonus.xml error");
	}
	DEBUG_LOG("reload ./conf/games_bonus.xml OK");
}

void reload_race_bonus_cmd(uint32_t opid, const void* body, int len)
{
	if(load_race_bonus("./conf/race_bonus.xml")){
		ERROR_LOG("reload ./conf/race_bonus.xml error");
	}
	DEBUG_LOG("reload ./conf/race_bonus.xml OK");
}

void reload_friend_box_cmd(uint32_t opid, const void* body, int len)
{
	if(load_friendbox_config("./conf/friendbox.xml")){
		ERROR_LOG("reload ./conf/friendbox.xml error");
	}
	DEBUG_LOG("reload ./conf/friendbox.xml OK");
}

void reload_lahm_students_cmd(uint32_t opid, const void* body, int len)
{
	if(load_lahm_students_conf("./conf/lahm_students.xml")){
		ERROR_LOG("reload ./conf/lahm_students.xml error");
	}
	DEBUG_LOG("reload ./conf/lahm_students.xml OK");
}

void reload_lahm_lessons_cmd(uint32_t opid, const void* body, int len)
{
	if(load_lahm_lessons_conf("./conf/lahm_lessons.xml")){
		ERROR_LOG("reload ./conf/lahm_lessons.xml error");
	}
	DEBUG_LOG("reload ./conf/lahm_lessons.xml OK");
}

void reload_lahm_classroom_bonus_cmd(uint32_t opid, const void* body, int len)
{
	if(load_lahm_lessons_conf("./conf/lahm_classroom_bonus.xml")){
		ERROR_LOG("reload ./conf/lahm_classroom_bonus.xml error");
	}
	DEBUG_LOG("reload ./conf/lahm_classroom_bonus.xml OK");
}


void reload_lahm_lesson_questions_cmd(uint32_t opid, const void* body, int len)
{
	if(load_lahm_lessons_conf("./conf/lahm_lesson_questions.xml")){
		ERROR_LOG("reload ./conf/lahm_lesson_questions.xml error");
	}
	DEBUG_LOG("reload ./conf/lahm_lesson_questions.xml OK");
}

void reload_donate_item_cmd(uint32_t opid, const void* body, int len)
{
	if(load_friendbox_config("./conf/donateitem.xml")){
		ERROR_LOG("reload ./conf/donateitem.xml error");
	}
	DEBUG_LOG("reload ./conf/donateitem.xml OK");
}

void reload_maps_cmd(uint32_t opid, const void* body, int len)
{
	if(load_maps(config_get_strval("maps_conf"))){
		ERROR_LOG("reload ./conf/maps.xml error");
	}
	DEBUG_LOG("reload ./conf/maps.xml OK");
}

void reload_td_tasks_cmd(uint32_t opid, const void* body, int len)
{
	if(load_tdtask_all()){
		ERROR_LOG("reload ./conf/TDTasks error");
	}
	DEBUG_LOG("reload ./conf/TDTasks OK");
}

void reload_angel_compose_cmd(uint32_t opid, const void* body, int len)
{
	if (load_angel_compose_material_conf("./conf/angel_compose.xml")) {
		ERROR_LOG("reload ./conf/angel_compose.xml error");
	}
	DEBUG_LOG("reload ./conf/angel_compose.xml OK");
}

void reload_dungeon_map_cmd(uint32_t opid,const void * body,int len)
{
	if ( dungeon_load_all_map_info())
	{
		ERROR_LOG("dungeon_load_all_map_info error");
	}
	DEBUG_LOG("dungeon_load_all_map_info OK");
}

void reload_tv_question_cmd(uint32_t opid, const void* body, int len)
{
	if (load_tv_questions("./conf/tv_question.xml", 2)) {
		ERROR_LOG("reload ./conf/tv_question.xml error");
	}
	DEBUG_LOG("reload ./conf/tv_questiom.xml");
}

void reload_af_skill_cmd(uint32_t opid, const void* body, int len)
{
	if (load_af_skill("./conf/af_skills.xml")) {
		ERROR_LOG("reload ./conf/af_skills.xml error");
	}
	DEBUG_LOG("reload ./conf/af_skills.xml");
}

void reload_af_prize_cmd(uint32_t opid, const void* body, int len)
{
	if (load_prize_conf("./conf/af_prize.xml")) {
		ERROR_LOG("reload ./conf/af_prize.xml error");
	}
	DEBUG_LOG("reload ./conf/af_prize.xml");
}

void reload_af_prize_type_cmd(uint32_t opid, const void* body, int len)
{
	if (load_prize_type_conf("./conf/af_prize_type.xml")) {
		ERROR_LOG("reload ./conf/af_prize_type.xml error");
	}
	DEBUG_LOG("reload ./conf/af_prize_type.xml");
}

void reload_af_barrier_cmd(uint32_t opid, const void* body, int len)
{
	if (load_barrier_conf("./conf/levelinfo.xml")) {
		ERROR_LOG("reload ./conf/levelinfo.xml error");
	}
	DEBUG_LOG("reload ./conf/levelinfo.xml");
}

void reload_af_task_cmd(uint32_t opid, const void* body, int len)
{
	if (load_af_task("./conf/af_task.xml")) {
		ERROR_LOG("reload ./conf/af_task.xml error");
	}
	DEBUG_LOG("reload ./conf/af_task.xml");

}

int reload_conf_op(uint32_t uid, void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 16);
	communicator_body_t* cbody = buf;
	uint32_t oi = *(uint32_t*)(buf + 12);
	if (oi != 0 && oi != config_cache.bc_elem->online_id)
		return 0;

	DEBUG_LOG("reload conf %u", cbody->opid);
#define RELOAD_CONF_OP(op_, func_) \
		case (op_): (func_)(op_, cbody->body, len - 16); break

	switch (cbody->opid) {

		RELOAD_CONF_OP(RELOAD_ITEM, reload_items_cmd);
		RELOAD_CONF_OP(RELOAD_GAME, reload_games_cmd);
		RELOAD_CONF_OP(RELOAD_QUESTION, reload_questions_cmd);
		RELOAD_CONF_OP(RELOAD_GIFTS, reload_gifts_cmd);
		RELOAD_CONF_OP(RELOAD_GAM_DBP_IP, reload_gam_dbp_ip_cmd);
		RELOAD_CONF_OP(RELOAD_PIC_SRV, reload_pic_ip_cmd);
		RELOAD_CONF_OP(RELOAD_TUYA_SRV, reload_tuya_ip_cmd);
		RELOAD_CONF_OP(SET_DB_TIMEOUT, set_db_timeout_cmd);
		RELOAD_CONF_OP(RELOAD_HOLIDAYS, reload_holidays_cmd);
		RELOAD_CONF_OP(SET_HOMESVR_SWITCH, set_homesvr_switch_cmd);
		RELOAD_CONF_OP(RELOAD_DIRTY_FILE, reload_dirty_cmd);
		RELOAD_CONF_OP(RELOAD_RAND_TYPE, reload_rand_type_cmd);
		RELOAD_CONF_OP(RELOAD_PRO_WORK, reload_profession_work_cmd);
		RELOAD_CONF_OP(RELOAD_CNADY, reload_candy_cmd);
		RELOAD_CONF_OP(RELOAD_CARDS, reload_cards_cmd);
		RELOAD_CONF_OP(RELOAD_EXCHANGE, reload_exchange_cmd);
		RELOAD_CONF_OP(RELOAD_EXCLU_THING, reload_exclu_things_cmd);
		RELOAD_CONF_OP(RELOAD_TASKS, reload_tasks_cmd);
		RELOAD_CONF_OP(RELOAD_NPC_TASKS, reload_npc_tasks_cmd);
		RELOAD_CONF_OP(RELOAD_SWAP, reload_swap_cmd);
		RELOAD_CONF_OP(RELOAD_RAND, reload_rand_cmd);
		RELOAD_CONF_OP(RELOAD_GAME_BONUS, reload_game_bonus_cmd);
		RELOAD_CONF_OP(RELOAD_RACE_BONUS, reload_race_bonus_cmd);
		RELOAD_CONF_OP(RELOAD_FRIEND_BOX, reload_friend_box_cmd);
		RELOAD_CONF_OP(RELOAD_LAHM_STUDENTS, reload_lahm_students_cmd);
		RELOAD_CONF_OP(RELOAD_LAHM_LESSONS, reload_lahm_lessons_cmd);
		RELOAD_CONF_OP(RELOAD_LAHM_CLASSROOM_BONUS, reload_lahm_classroom_bonus_cmd);
		RELOAD_CONF_OP(RELOAD_LAHM_LESSON_QUESTIONS, reload_lahm_lesson_questions_cmd);
		RELOAD_CONF_OP(RELOAD_DONATE_ITEM, reload_donate_item_cmd);
		RELOAD_CONF_OP(RELOAD_MAPS, reload_maps_cmd);
		RELOAD_CONF_OP(RELOAD_TD_TASKS, reload_td_tasks_cmd);
		RELOAD_CONF_OP(RELOAD_ANGEL_COMPOSE, reload_angel_compose_cmd);
		RELOAD_CONF_OP(RELOAD_DUNGEON_MAP, reload_dungeon_map_cmd);
		RELOAD_CONF_OP(RELOAD_TV_QUESTION, reload_tv_question_cmd);
		RELOAD_CONF_OP(RELOAD_AF_SKILL, reload_af_skill_cmd);
		RELOAD_CONF_OP(RELOAD_AF_PRIZE, reload_af_prize_cmd);
		RELOAD_CONF_OP(RELOAD_AF_PRIZE_TYPE, reload_af_prize_type_cmd);
		RELOAD_CONF_OP(RELOAD_AF_BARRIER, reload_af_barrier_cmd);
		RELOAD_CONF_OP(RELOAD_AF_TASK, reload_af_task_cmd);
		RELOAD_CONF_OP(RELOAD_TD_TASK, reload_angel_td_conf);

	default:
		break;
	}
#undef RELOAD_CONF_OP

	return 0;
}

int set_closet_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    if(!ISVIP(p->flag)) {
        ERROR_RETURN(("closet not vip [%d]", p->id), -1);
    }
    int i = 0;
    uint32_t itemid;
    UNPKG_UINT32(body, itemid, i);
	item_kind_t* itmkind = find_kind_of_item(itemid);
    if (!itmkind || (itmkind && itmkind->kind != SUIT_ITEM_KIND))
			ERROR_RETURN (("error item id=%d", itemid), -1);
    return send_request_to_db(SVR_PROTO_SET_CLOSET, p, 4, &itemid, p->id);
}

int unset_closet_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);

    int i = 0;
    uint32_t itemid;
    UNPKG_UINT32(body, itemid, i);
	item_kind_t* itmkind = find_kind_of_item(itemid);
    if (!itmkind || (itmkind && itmkind->kind != SUIT_ITEM_KIND))
			ERROR_RETURN (("error item id=%d", itemid), -1);
    return send_request_to_db(SVR_PROTO_UNSET_CLOSET, p, 4, &itemid, p->id);
}

int get_closet_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    return send_request_to_db(SVR_PROTO_GET_CLOSET, p, 0, NULL, p->id);
}

int get_closet_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN_GE(len, 4);
    int i = 0;
    uint32_t count;
    UNPKG_H_UINT32(buf, count, i);
    CHECK_BODY_LEN(len, 4 + count * 8);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, count, l);
    for(i = 0; i < count; i++) {
        PKG_UINT32(msg, *(uint32_t*)(buf + 4 + 8 * i), l);
        PKG_UINT32(msg, *(uint32_t*)(buf + 8 + 8 * i), l);
    }
    init_proto_head(msg, p->waitcmd, l);
    return send_to_self(p, msg, l, 1);
}

int set_closet_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

int unset_closet_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

 int db_buy_itm_freely(sprite_t* p, userid_t uid, uint32_t itmid, uint32_t cnt)
{
	item_t* pitm = get_item_prop(itmid);
	if (!pitm)
		ERROR_RETURN( ("ITEM NOT EXIST\t[%u]", p ? p->id : 0), -1 );

	item_kind_t* kind = find_kind_of_item(itmid);
	if (!kind)
		ERROR_RETURN( ("WRONG KIND\t[%u]", p ? p->id : 0), -1 );

	uint32_t flag = 0;
	if (kind->kind == HOME_ITEM_KIND) {
		flag = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND) {
		flag = 2;
	} else if (kind->kind == HAPPY_CARD_KIND) {
		flag = 6;
	}

	uint32_t tmp_buf[5];
	tmp_buf[0] = flag;
	tmp_buf[1] = itmid;
	tmp_buf[2] = cnt;
	tmp_buf[3] = pitm->max;
	tmp_buf[4] = 0;
	return send_request_to_db(SVR_PROTO_BUY_ITEM, p, 20, tmp_buf, uid);
}

//PROTO_GET_ITEMS_COUNT
int get_items_count_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 4);
	uint32_t count;
	int j = 0;
	UNPKG_UINT32(body, count, j);
	if (count > 20) {
		return send_to_self_error(p, p->waitcmd, -ERR_query_count_too_many, 1);
	}
	CHECK_BODY_LEN(bodylen, 4 + 4*count);
	int loop, i = 0;
	PKG_H_UINT32(msg, count, i);
	for (loop = 0; loop < count; loop ++) {
		uint32_t itmid;
		UNPKG_UINT32(body, itmid, j);
		PKG_H_UINT32(msg, itmid, i);
	}
	return send_request_to_db(SVR_QUERY_ITEM_COUNT, p, i, msg, p->id);
}

int get_items_count_new_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 8);
	uint32_t userid = 0;
	uint32_t count;
	int j = 0;
	UNPKG_UINT32(body, userid, j);
	UNPKG_UINT32(body, count, j);
	if (count > 20) {
		return send_to_self_error(p, p->waitcmd, -ERR_query_count_too_many, 1);
	}
	CHECK_BODY_LEN(bodylen, 8 + 4*count);
	int loop, i = 0;
	PKG_H_UINT32(msg, count, i);
	for (loop = 0; loop < count; loop ++) {
		uint32_t itmid;
		UNPKG_UINT32(body, itmid, j);
		PKG_H_UINT32(msg, itmid, i);
	}
	return send_request_to_db(SVR_QUERY_ITEM_COUNT, p, i, msg, userid);
}


int respond_items_count(sprite_t * p, char * buf, int len)
{
	uint32_t count;
	int j = 0, i = sizeof(protocol_t), loop;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, count * 8 + 4);
	PKG_UINT32(msg, count, i);
	for (loop = 0; loop < count; loop ++) {
		uint32_t itmid, itmcn;
		UNPKG_H_UINT32(buf, itmid, j);
		UNPKG_H_UINT32(buf, itmcn, j);
		PKG_UINT32(msg, itmid, i);
		PKG_UINT32(msg, itmcn, i);
	}
	init_proto_head(msg, p->waitcmd, i);
	DEBUG_LOG("respond_items_count uid=%u cmd=%d",p->id,p->waitcmd);
	return send_to_self(p, msg, i, 1);
}

int get_items_price_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 4);
	uint32_t count, itmid;
	int j = 0, i = sizeof(protocol_t), loop;
	UNPKG_UINT32(body, count, j);
	CHECK_BODY_LEN(bodylen, 4 + 4 * count);
	PKG_UINT32(msg, count, i);
	for (loop = 0; loop < count; loop ++) {
		UNPKG_UINT32(body, itmid, j);
		PKG_UINT32(msg, itmid, i);
		item_t* itm = get_item_prop(itmid);
		if (itm) {
			PKG_UINT32(msg, itm->sell_price, i);
			continue;
		}
		PKG_UINT32(msg, 88888888, i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int newyear_exchange_items_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	CHECK_BODY_LEN(bodylen,4);

	int i = 0;
	uint32_t itm_own;
	UNPKG_UINT32(body,itm_own,i);
	uint32_t itm_exchg[] = {190603,190604,190605};

	DEBUG_LOG("%u NEW YEAR ITEM %d",p->id,itm_own);

	if(itm_own == 190605 && !p->driver_time) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_driver, 1);
	}

	for (i = 0; i < sizeof(itm_exchg) / sizeof(uint32_t); i++) {
		if (itm_exchg[i] == itm_own) {
			*(uint32_t*)p->session = i;
			return db_single_item_op(p, p->id, itm_own, 1, 0);
		}
	}

	return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
}

int do_newyear_exchange_items(sprite_t *p)
{
	int i = sizeof(protocol_t);
	uint32_t itm_idx = *(uint32_t*)p->session;

	if (itm_idx == 0) {
		uint32_t buff[] = {
			0,4,0,0,
			0,13194,1,1,
			0,13195,1,1,
			0,13196,1,1,
			0,13197,1,1
			};

		PKG_UINT32(msg, 4, i);
		PKG_UINT32(msg, 13194, i);
		PKG_UINT32(msg, 13195, i);
		PKG_UINT32(msg, 13196, i);
		PKG_UINT32(msg, 13197, i);
		DEBUG_LOG("%u NEYYEAR cloth %d -- %d",p->id,13194,13197);

		send_request_to_db(SVR_PROTO_EXCHG_ITEM, 0, sizeof(buff), buff, p->id);
	} else if(itm_idx == 1) {
//		uint32_t tiger[2] = {1270040,1270041};//TIGER
//		uint32_t give_tiger = tiger[rand() % (sizeof(tiger)/sizeof(int))];
		uint32_t give_tiger = 1270040;
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, give_tiger, i);
		DEBUG_LOG("%u NEYYEAR tiger %d",p->id,give_tiger);
		db_single_item_op(0, p->id, give_tiger, 1, 1);
	} else if(itm_idx == 2) {
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, 1300011, i);//抬抬轿
		DEBUG_LOG("%u NEYYEAR car %d",p->id,1300011);

		db_add_car(0, 1300011, 0, p->id);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int give_npc_some_items_cmd(sprite_t * p, const uint8_t * body, int bodylen)
{
	CHECK_BODY_LEN(bodylen,16);

	int i = 0;
	uint32_t npctid;
	uint32_t need_return;
	uint32_t itm_own,itm_cnt;

	UNPKG_UINT32(body,itm_own,i);
	UNPKG_UINT32(body,itm_cnt,i);
	UNPKG_UINT32(body,npctid,i);
	UNPKG_UINT32(body,need_return,i);


	if(npctid < 1000 || npctid > 1002) {
		ERROR_LOG("uid=%u npctid = %u", p->id, npctid);
		npctid = 1000 + (rand() % 3);
	}

	*(uint32_t*)p->session = need_return;
	*(uint32_t*)(p->session + 4) = npctid;

	if (itm_own == 190627) {//tang yuan
		*(uint32_t*)(p->session + 8) = itm_own;
		*(uint32_t*)(p->session + 12) = itm_cnt;
		return db_set_sth_done(p, 220, 10, p->id);
	}

	return send_to_self_error(p, p->waitcmd, -ERR_invalid_item_id, 1);
}

int do_give_npc_items(sprite_t *p)
{
	int i = sizeof(protocol_t);
	uint32_t need_return = *(uint32_t*)p->session;
	uint32_t npctid = *(uint32_t*)(p->session + 4);
	uint32_t itm_own = *(uint32_t*)(p->session + 8);
//	uint32_t itm_cnt = *(uint32_t*)(p->session + 12);

	db_set_cnt(0, npctid, 1);

	if (need_return == 0) {
		PKG_UINT32(msg, 0, i);
	} else {
		if(itm_own == 190627)
		{
			uint32_t rand_itms[] = {180041,190601,190602,1220136,180041,190601,190602,1220136,180041,1220136};
			uint32_t give_itm = rand_itms[rand() % (sizeof(rand_itms)/sizeof(int))];

			PKG_UINT32(msg, 1, i);
			PKG_UINT32(msg, give_itm, i);

			if(give_itm == 1220136) {
				db_buy_itm_freely(0, p->id, give_itm, 1);
			} else {
				db_single_item_op(0, p->id, give_itm, 1, 1);
			}
		}
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

/* @brief 用点点豆购买物品
 */
int buy_item_by_beans_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	uint32_t count;
	uint32_t itemid;

	unpkg_uint32_uint32(body,bodylen,&itemid,&count);

	item_t *itm = get_item_prop(itemid);

	if(!itm) {
		ERROR_RETURN(("invalid item id=%u uid=%u", itemid,p->id), 0);
	}

	if(!itm_buyable_beans(p, itm)) {
		if(itm_vip_buyable(itm)) {
			return send_to_self_error(p,p->waitcmd,-ERR_not_vip,1);
		} else {
			return send_to_self_error(p,p->waitcmd,-ERR_cannot_buy_itm,1);
		}
	}
	p->sess_len = 0;
	PKG_UINT32(p->session,itemid,p->sess_len);
	PKG_UINT32(p->session,count,p->sess_len);
	PKG_UINT32(p->session,count * itm->beans,p->sess_len);

	int body_len = 0;
	uint8_t body_buf[128] = {0};

	PKG_H_UINT32(body_buf,1,body_len);
	PKG_H_UINT32(body_buf,1,body_len);
	PKG_H_UINT32(body_buf,0,body_len);
	PKG_H_UINT32(body_buf,0,body_len);

	if (pkg_item_kind(p, body_buf,16012,&body_len) == -1) {
		return -1;
	}
	PKG_H_UINT32(body_buf,16012,body_len);
	/*除以100是item表里把价格扩展了100倍, 加99是单个购买不是整数则加一*/
	int beans_num = ((count * itm->beans + 99) /  100);
	PKG_H_UINT32(body_buf, beans_num, body_len);

	if (pkg_item_kind(p, body_buf,itm->id,&body_len) == -1) {
		return -1;
	}
	PKG_H_UINT32(body_buf,itm->id,body_len);
	PKG_H_UINT32(body_buf,count,body_len);
	PKG_H_UINT32(body_buf,itm->max,body_len);

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p,body_len,body_buf,p->id);
}


int buy_item_by_beans_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t beans;
	uint32_t count;
	uint32_t itemid;

	p->sess_len = 0;
	UNPKG_UINT32(p->session,itemid,p->sess_len);
	UNPKG_UINT32(p->session,count,p->sess_len);
	UNPKG_UINT32(p->session,beans,p->sess_len);
	beans = (beans + 99)/100;
	beans = (beans + 99) / 100;

	uint32_t msg_buff[2] = {0,beans};
	msglog(statistic_logfile, 0x04020400,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	msglog(statistic_logfile, 0x04040107,get_now_tv()->tv_sec, &(p->id), 4);

	uint32_t index = 0;
	uint32_t ids[] = {
		1270017,1270018,1270021,1270012,1270044,1270010,1230044,1200040,
		1200041,1200037,12482,12483,12484,1270054,1270066,1270067,
		190750,190751,190752,190753,190754,190757,16011,1230032,
		17013,1270060,1270065,1230031,160435,160436,1200008,1200031,
		1200009,12601,12602,12603,12604,190574,1270021,12667,
		12668,190028,190351,190309,190809,190196,17008,190574,
		1230054,1230045,1270011,190216,1200033,1200015,12482,12483,
		12484,1200051,12032,12033,190840,1270077,1230068,190602,
		1270072,1200053,1200055,1200056,13694,1200056,1200055,12352,
		12378,1230066,190840,190309,1230050,1230026,190849,190852,
		13714,13715,13746,1200060,13753,13754,13755,1200061};
	while(index < sizeof(ids)/sizeof(ids[0])) {
		if(ids[index] == itemid) {
			msg_buff[0] = count;
			msg_buff[1] = p->id;

			if (index < 88)
			{
			    msglog(statistic_logfile, 0x04020401 + index,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
			}
			else
			{
			    msglog(statistic_logfile, 0x0408B3F2 + (index - 88),get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
			}

			break;
		}
		index++;
	}

	response_proto_uint32_uint32(p,p->waitcmd,itemid,count,0);
	return 0;
}

int add_temp_item_cmd(sprite_t * p,const uint8_t * body,int bodylen)
{
	uint32_t count;
	uint32_t itemid;

	unpkg_uint32_uint32(body,bodylen,&itemid,&count);

	if(itemid < 1351020 || itemid > 1351999) {
		ERROR_RETURN(("invalid item id=%u uid=%u", itemid,p->id), 0);
	}

	item_t *itm = get_item_prop(itemid);
	if(!itm) {
		ERROR_RETURN(("invalid item id=%u uid=%u", itemid,p->id), 0);
	}

	p->sess_len = 0;
	PKG_UINT32(p->session,itemid,p->sess_len);
	PKG_UINT32(p->session,count,p->sess_len);

	int body_len = 0;
	uint8_t body_buf[128] = {0};

	PKG_H_UINT32(body_buf,0,body_len);
	PKG_H_UINT32(body_buf,1,body_len);
	PKG_H_UINT32(body_buf,0,body_len);
	PKG_H_UINT32(body_buf,0,body_len);

	if (pkg_item_kind(p, body_buf,itm->id,&body_len) == -1) {
		return -1;
	}
	PKG_H_UINT32(body_buf,itm->id,body_len);
	PKG_H_UINT32(body_buf,count,body_len);
	PKG_H_UINT32(body_buf,itm->max,body_len);

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p,body_len,body_buf,p->id);
}

int add_temp_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t count;
	uint32_t itemid;

	p->sess_len = 0;
	UNPKG_UINT32(p->session,itemid,p->sess_len);
	UNPKG_UINT32(p->session,count,p->sess_len);

	response_proto_uint32_uint32(p,p->waitcmd,itemid,count,0);
	return 0;
}

static inline uint32_t
find_type_from_item_kind(item_kind_t* itmkind)
{
	int type = 0;
	if (!itmkind) {
		ERROR_RETURN(("get item array: item id error"), -1);
	} else {
		switch (itmkind->kind) {
			case SUIT_ITEM_KIND:
			case CHG_COLOR_ITEM_KIND:
			case CHG_SHOW_ITEM_KIND:
			case THROW_ITEM_KIND:
			case FEED_ITEM_KIND:
			case SPECIAL_ITEM_KIND:
			case HOMELAND_SEED_KIND:
			case ANIMAL_BABY_ITEM:
				type = 0;
				break;
			case HOME_ITEM_KIND:
				type = 1;
				break;
			case HOMELAND_ITEM_KIND:
				type = 2;
				break;
			case CARD_ITEM_KIND:
				type = 3;
				break;
			case PASTURE_ITEM_KIND:
				type = 4;
				break;
			case HAPPY_CARD_KIND:
				type = 6;
				break;
			case CLASS_ITEM_KIND:
				type = 7;
				break;
			case CAR_KIND:
				type = 8;
				break;
			case MDOG_KIND:
				type = 11;
				break;
			case RESTAURANT_ITEM_KIND:
				type = 26;
				break;
			case DRAGON_KIND:
				type = 27;
				break;
			case TEMP_ITEM_KIND:
				type = 99;
				break;
			case ATTR_ITEM_KIND:	//经验奖励
				type = 10;
				break;
			case FIGHT_ITEM_KIND:
			case FIGHT_CARD_KIND:
			case FIGHT_COLLECT_KIND:
				type = 32;
				break;
			case CUTE_PIG_KIND:
			case CUTE_PIG_ITEM_KIND:
			case CUTE_PIG_CLOTHES_KIND:
				type = 36;
				break;
			default:
				ERROR_RETURN(("get item array: item id error"), -1);
		}
	}
	return type;
}

int get_item_array_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	typedef struct item_type_array{
		uint32_t itemid;
		uint32_t type;
	}__attribute__((packed)) item_type_array_t;

	uint32_t item_count;
	uint32_t flag;
	item_type_array_t item_array[20];
	int j = 0;
	UNPKG_UINT32(body, flag, j);
	UNPKG_UINT32(body, item_count, j);

	CHECK_BODY_LEN(bodylen, 8 + 4 * item_count);
	CHECK_VALID_ID(p->id);
	if (item_count > 20) {
		DEBUG_LOG("item count too loog");
		return send_to_self_error(p, p->waitcmd, -ERR_query_count_too_many, 1);
	}

	uint8_t db_buff[1024] = {0};
	int len = 0;
	PKG_H_UINT32(db_buff, flag, len);
	PKG_H_UINT32(db_buff, item_count, len);

	int loop;
	for (loop = 0; loop < item_count; ++loop) {
		UNPKG_UINT32(body, item_array[loop].itemid, j);
		item_array[loop].type = find_type_from_item_kind(find_kind_of_item(item_array[loop].itemid));
		PKG_H_UINT32(db_buff, item_array[loop].itemid, len);
		PKG_H_UINT32(db_buff, item_array[loop].type, len);
	}

	return send_request_to_db(SVR_PROTO_GET_ITEM_ARRAY, p, len, db_buff, p->id);
}

int get_item_array_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	switch( p->waitcmd )
	{
	case PROTO_CP_EXCHANGE_PIG:
		return cutepig_exchange_pig_with_items_callback( p, id, buf, len );
	case PROTO_CHP_DONATE:
		return charparty_check_item_callback( p, id, buf, len );
	case PROTO_CP_USER_PIGLET_USE_ACCELERATE_TOOL:
		{
			typedef struct items_st{
				uint32_t itemid;
				uint32_t count;
			}items_t;
			uint32_t count = *(uint32_t*)buf;
			items_t* head = (items_t*)(buf+sizeof(uint32_t));
			 if(count > 0 && head->count > 0){
					 uint32_t type = 50019;
					 return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &type, p->id);
			 }
			 else{
				 return send_to_self_error(p, p->waitcmd, -ERR_use_acc_machine_tool, 1);
			 }
		}
	case PROTO_GET_MAKE_BIRTHDAY_CAKE_AWARD:
		{
			return get_make_birthday_cake_for_client(p,id, buf, len);
		}
	case PROTO_CHECK_DANCING_THREE_TIMES:
		{
			return get_crystal_dancing_times(p,id, buf, len);
		}
	case PROTO_LIGHT_CELEBRATE_FOURYEAR_BOOK:
		{
			return generate_book_light_flag(p, id, buf, len);
		}
	case PROTO_MOLE_GET_MISS_NOTE:
		{
			typedef struct items_st{
				uint32_t itemid;
				uint32_t count;
			}items_t;

			uint32_t count = 0;
			CHECK_BODY_LEN_GE(len, sizeof(count));
			count = *(uint32_t*)buf;
			CHECK_BODY_LEN(len, sizeof(count)+ count*sizeof(items_t));
			items_t* head = (items_t*)(buf+sizeof(uint32_t));

			*(uint32_t*)(p->session+4) = count;
			items_t* other = (items_t*)(p->session+sizeof(uint32_t)*2);
			int j = 0;
			for(; j < count; ++j){
				(other+j)->itemid = (head+j)->itemid;
				(other+j)->count = (head+j)->count;
			}
			return send_request_to_db(SVR_PROTO_GET_CHAPTER_STATE, p, 0, NULL, p->id);
			//return mole_get_miss_note_callback(p, id, buf, len);
		}
	case PROTO_MOLE_ATTACK_INVADE_MONSTER:
		{
			return mole_attack_invade_monster_callback(p, id, buf, len);
		}
	}
	

	int i = 0;
	uint32_t count;
	UNPKG_H_UINT32(buf, count, i);
	CHECK_BODY_LEN(len, 4 + count * 8);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, count, l);
	for(i = 0; i < count; i++) {
		PKG_UINT32(msg, *(uint32_t*)(buf + 4 + 8 * i), l);
		PKG_UINT32(msg, *(uint32_t*)(buf + 8 + 8 * i), l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}
