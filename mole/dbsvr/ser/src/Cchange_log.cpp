/*
 * =====================================================================================
 *
 *       Filename:  CCHANGE_LOG.cpp
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
#include "Cchange_log.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//#define INIT_SHM_FLAG  -100 
//create
Cchange_log::Cchange_log(mysql_interface * db ) :CtableDate( db, "DB_CHATLOG_0","change_log","userid" )
{ 

}

int Cchange_log::add_db( change_log_item *p_item  )
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,%d,%d,%d )",
		this->get_table_name(p_item->logtime ),
		p_item->change_type,p_item->userid,p_item->logtime, p_item->change_value,
		p_item->change_v1,p_item->change_v2);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cchange_log::update_db( change_log_item *p_item  )
{
	sprintf( this->sqlstr, "update  %s set change_value = change_value + %u where userid = %u and change_type = %u",
		this->get_table_name(p_item->logtime ),
		p_item->change_value,
		p_item->userid,
		p_item->change_type);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);	
}


int Cchange_log::create_table ( uint32_t logtime )
{
	sprintf( this->sqlstr, "create table if not exists %s (\
			change_type INT UNSIGNED NOT NULL,\
			userid      INT UNSIGNED NOT NULL,\
			logtime     INT UNSIGNED NOT NULL,\
			change_value  INT NOT NULL,\
			v1		 	INT  NOT NULL,\
			v2			INT  NOT NULL,\
		  	KEY i_log_id 	(userid,change_type),\
		  	KEY i_log_id_2 	(userid)\
		)ENGINE=myisam, CHARSET=utf8",
		this->get_table_name(logtime ));
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cchange_log::add( change_log_item *p_item  )
{
	 int ret;

	 if (p_item->change_type >= 10000)
	 {
		 ret = this->update_db(p_item);
		 if (ret != SUCC)
		 {
	 		ret=this->add_db(p_item);
	 		if ( ret!=SUCC ) 
			{
				this->create_table(p_item->logtime);	
	 			ret=this->add_db(p_item );
	 		} 
	 	}
	 }
	 else{
	 	ret=this->add_db(p_item);
	 	if ( ret!=SUCC ) {
			this->create_table(p_item->logtime);	
	 		ret=this->add_db(p_item );
	 	}
	 }

     return ret;		
}



