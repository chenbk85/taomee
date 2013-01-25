#ifndef BT_ONLINE_H
#define BT_ONLINE_H
#include  <list>
#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <async_serv/net_if.h>
#include <libtaomee/timer.h>
#include <libtaomee/list.h>


//#include "./proto/mole2.h"
#include "onlineapi.h"
#include "proto.h"
#include "utils.h"
/**************lua include file**********************/
#include <stdio.h>
#include <math.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
/**************lua include file**********************/
#ifdef __cplusplus
}
#endif
#include <libtaomee++/proto/proto_util.h>
extern int t_users[10][4] ;

#define TYPEID_2008 2008  

#define TYPEID_2004 2004  
#define TYPEID_3017 3017 
#define TYPEID_2005 2005
#define TYPEID_2012 2012
#define TYPEID_2013 2013

#define MAX_FD_NUM		30000
#define MAX_ONLINE_NUM	1200 + 1

#define MAX_FANJI_CNT			10
//在行动回合包中使用
#define UPDATE_WARRIOR_STATE	10000 
#define LAST_HEJI_WARR			(MAX_PLAYERS_PER_TEAM * 2)

#define MAX_SKILL_NUM        50000

#define MAX_TOPIC_TYPE       40
#define MAX_TOPIC_NUM        500
#define MAX_SKILL_NUM_W_MIN  100000
#define MAX_SKILL_LEVEL		 10

#define MAX_PROF_NUM		10	
#define MAX_WEAPON_NUM		8

#define T_USER_MAX_NUM		4
#define COL_USER_MAX_NUM		2
#define ROW_USER_MAX_NUM		5

#define     IS_REVIVING_MEDICAL(id) ((id)>=210012 && (id) <=210014)
//职业编号
enum{
	prof_none = 0,
	prof_swordman, // jian
	prof_ancher,   // gong jian shou
	prof_magician, // mofashi
	prof_churchman, // chuan jiao shi
};

//施加范围
enum {
	one_enemy	= 0,
	t_enemy		= 1,
	c_enemy		= 2,
	r_enemy		= 3,
	a_enemy		= 4,
};

//是否需要盾牌
enum {
	no_need_shield = 0,
	must_have_shield = 1,
	must_not_shield = 2,
};

//target_type
enum {
	self_only = 1,
	except_self = 2,//除自己以外
	self_team = 3,
	enemy_team = 4,
	all_attacked = 5,
	self_team_except_self = 6,
};

//usr_type,技能的使用者限制
enum {
	user_ren_only	= 0,//只有人使用
	user_pet_beast	= 1,//宠物和怪使用
	user_beast_only	= 2,//只用怪使用
};

//操作对象的生死类型
enum {
	attack_only_alive = 0,
	attack_only_dead = 1,
	attack_only_both = 2,
};
//输赢
enum {
	standoff	= 0,
	victory		= 1,
	fail		= 2,
};


enum {
	battle_challenging  = 0,
	battle_ready 		= 1,
	battle_started		= 2,
};
//单回合状态:one_loop_state
enum {
	one_loop_bit_huwei = 0,
	one_loop_bit_huiji = 1,
};

enum{
	btl_op_res_error = 0, // 错误
	btl_op_res_wait_skill = 1, // 等待发招
	btl_op_res_new_loop = 2, // 新一轮开始
	btl_op_res_over = 3, // 对战结束
};





enum skill_type{	
	physi_attack				= 1,
	mp_attack					= 2,
	defense						= 3,
	hp_resume					= 4,
	fh_magic					= 6,//吸血魔法
	magic_attack				= 7,
	
	abnormal_attack				= 8,//异常状态攻击
	set_fanyyu_state			= 9,//防御状态赋予
	use_medical					= 10,
	change_position             = 11,
	catch_pet_opt               = 12,
	recall_pet                  = 13,
	daiji                       = 14,//待机14
	xixue_gongji                = 15,//吸血攻击
	runaway               		= 16,//逃跑
};

//水晶属性
typedef struct pet_birth {
	uint8_t				earth;
	uint8_t				water;
	uint8_t				fire;
	uint8_t				wind;
}__attribute__((packed))pet_birth_t;






struct skill_id_level_t{
	uint32_t skill_id  ;
	uint32_t skill_level;
};

struct auto_skill_info_t{
	uint32_t skill_id  ;
	uint32_t skill_level;
	int32_t pos      ;
	uint32_t mak	  ;
	bool 	use_medical;//是否使用药品
};

typedef struct atk_info {
    uint8_t             atk_seq;//0,1

    uint32_t             atk_mark; // 0 self, 1 ememy
    int32_t              atk_pos;

    uint32_t            atk_type;
    uint8_t             atk_level;
    uint8_t             heji_seq;//atk_info_t[i]
    struct warrior*     wrior;//

    // this attack type is already deal
    uint8_t             atk_deal;//已经处理,该标志未使用
    // 
    uint8_t             atk_step;//表示打到第几次 目前用于( 追击)，
    uint8_t             confrm_decre_mp;//扣蓝(魔)方式 
    int16_t             atk_speed;//
    list_head_t         atk_info_list;//node
	inline bool  is_opt_self_team() {
		return this->atk_mark==0;
	}	
}__attribute__((packed))atk_info_t;

enum pet_action_state
{
	// 在小屋里面
	// 注意online不可能传送一个带这个状态位的宠物过来，否则就是bug
	in_warehouse = 0,
	// 在背包里面
	just_in_bag=1,
	// 待命状态
	on_standby=2,
	// 战斗状态，online传送过来的宠物中，仅且仅有一只宠物携带有该状态位
	rdy_fight=3,
	on_fuzhu=4, //辅助
};

/*受伤状态*/
enum injury_state
{
	// 健康
	injury_health = 0,
	// 白伤
	injury_white  = 1,
	// 黄伤
	injury_yellow = 2,
	// 红伤
	injury_red    = 3,
};

enum
{
	// 还没有扣蓝(魔法值 )
	confrm_decre_mp_no_need = 0,
	// 等到实际行动后在扣蓝(魔法值 )
	confrm_decre_mp_after_atk = 1,
	// 预先扣蓝(魔法值)
	confrm_decre_mp_when_set_state=2,
};

enum {
	bit_attacked = 2,
	bit_not_in_battle = 4,
	bit_attacked_dumy = 8,
	bit_user_set_skill =16, //自己手工设置技能
	bit_dubaoshu	=32, //用于毒爆术使用后是否使用过
};

class Cuser_skill_value{
	public:
		uint32_t skillid;
};


//种族配置
enum enum_race{
	race_yeshou=5,//野兽族
	race_undead=7,//不死族
};

#define WARRIOR_FLAG_IS_DOUBLE_ACK  0x01
#include    "Cwarrior_state.h"


// 调整该结构时，要小心entry_len, 在初始化warrior 时会使用到它，
typedef struct warrior {
	public:

	userid_t				userid;
	uint32_t				petid;
	uint32_t				type_id;
	char					nick[USER_NICK_LEN];
	uint32_t				warrior_flag;//0x01:是不是可以攻击两次
	uint32_t				vip;//vip
	uint32_t 				color;
	uint8_t					race;//种族 5:野兽
	uint8_t					prof;//职业
 	uint32_t				level;//等级
	
	uint16_t				physique;//体力
	uint16_t				strength;//力量
	uint16_t				endurance;//耐力
	uint16_t				quick;//敏捷
	uint16_t				intelligence;//智力
	uint16_t				attr_addition;//属性配点
	int32_t					hp;
	int32_t					mp;
	uint32_t				injury_lv; // 0 1 2 3
	uint8_t					in_front;//
	uint8_t					earth;
	uint8_t					water;
	uint8_t					fire;
	uint8_t					wind;
	//second level attr
	int32_t					hp_max;
	int32_t					mp_max;
	int16_t 				attack_value;
	int16_t 				mattack_value;
	int16_t 				defense_value;
	int16_t 				mdefense_value;
	int16_t					speed;
	int16_t					spirit; // jingshen
	int16_t					resume;//恢复力，
	int16_t					hit_rate;//命中率，不使用
	int16_t					avoid_rate; //不用
	int16_t					bisha;
	int16_t					fight_back;//反击 

	int16_t					r_zhongdu;
	int16_t					r_shihua;
	int16_t					r_hunshui;
	int16_t					r_zuzhou;
	int16_t					r_hunluan;
	int16_t					r_yiwang;

	uint8_t					is_need_add_protect_exp;//是否需要增加守护经验,如果到达每日上限则设置为0
	uint8_t					cloth_cnt;
	body_cloth_t			clothes[MAX_ITEMS_WITH_BODY];
	uint8_t					skill_cnt;
	skill_info_t			skills[MAX_SKILL_NUM_PER_PERSON];
	

	// uint32_t                          beibaochong_cnt;
	uint32_t				weapon_type;
	uint32_t				shield; // 盾牌

	uint8_t					pet_cnt;    // bei bao chong wu shuliang
	uint16_t				catchable;  // if can be catch 0 unable, >0 able
	uint32_t				handbooklv; // chongwu tu jian
	uint32_t                pet_contract_lv; // jing ling qiyue
	uint8_t                 pet_state; // chong wu zhuangtai
	uint32_t				last_atk_type[2];
	uint8_t					last_atk_level[2];
	uint32_t				auto_atk_rounds;
	 
	uint32_t 				change_race;
	uint32_t 				change_petid;
	//void read_from_struct_warrior(  stru_warrior_t  & w  );
	
	// 以下部分是程序自行定义的数据, 
	uint32_t 				skills_use_count[MAX_SKILL_NUM_PER_PERSON];
	int						skills_forgot_cnt;
	uint32_t				skills_forgot[18];
	uint8_t					load_rate;
	uint8_t                 ready;
	uint32_t                load_time;
	atk_info_t				atk_info[2];
	int16_t					heji_hurt;
	Cwarrior_state			*p_waor_state;
	list_head_t				waor_list;
	list_head_t			    timer_list;

	list_head_t				huwei_list_header;
	list_head_t				huwei_list_node;
	int8_t                  huwei_pos;

	struct battle_users*	self_team;
	struct battle_users* 	enemy_team;
	uint32_t				flag;
	uint8_t					ruodiantouxi_lv;//被使用弱点透析技能的等级
	//for cal in attack 保存攻击时相关微调值
	uint8_t					add_hit_rate;//100

	//是不是要攻击两次
	inline bool is_double_ack(){
		//DEBUG_LOG("is_double_ack:%u",this->warrior_flag );
		return this->warrior_flag&WARRIOR_FLAG_IS_DOUBLE_ACK;
	}



	inline  bool flag_check_bit( uint32_t bit_value ){
		return (this->flag & bit_value)==bit_value ;
	}
	inline  void  flag_set_bit( uint32_t bit_value,bool value ){
		if (value ){
			this->flag |= bit_value ;
		}else{
			this->flag &= ~bit_value;
		}
	}

	//是否怪物技能ai已经处理
	inline bool is_beast_deal_attacked (){
		return this->flag_check_bit(0x01);
	}

	//设置是否怪物技能ai已经处理
	inline void set_is_beast_deal_attacked(bool value ){
		this->flag_set_bit(0x01,value );
	}

	inline bool pet_not_in_battle_mode() {
		return (this->petid && (this->pet_state != rdy_fight && this->pet_state != on_fuzhu ));
	}
//	设置不在参与战斗
	inline void set_not_in_battle_mode(bool value ){
		this->flag_set_bit(bit_not_in_battle,value );
	}

//	检查不在参与战斗
	inline bool is_not_in_battle_mode() {
		return this->flag_check_bit(bit_not_in_battle);
	}
	inline bool check_state(uint8_t state_bit  ){
		
		if ( this->p_waor_state){
			 return this->p_waor_state->check_state(state_bit );
		}else{
			return false;
		}

	}
//
	inline void set_dubaoshu_bit(bool value ){
		this->flag_set_bit(bit_dubaoshu,value );
	}

//	
	inline bool get_dubaoshu_bit() {
		return this->flag_check_bit(bit_dubaoshu);
	}
//	设置不在参与战斗
	inline void set_attack_dumy(bool value ){
		this->flag_set_bit(bit_attacked_dumy,value );
	}

//	检查不在参与战斗
	inline bool is_attack_dumy() {
		return this->flag_check_bit(bit_attacked_dumy);
	}
	//is_self_team 是否是自己的队
	void get_t_user_list( uint32_t pos ,  bool is_self_team ,std::list<struct warrior *> &warrior_list   );
	void get_col_user_list( uint32_t pos ,  bool is_self_team ,std::list<struct warrior *> &warrior_list   );
	void get_row_user_list( uint32_t pos ,  bool is_self_team ,std::list<struct warrior *> &warrior_list   );
	bool check_mp_enough(  skill_info_t * p_skill );
	inline bool is_user(){
		return this->petid==0;
	}
	inline bool is_pet(){
		return !this->is_user();
	}
	inline bool is_zhu_pet(){
		return !this->is_user()&&this->pet_state==rdy_fight;
	}
	inline bool is_fuzhu_pet(){
		return !this->is_user()&&this->pet_state==on_fuzhu;
	}
	//
	inline bool is_boss(){
		return this->type_id%10000/1000==2;
	}



	int check_self_team_hp_enough();
	bool check_enermy_valid(struct warrior * lp);
	int get_weakest_warrior_pos();
	int get_self_weakest_warrior_pos();
	int get_most_warrior_pos();
	inline int get_level(){ return this->level; }	
	//是否在防御中
	bool is_in_fangyu() ;
} __attribute__ ((packed)) warrior_t;

typedef struct warrior_base{
	userid_t				userid;
	uint32_t				petid;
	uint32_t				type_id;
	char					nick[USER_NICK_LEN];
	uint32_t				warrior_flag;//0x01:是不是可以攻击两次
	uint32_t				vip;//vip
	uint32_t 				color;
	uint8_t					race;//种族 5:野兽
	uint8_t					prof;//职业
 	uint32_t				level;//等级
	
	uint16_t				physique;//体力
	uint16_t				strength;//力量
	uint16_t				endurance;//耐力
	uint16_t				quick;//敏捷
	uint16_t				intelligence;//智力
	uint16_t				attr_addition;//属性配点
	int32_t					hp;
	int32_t					mp;
	uint32_t				injury_lv; // 0 1 2 3
	uint8_t					in_front;//
	uint8_t					earth;
	uint8_t					water;
	uint8_t					fire;
	uint8_t					wind;
	//second level attr
	int32_t					hp_max;
	int32_t					mp_max;
	int16_t 				attack_value;
	int16_t 				mattack_value;
	int16_t 				defense_value;
	int16_t 				mdefense_value;
	int16_t					speed;
	int16_t					spirit; // jingshen
	int16_t					resume;//恢复力，
	int16_t					hit_rate;//命中率，不使用
	int16_t					avoid_rate; //不用
	int16_t					bisha;
	int16_t					fight_back;//反击 

	int16_t					r_zhongdu;
	int16_t					r_shihua;
	int16_t					r_hunshui;
	int16_t					r_zuzhou;
	int16_t					r_hunluan;
	int16_t					r_yiwang;

	uint8_t					is_need_add_protect_exp;//是否需要增加守护经验,如果到达每日上限则设置为0
	uint8_t					cloth_cnt;
	body_cloth_t			clothes[MAX_ITEMS_WITH_BODY];
	uint8_t					skill_cnt;
	skill_info_t			skills[MAX_SKILL_NUM_PER_PERSON];
	// uint32_t                          beibaochong_cnt;
	uint32_t				weapon_type;
	uint32_t				shield; // 盾牌

	uint8_t					pet_cnt;    // bei bao chong wu shuliang
	uint16_t				catchable;  // if can be catch 0 unable, >0 able
	uint32_t				handbooklv; // chongwu tu jian
	uint32_t                pet_contract_lv; // jing ling qiyue
	uint8_t                 pet_state; // chong wu zhuangtai
	uint32_t				last_atk_type[2];
	uint8_t					last_atk_level[2];
	uint32_t				auto_atk_rounds;

	uint32_t 				change_race;
	uint32_t 				change_petid;
} __attribute__ ((packed)) warrior_base_t;

typedef struct battle_users {
	uint32_t		teamid;
	uint8_t			wined_count;  //连胜次数 
	uint8_t			victory;  // 1 victory
	uint8_t			ren_count;    // 人数 
	uint8_t         ren_alive_count; // 还活着的人数+活着npc个数
	
	uint8_t         count_ex; // ren & pet;
	uint8_t         alive_count; // ren & pet alive

	uint8_t         atk_out;

	uint16_t        ave_lvl; // ping jun dengji
	
	uint8_t         npc_fri_cnt_alive;//活着npc个数
	warrior_t*		players_pos[MAX_PLAYERS_PER_TEAM * 2];
	warrior_t		players[MAX_PLAYERS_PER_TEAM * MAX_GROUP_PLAYERS];
	uint32_t		online_id;
	uint32_t        team_type;
	//得到一个对手
	inline uint32_t get_one_userid(){
		for (uint32_t i=0;i<MAX_PLAYERS_PER_TEAM * MAX_GROUP_PLAYERS;i++){
			if( this->players[i].userid>=50000){
				return this->players[i].userid;
			}
		}
		return 0;
	}
}__attribute__((packed))battle_users_t;
typedef struct nest_atk {
    uint32_t        uid;//攻击者
    uint32_t        petid;//petid
    uint8_t         fantan_ret; // 9

    uint32_t        atk_type;
    uint8_t         atk_level; // 5

    int16_t         fantan_hp;
    int16_t         fantan_mp;
    int16_t         add_hp;
    uint32_t        use_itemid; // 10

    uint32_t        atk_uid; //被攻击者
    uint32_t        atk_petid;//petid
    uint64_t        fightee_state; // 16

    int16_t         hurt_hp;
    int16_t         hurt_mp;    // 4
                                // = 9+5 + 10 + 16 + 4 := 44
    uint8_t         huwei_pos;//护卫不为0时,hurt_hp,hurt_mp,施加于huwei_pos上的护卫者
}__attribute__((packed))nest_atk_t;

typedef struct resolved_atk {
    uint32_t        seq; //第几个动作
    uint32_t        fighter_id;
    uint32_t        petid;
    uint32_t        atk_type;
    uint8_t         atk_level;
    uint8_t         heji_seq;
    uint64_t        fighter_state;
	int32_t			change_hp;//all states effect hp
	int32_t			change_mp;//all states effect mp
	/*int16_t         resume_hp1;*/
	/*int16_t         resume_hp2;*/
	/*int16_t         resume_hp3;*/
	/*int16_t         poison_hp;//中毒*/
	/*int16_t         curse_mp;//诅咒*/
	/*int16_t         zhushang_hp;*/

	int16_t			parasitism_hp;
	uint32_t		host_uid;
	uint32_t		host_petid;

    uint32_t        atk_uid;
    uint32_t        atk_petid;//petid
    uint32_t        topic_id;
    int32_t         hurthp_rate; //伤害比例,最终伤害*hurthp_rate
    int32_t         hurtmp_rate;//伤害比例

    uint16_t        atk_cnt;
    nest_atk_t      fanji[MAX_FANJI_CNT];//反击？！

    list_head_t     atk_list;
}__attribute__((packed))resolved_atk_t;


enum team_type
{
	team_person       = 0x01, // 人
	team_person_group = 0x02, // 组队
	team_beast        = 0x04, // 怪
	team_npc		  = 0x08, // 打npc
	team_npc_friend   = 0x10, // 己方队伍中存在npc
};

//偷袭标志,
enum touxi_type
{
	touxi_none = 0,// normal challge
	touxi_by_challger, // 挑战方 
	touxi_by_challgee, //接受方 
};


// battle info
typedef struct battle_info {
    battle_id_t         batid;//战队id
    uint32_t            batl_state;
    uint32_t  			groupid;//组id
    float  				difficulty;//难度系数,用于对方攻击修正
	uint32_t			round_timestamp;//回合时间戳,用来防外挂
    battle_users_t      challger;//创建者队伍
    battle_users_t      challgee;//挑战者队伍

    uint8_t             touxi; //偷袭 
	uint32_t  			flag;

    uint32_t            seq_in_round;//用于序列化播放列表 ,临时使用
    uint32_t            battle_loop;//第几轮
    uint8_t             rand_speed;//是否使用随机速度:0,1
    list_head_t         warrior_list;//header
    list_head_t         speed_list;//header
    list_head_t         timer_list;//header
    list_head_t         attack_list;//header
    timer_struct_t*     fight_timer;//发招定时器 120秒
    timer_struct_t*     load_timer;//加载定时器 120秒

	uint32_t        bt_mode;
	inline  bool flag_check_bit( uint32_t bit_value ){
		return (this->flag & bit_value ) == bit_value ;
	}
	inline  void  flag_set_bit( uint32_t bit_value,bool value ){
		if (value ){
			this->flag |= bit_value ;
		}else{
			this->flag &= (0xFFFFFFFF-bit_value) ;
		}
	}

	inline bool is_one_person_pk_beast(){
		return this->flag_check_bit(0x01);
	}
	inline void set_is_one_person_pk_beast(bool value ){
		this->flag_set_bit(0x01,value );
	}

	/*章界王*/
	inline bool is_pk_beast_2008(){
		return this->flag_check_bit(0x00010000);
	}

	inline void set_is_pk_beast_2008(bool value ){
		this->flag_set_bit(0x00010000,value );
	}

	inline bool is_pk_beast_2004(){
		return this->flag_check_bit(0x00020000);
	}

	inline void set_is_pk_beast_2004(bool value ){
		this->flag_set_bit(0x00020000,value );
	}

	inline bool is_no_boss_lt_3_in_pk_beast_2004(){//
		return this->flag_check_bit(0x00040000);
	}

	inline void set_is_no_boss_lt_3_in_pk_beast_2004( bool value ){
		this->flag_set_bit(0x00040000,value );
	}


	inline bool is_pk_beast_3017(){
		return this->flag_check_bit(0x00080000);
	}

	inline void set_is_pk_beast_3017(bool value ){
		this->flag_set_bit(0x00080000,value );
	}

	inline bool is_pk_beast_2005(){
		return this->flag_check_bit(0x00100000);
	}

	inline void set_is_pk_beast_2005(bool value ){
		this->flag_set_bit(0x00100000,value );
	}

	inline bool is_pets_pk_mode(){
		return this->flag_check_bit(0x00200000);
	}

	inline void set_pets_pk_mode(bool value ){
		this->flag_set_bit(0x00200000,value );
	}

	inline bool is_pk_beast_2906() {
		return this->flag_check_bit(0x00400000);
	}

	inline void set_is_pk_beast_2906(bool value) {
		this->flag_set_bit(0x00400000, value );
	}

	inline bool is_pk_beast_2013() {
		return this->flag_check_bit(0x00800000);
	}

	inline void set_is_pk_beast_2013(bool value) {
		this->flag_set_bit(0x00800000, value );
	}

	inline bool is_no_boss_lt_3_in_pk_beast_2013(){//
		return this->flag_check_bit(0x01000000);
	}

	inline void set_is_no_boss_lt_3_in_pk_beast_2013( bool value ){
		this->flag_set_bit(0x01000000,value );
	}

	inline bool is_pk_beast_2012() {
		return (this->groupid == 2510);
	}

	inline bool attack_out_able(warrior_t *w) {
		if	(IS_BEAST_ID(w->userid) && 
			(this->is_pk_beast_2004() ||
			 this->is_pk_beast_2005() ||
			 this->is_pk_beast_2008() ||
			 this->is_pk_beast_2012() )) {
			return 0;
		}

		return 1;
	}
} battle_info_t;

#define PKG_STATE(b, v, j) \
	do { \
		*(uint32_t*)((b)+(j)) = htonl((uint32_t)v); (j) += 4; \
		*(uint32_t*)((b)+(j)) = htonl((uint32_t)(v >> 32)); (j) += 4; \
	} while (0)

#define SET_WARRIOR_STATE(state, state_bit)		((state) |= ((uint64_t)1 << (state_bit)))
#define RESET_WARRIOR_STATE(state, state_bit)	((state) &= (~((uint64_t)1 << (state_bit))))
#define CHK_WARRIOR_STATE(state, state_bit)		((state) & ((uint64_t)1 << (state_bit)))

#define WARRIOR_BINDONG(p_)			(p_)->check_state( dongshang_bit)
#define WARRIOR_SHIHUA(p_)				(p_)->check_state( shihua_bit)
#define WARRIOR_HUNSHUI(p_)			(p_)->check_state( hunshui_bit)
#define WARRIOR_RUN_AWAY(p_)			(p_)->check_state( run_away_bit)
#define WARRIOR_ATTACKED_OUT(p_)		(p_)->check_state( attacked_out_bit)
#define WARRIOR_OFFLINE(p_)			(p_)->check_state( person_break_off)

#define WARRIOR_DEAD(p_)				(p_)->check_state( dead_bit)
#define SET_WARRIOR_DEAD(p_)			SET_WARRIOR_STATE((p_)->p_waor_state->state, dead_bit)
#define SET_WARRIOR_ALIVE(p_)			RESET_WARRIOR_STATE((p_)->p_waor_state->state, dead_bit)
#define WARRIOR_CATCHED_PET(p_)		(p_)->check_state( catch_pet_succ)
//宠物不在战斗中
#define PET_NOTIN_BATTLE_MODE(p_)      ((p_)->petid && ((p_)->pet_state != rdy_fight && (p_)->pet_state != on_fuzhu ))

//检查宠物对应的人是不是被打飞了。
#define PETS_OWNER_ATTACK_OUT(p_)      ((p_)->petid && (p_)->check_state( person_atk_out))

//以下几个要特别关注
#define RUN_OR_OUT(p_)	(WARRIOR_RUN_AWAY(p_) || WARRIOR_ATTACKED_OUT(p_))
#define CANNOT_ATTACK(p_)	(RUN_OR_OUT(p_) || WARRIOR_DEAD(p_) || WARRIOR_CATCHED_PET(p_) || (p_)->pet_not_in_battle_mode() || PETS_OWNER_ATTACK_OUT(p_))
#define NONEED_ATTACK(p_)	(RUN_OR_OUT(p_) || WARRIOR_DEAD(p_) || WARRIOR_CATCHED_PET(p_) || (p_)->pet_not_in_battle_mode() || PETS_OWNER_ATTACK_OUT(p_))
#define DEAD_FIGHTER(p_)  (!RUN_OR_OUT(p_) && WARRIOR_DEAD(p_) && !WARRIOR_CATCHED_PET(p_) && !(p_)->pet_not_in_battle_mode() && !PETS_OWNER_ATTACK_OUT(p_))

#define CANNOT_ATTACK_DISABLE(skill_, enemy_) ((skill_)->applystatus == attack_only_alive && NONEED_ATTACK(enemy_))
#define CANNOT_ATTACK_ENABLE(skill_, enemy_) ((skill_)->applystatus == attack_only_dead && (!NONEED_ATTACK(enemy_)))



#define IS_MAGIC_ATK_SKILL(skill) (skill >= skill_ma_yunshishu && skill <= skill_ma_longjuanren)
#define IS_PHYSI_ATK_SKILL(skill) (skill >= skill_pa_base_fight && skill <= skill_pa_meiying)
#define IS_DEF_SKILL(skill) (skill >= skill_pd_fangyu && skill <= skill_md_mokang)

#define IS_STATE_CHG_SKILL(skill) (skill >= skill_ms_shihuazhizhen && skill <= skill_ms_shuimianjiejie)
#define IS_MAGIC_AST_SKILL(skill) (skill >= skill_mas_zaishengzhizhen && skill <= skill_mas_mofabizhang)
#define IS_MAGIC_MED_SKILL(skill) (skill >= skill_rh_mingsi && skill <= skill_rs_bingqingjiejie)

#define IS_BASE_ATK_SKILL(skill) (skill == skill_pa_base_fight || skill == skill_pa_pet_base_fight)
#define IS_HEJI_SKILL(skill) (skill == skill_pa_heji || skill == skill_pa_pet_heji)

#define IS_PHYSI_DEFENSE_SKILL(skill) (skill == skill_pd_fangyu || skill == skill_pd_huandun || skill == skill_pd_pet_fangyu || skill == skill_pd_pet_huandun)

#define IS_BEAST(p_) 		(IS_BEAST_ID((p_)->userid) || (p_)->petid)


extern fdsession_t* online[];

int init_all_timer_type(int rstart);

int battle_info_op(void* buf, int len, fdsession_t* fdsess);
uint8_t init_battles();
void clean_fdsess_onlinex_down(int fd);
void response_battle_info (battle_info_t* abi, uint16_t cmd);
void response_attacks_info (battle_info_t* abi, uint16_t cmd);
void state_effect_hpmp(battle_info_t* abi, warrior_t* p_fighter, atk_info_t* aai);
uint32_t get_beast_topic_id(battle_info_t* abi, warrior_t* p_fighter, atk_info_t* aai);

uint8_t battle_attack_op_ex(battle_info_t* abi, warrior_t* pw, uint32_t enemy_mark,int32_t enemy_pos, uint32_t atk_type,uint32_t atk_level,uint32_t itemid=0, uint32_t add_hp=0 );
uint8_t chk_and_calc_attacks(battle_info_t* abi);
void calc_warrior_single_step_battle_result(battle_info_t* abi, resolved_atk_t* pra, atk_info_t* aai);
int battle_load_expired(void* owner, void* data);
uint8_t check_battle_start(battle_info_t* abi);
int attack_op_expired(void* owner, void* data);
void beast_intel_attack(battle_info_t* abi, warrior_t* p, atk_info_t* aai, int seq);
void clear_warrior_atkinfo(battle_info_t* abi, warrior_t* lp);
void set_fangyu_state_before_round_attack(battle_info_t* abi, warrior_t* p_fighter, atk_info_t* aai);
int calc_hit_rate(warrior_t* p_fighter, warrior_t* fightee);
int calc_hit_result(battle_info_t* abi, warrior_t* p_fighter, warrior_t* fightee);

float calc_shuijing_xiangke(warrior_t* p_fighter, warrior_t* fightee);
int calc_phy_fight_hurt_value(battle_info_t* abi, warrior_t* p_fighter, warrior_t* fightee, atk_info_t* aai, int16_t* bisha_happen);
int calc_phy_hurt_value(battle_info_t* abi, warrior_t* p_fighter, warrior_t* fightee, atk_info_t* aai, int16_t* bisha_happen);
int calc_magic_fight_hurt_value(battle_info_t* abi, warrior_t* p_fighter, warrior_t* fightee, atk_info_t* aai);
int8_t chk_repeat_atkinfo_insert(battle_info_t* abi, atk_info_t* aai);

void report_user_fighted(battle_info_t* abi, uint32_t uid, uint32_t petid, uint16_t cmd);
int load_lua_script();
int warrior_load_expired(void* owner, void* data);


int get_pet_phy_pos(battle_users_t* team, int personpos, uint32_t petid);
void calc_team_ave_lv(battle_users_t* abu);
inline int send_to_online(int body_len, const void* body_buf, userid_t id, battle_id_t challenger, uint32_t opid, uint32_t online_id)
{
	static uint8_t bcpkg[bt_proto_max_len];

	int len = sizeof(batrserv_proto_t) + body_len;
	if (!online[online_id] || (body_len > (int)(sizeof(bcpkg) - sizeof(batrserv_proto_t))) ) {
		ERROR_RETURN( ("Failed to Send Pkg, fd=%d online_id=%d", (online[online_id]) ? online[online_id]->fd : 0, online_id), -1 );
	}

	batrserv_proto_t* pkg = (batrserv_proto_t*)bcpkg;
	pkg->len        = len;
	pkg->online_id  = online_id;
	pkg->cmd        = battle_info_msg;
	pkg->ret        = 0;
	pkg->id         = id;
	pkg->challenger = challenger;
	pkg->opid       = opid;
	memcpy(pkg->body, body_buf, body_len);

	if ( g_log_send_buf_hex_flag==1){
		char outbuf[13000];
		bin2hex(outbuf,(char*)pkg,len  ,2000);
		DEBUG_LOG("CO[%u][%u][%s]",   pkg->id,pkg->opid ,outbuf  );
	}



	return send_pkg_to_client(online[online_id], bcpkg, len);
}
//
typedef struct stru_svr_cli_proto{
		batrserv_proto_t 	svr_proto;
		protocol_t			cli_proto;
} __attribute__((packed)) svr_cli_proto_t;

inline int send_msg_to_online(int svr_cmd, Cmessage *c_in, userid_t id, battle_id_t challenger, uint32_t cli_cmdid, uint32_t online_id)
{
	
	static svr_cli_proto_t svr_cli_proto;

	if (!online[online_id] ) {
		ERROR_RETURN( ("Failed to Send Pkg, fd=%d online_id=%d", (online[online_id]) ? online[online_id]->fd : 0, online_id), -1 );
	}


	svr_cli_proto.svr_proto.len        = sizeof(svr_cli_proto);
	svr_cli_proto.svr_proto.online_id  = online_id;
	svr_cli_proto.svr_proto.cmd        = svr_cmd;
	svr_cli_proto.svr_proto.ret        = 0;
	svr_cli_proto.svr_proto.id         = id;
	svr_cli_proto.svr_proto.challenger = challenger;
	svr_cli_proto.svr_proto.opid       =  cli_cmdid;

	svr_cli_proto.cli_proto.cmd= htons(cli_cmdid);
	svr_cli_proto.cli_proto.ret= 0;

	return send_msg_to_client(online[online_id],(char*)&svr_cli_proto,c_in,
			sizeof(svr_cli_proto.svr_proto ),true );
}

inline void  i64tostring(battle_id_t l, char* buffer)
{
	sprintf(buffer, "%lu", l);
}

inline int send_to_online_error(uint32_t err, userid_t id, battle_id_t challenger, uint32_t opid, uint32_t online_id)
{
	static uint8_t bcpkg[bt_proto_max_len];

	if (!online[online_id]) {
		ERROR_RETURN(("Failed to Send Pkg, online_id=%d", online_id), -1);
	}

	batrserv_proto_t* pkg = (batrserv_proto_t*)bcpkg;
	pkg->len        = sizeof(batrserv_proto_t);
	pkg->online_id  = online_id;
	pkg->cmd        = battle_info_msg;
	pkg->ret        = err;
	pkg->id         = id;
	pkg->challenger = challenger;
	pkg->opid       = opid;

	return send_pkg_to_client(online[online_id], bcpkg, pkg->len);
}

inline void send_to_2_leader(battle_info_t* abi, uint16_t cmd, const void* buf, int len)
{
	send_to_online(len, buf, abi->challger.teamid, abi->batid, cmd, abi->challger.online_id);
	if (!IS_BEAST_ID(abi->challgee.teamid)){
		send_to_online(len, buf, abi->challgee.teamid, abi->batid, cmd, abi->challgee.online_id);
	}
}
inline void send_msg_to_2_leader(battle_info_t* abi, uint16_t cmd, Cmessage *c_in )
{
	send_msg_to_online(battle_info_msg, c_in, abi->challger.teamid, abi->batid, cmd, abi->challger.online_id);
	if (!IS_BEAST_ID(abi->challgee.teamid)){
		send_msg_to_online(battle_info_msg, c_in, abi->challgee.teamid, abi->batid, cmd, abi->challgee.online_id);
	}
}



void insert_atkinfo_to_list(battle_info_t* abi, atk_info_t* aai);
void clean_one_state(warrior_t* p_fighter, uint8_t state_bit, int8_t times);
void add_warrior_to_huweilist(warrior_t* p_fighter, uint8_t pos);
void chg_warrior_pos_from_huweilist(battle_info_t* abi, warrior_t* p_fighter, uint8_t pos);
void remove_warrior_from_huweilist(warrior_t* p_fighter);
void clean_warrior_huiwei_list(battle_info_t* abi, warrior_t* p_fighter);
warrior_t* get_warrior_from_huweilist(warrior_t* p_fighter);

inline warrior_t* get_warrior_from_warriorlist(battle_info_t* abi, userid_t uid, uint32_t petid)
{
	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* p = list_entry(lh, warrior_t, waor_list);
		if (p->userid == uid && p->petid == petid)
			return p;
	}

	return NULL;
}

inline warrior_t* get_pet_from_warriorlist(battle_info_t* abi, userid_t uid)
{
	list_head_t* lh;
	list_for_each(lh, &abi->warrior_list) {
		warrior_t* p = list_entry(lh, warrior_t, waor_list);
		if (p->userid == uid && p->petid &&  p->pet_state == rdy_fight )
			return p;
	}
	return NULL;
}
 
inline warrior_t* get_warrior_by_pos(battle_users_t* team, int pos)
{
	return (pos >= 0 && pos < 2 * MAX_PLAYERS_PER_TEAM) ? team->players_pos[pos] : NULL;
}

inline warrior_t* get_attacked_warrior(warrior_t* attacker, atk_info_t* aai)
{
	battle_users_t* team = aai->atk_mark ? attacker->enemy_team : attacker->self_team;
	return get_warrior_by_pos(team, aai->atk_pos);
}

inline int get_rand_alive_warrior_pos( battle_users_t* enemy, uint32_t self_pet_state)
{
	int cnt = 0;
	int psw[2 * MAX_PLAYERS_PER_TEAM] = {0};
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = enemy->players_pos[loop];
		if (!lp || lp->is_not_in_battle_mode() || NONEED_ATTACK(lp))
			continue;

		if (self_pet_state==on_fuzhu && lp->level==1 && lp->catchable){//辅助宠物不打一级
			continue;
		}
		if (lp->flag_check_bit(bit_attacked)) {
			continue;
		}

		psw[cnt++] = loop;
	}
	
	if (!cnt){
		return -1;
	}
	
	return psw[rand() % cnt];
}

inline int get_rand_disable_warrior_pos(battle_users_t* enemy)
{		
	int cnt = 0;
	int psw[2 * MAX_PLAYERS_PER_TEAM] = {0};
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		if (enemy->players_pos[loop] && 
			DEAD_FIGHTER(enemy->players_pos[loop]) &&
			!enemy->players_pos[loop]->is_not_in_battle_mode()) {
			psw[cnt++] = loop;
		}
	}
	
	if (!cnt){
		return -1;
	}
	
	return psw[rand() % cnt];
}

//得到不是boss的，还活着的个数
inline int get_no_boss_count(battle_users_t* abu, uint32_t  bossid)
{		
	int ret=0; 
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = abu->players_pos[loop];
		if (lp ) {//存在
			if (! (WARRIOR_DEAD(lp) || WARRIOR_ATTACKED_OUT(lp) ) && lp->type_id!=bossid ){
				ret++;
			}
		}
	}
	return ret;
}


inline int get_warrior_pos(battle_users_t* abu, userid_t uid, uint32_t petid)
{		
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = abu->players_pos[loop];
		if (lp && lp->userid == uid && lp->petid == petid) {
			return loop;
		}
	}
	return -1;
}
//找个最近的随机敌人
inline int get_near_rand_alive_warrior_pos(warrior_t* self, int pos )
{
	battle_users_t* self_team = self->self_team;
	battle_users_t* enemy_team = self->enemy_team;
	int cnt = 0;
	int psw[2 * MAX_PLAYERS_PER_TEAM] = {0};


	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = enemy_team->players_pos[loop];
		if (lp && !NONEED_ATTACK(lp)) {
			//可以攻击
			if (lp->flag_check_bit(bit_attacked)) {
				continue;
			}
			if (lp->is_not_in_battle_mode()) {
				continue;
			}
			if (pos >= 0 && pos < MAX_PLAYERS_PER_TEAM){//自己在第一排
				if (!(self->pet_state==on_fuzhu && lp->level==1 && lp->catchable)){
					psw[cnt++] = loop;
				}		

				continue;
			}
			
			if (loop >= 0 && loop < MAX_PLAYERS_PER_TEAM){ //对方在第一排
				if (!(self->pet_state==on_fuzhu && lp->level==1  && lp->catchable )){
					psw[cnt++] = loop;
				}		
				continue;
			}

			int sub_pos = 0;

			//双方都在后排
			int self_front = pos - MAX_PLAYERS_PER_TEAM;
			int eme_front = loop - MAX_PLAYERS_PER_TEAM;
			warrior_t* lp_self_front = self_team->players_pos[self_front];
			if (!lp_self_front //自己前方有人
					|| NONEED_ATTACK(lp_self_front//自己前方不需要被攻击
						)){
				sub_pos++;
			}
			
			warrior_t*  lp_enemy_front = enemy_team->players_pos[eme_front];
			if (!lp_enemy_front || NONEED_ATTACK(lp_enemy_front)){
				sub_pos++;
			}

			if (sub_pos > 0){
				if (!(self->pet_state==on_fuzhu && lp->level==1  && lp->catchable)){
					psw[cnt++] = loop;
				}		

			}
		}
	}
		
	if (!cnt){
		return -1;
	}

	return psw[rand() % cnt];
}

inline void set_warrior_stop_attack(battle_info_t * abi, warrior_t* lp)
{
	if (IS_BEAST_ID(lp->userid)){
		return;
	}
	
	SET_WARRIOR_STATE(lp->p_waor_state->state, stop_attack);
	if (lp->petid){
		return;
	}
//xxx
	/*warrior_t* lp_pet = get_pet_from_warriorlist(abi, lp->userid);*/
	/*if (lp_pet == NULL){*/
	/*return;*/
	/*}*/

	/*SET_WARRIOR_STATE(lp_pet->p_waor_state->state, stop_attack);*/
}

void clean_warrior_loop_state(warrior_t* p_fighter, uint8_t state_bit);

inline void reset_warrior_stop_attack(battle_info_t * abi, warrior_t* lp)
{
	if (IS_BEAST_ID(lp->userid)){
		return;
	}
		
	RESET_WARRIOR_STATE(lp->p_waor_state->state, stop_attack);
	if (lp->petid){
		return;
	}

	warrior_t* lp_pet = get_pet_from_warriorlist(abi, lp->userid);
	if (lp_pet == NULL){
		return;
	}

	RESET_WARRIOR_STATE(lp_pet->p_waor_state->state, stop_attack);
}

inline void decr_person_count(battle_info_t * abi, warrior_t* lp)
{
	if (lp->self_team == &abi->challgee){
		abi->challgee.alive_count--; //challgee_cnt--;
		abi->challgee.ren_alive_count--; //challgee_cnt--;
		if (abi->challgee.team_type & team_npc_friend){
			abi->challgee.npc_fri_cnt_alive--;
		}
	}else{
		abi->challger.alive_count--;
		abi->challger.ren_alive_count--;//	abi->challger_cnt--;
		if (abi->challger.team_type & team_npc_friend){
			abi->challger.npc_fri_cnt_alive--;
		}
	}
}

inline void decr_pet_count(battle_info_t * abi, warrior_t* lp)
{
	DEBUG_LOG("decr_pet_count");
	if(!RUN_OR_OUT(lp) && !WARRIOR_DEAD(lp) && !lp->pet_not_in_battle_mode()){
		if (lp->self_team == &abi->challgee){
			if(abi->challgee.alive_count){
				abi->challgee.alive_count--; //challgee_cnt--;
			}
		}else{
			if(abi->challger.alive_count){
				abi->challger.alive_count--; //challgee_cnt--;
			}
		}
	}
}

inline void set_warrior_attack_out(battle_info_t * abi, warrior_t* lp)
{
	DEBUG_LOG("set_warrior_attack_out");
	if (lp->huwei_pos){
		remove_warrior_from_huweilist(lp);
	}
	if (lp->check_state( attacked_out_bit)){
		return;
	}
	if(lp->is_pet()){
		decr_pet_count(abi,lp);
	}
	SET_WARRIOR_STATE(lp->p_waor_state->state, attacked_out_bit);

	if ((abi->challger.team_type | abi->challgee.team_type) & team_beast ){
		//受攻击方等级<=15 不处理
		if (  lp->level>15 ){
			//直接设置红伤
			lp->injury_lv = injury_red;
		}
	}

	if (lp->p_waor_state->state & resume_hp1) { // zai sheng jiejie
		clean_warrior_loop_state(lp, resume_hp1_bit);
	}

	if (lp->p_waor_state->state & resume_hp2) { // zai sheng jiejie
		clean_warrior_loop_state(lp, resume_hp2_bit);
	}

	if (lp->p_waor_state->state & resume_hp3) { // zai sheng jiejie
		clean_warrior_loop_state(lp, resume_hp3_bit);
	}
	if (lp->p_waor_state->state & resume_hp4) { // zai sheng jiejie
		clean_warrior_loop_state(lp, resume_hp4_bit);
	}

	if (lp->check_state( pause_bit)){ // ting zhi xing dong biaozhiwei
		clean_one_state(lp, pause_bit, 1);
	}

	if (lp->petid){
		//DEBUG_LOG("alivecount--1");
		decr_pet_count(abi,lp);
		return;
	}

	decr_person_count(abi, lp);
	//如果是人被踢飞，则宠物也要被踢飞
	if (lp->self_team == &abi->challgee){
		abi->challgee.atk_out++;
	}else{
		abi->challger.atk_out++;
	}

	warrior_t* lp_pet = get_pet_from_warriorlist(abi, lp->userid);
	if (lp_pet == NULL){
		return;
	}

	if (lp_pet->huwei_pos){
		remove_warrior_from_huweilist(lp_pet);
	}
	decr_pet_count(abi,lp_pet);
	SET_WARRIOR_STATE(lp_pet->p_waor_state->state, person_atk_out);
	//set fuzhu pet bei da fei 
	/*list_head_t* lh;*/
	/*list_for_each(lh, &abi->warrior_list) {*/
	/*warrior_t* pfuzhu = list_entry(lh, warrior_t, waor_list);*/
	/*KDEBUG_LOG(lp->userid,"fuzhu %u %u %u",pfuzhu->userid,pfuzhu->petid,pfuzhu->pet_state);*/
	/*if (pfuzhu->userid == lp->userid && pfuzhu->petid &&  pfuzhu->pet_state == on_fuzhu ){*/
	/*decr_pet_count(abi, pfuzhu);*/
	/*//SET_WARRIOR_STATE(pfuzhu->p_waor_state->state, person_atk_out);*/
	/*}*/
	/*}*/
}

void clean_all_bad_state(warrior_t* aw);
inline void set_warrior_dead_opt(battle_info_t * abi, warrior_t* lp)
{
	KDEBUG_LOG(lp->userid,"set_warrior_dead_opt state=%u",lp->check_state( dead_bit));
	if (lp->check_state( dead_bit)){
		return;
	}
	SET_WARRIOR_DEAD(lp);

	// fixme:
	// when other put <jiushu> skill on the dead object, there is a bug.
	// these bits are in using.
	// so clear follow bit when warrior is dead
	if (lp->p_waor_state->state & resume_hp1) { // zai sheng jiejie
		clean_warrior_loop_state(lp, resume_hp1_bit);
	}

	if (lp->p_waor_state->state & resume_hp2) { // zai sheng jiejie
		clean_warrior_loop_state(lp, resume_hp2_bit);
	}

	if (lp->p_waor_state->state & resume_hp3) { // zai sheng jiejie
		clean_warrior_loop_state(lp, resume_hp3_bit);
	}

	if (lp->p_waor_state->state & resume_hp4) { // zai sheng jiejie
		clean_warrior_loop_state(lp, resume_hp4_bit);
	}


	clean_all_bad_state(lp);
	if (lp->p_waor_state->state & zhongdu) { 
		clean_one_state(lp, zhongdu_bit , 10);
	}


	if (lp->check_state( pause_bit)){ // ting zhi xing dong biaozhiwei
		clean_one_state(lp, pause_bit, 1);
	}
	
	if (lp->petid){
		//DEBUG_LOG("alivecount--3");
		//decr_pet_count(abi,lp);
		if(lp->self_team->alive_count){
			lp->self_team->alive_count--;
		}
		return;
	}
	decr_person_count(abi, lp);
}

inline void set_warrior_alive_opt(warrior_t* lp)
{
	SET_WARRIOR_ALIVE(lp);

	if (lp->petid){
		lp->self_team->alive_count++;
		return;
	}

	lp->self_team->ren_alive_count++;
	if (IS_BEAST_ID(lp->userid)){
		lp->self_team->npc_fri_cnt_alive++;
	}
}

inline int get_rand_alive_warrior_pos_ex(battle_users_t* enemy, warrior_t* except)
{
	int cnt = 0;
	int psw[2 * MAX_PLAYERS_PER_TEAM] = {0};
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = enemy->players_pos[loop];
		if (lp && !NONEED_ATTACK(lp) && !lp->is_not_in_battle_mode() && lp != except)
			psw[cnt++] = loop;
	}
	
	if (!cnt){
		return -1;
	}
	
	return psw[rand() % cnt];

}

inline warrior_t* get_rand_alive_warrior(battle_users_t* enemy,uint32_t self_pet_state )
{		
	int cnt = 0;
	warrior_t* psw[2 * MAX_PLAYERS_PER_TEAM] = {0};
	int loop;
	for (loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = enemy->players_pos[loop];
		if (lp && !NONEED_ATTACK(lp) && !lp->is_not_in_battle_mode()) {
			//辅助宠物，不打一级
			if (!((self_pet_state == on_fuzhu ) && lp->level == 1  && lp->catchable) )
			{
				psw[cnt++] = lp;
			}
			continue;
		}
	}
	
	if (!cnt){
		return NULL;
	}
	
	return psw[rand() % cnt];
}

inline int get_front_rand_alive_warrior_pos(battle_users_t* enemy)
{
	int pos = rand() % MAX_PLAYERS_PER_TEAM;
	for (int loop = 0; loop < MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = enemy->players_pos[pos % MAX_PLAYERS_PER_TEAM];
		if (lp && !NONEED_ATTACK(lp) && !lp->is_not_in_battle_mode())
			return pos;
		pos = (pos + 1) % MAX_PLAYERS_PER_TEAM;
	}
	
	return -1;
}

inline warrior_t* get_rand_alive_warrior_pet(battle_users_t* enemy)
{		
	int cnt = 0;
	warrior_t* psw[2 * MAX_PLAYERS_PER_TEAM] = {0};
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = enemy->players_pos[loop];
		if (lp && lp->petid && !NONEED_ATTACK(lp) && !lp->is_not_in_battle_mode()) {
			psw[cnt++] = lp;
		}
	}
	
	if (!cnt){
		return NULL;
	}
	
	return psw[rand() % cnt];
}

inline int get_pet_pos(battle_info_t* abi, battle_users_t* abu, userid_t uid)
{		
	for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		warrior_t* lp = abu->players_pos[loop];
		if (lp && lp->userid == uid && lp->petid) {
			return loop;
		}
	}
	return -1;
}

inline int get_warrior_pos_ex(battle_info_t* abi, userid_t uid, uint32_t petid, int* apos)
{		
	battle_users_t* pbu = &abi->challger;
	warrior_t* lp = NULL;
	int loop;
	for (loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		lp = pbu->players_pos[loop];
		if (lp && lp->userid == uid && lp->petid == petid) {
			*apos = loop;
			return 0;
		}
	}

	pbu = &abi->challgee;
	for (loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
		lp = pbu->players_pos[loop];
		if (lp && lp->userid == uid && lp->petid == petid) {
			*apos = loop;
			return 0;
		}
	}
	return -1;
}

inline void notify_use_medical(battle_info_t* abi, warrior_t* aw)
{	
	uint32_t buff[2] = {aw->userid, aw->p_waor_state->get_use_item_itemid()};
	send_to_online(8, buff, aw->self_team->teamid, abi->batid, proto_bt_use_chemical, aw->self_team->online_id);
}

inline void notify_chg_pos(battle_info_t* abi, warrior_t* aw)
{	
	send_to_online(4, &aw->userid, aw->self_team->teamid, abi->batid, proto_bt_change_pos, aw->self_team->online_id);
}

inline void notify_catch_pet(battle_info_t* abi, warrior_t* aw, uint32_t pet_type, int success, uint32_t petlevel)
{
	 Cwarrior_state_ex_catch_pet *p_state_ex ;
	 p_state_ex=(Cwarrior_state_ex_catch_pet *) aw->p_waor_state->get_state_ex_info(CATCH_PET_STATE_BIT );
	 uint32_t itemid=0;
	 if (p_state_ex ){
		itemid=p_state_ex->itemid;	
	 }


	uint32_t buff[5] = {aw->userid, pet_type, itemid, success, petlevel};
	send_to_online(20, buff, aw->self_team->teamid, abi->batid, proto_bt_catch_pet, aw->self_team->online_id);
}

inline void notify_recall_pet(battle_info_t* abi, warrior_t* aw, uint32_t cur_petid, uint32_t new_petid, int success)
{
	uint32_t buff[3] = {aw->userid, new_petid, cur_petid};
	if (success){
		send_to_online(12, buff, aw->self_team->teamid, abi->batid, proto_bt_recall_pet, aw->self_team->online_id);
	} else{
		send_to_online_error(1, aw->self_team->teamid, abi->batid, proto_bt_recall_pet, aw->self_team->online_id);
	}
}

inline skill_info_t* get_beast_skill_by_atktype(warrior_t* p_fighter, uint32_t atktype)
{
	for (int i = 0; i < p_fighter->skill_cnt; i++){
		if (p_fighter->skills[i].skill_id == atktype){
			return &(p_fighter->skills[i]);
		}
	}
	return NULL;
}

void init_cli_handle_funs();

#endif
