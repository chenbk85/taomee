/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_gen_vip.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:21:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"


class Croominfo_flower: public CtableRoute10x10 {
public:
	Croominfo_flower(mysql_interface *db);

	int update(userid_t userid, uint32_t flag, uint32_t type, int32_t value);

	int insert(userid_t userid, uint32_t flag, uint32_t type, int32_t value);

	int get_value(userid_t userid, uint32_t *p_flag, uint32_t *p_type, int32_t *p_count);
};
