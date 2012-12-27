/*
 * =========================================================================
 *
 *        Filename: Citem_conf.h
 *
 *        Version:  1.0
 *        Created:  2011-07-22 14:31:44
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#ifndef  CITEM_CONF_H
#define  CITEM_CONF_H
#include <stdint.h>
#include <vector>
#include <set>
#include  <map>

#define MAX_ITEM_COUNT		99999
#define MAX_SPEC_ITEM_COUNT	999999

enum {
	spec_item_task			= 1,	// 任务
	spec_item_xiaomee		= 2,	// 卜克豆
	spec_item_game_point	= 3,	// 小游戏积分
	spec_item_day_oltime	= 4,	// 在线时长

	// 以下三项用于（2011.12.9）的运营活动
	spec_item_work_count	= 5,	// 不再使用
	spec_item_lottery_got	= 6,	// 不再使用
	spec_item_lottery_used	= 7,	// 不再使用
	spec_item_game_chris_cert_get	= 8, // 不再使用
	spec_item_game_chris_cert_rand	= 9, // 不再使用
	spec_item_lottery_got_12_1_5	= 10, // 不再使用
	spec_item_lottery_used_12_1_5	= 11, // 不再使用

	spec_item_experience	= 12,	// 人物经验

	spec_item_pet_egg_count		= 13,

	// 每日登录奖励
	spec_item_login_count		= 14,
	spec_item_chest_get			= 15,
	spec_item_login_reward_1	= 16,
	spec_item_login_reward_2	= 17,
	spec_item_login_reward_3	= 18,
	spec_item_login_reward_5	= 19,
	spec_item_login_reward_10	= 20,
	spec_item_login_reward_15	= 21,
	spec_item_login_reward_20	= 22,
	spec_item_login_reward_0	= 23,
	spec_item_game_point_used	= 24,

	spec_item_npc_talk_begin	= 10000,
	spec_item_npc_talk_end		= 10300,

	spec_item_max			= 50000,

	item_christmas_cert			= 200043,
	item_christmas_suit			= 300025,
	item_christmas_hat			= 200064,
	item_snowflack				= 400008,

	item_bobo_baby_suit			= 300012,
	item_bobo_rabit_suit		= 300024,

	item_invitation				= 200068,

	item_eye_blue				= 400011,
	item_eye_red				= 400012,
	item_eye_green				= 400013,
	item_pet					= 400014,
};

enum {
	rand_id_login_chest		= 1,
};

struct item_conf_t {
	uint32_t itemid;	
	//是否可以换装
	uint32_t change;	

	std::set<uint32_t> islandid_list;	
	uint32_t clothtype;	

	uint32_t type;

	//来源 1:购买方式得到
	uint32_t source;	

	//多少钱
	uint32_t pay;	


	// 所能拥有的总上限
	uint32_t total_max;
	// 每日获得上限
	uint32_t day_max;
	// 所包含的物品列表(卡片)
	std::vector<uint32_t> comprise_itemid_list;
};

struct source_conf_t {
	//来源 1:购买方式得到
	uint32_t source;	
	//多少钱
	uint32_t pay;	
	// 需要多少积分
	uint32_t gamept;
};
class Citem_conf_map {
	private:
		//source
		std::map<uint32_t ,source_conf_t > source_map;
		// 岛屿勋章列表
		std::set<uint32_t> medal_set;
	public:
		//kindid , itemid, item_conf_t 
		std::map<uint32_t, std::map<uint32_t,  item_conf_t> > conf_map;

		std::map<uint32_t ,std::vector<uint32_t> > cardid_comprise_map;
		void add(uint32_t kindid , item_conf_t & item_conf );
		void add_medal(uint32_t medalid);
		
		//检查物品是否可能随便换
		bool check_can_change(uint32_t itemid )   ;
		bool is_on_island(uint32_t itemid, uint32_t islandid)   ;
		uint32_t get_clothtype(uint32_t itemid )   ;
		uint32_t get_type(uint32_t itemid )   ;

		source_conf_t * get_buy_info( uint32_t itemid );
		source_conf_t * get_item_info( uint32_t itemid );
		bool is_medal(uint32_t itemid);
		bool is_cloth(uint32_t itemid);
		item_conf_t * get_item_conf(uint32_t kindid, uint32_t itemid);
};

class Citem_movement_conf {
	private:
		std::map<uint32_t, uint32_t> item_movement_map;
	public:
		uint32_t get_movement_item(uint32_t exprid);
		void init();
		void add_movement(uint32_t exprid, uint32_t itemid);
};

class Citem;
class Citem_rand {
	public:
		uint32_t proability;
		std::vector<Citem> item_list;
};

class Citem_rand_group {
	public:
		uint32_t randbase;
		std::vector<Citem_rand> item_list;
};

#endif  /*CITEM_CONF_H*/
