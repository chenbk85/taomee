#ifndef MOON_CAKE_H
#define MOON_CAKE_H

typedef struct get_mooncake_cbk_pack{
	uint32_t sytle_id;
	uint32_t item_id;
	uint32_t count;
}__attribute__((packed)) get_mooncake_cbk_pack_t;

int user_eat_mooncake_cmd(sprite_t *p, const uint8_t *buf, int len);
int user_eat_mooncake_callback(sprite_t* p, uint32_t id, char* buf, int len);
int user_get_eat_mooncake_callback(sprite_t* p, uint32_t id, char* buf, int len);

int user_get_mooncake_cmd(sprite_t *p, const uint8_t *buf, int len);
int user_get_mooncake_callback(sprite_t* p, uint32_t id, char* buf, int len);

int user_swap_mooncake_cmd(sprite_t *p, const uint8_t *buf, int len);
int user_swap_mooncake_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_swap_mooncake_callback1(sprite_t* p, get_mooncake_cbk_pack_t* cbk_pack);
int get_swap_mooncake_callback2(sprite_t* p, get_mooncake_cbk_pack_t* cbk_pack);
#endif
