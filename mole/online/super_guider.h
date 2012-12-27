/*
 * =====================================================================================
 *
 *       Filename:  super_mogu.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/08/2011 11:12:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef SUPER_GUIDER_H
#define SUPER_GUIDER_H

#include "sprite.h"
#include "item.h"

//巡逻任务接取 4113
int sg_get_patrol_tast_cmd(sprite_t* p, const uint8_t* body, int len);
int sg_get_patrol_tast_callback(sprite_t * p, uint32_t id, char * buf, int len);

//小摩尔呼叫寻求帮助 4114
int sg_user_ask_for_help_cmd(sprite_t* p, const uint8_t* body, int len);

//超级蘑菇向导回应小摩尔寻求帮助 4115
int sg_guider_help_others_cmd(sprite_t* p, const uint8_t* body, int len);

//获得成就奖励 4116
int sg_get_guider_prize_cmd(sprite_t* p, const uint8_t* body, int len);
int sg_get_guider_prize_callback(sprite_t * p, uint32_t id, char * buf, int len);

//查询获得的成就奖励 4117
int sg_look_guider_prize_cmd(sprite_t* p, const uint8_t* body, int len);
int sg_look_guider_prize_callback(sprite_t * p, uint32_t id, char * buf, int len);

#endif
