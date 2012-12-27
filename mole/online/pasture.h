/*
 * =====================================================================================
 *
 *       Filename:  pasture.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/11/2011 09:23:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef PASTURE_H
#define PASTURE_H

#define MAX_AWARD_ITEM  2

typedef struct award_info{
	uint32_t itemid;
	uint32_t count;
}__attribute__((packed)) award_milk_t;

typedef struct purchase_award{
	uint32_t count;
	award_milk_t items[MAX_AWARD_ITEM];
}__attribute__((packed)) purchase_award_t;

//进入蒙牛牧场
int get_mongolia_cow_pasture_cmd(sprite_t *p, const uint8_t *body, int len);
int get_mongolia_cow_pasture_callback(sprite_t *p, uint32_t id, char *buf, int len);

//种植牧草
int plant_pasture_grass_cmd(sprite_t *p, const uint8_t *body, int len);
int plant_pasture_grass_callback(sprite_t *p, uint32_t id, char *buf, int len);

//玩家与奶牛互动
int play_with_milk_cow_cmd(sprite_t *p, const uint8_t *body, int len);
int play_with_milk_cow_callback(sprite_t *p, uint32_t id, char *buf, int len);

//妙妙收购牛奶
int purchase_pasture_milk_cmd(sprite_t *p, const uint8_t *body, int len);
int purchase_pasture_milk_callback(sprite_t *p, uint32_t id, char *buf, int len);


#endif
