/*
 * =====================================================================================
 *
 *       Filename:  Cuser_ocean_info.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/03/2012 10:08:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_OCEAN_INFO_INCL
#define CUSER_OCEAN_INFO_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_ocean_info:public CtableRoute100x10{
	private:
		int insert(userid_t userid, uint32_t exp);
		int update(userid_t userid, uint32_t exp);
	public:
		Cuser_ocean_info(mysql_interface *db);
		int add(userid_t userid, uint32_t exp);
		int get_exp(userid_t userid, uint32_t *exp);
		int set_exp(userid_t userid, uint32_t exp);
		int get(userid_t userid ,uint32_t* exp, uint32_t* datetime);
		int set(userid_t userid, uint32_t exp, uint32_t datetime);
		int update_coin(userid_t userid, uint32_t coin);
		int get_coin(userid_t userid, uint32_t *coin);
		int set_coin(userid_t userid, uint32_t coin);

};
#endif
