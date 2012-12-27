/*
 * =====================================================================================
 *
 *       Filename:  Cgroup_main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Cgroup_main.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
Cgroup_main::Cgroup_main(mysql_interface * db ) :Ctable( db, "GROUP_MAIN","t_group_main" )
{ 

}

int Cgroup_main::insert(uint32_t * p_groupid )
{
	sprintf( this->sqlstr, "insert into %s values(0)", 
		this->get_table_name());
	STD_INSERT_GET_ID (this->sqlstr, DB_ERR, *p_groupid  );
}
