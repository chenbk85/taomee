/*
 * =====================================================================================
 *
 *       Filename:  Cuser_map.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/02/2010 06:18:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef  CUSER_MAP_INC
#define  CUSER_MAP_INC

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"
 
#include <time.h>
#include <algorithm>



#define MAP_FLIGHT_PATH_ON	0x01
#define MAP_BOSS_POSITION_ON	0x02




class Cuser_map : public CtableRoute
{
public:
	Cuser_map(mysql_interface * db);

	int insert(uint32_t userid, uint32_t mapid, uint32_t flight);

	int flight_list_get(userid_t userid, user_map_flight_list_get_out_item** pp_out_item, uint32_t* p_count);

	int flight_get(uint32_t userid, uint32_t mapid, uint32_t* p_flight);

	int flight_set(uint32_t userid, uint32_t mapid, uint32_t flight);

	int get_mapstates(userid_t userid, std::vector<stru_map_state> &states);
};

#endif
