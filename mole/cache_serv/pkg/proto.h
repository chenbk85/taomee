/*
 * =====================================================================================
 * 
 *	   Filename:  proto.h
 * 
 *	Description:  
 * 
 *		Version:  1.0
 *		Created:  2007年11月01日 14时10分21秒 CST
 *	   Revision:  none
 *	   Compiler:  gcc
 * 
 *		 Author:  xcwen (xcwen), xcwenn@gmail.com
 *		Company:  NULL
 * 
 * =====================================================================================
 */

#ifndef  PROTO_INC
#define  PROTO_INC
#ifndef  MK_PROTO_DOC 
#include "proto_header.h"
#endif
//在cmdid 中数据库识别标志
#define RROTO_ROUTE_FIELD 				0xFC00 //使用前6个bit

//是否需要保存交易报文
//有涉及到更新数据库数据的协议，都要保存 
#define NEED_UPDATE							0x0100

/// 客户端私有数据，获取
#define ROOMINFO_CLIENT_DATA_GET_CMD (0xB469)

/// 客户端私有数据，设置
#define ROOMINFO_CLIENT_DATA_SET_CMD (0xB46A | NEED_UPDATE)

/// 取得用户所有宠物所有任务所有状态
#define USER_PET_TASK_GET_CLIENT_BY_USER_TASK_CMD (0x10EF)

/// 用户宠物使用一次技能
#define USER_USE_SKILL_ONE_CMD (0x10EE | NEED_UPDATE)

/// 取得神殿建设高度
#define USER_PET_HALT_GET_CMD (0x10ED)

/// 取得用户所有宠物所有任务所有状态
#define USER_PET_TASK_GET_BY_USER_CMD (0x10EC)

/// 更新神殿建设高度
#define USER_PET_HALT_SET_CMD (0x10EB | NEED_UPDATE)

/// 更新宠物技能点
#define USER_PET_SKILL_SET_CMD (0x10EA | NEED_UPDATE)

/// 更改宠物的逻辑生日，此时用于宠物泡温泉
#define USER_PET_HOT_SPRING_CMD (0x10E9 | NEED_UPDATE)

/// 根据列表查询宠物的装扮的数目
#define USER_PET_GET_COUNT_WITH_ATTIREID_CMD (0x10E8)

///勇士抽奖，询查目前抽奖情况
#define ROOMINFO_WARRIOR_LUCKY_DRAW_GET_CMD (0xB468)

///勇士抽奖，限制抽奖次数和概率
#define ROOMINFO_WARRIOR_LUCKY_DRAW_CMD (0xB467 | NEED_UPDATE)

/// 发现在企鹅，设置用户企鹅孵蛋的次数
#define ROOMINFO_PENGUIN_EGG_SET_CMD (0xB466 | NEED_UPDATE)

/// 发现在企鹅，取得用户企鹅孵蛋的次数
#define ROOMINFO_PENGUIN_EGG_GET_CMD (0xB465)

/// 用户搬石头，设置当天搬的石头和总共的石头
#define ROOMINFO_STONE_MOVE_SET_CMD (0xB464 | NEED_UPDATE)

/// 用户搬石头，取得当天搬的石头和总共的石头
#define ROOMINFO_STONE_MOVE_GET_CMD (0xB463)

/// 为VIP用户设置超拉等级
#define USER_VIP_PET_LEVEL_SET_CMD (0x10E7 | NEED_UPDATE)

/// @brief 设置一个VIP用户的成长值基值
#define USER_VIP_MARK_LAST_SET_CMD (0x10E6 | NEED_UPDATE)

/// @brief 一个宠物的任务已完成，赠送一个宠物物品
#define USER_PET_TASK_ATTIRE_GIVE_CMD (0x10E5 | NEED_UPDATE)

/// @brief 用摩尔豆购买批量物品
#define USER_BUY_MANY_THING_IN_ONE_TIME_CMD (0x10E4 | NEED_UPDATE)

/// @brief 得到植物的果实ID号
#define USER_JY_GET_MATURE_FRUIT_ID_CMD  	(0x10E3)

/// @brief 设置植物成长值，依据时间
#define USER_PLANT_GROWTH_ADD_BY_HOUR_CMD (0x10E2 | NEED_UPDATE)

/// @brief 新年拍照，用户照片上的服装，获取
#define USER_DRESS_IN_PHOTO_GET_CMD (0x10E1)

/// @brief 新年拍照，用户照片上的服装，设置
#define USER_DRESS_IN_PHOTO_SET_CMD (0x10E0 | NEED_UPDATE)

/**
 * @brief 钓鱼，取得所有垂钓的鱼总重量，对于指定用户
 * @in 用户米米号
 * @out 相应的鱼的总重量
 */
//钓鱼，取得相应用户鱼的总重量
#define ROOMINFO_FISH_WEIGHT_GET_CMD (0xB45F)

/**
 * @brief 钓鱼，取得所有垂钓的鱼总重量，对于指定用户
 * @in 用户米米号
 * @in 鱼的种类id号
 * @out 用户所有鱼的总重量
 */
//钓鱼，给相应用户添加一个条鱼的内容
#define ROOMINFO_FISH_WEIGHT_SET_CMD (0xB460 | NEED_UPDATE)

//增加能量心的数量
#define ROOMINFO_ADD_ENERGY_STAR_CMD     	(0xB461 |NEED_UPDATE)

//得到能量之星
#define ROOMINFO_GET_ENERGY_STAR_CMD     	(0xB462 )
/**
 * @brief 取得蛋糕的数目
 * @in 用户米米号
 * @out 用户所拥有的蛋糕数目
 */

#define ROOMINFO_CAKE_GIFT_GET_CMD (0xB45D)

/**
 * @brief 取得蛋糕的数目
 * @in 用户米米号
 * @in 需要更改的数值
 * @out 成功：1, 失败：0
 */
#define ROOMINFO_CAKE_GIFT_SWAP_DO_SWAP_CMD (0xB45E | NEED_UPDATE)

/**
 * @brief 读取鸭子的状态，可能存在两个状态
 * @in 用户ID
 * @out 鸭子的状态，0: 完好，1: 散架
 */
#define ROOMINFO_ROOMINFO_DUCK_STATU_GET_CMD (0xB459)

/**
 * @brief 设置鸭子的状态，可能存在两个状态
 * @in 鸭子的状态，用户ID
 * @out 成功或失败，无返回，只有状态码
 */
#define ROOMINFO_ROOMINFO_DUCK_STATU_SET_CMD (0xB45C | NEED_UPDATE)

/* 服装打分，取得一个组合的上次打分
 * 输入：一个服装的组合，目前最多只有12件衣服
 * 输出：相应服装组合的分数，
	如果不存在 错误返回: ROOMINFO_DRESSING_MARK_NO_UNIT_ERR 
 */
#define ROOMINFO_ROOMINFO_DRESSING_MARK_GET_CMD (0xB458)

/* 服装打分，设置当次组合的分数
 * 输入：一个服装的组合和这相应的分数
 * 输出：无
 */
#define ROOMINFO_ROOMINFO_DRESSING_MARK_SET_CMD (0xB457 | NEED_UPDATE)

//暖房，兔子可以在暖房里配对生出小兔子
//把一个兔子放到暖房，此时它已经配对了
//输入：用户ID和动物ID，此动物默认为兔子
#define USER_GREENHOUSE_INSERT_CMD	(0x3040 | NEED_UPDATE)

//暖房，领取之前放入的兔子，此时已经生出小兔子
//并且需要保证时间的充足，数据库不负责这一点
//输入：用户ID和动物ID
//输出：新生兔子的类型——黑、白、花
#define USER_GREENHOUSE_DROP_CMD	(0x3041 | NEED_UPDATE)

//暖房，查询指定兔子配对的时间
//输入：用户ID和动物ID
//输出：离生出小兔子的剩余时间，类型为(uint32_t)time_t
#define USER_GREENHOUSE_REMAIN_TIME_CMD	(0x3042)

//暖房，查询指定用户有没有动物正在配对
//输入：用户ID
//输出：存在或不存在，类型为uint32_t: 0, false; 1, true
#define USER_GREENHOUSE_USER_EXIST_CMD	(0x3043)


//暖房，取得指定用户所有正在配对的动物
//输入：用户ID
//输出：所有正在配对的动物列表
#define USER_GREENHOUSE_USER_ALL_ANIMAL_CMD (0x3044)


//user通过字段名设置值
#define USER_SET_INT_VALUE_CMD           	(0x3050 |NEED_UPDATE)
//user_ex通过字段名设置值
#define USER_EX_SET_INT_VALUE_CMD           	(0x3051 |NEED_UPDATE)
//设置拉姆的任务
#define USER_PET_TASK_SET_TASK_CMD       	(0x3052 |NEED_UPDATE)
//得到拉姆的任务
#define USER_PET_TASK_GET_TASKLIST_CMD   	(0x3053 )
//设置拉姆任务的临时数据
#define USER_PET_TASK_SET_TEMP_DATA_CMD  	(0x3054 |NEED_UPDATE)
//得到拉姆任务的临时数据
#define USER_PET_TASK_GET_TEMP_DATA_CMD  	(0x3055 )


//注册用户-ex
#define USERINFO_REGISTER_EX_CMD			(0x0003 | NEED_UPDATE)  
//删除用户
#define USERINFO_DEL_USER_CMD				(0x0004 | NEED_UPDATE)
//检查米米号是否存在
#define USERINFO_CHECK_EXISTED_CMD			(0x0005 )

//用户登入-ex
#define USERINFO_LOGIN_EX_CMD				(0x0006 ) 

//得到用户的游戏产品注册了哪些
#define USERINFO_GET_GAMEFLAG_CMD        	(0x0007 )

//用户的游戏产品注册了哪个产品
#define USERINFO_ADD_GAME_CMD            	(0x0008 | NEED_UPDATE)
//修改登入密码
#define USERINFO_CHANGE_PASSWD_CMD			(0x0009 | NEED_UPDATE)

//SU修改登入密码
#define USERINFO_SU_SET_PASSWD_CMD       	(0x000A |NEED_UPDATE)

//SU修改支付密码
#define USERINFO_SU_SET_PAYPASSWD_CMD    	(0x000B |NEED_UPDATE)
//带有统计的登入
#define USERINFO_LOG_WITH_STAT_CMD              (0x000C )
//登入，帕拉巫用
#define USERINFO_LOG_FOR_BLW_CMD         	(0x000D )
//设置密码邮箱
#define USERINFO_SET_PASSWDEMAIL_CMD     	(0x0017 |NEED_UPDATE)


//是否设置过密码邮箱
#define USERINFO_ISSET_PASSWDEMAIL_CMD   	(0x0018 )
//是否确认过密码邮箱
#define USERINFO_ISCONFIRM_PASSWDEMAIL_CMD	(0x0019 )
//确认密码邮箱
#define USERINFO_CONFIRM_PASSWDEMAIL_CMD 	(0x001A |NEED_UPDATE)
//得到密码邮箱
#define USERINFO_GET_PASSWDEMAIL_CMD     	(0x001B )

//得到历史更新的密码记录
#define USERINFO_GET_PASSWD_HISTORY_CMD  	(0x001C )

//设置密码邮箱-ex
#define USER_SET_PASSWDEMAIL_EX_CMD      	(0x001D |NEED_UPDATE)


//修改密码 与USERINFO_SET_FLAG_CHANGE_PASSWD配合使用
#define USERINFO_CHANGE_PASSWD_NOCHECK_CMD	(0x0040 | NEED_UPDATE)
//设置生日,性别
#define USERINFO_SET_USER_BIRTHDAY_SEX_CMD  (0x0041 | NEED_UPDATE)
#define USERINFO_IS_SET_USER_BIRTHDAY_CMD   (0x0042 )
#define USERINFO_SET_USER_EMAIL_CMD 	  	(0x0043 | NEED_UPDATE)
#define USERINFO_GET_USER_EMAIL_CMD   		(0x0044 )
//得到FLAG
#define USER_INFO_GET_SET_PASSWD_FLAG_CMD        	(0x0045 )

//设置 可以不用检查old passwd.(一次),与USERINFO_CHANGE_PASSWD_NOCHECK 一起使用
#define USERINFO_SET_FLAG_CHANGE_PASSWD_CMD	(0x005A | NEED_UPDATE)

//得到问题
#define USERINFO_GET_QUESTION_CMD        	(0x0070 )

#define USERINFO_GET_INFO_EX_CMD   			(0x0080 )
#define USERINFO_SET_INFO_EX_CMD   			(0x0081 | NEED_UPDATE)
#define USERINFO_GET_INFO_ALL_CMD   		(0x0082 )
#define USERINFO_SET_PAYINFO_CMD            (0x0083 |NEED_UPDATE)

//检查问题与答案
#define USERINFO_CHECK_QUESTION_CMD         (0x0084 )

//是否设置过问题了
#define USERINFO_ISSET_QUESTION_CMD         (0x0085 )


#define USERINFO_GET_SECURITY_FLAG_CMD      (0x0086 )
#define USERINFO_SET_PEMAIL_BY_QA_CMD       (0x0087 |NEED_UPDATE)
#define USERINFO_SET_PEMAIL_ISOK_CMD        (0x0088 |NEED_UPDATE)
//得到昵称和好友列表
#define USERINFO_GET_NICK_FLIST_CMD         (0x0089 )
//同步论坛
#define USERINFO_SYNC_DV_CMD             	(0x008A |NEED_UPDATE)

//验正支付密码
#define USERINFO_CHECK_PAYPASSWD_CMD     	(0x008B )
//设置支付密码
#define USERINFO_SET_PAYPASSWD_CMD       	(0x008C |NEED_UPDATE)

//是否已经设置过支付密码
#define USERINFO_ISSET_PAYPASSWD_CMD     	(0x008D )

//设置问题和答案
#define USERINFO_SET_QA_CMD              	(0x008E |NEED_UPDATE)

//清空支付密码
#define USERINFO_CLEAN_PAYPASSWD_CMD     	(0x008F |NEED_UPDATE)

//设置支付密码，需要检查之前的密码
#define USERINFO_SET_PAYPASSWD_WITH_CHECK_CMD	(0x0090 |NEED_UPDATE)

//得到登入信息
#define USERINFO_GET_LOGIN_INFO_CMD      	(0x0091 )
//设置 是否可以不检查旧密码，修改新的支付密码
#define USER_INFO_SET_FALG_CHANGE_PAYPASSWD_CMD	(0x0092 |NEED_UPDATE)
//修改支付密码，不需要验证旧密码
#define USER_INFO_CHANGE_PAYPASSWD_NOCHECK_CMD	(0x0093 |NEED_UPDATE)






//user
//测试：返回所在主机信息，当前程序版本
#define USER_TEST_PROTO_CMD				(0x1000) 
//登入
#define USER_LOGIN_CMD					(0x1001) 



//注册
#define USER_REGISTER_CMD				(0x1002 | NEED_UPDATE)  

//摩尔注册-ex
#define USER_REGISTER_EX_CMD              	(0x1003 |NEED_UPDATE)
//删除用户
#define USER_DEL_USER_CMD				(0x1004 | NEED_UPDATE)

//检查USERID 存在与否
#define USER_CHECK_EXISTED_CMD			(0x1005 )


//得到用户的信息-ex
#define USER_GET_INFO_EX_CMD             	(0x1006)

//设置任务值
#define USER_SET_TASK_CMD             		(0x1007 | NEED_UPDATE)

//登入-ex
#define USER_LOGIN_EX_CMD					(0x1008) 

//注册-ex2用于
#define USER_REGISTER_EX2_CMD			(0x1009 | NEED_UPDATE)  



//检查USERID 存在与否
#define USER_CHECK_EXISTED_EX_CMD		(0x100A )

//检查ID是否存在在好友列表中
#define USER_CHECK_FRIEND_EXISTED_CMD	(0x100B )



//得到摩尔豆
#define USER_GET_XIAOMEE_CMD             	(0x100D )


//得到vip标志和昵称
#define USER_GET_VIP_NICK_CMD            	(0x100E )

//激活用户
#define USER_SET_USER_ENABLED_CMD		(0x100F | NEED_UPDATE)

//设置在线信息
#define USER_SET_USER_ONLINE_CMD		(0x1010 | NEED_UPDATE)
//增加好友
#define USER_ADD_FRIEND_CMD 			(0x1011 | NEED_UPDATE)
//删除好友
#define USER_DEL_FRIEND_CMD 			(0x1012 | NEED_UPDATE)

//得到好友列表
#define USER_GET_FRIEND_LIST_CMD         	(0x1013 )



//设置VIP
#define USER_SET_VIP_FLAG_CMD 			(0x1014 | NEED_UPDATE)

//得到标志信息
#define USER_GET_FLAG_CMD               (0x1015 )

//用摩尔豆购买物品
#define USER_BUY_ATTIRE_BY_XIAOMEE_CMD	(0x1016 | NEED_UPDATE)

//卖物品
#define USER_SALE_ATTIRE_CMD			(0x1017 | NEED_UPDATE)

//得到物品列表 <br />特别事项:当attire_usedflag==0&& 时 endattireid必须=startattireid+1;
#define USER_GET_ATTIRE_LIST_CMD		(0x1018)

//交换装扮列表:in_item_1个数为oldcount,in_item_2个数为newcount,(<font color=red>online 不使用</font>)
#define USER_SWAP_ATTIRE_LIST_CMD		(0x1019 | NEED_UPDATE)
//设置装扮
#define USER_SET_ATTIRE_EX_CMD			(0x101A | NEED_UPDATE)


//user home attire
//编辑小屋装扮
#define USER_SET_HOME_ATTIRELIST_CMD	(0x101B | NEED_UPDATE)
//增加物品
#define USER_ADD_ATTIRE_CMD				(0x101C | NEED_UPDATE)

//得到小屋中的使用中的物品信息
#define USER_GET_HOME_CMD				(0x101D	)
//得到背包和衣柜里的物品
#define USER_GET_ATTIRE_LIST_ALL_CMD            (0x101E )

//交换小屋物品: 定义：IN_ITEM_1.count=oldcount,IN_ITEM_2.count=newcount(<font color=red>online 不使用</font> 目前只用于加物品)
#define USER_SWAP_HOMEATTIRE_LIST_CMD	(0x101F | NEED_UPDATE)
//设置用户任务
#define USER_SET_TASKLIST_CMD			(0x1020 | NEED_UPDATE)

//家园:得到使用的物品信息
#define USER_GET_JY_CMD					(0x1021	)


//家园:交换物品: <br />oldcount=IN_ITEM_1.count<br />newcount=IN_ITEM_2.count
#define USER_SWAP_JYATTIRE_LIST_CMD     (0x1022 |NEED_UPDATE)

//家园:设置所有物品: <br />usedcount=IN_ITEM_2.count<br />nousedcount=IN_ITEM_1.count
#define USER_SET_JY_ATTIRELIST_CMD		(0x1023 | NEED_UPDATE)

//家园:得到使用的物品信息-ex
#define USER_GET_JY_EX_CMD               	(0x1024 )

//家园:种种子
#define USER_ADD_SEED_CMD                	(0x1025 |NEED_UPDATE)
//家园:删除种子
#define USER_DEL_SEED_CMD                	(0x1026 |NEED_UPDATE)
//家园：浇水
#define USER_WATER_SEED_CMD                 (0x1027 |NEED_UPDATE)
//家园：除虫
#define USER_KILL_BUG_CMD                   (0x1028 |NEED_UPDATE)

//家园：得到单一种子数据
#define USER_GET_SEED_CMD                	(0x1029 )


//增加离线信息
#define USER_ADD_OFFLINE_MSG_CMD 		(0x102A | NEED_UPDATE)
#define USER_GET_OFFLINE_MSG_LIST_CMD 	(0x102B | NEED_UPDATE)

//家园：摘取果实
#define USER_CATCH_FRUIT_CMD             	(0x102C |NEED_UPDATE)

//家园:得到未使用的物品信息
#define USER_GET_JY_NOUSED_LIST_CMD      	(0x102D )
//偷取家园的果实
#define USER_THIEVE_FRUIT_CMD               (0x102E |NEED_UPDATE)

//得到用户的已使用列表
#define USER_GET_USED_ATTIERLIST_CMD     	(0x102F )


#define USER_GET_USER_CMD				(0x1031)

//设置摩尔颜色
#define USER_SET_PETCOLOR_CMD			(0x1032 | NEED_UPDATE)
#define USER_GET_USER_WITH_MSGLIST_CMD	(0x1033)
#define USER_GET_USER_ALL_CMD			(0x1034)
//根据游戏的胜负设置分数
#define USER_UPDATE_GAME_SCORE_CMD       (0x1035 |NEED_UPDATE)
//设置装扮
#define USER_SET_ATTIRE_CMD				(0x1036 | NEED_UPDATE)
#define USER_DEL_ATTIRE_CMD				(0x1037 | NEED_UPDATE)
//打兔子，加值
#define USER_JY_ADD_EXP_CMD              	(0x1038 |NEED_UPDATE)
//修改数值
#define USER_CHANGE_USER_VALUE_CMD 		(0x1039 | NEED_UPDATE)


//设置已使用中的家具
#define USER_SET_USED_HOMEATTIRE_CMD    (0x103A |NEED_UPDATE)
//设置已使用中的家园中的数据
#define USER_SET_USED_JYATTIRE_CMD    	(0x103B |NEED_UPDATE)
//修改小米
#define USER_CHANGE_XIAOMEE_CMD          	(0x103C |NEED_UPDATE)
//设置小米的最大值信息
#define USER_SET_XIAOMEE_MAX_INFO_CMD    	(0x103D |NEED_UPDATE)
// 设置打工的等级
#define USER_SET_WORK_LEVEL_CMD                 (0x103E )
//设置道具在身上
#define USER_ATTIRE_SET_USED_CMD         	(0x103F |NEED_UPDATE)


#define USER_SET_USER_BIRTHDAY_SEX_CMD  (0x1041 | NEED_UPDATE)
#define USER_SET_HOME_ATTIRE_NOUSED_CMD (0x1045 | NEED_UPDATE)
#define USER_SET_FLAG_CMD 				(0x1046 | NEED_UPDATE)
#define USER_HOME_ATTIRE_CHANGE_CMD     (0x1047 | NEED_UPDATE)

//得到物品列表.(通过指定的物品ID)
#define USER_CHECK_ATTIRELIST_CMD        	(0x1048 )

//得到未使用的家具类型总数
#define USER_HOME_NOUSED_ATTIRE_TYPE_COUNT_CMD	(0x1049 )
//设置昵称
#define USER_SET_NICK_CMD 				(0x104A | NEED_UPDATE)
//得到昵称
#define USER_GET_NICK_CMD 				(0x104B )

//得到历史IP
#define USER_GET_IP_HISTORY_CMD          	(0x104C )

//物品交换
#define USER_SWAP_LIST_CMD               	(0x104D |NEED_UPDATE)


//设置用户家园访问列表的操作
#define USER_JY_ACCESS_SET_OPT_CMD       	(0x104E |NEED_UPDATE)

//物品交换_2 和 USER_SWAP_LIST 一致
#define USER_SWAP_LIST_2_CMD               	(0x104F |NEED_UPDATE)

//USER PET
#define USER_GET_USER_PET_CMD 			(0x1050 )
#define USER_ADD_USER_PET_CMD 			(0x1051 | NEED_UPDATE)
#define USER_SET_USER_PET_INFO_CMD 		(0x1052 | NEED_UPDATE)

//设置宠物的相关属性
#define USER_SET_USER_PET_COMM_CMD 		(0x1053 | NEED_UPDATE)
#define USER_SET_USER_PET_POSLIST_CMD 	(0x1054 | NEED_UPDATE)
#define USER_DEL_USER_PET_CMD 			(0x1055 | NEED_UPDATE)
#define USER_SET_USER_PET_ALL_CMD 		(0x1056 | NEED_UPDATE)
#define USER_SET_USER_PET_SKILL_CMD 	(0x1057 | NEED_UPDATE)

/**
 * @brief 设置宠物装扮，同时返回当前装扮
 * @in 需要设置的装扮
 * @out 当前装扮
 */
//设置用户宠物装扮，同时返回宠物当前的装扮
#define USER_PET_ATTIRE_SET_USED_CMD	(0x1058 | NEED_UPDATE)

#define USER_SET_FLAG_UNUSED_CMD		(0x105B | NEED_UPDATE)
#define USER_SET_FLAG_OFFLINE24_CMD		(0x105C | NEED_UPDATE)

//职业信息：得到
#define USER_PROFESSION_GET_CMD          	(0x105D )
//职业信息：设置
#define USER_PROFESSION_SET_CMD          	(0x105E |NEED_UPDATE)
//设置sick_type
#define USER_SET_PET_SICK_TYPE_CMD          (0x105F |NEED_UPDATE)


//大使
#define USER_CONNECT_ADD_CHILD_CMD		(0x1060 | NEED_UPDATE)
//大使
#define USER_CONNECT_SET_PARENT_CMD		(0x1061 | NEED_UPDATE)
//大使
#define USER_CONNECT_GET_INFO_CMD		(0x1062 )

//大使设置下线个数
#define USER_CONNECT_SET_CHILDCOUNT_CMD  	(0x1063 |NEED_UPDATE)


//通过摩尔豆购买宠物装扮
#define USER_BUY_PET_ATTIRE_BY_XIAOMEE_CMD	(0x1065 |NEED_UPDATE)

//拉取宠物装扮列表
#define USER_GET_PET_ATTIRE_LIST_CMD     	(0x1066 )



//设置宠物装扮
#define USER_SET_PET_ATTIRE_CMD          	(0x1067 |NEED_UPDATE)

//拉取使用中所有宠物装扮列表
#define USER_GET_PET_ATTIRE_USED_ALL_CMD 	(0x1068 )

//拉取所有宠物装扮列表
#define USER_GET_PET_ATTIRE_ALL_CMD 		(0x1069 )

//设置宠物装扮-ex
#define USER_PET_ATTIRE_UPDATE_EX_CMD    	(0x106A |NEED_UPDATE)

//得到宠物信息
#define USER_PET_GET_INFO_CMD            	(0x106B )

//加入班级
#define USER_ADD_CLASS_CMD               	(0x106C |NEED_UPDATE)
//退出班级
#define USER_DEL_CLASS_CMD               	(0x106D |NEED_UPDATE)

//设置首个班级
#define USER_SET_FIRST_CLASS_CMD         	(0x106E |NEED_UPDATE)

//得到班级列表
#define USER_GET_CLASSLIST_CMD           	(0x106F )


//宠物任务：设置
#define USER_PET_TASK_SET_CMD 			(0x1071 | NEED_UPDATE)
//魔法课设置：
#define USER_PET_TASK_SET_EX_CMD          (0x1072 |NEED_UPDATE)
//宠物任务：得到列表
#define USER_PET_TASK_GET_LIST_CMD 		(0x1073 )
//设置接某阶段任务结束 
#define USER_TASK_PET_FIRST_STAGE_SET_CMD       (0x1074 |NEED_UPDATE)


//user_card
//卡片系统:得到信息<br />
//定义cards的个数为count 如count=0 表示未初始化<br\>
//cards中前lower_count个为低级卡片，其后为高级卡片
#define USER_CARD_GETINFO_CMD            	(0x1075 )


//卡片系统:添加卡片
#define USER_CARD_ADD_CARD_CMD           	(0x1076 | NEED_UPDATE )

//卡片系统:添加胜负数
#define USER_CARD_ADD_WIN_LOSE_CMD       	(0x1077 |NEED_UPDATE)

//卡片系统:添加低级卡片
#define USER_CARD_ADD_LOWER_CARD_CMD        (0x1078 | NEED_UPDATE )

//卡片系统:初始化
#define USER_CARD_INIT_CMD               	(0x1079 |NEED_UPDATE)

//卡片系统:是否初始化了
#define USER_CARD_IS_INITED_CMD          	(0x107A )

//卡片系统:该用户还可以拿几张初级卡片
#define USER_CARD_GET_ENABLE_COUNT_CMD   	(0x107B )

//卡片系统:该用户还可以拿几张初级卡片
#define USER_CARD_SET_WIN_LOSE_CMD   		(0x107C |NEED_UPDATE)


//卡片系统:得到信息 -EX
#define USER_CARD_GETINFO_EX_CMD           	(0x107D )
//卡片系统:设置标志位
#define USER_CARD_SET_FLAG_BIT_CMD       	(0x107E |NEED_UPDATE)
//删除某一任务
#define USER_TASK_TASKID_DEL_CMD                (0x107F |NEED_UPDATE)


//生日飞艇:用户设置生日
#define USER_SET_BIRTHDAY_CMD            	(0x1080 | NEED_UPDATE)

//生日飞艇:用户是否设置已经生日
#define USER_ISSET_BIRTHDAY_CMD          	(0x1081 )

//活动:走路次数+1
#define USER_TEMP_ADD_VALUE_CMD          	(0x1082 |NEED_UPDATE)
//活动:得到走路次数
#define USER_TEMP_GET_VALUE_CMD          	(0x1083 )

//得到活动的临时数据
#define USER_GET_TEMPBUF_CMD             	(0x1084 )


//设置活动的临时数据
#define USER_SET_TEMPBUF_CMD             	(0x1085 |NEED_UPDATE)
//用户端设置
#define USER_TASK_SET_CLIENT_DATA_CMD          (0x1086 |NEED_UPDATE)
//得到字段
#define USER_TASK_GET_CLIENT_DATA_CMD           (0x1087 )
//得到拉姆任务状态
#define USER_TASK_GET_PET_TASK_CMD              (0x1088 )
//得到昵称和好友列表
#define USER_GET_NICK_FLIST_CMD               (0x1089 )
//把背包物品放到衣柜里
#define USER_ATTIRE_PUT_CHEST_CMD             (0x108A |NEED_UPDATE)
//把衣柜里的东西放回个人背包里
#define USER_ATTIRE_DEL_CHEST_CMD               (0x108B |NEED_UPDATE)
//得到衣柜里的物品
#define USER_ATTIRE_GET_CHEST_LIST_CMD          (0x108C )
//得到宠物的全部信息
#define USER_MAGIC_TASK_PET_GET_ALL_CMD         (0x108D )
//更新宠物的任务信息
#define USER_MAGIC_TASK_PET_UPDATE_CMD          (0x108E |NEED_UPDATE)
//得到一系列装扮的数量
#define USER_ATTIRE_GET_SOME_ATTIRE_COUNT_CMD	(0x108F )
//银行命令号
//插入存款记录
#define USER_BANK_ADD_CMD                   (0x1090 |NEED_UPDATE)
//得到全部存款记录
#define USER_BANK_GET_ACCOUNT_CMD           (0x1091 )
//删除存款记录
#define USER_BANK_DELETE_ACCOUNT_CMD        (0x1092 |NEED_UPDATE)


//设置存款记录
#define USER_BANK_SET_ACCOUNT_CMD        	(0x1093 |NEED_UPDATE)
//班级增加分数
#define USER_CLASS_ADD_SCORE_CMD         	(0x1094 |NEED_UPDATE)
//得到班级的分数
#define USER_CLASS_GET_SCORE_CMD         	(0x1095 )

/// 得到背包里的物品的数目,去除衣柜里的数据，查询不连续的物品
#define USER_ATTIRE_GET_ATTIRE_EXCEPT_CHEST_CMD	(0x1096 )

/// 家园:得到种子列表
#define USER_GET_SEED_LIST_CMD           	(0x1098 )

//家园:设置种子
#define USER_SET_SEED_EX_CMD             	(0x1099 |NEED_UPDATE)

//班级留言:增加
#define USER_CLASSMSG_ADD_CMD            	(0x109A |NEED_UPDATE)
//班级留言:设置显示标志
#define USER_CLASSMSG_SET_SHOW_FLAG_CMD  	(0x109B |NEED_UPDATE)

//班级留言:得到列表
#define USER_CLASSMSG_GET_LIST_CMD       	(0x109C )

//班级留言:删除
#define USER_CLASSMSG_DEL_CMD            	(0x109D |NEED_UPDATE)
//班级留言:回复
#define USER_CLASSMSG_REPORT_CMD         	(0x109E |NEED_UPDATE)

//班级:得到随机的前10个
#define USER_CLASS_GET_RANDOM_TOP_CMD    	(0x109F )

//得到家园访问列表
#define USER_GET_ACCESS_JYLIST_CMD       	(0x10A0 )
//访问家园
#define USER_ACCESS_JY_CMD               	(0x10A1 |NEED_UPDATE)


//班级：加入荣誉
#define USER_CLASS_SET_MEDAL_INFO_CMD    	(0x10A2 |NEED_UPDATE)

//重设小屋：在设置小屋类型时调用 
#define USER_RESET_HOME_CMD              	(0x10A3 |NEED_UPDATE)
//得到装扮列表－ex
#define USER_ATTIRE_GET_LIST_EX_CMD      	(0x10A4 )
//增加mole时尚分数
#define USER_MODE_LEVEL_SET_CMD          	(0x10A5 |NEED_UPDATE)
//设置模特穿的衣服
#define USER_ATTIRE_SET_MODE_ATTIRE_CMD  	(0x10A6 |NEED_UPDATE)
//得到模特的衣服
#define USER_ATTIRE_GET_MODE_ATTIRE_CMD  	(0x10A7 )
//把模特的衣服设置为mole的衣服
#define USER_ATTIRE_SET_MODE_TO_MOLE_CMD 	(0x10A8 |NEED_UPDATE)
//把mole的衣服穿到模特身上
#define USER_ATTIRE_SET_CLOTHE_MOLE_TO_MODE_CMD	(0x10A9 |NEED_UPDATE)
//增加模特
#define USER_HOME_ADD_MODE_CMD           	(0x10AA |NEED_UPDATE)
//更新图鉴
//#define USER_PIC_BOOK_UPDATE_CMD         	(0x10AB |NEED_UPDATE)
//得到图鉴
#define USER_PIC_BOOK_GET_CMD            	(0x10AC )
//设置USER_FARM表一个字段
#define USER_FARM_SET_COL_CMD            	(0x10AD |NEED_UPDATE)
//放生昆虫
#define USER_ANIMAL_RELEASE_INSECT_CMD   	(0x10AE |NEED_UPDATE)
//设置动物的标志位
#define USER_ANIMAL_SET_ANIMAL_FLAG_CMD  	(0x10AF |NEED_UPDATE)
//加入黑名单
#define USER_ADD_BLACKUSER_CMD	 			(0x10B1 | NEED_UPDATE)
//删除黑名单
#define USER_DEL_BLACKUSER_CMD 				(0x10B2 | NEED_UPDATE)
//修改牧场信息，WEB页面用
#define USER_FARM_SET_WEB_CMD            	(0x10B7 |NEED_UPDATE)
//得到黑名单列表
#define USER_GET_BLACKUSER_LIST_CMD		 	(0x10B3 )
//得到牧场信息,WEB页面用
#define USER_FARM_GET_WEB_CMD            	(0x10B8 )
//更新任务
#define USER_TASK_SET_CMD                   (0x10B9 |NEED_UPDATE)
//删除任务
#define USER_TASK_DEL_CMD                   (0x10BA |NEED_UPDATE)
//得到任务
#define USER_TASK_GET_CMD                   (0x10BB )

//牧场：删除动物
#define USER_FARM_ANIMAIL_DEL_EX_CMD     	(0x10BC |NEED_UPDATE)

//任务:得到列表
#define USER_TASK_GET_LIST_CMD           	(0x10BD )
//设置班级荣誉
#define USER_CLASS_SET_MEDAL_LIST_CMD           (0x10BE )
//编辑牧场
#define  USER_FARM_SET_ATTIRELIST_CMD  		(0x10BF | NEED_UPDATE)
//得到所在群列表:groupidlist 的个数为count
#define USER_GET_GROUP_CMD           	(0x10C0 )
//加入群
#define USER_JOIN_GROUP_CMD          	(0x10C1 |NEED_UPDATE)

//离开群
#define USER_LEAVE_GROUP_CMD         	(0x10C2 |NEED_UPDATE)
//得到其他人到本牧场领取的羊的数目
#define USER_FARM_GET_SHEEP_NUM_CMD      	(0x10C3 )
//离开群 
#define USER_DEL_GROUP_CMD           	(0x10C4 |NEED_UPDATE)

//设置群的个人设定
#define USER_SET_GROUP_FLAG_CMD      	(0x10C5 |NEED_UPDATE)


//牧场:得到数据
#define USER_GET_FARM_CMD               (0x10C6 |NEED_UPDATE)
//牧场：编辑
#define USER_FARM_EDIT_CMD              (0x10C7 |NEED_UPDATE)
//捕获动物
#define USER_DEL_ANIMAL_CMD             (0x10C8 |NEED_UPDATE)
//增加饲料
#define USER_FARM_FEEDSTUFF_ADD_CMD     (0x10C9 |NEED_UPDATE)
//从仓库中把动物放置到农场
#define USER_FARM_PUT_ANIMAL_CMD        (0x10CA |NEED_UPDATE)
//更新水池的状态
#define USER_FARM_WATER_CMD             (0x10CB |NEED_UPDATE)
//得到鱼的状态
#define USER_FARM_GET_FISH_CMD           (0x10CC)
//增加农场的访问列表
#define USER_FARM_SET_ACCESTLIST_CMD     (0x10CD |NEED_UPDATE)
//得到农场的访问列表
#define USER_FARM_GET_ACCESTLIST_CMD     (0x10CE )
#define USER_FARM_GET_ALL_ANIMAL_INFO_CMD (0x10CF)
//设置宠物FLAG标志
#define USER_PET_SET_FLAG_CMD           (0x10D0 |NEED_UPDATE)
//设置宠物FLAG2标志
#define USER_PET_SET_FLAG2_CMD          (0x10D1 |NEED_UPDATE)
//设置宠物生日
#define USER_PET_SET_BIRTHDAY_CMD       (0x10D2 |NEED_UPDATE)
//设置宠物名称
#define USER_PET_SET_NICK_CMD           (0x10D3 |NEED_UPDATE)
//设置宠物颜色
#define USER_PET_SET_COLOR_CMD          (0x10D4 |NEED_UPDATE)
//设置生病时间
#define USER_PET_SET_SICKTIME_CMD       (0x10D5 |NEED_UPDATE)
//设置终止时间
#define USER_PET_SET_ENDTIME_CMD        (0x10D6 |NEED_UPDATE)
//设置心情等
#define USER_PET_SET_LIFE_CMD           (0x10D7 |NEED_UPDATE)
//喂养宠物
#define USER_PET_FEED_PET_CMD           (0x10D8 |NEED_UPDATE)
//托管/领会宠物
#define USER_PET_RETRIEVE_PET_CMD       (0x10D9 |NEED_UPDATE)
//接宠物任务
#define USER_PET_GET_PET_CMD            (0x10DA |NEED_UPDATE)
//宠物出游
#define USER_PET_TRAVEL_PET_CMD         (0x10DB |NEED_UPDATE)
#define USER_FARM_UPDATE_ANIMAL_INFO_CMD (0x10DC |NEED_UPDATE)

//牧场：拉取未使用的装扮和动物幼仔列表
#define USER_FARM_GET_NOUSEDLIST_CMD     	(0x10DE )

//牧场：动物增加
#define USER_FARM_ANIMAL_ADD_CMD         	(0x10DF |NEED_UPDATE)
//班级：创建
#define USER_CLASS_CREATE_CMD            	(0x10F0 |NEED_UPDATE)
//班级：得到信息
#define USER_CLASS_GET_INFO_CMD          	(0x10F1 )

//班级：修改信息
#define USER_CLASS_CHANGE_INFO_CMD       	(0x10F2 |NEED_UPDATE)

//班级：增加成员
#define USER_CLASS_ADD_MEMBER_CMD        	(0x10F3 |NEED_UPDATE)
//班级：删除成员
#define USER_CLASS_DEL_MEMBER_CMD        	(0x10F4 |NEED_UPDATE)
//班级：添加物品
#define USER_CLASS_ADD_ATTIRE_CMD        	(0x10F5 |NEED_UPDATE)


//班级：得到徽章信息
#define USER_CLASS_GET_FLAG_INFO_CMD     	(0x10F6 )

//班级：编辑小屋
#define USER_CLASS_EDIT_HOME_CMD         	(0x10F7 |NEED_UPDATE)

//班级：删除
#define USER_CLASS_DEL_CMD               	(0x10F8 |NEED_UPDATE)

//班级：设置访问标志
#define USER_CLASS_SET_ACCESS_FLAG_CMD   	(0x10F9 |NEED_UPDATE)

//班级：得到未使用列表
#define USER_CLASS_GET_ATTIRELIST_CMD    	(0x10FA )

//班级：减少未使用物品
#define USER_CLASS_REDUCE_ATTIRE_CMD     	(0x10FB |NEED_UPDATE)

//班级：得到简单信息
#define USER_CLASS_GET_SIMPLE_INFO_CMD   	(0x10FC )

//班级：得到首班级ID
#define USER_GET_FIRST_CLASS_CMD         	(0x10FD )

//班级：得到成员列表
#define USER_CLASS_GET_MEMBERLIST_CMD    	(0x10FE )
//得到班级荣誉榜
#define USER_CLASS_GET_MEDAL_LIST_CMD           (0x10FF )

/*//payser */
/*#define PAY_BUY_ATTIRE_BY_DAMEE_CMD 	(0x3001 | NEED_UPDATE)*/
/*#define PAY_INIT_USER_CMD 				(0x3002 | NEED_UPDATE)*/
/*//序列号充值*/
/*#define PAY_ADD_DAMEE_BY_SERIAL_CMD 	(0x3003 | NEED_UPDATE)*/

/*//用damee 包月*/
/*#define PAY_MONTH_BY_DAMEE_CMD 			(0x3004 | NEED_UPDATE)*/

/*#define PAY_CHANGE_DAMEE_CMD 			(0x3005 | NEED_UPDATE)*/

/*//处理包月续期返回的值*/
/*#define PAY_SET_AUTO_MONTH_DEALMSG_CMD 	(0x3008 | NEED_UPDATE)*/
/*//包月续期记录批量产生 //每月产生*/
/*#define PAY_AUTO_MONTH_RECORD_GEN_CMD 	(0x3009 | NEED_UPDATE)*/
/*//设置不续期*/
/*#define PAY_SET_NO_AUTO_MONTH_CMD 		(0x300B | NEED_UPDATE)*/

/*//得到包月信息*/
/*#define PAY_GET_PAY_INFO_CMD 			(0x300C)*/




/*//产生一条预交易记录*/
/*#define PAY_PAY_PRE_CMD 				(0x3017 | NEED_UPDATE)*/
/*//将预交易确认，并且加上相应的Damee*/
/*#define PAY_PAY_TRUE_CMD				(0x3018 | NEED_UPDATE)*/
/*#define PAY_PAY_ONCE_CMD				(0x3019 | NEED_UPDATE)*/
/*//得到大米使用信息*/
/*#define PAY_GET_DAMEE_LIST_CMD 			(0x301B )*/

/*#define PAY_DEL_MONTH_CMD 				(0x3020 | NEED_UPDATE)*/

//更新农场的状态
#define USER_FARM_SET_STATE_CMD                 (0x3001 |NEED_UPDATE)
//设置绵羊带出或者放回标志
#define USER_FARM_SET_OUTGO_CMD          	(0x3002 |NEED_UPDATE)
//设置动物的吃食时间
#define USER_FARM_SET_EATTIME_CMD        	(0x3003 |NEED_UPDATE)
//用网捕鱼
#define USER_FARM_NET_CATCH_FISH_CMD     	(0x3004 |NEED_UPDATE)
//买渔网
#define USER_FARM_SET_NET_CMD            	(0x3005 |NEED_UPDATE)
//用户渔网情况
#define USER_FARM_GET_NET_CMD            	(0x3006 )
//更新NPC的好感度，WEB用
#define USER_NPC_UPDATE_WEB_CMD          	(0x3007 |NEED_UPDATE)
// 查找卡牌
#define USER_SWAP_CARD_SEARCH_CMD            (0x3008 )
//设置某个卡牌与另一个卡牌交换
#define USER_SWAP_CARD_SET_SWAP_CMD                  (0x3009 |NEED_UPDATE)
//交换卡牌
#define USER_SWAP_CARD_SWAP_CMD                      (0x300A |NEED_UPDATE)
//兑换卡牌
#define USER_SWAP_CARD_GET_SUPER_CMD                 (0x300B |NEED_UPDATE)
//得到卡牌信息
#define USER_SWAP_CARD_GET_CARD_CMD                  (0x300C )
//设置卡牌
#define USER_SWAP_CARD_SET_CARD_CMD             (0x300D |NEED_UPDATE)
//改变NPC的好感度
#define USER_NPC_CHANGE_CMD              	(0x300E |NEED_UPDATE)
//得到NPC的信息
#define USER_NPC_GET_WEB_CMD             	(0x300F )
//设置用户封停信息
#define USER_SET_PUNISH_FLAG_CMD                (0x3010 |NEED_UPDATE)

//编辑拉姆日记信息
#define USER_LAMU_DIARY_EDIT_CMD              (0x3011 |NEED_UPDATE)
//得到拉姆日志的目录信息
#define USER_LAMU_CATALOG_GET_CMD               (0x3012 )
//删除日志
#define USER_LAMU_DIARY_DELETE_CMD              (0x3013 |NEED_UPDATE)
//更新锁的状态
#define USER_LAMU_DIARY_STATE_UPDATE_CMD        (0x3014 |NEED_UPDATE)
//投花
#define USER_LAMU_DIARY_FLOWER_UPDATE_CMD       (0x3015 |NEED_UPDATE)
//得到日记的内容
#define USER_LAMU_DIARY_CONTENT_GET_CMD         (0x3016 )
//设置日志的锁
#define USER_EX_SET_DIARY_LOCK_CMD              (0x3017 |NEED_UPDATE)
//得到锁
#define USER_EX_DIARY_LOCK_GET_CMD              (0x3018 )
//得到一个用户的日记的总数
#define USER_LAMU_DIARY_COUNT_CMD               (0x3019 )
//寄养兔子
#define USER_FARM_RABBIT_FOSTER_CMD      	(0x301A |NEED_UPDATE)
//领走寄养的兔子
#define USER_FARM_RABBIT_DEL_CMD         	(0x301B )
//得到家园和牧场的等级
#define USER_FARM_GET_PLANT_BREED_CMD    	(0x301C )
//剪羊毛
#define USER_FARM_ANIMAL_GET_WOOL_CMD    	(0x301D |NEED_UPDATE)
//设置拍照时的衣服
#define USER_EX_UPDATE_PIC_CLOTHE_CMD    	(0x301E |NEED_UPDATE)
//得到拍照时的衣服
#define USER_EX_GET_PIC_CLOTHE_CMD       	(0x301F )
//插入一条汽车的记录
#define USER_AUTO_INSERT_CMD             	(0x3020 |NEED_UPDATE)
//更换引擎或者颜色
#define USER_AUTO_CHANGE_ATTIRE_CMD      	(0x3021 |NEED_UPDATE)
//增加道具孔，增加道具，去除道具，交换道具
#define USER_AUTO_CHANGE_PROPERY_CMD     	(0x3022 |NEED_UPDATE)
//得到用户的全部信息
#define USER_AUTO_GET_ALL_CMD            	(0x3023 )
//给汽车加油
#define USER_AUTO_ADD_OIL_CMD            	(0x3024 |NEED_UPDATE)
//得到某个汽车的信息 
#define USER_AUTO_GET_ONE_INFO_CMD       	(0x3025 )
//设置驾照的获取时间
#define USER_EX_SET_DRIVER_TIME_CMD    	(0x3026 |NEED_UPDATE)
//设置展示的车
#define USER_EX_SET_SHOW_AUTO_CMD        	(0x3027 |NEED_UPDATE)
//得到展示车的信息
#define USER_EX_GET_SHOW_AUTO_CMD        	(0x3028 )
//设置熟练程度
#define USER_EX_SET_AUTO_SKILL_CMD       	(0x3029 |NEED_UPDATE)
//更改装饰
#define USER_AUTO_CHANGE_ADRON_CMD       	(0x302A |NEED_UPDATE)
//得到某个种类汽车的数量
#define USER_AUTO_GET_ONE_KIND_NUM_CMD   	(0x302B )
//删除某个汽车
#define USER_AUTO_DEL_CMD                	(0x302C |NEED_UPDATE)
//得到某个用户汽车的全部信息(web用)
#define USER_AUTO_GET_ALL_WEB_CMD        	(0x302D)
//更新一条汽车的信息
#define USER_AUTO_SET_AUTO_WEB_CMD       	(0x302E |NEED_UPDATE)
//蝴蝶给植物授粉
#define USER_JY_BUTTERFLY_POLLINATE_FLOWER_CMD 	(0x302F |NEED_UPDATE)
//放入蛋去孵
#define USER_EGG_INSERT_CMD              	(0x3030 |NEED_UPDATE)
//孵蛋 
#define USER_EGG_FUDAN_CMD               	(0x3031 |NEED_UPDATE)
//获取孵蛋的信息
#define USER_EGG_GET_INFO_CMD            	(0x3032 )
//接任务
#define USER_NPC_TASK_RECV_CMD           	(0x3033 |NEED_UPDATE)
//完成任务
#define USER_NPC_TASK_FINISH_CMD         	(0x3034 |NEED_UPDATE)
//得到客户端保存的数据
#define USER_NPC_TASK_GET_BUF_CMD        	(0x3035 )
//删除一个NPC任务
#define USER_NPC_TASK_DELETE_NPC_TASK_CMD 	(0x303F | NEED_UPDATE)
//设置客户端保存的数据
#define USER_NPC_TASK_SET_BUF_CMD        	(0x3036 |NEED_UPDATE)
//得到某个任务的状态
#define USER_NPC_ONE_TASK_GET_INFO_CMD   	(0x3037 )
//得到某个NPC的任务
#define USER_NPC_TASK_GET_NPC_TASK_CMD   	(0x3038 )
//蛋孵出
#define USER_EGG_BROOD_EGG_CMD           	(0x3039 |NEED_UPDATE)
//得到用户孵蛋的全部信息，WEB页面用
#define USER_EGG_GET_ALL_WEB_CMD         	(0x303A )
//更新孵蛋信息，WEB页面用
#define USER_EGG_UPDATE_WEB_CMD          	(0x303B |NEED_UPDATE)
//删除某个范围内的装扮
#define USER_ATTIRE_DELETE_ATTIRE_LIST_CMD	(0x303D |NEED_UPDATE)
//得到用户包月的月数
#define USER_GET_VIP_MONTH_CMD           	(0x303E )
//蝴蝶授粉
#define USER_ANIMAL_POLLINATE_CMD 	(0x303C |NEED_UPDATE)


//email db
//通过邮箱名得到userid
#define GET_USERID_BY_EMAIL_CMD		 	(0x4004)
#define MAP_EMAIL_USERID_CMD	 		(0x4005 | NEED_UPDATE)
#define DEL_MAP_EMAIL_USERID_CMD	 	(0x4006 | NEED_UPDATE)
#define EMAIL_SET_EMAIL_CMD 			(0x4007 | NEED_UPDATE)

//检查邮箱名是否已经存在
#define EMAIL_CHECK_EMAIL_IS_EXISTED_CMD 	(0x4008 )

//------------------------
//game score list

#define GET_GAME_SCORE_LIST_CMD	 		(0x5001)
#define SET_GAME_SCORE_CMD	 			(0x5002 | NEED_UPDATE)
#define INIT_GAME_SCORE_LIST_CMD	 	(0x5003 | NEED_UPDATE)
//设置分数
#define GAME_SCORE_SET_SCORE_CMD		(0x5004 | NEED_UPDATE)

//serial db
#define SET_SERIAL_USED_CMD	 			(0x6001 | NEED_UPDATE)
#define SET_SERIAL_UNUSED_CMD	 		(0x6002 | NEED_UPDATE)
#define SERIAL_GET_NOUSED_ID_CMD	 	(0x6003 | NEED_UPDATE)
//online...
//将序列号设置为使用
#define SERIAL_SET_SERIAL_STR_USED_CMD 	(0x6004 | NEED_UPDATE)
#define SERIAL_GET_INFO_SERIAL_STR_CMD	(0x6005 )

//msg board db
#define MSGBOARD_ADD_MSG_CMD	 		(0x7001 | NEED_UPDATE)
#define MSGBOARD_GET_MSGLIST_CMD	 	(0x7002)
#define MSGBOARD_GETMSG_BY_MSGID_CMD	(0x7003)
#define MSGBOARD_SET_MSG_FLAG_CMD		(0x7004 | NEED_UPDATE)
#define MSGBOARD_ADD_HOT_CMD			(0x7005 | NEED_UPDATE)
#define MSGBOARD_GET_LIST_WITH_FLAG_CMD	(0x7006)
#define MSGBOARD_CLEAR_BY_DEL_FLAG_CMD	(0x7007 | NEED_UPDATE)
//通过米米号得到审核过的列表
#define MSGBOARD_GET_MSGLIST_USERID_CMD    (0x7008 )
#define MSGBOARD_GET_MSG_NOCHECK_CMD       (0x7009 )
#define MSGBOARD_RESET_MSG_CMD           	(0x7010 |NEED_UPDATE)

//蘑菇向导: 添加 QA
#define MSGBOARD_QA_ADD_MSG_CMD          	(0x7020 |NEED_UPDATE)
//蘑菇向导: 得到 QA列表<br/>注意事项:问题241字节=米米号(12)+nick(16)+问题(213)
#define MSGBOARD_QA_GET_LIST_CMD         	(0x7021 )
//蘑菇向导: 删除 QA
#define MSGBOARD_QA_DEL_MSG_CMD          	(0x7022 |NEED_UPDATE)



//蘑菇向导: 得到 QA列表 - ex
#define MSGBOARD_QA_GET_LIST_EX_CMD      	(0x7023 )

//蘑菇向导: 得到 QA的总个数
#define MSGBOARD_QA_GET_COUNT_CMD        	(0x7024 )

//GAME
#define GAME_SET_SCORE_CMD               	(0x8000 |NEED_UPDATE)
//GAME:得到好友的某一游戏成绩列表
#define GAME_USER_GET_LIST_CMD           	(0x8001 )
//GAME:得到自己的所有游戏的列表
#define GAME_GET_LIST_CMD                	(0x8002 )
//GAME:设置PK信息
#define GAME_SET_PK_INFO_CMD             	(0x8003 |NEED_UPDATE)

//GAME:得到PK信息
#define GAME_GET_PK_INFO_CMD             	(0x8004 )

//用户报告信息
#define USERMSG_SET_REPORT_MSG_CMD	 	(0x9001 | NEED_UPDATE)
//用户投稿 <br /> USERMSG_ADD_WRITING_IN_ITEM.count=msglen
#define USERMSG_ADD_WRITING_CMD	 		(0x9002 | NEED_UPDATE)
#define USERMSG_GET_WRITING_CMD	 		(0x9003 )
#define USERMSG_GET_REPORT_BY_DATE_CMD	(0x9004 )
#define USERMSG_GET_REPORT_BY_DATE_USERID_CMD (0x9005 )
#define USERMSG_SET_DEL_WRITING_CMD 	(0x9006 |NEED_UPDATE)
#define USERMSG_SET_WRITING_REPORT_CMD	(0x9007 |NEED_UPDATE)
#define USERMSG_CLEAR_WRITING_CMD 		(0x9008 |NEED_UPDATE)
#define USERMSG_GET_WRITING_COUNT_CMD	(0x9009)
#define USERMSG_ADD_REPORT_DEAL_CMD		(0x900A |NEED_UPDATE)
#define USERMSG_CLEAR_BY_DATE_USERID_CMD (0x900B  | NEED_UPDATE )

#define USERMSG_GET_DEAL_REPORT_LIST_CMD (0x900C)
#define USERMSG_GET_WRITING_LIST_CMD 	(0x900D)

//超级管理员
#define SU_LOGIN_CMD 					(0xA001)
#define SU_CHANGE_CMD 					(0xA002| NEED_UPDATE)
#define SU_CHANGE_PASSWD_CMD 			(0xA003| NEED_UPDATE)
#define SU_ADD_MSG_ATTIME_CMD 			(0xA004| NEED_UPDATE)
#define SU_GET_MSG_ATTIME_ALL_CMD 		(0xA005)
#define SU_DEL_MSG_ATTIME_CMD 			(0xA006| NEED_UPDATE)
#define SU_GET_MSG_ATTIME_BY_DATE_CMD 	(0xA007)
#define SU_GET_ADMIN_LIST_CMD 			(0xA008)
#define SU_ADD_ADMIN_CMD 				(0xA009 | NEED_UPDATE)
#define SU_DEL_ADMIN_CMD 				(0xA00B | NEED_UPDATE)
#define SU_GET_ADMINID_BY_NICK_CMD 		(0xA00C)
//客服修改记录
#define SU_CHANGELIST_ADD_CMD            	(0xA010 |NEED_UPDATE)

//论坛同步:注册
#define DV_REGISTER_CMD 				(0xB001 |NEED_UPDATE)
//论坛同步:修改密码
#define DV_CHANGE_PASSWD_CMD 			(0xB002| NEED_UPDATE)
//论坛同步:修改昵称
#define DV_CHANGE_NICK_CMD 				(0xB003| NEED_UPDATE)

//SYSARG 
#define SYSARG_DAY_ADD_CMD				(0xC001| NEED_UPDATE)
#define SYSARG_DAY_QUERY_CMD			(0xC002)
//清除所有的任务限制
#define SYSARG_DAY_CLEAR_CMD            (0xC004 |NEED_UPDATE)
//清除某一类型的任务限制
#define SYSARG_DAY_CLEAN_BY_TYPE_CMD	(0xC005 | NEED_UPDATE)

//得到每天每周的任务信息
#define SYSARG_DAY_GET_LIST_BY_USERID_CMD	(0xC006 )

//设置每天每周的任务信息
#define SYSARG_DAY_SET_CMD               	(0xC007 |NEED_UPDATE)
//更新队伍的分数
//#define SYS_RALLY_UPDATE_SCORE_CMD       	(0xC008 |NEED_UPDATE)
//更新赛道的时间
//#define SYS_RALLY_UPDATE_RACE_CMD        	(0xC009 |NEED_UPDATE)
//得到各个赛道的时间值
//#define SYS_GET_TEAM_RACE_LIST_CMD       	(0xC00A )
//得到各个队的分数 
//#define SYS_GET_TEAM_SCORE_LIST_CMD      	(0xC00B )
//得到每天任务的列表
#define SYSARG_DAY_GET_LIST_CMD          	(0xC00C )
//更新动物领取的数目
#define SYSARG_DB_UPDATE_ANIMAL_LIMIT_CMD	(0xC00D |NEED_UPDATE)
//得到动物的领取数目
#define SYSARG_GET_ANIMAL_LIMIT_CMD      	(0xC00E )
//得到当前谁的鱼量最重
#define SYSARG_ANIMAL_GET_MAX_WEIGHT_FISH_CMD	(0xC00F |NEED_UPDATE)
#define SYSARG_GET_COUNT_CMD 			(0xC011)
#define SYSARG_SET_COUNT_CMD 			(0xC012| NEED_UPDATE)
#define SYSARG_GET_COUNT_LIST_CMD 		(0xC013)

//得到调查的数据*/
#define SYSARG_GET_QUESTIONNAIRE_LIST_CMD	(0xC014 )
//插入事件
#define SYSARG_DB_MSG_EDIT_CMD           	(0xC015 |NEED_UPDATE)
//删除事件
#define SYSARG_DB_MSG_DEL_CMD            	(0xC016 |NEED_UPDATE)
//得到事件
#define SYSARG_DB_MSG_GET_CMD            	(0xC017 )
//得到全部信息,WEB用
#define SYSARG_DB_MSG_GET_ALL_WEB_CMD    	(0xC018 )
/*//得到哪个队*/
/*#define SYSARG_GET_TEAM_CMD                  (0xC015 | NEED_UPDATE)*/
/*//得到NPC位置*/
/*#define SYSARG_GET_FIRE_CUP_CMD                 (0xC016 )*/
/*//增加奖牌数*/
/*#define SYS_ADD_TEAM_MEDAL_CMD                 (0xC017 | NEED_UPDATE)*/
/*//拉取排行榜*/
/*#define SYS_GET_ALL_MEDAL_LIST_CMD              (0xC018)*/

//用户投票(调查)
#define SYSARG_ADD_QUESTIONNAIRE_CMD     	(0xC020 |NEED_UPDATE)


//每月任务:得到任务列表
#define SYSARG_MONTH_TASK_GET_LIST_CMD   	(0xC030 )

//每月任务:增加任务的值
#define SYSARG_MONTH_TASK_ADD_CMD        	(0xC031 |NEED_UPDATE)

//TEMP,活动
#define CUP_ADD_CUP_CMD					(0xD001| NEED_UPDATE)
#define CUP_GET_CUP_CMD					(0xD002)
#define CUP_GET_LAST_USERLIST_CMD		(0xD003)
#define CUP_GET_GROUP_MSG_CMD			(0xD004)
#define CUP_GET_GROUP_HISTORY_CMD 		(0xD005)

#define CONFECT_CHANGE_VALUE_CMD        (0xD006 |NEED_UPDATE)
#define CONFECT_GET_VALUE_CMD           (0xD007 )
#define CONFECT_SET_TASK_CMD            (0xD008 |NEED_UPDATE)
#define CONFECT_SET_VALUE_CMD            	(0xD009 |NEED_UPDATE)
#define TEMPGS_GET_VALUE_CMD             	(0xD010 )
#define TEMPGS_SET_TASK_CMD              	(0xD011 |NEED_UPDATE)
#define TEMPGS_SET_COUNT_CMD              	(0xD012 |NEED_UPDATE)

//圣诞:加入祝福
#define CHRISTMAS_ADD_MSG_CMD            	(0xD020 |NEED_UPDATE)

//圣诞:得到祝福
#define CHRISTMAS_GET_MSG_CMD            	(0xD021 |NEED_UPDATE)

//圣诞:是否已经得到祝福
#define CHRISTMAS_IS_GET_MSG_CMD         	(0xD022 )

//圣诞:是否加入祝福
#define CHRISTMAS_IS_ADD_MSG_CMD         	(0xD023 )

//产生随机码
#define USERSERIAL_GEN_SERIAL_CMD        	(0xD030 |NEED_UPDATE)
//删除随机码
#define USERSERIAL_DEL_SERIAL_CMD        	(0xD031 |NEED_UPDATE)
//查询随机码
#define USERSERIAL_GET_SERIAL_CMD        	(0xD032 )

//春节:得到相关的值
#define SPRING_GET_VALUE_CMD             	(0xD040 )


//春节:调整金银元宝
#define SPRING_CHANGE_VALUE_CMD          	(0xD041 |NEED_UPDATE)

//春节:设置春联
#define SPRING_SET_MSG_CMD               	(0xD042 |NEED_UPDATE)
//春节:得到春联
#define SPRING_GET_MSG_CMD               	(0xD043 )

//春节:初始化金银元宝
#define SPRING_INIT_GOLD_SILVER_CMD       	(0xD044 |NEED_UPDATE)


//春节:得到标志位
#define SPRING_GET_FLAG_CMD              	(0xD045 )

//春节:得到相关的值
#define SPRING_GET_SPRING_CMD            	(0xD046 )

//春节:设置相关的值
#define SPRING_SET_SPRING_CMD            	(0xD047 |NEED_UPDATE)

//拉姆运动会:设置拉姆成绩,并为所在组的完成数加1
#define TEMP_PET_SPORTS_SET_SCORE_CMD 		(0xD048 |NEED_UPDATE)

//拉姆运动会:得到拉姆最高成绩
#define TEMP_PET_SPORTS_GET_MAX_SCORE_CMD 	(0xD049 )

//拉姆运动会:得到某一队的总完成数
#define TEMP_PET_GET_GROUP_ALLCOUNT_CMD 	(0xD04A )

//拉姆运动会:得到各队完成数据列表
#define TEMP_PET_SPORTS_GET_LIST_CMD     	(0xD04B )

//拉姆运动会:得到用户是否有拉姆报过名了
#define TEMP_PET_USER_REPORTED_CMD     		(0xD04C )

//拉姆运动会:得到用户的拉姆报名列表
#define TEMP_PET_GET_PET_LIST_CMD        	(0xD04D )

//拉姆运动会:拉姆报名
#define TEMP_PET_REPORT_CMD   	     		(0xD04E |NEED_UPDATE)

//拉姆运动会:拉姆成绩列表
#define TEMP_PET_GET_SCORE_LIST_CMD   	 	(0xD04F )






//EMAILSYS
//明信片:增加
#define EMAILSYS_ADD_EMAIL_CMD 			(0xE001| NEED_UPDATE)
//明信片:得到
#define EMAILSYS_GET_EMAIL_CMD 			(0xE002| NEED_UPDATE)
#define EMAILSYS_GET_EMAILID_LIST_CMD 	(0xE003)
#define EMAILSYS_DEL_EMAIL_CMD 			(0xE004| NEED_UPDATE)
#define EMAILSYS_GET_EMAIL_MSG_CMD         (0xE005 )
#define EMAILSYS_CLEAN_EMAIL_CMD         	(0xE006 |NEED_UPDATE)

//HOPE 许愿
#define HOPE_ADD_HOPE_CMD 				(0x1401| NEED_UPDATE)
#define HOPE_CHECK_HOPE_CMD 			(0x1402 )
#define HOPE_SET_HOPE_USED_CMD 			(0x1403| NEED_UPDATE)
#define HOPE_GET_HOPE_LIST_CMD 			(0x1404 )

#define HOPE_GET_HOPE_LIST_BY_DATE_CMD 	(0x1405 )
//得到用户的许愿数据
#define HOPE_GET_HOPE_ALL_CMD            	(0x1406 )

//设置许愿日期和时间
#define HOPE_SET_HOPE_EX_CMD             	(0x1407 |NEED_UPDATE)

//产品管理
#define PRODUCE_ADD_ATTIRE_BY_DAMEE_CMD (0x2001| NEED_UPDATE)

//ADMIN
//加入一个新管理员
#define ADMIN_ADD_ADMIN_CMD              	(0x5401| NEED_UPDATE)

//得到管理员列表
#define ADMIN_GET_ADMINLIST_CMD          	(0x5402 )

//修改管理员密码
#define ADMIN_CHANGE_PASSWD_CMD          	(0x5403| NEED_UPDATE)

//删除管理员
#define ADMIN_DEL_ADMIN_CMD              	(0x5404 |NEED_UPDATE)

//管理员登入
#define ADMIN_LOGIN_CMD                  	(0x5405 )

//得到权限列表
#define ADMIN_GET_POWERLIST_CMD          	(0x5406 )

//加权限
#define ADMIN_ADD_POWER_CMD              	(0x5407 |NEED_UPDATE)
//减权限
#define ADMIN_DEL_POWER_CMD              	(0x5408 |NEED_UPDATE)

//得到管理员ID
#define ADMIN_GET_ADMINID_BY_NICK_CMD       (0x5409 )

//通过权限得到ADMINID列表
#define ADMIN_GET_ADMINLIST_BY_POWERID_CMD	(0x5410 )

//超级用户修改密码
#define ADMIN_ROOT_CHANGE_PASSWD_CMD     	(0x5409 |NEED_UPDATE)

//设置用户使用状态
#define ADMIN_SET_USE_FLAG_CMD           	(0x5410 |NEED_UPDATE)

//添加派对
#define PARTY_ADD_PARTY_CMD              	(0x6401 |NEED_UPDATE)
//得到当天派对列表<br /> 每页50个派对
#define PARTY_GET_PARTYLIST_CMD          	(0x6402 )

//得到个人派对列表
#define PARTY_GETLIST_BY_OWNERID_CMD     	(0x6403 )

//得到服务器派对个数
#define PARTY_GET_COUNT_BY_SERVER_CMD    	(0x6404 )

//SALE
#define SALE_ADD_REQUEST_CMD 			(0xA401| NEED_UPDATE)

//小屋数据库

#define ROOMINFO_GET_HOT_CMD             	(0xB401 )
//增加或者减少圣诞节的礼物数目 
#define ROOMINFO_CHANGE_GIFT_NUM_CMD     	(0xB402 |NEED_UPDATE)
#define ROOMINFO_VOTE_ROOM_CMD           	(0xB403 |NEED_UPDATE)
#define ROOMINFO_PUG_ADD_CMD             	(0xB404 |NEED_UPDATE)
#define ROOMINFO_PUG_LIST_CMD            	(0xB405 )
//小屋 增加HOT值
#define ROOMINFO_ADD_HOT_CMD             	(0xB406 |NEED_UPDATE)
//增加被抓的糖果数量
//#define ROOMINFO_ADD_CATCH_CANDY_CMD     	(0xB407 |NEED_UPDATE)
//得到为班级贡献的分数和糖果数量 
#define ROOMINFO_GET_SCORE_CANDY_CMD     	(0xB408 )
#define ROOMINFO_INIT_MUD_CMD            	(0xB409 |NEED_UPDATE)
//改变礼物的数目
#define ROOMINFO_CHANGE_PRESENT_CMD          (0xB40A |NEED_UPDATE)
//得到礼物的数目
#define ROOMINFO_GET_PRESENT_NUM_CMD            (0xB40B )
//更改花的数量
#define ROOMINFO_FLOWER_UPDATE_CMD       	(0xB40C |NEED_UPDATE)
//得到花的数量
#define ROOMINFO_FLOWER_GET_CMD          	(0xB40D )
//增加糖果的数目
//#define ROOMINFO_ADD_CANDY_CMD               (0xB40C |NEED_UPDATE)
//得到糖果的数目
//#define ROOMINFO_GET_CANDY_CMD               (0xB40D)
//增加为班级贡献的分数
//#define ROOMINFO_ADD_CLASS_SCORE_CMD               (0xB40E |NEED_UPDATE)
//得到为班级贡献的分数
//#define ROOMINFO_GET_CLASS_SCORE_CMD               (0xB40F)
//设置用户是否拿过袜子
#define ROOMINFO_GET_SOCKES_CMD          	(0xB40F |NEED_UPDATE)

#define ROOMINFO_INIT_USER_CMD           	(0xB410 |NEED_UPDATE)


//小屋：清除一个泥巴
#define ROOMINFO_DEL_MUD_CMD             	(0xB411 |NEED_UPDATE)

#define ROOMINFO_SET_INFO_CMD            	(0xB412 |NEED_UPDATE)
//记录谁送礼物
#define ROOMINFO_RECORD_GIVE_GIFT_USERID_CMD	(0xB413 |NEED_UPDATE)
//获取谁送的礼物 
#define ROOMINFO_GET_GIVE_GIFT_USERID_CMD	(0xB414 )
//还有多少礼物可以送 
#define ROOMINFO_GET_GIFT_NUM_CMD        	(0xB415 )
//炫风拉力赛报名
//#define ROOMINFO_RALLY_SIGN_CMD          	(0xB413 |NEED_UPDATE)
//增加旋风拉力赛个人分数
//#define ROOMINFO_RALLY_ADD_SCORE_CMD     	(0xB414 |NEED_UPDATE)
//得到个人的分数 
//#define ROOMINFO_RALLY_GET_SCORE_CMD     	(0xB415 )
//得到参加各个赛道的次数
//#define ROOMINFO_RALLY_GET_RACE_INFO_CMD 	(0xB416 )
//得到当天的分数
//#define ROOMINFO_RALLY_GET_TODAY_SCORE_CMD	(0xB417 )
//每个赛道的次数减一 
//#define ROOMINFO_RALLY_DESC_RACE_CMD     	(0xB418 |NEED_UPDATE)
//得到用户的队名
//#define ROOMINFO_RALLY_GET_TEAM_CMD      	(0xB419 )
//增加徽章数
#define ROOMINFO_RALLY_ADD_MEDAL_CMD     	(0xB41A |NEED_UPDATE)
//得到合成列表
//#define ROOMINFO_GET_MERGE_CMD           	(0xB41B )
//更新合成列表
#define ROOMINFO_MERGE_UPDATE_CMD        	(0xB41C |NEED_UPDATE)
//设置用户已经领取过某个合成品
#define ROOMINFO_MERGE_SET_GET_CMD       	(0xB41D |NEED_UPDATE)
//检查是否有糖果未领取
#define ROOMINFO_CHECK_CANDY_CMD         	(0xB41E )
//得到用户拉力赛的各个字段的值
//#define ROOMINFO_RALLY_GET_USER_ALL_INFO_WEB_CMD	(0xB41F )
//火神杯报名
#define ROOMINFO_SIG_FIRE_CUP_CMD            (0xB420 |NEED_UPDATE)
//得到参加的队名
#define ROOMINFO_GET_FIRE_CUP_TEAM_CMD       (0xB421 )	
//更新奖杯数
#define ROOMINFO_FIRE_CUP_UPDATE_CMD         (0xB422 |NEED_UPDATE)
//增加打工的次数
#define ROOMINFO_WORK_SET_CMD                   (0xB423 |NEED_UPDATE)
//得到打工的工资
#define ROOMINFO_WORK_GET_CMD                   (0xB424 )
//得到ROOMINFO里的信息
#define ROOMINFO_GET_USER_ALL_INFO_WEB_CMD	(0xB425 )
//改变糖果的数量
//#define ROOMINFO_CHANGE_CANDY_EX_CMD       	(0xB426 |NEED_UPDATE)
//得到糖果数
//#define ROOMINFO_CHANGE_CANDY_CMD        	(0xB427 )
//设置roominfo表里字段信息
#define ROOMINFO_SET_USER_ALL_INFO_WEB_CMD	(0xB428 |NEED_UPDATE)
//校验一个用户拿的糖果数是否超过上限
//#define ROOMINFO_CHECK_CANDY_LIST_CMD    	(0xB429 |NEED_UPDATE)
//领取图纸订做
#define ROOMINFO_PIC_INSERT_CMD          	(0xB42A |NEED_UPDATE)
//更新心愿
#define ROOMINFO_UPDATE_WISH_CMD    	(0xB42B |NEED_UPDATE)
//得到心愿
#define ROOMINFO_GET_WISH_CMD       	(0xB42C )
//设置答题记录
#define ROOMINFO_QUESTION_SET_CMD           (0xB430 |NEED_UPDATE)
//得到用户的答题记录
#define ROOMINFO_QUESTION_GET_CMD           (0xB431 )
//检查今天是否可以再参加超级拉姆抽取
//#define ROOMINFO_CHECK_IF_GEN_VIP_CMD       (0xB438 )
//按概率生成用户是否可以成为VIP
//#define CROOMINFO_GEN_VIP_CMD               (0xB439 |NEED_UPDATE)
//ROOMINFO_GEN_VIP del
//#define ROOMINFO_GEN_VIP_DEL_CMD         	(0xB43A |NEED_UPDATE)
//得到衣服的损耗度
//#define ROOMINFO_GET_CLOTHE_WASTE_CMD           (0xB43B )
//设置衣服的损耗度
//#define ROOMINFO_SET_CLOTHE_WASTE_CMD           (0xB43C |NEED_UPDATE)
//得到point的值
//#define ROOMINFO_GET_POINT_CMD                  (0xB43D )
//设置point值
//#define ROOMINFO_SET_POINT_CMD                  (0xB43E |NEED_UPDATE)
//得到POS的值
#define ROOMINFO_GET_POS_CMD                    (0xB43F)

//设置POS的值
//
//
//设置家园的着火状态
//#define ROOMINFO_SET_JY_STATE_CMD            (0xB438 | NEED_UPDATE)
//设置家园火势的大小
//#define ROOMINFO_SET_JY_FIRE_CMD             (0xB439 |NEED_UPDATE)
//得到家园火势状况
//#define ROOMINFO_JY_FIRE_GET_CMD              (0xB43A )

//小屋：宝盒：添加 得到者的信息
#define ROOMINFO_BOX_ADD_CMD             	(0xB440 |NEED_UPDATE)
//小屋：宝盒：得到 得到者的信息列表
#define ROOMINFO_BOX_GET_LIST_CMD        	(0xB441 )
//设置POS值
#define ROOMINFO_SET_POS_CMD                (0xB442 |NEED_UPDATE)
//把PO设为0
#define ROOMINFO_CLEAR_POS_CMD              (0xB443 |NEED_UPDATE)
//设置任务的ID号
#define ROOMINFO_SET_TASK_CMD               (0xB444 |NEED_UPDATE)
//得到任务
#define ROOMINFO_GET_TASK_CMD                   (0xB445 )
//设置为谁浇水
#define MMS_SET_WATER_LIST_CMD              (0xB446 |NEED_UPDATE)
//得到浇水人数
#define MMS_GET_WATER_LIST_CMD                  (0xB447 )




//小屋：获得的礼物列表:增加
#define ROOMINFO_PRESENTLIST_ADD_CMD     	(0xB450 |NEED_UPDATE)
//小屋：获得的礼物列表:得到
#define ROOMINFO_PRESENTLIST_GET_CMD     	(0xB451)

//小屋：班级答题:增加分数
#define ROOMINFO_CLASS_QA_ADD_SCORE_CMD  	(0xB453 |NEED_UPDATE)

//小屋：班级答题:检查今天是否已答过了
#define ROOMINFO_CLASS_QA_CHECK_ADD_CMD  	(0xB454 )

//小屋：班级答题:得到答题数据
#define ROOMINFO_CLASS_QA_GET_INFO_CMD   	(0xB455 )
//设置班级谁已经领过奖
#define ROOMINFO_SET_CLASS_MEDAL_FLAG_CMD        (0xB456 | NEED_UPDATE)

//增加PK的好友信息
#define ROOMINFO_PK_ADD_CMD              	(0xB45A |NEED_UPDATE)
//得到PK历史列表
#define ROOMINFO_PK_GET_LIST_CMD         	(0xB45B )

#define ROOMMSG_ADD_MSG_CMD                     (0xC411 |NEED_UPDATE)
#define ROOMMSG_SHOW_MSG_CMD                    (0xC412 |NEED_UPDATE)

#define ROOMMSG_LIST_MSG_CMD                    (0xC414 )
#define ROOMMSG_DEL_MSG_CMD                     (0xC415 |NEED_UPDATE)
#define ROOMMSG_RES_MSG_CMD                     (0xC416 |NEED_UPDATE)
//插入bibo记录
#define ROOMMSG_INSERT_BIBO_CMD          	(0xC417 |NEED_UPDATE)
//删除bibo记录
#define ROOMMSG_DEL_BIBO_CMD             	(0xC418 |NEED_UPDATE)
//得到bibo记录
#define ROOMMSG_GET_BIBO_LIST_CMD        	(0xC419 )

//日记：添加
#define ROOMMSG_ADD_DIARY_CMD            	(0xC420 |NEED_UPDATE)

//日记：更新
#define ROOMMSG_SET_DIARY_CMD            	(0xC421 |NEED_UPDATE)

//日记：设置公开与否标志
#define ROOMMSG_SET_DIARY_PUB_FLAG_CMD   	(0xC422 |NEED_UPDATE)

//日记：得到写过列表
#define ROOMMSG_GET_DIARY_LIST_CMD       	(0xC423 )

//日记：得到日记内容
#define ROOMMSG_GET_DIARY_CMD            	(0xC424 )

//产生一个GROUPID
#define GROUPMAIN_GEN_GROUPID_CMD        	(0x3401 |NEED_UPDATE)
//产生一个CLASSID
#define CLASSID_GEN_CMD                     (0x3402 |NEED_UPDATE)


//添加群
#define GROUP_ADD_GROUP_CMD              	(0xD401 |NEED_UPDATE)
//
//添加群成员<br \>定义memberlist的个数为membercount
#define GROUP_ADD_MEMBER_CMD              	(0xD402 |NEED_UPDATE)
//删除群成员:<br \>定义memberlist的个数为membercount
#define GROUP_DEL_MEMBER_CMD             	(0xD403 |NEED_UPDATE)
//设置群信息
#define GROUP_SET_MSG_CMD                	(0xD404 |NEED_UPDATE)
//得到群信息:<br \>定义memberlist的个数为membercount
#define GROUP_GET_INFO_CMD               	(0xD405 )
//删除群
#define GROUP_DEL_GROUP_CMD              	(0xD406 |NEED_UPDATE)
//检查群是否存在
#define GROUP_CHECK_GROUP_CMD            	(0xD407 )
//得到群主ID.
#define GROUP_GET_OWNERID_CMD            	(0xD408 )

//得到成员列表:<br \>定义memberlist的个数为membercount
#define GROUP_GET_MEMBERLIST_CMD         	(0xD409 )

//得到毛毛树信息
#define MMS_GET_INFO_CMD                 	(0x4401 )

//操作:设置访问列表.加值
#define MMS_OPT_CMD                      	(0x4402 |NEED_UPDATE)

//得到对操作人的列表<br \>定义item的个数为count
#define MMS_GET_OPTLIST_CMD              	(0x4403 )

//摘掉一个毛毛果
#define MMS_PICK_ONE_FRUIT_CMD            	(0x4404  |NEED_UPDATE)

//得到类型列表:毛毛树,金蘑菇向导,...
#define MMS_GET_TYPE_LIST_CMD            	(0x4405  )

//设置相关的数据
#define MMS_SET_VALUE_CMD                	(0x4407 |NEED_UPDATE)


//得到一项数据
#define MMS_GET_INFO_BY_TYPE_CMD            (0x4408 )

//得到类型列表-ex
#define MMS_GET_TYPE_LIST_EX_CMD           	(0x4409  )

//图片:增加图片
#define PIC_ADD_PIC_CMD                  	(0x7401 |NEED_UPDATE)

//图片:得到用户图片列表
#define PIC_GET_USER_ABLUM_PICLIST_CMD      (0x7402 )

//图片:得到单张图片
#define PIC_GET_PIC_BY_PHOTOID_USERID_CMD   (0x7403 )

//图片:删除单张图片
#define PIC_DEL_PIC_BY_PHOTOID_USERID_CMD   (0x7404 |NEED_UPDATE)



//申诉:添加
#define APPEAL_ADD_APPEAL_CMD            	(0x8401 |NEED_UPDATE)
//申诉:得到列表
#define APPEAL_GET_LIST_CMD              	(0x8402 )
//申诉:得到一条记录
#define APPEAL_GET_APPEAL_CMD            	(0x8403 )

//申诉:处理结果
#define APPEAL_DEAL_CMD                  	(0x8404 |NEED_UPDATE)


//申诉-ex:添加
#define APPEAL_EX_ADD_APPEAL_CMD            	(0x8410 |NEED_UPDATE)
//申诉-ex:处理结果
#define APPEAL_EX_DEAL_CMD                  (0x8411 |NEED_UPDATE)

//申诉-ex:得到状态
#define APPEAL_EX_GET_STATE_CMD          	(0x8412 )

//申诉-ex:得到列表
#define APPEAL_EX_GET_LIST_CMD           	(0x8413 )

//申诉-ex:得到用户最近一次申诉信息
#define APPEAL_EX_GET_USER_LAST_STATE_CMD	(0x8414 )

//不路由的FC
//sendemail 
#define OTHER_SEND_EMAIL_CMD            (0x1014 | NEED_UPDATE)

//VIP
#define OTHER_SYNC_VIP_CMD            	(0xF001 | NEED_UPDATE)

//设置用户的VIP标志
#define PP_SET_VIP_CMD                          (0x186E |NEED_UPDATE)



//聊天记录
#define LOG_ADD_CHAT_CMD                 	(0xF002 | NEED_UPDATE)

//重要修改记录
#define LOG_ADD_CHANGE_CMD               	(0xF003 | NEED_UPDATE)
//密码修改记录
#define PASSWD_ADD_CHANGE_CMD                (0xF004 |NEED_UPDATE)

//登入记录
#define FF_LOGIN_ADD_CMD                 	(0xF005 |NEED_UPDATE)

//IP地址记录
#define HISTORY_IP_RECORD_CMD                (0xF006 |NEED_UPDATE)

//登入记录得到列表
#define FF_LOGIN_GET_LIST_CMD               (0xF007 )


//得到用户最近一次的IP
#define IP_HISTORY_GET_LASTIP_CMD        	(0xF008 )


//大玩国发信息给好友
#define DWG_SENDMSG_TO_FRIENDS_CMD       	(0xF020 |NEED_UPDATE) 



//SYS ARG 
//设置变量数值

#define MD5_LEN						16 
#define SERIAL_LEN  				8 
#define NICK_LEN					16 
#define GROUPNAME_LEN				25	
#define GROUPMSG_LEN				121	
#define GROUPITEM_LEN				100	
#define GROUP_LIST_LEN				50	
#define PIC_FILE_ID_LEN				50	
#define SPRING_MSG_LEN				100	
#define FEELING_LEN					10	
#define PROFESSION_COUNT			50	

#define SERVERNAME_LEN 				16
#define MAPNAME_LEN 				64 

#define CREATE_GROUP_MAX			3	
#define PASSWD_LEN					MD5_LEN 
#define MOBILE_LEN					16 
#define TELEPHONE_LEN 				16 
#define ADDR_LEN					64 
#define INTEREST_LEN				64 
#define SIGNATURE_LEN				128 
#define EMAIL_LEN					64 
#define QUESTION_LEN				64 
#define ANSWER_LEN					64 
#define SERIAL_STR_LEN				12 
#define FIELD_NAME_LEN 				50

#define USER_EMAIL_LEN				1800	 
//id list
#define ID_LIST_HEADER_LEN 			4 
#define ID_LIST_MAX					200 
#define FRIEND_COUNT_MAX 			200 
#define JY_ACCESS_MAX 				50


//msg list 
#define MSG_LIST_HEADER_LEN			8	 
#define MSG_LIST_BUF_SIZE			2000 
#define MSG_MAX_LEN					100	
#define MSGBOARD_MSG_MAX_LEN	 	200	
#define CHRISTMAS_MSG_LEN			255	

#define CHILD_COUNT_MAX				200 
#define ROOMINFO_PUG_LIST_LEN		40	

#define LIST_COUNT_SIZE  			(sizeof ( stru_count))

//home attire item 
#define HOME_ATTIRE_ITEM_VALUE_LEN 	12 
#define HOME_ATTIRE_ITEM_MAX 		100 
#define HOME_NOUSE_ATTIRE_ITEM_MAX 	300 

//新的定义
#define HOME_NOUSE_ATTIRE_ITEM_MAX_EX  1000 

#define VALUE_MAX					100000000

//属性最大值
#define ATTR_VALUE_MAX				100   
#define CARDLIST_INIT_LEN          	30 
#define CARDLIST_LEN                100
#define CARDLIST_MAX				80
#define POWER_COUNT_MAX 			1000

//银行允许的记录数
#define RECORD_MAX                  5
//五天
#define FIVE_DAY   					5								 
//十天
#define TEN_DAY                   	10 
//五天的存款利息
#define FIVE_DAY_INTEREST			0.04
//十天的存款利息
#define TEN_DAY_INTEREST			0.1
//22日之后的利息
#define FIVE_DAY_INTEREST_AFTER     0.03			
#define TEN_DAY_INTEREST_AFTER		0.08
//存款的最少豆豆数目
#define XIAOMEE_THRESHOLD			1000
//家园种子最小ID号
#define MIN_SEED_ID 				1230001
//即处于干旱和虫害
#define DROUGHT_AND_INSECT 			3
//虫害
#define INSECT 						2
//干旱
#define DROUGHT 					1
//健康
#define HEALTH 						0
//最大允许的种子数
#define MAX_SEED_NUM 				12
//user_task_ex的缓存的长度
#define TASK_LIST_EX_LEN			50
#define CLASS_NAME_LEN				16
#define CLASS_SLOGAN_LEN			60	
#define CLASS_MEMBER_NUM			200	

#define JOIN_CLASS_MAX				10	
#define PK_LIST_MAX_COUNT 			100	

struct stru_count{
	uint32_t		count;
}__attribute__((packed))  ;

struct stru_id{
	userid_t  	id;
} __attribute__((packed)) ;

struct  msg_max{
	uint32_t msglen;
	char	msg[4000];
}__attribute__((packed));

struct  char_stru{
	char   c; 
}__attribute__((packed));


struct  stru_email{
		char email[EMAIL_LEN];
}__attribute__((packed)); 
struct  nick_stru{
		char nick[NICK_LEN];
}__attribute__((packed)); 



struct  email_msg{
	uint32_t	email_noread;//未读数 
}__attribute__((packed));

struct  user_email_item{
	uint32_t	emailid;
	uint32_t	type; /*卡片类型*/
	uint32_t	sendtime;
	userid_t	senderid; 
	char		sendernick[NICK_LEN];
	uint32_t	mapid; /**/
	uint32_t	msglen;
	char 		msg[USER_EMAIL_LEN];
}__attribute__((packed));

struct  user_email_item_without_id{
	uint32_t	type; /*卡片类型*/
	uint32_t	sendtime;
	userid_t	senderid; 
	char		sendernick[NICK_LEN];
	uint32_t	mapid; /**/
	uint32_t	msglen;
	char 		msg[USER_EMAIL_LEN];
}__attribute__((packed));


struct  stru_group_member{
	uint32_t	membercount;
	uint32_t	memberlist[GROUPITEM_LEN];
}__attribute__((packed));

struct  stru_group_item{
	uint32_t	groupid;
	uint32_t	flag;
}__attribute__((packed));


struct  group_list{
	uint32_t	count;
	stru_group_item  groupidlist[GROUP_LIST_LEN];
}__attribute__((packed));
struct stru_classlist{
	uint32_t count;
	uint32_t classid[JOIN_CLASS_MAX];
}__attribute__((packed));



struct  group_item_without_id{
	userid_t	groupownerid;
	char		groupname[GROUPNAME_LEN];
	uint32_t	type;
	char		groupmsg[GROUPMSG_LEN];
	uint32_t	membercount;
	uint32_t	memberlist[GROUPITEM_LEN];
}__attribute__((packed));

struct stru_member_score  {
	uint32_t memberid;
	uint32_t score;
}__attribute__((packed));


struct stru_member_score_list  {
	uint32_t count;
	stru_member_score items[200];
}__attribute__((packed));


struct  group_msg_item{
	char		groupname[GROUPNAME_LEN];
	char		groupmsg[GROUPMSG_LEN];
}__attribute__((packed));


struct  type_stru{
	uint32_t	type;
}__attribute__((packed));

struct  stru_flag{
	uint32_t	flag;
}__attribute__((packed));



struct cup_stu{
	uint32_t	cup1;
	uint32_t	cup2;
	uint32_t	cup3;
} __attribute__((packed));

struct  stru_group_cup{
    uint32_t groupid;  
    uint32_t cup1;  
    uint32_t cup2;  
    uint32_t cup3;  
}__attribute__((packed));

struct cup_msg_item {
    userid_t userid;  
    uint32_t groupid;  
    uint32_t gameid;  
}__attribute__((packed)) ;

struct stru_date_group_item {
    uint32_t date;  
    uint32_t groupid;  
}__attribute__((packed)) ;

struct group_cup_history{
	uint32_t cachedate;
	uint32_t count;
	stru_date_group_item group_item[100];
}__attribute__((packed));
struct stru_appeal_ex{
	uint32_t id ;
	uint32_t appeal_type;
	uint32_t gameid;
	uint32_t userid	;
	uint32_t state	;
	uint32_t adminid	;
	uint32_t logtime	;
	uint32_t dealtime	;
	char realname	[16];
	char moblie	[16];
	char telephone	[16];
	char email	[64];
	uint32_t games	;
	uint32_t birth	;
	uint32_t first_login	;
	uint32_t last_login	;
	char last_place	[64];
	uint32_t vip	;
	uint32_t vip_type	;
	char vip_no	[20];
	uint32_t vip_time	;
	uint32_t mole_tasks	;
	uint32_t mole_WizardLevel	;
	uint32_t mole_KnightLevel	;
	uint32_t mole_IsLastWish	;
	uint32_t mole_BankBalance1	;
	uint32_t mole_BankBalance2	;
	uint32_t seer_tasks	;
	uint32_t seer_has_teacher	;
	uint32_t seer_has_student	;
	uint32_t seer_expirence1	;
	uint32_t seer_expirence2	;
	uint32_t seer_money1	;
	uint32_t seer_money2	;
	uint32_t seer_fairy_num1	;
	uint32_t seer_fairy_num2	;
	char seer_fairy_name	[16];
	uint32_t seer_fairy_level	;
	uint32_t seer_tower_level	;
}__attribute__((packed));



struct stru_appeal{
	uint32_t	id  ;
	uint32_t	state ;
	uint32_t	dealflag;
	uint32_t	dealtime;
	uint32_t	adminid;
	uint32_t	logtime  ;
	uint32_t	userid  ;

	uint32_t	flag;
	char 		why[200];

	uint32_t	vip;
	uint32_t	vip_way;
	uint32_t	reg_time;
	char 		reg_email[64];
	uint32_t	birthday;
	uint32_t	Ol_last;

	char 		question[64]; 	
	char 		answer[64]; 	

	char 		realname[16];
	char 		email   	[64];
	char 		mobile  	[16];
	char 		telephone  	[16];
	char 		realpasswd1 [20];
	char 		realpasswd2 [20];
	char 		realpasswd3 [20];
	char 		addr	[200]; 
	char 		othermsg [600]; 
}__attribute__((packed));

struct  cup_msg_list{ 
	uint32_t     index;/*要更新的ID,每更新一次,自增*/
    cup_msg_item cup_item[10];
}__attribute__((packed));








typedef stru_id   userinfo_del_friend_in; 
typedef stru_id   userinfo_add_blackuser_in; 
typedef stru_id   userinfo_del_blackuser_in; 
typedef stru_id userinfo_add_friend_in;

typedef stru_id	user_del_friend_in; 
typedef stru_id	user_add_blackuser_in; 
typedef stru_id  user_del_blackuser_in; 
typedef stru_id	user_add_friend_in;




struct  user_change_xiaomee_in{
	int32_t	change_xiaomee;
	uint32_t	reason;
	uint32_t	reason_ex;
}__attribute__((packed));

struct  user_change_xiaomee_out{
	uint32_t	left_xiaomee;
}__attribute__((packed));


struct user_change_user_value_in{
	int32_t	 	addexp;
	int32_t	 	addstrong;
	int32_t	 	addiq;
	int32_t	 	addcharm;
	int32_t	 	addxiaomee;
/*
 *   
 */
	uint32_t	reason;
	uint32_t	reason_ex;
} __attribute__((packed));

struct user_change_user_value_out {
	int32_t		exp;
	int32_t		strong;
	int32_t		iq;
	int32_t		charm;
	int32_t		xiaomee;
} __attribute__((packed));


struct userinfo_login_in {
	char 		passwd[PASSWD_LEN];
	uint32_t	addr_type;
	uint32_t	ip;
}__attribute__((packed));


struct  userinfo_login_ex_in{
	char 	passwd[PASSWD_LEN];
}__attribute__((packed));
struct  userinfo_login_ex_out{
	uint32_t	gameflag;
}__attribute__((packed));

struct  userinfo_login_ex_with_stat_in{
	char 	passwd[PASSWD_LEN];
	uint32_t ip;
	uint32_t login_channel;
}__attribute__((packed));
struct  userinfo_login_ex_with_stat_out{
	uint32_t	gameflag;
}__attribute__((packed));



struct  userinfo_log_with_stat_in{
	char passwd[PASSWD_LEN];
	uint32_t ip;
	uint32_t login_channel;
}__attribute__((packed));
struct  userinfo_log_with_stat_out{
	uint32_t gameflag;
}__attribute__((packed));


struct userinfo_login_out{
	uint32_t	vip_flag;
	uint32_t	friendcount;
	uint32_t	frienditem[FRIEND_COUNT_MAX];
}__attribute__((packed));




struct stru_passwd {
	char passwd[PASSWD_LEN];
}__attribute__((packed));

typedef stru_passwd su_login_in;
typedef stru_passwd userinfo_su_set_passwd_in ;
typedef  stru_passwd  userinfo_su_set_paypasswd_in;



struct su_login_out {
	char nick[NICK_LEN ];
	uint32_t  flag;
}__attribute__((packed));

struct su_get_adminid_by_nick_in{
	char nick[NICK_LEN ];
}__attribute__((packed));
typedef stru_id su_get_adminid_by_nick_out; 

struct userinfo_get_nick_out  {
	char nick[NICK_LEN ];
}__attribute__((packed));
typedef nick_stru user_get_nick_out; 

struct su_get_admin_list_in{
	uint32_t index;
}__attribute__((packed));

struct su_add_admin_in{
	uint32_t adminid;
	char 	 nick[NICK_LEN];
}__attribute__((packed));



struct admin_item{
	uint32_t adminid;
	uint32_t flag;
	char 	 nick[NICK_LEN];
	uint32_t useflag;
}__attribute__((packed));

struct admin_all_item{
	uint32_t adminid;
	uint32_t flag;
	char 	 nick[NICK_LEN];
}__attribute__((packed));




struct user_change_passwd_in {
	char oldpasswd[PASSWD_LEN];
	char newpasswd[PASSWD_LEN];
}__attribute__((packed));
typedef user_change_passwd_in userinfo_change_passwd_in ;
typedef user_change_passwd_in su_change_passwd_in;
typedef user_change_passwd_in  userinfo_set_paypasswd_with_check_in;



struct  stru_new_passwd{
	char newpasswd[PASSWD_LEN];
}__attribute__((packed));

typedef stru_new_passwd dv_change_passwd_in;
typedef stru_new_passwd userinfo_change_passwd_nocheck_in;

struct  user_change_nick_in{
	char newnick[NICK_LEN];
}__attribute__((packed));

typedef user_change_nick_in dv_change_nick_in;
typedef nick_stru user_set_nick_in;


struct  stru_tasklist{
	uint8_t		list[100];
}__attribute__((packed));



struct user_without_id{
		uint32_t 		vip; 
		uint32_t 		flag; 
		uint32_t		petcolor;
		uint32_t 		petbirthday;  /* set register time */
		uint32_t 		xiaomee;/*小米*/
		uint32_t 		xiaomee_max;/*小米*/
		uint32_t 		sale_xiaomee_max;/*小米*/
		uint32_t		exp;
		uint32_t		strong;
		uint32_t		iq;
		uint32_t		charm;
		uint32_t 		game; 
		uint32_t 		work; 
		uint32_t 		fashion; 
		uint32_t 		Ol_count; /*登入次数 */
		uint32_t 		Ol_today; /*last login */
		uint32_t		Ol_last; /* 最后一次登入时间 */
		uint32_t		Ol_time; /*在线总时间 */
		uint32_t		last_ip; /*在线总时间 */
		uint32_t		birthday; /*在线总时间 */
		stru_tasklist 		tasklist; /*任务完成列表*/
		uint32_t 		flag2; 
		char  			nick[NICK_LEN]; 
		uint32_t		driver;
		uint32_t 		level;
}__attribute__((packed));
typedef user_without_id user_get_user_all_out;


struct user_info_item
{
		uint32_t 		flag1; 
		uint32_t 		regflag; 
		char 			passwd[PASSWD_LEN];  
		char 			paypasswd[PASSWD_LEN];  
		char 			email[EMAIL_LEN];
		char 			parent_email[EMAIL_LEN];
		char 			passwdemail[EMAIL_LEN ];  
		char 			question[QUESTION_LEN];
		char 			answer[ANSWER_LEN];
		char 			signature[SIGNATURE_LEN];
}__attribute__((packed));

struct user_info_ex_item
{
		uint32_t 		flag; /*0x02:性别*/
		uint32_t 		birthday; 
		char 			telephone[TELEPHONE_LEN]; 
		char 			mobile[MOBILE_LEN]; 
		uint32_t		mail_number;
		uint16_t		addr_province;
		uint16_t		addr_city;
		char 			addr[ADDR_LEN];
		char 			interest[INTEREST_LEN];
}__attribute__((packed));

struct  creategroupid_list
{
	uint32_t	count;
	stru_group_item creategroupid[CREATE_GROUP_MAX];
}__attribute__((packed));

struct  stru_user_other
{
		uint32_t 		flag;
		uint32_t 		level;
		uint32_t		vip_mark_last; ///< VIP用户成长基础值
		uint32_t		vip_start_time;///< VIP用户开始的时间
		uint32_t		vip_end_time;///< VIP用户结束的时间
		uint32_t		vip_pet_level;///< VIP用户等级
		uint32_t 		driver;
		char 			nick[NICK_LEN];  
		uint32_t 		class_firstid;  //首班级ID
		stru_classlist 		classlist;
		uint32_t  		creategroup_count;
		uint32_t  		creategroup_id1;
		uint32_t  		creategroup_id2;
		uint32_t  		creategroup_id3;
}__attribute__((packed));

struct  userinfo_get_info_all_out
{
	user_info_item info;	 
	user_info_ex_item  info_ex;	 
}__attribute__((packed));

typedef  user_info_ex_item userinfo_get_info_ex_out;
typedef  user_info_ex_item userinfo_set_info_ex_in;


struct    user_set_user_birthday_sex_in
{
		uint32_t 		birthday; 
		uint32_t 		sex; 
}__attribute__((packed));
typedef user_set_user_birthday_sex_in userinfo_set_user_birthday_sex_in;


struct user_set_user_online_in
{
		uint32_t  onlinetime; 
		uint32_t  ip; 
}__attribute__((packed));

struct  user_get_flag_out 
{
		uint32_t   flag;  
		uint32_t   vip;  
}__attribute__((packed));

struct user_set_home_attire_noused_in
{
		uint32_t 	type;  
		uint32_t 	attireid;  
}__attribute__((packed));

struct home_attire_item
{
		uint32_t	attireid ;//装扮ID
		uint8_t		value [HOME_ATTIRE_ITEM_VALUE_LEN];//相关参数
		inline bool operator == ( const struct home_attire_item  & other  )
		{
			return (this->attireid ==other.attireid);
		}
		inline bool operator != ( const struct home_attire_item  & other  )
		{
			return (this->attireid !=other.attireid);
		}
		inline 	bool operator <(const struct home_attire_item & other  ) const {
			return (this->attireid < other.attireid);
		}
		inline 	bool operator >(const struct home_attire_item & other  ) const {
			return (this->attireid > other.attireid);
		}
} __attribute__((packed));

struct attire_count
{
		uint32_t	attireid ;
		uint32_t	count;
		inline bool operator == ( const struct attire_count  & other  )
		{
			return (this->attireid ==other.attireid);
		}
		inline 	bool operator <(const struct attire_count & other  ) const {
			return (this->attireid < other.attireid);
		}
		inline 	bool operator >(const struct attire_count & other  ) const {
			return (this->attireid > other.attireid);
		}

} __attribute__((packed));

struct attire_count_with_max{
		uint32_t	attireid ;
		uint32_t	count;
		uint32_t	maxcount;
		inline bool operator == ( const struct attire_count_with_max  & other  )const
		{
			return (this->attireid ==other.attireid);
		}
		inline 	bool operator <(const struct attire_count_with_max & other  ) const {
			return (this->attireid < other.attireid);
		}
		inline 	bool operator >(const struct attire_count_with_max & other  ) const {
			return (this->attireid > other.attireid);
		}
} __attribute__((packed));


typedef    attire_count  attire_noused_item ;
typedef    attire_count  stru_attire_noused;

struct noused_homeattirelist
{
	uint32_t	count;
	attire_noused_item item[HOME_NOUSE_ATTIRE_ITEM_MAX];

	void add_attire_count(uint32_t attireid, uint32_t add_to_value)
	{
			for (uint32_t i=0;i<this->count;i++)
			{
				if (this->item[i].attireid==attireid)
				{
					this->item[i].count += add_to_value;
					return;
				}
			}	

			if(count<HOME_NOUSE_ATTIRE_ITEM_MAX)
			{
				item[count].attireid=attireid;
				item[count].count=add_to_value;
				++count;
			}
	}

	uint32_t get_attire_count(uint32_t attireid)
	{
			for (uint32_t i=0;i<this->count;i++)
			{
				if (this->item[i].attireid==attireid)
				{
					return this->item[i].count;
				}
			}	
			return 0;
	}
}__attribute__((packed));

struct stru_noused_attirelist
{
	uint32_t	count;
	stru_attire_noused item[HOME_NOUSE_ATTIRE_ITEM_MAX_EX];
	inline 	uint32_t get_attire_count(uint32_t attireid)
	{
			for (uint32_t i=0;i<this->count;i++)
			{
				if (this->item[i].attireid==attireid)
				{
					return this->item[i].count;
				}
			}	
			return 0;
	}

} __attribute__((packed));

struct add_attire
{
		uint32_t	attiretype;///< 装扮: 0,小屋: 1,家园：2 */
		uint32_t	attireid;
		uint32_t	count;
		uint32_t	maxcount;
} __attribute__((packed));


typedef add_attire  user_add_attire_in; 
typedef add_attire  user_del_attire_in; 

struct   user_set_home_attirelist_in_header{
		uint32_t	homeid;
		uint32_t	usedcount;
} __attribute__((packed)) ;


struct   user_set_jy_attirelist_in_header{
		uint32_t	usedcount;
		uint32_t	nousedcount;
} __attribute__((packed)) ;

struct home_attirelist{
		uint32_t	count;
		home_attire_item item[HOME_ATTIRE_ITEM_MAX];
		inline 	uint32_t get_attire_count(uint32_t attireid){
			uint32_t used_count=0;
			for (uint32_t i=0;i<this->count;i++){
				if (this->item[i].attireid==attireid){
					used_count++;
				}
			}	
			return used_count;
		}
} __attribute__((packed));
typedef home_attirelist stru_home_used_attirelist;


typedef home_attire_item  user_set_home_attirelist_in_item_1;

typedef home_attire_item  user_set_jy_attirelist_in_item_1;
typedef attire_noused_item user_set_jy_attirelist_in_item_2;




struct home_del_attire_in {
		uint32_t	id;
} __attribute__((packed));

struct msg_item{
		uint32_t 	itemlen;
		char 		msg[MSG_MAX_LEN];
} __attribute__((packed));



struct   user_add_offline_msg_in_header{
		uint32_t 	msglen;/*大小包括自身4个字节*/
} __attribute__((packed));



struct msgboard_add_msg_in_header {
		uint32_t boardid;
		uint32_t color;
		char 	 nick[NICK_LEN];
		uint32_t msglen ;
} __attribute__((packed));

struct msgboard_add_msg_out{
		uint32_t msgid;
} __attribute__((packed));



struct msgboard_get_msglist_in{
		uint32_t boardid;
		uint32_t pageid;
} __attribute__((packed));

struct msgboard_get_list_with_flag_in {
		uint32_t boardid;
		uint32_t flag;
		uint32_t pageid;
} __attribute__((packed));


struct msgboard_item_max { 
	uint32_t msgid;  
	userid_t userid;  
	uint32_t logdate;  
	uint32_t hot;
	uint32_t color;
	char 	 nick[NICK_LEN];
	char   	 msg[1024];  
}__attribute__((packed));

struct stru_msgboard_item{ 
	uint32_t msgid;  
	userid_t userid;  
	uint32_t logdate;  
	uint32_t hot;
	uint32_t color;
	char 	 nick[NICK_LEN];
	char   	 msg[MSGBOARD_MSG_MAX_LEN];  
}__attribute__((packed));



struct msgboard_getmsg_by_msgid_in{ 
	uint32_t boardid;  
	uint32_t msgid;  
}__attribute__((packed));

typedef msgboard_getmsg_by_msgid_in msgboard_add_hot_in;



typedef	msgboard_item_max msgboard_getmsg_by_msgid_out ; 


struct msgboard_get_msglist_out {
	uint32_t 	  count_in_db; 
	uint32_t 	  count; 
	stru_msgboard_item  item_msgboard [6]; 
} __attribute__((packed));




struct stru_msg_list {
	uint32_t len;
	uint32_t count;
	char 	 buf[MSG_LIST_BUF_SIZE ];
}__attribute__((packed));

typedef add_attire pay_buy_attire_by_damee_in ;


struct pay_buy_attire_by_damee_out {
	uint32_t  leave_damee;  /*剩余大米*/
} __attribute__((packed));

struct    pay_add_damee_by_serial_in{
 	char serialid[20];
} __attribute__((packed));

typedef pay_add_damee_by_serial_in serial_get_noused_id_out;

struct pay_add_damee_by_serial_out
{
	uint32_t  price;  /*价值 */
	uint32_t  leave_damee;  /*剩余大米*/
} __attribute__((packed));

typedef stru_flag userinfo_set_vip_flag_in ;
struct user_set_vip_flag_in
{
	uint32_t flag;///< 一个复杂的flag标志，见表t_user_ex_0定义
	uint32_t level;///< 包月的月数

	uint32_t vip_start_time;///< 成为VIP的开始时间
	uint32_t vip_end_time;///< VIP的结束时间
	uint8_t  vip_is_auto_charge; ///< 是否自动续费
	uint32_t vip_charge_changel_last;///< 上次充值通道
}__attribute__((packed));

struct user_vip_mark_last_set_in
{
	uint32_t vip_mark_last;///< 当前VIP等级值
}__attribute__((packed));

struct user_vip_pet_level_set_in
{
	uint32_t vip_pet_level;///< 用户超拉等级
}__attribute__((packed));

typedef stru_flag userinfo_set_flag_change_passwd_in ;
typedef stru_flag user_set_flag_unused_in ;
typedef stru_flag user_set_flag_offline24_in;
typedef stru_flag user_set_flag_in;

struct msgboard_set_msg_flag_in{
	uint32_t  boardid; 
	uint32_t  flag;
} __attribute__((packed));



struct  su_change_in {
		userid_t userid;
		uint32_t attrid; 
		int32_t  changvalue; 
		int32_t  reason; 
} __attribute__((packed));




struct id_list{
		uint32_t	count;
		userid_t 	item[ID_LIST_MAX];
} __attribute__((packed));

typedef id_list  friend_list;
typedef id_list  black_list;
typedef id_list  userinfo_get_blackuser_list_out;
typedef id_list  user_get_blackuser_list_out;
typedef id_list  stru_idlist;


struct user_get_jy_ex_out {
	home_attirelist attirelist;
} __attribute__((packed));

struct  stru_user_get_home_out{
	home_attire_item home_type_item;
	home_attirelist attirelist;
}__attribute__((packed));

struct  user_get_home_out_header{
	home_attire_item home_type_item;
	uint32_t count;
}__attribute__((packed));

typedef home_attire_item user_get_home_out_item;

struct user_get_jy_out_header {
	uint32_t hometype_id;
	uint32_t	item1_count;
	uint32_t	item2_count;
} __attribute__((packed));

struct user_get_farm_out_header {
	uint32_t    runout_num;
	uint32_t    farm_state;
	uint32_t 	pool_state;
	uint32_t	insect_house;
	uint32_t	item1_count;
	uint32_t	item2_count;
	uint32_t	item3_count;
	uint32_t 	item4_count;
}__attribute__((packed));



struct jy_item{
	uint32_t id;
	uint32_t attireid;
	uint32_t x;
	uint32_t y;
	uint32_t value;
	uint32_t sickflag;
	uint32_t fruitnum;
	uint32_t cal_value_time;
	uint32_t water_time;
	uint32_t kill_bug_time;
	uint32_t status;
	uint32_t mature_time; ///< 成熟与否：0不成熟,其它成熟的时间
	uint32_t diff_mature;
	uint32_t cur_grow;
	uint32_t earth;///<  是否翻过地
	uint32_t pollinate_num;///< 授粉的次数 
}__attribute__((packed));

//种子的ID号
struct  user_water_seed_in{
	uint32_t id;
	uint32_t opt_userid;
}__attribute__((packed));

//typedef jy_item user_water_seed_out;
struct user_water_seed_out{
	jy_item item;
}__attribute__((packed));


struct  user_kill_bug_in {
	uint32_t id;
	uint32_t opt_userid;
}__attribute__((packed));



typedef user_water_seed_out user_kill_bug_out; 

struct seed_info {

    uint32_t item;
    uint32_t fruitid;
    uint32_t healthy_grow_rate; ///< 正常状态下的生长率
    uint32_t drought_grow_rate; ///< 干旱状态下的生长率
    uint32_t insect_grow_rate; ///< 虫害状态下的生长率
    uint32_t di_grow_rate;
    uint32_t drought_rate;
    uint32_t insect_rate;
    uint32_t mature; ///< 成熟需要的成长值
	uint32_t fruitsick;
    uint32_t fruits[4];
	uint32_t exp;///< 增加的种植能手经验值
	uint32_t pollinate_min_grow;///< 可以授粉的最小成长值
	uint32_t pollinate_max_grow;///< 可以授粉的最大成长值
	uint32_t pollinate_num;///<  可以授粉的次数
};


typedef home_attire_item user_get_jy_out_item_1;
typedef jy_item user_get_jy_out_item_2;
/* 保存动物的信息 */
struct stru_animal_info{
    uint32_t animal_id;
    uint32_t mature_id; ///< 成熟动物的编号
	uint32_t output_id; ///<
	uint32_t food_type;
    uint32_t health_grow_rate;
    uint32_t drought_grow_rate;
    uint32_t hungry_grow_rate;
    uint32_t both_grow_rate;
    uint32_t drought_interval;
    uint32_t hungry_interval;
    uint32_t mature;
    uint32_t output[4];
	uint32_t fruit_max;
	uint32_t output_interval;
	uint32_t output_value;
	uint32_t favor;/* 初始好感度 */
	uint32_t exp;/* 每次增加的养殖能手的数值 */
	uint32_t pollinate_num;/* 授粉的次数 */
	uint32_t animal_kind;/* 动物的总类 */
}__attribute__((packed));

struct stru_farm{
	uint32_t id ;
	uint32_t userid ;
	uint32_t animalid ;
	uint32_t state ;
	uint32_t growth ;
	uint32_t eattime ;
	uint32_t drinktime ;
	uint32_t total_output ;
	uint32_t output_time ;
	uint32_t lastcal_time ;
	uint32_t mature ;
	uint32_t animal_type;
	uint32_t favor_time;
	uint32_t favor;
	uint32_t outgo;
	uint32_t pollinate_num;
}__attribute__((packed));


/* 从数据库中选出的信息 */
struct animal_select
{
    uint32_t id; ///< 数据对应的唯一编号
    uint32_t animal_id; ///< 动物种类编号
    uint32_t state;
    uint32_t growth;
    uint32_t eat_time;
    uint32_t drink_time;
	uint32_t total_output;
	uint32_t output_time;
	uint32_t lastcal_time;
	uint32_t mature_time;
	uint32_t animal_type ;
	uint32_t favor_time;
	uint32_t favor;
	uint32_t outgo;
	uint32_t animal_flag;
	uint32_t pollinate_num;///<  授粉次数
	uint32_t pollinate_tm;
	uint32_t max_output;
	uint32_t diff_mature;///< 还有多长时间成年
	uint32_t cur_grow; ///<  目前的成长数据室多少
}__attribute__((packed));



typedef  animal_select user_get_farm_out_item_1;

typedef home_attire_item user_get_farm_out_item_2;
typedef attire_count 	user_get_farm_out_item_3;
struct user_get_farm_out_item_4
{
	uint32_t attireid;
	uint32_t pos;
	uint32_t num;
	uint32_t total_num;
}__attribute__((packed));

struct   user_set_petcolor_in
{
		uint32_t		petcolor;
} __attribute__((packed));

typedef   stru_tasklist user_set_tasklist_in;

struct   get_user_out{
		stru_user_other 	user_other;
		uint32_t 		vip; 
		uint32_t 		flag; 
		uint32_t		petcolor;
		uint32_t 		petbirthday;  ///<  set register time
		uint32_t 		xiaomee;///< 小米
		uint32_t		exp;
		uint32_t		strong;
		uint32_t		iq;
		uint32_t		charm;
		uint32_t 		game;///< 火神杯勇士分数
		uint32_t 		work;///< 建设卡的等级 
		uint32_t		fashion;///<  时尚分数 
		uint32_t 		plant;///< 植物的等级
		uint32_t 		breed;///<  动物的等级
		uint32_t 		plant_limit;
		uint32_t 		breed_limit;
		int16_t			npc[10];
		uint32_t 		Ol_count; ///< 登入次数
		uint32_t 		Ol_today; ///< last login
		uint32_t		Ol_last; ///<  最后一次登入时间
		uint32_t		Ol_time; ///< 在线总时间
		uint32_t		birthday; ///< 在线总时间
		stru_tasklist 		tasklist; ///< 任务完成列表
		uint32_t 		have_doing_task;
		uint32_t        	lamu_task;
		uint32_t		parentid; ///< 邀请人
		uint32_t		childcount; 
		uint32_t		old_childcount;
		uint32_t		is_bank_daoqi; ///< 到期

		stru_msg_list  		msglist;
} __attribute__((packed));
typedef get_user_out user_get_user_out; ///< 这里有一个错误的设计
typedef get_user_out user_get_user_with_msglist_out;

struct user_register_in {
	char 		passwd[PASSWD_LEN];
	uint32_t 	sex;
	uint32_t 	birthday;
	uint32_t 	petcolor;
	char 		nick[NICK_LEN];
	char 		email[EMAIL_LEN];
	char 		mobile[MOBILE_LEN ];
	uint16_t	addr_province;
	uint16_t	addr_city;
	char 		addr[ADDR_LEN ];
	char 		signature[SIGNATURE_LEN];
	uint32_t	reg_addr_type;
	uint32_t	ip;
}__attribute__((packed));

struct userinfo_register_in{
	char 		passwd[PASSWD_LEN];
	uint32_t 	sex;
	uint32_t 	birthday;
	char 		nick[NICK_LEN];
	char 		email[EMAIL_LEN];
	char 		mobile[MOBILE_LEN ];
	uint16_t	addr_province;
	uint16_t	addr_city;
	char 		addr[ADDR_LEN ];
	char 		signature[SIGNATURE_LEN];
	uint32_t	reg_addr_type;
	uint32_t	ip;
}__attribute__((packed));

struct userinfo_register_ex_in{
	char 		passwd[PASSWD_LEN];
	char 		email[EMAIL_LEN];

	uint32_t 	sex;
	char 		mobile[MOBILE_LEN ];
	uint32_t 	birthday;
	uint16_t	addr_province;
	uint16_t	addr_city;
	char 		addr[ADDR_LEN ];
	char 		signature[SIGNATURE_LEN];
	uint32_t	reg_addr_type;
	uint32_t	ip;
}__attribute__((packed));




struct dv_register_in {
	char 		passwd[PASSWD_LEN];
	uint32_t 	sex;
	char 		nick[NICK_LEN];
	char 		email[EMAIL_LEN];
}__attribute__((packed));



struct  get_register_serial_in{
	char register_serial_id[8];
}__attribute__((packed));

struct  get_register_serial_out{
	uint32_t  userid;
}__attribute__((packed));

////////////////////////////////////////////////////////////
//  user_attire 表
//
struct attire_property {
	uint32_t		attireid;
	uint32_t		usedcount; 
	uint32_t		count;
} __attribute__((packed));

struct  user_set_attire_ex_in{
	uint32_t		attireid;
	uint32_t		usedcount; 
	uint32_t		chestcount; 
	uint32_t		count;
} __attribute__((packed));

typedef attire_count user_get_attire_list_out_item;


struct  user_set_attire_in
{
	uint32_t		attireid;
	uint32_t		operatetype; 
	uint32_t		value; 
	uint32_t		maxvalue; 
} __attribute__((packed));

struct  user_buy_attire_by_xiaomee_in{
	add_attire 		attire_item;
	uint32_t		usexiaomee; 
} __attribute__((packed));

typedef user_buy_attire_by_xiaomee_in   user_sale_attire_in;



struct  user_buy_attire_by_xiaomee_out{
	uint32_t	leave_xiaomee; 
	uint32_t	buy_attireid; 
	uint32_t	buy_count; 
} __attribute__((packed));

struct user_sale_attire_out {
	uint32_t xiaomee;
}__attribute__((packed));



typedef  attire_property user_set_attire_out;



//------------------------------------------------
//table game_score

struct  score_list_item{
	userid_t 	userid;
	char  		nick[NICK_LEN];
	uint32_t 	score;
}__attribute__((packed));


struct  get_game_score_list_in{
	uint32_t 	gameid;
}__attribute__((packed));

struct   set_game_score_in {
	uint32_t 	gameid;
	userid_t 	userid;
	uint32_t 	score;
	char  		nick[NICK_LEN];
}__attribute__((packed));

struct init_game_score_list_in{
	uint32_t 	gameid;
	uint32_t 	count;
}__attribute__((packed));

struct game_score_set_score_in{
	uint32_t 	gameid;
	uint32_t 	userid;
	uint32_t 	score;
}__attribute__((packed));

///////////////////////////////////////////////////////////////
//					user_cart 表
///////////////////////////////////////////////////////////////


struct get_cardtype_list_out_item{
	uint16_t		cardtype;
}__attribute__((packed));

struct get_cardid_list_in{
	uint16_t		cardtype;
}__attribute__((packed));


struct get_cardid_list_out_item{
	uint32_t		cardid;
}__attribute__((packed));

struct  add_user_card_list_in_item {
	uint16_t		cardtype;
	uint32_t		cardid;
}__attribute__((packed));

struct  user_swap_attire_list_in_header{
	uint32_t		oldcount;
	uint32_t		newcount;
	uint32_t		reason;
	uint32_t		reason_ex;
}__attribute__((packed));

typedef attire_count user_swap_attire_list_in_item_1;
typedef attire_count_with_max user_swap_attire_list_in_item_2;

struct  user_swap_homeattire_list_in_header{
	uint32_t		oldcount;
	uint32_t		newcount;
}__attribute__((packed));

typedef attire_count user_swap_homeattire_list_in_item_1;
typedef attire_count_with_max user_swap_homeattire_list_in_item_2;

typedef user_swap_homeattire_list_in_header user_swap_jyattire_list_in_header;
typedef attire_count user_swap_jyattire_list_in_item_1;
typedef attire_count_with_max user_swap_jyattire_list_in_item_2;

typedef user_swap_homeattire_list_in_header user_farm_edit_in_header;
typedef attire_count user_farm_edit_in_item_1;
typedef attire_count_with_max user_farm_edit_in_item_2;




typedef stru_count user_get_attire_list_out_header;


 
typedef stru_email userinfo_set_user_email_in;
typedef stru_email userinfo_get_user_email_out;


/*******************************************************************************  
		email db ---- email
*******************************************************************************/
typedef stru_email get_userid_by_email_in ;
typedef stru_email map_email_userid_in;

struct  map_email_userid_out{
	uint32_t	is_succ;
	uint32_t	old_map_userid;
}__attribute__((packed));

typedef stru_email del_map_email_userid_in;
struct email_set_email_in {
	char old_email[EMAIL_LEN];		
	char new_email[EMAIL_LEN];		
} __attribute__((packed));

/*******************************************************************************  
		serial db ---- email
*******************************************************************************/
struct  set_serial_used_in {
	uint64_t	serialid;
	uint32_t	useflag;
}__attribute__((packed));

struct  set_serial_unused_in {
	uint64_t	serialid;
}__attribute__((packed));

typedef type_stru serial_get_noused_id_in ; 

struct  serial_str_stru{
	char   		serial_str[SERIAL_STR_LEN];
}__attribute__((packed));

typedef serial_str_stru serial_set_serial_str_used_in;
typedef serial_str_stru serial_get_info_serial_str_in;

typedef type_stru serial_get_info_serial_str_out; 
typedef type_stru serial_set_serial_str_used_out; 





struct   set_serial_used_out{
	uint32_t	price; /*对应的金额*/
}__attribute__((packed));

struct  pay_change_damee_in{
	int32_t	adddamee; /* 可以为负数*/
}__attribute__((packed));

struct  pay_change_damee_out{
	uint32_t	leavedamee; 
}__attribute__((packed));


struct pay_stru {
	uint32_t date;		
	uint32_t out_transid;		
	uint32_t paytype;		
	uint32_t userid;		
	uint32_t months;		
	uint32_t damee;		
	char  	private_msg[30];		
}__attribute__((packed));





typedef  pay_stru   pay_pay_once_in; 
typedef  pay_stru   pay_pay_pre_in; 
struct  pay_pay_pre_out{
	uint32_t	transid;
} __attribute__((packed));

typedef  pay_pay_pre_out pay_pay_once_out ; 

struct   pay_pay_true_in {
	uint32_t	transid;
	uint32_t	damee;
} __attribute__((packed));


struct   damee_record{
	uint32_t	date;
	uint32_t	paytype;
	int32_t		damee;
	uint32_t	leave_damee;
	char 		desc[20];
} __attribute__((packed));

struct  pay_get_damee_list_in {
	uint32_t	startdate;
	uint32_t	enddate;
	uint32_t	startindex;
	uint32_t	count;
} __attribute__((packed));



struct  msg{
	uint32_t	msglen; 
	char 		msg[1000]; 
}__attribute__((packed));

struct  pay_set_auto_month_dealmsg_in {
	uint32_t	transid; 
	uint32_t	dealflag; 
}__attribute__((packed));

struct key_stru {
	uint32_t	key; 
	uint32_t	value; 
}__attribute__((packed));

struct user_pay_info {
	uint32_t damee;
 	uint32_t month_used;
	uint32_t month_paytype;
	uint32_t month_enabledate;
 	uint32_t month_duedate;
 	uint32_t month_nexterrcount;
}__attribute__((packed));

typedef  user_pay_info pay_get_pay_info_out;

struct  user_get_user_pet_in{
	uint32_t petid; 
}__attribute__((packed)); 
typedef user_get_user_pet_in user_del_user_pet_in; 

typedef stru_count user_get_user_pet_out_header; 

struct  stru_user_pet
{
	uint32_t petid; 
	uint32_t flag;
	uint32_t flag2;
	uint32_t birthday;
	uint32_t logic_birthday;
	char 	 nick[NICK_LEN]; 
	uint32_t color;
	uint32_t sicktime;
	uint8_t pos_x;
	uint8_t pos_y;

	uint8_t hungry;
	uint8_t thirsty;
	uint8_t sanitary;
	uint8_t spirit;
	uint32_t endtime;
	uint32_t starttime;
	uint32_t taskid;
	uint32_t skill;//技巧
	uint32_t stamp;
	uint32_t sick_type;
	uint32_t fire_skill_bitflag; ///< 火系技能点
}__attribute__((packed)); 
typedef  stru_user_pet user_get_user_pet_out_item ;

typedef stru_user_pet user_set_user_pet_all_in;

//------------------------------------------------------------------
//设置宠物装扮，同时返回当前装扮
struct user_pet_attire_set_used_in_header
{
	uint32_t petid;
	uint32_t is_colth;
	uint32_t count;
}__attribute__((packed));

struct  user_pet_attire_set_used_in_item
{
	uint32_t attireid;
}__attribute__((packed));

struct user_pet_attire_set_used_out_header
{
	uint32_t petid;
	uint32_t count;
}__attribute__((packed));

struct user_pet_attire_set_used_out_item
{
	uint32_t attireid;
}__attribute__((packed));

//---------------------------------------------------------------------------------
struct  user_add_user_pet_in
{
	uint32_t flag;
	uint32_t flag2;
	uint32_t birthday;
	char 	 nick[NICK_LEN]; 
	uint32_t color;
	uint32_t sicktime;
	uint8_t pos_x;
	uint8_t pos_y;

	uint8_t hungry;
	uint8_t thirsty;
	uint8_t sanitary;
	uint8_t spirit;
	uint32_t endtime;
	uint32_t starttime;
	uint32_t taskid;
	uint32_t skill;
	uint32_t stamp;
	uint32_t sick_type;
}__attribute__((packed));

struct  user_task_item{
	uint32_t petid;
	uint32_t taskid;
	uint32_t flag;
	uint32_t usetime;
	uint32_t startime;
	uint32_t endtime;
}__attribute__((packed));


struct  user_pet_task_set_in
{
	uint32_t petid;
	uint32_t taskid;
	uint32_t flag;
	uint32_t usetime;
	uint32_t startime;
	uint32_t endtime;
	/*if usetime >=def_usetime then flag=def_flag  */
	uint32_t def_usetime;
	uint32_t def_flag;
//	uint32_t attireid; ///< 任务完成后送出的物品
}__attribute__((packed));


struct  user_pet_task_set_out
{
	uint32_t petid;
	uint32_t taskid;
}__attribute__((packed));


struct  user_pet_task_get_list_in
{
	uint32_t petid;
	uint32_t start_taskid;
	uint32_t end_taskid;
	uint32_t flag;
}__attribute__((packed));

struct   user_connect_get_info_out
{
	userid_t parentid; 
	uint32_t old_childcount; 
	uint32_t childcount; 
}__attribute__((packed));

struct    user_connect_set_parent_in{
	userid_t parentid; 
}__attribute__((packed));

struct    user_connect_add_child_in {
	userid_t childid; 
}__attribute__((packed));
struct    user_connect_add_child_out {
	uint32_t 	childcount; 
}__attribute__((packed));




struct  user_add_user_pet_out{
	uint32_t petid; 
}__attribute__((packed)); 

struct  user_set_user_pet_info_in{
	uint32_t petid; 
	uint32_t flag;
	uint32_t flag2;
	uint32_t birthday;
	char 	 nick[NICK_LEN]; 
	uint32_t color;
	uint32_t sicktime;
}__attribute__((packed));

typedef stru_count user_set_user_pet_poslist_in_header;
struct  user_test_proto_out{
	char 	msg[6010];
}__attribute__((packed));


struct  pet_pos_item {
	uint32_t petid; 
	uint8_t pos_x; 
	uint8_t pos_y; 
}__attribute__((packed));



struct  user_set_user_pet_comm_in{
	uint32_t petid; 
	uint32_t flag;
	uint32_t flag2;
	uint32_t sicktime;
	uint8_t  hungry;
	uint8_t  thirsty;
	uint8_t  sanitary;
	uint8_t  spirit;
	uint32_t endtime;
	uint32_t starttime;
	uint32_t taskid;
	uint32_t stamp;
}__attribute__((packed));

struct  user_set_user_pet_skill_in{
	uint32_t petid; 
	uint32_t skill;/*技巧*/
}__attribute__((packed));

struct  user_set_user_pet_skill_out{
	uint32_t skill;/*技巧*/
}__attribute__((packed));



struct  usermsg_set_report_msg_in{
	userid_t  objuserid;
	uint32_t  reason;
}__attribute__((packed));

struct   usermsg_add_writing_in_header{
		uint32_t type;
		uint32_t msglen ;
} __attribute__((packed));

typedef char_stru usermsg_add_writing_in_item;

struct   su_add_msg_attime_in_header{
		uint32_t deal_date;
		uint32_t deal_hour;
		uint32_t deal_minute;
		uint32_t flag; 
		uint32_t pic_id; 
		uint32_t msglen ;
} __attribute__((packed));

struct  usermsg_get_writing_in{
		uint32_t type;
		uint32_t srcuserid;
		char    date[21];
} __attribute__((packed));

struct  usermsg_searchkey_item{
		userid_t userid;
		uint32_t startdate;
		uint32_t enddate;
		uint32_t flag;
		uint32_t type;
		char     search_key[30];
		uint32_t index;
} __attribute__((packed));


typedef usermsg_searchkey_item usermsg_get_writing_count_in;
typedef usermsg_searchkey_item usermsg_get_writing_list_in;
typedef stru_count usermsg_get_writing_count_out;


struct     usermsg_clear_writing_in{
		uint32_t startdate;
		uint32_t enddate;
} __attribute__((packed));



struct  su_get_msg_attime_all_in{
		uint32_t index;
} __attribute__((packed));

struct  su_get_msg_attime_all_out{
		uint32_t deal_date;
		uint32_t deal_hour;
		uint32_t deal_minute;
		uint32_t flag;
		uint32_t pic_id;
		uint32_t msglen;
		char 	msg[4000];
} __attribute__((packed));

struct  su_del_msg_attime_in{
		uint32_t deal_date;
		uint32_t deal_hour;
		uint32_t deal_minute;
} __attribute__((packed));
struct     su_get_msg_attime_by_date_in{
		uint32_t deal_date;
		uint32_t deal_hour;
		uint32_t deal_minute;
		uint32_t index;
} __attribute__((packed));

struct    get_list_by_date_in{
		uint32_t startdate;
		uint32_t enddate;
		uint32_t index;
} __attribute__((packed));

struct    usermsg_get_report_by_date_in  {
		uint32_t startdate;
		uint32_t enddate;
		uint32_t sort_index;
		uint32_t index;
} __attribute__((packed));



typedef get_list_by_date_in usermsg_get_report_by_date_userid_in;
typedef get_list_by_date_in usermsg_clear_by_date_userid_in;
typedef get_list_by_date_in usermsg_get_deal_report_list_in;

struct  usermsg_get_deal_report_list_item {
		char     date[21];
		uint32_t objuserid;
		uint32_t adminid;
		uint32_t deal_type;	
		uint32_t reason;	
		char 	nick[NICK_LEN];	
} __attribute__((packed));


struct  usermsg_get_report_by_date_out_item{
		uint32_t userid;
		uint32_t count;
		uint32_t one_count;
		uint32_t reason1_count;
		uint32_t reason2_count;
		uint32_t reason3_count;
		uint32_t reason4_count;
} __attribute__((packed));

struct  usermsg_get_report_by_date_userid_out_item{
		char    date[21];
		uint32_t srcuserid;
		uint32_t reason;
} __attribute__((packed));

struct  usermsg_set_del_writing_in_header{
		uint32_t srcuserid;
		char    date[21];
		uint32_t msglen;
} __attribute__((packed));

struct  usermsg_add_report_deal_in{
		uint32_t amdinid;
		uint32_t deal_type;	
		uint32_t reason;	
		char  	nick[NICK_LEN];	
} __attribute__((packed));



struct usermsg_set_writing_report_in_header{
		char     date[21];
		uint32_t type;
		uint32_t flag;
		uint32_t msglen;
} __attribute__((packed));


struct  usermsg_get_writing_out{
		char     date[21];
		uint32_t type;
		uint32_t flag;
		uint32_t msgid;
		userid_t userid;
		uint32_t msglen;
		uint32_t reportlen;
		char 	msg[4020];
} __attribute__((packed));

struct   usermsg_get_writing_list_out_item{
		char     date[21];
		uint32_t type;
		uint32_t flag;
		uint32_t msgid;
		userid_t userid;
		char 	msg[560];
		char 	reportmsg[200];
} __attribute__((packed));



struct  sysarg_day_add_in{
	uint32_t	type;
	uint32_t	maxvalue;
}__attribute__((packed));

typedef type_stru sysarg_get_count_in;

struct   type_value_stru{
	uint32_t	type;
	int32_t		value;
}__attribute__((packed));

typedef type_value_stru sysarg_get_count_out;



struct   sysarg_get_count_list_in{
	uint32_t	start_type;
	uint32_t	end_type;
}__attribute__((packed));



struct  sysarg_set_count_in{
	uint32_t	type;
	int32_t		add_value;
}__attribute__((packed));

struct is_existed_stru{
		uint32_t is_existed;
} __attribute__((packed));
typedef is_existed_stru stru_is_existed;

struct stru_is_seted{
		uint32_t is_seted;
} __attribute__((packed));

struct bool_stru{
		uint32_t bool_value; 
} __attribute__((packed));







typedef type_stru sysarg_day_query_in;

typedef stru_count sysarg_day_query_out ; 
typedef is_existed_stru userinfo_is_set_user_birthday_out ; 
//TEMP

struct  cup_add_cup_in{
		uint32_t groupid;
		uint32_t gameid;
		uint32_t cup_type;/*1-3:金银铜*/
} __attribute__((packed));

struct  cup_add_cup_out{
		uint32_t add_succ_flag;
} __attribute__((packed));


struct cup_get_cup_out_header{
	cup_stu  cup_list;
	uint32_t groupcount;
} __attribute__((packed));

struct  emailsys_add_email_in_header{
	uint32_t	type; /*卡片类型*/
	uint32_t	sendtime;
	userid_t	senderid; 
	char		sendernick[NICK_LEN];
	uint32_t	mapid; /**/
	uint32_t	msglen;
}__attribute__((packed));
typedef stru_flag emailsys_add_email_out;

struct  emailsys_emailid{
	uint32_t	emailid;
}__attribute__((packed));

typedef  emailsys_emailid emailsys_get_email_in;
typedef	 user_email_item emailsys_get_email_out;

typedef  emailsys_emailid emailsys_del_email_in;

struct   emailsys_get_emailid_list_out_header{
	uint32_t	noread_count;
	uint32_t	read_count;
}__attribute__((packed));

struct hope_add_hope_in{
       uint32_t send_id;
       char 	send_nick[NICK_LEN];
       uint32_t recv_id;
       uint32_t recv_type;
       char 	recv_type_name[30];
}__attribute__((packed));
struct hope_add_hope_out{
       uint32_t recv_id;
       char 	recv_type_name[30];
}__attribute__((packed));

struct hope_get_hope_list_out_item {
       uint32_t hopedate;
       uint32_t send_id;
       char 	send_nick[NICK_LEN];
       uint32_t recv_type;/*摩尔豆：0*/
}__attribute__((packed));

typedef stru_count   hope_get_hope_all_out_header;
struct  hope_get_hope_all_out_item {
       uint32_t hopedate;
       uint32_t send_id;
       char 	send_nick[NICK_LEN];
       uint32_t recv_type;/*摩尔豆:0*/
       char 	recv_type_name[30];
       uint32_t useflag;
}__attribute__((packed));



struct hope_get_hope_by_date_out_item{
       uint32_t recv_id;
       uint32_t send_id;
       char 	send_nick[NICK_LEN];
       uint32_t recv_type;
       char 	recv_type_name[30];
}__attribute__((packed));

struct hope_set_hope_used_in{
       uint32_t send_id;
       uint32_t recv_id;
       uint32_t hopedate;
}__attribute__((packed));

struct hope_check_hope_in{
       uint32_t send_id;
}__attribute__((packed));

struct  hope_get_hope_list_in{
       uint32_t recv_id;
}__attribute__((packed));
typedef  stru_count hope_get_hope_list_out_header;

struct hope_set_hope_used_out{
       uint32_t recv_id;
       uint32_t recv_type;
}__attribute__((packed));

struct hope_get_hope_list_by_date_in{
       uint32_t logdate;
       uint32_t index;
}__attribute__((packed));

struct userinfo_set_payinfo_in{
	char 			parent_email[EMAIL_LEN];
	char 			question[QUESTION_LEN];
	char 			answer[ANSWER_LEN];
	uint32_t 		birthday; 
	char 			telephone[TELEPHONE_LEN]; 
	char 			mobile[MOBILE_LEN]; 
	uint32_t		mail_number;
	uint16_t		addr_province;
	uint16_t		addr_city;
	char 			addr[ADDR_LEN];
	char 			interest[INTEREST_LEN];
}__attribute__((packed));


struct userinfo_check_question_in{
	char 			question[QUESTION_LEN];
	char 			answer[ANSWER_LEN];
}__attribute__((packed));
typedef   stru_flag userinfo_get_security_flag_out;
struct userinfo_set_pemail_by_qa_in{
	char 			parent_email[EMAIL_LEN];
	char 			question[QUESTION_LEN ];
	char 			answer[ANSWER_LEN];
}__attribute__((packed));

struct  userinfo_get_nick_flist_out{
	char		nick[NICK_LEN];
	id_list 	fiend_list;
}__attribute__((packed));

struct  user_get_nick_flist_out{
	char		nick[NICK_LEN];
	id_list 	fiend_list;
}__attribute__((packed));


struct other_send_email_in{
	uint32_t flag;
	uint32_t add_level;

	uint32_t vip_start_time;///< 成为VIP的开始时间
	uint32_t vip_end_time;///< VIP的结束时间
	uint8_t  vip_is_auto_charge; ///< 是否自动续费
	uint32_t vip_charge_changel_last;///< 上次充值通道
}__attribute__((packed));

struct confect_change_value_in{
	uint32_t vipflag;
	uint32_t type;
	int32_t  value;
}__attribute__((packed));

struct confect_change_value_out{
	uint32_t flag;
	int32_t  real_add_value;
	uint32_t count_all;
}__attribute__((packed));


struct confect_get_value_out{
	uint32_t flag;
	uint32_t task;
	uint32_t count_all;
}__attribute__((packed));

struct confect_set_task_in{
	uint32_t task;
}__attribute__((packed));


struct user_home_attire_change_in{
	uint32_t		attireid;
	uint32_t		changeflag; /*0:减少未使用.	1:增加未使用, 2:减少已使用 */
	uint32_t		value; 
	uint32_t		maxvalue; /*只在1中使用*/
}__attribute__((packed));

struct msgboard_get_msglist_userid_in{
	uint32_t userid;
	uint32_t boardid;
	uint32_t pageid;
}__attribute__((packed));

typedef stru_count   msgboard_get_msglist_userid_out_header;

typedef stru_msgboard_item  msgboard_get_msglist_userid_out_item;

struct  other_sync_vip_in{
	uint32_t vipflag;
}__attribute__((packed));
struct  pp_set_vip_in{
    uint32_t flag;
    uint32_t vip_month_count;
}__attribute__((packed));


struct user_set_used_homeattire_in_header{
	uint32_t homeid;
	uint32_t count;
}__attribute__((packed));

typedef home_attire_item user_set_used_homeattire_in_item;

typedef stru_count   user_set_used_jyattire_in_header;
typedef home_attire_item user_set_used_jyattire_in_item;



struct msgboard_get_msg_nocheck_in{
	uint32_t msgid;//消息ID
}__attribute__((packed));
struct msgboard_get_msg_nocheck_out{
	char   	 msg[1024];  
}__attribute__((packed));

struct  emailsys_get_email_msg_out{
	uint32_t	email_unread;//没有读过个数
}__attribute__((packed));


struct  roominfo_get_hot_out{
	uint32_t	hot;
	uint32_t	flower;
	uint32_t	mud;
}__attribute__((packed));

struct  roominfo_vote_room_in{
	uint32_t		flag;
}__attribute__((packed));


struct  rooninfo_pug_item{
    uint32_t userid;
    uint32_t type;
    char     value[25];
	inline bool operator != ( const struct rooninfo_pug_item & other  )
	{
			return (this->userid!=other.userid);
	}

	inline bool operator == ( const struct rooninfo_pug_item & other  )
	{
			return (this->userid==other.userid);
	}
}__attribute__((packed));


typedef rooninfo_pug_item   roominfo_pug_add_in;

struct stru_present{
	uint32_t	userid;
	uint32_t 	presentid;	
	char 		value[25];
	char 		present_msg[151];
}__attribute__((packed));

typedef  stru_present roominfo_presentlist_add_in;





struct stru_presentlist{
	uint32_t	count;
	stru_present items[20];
}__attribute__((packed));

struct   roominfo_pug_list_out{
	uint32_t	count;
	rooninfo_pug_item item[ROOMINFO_PUG_LIST_LEN];
}__attribute__((packed));

typedef stru_count   roominfo_box_get_list_out_header;
typedef  rooninfo_pug_item roominfo_box_get_list_out_item;



struct  confect_set_value_in{
	uint32_t 	task;
	uint32_t 	flag;
	uint32_t	count_all;
}__attribute__((packed));


struct stru_tempgs {
	uint32_t task;
	uint32_t count;
}__attribute__((packed));

typedef  stru_tempgs  tempgs_get_value_out;

struct  tempgs_set_task_in{
	uint32_t task;
}__attribute__((packed));


struct sysarg_add_questionnaire_in_header{
	uint32_t count;
}__attribute__((packed));

struct  sysarg_add_questionnaire_in_item {
	uint32_t questionnaireid;
	uint32_t value;
}__attribute__((packed));

struct  groupmain_gen_groupid_out{
	uint32_t groupid;
}__attribute__((packed));

struct      group_add_group_in{
	userid_t	groupownerid;
	char		groupname[GROUPNAME_LEN];
	uint32_t	type;
	char		groupmsg[GROUPMSG_LEN];
}__attribute__((packed));


struct  group_add_member_in{
	userid_t	groupownerid;
	userid_t	memberid;
}__attribute__((packed));

struct  group_del_member_in{
	userid_t	groupownerid;
	userid_t	memberid;
}__attribute__((packed));

typedef group_msg_item group_set_msg_in;

typedef group_item_without_id group_get_info_out;


struct   userinfo_get_group_out{
	uint32_t	count;
	stru_group_item  groupidlist[53];//
}__attribute__((packed));

struct   user_get_group_out{
	uint32_t	count;
	stru_group_item  groupidlist[53];//
}__attribute__((packed));


struct  userinfo_jion_group_in{
	userid_t	groupid;
	uint32_t   	is_add_group;
}__attribute__((packed));

struct  user_join_group_in{
	userid_t	groupid;
	uint32_t   	is_add_group;
}__attribute__((packed));



struct  userinfo_leave_group_in{
	userid_t	groupid;
}__attribute__((packed));

struct  group_del_group_in{
	userid_t	groupownerid;
}__attribute__((packed));

struct  user_leave_group_in{
	userid_t	groupid;
}__attribute__((packed));


struct  userinfo_check_friend_existed_in{
	userid_t	friendid;
}__attribute__((packed));
struct  userinfo_check_friend_existed_out{
	uint32_t  is_existed;
}__attribute__((packed));

struct  user_check_friend_existed_in{
	userid_t	friendid;
}__attribute__((packed));

typedef is_existed_stru user_check_friend_existed_out;

struct  group_check_group_out{
	uint32_t  is_existed;
}__attribute__((packed));

struct  userinfo_add_group_in{
	uint32_t	groupid;
}__attribute__((packed));
struct  userinfo_del_group_in{
	uint32_t	groupid;
}__attribute__((packed));

struct  user_del_group_in{
	uint32_t	groupid;
}__attribute__((packed));


struct  group_get_ownerid_out{
	userid_t	groupownerid;
}__attribute__((packed));

typedef stru_group_member group_get_memberlist_out;
typedef stru_group_member group_add_member_out;
typedef stru_group_member group_del_member_out;
typedef stru_group_member group_del_group_out;

struct  mms_get_info_out{
	uint32_t water;
	uint32_t mud;
	uint32_t get_count;
}__attribute__((packed));

struct  mms_opt_item{
    uint32_t userid;
    uint32_t opt_type;
    char     value[25];
	inline bool operator != ( const struct mms_opt_item & other  )
	{
			return (this->userid!=other.userid);
	}

	inline bool operator == ( const struct mms_opt_item & other  )
	{
			return (this->userid==other.userid);
	}
}__attribute__((packed));



struct mms_opt_in{
	uint32_t  mms_type;
 	mms_opt_item opt_item;
}__attribute__((packed));


struct   mms_opt_list {
	uint32_t	count;
	mms_opt_item  item[20];
}__attribute__((packed));

typedef mms_opt_list mms_get_optlist_out;
struct  mms_get_optlist_in{
	uint32_t  mms_type;
}__attribute__((packed));




typedef stru_count   mms_get_type_list_out_header;
typedef stru_count   mms_get_type_list_ex_out_header;
struct  mms_get_type_list_out_item {
	uint32_t type;
	uint32_t v1;
	uint32_t v2;
	uint32_t v3;
	uint32_t v4;
	uint32_t v5;
}__attribute__((packed));
struct  mms_get_type_list_ex_out_item{
	uint32_t type;
	uint32_t opt_time;
	uint32_t opt_count;
	uint32_t v1;
	uint32_t v2;
	uint32_t v3;
	uint32_t v4;
	uint32_t v5;
}__attribute__((packed));


/* room_info msg */
struct roommsg_add_msg_in
{
    uint32_t    guest_id;
    char        guest_nick[NICK_LEN];
    uint32_t    edit_time;
    char        msg_buf[200];
}__attribute__((packed));

typedef roommsg_add_msg_in  user_classmsg_add_in;

struct roommsg_show_msg_in
{
    uint32_t    msg_id;
    uint8_t    is_show;
}__attribute__((packed));


typedef  roommsg_show_msg_in user_classmsg_set_show_flag_in;

struct  roommsg_list_msg_in{
    uint32_t    request_id;
    uint32_t    msg_count;
}__attribute__((packed));
struct roommsg_list_msg_out_header {
    uint32_t    msg_total;
    uint32_t    msg_count;
}__attribute__((packed));

struct  roommsg_list_msg_out_item {
    uint32_t    msg_id;
    uint32_t    guest_id;
    char        guest_nick[NICK_LEN];
    uint32_t    edit_time;
    uint32_t    show_time;
    char        msg_buf[200];
    char        res_buf[100];
}__attribute__((packed));
typedef roommsg_list_msg_in user_classmsg_get_list_in;
typedef roommsg_list_msg_out_header user_classmsg_get_list_out_header;
typedef roommsg_list_msg_out_item user_classmsg_get_list_out_item;



struct roommsg_del_msg_in
{
    uint32_t    msg_id;
    uint32_t    flag;
}__attribute__((packed));
typedef roommsg_del_msg_in user_classmsg_del_in ;



struct roommsg_res_msg_in
{
    uint32_t    msg_id;
    char        res_buf[100];
}__attribute__((packed));


typedef roommsg_res_msg_in user_classmsg_report_in;

typedef mms_get_type_list_ex_out_item  mms_set_value_in;
struct  roominfo_set_info_in{
	uint32_t	flower;
	uint32_t	mud;
}__attribute__((packed));

typedef stru_count   user_check_attirelist_in_header;
struct  user_check_attirelist_in_item {
    uint32_t    attireid;
}__attribute__((packed));

typedef stru_count   user_check_attirelist_out_header;
typedef  attire_count user_check_attirelist_out_item;

typedef stru_group_item userinfo_set_group_flag_in;

typedef stru_group_item user_set_group_flag_in;

struct  user_set_birthday_in{
    uint32_t    birthday;
}__attribute__((packed));

struct  userinfo_check_paypasswd_in{
	char 		paypasswd[PASSWD_LEN];
}__attribute__((packed));

struct  userinfo_set_paypasswd_in{
	char 		paypasswd[PASSWD_LEN];
}__attribute__((packed));

typedef  stru_is_seted userinfo_isset_paypasswd_out;


struct  userinfo_set_qa_in{
	char 			question[QUESTION_LEN];
	char 			answer[ANSWER_LEN];
}__attribute__((packed));

typedef stru_is_seted user_isset_birthday_out; 

struct  christmas_msg {
	char 			msg[CHRISTMAS_MSG_LEN];
}__attribute__((packed));

typedef christmas_msg christmas_add_msg_in;

struct christmas_get_msg_out {
	userid_t       send_id;	
	char 			msg[CHRISTMAS_MSG_LEN];
}__attribute__((packed));

typedef stru_is_seted christmas_is_get_msg_out;

typedef stru_is_seted  christmas_is_add_msg_out;

struct  userinfo_get_question_out{
	char 			question[QUESTION_LEN];
}__attribute__((packed));

struct  mms_get_info_by_type_in{
	uint32_t mms_type; 
}__attribute__((packed));

struct  mms_get_info_by_type_out{
	uint32_t opt_count;
	uint32_t v1;
	uint32_t v2;
	uint32_t v3;
	uint32_t v4;
	uint32_t v5;
}__attribute__((packed));


typedef mms_get_info_by_type_out mms_opt_out;

struct chat_item{
	userid_t  	send_id;
	userid_t  	recv_id;
	char 		servername[SERVERNAME_LEN];
	char		mapname[MAPNAME_LEN];
	uint32_t  	logtime;
	uint32_t  	msglen;
	char		msg[1024];
} __attribute__((packed));

struct change_log_item{
	uint32_t 	change_type;
	userid_t  	userid;
	uint32_t    logtime;
	int32_t    change_value;
	int32_t    change_v1;
	int32_t    change_v2;
} __attribute__((packed));


struct     log_add_chat_in_header{
	userid_t  	send_id;
	userid_t  	recv_id;
	char 		servername[SERVERNAME_LEN];
	char		mapname[MAPNAME_LEN];
	uint32_t  	logtime;
	uint32_t  	msglen;
} __attribute__((packed));
typedef char_stru log_add_chat_in_item;


typedef change_log_item log_add_change_in;

struct user_cardlist {
    uint32_t count;
    uint32_t cards[CARDLIST_LEN];
}__attribute__((packed));

struct user_card_getinfo_out {
    uint32_t flag;
    uint32_t win_count;
    uint32_t lose_count;
    uint32_t lower_count;
    uint32_t super_count;
	user_cardlist cardList;
}__attribute__((packed));


struct user_card_info{
    uint32_t flag;
    uint32_t win_count;
    uint32_t lose_count;
    uint32_t lower_count;
    uint32_t super_count;
    uint32_t logdate;
    uint32_t dayexp;
	user_cardlist cardList;
}__attribute__((packed));



struct  user_card_add_card_in{
    uint32_t  cardid;
}__attribute__((packed));

struct  user_card_add_win_lose_in{
    uint32_t  win_lose_flag;
}__attribute__((packed));

struct  admin_add_admin_in{
	uint32_t adminid;
	char 	 nick[NICK_LEN];
	char 	 passwd[ PASSWD_LEN ];
}__attribute__((packed));

typedef su_get_admin_list_in admin_get_adminlist_in;
typedef stru_count   admin_get_adminlist_out_header;
typedef   admin_all_item admin_get_adminlist_out_item;


typedef user_change_passwd_in admin_change_passwd_in ;

struct power_item{
	uint32_t	powerid;
}__attribute__((packed));


typedef  stru_passwd  admin_login_in;
struct   admin_login_out_header{
	char       nick[NICK_LEN];
	uint32_t   count; 
}__attribute__((packed));


struct  party_add_party_in{
	uint32_t	serverid;
	uint32_t	partytime;
	uint32_t	mapid;
	uint32_t	partytype;
	uint32_t	ownerid;
	uint32_t	ownerflag;
	uint32_t	ownercolor;
	char 		ownernick[NICK_LEN];
}__attribute__((packed));

struct  party_get_partylist_in{
	uint32_t	serverid;
	uint32_t	startindex;
	uint32_t	pagecount;
}__attribute__((packed));

struct  party_get_count_by_server_in{
	uint32_t	serverid;
}__attribute__((packed));
struct  party_get_count_by_server_out{
	uint32_t	count;
}__attribute__((packed));




typedef stru_count   party_get_partylist_out_header;
struct  party_get_partylist_out_item {
	uint32_t	partytime;
	uint32_t	mapid;
	uint32_t	partytype;
	uint32_t	ownerid;
	uint32_t	ownerflag;
	uint32_t	ownercolor;
	char 		ownernick[NICK_LEN];
}__attribute__((packed));

struct  party_getlist_by_ownerid_in{
	uint32_t	ownerid;
}__attribute__((packed));

typedef stru_count   party_getlist_by_ownerid_out_header;

struct  party_getlist_by_ownerid_out_item {
	uint32_t	serverid;
	uint32_t	partytime;
	uint32_t	mapid;
	uint32_t	partytype;
}__attribute__((packed));
struct  userserial_gen_serial_in{
	uint32_t  type;
}__attribute__((packed));

struct  userserial_gen_serial_out{
	char    serialid[SERIAL_LEN];
}__attribute__((packed));

struct  userserial_del_serial_in{
	uint32_t  type;
}__attribute__((packed));

struct  userserial_get_serial_in{
	uint32_t  type;
}__attribute__((packed));
struct  userserial_get_serial_out{
	char    serialid[SERIAL_LEN];
}__attribute__((packed));

typedef stru_email userinfo_set_passwdemail_in;

typedef stru_is_seted userinfo_isset_passwdemail_out;
typedef stru_is_seted userinfo_isconfirm_passwdemail_out;
typedef stru_email userinfo_get_passwdemail_out;
struct  admin_del_admin_in{
	userid_t adminid;
}__attribute__((packed));


struct  admin_add_power_in{
	uint32_t adminid;
	uint32_t powerid;
}__attribute__((packed));
struct  admin_del_power_in{
	uint32_t adminid;
	uint32_t powerid;
}__attribute__((packed));



typedef power_item admin_login_out_item;


typedef stru_count   admin_get_powerlist_out_header;
typedef power_item admin_get_powerlist_out_item;


struct  admin_root_change_passwd_in{
	uint32_t 	adminid;
	char 		passwd[PASSWD_LEN];
}__attribute__((packed));


struct  user_card_init_in{
    uint32_t init_lowercard_count;
    uint32_t cards[CARDLIST_INIT_LEN];
}__attribute__((packed));

struct  user_card_is_inited_out{
	 uint32_t is_inited;
}__attribute__((packed));



struct  user_card_add_lower_card_out{
	uint32_t		cardid;
}__attribute__((packed));

struct  user_card_get_enable_count_out{
	uint32_t		count;
}__attribute__((packed));

struct  pic_add_pic_in{
	uint32_t albumid ;
	char 	fileid[PIC_FILE_ID_LEN];
}__attribute__((packed));
struct  pic_add_pic_out{
	uint32_t photoid;
}__attribute__((packed));

struct  pic_get_user_ablum_piclist_in{
	uint32_t    albumid;
	uint32_t	startindex;
	uint32_t	count;
}__attribute__((packed));
typedef stru_count   pic_get_user_ablum_piclist_out_header;

struct  pic_get_user_ablum_piclist_out_item {
	uint32_t photoid;
	char 	fileid[PIC_FILE_ID_LEN];
}__attribute__((packed));

struct  pic_get_pic_by_photoid_userid_in{
	uint32_t photoid;
}__attribute__((packed));
struct  pic_get_pic_by_photoid_userid_out{
	uint32_t    albumid;
	char 		fileid[PIC_FILE_ID_LEN];
}__attribute__((packed));

typedef pic_get_pic_by_photoid_userid_in  pic_del_pic_by_photoid_userid_in;


typedef type_stru sysarg_day_clean_by_type_in;

struct  pic_del_pic_by_photoid_userid_out{
	char 		fileid[PIC_FILE_ID_LEN];
}__attribute__((packed));

struct  msgboard_reset_msg_in_header{
		uint32_t boardid;
		uint32_t msgid;  
		uint32_t msglen ;
}__attribute__((packed));
struct  msgboard_reset_msg_in_item {
		char  c;
}__attribute__((packed));

struct  spring_info{
		uint32_t flag;
		uint32_t gold;
		uint32_t silver;  
		char     spring_msg[SPRING_MSG_LEN];
}__attribute__((packed));


struct  spring_get_value_out{
		uint32_t gold;
		uint32_t silver;  
}__attribute__((packed));

struct  spring_change_value_in{
		int32_t change_gold;
		int32_t change_silver;
}__attribute__((packed));
typedef spring_get_value_out spring_change_value_out;


struct  spring_set_msg_in{
		char     spring_msg[SPRING_MSG_LEN];
}__attribute__((packed));

struct  spring_set_msg_out{
	uint32_t is_first_set;
}__attribute__((packed));



struct  admin_set_use_flag_in{
	uint32_t adminid;
	uint32_t useflag;
}__attribute__((packed));

struct  user_card_opt_info {
    uint32_t flag;
    uint32_t win_count;
    uint32_t lose_count;
    uint32_t logdate;
    uint32_t dayexp;
}__attribute__((packed));
typedef user_card_opt_info  user_card_set_win_lose_in;


struct  spring_get_msg_out{
		char     spring_msg[SPRING_MSG_LEN];
}__attribute__((packed));

struct  spring_get_flag_out{
		uint32_t flag;
		uint32_t gold;
		uint32_t silver;  
}__attribute__((packed));

struct spring_stru {
	uint32_t flag;
	uint32_t gold;
	uint32_t gold_logdate;
	uint32_t gold_count_today;

	uint32_t silver;
	uint32_t silver_logdate;
	uint32_t silver_count_today;
	char     spring_msg[SPRING_MSG_LEN];
}__attribute__((packed));


typedef spring_stru spring_get_spring_out;
typedef spring_stru spring_set_spring_in;

struct  roommsg_add_diary_in_header{
	uint32_t 	diarydate;
	char    	feeling[FEELING_LEN];
	uint32_t 	msglen;
}__attribute__((packed));
 
typedef roommsg_add_diary_in_header roommsg_set_diary_in_header;   

struct  roommsg_set_diary_pub_flag_in{
	uint32_t 	public_flag;
}__attribute__((packed));



struct  roommsg_get_diary_list_in{
	uint32_t 	pageid;
}__attribute__((packed));
typedef stru_count   roommsg_get_diary_list_out_header;
struct  roommsg_get_diary_list_out_item {
	uint32_t 	diarydate;
}__attribute__((packed));


struct  roommsg_get_diary_in{
	uint32_t 	diarydate;
}__attribute__((packed));

struct  roommsg_get_diary_out_header{
	char    	feeling[FEELING_LEN];
	uint32_t 	public_flag;
	uint32_t 	msglen;
}__attribute__((packed));
 
struct  admin_get_adminid_by_nick_in{
	char		nick[NICK_LEN];
}__attribute__((packed));
struct  admin_get_adminid_by_nick_out{
	uint32_t adminid;
}__attribute__((packed));

struct  admin_get_adminlist_by_powerid_in{
	uint32_t	powerid;
}__attribute__((packed));

typedef stru_count   admin_get_adminlist_by_powerid_out_header;
typedef stru_id admin_get_adminlist_by_powerid_out_item;

struct  qa_msg_item{
	char 			question[241];
	char 			answer[361];
}__attribute__((packed));

typedef qa_msg_item msgboard_qa_add_msg_in;

struct  msgboard_qa_get_list_in{
	uint32_t	startindex;
	uint32_t	count;
}__attribute__((packed));

typedef stru_count    msgboard_qa_get_list_out_header;
typedef qa_msg_item msgboard_qa_get_list_out_item;
struct  msgboard_qa_del_msg_in{
	uint32_t msgid;
}__attribute__((packed));


struct  user_temp_add_value_out{
	uint32_t cur_count;
}__attribute__((packed));

typedef msgboard_qa_get_list_in  msgboard_qa_get_list_ex_in;
typedef stru_count   msgboard_qa_get_list_ex_out_header;

struct  msgboard_qa_get_list_ex_out_item {
	uint32_t        msgid;	
	char 			question[241];
	char 			answer[361];
}__attribute__((packed));
typedef stru_count msgboard_qa_get_count_out;


struct  user_buy_pet_attire_by_xiaomee_in{
	uint32_t		petid;
	uint32_t		attireid;
	uint32_t		count;
	uint32_t		maxcount;
	uint32_t		usexiaomee; 
}__attribute__((packed));

struct  user_buy_pet_attire_by_xiaomee_out{
	uint32_t	leave_xiaomee; 
	uint32_t	buy_attireid; 
	uint32_t	buy_count; 
}__attribute__((packed));

struct  user_get_pet_attire_list_in{
	uint32_t		petid;
	uint32_t		startattireid;
	uint32_t		endattireid;
	uint32_t		attire_usedflag;
}__attribute__((packed));

struct   user_get_pet_attire_list_out_header{
	uint32_t petid;
	uint32_t count;
} __attribute__((packed));
typedef attire_count user_get_pet_attire_list_out_item;

struct  user_set_pet_attire_in{
	uint32_t petid;
	user_set_attire_in item;
}__attribute__((packed));

typedef attire_property user_set_pet_attire_out;

typedef user_temp_add_value_out  user_temp_get_value_out;

struct  temp_pet_sports_set_score_in{
	uint32_t petid;
	uint32_t sportstype;
	uint32_t score;
	char 	 usernick[NICK_LEN];
	char 	 petnick[NICK_LEN];
}__attribute__((packed));



struct  pet_group_item {
	uint32_t logdate;
	uint32_t pet_group_count_1;
	uint32_t pet_group_count_2;
	uint32_t pet_group_count_3;
	uint32_t pet_group_count_4;
}__attribute__((packed));

typedef stru_count   temp_pet_sports_get_list_out_header;
typedef pet_group_item temp_pet_sports_get_list_out_item;

typedef stru_count   user_get_pet_attire_used_all_out_header;

struct  user_get_pet_attire_used_all_out_item {
	uint32_t petid;
	uint32_t attireid;
	uint32_t count;
}__attribute__((packed));


typedef bool_stru temp_pet_user_reported_out;

typedef stru_count   temp_pet_get_pet_list_out_header;
struct  temp_pet_get_pet_list_out_item {
	uint32_t petid;
}__attribute__((packed));
struct  temp_pet_report_in{
	uint32_t petid;
	uint32_t pet_groupid;
}__attribute__((packed));

typedef stru_count   user_get_pet_attire_all_out_header;
struct  user_get_pet_attire_all_out_item {
	uint32_t petid;
	uint32_t attireid;
	uint32_t usedcount;
	uint32_t allcount;
}__attribute__((packed));

typedef  user_get_pet_attire_all_out_item user_pet_attire_update_ex_in;


typedef  user_card_info  user_card_getinfo_ex_out;


typedef stru_count   temp_pet_get_score_list_out_header;
struct  temp_pet_get_score_list_out_item {
	uint32_t petid;
	uint32_t pet_score_1;
	uint32_t pet_score_2;
	uint32_t pet_score_3;
	uint32_t pet_score_4;
	uint32_t pet_score_5;
}__attribute__((packed));


struct  temp_pet_sports_set_score_out{
	uint32_t is_first_set;
	uint32_t best_score;
}__attribute__((packed));

struct  temp_pet_sports_get_max_score_in{
	uint32_t sportstype;
}__attribute__((packed));

struct  temp_pet_sports_get_max_score_out{
	uint32_t score;
	uint32_t pet_groupid;
	char 	 usernick[NICK_LEN];
	char 	 petnick[NICK_LEN];
}__attribute__((packed));
struct  user_register_ex_in{
	uint32_t petcolor;
}__attribute__((packed));
struct  temp_pet_get_group_allcount_in{
	uint32_t pet_groupid;
}__attribute__((packed));
struct  temp_pet_get_group_allcount_out{
	uint32_t count;
}__attribute__((packed));


struct user_get_info_ex_out_header  {
		uint32_t		item1_count; /**/
		uint32_t		item2_count; /**/
		uint32_t		item3_count; /**/
		uint32_t 		item4_count;
}__attribute__((packed));

struct pet_task_state {
	int8_t taskid;
	int8_t state;
}__attribute__((packed));

struct lamu_task_state {
	uint32_t petid;
	pet_task_state item[20];
}__attribute__((packed));


typedef user_get_attire_list_out_item user_get_info_ex_out_item_1;
typedef user_get_user_pet_out_item user_get_info_ex_out_item_2;
typedef user_get_pet_attire_used_all_out_item user_get_info_ex_out_item_3;
typedef lamu_task_state user_get_info_ex_out_item_4; 
//bank
/**
 * @brief 插入记录时传递给DB的包体 
 */
struct  user_bank_add_in {
	uint32_t xiaomee; /*记录中存储的豆豆数目*/
	uint32_t flag; /*是否允许中途取出*/
	uint32_t time; /*无效字段*/
	uint32_t timelimit; /*存款的期限*/
}__attribute__((packed));

/**
 * @brief 插入完记录返回给用户的包体
 */
typedef user_bank_add_in  user_bank_add_out;

/**
 *  @brief 用户获取记录时返回给用户的包体前部分
 */
struct user_bank_get_account_out_header {
	uint32_t now_time; /*当前时间*/
	uint32_t count; /*记录的总数*/
}__attribute__((packed));

/**
 *  @brief 返回给用户的每条记录信息
 */
typedef user_bank_add_out user_bank_get_account_out_item;

/**
 *  @brief 删除记录时用户传给DB的时间，决定删除哪条记录
 */
struct  user_bank_delete_account_in{
	uint32_t time; /*记录存储时的时间*/
}__attribute__((packed));

/**
 *  @brief 删除记录时返回给用户的包体
 */
struct  user_bank_delete_account_out{
	uint32_t xiaomeeinterest; /*利息的数目*/
	uint32_t totalmoney; /*本金和利息的总数*/
	uint32_t tax;
	uint32_t flag;
}__attribute__((packed));

struct  user_bank_set_account_in{
	uint32_t old_time; /*无效字段*/
	uint32_t xiaomee; /*记录中存储的豆豆数目*/
	uint32_t flag; /*是否允许中途取出*/
	uint32_t time; /*无效字段*/
	uint32_t timelimit; /*存款的期限*/
}__attribute__((packed));

struct  user_set_task_in{
	uint32_t taskid;
	uint32_t value; ///< 0:表示清零，1:表示接收，2:表示完成
}__attribute__((packed));


struct  user_add_seed_in{
	uint32_t attireid;
	uint32_t x;
	uint32_t y;
}__attribute__((packed));

//typedef jy_item  user_add_seed_out;
struct user_add_seed_out {
	jy_item item;
}__attribute__((packed));
struct  user_del_seed_in{
	uint32_t id;
}__attribute__((packed));
struct  user_get_seed_in{
	uint32_t id;
}__attribute__((packed));

typedef jy_item  user_get_seed_out;

struct  user_catch_fruit_in{
	uint32_t id;
}__attribute__((packed));
struct  user_catch_fruit_out{
	uint32_t exp;
	uint32_t attireid;
	uint32_t fruitnum;
}__attribute__((packed));

struct  user_get_jy_noused_list_in{
	uint32_t seed_startid;
	uint32_t seed_endid;
}__attribute__((packed));
typedef stru_count   user_get_jy_noused_list_out_header;
typedef attire_noused_item user_get_jy_noused_list_out_item;
typedef stru_count   user_get_seed_list_out_header;

struct  user_farm_get_nousedlist_in{
	uint32_t animal_startid;
	uint32_t animal_endid;
}__attribute__((packed));
typedef stru_count   user_farm_get_nousedlist_out_header;
typedef attire_noused_item  user_farm_get_nousedlist_out_item;




typedef jy_item  user_get_seed_list_out_item;
struct user_set_seed_ex_in {
	uint32_t id;
	uint32_t attireid;
	uint32_t x;
	uint32_t y;
	uint32_t value;
	uint32_t sickflag;
	uint32_t fruitnum;
	uint32_t cal_value_time;
	uint32_t water_time;
	uint32_t kill_bug_time;
	uint32_t status;
	uint32_t mature_time;
	uint32_t earth;
	uint32_t pollinate_num;
}__attribute__((packed));
//typedef jy_item user_set_seed_ex_in;

struct access_jylist{
	uint32_t	count;
	mms_opt_item  item[50];
}__attribute__((packed));

typedef  access_jylist user_get_access_jylist_out;
typedef mms_opt_item user_access_jy_in;
struct  userinfo_get_gameflag_out{
	uint32_t	gameflag;
}__attribute__((packed));


struct  userinfo_add_game_in{
	uint32_t	gameid_flag;
}__attribute__((packed));

struct  user_login_ex_out{
	uint32_t	vip_flag;
	uint32_t	friendcount;
	uint32_t	frienditem[FRIEND_COUNT_MAX];
}__attribute__((packed));


struct  user_register_ex2_in{
	uint32_t	petcolor;
	char		nick[NICK_LEN];
}__attribute__((packed));

struct  user_home_noused_attire_type_count_out {
	uint32_t	count;
}__attribute__((packed));

typedef is_existed_stru  user_check_existed_ex_out;
struct user_get_attire_list_in {
	uint32_t	attiretype;///< 装扮0, 小屋1
	uint32_t	startattireid;
	uint32_t	endattireid;
	uint8_t		attire_usedflag;
} __attribute__((packed));

//user_task_ex 

/**
 * @brief 更新数据的请求包体
 */

struct stru_user_task_ex{
	uint32_t taskid;
	char data[TASK_LIST_EX_LEN]; ///< 存储客户端私有数据
}__attribute__((packed));


typedef stru_user_task_ex user_task_set_in ;

typedef user_task_set_in user_task_set_out;


/**
 * @brief 删除任务的请求包体
 */
struct  user_task_del_in{
	uint32_t taskid;
}__attribute__((packed));

typedef user_task_del_in user_task_del_out;
/**
 * @brief 得到任务的请求包体
 */
struct  user_task_get_in{
	uint32_t taskid;
}__attribute__((packed));

/**
 * @brief 得到任务的返回包体
 */
struct  user_task_get_out{
	uint32_t taskid;	
	char data[TASK_LIST_EX_LEN];
}__attribute__((packed));

struct  history_passwd_item{
	char passwd[PASSWD_LEN];	
	uint32_t logtime;	
	inline bool operator != ( const struct  history_passwd_item & other  )
	{
		return (memcmp(this->passwd,other.passwd,PASSWD_LEN)!=0);
	}

	inline bool operator == ( const struct  history_passwd_item & other  )
	{
		return (memcmp(this->passwd,other.passwd,PASSWD_LEN)==0);
		//	return (this->passwd==other.passwd);
	}

}__attribute__((packed));

struct  history_passwd{
	history_passwd_item passwditem[3];
}__attribute__((packed));
typedef history_passwd  userinfo_get_passwd_history_out ;


struct  history_ip_item{
	uint32_t ip;	
	uint32_t logtime;	
	inline bool operator != ( const struct  history_ip_item & other  )
	{
			return (this->ip!=other.ip);
	}

	inline bool operator == ( const struct  history_ip_item & other  )
	{
			return (this->ip==other.ip);
	}

}__attribute__((packed));

struct  history_ip{
	history_ip_item ipitem[3];
}__attribute__((packed));

struct  user_get_tempbuf_out{
	char		tmpbuf[40];
}__attribute__((packed));


typedef user_get_tempbuf_out user_set_tempbuf_in;

typedef history_ip  user_get_ip_history_out;



struct  user_get_xiaomee_out{
	uint32_t xiaomee;
}__attribute__((packed));

struct  user_get_vip_nick_out{
	uint32_t vipflag;
	char nick[NICK_LEN];
}__attribute__((packed));


typedef stru_count user_pet_task_get_list_out_header; 
typedef user_task_item user_pet_task_get_list_out_item;


/**
 * @brief 设置宠物FLAG标志请求包体
 */
struct  user_pet_set_flag_in{
	uint32_t petid;
	uint32_t flag;
}__attribute__((packed));
/* 
 * @brief 设置宠物FLAG标志的应答包体
 */
typedef user_get_user_pet_out_item user_pet_set_flag_out;


/**
 * @brief 设置宠物的FLAG2标志请求包体
 */
struct  user_pet_set_flag2_in{
	uint32_t petid;
	uint32_t flag2;
}__attribute__((packed));

/*
 * @brief 设置FLAG2的应答包体
 */
typedef user_get_user_pet_out_item user_pet_set_flag2_out;

/**
 * @brief 设置宠物生日的请求包体
 */
struct  user_pet_set_birthday_in{
	uint32_t petid;
	uint32_t birthday;
}__attribute__((packed));

/*
 * @brief 设置宠物生日的应答包体
 */
typedef user_get_user_pet_out_item user_pet_set_birthday_out;


/**
 * @brief 设置宠物名称的请求包体
 */
struct  user_pet_set_nick_in{
	uint32_t petid;
	char nickname[NICK_LEN];
}__attribute__((packed));

/* 
 * @brief 设置宠物名称的应答包体
 */
typedef user_get_user_pet_out_item user_pet_set_nick_out;

/**
 * @brief 设置宠物的颜色的请求包体
 */
struct  user_pet_set_color_in{
	uint32_t petid;
	uint32_t color;
}__attribute__((packed));
/*
 * @brief 设置宠物颜色的应答包体
 */
typedef user_get_user_pet_out_item user_pet_set_color_out;


/**
 * @brief 设置宠物生病时间的请求包体
 */
struct  user_pet_set_sicktime_in{
	uint32_t petid;
	uint32_t sicktime;
}__attribute__((packed));
/*
 * @设置宠物生病时间的应答包体
 */
typedef user_get_user_pet_out_item user_pet_set_sicktime_out;

/**
 * @brief 设置终止时间的请求包体
 */
struct  user_pet_set_endtime_in{
	uint32_t petid;
	uint32_t endtime;
}__attribute__((packed));

/*
 * @brief 设置终止时间的应答包体
 */
typedef user_get_user_pet_out_item user_pet_set_endtime_out;

/**
 * @brief 设置心情值等
 */
struct  user_pet_set_life_in{
	uint32_t petid;
	int32_t change_hungry;
	int32_t change_thirsty;
	int32_t change_sanitary;
	int32_t change_spirit;
	uint32_t stamp;
}__attribute__((packed));

/**
 * @brief 设置心情值应答包体
 */
typedef user_get_user_pet_out_item user_pet_set_life_out;


/*
 * @brief 喂养宠物请求包
 */
struct  user_pet_feed_pet_in{
	uint32_t petid;
	uint32_t flag;
	uint32_t sicktime;
	uint32_t stamp;
	uint32_t change_hungry;
	uint32_t change_thirsty;
	uint32_t change_sanitary;
	uint32_t change_spirit;
	uint32_t feed_userid;
}__attribute__((packed));

/*
 * @brief 喂养宠物的应答包
 */
typedef user_get_user_pet_out_item user_pet_feed_pet_out;

/* 
 * @brief 设置宠物托管/领回的请求包
 */
struct  user_pet_retrieve_pet_in{
	uint32_t petid;
	uint32_t flag;
	uint32_t endtime;
	uint32_t stamp;
	uint32_t change_hungry;
	uint32_t change_thirsty;
	uint32_t change_sanitary;
	uint32_t change_spirit;
}__attribute__((packed));

/*
 * @brief 设置宠物托管/领回的应答包
 */
typedef user_get_user_pet_out_item user_pet_retrieve_pet_out;

/*
 * @brief 接宠物任务的请求包体
 */
struct  user_pet_get_pet_in{
	uint32_t petid;
	uint32_t flag;
	uint32_t endtime;
	uint32_t starttime;
	uint32_t taskid;
	uint32_t stamp;
	uint32_t change_hungry;
    uint32_t change_thirsty;
    uint32_t change_sanitary;
    uint32_t change_spirit;
}__attribute__((packed));

/* 
 * @brief 接宠物任务的应答包体
 */
typedef user_get_user_pet_out_item user_pet_get_pet_out;

/*
 * @brief 宠物出游的请求包体
 */
struct  user_pet_travel_pet_in{
	uint32_t petid;
	uint32_t flag;
	uint32_t endtime;
}__attribute__((packed));

/*
 * @brief 宠物出游的应答包体
 */
typedef user_get_user_pet_out_item user_pet_travel_pet_out;

/* 
 * @brief 存储密码修改记录
 */
/* struct passwd_add_change_in {
	uint32_t time;
	char passwd[PASSWD_LEN];
} __attribute__((packed));*/

struct passwd_change_item {
	uint32_t userid;
	uint32_t opt_type;
	uint32_t time;
	char passwd[PASSWD_LEN];
}__attribute__((packed));

typedef passwd_change_item passwd_add_change_in;

typedef stru_appeal  appeal_add_appeal_in;
typedef stru_id appeal_add_appeal_out;

typedef stru_appeal_ex  appeal_ex_add_appeal_in;
typedef stru_id appeal_ex_add_appeal_out;
struct  appeal_get_list_in{
	uint32_t state;
	uint32_t dealflag;
	uint32_t userid;
	uint32_t pageid;
}__attribute__((packed));


typedef stru_count   appeal_get_list_out_header;
typedef stru_appeal appeal_get_list_out_item; 
struct  appeal_ex_get_list_in{
	uint32_t state;
	uint32_t userid;
	uint32_t pageid;
}__attribute__((packed));
typedef stru_count   appeal_ex_get_list_out_header;
typedef stru_appeal_ex appeal_ex_get_list_out_item; 



typedef stru_id appeal_get_appeal_in;
typedef stru_appeal  appeal_get_appeal_out; 

struct user_swap_list_in_header{
	uint32_t item1_count;
	uint32_t item2_count;
	uint32_t reason;
	uint32_t reason_ex;
}__attribute__((packed));

struct  user_swap_list_in_item_1 {
	uint32_t	attiretype;/*装扮0,小屋1*/
	uint32_t	attireid;
	uint32_t	count;
}__attribute__((packed));

struct  user_swap_list_in_item_2 {
	uint32_t	attiretype;/*装扮0,小屋1*/
	uint32_t	attireid;
	uint32_t	count;
	uint32_t	maxcount;
}__attribute__((packed));
struct  hope_set_hope_ex_in{
	uint32_t send_id;
    uint32_t recv_id;
    uint32_t old_hopedate;
    uint32_t new_hopedate;
    uint32_t recv_type;
}__attribute__((packed));

struct  appeal_deal_in{
	uint32_t	id;
	uint32_t	state ;
	uint32_t	dealflag ;
	uint32_t	adminid;
}__attribute__((packed));

struct  appeal_ex_deal_in{
	uint32_t	id;
	uint32_t	state;
	uint32_t	adminid;
}__attribute__((packed));



struct  get_userid_by_email_out{
	uint32_t	map_userid;
}__attribute__((packed));

struct  user_pet_get_info_in{
	uint32_t petid;	
}__attribute__((packed));
struct user_pet_get_info_out_header{
	stru_user_pet	pet_info; 
	uint32_t attirecount;
}__attribute__((packed));

typedef attire_count user_pet_get_info_out_item;

//question
/** 
 * @brief 设置答对题目的请求包
 */
struct  roominfo_question_set_in{
	uint32_t right;
	uint32_t question_id;
}__attribute__((packed));

/**
 * @brief 得到用户答题的返回包
 */
struct  roominfo_question_get_out{
	uint32_t question_id;
	uint32_t right_count;
	uint32_t question_count;
}__attribute__((packed));


typedef rooninfo_pug_item   roominfo_box_add_in;

/*
struct stru_attire_count_list{
	attire_count attire_item[4][20]; 
};

struct stru_attire_count_max_list{
	attire_count_with_max attire_max_item[4][20]; 
};
  */

//struct  roominfo_set_jy_state_in{
//	uint32_t state;
//	uint32_t fire;
//	uint32_t pos;
//}__attribute__((packed));
//
//struct  roominfo_set_jy_state_out{
//	uint32_t state;
//	uint32_t fire;
//	uint32_t pos;
//	uint32_t alive_state;
//}__attribute__((packed));
//
//struct  roominfo_set_jy_fire_in{
//	uint32_t fire;
//}__attribute__((packed));

/* struct roominfo_set_jy_fire_out {
	uint32_t state;
	uint32_t fire;
}__attribute__((packed));*/
//struct  roominfo_set_jy_fire_out{
//	uint32_t state;
//	uint32_t fire;
//	uint32_t pos;
//	uint32_t alive_die;
//}__attribute__((packed));


//typedef roominfo_set_jy_state_out roominfo_set_jy_fire_out;

//typedef roominfo_set_jy_fire_out roominfo_jy_fire_get_out;

struct  user_jy_access_set_opt_in{
	uint32_t opt_userid;
	uint32_t opt_type;
}__attribute__((packed));


typedef stru_count   user_get_friend_list_out_header;

typedef stru_id user_get_friend_list_out_item;
struct  dwg_sendmsg_to_friends_in_header {
	uint32_t  flag;
	char  msg[2000];
	uint32_t count; 
}__attribute__((packed));
typedef stru_id dwg_sendmsg_to_friends_in_item ;

struct  stru_user_profession_list{
	uint32_t profession_value[PROFESSION_COUNT];
}__attribute__((packed));


typedef  stru_user_profession_list user_profession_get_out;

struct  user_profession_set_in{
	uint32_t profession_id;
	uint32_t profession_value;
}__attribute__((packed));

typedef stru_count   sysarg_get_questionnaire_list_out_header;
struct  sysarg_get_questionnaire_list_out_item{
	uint32_t maintype;
	uint32_t subtype;
	uint32_t count;
}__attribute__((packed));

typedef stru_count   sysarg_day_get_list_by_userid_out_header;
struct  sysarg_day_get_list_by_userid_out_item{
	uint32_t type;
	uint32_t count;
}__attribute__((packed));

struct  sysarg_day_set_in{
	uint32_t type;
	uint32_t count;
}__attribute__((packed));

typedef stru_email user_set_passwdemail_ex_in;

struct  su_changelist_add_in{
    uint32_t type;
    uint32_t cmdid;
    uint32_t adminid;
    uint32_t userid;
    int32_t v1;
    int32_t v2;
    int32_t v3;
}__attribute__((packed));

struct  user_class_create_in{
	uint32_t interest;
	uint32_t class_logo;
	uint32_t class_word;
	uint32_t class_color;
	uint32_t class_jion_flag;
	uint32_t class_access_flag;
	char 	 class_name[CLASS_NAME_LEN];
	char 	 class_slogan[CLASS_SLOGAN_LEN];
}__attribute__((packed));

struct  user_class_change_info_in{
	uint32_t class_jion_flag;
	uint32_t class_access_flag;
	char class_name[CLASS_NAME_LEN];
	char class_slogan[CLASS_SLOGAN_LEN];
}__attribute__((packed));

struct  stru_user_class_simple_info{
	uint32_t interest;
	uint32_t class_logo;
	uint32_t class_color;
	uint32_t class_word;
	uint32_t class_jion_flag;
	uint32_t class_access_flag;
	char 	class_name[CLASS_NAME_LEN];
	char 	class_slogan[CLASS_SLOGAN_LEN];
	char onwernick[NICK_LEN];
}__attribute__((packed));
	
struct  stru_user_class_info{
	stru_user_class_simple_info simple_info;
	id_list memberlist;	
	home_attirelist home_used_list;
}__attribute__((packed));


struct  user_class_get_info_out_header{
	stru_user_class_simple_info simple_info;
	uint32_t		item1_count;
	uint32_t		item2_count;
}__attribute__((packed));

struct  user_class_get_info_out_item_1{
	userid_t  memberid;
}__attribute__((packed));

typedef home_attire_item user_class_get_info_out_item_2;


typedef stru_count   roominfo_presentlist_get_out_header;
typedef stru_present roominfo_presentlist_get_out_item;
struct  user_class_add_member_in{
	userid_t  memberid;
}__attribute__((packed));

struct  user_class_del_member_in{
	userid_t  memberid;
}__attribute__((packed));

struct  user_class_add_attire_in{
	attire_count_with_max opt_item;
	userid_t  	memberid;
	uint8_t    value[25];
}__attribute__((packed));

struct  user_add_class_in{
	uint32_t classid;
}__attribute__((packed));
struct  user_del_class_in{
	uint32_t classid;
}__attribute__((packed));


struct  user_class_get_flag_info_out{
	uint32_t class_logo;
	uint32_t class_color;
	uint32_t class_word;
	uint32_t class_jion_flag;
	uint32_t class_access_flag;
	uint32_t class_member_count;
}__attribute__((packed));

struct  user_set_first_class_in{
	uint32_t class_firstid;
}__attribute__((packed));

struct  user_get_frist_class_out{
	uint32_t classid;
}__attribute__((packed));

struct user_class_edit_home_in_header {
	uint32_t item1_count;
}__attribute__((packed));



typedef home_attire_item  user_class_edit_home_in_item_1;

typedef stru_count   sysarg_get_count_list_out_header;
struct  sysarg_get_count_list_out_item{
	uint32_t	type;
	int32_t		value;
}__attribute__((packed));


typedef stru_count   sysarg_month_task_get_list_out_header;
typedef sysarg_day_get_list_by_userid_out_item  sysarg_month_task_get_list_out_item;


struct  user_class_set_access_flag_in{
	uint32_t class_jion_flag;
}__attribute__((packed));


typedef sysarg_day_add_in sysarg_month_task_add_in;

struct sysarg_month_task_add_out {
	uint32_t type;
}__attribute__((packed));

struct  ff_login_add_in{
	uint32_t	loginflag;
	uint32_t	onlineid;
	uint32_t	logintime;
}__attribute__((packed));

typedef stru_count   user_class_get_attirelist_out_header;
typedef attire_count user_class_get_attirelist_out_item;

struct  user_class_reduce_attire_in{
	uint32_t attireid;
	uint32_t count;
}__attribute__((packed));

struct  user_class_get_simple_info_out{
	uint32_t class_logo;
	uint32_t class_color;
	uint32_t class_word;
	uint32_t class_jion_flag;
	uint32_t class_access_flag;
	char 	class_name[CLASS_NAME_LEN];
	char 	class_slogan[CLASS_SLOGAN_LEN];
	char 	onwernick[NICK_LEN];
	uint32_t class_member_count;
}__attribute__((packed));
struct  user_get_first_class_out{
		uint32_t 		class_firstid;  //首班级ID
}__attribute__((packed));


typedef stru_count   user_class_del_out_header;
struct  user_class_del_out_item{
	userid_t  memberid;
}__attribute__((packed));

typedef stru_count   user_class_get_memberlist_out_header;
struct  user_class_get_memberlist_out_item{
	userid_t  memberid;
}__attribute__((packed));

struct user_get_classlist_out_header{
	uint32_t class_firstid;
	uint32_t count;
}__attribute__((packed));
struct  user_get_classlist_out_item{
	uint32_t classid;
}__attribute__((packed));
struct  stru_user_class_list{
	uint32_t class_firstid;
	stru_classlist classlist;
}__attribute__((packed));

/* 偷取种子的ID号 */
struct thiever_info {
    uint32_t count;
    uint32_t thiever[20];
};

struct  user_thieve_fruit_in{
	uint32_t id;
	uint32_t thiever_id;
}__attribute__((packed));

struct  user_thieve_fruit_out{
	uint32_t fruitid;
	jy_item item;
}__attribute__((packed));

/* 产生超级拉姆*/
struct  croominfo_gen_vip_out{
	uint32_t vip_flag;
}__attribute__((packed));

typedef croominfo_gen_vip_out roominfo_check_if_gen_vip_out;

struct  roominfo_class_qa_add_score_in{
	uint32_t add_score;
	uint32_t memberid;
}__attribute__((packed));
struct  roominfo_class_qa_add_score_out{
	uint32_t  member_score;
}__attribute__((packed));



struct  roominfo_class_qa_check_add_in{
	uint32_t memberid;
}__attribute__((packed));
typedef stru_is_existed roominfo_class_qa_check_add_out;

typedef stru_count   user_class_get_random_top_out_header;
struct  user_class_get_random_top_out_item{
	uint32_t class_logo;
	uint32_t class_color;
	uint32_t class_word;
	uint32_t class_jion_flag;
	uint32_t class_access_flag;
	char 	class_name[CLASS_NAME_LEN];
	char 	class_slogan[CLASS_SLOGAN_LEN];
	char 	onwernick[NICK_LEN];
	uint32_t class_member_count;
}__attribute__((packed));

struct  roominfo_class_qa_get_info_out{
	uint32_t all_score;
}__attribute__((packed));

typedef home_attirelist farm_used_attirelist;
typedef noused_homeattirelist farm_noused_attirelist;

/*struct user_farm_goods {*/
/*uint32_t farm_state;*/
/*uint32_t water_time;*/
/*uint32_t insect_house;*/
/*farm_used_attirelist used_list;*/
/*farm_noused_attirelist feedstuff;*/
/*}__attribute__((packed));*/

/* 每种物品的数量和物品的种类 */
struct food_info {
	uint32_t type;
    uint32_t food_num;
}__attribute__((packed));

/* 仓库里物品的总数和各个物品的总数和物品的种类 */
struct farm_depot {
	uint32_t count;
	food_info attire[200];
}__attribute__((packed));
/* 在user_ex中农场的信息*/
struct all_goods_farm {
	uint32_t water_time;
	farm_depot feedstuff;
	home_attirelist used_list;
}__attribute__((packed));


/*进入农场拉取的信息*/
struct  user_farm_get_out_header{
	uint32_t farm_state;
	uint32_t runout_num;
	uint32_t pool_state;
	uint32_t insect_house;
	uint32_t item1_count;
	uint32_t item2_count;
	uint32_t item3_count;
	uint32_t item4_count;
}__attribute__((packed));
typedef animal_select user_farm_get_out_item_1;
typedef home_attire_item user_farm_get_out_item_2;
typedef attire_noused_item user_farm_get_out_item_3;
/* 得到饲料或者仓库的物品的请求包 */
//struct user_get_feedstuff_in {
//	uint32_t which_type;
//};
/* 得到饲料的应答包 */
struct  user_get_feedstuff_out_header{
	uint32_t item1_count;/*种类的个数*/
}__attribute__((packed));
typedef attire_noused_item user_get_feedstuff_out_item_1;

/* 得到仓库的应答包 */
struct user_farm_get_depot_out_header {
	uint32_t item1_count;
};
typedef attire_noused_item user_farm_get_depot_out_item_1;

 /* 删除某一动物的请求包 */
struct  user_del_animal_in{
	uint32_t animal_type;
	uint32_t userid;
	uint32_t id;
}__attribute__((packed));
struct user_del_animal_out {
	uint32_t exp;
	uint32_t animal_id;
}__attribute__((packed));

struct  stru_class_score_info{
	uint32_t score;
	uint32_t count;
	stru_member_score_list member_score_list;
}__attribute__((packed));
/* 向仓库或者饲料房里增加物品请求包 */
struct user_farm_feedstuff_add_in{
	uint32_t type;/* 1表示是向仓库里增加，2表示向饲料里增加 */
	food_info goods;
}__attribute__((packed));
//typedef food_info user_farm_feedstuff_add_in;
/* 向仓库或者饲料房里增加物品的应答包 */
struct user_farm_feedstuff_add_out_header{
	uint32_t exp;
	uint32_t type;
	uint32_t item1_count;
}__attribute__((packed));
typedef attire_noused_item  user_farm_feedstuff_add_out_item_1;
struct stru_user_farm_feedstuff_add_out_buf {
	uint32_t type;
	noused_homeattirelist noused_list; 
}__attribute__((packed));
/* 放置动物到农场的请求包 */
struct  user_farm_put_animal_in{
	uint32_t animal_id;
}__attribute__((packed));

//typedef animal_select user_farm_put_animal_out;
struct  user_farm_put_animal_out{
	animal_select item;
}__attribute__((packed));


/* 得到鱼塘里的鱼的应答包 */
struct user_farm_get_fish_in {
	uint32_t animal_type;
}__attribute__((packed));

struct  user_farm_get_fish_out_header{
	uint32_t animal_type;
	uint32_t item1_out;
}__attribute__((packed));
typedef animal_select user_farm_get_fish_out_item_1;
/* 水池加水的应答包 */
struct  user_farm_water_out_header{
	uint32_t item1_out;
}__attribute__((packed));
typedef animal_select user_farm_water_out_item_1;
//增加农场访问列表
typedef mms_opt_item user_farm_set_accestlist_in;
//得到农场的访问列表
struct  user_farm_get_accestlist_out_header{
	uint32_t item1_count;
}__attribute__((packed));
typedef mms_opt_item user_farm_get_accestlist_out_item_1;
struct  user_connect_set_childcount_in{
	uint32_t childcount;
}__attribute__((packed));

struct user_farm_get_all_animal_info_out_header {
	uint32_t item1_out;
}__attribute__((packed));
typedef stru_farm user_farm_get_all_animal_info_out_item_1;

typedef stru_farm user_farm_update_animal_info_in;
//设置农场状态
struct  user_farm_set_state_in{
	uint32_t state;
}__attribute__((packed));

typedef user_farm_set_state_in user_farm_set_state_out;
//偷取信息
struct stru_thiever_list {
	uint32_t count;
	uint32_t id[50];
}__attribute__((packed));

struct stru_thiever {
	uint32_t thiever_time;
	stru_thiever_list member_list;
}__attribute__((packed));

struct  user_card_set_flag_bit_in{
	uint32_t bitid;
	uint32_t value;
}__attribute__((packed));

struct  user_get_used_attierlist_in{
	uint32_t attiretype;
}__attribute__((packed));
typedef stru_count   user_get_used_attierlist_out_header;
//struct  home_attire_item user_get_used_attierlist_out_item;

//swap_card
struct swap_card_info {
	uint32_t super_card;
	uint32_t count;
	uint32_t lower_card[5];
}__attribute__((packed));

struct card_info_db {
	uint32_t card_type;
	uint32_t num;
	uint32_t swap_flag;
	uint32_t need_card;
}__attribute__((packed));

/*增加某个类的卡牌的请求包*/
struct  user_swap_card_search_in{
	uint32_t swap_id;
	uint32_t need_id;
}__attribute__((packed));
struct user_swap_card_search_out_header {
	uint32_t count;
}__attribute__((packed));
struct user_swap_card_search_out_item_1{
	userid_t userid;
	uint8_t attire[25];
}__attribute__((packed));
/*设置交换卡牌的请求包*/
struct  user_swap_card_set_swap_in{
	uint32_t swap_flag;
	uint32_t swap_card;
	uint32_t need_card;
	uint8_t  user_attire[25];
}__attribute__((packed));
/*交换卡牌*/
struct  user_swap_card_swap_in{
	uint32_t type;/*0表示对方，1表示自己*/
	uint32_t swap_card;
	uint32_t need_card;
}__attribute__((packed));
/* 兑换卡牌 */
typedef attire_noused_item  user_swap_card_get_super_out;

typedef stru_count   user_swap_card_get_card_out_header;
typedef card_info_db  user_swap_card_get_card_out_item;
struct  user_farm_animail_del_ex_in{
	uint32_t id;
}__attribute__((packed));

typedef stru_count   user_task_get_list_out_header;
typedef stru_user_task_ex user_task_get_list_out_item;

typedef card_info_db  user_swap_card_set_card_in;

//clothe waste
/* struct roominfo_get_clothe_waste_in {
	uint32_t clothe_type;
}__attribute__((packed));*/

struct pair_clothe {
	uint32_t clothe_type;
	uint32_t clothe_value;
}__attribute__((packed));

struct  roominfo_get_clothe_waste_out{
	uint32_t clothe_count;
	pair_clothe clothe_waste[5];
}__attribute__((packed));


typedef roominfo_get_clothe_waste_out roominfo_set_clothe_waste_in;

typedef roominfo_get_clothe_waste_out roominfo_set_clothe_waste_out;

struct  roominfo_get_point_out{
	uint8_t point_value[5];
}__attribute__((packed));

struct  roominfo_set_point_in{
	uint32_t value;
}__attribute__((packed));
struct  roominfo_set_point_out{
	uint8_t point_value[5];
}__attribute__((packed));

struct  user_set_xiaomee_max_info_in{
	uint32_t 	xiaomee_max;/*小米*/
	uint32_t 	sale_xiaomee_max;/*小米*/
	uint32_t	Ol_last; /* 最后一次登入时间 */
}__attribute__((packed));


struct class_member_medal_flag {
	uint32_t memberid;
	uint32_t flag;
}__attribute__((packed));

struct stru_class_medal_flag {
	uint32_t count;
	class_member_medal_flag class_flag[200];
}__attribute__((packed));

struct  roominfo_set_class_medal_flag_in{
	uint32_t memberid;
	uint32_t index;
}__attribute__((packed));
/* struct  user_ex_set_class_medal_flag_out{
	class_member_medal_flag class_medal_flag;
}__attribute__((packed));*/

struct class_medal_type {
	uint32_t type;
	uint32_t attire_id;
}__attribute__((packed));

struct class_medal {
	uint32_t count;
	class_medal_type items[200];
}__attribute__((packed));

typedef stru_count user_class_get_medal_list_out_header;
typedef class_medal_type user_class_get_medal_list_out_item_1;

typedef class_medal_type  user_class_set_medal_list_in;

struct  user_set_punish_flag_in{
	uint32_t flag_bit;
	uint32_t bool_set;
}__attribute__((packed));

struct  user_class_set_medal_info_out{
	uint32_t add_attire_count;/*0,1,2*/
}__attribute__((packed));

struct  user_class_set_medal_info_in{
	uint32_t medaltype;
	uint32_t attireid;

	uint32_t attireid_1;
	uint32_t attireid_2;
}__attribute__((packed));

struct  user_set_pet_sick_type_in{
 	uint32_t pet_id; 
	uint32_t sick_type;
}__attribute__((packed));

struct  roominfo_get_pos_out{
	uint32_t count;
	uint8_t pos_value[5];
}__attribute__((packed));

struct  roominfo_set_pos_in{
	uint32_t index;
}__attribute__((packed));
typedef roominfo_get_pos_out roominfo_set_pos_out;

struct  roominfo_clear_pos_in{
	uint32_t index;
}__attribute__((packed));


struct  roominfo_sig_fire_cup_in{
	uint32_t team;
}__attribute__((packed));

struct roominfo_get_fire_cup_team_out {
	uint32_t team;
	uint32_t total_gold;
	uint32_t total_silver;
	uint32_t total_copper;
	uint32_t day_gold;
	uint32_t day_silver;
	uint32_t day_copper;
}__attribute__((packed));

struct fire_cup_medal {
	uint32_t field_one;
	uint32_t field_two;
}__attribute__((packed));

struct roominfo_fire_cup_update_in {
	uint32_t type;
	uint32_t check;
	uint32_t num;
}__attribute__((packed));

//mms
struct mms_water_list {
	uint32_t count;
	uint32_t userid[10];
}__attribute__((packed));

struct  mms_set_water_list_in{
	userid_t userid;
}__attribute__((packed));

struct  mms_get_water_list_in{
	uint32_t type;
}__attribute__((packed));


struct  mms_get_water_list_out{
	uint32_t count;
}__attribute__((packed));

struct  sysarg_get_team_out{
	uint32_t team;
}__attribute__((packed));

struct  roominfo_set_task_in{
	uint32_t task_id;
	uint32_t opt;
}__attribute__((packed));

struct  roominfo_get_task_out{
	uint32_t task;
}__attribute__((packed));

struct  sysarg_get_fire_cup_out{
	uint32_t email;
	uint32_t email_air;
	uint32_t high_air;
	uint32_t task_id;
	uint32_t npc_pos;
	uint32_t npc_state;
}__attribute__((packed));

struct sys_get_all_medal_list_out_item {
	uint32_t date;
	uint32_t team;
	uint32_t gold;
	uint32_t silver;
	uint32_t copper;
	uint32_t num;
}__attribute__((packed));

struct sysarg_get_fire_cup_in {
	uint32_t team;
}__attribute__((packed));

struct sys_add_team_medal_in {
	uint32_t team;
	uint32_t type;
	uint32_t num;
}__attribute__((packed));

struct sys_get_all_medal_list_in {
	uint32_t type;
}__attribute__((packed));

struct sys_get_all_medal_list_out_header {
	uint32_t count;
}__attribute__((packed));

struct ip_history_item {	
	uint32_t userid;
	uint32_t which_game;
	uint32_t ip;
}__attribute__((packed));

typedef ip_history_item  history_ip_record_in;


struct  stru_game_score{
	uint32_t userid;
	uint32_t gameid;
	uint32_t score;
}__attribute__((packed));

struct  stru_game_pk{
    uint32_t win_count;
    uint32_t lose_count;
}__attribute__((packed));



typedef stru_game_score   game_set_score_in;
	
struct game_user_get_list_in_header{
	uint32_t gameid;
	uint32_t count;
}__attribute__((packed));

struct  game_user_get_list_in_item{
	uint32_t id;
}__attribute__((packed));
typedef stru_count   game_user_get_list_out_header;
struct  game_user_get_list_out_item{
	uint32_t id;
	uint32_t score;
}__attribute__((packed));

struct  game_get_list_in{
	uint32_t userid;
}__attribute__((packed));
typedef stru_count   game_get_list_out_header;
struct  game_get_list_out_item{
	uint32_t gameid;
	uint32_t score;
	uint32_t pkflag;
}__attribute__((packed));
struct  game_set_pk_info_in{
	uint32_t gameid;
	userid_t userid;
	userid_t userid_pk;
	uint32_t score;
}__attribute__((packed));
struct  game_set_pk_info_out{
	uint32_t	userid_score;
	uint32_t	userid_pk_score;
	uint32_t	is_win_flag;
}__attribute__((packed));



struct  stru_pk_info{
	userid_t guestid;
	char   		nick[NICK_LEN];
	uint32_t gameid;			
	uint32_t pktime;			
	uint32_t userid_score;			
	uint32_t userid_pk_score;
	uint32_t pkflag;			
}__attribute__((packed));

struct stru_pk_list{
	uint32_t count;
	stru_pk_info items[PK_LIST_MAX_COUNT]; 
}__attribute__((packed));
typedef stru_pk_info  roominfo_pk_add_in;
typedef stru_count   roominfo_pk_get_list_out_header;
typedef stru_pk_info roominfo_pk_get_list_out_item;

struct  game_get_pk_info_in{
	userid_t userid;
}__attribute__((packed));
typedef stru_game_pk game_get_pk_info_out;

struct  user_update_game_score_in{
	uint32_t score;
}__attribute__((packed));


struct user_lamu_diary_edit_in {
	uint32_t id;
	uint32_t mood;
	uint32_t state;
	char title[31];
	char lamu[61];
	char whisper[31];
	char star[61];
	char abc[145];
	char content[601];
}__attribute__((packed));


struct user_lamu_diary_edit_out {
	uint32_t id;
}__attribute__((packed));

struct  user_lamu_catalog_get_in{
	uint32_t userid;
	uint32_t start;
	uint32_t end;
}__attribute__((packed));
typedef stru_count   user_lamu_catalog_get_out_header;

struct  user_lamu_catalog_get_out_item{
	uint32_t id;
	uint32_t date;
	uint32_t state;
	char 	 title[31];
}__attribute__((packed));

struct  user_lamu_diary_delete_in{
	uint32_t id;
}__attribute__((packed));

struct  user_lamu_diary_state_update_in{
	uint32_t id;
	uint32_t state;
}__attribute__((packed));

struct  user_lamu_diary_flower_update_in{
	uint32_t id;
}__attribute__((packed));

struct  user_lamu_diary_content_get_in{
	uint32_t id;
	uint32_t userid;
}__attribute__((packed));

struct  user_lamu_diary_content_get_out{
	uint32_t mood;
	uint32_t flower;
	char lamu[61];
	char whisper[31];
	char star[61];
	char abc[145];
	char content[601];
}__attribute__((packed));

struct  roominfo_work_set_in{
	uint32_t type;
}__attribute__((packed));

struct  roominfo_work_set_out{
	uint32_t time_total;
}__attribute__((packed));


/* struct  roominfo_work_get_in{
	uint32_t type;
}__attribute__((packed));*/

struct  roominfo_work_get_out{
	uint32_t time_total;
	uint32_t time_tom;
	uint32_t time_nick;
	uint32_t this_tom;
	uint32_t this_nick;
}__attribute__((packed));

struct  user_ex_set_diary_lock_in{
	uint32_t lock;
}__attribute__((packed));

struct  user_ex_diary_lock_get_out{
	uint32_t lock;
}__attribute__((packed));

struct  user_lamu_diary_count_out{
	uint32_t count;
}__attribute__((packed));

struct  user_set_work_level_in{
	uint32_t work_level;
}__attribute__((packed));


struct  user_pet_task_set_ex_in{
	uint32_t petid;
	uint32_t taskid;
	uint32_t flag;
	uint32_t usetime;
	uint32_t startime;
	uint32_t endtime;
	uint32_t def_usetime;
	uint32_t def_flag;
	uint32_t stage_flag;
}__attribute__((packed));


struct  user_pet_task_set_ex_out{
	uint32_t petid;
	uint32_t taskid;
}__attribute__((packed));

struct  user_task_pet_first_stage_set_in{
	uint32_t petid;
	uint32_t taskid;
	uint32_t stage;
}__attribute__((packed));

struct user_task_pet_first_stage_set_out {
	uint32_t count;
}__attribute__((packed));

struct  user_task_taskid_del_in{
	uint32_t petid;
	uint32_t taskid;
}__attribute__((packed));

struct user_task_taskid_del_out {
	uint32_t count;
}__attribute__((packed));

struct  user_task_set_client_data_in{
	uint32_t petid;
	uint32_t taskid;
	uint32_t value;
}__attribute__((packed));

struct  user_task_get_client_data_in{
	uint32_t petid;
	uint32_t taskid;
}__attribute__((packed));
struct  user_task_get_client_data_out{
	uint32_t out;
}__attribute__((packed));

struct  user_task_get_pet_task_in{
	uint32_t petid;
	uint32_t start_taskid;
	uint32_t end_taskid;
}__attribute__((packed));


typedef stru_count   user_task_get_pet_task_out_header;

struct  user_task_get_pet_task_out_item{
	uint32_t petid;
	uint32_t taskid;
	uint32_t time;
	uint32_t step;
	uint32_t client;
}__attribute__((packed));

struct  user_get_home_in{
	uint32_t homeid;
}__attribute__((packed));

typedef home_attire_item user_reset_home_in;



typedef stru_count   user_reset_home_out_header;
typedef stru_attire_noused user_reset_home_out_item;

struct  user_attire_put_chest_in{
	uint32_t attireid;
}__attribute__((packed));

struct  user_attire_del_chest_in{
	uint32_t attireid;
}__attribute__((packed));


/* struct  user_attire_get_chest_list_in{
	uint32_t attireid_start;
	uint32_t attireid_end;
}__attribute__((packed));*/

typedef stru_count   user_attire_get_chest_list_out_header;
struct  user_attire_get_chest_list_out_item{
	uint32_t attireid;
	uint32_t count;
}__attribute__((packed));

struct  roominfo_change_present_in {
	int32_t change_value;
}__attribute__((packed));

struct  roominfo_get_present_num_out{
	uint32_t num;
}__attribute__((packed));


typedef stru_count   user_magic_task_pet_get_all_out_header;
struct  user_magic_task_pet_get_all_out_item{
	uint32_t petid;
	uint32_t taskid;
	uint32_t flag;
	uint32_t usetime;
	uint32_t startime;
	uint32_t endtime;
	uint32_t first_time;
	uint32_t client;
	uint32_t step;
}__attribute__((packed));

struct  user_magic_task_pet_update_in{
	uint32_t petid;
	uint32_t taskid;
	uint32_t flag;
	uint32_t usetime;
	uint32_t starttime;
	uint32_t endtime;
	uint32_t first_time;
	uint32_t client;
	uint32_t step;
}__attribute__((packed));
typedef stru_count   user_attire_get_list_ex_out_header;
struct  user_attire_get_list_ex_out_item{
	uint32_t attireid;
	uint32_t usedcount;
	uint32_t chest;
	uint32_t count;
}__attribute__((packed));
struct  user_attire_get_list_ex_in{
	uint32_t start;
	uint32_t count;
}__attribute__((packed));

typedef  user_get_attire_list_in user_get_attire_list_all_in;
typedef stru_count   user_get_attire_list_all_out_header;
typedef attire_count user_get_attire_list_all_out_item;

struct  user_farm_get_sheep_num_out{
	uint32_t num;
}__attribute__((packed));


/* 汽车相关初始化信息结构体 */
struct auto_attire {
	uint32_t auto_id; /* 汽车总类的ID号 */
	uint32_t oil; /* 汽车的初始油量 */
	uint32_t engine; /* 汽车的初始引擎 */
	uint32_t color; /* 汽车的初始颜色 */
	uint32_t hole_num; /* 汽车初始孔的数目 */
}__attribute__((packed));

/* 购买车请求包 */
struct  user_auto_insert_in{
	uint32_t auto_id;/* 汽车总类的ID号 */
	uint32_t xiaomee;/*买汽车需要的豆豆豆 */
}__attribute__((packed));
/* 购买汽车的应答包 */
struct  user_auto_insert_out{
	uint32_t id; /* 插入记录的自增的ID号  */
}__attribute__((packed));


/* 更换引擎，颜色的请求包 */
struct  user_auto_change_attire_in{
	uint32_t type;/* 1表示更换引擎，2表示更换颜色 */
	uint32_t id;/* 汽车的ID号 */
	uint32_t new_value;/* 更新的新值 */
}__attribute__((packed));
/* 更换引擎，颜色的返回包 */
struct  user_auto_change_attire_out{
	uint32_t id;/* 旧的返回的ID号 */
}__attribute__((packed));
/* 引擎对应的油耗 */
struct engine_and_oil {
	uint32_t engine;
	uint32_t oil;
}__attribute__((packed));


struct user_auto_propery_item {
	uint32_t propery_id;
	uint32_t attire_id;
}__attribute__((packed));

struct user_auto_propery {
	uint32_t count;
	uint32_t item[4];	
}__attribute__((packed));

typedef stru_count   user_auto_get_all_out_header;
struct user_auto_get_all_out_item {
	uint32_t id;
	uint32_t auto_id;
	uint32_t oil;
	uint32_t engine;
	uint32_t color;
	uint32_t oil_time;
	uint32_t total_oil;
	user_auto_propery propery;
	uint32_t adorn[2];
}__attribute__((packed));

struct  user_auto_change_propery_in{
	uint32_t type;
	uint32_t id;
	uint32_t propery_id;
	uint32_t new_propery;
}__attribute__((packed));
struct  user_auto_change_propery_out{
	uint32_t old_attire;
}__attribute__((packed));

/*struct engine_and_oil {*/
/*uint32_t oil;*/
/*}__attribute__((packed));*/

struct  user_auto_add_oil_in{
	uint32_t id;
}__attribute__((packed));
struct  user_auto_add_oil_out{
	uint32_t xiaomee;
}__attribute__((packed));

struct  user_auto_get_one_info_in{
	uint32_t id;
}__attribute__((packed));
typedef user_auto_get_all_out_item user_auto_get_one_info_out;

struct  user_ex_set_show_auto_in{
	uint32_t id;
	uint32_t type;/* 0表示取消，1表示设置展示的车 */
}__attribute__((packed));
typedef user_auto_get_all_out_item user_ex_get_show_auto_out;
//struct  user_ex_get_show_auto_out{
//}__attribute__((packed));

struct  user_ex_set_auto_skill_in{
	uint32_t skill_value;
}__attribute__((packed));

struct stru_adron {
	uint32_t adron[2];
}__attribute__((packed));

struct  user_auto_change_adron_in{
	uint32_t type;
	uint32_t id;
	uint32_t adron_id;
	uint32_t new_adron;
}__attribute__((packed));

struct  user_auto_change_adron_out{
	uint32_t old_attire;
}__attribute__((packed));

struct  user_class_add_score_in{
	uint32_t add_score;
}__attribute__((packed));
struct  user_class_add_score_out{
	uint32_t score;
}__attribute__((packed));

struct  user_class_get_score_out{
	uint32_t score;
}__attribute__((packed));

struct  roominfo_add_candy_in{
	uint32_t add_candy;
}__attribute__((packed));

struct  roominfo_add_candy_out{
	uint32_t candy_num;
}__attribute__((packed));

struct  roominfo_get_candy_out{
	uint32_t candy_num;
}__attribute__((packed));

struct  user_info_get_set_passwd_flag_out{
	uint32_t flag;
}__attribute__((packed));

struct  roominfo_add_class_score_in{
	uint32_t add_score;
}__attribute__((packed));

/*struct  roominfo_add_class_score_out{*/
/*uint32_t score;*/
/*}__attribute__((packed));*/

struct  roominfo_get_class_score_out{
	uint32_t score;
}__attribute__((packed));

struct  roominfo_get_score_candy_out{
	uint32_t candy;
}__attribute__((packed));

struct   user_farm_set_attirelist_in_header{
		uint32_t	nousedcount;
		uint32_t	usedcount;
} __attribute__((packed)) ;

typedef home_attire_item  user_farm_set_attirelist_in_item_2;
typedef attire_noused_item user_farm_set_attirelist_in_item_1;

struct  roominfo_add_catch_candy_in{
	uint32_t add_candy;
}__attribute__((packed));

struct  roominfo_add_catch_candy_out{
	uint32_t candy;
}__attribute__((packed));


/*  炫风拉力赛 */
struct  roominfo_rally_sign_out{
	uint32_t team;
}__attribute__((packed));

struct  roominfo_rally_add_score_in{
	uint32_t add_score;
	uint32_t race_num;
}__attribute__((packed));
struct  roominfo_rally_add_score_out{
	uint32_t finish;
}__attribute__((packed));

struct  roominfo_rally_get_score_out{
	uint32_t score;
}__attribute__((packed));

struct sys_get_team_score_list_out_item {
	uint32_t team;
	uint32_t score;
	uint32_t day_score;
}__attribute__((packed));

struct  sys_rally_update_score_in {
	uint32_t team;
	uint32_t add_score;
}__attribute__((packed));

struct  sys_rally_update_race_in{
	uint32_t team;
	uint32_t index;
	uint32_t value;
}__attribute__((packed));

typedef stru_count   sys_get_team_race_list_out_header;

struct sys_get_team_race_list_out_item {
	uint32_t team;
	uint32_t race1;
	uint32_t race2;
	uint32_t race3;
	uint32_t race4;
}__attribute__((packed));

typedef stru_count   sys_get_team_score_list_out_header;
/*struct  sys_get_team_score_list_out_item{*/

/*}__attribute__((packed));*/

struct  roominfo_rally_get_race_info_out{
	uint32_t race1_num;
	uint32_t race2_num;
	uint32_t race3_num;
	uint32_t race4_num;
}__attribute__((packed));

struct  roominfo_rally_get_today_score_out{
	uint32_t today_score;
}__attribute__((packed));

struct  roominfo_rally_get_team_out{
	uint32_t score;
	uint32_t team;
	uint32_t medal;
}__attribute__((packed));

struct  user_auto_get_one_kind_num_in{
	uint32_t auto_id;
}__attribute__((packed));
struct  user_auto_get_one_kind_num_out{
	uint32_t auto_count;
}__attribute__((packed));

struct  roominfo_rally_add_medal_in{
	uint32_t add_medal;
}__attribute__((packed));

struct  user_farm_set_outgo_in{
	uint32_t id;
	uint32_t flag;
}__attribute__((packed));

typedef stru_count   user_farm_set_outgo_out_header;
typedef animal_select user_farm_set_outgo_out_item;




struct  user_farm_set_eattime_in{
	uint32_t id;
}__attribute__((packed));

typedef animal_select user_farm_set_eattime_out;
struct  stru_set_int_value{
    char     field_name[FIELD_NAME_LEN];
    uint32_t value;
}__attribute__((packed));
typedef stru_set_int_value user_set_int_value_in;
typedef stru_set_int_value user_ex_set_int_value_in;

struct  user_auto_del_in{
	uint32_t id;
}__attribute__((packed));

typedef stru_count   user_attire_get_some_attire_count_in_header;
struct  user_attire_get_some_attire_count_in_item{
	uint32_t attireid;
}__attribute__((packed));
typedef stru_count   user_attire_get_some_attire_count_out_header;
struct  user_attire_get_some_attire_count_out_item{
	uint32_t attireid;
	uint32_t count;
}__attribute__((packed));

typedef stru_count   user_auto_get_all_web_out_header;
typedef user_auto_get_all_out_item  user_auto_get_all_web_out_item;

struct  user_auto_set_auto_web_in{
	uint32_t id;
	uint32_t autoid;
	uint32_t oil;
	uint32_t engine;
	uint32_t color;
	uint32_t oil_time;
}__attribute__((packed));

/* 合成列表 */
typedef stru_count   roominfo_get_merge_out_header;
struct roominfo_get_merge_out_item {
	uint8_t count;
	uint8_t flag;
}__attribute__((packed));

struct stru_roominfo_merge {
	uint32_t attireid;
	uint32_t count;
}__attribute__((packed));

struct  roominfo_get_merge_buf{
	uint32_t count;
	stru_roominfo_merge item[10];
}__attribute__((packed));

struct  roominfo_merge_update_in{
	uint32_t attireid;
	uint32_t count;
}__attribute__((packed));

/*struct roominfo_merge_set_get_out {*/
/*uint32_t count;*/
/*uint32_t rand_flag;*/
/*}__attribute__((packed));*/

struct  userinfo_log_for_blw_in{
	char passwd[PASSWD_LEN];
	uint32_t  ip;
}__attribute__((packed));
struct  userinfo_log_for_blw_out{
	uint32_t gameflag;
	char email[EMAIL_LEN];
}__attribute__((packed));

typedef stru_count   user_attire_set_used_in_header;
struct  user_attire_set_used_in_item{
	uint32_t attireid;
}__attribute__((packed));

struct  roominfo_check_candy_out{
	uint32_t flag;
}__attribute__((packed));

struct  roominfo_rally_get_user_all_info_web_out{
	uint32_t team;
	uint32_t score;
	uint32_t race1;
	uint32_t race2;
	uint32_t race3;
	uint32_t race4;
	uint32_t day_score;
}__attribute__((packed));

struct  roominfo_get_user_all_info_web_out{
	uint32_t candy_count;
	uint32_t prop_count;
	uint32_t flag;
}__attribute__((packed));

struct user_farm_get_mature_fish_out {
	uint32_t id;
	uint32_t animalid;
}__attribute__((packed));

struct user_farm_net_catch_fish_out_header{
	uint32_t exp;
	uint32_t net;
	uint32_t count;
}__attribute__((packed));
struct  user_farm_net_catch_fish_out_item{
	uint32_t fish_type;
	uint32_t count;
}__attribute__((packed));

struct  user_farm_get_net_out{
	uint32_t net;
}__attribute__((packed));

struct  roominfo_change_candy_ex_in{
	uint32_t change_candy;
	uint32_t change_prop;
	uint32_t flag;
}__attribute__((packed));
struct roominfo_change_candy_ex_out {
	uint32_t flag;
	uint32_t prop;
}__attribute__((packed));

struct  roominfo_change_candy_out{
	uint32_t candy_num;
	uint32_t prop_num;
}__attribute__((packed));

struct  roominfo_set_user_all_info_web_in{
	uint32_t candy_count;
	uint32_t prop_count;
	uint32_t flag;
}__attribute__((packed));

struct roominfo_candy_list_item {
	uint32_t userid;
	uint8_t count;
}__attribute__((packed));

struct roominfo_candy_list {
	uint8_t people_count;
	uint8_t fetch_count;
	roominfo_candy_list_item item[100];
}__attribute__((packed));

struct  roominfo_check_candy_list_in{
	uint32_t userid;
}__attribute__((packed));

struct  roommsg_insert_bibo_in{
	uint32_t date;
	uint32_t pic;
	char title[31];
	char bibo_content[100];
}__attribute__((packed));

struct  roommsg_del_bibo_in{
	uint32_t date;
}__attribute__((packed));

struct  roommsg_get_bibo_list_in{
	uint32_t date_start;
	uint32_t date_end;
}__attribute__((packed));

typedef stru_count   roommsg_get_bibo_list_out_header;

struct  roommsg_get_bibo_list_out_item{
	uint32_t date;
	uint32_t pic;
	char title[31];
	char bibo_content[100];
}__attribute__((packed));

struct  sysarg_db_msg_edit_in{
	uint32_t date;
	uint32_t start;
	uint32_t end;
	char content[100];
}__attribute__((packed));

struct  sysarg_db_msg_del_in{
	uint32_t date;
	uint32_t start;
}__attribute__((packed));

struct  sysarg_db_msg_get_in{
	uint32_t date_start;
	uint32_t date_end;
}__attribute__((packed));
typedef stru_count   sysarg_db_msg_get_out_header;
struct  sysarg_db_msg_get_out_item{
	uint32_t date;
	uint32_t start;
	uint32_t end;
	char content[100];
}__attribute__((packed));

typedef stru_count   sysarg_db_msg_get_all_web_out_header;
struct  sysarg_db_msg_get_all_web_out_item{
	uint32_t date;
	uint32_t start;
	uint32_t end;
	char content[100];
}__attribute__((packed));

typedef stru_email email_check_email_is_existed_in;
typedef stru_is_existed email_check_email_is_existed_out;

/*struct user_farm_water_in {*/
/*uint32_t id;*/
/*}__attribute__((packed));*/


struct user_npc_out {
	int16_t npc_1;
	int16_t npc_2;
	int16_t npc_3;
	int16_t npc_4;
	int16_t npc_5;
	int16_t npc_6;
}__attribute((packed));

struct  user_npc_change_in{
	int32_t change_value;
	uint32_t index;
}__attribute__((packed));

struct user_jy_add_exp_out {
	int32_t exp;
}__attribute__((packed));

struct user_jy_add_exp_in {
	uint32_t exp;
	uint32_t type;
}__attribute__((packed));

struct  user_farm_get_web_out{
	uint32_t farm_state;
	uint32_t water_time;
	uint32_t net;
	uint32_t plant;
	uint32_t breed;
	uint32_t plant_limit;
	uint32_t breed_limit;
	uint32_t time;
}__attribute__((packed));

typedef user_farm_get_web_out user_farm_set_web_in;
typedef user_npc_out user_npc_get_web_out;
//struct  user_npc_get_web_out{
//}__attribute__((packed));


//struct  user_farm_set_web_in{
//}__attribute__((packed));

typedef user_npc_out user_npc_update_web_in;

struct  user_farm_rabbit_foster_in{
	uint32_t id;
}__attribute__((packed));

struct rabbit_foster_info {
	uint32_t id;
	uint32_t time;
}__attribute__((packed));

struct rabbit_foster_list {
	uint32_t count;
	rabbit_foster_info item[15];
}__attribute__((packed));

struct  user_farm_rabbit_del_out{
	uint32_t count_1;/* 已经领取的数目 */
	uint32_t count_2;/* 未领取的数目 */
}__attribute__((packed));

struct  sysarg_day_get_list_in{
	uint32_t start_type;
	uint32_t end_type;
}__attribute__((packed));

typedef stru_count   sysarg_day_get_list_out_header;

struct  sysarg_day_get_list_out_item{
	uint32_t type;
	uint32_t value;
}__attribute__((packed));

struct  user_farm_get_plant_breed_out{
	uint32_t plant_lv;
	uint32_t breed_lv;
}__attribute__((packed));

struct stru_user_egg {
	uint32_t attireid;
	uint32_t pos;
	uint32_t num;
}__attribute__((packed));

struct stru_user_egg_ex {
	uint32_t attireid;
	uint32_t pos;
	uint32_t num;
	uint32_t total_num;
}__attribute__((packed));

struct stru_egg_member {
	uint32_t count;
	uint32_t item[20];
}__attribute__((packed));

//------------------------------------------------------------------------------------
//暖房:greenhouse
//暖房，插入配对动物，指定动物的信息
struct user_greenhouse_insert_in
{
	uint32_t animal_id;
	uint32_t animal_type;
}__attribute__((packed));

struct user_greenhouse_drop_in
{
	uint32_t animal_id;
}__attribute__((packed));

//暖房，领取动物命令的返回
struct user_greenhouse_drop_out
{
	uint32_t animal_type; //应新生动物的类型
}__attribute__((packed));

struct user_greenhouse_remain_time_in
{
	uint32_t animal_id;
}__attribute__((packed));

//暖房，返回动物配对的剩余时间
struct user_greenhouse_remain_time_out 
{
	uint32_t remain_time;
}__attribute__((packed));

//暖房，返回指定用户是（1）－否（0）有动物正在配对
struct user_greenhouse_user_exist_out
{
	uint32_t exist;
}__attribute__((packed));

//暖房，列表中的个数
typedef stru_count   user_greenhouse_user_all_animal_out_header;

//暖房，返回指定用户所有正在配对的动物列表
struct user_greenhouse_user_all_animal_out_item
{
	uint32_t animal_id;
	uint32_t animal_type;
	uint32_t remain_time;
}__attribute__((packed));

//------------------------------------------------------------------------------------
struct stru_user_egg_all {
	uint32_t attireid;
	uint32_t pos;
	uint32_t num;
	uint32_t time;
	stru_egg_member member;	
}__attribute__((packed));

struct  user_egg_insert_in{
	uint32_t pos;
	uint32_t attireid;
}__attribute__((packed));

struct  user_egg_fudan_in{
	uint32_t userid;
	uint32_t pos;
}__attribute__((packed));

struct  user_egg_get_info_in{
	uint32_t pos;
}__attribute__((packed));
struct  user_egg_get_info_out{
	uint32_t num;
	uint32_t total_num;
}__attribute__((packed));


struct roominfo_pic_insert_in {
	uint32_t type;
}__attribute__((packed));

struct stru_user_egg_info {
	uint32_t attireid;
	uint32_t child;
	uint32_t num;
};

struct  user_farm_animal_get_wool_in{
	uint32_t id;
}__attribute__((packed));

struct  sysarg_db_update_animal_limit_in{
	uint32_t attireid;
	int32_t  change_value;
	uint32_t maxvalue;
}__attribute__((packed));

struct  sysarg_get_animal_limit_in{
	uint32_t attireid;
}__attribute__((packed));
struct  sysarg_get_animal_limit_out{
	uint32_t count;
}__attribute__((packed));

struct  user_npc_task_recv_in{
	uint32_t npc;
	uint32_t taskid;
	int32_t npc_favor;
	uint32_t maxnum;
	uint32_t week_day;
}__attribute__((packed));

struct  user_npc_task_finish_in{
	uint32_t npc;
	uint32_t taskid;
}__attribute__((packed));

struct  user_npc_task_get_buf_in{
	uint32_t npc;
	uint32_t taskid;
}__attribute__((packed));

struct  user_npc_task_get_buf_out{
	char buf[50];
}__attribute__((packed));

struct  user_npc_task_set_buf_in{
	uint32_t npc;
	uint32_t taskid;
	char buf[50];
}__attribute__((packed));

struct  user_npc_one_task_get_info_in{
	uint32_t npc;
	uint32_t taskid;
}__attribute__((packed));

struct  user_npc_one_task_get_info_out{
	uint32_t state;
	uint32_t num;
}__attribute__((packed));

struct  user_npc_task_get_npc_task_in{
	uint32_t npc;
}__attribute__((packed));

typedef stru_count   user_npc_task_get_npc_task_out_header;

struct user_npc_task_delete_npc_task_in
{
	uint32_t npc;
	uint32_t taskid;
}__attribute__((packed));

struct  user_npc_task_get_npc_task_out_item{
	uint32_t taskid;
	uint32_t state;
	uint32_t num;
	uint32_t flag;
	uint32_t recv_tm;
}__attribute__((packed));

struct  user_egg_brood_egg_in{
	uint32_t pos;
}__attribute__((packed));

struct  user_egg_brood_egg_out{
	uint32_t animal_id;
}__attribute__((packed));

typedef stru_count   user_egg_get_all_web_out_header;
struct  user_egg_get_all_web_out_item {
	uint32_t attireid;
	uint32_t pos;
	uint32_t num;
	uint32_t time;
}__attribute__((packed));

struct user_npc_task_recv_out {
	uint32_t taskid;
	uint32_t state;
	uint32_t num;
}__attribute__((packed));

struct  user_egg_update_web_in{
	uint32_t attireid;
	uint32_t pos;
	uint32_t num;
	uint32_t time;
}__attribute__((packed));

struct user_npc_task_finish_out {
	uint32_t state;
	uint32_t num;
}__attribute__((packed));

//-------------------------------------------------------------------------------------------------
//服装打分
struct roominfo_roominfo_dressing_mark_get_in
{
	uint32_t num;
	uint32_t colth[12];
}__attribute__((packed));

struct roominfo_roominfo_dressing_mark_get_out
{
	//如果是0表示错误，其它指定相应的值
	uint32_t mark[3];
}__attribute__((packed));

struct roominfo_roominfo_dressing_mark_set_in
{
    	uint32_t num;
	uint32_t colth[12];
    	uint32_t mark[3];
}__attribute__((packed));

struct user_ex_pic_clothe {
	uint8_t type_count[2];/* 第一字节表示么么公主造型，第二字节表示衣服的数量 */
	uint32_t clothe_id[12];/* 最多十二件衣服 */
}__attribute__((packed));

typedef user_ex_pic_clothe user_ex_update_pic_clothe_in;

/*struct  user_ex_update_pic_clothe_in{*/
/*}__attribute__((packed));*/

/*struct  user_ex_get_pic_clothe_out{*/
/*}__attribute__((packed));*/
typedef user_ex_pic_clothe user_ex_get_pic_clothe_out;


typedef stru_count   roominfo_merge_set_get_out_header;
struct  roominfo_merge_set_get_out_item{
	uint32_t attireid;
	uint32_t count;
}__attribute__((packed));

struct  user_mode_level_set_in{
	uint32_t mode_score;
}__attribute__((packed));

//----------------------------------------------------------------------------------------------------
//鸭子状态
struct roominfo_roominfo_duck_statu_get_out
{
	uint32_t statu;//0: 正常，1: 散架
}__attribute__((packed));

typedef roominfo_roominfo_duck_statu_get_out roominfo_roominfo_duck_statu_set_in;

struct  user_attire_set_mode_attire_in_header {
	uint32_t index;
	uint32_t count;
}__attribute__((packed));
struct  user_attire_set_mode_attire_in_item{
	uint32_t attireid;
}__attribute__((packed));

struct user_attire_mode_clothe {
	uint32_t attireid;
	uint32_t mode_index;
}__attribute__((packed));

struct user_attire_one_mode_clothe {
	uint32_t mode_index;
	uint32_t count;
	uint32_t id[12];
}__attribute__((packed));

struct user_attire_all_mode_clothe {
	uint32_t count;
	user_attire_one_mode_clothe item[50];
}__attribute__((packed));

typedef stru_count   user_attire_get_mode_attire_out_header;
struct user_attire_get_mode_attire_out_item_1 {
	uint32_t attireid;
}__attribute__((packed));
struct  user_attire_get_mode_attire_out_item{
	uint32_t index;
	uint32_t count;
	user_attire_get_mode_attire_out_item_1 clothe;
}__attribute__((packed));

struct  user_attire_set_mode_to_mole_in{
	uint32_t mode_index;
}__attribute__((packed));


struct user_attire_set_mode_to_mole_out_header {
	uint32_t mode_index;
	uint32_t count;
}__attribute__((packed));
struct user_attire_set_mode_to_mole_out_item {
	uint32_t attireid;
}__attribute__((packed));

struct haqi_town_login {
	uint32_t index;
	userid_t userid;
}__attribute__((packed));

struct  user_attire_set_clothe_mole_to_mode_in{
	uint32_t mode_index;
}__attribute__((packed));
	/*typedef stru_count   user_attire_set_clothe_mole_to_mode_out_header;*/
	/*struct  user_attire_set_clothe_mole_to_mode_out_item{*/
	/*}__attribute__((packed));*/

typedef stru_count user_attire_set_used_out_header;
struct user_attire_set_used_out_item {
	uint32_t attireid;
}__attribute__((packed));

//----------------------------------------------------------------------------------------------------
//蛋糕用于交换礼品
struct roominfo_cake_gift_get_out
{
	uint32_t cake_num;
}__attribute__((packed));

struct roominfo_cake_gift_swap_do_swap_in
{
	int32_t cake_num;
}__attribute__((packed));

/**
 * @brief 用于返回命令执行状态， 成功：1, 失败：0
 */
struct roominfo_cake_gift_swap_do_swap_out
{
	uint32_t statu;//成功：1, 失败：0
}__attribute__((packed));

//----------------------------------------------------------------------------------------------------
struct  user_home_add_mode_out{
	uint32_t mode_index;
}__attribute__((packed));

typedef stru_count   user_pic_book_get_out_header;
struct  user_pic_book_get_out_item
{
	uint32_t attireid;
}__attribute__((packed));
//----------------------------------------------------------------------------------------------------
//钓鱼换取物品
struct roominfo_fish_weight_get_out
{
	uint32_t total_weight;
}__attribute__((packed));

struct roominfo_fish_weight_set_in
{
	uint32_t type;
	uint32_t weight;
}__attribute__((packed));

struct roominfo_fish_weight_set_out
{
	uint32_t total_weight;
}__attribute__((packed));

struct  roominfo_flower_update_in
{
	uint32_t flag;
	uint32_t type;
	int32_t value;
}__attribute__((packed));

struct  roominfo_flower_get_out
{
	uint32_t flag;
	uint32_t type;
	int32_t count;
}__attribute__((packed));

struct  sysarg_animal_get_max_weight_fish_in{
	uint32_t weight;
}__attribute__((packed));

struct  sysarg_animal_get_max_weight_fish_out{
	userid_t userid;
	uint32_t max_weight;
}__attribute__((packed));

//----------------------------------------------------------------------------------------------------
//昆虫小屋，追踪昆虫的信息
//取得一个用户在昆虫小屋的所有的昆虫
struct user_insect_cabin_get_all_out_header
{
	uint32_t count;
}__attribute__((packed));

//记录昆虫的当前信息
struct user_insect_cabin_get_all_out_item
{
	uint32_t inner_id; //系统内部的标识码，因为同一种动物可能有多个
	uint32_t insect_id;//昆虫的ID号，哪种昆虫
	uint32_t insect_state;//昆虫的状态
	uint32_t num_of_today;//今天活动的次数
}__attribute__((packed));

struct  user_farm_set_col_in{
	uint32_t type;
	uint32_t value;
}__attribute__((packed));

struct stru_roominfo_socks {
	uint32_t count;
	uint32_t items[99];
}__attribute__((packed));

struct roominfo_get_sockes_in {
	uint32_t userid;
}__attribute__((packed));
struct stru_login_info{
    uint32_t userid;
    uint32_t loginflag;
    uint32_t onlineid;
    uint32_t logintime;
}__attribute__((packed));


struct  ff_login_get_list_in{
    uint32_t login_date;
}__attribute__((packed));
typedef stru_count   ff_login_get_list_out_header;
typedef stru_login_info ff_login_get_list_out_item;

struct  user_animal_release_insect_in{
	uint32_t id;
	uint32_t type;
}__attribute__((packed));

struct stru_ip_item{
    uint32_t ip[3];
    inline bool add_ip(uint32_t newip){
        if (ip[0] == newip ){//不用更新
            return false;
        }
        int i = 0;
        int find_id=2;
        for (i = 1; i < 3; i++) {
            if (ip[i] == newip) {
                find_id=i;
                break;
            }
        }
        for (i = find_id ; i >= 1; i--) {
                ip[i] = ip[i -1];
        }
        ip[0] = newip;
		return true;
    };
}__attribute__((packed));
typedef stru_ip_item three_ip_history ;


struct  user_animal_set_animal_flag_in{
	uint32_t id;
	uint32_t index;
}__attribute__((packed));

struct  user_jy_butterfly_pollinate_flower_in{
	uint32_t plant_id;
}__attribute__((packed));
//struct  user_jy_butterfly_pollinate_out{
//}__attribute__((packed));
typedef jy_item user_jy_butterfly_pollinate_flower_out;
struct  appeal_ex_get_state_out{
	uint32_t state;
}__attribute__((packed));

struct  appeal_ex_get_state_in{
	uint32_t id;
}__attribute__((packed));

struct  user_animal_pollinate_in{
	uint32_t insect_id;
}__attribute__((packed));
struct  user_animal_pollinate_out{
	uint32_t pollinate_num;
}__attribute__((packed));

struct  ip_history_get_lastip_out{
	uint32_t ip;
}__attribute__((packed));

struct stru_roominfo_wish {
	uint8_t wish[360];
}__attribute__((packed));
	/*struct  user_roominfo_update_wish_in{*/
	/*}__attribute__((packed));*/
typedef stru_roominfo_wish roominfo_update_wish_in;
typedef stru_roominfo_wish   roominfo_get_wish_out;

struct pollinate_fruit {
	uint32_t plant_id;
	uint32_t fruitid[6];
};
struct  roominfo_change_gift_num_in{
	int32_t change_value;
}__attribute__((packed));
struct  roominfo_change_gift_num_out{
	uint32_t num;
}__attribute__((packed));


struct  roominfo_get_gift_num_out{
	uint32_t gift_num;
}__attribute__((packed));

struct roominfo_record_one_gift_userid {
	userid_t userid;
	uint8_t userid_info[25];
}__attribute__((packed));

typedef  roominfo_record_one_gift_userid roominfo_record_give_gift_userid_in;

struct roominfo_give_gift_userid {
	uint32_t count;
	roominfo_record_one_gift_userid items[100];	
	uint32_t get_real_len(){
		return 4+sizeof(roominfo_record_one_gift_userid)*this->count;
	}
}__attribute__((packed));

typedef roominfo_record_one_gift_userid roominfo_get_give_gift_userid_out;

struct  userinfo_get_login_info_out{
	char 		passwd[PASSWD_LEN];
	uint32_t	gameflag;
}__attribute__((packed));

struct  roominfo_add_energy_star_in{
	uint32_t add_num;
	uint32_t vip_flag;/* 0是非VIP，1是VIP */
}__attribute__((packed));
struct  roominfo_add_energy_star_out{
	uint32_t total_star;
	uint32_t day_star;
	uint32_t real_add;
}__attribute__((packed));

struct roominfo_get_energy_star_out {
	uint32_t total_star;
	uint32_t day_star;
}__attribute__((packed));

typedef stru_count user_dress_in_photo_set_in_header;

struct user_dress_in_photo_set_in_item
{
	uint32_t colth;
}__attribute__((packed));

typedef stru_count user_dress_in_photo_get_out_header;

struct user_dress_in_photo_get_out_item
{
	uint32_t colth;
}__attribute__((packed));

struct  user_attire_delete_attire_list_in{
	uint32_t start_index;
	uint32_t end_index;
}__attribute__((packed));

struct  user_get_vip_month_out{
	uint32_t month;
}__attribute__((packed));
struct  appeal_ex_get_user_last_state_in{
	uint32_t userid;
}__attribute__((packed));
struct  appeal_ex_get_user_last_state_out{
	uint32_t id ;
	uint32_t state	;
	uint32_t logtime ;
}__attribute__((packed));


struct  user_plant_growth_add_by_hour_in
{
	userid_t user_id; ///< 施肥的动作发出者
	uint32_t auto_id; ///< 数据库表中自动增加的ID
	uint32_t hours; ///< 需要增加的小时数
}__attribute__((packed));

struct jy_item_muck
{
	jy_item item;
	uint32_t last_muck_date;///< 上次施肥的日期
	uint32_t muck_times; ///< 当天施肥的次数
}__attribute__((packed));

struct  user_plant_growth_add_by_hour_out
{
	jy_item item;
}__attribute__((packed));

struct  user_info_set_falg_change_paypasswd_in{
	uint32_t flag;/* 0表示设置为需要检查旧密码，1表示不需要检查旧密码  */
}__attribute__((packed));

struct  user_info_change_paypasswd_nocheck_in{
	char new_paypasswd[16];
}__attribute__((packed));

struct  user_jy_get_mature_fruit_id_in
{
	uint32_t id;
}__attribute__((packed));

struct  user_jy_get_mature_fruit_id_out
{
	uint32_t fruit_id;
}__attribute__((packed));

/**
 * @brief 批量购买物品
 */
struct user_buy_many_thing_in_one_time_in_header
{
	uint32_t attire_in_where; ///< @see user_get_attire_list_in 
	uint32_t need_xiaomee_value; ///< 购买这些物品所需要的摩尔豆
	uint32_t number_of_thing; ///< 物品的个数
}__attribute__((packed));

struct user_buy_many_thing_in_one_time_in_item
{
	uint32_t attire_id;
	uint32_t count;
	uint32_t max;
}__attribute__((packed));

struct user_buy_many_thing_in_one_time_out
{
	uint32_t xiaomee;
}__attribute__((packed));

/**
 * @brief 宠物完成任务赠送物品
 */
struct user_pet_task_attire_give_in
{
	uint32_t petid; ///< 宠物编号
	uint32_t taskid; ///< 
	uint32_t attireid; ///< 宠物物品，需要赠送
}__attribute__((packed));

struct roominfo_stone_move_get_out
{
	uint32_t stone_number_today;
	uint32_t stone_number_total;
}__attribute__((packed));

struct roominfo_stone_move_set_out
{
	uint32_t stone_number_today;
	uint32_t stone_number_total;
}__attribute__((packed));

struct roominfo_penguin_egg_get_out
{
	uint32_t user_penguin_num;
	uint32_t user_penguin_client;
}__attribute__((packed));

struct roominfo_penguin_egg_set_in
{
	uint32_t user_penguin_flag;///< 0:抽取企鹅孵蛋次数，1:客户端私有数据
	uint32_t value;
}__attribute__((packed));

struct roominfo_penguin_egg_set_out
{
	uint32_t user_penguin_num;
	uint32_t user_penguin_client;
}__attribute__((packed));

struct roominfo_warrior_lucky_draw_in
{
	uint32_t attireid; ///< 勇士抽奖，抽中时的奖品
}__attribute__((packed));

struct roominfo_warrior_lucky_draw_out
{
	uint32_t attireid; ///< 勇士抽奖，0:没有抽中，其它：抽中的物品
}__attribute__((packed));

struct roominfo_warrior_lucky_draw_get_out
{
	uint32_t warrior_draw_times; ///< 抽奖的次数
	uint32_t warrior_draw_is_luck_times; ///< 抽中的次数
}__attribute__((packed));

struct user_pet_get_count_with_attireid_in_header
{
	uint32_t count; ///< 接下来的attiredid的数目
}__attribute__((packed));

struct user_pet_get_count_with_attireid_in_item
{
	uint32_t attireid; ///< 宠物装扮
}__attribute__((packed));

struct user_pet_get_count_with_attireid_out_header
{
	uint32_t count; ///< 接下来的attiredid的数目
}__attribute__((packed));

struct user_pet_get_count_with_attireid_out_item
{
	uint32_t attireid; ///< 宠物装扮
	uint32_t count; ///< 相应的attireid的数目
}__attribute__((packed));


typedef stru_count   user_attire_get_attire_except_chest_in_header;
struct  user_attire_get_attire_except_chest_in_item
{
	uint32_t attireid;
}__attribute__((packed));

typedef stru_count   user_attire_get_attire_except_chest_out_header;
struct  user_attire_get_attire_except_chest_out_item
{
	uint32_t attireid;
	uint32_t count;
}__attribute__((packed));

struct  user_pet_hot_spring_in
{
	uint32_t petid;///< 哪个宠物泡温泉
	uint32_t attireid;///< 消耗哪种物品
	uint32_t attire_count;///< 消耗物品多少个
	uint32_t birthday_logic_shift_second; ///< 取得提前成长的秒数
}__attribute__((packed));

struct  user_pet_hot_spring_out
{
	uint32_t petid;///< 哪个宠物泡温泉
	uint32_t logic_birthday; ///< 经处理后的逻辑生日
}__attribute__((packed));

struct  user_pet_skill_set_in
{
	uint32_t petid;///< 哪个宠物更新
	uint32_t type; ///< 更新哪个技能点:0, 火系；1, 水系；2, 木系
	uint32_t n_pos; ///< 设置第n_pos位
	uint32_t n_value; ///< 设置第n_pos位的值
}__attribute__((packed));

struct  user_pet_halt_set_in
{
	uint32_t type; ///< 更新哪种宫殿:0, 火系；1, 水系；2, 木系
	uint32_t height; ///<  宫殿的高度
}__attribute__((packed));

struct  user_pet_halt_set_out
{
	uint32_t height_after_set; ///<  宫殿的高度
}__attribute__((packed));

/*
struct  user_pet_halt_get_in
{
	uint32_t type; ///< 设置哪种宫殿:0, 火系；1, 水系；2, 木系
}__attribute__((packed));
*/

struct  user_pet_halt_get_out
{
	uint32_t fire_height; ///<  宫殿的高度
	uint32_t water_height; ///<  宫殿的高度
	uint32_t wood_height; ///<  宫殿的高度
}__attribute__((packed));

struct user_pet_task_set_task_in
{
	uint32_t petid;
	uint32_t taskid;
	uint32_t value;
}__attribute__((packed));

struct  user_pet_task_get_tasklist_in
{
	uint32_t taskid;
	uint32_t petid;
}__attribute__((packed));

struct  user_pet_task_get_tasklist_out
{
	uint32_t value;
}__attribute__((packed));

struct  user_pet_task_set_temp_data_in
{
	uint32_t petid;
	uint32_t taskid;
	uint8_t tasktemp[50];
}__attribute__((packed));

struct  user_pet_task_get_temp_data_in
{
	uint32_t petid;
	uint32_t taskid;
}__attribute__((packed));

struct  user_pet_task_get_temp_data_out
{
	uint8_t tasktemp[50];
}__attribute__((packed));

struct user_pet_task_get_by_user_in
{
	uint32_t petid; ///< 宠物编号
	uint32_t taskstat; ///< 任务状态：0, 未开始， 1:已经开始， 2： 已经结束
}__attribute__((packed));

struct  user_pet_task_get_by_user_out_header
{
	uint32_t petid; ///< 宠物编号
	uint32_t taskstat; ///< 任务状态：0, 未开始， 1:已经开始， 2： 已经结束
	uint32_t count; ///< 随后的列表个数
}__attribute__((packed));

struct  user_pet_task_get_by_user_out_item
{
	uint32_t taskid; ///< 任务编号
}__attribute__((packed));

struct  user_use_skill_one_in
{
	uint32_t skillid;
	uint32_t attireid;
	uint32_t size_max;
}__attribute__((packed));

struct  user_pet_task_get_client_by_user_task_in
{
	uint32_t taskid; ///< 任务编号
	uint32_t taskstat; ///< 任务状态：0, 未开始， 1:已经开始， 2： 已经结束
}__attribute__((packed));

struct  user_pet_task_get_client_by_user_task_out_header
{
	uint32_t count; ///< 随后的列表个数
}__attribute__((packed));

struct  user_pet_task_get_client_by_user_task_out_item
{
	uint32_t petid; ///< 宠物编号
	char data_from_client[TASK_LIST_EX_LEN]; ///< 客户端私有数据
}__attribute__((packed));

struct  roominfo_client_data_get_in
{
	uint32_t id; ///< 指定私有数据的编号
}__attribute__((packed));

struct  roominfo_client_data_get_out
{
	uint32_t id; ///< 指定私有数据的编号
	uint32_t data; ///< 指定私有数据
}__attribute__((packed));

struct  roominfo_client_data_set_in
{
	uint32_t id; ///< 指定私有数据的编号
	uint32_t data; ///< 指定私有数据
}__attribute__((packed));

#endif   /* ----- #ifndef PROTO_INC  ----- */

