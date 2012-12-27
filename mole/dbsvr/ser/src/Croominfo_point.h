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

#define POINT_VALUE "point_value"
#define POS_VALUE "pos_value"

class Croominfo_point : public CtableRoute10x10 {
	public:
		Croominfo_point(mysql_interface *db);
	
		int update(userid_t userid, uint32_t value, const char *field_str);

		int insert(userid_t userid);

		int set_value(userid_t userid, int32_t value, const char *field_str);

		int get_value(userid_t userid, uint8_t *p_value, const char *field_str);

		int get_value_ex(userid_t userid, uint32_t *p_value, const char *field_str); 

		int get(userid_t userid, uint32_t *p_value, const char *field_str);
		
		int get_task(userid_t userid, uint32_t *p_value, const char *field_str);

		int set_task(userid_t userid, uint32_t task_id, uint32_t opt);

		int get_water_list( userid_t userid, mms_water_list *p_out);

		int update_water_list( userid_t userid, mms_water_list *p_list);

		int set_water_list(userid_t userid, userid_t other_id);
};
