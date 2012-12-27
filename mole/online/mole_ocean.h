/*
 * =====================================================================================
 *
 *       Filename:  mole_ocean.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/05/2010 10:18:10 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#ifndef  MOLE_OCEAN_INC
#define  MOLE_OCEAN_INC

int get_user_ocean_info_cmd(sprite_t *p, uint8_t *body, int len);
int get_user_ocean_info_callback(sprite_t *p, uint32_t id, char* buf, int len);
int put_user_ocean_animal_in_ocean_cmd(sprite_t* p, uint8_t* body, int bodylen);
int put_user_ocean_animal_in_ocean_callback(sprite_t* p, uint32_t id, char* buf, int len);
int user_feed_ocean_animal_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_feed_ocean_animal_callback(sprite_t* p, uint32_t id, char* buf, int len);
int user_use_ocean_tool_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_use_ocean_tool_callback(sprite_t* p, uint32_t id, char* buf, int len);
int user_expand_ocean_capacity_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_expand_ocean_capacity_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_user_ocean_items_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_user_ocean_items_callback(sprite_t* p, uint32_t id, char* buf, int len);

int user_diy_self_ocean_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_diy_self_ocean_callback(sprite_t* p, uint32_t id, char* buf, int len);

int user_get_offline_shells_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_get_offline_shells_callback(sprite_t* p, uint32_t id, char* buf, int len);

int user_get_online_shells_cmd(sprite_t* p, uint8_t* body, int bodylen);
int user_get_online_shells_callback(sprite_t* p, uint32_t id, char* buf, int len);

int user_pick_online_shells_cmd(sprite_t* p, uint8_t* body, int bodylen);

int user_sale_ocean_fish_cmd(sprite_t *p, uint8_t* body, int bodylen);
int user_sale_ocean_fish_callback(sprite_t* p, uint32_t id, char* buf, int len);

int user_buy_ocean_things_cmd(sprite_t *p, uint8_t* body, int bodylen);
int user_buy_ocean_things_callback(sprite_t* p, uint32_t id, char* buf, int len);

int mole_get_ocean_adventure_cmd(sprite_t *p, uint8_t* body, int bodylen);
int mole_get_ocean_adventure_callback(sprite_t* p, uint32_t id, char* buf, int len);

int mole_set_ocean_adventure_cmd(sprite_t *p, uint8_t* body, int bodylen);
int mole_set_ocean_adventure_callback(sprite_t* p, uint32_t id, char* buf, int len);

int mole_get_ocean_handbook_cmd(sprite_t *p, uint8_t* body, int bodylen);
int mole_get_ocean_handbook_callback(sprite_t* p, uint32_t id, char* buf, int len);

int mole_get_friend_ocean_info_cmd(sprite_t *p, uint8_t* body, int bodylen);
int mole_get_friend_ocean_info_callback(sprite_t* p, uint32_t id, char* buf, int len);




#endif   /* ----- #ifndef MOLE_OCEAN_INC  ----- */

