/*
 * =====================================================================================
 *
 *       Filename:  Cuser_temp_team.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/20/2012 10:51:48 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_TEMP_TEAM
#define CUSER_TEMP_TEAM
#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
class Cuser_temp_team:public CtableRoute100x10
{
public:
	Cuser_temp_team(mysql_interface *db);
	int insert(userid_t userid, const char* teamname, uint32_t teamid, uint32_t logo);
	int add(userid_t userid,const char* teamname, uint32_t teamid, uint32_t logo);
	int get_one(userid_t userid, const char* col, uint32_t *data);
	int set_one(uint32_t userid, const char* col, uint32_t data);
	int get_teamname(userid_t userid, char *data);
	int update(userid_t userid,const char* name,uint32_t id, uint32_t logo);
	int remove(userid_t userid);
	int get_teamid(userid_t userid,uint32_t* teamid);
	int get_teaminfo(uint32_t userid, user_get_teaminfo_out_header *out);
	int update_badge(userid_t userid,uint32_t teamid,uint32_t badge);
};
#endif
