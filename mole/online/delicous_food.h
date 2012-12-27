/*
 * =====================================================================================
 *
 *       Filename:  delicous_food.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/26/2010 10:21:54 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#ifndef  DELICOUS_FOOD_INC
#define  DELICOUS_FOOD_INC

typedef struct itm_add_info {
	uint32_t itm_id;
	uint32_t count;
}itm_info;


int tantalize_cat_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int tantalize_cat_set_sth_done_callback(sprite_t *p);

int tantalize_cat_get_something_callbak(sprite_t *p, char *buf, int len);

int get_delicous_food_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_delicous_food_callback(sprite_t* p, uint32_t id, char* buf, int len);

int check_share_id(uint32_t item_id, uint32_t * p_reputation);

int share_delicous_food_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int check_exchange_id(uint32_t item_id, uint32_t * p_reputation);

int exchange_something_with_reputation_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int check_if_have_garge_callback(sprite_t *p, char *buf, int len);

int sub_reputation_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_reputation_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_reputation_callback(sprite_t *p, uint32_t id, char *buf, int len);

int tantalize_cat_check_callback(sprite_t *p, uint32_t id, char *buf, int len);

int share_delicoust_food_callback(sprite_t *p);

int pkg_itm_flag_count_max(sprite_t* p, uint32_t itemid, int count, uint8_t *db_buf, uint32_t *index);

int give_sth(sprite_t *p, struct itm_add_info itm_count[], uint32_t length, uint8_t *db_buf, uint32_t *index);

int give_something_accord_reputation_cmd(sprite_t *p, const uint8_t *body, int bodylen);
#endif   /* ----- #ifndef DELICOUS_FOOD_INC  ----- */

