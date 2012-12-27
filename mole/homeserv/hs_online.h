#ifndef HS_ONLINE_H_
#define HS_ONLINE_H_

#include <string.h>

#include <async_serv/net_if.h>

#include "proto.h"

#define MAX_FD_NUM		30000
#define MAX_ONLINE_NUM	1200 + 1
#define PET_NICK_LEN		16
#define PET_CLOTH_MAX_CNT	1
#define PET_HONOR_MAX_CNT	1
#define PET_TASK_MAX_CNT	20

#define MAX_ENTRY_NUM_PER_MAP	32
#define MAX_PLAYERS_PER_GAME	20
#define MAX_PETS_PER_SPRITE		4
#define MAX_WATCHERS_PER_GAME	20
#define MAX_ITEMS_WITH_BODY		12
#define USER_NICK_LEN			16


#define MAX_OWNED_MOLEGROUP_NUM		3
#define MAX_OWNED_MOLECLASS_NUM		10
#define MAX_MONTH_TASKS				48
#define MAX_DAILY_COUNT				3
#define START_DAY_LIMIT				1001
#define END_DAY_LIMIT				1003
#define MAX_NPC  					20
#define STUDENT_COUNT_MAX  	30

#define MAX_LATEST_ONLINE		60

#define	DUNGEON_MAX_ITEM_KIND						48			//掉落物品最大类型数
#define DUNGEON_MAX_MAP_AREA_NUM					64			//每屏幕最大探索区域数目
#define	DUNGEON_MAX_RANDOM_ELEMENT_TYPE 			32			//随机元素类型总数
#define DUNGEON_MAX_SCREEN_NUM						10			//最大屏幕数目
#define DUNGEON_MAX_RANDOM_ELEMENT_COUNT_PER_SCREEN 5			//每张地图的最大随区域数目

#define MAX_TALK_COUNT_IN_LIMIT_TIME                10

enum {
	ACTION_IDLE = 0,
	ACTION_DANCE,
	ACTION_WAVE,
	ACTION_SIT,
	ACTION_PAOPAO,
	ACTION_levelup,
	ACTION_gotmedal,
	ACTION_MAX
};

// holds all the @sprites that are in @mapid
typedef struct SpriteWhere {
	map_id_t 	mapid;
	uint8_t		latest_idx;
	uint16_t	onlineid[MAX_LATEST_ONLINE];
	GHashTable* sprites;
} sprite_where_t;

typedef struct _car {
    uint32_t db_id;
    uint32_t kindid;
    uint32_t oil;
    uint32_t oil_consump;
    uint32_t color;
    uint32_t oil_time;
	uint32_t max_oil;
    uint32_t slot_cnt;
    uint32_t slot[4];
    uint32_t addon[2];
} __attribute__((packed)) car_t;

typedef struct task_info {
	uint8_t	taskid;
	uint8_t	task_flag;
} __attribute__ ((packed)) task_info_t;

//pet task info
typedef struct PetTaskInfo {
	uint8_t	count;       // user of the pet
	task_info_t	taskinfo[PET_TASK_MAX_CNT];
} __attribute__ ((packed)) pet_task_info_t;

// supplemental info
typedef struct PetSupplInfo {
	userid_t	uid;       // user of the pet
	uint32_t	flag;
	uint32_t	cur_form;  // current formation of the pet
	map_id_t	mapid;     // map where the pet is located currently

	uint8_t		pet_cloth_cnt;
	uint32_t	pet_clothes[PET_CLOTH_MAX_CNT];
	uint8_t		pet_honor_cnt;
	uint32_t	pet_honor[PET_HONOR_MAX_CNT];
	pet_task_info_t		pti;
} __attribute__ ((packed)) pet_suppl_info_t;

typedef struct pet {
	uint32_t	id;
	uint32_t	flag;
	uint32_t	flag2;
	uint32_t	birth;
	uint32_t	logic_birthday;
	char		nick[PET_NICK_LEN];
	uint32_t	color;
	uint32_t	sicktime; // indicate sick time if is sick or dead time if is dead
	/*the head 40 byte can not change*/
	uint8_t		posX;
	uint8_t		posY;

	uint8_t		hungry;
	uint8_t		thirsty;
	uint8_t		sanitary;
	uint8_t		spirit;
	uint32_t	trust_end_tm; // indicate trust/task end time
	uint32_t		start_tm; //  indicate task start time
	uint32_t	taskid;   //  task id
	uint32_t	skill;
	uint32_t	stamp;
	uint32_t	sick_type;
	uint32_t	skill_ex[3];//0 :fire, 1:water, 2:wood
	uint32_t	skill_bits;
	uint32_t	skill_value;
	uint32_t	skill_hot[3];
	uint32_t	bit_flag;
	pet_suppl_info_t	suppl_info;
} __attribute__ ((packed)) pet_t;

typedef struct animal_attr{
	uint32_t number;
	uint32_t itemid;
	uint32_t sickflag;
	uint32_t value;
	uint32_t eat_time;
	uint32_t drink_time;
	uint32_t output_cnt;
	uint32_t output_time;
	uint32_t update_time;
	uint32_t mature_time;
	uint32_t animal_type;
	uint32_t calc_favor_time;
	uint32_t favor;
	uint32_t outgo;
	uint32_t other_attr;
	uint32_t remaind_pollinate_cnt;
	uint32_t last_pollinate_time;
	uint32_t max_output;
	uint32_t diff_mature;
	uint32_t cur_grow;
	uint32_t lvl;
} __attribute__ ((packed)) animal_attr_t;

typedef struct cp_attr_s
{
	uint32_t pig_id;
	uint32_t item_id;
	uint32_t state;
	uint32_t transform;
	uint32_t transform_time;
	uint32_t dress1;
	uint32_t dress2;
}__attribute__ ((packed)) cp_attr_t;

typedef union animal_follow_u
{
	animal_attr_t   tag_animal;
	cp_attr_t       tag_cp;
}__attribute__ ((packed)) animal_follow_t;;
#define MAX_ITEM_BONUS 10
/*
typedef struct game_group {
	uint32_t		id;
	uint8_t 		type;
	uint8_t			flag;
	uint8_t			count;
	struct sprite*	players[MAX_PLAYERS_PER_GAME];
	int				leavegame_pos_x;
	int				leavegame_pos_y;
	// For Search
	list_head_t		hash_list;
	list_head_t		timer_list;
	
	struct game*	game;
	uint32_t		start;
	uint32_t		end;
	void*			game_handler;
	struct map_item_group*	mig;
} game_group_t;

typedef struct  game { 
	int			id;
	int			players;
	char		name[32];
	char		file[128];
	int 		pk;

	int			yxb;
	int			strong;
	int			IQ;
	int			lovely;
	int			exp;
	int			score;
	uint32_t	aux_item;
	int			aux_yxb;

	int			itembonus[MAX_ITEM_BONUS];
	int			bonus_cnt;
	int			bonus_rate[MAX_ITEM_BONUS];
	int			bonus_rate_numerator[MAX_ITEM_BONUS];
	int			bonus_cond[2];

	int			tm;
	int			coins_per_min;

	//list_head_t	group_list;
	void*		handle;
	int			(*game_init)();
	void		(*game_destroy)();
	int			(*on_game_begin)(struct sprite*);
	void		(*on_game_end)(struct game_group*);
	int			(*on_game_data)(struct sprite*, int cmd, const uint8_t *body, int len);
} game_t;

typedef struct map_item_group {
	uint8_t			type   : 5;
	uint8_t			action : 3;
	uint8_t			flag;

	//for game
	game_t*			game;	
	game_group_t*	ggp;

	//for items search
	uint8_t		itm_count;
	int			itm_pos;
	int			leavegame_pos_x;
	int			leavegame_pos_y;
	//all groups
	list_head_t		list;
} map_item_group_t;

typedef struct map_item {
	uint8_t		id;

	uint8_t		occupied  : 4;
	uint8_t		direction : 4;
	map_item_group_t*	mig;

	//all items in one map
	list_head_t	list;
} map_item_t;

struct entry_pos {
	int			map;
	int			x;
	int			y;
};

typedef struct map {
	map_id_t	id;
	uint8_t		flag;
	char		name[64];
	int			party_period;

	int			sprite_num;
	list_head_t	sprite_list_head;
	list_head_t	hash_list;
	int			entry_num;
	union {
		struct entry_pos 	entries[MAX_ENTRY_NUM_PER_MAP];
	}u;

	uint8_t			pet_cnt		: 3;
	uint8_t			pet_loaded	: 1;
	struct pet		pets[MAX_PETS_PER_SPRITE];
	//for item - To Be Removed
	int				item_count;
	list_head_t		item_list_head;
	list_head_t		timer_list;
	list_head_t		ritem_list;
	time_t			next_flight;
	uint32_t 		open_hour;
	uint32_t		close_hour;
	uint32_t		door_map;

	// for Scene Game
	game_group_t*	gamegrp;
	uint32_t		blood;
	uint8_t			rick_attack_tag[3];
} map_t;
*/
typedef struct SpriteTmpInfo {
	uint32_t reserved1;
	uint32_t reserved2;
	uint32_t reserved3;
	uint32_t reserved4;
} sprite_tmp_info_t;

typedef struct _lahm_student{
	uint8_t    id;
	uint8_t    latent;
	uint8_t    moral;
	uint8_t    iq;
	uint8_t    sport;
	uint8_t    art;
	uint8_t    labor;
	uint8_t    sl_flag;
}__attribute__(( packed ))lahm_student_t;

typedef struct DungeonDropItemInfo_s
{
	uint32_t item_id;
	uint8_t item_count;
} dungeon_item_info_t;

typedef struct DungeonRandomElementCounter_s
{
	uint32_t element_id;		//包括元素类型和ID
	uint8_t counter;
} element_counter_t;
typedef struct DungeonRandomElementPos_s
{
	uint8_t element_id;
	uint8_t pos;
} element_pos_t;


typedef struct
{
	uint32_t prize_id;
	uint32_t prize_cnt;
	uint32_t rate;
}__attribute__((packed)) one_card_t;


typedef struct sprite {
	int			fd;
	struct fdsession*	fdsess;

	userid_t		id;
	char			nick[USER_NICK_LEN];
	userid_t		inviter_id;
	uint32_t		invitee_cnt;
	uint32_t		old_invitee_cnt;
	uint32_t		dining_flag; 
	uint32_t		dining_level;

	uint32_t		flag; // VIP, SMC, Lock Home, Mole Messenger, Deny Req of Adding Friends, used to be VIP
	uint32_t 		vip_level;
	uint32_t 		vip_month;
	uint32_t 		vip_value;
	uint32_t 		vip_end;
	uint32_t		vip_autopay;
	
	uint32_t		birth;
	uint32_t		color;

	uint32_t		exp;
	uint32_t		strong;
	uint32_t		iq;
	uint32_t		lovely;
	uint32_t		game_king;
	uint32_t		yxb;
	uint32_t		engineer;
	uint32_t 		fashion;
	uint32_t		cultivate_skill;
	uint32_t		breed_skill;
	uint32_t		cult_day_limit;
	uint32_t		breed_day_limit;
	int16_t	 		npc_lovely[MAX_NPC];
	uint32_t        medal; // super mushroom guider's civilizition medal count'

	uint32_t		olcount; //login count
	uint32_t		oltoday; // total time last day online 
	uint32_t		ollast;  // the last time to login 
	uint32_t		oltime;  // total time on line

	uint32_t 		birthday; // used for shengri feiting

	uint32_t 		ques_id; // used for ask and answer
    uint32_t        ques_correct_cnt;
    uint32_t        ques_total_cnt;
    uint8_t        	ques_ans_flag;
    
	uint32_t		my_molegroup_num;
	uint32_t		my_molegroup_id[MAX_OWNED_MOLEGROUP_NUM];

    uint32_t        my_moleclass_num;
    uint32_t        my_moleclass_first;
	uint32_t		my_moleclass_id[MAX_OWNED_MOLECLASS_NUM];
	uint32_t 		architect_exp;
	uint32_t		profession[50];
	uint32_t		only_one_flag[8];
	uint32_t		old_last_time;
	uint32_t		restaurant_evt;
	uint32_t		sendmail_flag;
	
    //tmp: class ques
    uint32_t        class_id_for_ques;
    uint32_t        class_ques_id_start;

	uint8_t			month_task[MAX_MONTH_TASKS];
	uint8_t			tasks[150];
	uint32_t 		magic_task; // 1, have learning magic task pet, else no pet learning
	uint32_t		pet_task; //1,have task going on, else no task going on
	uint32_t		stamp;  // login time
	uint8_t			login		: 1;
	uint8_t			direction	: 3;

	int8_t			item_cnt;
	uint32_t		items[MAX_ITEMS_WITH_BODY];

	//for map
	uint16_t		posX;
	uint16_t		posY;
	void*			tiles;
	uint32_t		action;
	uint32_t		action2;
	uint32_t 		sub_grid;
    
    uint32_t        car_status;//0, no car; 1, driving;
    uint32_t        driver_time;//
    car_t           car_driving;

    uint32_t        student_count;
    lahm_student_t  students[STUDENT_COUNT_MAX];
    uint32_t        teach_exp;
	uint32_t		exam_num;
	uint32_t		classroom_student;

	uint32_t        dietitian_exp;
    uint32_t        animal_nbr;
    animal_follow_t	animal;

	uint32_t 		dragon_id;
	uint8_t			dragon_name[16];
	uint32_t		dragon_growth;
	uint32_t		team_id;
	list_head_t		hash_list;
	list_head_t		timer_list;
	list_head_t		map_list;
	list_head_t		hero_list;

	//for angel fight
	uint32_t		fight_exp;		//angel fight all exp
	uint32_t		fight_level;	//angel fight level
	uint8_t			get_prize;
	uint8_t			get_apprentice;	//can get an apprentice
	uint32_t		old_master_id;
	uint32_t		apprentice_id;
	one_card_t		card[5];

	//cmd process system
	uint16_t		waitcmd; //current wait cmd
	void* 			pending_cmd_queue; //not handled cmd list, in FIFO order, 30 is the max queue length
	list_head_t 	busy_sprite_list_hook; //intrusive list hook

	void*			group;
	void*			gitem;
	struct pet*			followed;
	time_t			last_show_item;
	time_t			last_action_time;
	// identify if a user has been in a game, and if in, identify
	// if the user is a game watcher or a game player.
	uint8_t			game_status;
	time_t			game_start_time;
	uint16_t 			r4_type;
	uint16_t 			r4_cnt;
	uint32_t			tmp_fish_weight;
	// [0] for itemid, the remaining for game msg - use session instead
	// uint8_t			game_msg[17]; // for single player interactive game
	sprite_tmp_info_t   tmpinfo;
	
	uint8_t				daily_limit[MAX_DAILY_COUNT];
	void*				db_timer;
	uint16_t			db_timer_waitcmd; //timer wait cmd
	uint16_t			db_timer_dbcmd; //timer wait cmd
	uint8_t 			ver;
	uint8_t				pend_flag;
	void*				lamu_action_timer;
	uint32_t			lamu_action;
	uint32_t			game_gate;
	uint32_t			animal_count;
	uint32_t        	paradise_leve;
	uint32_t        	td_game_start_time;
	uint32_t        	td_task_id;     //td 任务id
	uint32_t            td_task_mode;
	uint32_t        	killed_monster_count;//杀死怪物数量
	uint32_t			drop_award_count;    //怪物掉落物品数量
	uint32_t        	get_award_count;     //得到奖励数量
	
	uint32_t            td_award_left;	
	uint32_t        	clean_angel_game_start_time; //净化游戏开始
	uint32_t            user_talk_flag; //talk flag
	uint32_t            ban_talk_time;  //ban talk start time
	uint32_t            cur_talk_idx;
	uint32_t            talk_time[MAX_TALK_COUNT_IN_LIMIT_TIME];

	uint32_t			dungeon_on_explore_map_id;								//正在挖掘的地图ID
	uint32_t			dungeon_on_explore_screen_id;							//子地图ID
	uint32_t			dungeon_explore_hp;
	uint32_t			dungeon_explore_exp;									//挖宝经验值
	uint32_t			dungeon_explore_last_time_count;						//开始挖掘的时间，用于计算体力恢复
	dungeon_item_info_t dungeon_item_buf[DUNGEON_MAX_ITEM_KIND];			//挖宝道具缓存
	char				dungeon_area_explore_counter[DUNGEON_MAX_SCREEN_NUM][DUNGEON_MAX_MAP_AREA_NUM];	//区域挖掘次数累计
	element_counter_t dungeon_element_counter[DUNGEON_MAX_RANDOM_ELEMENT_TYPE];		//记录各种随机元素出现次数
	element_pos_t	dungeon_rand_element_pos[DUNGEON_MAX_SCREEN_NUM][DUNGEON_MAX_RANDOM_ELEMENT_COUNT_PER_SCREEN];//记录出现的随机元素的位置信息
	char				dungeon_area_id_for_boss;								//记录boss对战的区域ID
	uint32_t		dungeon_enter_map_time;									//记录进入游戏的时间
	uint8_t			dungeon_explore_failed_counter;
	uint32_t		dungeon_explore_total_counter;
	uint32_t		has_state;												//捉迷藏游戏状态记录
	uint32_t		play_drum_time;
	uint32_t		water_bomb_time;		//上次丢水弹的时间
	uint32_t		cp_level;				//猪倌等级
	uint32_t		cp_exp;					//猪倌的经验值
	uint32_t		cp_last_lucky_time;		//猪倌上次获取随机好运时间
	uint8_t         cp_win_beauty_show;     //1win 2faild
	uint8_t			sg_guidering_flag;
    uint32_t		sg_need_help_id;
	uint8_t         snowball_flag;
	uint8_t			attack_cnt;
	uint8_t         fp_flower_idx;
	uint8_t         fp_can_get_award;
	uint32_t        ac_interactive_user_id[2];
	uint16_t		sess_len;
	uint8_t			session[];
} __attribute__ ((packed)) sprite_t;

typedef struct sprite_to_online{
	userid_t		id;
	char			nick[USER_NICK_LEN];
	userid_t		inviter_id;
	uint32_t		invitee_cnt;
	uint32_t		old_invitee_cnt;

	uint32_t		flag; // VIP, SMC, Lock Home, Mole Messenger, Deny Req of Adding Friends, used to be VIP
	uint32_t		color;
	uint8_t			login		: 1;
	uint8_t			direction	: 3;

	int8_t			item_cnt;
	uint32_t		items[MAX_ITEMS_WITH_BODY];

	//for map
	uint16_t		posX;
	uint16_t		posY;
	void*			tiles;
	uint32_t		action;
	uint32_t		action2;
	uint32_t 		sub_grid;
    
    uint32_t        car_status;//0, no car; 1, driving;
    car_t           car_driving;

    uint32_t        student_count;
    lahm_student_t  students[STUDENT_COUNT_MAX];
    uint32_t        teach_exp;
	uint32_t		exam_num;

	uint32_t        dietitian_exp;

    uint32_t        animal_nbr;
    animal_attr_t   animal;

	uint32_t 		dragon_id;
	uint8_t			dragon_name[16];
	uint32_t		dragon_growth;
	uint32_t		dungeon_explore_exp;
	uint32_t		team_id;

	//cmd process system
	void*			group;
	pet_t*			followed;
	sprite_tmp_info_t   tmpinfo;
	uint32_t			lamu_action;
}__attribute__ ((packed)) sprite_to_online_t;
// to cache sprite's basic info
typedef struct SpriteInfo {
	sprite_t		player;
	pet_t			pet;
} __attribute__((packed)) sprite_info_t;

typedef struct SpriteOnline {
	sprite_info_t 	sprit;
	uint32_t		onlineid;
} __attribute__((packed)) sprite_onli_t;

typedef struct CommunicatorBody {
	map_id_t    mapid;
	uint32_t	opid;
	uint8_t		body[];
} __attribute__((packed)) communicator_body_t;

extern fdsession_t* online[MAX_ONLINE_NUM];

static inline int send_to_online(int cmd, int body_len, const void* body_buf, userid_t id, map_id_t mapid, uint32_t opid, uint32_t online_id)
{
	static uint8_t bcpkg[pkg_size];

	int len = sizeof(communicator_header_t) + body_len;
	if ( !online[online_id] ||
			(body_len > sizeof(bcpkg) - sizeof(communicator_header_t)) ) {
		ERROR_RETURN( ("Failed to Send Pkg, fd=%d", online[online_id]->fd), -1 );
	}

	communicator_header_t* pkg = (void*)bcpkg;
	pkg->len        = len;
	pkg->online_id  = online_id;    // 0 is the homeserver
	pkg->cmd        = cmd;
	pkg->ret        = 0;
	pkg->id         = id;
	pkg->mapid      = mapid;
	pkg->opid       = opid;
	memcpy(pkg->body, body_buf, body_len);

	return send_pkg_to_client(online[online_id], bcpkg, len);
}

void send_to_online_map(int cmd, int body_len, const void* body_buf, sprite_onli_t* aso, sprite_where_t* asw, uint32_t opid);
int sprite_info_chg_op(void* buf, int len, fdsession_t* fdsess);
void init_communicator();
void clean_all_users_onlinex_down(int fd);

#endif // SWITCH_ONLINE_HPP_

