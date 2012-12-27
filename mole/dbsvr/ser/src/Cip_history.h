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

class Cip_history : public CtableRoute10x10 {
	public:
		Cip_history(mysql_interface *db);
		
		int update(userid_t userid, uint32_t which_game, uint32_t ip1, uint32_t ip2, uint32_t ip3);

		int add_ip(userid_t userid, uint32_t which_game, uint32_t ip);

		int insert(userid_t userid, uint32_t which_game, uint32_t ip);
		int  get_last_ip(userid_t userid,  uint32_t *p_ip );
		int  update_last_ip(userid_t userid, uint32_t ip);
		int get_ip(userid_t userid, uint32_t which_game, three_ip_history *p_count);
};
