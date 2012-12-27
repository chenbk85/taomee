/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_sock.h
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

#ifndef CROOMINFO_SOCK
#define CROOMINFO_SOCK
#include "proto.h"
#include "benchapi.h"
#include "CtableRoute10x10.h"

class Croominfo_sock: public CtableRoute10x10
{
	public:
		Croominfo_sock(mysql_interface *db);
		int insert(userid_t userid, uint32_t count, uint32_t date);
		int get(userid_t userid, uint32_t& count, uint32_t& date);
		int update_by_date(userid_t userid, uint32_t date, uint32_t count);
		int del(userid_t userid);
};
#endif

