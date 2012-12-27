/*
 * =====================================================================================
 *
 *       Filename:  stat.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月13日 15时28分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_STAT_H_20110713
#define H_STAT_H_20110713

extern "C"
{
#include <libtaomee/project/stat_agent/msglog.h>
}

enum
{
    //基础统计项(0x13000001-0x1300000f)

    stat_online_usernum     = 0x13000001,       //统计在线人数(电信)
    stat_login_logout       = 0x13000002,       //统计登录登出
    stat_register_num       = 0x13000003,       //统计注册人数
    stat_gender_distr       = 0x13000004,       //统计性别分布
    stat_age_distr          = 0x13000005,       //统计年龄分布
    stat_monster_num        = 0x13000006,       //各个怪兽选择的个数

    stat_register_throw_link = 0x1300000f, //听哦难过邀请注册过来的人数

    //留言(0x13000010-0x1300002F)
    stat_msg_num            = 0x13000010,       //留言总数/当天留言数/当天发送留言人数
    stat_msg_del            = 0x13000011,       //当天删除留言数
	stat_add_exp_buy_stuff =     0x13000020,       //通过购买物品获得经验值增加
    stat_online_usernum_netcom = 0x13000021, //网通在线人数
    stat_npc_score       =       0x13000022, //评一评按钮的点击次数
    stat_kick_bobo       =       0x13000023, //点击卜卜报人次人数
    stat_kick_bobo_reward =      0x13000024, //点击卜卜报里面咕咚果的人数
    stat_enter_petall     =      0x13000025, //打开精灵园人数人次
    stat_online_total       =    0x13000026,     //总在线人数
    stat_plantation_visit   =    0x13000027,   //种植园访问人数人次
    stat_thumb_count        =    0x13000028, //赞的人数和人次
    stat_open_door          =    0x13000029,//公测领取门的人数
    stat_open_window          =    0x1300002a,//公测领取窗的人数
    stat_open_floor          =    0x1300002b,//公测领取地板的人数
    stat_open_wallpaper          =    0x1300002c,//公测领取壁纸的人数
    stat_invitor_first          =    0x1300002d,//领取邀请好友第一关的人数
    stat_invitor_second          =    0x1300002e,//第二关
    stat_invitor_third          =    0x1300002f,//第三关



    //种植园(0x13000030-0x1300004F)
    stat_seeds_down         = 0x13000030,       //播种人数/播种子数量
    stat_seeds_out          = 0x13000031,       //铲除种子数量
    stat_attract_pet        = 0x13000032,       //种子成熟吸引到精灵
    stat_not_attract_pet    = 0x13000033,       //种子成熟没吸引到精灵

    //好友(0x13000100-0x130001ff)
    stat_friends_req        = 0x13000100,       //发送好友请求数
    stat_friends_agree      = 0x13000101,       //同意请求数

    stat_trade_pet          = 0x13000102,      //兑换精灵人数
    stat_get_leaf_a         = 0x13000103,       //获得A叶子人数
    stat_get_leaf_b         = 0x13000104,   //获得b叶子人数
    stat_get_leaf_c         = 0x13000105, //获得C叶子人数
    stat_vote_num           = 0x13000106, //点击竞猜人数
    stat_join_num           = 0x13000107, //点击报名人数

    //益智游戏(0x13000200-0x130002ff)
    stat_exercise_base      = 0x13000200,        //每日挑战

    stat_puzzle_num      = 0x130002ef,        //益智游戏答题正确率
    //题库0x13000201-0x1300020d  stat_exercise_base + 题库编号(1-13)
    multi_online_particate      = 0x130002f0,        //参与益智游戏联机对战
    multi_online_start      = 0x130002f1,            //益智游戏联机对战匹配成功

    //经济(0x13000300-0x130004ff)
    stat_gudong_buy_stuff   = 0x13000300,    //咕咚果消耗
    stat_puzzle_gudong      = 0x13000301,       //咕咚果益智游戏产出/人数
    stat_game_gudong        = 0x13000302,       //咕咚果小游戏产出/人数
    stat_game_huishou        = 0x13000303,       //咕咚果回收店产出/人数

    //心情(0x13000500-0x130005ff)
    stat_health_happy       = 0x13000500,       //健康值 愉悦值

    //物品商店相关(0x13000600-0x130008ff)
    stat_store_base         = 0x13000600,
    //商店访问数 stat_store_base + 商店编号(从1开始)

    stat_leave_store_base         = 0x13000700,

    //小游戏(0x13000900-0x13000aff)
    stat_picture_matching   = 0x13000900,   //怪卡连连看人数人次
    stat_leave_picture_matching   = 0x13000e00,   //离开怪卡连连看人数人次
    stat_daoju   = 0x13000d00,              //小游戏道具奖励
    stat_friend_num   = 0x13001000,              //好友数量分布
    stat_visit_room   = 0x13001001,              //小屋访问次数
    stat_badge_num  = 0x13002000, //成就系统的分布
    //0x13002001---0x13002014为各个成就项的统计

    stat_maggic_visit = 0x13000c00,     //麦咭空间访问

    stat_juqing_chuwuhe = 0x13000c02,   //储物盒
    stat_juqing_xunzhang = 0x13000c03,   //勋章
    stat_juqing_jiedao = 0x13000c04,   //街道
    stat_juqing_shangdian = 0x13000c05,   //商店
    stat_juqing_zhongzhiyuan = 0x13000c06,   //种植园

    stat_task_base = 0x13003000,    //任务统计 完成任务

    stat_task_start_base = 0x13000f00, //开始任务统计 + 任务id - 100000
    stat_denote_base = 0x13004000,  //捐助计划统计

    stat_museum_base = 0x13004100, //博物馆的统计 + 博物馆id
    stat_museum_reward_base = 0x13004200, //博物馆答题获得奖励统计 + 博物馆id
};

void init_log_file(const char * stat_path);
void msg_log(uint32_t type, const void *stat_data, int len);

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    uint32_t value;
}stat_one_param_t;

typedef struct
{
    uint32_t value_1;
    uint32_t value_2;
}stat_two_param_t;

typedef struct
{
    uint32_t value_1;
    uint32_t value_2;
    uint32_t value_3;
}stat_three_param_t;

typedef struct
{
    uint32_t value_1;
    uint32_t value_2;
    uint32_t value_3;
    uint32_t value_4;
}stat_four_param_t;

typedef struct
{
    uint32_t value_1;
    uint32_t value_2;
    uint32_t value_3;
    uint32_t value_4;
    uint32_t value_5;
    uint32_t value_6;
    uint32_t value_7;
    uint32_t value_8;
}stat_eight_param_t;

#pragma pack(pop)

#endif // H_STAT_H_20110713
