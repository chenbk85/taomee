#ifndef  POP_H
#define  POP_H
#include <libtaomee++/proto/proto_base.h>
#include <stdio.h>

class online_item_t: public Cmessage {
	public:
		/**/
		uint32_t		online_id;
		/**/
		uint32_t		user_num;
		/**/
		char		online_ip[16]; 
		/**/
		uint16_t		online_port;
		/**/
		uint32_t		friend_count;

		online_item_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class find_map_t: public Cmessage {
	public:
		/**/
		uint32_t		islandid;
		/**/
		uint32_t		mapid;

		find_map_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class game_info_t: public Cmessage {
	public:
		/*项目编号*/
		uint32_t		gameid;
		/*胜利次数*/
		uint32_t		win_time;
		/*输的次数*/
		uint32_t		lose_time;
		/*平的次数*/
		uint32_t		draw_time;

		game_info_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class item_day_limit_t: public Cmessage {
	public:
		/*物品ID（3战斗积分4在线时长）*/
		uint32_t		itemid;
		/*总数*/
		uint32_t		total;
		/*当日获得次数*/
		uint32_t		daycnt;

		item_day_limit_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class item_exchange_t: public Cmessage {
	public:
		/*1:任务，2:xiaomee*/
		uint32_t		itemid;
		/**/
		uint32_t		count;
		/**/
		uint32_t		max_count;
		/*今日增加数量*/
		uint32_t		day_add_cnt;

		item_exchange_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class item_ex_t: public Cmessage {
	public:
		/**/
		uint32_t		itemid;
		/**/
		uint32_t		use_count;
		/**/
		uint32_t		count;

		item_ex_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class item_t: public Cmessage {
	public:
		/**/
		uint32_t		itemid;
		/**/
		uint32_t		count;

		item_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class task_t: public Cmessage {
	public:
		/**/
		uint32_t		taskid;
		/**/
		uint32_t		task_nodeid;

		task_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class user_log_t: public Cmessage {
	public:
		/*记录时间*/
		uint32_t		logtime;
		/*1:注册 ，2：完成岛:(v2:岛id)*/
		uint32_t		v1;
		/**/
		uint32_t		v2;

		user_log_t();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
#endif // POP_H
