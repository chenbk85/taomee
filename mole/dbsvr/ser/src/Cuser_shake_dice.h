/*
 * =====================================================================================
 *
 *       Filename:  Cuser_shake_dice.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/19/2011 05:09:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_SHAKE_DICE_INCL
#define CUSER_SHAKE_DICE_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_shake_dice:public CtableRoute100x10{

    public:
        Cuser_shake_dice(mysql_interface *db);
		int update(userid_t userid, const char* col, uint32_t value);
		int query_time_flag(userid_t userid, uint32_t *last_time, uint32_t *six_flag, uint32_t *last_date);
        int insert(userid_t userid, uint32_t last_time, uint32_t six_flag);
};

#endif

