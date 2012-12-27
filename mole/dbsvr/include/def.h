/*
 * =====================================================================================
 * 
 *       Filename:  month_err.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年02月26日 18时12分59秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#ifndef  DEF_INC
#define  DEF_INC

#define MONTH_SUCC                0
#define MONTH_ERR_NO_DEFINE       1
#define MONTH_ERR_NO_DEAL         2
#define MONTH_ERR_USERID_NOFIND   3
#define MONTH_ERR_DAMEE_NO_ENOUGH 4

#define XIAOMEE_USE_NO_DEFINE  	  		0
#define XIAOMEE_USE_ADD_SU        		1
#define XIAOMEE_USE_DEL_SU        		2
#define XIAOMEE_USE_DEL_BUY_ATTIRE    	3
#define XIAOMEE_USE_ADD_BACK			4  //回滚
#define XIAOMEE_USE_ADD_QQ				5  //QQ活动中增加摩尔豆
#define XIAOMEE_USE_ADD_SALE			101//通过卖出获得的摩尔豆 


#define XIAOMEE_USE_DEL_BANK			200//存款
#define XIAOMEE_USE_ADD_BANK			201//取款
#define XIAOMEE_USE_ADD_NPC_TASK		202//任务
#define XIAOMEE_SALE_PIGLET_ITEM        203//猪仔系统道具

#define XIAOMEE_USE_DEL_BY_AUCTION		1000//伊莲慈善派对捐钱
#define XIAOMEE_USE_ADD_BY_GAME			1001//玩游戏
#define XIAOMEE_USE_DEL_BY_PET			1002//买宠物
#define XIAOMEE_USE_ADD_BY_PARTY		1003//参加活动
#define XIAOMEE_USE_ADD_BY_TASK			1004//完成任务
#define XIAOMEE_USE_ADD_BY_USER_GET		1005//客户端直接索取

#define XIAOMEE_BUY_CAR					1006//购买汽车
#define XIAOMEE_ADD_OIL					1007//给汽车加油

#define XIAOMEE_USE_ADD_ANGEL_BATTLE    2000//天使对战
#define XIAOMEE_USE_DEL_ANGEL_BATTLE    2001

#define XIAOMEE_USE_DEL_PIGLET          2002//猪倌建筑物升级
#define XIAOMEE_USE_GOLDEN_KEY          2003//获取金钥匙

#define USER_SWAP_ATTIRE_NO_LIMIT		3000//reason_ex，114D协议增加attire如果某一个不成功时不返回
#define USER_SWAP_IS_VIP_OPT            10000//用来表示是否是vip操作




#define PAY_SUCC                0
#define PAY_ERR_NO_DEFINE       1
#define PAY_ERR_NO_DEAL         2
#define PAY_ERR_USERID_NOFIND   3
#define PAY_ERR_DAMEE_NO_ENOUGH 4


#define  PAY_TYPE_DAMEE 		1
#define  PAY_TYPE_MONTH 		2

#define  PAY_TYPE_DAMEE_DAMEE 	1000
#define  PAY_TYPE_DAMEE_NCP 	1001
#define  PAY_TYPE_DAMEE_ZFB 	1002 
#define  PAY_TYPE_DAMEE_CFT 	1003 
#define  PAY_TYPE_DAMEE_SINA 	1004 
#define  PAY_TYPE_DAMEE_SERIAL 	1005 


#define  PAY_TYPE_MONTH_DAMEE 	2000
#define  PAY_TYPE_MONTH_NCP 	2001
#define  PAY_TYPE_MONTH_ZFB 	2002 
#define  PAY_TYPE_MONTH_CFT 	2003 
#define  PAY_TYPE_MONTH_SINA 	2004 
#define  PAY_TYPE_MONTH_SERIAL 	2005 

#define  PAY_TYPE_SYS_CHANGE 	5000 
#define  PAY_TYPE_BUY_ATTIRE 	5001 

#endif   /* ----- #ifndef DEF_INC  ----- */

