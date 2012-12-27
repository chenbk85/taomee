#ifndef MOLE_HAPPY_CARD
#define MOLE_HAPPY_CARD

typedef struct _trade_c_t {
    uint32_t identify;
    uint32_t card;
    uint32_t card_want;
}trade_hy_card_db_t;

int set_happy_card_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_happy_card_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int trade_happy_card_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_happy_card_cloth_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_trade_happy_card_info_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int set_happy_card_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_happy_card_callback(sprite_t *p, uint32_t id, char *buf, int len);
int trade_happy_card_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_trade_happy_card_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_happy_card_cloth_callback(sprite_t *p, uint32_t id, char *buf, int len);

int get_fire_cup_card_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int do_add_fire_cup_card(sprite_t* p);
int get_magic_card_reward_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int do_get_magic_card_reward(sprite_t* p, uint32_t count);
#endif
