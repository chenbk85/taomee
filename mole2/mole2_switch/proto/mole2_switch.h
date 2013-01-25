#ifndef  MOLE2_SWITCH_H
#define  MOLE2_SWITCH_H
#include "mole2.h"

class switch_add_luckystar_exchange_cnt_in: public Cmessage {
	public:
		/**/
		uint32_t		uid;
		/**/
		uint32_t		count;
		/**/
		uint32_t		max;

		switch_add_luckystar_exchange_cnt_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class switch_add_luckystar_exchange_cnt_out: public Cmessage {
	public:
		/*0 succ -1 fail*/
		uint32_t		ret;

		switch_add_luckystar_exchange_cnt_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class switch_broadcast_msg_in: public Cmessage {
	public:
		/**/
		uint32_t		onlineid;
		/**/
		uint32_t		uid;
		/**/
		uint32_t		type;
		/**/
		uint16_t		msglen;

		switch_broadcast_msg_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
#endif // MOLE2_SWITCH_H
