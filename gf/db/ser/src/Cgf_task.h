#ifndef  GF_CGF_TASK_H
#define    GF_CGF_TASK_H
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"



enum {
	SET_TASK_NULL 		= 0,
	SET_TASK_IN_HAND	= 1,
	SET_TASK_CANCEL 	= 2,
	SET_TASK_FINISHED	= 3
};

enum {
	SET_TASK_SER_VALUE = 1,
	SET_TASK_AS_VALUE  = 2
};


class Cgf_task:public CtableRoute{
	public:
		Cgf_task(mysql_interface * db ); 

		int get_task_finished(userid_t userid, userid_t role_regtime, uint32_t* p_count,
			gf_get_task_finished_out_item** pp_list);

		int get_task_in_hand(userid_t userid, userid_t role_regtime, uint32_t* p_count,
			gf_get_task_in_hand_out_item** pp_list);

        int get_task_cancel(userid_t userid, userid_t role_regtime, uint32_t* p_count,
            gf_get_task_cancel_out_item** pp_list);

		int get_all_task(userid_t userid, userid_t role_regtime, uint32_t* p_count,
			gf_get_all_task_out_item** pp_list);

		int get_task_list_count(userid_t userid, userid_t role_regtime, const char* str_where,
			uint32_t* p_count);
		int set_task_finished(userid_t userid, userid_t role_regtime, uint32_t taskid);
		int set_task_cancel(userid_t userid, userid_t role_regtime, uint32_t taskid);

		int set_task_in_hand(userid_t userid, userid_t role_regtime, uint32_t taskid);


		int set_task_ser_value(userid_t userid, userid_t role_regtime, uint32_t taskid, 
			uint8_t* binary_val,uint32_t binary_len);

		int set_task_as_value(userid_t userid, userid_t role_regtime, uint32_t taskid, 
			uint8_t* binary_val,uint32_t binary_len);
		
		int set_task_flg(userid_t userid, userid_t role_regtime, uint32_t taskid, int flg);

		int del_task(userid_t userid, userid_t role_regtime, uint32_t taskid);
		int del_daily_loop_task(userid_t userid, userid_t role_regtime);
		int del_old_daily_loop_task(userid_t userid, userid_t role_regtime);
		int clear_role_task(userid_t userid,uint32_t role_regtime);

		int clear_role_info(userid_t userid,uint32_t role_regtime);
		
};
#endif   /* ----- #ifndef CDD_ATTIRE_INCL  ----- */




