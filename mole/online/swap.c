/*
 * =====================================================================================
 *
 *       Filename:  arch_work.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/06/2010 09:01:01 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "exclu_things.h"
#include "swap.h"
#include "util.h"
#include "small_require.h"
#include "npc_event.h"
#include "foot_print.h"
#include "event.h"
#include "fire_cup.h"
#include "mole_cutepig.h"


#define MAX_SWAP_RECORD  2000

static in_out_item_t swap[MAX_SWAP_RECORD];


static int parse_single_item(item_unit_t* iut, int* cnt, xmlNodePtr cur, int flag)
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

static int parse_itemsin_itemsout(in_out_item_t *record, xmlNodePtr chl)
{
	xmlNodePtr cur;

	while(chl) {
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemIn")) {
			cur = chl->xmlChildrenNode;
			if (parse_single_item(record->in_item, &(record->in_cnt), cur, 0) != 0) {
				return -1;
			}
		}

		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemOut")) {
			cur = chl->xmlChildrenNode;
			if (parse_single_item(record->out_item, &(record->out_cnt), cur, 1) != 0) {
				return -1;
			}
		}
		chl = chl->next;
	}
	return 0;
}

int load_exchange_conf(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int record_count = 1;

	doc = xmlParseFile (file);
	if (!doc) {
		ERROR_RETURN (("load profession work config failed"), -1);
	}
	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Entry"))) {
			DECODE_XML_PROP_INT(i, cur, "ID");
			if (record_count > MAX_SWAP_RECORD) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, record_count, i);
				goto exit;
			}
			swap[i - 1].id = i;
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].mul_set, cur, "MulSet", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].need_vip, cur, "NeedVip", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].time_chk, cur, "TimeChk", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].flag_idx, cur, "FlagIdx", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].itemin_cmd_get, cur, "InCmdGet", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].itemin_cmd_del, cur, "InCmdDel", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].day_type, cur, "DayType", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].pl_count, cur, "PlCount", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].sl_count, cur, "SlCount", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].date_start, cur, "DateStart", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].date_end, cur, "DateEnd", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].animal_count, cur, "AnimalCount", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].holiday, cur, "Holiday", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].discount, cur, "Discount", 100);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].fight_lvl, cur, "FightLvl", 1);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].mul_start, cur, "MulStart", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].mul_end, cur, "MulEnd", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].vip_level, cur, "VipLevel", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].limit_type, cur, "LimitType", 0);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].random, cur, "Rand", 100);
			DECODE_XML_PROP_INT_DEFAULT(swap[i - 1].invitee_cnt, cur, "InviteeCnt", 0);

			chl = cur->xmlChildrenNode;
			if ((parse_itemsin_itemsout(&swap[i - 1], chl) != 0)) {
				goto exit;
			}
			record_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load profession work item file %s", file);
}

/* @brief 打包数据到DB
 * @param type 对应配置表里的ID号
 * @param mul 一次性兑换的数目
 */
static int pack_to_db(sprite_t *p, uint32_t type, uint32_t mul)
{
	/*如果不是从USER库删除，则删除的数目设置为零*/
	int count = swap[type - 1].in_cnt;
	if (swap[type - 1].itemin_cmd_get != 0) {
		count = 0;
	}

	//package for DB
	uint8_t buf[pagesize];
	int j = 0;
	PKG_H_UINT32(buf, count, j);
	PKG_H_UINT32(buf, swap[type - 1].out_cnt, j);
	PKG_H_UINT32(buf, NOT_INCLUDE_MONEY_DAY_LIMIT, j);
	PKG_H_UINT32(buf, NOT_LIMIT_BY_OTHER_STH_MAX, j);

	int i = 0;
	for (i = 0; i != count; ++i) {
		if (pkg_item_kind(p, buf, swap[type - 1].in_item[i].itm->id, &j) == -1) {
			return -1;
		}
		uint32_t in_count = swap[type - 1].in_item[i].count * mul;
		if (ISVIP(p->flag) && swap[type - 1].discount != 100) {//如果是VIP用户，并且兑换的东西打折
			in_count = swap[type - 1].discount * in_count / 100;
		}
		PKG_H_UINT32(buf, swap[type - 1].in_item[i].itm->id, j);
		PKG_H_UINT32(buf, in_count, j);
		add_fire_cup_medal_count(p, swap[type - 1].in_item[i].itm->id, -1 * (int32_t)in_count);
	}

	for (i = 0; i != swap[type - 1].out_cnt; ++i) {
		if (pkg_item_kind(p, buf, swap[type - 1].out_item[i].itm->id, &j) == -1) {
			return -1;
		}
		PKG_H_UINT32(buf, swap[type - 1].out_item[i].itm->id, j);
		PKG_H_UINT32(buf, swap[type - 1].out_item[i].count * mul , j);
		PKG_H_UINT32(buf, swap[type - 1].out_item[i].itm->max, j);

		add_foot_print_count(p, swap[type - 1].out_item[i].itm->id, swap[type - 1].out_item[i].count);
		add_fire_cup_medal_count(p, swap[type - 1].out_item[i].itm->id, (int32_t)(swap[type - 1].out_item[i].count * mul));
		add_lahm_sport_medal_count(p, swap[type - 1].out_item[i].itm->id, (int32_t)(swap[type - 1].out_item[i].count * mul));
	}

	if (type >= 592 && type <= 594)
	{
		uint32_t mm_exp = 0;
		if (type == 592){
			mm_exp = 200;
		}
		else if(type == 593){
			mm_exp = 100;
		}
		else{
			mm_exp = 50;
		}
		send_request_to_db(SVR_PROTO_AF_ADD_FIGHT_EXP, NULL, sizeof(mm_exp), &mm_exp, p->id);
	}

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buf, p->id);
}

/* @brief  交换物品的请求函数
 */
int swap_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t type = 0;
	uint32_t mul = 0;
	uint32_t flag = 0;
	CHECK_BODY_LEN(bodylen, sizeof(type) + sizeof(mul) + sizeof(flag));
	unpack(body, sizeof(type) + sizeof(mul) + sizeof(flag), "LLL", &type, &mul, &flag);
	DEBUG_LOG("userid[%u] swap cmd:type[%u] mul[%u] flag[%u]", p->id, type, mul, flag);

	*(uint32_t *)p->session = type;
	*(uint32_t *)(p->session + sizeof(type)) = mul;
	*(uint32_t *)(p->session + sizeof(type) + sizeof(mul)) = flag;

	if (mul == 0 && (type == 566 || type == 567)) {//勇者之战 精灵命运轮 转到的值为0，直接设置
		return db_set_sth_done(p, swap[type  - 1].day_type, 1, p->id);
	}
	
	if ((type > MAX_SWAP_RECORD ) || type < 1 || (swap[type - 1].id == 0) || (mul == 0)) {
		ERROR_LOG("SWAP WRONG %d %d %d", p->id, type, mul);
		return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
	}
	
	if (swap[type - 1].mul_start && swap[type - 1].mul_end)
	{
		if ((mul < swap[type - 1].mul_start) || (mul > swap[type - 1].mul_end))
		{
			return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
		}
	}
	
	/*扣除物品的系数，如果配置文件里有配置，忽律客户端传过来的，否则使用客户端传过来的*/
	if(swap[type - 1].mul_set) {
		mul = swap[type - 1].mul_set;
	}
	*(uint32_t *)(p->session + sizeof(type)) = mul;

	if (swap[type - 1].need_vip && !ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd,-ERR_no_super_lahm, 1);
	}
	
	if (type >= 566 && type <= 585 && p->team_id < 1 && p->team_id > 4) {
		return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
	}

	if (type >= 500 && type <= 550 && swap[type - 1].fight_lvl > p->fight_level) {
		return send_to_self_error(p, p->waitcmd, -ERR_af_limit_fight_level, 1);
	}

#ifndef TW_VER
	if (type == 592 && p->team_id != 3)
#else
	if (type == 592 && p->team_id != 4)
#endif
	{
		return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
	}
#ifndef TW_VER
	else if(type == 593 && p->team_id != 4)
#else
	else if(type == 593 && p->team_id != 2)
#endif
	{
		return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
	}
#ifndef TW_VER
	else if(type == 594 && !(p->team_id == 1 || p->team_id == 2))
#else
	else if(type == 594 && !(p->team_id == 1 || p->team_id == 3))
#endif
	{
		return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
	}

	if(swap[type - 1].vip_level)
	{
		int vip_month_level = calc_month_vip_level(p->vip_month);
		if ((vip_month_level < swap[type - 1].vip_level) && (p->vip_level < swap[type - 1].vip_level)) {
			return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
		}
	}
	
	//没有随即到 则返回空
	if( rand()%100 + 1 > swap[type-1].random  )
	{
		int send_len = sizeof(protocol_t);
		PKG_UINT32( msg, type, send_len );
		PKG_UINT32( msg, 0, send_len );
		init_proto_head( msg, p->waitcmd, send_len );
		return send_to_self( p, msg, send_len, 1 );
	}

	/*校验物品用户是否已经领取过*/
	if(swap[type - 1].flag_idx) {
		if(swap[type - 1].flag_idx >= 36 && swap[type - 1].flag_idx < 56) {
			if(p->invitee_cnt < swap[type - 1].flag_idx - 35) {
				return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
			}
		}

		if(!set_only_one_bit(p, swap[type - 1].flag_idx) ) {
			DEBUG_LOG("HAVE GET [%u]", p->id);
			return send_to_self_error(p, p->waitcmd, -Err_already_get_aword, 1);
		}
	}

	/* 校验圣光兽的数目是否达到领取的数据 */
	if(swap[type - 1].animal_count) {
		if (swap[type -1].animal_count > p->animal_count) {
			return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
		}
	}

	if(swap[type - 1].invitee_cnt) {
		if (swap[type -1].invitee_cnt > p->invitee_cnt) {
			return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
		}
	}

	if(swap[type - 1].time_chk) {
		if(swap[type - 1].time_chk - 5 > p->oltoday + (now.tv_sec - p->stamp)) {
			return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
		}
	}

	if(swap[type - 1].date_start && swap[type - 1].date_end) {
		uint32_t year = get_now_tm()->tm_year + 1900;
		uint32_t month= get_now_tm()->tm_mon + 1;
		uint32_t day = get_now_tm()->tm_mday;
		uint32_t cur_date = year*10000+month*100+day;
		if((cur_date < swap[type - 1].date_start) || (cur_date > swap[type - 1].date_end))
		{
			return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
		}
	}
	
	if (type >= 626 && type <= 633)
	{
		if (!(get_now_tm()->tm_hour >= 20 && get_now_tm()->tm_hour < 21))
		{
			return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
		}

		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B476,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	}

	// 2012 mole sport meet award ceremony
	if(type >= 1344 && type <= 1348)
	{
		if(type == 1344 ){
			if(p->team_id != 1){
				return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
			}
		}
		if(type == 1345){
			if(p->team_id != 2){
				return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
			}
		}
		if(type == 1346){
			uint32_t best_mvp_team[] = {14887893, 466296, 36191804, 37716311, 57606780, 56907061, 175494146, 7247338, 13380726, 62002206};
			uint32_t k = 0;
			for(; k < sizeof(best_mvp_team)/sizeof(uint32_t); ++k){
				if(best_mvp_team[k] == p->mvp_team){
					break;
				}
			}
			if(k >= sizeof(best_mvp_team)/sizeof(uint32_t)){
				return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1 );
			}
		}
		if(type == 1347){
			if(p->team_id != 1){
				return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
			}
		}
		
		if(type == 1348){
			if(p->team_id != 2){
				return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
			}
		}

	}

	if(swap[type - 1].holiday)
	{
		if(!is_holiday(get_now_tm()->tm_wday))
		{
			return send_to_self_error(p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1);
		}
	}
	
	if (type >= 61 && type <= 65) {
		msglog(statistic_logfile, 0x0409C322 + type-61,get_now_tv()->tv_sec, &(p->id), sizeof(p->id));
	}
	else if (type == 114)
	{
		uint32_t msg_buff[2] = {1, p->id};
		msglog(statistic_logfile, 0x04040003,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 115)
	{
		uint32_t msg_buff[2] = {1, p->id};
		msglog(statistic_logfile, 0x04040002,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if( type == 1197){
		uint32_t msg_buff[2] = {1, p->id};
		msglog(statistic_logfile, 0x0409C321,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	}
	else if(type >= 1264 && type <= 1272){
		uint32_t msg_buff[2] = {1, p->id};
		msglog(statistic_logfile, 0x0409C342,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if( type == 1299){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C354,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	}
	if(type == 1434){
		db_exchange_single_item_op(p, 202, 1351338, 1, 0);
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C38D,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	if(type == 1435){
		db_exchange_single_item_op(p, 202, 1351335, 1, 0);
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C38A,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	if(type == 1420){
		db_exchange_single_item_op(p, 202, 1351337, 1, 0);
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C38C,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	if(type == 1416){
		db_exchange_single_item_op(p, 202, 1351344, 1, 0);
	}
	if(type == 1436){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C389,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	if(type == 1577){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C451,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}


	/*如果不在USER库，则先查询*/
	if (swap[type - 1].itemin_cmd_get != 0) {
		return send_request_to_db(swap[type - 1].itemin_cmd_get, p, 0, NULL, p->id);
	}

	/*每天限制*/
	if (swap[type - 1].day_type != 0) {
		return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &swap[type - 1].day_type, p->id);
	}
	else{
		
		return pack_to_db(p, type, mul);
	}
	
}

/* @brief get day_type from SYSARG_DB.t_day 
 */
int exchange_check_if_set_something_done(sprite_t *p, uint32_t id, const char *buf, int len)
{
  CHECK_BODY_LEN(len ,4);
  uint32_t limit_count = 0;
  int i = 0;
  UNPKG_H_UINT32(buf, limit_count, i);
  uint32_t type = 0;
  uint32_t mul = 0;
  unpkg_host_uint32_uint32(p->session, &type, &mul);

  int day_count = 0;
  if (ISVIP(p->flag)) {
	  day_count = swap[type - 1].sl_count;
  }  else {
	  day_count = swap[type - 1].pl_count;
  }
  if(limit_count < day_count){
	  if (mul == 0 && (type == 566 || type == 567)) {//勇者之战 精灵命运轮 转到的值为0，不送东西
		  if (swap[type - 1].limit_type == 0) {			//次数限制
			  db_set_sth_done(p, swap[type  - 1].day_type, day_count, p->id);
		  } else if (swap[type - 1].limit_type == 1) {	//获得物品的数量限制
			  db_set_sth_done_cnt(p, swap[type - 1].day_type, day_count, mul, p->id);
		  }
		  int l = sizeof(protocol_t);
		  PKG_UINT32(msg, type, l);
		  PKG_UINT32(msg, 0, l);
		  init_proto_head(msg, p->waitcmd, l);
		  return send_to_self(p, msg, l, 1);
	  }
	  return pack_to_db(p, type, mul);
  }
  else{
	  DEBUG_LOG("exchange  daycount: %u, limit: count: %u", day_count, limit_count);
	  if(type >= 1461 && type <= 1475 && (type %2 == 1)){
		  return send_to_self_error(p, p->waitcmd,  -ERR_expedion_already_got_state , 1);
	  }
	  else{
		    return send_to_self_error(p, p->waitcmd, -ERR_have_got_day_limit, 1);
	  }
	
  }
}

/* @brief 如果物品不在USER库，查询物品数量的callback函数
 */
int itemin_cmd_get_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t type = 0;
	uint32_t mul = 0;
	unpkg_host_uint32_uint32(p->session, &type, &mul);

	/*数量校验*/
	switch(type) {
		case 7 ... 16:
		{
			uint32_t count = 0;
			unpkg_host_uint32((uint8_t *)(buf + 4), &count);
			if (count < swap[type - 1].in_item[0].count * mul || ((type == 10) && (mul != 1))) {
				return send_to_self_error(p, p->waitcmd, -ERR_not_enough_medal, 1);
			}
			break;
		}
		default:
			ERROR_RETURN(("WRONG TYPE %u", p->id), -1);
	}

	/*每天限制*/
	if (swap[type - 1].day_type != 0) {
		int day_count = 0;
		if (ISVIP(p->flag)) {
			day_count = swap[type - 1].sl_count;
		}  else {
			day_count = swap[type - 1].pl_count;
		}
		return db_set_sth_done(p, swap[type  - 1].day_type, day_count, p->id);
	}
	return pack_to_db(p, type, mul);
}

/* @brief 如果有每天限制，每天限制的callback函数
 */
int swap_day_limit_callback(sprite_t *p)
{
	uint32_t type = 0;
	uint32_t mul = 0;
	unpkg_host_uint32_uint32(p->session, &type, &mul);
	if (mul == 0 && (type == 566 || type == 567)) {//勇者之战 精灵命运轮 转到的值为0，不送东西
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, type, l);
		PKG_UINT32(msg, 0, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}
	return pack_to_db(p, type, mul);
}

/* @brief 返回交换得到的物品
 */
int swap_callback(sprite_t *p)
{
	uint32_t type = 0;
	uint32_t mul = 0;
	unpkg_host_uint32_uint32(p->session, &type, &mul);
	if(swap[type - 1].day_type != 0){
		int day_count = 0;
		if (ISVIP(p->flag)) {
			day_count = swap[type - 1].sl_count;
		}  else {
			day_count = swap[type - 1].pl_count;
		}
		if (swap[type - 1].limit_type == 0) {			//次数限制
			db_set_sth_done(p, swap[type  - 1].day_type, day_count, p->id);
		} else if (swap[type - 1].limit_type == 1) {	//获得物品的数量限制
			db_set_sth_done_cnt(p, swap[type - 1].day_type, day_count, mul, p->id);
		}

	}

	/*扣除用于交换的物品*/
	if (swap[type - 1].itemin_cmd_del != 0) {
		switch (type) {
			case 7 ... 16:
			{
				int count = swap[type - 1].in_item[0].count * mul * -1;
				int32_t db_buf[] = {count, 0};
				send_request_to_db(swap[type - 1].itemin_cmd_del, NULL, sizeof(db_buf), &db_buf, p->id);
				break;
			}
			default:
				ERROR_RETURN(("WRONG TYPE %u %u", p->id, type), -1);
		}
	}

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, type, j);
	PKG_UINT32(msg, swap[type - 1].out_cnt, j);
	int i = 0;
	//for cutepig collect exp and gold added
	for (i = 0; i != swap[type - 1].out_cnt; ++i) {
		PKG_UINT32(msg, swap[type - 1].out_item[i].itm->id, j);
		uint32_t add_cnt = swap[type - 1].out_item[i].count * mul;
		if( swap[type - 1].out_item[i].itm->id == CP_EXP_ITEM_ID )
		{
			cp_add_exp_gold_info_t add_info;
			add_info.p = p;
			add_info.tar_id = p->id;
			add_info.cur_lv = p->cp_level;
			add_info.cur_exp = p->cp_exp;
			add_info.add_exp = &add_cnt;
			add_info.add_gold = 0;
			add_info.is_from_db = 1;
			cutepig_add_exp_gold( &add_info );
		}
		else if( swap[type - 1].out_item[i].itm->id == CP_GOLD_ITEM_ID )
		{
			cp_add_exp_gold_info_t add_info;
			add_info.p = p;
			add_info.tar_id = p->id;
			add_info.cur_lv = p->cp_level;
			add_info.cur_exp = p->cp_exp;
			add_info.add_exp = NULL;
			add_info.add_gold = add_cnt;
			add_info.is_from_db = 1;
			cutepig_add_exp_gold( &add_info );
		}
		PKG_UINT32(msg, add_cnt, j);
	}

	if (type == 721) {	//肥肥抓捕小游戏 肥肥馆荣誉
		send_honor_event_to_db(NULL, CHT_3TIMES_CATCH, 0, p->id);
	}

	if(type >= 19 && type <= 23) {
		uint32_t msg_buff[5] = {0};
		msg_buff[type-19] = 1;
		msglog(statistic_logfile, 0x020D2000,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type >= 49 && type <= 52) {
		uint32_t msg_buff[2] = {1,p->id};
		msglog(statistic_logfile, 0x02103001,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type >= 61 && type <= 66) {
	    if( ISVIP(p->flag)){
		    uint32_t msg_buff[4] = {p->id,1,0,0};
		    msglog(statistic_logfile, 0x0409BF39+type-61,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
		else {
		    uint32_t msg_buff[4] = {0,0,p->id,1};
		    msglog(statistic_logfile, 0x0409BF39+type-61,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
	}
	else if (type >= 169 && type <= 178)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x040413FF + type - 169,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 201 && type <= 206)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x04041409 + type - 201,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 221 && type <= 226)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x04040FFF + type - 221,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 263 && type <= 265)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0405CAC3 + type - 263,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 269 && type <= 275)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0406FAB1 + type - 269,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 286 && type <= 317)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x04041424 + type - 286,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

		uint32_t msgbuff[2] = {p->id, swap[type - 1].in_item[0].count};
		msglog(statistic_logfile, 0x04040C36,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	}
	else if (type >= 338 && type <= 345)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x04041444 + type - 338,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

		uint32_t msgbuff[2] = {p->id, swap[type - 1].in_item[0].count};
		msglog(statistic_logfile, 0x04040C36,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	else if (type >= 321 && type <= 335)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0406FC56 + type - 321,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

		uint32_t msgbuff[2] = {p->id, swap[type - 1].in_item[0].count};
		msglog(statistic_logfile, 0x0406FC54,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	else if (type >= 353 && type <= 356)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0408A339 + type - 353,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 373 && type <= 377)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0408A348, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

		msglog(statistic_logfile, 0x0408A34B + type - 373, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 401 && type <= 402)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0408A348, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		msglog(statistic_logfile, 0x0408A350 + type - 401, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 382 && type <= 400)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0408B3B3 + type - 382,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 407)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0408B34C,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 403 && type <= 406)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0408B3B1,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 421 && type <= 428)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0408B3DF + type - 421, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 439)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BBC8, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 441 && type <= 452)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B442, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 551 && type <= 552)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B40B + type - 551, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 553 && type <= 559)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B410 + type - 553, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 561 && type <= 563)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B40D + type - 561, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 564 && type <= 565)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B440 + type - 564, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 568 || (type >= 583 && type <= 585))
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B448, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 566 || type == 567)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B449, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 574)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B44E, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 579 && type <= 582)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B44F + type - 579, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 586)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B45E, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 587)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B45D, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 588)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B470, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 592 && type <= 593)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B464+type-592, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 604)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B46E, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 606)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B46F, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 594)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		if (p->team_id == 1)
		{
			msglog(statistic_logfile, 0x0409B466, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
		else
		{
			msglog(statistic_logfile, 0x0409B467, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
	}
	else if (type >= 595 && type <= 598)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B468+type-595, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 640 && type <= 648)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B486+type-640, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 649 && type <= 656)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B47D, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 657)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B47E, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 663)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B48F, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 670 && type <= 673)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B494, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 678 && type <= 720)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BD80, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 664 && type <= 669) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x02011382 + type - 664, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 731 && type <= 736) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x02011388 + type - 731, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if( type == 674 )
	{
		//统计 随机宝箱领取
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0409BCFD, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		//通知db采集宝箱，用于任务统计
		cutepig_notice_db_sth_done( p, CSDE_PICK_AWAED );
	}
	else if (type == 744) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BD8C, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 745 && type <= 746) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BD89, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 747 && type <= 748) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BD88, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 749) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BD8A, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 782) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BD8F, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 819) {
		//需要设置状态为1
		uint32_t db_buf[] = {10000, 1};
		send_request_to_db(SVR_PROTO_SET_CONTINUE_LOGIN_VALUE, NULL, sizeof(db_buf), db_buf, p->id);
	}
	else if (type == 820) {
	    uint32_t msg_buff[2] = {p->id, 1};
	    if (ISVIP(p->flag)){
		    msglog(statistic_logfile, 0x0409BDA3, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
		else{
		    msglog(statistic_logfile, 0x0409BDA4, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
	}
	else if (type == 846) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BEED, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if( type >= 847 && type <= 854 )
	{
		//统计美衣制作成功
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0409BDA7, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	else if( type >= 864 && type <= 867 )
	{
		//统计照镜子成功的次数
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0409BDA8, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	else if( type == 868 )
	{
		//统计盖章
		uint32_t msgbuff[6] = { 0, 0, p->id, 1, 0, 0};
		uint32_t* msg_ptr = msgbuff;
		if( ISVIP(p->flag) )
		{
			msg_ptr += 2;
		}
		msglog(statistic_logfile, 0x0409BF03, get_now_tv()->tv_sec, msg_ptr, sizeof(uint32_t) * 4);
	}
	else if( type == 869 )
	{
		//统计盖章
		uint32_t msgbuff[6] = { 0, 0, p->id, 1, 0, 0};
		uint32_t* msg_ptr = msgbuff;
		if( ISVIP(p->flag) )
		{
			msg_ptr += 2;
		}
		msglog(statistic_logfile, 0x0409BF07, get_now_tv()->tv_sec, msg_ptr, sizeof(uint32_t) * 4);
	}
	else if( type == 870 )
	{
		//统计盖章
		uint32_t msgbuff[6] = { 0, 0, p->id, 1, 0, 0};
		uint32_t* msg_ptr = msgbuff;
		if( ISVIP(p->flag) )
		{
			msg_ptr += 2;
		}
		msglog(statistic_logfile, 0x0409BF05, get_now_tv()->tv_sec, msg_ptr, sizeof(uint32_t) * 4);
	}
	else if( type == 871 )
	{
		//统计盖章
		uint32_t msgbuff[6] = { 0, 0, p->id, 1, 0, 0};
		uint32_t* msg_ptr = msgbuff;
		if( ISVIP(p->flag) )
		{
			msg_ptr += 2;
		}
		msglog(statistic_logfile, 0x0409BF04, get_now_tv()->tv_sec, msg_ptr, sizeof(uint32_t) * 4);
	}
	else if( type == 872 )
	{
		//公主兑换券
		uint32_t msgbuff[6] = { 0, 0, p->id, 1, 0, 0};
		uint32_t* msg_ptr = msgbuff;
		if( ISVIP(p->flag) )
		{
			msg_ptr += 2;
		}
		msglog( statistic_logfile, 0x0409BF07, get_now_tv()->tv_sec, msg_ptr, sizeof(uint32_t) * 4);
	}
	else if( type >= 943 && type <= 982 )
	{
		//天使兑换券兑换物品
		uint32_t msgbuff[2] = { p->id, 1 };
		msglog( statistic_logfile, 0x0409BF36, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	else if (type == 1131) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C316, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 1200) {
		//减负书包统计
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C31F, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 1254 && type <= 1263) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C347, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type == 1349) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C369, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 1350 && type <= 1364) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C36A, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (type >= 1380 && type <= 1390) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C36B, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type >= 1394 && type <= 1397){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C370, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type >= 1398 && type <= 1399){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C371, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type >=1400  && type <= 1409 ){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C37F + type -1400 , get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type ==1422){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C391, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type ==1435){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C389, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type ==1436){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C38A, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type >= 1448 && type <= 1451){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x02001054 + (type - 1448), get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type ==1476){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3B9, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type >=1477 && type <=1478){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3BA, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type == 1485){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3E2, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type == 1488){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x04040D6D, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type == 1489){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3D9, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type == 1491){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3E8, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type == 1492){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x04040D6F, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}

	
	if(type >= 1461 && type <= 1475 && (type %2 == 1)){
		uint32_t seq = (type - 1461)/2;
		uint32_t db_buff[] = {38, seq};
		send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, NULL, sizeof(db_buff), db_buff, p->id);

		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3C9 + seq, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	} 
	if(type >= 1460 && type <= 1474 && (type%2 == 0)){
		uint32_t seq = (type-1460)/2;
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3C1 + seq, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	if(type == 1506){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x04040D71, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	if(type == 1507){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x04040D72, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	if(type == 1540){
		uint32_t db_buff[] = {42, 1};
		send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, NULL, sizeof(db_buff), db_buff, p->id);
	}
	if(type == 1542){
		uint32_t db_buff[] = {42, 2};
		send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, NULL, sizeof(db_buff), db_buff, p->id);
	}
	if(type == 1560){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C43F, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

//使用天使战斗礼包
int user_use_gift_package_item(sprite_t* p, uint32_t swap_id)
{
	return pack_to_db(p, swap_id, 1);
}

/*
 * @brief 得到礼包兑换的物品
 */
int get_gift_swap_items(uint32_t swap_id, uint32_t* buff)
{
	int len = 0;
	buff[len++] = swap[swap_id - 1].out_cnt;

	int mul = swap[swap_id - 1].mul_set;
	int i = 0;
	for (i = 0; i != swap[swap_id - 1].out_cnt; ++i) {
		buff[len++] = swap[swap_id - 1].out_item[i].itm->id;
		buff[len++] = swap[swap_id - 1].out_item[i].count * mul;
	}
	return 1;
}
