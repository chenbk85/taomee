/*
 * =====================================================================================
 *
 *       Filename:  Cuser_fa_speed_tool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/20/2011 01:45:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_FARM_SPEED_TOOL
#define CUSER_FARM_SPEDD_TOOL


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "algorithm"

class Cuser_farm_speed_tool:public CtableRoute100x10
{
	public:
		Cuser_farm_speed_tool(mysql_interface *db);
		int insert(userid_t userid, uint32_t id, uint32_t animalid, uint32_t toolid,
				uint32_t eat_count, uint32_t date);
		int update(userid_t userid, uint32_t id ,uint32_t toolid, uint32_t count, uint32_t date);
		int get_date_count(userid_t userid, uint32_t id, uint32_t toolid, uint32_t &count, uint32_t&date);
		int get_date(userid_t userid, uint32_t id, uint32_t toolid, uint32_t&date);
		int get_date(userid_t userid, uint32_t id, uint32_t toolid1, uint32_t toolid2, uint32_t&date);
		int del_record(userid_t userid, uint32_t id);

};

#endif
