/*
 * =====================================================================================
 * 
 *	   Filename:  common.h
 * 
 *	Description:  
 * 
 *		Version:  1.0
 *		Created:  2007年11月02日 18时33分18秒 CST
 *	   Revision:  none
 *	   Compiler:  gcc
 * 
 *		 Author:  xcwen (xcwen), xcwenn@gmail.com
 *		Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  proxy_route   
#define  proxy_route 

//库标示:
//当第一个bit为1时：			1111110111 00000 	==库标示9位(64个库)(可用命令号32个 )
//当第一个bit为0时：(cmdid>>9): 1111110  00000000 ==库标示6位(512个库)(可用命令号256个 )
#define GET_ROUTE(cmdid) ((cmdid&0x8000)?(70+((cmdid&0x7E00)>>6)+((cmdid &0x00E0)>>5)):(cmdid>>9))  
#define get_db_type( cmdid )  enum_route_type( GET_ROUTE(cmdid) )

//define route db for com 
enum enum_route_type {
	ROUTE_NULL=-1, 
	ROUTE_USERINFO=GET_ROUTE(0x0000),//用户基础信息库
	ROUTE_XHX=GET_ROUTE(0x0200),//小花仙
	ROUTE_XHXOTHER=GET_ROUTE(0x0400),//小花仙.other
	ROUTE_GF=GET_ROUTE(0x0600),//功夫
	ROUTE_MOLE2=GET_ROUTE(0x0800),//MOLE2
	ROUTE_SESSION=GET_ROUTE(0x0A00),//base.session server ,
	ROUTE_REG_USERID=GET_ROUTE(0x0C00),//base.注册米米号
	ROUTE_GF_OTHER=GET_ROUTE(0x0E00),//功夫_OTHER

	ROUTE_USER=GET_ROUTE(0x1000),//mole.用户数据-摩尔庄园
	ROUTE_PAIPAI_2=GET_ROUTE(0x1200),//pp2
	ROUTE_HOPE=GET_ROUTE(0x1400),//mole.许愿池
	ROUTE_PAIPAI=GET_ROUTE(0x1800),//派派
	ROUTE_PP_EMAIL=GET_ROUTE(0x1A00),//派派明信片
	ROUTE_DD=GET_ROUTE(0x1C00),//DUDU


	ROUTE_VIP=GET_ROUTE(0x2000),//会员系统
	ROUTE_MOLE_DINING=GET_ROUTE(0x2200),//mole.餐厅
	ROUTE_PRODUCE=GET_ROUTE(0x2400),//产品管理
	ROUTE_BAOBAO=GET_ROUTE(0x2800),//mole宝宝

	ROUTE_USER_2=GET_ROUTE(0x3000),//mole.用户数据-摩尔庄园2
	ROUTE_GROUPMAIN=GET_ROUTE(0x3400),//mole.产生群号
	ROUTE_PPTASKDAY=GET_ROUTE(0x3800),//派派的每日每周任务
	ROUTE_USER_LOGIN=GET_ROUTE(0x3C00),//用户登入信息

	ROUTE_EMAIL=GET_ROUTE(0x4000),//email对应的米米号
	ROUTE_MMS=GET_ROUTE(0x4400),//mole.毛毛树,金蘑菇向导,...
	ROUTE_PPOTHER=GET_ROUTE(0x4800),//派派的其它的数据

	ROUTE_GAME_SCORE=GET_ROUTE(0x5000),//mole.游戏排名
	ROUTE_ADMIN=GET_ROUTE(0x5400),//整体权限管理
	ROUTE_PP_PK_TEAM=GET_ROUTE(0x5800),//派派的战队信息

	ROUTE_SERIAL=GET_ROUTE(0x6000),//mole.神奇密码
	ROUTE_PARTY=GET_ROUTE(0x6400),//mole.PARTY

	ROUTE_MSGBOARD=GET_ROUTE(0x7000),//mole.记者投稿
	ROUTE_MOLE_NOAH=GET_ROUTE(0x7200),//mole.诺亚舟
	ROUTE_PICTURE=GET_ROUTE(0x7400),//mole.PICTURE图片

	ROUTE_GAME=GET_ROUTE(0x8000),//mole.用户的游戏积分
	ROUTE_USERMSG =GET_ROUTE(0x9000),//用户投稿

	ROUTE_SU=GET_ROUTE(0xA000),//mole.客服系统,定时信息

	ROUTE_ROOM=GET_ROUTE(0xB400),//mole.小屋信息
	ROUTE_ROOM_2=GET_ROUTE(0xB420),//mole.小屋信息
	ROUTE_ROOM_4=GET_ROUTE(0xB440),//mole.小屋信息
	ROUTE_ROOM_6=GET_ROUTE(0xB460),//mole.小屋信息
	ROUTE_ROOM_8=GET_ROUTE(0xB480),//mole.小屋信息

	ROUTE_SYSARG=GET_ROUTE(0xC000),//mole.系统参数
	ROUTE_SYSARG_2=GET_ROUTE(0xC020),//mole.系统参数
	ROUTE_SYSARG_4=GET_ROUTE(0xC040),//mole.系统参数
	ROUTE_SYSARG_6=GET_ROUTE(0xC060),//mole.系统参数

	ROUTE_ROOMMSG=GET_ROUTE(0xC400),//mole.小屋留言 
	ROUTE_ROOMMSG_2=GET_ROUTE(0xC420),//mole.小屋留言 

	ROUTE_TEMP=GET_ROUTE(0xD000),//mole.活动使用的,时间过后，就删掉
	ROUTE_TEMP_2=GET_ROUTE(0xD020),//mole.活动使用的,时间过后，就删掉
	ROUTE_TEMP_4=GET_ROUTE(0xD040),//mole.活动使用的,时间过后，就删掉

	ROUTE_GROUP=GET_ROUTE(0xD400),//mole.群组
	ROUTE_MOLE2_RAND_ITEM=GET_ROUTE(0xD420),//

	ROUTE_EMAIL_SYS=GET_ROUTE(0xE000),//mole.邮件系统

	ROUTE_NOROUTE=GET_ROUTE(0xF000),//不用路由

	ROUTE_IM_USER_CACHE=GET_ROUTE(0xF020),//im.用户信息缓存
	ROUTE_IM_ONLINE_INFO=GET_ROUTE(0xF040),//im.用户在线信息
	ROUTE_IM_USER=GET_ROUTE(0xF060),//im.用户数据库
	ROUTE_IM_USER_2=GET_ROUTE(0xF080),//im.用户数据库

}; 

#endif   /* ----- #ifndef COMMON_INC  ----- */

