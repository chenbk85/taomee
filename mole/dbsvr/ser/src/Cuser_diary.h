#ifndef _CUSER_DIARY_H_20081110_
#define _CUSER_DIARY_H_20081110_

#include "CtableRoute10x10.h"
#include "proto.h"

class Cuser_diary : public CtableRoute10x10
{
public:
    Cuser_diary(mysql_interface * db );
    int add_msg(userid_t userid, roommsg_add_diary_in_header  * p_msg);
	/*
    int show_msg(userid_t userid, USER_DIARY_SHOW_MSG_IN * p_msg);
    int list_msg(userid_t userid, USER_DIARY_LIST_MSG_IN * p_msgin, uint32_t* p_total, uint32_t* p_count, USER_DIARY_LIST_MSG_OUT_ITEM** pp_list);
    int del_msg(userid_t userid, USER_DIARY_DEL_MSG_IN * p_msg);
    int res_msg(userid_t userid, USER_DIARY_RES_MSG_IN * p_msg);
	*/
};



#endif // _CUSER_DIARY_H_20081110_
/**********************************EOF*****************************************/


