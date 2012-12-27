/*
 * =====================================================================================
 *
 *       Filename:  chris_cake.h
 *
 *    Description: 圣诞蛋糕坊之庄园宅急送
 *
 *        Version:  1.0
 *        Created:  12/28/2010 10:37:05 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#ifndef  CHRIS_CAKE_INC
#define  CHRIS_CAKE_INC

typedef struct {
	uint32_t		weekid;
	uint32_t		item_count;
	item_unit_t  	item[15];
}__attribute__(( packed )) weekend_bonus_t;


int get_chris_cake_info_cmd(sprite_t * p,const uint8_t * body,int len);

int get_chris_cake_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

int end_make_one_cake_cmd(sprite_t * p,const uint8_t * body,int len);

int give_cake_to_npc_cmd(sprite_t * p,const uint8_t * body,int len);

int get_chris_cake_score_cmd(sprite_t * p,const uint8_t * body,int len);

int get_chris_cake_score_callback(sprite_t* p, uint32_t id, char* buf, int len);

int exchange_sth_by_cake_score_callback(sprite_t* p, uint32_t id, char* buf, int len);

int give_cake_to_npc_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_continue_login_bonus_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_continue_login_bonus_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_30_day_login_bonus_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_weekend_login_bonus_cmd(sprite_t * p,const uint8_t * body,int len);
int get_weekend_login_bonus(sprite_t * p);
int get_weekend_continue_login_count_cmd(sprite_t * p,const uint8_t * body,int len);
int get_weekend_continue_login_count_callback(sprite_t* p, uint32_t id, char* buf, int len);
int load_weekend_bonus(const char *file);

int get_another_weekend_login_info_cmd(sprite_t * p,const uint8_t * body,int len);
int get_another_weekend_login_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_another_weekend_login_bonus_cmd(sprite_t * p,const uint8_t * body,int len);


#endif   /* ----- #ifndef CHRIS_CAKE_INC  ----- */


