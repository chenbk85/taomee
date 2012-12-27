#ifndef  GF_DB_ENUM_H
#define  GF_DB_ENUM_H

/*命令定义*/	
enum  enum_gf_db_cmd{
	 gf_get_user_base_info_kf_cmd		=	0x2E01, /*得到用户基础信息*/
	 gf_get_user_vip_kf_cmd		=	0x2E02, /*获取vip相关信息*/
	 gf_get_info_for_login_kf_cmd		=	0x2E03, /*拉取玩家基础信息*/
	 gf_get_role_list_kf_cmd		=	0x2E04, /*获取用户角色列表*/
	 gf_get_clothes_list_kf_cmd		=	0x2E05, /*拉取角色所有装备信息*/
	 gf_get_killed_boss_kf_cmd		=	0x2E06, /*获取玩家通关关卡的信息*/
	 gf_del_attire_kf_cmd		=	0x2E07, /*删除角色装备*/
	 gf_add_attire_kf_cmd		=	0x2E08, /*给角色添加装备*/
	 gf_add_role_kf_cmd		=	0x2E09, /*创建新的角色*/
	 gf_del_role_kf_cmd		=	0x2E0A, /*删除玩家角色*/
	 gf_add_item_kf_cmd		=	0x2E0B, /*添加道具物品*/
	 gf_set_role_delflg_kf_cmd		=	0x2E0C, /*设置角色可是被删除的标志（该接口暂时不可用）*/
	 gf_get_friend_list_type_kf_cmd		=	0x2E0D, /*获取朋友列表*/
	 gf_add_friend_kf_cmd		=	0x2E0E, /*添加好友*/
	 gf_del_friend_kf_cmd		=	0x2E0F, /*删除各类型的好友*/
	 gf_set_nick_kf_cmd		=	0x2E10, /*修改昵称*/
	 gf_set_role_int_value_kf_cmd		=	0x2E11, /*修改角色属性*/
	 gf_set_item_int_value_kf_cmd		=	0x2E12, /*修改物品的数量*/
	 gf_get_user_item_list_kf_cmd		=	0x2E13, /*获取玩家物品列表*/
	 gf_get_material_list_kf_cmd		=	0x2E14, /*得到材料道具类表*/
	 gf_del_item_by_num_kf_cmd		=	0x2E15, /*减少物品数量*/
	 gf_get_all_task_kf_cmd		=	0x2E16, /*获取所有任务列表*/
	 gf_add_killed_boss_kf_cmd		=	0x2E17, /*添加玩家杀死boss信息*/
	 gf_set_task_flg_kf_cmd		=	0x2E18, /*设置任务的状态*/
	 gf_get_skill_list_kf_cmd		=	0x2E19, /*获取技能列表*/
	 gf_add_skill_with_no_book_kf_cmd		=	0x2E1A, /*添加技能*/
	 gf_del_skill_kf_cmd		=	0x2E1B, /*删除技能*/
	 gf_get_summon_list_kf_cmd		=	0x2E1C, /*拉取召唤兽信息*/
	 gf_set_account_forbid_kf_cmd		=	0x2E1D, /*设置封号信息*/
	 gf_post_msg_kf_cmd		=	0xEA61, /*发送即时消息给用户(switch接口)*/
	 gf_kick_user_offline_kf_cmd		=	0xEA62, /*将用户踢下线(switch接口)*/
	 gf_official_notice_kf_cmd		=	0xEA68, /*发送官方消息的接口(switch接口)*/

};

/*错误码定义*/	
enum  enum_gf_db_error{
	 		=	0, /**/
	 数据库错误		=	1002, /**/
	 网络错误		=	1003, /**/
	 协议没有定义		=	1004, /**/
	 协议长度不对		=	1005, /**/
	 VALUE_OUT_OF_RANGE_ERR		=	1006, /*值越界*/
	 ROLE_ID_NOFIND_ERR		=	1106, /*角色不存在*/
	 ROLE_ID_EXISTED_ERR		=	1107, /*角色已经存在*/
	 ROLE_LV_NOT_REACH_ERR		=	1108, /*角色等级不够*/
	 好友已经存在		=	3103, /**/
	 好友不存在		=	3104, /**/
	 不能添加自己好友		=	3106, /**/
	 物品已经存在		=	3120, /**/
	 物品没有找到		=	3121, /**/
	 物品数量不足		=	3122, /**/
	 物品种类太多		=	3123, /**/
	 任务已经存在		=	3130, /**/
	 任务不存在		=	3131, /**/
	 杀死boss信息已经存在		=	3132, /**/
	 没有找到杀死boss信息		=	3133, /**/
	 技能已存在		=	3140, /**/
	 技能不存在		=	3141, /**/
	 技能点不够		=	3142, /**/
	 召唤兽已经存在		=	3160, /**/
	 没有发现召唤兽		=	3161, /**/
	 角色数量已达到上限		=	3300, /**/

};



#endif // GF_DB_ENUM_H
