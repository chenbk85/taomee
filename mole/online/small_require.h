
#ifndef ONLINE_SMALL_REQUIRE
#define ONLINE_SMALL_REQUIRE

#define PROTO_BECOME_BIGGER				1132
#define PROTO_BECOME_BIGGER_EXPIRE		1133
#define PROTO_FINISH_WATER_BUCKET		1134
#define PROTO_BECOME_VICTORY			1141
#define PROTO_BECOME_FAIL				1142
#define PROTO_BECOME_VICTORY_EXPIRE		1143
#define PROTO_BECOME_FAIL_EXPIRE		1144
#define WATER_BUCKET_ITEMNUM			120000
#define BECOME_NANGUA					120001
#define VICTORY_ACTION_ITEMNUM			19001
#define FAIL_ACTION_ITEMNUM				19002

#define MAX_CANDYS_NUM_DAY				30
#define MAX_VIP_CANDYS_NUM_DAY			60
#define MAX_SCORE_NUM_DAY				48
#define MAX_VIP_SCORE_NUM_DAY			78

typedef struct op_baohe {
	userid_t	userid;
	uint32_t	itm_type;
	char		nick[USER_NICK_LEN];
	uint32_t	color;
	uint8_t 	isvip;
	uint32_t	op_stamp;
}__attribute__((packed)) op_baohe_t;

typedef struct chris_gift_list {
	userid_t	userid;
	char		nick[USER_NICK_LEN];
	uint32_t	color;
	uint8_t		isvip;
	uint32_t	op_stamp;
}__attribute__((packed)) chris_gift_list_t;

enum FOLLOW_ANIMAL_TYPE
{
	FAT_ANI		= 0,
	FAT_ANGEL,
	FAT_PIG,
};

extern uint32_t pos_dance[2];

#define db_inc_run_times(p_)\
		send_request_to_db(SVR_PROTO_INC_RUN_TIMES, (p_), 0, NULL, (p_)->id)

#define db_class_add_score(p_, buf_, uid_)\
		send_request_to_db(SVR_PROTO_CLASS_ADD_SCORE, (p_), 4, buf_, uid_)
#define db_class_get_score(p_, uid_)\
		send_request_to_db(SVR_PROTO_CLASS_GET_SCORE, (p_), 0, NULL, uid_)

#define db_add_tangguo_times(p_, buf_, uid_)\
		send_request_to_db(SVR_PROTO_ADD_TANGGUO_TIMES, (p_), 4, buf_, uid_)

#define db_add_tangguo_times_by_other(p_, buf_, uid_)\
		send_request_to_db(SVR_PROTO_ADD_TANGGUO_TIMES_BY_OTHER, (p_), 4, buf_, uid_)

#define db_add_class_score_times(p_, buf_, uid_)\
		send_request_to_db(SVR_PROTO_ADD_CLASS_SCORE_TIMES, (p_), 4, buf_, uid_)

#define db_get_tangguo_score_times(p_, uid_)\
		send_request_to_db(SVR_PROTO_GET_CANDY_TIMES, (p_), 0, NULL, uid_)

#define db_set_question_naire(p_, buf, len, uid)\
		send_request_to_db(SVR_PROTO_SET_QUESTIONNAIRE, (p_), len, buf, uid)

static inline int db_change_gift_num(sprite_t* p, int32_t num, userid_t uid)
{
	return send_request_to_db(SVR_PROTO_CHANGE_GIFT_NUM, p, 4, &num, uid);
}

static inline int db_add_op_to_baohe(sprite_t *p, userid_t id, uint32_t itmid)
{
	op_baohe_t visitor;

	visitor.userid = p->id;
	visitor.itm_type = itmid;
	memcpy(visitor.nick, p->nick, sizeof(visitor.nick));
	visitor.color = p->color;
	visitor.isvip = ISVIP(p->flag);
	visitor.op_stamp = get_now_tv()->tv_sec;

	return send_request_to_db(SVR_PROTO_ADD_USER_TO_BAOHE, 0, sizeof(visitor), &visitor, id);
}

static inline int db_add_op_to_chris(sprite_t *p, userid_t id)
{
	chris_gift_list_t cglt;

	cglt.userid = p->id;
	memcpy(cglt.nick, p->nick, sizeof(cglt.nick));
	cglt.color = p->color;
	cglt.isvip = ISVIP(p->flag);
	cglt.op_stamp = get_now_tv()->tv_sec;

	return send_request_to_db(SVR_PROTO_ADD_SEND_GIFT_LIST, p, sizeof(cglt), &cglt, id);
}

static inline int db_chk_itm_cnt(sprite_t* p, uint32_t first_itm, uint32_t last_itm)
{

	item_kind_t* kind = find_kind_of_item(first_itm);
	if (!kind) {
		ERROR_RETURN(("error item kind, item=%u, uin=%u", first_itm, p->id), -1);
	}

	uint32_t type = 0;
	if (kind->kind == HOME_ITEM_KIND) {
		type = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND) {
		type = 2;
	} else if (kind->kind == HAPPY_CARD_KIND) {
		type = 6;
	} else if (kind->kind == CLASS_ITEM_KIND) {
		type = 7;
	} else if (kind->kind == CAR_KIND) {
		type = 8;
	}

	int j = 0;
	uint8_t buff[13];
	PKG_H_UINT32(buff, type, j);
	PKG_H_UINT32(buff, first_itm, j);
	PKG_H_UINT32(buff, last_itm, j);
	PKG_UINT8(buff, 2, j);
	return send_request_to_db(SVR_PROTO_CHK_ITEM, p, j, buff, p->id);
}

static inline int db_add_item(sprite_t* p, uint32_t uid, uint32_t itmid, int count)
{

	item_kind_t* kind = find_kind_of_item(itmid);
	item_t* pitm = get_item_prop(itmid);
	if (!kind || !pitm) {
		ERROR_RETURN(("error item kind, item=%u, uin=%u", itmid, p->id), -1);
	}

	int i = 0;
	char buf[20];
	uint32_t flag = 0;
	if (kind->kind == HOME_ITEM_KIND) {
		flag = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND) {
		flag = 2;
	} else if (kind->kind == HAPPY_CARD_KIND) {
		flag = 6;
	}
	PKG_H_UINT32(buf, flag, i);
	PKG_H_UINT32(buf, itmid, i);
	PKG_H_UINT32(buf, count, i);
	PKG_H_UINT32(buf, pitm->max, i);
	PKG_H_UINT32(buf, 0, i);

	return send_request_to_db(SVR_PROTO_BUY_ITEM, p, 20, buf, uid);
}

static inline int db_del_multi_itm(sprite_t* p, uint32_t first_itm, uint32_t last_itm, userid_t uid)
{
	int j = 0;
	uint8_t buff[8];
	PKG_H_UINT32(buff, first_itm, j);
	PKG_H_UINT32(buff, last_itm, j);
	return send_request_to_db(SVR_PROTO_DEL_MULTI_ITEM, p, j, buff, uid);
}

#define db_get_baohe_op_list(p_, id)	\
		send_request_to_db(SVR_PROTO_GET_BAOHE_OP_LIST, (p_), 0, NULL, id)

#define PET_FORM_IS_CHANGED(p_) \
		((p_)->suppl_info.cur_form == 1)
#define PET_RECOVER_FORM(p_) \
		((p_)->suppl_info.cur_form = 0)
#define PET_CHANGE_FORM(p_) \
		((p_)->suppl_info.cur_form = 1)


int get_ques_answer_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_ques_answer_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_ques_num_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_ques_num_callback(sprite_t* p, uint32_t id, char* buf, int len);
int inc_run_times_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int inc_run_times_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_run_times_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_run_times_callback(sprite_t* p, uint32_t id, char* buf, int len);
int lahm_play_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int discover_random_item_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int become_bigger_cmd (sprite_t *p, const uint8_t *body, int len);
int become_bigger_expeired(void* owner, void* data);
int add_random_item(sprite_t* p);
int get_user_flag_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_user_flag_callback(sprite_t * p, uint32_t id, char * buf, int len);
int lahm_form_change_cmd(sprite_t *p, const uint8_t *body, int len);
int lahm_form_change_callback(sprite_t * p, uint32_t id, char * buf, int len);
int present_birthpet_hat_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int del_item_from_baohe(sprite_t* p);
int get_item_from_baohe_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_baohe_op_list_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_baohe_op_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_ques_naire_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int req_tmp_superlamn_stat_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int req_tmp_superlamn_stat_callback(sprite_t* p, uint32_t id, char* buf, int len);
int request_tmp_superlamn_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int request_tmp_superlamn_callback(sprite_t* p, uint32_t id, char* buf, int len);
int enable_tmp_superlamn_callback(sprite_t* p, uint32_t id, char* buf, int len);
int give_jijile_card_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_500_xiaome_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int pay_money_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_lottery_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int calculate_lottery(sprite_t* p, uint8_t* buf, int count);

int become_victory_cmd(sprite_t *p, const uint8_t *body, int len);
int become_fail_cmd(sprite_t *p, const uint8_t *body, int len);

int get_char_cloth_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int do_get_char_cloth(sprite_t* p, uint32_t count);
int catch_player_by_word_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int exchg_char_cloth_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_class_score_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_class_score_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_tangguo_score_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_tangguo_score_total_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int english_lahm_get_cloth_and_honor_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int vip_level_mole_gifts_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int vip_level_lahm_gifts_cmd(sprite_t * p, const uint8_t * body, int bodylen);

int english_award_class_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int are_you_get_xiaome_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int english_award_class_ex_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int do_send_candy_packet(sprite_t * p);
int send_candy_packet_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int become_nangua_expeired(void* owner, void* data);
int do_change_self_nangua(sprite_t *p);
int change_self_nangua_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int do_already_get_packet(sprite_t* p);
int get_bibo_date_info_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_bibo_date_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int send_gift_oneoftwo_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int do_send_gift_oneoftwo(sprite_t * p, char * buf, int len);
int send_gift_twooffour_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int do_send_gift_twooffour(sprite_t * p, char * buf, int len);

int set_photo_dress_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int set_photo_dress_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_photo_dress_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_photo_dress_callback(sprite_t* p, uint32_t id, char* buf, int len);
int echo_set_num_type_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int echo_get_num_type_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int echo_set_num_type_callback(sprite_t* p, uint32_t id, char* buf, int len);
int echo_get_num_type_callback(sprite_t* p, uint32_t id, char* buf, int len);
int broadcast_value_cmd(sprite_t * p, const uint8_t * body, int bodylen);

int get_all_tujian_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_all_tujian_callback(sprite_t* p, uint32_t id, char* buf, int len);

int weighing_fish_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int weighing_fish_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_weight_to_total(sprite_t * p);
int finshing_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_total_fish_weight_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int update_max_fish_weight_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_ring_from_other_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_ring_from_other_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_10xiaomee_50times_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_christmas_gift_num_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_five_christmas_gifts_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int send_chris_gift_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_chris_gift_from_bilu_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_christmas_gift_num_callback(sprite_t* p, uint32_t id, char* buf, int len);
int change_chris_gifts_num_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_op_to_chris_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_chris_gift_from_bilu_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_xiao_xiong_car_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int gifts_rain(void* owner, void* data);
int gift_disappear(sprite_t* p);
int collect_gift_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int send_chris_gift_to_npc_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int add_nengliangxing_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_nengliang_cnt_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int add_nengliang_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_nengliang_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len);
int response_chris_gift(sprite_t* p);
int lahm_food_machine_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int do_lahm_food_mechine(sprite_t* p);
int resp_lahm_food_mechine(sprite_t* p);
int set_photo_dahezhao_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_photo_dahezhao_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int set_photo_dahezhao_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_photo_dahezhao_callback(sprite_t* p, uint32_t id, char* buf, int len);

int set_type_photo_hezhao_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int set_type_photo_hezhao_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_type_photo_hezhao_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_type_photo_hezhao_callback(sprite_t* p, uint32_t id, char* buf, int len);



int do_get_dajiaoyin_reward(sprite_t* p, uint32_t count);
int exchg_mibi_coupon_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_dajiaoyin_reward_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int fetch_jinian_jiaoyin_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int vip_level_mole_gifts_car(sprite_t *p, int count);

int get_penguin_egg_hatch_times_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_penguin_egg_hatch_times_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_penguin_egg_hatch_times_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int set_penguin_egg_hatch_times_callback(sprite_t* p, uint32_t id, char* buf, int len);
int buy_some_cloths_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int buy_some_cloths_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_rand_times_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_rand_times_callback(sprite_t* p, uint32_t id, char* buf, int len);
int rand_item_want_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int rand_item_want_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_pet_attires_count_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_pet_attries_count_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_items_cnt_in_bag_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_items_cnt_in_bag_callback(sprite_t* p, uint32_t id, char* buf, int len);

int mole_get_carry_rock_num_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int mole_get_carry_rock_num_callback(sprite_t* p, uint32_t id, char* buf, int len);
int mole_set_carry_rock_num_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int mole_set_carry_rock_num_callback(sprite_t *p, uint32_t id, char *buf, int len);
int calc_month_vip_level(int vip_month);
int quit_machine_dog_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int quit_machine_dog_callback(sprite_t *p, uint32_t id, char *buf, int len);
int feed_machine_dog_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int feed_machine_dog_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_machine_dog_do_thing_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_machine_dog_do_thing_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_architect_exp_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_pipi_or_douya_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int add_pipi_or_douya(sprite_t *p);
int set_only_one_flag_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int add_score_for_wo_wei_hui_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int add_score_for_wo_wei_hui_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_score_for_wo_wei_hui_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_score_for_wo_wei_hui_callback(sprite_t *p, uint32_t id, char *buf, int len);
int feed_rabit_get_seed_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int feed_rabit_get_seed_callback(sprite_t *p);
int get_support_npc_wo_wei_hui_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_login_session_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int become_shibohui_expeired(void *owner, void *data);
int get_money_by_gua_gua_card_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_money_by_gua_gua_card_callback(sprite_t *p, uint32_t id, char *buf, int len);

int get_type_data_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_type_data_callback(sprite_t *p, uint32_t id, char *buf, int len);
int set_type_data_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_type_data_callback(sprite_t *p, uint32_t id, char *buf, int len);

int get_session_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_session_callback(sprite_t *p, uint32_t id, char *buf, int len);

int leave_hero_game_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int notify_enter_leave_hero_game(sprite_t* p, uint32_t flag);
int get_in_hero_userid_list_cmd (sprite_t *p, const uint8_t *body, int len);


int add_red_clothe_accord_exp_lance_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_card_game_info_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int get_lance_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int set_only_one_bit(sprite_t *p, uint32_t index);

int check_only_one_bit(sprite_t *p, uint32_t index);

int get_businessman_goods_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int get_businessman_goods_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int buy_businessman_goods_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int buy_businessman_goods_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int buy_businessman_add_bag_callback(sprite_t *p);

int get_dragon_egg_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int exchange_one_thing_to_another_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int exchange_one_thing_to_another_callback(sprite_t *p);

int get_pig_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int get_pig_callback(sprite_t *p,uint32_t id,const uint8_t *buf, int len);

int get_open_school_gift_cmd(sprite_t *p, const uint8_t *buf, int bodylen);

int get_profession_exp_level_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int look_sth_cnt_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int look_sth_cnt_callback_0(sprite_t *p,uint32_t type,uint32_t cnt);
int look_sth_cnt_callback_1(sprite_t *p);
int get_ques_naire_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_ques_naire_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_octopus_car_win_or_lose_cmd(sprite_t *p, const uint8_t *buf, int bodylen);
int add_client_msg_log_cmd(sprite_t *p, const uint8_t *body, int bodylen);

//∑ ∑ ¥Ô»À–„Õ∂∆±¡ÏΩ±
int get_feifei_daren_prize_cmd(sprite_t* p, const uint8_t* body, int len);
int get_feifei_daren_prize_callback(sprite_t* p, uint32_t id, char* buf, int len);

//≥ËŒÔ∏˙ÀÊ
int set_sth_follow( sprite_t* p, uint32_t ani_idx, uint32_t is_follow, uint32_t ani_type );

int get_day_type_count_cmd(sprite_t * p,const uint8_t * body,int len);
int get_day_type_count_callback(sprite_t* p, uint32_t id, char* buf, int len);


#endif

