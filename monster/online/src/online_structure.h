/** *
=====================================================================================
 *       @file  online_structrue.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/20/2011 08:49:39 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_ONLINE_STRUCTURE_H_20110711
#define H_ONLINE_STRUCTURE_H_20110711

extern "C"
{
#include <async_serv/net_if.h>
#include <libtaomee/list.h>
}

#include "data_structure.h"
#include "./online_constant.h"

#pragma pack(push)
#pragma pack(1)

/**
 * @brief 记录每个包的处理时间
 */
typedef struct
{
    uint16_t msg_type;
    uint8_t time;
} process_time_t;

/**
 * @brief 保存用户更新的房间信息
 */
typedef struct
{
    uint32_t room_id;
    uint16_t buf_len;
    char buf[0];
} room_update_t;

/**
 * @brief 用户登录基本信息的缓存
 */
typedef struct
{
    role_t role;
   // uint32_t approved_message_num;
   // uint32_t unapproved_message_num;
    uint16_t pending_req_num;
    uint32_t last_update_health_time;
    uint16_t unread_visits;
} role_cache_t;

/**
 * @brief 用户房间缓存的value
 */
typedef struct
{
    uint16_t buf_len;
    char buf[MAX_ROOM_STUFF * 32];
} room_value_t;

/**
 * @brief 用户房间摆放的物品
 */
typedef struct
{
    uint16_t stuff_num;
    as_msg_buy_stuff_req_t stuff[0];
} room_stuff_t;

/**
 * @brief 物品的数量和已经使用的数量
 */
typedef struct
{
    uint16_t stuff_num;
    uint16_t used_num;
} stuff_num_t;

/**
 * @brief 解析as记录的物品摆放位置
 */
typedef struct
{
    uint32_t stuff_id;
    uint16_t depth_index;
    uint32_t pos_x;
    uint32_t pos_y;
    uint16_t child_num;
} stuff_child_t;

/**
 * @brief 商店里购买商品的信息
 */
typedef struct
{
    uint8_t category;
    uint16_t price;
    uint16_t level;
    uint16_t happy;
    uint16_t health;
    uint8_t rarity; //物品的稀有程度，0-3,0表示普通物品
} item_t;

/**
 * @brief 缓存里保持的每个类型puzzle对应的信息
 */
typedef struct
{
    uint32_t last_playtime;
    uint16_t max_score;
    uint32_t total_score;
    uint32_t total_num;
	uint16_t play_times;
} puzzle_info_t;



typedef struct
{
    uint8_t count;
    uint16_t page;
    uint32_t peer_id;
} pinboard_count_t;


typedef struct
{
    uint8_t page;
    uint16_t count;
    db_msg_get_friend_rsp_t role_info[0];
} role_list_t;

/**
 * @brief 好友申请要保存的每个用户的信息
 */
typedef struct
{
    uint32_t user_id;
    char name[16];
    uint8_t gender;
    uint32_t country_id;
    uint8_t user_type;
    uint32_t birthday;
    uint32_t mon_id;
    uint32_t mon_main_color;
    uint32_t mon_ex_color;
    uint32_t mon_eye_color;
  //  uint8_t remark_count;
  //  char remark[MAX_REMARK_COUNT];
} apply_info_t;

/**
 * @brief 给as的好友申请的返回信息
 */
typedef struct
{
    uint8_t flag;
    uint8_t count;
    uint8_t cur_count;
    apply_info_t apply[0];
} friend_apply_rsp_t;

typedef struct
{
    uint32_t user_id;
    uint16_t mon_level;
} best_friend_key_t;

typedef struct
{
    uint32_t user_id;
    uint32_t last_login_time;
} normal_friend_key_t;

typedef struct
{
    uint32_t last_login_time;
    uint16_t mon_level;
    uint8_t is_best_friend;
} friend_cmp_t;

/**
 * @brief 好友列表要保存的每个好友的信息
 */
typedef struct
{
    char name[16];
    uint8_t gender;
    uint32_t country_id;
    uint8_t user_type;
    uint32_t birthday;
    uint32_t last_login_time;
    uint32_t mon_id;
    uint32_t mon_main_color;
    uint32_t mon_ex_color;
    uint32_t mon_eye_color;
    uint16_t mon_level;
    uint32_t user_id;
    uint8_t is_best_friend;
} friend_list_t;

/**
 * @brief 给as的好友列表的返回信息
 */
typedef struct
{
    uint16_t count;
    friend_list_t friend_list[0];
} friend_rsp_t;

typedef struct
{
    uint16_t cur_count;
    uint8_t page;
    uint8_t page_num;
    uint32_t peer_id;
    uint16_t begin_index;
    db_msg_friend_list_rsp_t friend_list;
} friend_cache_t;

typedef struct
{
    uint8_t count;
    uint8_t page;
    uint8_t page_num;
    uint32_t user_id;
} friend_page_t;

typedef struct
{
    uint16_t count;
    pet_info_t pet[0];
} all_pet_t;

typedef struct
{
    uint32_t seed_id;
    uint8_t color;
} seed_match_t;

typedef struct
{
    uint8_t count;
    uint32_t user_id[0];
} active_user_t;

typedef struct
{
    uint32_t user_id;
    db_msg_street_user_rsp_t stranger_info;
} stranger_info_t;

typedef struct
{
    uint8_t cur_num;
    uint8_t count;
    stranger_info_t stranger[0];
} stranger_t;

typedef struct
{
    uint16_t cur_level_min;
    uint16_t cur_level_max;
    uint16_t next_level_min;
    uint16_t next_level_max;
} shop_num_t;

typedef struct
{
    uint32_t id;
    uint8_t type;
} shop_key_t;

typedef struct
{
    uint32_t time;
    uint32_t type;
} restrict_key_t;

typedef struct
{
    db_msg_puzzle_req_t puzzle_reward;
    uint16_t extra_coins;
    uint32_t stuff_id;
    uint8_t category;
    db_msg_update_plant_req_t plant_change;
} commit_puzzle_cache_t;



typedef struct
{
    uint8_t category;//暂时保留
    db_msg_add_stuff_req2_t add_stuff;
    uint32_t old_happy;
    db_msg_update_plant_req_t plant_change;
}commit_stuff_cache_t;

typedef struct
{
    db_msg_puzzle_req_t puzzle_reward;
    uint16_t extra_coins;
    uint32_t old_happy;
    db_msg_update_plant_req_t plant_change;
}puzzle_cache_t;

typedef struct
{
    uint8_t type;
    uint32_t game_id;
    uint32_t level_id;
    uint32_t game_key;//为game_id和level_id的组合
    uint8_t coin_restrict; //奖励金币 0:正常 1：达到每日上限
    uint32_t reward_coins;
    uint32_t score;
    uint32_t max_coins;
    uint8_t need_update_db;//是否需要更新数据库 0:不需要 1：需要
    uint8_t star_num;//获得星星数量
    uint8_t is_passed;//本次玩的分数是否通关    0:未通关 1：通关
    uint8_t reason; //没有获得道具奖励的原因(0:正常 1：分数不够 2：达到每日限制)
    uint8_t reward_items;
    uint32_t reward_item_score;//奖励道具的最低分数要求
   // uint8_t count;
    restrict_value_t value[0];
} game_t;

typedef struct
{
    int num;
    int item_num;
    int item_id[100];
} game_item_t;

typedef struct
{
    uint32_t level_id;          //关卡id
    uint32_t one_star_score;    //获得一颗星需要的分数
    uint32_t two_star_score;
    uint32_t three_star_score;
    uint32_t finished_score; //过关所需要的最低分数
    uint16_t unlock_cond;   //解锁这一关需要的星星数量
    uint32_t reward_item_score;//奖励道具的最低分数要求
}game_level_item_t;

typedef struct
{
    uint8_t percent;
    uint32_t coins;
    uint32_t item_id;
} stuff_trade_t;

typedef struct
{
    uint32_t total_num;
    uint8_t follow_num;
} pet_num_t;

typedef struct
{
    uint16_t min_rewards;
    uint16_t max_rewards;
}interactive_rewards_t;

typedef struct
{
    uint32_t seed1;
    uint32_t seed2;
    uint32_t seed3;
} seed_reward_t;

typedef struct
{
    two_puzzle_result_t result;
    uint16_t msg_type;
} two_puzzle_req_t;

typedef struct
{
    uint32_t user_id;
    uint32_t timestamp;
    uint8_t status;
    uint8_t type;
    char name[16];
    uint8_t is_vip;
    uint32_t mon_id;
    uint32_t main_color;
    uint32_t ex_color;
    uint32_t eye_color;
} visit_info_t;

typedef struct
{
    uint8_t count;
    uint8_t cur_index;
    visit_info_t visit_info[0];
} latest_visit_t;

typedef struct
{
        uint32_t user_id;
        uint32_t timestamp;
        uint8_t type;
        uint8_t status;
} visit_ucount_t;

typedef struct
{
    uint8_t count;
    visit_ucount_t visit_info[0];
} visit_id_t;

typedef struct
{
    uint16_t remain_time;
    uint16_t count;
    uint32_t item_id[0];
} shop_t;

typedef struct
{
    uint32_t badge_cond;        //成就项条件
    uint32_t badge_reward_id;  //成就项奖品id
    uint8_t badge_type;         //成就项类型，(0:SET 1:SUM)
    uint8_t badge_progress_type; //成就项进度类型(记录最高分还是最新成绩)(0:最高分(TOP) 1:最新成绩(LATEST)))
}badge_xml_info_t; //成就项xml表中的信息

typedef struct
{
    uint32_t item_id;
    uint16_t item_num;
} factory_cost_t;

typedef struct
{
    uint32_t item_id;
    uint8_t type;
} factory_op_t;

typedef struct
{
    uint8_t unlock_type;
    uint32_t value;
} factory_unlock_t;

typedef struct
{
    uint32_t game_id;
    uint32_t total_level;
}game_level_t;

typedef struct
{
    uint16_t coins;
    uint8_t paper_term;
} bobo_cache_t;

typedef struct
{
    uint8_t type;
    uint16_t reward_coins;
    uint16_t reward_growth;
    uint16_t reward_happy;
    uint16_t reward_exp;
    uint8_t rate;
}maintain_type_t;

typedef struct
{
	uint32_t museum_id;
	uint8_t level_id;
	uint8_t question_num;
	uint32_t reward_id;
	uint16_t reward_num;
}level_item_t;

typedef struct
{
    char name[16];
}user_name_t;

typedef struct
{
    uint32_t activity_id;
    uint32_t start_timestamp;
    uint32_t end_timestamp;
    std::vector<task_reward_t> reward_vec;
}activity_info_t;//活动配表的信息

typedef struct
{
    pinboard_message_t message;
    db_msg_get_friend_rsp_t user_info;
} pinboard_cache_t;

typedef struct
{
    uint8_t count;
    pinboard_cache_t message[0];
} pinboard_t;

typedef struct
{
    uint32_t peer_id;
    uint32_t real_num;
}peer_real_msg_num_t;


#pragma pack(pop)

#endif // H_LOGIN_STRUCTURE_H_20110711
