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
#include "Cparty.h"
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
Cparty::Cparty(mysql_interface * db ) :Ctable( db, "PARTY_DB","t_party" )
{ 

}

int Cparty::add_db( party_add_party_in *p_item  )
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,p_item->ownernick,sizeof(p_item->ownernick) );

	sprintf( this->sqlstr, "insert into %s values (%u, %u,%u,%u,%u,%u,%u,%u,'%s' )",
		this->get_table_name(),get_date(p_item->partytime) ,
		p_item->serverid,p_item->partytime,p_item->mapid, p_item->partytype,
		p_item->ownerid,p_item->ownerflag,p_item->ownercolor,nick_mysql );
	STD_INSERT_RETURN(this->sqlstr, DB_ERR);	
}

int Cparty::check_count(uint32_t partydate, uint32_t  serverid)
{
	uint32_t count;
	int ret;
	ret=this->get_count(partydate,serverid,&count );
	if (ret!=SUCC) return ret;
	if (count>=200 ){
		return  SERVER_PER_DAY_PARTY_MAX_ERR;
	}
	return SUCC;

}

int Cparty::add( party_add_party_in *p_item  )
{
	 int ret;	
	 ret=this->check_count(get_date( p_item->partytime),p_item->serverid);
	 if(ret==SERVER_PER_DAY_PARTY_MAX_ERR) {
		 return ret;
	 }
	return  this->add_db(p_item);
}

int Cparty::get_count(uint32_t partydate, uint32_t  serverid,uint32_t *p_count )
{
	sprintf( this->sqlstr, "select  count(1)\
			from %s where partydate=%u and serverid=%u ", 
			this->get_table_name(),partydate, serverid );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,SUCC);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}
int Cparty::get_count_by_time(uint32_t partytime, uint32_t  serverid,uint32_t *p_count )
{
	sprintf( this->sqlstr, "select  count(1)\
			from %s where partydate=%u and  partytime>%u  and serverid=%u ", 
			this->get_table_name(),get_date(partytime), partytime , serverid );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,SUCC);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}


int Cparty::get_partylist(uint32_t serverid,  uint32_t index , uint32_t pagecount  ,
	 uint32_t *count, party_get_partylist_out_item ** pp_list )
{
	uint32_t now=time(NULL);
	sprintf( this->sqlstr,"select partytime,mapid,partytype,ownerid,ownerflag,\
			ownercolor,ownernick from  %s \
			where partydate=%u and partytime>%u  and  serverid=%u  limit %u,%u " , 
		this->get_table_name(), get_date(now), now, serverid , index, pagecount  ); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, count ) ;
		INT_CPY_NEXT_FIELD((*pp_list+i)->partytime );
		INT_CPY_NEXT_FIELD((*pp_list+i)->mapid );
		INT_CPY_NEXT_FIELD((*pp_list+i)->partytype );
		INT_CPY_NEXT_FIELD((*pp_list+i)->ownerid );
		INT_CPY_NEXT_FIELD((*pp_list+i)->ownerflag );
		INT_CPY_NEXT_FIELD((*pp_list+i)->ownercolor );
		BIN_CPY_NEXT_FIELD((*pp_list+i)->ownernick,sizeof((*pp_list+i)->ownernick) );
	STD_QUERY_WHILE_END();
}

int Cparty::get_partylist_by_ownerid(uint32_t ownerid, 
	 uint32_t *count, party_getlist_by_ownerid_out_item ** pp_list )
{
	sprintf( this->sqlstr,"select serverid, partytime,mapid,partytype \
		   	from  %s where  ownerid=%u limit 0,50 " , 
		this->get_table_name(), ownerid ); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, count ) ;
		INT_CPY_NEXT_FIELD((*pp_list+i)->serverid);
		INT_CPY_NEXT_FIELD((*pp_list+i)->partytime );
		INT_CPY_NEXT_FIELD((*pp_list+i)->mapid );
		INT_CPY_NEXT_FIELD((*pp_list+i)->partytype );
	STD_QUERY_WHILE_END();
}

