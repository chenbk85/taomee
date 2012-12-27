/*
 * =====================================================================================
 *
 *       Filename:  Cchat.cpp
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
#include "Cchat.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include "msglog.h"

//#define INIT_SHM_FLAG  -100 
//create
Cchat::Cchat(mysql_interface * db ) :CtableDate( db, "DB_CHATLOG_0","t_chat_log","userid" )
{ 
 	this->msglog_file=config_get_strval("MSG_LOG_FILE");
	this->chat_log.chat_count=1;
}

int Cchat::add_db( chat_item *p_item  )
{
	char msg_mysql[mysql_str_len(sizeof( p_item->msg))];
	set_mysql_string(msg_mysql,(char*)(p_item->msg), p_item->msglen); 
	this->chat_log.userid=p_item->send_id;
	msglog(this->msglog_file, 0x02020001,time(NULL) ,&(this->chat_log), sizeof(this->chat_log));	

	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,%u,%u,%u,%u,%u,'%s')",
		this->get_table_name(p_item->logtime ),
		p_item->game_id,
		p_item->send_id,
		p_item->recv_id,
		p_item->onlineid,
		p_item->maptype,
		p_item->map_id,
		p_item->logtime,
		p_item->msglen,
		msg_mysql					
	);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}
int Cchat::create_table ( uint32_t logtime )
{
	sprintf( this->sqlstr, "create table if not exists %s (\
		  `game_id` int(10) NOT NULL default '0',\
		  `send_id` int(10) NOT NULL default '0',\
		  `recv_id` int(10) NOT NULL default '0',\
		  `onlineid` int(6) NOT NULL default '0',\
		  `maptype` int(10) NOT NULL default '0',\
		  `map_id` int(10) NOT NULL default '0',\
		  `chat_time` int(10) NOT NULL default '0',\
		  `chat_msg_len` int(5) NOT NULL default '0',\
		  `chat_msg_buf` varchar(1024),\
		  PRIMARY KEY  (send_id, chat_time) \
		)ENGINE=myisam, CHARSET=utf8",
		this->get_table_name(logtime ));
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cchat::add( chat_item *p_item  )
{
	 int ret;	
	 ret=this->add_db(p_item);
	 if ( ret!=SUCC ) {
		this->create_table(p_item->logtime);	
	 	this->add_db(p_item );
	 }
    return ret;		
}
