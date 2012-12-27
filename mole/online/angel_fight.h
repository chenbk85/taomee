#ifndef ANGEL_FIGHT_H
#define ANGEL_FIGHT_H

#define MAX_SKILL_LEVEL_CNT 10
#define MAX_TASK_ITEM_CNT	4

#define ANGEL_FIGHT_DONATE_ITEM_ID	1351124

//基本属性
typedef struct af_user_info {
	uint32_t cur_exp;
	uint32_t max_exp;
	uint32_t level;
	uint32_t iq;
	uint32_t smart;
	uint32_t power;
	uint32_t strong;
	uint32_t hp;
	uint32_t mp;
	uint32_t energy;
	uint32_t max_energy;
	uint32_t vigour;
	uint32_t max_vigour;
	uint32_t get_point;
}__attribute__((packed)) af_user_info_t;

//技能升级的条件
typedef struct {
	uint32_t skill_lvl; //技能等级
	uint32_t add_val[4];//增加的属性值
	uint32_t add_hp;	//增加生命值
	uint32_t exp_lvl;	//角色等级
    uint32_t need_cnt;	//所需卡片的数量
	uint32_t need_money;//所需游戏币
	uint32_t rate;		//升级的成功率
}__attribute__((packed)) af_skill_level_t;

//技能
typedef struct af_skill {
    uint32_t skill_id;	//技能ID
	uint32_t skill_type;
    uint32_t card_id;	//卡片ID
    af_skill_level_t skill_level[MAX_SKILL_LEVEL_CNT];	//每一级的属性
}__attribute__((packed)) af_skill_t;

typedef struct af_task {
	uint32_t task_id;	//任务ID
	uint32_t task_type;	//任务类型
	uint32_t low_lvl;	//接受任务的最低等级限制
	uint32_t high_lvl;	//接受任务的最高等级限制
	uint32_t max_cnt;	//完成任务所需的次数
	uint32_t event_id;	//task_type=1、5、6时，1(许愿祝福) 2（你快回来） 3(抱抱你) 4(危机见真情)5（互相帮助） task_type=2、3、4时，关卡ID
	uint32_t out_cnt;	//输出奖励的个数
	item_unit_t item_out[MAX_TASK_ITEM_CNT];	//任务送的奖励
}__attribute__((packed)) af_task_t;

typedef struct {
	uint32_t apprentice_id;	//徒弟ID
	uint32_t expel_type;	//1师傅主动赶徒弟 2徒弟背叛师傅
}__attribute__((packed)) db_expel_apprentice_package_t;

typedef struct {
	uint32_t retray_type;	//1徒弟背叛师傅 2师傅主动赶徒弟
}__attribute__((packed)) db_retray_master_package_t;

enum FRIGHT_TYPE {
	FT_NORMAL   = 0,
	FT_BOSS,
	FT_BOSS_TIME,
	FT_BOSS_VIP,
	FT_BOSS_COST,
	FT_FRIEND,
	FT_GET_APPRENTICE,      //收徒弟
	FT_GRAB_APPRENTICE,     //抢徒弟
	FT_PK_MASTER,           //挑战师傅
	FT_PK_APPRENTICE,       //挑战徒弟
	FT_TRAIN,               //切磋武艺
	FT_PK,					//PVP
};

//拉取基本属性 8200
int af_get_user_base_info_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int af_get_user_base_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取角色背包 8201
int af_get_user_package_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int af_get_user_package_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取卡册  8203
int af_get_fight_card_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int af_get_fight_card_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取技能 8202
int af_get_fight_skill_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int af_get_fight_skill_callback(sprite_t* p, uint32_t id, char* buf, int len);

//使用道具 8204
int af_use_item_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int af_use_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_use_gift_package_callback(sprite_t* p);

int load_af_skill(const char* file);
af_skill_t* af_get_skill_from_skill_id(uint32_t skill_id);
af_skill_t* af_get_skill_from_card_id(uint32_t card_id);

//卡牌使用 8205
int af_use_card_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int af_use_card_callback(sprite_t* p, uint32_t id, char* buf, int len);

//技能升级 8206
int af_user_skill_level_up_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_skill_level_up_callback(sprite_t* p, uint32_t id, char* buf, int len);

//更换技能或装备 8207
int af_user_change_equip_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_change_equip_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取所有好友信息 8208
int af_get_all_friends_cmd(sprite_t* p, const uint8_t *body, int len);
int af_get_all_friends_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取所有好友今日对战情况 8209
int af_get_all_friends_fight_cmd(sprite_t* p, const uint8_t *body, int len);
int af_get_all_friends_fight_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取某一地图的所有关卡完成度 8210
int af_get_map_pass_point_cmd(sprite_t* p, const uint8_t *body, int len);
int af_get_map_pass_point_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取一个技能的等级 8211
int af_get_one_skill_level_cmd(sprite_t* p, const uint8_t *body, int len);
int af_get_one_skill_level_callback(sprite_t* p, uint32_t id, char* buf, int len);

//check是否可以翻牌 8212
int af_check_user_get_prize_cmd(sprite_t* p, const uint8_t *body, int len);
int af_check_user_get_prize_callback(sprite_t* p, uint32_t id, char* buf, int len);

//用户翻牌 8213
int af_user_get_prize_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_prize_callback(sprite_t* p, uint32_t id, char* buf, int len);

//用户许愿 8214
int af_user_set_wishing_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_set_wishing_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_user_check_wishing_callback(sprite_t* p, uint32_t id, char* buf, int len);

//帮助好友实现愿望 8215
int af_deal_friend_wishing_cmd(sprite_t* p, const uint8_t *body, int len);
int af_get_friend_wishing_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_deal_friend_wishing_callback(sprite_t* p, uint32_t id, char* buf, int len);

//用户当前许愿状态 8216
int af_user_get_wishing_state_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_wishing_itemid_callback(sprite_t* p, uint32_t id, char* buf, int len);

//处理好友事件 8217
int af_deal_friend_event_cmd(sprite_t* p, const uint8_t *body, int len);
int af_get_friend_event_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_deal_friend_event_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_check_friend_event_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取指定数量的好友对战、事件信息 8218
int af_user_get_some_friends_info_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_some_friends_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_user_get_some_friends_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取PVP对战记录 8219
int af_user_get_fight_record_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_fight_record_callback(sprite_t* p, uint32_t id, char* buf, int len);

//获得许愿收集品 8220
int af_user_get_wishing_collect_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_wishing_collect(sprite_t* p, uint32_t wish_cnt);
int af_user_get_wishing_collect_callback(sprite_t* p, uint32_t id, char* buf, int len);

//读取每日任务配置文件
int load_af_task(const char* file);
int get_some_task_without_in_array(uint32_t* ptask, uint32_t task_idx, uint32_t get_cnt, uint32_t level, uint32_t cur_task_id);
af_task_t* get_task_from_task_id(uint32_t task_id);

//拉取每日任务状态 8221
int af_user_get_daily_task_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_daily_task_callback(sprite_t* p, uint32_t id, char* buf, int len);

//完成或任务终结卡完成任务 8222
int af_user_finish_one_task_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_check_finish_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_user_add_fight_exp_callback(sprite_t* p, uint32_t id, char* buf, int len);

//刷新任务 8223
int af_user_refresh_one_task_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_check_refresh_task_callback(sprite_t* p, uint32_t id, char* buf, int len);

//每日boss对战次数 8224
int af_user_get_fight_boss_count_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_fight_boss_count_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取徒弟市场信息 8225
int af_user_get_apprentice_market_cmd(sprite_t* p, const uint8_t *body, int len);
int af_get_master_and_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_user_get_apprentice_market_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取师徒信息 8226
int af_get_master_apprentice_info_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_apprentice_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_user_get_master_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//判断是否可以收徒 8227
int af_check_get_an_apprentice_cmd(sprite_t* p, const uint8_t *body, int len);
int af_check_get_an_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_check_apprentice_get_master_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取训练信息 8228
int af_get_apprentice_trainning_info_cmd(sprite_t* p, const uint8_t *body, int len);
int af_get_apprentice_trainning_info_callback(sprite_t* p, uint32_t id, char* buf, int len);

//训练徒弟 8229
int af_user_train_apprentice_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_train_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len);

//逐出师门 8230
int af_user_expel_apprentice_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_expel_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len);

//徒弟孝敬师傅（感谢金） 8231
int af_apprentice_respect_master_cmd(sprite_t* p, const uint8_t *body, int len);
int af_apprentice_respect_master_callback(sprite_t* p, uint32_t id, char* buf, int len);

//师傅领取感谢金 8232
int af_user_get_respect_cash_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_respect_cash_callback(sprite_t* p, uint32_t id, char* buf, int len);

//背叛师门 8233
int af_apprentice_retray_master_cmd(sprite_t* p, const uint8_t *body, int len);
int af_apprentice_retray_master_callback(sprite_t* p, uint32_t id, char* buf, int len);

//出师（师傅和徒弟出师都用这条） 8234
int af_finish_apprentice_cmd(sprite_t* p, const uint8_t *body, int len);
int af_apprentice_check_finish_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_master_finish_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_apprentice_finish_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len);

//师徒消息 8235
int af_user_get_master_record_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_master_record_callback(sprite_t* p, uint32_t id, char* buf, int len);

//收徒对战结束后，用户收徒 8236
int af_user_add_apprentice_cmd(sprite_t* p, const uint8_t *body, int len);
int af_user_get_level_callback(sprite_t* p, uint32_t id, char* buf, int len);
int af_user_add_apprentice_callback(sprite_t* p, uint32_t id, char* buf, int len);

//添加师徒交互消息
int af_add_master_apprentice_msg(uint32_t id, uint32_t master, uint32_t apprentice, uint32_t msg_id, uint32_t exp, int32_t merit);

//拉取PVP对战次数及兑换记录 8237
int af_get_pvp_fight_record_cmd(sprite_t* p, const uint8_t *body, int len);
int af_get_pvp_fight_record_callback(sprite_t* p, uint32_t id, char* buf, int len);

//PVP获胜次数兑换成就卡 8238
int af_get_pvp_win_prize_cmd(sprite_t* p, const uint8_t *body, int len);
int af_get_pvp_win_prize_callback(sprite_t* p, uint32_t id, char* buf, int len);

//拉取竞技场信息
int af_get_fight_arena_cmd();
int af_get_fight_arena_callback();

//捐献卡片
int af_donate_card_cmd(sprite_t* p, const uint8_t *body, int len);

//兑换卡片
int af_exchange_card_cmd(sprite_t* p, const uint8_t *body, int len);

//查询捐献点数
int af_get_donate_info_cmd(sprite_t* p, const uint8_t *body, int len);

#endif
