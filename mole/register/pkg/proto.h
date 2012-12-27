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
#define NEED_CACHED_DEAL 					0x0200

//注册用户-ex
#define USERINFO_REGISTER_EX_CMD			(0x0003 | NEED_UPDATE)  
//删除用户
#define USERINFO_DEL_USER_CMD				(0x0004 | NEED_UPDATE)
//检查米米号是否存在
#define USERINFO_CHECK_EXISTED_CMD			(0x0005 )
//
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



//修改密码
#define USERINFO_CHANGE_PASSWD_NOCHECK_CMD	(0x0040 | NEED_UPDATE)
//设置生日,性别
#define USERINFO_SET_USER_BIRTHDAY_SEX_CMD  (0x0041 | NEED_UPDATE)
#define USERINFO_IS_SET_USER_BIRTHDAY_CMD   (0x0042 )
#define USERINFO_SET_USER_EMAIL_CMD 	  	(0x0043 | NEED_UPDATE)
#define USERINFO_GET_USER_EMAIL_CMD   		(0x0044 )


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
#define USER_GET_INFO_EX_CMD             	(0x1006 )

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

//得到另外的一些信息   
#define USER_GET_INFO_EX2_CMD			(0x100C )


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

//交换装扮列表:in_item_1个数为oldcount,in_item_2个数为newcount,
#define USER_SWAP_ATTIRE_LIST_CMD		(0x1019 | NEED_UPDATE)
//设置装扮
#define USER_SET_ATTIRE_EX_CMD			(0x101A | NEED_UPDATE)


//user home attire
//设置小屋装扮<br />usedcount=IN_ITEM_2.count<br />nousedcount=IN_ITEM_1.count
#define USER_SET_HOME_ATTIRELIST_CMD	(0x101B | NEED_UPDATE)
//增加物品
#define USER_ADD_ATTIRE_CMD				(0x101C | NEED_UPDATE)

//得到小屋中的使用中的物品信息
#define USER_GET_HOME_CMD				(0x101D	)


//交换小屋物品: 定义：IN_ITEM_1.count=oldcount,IN_ITEM_2.count=newcount
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


#define USER_GET_USER_CMD				(0x1031)
//设置摩尔颜色
#define USER_SET_PETCOLOR_CMD			(0x1032 | NEED_UPDATE)
#define USER_GET_USER_WITH_MSGLIST_CMD	(0x1033)
#define USER_GET_USER_ALL_CMD			(0x1034)
//设置装扮
#define USER_SET_ATTIRE_CMD				(0x1036 | NEED_UPDATE)
#define USER_DEL_ATTIRE_CMD				(0x1037 | NEED_UPDATE)
//修改数值
#define USER_CHANGE_USER_VALUE_CMD 		(0x1039 | NEED_UPDATE)

//设置已使用中的家具
#define USER_SET_USED_HOMEATTIRE_CMD    (0x103A |NEED_UPDATE)
//设置已使用中的家园中的数据
#define USER_SET_USED_JYATTIRE_CMD    	(0x103B |NEED_UPDATE)



#define USER_CHANGE_PASSWD_NOCHECK_CMD	(0x1040 | NEED_UPDATE)
#define USER_SET_USER_BIRTHDAY_SEX_CMD  (0x1041 | NEED_UPDATE)
#define USER_SET_USER_EMAIL_CMD   		(0x1043 | NEED_UPDATE)
#define USER_GET_USER_EMAIL_CMD   		(0x1044 )
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

#define USER_SET_FLAG_UNUSED_CMD		(0x105B | NEED_UPDATE)
#define USER_SET_FLAG_OFFLINE24_CMD		(0x105C | NEED_UPDATE)

//大使
#define USER_CONNECT_ADD_CHILD_CMD		(0x1060 | NEED_UPDATE)
//大使
#define USER_CONNECT_SET_PARENT_CMD		(0x1061 | NEED_UPDATE)
//大使
#define USER_CONNECT_GET_INFO_CMD		(0x1062 )


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

//宠物任务：设置
#define USER_PET_TASK_SET_CMD 			(0x1071 | NEED_UPDATE)

//宠物任务：得到列表
#define USER_PET_TASK_GET_LIST_CMD 		(0x1073 )

//user_card
//卡片系统:得到信息<br />定义cards的个数为count 如count=0 表示未初始化 <br \>cards中前lower_count个为低级卡片，其后为高级卡片
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

//得到昵称和好友列表
#define USER_GET_NICK_FLIST_CMD         (0x1089 )

//银行命令号
//插入存款记录
#define USER_BANK_ADD_CMD                   (0x1090 |NEED_UPDATE)
//得到全部存款记录
#define USER_BANK_GET_ACCOUNT_CMD           (0x1091 )
//删除存款记录
#define USER_BANK_DELETE_ACCOUNT_CMD        (0x1092 |NEED_UPDATE)


//设置存款记录
#define USER_BANK_SET_ACCOUNT_CMD        	(0x1093 |NEED_UPDATE)

//家园:得到种子列表
#define USER_GET_SEED_LIST_CMD           	(0x1098 )

//家园:设置种子
#define USER_SET_SEED_EX_CMD             	(0x1099 |NEED_UPDATE)

//得到家园访问列表
#define USER_GET_ACCESS_JYLIST_CMD       	(0x10A0 )
//访问家园
#define USER_ACCESS_JY_CMD               	(0x10A1 |NEED_UPDATE)

//加入黑名单
#define USER_ADD_BLACKUSER_CMD	 			(0x10B1 | NEED_UPDATE)
//删除黑名单
#define USER_DEL_BLACKUSER_CMD 				(0x10B2 | NEED_UPDATE)
//得到黑名单列表
#define USER_GET_BLACKUSER_LIST_CMD		 	(0x10B3 )
//更新任务
#define USER_TASK_SET_CMD                   (0x10B9 |NEED_UPDATE)
//删除任务
#define USER_TASK_DEL_CMD                   (0x10BA |NEED_UPDATE)
//得到任务
#define USER_TASK_GET_CMD                   (0x10BB )

//得到所在群列表:groupidlist 的个数为count
#define USER_GET_GROUP_CMD           	(0x10C0 )
//加入群
#define USER_JOIN_GROUP_CMD          	(0x10C1 |NEED_UPDATE)

//离开群
#define USER_LEAVE_GROUP_CMD         	(0x10C2 |NEED_UPDATE)

//离开群 
#define USER_DEL_GROUP_CMD           	(0x10C4 |NEED_UPDATE)

//设置群的个人设定
#define USER_SET_GROUP_FLAG_CMD      	(0x10C5 |NEED_UPDATE)

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



//email db
#define GET_USERID_BY_EMAIL_CMD		 	(0x4004)
#define MAP_EMAIL_USERID_CMD	 		(0x4005 | NEED_UPDATE)
#define DEL_MAP_EMAIL_USERID_CMD	 	(0x4006 | NEED_UPDATE)
#define EMAIL_SET_EMAIL_CMD 			(0x4007 | NEED_UPDATE)

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

//注册序列号 db
#define GET_REGISTER_SERIAL_CMD	 		(0x8001 | NEED_UPDATE)

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

#define SYSARG_GET_COUNT_CMD 			(0xC011)
#define SYSARG_SET_COUNT_CMD 			(0xC012| NEED_UPDATE)
#define SYSARG_GET_COUNT_LIST_CMD 		(0xC013)

//用户投票(调查)
#define SYSARG_ADD_QUESTIONNAIRE_CMD     	(0xC020 |NEED_UPDATE)



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
//#define HOME_ADD_REQUEST_CMD 			(0xB401| NEED_UPDATE)

#define ROOMINFO_GET_HOT_CMD             	(0xB401 )
#define ROOMINFO_VOTE_ROOM_CMD           	(0xB403 |NEED_UPDATE)
#define ROOMINFO_PUG_ADD_CMD             	(0xB404 |NEED_UPDATE)
#define ROOMINFO_PUG_LIST_CMD            	(0xB405 )
//小屋 增加HOT值
#define ROOMINFO_ADD_HOT_CMD             	(0xB406 |NEED_UPDATE)
#define ROOMINFO_INIT_MUD_CMD            	(0xB409 |NEED_UPDATE)
#define ROOMINFO_INIT_USER_CMD           	(0xB410 |NEED_UPDATE)


//小屋：清除一个泥巴
#define ROOMINFO_DEL_MUD_CMD             	(0xB411 |NEED_UPDATE)

#define ROOMINFO_SET_INFO_CMD            	(0xB412 |NEED_UPDATE)

#define ROOMMSG_ADD_MSG_CMD                     (0xC411 |NEED_UPDATE)
#define ROOMMSG_SHOW_MSG_CMD                    (0xC412 |NEED_UPDATE)
#define ROOMMSG_INDEX_MSG_CMD                   (0xC413 )
#define ROOMMSG_LIST_MSG_CMD                    (0xC414 )
#define ROOMMSG_DEL_MSG_CMD                     (0xC415 |NEED_UPDATE)
#define ROOMMSG_RES_MSG_CMD                     (0xC416 |NEED_UPDATE)

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

//不路由的FC
//sendemail 
//#define OTHER_SEND_EMAIL_CMD            (0x1014 | NEED_UPDATE)

//VIP
#define OTHER_SYNC_VIP_CMD            	(0xF001 | NEED_UPDATE)


//聊天记录
#define LOG_ADD_CHAT_CMD                 	(0xF002 | NEED_UPDATE)

//重要修改记录
#define LOG_ADD_CHANGE_CMD               	(0xF003 | NEED_UPDATE)
//密码修改记录
#define PASSWD_ADD_CHANGE_CMD                (0xF004 |NEED_UPDATE)




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

#define LIST_COUNT_SIZE  			(sizeof ( stru_count))

//home attire item 
#define HOME_ATTIRE_ITEM_VALUE_LEN 	12 
#define HOME_ATTIRE_ITEM_MAX 		100 
#define HOME_NOUSE_ATTIRE_ITEM_MAX 	300 

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
#define MAX_SEED_NUM 				20
//user_task_ex的缓存的长度
#define TASK_LIST_EX_LEN			50	

struct stru_count{
	uint32_t		count;
} __attribute__((packed))  ;

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


struct  group_item_without_id{
	userid_t	groupownerid;
	char		groupname[GROUPNAME_LEN];
	uint32_t	type;
	char		groupmsg[GROUPMSG_LEN];
	uint32_t	membercount;
	uint32_t	memberlist[GROUPITEM_LEN];
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

struct  group_cup_item{
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


struct stru_appeal{
	uint32_t	id  ;
	uint32_t	state ;
	uint32_t	dealflag;
	uint32_t	dealtime;
	uint32_t	logtime  ;
	uint32_t	userid  ;
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
	char 		passwd[PASSWD_LEN];
}__attribute__((packed));
struct  userinfo_login_ex_out{
	uint32_t	gameflag;
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

struct  user_change_passwd_nocheck_in{
	char newpasswd[PASSWD_LEN];
}__attribute__((packed));

typedef user_change_passwd_nocheck_in dv_change_passwd_in;
typedef user_change_passwd_nocheck_in userinfo_change_passwd_nocheck_in;

struct  user_change_nick_in{
	char newnick[NICK_LEN];
}__attribute__((packed));

typedef user_change_nick_in dv_change_nick_in;
typedef user_change_nick_in userinfo_set_nick_in;
typedef nick_stru user_set_nick_in;


struct  stru_tasklist{
	uint8_t		list[50];
}__attribute__((packed));



struct user_without_id{
		uint32_t 		vip; 
		uint32_t 		flag; 
		uint32_t		petcolor;
		uint32_t 		petbirthday;  /* set register time */
		uint32_t 		xiaomee;/*小米*/
		uint32_t 		xiaomee_max;/*小米*/
		uint32_t		exp;
		uint32_t		strong;
		uint32_t		iq;
		uint32_t		charm;
		uint32_t 		Ol_count; /*登入次数 */
		uint32_t 		Ol_today; /*last login */
		uint32_t		Ol_last; /* 最后一次登入时间 */
		uint32_t		Ol_time; /*在线总时间 */
		uint32_t		last_ip; /*在线总时间 */
		uint32_t		birthday; /*在线总时间 */
		stru_tasklist 		tasklist; /*任务完成列表*/
		uint32_t 		flag2; 
		char  			nick[NICK_LEN]; 
}__attribute__((packed));
typedef user_without_id user_get_user_all_out;


struct user_info_item{
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

struct user_info_ex_item{
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



struct  creategroupid_list {
	uint32_t	count;
	stru_group_item creategroupid[CREATE_GROUP_MAX];
}__attribute__((packed));



struct  userinfo_get_info_out{
		uint32_t 		flag1;
		uint32_t 		flag2;
		char 			nick[NICK_LEN];  
		uint32_t  		creategroup_count;
		uint32_t  		creategroup_id1;
		uint32_t  		creategroup_id2;
		uint32_t  		creategroup_id3;
}__attribute__((packed));

struct  user_get_info_ex2_out{
		uint32_t 		flag;
		char 			nick[NICK_LEN];  
		uint32_t  		creategroup_count;
		uint32_t  		creategroup_id1;
		uint32_t  		creategroup_id2;
		uint32_t  		creategroup_id3;
}__attribute__((packed));

struct  stru_user_other{
		uint32_t 		flag;
		char 			nick[NICK_LEN];  
		uint32_t  		creategroup_count;
		uint32_t  		creategroup_id1;
		uint32_t  		creategroup_id2;
		uint32_t  		creategroup_id3;
}__attribute__((packed));






struct  userinfo_get_info_all_out{
	user_info_item info;	 
	user_info_ex_item  info_ex;	 
}__attribute__((packed));




typedef  user_info_ex_item userinfo_get_info_ex_out;
typedef  user_info_ex_item userinfo_set_info_ex_in;


struct    user_set_user_birthday_sex_in {
		uint32_t 		birthday; 
		uint32_t 		sex; 
}__attribute__((packed));
typedef user_set_user_birthday_sex_in userinfo_set_user_birthday_sex_in;


struct user_set_user_online_in {
		uint32_t  onlinetime; 
		uint32_t  ip; 
}__attribute__((packed));

struct  user_get_flag_out  {
		uint32_t   flag;  
		uint32_t   vip;  
}__attribute__((packed));



struct      user_set_home_attire_noused_in{
		uint32_t 	type;  
		uint32_t 	attireid;  
}__attribute__((packed));



struct home_attire_item {
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

struct attire_count{
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

struct noused_homeattirelist{
		uint32_t	count;
		attire_noused_item item[HOME_NOUSE_ATTIRE_ITEM_MAX];
} __attribute__((packed));


struct    add_attire{
		uint32_t	attiretype;/*装扮0,小屋1*/
		uint32_t	attireid;
		uint32_t	count;
		uint32_t	maxcount;
} __attribute__((packed));



typedef add_attire  user_add_attire_in; 
typedef add_attire  user_del_attire_in; 


struct   user_set_home_attirelist_in_header{
		uint32_t	usedcount;
		uint32_t	nousedcount;
} __attribute__((packed)) ;

struct home_attirelist{
		uint32_t	count;
		home_attire_item item[HOME_ATTIRE_ITEM_MAX];
} __attribute__((packed));

typedef user_set_home_attirelist_in_header user_set_jy_attirelist_in_header;

typedef home_attire_item  user_set_home_attirelist_in_item_1;
typedef attire_noused_item user_set_home_attirelist_in_item_2;

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
} __attribute__((packed));

typedef add_attire pay_buy_attire_by_damee_in ;


struct pay_buy_attire_by_damee_out {
	uint32_t  leave_damee;  /*剩余大米*/
} __attribute__((packed));

struct    pay_add_damee_by_serial_in{
 	char serialid[20];
} __attribute__((packed));

typedef pay_add_damee_by_serial_in serial_get_noused_id_out;

struct pay_add_damee_by_serial_out {
	uint32_t  price;  /*价值 */
	uint32_t  leave_damee;  /*剩余大米*/
} __attribute__((packed));

typedef stru_flag userinfo_set_vip_flag_in ;
typedef stru_flag user_set_vip_flag_in ;
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


struct user_get_home_out{
	home_attirelist attirelist;
} __attribute__((packed));
typedef user_get_home_out  user_get_jy_ex_out ;

struct user_get_jy_out_header {
	uint32_t hometype_id;
	uint32_t	item1_count;
	uint32_t	item2_count;
} __attribute__((packed));

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
	uint32_t mature_time;
} __attribute__((packed));

//种子的ID号
struct  user_water_seed_in{
	uint32_t id;
	uint32_t opt_userid;
}__attribute__((packed));

typedef jy_item user_water_seed_out;

struct  user_kill_bug_in {
	uint32_t id;
	uint32_t opt_userid;
}__attribute__((packed));



typedef jy_item user_kill_bug_out; 
/*struct  user_kill_bug_in{


}ttribute__((packed));
struct  user_kill_bug_out{


}ttribute__((packed));*/


/*struct  user_water_seed_out{

}ttribute__((packed));*/


struct seed_info {

    uint32_t item;
    uint32_t fruitid;
    uint32_t healthy_grow_rate;
    uint32_t drought_grow_rate;
    uint32_t insect_grow_rate;
    uint32_t di_grow_rate;
    uint32_t drought_rate;
    uint32_t insect_rate;
    uint32_t mature;
	uint32_t fruitsick;
    uint32_t fruits[4];
	//uint32_t fruitsick;
   /*uint32_t healthy_fruits;
    uint32_t drought_fruits;
    uint32_t insect_fruits;
    uint32_t di_fruits;*/
};


typedef home_attire_item user_get_jy_out_item_1;
typedef jy_item user_get_jy_out_item_2;


struct   user_set_petcolor_in {
		uint32_t		petcolor;
} __attribute__((packed));
typedef   stru_tasklist user_set_tasklist_in;

struct   get_user_out{
		stru_user_other user_other;
		uint32_t 		vip; 
		uint32_t 		flag; 
		uint32_t		petcolor;
		uint32_t 		petbirthday;  /* set register time */
		uint32_t 		xiaomee;/*小米*/
		uint32_t		exp;
		uint32_t		strong;
		uint32_t		iq;
		uint32_t		charm;
		uint32_t 		Ol_count; /*登入次数 */
		uint32_t 		Ol_today; /*last login */
		uint32_t		Ol_last; /* 最后一次登入时间 */
		uint32_t		Ol_time; /*在线总时间 */
		uint32_t		birthday; /*在线总时间 */
		stru_tasklist 		tasklist; /*任务完成列表*/
		uint32_t		parentid; /*邀请人*/
		uint32_t		childcount; /**/
		uint32_t		old_childcount; /**/
		uint32_t		is_bank_daoqi; /*到期*/
		stru_msg_list  		msglist;
} __attribute__((packed));
typedef get_user_out user_get_user_out;
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

typedef attire_property user_set_attire_ex_in;

typedef attire_count user_get_attire_list_out_item;


struct  user_set_attire_in{
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


typedef stru_count user_get_attire_list_out_header;


typedef stru_email user_set_user_email_in;
typedef stru_email user_get_user_email_out;
 
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

struct  user_get_user_pet_out_item{
	uint32_t petid; 
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
	uint32_t skill;//技巧
	uint32_t stamp;
}__attribute__((packed)); 

typedef user_get_user_pet_out_item user_set_user_pet_all_in;

struct  user_add_user_pet_in{
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
}__attribute__((packed));

struct  user_task_item{
	uint32_t petid;
	uint32_t taskid;
	uint32_t flag;
	uint32_t usetime;
	uint32_t startime;
	uint32_t endtime;
}__attribute__((packed));


struct  user_pet_task_set_in{
	uint32_t petid;
	uint32_t taskid;
	uint32_t flag;
	uint32_t usetime;
	uint32_t startime;
	uint32_t endtime;
	/*if usetime >=def_usetime then flag=def_flag  */
	uint32_t def_usetime;
	uint32_t def_flag;
}__attribute__((packed));


struct  user_pet_task_set_out{
	uint32_t petid;
	uint32_t taskid;
}__attribute__((packed));


struct  user_pet_task_get_list_in {
	uint32_t petid;
	uint32_t start_taskid;
	uint32_t end_taskid;
	uint32_t flag;
}__attribute__((packed));




struct   user_connect_get_info_out{
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
typedef type_value_stru sysarg_get_count_list_out_item;



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

typedef stru_count   user_set_used_homeattire_in_header;
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

struct   roominfo_pug_list_out{
	uint16_t	count;
	rooninfo_pug_item item[50];
}__attribute__((packed));

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
	uint32_t maintype;
	uint32_t subtypecount;
}__attribute__((packed));

struct  sysarg_add_questionnaire_in_item {
	uint32_t subtype;
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

struct roommsg_show_msg_in
{
    uint32_t    msg_id;
    uint8_t    is_show;
}__attribute__((packed));


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

struct roommsg_del_msg_in
{
    uint32_t    msg_id;
    uint32_t    flag;
}__attribute__((packed));

struct roommsg_res_msg_in
{
    uint32_t    msg_id;
    char        res_buf[100];
}__attribute__((packed));

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
    uint32_t win_count;
    uint32_t lose_count;
    uint32_t lower_count;
    uint32_t super_count;
	user_cardlist cardList;
}__attribute__((packed));

struct user_card_info{
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
}__attribute__((packed));


typedef user_get_attire_list_out_item user_get_info_ex_out_item_1;
typedef user_get_user_pet_out_item user_get_info_ex_out_item_2;
typedef user_get_pet_attire_used_all_out_item user_get_info_ex_out_item_3;


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
	uint32_t value;
}__attribute__((packed));


struct  user_add_seed_in{
	uint32_t attireid;
	uint32_t x;
	uint32_t y;
}__attribute__((packed));

typedef jy_item  user_add_seed_out;

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

typedef jy_item  user_get_seed_list_out_item;
typedef jy_item user_set_seed_ex_in;

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
	uint32_t	attiretype;/*装扮0,小屋1*/
	uint32_t	startattireid;
	uint32_t	endattireid;
	uint8_t		attire_usedflag;
} __attribute__((packed));

//user_task_ex 

/**
 * @brief 更新数据的请求包体
 */
struct user_task_set_in {
	uint32_t taskid;
	char data[TASK_LIST_EX_LEN];
}__attribute__((packed));

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
struct  appeal_get_list_in{
	uint32_t state;
	uint32_t dealflag;
	uint32_t userid;
	uint32_t pageid;
}__attribute__((packed));
typedef stru_count   appeal_get_list_out_header;
typedef stru_appeal appeal_get_list_out_item; 

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
}__attribute__((packed));



struct  get_userid_by_email_out{
	uint32_t	map_userid;
}__attribute__((packed));
/*
struct stru_attire_count_list{
	attire_count attire_item[4][20]; 
};

struct stru_attire_count_max_list{
	attire_count_with_max attire_max_item[4][20]; 
};
  */

#endif   /* ----- #ifndef PROTO_INC  ----- */


