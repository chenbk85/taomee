#ifndef _Cuser_classmsg_H_20081110_
#define _Cuser_classmsg_H_20081110_

#include "CtableRoute100x10.h"
#include "proto.h"

class Cuser_classmsg : public CtableRoute100x10
{
public:
    Cuser_classmsg(mysql_interface * db );
	
    	int get_count(userid_t userid, uint32_t  * p_count);
		int add_msg(userid_t userid, user_classmsg_add_in * p_msg);

		int show_msg(userid_t userid,  user_classmsg_set_show_flag_in * p_msg);

		int get_list(userid_t userid,  user_classmsg_get_list_in * p_msgin, 
				uint32_t* p_total, uint32_t* p_count, user_classmsg_get_list_out_item  ** pp_list);

		int del_msg(userid_t userid,  user_classmsg_del_in* p_msg);

		int res_msg(userid_t userid, user_classmsg_report_in * p_msg);

		int del_class(userid_t userid );
};



#endif // _Cuser_classmsg_H_20081110_
/**********************************EOF*****************************************/


