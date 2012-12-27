/*
 * =====================================================================================
 * 
 *       Filename:  dbdeal.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 16时40分57秒 CST
 *        PRIu64
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CROUTE_FUNC_INC
#define  CROUTE_FUNC_INC
#include "Cfunc_route_db_v2.h"
#include "Cuser.h"
#include "Ctask.h"
#include "Cfind_map.h"
#include "Citem.h"
#include "Cuser_log.h"
#include "Cday.h"
#include "Cgame.h"

/*
 * =====================================================================================
 *        Class:  Croute_func
 *  Description:  
 * =====================================================================================
 */

class Croute_func:public Cfunc_route_db_v2
{
  private:
	Cuser user;
	Ctask task;
	Citem item;
	Cfind_map find_map;
	Cuser_log user_log;
	Cday  day;
	Cgame game;
  public:
	Croute_func (mysql_interface * db); 
//PROTO_BEGIN
  
//alsdfkas dfa f

//PROTO_END
#define PROTO_FUNC_DEF(cmd_name)\
	int cmd_name(DEAL_FUNC_ARG);
	#include "./proto/pop_db_func_def.h"
}; /* -----  end of class  Croute_func  ----- */

#endif   /* ----- #ifndef CROUTE_FUNC_INC  ----- */

