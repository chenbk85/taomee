/*
 * =====================================================================================
 *
 *       Filename:  lanterns_day.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/31/2012 05:13:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee (), ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef _LANTERNS_DAY_H
#define _LANTERNS_DAY_H

//刷新场景中的特殊汤圆
int ld_timer_initialize();
int ld_update_sp_lanterns( void* onwer, void* data );

//获取场景中的特殊汤圆信息
int ld_get_sp_lanterns_info_cmd(sprite_t* p, const uint8_t* body, int len);

//攻击特殊汤圆
int ld_attack_sp_lanterns_cmd(sprite_t* p, const uint8_t* body, int len);
int ld_attack_sp_lanterns_callback(sprite_t* p);

//攻击玩家
int ld_attack_player(sprite_t* p_attacker, sprite_t* p_tar);

#endif
