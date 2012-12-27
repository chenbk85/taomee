/*
 * =====================================================================================
 *
 *       Filename:  Cmsg_attime.cpp
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
#include "Cmsg_attime.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
Cmsg_attime::Cmsg_attime(mysql_interface * db ) :Ctable( db, "SU_CHANGE_DB","t_msg_attime" )
{ 

}

int Cmsg_attime::insert( uint32_t deal_date, uint32_t deal_hour, uint32_t deal_minute, 
		uint32_t flag,uint32_t pic_id , msg_item *msg) 
{
	char msg_mysql[mysql_str_len(4004)];
	set_mysql_string(msg_mysql,(char*)(msg), msg->itemlen+4); 
	DEBUG_LOG("msg flag:%u",flag );

	sprintf(this->sqlstr, "insert into %s values (%u,%u,%u,%u,%u,'%s')", 
			this->get_table_name(), deal_date ,deal_hour , 
			deal_minute,flag,pic_id, msg_mysql   );
	STD_INSERT_RETURN(this->sqlstr, EMAIL_EXISTED_ERR);	
}


int Cmsg_attime::get(  su_get_msg_attime_all_in *p_in ,su_get_msg_attime_all_out *p_out  )
{
	sprintf( this->sqlstr, "select deal_date, deal_hour,  deal_minute, flag,pic_id,deal_msg from %s \
			Limit %u,1", 
			this->get_table_name(), p_in->index  );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,NO_DEFINE_ERR );
			p_out->deal_date =atoi_safe(NEXT_FIELD);
			p_out->deal_hour=atoi_safe(NEXT_FIELD);
			p_out->deal_minute=atoi_safe(NEXT_FIELD);
			p_out->flag=atoi_safe(NEXT_FIELD);
			p_out->pic_id=atoi_safe(NEXT_FIELD);
			BIN_CPY_NEXT_FIELD (&(p_out->msglen), 4004);
	STD_QUERY_ONE_END();
}	

int Cmsg_attime::get_by_date( su_get_msg_attime_by_date_in *p_in,
	   	su_get_msg_attime_all_out *p_out  )
{
	sprintf( this->sqlstr, "select deal_date, deal_hour,  deal_minute, flag,pic_id, deal_msg from %s \
			where ( deal_date=0 or deal_date=%u) and deal_hour=%u and deal_minute=%u limit %u,1 ", 
			this->get_table_name(),  p_in->deal_date,
			p_in->deal_hour,p_in->deal_minute,p_in->index );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,NO_DEFINE_ERR );
			p_out->deal_date =atoi_safe(NEXT_FIELD);
			p_out->deal_hour=atoi_safe(NEXT_FIELD);
			p_out->deal_minute=atoi_safe(NEXT_FIELD);
			p_out->flag=atoi_safe(NEXT_FIELD);
			p_out->pic_id=atoi_safe(NEXT_FIELD);
			BIN_CPY_NEXT_FIELD (&(p_out->msglen), 4004);
	STD_QUERY_ONE_END();
}


int Cmsg_attime::remove(  uint32_t deal_date, uint32_t deal_hour, uint32_t deal_minute )
{
	sprintf( this->sqlstr, "delete from %s \
			where deal_date=%u and deal_hour=%u and deal_minute=%u", 
			this->get_table_name(), deal_date,deal_hour,deal_minute );
	STD_SET_RETURN_EX(this->sqlstr ,NO_DEFINE_ERR);
}	

