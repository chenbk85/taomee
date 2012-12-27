#ifndef  POP_DB_H
#define  POP_DB_H
#include "pop.h"

class get_server_version_out: public Cmessage {
	public:
		/**/
		char		version[255]; 

		get_server_version_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_add_game_info_in: public Cmessage {
	public:
		/*项目编号*/
		uint32_t		gameid;
		/*胜负标志0：负 1：胜 2：平*/
		uint32_t		win_flag;

		pop_add_game_info_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_copy_user_in: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		dsc_userid;

		pop_copy_user_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_exchange_in: public Cmessage {
	public:
		/*未使用*/
		uint32_t		opt_flag;
		//
		std::vector<item_exchange_t>		add_item_list;
		//
		std::vector<item_exchange_t>		del_item_list;

		pop_exchange_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_find_map_add_in: public Cmessage {
	public:
		/**/
		uint32_t		islandid;
		/**/
		uint32_t		mapid;

		pop_find_map_add_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class user_base_info_t: public Cmessage {
	public:
		/**/
		uint32_t		register_time;
		/*0x01:用户是否注册过*/
		uint32_t		flag;
		/**/
		uint32_t		xiaomee;
		/**/
		uint32_t		color;
		/**/
		uint32_t		age;
		/**/
		char		nick[16]; 
		/**/
		uint32_t		last_islandid;
		/**/
		uint32_t		last_mapid;
		/**/
		uint32_t		last_x;
		/**/
		uint32_t		last_y;
		/**/
		uint32_t		last_login;
		/**/
		uint32_t		online_time;

		user_base_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_get_all_info_out: public Cmessage {
	public:
		/**/
		user_base_info_t		user_base_info;
		//
		std::vector<item_ex_t>		item_list;
		//每日获取物品列表
		std::vector<item_day_limit_t>		item_day_list;
		//
		std::vector<task_t>		task_list;
		//
		std::vector<find_map_t>		find_map_list;
		//用户足迹列表
		std::vector<user_log_t>		user_log_list;
		//小游戏统计信息
		std::vector<game_info_t>		game_info_list;

		pop_get_all_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_get_base_info_out: public Cmessage {
	public:
		/*最后一次登录的服务器*/
		uint32_t		last_online_id;

		pop_get_base_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_login_out: public Cmessage {
	public:
		/**/
		user_base_info_t		user_base_info;
		//
		std::vector<item_ex_t>		item_list;
		//每日获取物品列表
		std::vector<item_day_limit_t>		item_day_list;
		//
		std::vector<task_t>		task_list;
		//
		std::vector<find_map_t>		find_map_list;
		//用户足迹列表
		std::vector<user_log_t>		user_log_list;
		//小游戏统计信息
		std::vector<game_info_t>		game_info_list;

		pop_login_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_logout_in: public Cmessage {
	public:
		/**/
		uint32_t		last_islandid;
		/**/
		uint32_t		last_mapid;
		/**/
		uint32_t		last_x;
		/**/
		uint32_t		last_y;
		/**/
		uint32_t		last_login;
		/**/
		uint32_t		online_time;
		/*最后一次登录的服务器*/
		uint32_t		last_online_id;

		pop_logout_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_opt_in: public Cmessage {
	public:
		/*1:物品;2任务,3find_map*/
		uint32_t		opt_groupid;
		/*1:增加/减少,2:设置*/
		uint32_t		opt_type;
		/*操作物的id（如ItemID）*/
		uint32_t		optid;
		/*改变的数量*/
		int32_t		count;
		/**/
		uint32_t		v1;
		/**/
		uint32_t		v2;

		pop_opt_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_reg_in: public Cmessage {
	public:
		/**/
		uint32_t		color;
		/**/
		uint32_t		age;
		/**/
		char		nick[16]; 
		/*0x01:用户是否注册过 0x02是否新米米号*/
		uint32_t		flag;
		//
		std::vector<item_exchange_t>		add_item_list;

		pop_reg_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_set_color_in: public Cmessage {
	public:
		/**/
		uint32_t		color;

		pop_set_color_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_set_flag_in: public Cmessage {
	public:
		/*0x01:用户是否注册过 0x02是否新米米号*/
		uint32_t		flag;
		/*掩码*/
		uint32_t		mask;

		pop_set_flag_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_set_item_used_list_in: public Cmessage {
	public:
		//
		std::vector<uint32_t>		del_can_change_list;
		//
		std::vector<uint32_t>		set_noused_list;
		//
		std::vector<uint32_t>		add_can_change_list;
		//
		std::vector<uint32_t>		set_used_list;

		pop_set_item_used_list_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_set_nick_in: public Cmessage {
	public:
		/**/
		char		nick[16]; 

		pop_set_nick_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_task_complete_node_in: public Cmessage {
	public:
		/**/
		uint32_t		taskid;
		/**/
		uint32_t		task_nodeid;

		pop_task_complete_node_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_task_del_in: public Cmessage {
	public:
		/**/
		uint32_t		taskid;
		/**/
		uint32_t		task_nodeid;

		pop_task_del_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_user_log_add_in: public Cmessage {
	public:
		/**/
		user_log_t		user_log;

		pop_user_log_add_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_user_set_field_value_in: public Cmessage {
	public:
		/*字段名*/
		char		field_name[64]; 
		/*字段值*/
		char		field_value[255]; 

		pop_user_set_field_value_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class udp_post_msg_in: public Cmessage {
	public:
		/*项目编号*/
		uint32_t		gameid;
		/*无效填0*/
		uint32_t		nouse;
		/*投稿类型*/
		uint32_t		type;
		/*昵称*/
		char		nick[16]; 
		/*主题*/
		char		title[60]; 
		//投稿内容
		uint32_t _msg_len;
		char		msg[4096];

		udp_post_msg_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
#endif // POP_DB_H
