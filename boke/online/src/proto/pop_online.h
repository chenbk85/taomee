#ifndef  POP_ONLINE_H
#define  POP_ONLINE_H
#include "pop.h"

class cli_buy_item_in: public Cmessage {
	public:
		/*物品id*/
		uint32_t		itemid;

		cli_buy_item_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_buy_item_out: public Cmessage {
	public:
		/*剩余多少钱*/
		uint32_t		left_xiaomee;

		cli_buy_item_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_buy_item_use_gamept_in: public Cmessage {
	public:
		/*物品ID*/
		uint32_t		itemid;

		cli_buy_item_use_gamept_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_buy_item_use_gamept_out: public Cmessage {
	public:
		/*剩余可用游戏积分*/
		uint32_t		left_gamept;

		cli_buy_item_use_gamept_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_click_stat_in: public Cmessage {
	public:
		/*点击类型（1蜘蛛 2石碑）*/
		uint32_t		type;

		cli_click_stat_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_cur_game_end_in: public Cmessage {
	public:
		/*自己是不是羸了 :0:不是,1:是,2:平局*/
		uint32_t		win_flag;

		cli_cur_game_end_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_del_item_in: public Cmessage {
	public:
		//要删除的物品列表
		std::vector<item_t>		itemlist;

		cli_del_item_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_draw_lottery_out: public Cmessage {
	public:
		/*获取的物品（2卜克豆）*/
		item_t		item_get;

		cli_draw_lottery_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_find_map_add_in: public Cmessage {
	public:
		/**/
		uint32_t		islandid;
		/**/
		uint32_t		mapid;

		cli_find_map_add_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_game_opt_in: public Cmessage {
	public:
		//游戏操作信息
		uint32_t _gamemsg_len;
		char		gamemsg[9999];

		cli_game_opt_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_game_play_with_other_in: public Cmessage {
	public:
		/**/
		uint32_t		gameid;
		/*该请求的key, 在 确认时将之一起发出*/
		char		request_session[32]; 
		/*目标userid*/
		uint32_t		obj_userid;
		/*是否开始,0:否(终止游戏), 1:是*/
		uint32_t		is_start;
		/*为什么不玩, 1:自己关，2：在换装，3：购物*/
		uint32_t		why_not_start_flag;

		cli_game_play_with_other_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_game_play_with_other_out: public Cmessage {
	public:
		/*1:不存地图，2:已经在其它游戏中*/
		uint32_t		obj_user_stat;

		cli_game_play_with_other_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_game_request_in: public Cmessage {
	public:
		/**/
		uint32_t		gameid;
		/*目标userid*/
		uint32_t		obj_userid;

		cli_game_request_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_game_request_out: public Cmessage {
	public:
		/*1:不存地图，2:已经在其它游戏中*/
		uint32_t		obj_user_stat;

		cli_game_request_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class online_user_info_t: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		obj_userid;
		/*肤色*/
		uint32_t		color;
		/**/
		char		nick[16]; 
		/*卜克豆*/
		uint32_t		xiaomee;
		/*等级*/
		uint32_t		level;
		/*经验值*/
		uint32_t		experience;
		/**/
		int32_t		x;
		/**/
		int32_t		y;
		//勋章个数
		std::vector<uint32_t>		medal_list;
		/*小游戏积分*/
		uint32_t		game_point;
		//
		std::vector<uint32_t>		use_clothes_list;
		//使用中的特效卡片列表
		std::vector<uint32_t>		effect_list;

		online_user_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_all_user_info_from_cur_map_out: public Cmessage {
	public:
		//
		std::vector<online_user_info_t>		online_user_info_list;

		cli_get_all_user_info_from_cur_map_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_card_list_by_islandid_in: public Cmessage {
	public:
		/**/
		uint32_t		islandid;

		cli_get_card_list_by_islandid_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_card_list_by_islandid_out: public Cmessage {
	public:
		//
		std::vector<item_t>		cardid_list;

		cli_get_card_list_by_islandid_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class game_user_t: public Cmessage {
	public:
		/*发起的userid*/
		uint32_t		src_userid;
		/*项目编号*/
		uint32_t		gameid;
		//游戏中的其他玩家列表
		std::vector<uint32_t>		userlist;

		game_user_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_game_user_out: public Cmessage {
	public:
		//游戏中的用户列表
		std::vector<game_user_t>		game_user_list;

		cli_get_game_user_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class island_time_t: public Cmessage {
	public:
		/*岛屿ID*/
		uint32_t		islandid;
		/*当前是否开放*/
		uint32_t		is_open;
		/*下次状态改变时间*/
		uint32_t		next_state_time;

		island_time_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_island_time_out: public Cmessage {
	public:
		/*当前服务器时间*/
		uint32_t		svr_time;
		//岛屿开放时间列表
		std::vector<island_time_t>		islandlist;

		cli_get_island_time_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_item_list_in: public Cmessage {
	public:
		/**/
		uint32_t		startid;
		/**/
		uint32_t		endid;

		cli_get_item_list_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_item_list_out: public Cmessage {
	public:
		//
		std::vector<item_t>		item_list;

		cli_get_item_list_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_login_chest_out: public Cmessage {
	public:
		/*获取的物品（2卜克豆）*/
		item_t		item_get;

		cli_get_login_chest_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_login_reward_in: public Cmessage {
	public:
		/*序号（0~6）*/
		uint32_t		index;

		cli_get_login_reward_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_login_reward_out: public Cmessage {
	public:
		/*获取的物品（2卜克豆）*/
		item_t		item_get;

		cli_get_login_reward_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_lottery_count_out: public Cmessage {
	public:
		/**/
		uint32_t		count;

		cli_get_lottery_count_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_spec_item_in: public Cmessage {
	public:
		/*物品ID（3战斗积分4在线时长5打工次数8圣诞礼券）*/
		uint32_t		itemid;

		cli_get_spec_item_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_spec_item_list_in: public Cmessage {
	public:
		/**/
		uint32_t		startid;
		/**/
		uint32_t		endid;

		cli_get_spec_item_list_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_spec_item_list_out: public Cmessage {
	public:
		//
		std::vector<item_day_limit_t>		spec_item_list;

		cli_get_spec_item_list_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_spec_item_out: public Cmessage {
	public:
		/*物品数量*/
		item_day_limit_t		item;

		cli_get_spec_item_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_user_game_stat_in: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		obj_userid;

		cli_get_user_game_stat_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_user_game_stat_out: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		obj_userid;
		//小游戏统计信息
		std::vector<game_info_t>		game_stat_list;

		cli_get_user_game_stat_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_user_info_in: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		obj_userid;

		cli_get_user_info_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_user_info_out: public Cmessage {
	public:
		/**/
		uint32_t		regtime;
		/*肤色*/
		uint32_t		color;
		/**/
		char		nick[16]; 
		/*剩余多少钱*/
		uint32_t		left_xiaomee;
		/*等级*/
		uint32_t		level;
		/*经验值*/
		uint32_t		experience;
		/*小游戏积分*/
		uint32_t		game_point;
		//
		std::vector<uint32_t>		use_clothes_list;
		//使用中的特效卡片列表
		std::vector<uint32_t>		effect_list;
		//用户足迹列表
		std::vector<user_log_t>		user_log_list;
		//完成 岛的列表
		std::vector<uint32_t>		complete_islandid_list;

		cli_get_user_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_user_island_find_map_info_in: public Cmessage {
	public:
		/**/
		uint32_t		islandid;

		cli_get_user_island_find_map_info_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_user_island_find_map_info_out: public Cmessage {
	public:
		//
		std::vector<uint32_t>		find_map_list;

		cli_get_user_island_find_map_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_user_island_task_info_in: public Cmessage {
	public:
		/**/
		uint32_t		islandid;

		cli_get_user_island_task_info_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_user_island_task_info_out: public Cmessage {
	public:
		//
		std::vector<uint32_t>		task_nodeid_list;

		cli_get_user_island_task_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_get_valid_gamept_out: public Cmessage {
	public:
		/*剩余可用游戏积分*/
		uint32_t		left_gamept;

		cli_get_valid_gamept_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_hang_bell_get_item_in: public Cmessage {
	public:
		/*铃铛类型(0五彩1金色2银色)*/
		uint32_t		bell_type;
		/*投中区域(0蓝1绿2红3没投中)*/
		uint32_t		zone_type;

		cli_hang_bell_get_item_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_hang_bell_get_item_out: public Cmessage {
	public:
		/*获取的物品（2卜克豆）*/
		item_t		item_get;

		cli_hang_bell_get_item_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_login_in: public Cmessage {
	public:
		/*服务器编号*/
		uint32_t		server_id;
		/*米米号*/
		uint32_t		login_userid;
		/*用于跳转时无需密码*/
		char		session[32]; 

		cli_login_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_login_out: public Cmessage {
	public:
		/*按位标记：0-》注册 1-》新注册米米号 2-》聊天样本 3-》点击过指引*/
		uint32_t		regflag;
		/*剩余多少钱*/
		uint32_t		left_xiaomee;
		/*肤色*/
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
		//
		std::vector<uint32_t>		task_nodeid_list;
		//
		std::vector<uint32_t>		use_clothes_list;
		//使用中的特效卡片列表
		std::vector<uint32_t>		effect_list;
		//
		std::vector<uint32_t>		find_map_list;

		cli_login_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_effect_used_out: public Cmessage {
	public:
		/*发起的userid*/
		uint32_t		src_userid;
		/*物品ID*/
		uint32_t		itemid;
		/*要取消的特效卡片ID（没有为0）*/
		uint32_t		unset_itemid;

		cli_noti_effect_used_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_first_enter_island_out: public Cmessage {
	public:
		/**/
		uint32_t		islandid;
		/*获得卜克豆*/
		uint32_t		xiaomee;
		/*是否新注册的米米号*/
		uint32_t		is_newid;

		cli_noti_first_enter_island_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class user_game_win_t: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		obj_userid;
		/*输羸 :0:不是,1:是,2:平局*/
		uint32_t		win_flag;
		/*正数得到负数减去*/
		int32_t		get_point;

		user_game_win_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_game_end_out: public Cmessage {
	public:
		//
		std::vector<user_game_win_t>		user_win_list;

		cli_noti_game_end_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_game_is_start_out: public Cmessage {
	public:
		/**/
		uint32_t		gameid;
		/*是否开始,0:否(终止游戏), 1:是*/
		uint32_t		is_start;
		/*为什么不玩, 1:自己关，2：在换装，3：购物*/
		uint32_t		why_not_start_flag;
		/*目标userid*/
		uint32_t		obj_userid;

		cli_noti_game_is_start_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_game_opt_out: public Cmessage {
	public:
		/*发起的userid*/
		uint32_t		src_userid;
		//游戏操作信息
		uint32_t _gamemsg_len;
		char		gamemsg[9999];

		cli_noti_game_opt_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_game_request_out: public Cmessage {
	public:
		/**/
		uint32_t		gameid;
		/*发起的userid*/
		uint32_t		src_userid;
		/*该请求的key, 在 确认时将之一起发出*/
		char		request_session[32]; 

		cli_noti_game_request_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class game_seat_t: public Cmessage {
	public:
		/*项目编号*/
		uint32_t		gameid;
		/*0左1右*/
		uint32_t		side;
		/*在座位上的米米号*/
		uint32_t		userid;

		game_seat_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_game_seat_out: public Cmessage {
	public:
		//小游戏列表
		std::vector<game_seat_t>		gamelist;

		cli_noti_game_seat_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_game_start_out: public Cmessage {
	public:
		/**/
		uint32_t		gameid;
		//一起玩的userid列表
		std::vector<uint32_t>		userid_list;
		/*第一步的用户,如果为0，则没有限定*/
		uint32_t		start_userid;
		/*每一步超时时间 ,0：不限定*/
		uint32_t		step_timeout;

		cli_noti_game_start_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_game_user_left_game_out: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		obj_userid;

		cli_noti_game_user_left_game_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_game_user_out: public Cmessage {
	public:
		/*状态（1开始2结束）*/
		uint32_t		state;
		/**/
		game_user_t		game_user;

		cli_noti_game_user_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_get_invitation_out: public Cmessage {
	public:
		/*物品邀请函ID*/
		uint32_t		itemid;

		cli_noti_get_invitation_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_get_item_list_out: public Cmessage {
	public:
		//得到的物品列表
		std::vector<item_t>		item_list;

		cli_noti_get_item_list_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_island_complete_out: public Cmessage {
	public:
		/**/
		uint32_t		userid;
		/**/
		char		nick[16]; 
		/**/
		uint32_t		islandid;

		cli_noti_island_complete_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_leave_island_out: public Cmessage {
	public:
		/*当前岛ID*/
		uint32_t		cur_island;
		/*当前地图ID*/
		uint32_t		cur_mapid;
		/*前往的岛屿ID*/
		uint32_t		islandid;
		/*前往的地图ID*/
		uint32_t		mapid;

		cli_noti_leave_island_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_one_user_info_out: public Cmessage {
	public:
		/**/
		online_user_info_t		user_info;

		cli_noti_one_user_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_set_color_out: public Cmessage {
	public:
		/**/
		uint32_t		userid;
		/*肤色*/
		uint32_t		color;

		cli_noti_set_color_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_show_out: public Cmessage {
	public:
		/*发起的userid*/
		uint32_t		src_userid;
		/*表情ID*/
		uint32_t		expression_id;
		/*目标userid（0：场景 1游戏中）*/
		uint32_t		obj_userid;

		cli_noti_show_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_svr_time_out: public Cmessage {
	public:
		/**/
		uint32_t		timestamp;

		cli_noti_svr_time_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_talk_out: public Cmessage {
	public:
		/*发起的userid*/
		uint32_t		src_userid;
		/**/
		char		src_nick[16]; 
		/*目标userid*/
		uint32_t		obj_userid;
		//
		uint32_t _msg_len;
		char		msg[9999];

		cli_noti_talk_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_user_left_map_out: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		obj_userid;

		cli_noti_user_left_map_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_user_level_up_out: public Cmessage {
	public:
		/*发起的userid*/
		uint32_t		src_userid;
		/*等级*/
		uint32_t		level;
		/*经验值*/
		uint32_t		experience;
		/*此次得到的经验*/
		uint32_t		addexp;
		/*是否升级*/
		uint32_t		is_level_up;
		/*升级获得的奖励物品*/
		uint32_t		itemid;

		cli_noti_user_level_up_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_user_move_out: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		obj_userid;
		/**/
		uint32_t		start_x;
		/**/
		uint32_t		start_y;
		/**/
		uint32_t		mouse_x;
		/**/
		uint32_t		mouse_y;
		/*移动模式*/
		uint32_t		type;

		cli_noti_user_move_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_user_online_time_out: public Cmessage {
	public:
		/*计时类型（0连续 1累计）*/
		uint32_t		type;
		/*在线时间（分钟）*/
		uint32_t		time;

		cli_noti_user_online_time_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_noti_user_use_clothes_list_out: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		obj_userid;
		//
		std::vector<uint32_t>		use_clothes_list;

		cli_noti_user_use_clothes_list_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_post_msg_in: public Cmessage {
	public:
		/*投稿类型*/
		uint32_t		type;
		/*主题*/
		char		title[60]; 
		//投稿内容
		uint32_t _msg_len;
		char		msg[4096];

		cli_post_msg_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_proto_header_in: public Cmessage {
	public:
		/**/
		uint32_t		proto_length;
		/**/
		uint16_t		cmdid;
		/**/
		uint32_t		timestamp;
		/**/
		uint32_t		seq;
		/**/
		uint16_t		result;
		/**/
		uint16_t		useSever;

		cli_proto_header_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_reg_in: public Cmessage {
	public:
		/*渠道id(填0)*/
		uint32_t		channelid;
		/**/
		char		nick[16]; 
		/**/
		uint32_t		age;
		/*肤色*/
		uint32_t		color;
		//
		std::vector<item_t>		item_list;

		cli_reg_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_reg_out: public Cmessage {
	public:
		/*剩余多少钱*/
		uint32_t		left_xiaomee;
		/*昵称*/
		char		nick[16]; 

		cli_reg_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_set_busy_state_in: public Cmessage {
	public:
		/*忙状态*/
		uint32_t		state;

		cli_set_busy_state_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_set_color_in: public Cmessage {
	public:
		/*肤色*/
		uint32_t		color;

		cli_set_color_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_set_effect_used_in: public Cmessage {
	public:
		/*物品ID*/
		uint32_t		itemid;
		/*（1穿上 0去掉）*/
		uint32_t		type;

		cli_set_effect_used_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_set_item_used_list_in: public Cmessage {
	public:
		//
		std::vector<uint32_t>		itemid_list;

		cli_set_item_used_list_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_set_nick_in: public Cmessage {
	public:
		/**/
		char		nick[16]; 

		cli_set_nick_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_show_in: public Cmessage {
	public:
		/*表情ID*/
		uint32_t		expression_id;
		/*目标userid（0：场景 1：游戏中）*/
		uint32_t		obj_userid;

		cli_show_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_talk_in: public Cmessage {
	public:
		/*目标userid*/
		uint32_t		obj_userid;
		//
		uint32_t _msg_len;
		char		msg[9999];

		cli_talk_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_talk_npc_in: public Cmessage {
	public:
		/**/
		uint32_t		npc_id;

		cli_talk_npc_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_task_complete_node_in: public Cmessage {
	public:
		/**/
		uint32_t		islandid;
		/*子任务id*/
		uint32_t		task_nodeid;

		cli_task_complete_node_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_task_complete_node_out: public Cmessage {
	public:
		//
		std::vector<uint32_t>		task_nodeid_list;

		cli_task_complete_node_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_task_del_node_in: public Cmessage {
	public:
		/**/
		uint32_t		islandid;
		/*子任务id*/
		uint32_t		task_nodeid;

		cli_task_del_node_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_user_move_in: public Cmessage {
	public:
		/**/
		uint32_t		start_x;
		/**/
		uint32_t		start_y;
		/**/
		uint32_t		mouse_x;
		/**/
		uint32_t		mouse_y;
		/*移动模式*/
		uint32_t		type;

		cli_user_move_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_walk_in: public Cmessage {
	public:
		/**/
		uint32_t		islandid;
		/**/
		uint32_t		mapid;
		/**/
		uint32_t		x;
		/**/
		uint32_t		y;

		cli_walk_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cli_work_get_lottery_out: public Cmessage {
	public:
		/*今天获得的次数*/
		uint32_t		get_count;

		cli_work_get_lottery_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_activate_game_in: public Cmessage {
	public:
		/*项目id (填12)*/
		uint32_t		which_game;
		/*激活码：6-8个0-9、A-Z、a-z字符，没有用到的字节填0*/
		char		active_code[10]; 
		/*验证码ID*/
		char		imgid[16]; 
		/*验证码 (空)*/
		char		verif_code[6]; 

		login_activate_game_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_activate_game_out: public Cmessage {
	public:
		/*0表示激活成功，不会发送蓝色部分的内容 1激活码错误次数过多，需要输入验证码后再激活 2验证码输入错误*/
		uint32_t		flag;
		/*验证图片id(空）*/
		char		img_id[16]; 
		//验证图片
		uint32_t _img_len;
		char		img[1000];

		login_activate_game_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_check_game_activate_in: public Cmessage {
	public:
		/*项目id (填12)*/
		uint32_t		which_game;

		login_check_game_activate_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_check_game_activate_out: public Cmessage {
	public:
		/*0未激活1已激活*/
		uint32_t		flag;

		login_check_game_activate_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_get_ranged_svr_list_in: public Cmessage {
	public:
		/**/
		uint32_t		startid;
		/**/
		uint32_t		endid;

		login_get_ranged_svr_list_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_get_ranged_svr_list_out: public Cmessage {
	public:
		//
		std::vector<online_item_t>		online_list;

		login_get_ranged_svr_list_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_get_recommeded_list_in: public Cmessage {
	public:
		/*用于跳转时无需密码*/
		char		session[32]; 

		login_get_recommeded_list_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_get_recommeded_list_out: public Cmessage {
	public:
		/**/
		uint32_t		max_online_id;
		/**/
		uint32_t		vip;
		//
		std::vector<online_item_t>		online_list;

		login_get_recommeded_list_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_login_in: public Cmessage {
	public:
		/*密码的两次md5值*/
		char		passwd_md5_two[32]; 
		/*渠道id(填0)*/
		uint32_t		channelid;
		/*项目id (填12)*/
		uint32_t		which_game;
		/*ip 地址*/
		uint32_t		user_ip;
		/*验证图片id(空）*/
		char		img_id[16]; 
		/*验证码 (空)*/
		char		verif_code[6]; 

		login_login_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_login_out: public Cmessage {
	public:
		/**/
		uint32_t		verif_flag;
		/*用于跳转时无需密码*/
		char		session[16]; 
		/**/
		uint32_t		create_role_flag;

		login_login_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_refresh_img_in: public Cmessage {
	public:
		/*0正常登录； 1使用激活码时（可以不发送，兼容旧版本即正常登录）*/
		uint32_t		flag;

		login_refresh_img_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class login_refresh_img_out: public Cmessage {
	public:
		/*0不需要输入验证码，1需要输入验证（为1时才有验证码)）*/
		uint32_t		flag;
		/*图片ID。发送验证码时，需要把这个ID带上*/
		char		imgid[16]; 
		//验证图片
		uint32_t _img_len;
		char		img[1000];

		login_refresh_img_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class noti_cli_leave_out: public Cmessage {
	public:
		/*离线原因 （1午夜休息）*/
		uint32_t		reason;

		noti_cli_leave_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
#endif // POP_ONLINE_H
