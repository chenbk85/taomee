/*
 * =====================================================================================
 * 
 *       Filename:  Cchangelist.h
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

#ifndef  CCHANGELIST_INCL
#define  CCHANGELIST_INCL
#include "Ctable.h"
#include "proto.h"
#include "Cclientproto.h"

#define   ATTR_EXP			1001
#define   ATTR_STRONG		1002
#define   ATTR_IQ			1003
#define   ATTR_CHARM		1004
#define   ATTR_XIAOMEE  	1005
#define   ATTR_DEL_WRINGE  	2001

#define   REASON_SU 		1
#define   REASON_WRITING 	2

class Cchangelist : public Ctable{
	protected:
		Cclientproto  *cp;
	public:
		Cchangelist(mysql_interface * db, Cclientproto *cp ); 
		int changevalue( userid_t adminid, su_change_in *p_in );
		int change_user_value(  su_change_in *p_in );


		int insert( su_changelist_add_in  *p_in ) ;

};

#endif   /* ----- #ifndef CCHANGELIST_INCL  ----- */

