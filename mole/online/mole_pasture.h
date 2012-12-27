
#ifndef ONLINE_PASTURE_H
#define ONLINE_PASTURE_H

#define ANIMAL_LEVEL_MAX    5
#define ANIMAL_STAR_MAX    4
#define ANIMAL_STAR_LEVEL_SKILL_MAX    20
#define ANIMALS_SKILLS_MAX    200
#define ANIMAL_ID_MIN_VALUE   1270001
#define ANIMAL_RAIN_EGG_POS_MAX    10
#define ANIMAL_RAIN_EGG_MAP_MAX    200





typedef struct {
	uint32_t    skill_id;
	uint32_t    type;
	uint32_t    cold_time;
	uint32_t    max_time;
}__attribute__(( packed ))skill_info_t;

typedef struct {
	uint32_t		level_id;
	uint32_t		skill_count;
	skill_info_t    level_skills[ANIMAL_STAR_LEVEL_SKILL_MAX];
}__attribute__(( packed ))level_skills_t;

typedef struct {
	uint32_t		 star_id;
	uint32_t		 level_count;
	level_skills_t   star_levels[ANIMAL_LEVEL_MAX];
}__attribute__(( packed )) star_levels_t;

typedef struct {
	uint32_t		 animal_id;
	uint32_t		 star_count;
	star_levels_t    animal_stars[ANIMAL_STAR_MAX];
}__attribute__(( packed )) animal_skills_t;

typedef struct s_pos_info{
	uint32_t    posid;
	uint32_t    x;
	uint32_t    y;
	uint32_t    itemid;
}__attribute__(( packed ))map_pos_info_t;

typedef struct {
	uint32_t		mapid;
	uint32_t		pos_count;
	map_pos_info_t  pos_infos[ANIMAL_RAIN_EGG_POS_MAX];
}__attribute__(( packed ))map_pos_t;


#define PASTURE_VISITOR_MAX_NUM	50
#define PASTURE_VISITOR_LIST    0  // 	used  for pasture visitor
#define PASTURE_VISITOR_FISHING	1  //     1  used for get fish
#define PASTURE_GET_SHEEP_LIST  4  // 	used for visitor get sheep
typedef struct visitor_pasture
{
	uint32_t userid;
	uint32_t op_type;    //ox01  water, 0x02, kill bug
	uint8_t  nick[16];
	uint32_t color;
	uint8_t	 isvip;
	uint32_t stamp;
}__attribute__((packed))visitor_pasture_t;

typedef struct pv_list_res
{
	uint32_t 			cnt;
	visitor_pasture_t 	visitor[];
}__attribute__((packed))pv_list_res_t;

#define db_list_recent_pasture_visitors(p_, uid) \
		send_request_to_db(SVR_PROTO_LIST_RECENT_PASTURE_VISITOR, (p_), 0, NULL, uid)

#define db_pasture_add_feed(p_, buf, uid_) \
		send_request_to_db(SVR_PROTO_PASTURE_ADD_FEED, p_, 12, buf, uid_)

int get_pasture_cmd(sprite_t* p, const uint8_t* body, int len);
int get_pasture_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pasture_herd_animal_cmd(sprite_t* p, uint8_t* body, int len);
int pasture_herd_animal_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pasture_add_feed_cmd(sprite_t* p, uint8_t* body, int len);
int pasture_add_feed_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pasture_capture_animal_cmd(sprite_t* p, uint8_t* body, int len);
int pasture_capture_animal_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pasture_get_animal_cmd(sprite_t* p, const uint8_t* body, int len);
int pasture_get_animal_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pasture_add_water_cmd(sprite_t* p, const uint8_t* body, int len);
int pasture_add_water_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_recent_pasture_visitors_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_recent_pasture_visitors_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_pasture_items_cmd(sprite_t* p, const uint8_t* body, int len);
int set_pasture_items_cmd(sprite_t* p, uint8_t* body, int len);
int set_pasture_items_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_store_items_cmd(sprite_t* p, uint8_t* body, int len);
int get_store_items_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_feed_items_cmd(sprite_t* p, uint8_t* body, int len);
int pasture_lock_cmd(sprite_t* p, uint8_t* body, int len);
int pasture_lock_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_sheep_from_vip_cmd(sprite_t* p, uint8_t* body, int len);
int do_get_sheep(sprite_t* p, uint32_t count, uint8_t* buf);
int do_get_sheep2(sprite_t* p);
int get_sheep_reward_cmd(sprite_t* p, uint8_t* body, int len);
int get_sheep_reward_callback(sprite_t* p, uint32_t id, char* buf, int len);
int follow_animal_cmd(sprite_t* p, uint8_t* body, int len);
int follow_animal_callback(sprite_t* p, uint32_t id, char* buf, int len);
int animal_chicao_cmd(sprite_t* p, uint8_t* body, int len);
int animal_chicao_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pasture_get_nets_cmd(sprite_t* p, uint8_t* body, int len);
int pasture_get_nets_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pasture_get_nets_status_cmd(sprite_t* p, uint8_t* body, int len);
int do_pasture_get_nets(sprite_t *p);
int pasture_get_nets_status_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pasture_catch_fish_cmd(sprite_t* p, uint8_t* body, int len);
int pasture_catch_fish_callback(sprite_t* p, uint32_t id, char* buf, int len);
int pasture_get_level_cmd(sprite_t* p, uint8_t* body, int len);
int level_change_and_send_postcard(uint32_t new_level, uint32_t old_level, uint32_t post_id, uint32_t uid);
int calculation_level_from_skill(uint32_t skill);
int add_breed_exp(sprite_t* p, int breed_exp);
int pasture_get_level_ex_cmd(sprite_t* p, uint8_t* body, int len);
int pasture_get_level_ex_callback(sprite_t* p, uint32_t id, char* buf, int len);
int calculation_level_from_fashion(uint32_t fashion);
int pasture_release_animal_cmd(sprite_t* p, uint8_t* body, int len);
int pasture_release_animal_callback(sprite_t* p, uint32_t id, char* buf, int len);
int notify_skill_change(sprite_t* p, int cultiv_skill_change, int breed_skill_change);
int get_milk_from_cow_cmd(sprite_t* p, uint8_t* body, int len);
int get_milk_from_cow_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_animal_lvl_cmd(sprite_t *p, uint8_t body[], int len);
int get_animal_lvl_callback(sprite_t *p, uint32_t id, char *buf, int len);
int farm_animal_use_item_cmd(sprite_t * p,const uint8_t * body,int len);
int farm_animal_use_item_callback(sprite_t *p, uint32_t id, char *buf, int len);
int load_animal_skills_conf(const char *file);
int cal_level_by_growth(uint32_t animalid, uint32_t growth);
int check_skill_use_enable(level_skills_t * p_level_skills, uint32_t skill_id);
int get_animal_used_skill_info_cmd(sprite_t *p, uint8_t body[], int len);
int farm_animal_use_skill_cmd(sprite_t * p,const uint8_t * body,int len);
int get_animal_used_skill_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_animal_base_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int farm_animal_use_skill_callback(sprite_t *p, uint32_t id, char *buf, int len);
int notify_animal_use_skill_map(sprite_t* p, uint32_t skill_id);
int notify_animal_use_skill_all(sprite_t* p, uint32_t skill_id);
int load_rain_egg_map_pos_conf(const char *file);
int get_map_rain_egg_info_cmd(sprite_t *p, uint8_t body[], int len);
int gain_map_pos_egg_cmd(sprite_t *p, uint8_t body[], int len);
int gain_map_pos_egg(sprite_t *p);
int get_very_good_animal_count_cmd(sprite_t *p, uint8_t body[], int len);
int get_very_good_animal_count_callback(sprite_t* p, uint32_t id, char* buf, int len);
int notify_all_use_skill_prize(sprite_t* p, uint32_t prize_flag);
int animal_add_use_skill_count_callback(sprite_t *p, uint32_t id, char *buf, int len);
int gain_use_skill_prize_item_cmd(sprite_t *p, uint8_t body[], int len);
int animal_dec_use_skill_count_callback(sprite_t *p, uint32_t id, char *buf, int len);
int gain_use_skill_prize_item(sprite_t *p);
int get_use_skill_prize_info_cmd(sprite_t *p, uint8_t body[], int len);
int get_use_skill_prize_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int notify_one_use_skill_prize(sprite_t* p, uint32_t prize_flag);


static inline int db_add_pasture_visitors(sprite_t *p, userid_t id, int type)
{
	visitor_pasture_t visitor;

	visitor.userid = p->id;
	visitor.op_type = type;
	memcpy(visitor.nick, p->nick, sizeof(visitor.nick));
	visitor.color = p->color;
	visitor.isvip = ISVIP(p->flag);
	visitor.stamp = get_now_tv()->tv_sec;

	return send_request_to_db(SVR_PROTO_ADD_PASTURE_VISITOR, 0, sizeof(visitor), &visitor, id);
}

static inline int db_animal_chicao(sprite_t *p, userid_t id, int nbr)
{
	return send_request_to_db(SVR_PROTO_ANIMAL_CHICAO, p, 4, &nbr, id);
}

static inline int db_animal_set_outgo(sprite_t *p, userid_t id, int nbr, int out_flag)
{
	uint32_t buff[2];
	buff[0] = nbr;
	buff[1] = out_flag;
	return send_request_to_db(SVR_PROTO_ANIMAL_OUTGO, p, 8, buff, id);
}
static inline int db_put_angel_home(sprite_t *p, userid_t id, int nbr)
{
	uint32_t buff[2];
	buff[0] = nbr;
	buff[1] = 0;
	return send_request_to_db(SVR_PROTO_MAKE_ANGEL_FOLLOWED, p, 8, buff, id);
}

static inline void pkg_animal_info(void* buf, const animal_attr_t* aat, int* indx)
{
	PKG_UINT32(buf, aat->number, *indx);
	PKG_UINT32(buf, aat->itemid, *indx);
	PKG_UINT32(buf, aat->sickflag, *indx);
	PKG_UINT32(buf, aat->value, *indx);
	PKG_UINT32(buf, aat->eat_time, *indx);
	PKG_UINT32(buf, aat->drink_time, *indx);
	PKG_UINT32(buf, aat->output_cnt, *indx);
	PKG_UINT32(buf, aat->output_time, *indx);
	PKG_UINT32(buf, aat->update_time, *indx);
	PKG_UINT32(buf, aat->mature_time, *indx);
	PKG_UINT32(buf, aat->animal_type, *indx);
	PKG_UINT32(buf, aat->calc_favor_time, *indx);
	PKG_UINT32(buf, aat->favor, *indx);
	PKG_UINT32(buf, aat->outgo, *indx);
	PKG_UINT32(buf, aat->other_attr, *indx);
	PKG_UINT32(buf, aat->remaind_pollinate_cnt, *indx);
	PKG_UINT32(buf, aat->max_output, *indx);
	PKG_UINT32(buf, aat->diff_mature, *indx);
	PKG_UINT32(buf, aat->cur_grow, *indx);
	PKG_UINT32(buf, aat->lvl, *indx);
}

static inline int db_add_animal_num(sprite_t *p, uint32_t itmid, int num, int max_num)
{
	uint32_t buff[3];
	buff[0] = itmid;
	buff[1] = num;
	buff[2] = max_num;
	return send_request_to_db(SVR_PROTO_YOUYOU_ADD_ANIMAL, p, 12, buff, 0);
}

#endif

