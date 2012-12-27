/*
 * =====================================================================================
 *
 *       Filename:  Cregister_serial.cpp
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
#include "Cregister_serial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"


Cregister_serial::Cregister_serial(mysql_interface * db ) 
	:Ctable( db,"REGISTER_SERIAL_DB","t_register_serial")
{ 

}

int Cregister_serial::get_userid (char*  register_serialid , uint32_t *p_userid )
{	
	uint32_t t_useflag;
	sprintf( this->sqlstr, "select useflag ,userid from %s where register_serialid ='%8.8s'", 
		this->get_table_name(),register_serialid );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, SERIAL_ID_NOFIND_ERR );
			t_useflag=atoi_safe(NEXT_FIELD);
			*p_userid=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	if (t_useflag==0)
	{//no used
		sprintf( this->sqlstr, "update %s set  useflag=1   where register_serialid ='%8.8s'", 
		this->get_table_name(),register_serialid );
		STD_SET_RETURN_EX (this->sqlstr,SERIAL_ID_NOFIND_ERR );	
	}
	return DB_SUCC;
}
