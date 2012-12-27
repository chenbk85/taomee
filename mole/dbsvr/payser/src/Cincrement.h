/*
 * =====================================================================================
 * 
 *       Filename:  Cincrement.h
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

#ifndef  CINCREMENT_INCL
#define  CINCREMENT_INCL
#include "CtableRoute.h"
#include "Cdamee_history.h"
#include "proto.h"

#define  NOVIP_NEXTERRCOUNT  	2

typedef struct  Tincrement {
	userid_t userid;
	uint32_t flag;
	uint32_t damee;
	uint32_t month_paytype; 
	uint32_t month_used; 
	uint32_t month_enabledate; 
	uint32_t month_duedate; 
	uint32_t month_nexterrcount;
} __attribute__((packed)) TINCREMENT ;


class Cincrement :public CtableRoute {
	protected:
		Cdamee_history damee_history;
		int insert(TINCREMENT * t);
		virtual char*  get_table_name(uint32_t id);
	public:
		Cincrement(mysql_interface * db ); 
		int init_user(userid_t userid,uint32_t damee );
		int set_damee(userid_t userid,int32_t value, uint32_t* p_leave_damee,
			uint32_t paytype,char*pri_msg );
		int isMonthed(userid_t userid,uint32_t * p_duetime);
		int set_no_auto_month(userid_t userid);
		int get_record(userid_t userid, USER_PAY_INFO * p_out  );
		int update_duetime(userid_t userid,uint32_t newduetime,uint32_t nexterrcount );
		int set_del_month(userid_t userid);

		int set_month(  userid_t userid, uint32_t paytype, uint32_t used, 
	 		uint32_t enabledate, uint32_t duedate, uint32_t  nexterrcount);
};

#endif   /* ----- #ifndef CINCREMENT_INCL  ----- */

