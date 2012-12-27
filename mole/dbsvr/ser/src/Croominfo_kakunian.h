/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_kakunian.h
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

#ifndef CROOMINFO_KAKUNIAN
#define CROOMINFO_KAKUNIAN
#include "proto.h"
#include "benchapi.h"
#include "CtableRoute10x10.h"

class Croominfo_kakunian: public CtableRoute10x10
{
	public:
		Croominfo_kakunian(mysql_interface *db);
		int insert(userid_t userid, uint32_t count);
		int get(userid_t userid, uint32_t& count);
		int update(userid_t userid, uint32_t count);
		int del(userid_t userid);
};
#endif

