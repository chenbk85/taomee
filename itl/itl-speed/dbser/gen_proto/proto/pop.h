#ifndef  POP_H
#define  POP_H
#include <libtaomee++/proto/proto_base.h>
#include <stdio.h>

class item_exchange_t: public Cmessage {
	public:
		/**/
		uint32_t		itemid;
		/**/
		uint32_t		count;
		/**/
		uint32_t		max_count;

		item_exchange_t();
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

	
#endif // POP_H
