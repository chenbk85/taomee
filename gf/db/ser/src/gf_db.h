#ifndef  GF_DB_H
#define  GF_DB_H
#include "gf.h"

class finish_stage_list_item: public Cmessage {
	public:
		/*关卡编号*/
		uint32_t		stage_id;
		/*存放对应关卡通关得分(1:S,2:A…6:E )*/
		uint8_t		grade[4]; 

		finish_stage_list_item();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class friend_list_item_t: public Cmessage {
	public:
		/*好友ID*/
		uint32_t		friend_id;
		/*1:好友; 2:黑名单; 3:师傅;4:徒弟;5:丈夫;6:妻子;*/
		uint32_t		friend_type;

		friend_list_item_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_add_attire_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;
		/*装备类型ID*/
		uint32_t		attireid;
		/*装备获取时间*/
		uint32_t		gettime;
		/*0：没穿，在包裹里，1：穿在身上, 2:超过使用期限的装备，*/
		uint32_t		usedflag;
		/*耐久值*/
		uint32_t		duration;
		/*装备的等级。(取值范围:0-12)(饰品没有等级概念)*/
		uint32_t		attire_lv;
		/*背包最大格子数，默认是50*/
		uint32_t		max_bag_count;

		gf_add_attire_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_add_friend_kf_in: public Cmessage {
	public:
		/*好友ID*/
		uint32_t		friend_id;

		gf_add_friend_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_add_item_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;
		/*物品类型ID*/
		uint32_t		item_id;
		/*物品数量*/
		uint32_t		add_num;
		/*背包格子数，默认值50*/
		uint32_t		max_bag_count;

		gf_add_item_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_add_killed_boss_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;
		/*boss的ID*/
		uint32_t		boss_id;

		gf_add_killed_boss_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_add_role_kf_in: public Cmessage {
	public:
		/*角色类型*/
		uint32_t		role_type;
		/*等级*/
		uint32_t		level;
		/*血量(实际不需要填)*/
		uint32_t		hp;
		/*魔力值(实际不需要)*/
		uint32_t		mp;
		/*昵称*/
		char		nick[16]; 
		/*邀请者的ID*/
		uint32_t		parentid;

		gf_add_role_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_add_role_kf_out: public Cmessage {
	public:
		/*当前玩家的角色数量*/
		uint32_t		role_num;
		/*玩家是否存在*/
		uint32_t		user_existed;

		gf_add_role_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_add_skill_with_no_book_kf_in: public Cmessage {
	public:
		/*技能id*/
		uint32_t		skill_id;
		/*技能等级*/
		uint32_t		skill_lv;
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_add_skill_with_no_book_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_add_skill_with_no_book_kf_out: public Cmessage {
	public:
		/*技能id*/
		uint32_t		skill_id;

		gf_add_skill_with_no_book_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_del_attire_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;
		/*装备类型ID*/
		uint32_t		attire_id;
		/*装备唯一ID*/
		uint32_t		index_id;

		gf_del_attire_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_del_attire_kf_out: public Cmessage {
	public:
		/*装备类型ID*/
		uint32_t		attire_id;
		/*装备唯一ID*/
		uint32_t		index_id;

		gf_del_attire_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_del_friend_kf_in: public Cmessage {
	public:
		/*好友ID*/
		uint32_t		friend_id;

		gf_del_friend_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_del_item_by_num_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;
		/*物品类型ID*/
		uint32_t		item_id;
		/*物品数量*/
		uint32_t		del_num;

		gf_del_item_by_num_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_del_item_by_num_kf_out: public Cmessage {
	public:
		/*物品类型ID*/
		uint32_t		item_id;
		/*物品数量*/
		uint32_t		del_num;

		gf_del_item_by_num_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_del_role_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_del_role_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_del_skill_kf_in: public Cmessage {
	public:
		/*技能id*/
		uint32_t		skill_id;
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_del_skill_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_all_task_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_get_all_task_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class task_list_t: public Cmessage {
	public:
		/*任务ID*/
		uint32_t		taskid;
		/*1：正在做(如果没有会insert)，3：做完，0：既删除*/
		uint32_t		task_flag;
		/**/
		uint32_t		task_tm;

		task_list_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_all_task_kf_out: public Cmessage {
	public:
		//任务列表
		std::vector<task_list_t>		task_list;

		gf_get_all_task_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_clothes_list_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_get_clothes_list_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class role_clothes_list_item: public Cmessage {
	public:
		/*装备类型ID*/
		uint32_t		attireid;
		/*装备的唯一id数据产生*/
		uint32_t		id;
		/*0：没穿，在包裹里，1：穿在身上, 2:超过使用期限的装备， 0xffffffff(包裹穿在身上的和背包中的)*/
		uint32_t		usedflag;
		/*耐久值*/
		uint32_t		duration;

		role_clothes_list_item();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_clothes_list_kf_out: public Cmessage {
	public:
		//角色装备信息列表
		std::vector<role_clothes_list_item>		role_clothse_list;

		gf_get_clothes_list_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_friend_list_type_kf_in: public Cmessage {
	public:
		/*1:好友; 2:黑名单; 3:师傅;4:徒弟;5:丈夫;6:妻子;*/
		uint32_t		friend_type;

		gf_get_friend_list_type_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_friend_list_type_kf_out: public Cmessage {
	public:
		//
		std::vector<friend_list_item_t>		friend_list;

		gf_get_friend_list_type_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_info_for_login_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_get_info_for_login_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_info_for_login_kf_out: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;
		/*角色类型*/
		uint32_t		role_type;
		/*无意义*/
		uint32_t		flag;
		/*账号注册时间*/
		uint32_t		regtime;
		/*昵称*/
		char		nick[16]; 
		/*vip用户标识(0:未开通vip用户，2：曾经是vip的用户，3：当前是vip的用户)*/
		uint32_t		vip;
		/*用户目前总包月月数(only for vip用户)*/
		uint32_t		vip_month_cnt;
		/*开始时间(only for vip用户)*/
		uint32_t		start_time;
		/*结束时间(only for vip用户)*/
		uint32_t		end_time;
		/*侠士值(only for vip用户)*/
		uint32_t		x_value;
		/*游戏中赠送的侠士值*/
		uint32_t		sword_value;
		/*为vip用户扩展的仓库(无意义)*/
		uint32_t		vip_exwarehouse;
		/*技能点*/
		uint32_t		skill_point;
		/*地图id（玩家当前所在）*/
		uint32_t		map_id;
		/*x轴(坐标点)*/
		uint32_t		xpos;
		/*y轴(坐标点)*/
		uint32_t		ypos;
		/*等级*/
		uint32_t		level;
		/*经验*/
		uint32_t		exp;
		/*经验分配器中的经验*/
		uint32_t		alloter_exp;
		/*血量(实际不需要填)*/
		uint32_t		hp;
		/*魔力值(实际不需要)*/
		uint32_t		mp;
		/*功夫豆*/
		uint32_t		xiaomee;
		/*荣誉值*/
		uint32_t		honour;
		/*武勋值*/
		uint32_t		fight;
		/*胜利次数*/
		uint32_t		win;
		/*失败次数*/
		uint32_t		fail;
		/*连胜最大次数*/
		uint32_t		winning_streak;
		/*今天获得的伏魔点*/
		uint32_t		fumo_points_today;
		/*伏魔点*/
		uint32_t		fumo_points_total;
		/*记录通关伏魔塔的最高层数*/
		uint32_t		fumo_tower_top;
		/*记录通关伏魔塔的最高层数的通关时间*/
		uint32_t		fumo_tower_used_tm;
		/*在线总次数*/
		uint32_t		Ol_count;
		/*最后一次登陆的那天在线时长*/
		uint32_t		Ol_today;
		/*最后一次离线的时间*/
		uint32_t		Ol_last;
		/*在线总时长*/
		uint32_t		Ol_time;
		/**/
		uint8_t		uniqueitem[20]; 
		/*物品绑定信息*/
		uint8_t		itembind[40]; 
		/*大使任务标志位0--未接取；1--已接取；2--已完成*/
		uint8_t		amb_status;
		/*邀请者的ID*/
		uint32_t		parentid;
		/*该用户邀请的玩家个数*/
		uint32_t		child_cnt;
		/*该用户邀请的玩家个数达标个数*/
		uint32_t		achieve_cnt;
		/*(账号)领取奖励标志位*/
		uint8_t		flag_bit[40]; 
		/*可接活动条件 (vip活动专员)*/
		uint8_t		act_record[40]; 
		/*当天双倍经验剩余时间*/
		uint32_t		double_exp_time;
		/*当天完成任务标志*/
		uint32_t		day_flag;
		/*角色阅读过的时报最大章节*/
		uint32_t		max_times_chapter;
		/**/
		uint32_t		show_state;
		/*当天强化次数*/
		uint32_t		strengthen_cnt;
		/*成就点*/
		uint32_t		achieve_point;
		/*最后一次更新成就点时间*/
		uint32_t		last_update_tm;
		/*当前使用的称号*/
		uint32_t		achieve_title;
		/*禁止添加好友的标志位*/
		uint32_t		forbiden_add_friend_flag;

		gf_get_info_for_login_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_killed_boss_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_get_killed_boss_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_killed_boss_kf_out: public Cmessage {
	public:
		//
		std::vector<finish_stage_list_item>		kill_boss_stage_list;

		gf_get_killed_boss_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_material_list_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_get_material_list_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class item_list_t: public Cmessage {
	public:
		/*物品类型ID*/
		uint32_t		item_id;
		/*数量*/
		uint32_t		count;

		item_list_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_material_list_kf_out: public Cmessage {
	public:
		//材料列表
		std::vector<item_list_t>		material_list;

		gf_get_material_list_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class role_list_item: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_time;
		/*角色类型（1:猴子2:兔子3:熊猫4:龙人）*/
		uint32_t		role_type;
		/*角色等级*/
		uint32_t		level;
		/*0:正常;1:XXX 2:删除*/
		uint32_t		status;
		/*昵称*/
		char		nick[16]; 

		role_list_item();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_role_list_kf_out: public Cmessage {
	public:
		//角色列表信息
		std::vector<role_list_item>		role_list;

		gf_get_role_list_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_skill_list_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_get_skill_list_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class skill_list_t: public Cmessage {
	public:
		/*技能id*/
		uint32_t		skill_id;
		/*技能等级*/
		uint32_t		skill_lv;
		/*当技能累加的技能点*/
		uint32_t		skill_point;

		skill_list_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_skill_list_kf_out: public Cmessage {
	public:
		/*剩余技能点*/
		uint32_t		left_sp;
		//技能列表
		std::vector<skill_list_t>		skill_list;

		gf_get_skill_list_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_summon_list_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_get_summon_list_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class summon_list_t: public Cmessage {
	public:
		/*宠物获取时间*/
		uint32_t		mon_tm;
		/*宠物类型*/
		uint32_t		mon_type;
		/*宠物昵称*/
		char		mon_nick[16]; 
		/*经验*/
		uint32_t		exp;
		/*等级*/
		uint32_t		lv;
		/*战斗值*/
		uint32_t		fight_value;
		/*状态(0--未出战；1--出战 )*/
		uint32_t		status;

		summon_list_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_summon_list_kf_out: public Cmessage {
	public:
		//宠物列表
		std::vector<summon_list_t>		summon_list;

		gf_get_summon_list_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_user_base_info_kf_out: public Cmessage {
	public:
		/*无意义*/
		uint32_t		flag;
		/*时间*/
		uint32_t		regtime;
		/*0-正常标志位；1-因为名字不文明，被封号；2-说话不文明；3-索要个人隐私；4-使用外挂*/
		uint32_t		forbid_flag;
		/*1-封号24小时；2-封号7天；3-封号14天0xffffffff-永久封号*/
		uint32_t		deadline;
		/*vip用户标识(0:未开通vip用户，2：曾经是vip的用户，3：当前是vip的用户)*/
		uint32_t		vip;
		/*用户目前总包月月数(only for vip用户)*/
		uint32_t		vip_month_count;
		/*开始时间(only for vip用户)*/
		uint32_t		start_time;
		/*结束时间(only for vip用户)*/
		uint32_t		end_time;
		/*知否自动续费(only for vip用户)*/
		uint32_t		auto_incr;
		/*侠士值(only for vip用户)*/
		uint32_t		x_value;
		/*在线总次数*/
		uint32_t		Ol_count;
		/*最后一次登陆的那天在线时长*/
		uint32_t		Ol_today;
		/*最后一次离线的时间*/
		uint32_t		Ol_last;
		/*在线总时长*/
		uint32_t		Ol_time;
		/*大使任务标志位0--未接取；1--已接取；2--已完成*/
		uint32_t		amb_status;
		/*该用户的邀请者*/
		uint32_t		parentid;
		/*该用户邀请的玩家个数*/
		uint32_t		child_cnt;
		/*该用户邀请的玩家个数达标个数*/
		uint32_t		achieve_cnt;
		/*背包最大格子数，默认是50*/
		uint32_t		max_bag;

		gf_get_user_base_info_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_user_item_list_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_get_user_item_list_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_user_item_list_kf_out: public Cmessage {
	public:
		//物品列表
		std::vector<item_list_t>		item_list;

		gf_get_user_item_list_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_get_user_vip_kf_out: public Cmessage {
	public:
		/*vip用户标识(0:未开通vip用户，2：曾经是vip的用户，3：当前是vip的用户)*/
		uint32_t		vip_is;
		/*侠士值(only for vip用户)*/
		uint32_t		x_value;
		/*vip等级*/
		uint32_t		vip_lv;
		/*用户目前总包月月数(only for vip用户)*/
		uint32_t		vip_month_cnt;
		/*开始时间(only for vip用户)*/
		uint32_t		start_tm;
		/*结束时间(only for vip用户)*/
		uint32_t		end_tm;
		/*知否自动续费(only for vip用户)*/
		uint32_t		auto_incr;
		/*充值渠道(only for vip用户)*/
		uint32_t		method;

		gf_get_user_vip_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_kick_user_offline_kf_in: public Cmessage {
	public:
		/*消息接收着得id(被踢下线的用户)*/
		uint32_t		recvid;

		gf_kick_user_offline_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_official_notice_kf_in: public Cmessage {
	public:
		/*公告的唯一标识*/
		uint32_t		index;
		/*1:即时公告2：循环公告*/
		uint32_t		type;
		/*预留字段(填1)*/
		uint32_t		frontid;
		/*消息的内容*/
		uint8_t		msg[500]; 

		gf_official_notice_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_post_msg_kf_in: public Cmessage {
	public:
		/*消息接收着得id*/
		uint32_t		recvid;
		/*npc的ID(默认值为0)*/
		uint32_t		npc_type;
		/*发送消息时间*/
		uint32_t		msg_tm;
		/*消息内容的长度*/
		uint32_t		msglen;
		//消息的内容
		std::vector<uint8_t>		msg;

		gf_post_msg_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_set_account_forbid_kf_in: public Cmessage {
	public:
		/*0-正常标志位；1-因为名字不文明，被封号；2-说话不文明；3-索要个人隐私；4-使用外挂*/
		uint32_t		forbid_flag;
		/*1-封号24小时；2-封号7天；3-封号14天0xffffffff-永久封号*/
		uint32_t		deadline;

		gf_set_account_forbid_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_set_item_int_value_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;
		/*数据库列的名字 (需要知道数据库表结构)*/
		char		column_name[32]; 
		/*物品类型ID*/
		uint32_t		item_id;
		/*修改后的值*/
		uint32_t		value;

		gf_set_item_int_value_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_set_nick_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;
		/*昵称*/
		char		nick[16]; 
		/*物品ID，可以没有(默认值填0)*/
		uint32_t		use_item;

		gf_set_nick_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_set_nick_kf_out: public Cmessage {
	public:
		/*物品ID，可以没有(默认值为0)*/
		uint32_t		use_item;

		gf_set_nick_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_set_role_delflg_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_set_role_delflg_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_set_role_int_value_kf_in: public Cmessage {
	public:
		/*角色注册时间*/
		uint32_t		role_regtime;
		/*数据库列的名字 (需要知道数据库表结构)*/
		char		column_name[32]; 
		/*修改后的属性值*/
		uint32_t		value;

		gf_set_role_int_value_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_set_task_flg_kf_in: public Cmessage {
	public:
		/*任务ID*/
		uint32_t		taskid;
		/*1：正在做(如果没有会insert)，3：做完，0：既删除*/
		uint32_t		task_flag;
		/*角色注册时间*/
		uint32_t		role_regtime;

		gf_set_task_flg_kf_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class gf_set_task_flg_kf_out: public Cmessage {
	public:
		/*任务ID*/
		uint32_t		taskid;

		gf_set_task_flg_kf_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
#endif // GF_DB_H
