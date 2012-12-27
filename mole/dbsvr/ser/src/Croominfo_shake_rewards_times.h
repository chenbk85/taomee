/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_shake_rewards_times.h
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

#ifndef CROOMINFO_REWARDS
#define CROOMINFO_REWARDS
#include "proto.h"
#include "benchapi.h"
#include "CtableRoute10x10.h"

class Croominfo_shake_rewards_times : public CtableRoute10x10
{
	public:
		Croominfo_shake_rewards_times(mysql_interface *db);
		int insert(userid_t userid);
		int get(userid_t userid, uint32_t *num);
		int update(userid_t userid);
		//int del_record(userid_t userid);
		int clear_times(userid_t userid);
};
#endif

