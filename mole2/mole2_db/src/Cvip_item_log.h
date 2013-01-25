
/*
 * =====================================================================================
 * 
 *       Filename:  Cpet.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  g++
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CVIP_ITEM_LOG_INC
#define  CVIP_ITEM_LOG_INC

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"
#include <time.h>
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"

#include <libtaomee++/utils/tcpip.h>

class Cvip_item_log: public Ctable
{
public:
	static Cudp_sender *p_sender;
public:
	Cvip_item_log(mysql_interface * db);
	int add(uint32_t logtype, uint32_t itemid, int32_t count);
	void log(userid_t userid, uint32_t itemid, int32_t count);
};

#endif /* ----- #ifndef CPET_INC  ----- */

