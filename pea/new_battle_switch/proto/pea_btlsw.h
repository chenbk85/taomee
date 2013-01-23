#ifndef  PEA_BTLSW_H
#define  PEA_BTLSW_H
#include "pea.h"

class btlsw_chat_msg_transfer_in: public Cmessage {
	public:
		/**/
		uid_role_t		sender;
		/*发信人昵称*/
		char		sender_nick[16]; 
		/*信息类型*/
		uint32_t		msg_type;
		//信息内容
		uint32_t _msg_content_len;
		char		msg_content[64];

		btlsw_chat_msg_transfer_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_chat_msg_transfer_out: public Cmessage {
	public:
		/*发送者*/
		uid_role_t		sender;
		/*发信人昵称*/
		char		sender_nick[16]; 
		/*信息类型*/
		uint32_t		msg_type;
		//信息内容
		uint32_t _msg_content_len;
		char		msg_content[64];

		btlsw_chat_msg_transfer_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_chg_player_status_notify_out: public Cmessage {
	public:
		/*状态变更的米米号*/
		uint32_t		chg_uid;
		/*状态变更的创建角色时间*/
		int32_t		chg_role_tm;
		/*原状态*/
		uint8_t		from_status;
		/*现在的新状态 (0: 取消准备, 1: 准备, 2: 对战中)*/
		uint8_t		to_status;

		btlsw_chg_player_status_notify_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_get_room_full_info_out: public Cmessage {
	public:
		/*房间详细信息(房间基本信息+房间中玩家信息)*/
		room_full_info_t		room_full_info;

		btlsw_get_room_full_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_get_room_player_show_info_in: public Cmessage {
	public:
		/*米米号 (有可能不是拉取自己的信息)*/
		uint32_t		uid;
		/*角色创建时间*/
		uint32_t		role_tm;

		btlsw_get_room_player_show_info_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_get_room_player_show_info_out: public Cmessage {
	public:
		/*拉取到的数据，具体看内部结构定义*/
		room_player_show_info_t		show_info;

		btlsw_get_room_player_show_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_kick_room_player_in: public Cmessage {
	public:
		/*被踢者的米米号*/
		uint32_t		kickee_uid;
		/*被踢者的创建角色时间*/
		int32_t		kickee_role_tm;

		btlsw_kick_room_player_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_notify_room_seat_onoff_out: public Cmessage {
	public:
		/*队伍编号*/
		uint8_t		team_id;
		/*座位编号*/
		uint8_t		seat_id;
		/*0: 关闭座位, 1: 打开座位*/
		uint8_t		onoff;

		btlsw_notify_room_seat_onoff_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_online_register_in: public Cmessage {
	public:
		/*online服务器的id*/
		uint32_t		online_id;

		btlsw_online_register_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_player_enter_hall_in: public Cmessage {
	public:
		/*进入大厅时同步所有需要用于展示的信息(有可能跨服, 所以只能导btlsw拉取)*/
		room_player_show_info_t		player_info;

		btlsw_player_enter_hall_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_player_leave_notify_out: public Cmessage {
	public:
		/*离开房间者的team编号*/
		uint8_t		leaver_team;
		/*离开者的座位编号*/
		uint8_t		leaver_seat;
		/*离开者的uid*/
		uint32_t		leaver_uid;
		/*离开者的创建角色时间*/
		int32_t		leaver_role_tm;
		/*0: 自己离开(不发给自己); 1: 被踢(发给所有人)*/
		uint8_t		is_kicked;
		/*房主所在的team编号*/
		uint8_t		owner_team;
		/*房主的座位编号*/
		uint8_t		owner_seat;
		/*房主的id*/
		uint32_t		owner_uid;
		/*房主的创建角色时间*/
		int32_t		owner_role_tm;

		btlsw_player_leave_notify_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_player_sitdown_notify_out: public Cmessage {
	public:
		/*坐下的玩家米米号*/
		uint32_t		sitdown_uid;
		/*坐下者的创建角色时间*/
		int32_t		sitdown_role_tm;
		/*原所在队伍编号(0xFF表示第一次坐到房间里)*/
		uint8_t		from_team_id;
		/*原所在座位编号(0xFF表示第一次坐到房间里)*/
		uint8_t		from_seat_id;
		/*当前切换到的队伍编号*/
		uint8_t		to_team_id;
		/*当前切换到的座位编号*/
		uint8_t		to_seat_id;

		btlsw_player_sitdown_notify_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_create_room_in: public Cmessage {
	public:
		/*房间名字*/
		char		room_name[16]; 
		/*房间密码*/
		char		password[16]; 
		/*房间对战模式 (0: free(svr不干预), 1: compete(svr帮助匹配))*/
		uint8_t		room_mode;
		/*副本地图id*/
		uint32_t		map_id;
		/*副本难度 (只对pve有意义)*/
		uint8_t		map_lv;

		btlsw_pvp_create_room_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_create_room_out: public Cmessage {
	public:
		/*队伍编号*/
		uint8_t		team_id;
		/*座位编号*/
		uint8_t		seat_id;
		/**/
		room_info_t		room_info;
		/*房间中玩家的宠物信息*/
		room_pet_info_t		pet_info;

		btlsw_pvp_create_room_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_end_battle_in: public Cmessage {
	public:
		/*online服务器的id*/
		uint32_t		online_id;
		/*房间的id*/
		uint32_t		room_id;

		btlsw_pvp_end_battle_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_join_room_in: public Cmessage {
	public:
		/*房间的id*/
		uint32_t		room_id;
		/*房间密码*/
		char		password[16]; 

		btlsw_pvp_join_room_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_join_room_out: public Cmessage {
	public:
		/*进入的房间编号*/
		uint32_t		room_id;
		/*队伍编号*/
		uint8_t		team_id;
		/*座位编号*/
		uint8_t		seat_id;

		btlsw_pvp_join_room_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_query_room_info_in: public Cmessage {
	public:
		/*房间编号*/
		uint32_t		room_id;

		btlsw_pvp_query_room_info_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_query_room_info_out: public Cmessage {
	public:
		/*房间信息*/
		room_list_info_t		room_info;

		btlsw_pvp_query_room_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_room_attr_notify_out: public Cmessage {
	public:
		/*房间基本信息*/
		room_info_t		room_info;

		btlsw_pvp_room_attr_notify_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_room_list_in: public Cmessage {
	public:
		/*0xFF表示所有模式, 房间对战模式 (0: free(svr不干预), 1: compete(svr帮助匹配))*/
		uint8_t		room_mode;
		/*要多少个房间的信息(不超过32个)*/
		uint8_t		count;
		/*0: 向前翻页, 1: 向后翻页*/
		uint8_t		turn;
		/*边界roomid(返回中不包含这个id)*/
		uint32_t		bound_roomid;

		btlsw_pvp_room_list_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_room_list_out: public Cmessage {
	public:
		//拉取到的房间列表(最多32个)
		std::vector<room_list_info_t>		room_list;

		btlsw_pvp_room_list_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_room_set_attr_in: public Cmessage {
	public:
		/**/
		room_info_t		room_info;

		btlsw_pvp_room_set_attr_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_room_start_battle_in: public Cmessage {
	public:
		/*战斗id*/
		uint32_t		btl_id;
		/**/
		uint32_t		fd_idx;

		btlsw_pvp_room_start_battle_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_room_start_battle_out: public Cmessage {
	public:
		/*战斗id*/
		uint32_t		btl_id;
		/**/
		uint32_t		fd_idx;

		btlsw_pvp_room_start_battle_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_pvp_room_start_out: public Cmessage {
	public:
		/*地图ID*/
		uint32_t		map_id;
		/*玩家总数，包括匹配到的*/
		uint16_t		player_count;

		btlsw_pvp_room_start_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_room_player_set_attr_in: public Cmessage {
	public:
		/*0: 换位子, 1: 准备状态变更, 其它值: 非法*/
		uint8_t		which;
		/*希望换到的队伍编号*/
		uint8_t		new_team_id;
		/*希望换到的座位编号*/
		uint8_t		new_seat_id;
		/*希望切换到的玩家状态*/
		uint8_t		new_status;

		btlsw_room_player_set_attr_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
class btlsw_set_room_seat_onoff_in: public Cmessage {
	public:
		/*队伍编号*/
		uint8_t		team_id;
		/*座位编号*/
		uint8_t		seat_id;
		/*0: 关闭座位, 1: 打开座位*/
		uint8_t		onoff;

		btlsw_set_room_seat_onoff_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );

};


	
#endif // PEA_BTLSW_H
