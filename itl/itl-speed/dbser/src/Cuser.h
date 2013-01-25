/*
 * =========================================================================
 *
 *        Filename: Cuser.h
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:29:51
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#ifndef  CUSER_H
#define  CUSER_H

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser:public CtableRoute100x10
{
private:
public:
	Cuser (mysql_interface * db);
	int get_all_info(userid_t userid  , pop_login_out* p_out);
	int	insert(userid_t userid, pop_reg_in *p_in );

};

#endif  /*CUSER_H*/
