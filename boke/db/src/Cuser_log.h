/*
 * =========================================================================
 *
 *        Filename: Cuser_log.h
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
#ifndef  CUSER_LOG_H
#define  CUSER_LOG_H

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"


class Cuser_log:public CtableRoute100x10
{
private:
public:
	Cuser_log (mysql_interface * db);
	int	insert(userid_t userid, user_log_t * p_in );
	int	get_list(userid_t userid, std::vector<user_log_t> &user_log_list );
};

#endif  /*CUSER_LOG_H*/
