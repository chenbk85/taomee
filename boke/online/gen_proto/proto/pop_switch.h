#ifndef  POP_SWITCH_H
#define  POP_SWITCH_H
#include "pop.h"

class sw_get_ranged_svrlist_in: public Cmessage {
	public:
		/**/
		uint32_t		start_id;
		/**/
		uint32_t		end_id;
		//
		std::vector<uint32_t>		friend_list;

		sw_get_ranged_svrlist_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class sw_get_ranged_svrlist_out: public Cmessage {
	public:
		/**/
		uint32_t		max_online_id;
		//
		std::vector<online_item_t>		online_list;

		sw_get_ranged_svrlist_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class sw_get_recommend_svr_list_out: public Cmessage {
	public:
		/**/
		uint32_t		max_online_id;
		//
		std::vector<online_item_t>		online_list;

		sw_get_recommend_svr_list_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class sw_get_user_count_out: public Cmessage {
	public:
		/**/
		uint32_t		online_count;

		sw_get_user_count_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class sw_noti_across_svr_in: public Cmessage {
	public:
		/**/
		uint32_t		cmdid;
		//
		uint32_t _buflist_len;
		char		buflist[999];

		sw_noti_across_svr_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class sw_noti_across_svr_out: public Cmessage {
	public:
		/**/
		uint32_t		cmdid;
		//
		uint32_t _buflist_len;
		char		buflist[999];

		sw_noti_across_svr_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class sw_report_online_info_in: public Cmessage {
	public:
		/**/
		uint16_t		domain_id;
		/**/
		uint32_t		online_id;
		/**/
		char		online_name[16]; 
		/**/
		uint16_t		online_ip[16]; 
		/**/
		uint16_t		online_port;
		/**/
		uint32_t		user_num;
		/**/
		uint32_t		seqno;
		//在线的用户列表
		std::vector<uint32_t>		userid_list;

		sw_report_online_info_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class sw_report_user_onoff_in: public Cmessage {
	public:
		/**/
		uint32_t		is_on_online;

		sw_report_user_onoff_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
#endif // POP_SWITCH_H
