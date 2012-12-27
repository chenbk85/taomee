/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_noah_info.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年02月26日 15时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_NOAH_INFO_INCL
#define  CUSER_NOAH_INFO_INCL
#include "CtableRoute10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_noah_info : public CtableRoute10 {
public:
	Cuser_noah_info(mysql_interface * db); 

	int insert_user_noah_info(userid_t userid, uint32_t last_time, user_noah_add_user_info_in &st_info); 

	int get_user_noah_info_sum(userid_t userid, char *sn, uint32_t last_time, user_noah_user_info& st_info  );
	
	int get_user_xiaomee_sum_week(userid_t userid, char *sn, uint32_t last_week, uint32_t& uixiaomee);

};

#endif   /* ----- #ifndef CUSER_NOAH_INFO_INCL  ----- */
