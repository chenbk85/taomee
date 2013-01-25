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

#ifndef  CTASK_HANDBOOK_INC
#define  CTASK_HANDBOOK_INC
  
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"

#include <algorithm>


  
 
  
  
  
class Ctask_handbook : public CtableRoute
{
public:
	Ctask_handbook(mysql_interface * db);
  
	int insert(uint32_t userid, uint32_t taskid, const char* content);

	int content_set(uint32_t userid, uint32_t taskid, const char* content);

	int content_get(uint32_t userid, uint32_t taksid, char* sz_content);
  

  
};
  
#endif

