/*
 * =====================================================================================
 *
 *       Filename:  final_boss_2011.h
 *
 *    Description:  2011火神杯最终BOSS
 *
 *        Version:  1.0
 *        Created:  08/15/2011 03:01:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef FINAL_BOSS_2011_H
#define FINAL_BOSS_2011_H

#define FB_MAP_ID				225

enum WEAPON_TYPE
{
	WT_MAGIC_ICE		= 0,//水
	WT_MAGIC_NATURE,		//木
	WT_MAGIC_FIRE,			//火
	WT_MAGIC_SPIRIT,		//超级
	WT_WATER_BOMB,			//水弹
	WT_BLUE_BOMB,			//蓝色炸弹
	WT_MAX
};

typedef struct weapon_info_s
{
	int				damage;		//攻击伤害
	int				cd;			//攻击施法时间
	timer_struct_t*	timer;		//攻击定时器
} weapon_info_t;

int fb_initialize();

void fb_water_bomb_attack(sprite_t*p, uint32_t x, uint32_t y);

//获取boss血量等数据
int	fb_get_boss_info_cmd(sprite_t* p, const uint8_t* body, int len);
//进入炮台
int fb_enter_turret_cmd(sprite_t* p, const uint8_t* body, int len);
int fb_enter_bomb_trret(sprite_t* p, int bomb_count);
//离开炮台
int fb_leave_turret_cmd(sprite_t* p, const uint8_t* body, int len);

#endif
