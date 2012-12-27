/*
 * =====================================================================================
 *
 *       Filename:  Cuser_as_common.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/05/2011 03:56:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_AS_COMMON_INC
#define CUSER_AS_COMMON_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_as_common:public CtableRoute100x10
{
	private:
		int insert(userid_t userid, uint32_t type, uint32_t data);	
		int update(userid_t userid, uint32_t type, uint32_t data);
	public:
		Cuser_as_common(mysql_interface *db);
		int add(userid_t userid, uint32_t type, uint32_t data);
		int get_one(userid_t userid, uint32_t type, uint32_t *data);

};
#endif
