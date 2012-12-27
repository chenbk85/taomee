/*
 * =====================================================================================
 *
 *       Filename:  Cuser_exhange_bean.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/12/2011 09:25:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CUSER_EXCHANGE_BEAN_INC
#define CUSER_EXCHANGE_BEAN_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_exchange_bean:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_exchange_bean(mysql_interface *db);
		int insert(userid_t userid, uint32_t count);
		int update(userid_t userid, uint32_t count);
		int select(userid_t userid, uint32_t &count);
};

#endif
