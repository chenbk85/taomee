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
#include "./proto/mole2_db.h"
#endif


//在cmdid 中数据库识别标志
#define rroto_route_field 				0xFE00 //使用前7个bit

//是否需要保存交易报文
//有涉及到更新数据库数据的协议，都要保存 
#define NEED_UPDATE							0x0100

//view version
#define get_server_info_cmd                 (0x1000 )

//mole2:拉取用户角色信息
#define mole2_login_cmd                  	(0x0801 )
//mole2:用户注册角色信息
#define mole2_user_user_create_role_cmd  	(0x0802 |NEED_UPDATE)
//mole2:修改用户昵称
#define mole2_user_user_update_nick_cmd  	(0x0803 |NEED_UPDATE)
//mole2:修改用户签名
#define mole2_user_user_update_signature_cmd	(0x0804 |NEED_UPDATE)
//mole2:修改颜色
#define mole2_user_user_update_color_cmd 	(0x0805 |NEED_UPDATE)
//mole2:修改职业
#define mole2_user_user_update_professtion_cmd	(0x0806 |NEED_UPDATE)
//mole2:调整小米(+/-)
#define mole2_user_user_add_xiaomee_cmd  	(0x0807 |NEED_UPDATE)
//mole2:设置人物前后排位置
#define mole2_user_user_set_pos_cmd      	(0x0808 |NEED_UPDATE)
//mole2:增加经验
#define mole2_user_user_add_exp_cmd      	(0x0809 |NEED_UPDATE)
//mole2:调整人物基本属性：体力、力量、耐力、敏捷、智力
#define mole2_user_user_add_base_attr_cmd	(0x080A |NEED_UPDATE)
//mole2:调整人物剩余属性点(+/-)
#define mole2_user_user_add_attr_addition_cmd	(0x080B |NEED_UPDATE)
//mole2:拉取当天累计数量
#define mole2_user_get_day_list_cmd			(0x080C)
//mole2:治疗人物
#define mole2_user_user_cure_cmd         	(0x080D |NEED_UPDATE)
//mole2:增加宠物信息
#define mole2_user_pet_add_cmd           	(0x080E |NEED_UPDATE)
//mole2:获取某个宠物的信息
#define mole2_user_pet_get_info_cmd      	(0x080F )
//mole2:获取用户宠物列表
#define mole2_user_pet_get_list_cmd       	(0x0810 )
//mole2:修改宠物昵称
#define mole2_user_pet_update_nick_cmd   	(0x0811 |NEED_UPDATE)
//mole2:获取宠物基本属性初值
#define mole2_user_pet_get_base_attr_initial_cmd	(0x0812 )
//mole2:获取宠物基本属性成长值
#define mole2_user_pet_get_base_attr_param_cmd	(0x0813 )
//mole2:获取宠物简单信息
#define mole2_user_pet_cure_cmd				(0x0814 |NEED_UPDATE)
//mole2:修改宠物经验值
#define mole2_user_pet_add_exp_cmd       	(0x0815 |NEED_UPDATE)
//mole2:设置宠物剩余属性点
#define mole2_user_pet_add_attr_addition_cmd	(0x0816 |NEED_UPDATE)
//mole2:获取宠物四相属性
#define mole2_user_pet_get_four_attr_cmd 	(0x0817 )
//mole2:修改使用标志，0x02表示出战，其他暂未指定
#define mole2_user_pet_update_carry_flag_cmd               	(0x0818 |NEED_UPDATE)
//
#define add_item_type_counter_cmd               (0x0819 |NEED_UPDATE)
//mole2:用户登录，获取VIP标志、好友列表用于拉取服务器列表
#define mole2_user_user_login_cmd             	(0x081A )
//mole2:修改宠物基本属性初值（用于实现加点）
#define mole2_user_pet_add_base_attr_initial_cmd	(0x081B |NEED_UPDATE)
//恢复
#define get_item_type_counter_cmd               (0x081C)

//mole2:获取宠物标志
#define mole2_user_pet_get_flag_cmd      	(0x081E )
//mole2:添加离线消息
#define mole2_user_user_add_offline_msg_cmd	(0x0820 |NEED_UPDATE)
//mole2:获取离线消息
#define mole2_user_user_get_offline_msg_cmd	(0x0821 )
//mole2:添加好友
#define mole2_user_user_add_friend_cmd   	(0x0823 |NEED_UPDATE)
//mole2:删除好友
#define mole2_user_user_del_friend_cmd   	(0x0824 |NEED_UPDATE)
//mole2:获取好友列表
#define mole2_user_user_get_friend_list_cmd	(0x0825 )
//mole2:检查是否好友
#define mole2_user_user_check_is_friend_cmd	(0x0826 )
//mole2:添加黑名单
#define mole2_user_user_add_blackuser_cmd	(0x0827 |NEED_UPDATE)
//mole2:从黑名单删除
#define mole2_user_user_del_blackuser_cmd	(0x0828 |NEED_UPDATE)
//mole2:获取黑名单列表
#define mole2_user_user_get_black_list_cmd	(0x0829 )
//mole2:获取宠物配点值
#define mole2_user_pet_get_attr_add_cmd  	(0x082A )
//mole2:增加宠物配点值
#define mole2_user_pet_add_attr_add_cmd  	(0x082B |NEED_UPDATE)
//mole2:
#define mole2_user_get_nick_cmd  				(0x082C)
//mole2:宠物受伤治疗
#define mole2_user_pet_injury_cure_cmd   	(0x082D |NEED_UPDATE)
//mole2:添加用户装扮
#define mole2_user_attire_add_cmd        	(0x0830 |NEED_UPDATE)
//mole2:获取用户身上装扮列表
#define mole2_user_attire_get_list_cmd   	(0x0831 )
//mole2:获取某个装扮信息
#define mole2_user_attire_get_cmd        	(0x0832 )
//mole2:更新装扮位置信息
#define mole2_user_attire_update_gridid_cmd	(0x0833 |NEED_UPDATE)
//mole2:删除用户装扮
#define mole2_user_attire_del_cmd        	(0x0834 |NEED_UPDATE)
//mole2:获取用户背包中的装扮列表
#define mole2_user_attire_get_list_ex_cmd	(0x0835 )
//mole2:修改一系列装扮的属性
//#define mole2_user_attire_attr_add_list_cmd	(0x0836 |NEED_UPDATE)
//mole2:修改一系列装扮的位置信息
#define mole2_user_attire_update_gridid_list_cmd	(0x0837 |NEED_UPDATE)
//mole2:登录协议,拉取角色信息、携带宠物信息、装扮列表信息
#define mole2_user_user_login_ex_cmd     	(0x0838 )
//mole2:设置用户角色的hp和mp
#define mole2_user_user_set_hp_mp_cmd    	(0x0839 |NEED_UPDATE)
//mole2:设置宠物的hp和mp
#define mole2_user_pet_set_hp_mp_cmd     	(0x083A |NEED_UPDATE)
//拉取怪物手册中指定id区间的信息 id > 100000,为怪物组id - 100000
#define monster_handbook_range_get_cmd      (0x083B )

//mole2:添加一个新的技能
#define mole2_user_skill_add_cmd         	(0x0840 |NEED_UPDATE)
//mole2:获取用户技能列表
#define mole2_user_skill_get_list_cmd    	(0x0841 )
//mole2:用户技能等级变化
#define mole2_user_skill_add_level_cmd   	(0x0842 |NEED_UPDATE)
//mole2:删除用户的一个技能
#define mole2_user_skill_del_cmd         	(0x0843 |NEED_UPDATE)
//mole2:技能经验增加
#define mole2_user_skill_add_exp_cmd     	(0x0844 |NEED_UPDATE)
//mole2:添加一个宠物技能
//#define mole2_user_pet_skill_add_cmd     	(0x0845 |NEED_UPDATE)
//mole2:获取宠物技能列表
#define mole2_user_pet_skill_get_list_cmd	(0x0846 )
//mole2:购买技能
#define mole2_user_skill_buy_cmd         	(0x0847 |NEED_UPDATE)
//mole2:购买宠物技能
#define mole2_user_pet_skill_buy_cmd     	(0x0848 |NEED_UPDATE)
//mole2:删除宠物技能
//#define mole2_user_pet_skill_del_cmd     	(0x0849 |NEED_UPDATE)
//mole2:设置装备属性
#define user_attire_attrib_set_cmd			(0x0850 |NEED_UPDATE)
//mole2:获取仓库中的宠物列表
#define mole2_user_pet_get_in_store_list_cmd    (0x0851 )
//mole2:设置宠物属性
#define mole2_user_pet_set_cmd               (0x0852 |NEED_UPDATE)
//mole2:删除宠物
#define mole2_user_pet_delete_cmd            (0x0853 |NEED_UPDATE)

//mole2:物品增加（+/-）
#define mole2_user_item_add_cmd          	(0x0870 |NEED_UPDATE)
//mole2:获取物品列表
#define mole2_user_item_get_list_cmd     	(0x0871 )
//mole2:添加物品列表
#define mole2_user_item_add_list_cmd     	(0x0872 |NEED_UPDATE)
//mole2:更新物品在背包中的位置
#define mole2_user_item_buy_cmd			  	(0x0873 |NEED_UPDATE)
//mole2:获取itemid位于区间的物品列表
#define mole2_user_item_ranged_get_list_cmd	(0x0874 )

//设置用户伤害
#define user_user_injury_state_set_cmd   	(0x0875 |NEED_UPDATE)

//设置宠物伤害
#define user_pet_injury_state_set_cmd    	(0x0876 |NEED_UPDATE)

//获得用户伤害
#define user_user_injury_state_get_cmd   	(0x0877 )

//获得宠物伤害
#define user_pet_injury_state_get_cmd    	(0x0878 )

//设置任务奖励
//#define user_task_reward_set_cmd         	(0x0879 |NEED_UPDATE)

//获得衣服简单信息
#define user_attire_info_get_cmd         	(0x087A )

//卖衣服操作
#define user_attire_sell_set_cmd         	(0x087B |NEED_UPDATE)

//创建用户角色
#define user_create_role_set_cmd         	(0x087C |NEED_UPDATE)

//设置用户升级数据
#define user_user_levelup_set_cmd        	(0x087D |NEED_UPDATE)

//设置宠物升级数据
#define user_pet_levelup_set_cmd         	(0x087E |NEED_UPDATE)

//设置用户技能升级数据
#define user_user_skill_up_set_cmd       	(0x087F |NEED_UPDATE)

//往任务道具包中添加物品
#define mole2_user_task_item_add_cmd     	(0x0890 |NEED_UPDATE)

//删除任务道具
#define mole2_user_task_item_del_cmd     	(0x0891 |NEED_UPDATE)

//获取任务道具列表
#define mole2_user_task_item_get_list_cmd	(0x0892 )

//设置用户多个技能升级
#define user_user_skill_list_levelup_set_cmd         	(0x0893 |NEED_UPDATE)

//设置用户flag标识位
#define user_user_flag_set_cmd           	(0x0894 |NEED_UPDATE)

//获取宠物信息和宠物技能信息
#define user_pet_all_get_cmd             	(0x0895 )

//用户吃物品回复血蓝
#define user_user_medicine_cure_cmd      	(0x0896 |NEED_UPDATE)

//设置宠物位置：仓库or背包
#define user_pet_location_set_cmd         	(0x0897 |NEED_UPDATE)

//设置用户职业，勇士梦想记录
#define user_user_note_dream_set_cmd     	(0x0898 |NEED_UPDATE)

//设置抓捕的宠物信息
#define user_pet_catch_set_cmd           	(0x0899 |NEED_UPDATE)

//获得用户所有宠物和宠物技能信息
#define user_pet_list_all_get_cmd             	(0x089A )

//获得用户飞行点列表
#define user_map_flight_list_get_cmd     	(0x089B )

//验证用户飞行点
#define mole2_user_map_flight_check_cmd  	(0x089C )

//设置用户飞行点
#define user_map_flight_set_cmd          	(0x089D |NEED_UPDATE)

//设置用户登出信息
#define user_logout_position_set_cmd     	(0x089E |NEED_UPDATE)

//拉取图鉴信息列表
#define user_handbook_list_get_cmd       	(0x089F )

//增加物品使用次数
#define item_handbook_count_add_cmd      	(0x08A0 |NEED_UPDATE)

//增加多种怪物击杀次数
#define monster_handbook_count_list_add_cmd	(0x08A1 |NEED_UPDATE)

//设置物品图鉴状态
#define item_handbook_state_set_cmd      	(0x08A2 |NEED_UPDATE)

//设置怪物图鉴状态
#define monster_handbook_state_set_cmd   	(0x08A3 |NEED_UPDATE)

//获得用户称号列表
#define user_title_list_get_cmd          	(0x08A4 )

//设置用户当前使用的称号
#define user_honor_set_cmd               	(0x08A5 |NEED_UPDATE)

//设置宠物跟随标志
#define user_pet_follow_set_cmd          	(0x08A6 |NEED_UPDATE)

//获取物品图鉴的状态
#define item_handbook_state_get_cmd			(0x08A8)

//boss地图点激活
#define user_map_boss_position_set_cmd   	(0x08A9 |NEED_UPDATE)

//获得任务更新时间
#define user_task_optdate_get_cmd        	(0x08AB )

//任务笔记记录
#define tash_handbook_content_set_cmd    	(0x08AC |NEED_UPDATE)

//设置装备当前耐久值
#define user_attire_list_duration_set_cmd	(0x08AD |NEED_UPDATE)

//修理多件装备
#define user_attire_list_mend_set_cmd    	(0x08AE |NEED_UPDATE)

//删除用户称号
#define user_title_delete_cmd            	(0x08AF |NEED_UPDATE)

//获得用户仓库中所有宠物信息
#define pet_in_store_list_get_cmd        	(0x08B0 )

//设置仓库中宠物位置
#define pet_in_store_location_set_cmd    	(0x08B1 |NEED_UPDATE)

///用户物品放入,取出仓库
#define user_item_storage_set_cmd        	(0x08B2 |NEED_UPDATE)

//获得仓库中物品列表
#define user_item_in_storage_list_get_cmd	(0x08B3 )

//获得仓库中所有装备
#define user_attire_in_storage_list_get_cmd	(0x08B4 )

//设置用户对战信息
#define user_battle_info_set_cmd         	(0x08B5 |NEED_UPDATE)


//获得小屋信息
#define home_info_get_cmd                	(0x08B6 )

//增加减少仓库物品数量
#define item_storage_add_cmd             	(0x08B7 |NEED_UPDATE)

//获得其他用户基本信息和身上的装备
#define mole2_user_other_user_info_cmd   	(0x08B8 )

//增加信件
#define user_mail_send_set_cmd           	(0x08B9 |NEED_UPDATE)

//获取邮件携带的物品
#define mail_get_post_items_cmd          (0x08BA |NEED_UPDATE)

//拉取邮件简单信息列表
#define user_mail_simple_list_get_cmd    	(0x08BB )

//经验树经验设置
#define user_exp_info_set_cmd            	(0x08BC |NEED_UPDATE)

//拉取邮件内容信息
#define user_mail_content_get_cmd        	(0x08BD )

//删除邮件
#define user_mail_list_del_cmd           	(0x08BE |NEED_UPDATE)

//设置邮件标志位
#define user_mail_flag_set_cmd           	(0x08BF |NEED_UPDATE)

//设置任务信息
#define mole2_task_set_cmd               	(0x08C0 |NEED_UPDATE)

//设置任务精力值
#define user_energy_set_cmd              	(0x08C1 |NEED_UPDATE)

//获得任务信息
#define mole2_task_info_get_cmd          	(0x08C2 )

//获得任务信息列表
#define mole2_task_info_list_get_cmd     	(0x08C3 )

//设置任务奖励
#define user_task_reward_set_cp_cmd         	(0x08C4 |NEED_UPDATE)

//设置后端buf
#define mole2_task_ser_buf_set_cmd       	(0x08C5 |NEED_UPDATE)

//删除任务
#define mole2_task_delete_cmd            	(0x08C6 |NEED_UPDATE)

//设置小屋flag位
#define mole2_home_flag_set_cmd          	(0x08C7 |NEED_UPDATE)

//设置打赢的BOSSID
#define set_winbossid_cmd                       (0x08C8 |NEED_UPDATE)

//设置封停标志
#define set_ban_flag_cmd                        (0x08C9 |NEED_UPDATE)

//用户交换，宠物
#define user_swap_pets_cmd                      (0x08CA |NEED_UPDATE)

//设置称号和职业等级
#define set_title_and_job_level_cmd            (0x08CB |NEED_UPDATE)

//获取经验树经验
#define proto_get_expbox_cmd						(0x08CC)

//设置经验树经验
#define proto_set_expbox_cmd                    (0x08CD |NEED_UPDATE)

//获取封号标志
#define user_get_ban_flag_cmd                   (0x08CE )

//增加SSID对应的数量并检查是否超出当日上限
#define day_add_ssid_count_cmd              	   (0x08CF |NEED_UPDATE)

//获取SSID对应的信息
#define day_get_ssid_info_cmd                   (0x08D0 )
//减少SSID对应的总数量
#define day_sub_ssid_total_cmd                  (0x08D1 |NEED_UPDATE)

//########### RAND_ITEM库  ########

//检查用户是否能获得活动物品
#define rand_item_info_check_cmd         	(0xD420 |NEED_UPDATE)

//添加用户获得物品记录
#define rand_item_info_set_cmd           	(0xD421 |NEED_UPDATE)

//激活码检查和记录
#define activation_code_check_cmd        	(0xD422 |NEED_UPDATE)

//设置神奇密码卡礼包获取
#define rand_item_present_set_cmd        	(0xD423 |NEED_UPDATE)

//激活码设置
#define rand_item_activation_set_cmd     	(0xD424 |NEED_UPDATE)

#define get_args_cmd     	(0xD425 )
#define set_args_cmd     	(0xD426 |NEED_UPDATE)

//########### event 库  ########

//增加邀请成功的人数
#define mole2_user_add_invited_cmd              (0xC0A0 |NEED_UPDATE)

//获取邀请成功的人数
#define mole2_user_get_invited_cmd              (0xC0A1 )

//#################################


#define TITLE_LEN	60 
#define NICK_LEN	16 
#define SIGN_LEN	32
#define HOLE_LIST_BUF_LEN	16

#define	FRIEND_COUNT_MAX	200
#define ID_LIST_MAX			200
#define MSG_MAX_LEN			100	
#define MSG_LIST_BUF_SIZE		1992
#define TASK_SVR_BUF_LEN		20
#define TASK_CLI_BUF_LEN    	128	
#define TASK_CLI_BUF_LEN_OLD	40

#define ATTRIBUTE_VALUE		5

#define NOTE_LEN	156	
#define ACTIVATION_CODE_LEN	16

#define MAIL_ITEM_LIST_MAX	64
#define MAIL_MESSAGE_LEN	256
#define MAIL_TITLE_LEN	64
#define MAIL_SEND_NICK_LEN	64
#define MAIL_ID_LIST_LEN	32

#define PRESENT_STRCODE_LEN		16
#define ACTIVATION_STRCODE_LEN	16


struct  add_item_type_counter_in{
    uint32_t type;
    uint32_t itemid;
    uint32_t count;
}__attribute__((packed));
struct  add_item_type_counter_out{
    uint32_t type;
    uint32_t itemid;
    uint32_t count;
}__attribute__((packed));

struct  get_item_type_counter_in{
    uint32_t type;
    uint32_t itemid;
}__attribute__((packed));

struct  get_item_type_counter_out{
    uint32_t type;
    uint32_t itemid;
    uint32_t count;
}__attribute__((packed));


struct  stru_char{
    char   c;
}__attribute__((packed));
struct stru_count{
    uint32_t        count;
} __attribute__((packed));

struct stru_id_list{
	uint32_t	count;
	uint32_t	iditem[ID_LIST_MAX];
}__attribute__((packed));

struct stru_mole2_user_skill_info{
	uint32_t	skill_id;
	uint8_t		skill_lv;
	uint8_t		use_level;
	uint32_t	skill_exp;
}__attribute__((packed));

struct mole2_user_vip_info{
	uint32_t	vip_auto;
	uint32_t	vip_ex_val;
	uint32_t	vip_base_val;
	uint32_t	vip_end_time;
	uint32_t	vip_begin_time;
	uint32_t	vip_activity;
	uint32_t	vip_activity_2;
	uint32_t	vip_activity_3;
	uint32_t	vip_activity_4;
}__attribute__((packed));

struct  stru_mole2_user_pet_info{
	uint32_t	pettype;
	uint8_t		race;
	uint32_t	flag;
	char		nick[NICK_LEN];
	uint32_t	level;
	uint32_t	exp;
	uint16_t	physiqueinit;
	uint16_t	strengthinit;
	uint16_t	enduranceinit;
	uint16_t	quickinit;
	uint16_t	iqinit;
	uint16_t	physiqueparam;
	uint16_t	strengthparam;
	uint16_t	enduranceparam;
	uint16_t	quickparam;
	uint16_t	iqparam;
	uint16_t	physique_add;
	uint16_t	strength_add;
	uint16_t	endurance_add;
	uint16_t	quick_add;
	uint16_t	iq_add;
	uint16_t	attr_addition;
	uint32_t	hp;
	uint32_t	mp;
	uint8_t		earth;
	uint8_t		water;
	uint8_t		fire;
	uint8_t		wind;
	uint32_t	injury_state;
	uint32_t		location;
}__attribute__((packed));

struct  stru_mole2_user_pet_info_1{
	uint32_t	gettime;
	stru_mole2_user_pet_info	pet_info;
}__attribute__((packed));

struct stru_mole2_user_pet_info_2{
	uint32_t	gettime;
	stru_mole2_user_pet_info	pet_info;
	uint32_t	skillcnt;
}__attribute__((packed));

struct	stru_mole2_user_user_info{
	char		nick[NICK_LEN];
	char		signature[SIGN_LEN];
	uint32_t	flag;
	uint32_t	color;
	uint32_t	register_time;
	uint8_t		race;
	uint8_t		professtion;
	uint32_t	joblevel;
	uint32_t	honor;
	uint32_t	xiaomee;
	uint32_t	level;
	uint32_t	experience;
	uint16_t	physique;
	uint16_t	strength;
	uint16_t	endurance;
	uint16_t	quick;
	uint16_t	intelligence;
	uint16_t	attr_addition;
	uint32_t	hp;
	uint32_t	mp;
	uint32_t	injury_state;
	uint8_t		in_front;
	uint32_t	max_attire;
	uint32_t	max_medicine;
	uint32_t	max_stuff;
}__attribute__((packed));

struct	stru_mole2_user_user_info_ex{   //using in login_ex
	stru_mole2_user_user_info userinfo;
	uint32_t time;
	uint32_t axis_x;
	uint32_t axis_y;
	uint32_t mapid;
	uint32_t maptype;
	uint32_t daytime;
	uint32_t fly_mapid;
	uint32_t expbox;
	uint32_t energy;
	uint32_t skill_expbox;
	uint32_t flag_ex;
	uint32_t winbossid;
	userid_t parent;
	uint32_t total_time;
	uint32_t levelup_time;
	uint32_t sign_count;
	mole2_user_vip_info vip_info;
}__attribute__((packed));

struct  stru_mole2_user_attire_info{
	uint32_t	attire_id;
	uint32_t	gridid;
	uint16_t	level;
	uint16_t	mduration;
	uint16_t	duration;
	uint32_t	hpmax;
	uint32_t	mpmax;
	uint16_t	atk;
	uint16_t	matk;
	uint16_t	def;
	uint16_t	mdef;
	uint16_t	speed;
	uint16_t	spirit;
	uint16_t	resume;
	uint16_t	hit;
	uint16_t	dodge;
	uint16_t	crit;
	uint16_t	fightback;
	uint16_t	rpoison;
	uint16_t	rlithification;
	uint16_t	rlethargy;
	uint16_t	rinebriation;
	uint16_t	rconfusion;
	uint16_t	roblivion;
	uint32_t	quality;
	uint32_t	validday;
	uint32_t	crystal_attr;
	uint32_t	bless_type;
}__attribute__((packed));

struct  stru_mole2_user_attire_info_1{
	uint32_t	gettime;
	stru_mole2_user_attire_info attire_info;
}__attribute__((packed));

typedef stru_mole2_user_attire_info_1 user_attire_attrib_set_in;

struct  stru_mole2_user_user_xiaomee_info{
	uint32_t	xiaomee;
	int32_t		add_value;
}__attribute__((packed));

struct stru_mole2_pet_skill_info{
	uint32_t	skillid;
	uint8_t		level;
	uint8_t		use_level;
	uint32_t	exp;
}__attribute__((packed));

struct  stru_mole2_user_item_info{
	uint32_t	itemid;
	int32_t	count;
}__attribute__((packed));

struct stru_mole2_task_info{
	uint32_t	taskid;
	uint32_t	node;
	uint32_t	state;
	uint32_t	optdate;
	uint32_t	fin_time;
	uint32_t	fin_num;
	char		cli_buf[TASK_CLI_BUF_LEN];
	char		ser_buf[TASK_SVR_BUF_LEN];
}__attribute__((packed));

struct  stru_mole2_task_info_simple{
	uint32_t	taskid;
	uint32_t	state;
	uint32_t	optdate;
	uint32_t	fin_time;
	uint32_t	fin_num;
}__attribute__((packed));


struct stru_mole2_item_handbook_info{
	uint32_t itemid;
	uint32_t state;
	uint32_t count;
}__attribute__((packed));

struct stru_mole2_monster_handbook_info{
	uint32_t monsterid;
	uint32_t state;
	uint32_t count;
}__attribute__((packed));

struct stru_mole2_user_title_info{
	uint32_t titleid;
}__attribute__((packed));

struct stru_mole2_user_title_update{
	uint32_t titleid_old;
	uint32_t titleid_new;
	uint32_t time;
}__attribute__((packed));

struct stru_mole2_user_battle_info{
	uint32_t	all_score;
	uint32_t	day_score;
	uint32_t	update_time;
}__attribute((packed));

struct stru_mole2_user_battle_info_1{
	stru_mole2_user_battle_info	binfo;
	uint32_t	person_win;
	uint32_t	person_fail;
	uint32_t	team_win;
	uint32_t	team_fail;
}__attribute__((packed));

struct stru_mole2_home_info_t{
	uint32_t	userid;
	uint32_t	flag;
	uint32_t	type;
	uint32_t	level;
	uint32_t	exp;
	char		name[NICK_LEN];
	uint32_t	photo;
	uint32_t	blackboard;
	uint32_t	bookshelf;
	uint32_t	honorbox;
	uint32_t	petfun;
	uint32_t	expbox;
	uint32_t	effigy;
	uint32_t	postbox;
	uint32_t	itembox;
	uint32_t	petbox;
	uint32_t	compose1;
	uint32_t	compose2;
	uint32_t	compose3;
	uint32_t	compose4;
	uint32_t	wall;
	uint32_t	floor;
	uint32_t	bed;
	uint32_t	desk;
	uint32_t	ladder;
	uint32_t	petbox_cnt;
}__attribute__((packed));

/*
struct stru_mole2_mail_item_info{
	uint32_t	count;
	uint32_t	item[MAIL_ITEM_LIST_MAX];
}__attribute__((packed));
*/
struct stru_mole2_mail_info{
	uint32_t	flag;
	uint32_t	type;
	uint32_t	themeid;
	uint32_t	senderid;
	char		sender_nick[MAIL_SEND_NICK_LEN];
	char		title[MAIL_TITLE_LEN];
	char		message[MAIL_MESSAGE_LEN];
	char		items[MAIL_ITEM_LIST_MAX];
	char		ids[MAIL_ID_LIST_LEN];
}__attribute__((packed));


struct stru_mole2_mail_info_1{
	uint32_t	userid;
	uint32_t	mailid;
	stru_mole2_mail_info mail_info;
}__attribute__((packed));


struct  stru_mole2_user_levelup_info{
	uint32_t	experience;
	uint32_t	level;
	uint32_t	addition;
	uint32_t	hp;
	uint32_t	mp;
}__attribute__((packed));

struct  stru_mole2_pet_levelup_info{
	uint32_t	petid;
	uint32_t	experience;
	uint32_t	level;
	uint32_t	addition;
	uint32_t	hp;
	uint32_t	mp;
}__attribute__((packed));












//--------------
struct  get_server_info_out{
    char    msg[201];
}__attribute__((packed));

//typedef stru_mole2_user_user_info mole2_login_out;

struct mole2_user_user_login_out_header{
	uint32_t	vip;
	uint32_t	lastonline;
	uint32_t	friend_cnt;
	uint32_t	black_cnt;
}__attribute__((packed));

struct mole2_user_user_login_out_item{
	uint32_t	userid;
}__attribute__((packed));

/*
struct  mole2_user_user_create_role_in{
	userid_t	parent;
	char		nick[NICK_LEN];
	char		signature[SIGN_LEN];
	uint32_t	color;
	uint8_t		professtion;
}__attribute__((packed));
*/

struct	mole2_user_user_add_offline_msg_in_header{
	uint32_t	msglen;
}__attribute__((packed));

struct  mole2_user_user_get_offline_msg_out{
	uint32_t	msglen;
	uint32_t	count;
	char		msg[MSG_LIST_BUF_SIZE];
}__attribute__((packed));

typedef mole2_user_user_get_offline_msg_out \
			mole2_user_user_update_offline_msg_in;

struct  mole2_user_user_add_friend_in{
	uint32_t	userid;
}__attribute__((packed));

struct  mole2_user_user_del_friend_in{
	uint32_t	userid;
}__attribute__((packed));

typedef stru_id_list  mole2_user_user_get_friend_list_out;

struct  mole2_user_user_check_is_friend_in{
	uint32_t	userid;
}__attribute__((packed));

struct  mole2_user_user_check_is_friend_out{
	uint32_t	isfriend;
}__attribute__((packed));

struct  mole2_user_user_add_blackuser_in{
	uint32_t userid;
}__attribute__((packed));

struct  mole2_user_user_del_blackuser_in{
	uint32_t	userid;
}__attribute__((packed));

typedef  stru_id_list mole2_user_user_get_black_list_out; 

/*
struct  mole2_user_user_update_nick_in{
	char		nick[NICK_LEN];
}__attribute__((packed));

struct  mole2_user_user_update_nick_out{
	char		nick[NICK_LEN];
}__attribute__((packed));
*/
struct  mole2_user_user_update_signature_in{
	char		signature[SIGN_LEN];
}__attribute__((packed));
struct  mole2_user_user_update_signature_out{
	char		signature[SIGN_LEN];
}__attribute__((packed));

struct  mole2_user_user_update_color_in{
	uint32_t	color;
}__attribute__((packed));
struct  mole2_user_user_update_color_out{
	uint32_t	color;
}__attribute__((packed));

struct  mole2_user_user_update_professtion_in{
	uint8_t		professtion;
}__attribute__((packed));
struct  mole2_user_user_update_professtion_out{
	uint8_t		professtion;
}__attribute__((packed));

struct  mole2_user_user_add_xiaomee_in{
	int32_t		xiaomee;
}__attribute__((packed));

typedef stru_mole2_user_user_xiaomee_info mole2_user_user_add_xiaomee_out;


struct  mole2_user_user_add_exp_in{
	int32_t		experience;
}__attribute__((packed));
struct  mole2_user_user_add_exp_out{
	uint32_t	level;
	uint32_t	experience;
	uint16_t	attr_addition;
}__attribute__((packed));

struct  mole2_user_user_set_pos_in{
	uint8_t		pos;
}__attribute__((packed));
struct  mole2_user_user_set_pos_out{
	uint8_t		pos;
}__attribute__((packed));

struct  mole2_user_user_injury_cure_in{
	int32_t		xiaomee;
}__attribute__((packed));
typedef mole2_user_user_add_xiaomee_out mole2_user_user_injury_cure_out;

struct  mole2_user_user_add_base_attr_in{
	uint16_t		physique;
	uint16_t		strength;
	uint16_t		endurance;
	uint16_t		quick;
	uint16_t		intelligence;
	uint16_t		attr_addition;
}__attribute__((packed));
struct  mole2_user_user_add_base_attr_out{
	uint16_t		physique;
	uint16_t		strength;
	uint16_t		endurance;
	uint16_t		quick;
	uint16_t		intelligence;
	uint16_t		attr_addition;
}__attribute__((packed));

struct  mole2_user_user_add_attr_addition_in{
	int16_t		attr_addition;
}__attribute__((packed));
struct  mole2_user_user_add_attr_addition_out{
	uint16_t	attr_addition;
}__attribute__((packed));

struct  mole2_user_user_add_hp_mp_in{
	int32_t		hp;
	int32_t		mp;
}__attribute__((packed));
struct  mole2_user_user_add_hp_mp_out{
	int		hp;
	int		mp;
}__attribute__((packed));

struct  mole2_user_user_cure_in_header {
	uint32_t	count;
	int32_t		add_xiaomee;
}__attribute__((packed));

struct  mole2_user_user_cure_in_item{
	uint32_t    petid;
	uint32_t	hp;
	uint32_t	mp;
	uint32_t	injury;
}__attribute__((packed));

struct mole2_user_user_cure_out_header{
	uint32_t	count;
	mole2_user_user_add_xiaomee_out xiaomee_info;
}__attribute__((packed));

typedef mole2_user_user_cure_in_item mole2_user_user_cure_out_item;

/* 
struct  mole2_user_user_add_four_attr_in{
	int8_t		earth;
	int8_t		water;
	int8_t		fire;
	int8_t		wind;
}__attribute__((packed));
struct  mole2_user_user_add_four_attr_out{
	uint8_t		earth;
	uint8_t		water;
	uint8_t		fire;
	uint8_t		wind;
}__attribute__((packed));
*/

struct  mole2_user_user_offline_in{
	uint32_t	time;
}__attribute__((packed));

typedef stru_mole2_user_pet_info mole2_user_pet_add_in;

typedef stru_mole2_user_pet_info_1 mole2_user_pet_add_out;

typedef stru_mole2_user_pet_info_1 mole2_user_pet_get_info_out;
typedef stru_mole2_user_pet_info_1 mole2_user_pet_info_out_item;

struct mole2_user_pet_cure_in{
	uint32_t	petid;
	uint32_t	hp;
	uint32_t	mp;
	int32_t		xiaomee;
}__attribute__((packed));
struct mole2_user_pet_cure_out{
	uint32_t	petid;
	uint32_t	hp;
	uint32_t	mp;
	stru_mole2_user_user_xiaomee_info xiaomee_info;
}__attribute__((packed));

struct  mole2_user_pet_get_attr_add_in{
	uint32_t	petid;
}__attribute__((packed));
struct  mole2_user_pet_get_attr_add_out{
	uint32_t	petid;
	uint16_t	physique_add;
	uint16_t	strength_add;
	uint16_t	endurance_add;
	uint16_t	quick_add;
	uint16_t	iq_add;
}__attribute__((packed));

struct  mole2_user_pet_injury_cure_in{
	uint32_t	petid;
	int32_t		xiaomee;
}__attribute__((packed));
struct  mole2_user_pet_injury_cure_out{
	uint32_t	petid;
	stru_mole2_user_user_xiaomee_info xiaomee_info; 
}__attribute__((packed));

struct  mole2_user_attire_add_out{
	uint32_t	xiaomee_all;
	int32_t		xiaomee_change;
	stru_mole2_user_attire_info_1 attire_info_ex;
}__attribute__((packed));

struct mole2_user_attire_add_in{
	int32_t		xiaomee;
	stru_mole2_user_attire_info attire_info;
}__attribute__((packed));
typedef stru_count   mole2_user_attire_get_list_out_header;
typedef stru_mole2_user_attire_info_1 \
			mole2_user_attire_get_list_out_item;
typedef stru_count   mole2_user_attire_get_list_ex_out_header;
typedef stru_mole2_user_attire_info_1 \
			mole2_user_attire_get_list_ex_out_item;
struct  mole2_user_attire_get_in{
	uint32_t	gettime;
}__attribute__((packed));
typedef stru_mole2_user_attire_info_1 mole2_user_attire_get_out;
struct  mole2_user_attire_update_gridid_in{
	uint32_t	gettime;
	uint32_t	gridid;
}__attribute__((packed));

typedef stru_mole2_user_attire_info_1 mole2_user_attire_update_gridid_out;

struct  mole2_user_attire_del_in{
	uint32_t	gettime;
}__attribute__((packed));

struct  mole2_user_attire_del_out{
	uint32_t	gettime;
}__attribute__((packed));

typedef stru_count   mole2_user_attire_attr_add_list_in_header;
struct  mole2_user_attire_attr_add_list_in_item{
	uint32_t	gettime;
	int16_t		level;
	int16_t		duration;
	int32_t		hpmax;
	int32_t		mpmax;
	int16_t		atk;
	int16_t		matk;
	int16_t		def;
	int16_t		mdef;
	int16_t		speed;
	int16_t		spirit;
	int16_t		resume;
	int16_t		hit;
	int16_t		dodge;
	int16_t		crit;
	int16_t		fightback;
	int16_t		rpoison;
	int16_t		rlithification;
	int16_t		rlethargy;
	int16_t		rinebriation;
	int16_t		rconfusion;
	int16_t		roblivion;
}__attribute__((packed));
typedef stru_count   mole2_user_attire_attr_add_list_out_header;
typedef stru_mole2_user_attire_info_1  mole2_user_attire_attr_add_list_out_item;
typedef stru_count   mole2_user_attire_update_gridid_list_in_header;
typedef mole2_user_attire_update_gridid_in  mole2_user_attire_update_gridid_list_in_item;
struct mole2_user_attire_update_gridid_list_out_header {
	uint32_t	attire_body_cnt;
	uint32_t	attire_bag_cnt;
}__attribute__((packed));
typedef stru_mole2_user_attire_info_1  mole2_user_attire_update_gridid_list_out_item_1;
/*  struct mole2_user_attire_update_gridid_list_out_item_2 {
	uint32_t	gettime;
	uint32_t	attire_id;
	uint16_t	attire_lv;
}__attribute__((packed));
*/
struct  mole2_user_pet_add_attr_add_in{
	uint32_t	petid;
	int16_t		physique_add;
	int16_t		strength_add;
	int16_t		endurance_add;
	int16_t		quick_add;
	int16_t		iq_add;
}__attribute__((packed));
typedef mole2_user_pet_get_attr_add_out  mole2_user_pet_add_attr_add_out;

struct	mole2_user_pet_get_info_in{
	uint32_t	petid;
}__attribute__((packed));

typedef stru_count   mole2_user_pet_get_list_out_header;
typedef stru_mole2_user_pet_info_1 \
	mole2_user_pet_get_list_out_item;
struct  mole2_user_pet_update_nick_in{
	uint32_t	petid;
	char		nick[NICK_LEN];
}__attribute__((packed));
struct  mole2_user_pet_update_nick_out{
	uint32_t	petid;
	char		nick[NICK_LEN];
}__attribute__((packed));
struct  mole2_user_pet_get_base_attr_initial_in{
	uint32_t	petid;
}__attribute__((packed));
struct  mole2_user_pet_get_base_attr_initial_out{
	uint32_t	petid;
	uint16_t	physiqueinit;
	uint16_t	strengthinit;
	uint16_t	enduranceinit;
	uint16_t	quickinit;
	uint16_t	iqinit;
}__attribute__((packed));
struct  mole2_user_pet_add_base_attr_initial_in{
	uint32_t	petid;
	int16_t		physiqueinit;
	int16_t		strengthinit;
	int16_t		enduranceinit;
	int16_t		quickinit;
	int16_t		iqinit;
}__attribute__((packed));
struct  mole2_user_pet_get_base_attr_param_in{
	uint32_t	petid;
}__attribute__((packed));
struct  mole2_user_pet_get_base_attr_param_out{
	uint32_t 	petid;
	uint8_t		physiqueparam;
	uint8_t		strengthparam;
	uint8_t		enduranceparam;
	uint8_t		quickparam;
	uint8_t		iqparam;
}__attribute__((packed));
struct  mole2_user_pet_add_exp_in{
	uint32_t	petid;
	int32_t		exp;
}__attribute__((packed));
struct  mole2_user_pet_add_exp_out{
	uint32_t	petid;
	uint32_t	exp;
}__attribute__((packed));
struct  mole2_user_pet_add_attr_addition_in{
	uint32_t	petid;
	uint16_t	physique;
	uint16_t	strength;
	uint16_t	endurance;
	uint16_t	quick;
	uint16_t	iq;
	uint16_t	attr_addition;
}__attribute__((packed));

typedef mole2_user_pet_add_attr_addition_in  mole2_user_pet_add_attr_addition_out;

struct  mole2_user_pet_get_four_attr_in{
	uint32_t	petid;
}__attribute__((packed));
struct  mole2_user_pet_get_four_attr_out{
	uint32_t	petid;
	uint8_t		earth;
	uint8_t		water;
	uint8_t		fire;
	uint8_t		wind;
}__attribute__((packed));
struct  mole2_user_pet_update_carry_flag_in{
	uint32_t	petid;
	uint32_t	flag;
}__attribute__((packed));
typedef stru_mole2_user_pet_info_1 \
			mole2_user_pet_update_carry_flag_out; 
struct  mole2_user_pet_add_hp_mp_in{
	uint32_t	petid;
	int32_t		hp;
	int32_t		mp;
}__attribute__((packed));
struct  mole2_user_pet_add_hp_mp_out{
	uint32_t	petid;
	int	hp;
	int	mp;
}__attribute__((packed));

struct	mole2_user_pet_set_hp_mp_in{
	uint32_t petid;
	int	hp;
	int	mp;
	uint32_t	injury_state;
}__attribute__((packed));
typedef mole2_user_pet_set_hp_mp_in mole2_user_pet_set_hp_mp_out;

struct  mole2_user_pet_update_color_in{
	uint32_t	petid;
	uint32_t	color;
}__attribute__((packed));
struct  mole2_user_pet_update_color_out{
	uint32_t	petid;
	uint32_t	color;
}__attribute__((packed));
struct  mole2_user_pet_get_flag_in{
	uint32_t	petid;
}__attribute__((packed));
struct  mole2_user_pet_get_flag_out{
	uint32_t	petid;
	uint32_t	flag;
}__attribute__((packed));

struct  mole2_user_skill_add_in{
	uint32_t	skillid;
	uint8_t		level;
}__attribute__((packed));

struct  mole2_user_skill_buy_in{
	uint32_t	skill_id;
	uint8_t		skill_lv;
	int32_t		xiaomee;
}__attribute__((packed));
struct  mole2_user_skill_buy_out{
	stru_mole2_user_skill_info	skill;
	stru_mole2_user_user_xiaomee_info xiaomee;
}__attribute__((packed));
struct  mole2_user_pet_skill_buy_in{
	uint32_t	petid;
	uint32_t	skillid;
	uint8_t		level;
	int32_t		xiaomee;
}__attribute__((packed));
struct  mole2_user_pet_skill_buy_out{
	uint32_t	petid;
	uint32_t	skillid;
	uint8_t		level;
	mole2_user_user_add_xiaomee_out xiaomee;
}__attribute__((packed));


struct  mole2_user_skill_get_list_out_header{
	uint32_t	count;
}__attribute__((packed));

struct  mole2_user_skill_get_list_out_item{
	uint32_t	skillid;
	uint8_t		level;
	uint8_t		use_level;
	uint32_t	exp;
}__attribute__((packed));

struct  mole2_user_skill_add_level_in{
	uint32_t	skillid;
	int8_t		level;
}__attribute__((packed));
struct  mole2_user_skill_add_level_out{
	uint32_t	skillid;
	uint8_t		level;
}__attribute__((packed));

struct  mole2_user_skill_del_in{
	uint32_t	skillid;
}__attribute__((packed));

struct  mole2_user_skill_del_out{
	uint32_t	skillid;
}__attribute__((packed));

struct  mole2_user_skill_add_exp_in{
	uint32_t	skillid;
	int32_t		exp;
}__attribute__((packed));
struct  mole2_user_skill_add_exp_out{
	uint32_t	skillid;
	uint32_t	exp;
}__attribute__((packed));
/*
struct  mole2_user_pet_skill_add_in{
	uint32_t	petid;
	uint32_t	skillid;
	uint8_t		level;
}__attribute__((packed));
struct  mole2_user_pet_skill_add_out{
	uint32_t	petid;
	uint32_t	skillid;
	uint8_t		level;
}__attribute__((packed));
*/
struct  mole2_user_pet_skill_get_list_in{
	uint32_t	petid;
}__attribute__((packed));

struct mole2_user_pet_skill_get_list_out_header{
	uint32_t	petid;
	uint32_t	count;
}__attribute__((packed));

typedef stru_mole2_pet_skill_info mole2_user_pet_skill_get_list_out_item;
/*
struct  mole2_user_pet_skill_del_in{
	uint32_t	petid;
	uint32_t	skillid;
	uint8_t		level;
}__attribute__((packed));

struct  mole2_user_pet_skill_del_out{
	uint32_t	petid;
	uint32_t	skillid;
	uint8_t		level;
}__attribute__((packed));
*/


struct  mole2_user_item_add_in{
	uint32_t	type;
	uint32_t	itemid;
	int			count;
}__attribute__((packed));

typedef mole2_user_item_add_in mole2_user_item_add_out;

typedef stru_count mole2_user_item_get_list_out_header;

typedef stru_mole2_user_item_info mole2_user_item_get_list_out_item;


struct  mole2_user_item_ranged_get_list_in{
	uint32_t	start_id;
	uint32_t	end_id;
}__attribute__((packed));
typedef stru_count   mole2_user_item_ranged_get_list_out_header;
struct mole2_user_item_ranged_get_list_out_item {
	uint32_t itemid;
	uint32_t bag_count;
	uint32_t store_count;
}__attribute__((packed));

struct mole2_user_item_add_list_in_header{
	int32_t		xiaomee;
	uint32_t	count;
}__attribute__((packed));
struct mole2_user_item_add_list_out_header{
	uint32_t	xiaomee;
	int32_t		add_value;
	uint32_t	count;
}__attribute__((packed));
typedef mole2_user_item_add_in mole2_user_item_add_list_in_item;
typedef mole2_user_item_add_in mole2_user_item_add_list_out_item;
typedef mole2_user_item_add_out mole2_user_item_get_out;


struct mole2_user_user_task_in{
	uint32_t	taskid;
}__attribute__((packed));

struct  mole2_user_item_buy_in{
	uint32_t	itemid;
	int32_t		count;
	int32_t		xiaomee;
}__attribute__((packed));
struct  mole2_user_item_buy_out{
	uint32_t	itemid;
	uint32_t	count;
	stru_mole2_user_user_xiaomee_info xiaomee_info;
}__attribute__((packed));

typedef stru_count mole2_user_pet_info_out_header;

typedef mole2_user_skill_get_list_out_item mole2_user_user_login_ex_out_item_1;

typedef stru_mole2_user_attire_info_1 mole2_user_user_login_ex_out_item_2;


typedef stru_mole2_user_pet_info_2 	mole2_user_user_login_ex_out_item_3;

/*
struct mole2_user_user_login_ex_out_item_4{
	uint32_t	gettime;
	uint32_t	attireid;
	uint16_t	level;
}__attribute__((packed));
*/


typedef stru_mole2_user_attire_info_1 mole2_user_user_login_ex_out_item_4;

struct mole2_user_user_login_ex_out_item_5{
	uint32_t	itemid;
	uint32_t	count;
}__attribute__((packed));

struct mole2_user_user_login_ex_out_item_6{
	uint32_t	taskid;
}__attribute__((packed));

typedef stru_mole2_user_title_info mole2_user_user_login_ex_out_item_7;

typedef struct {
	uint32_t	uid;
	uint32_t	day;
	uint32_t	val;
}__attribute__((packed))priv_relation_t;

typedef struct {
	uint32_t flag;
	uint32_t exp;
	uint32_t graduation;
	uint32_t total_val;
	uint32_t count;
	priv_relation_t relations[5];
}__attribute__((packed))stru_relation_info_t;

//typedef mole2_user_user_task_get_list_out_item mole2_user_user_login_ex_out_item_4;
struct  mole2_user_user_login_ex_out_header{
	stru_mole2_user_user_info_ex	userinfo_ex;
	stru_mole2_user_battle_info		user_battle_info;
	stru_relation_info_t			relation_info;
	uint32_t	friends_cnt;
	uint32_t	all_pet_cnt;
	uint32_t	skill_count;
	uint32_t	pet_count;
	uint32_t	attire_on_body_count;
	uint32_t	attire_in_grid_count;
	uint32_t	item_count;
	uint32_t	task_recv_count;
	uint32_t	title_count;
}__attribute__((packed));

struct  user_user_injury_state_set_in{
	uint32_t injury_state;
}__attribute__((packed));

struct  user_pet_injury_state_set_in{
	uint32_t petid;
	uint32_t injury_state;
}__attribute__((packed));

struct  user_user_injury_state_get_out{
	uint32_t injury_state;
}__attribute__((packed));

struct  user_pet_injury_state_get_in{
	uint32_t petid;
}__attribute__((packed));


struct  user_pet_injury_state_get_out{
	uint32_t injury_state;
}__attribute__((packed));

struct  user_task_reward_set_in_header{
	stru_mole2_task_info task_in;
	uint32_t	xiaomee;
	uint32_t	level;
	uint32_t	exp;
	uint32_t	expbox;
	uint32_t	skill_expbox;
	uint32_t	attr_addition;
	uint32_t	hp;
	uint32_t	mp;
	uint32_t	flag_ex;
	uint32_t	joblevel;
	uint32_t	clothes_count;
	uint32_t	item_count;
	uint32_t	pet_count;
	uint32_t	title_count;
	uint32_t	user_skill_cnt;
	uint32_t	petcnt;
}__attribute__((packed));

typedef	stru_mole2_user_attire_info user_task_reward_set_in_item_1;

typedef	stru_mole2_user_item_info user_task_reward_set_in_item_2;	 

struct user_task_reward_set_in_item_attr{
	uint32_t type;
	uint32_t value; 
}__attribute__((packed));

struct user_task_reward_set_in_item_3{
	uint32_t	gettime;
	stru_mole2_user_pet_info	pet_info;
	uint32_t	skillcnt;
	stru_mole2_pet_skill_info	pet_skill[10];
}__attribute__((packed));

typedef user_task_reward_set_in_header user_task_reward_set_out_header;

typedef stru_mole2_user_attire_info_1 user_task_reward_set_out_item_1; // attire

typedef stru_mole2_user_item_info user_task_reward_set_out_item_2; //item

typedef user_task_reward_set_in_item_3 user_task_reward_set_out_item_3; //pet

struct user_task_reward_set_in_item_6 {
	uint32_t		petid;
	uint32_t		pettype;
}__attribute__((packed));

typedef stru_count   user_attire_info_get_in_header;

struct  user_attire_info_get_in_item{
	uint32_t gettime;
}__attribute__((packed));

typedef stru_count   user_attire_info_get_out_header;

struct  user_attire_info_get_out_item{
	uint32_t gettime;
	uint32_t attire_id;
	uint32_t level;
}__attribute__((packed));

struct user_attire_sell_set_in_header{
	int32_t		xiaomee;
	uint32_t	count;
}__attribute__((packed));

struct user_attire_sell_set_out_header{
	stru_mole2_user_user_xiaomee_info xiaomee_info;
	uint32_t	count;
}__attribute__((packed));

struct user_attire_sell_set_out_item{
	uint32_t	gettime;
}__attribute__((packed));
	

struct  user_attire_sell_set_in_item{
	uint32_t gettime;
}__attribute__((packed));

struct  user_create_role_set_in_header{
	char		nick[NICK_LEN];
	uint32_t	prof;
	uint32_t	color;
	uint16_t	attribute_value[ATTRIBUTE_VALUE];
	uint32_t	hp;
	uint32_t 	mp;
	uint32_t	count;
}__attribute__((packed));

struct  user_create_role_set_out_header{
	stru_mole2_user_user_info	userinfo;
	uint32_t					count;
}__attribute__((packed));

typedef stru_mole2_user_attire_info_1 user_create_role_set_in_item;
typedef stru_mole2_user_attire_info_1 user_create_role_set_out_item;

typedef stru_mole2_user_levelup_info	user_user_levelup_set_in;

typedef stru_mole2_pet_levelup_info	user_pet_levelup_set_in;


struct  user_user_skill_up_set_in{
	uint32_t	skillid;
	uint32_t	experience;
	uint32_t	level;
}__attribute__((packed));

struct	mole2_user_user_set_hp_mp_in{
	int	hp;
	int	mp;
	uint32_t	injury_state;
} __attribute__((packed));

typedef mole2_user_user_set_hp_mp_in	mole2_user_user_set_hp_mp_out;

struct  mole2_user_task_item_add_in{
	uint32_t	taskid;
	uint32_t	itemid;
	uint32_t	itemtype;
	int32_t		count;
	uint32_t	expiretime;
}__attribute__((packed));

struct  mole2_user_task_item_add_out{
	uint32_t	taskid;
	uint32_t	itemid;
	uint32_t	itemtype;
	uint32_t	count;
	uint32_t	expiretime;
}__attribute__((packed));

struct  mole2_user_task_item_del_in{
	uint32_t	taskid;
}__attribute__((packed));

typedef stru_count   mole2_user_task_item_get_list_out_header;
typedef mole2_user_task_item_add_out \
			mole2_user_task_item_get_list_out_item;

typedef stru_count   user_user_skill_list_levelup_set_in_header;

struct  user_user_skill_list_levelup_set_in_item{
	uint32_t	skillid;
	uint32_t	level;
	uint32_t	exp;
}__attribute__((packed));

struct  user_user_flag_set_in{
	uint32_t	flag;
	uint32_t	flag_ex;
}__attribute__((packed));

struct user_user_flag_set_out{
	uint32_t	flag;
	uint32_t	flag_ex;
}__attribute__((packed));

struct  rand_item_info_check_in{
	uint32_t	randid;
	uint32_t	date_type;
	uint32_t	max_count;
}__attribute__((packed));

struct  rand_item_info_check_out{
	uint32_t	randid;
	uint32_t 	time;
	uint32_t	count;
}__attribute__((packed));


struct  rand_item_info_set_in{
	uint32_t	randid;
}__attribute__((packed));

typedef  mole2_user_user_login_ex_out_item_4 mole2_user_attire_update_gridid_list_out_item_2;

struct  user_pet_all_get_in{
	uint32_t gettime;
}__attribute__((packed));

struct user_pet_all_get_out_header{
	stru_mole2_user_pet_info_1 pet_info_ex;
	uint32_t count;
}__attribute__((packed));

typedef stru_mole2_pet_skill_info user_pet_all_get_out_item;

struct  user_user_medicine_cure_in{
	uint32_t hp;
	uint32_t mp;
	uint32_t itemid;
}__attribute__((packed));

typedef	user_user_medicine_cure_in	user_user_medicine_cure_out;

struct  user_pet_location_set_in{
	uint32_t	petid;
	uint32_t		location;	
}__attribute__((packed));


struct user_pet_location_set_out_header{
	stru_mole2_user_pet_info_1 pet_info_ex;
	uint32_t	count;
}__attribute__((packed));

typedef stru_mole2_pet_skill_info user_pet_location_set_out_item;

struct  user_user_note_dream_set_in{
	uint8_t		profession;
}__attribute__((packed));

struct user_user_note_dream_set_out{
	uint8_t		profession;
}__attribute__((packed));

typedef stru_mole2_user_pet_info_2 user_pet_catch_set_in_header;


typedef stru_mole2_pet_skill_info user_pet_catch_set_in_item;

typedef stru_mole2_user_pet_info_2 user_pet_catch_set_out_header;

typedef stru_mole2_pet_skill_info user_pet_catch_set_out_item;


typedef stru_count   user_pet_list_all_get_out_header;

typedef stru_mole2_user_pet_info_2	user_pet_list_all_get_out_item_1;

typedef stru_mole2_pet_skill_info	user_pet_list_all_get_out_item_2;

struct  activation_code_check_in{
	char acode[ACTIVATION_CODE_LEN];
}__attribute__((packed));

typedef stru_count   user_map_flight_list_get_out_header;

struct  user_map_flight_list_get_out_item{
	uint32_t mapid;
	uint32_t flight;
}__attribute__((packed));

struct  mole2_user_map_flight_check_in{
	uint32_t mapid;
}__attribute__((packed));

struct  mole2_user_map_flight_check_out{
	uint32_t mapid;
}__attribute__((packed));

struct  user_map_flight_set_in{
	uint32_t mapid;
}__attribute__((packed));

struct  user_logout_position_set_in{
	uint32_t time;
	uint16_t axis_x;
	uint16_t axis_y;
	uint32_t mapid;
	uint32_t maptype;
	uint32_t fly_mapid;
	uint32_t last_online;
}__attribute__((packed));

struct user_handbook_list_get_out_header{
	uint32_t item_cnt;
	uint32_t monster_cnt;
}__attribute__((packed));

typedef stru_mole2_item_handbook_info user_handbook_list_get_out_item_1;

typedef stru_mole2_monster_handbook_info user_handbook_list_get_out_item_2;

struct  item_handbook_state_set_in{
	uint32_t handbookid;
	uint32_t state;
	uint32_t itemid;
	int32_t  count;
}__attribute__((packed));

struct item_handbook_state_set_out {
	uint32_t itemid;
}__attribute((packed));

struct  monster_handbook_state_set_in{
	uint32_t handbookid;
	uint32_t state;
	uint32_t itemid;
	int32_t	 count;
}__attribute__((packed));

struct monster_handbook_state_set_out{
	uint32_t	itemid;
}__attribute__((packed));

struct  item_handbook_count_add_in{
	uint32_t itemid;
	uint32_t count;
}__attribute__((packed));

typedef stru_count   monster_handbook_count_list_add_in_header;

struct  monster_handbook_count_list_add_in_item{
	uint32_t monsterid;
	uint32_t count;
}__attribute__((packed));

typedef stru_count   user_title_list_get_out_header;

typedef stru_mole2_user_title_info  user_title_list_get_out_item;


typedef stru_mole2_user_title_info user_honor_set_in;

typedef stru_mole2_user_title_info user_honor_set_out;

struct  user_pet_follow_set_in{
	uint32_t petid;
}__attribute__((packed));

struct  user_pet_follow_set_out{
	uint32_t petid;
}__attribute__((packed));

struct  user_map_boss_position_set_in{
	uint32_t mapid;
}__attribute__((packed));

typedef stru_count   user_map_boss_position_list_get_out_header;

struct  user_map_boss_position_list_get_out_item{
	uint32_t mapid;
}__attribute__((packed));

struct  user_task_optdate_get_in{
	uint32_t taskid;
}__attribute__((packed));

struct  user_task_optdate_get_out{
	uint32_t taskid;
	uint32_t optdate;
	char content[NOTE_LEN];
}__attribute__((packed));

struct  tash_handbook_content_set_in{
	uint32_t taskid;
	char	content[NOTE_LEN];
}__attribute__((packed));
struct  tash_handbook_content_set_out{
	uint32_t	taskid;
}__attribute__((packed));


typedef stru_count   user_attire_list_duration_set_in_header;

struct  user_attire_list_duration_set_in_item{
	uint32_t attireid;
	uint16_t duration;
}__attribute__((packed));


struct user_attire_list_mend_set_in_header{
	int32_t		xiaomee;
	uint32_t 	count;
};

struct  user_attire_list_mend_set_in_item{
	uint32_t	attireid;
	uint16_t	mduration;
	uint16_t	duration;
}__attribute__((packed));

struct  user_title_delete_in{
	uint32_t	titleid;
}__attribute__((packed));

struct  pet_in_store_list_get_in{
	uint32_t	start;
	uint32_t	count;
}__attribute__((packed));

struct  pet_in_store_list_get_out_header{
	uint32_t	start;
	uint32_t    count;
	uint32_t	total;
}__attribute__((packed));

struct  pet_in_store_list_get_out_item{
	uint32_t	petid;
	uint32_t	pettype;
	uint8_t		race;
	char		nick[NICK_LEN];
	uint32_t	level;
	uint32_t	location;
}__attribute__((packed));

struct  pet_in_store_location_set_in{
	uint32_t	petid_1;
	uint32_t	location_1;
	uint32_t	petid_2;
	uint32_t	location_2;
}__attribute__((packed));

struct  user_item_storage_set_in{
	uint32_t	itemid;
	int32_t		count;	
}__attribute__((packed));

typedef user_item_storage_set_in  user_item_storage_set_out;

typedef stru_count   user_item_in_storage_list_get_out_header;

typedef stru_mole2_user_item_info user_item_in_storage_list_get_out_item;

struct user_attire_in_storage_list_get_in{
	uint32_t start;
	uint32_t count;
}__attribute__((packed));

struct user_attire_in_storage_list_get_out_header{
	uint32_t total;
	uint32_t start;
	uint32_t count;
}__attribute__((packed));

typedef stru_mole2_user_attire_info_1 user_attire_in_storage_list_get_out_item;

typedef stru_mole2_user_battle_info_1  user_battle_info_set_in;

struct  home_info_get_in{
	uint32_t	homeid;
}__attribute__((packed));



struct home_info_get_out{
	stru_mole2_home_info_t	home_info;
	uint32_t expbox;
	uint32_t joblevel;
}__attribute__((packed));

typedef stru_mole2_user_item_info  item_storage_add_in;

struct mole2_user_other_user_info_out_header{
	stru_mole2_user_user_info user_info;
	mole2_user_vip_info		vipinfo;
	uint32_t	all_score;
	uint32_t	attire_cnt;
}__attribute__((packed));

typedef stru_mole2_user_attire_info_1 mole2_user_other_user_info_out_item;

typedef stru_mole2_mail_info	user_mail_send_set_in;

typedef stru_count   user_mail_simple_list_get_out_header;

struct  user_mail_simple_list_get_out_item{
	uint32_t	mailid;
	uint32_t	flag;
	uint32_t	type;
	uint32_t	themeid;
	uint32_t	senderid;
	char		sender_nick[MAIL_SEND_NICK_LEN];
	char		title[MAIL_TITLE_LEN];
}__attribute__((packed));


struct user_exp_info_set_in{
	stru_mole2_pet_levelup_info pet_levelup_info;
	uint32_t	expbox;
}__attribute__((packed));

typedef user_exp_info_set_in  user_exp_info_set_out;

struct  user_mail_content_get_in{
	uint32_t	mailid;
}__attribute__((packed));

struct  user_mail_content_get_out{
	uint32_t	mailid;
	char		message[MAIL_MESSAGE_LEN];
	char		item_list[MAIL_ITEM_LIST_MAX];
	char		id_list[MAIL_ID_LIST_LEN];
}__attribute__((packed));

typedef stru_count   user_mail_list_del_in_header;

struct  user_mail_list_del_in_item{
	uint32_t	mailid;
}__attribute__((packed));

typedef stru_count   user_mail_list_del_out_header;

typedef user_mail_list_del_in_item user_mail_list_del_out_item;

struct  user_mail_flag_set_in{
	uint32_t	mailid;
	uint32_t	flag;
}__attribute__((packed));

typedef user_mail_flag_set_in user_mail_flag_set_out;

typedef stru_mole2_task_info mole2_task_set_in;

typedef stru_mole2_task_info mole2_task_set_out;

struct  user_energy_set_in{
	uint32_t energy;
}__attribute__((packed));

struct  user_energy_set_out{
	uint32_t energy;
}__attribute__((packed));

struct  mole2_task_info_get_in{
	uint32_t taskid;
}__attribute__((packed));

typedef stru_mole2_task_info  mole2_task_info_get_out;

struct mole2_task_info_list_get_out_header{
	uint32_t	on_cnt;
	uint32_t	other_cnt;
}__attribute__((packed));

typedef stru_mole2_task_info  mole2_task_info_list_get_out_item_1;

typedef stru_mole2_task_info_simple mole2_task_info_list_get_out_item_2;

typedef user_task_reward_set_in_header user_task_reward_set_cp_in_header;

struct  mole2_task_ser_buf_set_in{
	uint32_t	taskid;
	char ser_buf[TASK_SVR_BUF_LEN];
}__attribute__((packed));

typedef mole2_task_ser_buf_set_in  mole2_task_ser_buf_set_out;

struct  rand_item_present_set_in{
	char	strcode[PRESENT_STRCODE_LEN];
}__attribute__((packed));

struct  rand_item_present_set_out{
	char	strcode[PRESENT_STRCODE_LEN];
}__attribute__((packed));

struct  mole2_task_delete_in{
	uint32_t	taskid;
}__attribute__((packed));

struct  mole2_task_delete_out{
	uint32_t	taskid;
}__attribute__((packed));

struct  rand_item_activation_set_in{
	char	sz_code[ACTIVATION_STRCODE_LEN];
}__attribute__((packed));

typedef rand_item_activation_set_in  rand_item_activation_set_out;

struct  mole2_home_flag_set_in{
	uint32_t	index;
	uint32_t	flag;
}__attribute__((packed));

struct  mole2_home_flag_set_out{
	uint32_t	flag;
}__attribute__((packed));

struct  set_winbossid_in{
	uint32_t bossid;
}__attribute__((packed));

struct  set_ban_flag_in{
	uint32_t ban_flag;
}__attribute__((packed));

struct  user_swap_pets_in_header{
	uint32_t del_count;
	uint32_t add_count;
}__attribute__((packed));
struct  user_swap_pets_in_item_1{
	uint32_t petid;
	uint32_t pettype;
	uint32_t location;
}__attribute__((packed));
struct  user_swap_pets_in_item_2{
	uint32_t petid;
	mole2_user_pet_add_in pet_info;
	uint32_t skill_cnt;
	stru_mole2_pet_skill_info skill[10];
}__attribute__((packed));
typedef user_swap_pets_in_header user_swap_pets_out_header;
typedef user_swap_pets_in_item_1 user_swap_pets_out_item_1;
typedef user_swap_pets_in_item_2 user_swap_pets_out_item_2;

struct  mole2_user_get_invited_out{
	uint32_t invited_count;
	uint32_t already_times;
}__attribute__((packed));

typedef mole2_user_get_invited_out mole2_user_add_invited_out;

struct  set_title_and_job_level_in{
	uint32_t title_old;
	uint32_t title_new;
	uint32_t time;
	uint32_t job_level;
}__attribute__((packed));

struct  proto_get_expbox_out{
	uint32_t expbox;
}__attribute__((packed));
struct  proto_set_expbox_in{
	uint32_t expbox;
}__attribute__((packed));

typedef stru_count   mole2_user_pet_get_in_store_list_out_header;
typedef stru_mole2_user_pet_info_1 mole2_user_pet_get_in_store_list_out_item;
typedef stru_mole2_user_pet_info_1 mole2_user_pet_set_in;
typedef user_pet_follow_set_in mole2_user_pet_delete_in;

struct  mole2_user_get_day_list_out_header {
	uint32_t	count;
}__attribute__((packed));

struct  mole2_user_get_day_list_out_item{
	uint32_t ssid;
	uint32_t total;
	uint32_t count;
}__attribute__((packed));

struct  mole2_user_get_max_list_out_item{
	uint32_t 	type;
	uint32_t 	id;
	uint32_t 	count;
}__attribute__((packed));


struct  item_handbook_state_get_in{
	uint32_t itemid;
}__attribute__((packed));
struct  item_handbook_state_get_out{
	uint32_t itemid;
	uint32_t state;
}__attribute__((packed));

struct  user_get_ban_flag_out{
	uint32_t flag;
}__attribute__((packed));

struct day_add_ssid_count_in{
	uint32_t ssid;
	uint32_t count;
	uint32_t limit;
}__attribute__((packed));

struct  day_get_ssid_info_in{
	uint32_t ssid;
}__attribute__((packed));

struct  day_get_ssid_info_out{
	uint32_t total;
	uint32_t count;
}__attribute__((packed));

struct  day_sub_ssid_total_in{
	uint32_t ssid;
	uint32_t count;
}__attribute__((packed));

struct  mail_get_post_items_in{
	uint32_t mailid;
	uint32_t isrecv;
}__attribute__((packed));

struct  monster_handbook_range_get_in{
	uint32_t min;
	uint32_t max;
}__attribute__((packed));
typedef stru_count   monster_handbook_range_get_out_header;
typedef stru_mole2_monster_handbook_info  monster_handbook_range_get_out_item;

struct  mole2_user_get_nick_out{
	char nick[16];
}__attribute__((packed));


struct	set_args_in{
	uint32_t key;
	uint32_t value;
}__attribute__((packed));

struct	get_args_in{
	uint32_t min;
	uint32_t max;
}__attribute__((packed));

typedef stru_count   get_args_out_header;

struct  get_args_out_item{
	uint32_t key;
	uint32_t value;
}__attribute__((packed));

#endif
