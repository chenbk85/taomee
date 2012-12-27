/*
 * =====================================================================================
 *
 *       Filename:  constant.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月12日 13时51分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_CONSTANT_H_20110712
#define H_CONSTANT_H_20110712

#ifndef MAX_RECV_PKG_SIZE
#define MAX_RECV_PKG_SIZE    8192   //最大可接受包的长度
#endif

#ifndef MAX_SEND_PKG_SIZE
#define MAX_SEND_PKG_SIZE   8192    //一次最大发送包的长度
#endif

//系统错误返回码
#define SYSTEM_ERR_NUM 100000

#define MAX_ONLINE_NUM     2000     //总的online进程的数量
#define MAX_NUM_PER_ONLINE 500      //每台online最多允许的人数
#define MAX_REQ_PER_USER  10        //每个用户最多同时请求的数量

#define ACCOUNT_GAME_ID 13          //game id

//#define ACCOUNT_CHANNEL_ID 78       //渠道号
//#define SECURITY_CODE     12345678  //安全码
//#define SECURITY_CODE   45873822  //安全码

#define GAME_REGION     0           //渠道号,电信

#define REQUEST_TIMEOUT 10         //异步请求响应超时时间

#define MAX_ROOM_STUFF  101          //房间里面最多可以摆放的物品

#define MAX_USER_STUFF  200         //用户最多可以获得的物品

#define MAX_PUZZLE_TYPE  50         //最多的益智游戏的种类

#define MAX_FRIEND_NUM  10000       //做多的好友数量
#define MAX_FRIEND_REQ  99          //每次最多拉取的好友申请数量

#define MAX_REMARK_COUNT 256        //申请好友的留言的最大字节数

#define MAX_MESSAE_PER_PAGE 8       //每页留言的最大数量
#define MAX_MESSAGE_BYTE  132       //每条留言的最大字节

#define MAX_SEARCH_FRIEND_NUM 1     //通过名字查找返回的好友的最大数量

#define MAX_ACTIVE_USER_NUM 64      //memcache保存的最大的活跃用户id的数量

#define MAX_SIGN_BYTE  64          //最大的个性签名字节数

#define MAX_SEARCH_NAME_NUM 1       //通过名字查找最多获得的用户数量

#define STREET_USER_NUM   6 	    //街道最少保存的用户数量

#define MAX_REWARD_ID 536870911  //29个1 最大的奖励id

#define MAX_MAINTAIN_COUNT_PER_QUARTER 5 //15分钟内做多的维护次数


//种植物坑的数量
#define PLANTATION_NUM 3

//用户可以拥有的精灵最大数量
#define OWNED_PET_NUM   65535

//参与小屋评分的评委个数
#define NPC_SCORE_NUM   3


#define INVITE_FRIEND_REMARK "invite friend"

//小屋最高得分
#define MAX_NPC_SCORE 90

//益智游戏最高得分
#define PUZZLE_MAX_RIGHT_NUM 50

/*
 * @健康值的等级
 */
enum
{
    HEALTH_MIN = 100,   //健康值不能低于这个值
    HEALTH_LOW = 300,
    HEALTH_MIDDLE = 600,
};

enum
{
    flow_bag_to_room = 0,       //物品从背包到房间
    flow_room_to_bag = 1,       //物品从房间到背包
};

enum
{
    DB_ADD_ROLE = 0,            //增加role表的值
    DB_DESC_ROLE = 1,           //减少role表的值
};

enum
{
    FRIEND_PENDING = 0,         //等待处理的好友申请
    FRIEND_DEFAULT = 1,         //未分组的好友
    FRIEND_BLOCK = 2,           //被屏蔽的用户
};

enum
{
    MESSAGE_DELETE = 0,      	 //删除留言
    MESSAGE_UNAPPROVED = 1,      //未审核的留言
    MESSAGE_APPROVED = 2,        //审核通过的留言
    MESSAGE_REPORT = 4,          //被举报的留言
};

enum
{
    PET_ADD = 1,                 //增加一个小怪兽
    PET_DEL = 2,      	         //放弃一个小怪兽
    PET_DROP = 3,                //删除一个小怪兽
};

enum
{
    PET_ZOOM = 0,                 //小怪兽在动物园
    PET_FOLLOWING = 1,      	  //跟随的小怪兽
};

enum
{
    FRIEND_NOTIFY = 1,            //好友申请的通知
    MESSAGE_NOTIFY = 2,      	  //新留言的通知
    MESSAGE_APPROVED_FRIEND = 3,  //通过好友请求
    MESSAGE_DEL_FRIEND = 4,       //对方删除好友
    NEW_VISIT= 5,                 //新的访客信息
};

enum
{
    COIN_ID = 1001,               //金币的物品id
};

/**
 * @brief db修改role表的字段编号
 */
enum
{
    FIELD_LOGIN_TIME = 1,
    FIELD_HAPPY = 2,
    FIELD_HEALTH = 3,
    FIELD_LAST_VISIT_PLANTATION = 4,
    FIELD_PUZZLE_MAX_SCORE = 5,
    FIELD_LOGOUT_TIME = 6,
    FIELD_COIN = 7,
    FIELD_EXP = 8,
    FIELD_LEVEL = 9,
    FIELD_THUMB = 10,
    FIELD_VISITS = 11,
    FIELD_FAV_COLOR = 12,
    FIELD_FAV_FRUIT = 13,
    FIELD_FAV_PET = 14,
    FIELD_MOOD = 15,
    FIELD_FLAG1 = 16,
    FIELD_MAX_PUZZLE_SCORE = 17,
    FIELD_COMPOSE_TIME = 18,
    FIELD_COMPOSE_ID = 19,
    FIELD_ONLINE_TIME = 20,
    FIELD_OFFLINE_TIME = 21,
    FIELD_BOBO_REWARD = 22,
    FIELD_BOBO_READ = 23,
    FIELD_NPC_SCORE = 24,
    FIELD_NPC_SCORE_TIME = 25,
    FIELD_GUIDE_FLAG    = 26,
    FIELD_LAST_SHOW_ID    = 27,//上次参与的大众点评的届数
};

//用户的类型
enum
{
    USER_TYPE_NORMAL = 0,
    USER_TYPE_VIP = 1,
    USER_TYPE_SPECIAL = 2,
};

enum
{
    PET_HAS_NEXT = 0,
    PET_IS_END = 1,
};


/**
 * @brief 对应40134协议返回包的几种状态值
 */
enum
{
    opp_leaved  = 0,        //对手已离开
    opp_not_start = 1,      //对手尚未点击start按钮
    opp_started = 2,        //对手已开始
};


/**
 * @brief 对应42004协议的几种状态
 */
enum
{
    self_unstarted_timeout = 0, //用户自己长时间未点击start按钮超时
    opp_unstarted_timeout = 1,  //对手长时间未点击start按钮超时
    enter_game            = 2,  //进入游戏
};



/**
 * @brief 对应40135答题协议的返回包的几种状态值
 */
enum
{
    self_not_finished = 1,      //用户自己未结束
    opp_not_finished = 2,       //用户自己答题结束，但对手未结束
    both_finished   = 3,        //双方答题均已结束
};


/**
 * @brief 二人益智游戏比赛结果
 */
enum
{
    two_puzzle_tie =0,    //平局
    two_puzzle_win = 1,    //胜利
   two_puzzle_lose = 2,   //失败
};


enum
{
    badge_running = 0,             //成就项进行中
    badge_acquired_no_reward = 1,   //成就项已获得但未领奖杯
    badge_acquired_no_screen = 2,   //成就项已获得未查看
    badge_acquired_and_reward = 3,  //成就项已获得并已领取奖杯
};

enum
{
    NOT_UNLOCK = 0, //未解锁
    UNLOCK = 1,//解锁
    NO_NEXT = 2,//没有下一关了
};

enum
{
    STRICT_COIN = 1,    //小游戏金币的限制(所有小游戏一个总的限制值)
    STRICT_ITEM = 2,    //小游戏道具的限制(实际的限制ID为2 + 小游戏的编号, 编号从1开始)
    //1001-....未街道上互动抽奖元素每日获得的金币限制
    STRICT_STUFF_EXP  = 1000001,//购买商品每日的经验值限制
    STRICT_PUZZLE_EXP  = 1000002,//益智游戏每日的经验值限制
    STRICT_SUN_EXP     = 1000003,//阳关奖励包中每日的经验值限制
};

enum
{
    NOT_MAINTAIN = 0, //不需要维护
    MAINTAIN_WATER = 1, //浇水
    MAINTAIN_MUSIC = 2, //音乐
    MAINTAIN_MATERIAL = 3, //施肥
    MAINTAIN_GET    = 4,//收获
    MAINTAIN_DEL = 5, //铲除
};//对植物的维护操作类型


enum
{
    NPC_SCORE_INIT  = 0,//初始状态
    NPC_SCORE_NOT   = 1,//未点评过
    NPC_SCORED      = 2,//已经点评过
};


enum
{
    ELF_DROP    = 0,//放弃捕获的小精灵
    ELF_ATTRACT = 1, //收养了捕获的小精灵
    ELF_DEL     = 2, //删除了曾经收养的小精灵
};


enum
{
	PET_ID_BEGIN		= 1,
	PET_ID_END 		= 100,
    ITEM_ID_BEGIN    = 1000,//道具起始id
    ITEM_ID_END     = 1000000,//道具结束id
    TASK_REWARD_NONE = 0,
	TASK_REWARD_COIN = 10000001,//任务奖励金币
	TASK_REWARD_EXP = 10000002,//任务奖励经验
};

enum
{
    DENOTE_SUCCESS_BUT_NOT_REACHED  = 0,//募捐成功但计划尚未达成
    DENOTE_SUCCESS_AND_REACHED      = 1, //募捐成功并计划达成(用户达成)
    DENOTE_FAIL_AND_REACHED         = 2, //募捐已结束(别人导致募捐结束)
    DENOTE_COINS_NOT_ENOUGH         = 3, //咕咚果不够
    DENOTE_REACHED_LIMIT            = 4, //已达到募捐上限
};


enum
{
	HAVENOT_GET_REWARD  = 0,//没有领奖
	HAVE_GET_REWARD	    = 1,//已经领奖
};


enum
{
	ROOM_SCORE			= 1,//小屋评分
	DAILY_TEST			= 2,//每日挑战
};

enum
{
    ITEMS_CONF = 0,//parse_xml_items;
    SHOPS_CONF = 1, //parse_xml_shops;

    PET_CONF    =  2, //parse_xml_pet;

    LEVEL_CONF = 3, //parse_xml_level;

    GAME_CONF   = 4, //parse_xml_game;

    INTERACTIVE_CONF    = 5,//parse_xml_interactive;

    SYSTEM_CONF = 6, //parse_xml_system;

    BADGE_CONF = 7, //parse_xml_badge;

    PET_SERIES_CONF = 8,//parse_xml_pet_series;

    FORMULAS_CONF = 9, //parse_xml_formulaes;

    BOBO_CONF = 10, //parse_xml_bobo;

    MAINTAIN_CONF   = 11, //parse_xml_maintain;

    GAME_CHANGE_CONF = 12, //parse_xml_game_change;

    TASK_CONF = 13, //parse_xml_task;

    DENOTE_CONF = 14, //parse_xml_denote;

    MUSEUM_CONF = 15, //parse_xml_museum;

    USER_NAME_CONF = 16, // parse_xml_user_name;

    ACTIVITY_CONF = 17, // parse_xml_activity;

    MAX_CONF_FILE_NUM = 50,
};

enum
{
    LIMIT_ACTIVITY_ID   = 1,//封测活动id
    OPEN_ACTIVITY_ID    = 2, //公测活动id
    INVITOR_ACTIVITY_ID = 3, //邀请好友活动id
	DRAGON_BOAT_ACTIVITY_ID =4,

    LIMIT_PAPER_ID  = 20146, //封测大礼海报id
    LIMIT_STATUE_ID = 20145, //封测大礼雕像id
    LIMIT_END_TIMESTATMP = 1338393599, //封测结束时间2012-05-30 23:59:59
};

enum
{
    OPEN_ACT_REWARD1 = 13033, //公测活动奖品1 窗户
    OPEN_ACT_REWARD2 = 12035, //地板
    OPEN_ACT_REWARD3 = 11035,//men
    OPEN_ACT_REWARD4 = 10045,//bizhi
};

enum
{
    ACTIVITY_STATUS_RUNNING = 0, //活动状态， 进行中
    ACTIVITY_STATUS_NEW     = 1, //活动状态  新添加的活动
    ACTIVITY_STATUS_FINISHED = 2, //活动状态 ，完成
};

enum
{
    CANNOT_GET  = 0, //活动奖品不可以领取
    HAVE_GET    = 1, //已经领取
    CAN_GET     = 2, //可以领取
};

enum
{
	NOT_FINISH = 0,
	FINISH_HAVE_REWARD = 1,
	FINISH_HAVENOT_REWARD = 2,

};

enum
{
    TEL_COM = 0, //电信用户
    NET_COM = 1,//网通用户
};
#endif //H_CONSTANT_H_20110712
