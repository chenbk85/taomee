#ifndef  POP_DB_H
#define  POP_DB_H
#include "pop.h"

class pop_exchange_in: public Cmessage {
	public:
		/**/
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

	
class pop_login_out: public Cmessage {
	public:
		/**/
		uint32_t		register_time;
		/**/
		uint32_t		sex;
		/**/
		uint32_t		age;
		/**/
		char		nick[16]; 
		//
		std::vector<task_t>		task_list;

		pop_login_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pop_reg_in: public Cmessage {
	public:
		/**/
		uint32_t		sex;
		/**/
		uint32_t		age;
		/**/
		char		nick[16]; 
		/**/
		uint32_t		item_configid;

		pop_reg_in();
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

	
class pop_task_get_list_out: public Cmessage {
	public:
		//
		std::vector<task_t>		task_list;

		pop_task_get_list_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
#endif // POP_DB_H
