/*
 * =====================================================================================
 *
 *       Filename:  Cgf_stat_info.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/27/2012 02:16:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#ifndef CGF_STAT_INFO_H
#define CGF_STAT_INFO_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"


class Cgf_stat_info: public CtableRoute
{
public:	
	Cgf_stat_info( mysql_interface * db );
	int add_player_stat_val(userid_t userid, uint32_t role_regtime, uint32_t stat_id, uint32_t add_cnt);
	int list_player_stat_val(userid_t userid, uint32_t role_regtime, gf_get_stat_info_out_item **items, uint32_t * p_count);
private:
	int get_stat_val(userid_t userid, uint32_t role_regtime, uint32_t stat_id, uint32_t * val);
	int insert_stat_val(userid_t userid, uint32_t role_regtime, uint32_t stat_id, uint32_t cnt);
};





#endif


