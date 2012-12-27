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
#include "Cclass_id.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
Cclass_id :: Cclass_id(mysql_interface * db) : Ctable(db, "CLASS_ID","t_class_id")
{ 

}

int Cclass_id :: insert(uint32_t * p_groupid )
{
	sprintf( this->sqlstr, "insert into %s values(0)", 
			 this->get_table_name()
		   );
	STD_INSERT_GET_ID (this->sqlstr, DB_ERR, *p_groupid  );
}
