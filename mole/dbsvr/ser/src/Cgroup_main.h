/*
 * =====================================================================================
 * 
 *       Filename:  Cgroup_main.h
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

#ifndef  CGROUP_MAIN_INCL
#define  CGROUP_MAIN_INCL
#include "Ctable.h"
#include "proto.h"


class Cgroup_main : public Ctable{
	protected:
		
	public:
		Cgroup_main(mysql_interface * db ); 
		int insert(uint32_t * p_groupid );
};
#endif   /* ----- #ifndef CGROUP_MAIN_INCL  ----- */

