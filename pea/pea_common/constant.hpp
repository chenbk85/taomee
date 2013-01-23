#ifndef H_CONSTANT_H_2011_11_25
#define H_CONSTANT_H_2011_11_25


// 游戏id
#define GAME_ID                 (1)


// session长度
#define SESSION_LEN             (16)

// 用户名字长度
#define MAX_USER_NAME_LEN       (16)

// 用户体力值上限
#define MAX_USER_STRENGTH       (100)

#define MAX_NICK_SIZE           MAX_USER_NAME_LEN


/*! 房间名最大长度 (最后一个字符强制改成 '\0') */
#define MAX_ROOM_NAME_LEN       16


/*! 房间密码最大长度 (最后一个字符强制改成 '\0') */
#define MAX_ROOM_PWD_LEN        16

// 房间里座位数量上限
#define MAX_PVP_ROOM_SEAT_NUM   8


#define MAX_PVE_ROOM_SEAT_NUM   4

// 每场战斗参与房间的上限
#define MAX_ROOM_PER_BATTLE     4


// 创建者的初始座位号
#define INITIAL_PLAYER_SEAT     0


// 错误描述缓冲区的长度
#define MAX_ERROR_BUFFER_LEN    4096



////////////////////////////// 精灵 ///////////////////////////////

// 精灵等级上限
// 配表给到450, 程序中多+1防止数组越界
#define MAX_PET_LEVEL           451


// 精灵的辅助影响的属性个数
#define PET_ASSIST_ATTR_NUM     4

// 精灵的随机成长属性个数
#define PET_RAND_ATTR_NUM       4

// 精灵的培养属性个数
#define PET_TRAIN_ATTR_NUM      4

// 精灵培养的类型数
// 6种精灵培养类型
#define PET_TRAIN_ID_NUM        6

// 精灵的血脉个数上限
#define MAX_PET_BADGE_NUM       10

// 辅助精灵数量上限
#define MAX_ASSIST_PET_NUM      2

// 背包中精灵数量上限
#define MAX_BAG_PET_NUM         99

// 身上最多穿上的装备数量
#define MAX_EQUIP_ON_PLAYER		6



// 玩家阵营
enum
{
	TEAM_ONE   = 0,
	TEAM_TWO   = 1,
	TEAM_NO    = 0xffffffff,
};

//邮件相关
#define MAX_MAIL_TITLE_LEN     (50)
#define MAX_MAIL_CONTENT_LEN   (300)
#define MAX_MAIL_PAY_FOR       (10)
#define SYSTEM_SENDER_ID       (0)
#define SYSTEM_SENDER_NAME     ("系统邮件")
#define MAX_MAIL_COUNT         (100)
#define MAX_NUM_ENCLOSURE_COUNT 3
#define MAX_ITEM_ENCLOSURE_COUNT 3
#define MAX_EQUIP_ENCLOSURE_COUNT 3


#define PEA_MAX(a,b) \
({ typeof (a) _a = (a); \
	typeof (b) _b = (b); \
	_a > _b ? _a : _b; })

#define PEA_MIN(a,b) \
({ typeof (a) _a = (a); \
    typeof (b) _b = (b); \
	    _a < _b ? _a : _b; })



///////////////////// 一些固定物品的id /////////////////////

// 聪明豆
#define CONG_MING_DOU_ID    10029

// 幻石
#define HUAN_SHI_ID         10032

#endif
