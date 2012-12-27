#ifndef _CROOMMSG_H_20081110_
#define _CROOMMSG_H_20081110_

#include "CtableRoute10x10.h"
#include "proto.h"

class Croommsg : public CtableRoute10x10
{
public:
    Croommsg(mysql_interface * db );
    int add_msg(userid_t userid, roommsg_add_msg_in * p_msg);
    int show_msg(userid_t userid, roommsg_show_msg_in * p_msg);
    int list_msg(userid_t userid, roommsg_list_msg_in * p_msgin, uint32_t* p_total, uint32_t* p_count, roommsg_list_msg_out_item** pp_list);
    int del_msg(userid_t userid, roommsg_del_msg_in * p_msg);
    int res_msg(userid_t userid, roommsg_res_msg_in * p_msg);
};



#endif // _CROOMMSG_H_20081110_
/**********************************EOF*****************************************/


