#ifndef _VIPONLY_H_
#define _VIPONLY_H_

#include "benchapi.h"
#include "util.h"
#include "global.h"
#include "./proto/mole2_db.h"

#define VIP_DRAW_CNT(p_) ((ISVIP(p_->flag) ? DRAW_GIFT_CNT : 0) + p_->draw_cnt_gift)
#define VIP_DRAW_CNT_USE(p_) (p_->vip_draw_cnt_gift_use + p_->vip_draw_cnt_day)

int get_vip_item_cmd(sprite_t* p, uint8_t* body, uint32_t len);
int get_vip_item_cnt_cmd(sprite_t* p, uint8_t* body, uint32_t len);

int load_vip_item(xmlNodePtr cur);
void init_vip_items();
vip_item_t* get_vip_item(uint32_t vid);


#endif
