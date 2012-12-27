/*
 * =====================================================================================
 *
 *       Filename:  Cdamee_history.cpp
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
#include "Cdamee_history.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"

//create
Cdamee_history::Cdamee_history(mysql_interface * db ) 
	:CtableMonth( db,"INCREMENT_DB","t_damee_history","transid")
{ 

}

int Cdamee_history::gen_record( uint32_t date,uint32_t paytype ,userid_t userid,
			int damee, uint32_t leave_damee  , char * pri_msg )
{
	char mysql_private_msg [mysql_str_len (20)];
	set_mysql_string(mysql_private_msg,set_space_end(pri_msg,20), 20);
	sprintf(this->sqlstr, "insert into %s values (0,%u,%u,%d,%d,%u,'%s')", 
			this->get_table_name(),date,userid,
			paytype,damee,leave_damee, mysql_private_msg); 
	STD_INSERT_RETURN(this->sqlstr, TRANS_ID_EXISTED_ERR);	
}

int Cdamee_history::get_record(userid_t userid,uint32_t startdate, uint32_t enddate ,
				uint32_t startindex, uint32_t precount , 
				PAY_GET_DAMEE_LIST_OUT * p_out)
{
	int ret;
	uint32_t startdate_loop,enddate_loop;
	uint32_t count_loop,precount_loop,startindex_loop;
	DAMEE_RECORD*p_record_loop;
	if (startdate < 20080101 ) startdate=20080101 ;  
	if (enddate/100 > (uint32_t)get_year_month(time(NULL))) enddate=get_date(time(NULL))+1;
	uint32_t startmonth=startdate/100;	
	uint32_t endmonth=enddate/100;	

	p_out->count=0;
	for (uint32_t i=startmonth; i<=endmonth;i= change_date(i,1)){
		if (i==startmonth){
			startdate_loop=startdate;
			startindex_loop=startindex;
		} else {
			startdate_loop=i*100;
			startindex_loop=0;
		}

		if (i==endmonth) enddate_loop=enddate;
		else enddate_loop=(i+1)*100;
		precount_loop=precount- (p_out->count);

		ret=this->get_record(userid, startdate_loop, enddate_loop, 
				startindex_loop,
			   precount_loop,&count_loop,&p_record_loop	);
		if (ret==SUCC){
			memcpy(( p_out->damee_item +p_out->count ),
					p_record_loop,sizeof(*p_record_loop)*count_loop);
			 p_out->count +=count_loop;
			 //free memory
			free(p_record_loop);

			if (p_out->count==precount){//足够了
				p_out->next_startdate=startdate_loop;
				p_out->next_startindex=startindex_loop+precount_loop;
				return SUCC;					
			}	
		}
	}

	p_out->next_startdate=0;
	p_out->next_startindex=0;
	return SUCC;
}


int Cdamee_history::get_record(userid_t userid,uint32_t startdate, uint32_t enddate ,
				uint32_t startindex, uint32_t precount , uint32_t*p_count,	
				DAMEE_RECORD**pp_record)
{
	sprintf( this->sqlstr, "select date,paytype,damee,leave_damee,private_msg from %s \
		where userid=%u  and date>=%u and date<=%u  Limit  %u,%u  ", 
		this->get_table_name(startdate/100 ), userid,
	   	get_time_t (startdate), get_time_t(enddate)+3600*24 , startindex,precount ); 

	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_record, p_count ) ;
		(*pp_record+i)->date=atoi_safe(NEXT_FIELD ); 
		(*pp_record+i)->paytype=atoi_safe(NEXT_FIELD ); 
		(*pp_record+i)->damee =atoi_safe(NEXT_FIELD ); 
		(*pp_record+i)->leave_damee =atoi_safe(NEXT_FIELD ); 
		memcpy( (*pp_record+i)-> desc,NEXT_FIELD, sizeof((*pp_record+i)-> desc ) );
		set_space_end((*pp_record+i)-> desc, sizeof((*pp_record+i)-> desc ));
	STD_QUERY_WHILE_END();
}

