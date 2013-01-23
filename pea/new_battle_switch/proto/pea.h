#ifndef  PEA_H
#define  PEA_H
#include <libtaomee++/proto/proto_base.h>
#include <stdio.h>

class uid_role_t: public Cmessage {
	public:
		/*米米号*/
		uint32_t		user_id;
		/*角色创建时间*/
		uint32_t		role_tm;

		uid_role_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class room_info_t: public Cmessage {
	public:
		/*房间的id*/
		uint32_t		room_id;
		/*房间名字*/
		char		room_name[16]; 
		/*房间密码*/
		char		password[16]; 
		/*房间类型(0:pvp, 1:pve)*/
		uint8_t		room_type;
		/*房间对战模式 (0: free(svr不干预), 1: compete(svr帮助匹配))*/
		uint8_t		room_mode;
		/*副本地图id*/
		uint32_t		map_id;
		/*副本难度*/
		uint8_t		map_lv;

		room_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class model_info_t: public Cmessage {
	public:
		/*眼部模型*/
		uint32_t		eye_model;
		/*裸模ID*/
		uint32_t		resource_id;

		model_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class room_pet_info_t: public Cmessage {
	public:
		/*精灵id*/
		uint32_t		pet_id;
		/*精灵等级*/
		uint32_t		pet_level;
		/*默认技能*/
		uint32_t		skill_1;
		/**/
		uint32_t		skill_2;
		/**/
		uint32_t		skill_3;
		/*绝招*/
		uint32_t		uni_skill;

		room_pet_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class simple_equip_info_t: public Cmessage {
	public:
		/*装备ID*/
		uint32_t		equip_id;

		simple_equip_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class room_player_show_info_t: public Cmessage {
	public:
		/*米米号*/
		uint32_t		uid;
		/*角色创建时间*/
		uint32_t		role_tm;
		/*队伍编号*/
		uint8_t		team_id;
		/*座位编号*/
		uint8_t		seat_id;
		/*玩家状态(0: 没准备好, 1:准备好了)*/
		uint8_t		player_status;
		/*名字*/
		char		nick[16]; 
		/*模型*/
		model_info_t		model_info;
		/*房间中玩家的宠物信息*/
		room_pet_info_t		pet_info;
		//装备信息
		std::vector<simple_equip_info_t>		equip_info;

		room_player_show_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class room_full_info_t: public Cmessage {
	public:
		/*房主的id*/
		uint32_t		owner_uid;
		/*房主的创建角色时间*/
		int32_t		owner_role_tm;
		/*房间基本信息*/
		room_info_t		room_info;
		//房间中玩家具体信息
		std::vector<room_player_show_info_t>		player_info;

		room_full_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class room_list_info_t: public Cmessage {
	public:
		/*房间的id*/
		uint32_t		room_id;
		/*地图ID*/
		uint32_t		map_id;
		/*房间名字*/
		char		room_name[16]; 
		/*房间类型(0:pvp, 1:pve)*/
		uint8_t		room_type;
		/*房间模式(0: 自由(svr不干预), 1: 竞技(svr帮助配对))*/
		uint8_t		room_mode;
		/*是否加密，1加密，0未加密*/
		uint8_t		encrypted_flag;
		/*0:空状态; 1:寻找匹配状态; 2: 对战状态*/
		uint8_t		room_status;
		/*房间中当前的人数*/
		uint8_t		player_count;
		/*房间的最大人数*/
		uint8_t		max_count;

		room_list_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btl_pet_info_t: public Cmessage {
	public:
		/*精灵编号*/
		uint32_t		pet_no;
		/*精灵id*/
		uint32_t		pet_id;
		/*级别*/
		uint32_t		level;
		/*经验*/
		uint32_t		exp;
		/*品质*/
		uint32_t		quality;
		/*1出战，2辅助，3背包*/
		uint32_t		status;
		/**/
		uint32_t		width;
		/**/
		uint32_t		height;

		btl_pet_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btl_player_statistical_info_t: public Cmessage {
	public:
		/*米米号*/
		uint32_t		uid;
		/*队伍*/
		uint32_t		team;
		/*名字*/
		char		nick[16]; 
		/*命中率分子*/
		uint32_t		hit_rate;
		/*伤害总值*/
		uint32_t		damage;
		/*经验*/
		uint32_t		exp;
		/*额外经验*/
		uint32_t		extern_exp;

		btl_player_statistical_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btl_skill_info_t: public Cmessage {
	public:
		/*技能ID*/
		uint32_t		skill_id;
		/*技能级别*/
		uint32_t		skill_lv;

		btl_skill_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_add_item_reply_t: public Cmessage {
	public:
		/*物品索引ID*/
		uint32_t		id;
		/*格子位置*/
		uint32_t		grid_index;
		/*物品ID*/
		uint32_t		item_id;
		/*数量*/
		uint32_t		item_count;
		/*获得时间*/
		uint32_t		get_time;
		/*过期时间戳*/
		uint32_t		expire_time;

		db_add_item_reply_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_add_item_request_t: public Cmessage {
	public:
		/*格子位置*/
		uint32_t		grid_index;
		/*物品ID*/
		uint32_t		item_id;
		/*数量*/
		uint32_t		item_count;
		/*获得时间*/
		uint32_t		get_time;
		/*过期时间戳*/
		uint32_t		expire_time;

		db_add_item_request_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_change_pet_status_t: public Cmessage {
	public:
		/*1出战，2辅助*/
		uint32_t		status;
		/*原先的精灵编号*/
		uint32_t		src_pet_no;
		/*目标的精灵编号*/
		uint32_t		des_pet_no;

		db_change_pet_status_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_del_item_reply_t: public Cmessage {
	public:
		/*格子位置*/
		uint32_t		grid_index;
		/*物品ID*/
		uint32_t		item_id;
		/*数量*/
		uint32_t		item_count;

		db_del_item_reply_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_del_item_request_t: public Cmessage {
	public:
		/*格子位置*/
		uint32_t		grid_index;
		/*物品ID*/
		uint32_t		item_id;
		/*数量*/
		uint32_t		item_count;

		db_del_item_request_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_equip_info_t: public Cmessage {
	public:
		/*物品索引ID*/
		uint32_t		id;
		/*物品ID*/
		uint32_t		item_id;
		/*数量*/
		uint32_t		item_count;
		/*格子位置*/
		uint32_t		grid_index;
		/*获得时间*/
		uint32_t		get_time;
		/*过期时间戳*/
		uint32_t		expire_time;
		/*装备是否隐藏0不隐藏，1隐藏*/
		uint8_t		hide;

		db_equip_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_extra_info_t: public Cmessage {
	public:
		/*信息的id*/
		uint32_t		info_id;
		/*信息的值*/
		uint32_t		info_value;

		db_extra_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_item_info_t: public Cmessage {
	public:
		/*物品索引ID*/
		uint32_t		id;
		/*物品ID*/
		uint32_t		item_id;
		/*数量*/
		uint32_t		item_count;
		/*格子位置*/
		uint32_t		grid_index;
		/*获得时间*/
		uint32_t		get_time;
		/*过期时间戳*/
		uint32_t		expire_time;

		db_item_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_mail_head_info_t: public Cmessage {
	public:
		/*邮件ID*/
		uint32_t		mail_id;
		/*邮件时间*/
		uint32_t		mail_time;
		/*邮件状态*/
		uint32_t		mail_state;
		/*邮件模板*/
		uint32_t		mail_templet;
		/*邮件类型*/
		uint32_t		mail_type;
		/*发送人米米号*/
		uint32_t		sender_id;
		/*发件人ROLE_TM*/
		uint32_t		sender_role_tm;
		/*发件人的名字*/
		uint8_t		sender_nick[16]; 
		/*邮件标题*/
		uint8_t		mail_title[50]; 

		db_mail_head_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_pet_gift_info_t: public Cmessage {
	public:
		/*天赋位置编号*/
		uint32_t		gift_no;
		/*天赋id*/
		uint32_t		gift_id;
		/*天赋等级*/
		uint32_t		gift_level;

		db_pet_gift_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_pet_info_t: public Cmessage {
	public:
		/*精灵编号*/
		uint32_t		pet_no;
		/*精灵id*/
		uint32_t		pet_id;
		/*级别*/
		uint32_t		level;
		/*经验*/
		uint32_t		exp;
		/*品质*/
		uint32_t		quality;
		/*IQ*/
		uint32_t		iq;
		/*1出战，2辅助，3背包*/
		uint32_t		status;
		/*随机成长属性*/
		uint32_t		rand_attr[4]; 
		/*培养获得的属性值*/
		uint32_t		train_attr[4]; 
		/*未保存的培养属性值*/
		uint32_t		try_train_attr[4]; 
		//天赋
		std::vector<db_pet_gift_info_t>		gift;

		db_pet_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_player_info: public Cmessage {
	public:
		/*米米号*/
		uint32_t		user_id;
		/*角色创建时间*/
		uint32_t		role_tm;
		/*名字*/
		char		nick[16]; 
		/*模型*/
		model_info_t		model;
		/*区号*/
		uint32_t		server_id;
		/*最后登录时间*/
		uint32_t		last_login_tm;
		/*最后下线时间*/
		uint32_t		last_off_line_tm;
		/*经验*/
		uint32_t		exp;
		/*级别*/
		uint32_t		level;
		/*包裹的格子数*/
		uint32_t		max_bag_grid_count;
		/*地图ID*/
		uint32_t		map_id;
		/*X坐标*/
		uint32_t		map_x;
		/*Y坐标*/
		uint32_t		map_y;
		/*钱*/
		uint32_t		gold;
		/*禁加好友标志(0可加，1不可加)*/
		uint32_t		forbid_friends_me;

		db_player_info();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_prize_t: public Cmessage {
	public:
		/*奖励ID*/
		uint32_t		prize_id;
		/*数量*/
		uint32_t		count;

		db_prize_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_update_pet_gift_t: public Cmessage {
	public:
		/*天赋位置编号*/
		uint32_t		gift_no;
		/*天赋id*/
		uint32_t		gift_id;
		/*天赋等级*/
		uint32_t		gift_level;

		db_update_pet_gift_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class db_user_id_t: public Cmessage {
	public:
		/*用户id*/
		uint32_t		user_id;
		/*角色创建时间*/
		uint32_t		role_tm;
		/*区号*/
		uint32_t		server_id;

		db_user_id_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class equip_change_t: public Cmessage {
	public:
		/*身上的索引*/
		uint32_t		body_index;
		/*包包位置*/
		uint32_t		empty_bag_index;
		/*装备是否隐藏0不隐藏，1隐藏*/
		uint8_t		hide;

		equip_change_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class equip_enclosure_info_t: public Cmessage {
	public:
		/*装备ID*/
		uint32_t		equip_id;
		/**/
		uint32_t		equip_count;
		/**/
		uint32_t		duration_time;
		/**/
		uint32_t		end_time;

		equip_enclosure_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class item_enclosure_info_t: public Cmessage {
	public:
		/*物品ID*/
		uint32_t		item_id;
		/*数量*/
		uint32_t		item_count;
		/**/
		uint32_t		duration_time;
		/**/
		uint32_t		end_time;

		item_enclosure_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class login_player_info_t: public Cmessage {
	public:
		/*米米号*/
		uint32_t		user_id;
		/*角色创建时间*/
		uint32_t		role_tm;
		/*名字*/
		char		nick[16]; 
		/*模型*/
		model_info_t		model;
		/*区号*/
		uint32_t		server_id;
		/*级别*/
		uint32_t		level;
		//玩家装备列表
		std::vector<simple_equip_info_t>		equip_info;

		login_player_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class msg_t: public Cmessage {
	public:
		//信息内容
		uint32_t _msg_len;
		char		msg[512];

		msg_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class msg_list_t: public Cmessage {
	public:
		//
		std::vector<msg_t>		msg_list;

		msg_list_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class numerical_enclosure_info_t: public Cmessage {
	public:
		/*数值附件类型 1：EXP 2:GOLD*/
		uint32_t		type;
		/*数值附件数量*/
		uint32_t		number;

		numerical_enclosure_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class room_player_info_t: public Cmessage {
	public:
		/*米米号*/
		uint32_t		uid;
		/*角色创建时间*/
		uint32_t		role_tm;
		/*昵称*/
		char		nick[16]; 
		/*服务器编号*/
		uint32_t		online_id;
		/*所在房间编号(0: 表示不在任何房间)*/
		uint32_t		room_id;
		/*房主的id*/
		uint32_t		owner_uid;
		/*队伍编号(如果 room_id是0, 则team的值没有意义)*/
		uint8_t		team_id;
		/*玩家进入房间后的座位号(如果room_id是0, 则seat的值没有意义)*/
		uint8_t		seat_id;

		room_player_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class simple_union_info_t: public Cmessage {
	public:
		/*工会ID*/
		uint32_t		union_id;
		/*工会名称*/
		char		union_name[16]; 

		simple_union_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class simple_user_info_t: public Cmessage {
	public:
		/*米米号*/
		uint32_t		user_id;
		/*昵称*/
		char		user_nick[16]; 

		simple_user_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
#endif // PEA_H
