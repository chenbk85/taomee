/*
 * =====================================================================================
 *
 *       Filename:  Citem_handbook.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/03/2010 04:22:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

 
#ifndef  CITEM_HANDBOOK_INC
#define  CITEM_HANDBOOK_INC
 
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"
  
#include <time.h>
#include <algorithm>
 

 

#define ITEM_HANDBOOK_STATE_ON	1 
 
 
 
class Citem_handbook : public CtableRoute
{
public:
	Citem_handbook(mysql_interface * db);
 
	int insert(uint32_t userid, uint32_t itemid, uint32_t state, uint32_t count);

	int state_set(uint32_t userid, uint32_t itemid, uint32_t state);
 
	int count_add(userid_t userid, uint32_t itemid, uint32_t count);
 
	int list_get(userid_t userid, stru_mole2_item_handbook_info** pp_out_item, uint32_t* p_count);

	int state_get(uint32_t userid, uint32_t itemid, uint32_t* p_state);

 
};
 
#endif

