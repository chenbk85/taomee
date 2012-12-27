/*
 * =====================================================================================
 *
 *       Filename:  flower_party.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2012 02:17:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee (), ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef _FLOWER_PARTY_H
#define _FLOWER_PARTY_H

typedef struct flower_info_s
{
	uint32_t	item_id;
	char		is_valid;
}flower_info_t;

typedef struct bouquet_info_s
{
	uint8_t		flower_idx;
	uint8_t		is_set;
}bouquet_info_t;

typedef struct fp_timer {
	list_head_t timer_list;
}__attribute__(( packed )) fp_timer_t;

int fp_game_initialize();
//选择一种花
int fp_get_flower_cmd(sprite_t* p, const uint8_t* body, int len);
//插花
int fp_set_flower_cmd(sprite_t* p, const uint8_t* body, int len);
//开启VIP花篮
int fp_open_vip_flower_cmd(sprite_t* p, const uint8_t* body, int len);
//关闭vip花篮
int fp_close_vip_flower( void* onwer, void* data );
//获取游戏信息
int fp_get_game_info_cmd(sprite_t* p, const uint8_t* body, int len);
//刷新花束
int fp_update_bouquet( int is_force );
//领取奖励
int fp_get_award_cmd(sprite_t* p, const uint8_t* body, int len);
int fp_get_award_day_limit_callback( sprite_t* p );

//本服广播信息
int fp_game_broadcast( void* onwer, void* data );

#endif
