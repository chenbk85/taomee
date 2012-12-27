/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_consume_money.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/20/2010 02:27:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CROOMINFO_CONSUME_MONEY
#define CROOMINFO_CONSUME_MONEY
#include "proto.h"
#include "benchapi.h"
#include "CtableRoute10x10.h"

class Croominfo_consume_money: public CtableRoute10x10
{
	public:
		Croominfo_consume_money(mysql_interface *db);
		int insert(userid_t userid, uint32_t value);
		int get(userid_t userid, uint32_t *value);
		int update_inc(userid_t userid, uint32_t value);
		int update_dec(userid_t userid, uint32_t value);
};
#endif

