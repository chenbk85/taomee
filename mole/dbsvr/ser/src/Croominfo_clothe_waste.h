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

class Croominfo_clothe_waste : public CtableRoute10x10 {
	public:
		Croominfo_clothe_waste(mysql_interface *db);
		
		int update(userid_t userid, uint32_t clothe_type, uint32_t value);

		int insert(userid_t userid, uint32_t type, int32_t init_value);

		int set_value(userid_t userid, uint32_t clothe_type, int32_t value, uint32_t *p_back);
		
		int get_value(userid_t userid, uint32_t clothe_type, uint32_t *p_value);

		int get(userid_t userid, uint32_t clothe_type, uint32_t *p_value);

		int get_value_all(userid_t userid, pair_clothe *p_array, uint32_t *p_count);

		int set_value_ex(userid_t userid, roominfo_set_clothe_waste_out *p_in, roominfo_set_clothe_waste_out *p_out);
};
