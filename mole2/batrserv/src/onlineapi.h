#ifndef ONLINE_API_H
#define ONLINE_API_H
#include    <stdint.h>
#define MAX_PLAYERS_PER_TEAM	5
#define MAX_GROUP_PLAYERS       4 // one person and 3 pet at most
#define MAX_ITEMS_WITH_BODY		8
#define USER_NICK_LEN			16

#define BEGIN_USER_ID	50000
#define BEAST_START_ID	1
#define BEAST_END_ID	10000
#define BEAST_NUM_PER_TYPE	4

#define RAND_NUM(mod, rate) (uint32_t( (rand() % 100)) < rate)

#define MAX_BATTLE_LOOP 2000

#define IS_BEAST_ID(uid_)  (uid_ < BEGIN_USER_ID)
#define IS_BEAST_GRP_ID(grp_id)  (grp_id < BEGIN_USER_ID)
#define MAX_SKILL_NUM_PER_PERSON	10
#define MAX_RACE_TYPE	10

#define BEGIN_BOSS_ID		2001
#define MAX_BEAST_TYPE	5000
#define IS_BOSS_ID(uid_) (IS_BEAST_ID(uid_) && (uid_) % MAX_BEAST_TYPE >= BEGIN_BOSS_ID)

#define ADD_ONLINE_TIMER(owner, nbr_, data_, last_time_)  \
	ADD_TIMER_EVENT_EX((owner), (nbr_), (data_), get_now_tv()->tv_sec + (last_time_))



#define BATRSERV_LUA_SCRIPT "../conf/batrserv_ai.ai"
#define BATRSERV_LUA_RELOAD_STAMP (60 * 10) 
#define BATRSERV_LUA_FOR_C_BEAST_AI "lua_beast_attack_ai"
#define BATRSERV_LUA_FOR_C_NEED_2_ATTACK "lua_beast_need_2_attack"


#define REGISTER_TIMER_TYPE(nbr_, cb_, rstart_) \
			do { \
				if (register_timer_callback(nbr_, cb_) == -1) \
					ERROR_RETURN(("register timer type error\t[%u]", nbr_), -1); \
			} while(0)

#define REGISTER_LUA_FUN(state, name, fun) lua_pushstring(state, name);\
									lua_pushcfunction(state, fun);\
									lua_settable(state, LUA_GLOBALSINDEX);

			
enum atack_target_type{
	atk_self,
	atk_sefl_all,
	atk_except_self,
	atk_enemy_all,
	atk_all,
};
				
enum cloth_type {
	weapon_kongshou         = 0,
	weapon_jian				= 1,
	weapon_gong				= 2,
	weapon_zhang			= 3,
	weapon_biao				= 4,
	weapon_qiang			= 5,
	weapon_mo_zhang			= 6,
	weapon_sheng_zhang		= 7,
};

enum equip_part {
	part_hat				= 1,
	part_cloth				= 2,
	part_glove				= 3,
	part_shoe				= 4,
	part_weapon				= 5,
	part_shield				= 6,
	part_guajian			= 7,
	part_crystal			= 8,
};


enum {
/*
	惊魂拳（Lv30转职习得）：近身单体攻击，同时造成HP和MP伤害。
劲透拳（Lv30转职习得）：近身单体攻击，对未防御的目标造成的HP伤害降低，对防御中的目标造成的HP伤害增加。
	气斗拳（Lv30转职习得）：远程群体攻击，对指定目标方单个或多个对象释放气弹，造成HP伤害。
	气疗盾（Lv40升级领悟）：同时对物理攻击和魔法攻击进行防御，并恢复自身HP。
	迷返拳（Lv50升级领悟）：降低回避并提升防御，受到近身单体攻击时发动反击，造成HP伤害。
	绝气拳（Lv60升级领悟）：近身单体攻击，降低自身防御并提升自身回避，命中目标时有几率秒杀。 
*/
	skill_jinghunquan 	= 4001,
	skill_jintouquan 	= 4002,
	skill_qidouquan 	= 4003,
	skill_qiliaodun 	= 4004,
	skill_mifanquan 	= 4005,
	skill_jueqiquan 	= 4006,

/*
炎蝶之舞（Lv30转职习得）：单体火属性魔法，造成HP伤害的同时有几率让目标陷入灼伤状态（回合行动前额外扣除HP）。
晶棱封杀（Lv30转职习得）：单体水属性魔法，造成HP伤害的同时有几率让目标陷入冻伤状态（回合行动前额外扣除MP）。
大地刺枪（Lv30转职习得）：T范围地属性魔法，造成HP伤害的同时有几率让目标陷入束缚状态（降低敌方闪躲）。
轮转风暴（Lv40升级领悟）：T范围风属性魔法，造成HP伤害的同时有几率让目标陷入失明状态（降低敌方命中）。
暗云裂解（Lv50升级领悟）：全体暗属性魔法，造成HP伤害的同时有几率让目标陷入脱力状态（降低敌方攻防）。
生命契约（Lv60升级领悟）：单体无属性魔法，造成HP伤害的同时汲取伤害的50%恢复自身HP。
*/
	skill_yandiezhiwu 		= 4011,
	skill_jinglengfengsha 	= 4012,
	skill_dadiciqiang		= 4013,
	skill_lunzhuanfengbao	=4014,
	skill_anyunliejie		=4015,
	skill_shengmingqiyue	=4016,
/*
圣枪投射（Lv30转职习得）：单体圣属性魔法，造成HP伤害，对不死系魔物造成的伤害更大。
圣灵召唤（Lv30转职习得）：T范围圣属性魔法，造成HP伤害，对不死系魔物造成的伤害更大。
圣堂领域（Lv30转职习得）：恢复T范围目标的HP。
神羽呼唤（Lv40升级领悟）：复活单体目标，并恢复一定量的HP。
神恩结界（Lv50升级领悟）：恢复全体目标的HP。
圣言诗篇（Lv60升级领悟）：清除T范围目标的负面状态。 
*/
	skill_shengqiangtoushe		=4021,
	skill_shenglingzhaohuan		=4022,
	skill_shengtanglingyu		=4023,
	skill_shenyuhuhuan		=4024,
	skill_shenenjiejie		=4025,
	skill_shengyanshipian	=4026,
/*
灾厄之爪（Lv30转职习得）：远程单体攻击，造成HP伤害的同时有几率附带中毒或诅咒效果。
混沌之刃（Lv30转职习得）：远程单体攻击，造成HP伤害的同时有几率附带石化、昏睡或混乱效果。
审判之轮（Lv30转职习得）：远程T范围攻击，造成HP伤害的同时有几率附带各种异常状态。
咒怨之门（Lv50升级领悟）：全体咒术，让目标方有几率陷入各种异常状态。
绝对壁障（Lv40升级领悟）：给目标附加保护罩，受到攻击时可抵消一定伤害。
再生结界（Lv60升级领悟）：给目标附加反射罩，受到攻击时可发射伤害给攻击方。
*/
	skill_zaiezhizhao		=4031,
	skill_hundunzhiren		=4032,
	skill_shenpanzhilun		=4033,
	skill_zhouyuanzhimen	=4034,
	skill_jueduibizhang		=4035,
	skill_zaishengjiejie	=4036,
//--------------------------------------------------


	skill_run_away				= 1,
	skill_user_break_off		= 2,//强制逃跑
	skill_use_chemical			= 4,
	skill_chg_position          = 5, //换位 
	skill_catch_pet				= 6,
	skill_recall_pet			= 7,//召唤
	skill_pa_dai_ji             = 8,//待机
	skill_pa_dead_dumy          = 9,//击倒,在用户死时,不能发招就发这个
	
	skill_pa_base_fight			= 10,//普通攻击
	skill_pa_heji		 		= 11,
	skill_callback_fuzhu		= 12,//召回辅助宠物

	skill_pa_xianfazhiren	 	= 101,
	skill_pa_zhuiji		 		= 102,//追击
	skill_pa_bengya				= 103,
	skill_pa_chuanci			= 104,//穿刺
	skill_pa_jizhonggongji		= 105,
	skill_pa_kuangji			= 106,
	skill_pa_douqi				= 107,
	skill_pa_huishang			= 108,
	skill_pa_duochongjian		= 109,
	skill_pa_shuangxingjian		= 110,
	skill_pa_ninshenjian		= 111,
	skill_pa_meiying			= 112,
	
	skill_sanyeshi				= 113,
	skill_feiyejian				= 114,
	skill_muyedun				= 115,
	skill_shunsha				= 116,

	skill_hm_pohun 				= 501,

	skill_pd_fangyu				= 701,
	skill_md_mokang				= 702,
	skill_pd_huandun			= 703,
	skill_pd_huiji				= 704,
	
	skill_rh_mingsi				= 1001,
	skill_rh_zhiyuzhizhen		= 1002,
	skill_rh_zhiyulingyu		= 1003,
	skill_rh_zhiyujiejie		= 1004,
	skill_rs_bingqingzhizhen	= 1005,
	skill_rs_bingqinglingyu		= 1006,
	skill_rs_bingqingjiejie		= 1007,
	skill_rl_jiushu 			= 1008, //救赎
	
	
	skill_fh_chouqu				= 1801,

	skill_ma_yunshishu			= 2001,
	skill_ma_shuangdongshu		= 2002,
	skill_ma_liuhuoshu			= 2003,
	skill_ma_xuanfengshu		= 2004,
	
	skill_ma_jushizhou			= 2005,
	skill_ma_hanbingzhou		= 2006,
	skill_ma_liehuozhou			= 2007,//烈火咒
	skill_ma_kuangfengzhou		= 2008,
	skill_ma_liuxingyu			= 2009,
	skill_ma_youyuanbing		= 2010,
	skill_ma_diyuhuo 			= 2011,
	skill_ma_longjuanren		= 2012,

	skill_r_shihua_dikang		= 1501,
	skill_r_yiwang_dikang		= 1502,
	skill_r_hunluan_dikang	= 1503,
	skill_r_zuzhou_dikang		= 1504,
	skill_r_judu_dikang		= 1505,
	skill_r_shuimian_dikang	= 1506,

	skill_ms_shihuazhizhen	= 2501,
	skill_ms_yiwangzhizhen	= 2502,
	skill_ms_hunluanzhizhen	= 2503,
	skill_ms_yuanlingzhifu	= 2504,
	skill_ms_juduzhizhen		= 2505,
	skill_ms_shuimianzhizhen	= 2506,
	skill_ms_shihualingyu		= 2507,
	skill_ms_yiwanglingyu		= 2508,
	skill_ms_hunluanlingyu	= 2509,
	skill_ms_yuanlinglingyu	= 2510,
	skill_ms_judulingyu		= 2511,
	skill_ms_shuimianlingyu	= 2512,
	skill_ms_shihuajiejie		= 2513,
	skill_ms_yiwangjiejie		= 2514,
	skill_ms_hunluanjiejie	= 2515,
	skill_ms_yuanlingjiejie	= 2516,
	skill_ms_judujiejie		= 2517,
	skill_ms_shuimianjiejie	= 2518,
	skill_ms_shuxingfanzhuan	= 2519,

	skill_mas_zaishengzhizhen	= 3001,
	skill_mas_zaishenglingyu	= 3002,
	skill_mas_zaishengjiejie	= 3003,
	skill_mas_wulixishou		= 3004,
	skill_mas_mofaxishou		= 3005,
	skill_mas_wulifantan		= 3006,
	skill_mas_mofafantan		= 3007,
	skill_mas_wulibizhang		= 3008,
	skill_mas_mofabizhang		= 3009,
	skill_hw_huwei              = 3501,


	skill_pet_begin				= 5000,
	skill_pet_run_away			= 5001,

	skill_pet_break_off         = 5002,//逃跑
	skill_pet_use_chemical			= 5004,

	skill_pet_chg_position      = 5005, //换位 
	skill_pa_pet_dai_ji         = 5008,//待机
	skill_pa_pet_base_fight		= 5010,//普通攻击
	skill_pa_pet_heji			= 5011,
	skill_pa_pet_xianfazhiren   = 5101,//先制
	skill_pa_pet_zhuiji			= 5102,
	skill_pa_pet_bengya			= 5103,
	skill_pa_pet_chuanci		= 5104,
	skill_pa_pet_jizhonggongji	= 5105,
	skill_pa_pet_kuangji		= 5106,//狂击
	skill_pa_pet_douqi 			= 5107,//斗气
	skill_pa_pet_huishang		= 5108,//毁伤
	skill_pa_pet_duochongjian	= 5109,
	skill_pa_pet_shuangxingjian = 5110,
	skill_pa_pet_ninshenjian	= 5111,
	skill_pa_pet_meiying		= 5112,
	
	skill_pet_sanyeshi			= 5113,
	skill_pet_feiyejian 			= 5114,
	skill_pet_muyedun				= 5115,
	skill_pet_shunsha				= 5116,

	skill_hm_pet_pohun			= 5501,

	skill_pd_pet_fangyu			= 5701,//防御
	skill_md_pet_mokang         = 5702,
	skill_pd_pet_huandun		= 5703,
	skill_pd_pet_huiji			= 5704,
	
	skill_rh_pet_mingsi			= 6001,//冥思
	skill_rh_pet_zhiyuzhizhen	= 6002,
	skill_rh_pet_zhiyulingyu	= 6003,
	skill_rh_pet_zhiyujiejie	= 6004,

	skill_rs_pet_bingqingzhizhen	= 6005,
	skill_rs_pet_bingqinglingyu 	= 6006,
	skill_rs_pet_bingqingjiejie 	= 6007,
	skill_rl_pet_jiushu 			= 6008,

	skill_rl_pet_jiushu_all_only_svr = 6109,//复活所有死的打飞的怪物,程序内部使用，用户不用使用

	skill_rh_pet_zhiyuhuahuan	= 6201,

	//6202：狂野之魂
	skill_rh_pet_kuangyezhifun = 6202,
	//弱点透析
	skill_rh_pet_ruodiantouxi		 =6203,

	skill_fh_pet_chouqu			= 6801,
	skill_ma_pet_yunshishu		= 7001,//
	skill_ma_pet_shuangdongshu	= 7002,//
	skill_ma_pet_liuhuoshu		= 7003,//
	skill_ma_pet_xuanfengshu	= 7004,//
	skill_ma_pet_jushizhou		= 7005,//
	skill_ma_pet_hanbingzhou	= 7006,//
	skill_ma_pet_liehuozhou		= 7007,//
	skill_ma_pet_kuangfengzhou	= 7008,//
	skill_ma_pet_liuxingyu		= 7009,
	skill_ma_pet_youyuanbing	= 7010,
	skill_ma_pet_diyuhuo 		= 7011,
	skill_ma_pet_longjuanren	= 7012,

	skill_pet_shihua_dikang		= 6501,
	skill_pet_yiwang_dikang		= 6502,
	skill_pet_hunluan_dikang	= 6503,
	skill_pet_zuzhou_dikang		= 6504,
	skill_pet_judu_dikang		= 6505,
	skill_pet_shuimian_dikang	= 6506,

	skill_ma_pet_zhimianfenzhen	= 7201,

	//7202：万兽噬身
	skill_ma_pet_wanshoushishen = 7202,
	
	skill_ma_pet_dubaoshu =7203,

	
	skill_ms_pet_shihuazhizhen	  = 7501,
	skill_ms_pet_yiwangzhizhen	  = 7502,
	skill_ms_pet_hunluanzhizhen   = 7503,
	skill_ms_pet_yuanlingzhifu	  = 7504,
	skill_ms_pet_juduzhizhen	  = 7505,
	skill_ms_pet_shuimianzhizhen  = 7506,
	skill_ms_pet_shihualingyu	  = 7507,
	skill_ms_pet_yiwanglingyu	  = 7508,
	skill_ms_pet_hunluanlingyu	  = 7509,
	skill_ms_pet_yuanlinglingyu   = 7510,
	skill_ms_pet_judulingyu 	  = 7511,
	skill_ms_pet_shuimianlingyu   = 7512,
	skill_ms_pet_shihuajiejie	  = 7513,
	skill_ms_pet_yiwangjiejie	  = 7514,
	skill_ms_pet_hunluanjiejie	  = 7515,
	skill_ms_pet_yuanlingjiejie   = 7516,
	skill_ms_pet_judujiejie 	  = 7517,
	skill_ms_pet_shuimianjiejie   = 7518,
	skill_ms_pet_shuxingfanzhua   = 7519,
	/*
8802：猛兽猎袭
7202：万兽噬身
6202：狂野之魂
*/



	skill_mas_pet_zaishengzhizhen	= 8001,
	skill_mas_pet_zaishenglingyu	= 8002,
	skill_mas_pet_zaishengjiejie	= 8003,
	skill_mas_pet_wulixishou		= 8004,
	skill_mas_pet_mofaxishou		= 8005,
	skill_mas_pet_wulifantan		= 8006,
	skill_mas_pet_mofafantan		= 8007,
	skill_mas_pet_wulibizhang		= 8008,
	skill_mas_pet_mofabizhang		= 8009,
	skill_pet_recallbeast			= 8010,
	skill_hw_pet_huwei              = 8501,
	skill_ph_pet_xixuegongji		= 8701,//吸血攻击
	skill_ph_pet_shihua			= 8702,
	skill_ph_pet_yiwang			= 8703,
	skill_ph_pet_hunluan			= 8704,
	skill_ph_pet_zuzhou			= 8705,
	skill_ph_pet_zhongdu			= 8706,
	skill_ph_pet_hunshui			= 8707,

	skill_ph_pet_chanshentengman	= 8801,
	//猛兽猎袭
	skill_ph_pet_mengshouliexi = 8802,
//高频振击
	skill_ph_pet_gaopinzhenji =8803,
	skill_auto_attack				= 1000000
};

typedef uint64_t map_id_t;
typedef uint32_t userid_t;

typedef struct skill_info {
	uint32_t		skill_id;
	uint8_t			skill_level;
	uint8_t			use_level;
	uint32_t		skill_exp;//如果是宠物，1:是天赋技能
	uint16_t		rand_rate;
	uint8_t			cool_round;
	uint8_t			cool_update;
}__attribute__((packed))skill_info_t;

typedef struct body_cloth {
	uint32_t 			cloth_id;
	uint32_t			type_id;
	uint16_t 			clevel;
	int16_t            	durable_val;
	uint8_t             equip_type;
}__attribute__((packed))body_cloth_t;

#endif
