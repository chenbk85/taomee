/*
 * =====================================================================================
 * 
 *       Filename:  Cgroup.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CGROUP_INCL
#define  CGROUP_INCL
#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"

class Cgroup:public CtableRoute10x10 {
	private:

	int set_memberlist(uint32_t groupid, stru_group_member *p_item);
	public:
		Cgroup(mysql_interface * db ); 
		int	get_member(uint32_t groupid,stru_group_member *p_out,userid_t * p_ownerid);
		int insert(uint32_t groupid,group_item_without_id *p_item );
		int get(uint32_t groupid,group_item_without_id *p_out);
		int set_msg(uint32_t groupid, group_msg_item*p_item );
		int add_member( uint32_t groupid, userid_t groupownerid, uint32_t memberid,
				stru_group_member *p_group_memberlist);
		int del_member( uint32_t groupid, userid_t groupownerid, uint32_t memberid,
				stru_group_member *p_group_memberlist);
		int del( uint32_t groupid, userid_t groupownerid);
		int get_ownerid(uint32_t groupid, userid_t *p_ownerid);
};

#endif   /* ----- #ifndef CGROUP_INCL  ----- */

