
/*
 * =====================================================================================
 * 
 *       Filename:  Cuser.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  g++
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CVIPBUFF_INC
#define  CVIPBUFF_INC
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cvip_buff:public CtableRoute100x10
{
public:
	Cvip_buff (mysql_interface * db);
	int del_vip_buff(userid_t userid,uint32_t petid);
	int set_vip_buff(userid_t userid,stru_vip_buff *parg);
	int get_buffs(userid_t userid,std::vector<stru_vip_buff> &buffs);
	int get_vip_buff_list(userid_t userid,mole2_get_vip_buff_in *p_in,mole2_get_vip_buff_out *p_out);
};

#endif /* ----- #ifndef CVIPBUFF_INC  ----- */
