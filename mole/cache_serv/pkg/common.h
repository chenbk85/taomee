/*
 * =====================================================================================
 * 
 *       Filename:  common.h
 * 
 *    Description:  本文件是一个被广泛包含的文件，它定义了错误码，定义了大量的宏，这些
 *    			宏会被用于dbser，作为整个框架的基础
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

#ifndef  COMMON_INC
#define  COMMON_INC
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "def.h" 

#define             T         		        1
#define         F                   		0


#define SUCC 								0	
#define FAIL 								-1	
#define DB_SUCC 							0	
//nash home  5000-6000:

//没有定义
#define NO_DEFINE_ERR   					1000
//系统出错 一般是内存出错
#define SYS_ERR   							1001

//数据库出错
#define DB_ERR   							1002

//NET出错
#define NET_ERR   							1003

//命令ID没有定义
#define CMDID_NODEFINE_ERR 			  		1004
//协议长度不符合
#define PROTO_LEN_ERR  			 			1005

//数值越界
#define VALUE_OUT_OF_RANGE_ERR 	 			1006

//要设置的flag和原有一致 
#define FLAY_ALREADY_SET_ERR 	 			1007

//数据不完整
#define VALUE_ISNOT_FULL_ERR 	 			1008
//枚举越界
#define ENUM_OUT_OF_RANGE_ERR 	 			1009

//返回报文长度有问题
#define PROTO_RETURN_LEN_ERR 	 			1010

//数据库中数据有误
#define DB_DATA_ERR   						1011

#define NET_SEND_ERR   						1012
#define NET_RECV_ERR   						1013

//在insert，KEY已存在
#define KEY_EXISTED_ERR  					1014
//在select update, delect 时，KEY不存在
#define KEY_NOFIND_ERR  					1015

#define DATE_INVALID_ERR 	 				1016

#define VALUE_NOENOUGH_E                    1020
#define VALUE_MAX_E                         1021




//登入时，检查用户名和密码出错
#define CHECK_PASSWD_ERR   					1103

//在insert，米米号已存在
#define USER_ID_EXISTED_ERR  				1104

//在select update, delete 时，米米号不存在
#define USER_ID_NOFIND_ERR  				1105

//用户的EMAIL 为空
#define USER_EMAIL_NO_EXISTED_ERR			1106

//用户没有激活
#define USER_NOT_ENABLED_ERR 				1107
#define USER_IS_ENABLED_ERR 				1108


//列表中的ID  好友列表，黑名单
#define LIST_ID_EXISTED_ERR 				1109
//列表中的ID  好友列表，黑名单
#define LIST_ID_NOFIND_ERR 					1110

//装扮ID不存在
#define USER_ATTIRE_ID_NOFIND_ERR  			1111

//编辑小屋时,上送的和DB中原有的数据不一致
#define USER_SET_ATTIRE_DATA_ERR  			1112
//交换:物品的个数不够
#define USER_SWAP_ATTIRE_NOENOUGH_ERR  		1113
//交换:物品的个数超过最大值
#define USER_SWAP_ATTIRE_MAX_ERR  			1114

//列表中的ID  好友列表，黑名单
#define LIST_ID_MAX_ERR 					1115
//小米超过限制
#define XIAOMEE_MAX_A_DAY_ERR 	 			1116

//小米不够
#define XIAOMEE_NOT_ENOUGH_ERR 	 			1117
//大使的下线太多了
#define CHILD_COUNT_MAX_ERR 	 			1118

//装扮个数不足
#define ATTIRE_COUNT_NO_ENOUGH_ERR 			1119

//封号
#define USER_NOUSED_ERR  					1120
//24小时离线
#define USER_OFFLIN24_ERR  					1121
#define USER_BIRTHDAY_IS_SET_ERR  			1122
//问题检查失败
#define USER_CHECK_QUESTION_ERR  			1123
//问题没有设置
#define USER_QUESTION_NOSET_ERR  			1124
//问题已经设置
#define USER_QUESTION_SETED_ERR  			1125


#define ADD_FRIEND_EXISTED_BLACKLIST_ERR 	1126


//用户已经设置过生日了
#define USER_BIRTHDAY_IS_SETED_ERR  		1127

//用户的密码邮箱已经确认过了
#define USER_PASSWDEMAIL_CONFIRMED_ERR  	1128

//用户的密码邮箱未设置
#define USER_PASSWDEMAIL_NOTSET_ERR  		1129

//用户的宠物个数超过最大值
#define USER_PET_MAX_ERR  					1130

//宠物装扮已经存在
#define USER_PET_ATTIRE_ID_EXISTED_ERR 		1132

//宠物装扮没有找到
#define USER_PET_ATTIRE_ID_NOFIND_ERR 		1133

//宠物已经存在
#define USER_PET_ID_EXISTED_ERR 			1134

//宠物没有找到
#define USER_PET_ID_NOFIND_ERR 				1135



//用户任务:ID已存在
#define USER_TASK_EXISTED_ERR  				1140

//用户任务:ID不存在
#define USER_TASK_NOFIND_ERR  				1141

//超过卡片最大值
#define USER_CARDLIST_COUNT_MAX_ERR 		1143

//卡片ID已经存在
#define USER_CARDID_EXISTED_ERR				1144

//增加初级卡片超过等级限制
#define USER_CARDID_ADD_LOWER_MAX_GRADE_ERR 1145


//卡片:已经初始化了
#define USER_CARD_IS_INITED_ERR				1146

//卡片:超过当天经验限制
#define USER_CARD_MAX_EXP_DAY_ERR 			1147


//已到达每天上限,但还是加上了摩尔豆
#define XIAOMEE_A_DAY_ENOUGH_ERR 			1148

//群组
//群组ID 不存在
#define GROUPID_NOFIND_ERR  				1150

//群组ID 已存在
#define GROUPID_EXISTED_ERR  				1151
//不是群主，没有权限修改,
#define GROUP_ISNOT_OWNER_ERR  				1152

//超过最大群数限制
#define GROUP_COUNT_MAX_ERR 				1153
//超过创建最大群数限制
#define GROUP_CREATE_COUNT_MAX_ERR 			1154

//超过成员最大限制
#define GROUP_MEMBER_COUNT_MAX_ERR 			1155

//成员不存在
#define GROUP_MEMBERID_NOFIND_ERR 			1156
//成员已经存在
#define GROUP_MEMBERID_EXISTED_ERR 			1157

//检查用户名和支付密码出错
#define CHECK_PAYPASSWD_ERR   				1160

//银行错误码
//记录数大于记录限制
#define RECORD_BIG_MAX_ERR 					1170
//存款豆豆必须大于1000 
#define  XIAOMEE_LESS_THRESHOLD_ERR			1171
//不允许中途取出
#define REFUSE_GET_ERR 						1172
//记录不存在
#define RECORD_NOT_EXIST_ERR 				1173
//记录已经存在（同一秒中不允许入两次）
#define RECORD_EXIST_ERR 					1174
//无效的存款期限值
#define INVALID_TIME_LIMIT_ERR 				1175

//任务ID不存在
#define TASKID_NOT_EXIST_ERR				1176

//增加的好友是自己
#define ADD_FRIEND_IS_SELF_ERR 				1180
//游戏分数不是最高分 
#define USER_GAME_NOT_MAX_ERR				1182	
//增加到黑名单中的是自己
#define ADD_BLACKUSER_IS_SELF_ERR 			1181

//通过卖出得到的摩尔豆已到达最大上限
#define SALE_XIAOMEE_MAX_A_DAY_ERR 	 		1190
//用户的装扮个数超过限定的最大值
#define USER_ATTIRE_MAX_ERR  				1201

//没有找到果实
#define USER_NOFIND_FRUIT_ERR  				1202
//果实个数已是最大值
#define USER_FRUIT_MAX_ERR  				1203
//计算VALUE值时间错误
#define VALUE_TIME_ERR						1205
//种子ID找不到
#define SEEDID_NOFIND_ERR  					1206
//超过最大种子允许数目
#define OUT_MAX_SEED_NUM_ERR				1207
//果实已经过期
#define FRUIT_IS_BAD_ERR					1204
//种子在同一位置
#define SEED_SAME_PLACE_ERR					1209

// EMAIL 已存在  
#define EMAIL_EXISTED_ERR 					1301
#define EMAIL_NOFIND_ERR 					1302

//怪怪树每天操作次数超过最大数
#define  MMS_OPT_DAY_COUNT_MAX_ERR 			1311


//怪怪树:操作类型没有定义(必须是0-4) 
#define  MMS_OPT_TPYE_NO_DEFINE_ERR 		1312

//向导：当天投票超过最大数
#define  GUIDE_OPT_DAY_COUNT_MAX_ERR 		1313
//TEMP
//圣诞己拿到祝福了
#define  CHRISTMAS_IS_GETED_ERR				1320
//圣诞己加入祝福
#define  CHRISTMAS_IS_ADD_ERR				1321

//圣诞未加入祝福
#define  CHRISTMAS_ISNOT_ADD_ERR			1322

//春节:金牌不够
#define  SPRING_GOLD_NOENOUGH_ERR			1323
//春节:银牌不够
#define  SPRING_SILVER_NOENOUGH_ERR			1324

//春节:金元宝数超过当天限制
#define  SPRING_GOLD_MAX_A_DAY_ERR			1325

//春节:银元宝数超过当天限制
#define  SPRING_SILVER_MAX_A_DAY_ERR		1326

//春节:已经初始化金银元宝
#define SPRING_GOLD_SILVER_INITED_ERR 		1327 

//春节:还没有初始化金银元宝
#define SPRING_GOLD_SILVER_NOT_INIT_ERR    	1328 

//班级问答:已经回答过了
#define CLASS_QA_IS_ADD_ERR  			  	1330 




//该用户游戏积分没有找到
#define GAME_SCORE_NOFIND_ERR  				1401
#define GAME_SCORE_NONEED_SET_ERR 			1402


#define MSGID_NOFIND_ERR 					1501

//temp
#define CUP_VALUE_MAX_ERR 					1601

//#define CONFECT_TODAY_VALUE_MAX_ERR 		1602
//#define CONFECT_ALL_VALUE_MAX_ERR 			1603

//邮件超过最大值
#define USER_EMAIL_MAX_ERR 					1701
#define USER_EMAIL_NOFIND_ERR 				1702
//许愿池
//愿望已经存在
#define HOPE_IS_EXISTED_ERR 				1801
//愿望没有找到
#define HOPE_NOFIND_ERR 					1802

//当天该服务器的party超过每天的最大数
#define SERVER_PER_DAY_PARTY_MAX_ERR 		1810



//define for payser  
#define PAY_ATTITE_ID_NOFIND_ERR 			2001
#define PAY_ATTITE_PAYTPYE_NODEFINE_ERR 	2002

// 序列号不存在 
#define SERIAL_ID_NOFIND_ERR 				2101

//序列号已经使用过了
#define SERIAL_ID_IS_USED_ERR 				2102

//序列号已经到期
#define SERIAL_ID_IS_DUE_ERR 				2103

// transid 已存在
#define TRANS_ID_EXISTED_ERR				2104
// transid 不存在
#define TRANS_ID_NOFIND_ERR 				2105


//已经处于包月状态
#define IS_MONTHED_ERR 						2106
//不处于包月状态
#define IS_NOT_MONTHED_ERR 					2107

//对账标志核对出错
#define IS_NOT_VALIDATED_ERR 				2108
#define IS_VALIDATED_ERR 					2109

//对账金额出错
#define VALIDATE_ERR 						2110

#define ROOMMSG_MSG_NUM_OUT_ERR        		6020

//超过每个用户最大上传限制
#define PIC_MAX_USER_PIC_ERR 				6101

//没有找到该图片
#define PIC_NOFIND_PIC_ERR 					6102

//2200-2299: 用于会员部分:tony
//
//账户管理:权限ID已经有了
#define ADMIN_POWERID_EXISTED_ERR      		2501
//账户管理:权限ID不存在
#define ADMIN_POWERID_NOFIND_ERR      		2502

//班级号已经存在
#define CLASSID_EXISTED_ERR					2503
//班级不存在
#define CLASSID_NOFIND_ERR					2504

//班级数太多了
#define CLASSID_MAX_COUNT_ERR				2505

//班级:成员太多了
#define CLASS_MEMBER_MAX_COUNT_ERR			2506

//班级:成员已经存在
#define CLASS_MEMBER_EXISTED_ERR			2507
//班级:成员不存在
#define CLASS_MEMBER_NOFIND_ERR				2508

//班级信息太多了:
#define CLASSMSG_MAX_ERR                  	2520
//果实还没有成熟
#define FRUIT_NOT_MATURE_ERR				2521
//还没到偷取时间
#define TIME_IS_NOT_ENOUGH_ERR				2522
//已经偷取过果实
#define HAVE_THIEVE_FRUIT_ERR				2523
//今天已经抽取过VIP或者消费卷
#define HAVE_DONE_THIS_TODAY_ERR			2524
//VIP和消费卷抽中次数都使用完
#define OUT_OF_MAX_COUNT_ERR				2525
//动物还没成年
#define ANIMAL_NOT_ADULT_ERR				2526
//数量不够
#define NUM_IS_NOT_ENOUGH_ERR				2527
//动物的ID号不存在
#define ANIMAL_IS_NOT_EXIST_ERR				2528				
//此次抽取失败
#define YOU_LOST_THIS_TIME_ERR				2529
//农场已经上锁
#define FARM_HAS_BEEN_LOCKED_ERR			2530		
//今天已经偷取过
#define FARM_HAVE_THIEVER_TODAY_ERR			2531
//卡牌数目不够
#define CARD_IS_NOT_ENOUGH_ERR				2532
//卡牌没有处于交换状态
#define CARD_IS_NOT_CHANGE_ERR				2533
//这个不是需要交换的卡牌
#define CARD_IS_NOT_NEED_ERR				2534
//陆生动物超过上限
#define LAND_ANIMAL_THAN_MAX_ERR			2535
//水生动物超过上限
#define WATER_ANIMAL_THAN_MAX_ERR			2536
//成年动物超过上限
#define ADULT_ANIMAL_THAN_MAX_ERR			2537
//饲料房饲料超过上限
#define FEEDSTUFF_THAN_MAX_ERR				2538
//此为已经设置过
#define HAVE_SET_THIS_BIT_ERR				2539
//已经领取过奖
#define CLASS_HAVE_THIS_MEDAL_ERR			2540
#define USER_OFFLINE_SEVEN_DAY_ERR			2541
#define USER_OFFLINE_FOURTEEN_DAY_ERR		2542
//已经报过名
#define YOU_HAVE_SIG_FIRE_CUP_ERR	    	2543
//应经接过任务
#define YOU_HAVE_GOT_THIS_TASK_ERR			2544
//还没接任务
#define YOU_HAVE_NOT_GOT_TASK_ERR			2545
//已经完成任务
#define YOU_HAVE_DONE_TASK_ERR				2546
//没有报名
#define YOU_HAVE_NOT_SIG_ERR				2547
//被PK者分数小于一定的数值
#define PK_GAME_SCORE_TOO_SMALL_ERR         2548
//没有权限读日志
#define  YOU_HAVE_NOT_RIGHT_TO_READ_ERR		2549
//日记不存在
#define DIARY_NOT_EXIST_ERR            2550
//已经加锁
#define DIARY_IS_LOCKED_ERR				2551
//笔记本没加锁
#define DIARY_IS_OPEN_ERR				2552
//没到接第二任务的时间
#define PET_TIME_IS_NOT_ENOUGH_ERR	       2553
//还没有完成第二阶段任务
#define YOU_HAVE_NOT_FINISH_SECOND_STAGE_ERR 2554
//已经参加了魔法课
#define YOU_HAVE_ATTEND_MAGIC_ERR		2555
//没有权限参加
#define YOU_HAVE_NOT_RIGHT_ERR			2556
//还没完成前一阶段任务
#define YOU_HAVE_NOT_FINISH_PRE_ERR		2557
//超过衣柜的物品数目限制
#define CHEST_OUT_OF_RANGER_ERR         2558
//衣柜里没有物品
#define CHEST_ATTIRE_NOT_ENOUGH_ERR    2559
//礼品的数目不够
#define PRESENT_IS_NOT_ENOUGH_ERR      2560
//道具孔已经存在
#define USER_AUTO_PROPERY_EXIST_ERR    2561
//道具孔不存在
#define USER_AUTO_PROPERY_NOT_EXIST_ERR    2562
//已经有道具存在
#define USER_AUTO_ATTIRE_EXIST_ERR	2563
//道具不存在
#define USER_AUTO_ATTIRE_NOT_EXIST_ERR  2564
//汽车不存在
#define AUTO_IS_NOT_EXIST_ERR			2565
//装饰孔已经存在装饰
#define ADORN_HAVE_EXIST_ERR			2566
//参赛次数超过上限
#define YOU_HAVE_ATTEND_TOO_MANY_ERR	2567
//没有参加完整的赛道
#define YOU_HAVE_NOT_FISH_ALL_RACE_ERR 2568
//已经拥有此类车
#define YOU_HAVE_THIS_KIND_AUTO_ERR   2569
//你没有这个ID号的车
#define YOU_HAVE_NOT_THIS_ID_ERR      2570
//安心服务已经过期
#define ANXIN_SERVIECE_OUT_OF_RANGE_ERR 2571
//已经有动物带出
#define YOU_HAVE_ANIMAL_OUTGO_ERR 2572
//此植物还没被偷取过
#define THIS_TREE_NOT_THIEVER_ERR 2573
//偷取果实数太多
#define THIEVER_THIS_MANY_TIMES_ERR 2574
//已经偷取过怪怪糖
#define HAVE_THIEVE_GUAI_TANG_ERR 2575
//渔网已经损坏
#define YOU_NET_HAVE_BROKEN_ERR 2576
//你还没有渔网
#define YOU_HAVE_NOT_NET_ERR	2577
//没有足够的糖果兑换道具
#define YOU_HAVE_NOT_ENOUGH_CANDY_ERR 2578
//这个拉姆糖已经被领超过一百次
#define FETCH_CANDY_OUT_OF_MAX_ERR 2579
//你已经从这个拉姆领取超过五次的糖果
#define YOU_HAVE_GOT_MANY_TIME_ERR 2580
//设置的时间比现在时间迟
#define YOU_TIME_OLD_THEM_CURRENT_ERR 2581
//今天太多人孵了这个蛋
#define MANY_PEOPLE_BROOD_EGG_ERR 2582
//你今天已经孵过蛋
#define YOU_HAVE_BROOD_EGG_ERR 2583
//你已经有画卷了
#define YOU_HAVE_PIC_ERR 2584
//还没到领取时间
#define YOU_HAVE_NOT_ENOUGH_TIME_ERR 2585
//已经剪过羊毛
#define YOU_HAVE_GET_WOOL_ERR  2586
//接任务超过上限
#define YOU_HAVE_GOT_TASK_TOO_MANY_ERR 2587
//还没有完成任务
#define YOU_HAVE_NOT_FINISH_TASK_ERR 2588
//还没有接任务
#define YOU_HAVE_NOT_RECV_TASK_ERR 2589
//NPC的好感度不够
#define YOU_NPC_FAVOR_NOT_ENOUGH_ERR 2590
//达到孵蛋的次数
#define EGG_HAVE_ENOUGH_BROEED_NUM_ERR 2591
//还没有达到孵蛋的次数
#define EGG_HAVE_NOT_BROOD_NUM_ERR 2592
//领取兔子时，时间还没有到
#define USER_GREENHOUSE_DROP_NOT_TOUCH_TIME_ERR 2593
//查询的服装组合不存在
#define ROOMINFO_ROOMINFO_DRESSING_MARK_NO_UNIT_ERR 2594

//想使用蛋糕交换物品，但是此时却没有足够的蛋糕
#define ROOMINFO_CAKE_GIFT_SWAP_NOT_ENOUGH_ERR 2595
//不需要授粉
#define THIS_PLANT_NOT_NEED_POLLINATE_ERR 2596
//已达到授粉次数
#define THIS_PLANT_HAVE_ENOUGH_POLLINATE_ERR 2597
//没有动物可以再领取
#define HAVE_NOT_ENGOUGH_ANIMAL_ERR 2598
//已经领取过袜子
#define HAVE_BEEN_GOT_SOCKES_ERR 2599
//昆虫房达到上限
#define INSECT_THAN_MAX_ERR 2600
//此种动物不能吃草
#define THIS_ANIMAL_CANNOT_EAT_GRASS_ERR 2601
//蛋超过数量的限制
#define EGG_MORE_THAN_LIMIT_ERR 2602
//此种动物超过上限
#define THIS_ANIMAL_OUT_OF_LIMIT_ERR 2603
//授粉次数达到上限
#define THIS_PLANT_POLLINATE_ENOUGH_ERR 2604
//此阶段不能授粉
#define THIS_STAGE_CNANOT_POLLINATE_ERR 2605
//昆虫授粉已经达到上限
#define THIS_ANIMAL_POLLINATE_TOO_MANY_TIMES_ERR 2606
//没有足够的礼物
#define GIFT_IS_NOT_ENOUGH_ERR 2607
//你钓的不是水生动物
#define THIS_ANIMAL_NOT_WATER_ANIMAL_ERR 2608
//没有人送礼物
#define NO_BODY_GIVE_GIFT_ERR 2609
//能量星超过上限
#define ENERG_STAR_OUT_OF_RANGE_ERR 2610

/// @brief 错误码： 植物施肥错误，不能施肥，植物死或其他
#define PLANT_MUCK_ERR 2611

/// @brief 错误码： 没有化肥
#define PLANT_MUCK_NO_MUCK_ERR 2612

/// @brief 错误码： 当天施肥次数太多
#define PLANT_MUCK_TIMES_OVER_ERR 2613

/// @brief 错误码：不支持的操作类型
#define BUY_THING_UNKOWN_PLACE_ERR 2614

/// @brief 错误码：你已经领取了赠送的物品
#define PET_TASK_ATTIRE_HAVE_GIVEN_ERR 2615

/// @brief 错误码：你的宠物任务没有完成
#define PET_TASK_NOT_COMPLETE_ERR 2616

/// @brief 错误码：搬石头达到上限
#define USER_STONE_MOVE_OVER_LIMIT_ERR 2617

/// @brief 错误码：勇士抽奖，当天超过10次上限
#define WARRIOR_LUCK_DRAW_OVER_LIMIT_ERR 2618

/// @brief 错误码：勇士抽奖，当天已经抽中2次，达到上限
#define WARRIOR_IS_LUCK_DRAW_OVER_LIMIT_ERR 2619

/// @brief 错误码：批量购买物品不满足条件
#define BUY_MANY_THING_IN_ONE_TIME_NOT_COMPLETENESS_ERR 2620

//编号ID不存在
#define ID_NOT_EXIST_ERR	 2621

/// @brief 错误码：你养殖了不能养殖的动物
#define ANIMAL_IS_NO_NEED_ERR 2622

/// @brief 错误码：设置宠物技能点时，指定了不认识的系别
#define USER_PET_SKILL_SET_UNKOWN_TYPE_ERR 2623

/// @brief 错误码：任务状态转移不合法
#define USER_PET_TASK_STATU_TRANSFER_ERR 2624

/// 错误码：用户宠物使用技能时，取得物品达限或技能点使用次数超限
#define USER_USE_SKILL_ONE_OVER_ERR 2625

/// 错误码：用户宠物使用技能时，取得不认识的物品
#define USER_USE_SKILL_UNKOWN_ATTIRE_ERR 2626

/// 错误码：客户端私有数据，抽取时不存在
#define USER_GET_CLIENT_DATA_IS_NOT_EXIST_ERR 2627

//添加错误在此上进行
/*************** border_border *********************************/


#define safe_copy_string(dst,src) { \
	if (src){\
		strncpy (dst, src, sizeof(dst) - 1); \
		dst[sizeof(dst) - 1] = '\0'; \
	}else{\
		dst[0] = '\0'; \
	}\
}

#define memcpy_with_dstsize(dst,src) { \
	memcpy(dst, src, sizeof(dst)); \
}

#define set_mysql_string(dst,src,n)    mysql_real_escape_string(  \
          &(this->db->handle), dst, src, n)
#define set_mysql_string(dst,src,n)    mysql_real_escape_string(  \
          &(this->db->handle), dst, src, n)
#define inet_uint32toa(saddr)	inet_ntoa(*((struct in_addr*)(&(saddr))))

struct idlist{
	uint32_t count;
	uint32_t item[];
}__attribute__((packed)) ;


#include <algorithm>

inline int update_item_to_list_ex( char * p_list, char * p_item, int p_item_len )
{
	//p_list  前4字节:个数
	uint32_t count=*((uint32_t *)p_list);//
	char * p_list_item=p_list+4;//
	uint32_t item_id=*((uint32_t *)p_item);//

	uint32_t i;
	for (i=0;i<count;i++){
		if ( *((uint32_t*)(p_list_item+(p_item_len*i)))==item_id){
			break;	
		}
	}	

	//没有找到
	if (i>=count)
		return FAIL;

	//更新数据
	memcpy(p_list_item+(p_item_len*i),
				p_item ,p_item_len );
	return SUCC;
}

inline int del_item_from_list_ex( char * p_list, char * p_item, int p_item_len )
{
	//p_list  前4字节:个数
	uint32_t count=*((uint32_t *)p_list);//
	char * p_list_item=p_list+4;//
	uint32_t item_id=*((uint32_t *)p_item);//

	uint32_t i;
	for (i=0;i<count;i++){
		if ( *((uint32_t*)(p_list_item+(p_item_len*i)))==item_id){
			break;	
		}
	}	

	//没有找到
	if (i>=count)
		return FAIL;

	
	//copy last
	//i定位到要删除的位置
	for (;i<count;i++ ){
		memcpy(p_list_item+(p_item_len*i),
				p_list_item+(p_item_len*(i+1)),p_item_len );
	}

	(*((uint32_t *)p_list))--;//个数减1
	return SUCC;

}

inline int add_item_to_list_ex( char * p_list, char * p_item, int p_item_len, uint32_t item_max )
{
	//p_list  前4字节:个数
	uint32_t count=*((uint32_t *)p_list);//
	char * p_list_item=p_list+4;//
	uint32_t item_id=*((uint32_t *)p_item);//

	uint32_t i;
	for (i=0;i<count;i++){
		if ( *((uint32_t*)(p_list_item+(p_item_len*i)))==item_id){
			break;	
		}
	}	
	//找到了。
	if (i<count)
		return LIST_ID_EXISTED_ERR;
	
	if (count>=item_max)
		return LIST_ID_MAX_ERR;

	//加入一项	
	memcpy(p_list_item+(p_item_len*count ),p_item,  p_item_len );
	(*((uint32_t *)p_list))++;//个数+1
	return SUCC;
}

#define del_group_item_from_list(p_list,p_item)  del_item_from_list_ex((char*)p_list,(char*)p_item, sizeof(stru_group_item))
#define add_group_item_to_list(p_list,p_item,item_max) add_item_to_list_ex ((char*)p_list,(char*)p_item, sizeof(stru_group_item), item_max )

#define set_group_item_to_list(p_list,p_item) update_item_to_list_ex ((char*)p_list,(char*)p_item, sizeof(stru_group_item) )

inline int del_id_from_list( struct idlist * p_list,uint32_t id )
{
	uint32_t * new_idend, *idstart, *idend;	
	idstart=p_list->item;
	idend=idstart+p_list->count;	
	new_idend=std::remove( idstart ,  idend , id);
	if (new_idend != idend) {
		p_list->count=new_idend-idstart;	
		return SUCC;
	}else{
		return FAIL;
	}
}

inline int add_id_to_list( struct idlist * p_list, uint32_t id, uint32_t max_count )
{
	uint32_t * find_index, *idstart, *idend;	
	idstart=p_list->item;
	idend=idstart+p_list->count;	
	find_index=std::find( idstart ,idend, id);
	if (find_index!=idend) {
		//find 
		return LIST_ID_EXISTED_ERR;
	}else if ( p_list->count >= max_count ){
		//检查是否超过最大个数
		//out of max value
		return LIST_ID_MAX_ERR;
	}else{
		p_list->item[p_list->count]=id;
		p_list->count++;	
		return SUCC;
	}
}

inline bool check_id_in_list( struct idlist * p_list, uint32_t id )
{
	uint32_t * find_index, *idstart, *idend;	
	idstart=p_list->item;
	idend=idstart+p_list->count;	
	find_index=std::find( idstart ,idend, id);
	if (find_index!=idend) {
		return true;
	}else{
		return false;
	}
}



inline uint32_t atoi_safe (char *str) 
{
	return 	(str!= NULL ? atoll(str):0 );
}

inline uint64_t get_serialid (const char *str) 
{
	uint64_t value=0 ;
	const char * p=str;
	while (*p!='\0'){
		if (*p>='0' && *p<='9' ){
			value=value*36+*p-'0';
		}else{
			value=value*36+*p-'A'+10;
		}
		p++;
	}
	return value;
}

inline char * get_serialid_str (uint64_t serailid  ) 
{
	char serail_str_tmp[30];
	static char serail_str[30];
	int len=0;
	uint64_t tmp=serailid;  
	uint64_t value;  
	int i=0;
	while (tmp>0 ){
		value=tmp%36;
		if (value<10){
			serail_str_tmp[i]='0'+ value;
		}else{
			serail_str_tmp[i]='A'+  value - 10 ;
		}
		tmp/=36;	
		i++;
	};
	serail_str_tmp[i]='\0';

	len=i;
	for (i=0;i<len;i++){
		serail_str[i]=serail_str_tmp[len-i-1];
	}
	serail_str[len]='\0';
	return serail_str;
}

inline uint32_t  get_addr_int(char *ip) 
{
	int ip_1;
	int ip_2;
	int ip_3;
	int ip_4;
	sscanf(ip,"%d.%d.%d.%d", &ip_1,&ip_2,&ip_3,&ip_4);
	return (ip_1<<24)+(ip_2<<16)+(ip_3<<8)+ip_4; 
}

inline int atoll_safe (char *str) 
{
	return 	(str!= NULL ? atoll(str):0 );
}

inline int get_year_month(time_t t  ) 
{
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*100+tm_tmp.tm_mon+1;
}

inline int get_week_id(time_t now)
{
	//周五起
	now += 86400 * 3;
	char buffer[10];
	strftime(buffer, 10, "%W", localtime(&now));
	return  atoll(buffer);
}	

inline int get_date(time_t t  ) 
{
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*10000+(tm_tmp.tm_mon+1)*100+tm_tmp.tm_mday;
}

inline char * get_datetime(time_t t  ) 
{
	static char  buf[100];
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	sprintf( buf , "%d-%d-%d %d:%d:%d",
		tm_tmp.tm_year+1900,tm_tmp.tm_mon+1,tm_tmp.tm_mday, tm_tmp.tm_hour,
			tm_tmp.tm_min,tm_tmp.tm_sec);
	return buf ;
}

inline char * get_datetime_ex_1(time_t t  ) 
{
	static char  buf[100];
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	sprintf( buf , "%d%02d%02d%02d%02d%02d",
		tm_tmp.tm_year+1900,tm_tmp.tm_mon+1,tm_tmp.tm_mday, tm_tmp.tm_hour,
			tm_tmp.tm_min,tm_tmp.tm_sec);
	return buf ;
}

inline char * get_datetime_ex(time_t t  ) 
{
	static char  buf[100];
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	sprintf( buf , "%d%02d%02d_%02d%02d%02d",
		tm_tmp.tm_year+1900,tm_tmp.tm_mon+1,tm_tmp.tm_mday, tm_tmp.tm_hour,
			tm_tmp.tm_min,tm_tmp.tm_sec);
	return buf ;
}

inline int get_minutes_count(time_t t  ) 
{
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	return (tm_tmp.tm_hour)*60 + tm_tmp.tm_min;
}

inline uint32_t get_time_t(uint32_t  value ) 
{
	struct tm tm_tmp;
	time_t t=time(NULL);
	localtime_r(&t, &tm_tmp) ;
	tm_tmp.tm_year=value/10000-1900;
	tm_tmp.tm_mon= value%10000/100-1;
	tm_tmp.tm_mday=value%100;
	tm_tmp.tm_hour=0;
	tm_tmp.tm_min=0;
	tm_tmp. tm_sec=0;
	return mktime(&tm_tmp);
}

inline int32_t get_valid_value (int32_t value,int32_t min,int32_t max) 
{
	if (((value< min)))   return min ;
	else  if (( value > max))  return max ;
	else return value;
}

#define mysql_str_len(n)    ( (n) *2 +1)

#define SET_SNDBUF(private_len)  \
	if (!(set_std_return (sendbuf,sndlen,  (PROTO_HEADER*)recvbuf, SUCC, (private_len)))){ \
		DEBUG_LOG("set_std_return:private size err [%u]",private_len);\
		return SYS_ERR;\
	}

//没有私有返回数据时，可用 在Croute_func 的 相关处理函数中使用
#define STD_RETURN(ret) {\
	if (ret==SUCC){  \
		SET_SNDBUF(0);\
	}\
	return ret;\
}

//用于输入长度为变长的情况下，长度的验正
#define CHECK_PRI_IN_LEN_WITHOUT_HEADER(pri_len) {\
	if ( PRI_IN_LEN != (sizeof(*p_in)+(pri_len)) ){\
		DEBUG_LOG("check size err [%u][%u]", PRI_IN_LEN ,\
			    (sizeof(*p_in)+(pri_len))	);\
		return PROTO_LEN_ERR; \
	}	\
}

//用于输入长度为变长的情况下，长度的验正
#define CHECK_PRI_IN_LEN_WITHOUT_HEADER_EX(pri_len, max_len){\
	if ( PRI_IN_LEN>(sizeof(*p_in)+max_len) || PRI_IN_LEN != (sizeof(*p_in)+(pri_len))   ){\
		DEBUG_LOG("check size err  in[%u] need [%u] max [%d] ", PRI_IN_LEN ,\
			    (sizeof(*p_in)+(pri_len)) , max_len	);\
		return PROTO_LEN_ERR; \
	}	\
}



#define STD_RETURN_WITH_BUF(ret,_buf,_count ) { \
	if (ret==SUCC){ \
		SET_SNDBUF( (_count) ); \
		memcpy(SNDBUF_PRIVATE_POS, (char*)(_buf) , (_count)  ); \
	}\
	return ret; \
}

//在有私有数据时采用， stru 为 返回的私有结构体。
#define STD_RETURN_WITH_STRUCT(ret,stru ) STD_RETURN_WITH_BUF (ret,((char*)&stru), sizeof(stru)  )



//在有返回列表 时采用， 
//ret:返回值  
//stru_header:结构体头部
//pstru_item:列表起始指针
//itemcount: 个数
//注意事项：pstru_item所指向的空间应是molloc得到的,:free(pstru_item);
#define STD_RETURN_WITH_STRUCT_LIST(ret,stru_header,pstru_item ,itemcount) \
	if (ret==DB_SUCC){ \
		uint32_t stru_header_size=sizeof(stru_header );\
		uint32_t out_list_size= sizeof(*pstru_item ) * (itemcount);\
		SET_SNDBUF(stru_header_size + out_list_size);\
		memcpy(SNDBUF_PRIVATE_POS,&stru_header,stru_header_size);\
		memcpy(SNDBUF_PRIVATE_POS+stru_header_size,pstru_item, out_list_size );\
		free(pstru_item);\
		return SUCC;\
	} else {\
		return ret;\
	} 

// 在各个表的操作中使用  用于SET
#define STD_SET_RETURN( sqlstr, id, id_no_find_err )  {\
	int acount; \
	int dbret; \
	if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){ \
		if (acount ==1){\
			return SUCC;\
		}else{\
			return id_no_find_err; \
		}\
	}else {\
		return DB_ERR;\
	} \
}

// 在各个表的操作中使用  用于SET
#define STD_SET_WITHOUT_RETURN( sqlstr, id, id_no_find_err )  {\
	int acount; \
	int dbret; \
	if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){ \
		if (acount ==1){\
		}else{\
			return id_no_find_err; \
		}\
	}else {\
		return DB_ERR;\
	} \
}

//不检查记录是否存在，我们假设只影响一条记录，否则认为出错
#define STD_SET_RETURN_EX( sqlstr, id_no_find_err )  {\
	int dbret; \
	int acount; \
	if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){ \
		if (acount ==1){\
			return SUCC;\
		}else{\
			return id_no_find_err;\
		}\
	}else {\
		return DB_ERR;\
	} \
}

//不检查受影响的行数
#define STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr)  {\
	int dbret; \
	int acount; \
	if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){ \
			return SUCC;\
	}else {\
		return DB_ERR;\
	} \
}


//设置多行数据
#define STD_SET_LIST_RETURN_WITH_CHECK(sqlstr, count, error)  {\
	int dbret; \
	int acount; \
	if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){ \
		if (acount != count) {\
			return error;\
		} else {\
			return DB_SUCC;\
		}\
	}else {\
		return DB_ERR;\
	} \
}





//设置多行数据
#define STD_SET_LIST_RETURN( sqlstr )  {\
	int dbret; \
	int acount; \
	if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){ \
			return DB_SUCC;\
	}else {\
		return DB_ERR;\
	} \
}

//不检查记录是否存在
#define STD_SET_WITHOUT_RETURN_EX( sqlstr, id_no_find_err )  {\
	int dbret; \
	int acount; \
	if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){ \
		if (acount ==1){\
		}else{\
			return id_no_find_err;\
		}\
	}else {\
		return DB_ERR;\
	} \
}

// 在各个表的操作中使用  用于INSERT
#define STD_INSERT_RETURN( sqlstr,existed_err )  \
	{ int dbret;\
		int acount; \
		if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){\
			return DB_SUCC;\
		}else {\
			if (dbret==ER_DUP_ENTRY)\
				return  existed_err;\
			else return DB_ERR;\
		}\
	}

#define STD_INSERT_WITHOUT_RETURN( sqlstr,existed_err )  \
	{ int dbret;\
		int acount; \
		if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){\
		}else{\
			if (dbret==ER_DUP_ENTRY)\
				return  existed_err;\
			else return DB_ERR;\
		}\
	}



#define STD_INSERT_GET_ID( sqlstr,existed_err, id )  \
	{ int dbret;\
		int acount; \
		if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){\
			id=mysql_insert_id(&(this->db->handle));\
			return DB_SUCC;\
		}else {\
			if (dbret==ER_DUP_ENTRY)\
				return  existed_err;\
			else return DB_ERR;\
		}\
	}

//modified by tony, add rollbak
#define STD_OPEN_AUTOCOMMIT()       mysql_autocommit(&(this->db->handle), T)
#define STD_ROLLBACK()  mysql_rollback(&(this->db->handle))
#define STD_CLOSE_AUTOCOMMIT(nret) {\
        if (mysql_autocommit(&(this->db->handle), F)==DB_SUCC){\
                nret = DB_SUCC;\
        }else {\
                nret = DB_ERR;\
        }\
    }
#define STD_COMMIT(nret) {\
        if (mysql_commit(&(this->db->handle))==DB_SUCC){\
                nret = DB_SUCC;\
        }else {\
                nret = DB_ERR;\
        }\
    }
//end modify



#define  STD_REMOVE_RETURN STD_SET_RETURN 
#define  STD_REMOVE_RETURN_EX STD_SET_RETURN_EX

//依次得到row[i]
// 在STD_QUERY_WHILE_BEGIN  和 STD_QUERY_ONE_BEGIN
//#define FIRST_FIELD  (row[_fi=0])
#define NEXT_FIELD 	 (row[++_fi])

//DEBUG_LOG("len %d:%d",res->lengths[_fi] );

//	DEBUG_LOG("len %d:%d",res->lengths[_fi],max_len  );
//变长方式copy
#define BIN_CPY_NEXT_FIELD( dst,max_len)  ++_fi; \
		mysql_fetch_lengths(res); \
		res->lengths[_fi]<max_len? \
		memcpy(dst,row[_fi],res->lengths[_fi] ): memcpy(dst,row[_fi],max_len)

//定长方式copy
#define MEM_CPY_NEXT_FIELD(p_buf,len) memcpy((p_buf),NEXT_FIELD ,(len) )

//得到int
#define INT_CPY_NEXT_FIELD(value )  (value)=atoi_safe(NEXT_FIELD )


// malloc for list ,set record count to count
#define STD_QUERY_WHILE_BEGIN( sqlstr,pp_list,p_count )  \
	{ 	MYSQL_RES *res;\
		MYSQL_ROW  row;\
		int i;\
		if (( this->db->exec_query_sql(sqlstr,&res))==DB_SUCC){\
			*p_count=mysql_num_rows(res);\
			if ((*pp_list =( typeof(*pp_list))malloc(\
				sizeof(typeof(**pp_list) ) *(*p_count))) ==NULL){\
				return SYS_ERR;\
			}\
			memset(*pp_list,0, sizeof(typeof(**pp_list) ) *(*p_count) );\
			i=0;\
			while((row = mysql_fetch_row(res))){\
				int _fi;\
			   	_fi=-1;
	

#define STD_QUERY_WHILE_END()  \
				i++;\
			}\
			mysql_free_result(res);	\
			return DB_SUCC;\
		}else {\
			return DB_ERR;\
		}\
	}

#define STD_QUERY_WHILE_END_WITHOUT_RETURN()  \
				i++;\
			}\
			mysql_free_result(res);	\
		}else {\
			return DB_ERR;\
		}\
	}


#define STD_QUERY_ONE_BEGIN( sqlstr, no_finderr ) {\
		int ret;\
		MYSQL_RES *res;\
		MYSQL_ROW row;\
		int rowcount;\
		ret =this->db->exec_query_sql(sqlstr,&res);\
		if (ret==DB_SUCC){\
			rowcount=mysql_num_rows(res);\
			if (rowcount!=1) { \
	 			mysql_free_result(res);		 \
				DEBUG_LOG("no select a record [%u]",no_finderr );\
				return no_finderr;	 \
			}else { \
				row= mysql_fetch_row(res); \
				int _fi	 ; _fi=-1;
	


#define STD_QUERY_ONE_END()\
				mysql_free_result(res);\
				return DB_SUCC;\
			}\
		}else { \
			return DB_ERR;	 \
		}\
	}

#define STD_QUERY_ONE_END_WITHOUT_RETURN()  \
				mysql_free_result(res);		 \
			} \
		}else { \
			return DB_ERR;	 \
		}\
	}

inline void ASC2HEX_2(char * dst, char * src,int len)
{
	int hex;
	int i;
	int di;
	for(i=0;i<len;i++){
		hex=((unsigned char)src[i])>>4;
		di=i*2;
		dst[di]=hex<10?'0'+hex:'A'-10 +hex ;
		hex=((unsigned char)src[i])&0x0F;
		dst[di+1]=hex<10?'0'+hex:'A'-10 +hex ;
	}
	dst[len*2]=0;
}

inline bool check_serial_str(char * serial_str )
{
	char *p=serial_str;
	while( *p !='\0' ){
		if (*p=='0'||*p=='O'|| *p=='I' || *p=='1' 
				||*p=='2'||*p=='Z' ||*p=='8'||*p=='B' ) {
			return false;
		}
		p++;
	}
	return true;
}

inline void ASC2HEX_2_lower(char * dst, char * src,int len)
{
	int hex;
	int i;
	int di;
	for(i=0;i<len;i++){
		hex=((unsigned char)src[i])>>4;
		di=i*2;
		dst[di]=hex<10?'0'+hex:'a'-10 +hex ;
		hex=((unsigned char)src[i])&0x0F;
		dst[di+1]=hex<10?'0'+hex:'a'-10 +hex ;
	}
	dst[len*2]=0;
}

inline void ASC2HEX_3(char * dst, char * src,int len)
{
	int hex;
	int i;
	int di;
	for(i=0;i<len;i++){
		hex=((unsigned char)src[i])>>4;
		di=i*3;
		dst[di]=hex<10?'0'+hex:'A'-10 +hex ;
		hex=((unsigned char)src[i])&0x0F;
		dst[di+1]=hex<10?'0'+hex:'A'-10 +hex ;
		dst[di+2]=' ';
	}
	dst[len*3]=0;
}
/*
 * p_logdate 记录的时间  
 * p_day_value 当天多少 
 * p_all_value  所有多少
 * add_date  操作时间 
 * add_value   +多少
 * max_a_day_value   每天多少  
 */
inline int day_add_do_count(uint32_t *p_logdate, uint32_t * p_day_value, 
		uint32_t * p_all_value,
		uint32_t add_date, uint32_t add_value, uint32_t max_a_day_value)
{
	uint32_t tmp_logdate;	
	uint32_t tmp_value;	
	if (add_date!=*p_logdate){ //不是同一天
		tmp_value=add_value;	
		tmp_logdate=add_date;
	}else{//同一天
		tmp_logdate=add_date;
		tmp_value=*p_day_value+add_value;
	}

	if (tmp_value>max_a_day_value ){//超过当日最大值
		return FAIL;
	}else{
		*p_logdate=tmp_logdate;
		*p_day_value=tmp_value;
		*p_all_value+=add_value;
		return SUCC;
	}
}



inline int get_hex(char ch)
{
	if (ch>='0' && ch<='9') return  ch-'0';
	else if (ch>='A' && ch<='F') return  10+ch-'A';
	else if (ch>='a' && ch<='f') return  10+ch-'a';
	else return 0; 
}

inline void HEX2ASC(char * dst, const char * src,int len)
{
	for(int i=0;i<len;i++){
		dst[i]=(unsigned char)((get_hex(src[i*2])<<4)+get_hex(src[i*2+1]));
	}
}
inline uint32_t hash_str(const char * key )
{
	register unsigned int h;
	register unsigned char *p; 
	for(h=0, p = (unsigned char *)key; *p ; p++)
		h = 31 * h + *p; 
	return h;
}
inline int change_date ( int date, int addmonths)
{
	int allmonth=  (date/100)*12 + date%100 + addmonths; 
	return (allmonth /12 ) *100 + allmonth%12; 
}

inline char * set_space_end(char * src, int len  )
{
	int i=0;
	while (i<len &&  src[i] !='\0' ) i++; 

	for (;i<len;i++) src[i]=' ';
	return src ;

}

inline  int  mysql_date( char*date_str, time_t t , int len  )
{
	  return strftime(date_str ,len, "%Y/%m/%d %H:%M:%S", localtime(&t));
}

//定义更新数据类型
enum enum_update_user_type{USER_FLAG=1,USER_NICK=2,USER_PASSWD=3,USER_OTHER=4};
//定义数据库类型 
enum enum_db_type{DB_NULL=-1,DB_USER=0};

#define SET_ROUTE(hex) (hex>>2) 
//define route db for com 
enum enum_route_type {ROUTE_NULL=-1, 
	ROUTE_USERINFO=SET_ROUTE(0x00),//用户基础信息库
	ROUTE_USER=SET_ROUTE(0x10),//用户数据-摩尔庄园
	ROUTE_VIP=SET_ROUTE(0x20),//会员系统
	ROUTE_USER2=SET_ROUTE(0x30),//用户数据-摩尔庄园2
	ROUTE_EMAIL=SET_ROUTE(0x40),//email对应的米米号
	ROUTE_GAME_SCORE=SET_ROUTE(0x50),//游戏排名
	ROUTE_SERIAL=SET_ROUTE(0x60),//神奇密码
	ROUTE_MSGBOARD=SET_ROUTE(0x70),//记者投稿
	ROUTE_GAME=SET_ROUTE(0x80),//用户游戏积分
	ROUTE_USERMSG =SET_ROUTE(0x90),//用户投稿
	ROUTE_SU=SET_ROUTE(0xA0),//客服系统
	ROUTE_DV=SET_ROUTE(0xB0),//同步论坛
	ROUTE_SYSARG=SET_ROUTE(0xC0),//系统参数
	ROUTE_TEMP=SET_ROUTE(0xD0),//活动使用的,时间过后，就删掉
	ROUTE_EMAIL_SYS=SET_ROUTE(0xE0),//*邮件系统
	ROUTE_SALE=SET_ROUTE(0xA4),//支付系统
	ROUTE_HOPE=SET_ROUTE(0x14),//许愿池
	ROUTE_PRODUCE=SET_ROUTE(0x24),//产品管理
	ROUTE_GROUPMAIN=SET_ROUTE(0x34),//产生群号
	ROUTE_MMS=SET_ROUTE(0x44),//毛毛树,金蘑菇向导,...
	ROUTE_ADMIN=SET_ROUTE(0x54),//整体权限管理
	ROUTE_PARTY=SET_ROUTE(0x64),//PARTY
	ROUTE_PICTURE=SET_ROUTE(0x74),//PICTURE图片
	ROUTE_APPEAL=SET_ROUTE(0x84),//申诉
	ROUTE_CLASS=SET_ROUTE(0x94),//班级
	ROUTE_ROOM=SET_ROUTE(0xB4),//小屋信息
	ROUTE_ROOMMSG=SET_ROUTE(0xC4),//小屋留言 
	ROUTE_GROUP=SET_ROUTE(0xD4),//群组
	ROUTE_PAIPAI=SET_ROUTE(0x18),//派派

	ROUTE_NOROUTE=SET_ROUTE(0xF0),//不用路由

}; 


void DES_n( char *key,char *s_text,char *d_text ,int count );
void _DES_n( char *key,char *s_text,char *d_text ,int count);

bool _iconv( char *in ,char *out, size_t inlen , 
		size_t  outlen, char *  inencode= (char *)"UTF-8",char * outencode=(char*)"GBK" );

#endif   /* ----- #ifndef COMMON_INC  ----- */

