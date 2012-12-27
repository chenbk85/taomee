#ifndef  GF_DB_ERROR_H
#define  GF_DB_ERROR_H
#include <db_error_base.h>

//角色不存在
#define ROLE_ID_NOFIND_ERR					1106

#define ROLE_ID_NO_VALID_ROLE_2_DELETE      1109

//角色已经存在
#define ROLE_ID_EXISTED_ERR					1107
//角色等级不够
#define ROLE_LV_NOT_REACH_ERR				1108

#define ROLE_TYPE_NOFIND_ERR				1111

#define ROLE_NUM_MIN_ERR					1112

//好友已经存在
#define  GF_FRIENDID_EXISTED_ERR 			3103
//好友不存在
#define  GF_FRIENDID_NOFIND_ERR 			3104
//好友个数越过限制
#define  GF_FRIEND_MAX_ERR 					3105
//不能添加自己好友
#define GF_FRIEND_SELF_ERR					3106

//使用的衣服超过最大数
#define  GF_CLOTHES_MAX_ERR					3110

//装扮衣服不存在
#define GF_ATTIREID_NOFIND_ERR				3111

//装扮衣服已经存在
#define GF_ATTIREID_EXISTED_ERR				3112

//added by cws 0608
#define GF_SWAPID_EXISTED_ERR				3192

//装备数量已到最大
#define GF_ATTIRE_MAX_ERR					3113
//物品超过购买上限
#define GF_ATTIRE_MAX_BUYLIMIT_ERR          3114

//item id existed
#define GF_ITEM_EXISTED_ERR					3120

//item id not existed
#define GF_ITEM_NOFIND_ERR					3121


//物品数量不足
#define GF_ITEM_NOTENOUGH_ERR				3122

//物品种类太多
#define GF_ITEM_KIND_MAX_ERR				3123

//task err
//任务已经存在
#define GF_TASK_EXISTED_ERR					3130

//任务不存在
#define GF_TASKID_NOFIND_ERR				3131

#define GF_KILLED_BOSS_EXISTED_ERR			3132

#define GF_KILLED_BOSS_NOTFIND_ERR			3133


//skill err
//技能已存在
#define GF_SKILL_EXISTED_ERR				3140

//技能不存在
#define GF_SKILL_NOFIND_ERR					3141

//技能点不够
#define GF_SKILL_POINT_NOENOUGH_ERR			3142

//begin summon
//召唤兽已经存在
#define GF_SUMMON_EXISTED_ERR               3160
//没有发现召唤兽
#define GF_SUMMON_NOFIND_ERR                3161
//end summon
//
#define GF_PLANT_EXISTED_ERR               3162
#define GF_PLANT_NOFIND_ERR                3163
//
#define GF_DAILY_ACTION_TYPE_NOFIND_ERR     3150
#define GF_DAILY_ACTION_TYPE_MAX_ERR        3151
#define GF_DAILY_ACTION_TYPE_EXISTED_ERR    3152
#define GF_DAILY_ACTION_TYPE_FLAG_ERR       3153
//
#define GF_WEEK_ACTION_TYPE_NOFIND_ERR     3170
#define GF_WEEK_ACTION_TYPE_MAX_ERR        3171
#define GF_WEEK_ACTION_TYPE_EXISTED_ERR    3172
#define GF_WEEK_ACTION_TYPE_FLAG_ERR       3173
//
#define GF_MONTH_ACTION_TYPE_NOFIND_ERR     3180
#define GF_MONTH_ACTION_TYPE_MAX_ERR        3181
#define GF_MONTH_ACTION_TYPE_EXISTED_ERR    3182
#define GF_MONTH_ACTION_TYPE_FLAG_ERR       3183

//钱不够
#define GF_XIAOMEE_NOENOUGH_ERR				3200

//数量不足
#define GF_NUM_NOENOUGH_ERR					3201

//经验不足
#define GF_EXP_NOENOUGH_ERR					3202

//伏魔点不足
#define GF_FUMO_NOENOUGH_ERR			    3203
//经验分配器经验不足
#define GF_ALLOTER_EXP_NOENOUGH_ERR			3204

//角色数量已达到上限
#define GF_ROLE_NUM_MAX_ERR					3300

//用户没有参加大使任务
#define USER_ID_ABSENT_AMB                  3310

//离线信息大多了
#define GF_OFFLINE_MSG_MAX_ERR 				4101

//不合法的邀请码
#define GF_INVALID_INVITE_CODE				5101

#define GF_INVITE_CODE_USED					5102

#define GF_USER_NOT_INVITED					5103
//仓库物品数量超过了最大限制
#define GF_WAREHOUSE_MAX_COUNT_ERROR                            5104
//该用户没有该副职
#define GF_SECONDARY_PRO_NOT_EXIST          5105
//该用户副职已经存在
#define GF_SECONDARY_PRO_EXIST              5106
//该邮件ID不存在
#define GF_MAIL_ID_NOT_EXIST                5120
//邮箱已近满
#define GF_MAIL_FULL                        5121

#define GF_HERO_TOP_ERR                     5130
#define GF_HUNTER_TOP_ERR                   5131
#define GF_CONTEST_INFO_ERR                 5132
#define GF_CONTEST_TEAM_ERR                 5133
#define GF_WUSENG_INFO_ERR                  5136

#define GF_CARD_INFO_ERR                    5134

#define GF_TEAM_MEMBER_FULL_ERR             5135
#define GF_TEAM_MEMBER_COINS_LIMIT          5136
#define GF_TEAM_MEMBER_NOT_FOUND            5137

#define GF_STAT_NO_DATA                     5140

#endif   /* ----- #ifndef COMMON_INC  ----- */

