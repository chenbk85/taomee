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

#ifndef  CCLASS_ID_INCL
#define  CCLASS_ID_INCL
#include "Ctable.h"
#include "proto.h"


class Cclass_id : public Ctable{
	public:
		Cclass_id(mysql_interface * db ); 
		int insert(uint32_t * p_classid );
};
#endif   /* ----- #ifndef CGROUP_MAIN_INCL  ----- */

