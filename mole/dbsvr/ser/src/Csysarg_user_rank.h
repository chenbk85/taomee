/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_user_rank.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/15/2010 05:52:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_USR_RANK
#define CSYSARG_USR_RANK

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_user_rank : public Ctable
{
	public:
		Csysarg_user_rank(mysql_interface *db);
		int insert(userid_t userid, uint32_t type, uint32_t rank, uint32_t date);
		int update(userid_t userid, uint32_t type, uint32_t rank, uint32_t date);
		int get_user_rank(userid_t userid, uint32_t type, uint32_t &rank, uint32_t &date);
};
#endif

