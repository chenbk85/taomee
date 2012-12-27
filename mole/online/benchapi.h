#ifndef ONLINE_BENCHAPI_H_
#define ONLINE_BENCHAPI_H_

// C89
#include <time.h>
// C99
#include <stdint.h>
#include <glib.h>

#include <libtaomee/timer.h>
#include <libtaomee/list.h>

#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif

#define MAX_ENTRY_NUM_PER_MAP		32
#define MAX_PLAYERS_PER_GAME		20
#define MAX_PETS_PER_SPRITE			4
#define MAX_WATCHERS_PER_GAME		20
#define MAX_ITEMS_WITH_BODY			12
#define USER_NICK_LEN				16
#define DRAGON_NICK_LEN				16
#define MAX_OWNED_MOLEGROUP_NUM		3
#define MAX_OWNED_MOLECLASS_NUM		10
#define MAX_MONTH_TASKS				48
#define DEFENSE_TYPE_MAX			5
#define MAX_DAILY_COUNT				3
#define START_DAY_LIMIT				1001
#define END_DAY_LIMIT				1003
#define SESS_LEN					16
#define MAX_NPC						20
#define BLACK_USERID_LIST_MAX		100
#define STUDENT_COUNT_MAX  	30
#define	DUNGEON_MAX_ITEM_KIND						48			//掉落物品最大类型数
#define DUNGEON_MAX_MAP_AREA_NUM					64			//每屏幕最大探索区域数目
#define	DUNGEON_MAX_RANDOM_ELEMENT_TYPE 			32			//随机元素类型总数
#define DUNGEON_MAX_SCREEN_NUM						10			//最大屏幕数目
#define DUNGEON_MAX_RANDOM_ELEMENT_COUNT_PER_SCREEN 5			//每张地图的最大随区域数目

#define MAX_TALK_COUNT_IN_LIMIT_TIME				10


#define FIRST_VERSION	0x61

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
	animal_attr_t	tag_animal;
	cp_attr_t		tag_cp;
} __attribute__ ((packed)) animal_follow_t;;

struct map_item;
struct pet;
struct fdsession;
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

typedef struct _lahm_student{
	uint8_t id;
	uint8_t latent;
	uint8_t moral;
	uint8_t iq;
	uint8_t sport;
	uint8_t art;
	uint8_t labor;
	uint8_t sl_flag;
}__attribute__(( packed ))lahm_student_t;


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

typedef struct SpriteTmpInfo {
	uint32_t reserved1;
	uint32_t reserved2;
	uint32_t reserved3;
	uint32_t reserved4;
} sprite_tmp_info_t;

#define ITEM_MAX_COUNT  10
typedef struct CandyInfo {
	uint32_t itmid;
	uint32_t itmcn;
}candy_info_t;

typedef struct SpriteTeamInfo {
	uint32_t team;
	uint32_t huizhang_cnt;
	uint32_t score;
} sprite_team_info_t;

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
typedef uint64_t map_id_t;
typedef uint32_t userid_t;

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
	uint32_t		vip_level;
	uint32_t		vip_month;
	uint32_t		vip_value;
	uint32_t		vip_end;
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
	uint32_t		fashion;
	uint32_t		cultivate_skill;
	uint32_t		breed_skill;
	uint32_t		cult_day_limit;
	uint32_t		breed_day_limit;
	int16_t			npc_lovely[MAX_NPC];
	uint32_t 		medal; // super mushroom guider's civilizition medal count

	uint32_t		olcount;	//login count
	uint32_t		oltoday;	//total time last day online
	uint32_t		ollast;		//the last time to login
	uint32_t		oltime;		//total time on line

	uint32_t		birthday;	//used for shengri feiting

	uint32_t		ques_id;	//used for ask and answer
	uint32_t		ques_correct_cnt;
	uint32_t		ques_total_cnt;
	uint8_t			ques_ans_flag;

	uint32_t		my_molegroup_num;
	uint32_t		my_molegroup_id[MAX_OWNED_MOLEGROUP_NUM];

	uint32_t		my_moleclass_num;
	uint32_t		my_moleclass_first;
	uint32_t		my_moleclass_id[MAX_OWNED_MOLECLASS_NUM];
	uint32_t		architect_exp;
	uint32_t		profession[50];
	uint32_t		only_one_flag[8];
	uint32_t		old_last_time;
	uint32_t		restaurant_evt;
	uint32_t		sendmail_flag;

	//tmp: class ques
	uint32_t		class_id_for_ques;
	uint32_t		class_ques_id_start;

	uint8_t			month_task[MAX_MONTH_TASKS];
	uint8_t			tasks[150];
	uint32_t		magic_task;	//1, have learning magic task pet, else no pet learning
	uint32_t		pet_task;	//1,have task going on, else no task going on
	uint32_t		stamp;		//login time
	uint8_t			login		: 1;
	uint8_t			direction	: 3;

	int8_t			item_cnt;
	uint32_t		items[MAX_ITEMS_WITH_BODY];

	//for map
	uint16_t		posX;
	uint16_t		posY;
	struct map*		tiles;
	uint32_t		action;			//行动状态，以及变身状态
	uint32_t		action2;
	uint32_t		sub_grid;

	uint32_t		car_status;		//0, no car; 1, driving;
	uint32_t		driver_time;	//
	car_t			car_driving;

	uint32_t		student_count;
	lahm_student_t	students[STUDENT_COUNT_MAX];
	uint32_t		teach_exp;
	uint32_t		exam_num;
	uint32_t		classroom_student;

	uint32_t		dietitian_exp;	//鍋ュ悍钀ュ吇甯堢粡楠?
	uint32_t		animal_nbr;
	animal_follow_t	animal_follow_info;

	uint32_t		dragon_id;
	uint8_t			dragon_name[DRAGON_NICK_LEN];
	uint32_t		dragon_growth;
	uint32_t		team_id;
	uint32_t		mvp_team;
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
	uint16_t		waitcmd;				//current wait cmd
	GQueue* 		pending_cmd_queue;		//not handled cmd list, in FIFO order, 30 is the max queue length
	list_head_t	busy_sprite_list_hook;	//intrusive list hook

	struct game_group*	group;
	struct map_item*	gitem;
	struct pet*			followed;
	time_t				last_show_item;
	time_t				last_action_time;
	// identify if a user has been in a game, and if in, identify
	// if the user is a game watcher or a game player.
	uint8_t				game_status;
	time_t				game_start_time;
	uint16_t			r4_type;
	uint16_t			r4_cnt;
	uint32_t			tmp_fish_weight;
	// [0] for itemid, the remaining for game msg - use session instead
	// uint8_t			game_msg[17]; // for single player interactive game
	sprite_tmp_info_t	tmpinfo;
	//sprite_team_info_t		teaminfo;
	uint8_t				daily_limit[MAX_DAILY_COUNT];
	timer_struct_t*		db_timer;
	uint16_t			db_timer_waitcmd; //timer wait cmd
	uint16_t			db_timer_dbcmd; //timer wait cmd
	uint8_t				ver;
	uint8_t				pend_flag;
	timer_struct_t		*lamu_action_timer;
	uint32_t			lamu_action;
	uint32_t			game_gate;		//璁板綍绔犻奔鎵戞墸娓告垙鐨勫叧鍗℃暟
	uint32_t			animal_count;	//淇濆瓨鍦ｅ厜鍏界殑鏁扮洰
	uint32_t			paradise_leve;
	uint32_t			td_game_start_time;
	uint32_t			td_task_id;				//td 任务id
	uint32_t			td_task_mode;			//休闲模式0，挑战模式1
	uint32_t			killed_monster_count;	//杀死怪物数量
	uint32_t			drop_award_count;		//怪物掉落物品数量
	uint32_t			get_award_count;		//得到奖励数量
	//天使挑战赛
	uint32_t			td_award_left;			//通关后可获取奖励次数
	uint32_t			clean_angel_game_start_time; //净化游戏开始
	uint32_t			user_talk_flag;	//talk flag
	uint32_t 			ban_talk_time;	//ban talk start time
	uint32_t			cur_talk_idx;
	uint32_t			talk_time[MAX_TALK_COUNT_IN_LIMIT_TIME];

	uint32_t			dungeon_on_explore_map_id;								//正在挖掘的地图ID
	uint32_t			dungeon_on_explore_screen_id;							//子地图ID
	uint32_t			dungeon_explore_hp;
	uint32_t			dungeon_explore_exp;									//挖宝经验值
	uint32_t			dungeon_explore_last_time_count;						//开始挖掘的时间，用于计算体力恢复
	dungeon_item_info_t dungeon_item_buf[DUNGEON_MAX_ITEM_KIND];			//挖宝道具缓存
	char				dungeon_area_explore_counter[DUNGEON_MAX_SCREEN_NUM][DUNGEON_MAX_MAP_AREA_NUM];	//区域挖掘次数累计
	element_counter_t	dungeon_element_counter[DUNGEON_MAX_RANDOM_ELEMENT_TYPE];		//记录各种随机元素出现次数
	element_pos_t		dungeon_rand_element_pos[DUNGEON_MAX_SCREEN_NUM][DUNGEON_MAX_RANDOM_ELEMENT_COUNT_PER_SCREEN];//记录出现的随机元素的位置信息
	char				dungeon_area_id_for_boss;								//记录boss对战的区域ID
	uint32_t			dungeon_enter_map_time;									//记录进入游戏的时间
	uint8_t				dungeon_explore_failed_counter;
	uint32_t			dungeon_explore_total_counter;
	uint32_t			has_state;												//捉迷藏游戏状态记录
	uint32_t			play_drum_time;
	uint32_t			water_bomb_time;		//上次丢水弹的时间		冬天vs春天阵营 上次攻击时间
	uint32_t			cp_level;				//猪倌等级
	uint32_t			cp_exp;					//猪倌的经验值
	uint32_t			cp_last_lucky_time;		//猪倌上次获取随机好运时间
	uint8_t				cp_win_beauty_show;		//1win 2faild
	uint8_t				sg_guidering_flag;
	uint32_t			sg_need_help_id;
	uint8_t				snowball_flag;
	uint8_t				attack_cnt;				//被攻击次数 for 冬天vs春天
	uint8_t				fp_flower_idx;
	uint8_t				fp_can_get_award;
	uint32_t			ac_interactive_user_id[2];		//临时记录已交互玩家ID
	uint32_t            ocean_drop_item[10]; // drop item and count;
	uint32_t            ocean_drop_count[10]; // drop item and count;
	uint32_t             invade_monster;// ocean chapter invade monster;
	uint32_t             monster_hp;//invade monster's hp
	uint32_t             adventure_interval;// interval of adventure mode
	uint32_t             game_exp;//game exp
	uint16_t			sess_len;
	uint8_t				session[];

} __attribute__ ((packed)) sprite_t;

#define MAX_ITEM_BONUS 10

typedef struct  game {
	int			id;
	int			players;
	char		name[32];
	char		file[128];
	int 		pk;
	int			vip;
	int			game_bonus_id;
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

#define SPRITE_SESS_AVAIL_LEN	(SPRITE_STRUCT_LEN - sizeof(sprite_t))
#endif
