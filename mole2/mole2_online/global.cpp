#include "global.h"

/*! for client */
uint8_t msg[1<<20];
cli_handle_t	cli_handles[MAX_PROC_FUNCS];
list_head_t g_busy_sprite_list;

/*! dbproxy socket fd */
int proxysvr_fd = -1;
/*! siwtch socket fd */
int switch_fd = -1;
int magic_fd	= -1;
int spacetime_fd = -1;
int mall_fd = -1;
int vipserv_fd = -1;

/*! batrserv_fd */
int batrserv_fd[MAX_BATRSERV_NUM] = {-1, -1, -1};

uint8_t dbbuf[db_proto_max_len];

int udp_report_fd;
struct sockaddr_in udp_report_addr;
int udp_post_fd;
struct sockaddr_in udp_post_addr;
int chat_svr_fd;
struct sockaddr_in udp_chat_svr_addr;



/*! save handles for handling protocol packages from dbproxy */
dbproto_handle_t db_handles[65536];
code_handle_t code_handles[65536];
code_handle_t spacetime_handles[65536];
code_handle_t mall_handles[65536];

/*! hold common timers */
timer_head_t g_events;

char* statistic_logfile = NULL;
int idc_type = 1;

/*! for sprite */
GHashTable*	all_sprites;
GHashTable*	war_sprites;

list_head_t			idslots[HASH_SLOT_NUM];
uint32_t			sprites_count;
int injury_effect_ratio[4] = {10, 9, 6, 2};
uint32_t exp_lv[MAX_SPRITE_LEVEL + 1];
uint32_t exp_hang_base[MAX_SPRITE_LEVEL + 1] = {0,    \
	0,   1,   2,   3,   4,   5,   7,   9,   9,   11,  \
	13,  15,  17,  19,  21,  24,  26,  29,  31,  30,  \
	33,  35,  38,  40,  43,  46,  49,  52,  55,  51, \
	53,  56,  59,  62,  65,  68,  71,  74,  77,  69, \
	71,  74,  77,  80,  83,  86,  89,  92,  95,  82, \
	84,  87,  90,  92,  95,  98,  101, 104, 106, 87, \
	90,  92,  95,  97,  99,  102, 104, 107, 109, 101, \
	103, 105, 108, 110, 113, 115, 117, 120, 122, 111, \
	113, 115, 118, 120, 122, 125, 127, 129, 132, 134, \
	128, 130, 132, 135, 137, 139, 142, 144, 146, 0};

uint32_t relation_level[10][2] = {
		{0, 3021}, {1500, 3022}, {3000, 3023}, {4500, 3024}, {7500, 3025},
		{12000, 3026}, {19500, 3027}, {31500, 3028}, {51000, 3029}, {82500, 3030}
};

/*! for map */
list_head_t			map_hash_slots[MAP_HASH_SLOT_NUM];
map_t normal_map[NORMAL_MAP_NUM];

/*! for task */
GHashTable* all_tasks;
task_loop_t task_loops[MAX_TASK_LOOP_CNT];

/*! for skill */
GHashTable* sprite_skills = NULL;
GHashTable* pet_skills = NULL;

/*! for pet and beast */
beast_t beasts_attr[MAX_BEAST_TYPE];
GHashTable* beast_grp = NULL;

uint32_t valid_beast_cnt[LOOP_GRP_CNT];
uint32_t valid_beasts[LOOP_GRP_CNT][MAX_BEAST_TYPE];

uint32_t refresh_group_cnt;
refresh_group_t refresh_grps[MAX_REFRESH_GROUP_CNT];

GHashTable* npcs = NULL;
shop_t shops[MAX_SHOP_CNT];

pet_exchange_t pet_exchanges[MAX_PET_EXCHANGE_CNT];

pet_exchange_egg_t exchange_egg[MAX_PET_EX_EGG_CNT]; 
/*! for cloth and item */
GHashTable* all_clothes = NULL;
GHashTable*	all_items;
GHashTable* rand_infos = NULL;
exchange_info_t exchange_infos[MAX_EXCHANGE_INFO_CNT];
suit_t suits[MAX_SUIT_NUM];
GHashTable* vip_items = NULL;
GHashTable*	all_products = NULL;
uint32_t drop_clothes[CLOTH_LV_PHASE][100];
uint32_t cloth_drop_rate[CLOTH_LV_PHASE][PRODUCT_LV_CNT + 1];
uint32_t crystal_attr[attr_id_max][CLOTH_QUALITY_CNT][CLOTH_LV_PHASE];

/*! for honor title */
honor_attr_t honors[MAX_HONOR_NUM];

/*! for battle and team */
GHashTable*	batter_teams;
uint32_t			batter_team_cnt;
GHashTable*	batter_infos;
uint32_t			batter_info_cnt;

/*! for mail */
mail_t	sz_mail[MAX_MAIL_COUNT];
mail_op_t sz_mail_op[mail_sys_max][MAX_MAIL_COUNT];

GHashTable* all_mails;
GHashTable* sys_infos;

exp_factor_t holidy_factor[DAYS_PER_WEEK];

/*! for other online users */
GHashTable*	maps;
int home_serv_fd[MAX_HOMESERV_NUM] = {-1};

box_t all_boxes[MAX_BOX_CNT];
uint32_t active_box_cnt = 0;
uint32_t active_box[MAX_ACTIVE_BOX_CNT];
box_pos_t box_pos[MAX_BOX_POS_CNT];
list_head_t active_box_list;


ipport_t pic_srv[PIC_SRV_MAX_NUM];
int pic_srv_num = 0;

maze_config_t maze_config;

int maze_next = 0;
maze_t maze_all[256];

int total_water_cnt = 0;
int last_min_water_cnt = 0;
int auto_water_cnt = 0;

gift_pet_t g_gook_gift_pets[1];

init_prof_info_t init_prof_infos[MAX_PROF_CNT];

int g_log_send_buf_hex_flag = 1;

uint32_t hangup_uids[SEAT_TYPE * SEAT_NUM] = {0};

item_t rewards[3][4] = {{{310027, 0},	{310001, 0},	{350010, 0}, {300041, 1}}, \
						{{310024, 0},	{310025, 0},	{350013, 0}, {300041, 1}}, \
						{{100006, 0},	{310026, 0},	{310004, 0}, {300041, 1}}};

int beast_cnt = -1;

map_copy_config_t map_copy_configs[MAX_COPY_CONFIG_CNT];
uint32_t gvg_groups[MAX_GVG_WAIT]={0};


user_wait_info_t user_for_team[MAX_USER_WAIT]={};
GHashTable*	 map_copys = NULL;
uint8_t max_openbox_times=2;
uint32_t hangup_rewards[4]={300059,300060,300061,300062};
uint32_t g_luckystar_exchange;

