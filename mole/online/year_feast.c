/*
 * =====================================================================================
 *
 *       Filename:  year_feast.c
 *
 *    Description:  新年盛宴
 *
 *        Version:  1.0
 *        Created:  01/14/2012 11:30:38 AM
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
#include "map.h"
#include "year_feast.h"

feast_info_t g_feast_info;
static year_feast_timer_t  feast_timer;

enum TIMER_HANDLE {
	TH_FEAST = 0,
	TH_COOK,
	TH_CAKE,
};

enum FEAST_MSG_TYPE {
	MSG_TYPE_EAT		= 0x0409C305,
	MSG_TYPE_COOK		= 0x0409C306,
	MSG_TYPE_FIRE		= 0x0409C307,
	MSG_TYPE_WASH		= 0x0409C308,
	MSG_TYPE_CAKE		= 0x0409C309,
};

//100秒的烹饪时间限制，如果在80秒内用户没有操作，将用户踢下烹饪区
#define ONE_TIME_COOK_TIMER			80
//100秒的蛋糕制作时间限制，如果在80秒内用户没有操作，将用户踢下蛋糕制作区
#define ONE_TIME_CAKE_TIMER			80

#define CHECK_NOT_IN_FEAST_STATE(_p)		\
	do { \
		if (g_feast_info.start_state == FEAST_START) {\
			DEBUG_LOG("new year feast not wait");\
			return send_to_self_error((_p), (_p)->waitcmd, -ERR_invalid_session, 1);\
		}\
	} while(0)

#define CHECK_IN_FEAST_STATE(_p)			\
	do {\
		if (g_feast_info.start_state != FEAST_START) {\
			DEBUG_LOG("new year feast not start");\
			return send_to_self_error((_p), (_p)->waitcmd, -ERR_invalid_session, 1);\
		}\
	} while(0)

#ifndef FEAST_TEST_VERSION
//120秒的宴会时间
#define	NEW_YEAR_FEAST_TIMER		120
#define CHECK_IN_YEAR_FEAST_MAP(_p)		\
	do {\
		if (!(_p)->tiles || !IS_YEAR_FEAST_MAP((_p)->tiles->id)) {\
			DEBUG_LOG("user not in year feast map: uid[%u] mapid[%lu]", (_p)->id, ((_p)->tiles->id));\
			return send_to_self_error((_p), (_p)->waitcmd, -ERR_invalid_session, 1);\
		}\
	} while(0)
#else
#define CHECK_IN_YEAR_FEAST_MAP(_p)		do{ }while(0)
#define	NEW_YEAR_FEAST_TIMER			120
#endif

#define YEAR_FEAST_DAY_LIMIT_TYPE	50016
#define MAX_FEAST_GIFT_DAY_LIMIT	30
#define RAND_GET_SPICIAL_FEAST_GIFT	50

int print_all_feast_info_for_test()
{
#ifdef FEAST_TEST_VERSION
	DEBUG_LOG("FEAST INFO1:food_rate[%u] food_cnt[%u] fire_state[%u] fire_userid[%u]",
		g_feast_info.feast_food.food_rate, g_feast_info.feast_food.food_cnt,
		g_feast_info.feast_cook.fire_state, g_feast_info.feast_cook.fire_userid);
	
	DEBUG_LOG("FEAST INFO2:cook_rate[%u] cook_uid[%u %u %u] cook_time[%u %u %u]",
		g_feast_info.feast_cook.cook_rate, g_feast_info.feast_cook.userid_list[0], 
		g_feast_info.feast_cook.userid_list[1], g_feast_info.feast_cook.userid_list[2], 
		g_feast_info.feast_cook.cook_time[0], g_feast_info.feast_cook.cook_time[1], g_feast_info.feast_cook.cook_time[2]);
	
	DEBUG_LOG("FEAST INFO3:cake_rate[%u] cake_uid[%u %u] cake_time[%u %u]",
		g_feast_info.feast_cake.cake_rate, g_feast_info.feast_cake.userid_list[0], 
		g_feast_info.feast_cake.userid_list[1], g_feast_info.feast_cake.cake_time[0], g_feast_info.feast_cake.cake_time[1]);
	
	DEBUG_LOG("FEAST INFO4:start_state[%u] start_time[%u] table_rate[%u] table_uid[%u %u %u %u %u %u %u %u %u %u]",
		g_feast_info.start_state, g_feast_info.start_time, g_feast_info.feast_table.table_rate,
		g_feast_info.feast_table.userid_list[0], g_feast_info.feast_table.userid_list[1],
		g_feast_info.feast_table.userid_list[2], g_feast_info.feast_table.userid_list[3],
		g_feast_info.feast_table.userid_list[4], g_feast_info.feast_table.userid_list[5],
		g_feast_info.feast_table.userid_list[6], g_feast_info.feast_table.userid_list[7],
		g_feast_info.feast_table.userid_list[8], g_feast_info.feast_table.userid_list[9]);
#endif
	return 0;
}

int init_year_feast_timer()
{
	INIT_LIST_HEAD(&feast_timer.timer_list);

#ifdef FEAST_TEST_VERSION
	g_feast_info.feast_food.food_cnt = 100;
	g_feast_info.feast_table.table_rate = 90;
#endif
	return 0;
}

/*
 * @brief （米米号，数量）的统计项
 */
int msglog_format_1(sprite_t* p, uint32_t msg_type)
{
	uint32_t msg_buf[] = {p->id, 1};
	msglog(statistic_logfile, msg_type, get_now_tv()->tv_sec, msg_buf, sizeof(msg_buf));
	return 0;
}
/*
 * @brief 新一轮的宴会开，设置开始时间，开始状态
 */
static int start_feast_timer(uint32_t timer_type)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	switch (timer_type) {
	case TH_FEAST:
		{
			g_feast_info.start_time = cur_time;
			g_feast_info.start_state = FEAST_START;
			DEBUG_LOG("A NEW START YEAR FEAST: start time[%u]", cur_time);
			
			//将所有人T下
			/*memset(&(g_feast_info.feast_cook), 0, sizeof(g_feast_info.feast_cook));
			memset(&(g_feast_info.feast_cake), 0, sizeof(g_feast_info.feast_cake));
			memset(&(g_feast_info.feast_table), 0, sizeof(g_feast_info.feast_table));*/
			year_feast_tell_new_state();

			send_all_feast_info_to_map(NULL, ROT_YEAR_FEAST_START, 0, 0);
			ADD_TIMER_EVENT(&feast_timer, handle_year_feast_time_out, NULL, cur_time + NEW_YEAR_FEAST_TIMER);
		}
		break;
	case TH_COOK:
		{
			ADD_TIMER_EVENT(&feast_timer, handle_one_time_cook_time_out, NULL, cur_time + ONE_TIME_COOK_TIMER);
		}
		break;
	case TH_CAKE:
		{
			ADD_TIMER_EVENT(&feast_timer, handle_one_time_cake_time_out, NULL, cur_time + ONE_TIME_CAKE_TIMER);
		}
		break;
	default:
		break;
	}
	return 0;
}

/*
 * @brief 宴会结束，清空相关信息
 */
int handle_year_feast_time_out(void* onwer, void* data)
{
	memset(&(g_feast_info.feast_cook), 0, sizeof(g_feast_info.feast_cook));
	memset(&(g_feast_info.feast_cake), 0, sizeof(g_feast_info.feast_cake));
	memset(&(g_feast_info.feast_table), 0, sizeof(g_feast_info.feast_table));
	
	DEBUG_LOG("A YEAR FEAST END TIME: end time[%u]", (uint32_t)get_now_tv()->tv_sec);
	print_all_feast_info_for_test();
	g_feast_info.start_time = 0;
	g_feast_info.start_state = FEAST_WAIT;

//	year_feast_tell_new_state();
	send_all_feast_info_to_map(NULL, ROT_YEAR_FEAST_OVER, 0, 0);
	return 0;
}

/*
 * @brief 烹饪区时间限制，如果规定时间内没有操作，将用户踢出
 */
int handle_one_time_cook_time_out(void* onwer, void* data)
{
	int i = 0;
	int flag = 0;
	uint32_t opt_uid =0;
	uint32_t cur_time = get_now_tv()->tv_sec;
	for (i = 0; i < MAX_FEAST_COOK_USER_CNT; i++) {
		if (g_feast_info.feast_cook.userid_list[i] != 0
			&& cur_time > g_feast_info.feast_cook.cook_time[i]
			&& (cur_time - g_feast_info.feast_cook.cook_time[i]) > ONE_TIME_COOK_TIMER) {
			opt_uid = g_feast_info.feast_cook.userid_list[i];
			g_feast_info.feast_cook.userid_list[i] = 0;
			g_feast_info.feast_cook.cook_time[i] = 0;
			flag = 1;
		}
	}
	if (flag == 1) {
		send_all_feast_info_to_map(NULL, ROT_KICK_COOK_USER, opt_uid, 0);		//ROT_KICK_COOK_USER
	}
	DEBUG_LOG("handle_one_time_cook_time_out:");
	print_all_feast_info_for_test();

	return 0;
}

/*
 * @brief 蛋糕区时间限制，如果规定时间内没有操作，将用户踢出
 */
int handle_one_time_cake_time_out(void* onwer, void* data)
{
	int i = 0;
	int flag = 0;
	uint32_t opt_uid =0;
	uint32_t cur_time = get_now_tv()->tv_sec;
	//DEBUG_LOG("handle_one_time_cake_time_out:");
	print_all_feast_info_for_test();
	
	for (i = 0; i < MAX_FEAST_CAKE_USER_CNT; i++) {
		if (g_feast_info.feast_cake.userid_list[i] != 0
			&& cur_time > g_feast_info.feast_cake.cake_time[i]
			&& (cur_time - g_feast_info.feast_cake.cake_time[i]) > ONE_TIME_COOK_TIMER) {
			opt_uid = g_feast_info.feast_cake.userid_list[i];
			g_feast_info.feast_cake.userid_list[i] = 0;
			g_feast_info.feast_cake.cake_time[i] = 0;
			flag = 1;
		}
	}
	if (flag == 1) {
		send_all_feast_info_to_map(NULL, ROT_KICK_CAKE_USER, opt_uid, 0);		//ROT_KICK_CAKE_USER
	}
	DEBUG_LOG("handle_one_time_cake_time_out out:");
	print_all_feast_info_for_test();
	return 0;
}

/*
 * @brief 发送所有宴会信息
 * @param
 */
int send_all_feast_info_to_map(sprite_t* p, uint32_t opt_type, uint32_t opt_uid, int completed)
{
	uint32_t left_time;
	if (g_feast_info.start_state == FEAST_START) {
		left_time = (NEW_YEAR_FEAST_TIMER) - (get_now_tv()->tv_sec - g_feast_info.start_time);
	} else {
		left_time = 0;
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, opt_type, l);
	PKG_UINT32(msg, opt_uid, l);
	PKG_UINT32(msg, left_time, l);
	PKG_UINT32(msg, g_feast_info.feast_food.food_rate, l);
	PKG_UINT32(msg, g_feast_info.feast_food.food_cnt, l);
	PKG_UINT32(msg, g_feast_info.feast_cook.fire_state, l);
	PKG_UINT32(msg, g_feast_info.feast_cook.fire_userid, l);
	PKG_UINT32(msg, g_feast_info.feast_cook.cook_rate, l);

	int i = 0;
	for (i = 0; i < MAX_FEAST_COOK_USER_CNT; i++) {
		PKG_UINT32(msg, g_feast_info.feast_cook.userid_list[i], l);
	}
	for (i = 0; i < MAX_FEAST_CAKE_USER_CNT; i++) {
		PKG_UINT32(msg, g_feast_info.feast_cake.userid_list[i], l);
	}
	PKG_UINT32(msg, g_feast_info.feast_table.table_rate, l);
	for (i = 0; i < MAX_FEAST_TABLE_USER_CNT; i++) {
		PKG_UINT32(msg, g_feast_info.feast_table.userid_list[i], l);
	}
	
	if (p) {
		init_proto_head(msg, p->waitcmd, l);
		send_to_self(p, msg, l, completed);
	} else {
		init_proto_head(msg, PROTO_YEAR_FEAST_GET_ALL_INFO, l);
		send_to_map3(NEW_YEAR_FEAST_MAP, msg, l);
	}

	return 0;
}

/*
 * @brief 新年盛宴:得到所有信息  8356
 */
int year_feast_get_all_info_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_IN_YEAR_FEAST_MAP(p);

	DEBUG_LOG("year_feast_get_all_info_cmd: uid[%u]", p->id);

	return send_all_feast_info_to_map(p, ROT_GET_INFO, p->id, 1);	//ROT_GET_INFO
}

/*
 * @brief 新年盛宴:清洗食物 8357
 */
int year_feast_wash_food_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_IN_YEAR_FEAST_MAP(p);
	
	CHECK_NOT_IN_FEAST_STATE(p);//宴会时间 不能清洗
	int flag = 1;
	g_feast_info.feast_food.food_rate += PRE_FEAST_WASH_FOOD_RATE;
	if (g_feast_info.feast_food.food_rate >= 100) {
		g_feast_info.feast_food.food_rate %= 100;
		if (g_feast_info.feast_food.food_cnt < MAX_FEAST_FOOD_CNT) {
			g_feast_info.feast_food.food_cnt++;
			msglog_format_1(p, MSG_TYPE_WASH);
		} else {	//食材到达上限了
			flag = 0;
		}
	}

	if (flag == 1) {
		send_all_feast_info_to_map(NULL, ROT_CLEAR_FOOD, p->id, 0);	//ROT_CLEAR_FOOD
	}
	
	DEBUG_LOG("year_feast_wash_food_cmd:");
	print_all_feast_info_for_test();
	
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, g_feast_info.feast_food.food_rate, l);
	PKG_UINT32(msg, g_feast_info.feast_food.food_cnt, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 新年盛宴:生火 8358
 */
int year_feast_cook_fire_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t fire_type = 0;
	int i = 0;
	UNPKG_UINT32(buf, fire_type, i);

	//CHECK_NOT_IN_FEAST_STATE(p);//宴会时间 不能生火
	CHECK_IN_YEAR_FEAST_MAP(p);
	print_all_feast_info_for_test();

	if (!ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_vip, 1);
	}

	//DEBUG_LOG("year_feast_cook_fire_cmd: uid[%u] fire[%u] state[%u]", p->id, fire_type, g_feast_info.start_state);
	print_all_feast_info_for_test();
	
	enum FEAST_FIRE_OPT_STATE{
		OPT_FIRE_START = 1, //生火,加入到生火点
		OPT_FIRE_OVER = 2,	//灭火，离开生火点
		OPT_FIRE_ADD = 3,	//完成1次加柴
	};
	enum RET_STATE {
		SUCC = 0,
		FIRE_FAILD = 1,
	};
	uint32_t ret_type = 0;
	uint32_t ret_state = SUCC;
	switch (fire_type) {
	case OPT_FIRE_START:
		ret_type = ROT_ON_COOK_FIRE;
		CHECK_NOT_IN_FEAST_STATE(p);
		if (year_feast_check_userid_in_feast(p->id) != ON_NULL_POS) {
			ret_state = FIRE_FAILD;
		} else {
			if (g_feast_info.feast_cook.fire_state == FFS_FIRE_OFF
				&& g_feast_info.feast_cook.fire_userid == 0) {
				g_feast_info.feast_cook.fire_userid = p->id;
				g_feast_info.feast_cook.fire_state = FFS_FIRE_ON;
				msglog_format_1(p, MSG_TYPE_FIRE);
			} else {
				ret_state = FIRE_FAILD;
			}
		}
		break;

	case OPT_FIRE_ADD:
		ret_type = ROT_ADD_COOK_FIRE;
		CHECK_NOT_IN_FEAST_STATE(p);
		//只有当前用户才能加柴
		if (g_feast_info.feast_cook.fire_state == FFS_FIRE_ON
			&& g_feast_info.feast_cook.fire_userid == p->id) {
		
		} else {
			ret_state = FIRE_FAILD;
		}
		break;

	case OPT_FIRE_OVER:
		ret_type = ROT_OFF_COOK_FIRE;
		//熄灭火焰，清除烹饪区用户
		if (g_feast_info.feast_cook.fire_state == FFS_FIRE_ON
			&& g_feast_info.feast_cook.fire_userid == p->id) {
			g_feast_info.feast_cook.fire_state = FFS_FIRE_OFF;
			year_feast_clear_cook_user();
		} else {
			ret_state = FIRE_FAILD;
		}
		break;

	default:
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	
	if (ret_state == SUCC) {
		send_all_feast_info_to_map(NULL, ret_type, p->id, 0);	//ROT_ON_COOK_FIRE ROT_OFF_COOK_FIRE ROT_ADD_COOK_FIRE
	}

	DEBUG_LOG("year_feast_cook_fire_cmd out: uid[%u] ret_state[%u]", p->id, ret_state);
	print_all_feast_info_for_test();
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 烹饪区的进度增加10%
 */
static int finish_cook_step(uint32_t type)
{
	switch (type) {
	case TH_COOK:
		g_feast_info.feast_cook.cook_rate += PRE_FEAST_COOK_FOOD_RATE;
		if (g_feast_info.feast_cook.cook_rate >= 100) {
			g_feast_info.feast_cook.cook_rate %= 100;
			g_feast_info.feast_table.table_rate += PRE_FEAST_FOOD_TABLE_RATE;
		}
		break;
	
	case TH_CAKE:
		g_feast_info.feast_table.table_rate += PRE_FEAST_FOOD_TABLE_RATE;
		break;
	}

	if (g_feast_info.feast_table.table_rate >= 100) {
		g_feast_info.feast_table.table_rate = 100;
		//memset(g_feast_info.feast_table.userid_list, 0, sizeof(g_feast_info.feast_table.userid_list));
			
		//开始新的一轮的宴会
		start_feast_timer(TH_FEAST);
	}
	DEBUG_LOG("finish_cook_step : type[%u]", type);
	print_all_feast_info_for_test();

	return 0;
}

/*
 * @brief 新年盛宴:烧煮完成菜肴 8359
 */
int year_feast_cook_food_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	int cook_type = 0;
	int cook_pos = 0;
	int i = 0;
	UNPKG_UINT32(buf, cook_type, i);
	UNPKG_UINT32(buf, cook_pos, i);

	CHECK_IN_YEAR_FEAST_MAP(p);

	if (cook_pos > 2) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	enum FEAST_COOK_OPT_STATE{
		OPT_IN_COOK_POS = 1,	//加入到烹饪区的位置
		OPT_OFF_COOK_POS = 2,	//离开
		OPT_FINISH_COOK = 3,	//完成1次烹饪
	};
	enum RET_STATE {
		SUCC = 0,				//成功
		FAILD = 1,				//加入或离开位置失败
		NO_FIRE = 2,			//没火状态
		NO_FOOD = 3,			//没有食材了
		NOT_RIGHT_POS = 4,		//不是该位置的人
	};
	uint32_t ret_state = SUCC;
	if (g_feast_info.feast_cook.fire_state == FFS_FIRE_OFF
		&& cook_type != OPT_OFF_COOK_POS) {
		ret_state = NO_FIRE;
	}
	if (ret_state == SUCC
		&& g_feast_info.feast_food.food_cnt == 0
		&& cook_type != OPT_OFF_COOK_POS) {
		ret_state = NO_FOOD;
	}

	uint32_t ret_type = 0;
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (ret_state == SUCC) {
		switch (cook_type) {
		case OPT_IN_COOK_POS:
			{
				ret_type = ROT_ON_COOK_POS;
				CHECK_NOT_IN_FEAST_STATE(p);
				if (year_feast_check_userid_in_feast(p->id) == ON_NULL_POS) {
					if (g_feast_info.feast_cook.userid_list[cook_pos] != 0) {
						ret_state = FAILD;
					} else {
						g_feast_info.feast_cook.userid_list[cook_pos] = p->id;
						g_feast_info.feast_cook.cook_time[cook_pos] = cur_time;
						start_feast_timer(TH_COOK);
					}
				} else {
					ret_state = FAILD;
				}
			}
			break;

		case OPT_OFF_COOK_POS:
			{
				ret_type = ROT_OFF_COOK_POS;
				if (g_feast_info.feast_cook.userid_list[cook_pos] != p->id) {
					ret_state = FAILD;
				} else {
					g_feast_info.feast_cook.userid_list[cook_pos] = 0;
					g_feast_info.feast_cook.cook_time[cook_pos] = 0;
				}
			}
			break;

		case OPT_FINISH_COOK:
			{
				ret_type = ROT_DO_ONCE_COOK;
				CHECK_NOT_IN_FEAST_STATE(p);
				if (g_feast_info.feast_cook.userid_list[cook_pos] == p->id) {
					g_feast_info.feast_cook.cook_time[cook_pos] = cur_time;
					g_feast_info.feast_food.food_cnt--;
					start_feast_timer(TH_COOK);
					finish_cook_step(TH_COOK);
					msglog_format_1(p, MSG_TYPE_COOK);
				} else {
					ret_state = NOT_RIGHT_POS;
				}
			}
			break;

		default:
			return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
		}
	}

	DEBUG_LOG("year_feast_cook_food_cmd out: uid[%u] type[%u] pos[%u] ret_state[%u]", p->id, cook_type, cook_pos, ret_state);
	print_all_feast_info_for_test();
	if (ret_state == SUCC) {
		send_all_feast_info_to_map(NULL, ret_type, p->id, 0);	//ROT_ON_COOK_POS ROT_OFF_COOK_POS ROT_DO_ONCE_COOK
	}
	
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 新年盛宴:加入蛋糕制作点 8360
 */
int year_feast_set_cake_pos_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t opt_type = 0;
	uint32_t pos = 0;
	int i = 0;
	UNPKG_UINT32(buf, opt_type, i);
	UNPKG_UINT32(buf, pos, i);

	//CHECK_NOT_IN_FEAST_STATE(p);
	CHECK_IN_YEAR_FEAST_MAP(p);
	if (!ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_vip, 1);
	}
	if (pos >= 2) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	//DEBUG_LOG("year_feast_set_cake_pos_cmd: uid[%u] type[%u] pos[%u]", p->id, opt_type, pos);
	print_all_feast_info_for_test();
	enum FEAST_CAKE_OPT_STATE{
		OPT_IN_POS = 1,		//加入蛋糕制作点
		OPT_OFF_POS = 2,	//离开蛋糕制作点
		OPT_FINISH_CAKE = 3,//完成一次蛋糕制作
	};
	enum {
		SUCC = 0,
		FAILD = 1,
	};
	uint32_t ret_state = SUCC;
	uint32_t cur_time = get_now_tv()->tv_sec;
	uint32_t ret_type = 0;
	switch (opt_type) {
	case OPT_IN_POS:
		ret_type = ROT_ON_CAKE_POS;
		CHECK_NOT_IN_FEAST_STATE(p);
		if (year_feast_check_userid_in_feast(p->id) == ON_NULL_POS) {
			if (g_feast_info.feast_cake.userid_list[pos] != 0) {
				ret_state = FAILD;
			} else {
				g_feast_info.feast_cake.userid_list[pos] = p->id;
				g_feast_info.feast_cake.cake_time[pos] = cur_time;
				start_feast_timer(TH_CAKE);
			}
		} else {
			ret_state = FAILD;
		}
		break;

	case OPT_OFF_POS:
		ret_type = ROT_OFF_CAKE_POS;
		if (g_feast_info.feast_cake.userid_list[pos] == p->id) {
			g_feast_info.feast_cake.userid_list[pos] = 0;
			g_feast_info.feast_cake.cake_time[pos] = 0;
		} else {
			ret_state = FAILD;
		}
		break;

	case OPT_FINISH_CAKE:
		ret_type = ROT_DO_ONCE_CAKE;
		CHECK_NOT_IN_FEAST_STATE(p);
		if (g_feast_info.feast_cake.userid_list[pos] == p->id && g_feast_info.feast_cake.userid_list[!pos] != 0) {
			g_feast_info.feast_cake.cake_time[pos] = cur_time;
			start_feast_timer(TH_CAKE);
			finish_cook_step(TH_CAKE);
			year_feast_clear_cake_user();
			msglog_format_1(p, MSG_TYPE_CAKE);
		} else {
			ret_state = FAILD;
		}
		break;

	default:
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	
	DEBUG_LOG("year_feast_set_cake_pos_cmd out: uid[%u] type[%u] pos[%u] ret_state[%u]", p->id, opt_type, pos, ret_state);
	print_all_feast_info_for_test();

	if (ret_state == SUCC) {
		send_all_feast_info_to_map(NULL, ret_type, p->id, 0);	//ROT_ON_CAKE_POS ROT_OFF_CAKE_POS ROT_DO_ONCE_CAKE
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
	send_to_self(p, msg, l, 1);
	return 0;
}

/*
 * @brief 新年盛宴:加入宴会桌 8361
 */
int year_feast_set_table_pos_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_IN_FEAST_STATE(p);
	CHECK_IN_YEAR_FEAST_MAP(p);

	uint32_t opt_type = 0;
	uint32_t table_pos = 0;
	int i = 0;
	UNPKG_UINT32(buf, opt_type, i);
	UNPKG_UINT32(buf, table_pos, i);

	if (table_pos >= 10) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	enum FEAST_CAKE_OPT_STATE{
		OPT_IN_POS = 1,		//加入宴会桌
		OPT_OFF_POS = 2,	//离开宴会桌
	};
	enum {
		SUCC = 0,
		FAILD = 1,
	};

	//DEBUG_LOG("year_feast_set_table_pos_cmd: uid[%u] type[%u] pos[%u]", p->id, opt_type, table_pos);
	print_all_feast_info_for_test();
	uint32_t ret_type = 0;
	uint32_t ret_state = SUCC;
	switch (opt_type) {
	case OPT_IN_POS:
		ret_type = ROT_ON_TABLE_POS;
		if (year_feast_check_userid_in_feast(p->id) != ON_TABLE_POS
			&& g_feast_info.feast_table.userid_list[table_pos] == 0) {
			g_feast_info.feast_table.userid_list[table_pos] = p->id;
		} else {
			ret_state = FAILD;
		}
		break;

	case OPT_OFF_POS:
		ret_type = ROT_OFF_TABLE_POS;
		if (g_feast_info.feast_table.userid_list[table_pos] == p->id) {
			g_feast_info.feast_table.userid_list[table_pos] = 0;
		} else {
			ret_state = FAILD;
		}
		break;

	default:
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	DEBUG_LOG("year_feast_set_table_pos_cmd out: uid[%u] type[%u] pos[%u] ret_state[%u]", p->id, opt_type, table_pos, ret_state);
	print_all_feast_info_for_test();
	if (ret_state == SUCC) {
		send_all_feast_info_to_map(NULL, ret_type, p->id, 0);		//ROT_ON_TABLE_POS	ROT_OFF_TABLE_POS
	}
	
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 通知前端新一轮的宴会开始 8363
 */
int year_feast_tell_new_state()
{
	DEBUG_LOG("year_feast_tell_new_start: start_time[%u]", g_feast_info.start_time);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, g_feast_info.start_state, l);
	init_proto_head(msg, PROTO_YEAR_FEAST_NEW_START, l);
	send_to_map3(NEW_YEAR_FEAST_MAP, msg, l);
	return 0;
}

/*
 * @brief 新年盛宴:领取奖励
 */
int year_feast_get_prize_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_IN_YEAR_FEAST_MAP(p);

	uint32_t start_time = g_feast_info.start_time;
	return send_request_to_db(SVR_PROTO_YEAR_FEAST_GET_PRIZE, p, sizeof(start_time), &start_time, p->id);
}

enum FEAST_PRIZE_RET {
	RET_SUCC = 0,
	RET_HAS_GET = 1,
	RET_DAY_LIMIT = 2,
};

static int send_year_feast_prize_to_self(sprite_t* p, uint32_t ret_type)
{
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_type, l);
	PKG_UINT32(msg, 0, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int year_feast_get_prize_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t ret_state;
	}__attribute__((packed)) feast_get_prize_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(feast_get_prize_cbk_pack_t));
	feast_get_prize_cbk_pack_t* cbk_pack = (feast_get_prize_cbk_pack_t*)buf;

	if (cbk_pack->ret_state == 0) {//已经领取过了
		return send_year_feast_prize_to_self(p, RET_HAS_GET);
	}
	//没有领取
	const uint32_t day_type = YEAR_FEAST_DAY_LIMIT_TYPE;
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &day_type, p->id);
}

int year_feast_check_day_limit_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t day_cnt = *(uint32_t*)buf;
	if (day_cnt >= MAX_FEAST_GIFT_DAY_LIMIT) {
		return send_year_feast_prize_to_self(p, RET_DAY_LIMIT);
	}
	
	//清掉占座的信息
	int i = 0;
	for (i = 0; i < MAX_FEAST_TABLE_USER_CNT; i++) {
		if (p->id == g_feast_info.feast_table.userid_list[i]) {
			g_feast_info.feast_table.userid_list[i] = 0;
			send_all_feast_info_to_map(NULL, ROT_OFF_TABLE_POS, p->id, 0);
			break;
		}
	}
	
	const uint32_t gift_id = 190909;
	const uint32_t gift_cnt = 5;
	const uint32_t gift_list[] = {160885, 160886, 160887, 1220132, 1220133, 1220134, 1220135, 1220161, 1230087, 1220126};
	uint32_t rand_idx = rand() % (sizeof(gift_list) / sizeof(gift_list[0]));
	
	const uint32_t more_gift_list[] = {1351292, 1351293, 1351296, 1351297};
	uint32_t more_rand_idx = 0;
	uint32_t Count = 2;
	uint32_t rand_range = rand() % 100;
	if (rand_range < RAND_GET_SPICIAL_FEAST_GIFT) {
		Count = 3;
		more_rand_idx = rand() % (sizeof(more_gift_list) / sizeof(more_gift_list[0]));
	}
	
	uint8_t db_buf[256] = {0};
	int j = 0;
	PKG_H_UINT32(db_buf, 0, j);
	PKG_H_UINT32(db_buf, Count, j);
	PKG_H_UINT32(db_buf, 0, j);
	PKG_H_UINT32(db_buf, 0, j);

	pkg_item_kind(p, db_buf, gift_id, &j);
	PKG_H_UINT32(db_buf, gift_id, j);
	PKG_H_UINT32(db_buf, gift_cnt, j);
	item_t* item_u1 = get_item_prop(gift_id);
	PKG_H_UINT32(db_buf, item_u1->max, j);

	pkg_item_kind(p, db_buf, gift_list[rand_idx], &j);
	PKG_H_UINT32(db_buf, gift_list[rand_idx], j);
	PKG_H_UINT32(db_buf, 1, j);
	item_t* item_u2 = get_item_prop(gift_list[rand_idx]);
	PKG_H_UINT32(db_buf, item_u2->max, j);

	if (Count == 3) {
		pkg_item_kind(p, db_buf, more_gift_list[more_rand_idx], &j);
		PKG_H_UINT32(db_buf, more_gift_list[more_rand_idx], j);
		PKG_H_UINT32(db_buf, 1, j);
		PKG_H_UINT32(db_buf, 99999, j);
	}
	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, db_buf, p->id);
	
	uint32_t day_buf[] = {YEAR_FEAST_DAY_LIMIT_TYPE, MAX_FEAST_GIFT_DAY_LIMIT, 1};
    send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
	msglog_format_1(p, MSG_TYPE_EAT);
	
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, 0, l);
	PKG_UINT32(msg, Count, l);
	PKG_UINT32(msg, gift_id, l);
	PKG_UINT32(msg, gift_cnt, l);
	PKG_UINT32(msg, gift_list[rand_idx], l);
	PKG_UINT32(msg, 1, l);
	if (Count == 3) {
		PKG_UINT32(msg, more_gift_list[more_rand_idx], l);
		PKG_UINT32(msg, 1, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 用户离线时，踢出用户
 */
int year_feast_kill_user(sprite_t *p)
{
	int i = 0;
	uint32_t flag = 0;
	if (p->id == g_feast_info.feast_cook.fire_userid) {
		g_feast_info.feast_cook.fire_state = FFS_FIRE_OFF;
		year_feast_clear_cook_user();
		
		flag = 1;
	}

	for (i = 0; i < MAX_FEAST_COOK_USER_CNT; i++) {
		if (p->id == g_feast_info.feast_cook.userid_list[i]) {
			g_feast_info.feast_cook.userid_list[i] = 0;
			g_feast_info.feast_cook.cook_time[i] = 0;
			flag = 1;
		}
	}

	for (i = 0; i < MAX_FEAST_CAKE_USER_CNT; i++) {
		if (p->id == g_feast_info.feast_cake.userid_list[i]) {
			g_feast_info.feast_cake.userid_list[i] = 0;
			g_feast_info.feast_cake.cake_time[i] = 0;
			flag = 1;
		}
	}

	for (i = 0; i < MAX_FEAST_TABLE_USER_CNT; i++) {
		if (p->id == g_feast_info.feast_table.userid_list[i]) {
			g_feast_info.feast_table.userid_list[i] = 0;
			flag = 1;
		}
	}

	if (flag == 1) {
		send_all_feast_info_to_map(NULL, ROT_KICK_OFFLINE_USER, p->id, 0);		//ROT_KICK_OFFLINE_USER
	}
	return 0;
}

/*
 * @brief 判断userid是否在已存在 防外挂
 */
int year_feast_check_userid_in_feast(uint32_t userid)
{
	int i = 0;
	if (userid == g_feast_info.feast_cook.fire_userid) {
		return ON_FIRE_POS;
	}
	for (i = 0; i < MAX_FEAST_COOK_USER_CNT; i++) {
		if (userid == g_feast_info.feast_cook.userid_list[i]) {
			return ON_COOK_POS;
		}
	}
	for (i = 0; i < MAX_FEAST_CAKE_USER_CNT; i++) {
		if (userid == g_feast_info.feast_cake.userid_list[i]) {
			return ON_CAKE_POS;
		}
	}
	for (i = 0; i < MAX_FEAST_TABLE_USER_CNT; i++) {
		if (userid == g_feast_info.feast_table.userid_list[i]) {
			return ON_TABLE_POS;
		}
	}

	return ON_NULL_POS;
}

//火灭时，将烹饪区的人踢下
int year_feast_clear_cook_user()
{
	if (g_feast_info.feast_cook.fire_state == FFS_FIRE_OFF) {
		g_feast_info.feast_cook.fire_userid = 0;
		memset((char*)(g_feast_info.feast_cook.userid_list), 0, sizeof(g_feast_info.feast_cook.userid_list));
		memset((char*)(g_feast_info.feast_cook.cook_time), 0, sizeof(g_feast_info.feast_cook.cook_time));
	}
	return 0;
}

//火灭时，将烹饪区的人踢下
int year_feast_clear_cake_user()
{
	memset(g_feast_info.feast_cake.userid_list, 0, sizeof(g_feast_info.feast_cake.userid_list));
	memset(g_feast_info.feast_cake.cake_time, 0, sizeof(g_feast_info.feast_cake.cake_time));
	return 0;
}
