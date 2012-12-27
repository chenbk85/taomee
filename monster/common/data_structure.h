/*
 * =====================================================================================
 *
 *       Filename:  message.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月11日 15时58分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_DATA_STRUCTURE_H_20110711
#define H_DATA_STRUCTURE_H_20110711

#include <stdint.h>
#include <vector>
#include "constant.h"

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    char name[16];
    uint8_t gender;
    uint32_t country_id;
    uint8_t user_type;
    uint32_t birthday;
    uint32_t register_time;
    uint32_t coins;
    uint32_t last_login_time;
    uint32_t last_logout_time;
	uint32_t is_first_login;
    uint32_t last_visit_plantation_time;
    uint32_t monster_id;
    char monster_name[16];
    uint32_t monster_main_color;
    uint32_t monster_ex_color;
    uint32_t monster_eye_color;
    uint32_t monster_exp;
    uint16_t monster_level;
    uint32_t monster_health;
    uint32_t monster_happy;
    uint16_t friend_num;
    uint32_t thumb;
    uint32_t visits;
    uint16_t recent_unread_badge;//最近未读的成就数
    uint8_t mood;
    uint8_t fav_color;
    uint8_t fav_pet;
    uint8_t fav_fruit;
    char personal_sign[64];
    uint8_t room_num;
    uint8_t pet_num;
    uint16_t max_puzzle_score;
    uint32_t flag1;     //用于新手引导，每一位表示一个新手引导是否完成
    uint32_t compose_id;    //合成工厂的合成物品id
    uint32_t map_id;        //合成工厂的合成图纸id
    uint32_t compose_time;  //合成开始的时间
    uint32_t online_time;   //合成开始的时间
    uint32_t npc_score_daytime;   //npc打分的时间(当天的起始时间)
    uint32_t npc_score;           //npc打的分数
    uint16_t last_paper_reward;   //上一次卜卜报领奖的期数
    uint16_t last_paper_read;     //上一次读的卜卜报的期数
    uint32_t offline_time;  //合成开始的时间
    uint32_t guide_flag;//强制引导标志
    uint8_t limit_reward; //封测大礼是否领取(0:未领取 1：已经领取)
	uint32_t invitor_id;//邀请人米米号
    uint32_t approved_message_num;
    uint32_t unapproved_message_num;
    uint32_t last_show_id;//用户上次参与的大众点评届数
} role_t;

typedef struct
{
    char name[16];
    uint8_t gender;
    uint32_t country_id;
    uint8_t user_type;
    uint32_t birthday;
    uint32_t monster_id;
    char monster_name[16];
    uint32_t monster_main_color;
    uint32_t monster_ex_color;
    uint32_t monster_eye_color;
    uint32_t monster_exp;
    uint16_t monster_level;
    uint32_t monster_health;
    uint32_t monster_happy;
    uint16_t friend_num;
    uint32_t visits;
    uint8_t mood;
    uint8_t fav_color;
    uint8_t fav_pet;
    uint8_t fav_fruit;
    char personal_sign[65];
    uint8_t room_num;
    uint8_t pet_num;
    uint16_t max_puzzle_score;
} profile_t;

/**
 * @brief 默认的房间装饰
 */
typedef struct
{
    uint16_t buf_len;       //整个buf的长度
    uint32_t wallpaper_id;  //墙纸id
    uint32_t door_id;       //门id
    uint32_t floor_id;      //地板id
    uint32_t windows_id;    //窗户id
    uint16_t stuff_num;     // 摆放的物品个数
} user_room_buf_t;

/**
 * @brief 背包中的一个物品
 */
typedef struct
{
    uint32_t stuff_id;
    uint16_t stuff_num;
    uint16_t used_num;
} stuff_t;

typedef struct
{
    uint32_t stuff_id;
    uint16_t stuff_num;
    uint8_t stuff_flow;
} stuff_flow_t;

/**
 * @brief 更新房间buf时表示房间中物品的变化
 */
typedef struct
{
    uint16_t num;
    stuff_flow_t stuff_flow[2 * MAX_ROOM_STUFF];    //每个物品最少16个字节，最多32个字节，所以解析出来的物品个数极限情况下要乘以2
} stuff_change_t;


/**
 * @brief 每种类型的puzzle保持的信息
 */
typedef struct
{
    uint8_t type;
    uint32_t last_playtime;
    uint16_t max_score;
    uint32_t score;
    uint32_t num;
} puzzle_t;

/**
 * @brief 益智游戏答题的奖励
 */
typedef struct
{
    uint16_t coins;
    uint16_t exp;
    uint8_t level;
    uint16_t happy;
} puzzle_reward_t;

typedef struct
{
    uint32_t friend_id;
    uint8_t type;
    uint8_t is_bestfriend;
} friend_info_t;

typedef struct
{
    uint32_t pet_id;
    uint16_t total_num;
    uint16_t follow_num;
} pet_info_t;

typedef struct
{
    uint16_t count;
    uint32_t pet_id[0];
} following_pet_t;

typedef struct
{
    uint16_t count;
    pet_info_t pet[0];
} pet_t;

typedef struct
{
    uint8_t hole_id;//坑的编号(1,2,3)
    uint32_t plant_id;//植物编号
    uint16_t color; //颜色编号
    uint16_t growth;//当前的成长值
    uint32_t last_growth_time;//上次成长的时间
    uint32_t last_add_extra_growth_time;//上次增加额外的成长值的时间，每次进种植园都会有额外的成长值
    uint16_t last_growth_value;//上次看到的成长值
    uint8_t maintain;//本次维护选项
    uint32_t next_maintain_time;//下一个15分钟维护的开始时间
    uint32_t last_reward_id;//上一个用到的奖励id
    uint8_t maintain_count;//15分钟内维护过的次数
}hole_info_t;


typedef struct
{
    uint8_t plant_count;//实际的植物的数量
    uint8_t hole_count;// 坑的数量 固定为3
    hole_info_t hole[PLANTATION_NUM];
}all_hole_info_t;


typedef struct
{
    uint16_t reward_happy;//奖励的快乐值
    uint16_t reward_exp;//奖励的经验值
    uint16_t reward_coins;//奖励的咕咚果
}hole_sun_reward_t;//种植园阳光奖励

typedef struct
{
    uint8_t hole_id;
    uint32_t reward_id;
    hole_sun_reward_t sun_reward;
}all_hole_reward_t;

typedef struct
{
    uint32_t reward_num;
    all_hole_reward_t hole_reward[0];
}db_all_hole_reward_rsp_t;

typedef struct
{
    uint32_t reward_num;
    uint32_t reward_id[0];
}single_hole_reward_t;//单个坑的奖励

typedef struct
{
    uint32_t plant_id;
    uint16_t color;
    uint16_t growth;
    uint32_t last_growth_time;
    uint32_t last_add_extra_growth_time;//上次增加额外的成长值的时间，也就是上次进种植园的时间
    uint16_t last_growth_value;
} plant_info_t;

typedef struct
{
    uint8_t hole_id;
    plant_info_t plant_info;
} plant_t;

typedef struct
{
    uint8_t count;
    plant_t plant[0];
} all_plant_t;

typedef struct
{
    uint8_t hole_id;
    uint16_t growth;//这次增加的成长值
    uint32_t growth_time;//这次增加成长值的时间，如果这次没有增加成长值，取值为0
    uint32_t last_add_extra_growth_time;
    uint32_t last_growth_value;//这次增加成长值后的当前成长值
    uint8_t new_maintain_type; //新的维护类型
    uint32_t next_maintain_time;//新的一抡维护开始时间
    uint32_t last_reward_id;//从1开始
    uint8_t maintain_count;//15分钟内维护过的次数
} plant_growth_t;

typedef struct
{
    uint8_t hole_id;
    uint16_t add_growth;
    uint32_t add_growth_time;
    uint8_t new_maintain_type;
    uint8_t maintain_count;
    uint32_t last_reward_id;
    uint16_t reward_happy;
    uint16_t reward_exp;
    uint16_t reward_coins;
}db_msg_maintain_req_t;//向db请求维护操作(浇水，音乐，施肥)

typedef struct
{
    uint8_t hole_id;
    uint16_t reward_coins;
}db_msg_get_plant_req_t;//向db请求收获植物的操作

typedef struct
{
    uint32_t user_id;
    char name[16];
    uint8_t gender;
    uint32_t country_id;
    uint8_t user_type;
    uint32_t birthday;
    uint32_t last_login_time;
    uint32_t monster_id;
    uint32_t monster_main_color;
    uint32_t monster_ex_color;
    uint32_t monster_eye_color;
    uint32_t monster_exp;
} search_friend_item_t;

typedef struct
{
    uint8_t num;
    uint8_t index;
    uint32_t user_id[MAX_ACTIVE_USER_NUM];
} active_user_id_t;

/////////////////////////////////////////////////////////////////////////
//包头结构
/////////////////////////////////////////////////////////////////////////
/**
 * @brief 同AS通信的消息头结构
 */
typedef struct
{
    uint32_t len;
    uint16_t msg_type;
    uint32_t user_id;
    uint32_t result;
    uint32_t err_no;
    char body[0];
} as_msg_header_t;


/**
 * @brief 后台内部通信使用的消息头结构
 */
typedef struct
{
    uint32_t len;
    uint32_t seq_id;
    uint16_t msg_type;
    uint32_t result;
    uint32_t user_id;
    char body[0];
} svr_msg_header_t;

////////////////////////////////////////////////////////////////////////////////////////
//ucount服务的请求包
////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief online向ucount服务的请求包,50301和50302协议
 */
typedef struct
{
    uint32_t user_id;
} ucount_msg_req_t;

/////////////////////////////////////////////////////////////////////////////////
//online和account的请求包和响应包
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief online向请account验证session的请求包体
 */
typedef struct
{
    uint32_t game_id;
    char session[16];
    uint32_t del_session_flag;
} account_msg_check_session_t;



/////////////////////////////////////////////////////////////////////////////////
//online和switch的请求包和响应包
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief online向switch注册时发的请求包体
 */
typedef struct
{
    uint32_t online_id;
    char online_ip[16];
    uint16_t online_port;
    uint8_t zone;//0:电信 1：网通
} online_register_switch_t;

/**
 * @brief online向switch通知有实时消息的请求包体
 */
typedef struct
{
    uint32_t user_id;
    uint8_t type;
} online_notify_t;




/////////////////////////////////////////////////////////////////////////////
//as请求包和响应的包
/////////////////////////////////////////////////////////////////////////////
/**
 * @brief as向online发注册请求的包,40101协议
 */
typedef struct
{
    char name[16];
    uint8_t gender;
    uint16_t country_id;
    uint32_t birthday;
    uint32_t monster_id;
    char monster_name[16];
    uint32_t monster_main_color;
    uint32_t monster_ex_color;
    uint32_t monster_eye_color;
    uint32_t invitor_id;//邀请人米号
} as_msg_register_req_t;

/**
 * @brief as向online注册用户的返回包，40101协议
 */
typedef struct
{
    role_t role;
   // uint32_t approved_message_num;
   // uint32_t unapproved_message_num;
    uint16_t pending_req_num;
    char buf[0];
} as_msg_register_rsp_t;

/**
 * @brief as向online登录请求包,40102协议
 */
typedef struct
{
    char session[16];
} as_msg_login_req_t;

/**
 * @brief as向online拉取房间信息请求包,40104协议
 */
typedef struct
{
    uint32_t user_id;
    uint32_t room_id;
} as_msg_get_room_req_t;

/**
 * @brief as向online更新房间请求包,40105协议
 */
typedef struct
{
    uint32_t room_id;
    char buf[0];
} as_msg_update_room_req_t;

/**
 * @brief as向online获取房间数量的返回包,40106协议
 */
typedef struct
{
    uint16_t room_num;
    uint32_t room_id[0];
} as_msg_room_num_rsp_t;

/**
 * @brief as向online购买物品的请求包,40108协议
 */
typedef struct
{
    uint32_t stuff_id;
    uint16_t stuff_num;
} as_msg_buy_stuff_req_t;

/**
 * @brief as向online提交益智游戏结果的请求包,40110协议
 */
typedef struct
{
    uint8_t type;
    uint16_t total_num;
    uint16_t right_num;
} as_msg_puzzle_req_t;

/**
 * @brief as向online提交益智游戏结果的返回包,40110协议
 */
typedef struct
{
    uint8_t type;
    puzzle_reward_t reward;
} as_msg_puzzle_rsp_t;

/**
 * @brief as向online拉取好友列表的请求包,40111协议
 */
typedef struct
{
    uint8_t page;
    uint8_t page_num;
    uint32_t user_id;
} as_msg_get_friend_req_t;


/**
 * @brief as向online拉取用户信息的请求包,40112协议
 */
typedef struct
{
    uint32_t user_id;
} as_msg_get_user_req_t;

/**
 * @brief as向online好友操作的请求包,40113协议
 */
typedef struct
{
    uint32_t friend_id;
    uint8_t op_type;
} as_msg_friend_op_req_t;

/**
 * @brief as向online超找好友的请求包,40114协议
 */
typedef struct
{
    char buffer[16];
} as_msg_find_friend_req_t;

/**
 * @brief as向online超找好友的返回包,40114协议
 */
typedef struct
{
    uint8_t current_count;
    uint8_t total_count;
    search_friend_item_t friend_info[0];
} as_msg_find_frined_rsp_t;

/**
 * @brief as向online添加好友的请求包,40115协议
 */
typedef struct
{
    uint32_t user_id;
    uint8_t remark_count;
    char remark[0];
} as_msg_add_friend_req_t;

/**
 * @brief as向online操作好友申请的请求包,40117协议
 */
typedef struct
{
    uint32_t user_id;
    uint8_t type;
} as_msg_friend_apply_op_req_t;

/**
 * @brief as向online拉取留言板信息的请求包,40118协议
 */
typedef struct
{
    uint32_t user_id;
    uint32_t page;
    uint8_t page_num;
} as_msg_pinboard_req_t;

typedef struct
{
    uint32_t id;
    uint32_t peer_id;
    uint8_t icon;
    uint8_t color;
    uint8_t status;
    uint32_t create_time;
    uint16_t message_count;
    char message[MAX_MESSAGE_BYTE];
} pinboard_message_t;




/**
 * @brief as向online插入一条新留言的请求包, 40119协议
 */
typedef struct
{
    uint32_t peer_id;
    uint8_t icon;
    uint8_t color;
    char message[0];
} as_msg_add_message_req_t;

/**
 * @brief as更改留言状态的请求包, 40120协议
 */
typedef struct
{
    uint32_t peer_id;
    uint32_t id;
    uint8_t type;
} as_msg_message_status_req_t;

/**
 * @brief as向online顶用户小屋的请求包,40121协议
 */
typedef struct
{
    uint32_t user_id;
} as_msg_thumb_user_req_t;

/**
 * @brief as向online种植物的请求包,40123协议
 */
typedef struct
{
    uint32_t plant_id;
    uint8_t hole_id;
} as_msg_grow_plant_req_t;

/**
 * @brief as向online铲除植物的请求包,40124协议
 */
typedef struct
{
    uint8_t hole_id;
    uint8_t maintain_type;
} as_msg_maintain_plant_req_t;

/**
 * @brief as向online操作吸引到的小怪兽的请求包,40125协议
 */
typedef struct
{
    uint8_t type;
    uint32_t pet_id;
} as_msg_attract_pet_req_t;

/**
 * @brief as向online提交小游戏答题结果的请求包,40127协议
 */
typedef struct
{
    uint32_t game_id;
    uint32_t level_id;//关卡id
    uint32_t coins;
    uint32_t score;
} as_msg_game_req_t;

/**
 * @brief as向online更新新手引导flag的请求包,40131协议
 */
typedef struct
{
    uint32_t flag1;
} as_msg_update_flag_req_t;

/**
 * @brief as向online兑换物品的请求包,40132协议
 */
typedef struct
{
    uint32_t stuff_id;
    uint8_t type;
} as_msg_trade_stuff_req_t;

/////////////////////////////////////////////////////////////////////////////
//online和db请求包和响应的包
/////////////////////////////////////////////////////////////////////////////
/**
 * @brief 向db新增一个用户的请求包，53408协议
 */
typedef struct
{
    char name[16];
    uint8_t gender;
    uint16_t country_id;
    uint32_t birthday;
    uint32_t register_time;
    uint32_t last_login_time;
    uint32_t coins;
    uint32_t monster_id;
    char monster_name[16];
    uint32_t monster_main_color;
    uint32_t monster_ex_color;
    uint32_t monster_eye_color;
	uint32_t invitor_id;
    uint32_t monster_exp;
    uint16_t monster_level;
    uint32_t monster_health;
    uint32_t monster_happy;
    uint32_t seed1;
    uint32_t seed2;
    uint32_t seed3;
    uint32_t default_friend;
    uint32_t wallpaper_id;
    uint32_t floor_id;
    uint32_t window_id;
    uint32_t door_id;
    uint32_t room_id;
    user_room_buf_t room_buf;
    uint8_t stuff_count;
    uint32_t stuff_id[0];
} db_msg_add_role_req_t;

/**
 * @brief 向db查询用户信息的返回包,53409协议
 */
typedef struct
{
    uint8_t is_register;
    as_msg_register_rsp_t register_rsp;
} db_msg_get_role_rsp_t;

/**
 * @brief db查询背包物品返回包,53410协议
 */
typedef struct
{
    uint16_t num;
    stuff_t stuff[0];
} db_msg_get_bag_rsp_t;

/**
 * @brief db背包中增加物品的请求包,53411协议
 */
typedef struct
{
    uint32_t coins;
    uint32_t happy;
    uint16_t count;
    uint16_t reward_exp; //奖励的经验值
    uint8_t level_up;//上升的等级数
    as_msg_buy_stuff_req_t stuff[0];
} db_msg_add_stuff_req_t;

typedef struct
{
    uint32_t coins;
    uint32_t happy;
    uint16_t count;//count取值永远为1
    uint16_t reward_exp; //奖励的经验值
    uint8_t level_up;//上升的等级数
    as_msg_buy_stuff_req_t stuff[1];
} db_msg_add_stuff_req2_t;
/**
 * @brief db获得房间id的返回包,53413协议
 */
typedef struct
{
    uint16_t num;
    uint32_t room_id[0];
} db_msg_room_id_rsp_t;



/**
 * @brief db拉取房间信息请求包,53414协议
 */
typedef struct
{
    uint32_t room_id;
} db_msg_get_room_req_t;

/**
 * @brief db增加新房间的请求包,53415协议
 */
typedef struct
{
    uint32_t wallpaper_id;
    uint32_t floor_id;
    uint32_t windows_id;
    uint32_t door_id;
    uint32_t room_id;
    char room_buf[0];
} db_msg_add_room_req_t;

typedef struct
{
    uint8_t type;
    uint32_t value;
} field_t;

/**
 * @brief db修改role信息的请求包,53416协议
 */
typedef struct
{
    uint8_t count;
    field_t field[0];
} db_msg_update_login_t;

/**
 * @brief db益智游戏信息的返回包,53418协议
 */
typedef struct
{
    uint16_t num;
    puzzle_t puzzle[MAX_PUZZLE_TYPE];
} db_msg_puzzle_rsp_t;

/**
 * @brief db提交益智游戏信息的请求包,53419协议
 */
typedef struct
{
    puzzle_t puzzle;
    puzzle_reward_t reward;
} db_msg_puzzle_req_t;

/**
 * @brief db修改role信息的请求包,53420协议
 */
typedef struct
{
    uint8_t type;
    uint8_t count;
    field_t field[0];
} db_msg_update_role_req_t;

/**
 * @brief db拉取好友id信息的返回包,53421协议
 */
typedef struct
{
    uint16_t begin;
} db_msg_friend_list_req_t;

/**
 * @brief db拉取好友id信息的返回包,53421协议
 */
typedef struct
{
    uint8_t is_end;
    uint16_t count;
    friend_info_t friend_info[0];
} db_msg_friend_list_rsp_t;

/**
 * @brief db增加一条好友记录时的请求包,53422协议
 */
typedef struct
{
    uint32_t peer_id;
    uint8_t type;
    uint8_t remark_count;
    char remark[0];
} db_msg_friend_apply_req_t;

/**
 * @brief db设置好友状态的请求包,53424协议
 */
typedef struct
{
    friend_info_t friend_info;
} db_msg_set_friend_status_req_t;

/**
 * @brief db拉取单个好友信息的返回包,53425协议
 */
typedef struct
{
    char name[16];
    uint8_t gender;
    uint32_t country_id;
    uint8_t user_type;
    uint32_t birthday;
    uint32_t last_login_time;
    uint32_t monster_id;
    uint32_t monster_main_color;
    uint32_t monster_ex_color;
    uint32_t monster_eye_color;
    uint16_t monster_level;
} db_msg_get_friend_rsp_t;

/**
 * @brief db拉取留言板信息的请求包,53426协议
 */
typedef struct
{
    uint32_t page;
    uint8_t page_num;
    uint8_t status;
    uint32_t peer_id;
    int real_num;//用户实际可以看到的留言数
} db_msg_pinboard_req_t;

/**
 * @brief db插入一条新留言的请求包, 53427协议
 */
typedef struct
{
    uint32_t peer_id;
    uint8_t icon;
    uint8_t color;
    uint8_t type;
    uint32_t create_time;
    uint16_t message_count;
    char message[0];
} db_msg_add_message_req_t;

/**
 * @brief db更改留言状态的请求包, 53428协议
 */
typedef struct
{
    uint32_t id;
    uint8_t type;
} db_msg_message_status_req_t;

/**
 * @brief db删除一个好友的请求包, 53429协议
 */
typedef struct
{
    uint32_t friend_id;
} db_msg_del_friend_req_t;

/**
 * @brief db小怪兽操作的请求包, 53430协议
 */
typedef struct
{
    uint8_t type;
    uint32_t id;
    uint8_t status;
} db_msg_pet_req_t;

/**
 * @brief db新增种植园植物时的请求包, 53431协议
 */
typedef struct
{
    uint32_t plant_id;
    uint8_t hole_id;
    uint16_t color;
    uint32_t last_extra_growth_time;
    uint32_t last_grown_time;
    uint8_t maintain;
    uint32_t maintain_time;
} db_msg_add_plant_req_t;

/**
 * @brief db删除种植园植物时的请求包, 53432协议
 */
typedef struct
{
    uint8_t hole_id;
} db_msg_del_plant_req_t;

/**
 * @brief db更新种植园植物成长值的请求包, 53433协议
 */
typedef struct
{
    uint8_t count;
    plant_growth_t plant_growth[0];
} db_msg_update_plant_req_t;

typedef struct
{
    uint32_t user_id;
    uint8_t is_vip;
} user_vip_t;

/**
 * @brief db拉取活跃用户的返回包, 53434协议
 */
typedef struct
{
    uint8_t count;
    user_vip_t user[0];
} db_msg_active_user_rsp_t;


/**
 * @brief switch返回的活跃用户
 */
typedef struct
{
    uint16_t count;
    user_vip_t user[0];
}switch_msg_active_user_t;

/**
 * @brief db获得天限制表值的请求包, 53435协议
 */
typedef struct
{
    uint32_t type;
    uint32_t time;
} db_msg_get_day_restrict_req_t;

typedef struct
{
    uint32_t value_id;
    uint16_t value;
} restrict_value_t;

/**
 * @brief db增加天限制表值的请求包, 53436协议
 */
typedef struct
{
    uint32_t type;
    uint32_t time;
    uint16_t value;
    uint8_t count;
    restrict_value_t restrict_stuff[0];
} db_msg_add_day_restrict_req_t;

typedef struct
{
    uint32_t time;
    uint32_t reward_coins;
    uint8_t type;
    uint8_t count;
    restrict_value_t restrict_stuff[0];
}db_msg_add_game_day_restrict_req_t;

/**
 * @brief db获得商业街用户信息的返回包, 53437协议
 */
typedef struct
{
    profile_t profile;
    uint32_t pet_id;
} db_msg_street_user_rsp_t;

/**
 * @brief db怪兽吃食物请求包, 53438协议
 */
typedef struct
{
    uint32_t item_id;
    uint16_t reward_happy;
    uint16_t reward_health;
    uint16_t reward_coins;
} db_msg_eat_food_req_t;

/**
 * @brief db修改用户登录时的信息, 53441协议
 */
typedef struct
{
    uint32_t login_time;
    uint32_t monster_happy;
    uint32_t offline_time;
    uint8_t limit_reward; //0:没有 1：封测大礼海报 2：封测大礼海报加雕像
    uint8_t plant_count;
    plant_growth_t plant_growth[0];
} db_msg_update_login_req_t;

typedef struct
{
    uint32_t item_id;
    uint16_t item_num;
} item_cost_t;

/**
 * @brief db合成物品的请求信息
 */
typedef struct
{
    uint32_t finish_time;
    uint32_t stuff_id;
    uint32_t map_id;
    uint16_t num;
    item_cost_t item_cost[0];
} db_msg_compose_stuff_req_t;

/**
 * @brief db插入用户名字和米米号的请求包, 20001协议
 */
typedef struct
{
    char name[16];
} db_msg_insert_name_req_t;

/**
 * @brief db通过用户名查询相应米米号的请求包, 20002协议
 */
typedef struct
{
    char name[16];
} db_msg_search_name_req_t;

/**
 * @brief db通过用户名查询相应米米号的应答包, 20002协议
 */
typedef struct
{
    uint8_t count;
    uint32_t user_id[0];
} db_msg_search_name_rsp_t;

/**
 * @brief db通过用户名查询相应米米号的应答包, 20002协议
 */
typedef struct
{
    uint16_t begin_id;
    uint16_t num;
} db_msg_friend_id_req_t;

/**
 * @brief 互动抽奖
 */
typedef struct
{
    uint32_t day_time;
    uint32_t interactive_id;
    uint16_t reward_coins;
}db_msg_interactive_req_t;

typedef struct
{
    uint8_t is_restrict;
    uint16_t reward_coins;
}db_msg_interactive_res_t;

/**
 * @brief 下面两个结构体用于终端查询switch内部变量值的结构
 **/
typedef struct
{
    uint8_t is_online;
    uint32_t login_time;
    uint32_t online_id;
    char online_ip[16];
    uint16_t online_port;
}switch_user_t;

typedef struct
{
    uint32_t online_id;
    uint32_t online_fd;
    uint32_t conn_num;
    uint16_t cur_index;
    uint16_t online_port;
}online_t;

typedef struct
{
    uint16_t online_num;
    online_t online[0];
}online_ip_t;


typedef struct
{
    char online_ip[16]; //online机器的ip
    uint32_t online_num;//本机上的人数
}ip_info_t;

typedef struct
{
    uint32_t cur_user_num;      //当前在线人数
    uint16_t  ip_num;            //online机器的数量(物理机)
    ip_info_t ip_info[0];
}switch_online_info_t;


/**
 * @brief 二人益智游戏 online发给multi-server的用户信息
 */
typedef struct
{
    char uname[16];
    uint8_t is_vip;
    uint32_t monster_id;
    uint32_t monster_main_color;
    uint32_t monster_exp_color;
    uint32_t monster_eye_color;
}two_gamer_info_t;


typedef struct
{
    uint8_t quest_type;
    uint32_t quest_id;
}quest_type_t;


/**
 * @brief 二人益智游戏比赛题目
 */
typedef struct
{
    uint16_t compete_init_time; //比赛初始总时间
    uint16_t question_num;
    quest_type_t quests[0];
}two_puzzle_compete_t;

typedef struct
{
    uint32_t uid;
    char uname[16];
    uint8_t is_vip;
    uint32_t mon_id;
    uint32_t mon_main_color;
    uint32_t mon_exp_color;
    uint32_t mon_eye_color;
}two_puzzle_opponent_info_t;

/**
 * @brief 匹配对手信息(40134协议的返回包)
 */
typedef struct
{
    two_puzzle_opponent_info_t opp_info;
    two_puzzle_compete_t puz_com;       /**<@brief 比赛题目 */
}matching_opponent_t;


/**
 * @brief 40136协议的请求包(用户答题)
 */
typedef struct
{
    uint8_t is_corrent;             /**<@brief 答题是否正确(0:答题正确 1：答题错误) */
    uint8_t is_end;                 /**<@brief 答题是否结束(0:未结束 1：结束) */
    uint16_t remain_time;           /**<@brief 比赛剩余时间 */
}answer_question_request_t;


/**
 * @brief 对手答题信息
 */
typedef struct
{
    uint16_t opp_cur_score;         /**<@brief 对手当前得分 */
    uint16_t opp_remain_time;       /**<@brief 对手剩余时间 */
    uint8_t opp_is_end;             /**<@brief 对手是否已答题结束(0:未结束 1：已结束) */
}opp_answer_info_t;

/**
 * @brief 二人益智游戏比赛结果信息
 */
typedef struct
{
    uint16_t self_score;        /**<@brief 用户自己得分 */
    uint16_t opp_score;         /**<@brief 对手得分 */
    uint8_t result;             /**<@brief 比赛结果 0:平局 1：胜利 2：失败 */
    uint16_t rewards_coins;     /**<@brief 奖励的咕咚果 */
    uint8_t is_restrict;        /**<@brief 是否达到每日上限(0:没有 1：有) */
}two_puzzle_result_t;


/**
 * @brief 成就项信息
 */
typedef struct
{
    uint32_t badge_id;          /**<@brief 成就项id */
    uint8_t badge_status;       /**<@brief 成就项状态(0:进行中 1：已获得未领奖杯 2:已获得已领奖) */
    uint32_t badge_progress;     /**<@brief 成就项进度 */
}badge_info_t;

typedef struct
{
    uint16_t badge_num;
    badge_info_t badge[0];
}all_badge_info_t;

typedef struct
{
    uint32_t game_id;
    uint32_t level_id;
    uint32_t max_score;
    uint8_t star_num;
    uint8_t is_passed;//是否过关
}level_info_t;

typedef struct
{
    uint8_t unlock_next_level;
    uint16_t level_num;
    level_info_t level[0];
}game_level_info_t;

typedef struct
{
    uint32_t game_id;
    uint32_t level_id;
    uint32_t score;
    uint8_t star_num;
    uint8_t is_passed;
}db_msg_update_game_level_req_t;


typedef struct
{
    uint8_t hole_id;
    uint32_t reward_id;
}as_msg_get_sun_reward_t;

typedef struct
{
    as_msg_get_sun_reward_t sun_reward;
    uint16_t reward_happy;
}db_msg_sun_reward_happy_t;

typedef struct
{
    db_msg_sun_reward_happy_t sun_reward_happy;
    uint32_t old_happy;
    db_msg_update_plant_req_t plant_change;
}sun_reward_happy_cache_t;

typedef struct
{
    as_msg_get_sun_reward_t sun_reward;
    uint16_t reward_exp;
    uint16_t level_up; //可以升的级数
}sun_reward_exp_cache_t;

typedef struct
{
    as_msg_get_sun_reward_t sun_reward;
    uint16_t reward_coins;
}sun_reward_coins_cache_t;

typedef struct
{
    uint32_t npc_id;
    uint32_t npc_score;
}npc_score_t;

typedef struct
{
    uint8_t npc_num;
    npc_score_t npc[NPC_SCORE_NUM];
}db_return_npc_score_t;

typedef struct
{
    uint8_t status;//0:初始状态（服务端使用） 1:今天未点评过 2：今天点评过
    uint32_t total_score;
    uint8_t npc_num; //目前未3个
    npc_score_t npc[NPC_SCORE_NUM];
}npc_score_info_t;//小屋评分


typedef struct
{
    uint32_t day_time;
    uint32_t total_score;
    uint8_t npc_num;
    npc_score_t npc[NPC_SCORE_NUM];
}db_msg_set_npc_score_t;

typedef struct
{
  uint32_t game_id;
  uint16_t changed_stuff_num;
  uint32_t changed_stuff[0];
}db_msg_changed_stuff_t;

typedef struct
{
    uint32_t game_id;
    uint32_t period_id;
    uint32_t stuff_id;
    uint8_t repeat;
}db_msg_get_changed_stuff_t;

typedef struct
{
    uint32_t reward_id;
    uint16_t reward_num;
}task_reward_t;

typedef struct
{
    uint32_t task_id;           //任务id
    uint16_t lvl;               //等级要求
    uint32_t pre_mission;       //前置任务
    uint16_t reward_cycle;      //奖励周期(天为单位)
    std::vector<task_reward_t> task_reward_vec; //任务奖励
}task_xml_t;

typedef struct
{
    uint32_t task_id;
    uint8_t level_up; //上升的等级数
    uint32_t reward_time;
    uint8_t reward_num;
    task_reward_t reward[0];
}db_msg_task_reward_t;


typedef struct
{
    uint32_t task_id;
    uint32_t last_reward_time;
}finished_task_t;

typedef struct
{
    uint16_t finished_task_num;
    uint32_t task_id[0];
}as_msg_finished_task_rsp_t;//返回给as的已完成任务列表

typedef struct
{
    uint16_t finished_task_num;
    finished_task_t finished_task[0];
}db_msg_finished_task_rsp_t;//从db返回的已完成任务列表

typedef struct
{
    uint32_t donate_id;
    uint32_t donate_total;
    uint32_t helped_pet;
    uint32_t donate_limit;
    uint32_t reward_id;
}donate_info_t;

typedef struct
{
    uint32_t req_donate_id;
    uint32_t req_donate_coins;
    uint32_t req_donate_timestamp;
    donate_info_t donate_info;
}db_request_donate_coins_t;

typedef struct
{
    uint32_t donate_id;
    uint8_t flag;
    uint32_t reward_donate_id;
    uint32_t next_donate_id;
    uint32_t money;
    uint32_t reward_id;
}db_return_donate_coins_t;


typedef struct
{
    uint32_t reward_donate_id; //未领奖的捐助id
    uint32_t donate_id;
    uint32_t donate_num;
    uint32_t donor_count;
    uint32_t money;
    uint32_t reward_id;
}db_return_cur_donate_t;

typedef struct
{
    uint32_t money;
    uint32_t stuff_id;
    uint16_t stuff_num;
    char buf[256];
}db_request_update_wealth_t;


typedef struct
{
    uint32_t last_unreward_donate;   //未领取奖励的援助计划id（0：表示没有）
    uint32_t last_reward_id;        //上次未领取的奖励id
    uint32_t donate_id;             //当前期的援助计划id（0：表示当前暂无援助计划）
    uint32_t cur_donate_num;        //当前期已经募捐的数额
    uint32_t donater_num;           //当前期的募捐参与者人数
}msg_db_select_donate_t;

typedef struct
{
    uint32_t donate_id;
    uint32_t end_time;//达成时间
}history_donate_t;

typedef struct
{
    uint32_t donate_total;//总共募捐到的数额
    uint16_t helped_pet; //总共帮助过的精灵数量
    uint32_t total_participater;//总共参与的人数
    uint16_t history_num;//举办过的届数
    history_donate_t donate[0];
}msg_history_donate_t;

typedef struct
{
    uint32_t donate_id;
    uint32_t donate_num;
    uint32_t donate_total;
    uint32_t helped_pet;
    uint32_t donate_limit;
    uint32_t reward_id;
}msg_db_donate_request_t;

typedef struct
{
    uint32_t donate_id;
    uint8_t flag;
    uint32_t donate_num;
    uint32_t reward_id;
    uint32_t new_donate_id;
}db_msg_donate_response_t;

typedef struct
{
    uint16_t count;
    as_msg_buy_stuff_req_t stuff[0];
}factory_op_get_stuff_t;

typedef struct
{
	uint32_t museum_id;
	uint8_t level_id;
	uint8_t reward_flag;
	uint8_t next_level_id;
	uint32_t timestamp;
}museum_info_t;

typedef struct
{
    uint32_t museum_id;
    uint8_t level_id;
}as_msg_get_museum_req_t;

typedef struct
{
    uint32_t museum_id;
    uint8_t level_id;
    uint32_t reward_id;
    uint16_t reward_num;
	uint32_t timestamp;
}db_msg_add_museum_reward_t;

typedef struct
{
    uint32_t museum_id;
    uint8_t level_id;
    uint32_t timestamp;
}db_request_commit_museum_t;

typedef struct
{
	uint8_t type;
	uint32_t score;
	uint32_t timestamp;
}db_request_encourage_t;

typedef struct
{
	uint8_t type;
	uint32_t score;
	uint32_t win_num;
	uint32_t total_num;
	uint32_t user_id;
}db_return_encourage_t;

typedef struct
{
    uint32_t activity_id;
    uint8_t activity_status;//取值  ACTIVITY_STATUS_RUNNING,ACTIVITY_STATUS_NEW,ACTIVITY_STATUS_FINISHED
}a_activity_t;

typedef struct
{
    uint16_t activity_num;
    a_activity_t activity[0];
}activity_list_t;

typedef struct
{
    uint32_t activity_id;
    uint32_t reward_id;
}as_request_activity_reward_t;

typedef struct
{
	uint32_t game_id;
	uint32_t level_id;
	uint32_t score;
}as_msg_cake_game_t;

typedef struct
{
    uint16_t num;
    uint32_t value[0];
}num_value_t;

typedef struct
{
    uint32_t activity_id;
    uint16_t reward_num; //已经领过的奖励个数
    uint32_t reward_id[0];
}activity_rewarded_t;

typedef struct
{
    uint32_t reward_id;
    uint16_t reward_num;
    uint8_t reward_status;//取值CANNOT_GET HAVE_GET CAN_GET
}single_reward_t;

typedef struct
{
    uint32_t activity_id;
    uint16_t kind_num;
    single_reward_t reward[0];
}single_activity_t;

typedef struct
{
    uint32_t act_id;
    uint32_t reward_id;
    uint16_t reward_num;
}db_msg_get_open_reward_t;

typedef struct
{
	uint16_t invite_num;
	uint16_t qualified_num;
	uint8_t level_status[3];
}db_return_get_invite_info_t;

typedef struct
{
	uint32_t activity_id;
	uint32_t level_id;
	uint32_t reward_id;
	uint16_t reward_num;
}db_request_get_activity_reward_ex_t;

typedef struct
{
	uint32_t reward_id;
	uint16_t reward_num;
}db_return_get_activity_reward_ex_t;

typedef struct
{
	uint32_t stuff_a_id;
	uint16_t cost;
	uint32_t stuff_b_id;
	uint8_t category;
}db_request_prize_lottery_t;


typedef struct
{
    uint32_t show_id;
    uint32_t peer_id;
}msg_vote_show_t;//大众点评投票

typedef struct
{
    uint32_t total_num;//总共报名的人数
    uint8_t user_num;  //进入候选的人数
}show_user_num_t;//大众点评显示的用户数

typedef struct
{
    uint32_t user_id;
    char user_name[16];
    uint32_t mon_id;
    uint32_t mon_main_color;
    uint32_t mon_exp_color;
    uint32_t mon_eye_color;
    uint32_t npc_score;
    uint32_t npc_timestamp;
    uint32_t votes;
    uint8_t is_top;
    uint8_t is_notice;
}show_user_info_t;//大众点评的用户个人信息

typedef struct
{
    uint32_t show_id;
    show_user_info_t user_info;
}online_msg_join_show_t;//online同db-cache-server的通信 用户参与大众点评的协议

typedef struct
{
	uint8_t is_reward;
	uint8_t status[3];
}db_return_get_dragon_boat_t;

typedef struct
{
    uint32_t show_id;
    show_user_num_t user_num;
    show_user_info_t user_info[0];
}msg_history_show_t;

typedef struct
{
    uint32_t show_id;
    uint8_t show_status;
    show_user_num_t user_num;
    show_user_info_t user_info[0];
}msg_enter_show_return_t;

typedef struct
{
    uint8_t is_join;//是否已参加或已投票(0:否1: 是)
    uint8_t is_notice;//是否已通知(0:未 1：已);
}msg_next_of_enter_t;

typedef struct
{
    uint32_t unreward_show_id;//未奖励的大众点评届数
    uint8_t unreward_type;//奖励的类型(0:冠军奖励 1：猜中冠军奖励)
    uint32_t reward_coins;  //奖励的金币数
}show_reward_t;//大众点评的奖励

typedef struct
{
	uint8_t level_id;
	uint8_t status;
}db_return_update_dragon_boat_t;

#pragma pack(pop)

#endif //H_DATA_STRUCTURE_H_20110711
