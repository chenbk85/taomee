/*
 * =========================================================================
 *
 *        Filename: Ctask.h
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
#ifndef  CTASK_H
#define  CTASK_H

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"


class Ctask:public CtableRoute100x10
{
private:
public:
	Ctask (mysql_interface * db);
	int	insert(userid_t userid,  uint32_t taskid , uint32_t task_nodeid);
	int	get_list(userid_t userid, std::vector<task_t> &task_list );
	int	del(userid_t userid, uint32_t taskid , uint32_t task_nodeid);

};

#endif  /*CTASK_H*/
