/*
 * =====================================================================================
 *
 *       Filename:  Creport_deal_history.cpp
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
#include "Creport_deal_history.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
Creport_deal_history::Creport_deal_history(mysql_interface * db ) :Ctable( db, "USERMSG_DB","t_report_deal_history" )
{ 

}

int Creport_deal_history::insert( uint32_t userid, uint32_t adminid,  
		uint32_t  deal_type,uint32_t reason, char * nick)
{
	char date_str[30];
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,nick,NICK_LEN);
	mysql_date( date_str, time(NULL),sizeof(date_str));
	sprintf( this->sqlstr, "insert into %s values (0,'%s',%u,%u,%u,%u,'%s')", 
			this->get_table_name(), date_str ,
			userid, adminid ,  deal_type, reason ,nick_mysql );
	STD_INSERT_RETURN(this->sqlstr, EMAIL_EXISTED_ERR);	
}

int Creport_deal_history::get_list(userid_t objuserid , 
			usermsg_get_deal_report_list_in *p_in, uint32_t * p_count,  
			 usermsg_get_deal_report_list_item ** pp_item )
{
	char userid_str[100];
	if (objuserid==0){
		strcpy(userid_str," 1=1 " );
	}else{
		sprintf(userid_str,"userid=%u", objuserid);

	}
	sprintf( this->sqlstr, "select logdate,userid,adminid,deal_type,reason,nick from %s \
     	where   logdate>=%u and logdate<=%u  and %s order by logdate desc \
		Limit %u,50", 
		this->get_table_name(), p_in->startdate,p_in->enddate,userid_str ,p_in->index);
	STD_QUERY_WHILE_BEGIN(this-> sqlstr, pp_item,p_count );
		memset((*pp_item+i)->date ,0, sizeof((*pp_item+i)->date));
		strcpy((*pp_item+i)->date ,NEXT_FIELD);
		(*pp_item+i)->objuserid=atoi_safe(NEXT_FIELD); 
		(*pp_item+i)->adminid=atoi_safe(NEXT_FIELD); 
		(*pp_item+i)->deal_type=atoi_safe(NEXT_FIELD); 
		(*pp_item+i)->reason=atoi_safe(NEXT_FIELD); 
		BIN_CPY_NEXT_FIELD((*pp_item+i)->nick,NICK_LEN );
    STD_QUERY_WHILE_END();
}
