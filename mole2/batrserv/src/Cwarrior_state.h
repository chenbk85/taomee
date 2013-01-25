/*
 * =========================================================================
 *
 *        Filename: Cwarrior_state.h
 *
 *        Version:  1.0
 *        Created:  2012-02-20 11:32:34
 *        Description:   
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#ifndef  CWARRIOR_STATE_H
#define  CWARRIOR_STATE_H
#include  "onlineapi.h"

enum {
	// 石化状态位 该状态下受到攻击造成的伤害减半 
	shihua_bit				= 0,
	// 遗忘状态位 随机4~6项指令、技能无法选用
	yiwang_bit				= 1,
	// 混乱状态位 50%几率由系统强制“攻击”敌我双方（除自己）任一目标
	hunluan_bit				= 2,
	// 诅咒状态位 损失当前MP的10%~12%
	zuzhou_bit				= 3,
	// 中毒 损失当前HP的10%~12%
	zhongdu_bit				= 4,
	// 昏睡 该状态下受到攻击立即苏醒
	hunshui_bit				= 5,

	// 攻击吸收
	gongji_xishou_bit		= 6,
	// 攻击反弹
	gongji_fantan_bit		= 7,
	// 攻击无效
	gongji_wuxiao_bit		= 8,

	// 魔法吸收
	mofa_xishou_bit			= 9,
	// 魔法反弹
	mofa_fantan_bit			= 10,
	// 魔法无效
	mofa_wuxiao_bit			= 11,

	// 再生之阵
	resume_hp1_bit			= 12,
	// 再生领域
	resume_hp2_bit			= 13,
	// 再生结界
	resume_hp3_bit			= 14,
	////灼伤 损失当前等级*2的HP 
	zhuoshang_bit			= 15,//灼伤
	//木叶盾
	muyedun_bit				= 16,
	//冰冻 损失当前等级*1.5的MP
	dongshang_bit			= 17,
	//束缚 回避下降25%
	shufu_bit				= 18,
	//眩晕 命中下降25%
	xuanyun_bit				= 19,

	resume_hp4_bit			= 20,
	//缠绕
	chanrao_bit				= 21,
	//神衰
	shenshuai_bit			= 22,

	//高频震击 解冻 
	gaopin_zhenji_bit    	=23,

	//脱力 物攻、物防下降10%
	tuoli_bit  				=24,
	//防护罩 发给客户端
	fanghuzhao_bit  		=25,
	//绝对壁障
	jueduibizhang_bit  		=26,



	// 逃跑
	run_away_bit 			= 32,
	// 被打死
	dead_bit	 			= 33,
	// 被打飞
	attacked_out_bit	 	= 34,
	// 被打
	attacked_bit			= 35,
	// 防御
	fangyu_bit				= 36,
	// 回避
	huibi_bit				= 37,
	// 魅影
	meiying_bit				= 38,
	// 幻盾
	huandun_bit				= 39,
	// 魔抗
	mokang_bit				= 40,
	// 停止行动
	stop_attack 			= 41,

	// 必杀
	bisha_bit				= 42,
	// 使用药瓶
	use_item_bit			= 43,
	// 救活
	revive_bit				= 44,
	// 回击
	huiji_bit               = 45,
	// 暂停行动
	pause_bit               = 46,
	// 抓宠成功
	catch_pet_succ          = 47,
	// 主人被打飞
	person_atk_out			= 48,
	// 掉线
	person_break_off		= 49,
	// 打飞结果同步
	attack_out_sync			= 50,

	//设置辅助宠物 callback 
	fuzhu_callback_bit	 	= 51,

	// parasitism bit
	parasitism_bit			= 52,
	new_parasitism_bit		= 53,

	//下次近身反击
	next_atk_fanji_bit    = 54,

	//野兽攻击时加成
	yeshou_add_hert_hp_bit    = 55,
	//弱点透析
	ruodiantouxi_bit    = 56,
		
};

enum {	
	shihua				= 1 << shihua_bit,//石化
	yiwang				= 1 << yiwang_bit,//遗忘
	hunluan				= 1 << hunluan_bit,//混乱
	zuzhou				= 1 << zuzhou_bit,//诅咒
	zhongdu				= 1 << zhongdu_bit,//中毒
	hunshui				= 1 << hunshui_bit,//昏睡
	
	gongji_xishou		= 1 << gongji_xishou_bit,
	gongji_fantan		= 1 << gongji_fantan_bit,
	gongji_wuxiao		= 1 << gongji_wuxiao_bit,
	mofa_xishou			= 1 << mofa_xishou_bit,
	mofa_fantan			= 1 << mofa_fantan_bit,
	mofa_wuxiao			= 1 << mofa_wuxiao_bit,

	resume_hp1			= 1 << resume_hp1_bit,
	resume_hp2			= 1 << resume_hp2_bit,
	resume_hp3			= 1 << resume_hp3_bit,
	resume_hp4			= 1 << resume_hp4_bit,
	zhuoshang			= 1 << zhuoshang_bit,//灼伤
	dongshang			= 1 << dongshang_bit,
	shufu				= 1 << shufu_bit,
	xuanyun 			= 1 << xuanyun_bit,
	tuoli 				= 1 << tuoli_bit,

	gaopin_zhenji	=	(1 << gaopin_zhenji_bit),

	abnormal_states	= 0x2F
};

//不可以叠加的状态
const uint64_t not_at_time_state_bit = 
			shihua 	| yiwang | hunluan | zuzhou 
		| 	zhongdu | hunshui  | zhuoshang 
		|	dongshang |	shufu	|xuanyun 
		|	((uint64_t)1 << tuoli_bit) 	
	;

//这些状态要一直发给客户端 !!! 发给客户端的一定要再这里添加
const uint64_t need_send_warrior_state_flag=abnormal_states
	|	((uint64_t)1 << parasitism_bit) 
	|	((uint64_t)1 << next_atk_fanji_bit) 
	|	((uint64_t)1 << yeshou_add_hert_hp_bit) 
	|	((uint64_t)1 << ruodiantouxi_bit) 
	|	((uint64_t)1 << zhuoshang_bit)
	|	((uint64_t)1 << dongshang_bit) 
	|	((uint64_t)1 << shufu_bit)
	|	((uint64_t)1 << xuanyun_bit)
	|	((uint64_t)1 << fanghuzhao_bit)
	|	((uint64_t)1 << tuoli_bit)
	|	((uint64_t)1 << run_away_bit)
	|	((uint64_t)1 << attacked_out_bit)
	|	((uint64_t)1 << dead_bit);
/*
    *Type行动类型， 物理攻击1，Mp攻击2，防御技能3，Hp回复4，状态防御5，吸血魔法6，魔法攻击7，异常状态攻击8，防御状态赋予9，使用道具10，换位11，捕捉12，召唤13，待机14，吸血攻击15，逃>跑16，护卫17，死亡待机18
  */




class Cwarrior_state_ex_info_base {
	private:
	public:

};

class Cwarrior_state_ex_catch_pet:public Cwarrior_state_ex_info_base {
	private:
	public:
	    uint32_t            feng_mo_lv;//封魔
	    uint32_t            itemid;//卡片id
	    uint32_t            race_cnt;//个数
	    uint32_t            race[MAX_RACE_TYPE];
};
class Cwarrior_state_ex_uint32:public Cwarrior_state_ex_info_base {
	private:
	public:
		uint8_t 			state_bit;
	    uint32_t            value;
};




#include  <map>

#define CATCH_PET_STATE_BIT 200
#define WARRIOR_STATE_EX_BIT_HOST_ID 201
#define WARRIOR_STATE_EX_BIT_HOST_PETID 202

#define WARRIOR_STATE_EX_BIT_USE_ITEM_ITEMID 203
#define WARRIOR_STATE_EX_BIT_USE_ITEM_ADD_HP 204
#define WARRIOR_STATE_EX_BIT_YESHOU_HURT_HP_PERCENT 205
#define WARRIOR_STATE_EX_BIT_HUWEI_LEVEL 206
#define WARRIOR_STATE_EX_BIT_HUNDUN_LEVEL 207
#define WARRIOR_STATE_EX_BIT_MEIYIN_LEVEL 209
#define WARRIOR_STATE_EX_BIT_MUYEDUN_LEVEL 210


#define WARRIOR_STATE_EX_BIT_FANGHU_HP_VALUE  211//防护值
#define WARRIOR_STATE_EX_BIT_FANGHU_TIMES 	    212//防护次数

#define WARRIOR_STATE_EX_BIT_QILIAODUN_LEVEL   213
#define WARRIOR_STATE_EX_BIT_MIFANQUAN_LEVEL   214
#define WARRIOR_STATE_EX_BIT_JUEQIQUAN_LEVEL 	215
#define WARRIOR_STATE_EX_BIT_FANGHUZHAO_VALUE 216


#define WARRIOR_STATE_EX_BIT_RESUME_HP1_LEVEL 220
#define WARRIOR_STATE_EX_BIT_RESUME_HP2_LEVEL 221
#define WARRIOR_STATE_EX_BIT_RESUME_HP3_LEVEL 222
#define WARRIOR_STATE_EX_BIT_RESUME_HP4_LEVEL 224

#define WARRIOR_STATE_EX_BIT_JINGHUNQUAN_LEVEL 225
#define WARRIOR_STATE_EX_BIT_JINTOUQUAN_LEVEL 226
//保存勇士的当时状态
class Cwarrior_state {
	private:
	    int8_t              round_cnt[64]; //持续回合数,对应state的每一位
	    int8_t              effect_cnt[64];//影响回合数,对应state的每一位
		//用于保存扩展数据
		std::map<int8_t,Cwarrior_state_ex_info_base * > state_info_map;
	public:
	    uint64_t            state;//64位,每一位表不同的状态
	    uint64_t            one_loop_state;//单回合状态, 

		Cwarrior_state();
		~Cwarrior_state();
		inline uint64_t get_state() {
			return this->state;
		}
		int8_t  change_round_cnt(uint8_t state_bit ,int8_t value=0);
		int8_t  change_effect_cnt(uint8_t state_bit ,int8_t value=0);
		int8_t  get_round_cnt(uint8_t state_bit );
		int8_t  get_effect_cnt(uint8_t state_bit );


		uint32_t 	get_fanghuzhao_value( );
		void 		set_fanghuzhao_value(uint32_t value );
		uint32_t 	get_qiliaodun_level ( );
		void 		set_qiliaodun_level (uint32_t value );
		uint32_t 	get_mifanquan_level ( );
		void 		set_mifanquan_level (uint32_t value );
		uint32_t 	get_jueqiquan_level( );
		void 	  	set_jueqiquan_level	(uint32_t value );
		//add_hit_rate




	
		int8_t get_muyedun_level ( );
		void set_muyedun_level (uint32_t value );
	
		int8_t 	get_huwei_level( );
		void set_huwei_level(uint32_t value );		
		int8_t 	get_hundun_level( );
		void set_hundun_level(uint32_t value );

		int8_t 	get_resume_hp1_level( );
		void set_resume_hp1_level(uint32_t value );
		int8_t 	get_resume_hp2_level( );
		void set_resume_hp2_level(uint32_t value );
		int8_t 	get_resume_hp3_level( );
		void set_resume_hp3_level(uint32_t value );

		int8_t 	get_jinghunquan_level( );
		void set_jinghunquan_level(uint32_t value );

		int8_t 	get_jintouquan_level( );
		void set_jintouquan_level(uint32_t value );

		int8_t 	get_resume_hp4_level( );
		void set_resume_hp4_level(uint32_t value );

		int8_t 	get_meiyin_level( );
		void set_meiyin_level(uint32_t value );

		uint32_t 	get_host_uid( );
		void set_host_uid(uint32_t value );

		uint32_t 	get_host_petid( );
		void set_host_petid(uint32_t value );

		uint32_t 	get_use_item_itemid ( );
		void set_use_item_itemid (uint32_t value );

		int16_t 	get_use_item_add_hp ( );
		void set_use_item_add_hp (uint32_t value );

		//万兽噬身-野兽
		//对目标发动无属性魔法并留下魔法印记，被标记的目标受到野兽系攻击时伤害增加n%，效果持续2回合

		uint32_t 	get_yeshou_hurt_hp_percent( );

		void set_yeshou_hurt_hp_percent(uint32_t value );

	
		void set_state_ex_info( uint8_t state_bit, Cwarrior_state_ex_info_base * p_ex_info );
		Cwarrior_state_ex_info_base *get_state_ex_info (uint8_t state_bit );
		uint32_t get_state_ex_info_uint32( uint8_t state_bit );
		void set_state_ex_info_uint32( uint8_t state_bit , uint32_t value );
	
		//
		bool set_state_info(uint8_t state_bit,int8_t effect_count=0,int8_t round_count=0 );
		//设置概率状态
		uint8_t set_rand_state_info(uint8_t *p_state_bit_list, uint32_t  state_bit_count,
			int rand_value	,int8_t effect_count=0,int8_t round_count=0 );
		uint8_t set_some_state_info(uint8_t state_bit, int rand_value,int8_t effect_count=0,int8_t round_count=0 );

		void unset_state_info(uint8_t state_bit );
		bool check_state(uint8_t state_bit);

};




#endif  /*CWARRIOR_STATE_H*/
