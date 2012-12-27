/*
 * =====================================================================================
 *
 *       Filename:  Cuser_synth_history.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/04/2011 09:42:56 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_SYNTH_HISTORY_IC
#define CUSER_SYNTH_HISTORY_IC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 

class Cuser_synth_history:public CtableRoute100x10{
	public:
		Cuser_synth_history(mysql_interface *db);
		int insert(userid_t userid, uint32_t id, uint32_t type, uint32_t count);
		int update(userid_t userid, uint32_t id, uint32_t type ,uint32_t count);
};

#endif
