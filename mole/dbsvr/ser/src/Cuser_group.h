/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_group.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 13时47分01秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_GROUP_INCL
#define    CUSER_GROUP_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_group:public CtableRoute100x10{
	private:
		//p_existed :记录是否存在
		int get_grouplist_from_db(userid_t userid ,creategroupid_list *p_clist,group_list *p_list );
		int update_grouplist_db(userid_t userid,creategroupid_list *p_clist,  group_list *p_list );
		int update_grouplist(userid_t userid,creategroupid_list *p_clist,  group_list *p_list );
		int insert  (userid_t userid,  creategroupid_list *p_clist,  group_list *p_list );
	public:

		int get_creategroup_list( userid_t userid,  creategroupid_list *p_clist );
		int jion_group(userid_t userid , uint32_t  groupid );
		int add_group(userid_t userid, uint32_t  groupid );
		int del_group(userid_t userid, uint32_t  groupid );
		int get_grouplist(userid_t userid ,creategroupid_list *p_clist, group_list *p_list );
		Cuser_group(mysql_interface * db ) ;
		int set_group_flag(userid_t userid, stru_group_item *p_group_item );
};
#endif   /* ----- #ifndef CUSER_GROUP_INCL  ----- */
