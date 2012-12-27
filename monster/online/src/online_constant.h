/**
 * =====================================================================================
 *       @file  online_constant.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/20/2011 08:02:09 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * ===================================================================================== */

#ifndef H_ONLINE_CONSTANT_H_20110720
#define H_ONLINE_CONSTANT_H_20110720


//广播包发送时间
#define BROADCAST_TIME 60

//用户初始的金币值
#define INIT_COINS 200

//大怪兽初始的经验值
#define INIT_EXP 0

//大怪兽初始的健康值
#define INIT_HEALTH 700

//大怪兽初始的高兴值
#define INIT_HAPPY 700

//默认的小屋id
#define ROOM_DEFAULT_ID  1
//用户最大的房间数量
#define MAX_ROOM_NUM  10

//最多缓存留言板的页数
#define PINBOARD_CACHE_NUM 10

#define MAX_PET_NUM  10000

//益智游戏最大奖励金币
#define MAX_PUZZLE_COIN 99

//每种物品拥有的最大数量
#define MAX_STUFF_NUM 99


//新手注册时送的
#define DOOR_DEFAULT_ID	11034 //破坏门
#define WALLPAPER_DEFAULT_ID 10044//破坏墙纸
#define WINDOWS_DEFAULT_ID	13032 //破坏窗
#define FLOOR_DEFAULT_ID	12034 //破坏地板

///10001 --- 20000 为固定的家具装饰品id (门 窗户 墙纸 地板)
//门
#define MALE_DOOR_DEFAULT_ID 11001
#define FEMALE_DOOR_DEFAULT_ID 11001

//默认窗户
#define MALE_WINDOWS_DEFAULT_ID 13001
#define FEMALE_WINDOWS_DEFAULT_ID 13001

//默认墙纸
#define MALE_WALLPAPER_DEFAULT_ID 10001
#define FEMALE_WALLPAPER_DEFAULT_ID 10002

//默认地板
#define MALE_FLOOR_DEFAULT_ID 12001
#define FEMALE_FLOOR_DEFAULT_ID 12002


//每日挑战的类型
#define DAILY_PUZZLE 0

//最大的等级
#define MAX_LEVEL 100

//种植园植物颜色的数量
#define PLANT_COLOR_NUM  6

//怪兽高兴时植物成长1点需要的时间
#define HAPPY_GROWTH_TIME (15)

//怪兽不高兴时植物成长1点需要的时间
#define UNHAPPY_GROWTH_TIME (30)

//最大的健康值
#define MAX_HEALTH_VALUE 999
//最大的愉悦值
#define MAX_HAPPY_VALUE 999


//怪兽高兴所需的愉悦度
#define  HAPPY_VALUE 500

//怪兽升级所需的健康值
#define  LEVEL_HEALTH_VALUE 400
//怪兽升级所需的愉悦值
#define  LEVEL_HAPPY_VALUE 300

//植物成熟需要的成长值
#define  MATURITY_GROWTH_VALUE 2880

//益智游戏最大类别
#define MAX_PUZZLE_TYPE_NUM 15

//益智游戏额外金币奖励
#define EXTRA_COIN_REWARD 10
//每日游戏金币奖励
#define DAILY_COIN_REWARD 2
//单个游戏金币奖励
//#define SINGLE_COIN_REWARD 5
//益智游戏经验奖励
#define DAILY_EXP_REWARD 2
//益智游戏愉悦度奖励
#define PUZZLE_HAPPY_REWARD 5

//益智游戏愉悦度奖励
#define PUZZLE_DAILY_HAPPY_REWARD 5
#define PUZZLE_DAILY_HAPPY_ASC 2
#define PUZZLE_DAILY_HAPPY_DESC 3
#define PUZZLE_SINGLE_HAPPY_REWARD 2
#define PUZZLE_SINGLE_HAPPY_ASC 1
#define PUZZLE_SINGLE_HAPPY_DESC 2

#define DRAGON_BOAT_INTERACTIVE_ID 2001

//记录包处理时间的文件的前缀名
#define PKG_FILE "process_time"


//商店物品的稀有度数量
#define SHOP_RARITY_NUM 4

enum
{
    FEMALE = 0,
    MALE = 1,
};

/*
 * @brief 对好友的操作类型
 */
enum
{
    BEST_FRIEND = 0,
    CANCEL_BEST_FRIEND = 1,
    DEL_FRIEND = 2,
    BLOCK_FRIEND = 3,
};

/*
 * @brief 对好友申请的操作
 */
enum
{
    FRIEND_REQ_AGREE = 0,
    FRIEND_REQ_REFUSE = 1,
    FRIEND_REQ_BLOCK = 2,
};

/*
 * @brief 种植园状态
 */
enum
{
    PLANTATION_ATTRACT_PET = 1,
    PLANTATION_UNATTRACT_PET = 2,
    PLANTATION_NOT_MATCH = 3,
};

/*
 * @brief 小怪兽的类型
 */
enum
{
    COMMON_PET = 1,
    GOOD_PET = 2,
    RARE_PET = 3,
    VIP_PET = 4,
};

/*
 * @brief 查找好友的方式
 */
enum
{
    FIND_BY_USER_ID = 1,
    FIND_BY_NAME = 2,
};

/*
 * @brief 物品的类型
 */
enum
{
    ITEM_FRUNITURE = 1, //家具
    ITEM_SEED = 2,      //种子
    ITEM_FOOD = 3,      //食物
};

enum
{
    THUMB_ROOM_BEFORE = 0,
    THUMB_ROOM_SUCCESS = 1,
};

enum
{
    PERCENT_FORTY = 1,
    PERCENT_RANDOM = 2,
};


enum
{
    GAME_GET_STUFF = 0,     //获得奖励的道具
    GAME_NO_STUFF = 1,      //没有达到奖励道具的分数
    GAME_STUFF_FULL = 2,    //当天奖励的道具已经满了
};



//更新用户profile信息的字段类型
enum
{
    PROFILE_SIGN = 1,
    PROFILE_MOOD = 2,
    PROFILE_COLOR = 3,
    PROFILE_PET = 4,
    PROFILE_FRUIT = 5,
};

//as统计消息的字段类型
enum
{
    AS_STAT_LEAVE_SHOP = 1,
    AS_STAT_START_GAME = 2,
    AS_MAGGIC_VISIT = 3,
    AS_START_TASK = 4,
};

enum
{
    BADGE_TYPE_SET  = 0, //直接"设置"型成就
    BADGE_TYPE_SUM  = 1,//"累加"型成就项
};

enum
{
    BADGE_PROGRESS_TOP = 0,//成就项记录最高分
    BADGE_PROGRESS_LATEST = 1,//记录最新成绩
};

enum
{
    FACTORY_IDLE = 0,       //合成工厂空闲
    FACTORY_CREATE = 1,     //合成工厂正在合成中
    FACTORY_FINISH = 2,     //合成完成
};

enum
{
    GET_COMPOSE = 1,        //领取合成物品
    CANCEL_COMPOSE = 2,     //取消合成物品
};

enum
{
    CONDITION_LEVEL = 0,        //根据等级解锁
};

enum
{
    DIRTY_NO = 0,         //不思脏词
    DIRTY_YES = 1,        //是脏词
};

enum
{
    JUQING_OPEN_BOX = 2,    //打开储物箱
    JUQING_ENTER_STREET = 64,    //触发街道
    JUQING_ENTER_STORE = 128,    //触发商店
    JUQING_ENTER_PLANTATION = 32,    //触发种植园
};

enum
{
    CHENGMI_HALF = 3 * 60 * 60,    //放沉迷收益减半的时间
    CHENGMI_NO = 5 * 60 * 60,      //放沉迷没有收益时间
    CHENGMI_OFFLINE = 5 * 60 * 60, //放沉迷下线到达这个时间，时间都清零
};

enum
{
    BOBO_UNREAD = 0,
    BOBO_UNREWARD = 1,
    BOBO_REWARDED = 2,
};






#endif //H_ONLINE_CONSTANT_H_20110720

