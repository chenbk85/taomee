/*
 * =====================================================================================
 *
 *       Filename:  lucky_lottery.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/26/2011 11:27:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alexhe, alexhe@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "item.h"
#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "message.h"
#include "lucky_lottery.h"
#include "fight_prize.h"

#ifndef TW_VER
const uint32_t g_daily_ticket[] = {39, 168, 347, 489, 259, 79, 237, 57};
static uint32_t g_ticket_prize[][9] = {
	{20110930, 1, 1, 17013, 5},
	{20110930, 2, 1, 1270071, 2},
	{20110930, 3, 1, 1593016, 1},
	{20111001, 1, 1, 17017, 5},
	{20111001, 2, 1, 1270059, 2},
	{20111001, 3, 1, 1593020, 1},
	{20111002, 1, 1, 1230032, 5},
	{20111002, 2, 1, 1270067, 2},
	{20111002, 3, 1, 1593014, 1},
	{20111003, 1, 1, 1230056, 5},
	{20111003, 2, 1, 1270068, 2},
	{20111003, 3, 1, 1593021, 1},
	{20111004, 1, 1, 17009, 5},
	{20111004, 2, 1, 1270040, 2},
	{20111004, 3, 1, 1593015, 1},
	{20111005, 1, 1, 1613100, 5},
	{20111005, 2, 1, 1353289, 2},
	{20111005, 3, 1, 1270069, 1},
	{20111006, 1, 1, 1613102, 5},
	{20111006, 2, 1, 1353282, 2},
	{20111006, 3, 1, 1593017, 1},
	{20111007, 1, 1, 1353259, 2},
	{20111007, 2, 1, 1353293, 1},
	{20111007, 3, 1, 1593019, 1}
};
#else
const uint32_t g_daily_ticket[] = {39, 168, 347, 489, 259, 79, 237};
static uint32_t g_ticket_prize[][9] = {
	{20111014, 1, 1, 17013, 5},
	{20111014, 2, 1, 1270071, 2},
	{20111014, 3, 1, 1593016, 1},
	{20111015, 1, 1, 17017, 5},
	{20111015, 2, 1, 1270059, 2},
	{20111015, 3, 1, 1593020, 1},
	{20111016, 1, 1, 1230032, 5},
	{20111016, 2, 1, 1270067, 2},
	{20111016, 3, 1, 1593014, 1},
	{20111017, 1, 1, 1230056, 5},
	{20111017, 2, 1, 1270068, 2},
	{20111017, 3, 1, 1593021, 1},
	{20111018, 1, 1, 17009, 5},
	{20111018, 2, 1, 1270040, 2},
	{20111018, 3, 1, 1593015, 1},
	{20111019, 1, 1, 1613100, 5},
	{20111019, 2, 1, 1353289, 2},
	{20111019, 3, 1, 1270069, 1},
	{20111020, 1, 1, 1613102, 5},
	{20111020, 2, 1, 1353282, 2},
	{20111020, 3, 1, 1593017, 1}
};
#endif

int cmp1(const void *v1, const void *v2)
{
	return *(uint32_t*)v1 - *(uint32_t*)v2;
}

/*
 * @brief :得到今天领到的彩票的期数
 */
static uint32_t get_ticket_date()
{
	uint32_t date = get_today();

	if (get_now_tm()->tm_hour >= 20) {
#ifndef TW_VER
		if (date == 20110930) {
			date = 20111001;
		} else  {
			date += 1;
		}
#else
		date += 1;
#endif
	}

#ifndef TW_VER
	if (date > 20111007) {
#else
	if (date > 20111020) {
#endif
		return 0;
	}
	return date;
}

/*
 * @brief: 根据期数得到中奖号码
 */
static uint32_t get_daily_ticket_from_date(uint32_t date)
{
#ifndef TW_VER
	if (date == 20110930) {
		return g_daily_ticket[0];
	} else {
		return g_daily_ticket[date - 20111001 + 1];
	}
#else
	return g_daily_ticket[date - 20111014];
#endif
}

static uint32_t get_rand_ticket(sprite_t *p, uint32_t date, uint32_t *ticket, uint32_t *prize_level)
{
	const uint32_t plist[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	uint32_t ticket_cnt = 3;
	if (ISVIP(p->flag)) {
		ticket_cnt = 5;
	}
	uint32_t daily_ticket = get_daily_ticket_from_date(date);

	uint32_t value[5] = {0};
	rand_some_prize_in_array(value, plist, 10, ticket_cnt);
	qsort(value, 5, sizeof(uint32_t), cmp1);
	*ticket =  value[0] * 10000 + value[1] * 1000 + value[2] * 100 +  value[3] * 10 + value[4];

	DEBUG_LOG("get_rand_ticket: 1[%u] 2[%u] 3[%u]", (daily_ticket / 100), (daily_ticket % 100 / 10), (daily_ticket % 10));
	*prize_level = 0;
	int i = (ticket_cnt == 5) ? 0: 2;
	for (; i < 5; i++) {
		if (value[i] == daily_ticket / 100) {
			(*prize_level)++;
		} else if (value[i] == daily_ticket % 100 / 10) {
			(*prize_level)++;
		} else if (value[i] == daily_ticket % 10) {
			(*prize_level)++;
		}
	}
	return 0;
}

/*
 * @brief 领彩蛋 4103
 */
int user_get_lucky_ticket_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);
	uint32_t date = get_ticket_date();
	if (date == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_get_ticket_time_err, 1);
	}
	*(uint32_t*)(p->session) = date;
	return send_request_to_db(SVR_PROTO_USER_CHECK_GET_TICKET, p, 4, &date, p->id);
}

/*
 * @brief check本轮有没有领过彩蛋callback
 */
int user_check_get_ticket_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(check_ticket_ckb_pack_t));
	check_ticket_ckb_pack_t* cbk_pack = (check_ticket_ckb_pack_t*)buf;

	switch (p->waitcmd) {
	case PROTO_USER_GET_LUCKY_TICKET:
		{
			if (cbk_pack->ticket != 0) {
				return send_to_self_error(p, p->waitcmd, -ERR_have_got_ticket_err, 1);
			}
			get_lucky_ticket_package_t package = {0};
			package.date = *(uint32_t*)(p->session);

			get_rand_ticket(p, package.date, &(package.ticket), &(package.prize_level));
			DEBUG_LOG("user get ticket: uid[%u] ticket[%u] date[%u] level[%u]", p->id, package.ticket, package.date, package.prize_level);
	
			memcpy(p->session, &package, sizeof(package));
			return send_request_to_db(SVR_PROTO_USER_GET_LUCKY_TICKET, p, sizeof(package), &package, p->id);
		}
	case PROTO_USER_LOOK_TODAY_TICKET:
		return user_look_today_prize_ticket_callback(p, id, buf, len);

	default:
		break;
	}

	return 0;
}

/*
 * @breif 领彩蛋callback
 */
int user_get_lucky_ticket_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	
	get_lucky_ticket_package_t* pack = (get_lucky_ticket_package_t*)p->session;
	
	uint32_t msgbuf[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409BD82, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, pack->date, l);
	PKG_UINT32(msg, pack->ticket, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

static int get_ticket_prize_from_date(uint32_t date, uint32_t prize_level)
{
	int i = 0;
	for (i = 0; i < sizeof(g_ticket_prize) / sizeof(g_ticket_prize[0]); i++) {
		if (date == g_ticket_prize[i][0] && prize_level == g_ticket_prize[i][1]) {
			break;
		}
	}
	if (i == sizeof(g_ticket_prize) / sizeof(g_ticket_prize[0])) {
		return -1;
	}

	return i;
}

/*
 * @brief 兑换彩票 4104
 */
int user_swap_ticket_prize_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t date = 0;
	int i = 0;
	UNPKG_UINT32(buf, date, i);
#ifndef TW_VER
	if (date > get_today() || date < 20110930 || (date > 20110930 && date <20111001) || (date > 20111007)) {
#else
	if (date > get_today() || date < 20111014 || date > 20111020) {
#endif
		return send_to_self_error(p, p->waitcmd, -ERR_get_ticket_time_err, 1);
	}

	if (date == get_today() && get_now_tm()->tm_hour < 20) {
		return send_to_self_error(p, p->waitcmd, -ERR_get_ticket_time_err, 1);
	}

	*(uint32_t*)p->session = date;
	return send_request_to_db(SVR_PROTO_USER_SWAP_TICKET_PRIZE, p, 4, &date, p->id);
}

int user_swap_ticket_prize_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t is_get;
		uint32_t prize_level;
	}__attribute__((packed)) swap_prize_cbk_t;
	CHECK_BODY_LEN(len, sizeof(swap_prize_cbk_t));
	swap_prize_cbk_t* cbk_pack = (swap_prize_cbk_t*)buf;

	if (cbk_pack->is_get == 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_have_got_ticket_err, 1);
	}
	
	uint32_t date = *(uint32_t*)p->session;
	int index = get_ticket_prize_from_date(date, cbk_pack->prize_level);
	
	//DEBUG_LOG("user_swap_ticket_prize: uid[%u] prize_level[%u]", p->id, cbk_pack->prize_level);
	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->prize_level, l);
	if (index != -1) {
		uint8_t db_buf[pagesize];
		int j = 16;
		int i = 0;
		PKG_UINT32(msg, g_ticket_prize[index][2], l);
		DEBUG_LOG("user_swap_ticket_prize: uid[%u] prize_level[%u] [%u] [%u] [%u]", p->id, cbk_pack->prize_level, g_ticket_prize[index][2],
			g_ticket_prize[index][2+2*i+1],g_ticket_prize[index][2+2*i+2]);
		for (i = 0; i < g_ticket_prize[index][2]; i++) {
			PKG_UINT32(msg, g_ticket_prize[index][2+2*i+1], l);
			PKG_UINT32(msg, g_ticket_prize[index][2+2*i+2], l);
		
			item_t *itm = get_item_prop(g_ticket_prize[index][2+2*i+1]);
			if (!itm) {
				ERROR_LOG("item error!");
				return -1;
			}
			if (pkg_item_kind(p, db_buf, itm->id, &j) == -1) {
				return -1;
			}
			PKG_H_UINT32(db_buf, itm->id, j);
			PKG_H_UINT32(db_buf, g_ticket_prize[index][2+2*i+2], j);
			PKG_H_UINT32(db_buf, itm->max, j);
		}
		int k = 0;
		PKG_H_UINT32(db_buf, 0, k);
		PKG_H_UINT32(db_buf, g_ticket_prize[index][2], k);
		PKG_H_UINT32(db_buf, 0, k);
		PKG_H_UINT32(db_buf, 0, k);
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, db_buf, p->id);
	
		uint32_t msgbuf[4] = {0};
		if (ISVIP(p->flag)) {
			msgbuf[0] = p->id;
			msgbuf[1] = 1;
		} else {
			msgbuf[2] = p->id;
			msgbuf[3] = 1;
		}
		msglog(statistic_logfile, 0x0409BD83 + cbk_pack->prize_level - 1, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 查看今日中奖号码及自己是否领过彩蛋 4105
 */
int user_look_today_prize_ticket_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	uint32_t today = get_today();
	return send_request_to_db(SVR_PROTO_USER_CHECK_GET_TICKET, p, 4, &today, p->id);
}

int user_look_today_prize_ticket_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t ticket;
	if (get_now_tm()->tm_hour < 20) {	//没到开奖时间
		ticket = 0;
	} else {	//过了开奖时间
		ticket = get_daily_ticket_from_date(get_today());
	}

	check_ticket_ckb_pack_t* cbk_pack = (check_ticket_ckb_pack_t*)buf;
	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, ticket, l);
	PKG_UINT32(msg, cbk_pack->ticket, l);
	PKG_UINT32(msg, cbk_pack->prize_level, l);
	PKG_UINT32(msg, cbk_pack->is_get, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 查看历史中奖号码 4106
 */
int user_look_history_prize_ticket_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_USER_GET_HISTORY_TICKET, p, 0, NULL, p->id);
}

int user_look_history_prize_ticket_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t count;
	}__attribute__((packed)) history_ticket_cbk_head_t;
	typedef struct {
		uint32_t ticket;
		uint32_t date;
		uint32_t prize_level;
		uint32_t is_get;
	}__attribute__((packed)) history_ticket_cbk_pack_t;

	CHECK_BODY_LEN_GE(len, sizeof(history_ticket_cbk_head_t));
	history_ticket_cbk_head_t* cbk_head = (history_ticket_cbk_head_t*)buf;
	CHECK_BODY_LEN(len, sizeof(history_ticket_cbk_head_t) + cbk_head->count * sizeof(history_ticket_cbk_pack_t));
	history_ticket_cbk_pack_t* p_cbk_pack = (history_ticket_cbk_pack_t*)(buf + sizeof(history_ticket_cbk_head_t));

	uint32_t l = sizeof(protocol_t);
	len = l;
	l += 4;
	
	uint32_t count = 0;
	uint32_t today = get_today();

	int date;

#ifndef TW_VER
	for (date = 20111001; date < today && date <= 20111007; date ++) {
#else
	for (date = 20111014; date < today && date <= 20111020; date++) {
#endif
		uint32_t ticket = get_daily_ticket_from_date(date);
		PKG_UINT32(msg, ticket, l);
		PKG_UINT32(msg, date, l);
		int i = 0;
		for (i = 0; i < cbk_head->count; i++) {
			if ((p_cbk_pack + i)->date == date) {
				PKG_UINT32(msg, (p_cbk_pack + i)->ticket, l);
				PKG_UINT32(msg, (p_cbk_pack + i)->prize_level, l);
				PKG_UINT32(msg, (p_cbk_pack + i)->is_get, l);
				break;
			}
		}
		if (i == cbk_head->count) {
			PKG_UINT32(msg, 0, l);
			PKG_UINT32(msg, 0, l);
			PKG_UINT32(msg, 0, l);
		}
		count++;
	}
	PKG_UINT32(msg, count, len);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}



/*
 * @brief 国庆7天连续登陆，查询登陆情况 4108
 */
int user_get_national_day_login_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_USER_NATIONAL_DAY_LOGIN, p, 0, NULL, p->id);
}

int user_get_national_day_login_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t login_val;
	}__attribute__((packed)) national_day_login_cbk_pack;
	CHECK_BODY_LEN(len, sizeof(national_day_login_cbk_pack));
	national_day_login_cbk_pack* cbk_pack = (national_day_login_cbk_pack*)buf;

	uint32_t day_cnt = cbk_pack->login_val >> 16;	//前16位表示连续登陆的天数
	
	DEBUG_LOG("get national day: uid[%u], day_cnt[%u], login_val[%u]", p->id, day_cnt, cbk_pack->login_val);
	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, day_cnt, l);
	len = l;
	l += 4;
	uint32_t count = 0;
	int i = 0;
	for (i = 0; i < day_cnt && i < 8; i++) {		//后16位表示领取的标志
		if (cbk_pack->login_val & (1 << i)) {
		//	DEBUG_LOG("get login_val: uid[%u], i[%u], count[%u]", p->id, i, count);
			count ++;
			PKG_UINT32(msg, i + 1, l);
		}
	}
	PKG_UINT32(msg, count, len);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief Online 协议：国庆7天连续登陆，领取礼包 4109
 */
int user_get_national_day_gift_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t gift_val = 0;
	int i = 0;
	UNPKG_UINT32(buf, gift_val, i);

	if (gift_val == 0 || gift_val > 7) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	*(uint32_t*)p->session = gift_val;
	return send_request_to_db(SVR_PROTO_USER_GET_NATIONAL_DAY_GIFT, p, 4, &gift_val, p->id);
}

int user_get_national_day_gift_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t state;
	}__attribute__((packed))national_day_gift_cbk_pack;
	CHECK_BODY_LEN(len, sizeof(national_day_gift_cbk_pack));
	
	national_day_gift_cbk_pack* cbk_pack = (national_day_gift_cbk_pack*)buf;
	if (cbk_pack->state == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_have_got_ticket_err, 1);
	}

	const uint32_t national_day_gift[][9] = {
		{3, 1613100, 3, 1613114, 3, 0, 500},
		{3, 1613102, 3, 1613108, 3, 0, 600},
		{3, 1613109, 3, 1613111, 3, 0, 700},
		{3, 1230034, 1, 1230028, 1, 0, 800},
		{3, 1270063, 1, 1270055, 1, 0, 900},
		{3, 160084, 1, 160225, 1, 0, 1000},
#ifndef TW_VER
		{4, 1593006, 1, 1353267, 1, 160412, 1, 0, 5000}
#else
		{3, 1593006, 1, 1353267, 1, 0, 5000}
#endif
	};
	uint32_t gift_val = *(uint32_t*)p->session;
	uint32_t l = sizeof(protocol_t);
	PKG_UINT32(msg, national_day_gift[gift_val-1][0], l);

	uint8_t db_buf[pagesize];
	int j = 16;
	int i = 0;
	for (i = 0; i < national_day_gift[gift_val-1][0]; i++) {
		PKG_UINT32(msg, national_day_gift[gift_val-1][2*i+1], l);
		PKG_UINT32(msg, national_day_gift[gift_val-1][2*i+2], l);
		
		item_t *itm = get_item_prop(national_day_gift[gift_val-1][2*i+1]);
		if (!itm) {
			ERROR_LOG("item error!");
			return -1;
		}
		if (pkg_item_kind(p, db_buf, itm->id, &j) == -1) {
			return -1;
		}
		PKG_H_UINT32(db_buf, itm->id, j);
		PKG_H_UINT32(db_buf, national_day_gift[gift_val-1][2*i+2], j);
		PKG_H_UINT32(db_buf, itm->max, j);
	}
	int k = 0;
	PKG_H_UINT32(db_buf, 0, k);
	PKG_H_UINT32(db_buf, national_day_gift[gift_val-1][0], k);
	PKG_H_UINT32(db_buf, 0, k);
	PKG_H_UINT32(db_buf, 0, k);
	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, db_buf, p->id);

	uint32_t msgbuf[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409BD81, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}
