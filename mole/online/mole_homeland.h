
#ifndef ONLINE_HOMELAND_H
#define ONLINE_HOMELAND_H

#define MAX_BG_NUM		40
#define MAX_GRID_NUM 	12
typedef struct seed_grid
{
    uint32_t x;
    uint32_t y;
}__attribute__((packed))seed_grid_t;

typedef struct bg_seed_grid
{
    uint32_t 	itmid;
	seed_grid_t	grid[MAX_GRID_NUM];
}__attribute__((packed))bg_seed_grid_t;

#define VISITOR_MAX_NUM	50
typedef struct visitor_jy
{
    uint32_t userid;
	uint32_t op_type;    //ox01  water, 0x02, kill bug
    uint8_t  nick[16];
	uint32_t color;
	uint8_t	 isvip;
    uint32_t stamp;
}__attribute__((packed))visitor_jy_t;

typedef struct visitor_list_res
{
    uint32_t cnt;
    visitor_jy_t visitor[];
}__attribute__((packed))visitor_list_res_t;

typedef struct plant_attr{
	uint32_t number;
	uint32_t itemid;
	uint32_t x;
	uint32_t y;
	uint32_t value;
	uint32_t sickflag;
	uint32_t fruit_num;
	uint32_t update_time;
	uint32_t water_time;
	uint32_t kill_bug_time;
	uint32_t fruit_status;
	uint32_t mature_time;
	uint32_t diff_mature_time;
	uint32_t cur_grow_rate;
	uint32_t earth;
	uint32_t pollinate_num;
} __attribute__ ((packed)) plant_attr_t;

#define db_set_homeland_item(p, b, l) \
		send_request_to_db(SVR_PROTO_SET_HOMELAND_ITEM, p, l, b, p->id)
#define db_list_recent_jy_visitors(p_, uid) \
		send_request_to_db(SVR_PROTO_LIST_RECENT_JY_VISITOR, (p_), 0, NULL, uid)
#define db_fertilize_flower(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_FERTILIZE_FLOWER, (p_), 12, buf_, (uid_))
#define db_get_fruit_from_nbr(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_GET_FRUIT_FROM_NBR, (p_), 4, buf_, (uid_))

int enter_homeland_cmd(sprite_t* p, const uint8_t* body, int len);
int get_homeland_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_homeland_items_cmd(sprite_t * p, uint8_t * body, int len);
int get_homeland_box_items_cmd(sprite_t * p, uint8_t * body, int len);
int get_homeland_box_item_callback(sprite_t * p, uint32_t id, char * buf, int len);
int homeland_plant_cmd(sprite_t * p, uint8_t * body, int len);
int homeland_plant_callback(sprite_t * p, uint32_t id, char * buf, int len);
int homeland_root_out_plant_cmd(sprite_t * p, uint8_t * body, int len);
int homeland_root_out_callback(sprite_t * p, uint32_t id, char * buf, int len);
int homeland_water_plant_cmd(sprite_t * p, uint8_t * body, int len);
int homeland_water_plant_callback(sprite_t * p, uint32_t id, char * buf, int len);
int homeland_kill_bug_cmd(sprite_t * p, uint8_t * body, int len);
int homeland_kill_bug_callback(sprite_t * p, uint32_t id, char * buf, int len);
int homeland_get_one_plant_cmd(sprite_t * p, uint8_t * body, int len);
int homeland_get_one_plant_callback(sprite_t * p, uint32_t id, char * buf, int len);
int homeland_harvest_fruits_cmd(sprite_t * p, uint8_t * body, int len);
int homeland_harvest_fruits_callback(sprite_t * p, uint32_t id, char * buf, int len);
int sell_fruits_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_recent_jy_visitors_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_recent_jy_visitors_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_homeland_used_itms_cmd(sprite_t* p, uint8_t* body, int len);
int chk_if_user_exist_cmd(sprite_t* p, uint8_t* body, int len);
int chk_if_user_exist_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_fruit_free_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int do_get_fruit_free(sprite_t *p, uint32_t itmid);
int homeland_thief_fruit_cmd(sprite_t * p, uint8_t * body, int len);
int homeland_thief_fruit_callback(sprite_t * p, uint32_t id, char * buf, int len);
int add_culti_breed_exp_callback(sprite_t* p, uint32_t id, char* buf, int len);
int gift_draw_cmd(sprite_t* p, uint8_t* body, int len);
int do_draw(sprite_t *p);
int add_cultivate_exp(sprite_t* p, int cultivt_exp);
int homeland_pollinate_cmd(sprite_t* p, uint8_t* body, int len);
int homeland_dec_pollinate_callback(sprite_t* p, uint32_t id, char* buf, int len);
int homeland_pollinate_flower_callback(sprite_t* p, uint32_t id, char* buf, int len);
int homeland_fertilize_plant_cmd(sprite_t* p, uint8_t* body, int len);
int homeland_fertilize_plant_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_fruit_from_nbr_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_jy_anmimal_items_cmd(sprite_t* p, uint8_t* body, int len);
int get_jy_anmimal_items_callback(sprite_t* p, uint32_t id, char* buf, int len);



static inline int db_add_jy_visitors(sprite_t *p, userid_t id)
{
	visitor_jy_t visitor;

	visitor.userid = p->id;
	visitor.op_type = 0;
	memcpy(visitor.nick, p->nick, sizeof(visitor.nick));
	visitor.color = p->color;
	visitor.isvip = ISVIP(p->flag);
	visitor.stamp = get_now_tv()->tv_sec;

	return send_request_to_db(SVR_PROTO_ADD_JY_VISITOR, 0, sizeof(visitor), &visitor, id);
}

static inline int db_add_culti_breed_exp(sprite_t *p, uint32_t cb_exp, uint32_t cb_type, userid_t id)
{
	uint32_t buff[2];
	buff[0] = cb_exp;
	buff[1] = cb_type;
	return send_request_to_db(SVR_PROTO_ADD_CULTI_BREED_EXP, p, 8, buff, id);
}


#endif

