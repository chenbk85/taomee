/*
 * =====================================================================================
 *
 *       Filename:  fire_cup.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/26/2010 05:14:09 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#ifndef  FIRE_CUP_INC
#define  FIRE_CUP_INC

#define WVS_TEAM_ID_BASE		1000
#define AC_TEAM_ID_BASE			2000

int sign_fire_cup_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int sign_fire_cup_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int get_team_id_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int get_team_id_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int get_team_medal_num_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int get_team_medal_num_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int get_sprite_medal_num_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int get_sprite_medal_num_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int add_medal_num_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int add_medal_num_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int add_fire_cup_medal_count(sprite_t* p, uint32_t item_id, int32_t item_cnt);
int add_lahm_sport_medal_count(sprite_t* p, uint32_t item_id, int32_t item_cnt);

#endif   /* ----- #ifndef FIRE_CUP_INC  ----- */

