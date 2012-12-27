/*
 * =====================================================================================
 *
 *       Filename:  winter_vs_spring.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/09/2012 03:31:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee (), ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef WINTER_VS_SPRING_H
#define WINTER_VS_SPRING_H

// ¶ÓÎé±ä¸ü
int wvs_team_switch_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int wvs_get_team_power_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int wvs_get_team_power_callback(sprite_t* p, uint32_t id, char* buf, int len);

#endif
