/*
 * =====================================================================================
 *
 *       Filename:  spring_couplets.c
 *
 *    Description:  春联魔法墙
 *
 *        Version:  1.0
 *        Created:  01/09/2012 02:20:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alexhe, alexhe@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef COUPLETS_H__
#define COUPLETS_H__

#include "sprite.h"
#include "item.h"

#define MAX_COUPLETS_WORD_ID	53

#define MAX_COUPLETS_TITLE_SIZE		4
#define MAX_COUPLETS_WORD_SIZE		7

typedef struct {
	uint32_t couplets_id;
	uint8_t couplets_title[MAX_COUPLETS_TITLE_SIZE];
	uint8_t first_line[MAX_COUPLETS_WORD_SIZE];
	uint8_t second_line[MAX_COUPLETS_WORD_SIZE];
}__attribute__((packed)) spring_couplets_info_t;

//春联魔法墙:提交春联 4124
int user_submit_spring_couplets_cmd(sprite_t *p, const uint8_t *buf, int len);
int user_submit_spring_couplets_callback(sprite_t* p, uint32_t id, char* buf, int len);

//春联魔法墙:查看用户的所有春联 4125
int user_get_spring_couplets_cmd(sprite_t *p, const uint8_t *buf, int len);
int user_get_spring_couplets_callback(sprite_t* p, uint32_t id, char* buf, int len);

//春联魔法墙:查看墙上的春联列表 4126
int user_get_couplets_userid_list_cmd(sprite_t *p, const uint8_t *buf, int len);
int user_get_couplets_userid_list_callback(sprite_t* p, uint32_t id, char* buf, int len);

//春联魔法墙:贴了春联后许愿 4128
int user_set_couplets_prize_cmd(sprite_t *p, const uint8_t *buf, int len);
int user_set_couplets_prize_callback(sprite_t* p, uint32_t id, char* buf, int len);

//春联魔法墙:领取礼物 4127
int user_get_couplets_prize_cmd(sprite_t *p, const uint8_t *buf, int len);
int user_get_couplets_prize_callback(sprite_t* p, uint32_t id, char* buf, int len);

#endif
