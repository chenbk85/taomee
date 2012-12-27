/*
 * =====================================================================================
 *
 *       Filename:  Cuser_temp_decorate.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/13/2012 05:20:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_TEMP_DECORATE_INC
#define CUSER_TEMP_DECORATE_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_temp_decorate:public CtableRoute100x10
{
	private:
    int insert(userid_t userid, char* value);
	int update(userid_t userid, char* value);
	public:
	Cuser_temp_decorate(mysql_interface *db);
	int add(userid_t userid, char* value);
	int get_one(userid_t userid, char *value);
};
#endif
