/*
 * =====================================================================================
 *
 *       Filename:  arch_work.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/06/2010 09:03:21 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#ifndef EXCHANGE_STH_H
#define EXCHANGE_STH_H

typedef struct {
	uint32_t id;
	uint32_t mul_set;
	uint32_t need_vip;
	uint32_t time_chk;
	uint32_t flag_idx;
	int itemin_cmd_get;
	int itemin_cmd_del;
	uint32_t day_type;
	uint32_t pl_count;
	uint32_t sl_count;
	uint32_t date_start;
	uint32_t date_end;
	uint32_t animal_count; /* 圣光兽的数目 */
	uint32_t holiday;
	uint32_t discount;	//折扣，默认为100，只针对超拉
	uint32_t fight_lvl;
	uint32_t mul_start;
	uint32_t mul_end;
	uint32_t vip_level;
	uint32_t limit_type;//0表示次数限制，1表示获得东西的数量限制
	uint32_t random;		//本次交换的执行概率
	uint32_t invitee_cnt; //摩尔大使下线数量
	int in_cnt;
	item_unit_t in_item[MAX_EXCHANGE_ITEMS];
	int out_cnt;
	item_unit_t out_item[MAX_EXCHANGE_ITEMS];
} in_out_item_t;


int load_exchange_conf(const char *file);

int swap_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int swap_callback(sprite_t *p);

int exchange_check_if_set_something_done(sprite_t *p, uint32_t id, const char *buf, int len);

int swap_day_limit_callback(sprite_t *p);

int itemin_cmd_get_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int user_use_gift_package_item(sprite_t* p, uint32_t swap_id);
int get_gift_swap_items(uint32_t swap_id, uint32_t* buff);
#endif   /* ----- #ifndef ARCH_WORK_INC  ----- */

