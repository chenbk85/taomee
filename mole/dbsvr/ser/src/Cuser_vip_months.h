/*
 * =====================================================================================
 *
 *       Filename:  Cuser_vip_months.h
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

#ifndef CUSER_VIP_MONTHS_INCL
#define CUSER_VIP_MONTHS_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_vip_months:public CtableRoute100x10{

    public:
        Cuser_vip_months(mysql_interface *db);
        int add(userid_t userid, uint32_t type, uint32_t months);
        int query(userid_t userid, uint32_t type, const char* col, uint32_t *value);
        int select(userid_t userid, uint32_t type, uint32_t *months, uint32_t *receive);
        int update(userid_t userid, uint32_t type, const char* col, uint32_t value);
		int update_inc(userid_t userid, uint32_t type, const char* col, uint32_t value);
    private:
        int insert(userid_t userid, uint32_t type, uint32_t count, uint32_t flag);
        int update_all(userid_t userid, uint32_t type, uint32_t count, uint32_t receive);
            
};
#endif

