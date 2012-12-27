/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angel_capture.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/15/2011 10:54:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_ANGEL_CAPTURE_INC
#define  CUSER_ANGEL_CAPTURE_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_angel_capture:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_angel_capture(mysql_interface *db);
		int insert(userid_t userid, uint32_t angelid, uint32_t count, uint32_t date);
		int update(userid_t userid, uint32_t angelid, uint32_t count, uint32_t date);
		int select(userid_t userid, uint32_t angelid, uint32_t &count, uint32_t &date);	
		int select_total(userid_t userid, uint32_t **pp_list, uint32_t *p_count);	
		int capture_angel_rand(uint32_t &partition,uint32_t &dinominator);
		int capture_angel_rand(uint32_t total, uint32_t base_point, double beg, 
				uint32_t &partition, uint32_t &dinominator);		
		int select_date(userid_t userid, uint32_t angelid, uint32_t &date);
};




#endif
