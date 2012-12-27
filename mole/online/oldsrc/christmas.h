
#ifndef CHRISTMAS_H
#define CHRISTMAS_H

#include "benchapi.h"

#define SVR_PROTO_SET_CHRISTMAS_WISH		0xD120
#define SVR_PROTO_GET_CHRISTMAS_WISH		0xD121
#define SVR_PROTO_IS_GET_CHRISTMAS_WISH		0xD022
#define SVR_PROTO_IS_SET_CHRISTMAS_WISH		0xD023

#define MAX_CHRISTMAS_WISH_LEN		255

#define PET_IS_DRESS_HAT(p_) \
		((p_)->suppl_info.cur_form == 1)
#define PET_UNDRESS_HAT(p_) \
			((p_)->suppl_info.cur_form = 0)
#define PET_DRESS_HAT(p_) \
			((p_)->suppl_info.cur_form = 1)

int set_christmas_wish_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int set_christmas_wish_callback(sprite_t * p,uint32_t id,char * buf,int len);
int is_set_christmas_wish_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int is_set_christmas_wish_callback(sprite_t * p,uint32_t id,char * buf,int len);
int get_christmas_wish_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int get_christmas_wish_callback(sprite_t * p,uint32_t id,char * buf,int len);
int is_get_christmas_wish_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int is_get_christmas_wish_callback(sprite_t * p,uint32_t id,char * buf,int len);
int lahm_dress_hat_cmd (sprite_t *p, const uint8_t *body, int len);

#endif


