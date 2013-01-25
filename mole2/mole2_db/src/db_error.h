/*
 * =====================================================================================
 * 
 *       Filename:  common.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 18时33分18秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  db_error_INC
#define  db_error_INC

#include <db_error_base.h>
//已被封号
#define BAN_FOREVER_ERR			1120
//被封号14天
#define BAN_14DAYS_ERR			2541
//被封号7天
#define BAN_7DAYS_ERR			2542
//被封号24小时
#define BAN_24HOURS_ERR			1121
//离线消息太长了
#define MOLE2_OFFLINE_MSG_MAX_ERR			3101
//好友已经存在 
#define  MOLE2_FRIENDID_EXISTED_ERR         3103
//好友不存在
#define  MOLE2_FRIENDID_NOFIND_ERR          3104
//好友个数越过限制
#define  MOLE2_FRIEND_MAX_ERR               3105
//黑名单用户已经存在 
#define  MOLE2_BLACKID_EXISTED_ERR         3103
//黑名单用户账号不存在
#define  MOLE2_BLACKID_NOFIND_ERR          3104
//黑名单个数越过限制
#define  MOLE2_BLACKID_MAX_ERR               3105
//没有指定的装扮信息（装备不存在）
#define MOLE2_ATTIRE_KEY_NOFIND_ERR			3107
//相同关键字的装备已经存在
#define MOLE2_ATTIRE_KEY_EXISTED_ERR		3108
//指定宠物不存在
#define MOLE2_PETID_NOFIND_ERR				3109
//宠物ID已经存在
#define MOLE2_PETID_EXISTED_ERR				3110
//指定技能ID已经存在
#define	MOLE2_SKILLID_EXISTED_ERR			3111 
//指定技能ID不存在
#define MOLE2_SKILLID_NOFIND_ERR			3112
//指定任务已存在
#define MOLE2_TASK_EXISTED_ERR				3113
//指定任务不存在
#define MOLE2_TASK_NOFIND_ERR				3114
//任务重复接收错误
#define MOLE2_TASK_RECVED_ERR				3115
//接已完成任务错误
#define MOLE2_TASK_FINISHED_ERR				3116
//该类任务同时接受达到上限
#define MOLE2_TASK_PER_TYPE_MAX_ERR			3117
//重复任务次数已达上限
#define MOLE2_TASK_REPEAT_MAX_ERR			3118
//任务不在进行中
#define MOLE2_TASK_NOT_ONGOING_ERR			3119
//依赖的父任务没有完成
#define MOLE2_TASK_PARENT_NOT_FIN_ERR		3120
//背包中没有对应ID的物品
#define MOLE2_ITEM_ID_NOFIND_ERR			3121
//物品ID已在列表中
#define MOLE2_ITEM_ID_EXISTED_ERR			3122
//物品包裹已满
#define MOLE2_ITEM_BAG_FULL_ERR				3123
//包裹里的物品不够减少
#define MOLE2_ITEM_NOT_ENOUGH_ERR			3124
//人物属性点不够
#define MOLE2_ATTR_ADD_NOT_ENOUGH_ERR		3125

//人物属性达到上限
#define MOLE2_ATTR_MAX_ERR					3126

//随机信息已存在
#define MOLE2_RAND_INFO_EXISTED_ERR			3127

//随机信息不存在
#define MOLE2_RAND_INFO_NOFIND_ERR			3128

//获取随机物品重复次数上限
#define MOLE2_RAND_REPEAT_MAX_ERR			3129

//获得活动物品达到上限
#define RAND_ITEM_COUNT_FULL_ERR			3130

//主键重复
#define MOLE2_KEY_EXIST_ERR				3131

#define MOLE2_KEY_NOT_EXIST_ERR				3132

//update 时数据库中无相应数据
#define USER_LOGIN_NOFIND_ERR               4327

//写入登录信息错误
#define LOGIN_INFO_WRITE_ERR                4328

//读出登录信息错误
#define LOGIN_INFO_READ_ERR                 4329

//打开文件失败
#define LOGIN_INFO_OPEN_ERR                 4330

//包裹物品携带数量达到上限
#define MOLE2_ITEM_COUNT_MAX_ERR			4331	

//激活码-用户账户已经被激活
#define MOLE2_USER_HAS_ACTIVED_ERR			4332

//激活码-激活码已经被使用
#define MOLE2_ACTIVATION_HAS_USED_ERR		4333

//激活码-激活码未找到
#define MOLE2_ACTIVATION_NOFIND_ERR				4334

//地图ID未找到
#define USER_MAP_MAPID_NOFIND_ERR				4335

//没飞行点权限
#define USER_MAP_FLIGHT_PATH_ERR				4336

//地图id已经存在
#define USER_MAP_MAPID_EXISTED_ERR			4337

//物品图鉴中id已经存在
#define ITEM_HANDBOOK_ID_EXISTED_ERR		4338

//物品图鉴中id未找到
#define ITEM_HANDBOOK_ID_NOFIND_ERR		4339

//怪物图鉴中id已经存在
#define MONSTER_HANDBOOK_ID_EXISTED_ERR		4340

//怪物图鉴中id未找到
#define MONSTER_HANDBOOK_ID_NOFIND_ERR		4341

//物品图鉴信息已经开启
#define ITEM_HANDBOOK_STATE_IS_ON_ERR	4342

//怪物图鉴信息已经开启
#define MONSTER_HANDBOOK_STATE_IS_ON_ERR	4343

//用户称号已经存在
#define USER_TITLE_TITLEID_EXISTED_ERR		4344

//用户称号不存在
#define USER_TITLE_TITLEID_NOFIND_ERR		4345

//boss点已经激活
#define UER_MAP_BOSS_POSITION_ON_ERR				4346

//任务未完成
#define TASK_NO_FINISH_ERR					4347

//存取物品错误
#define ITEM_STORAGE_COUNT_ERR			4348

//邮件ID已存在
#define MAILID_EXISTED_ERR				4349
//邮件ID找不到
#define MAILID_NOFIND_ERR				4350

//邮件达到上限
#define	MAIL_COUNT_MAX_ERR				4351

//神奇密码卡不存在
#define MOLE2_PRESENT_ID_NOFIND_ERR		4352

//神奇密码卡用户存在
#define MOLE2_PRESENT_UID_EXISTS_ERR		4353

//用户id未找到
#define MOLE2_PRESENT_UID_NOFIND_ERR		4354

//神奇密码卡已经使用了
#define MOLE2_PRESENT_ID_USED_ERR				4355

//激活码不存在
#define MOLE2_ACTIVATION_ID_NOFIND_ERR		4356

//用户存在
#define MOLE2_ACTIVATION_UID_EXISTS_ERR		4357

//用户id未找到
#define MOLE2_ACTIVATION_UID_NOFIND_ERR		4358

//激活码已经使用了
#define MOLE2_ACTIVATION_ID_USED_ERR				4359

//激活码尝试次数到达上限
#define MOLE2_ACTIVATION_TRY_ERR			4360
//数值已超过上限		
#define VALUE_MORE_THAN_LIMIT_ERR		4361
//数量不足
#define VALUE_NOT_ENOUGH_ERR				4362
//师徒标志不匹配
#define USER_RELATION_FLAG_ERR			4363
//师徒人数已到上限
#define USER_RELATION_LIMIT_ERR			4364
//今天不能删除
#define MOLE2_ID_EXISTED_ERR			4365
//用户不是师傅,非法操作
#define USER_NOT_TEACHER_ERR			4366
#define TEAM_BALANCE_ERR				4367
#define USER_RELATION_LEVEL_ERR		4368
#define MOLE2_ID_NOT_EXISTED_ERR		4369


#endif   /* ----- #ifndef COMMON_INC  ----- */

