#ifndef THINKINGGIVING_H
#define THINKINGGIVING_H

typedef struct booth {
	userid_t uid;
	uint32_t selled_cnt;
	uint32_t itm_selling;
	uint32_t zaoxing;
	timer_struct_t* seller_timer;
} booth_t;

int get_vip_invitation_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_egg_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_egg_pos_callback(sprite_t* p, uint32_t id, char* buf, int len);
int process_fudan_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int process_fudan_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_fudan_times_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_fudan_times_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cut_yang_mao_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int cut_yangmao_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_egg_only_one_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int do_get_egg_only_one(sprite_t* p);
int broadcast_pos();
int clear_rabit_pos(uint32_t uid);
int is_animal_match(uint32_t itemid, uint32_t other_itemid);
int get_occupy_rabit_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int occupy_rabit_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int do_occupy_rabit_pos(sprite_t *p);
int get_little_rabit_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_little_rabit_callback(sprite_t* p, uint32_t id, char* buf, int len);
int check_is_sheng_rabit_callback(sprite_t* p, uint32_t id, char* buf, int len);
int do_buy_limited_item(sprite_t * p);
int buy_limited_item_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int break_fire_egg_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int break_fire_egg_callback(sprite_t* p, uint32_t id, char* buf, int len);
int fetch_little_rabit_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int fetch_little_rabit_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cut_occupy_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int cut_occupy_pos_callback(sprite_t* p, uint32_t id, char* buf, int len);
int cut_get_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int cut_get_pos_callback(sprite_t* p, uint32_t id, char* buf, int len);
void clear_cut_array(uint32_t uid);
int get_cut_position_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int beauty_dress_compete_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int do_beauty_dress_compete(sprite_t *p);
int do_beauty_dress_compete_ex(sprite_t *p);
int beauty_dress_compete_callback(sprite_t* p, uint32_t id, char* buf, int len);
int beauty_dress_compete_update_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_beauty_show_stat_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_model_wear_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_model_wear_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_model_wear_to_mole_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_model_wear_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_model_wear_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_model_wear_to_mole_callback(sprite_t* p, uint32_t id, char* buf, int len);
int do_beauty_dress_compete_update(sprite_t* p, char* buf, int len);
int set_mole_wear_to_model_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_mole_wear_to_model_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_cake_cmd (sprite_t *p, const uint8_t *body, int len);
int get_cake_info_cmd (sprite_t *p, const uint8_t *body, int len);
int get_rabit_master_cloth_cmd (sprite_t *p, const uint8_t *body, int len);
int get_rabit_master_cloth_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_beauty_show_score_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int show_query_model_index_callback(sprite_t* p, uint32_t id, char* buf, int len);
int buy_limited_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_limited_item_num_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_limited_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
int occupy_booth_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int buy_furni_from_booth_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int leave_booth_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_boothes_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int clean_booth(uint32_t uid);
int set_christmas_wish_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_christmas_wish_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_christmas_wish_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_christmas_wish_callback(sprite_t* p, uint32_t id, char* buf, int len);
int do_occupy_booth(sprite_t* p);
int get_little_tiger_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_little_tiger_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_yuanxiao_wish_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_yuanxiao_wish_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_yuanxiao_wish_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_yuanxiao_wish_callback(sprite_t* p, uint32_t id, char* buf, int len);
int exchange_gold_and_silver_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_the_event_recharge_months_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_the_event_recharge_months_callback(sprite_t* p, uint32_t id, char* buf, int len);

int set_the_event_recharge_replay_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_used_mibi_count_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_used_mibi_count_callback(sprite_t *p, uint32_t id, char* buf, int len);
int exchange_beans_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int dec_used_mibi_callback(sprite_t *p, uint32_t id, char* buf, int len);
#endif
