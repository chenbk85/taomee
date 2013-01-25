/*
 * =====================================================================================
 *
 *       Filename:  Cmonster_handbook.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/03/2010 04:23:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef  CMONSTER_HANDBOOK_INC
#define  CMONSTER_HANDBOOK_INC
  
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"

#include <time.h>
#include <algorithm>


  
 
#define MONSTER_HANDBOOK_STATE_ON	1  
  
  
  
class Cmonster_handbook : public CtableRoute
{
public:
	Cmonster_handbook(mysql_interface * db);
  
	int insert(uint32_t userid, uint32_t monsterid, uint32_t state, uint32_t count);

	int state_set(uint32_t userid, uint32_t monsterid, uint32_t state);
  
	int count_add(userid_t userid, uint32_t monsterid, uint32_t count);
	
 	int list_get(uint32_t userid, stru_mole2_monster_handbook_info **pp_out_item, uint32_t* p_count);

	int list_range_get(userid_t userid, uint32_t min,uint32_t max,stru_mole2_monster_handbook_info** pp_out_item, uint32_t* p_count);

	int state_get(uint32_t userid, uint32_t monsterid, uint32_t* p_state);

	int get_beasts(userid_t userid, std::vector<stru_beast_book> &beasts,uint32_t min,uint32_t max);
};
  
#endif

