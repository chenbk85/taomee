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

#ifndef  ARCH_WORK_INC
#define  ARCH_WORK_INC

#define MAX_RECORD	100

struct profession_work_config {
	uint32_t id;
	uint32_t profession;
	uint32_t level;
	uint32_t vip;
	int in_cnt;
	item_unit_t in_item[MAX_EXCHANGE_ITEMS];
	int out_cnt;
	item_unit_t out_item[MAX_EXCHANGE_ITEMS];
};


int load_profession_work(const char *file);

int parse_profession_work_single_item(item_unit_t* iut, int* cnt, xmlNodePtr cur, int flag);

int parse_profession_work_items(struct profession_work_config *record, xmlNodePtr chl);

int do_profession_work_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int do_profession_work_callback(sprite_t *p);

#endif   /* ----- #ifndef ARCH_WORK_INC  ----- */

