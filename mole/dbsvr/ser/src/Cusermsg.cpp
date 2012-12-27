/*
 * =====================================================================================
 *
 *       Filename:  Cusermsg.cpp
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
#include "Cusermsg.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
Cusermsg::Cusermsg(mysql_interface * db ) :Ctable( db, "USERMSG_DB","t_usermsg" )
{ 

}

int Cusermsg::get_accomplishment_by_medals(uint32_t medals, uint32_t &accomplishment)
{
	accomplishment = 0;
	if(medals >= 1 && medals <= 10){
		accomplishment = 1;
	}
	else if(medals >= 11 && medals <= 50){
		accomplishment = 2;
	}
	else if(medals >= 51 && medals <= 100){
		accomplishment = 3;
	}
	else if(medals >= 101 && medals <= 150){
		accomplishment = 4;
	}
	else {
		accomplishment =  5;
	}
	return 0;
}

int Cusermsg::insert( uint32_t userid, uint32_t objuserid, 
		uint32_t reason , uint32_t medal)
{
	char date_str[30];
	mysql_date( date_str, time(NULL),sizeof(date_str));
	sprintf( this->sqlstr, "insert into %s values ('%s',%u,%u,%u, %u)", 
			this->get_table_name(), date_str ,
			userid, objuserid, reason , medal);
	STD_INSERT_RETURN(this->sqlstr, EMAIL_EXISTED_ERR);	
}

int Cusermsg::get_count1(su_get_usermsg_mushroom_guide_in *p_in, uint32_t* count)
{
	if(p_in->userid == 0){

		if(p_in->reason == 0){
			sprintf(this->sqlstr, "select count(*) from %s where logdate >= FROM_UNIXTIME(%u) and \
				logdate <= FROM_UNIXTIME(%u)",
				this->get_table_name(),
				p_in->start_date,
				p_in->end_date
				);
		}
		else{
			sprintf(this->sqlstr, "select count(*) from %s where logdate >= FROM_UNIXTIME(%u) and \
				logdate <= FROM_UNIXTIME(%u) and reason = %u",
				this->get_table_name(),
				p_in->start_date,
				p_in->end_date,
				p_in->reason
				);
		}
	}
	else{
		if(p_in->reason == 0){
			sprintf(this->sqlstr, "select count(*) from %s where userid = %u and logdate >= FROM_UNIXTIME(%u) \
					logdate <= FROM_UNIXTIME(%u)",
					this->get_table_name(),
					p_in->userid,
					p_in->start_date,
					p_in->end_date
					);
		}
		else{
			sprintf(this->sqlstr, "select count(*) from %s where userid = %u and logdate >= FROM_UNIXTIME(%u) \
					logdate <= FROM_UNIXTIME(%u) and reason = %u",
					this->get_table_name(),
					p_in->userid,
					p_in->start_date,
					p_in->end_date,
					p_in->reason
					);
		}
	}

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}

int Cusermsg::get_tip_offs_by_date(su_get_usermsg_mushroom_guide_in *p_in, su_get_usermsg_mushroom_guide_out_item **pp_list,
	   uint32_t	*p_count)
{
	uint32_t page = 0;
	if(p_in->index >= 1){
		page = p_in->index - 1;
	}
	DEBUG_LOG("userid: %u, index: %u, start_date: %u, end_date: %u", 
			p_in->userid, p_in->index, p_in->start_date, p_in->end_date);
	if(p_in->userid == 0){
		if(p_in->reason != 0){
			sprintf(this->sqlstr, "select objuserid, reason, userid, medal, unix_timestamp(logdate) from %s where \
				logdate >= FROM_UNIXTIME(%u) and logdate <= FROM_UNIXTIME(%u) and reason = %u order by medal desc limit %u, 15",
				this->get_table_name(),
				p_in->start_date,
				p_in->end_date,
				p_in->reason,
				page*15
				);
		}
		else{
			sprintf(this->sqlstr, "select objuserid, reason, userid, medal, unix_timestamp(logdate) from %s where \
				logdate >= FROM_UNIXTIME(%u) and logdate <= FROM_UNIXTIME(%u) order by medal desc limit %u, 15",
				this->get_table_name(),
				p_in->start_date,
				p_in->end_date,
				page*15
				);
		}
	}
	else{
		if(p_in->reason == 0){
			sprintf(this->sqlstr, "select objuserid, reason, userid, medal, unix_timestamp(logdate) from %s where userid = %u \
				and logdate >= FROM_UNIXTIME(%u) and logdate <= FROM_UNIXTIME(%u) order by objuserid asc limit %u, 15",
				this->get_table_name(),
				p_in->userid,
				p_in->start_date,
				p_in->end_date,
				page*15
				);
		}
		else{
			sprintf(this->sqlstr, "select objuserid, reason, userid, medal, unix_timestamp(logdate) from %s where userid = %u \
				and logdate >= FROM_UNIXTIME(%u) and logdate <= FROM_UNIXTIME(%u) and reason = %u order by objuserid \
				asc limit %u, 15",
				this->get_table_name(),
				p_in->userid,
				p_in->start_date,
				p_in->end_date,
				p_in->reason,
				page*15
				);

		}
	}
	
	uint32_t medal = 0, accomplishment = 0;
    STD_QUERY_WHILE_BEGIN(this-> sqlstr, pp_list, p_count);
		(*pp_list+i)->object_userid = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->reason = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->userid = atoi_safe(NEXT_FIELD); 
		medal = atoi_safe(NEXT_FIELD); 
		get_accomplishment_by_medals(medal, accomplishment);
		(*pp_list+i)->medal = accomplishment; 
		(*pp_list+i)->logdate = atoi_safe(NEXT_FIELD); 
    STD_QUERY_WHILE_END();
}

int Cusermsg::get_by_date( userid_t objuserid	, 
		usermsg_get_report_by_date_in *p_in, uint32_t * p_count,  
			usermsg_get_report_by_date_out_item ** pp_item ) 
{
	char objuserid_str[100]	;
	if (objuserid==0 ){
		strcpy(objuserid_str,"true" );
	}else{
		sprintf(objuserid_str,"objuserid=%u",objuserid  );
	}

	if (p_in->sort_index<1 ||p_in->sort_index>6) 	p_in->sort_index=1; 

	sprintf( this->sqlstr, "select objuserid , count(userid) t1, \
			count(DISTINCT userid) t2,sum(reason=1) t3,sum(reason=2) t4,\
			sum(reason=3) t5,sum(reason=4) t6  from %s \
            where  unix_timestamp(logdate)>=%u and unix_timestamp(logdate)<=%u and %s  group by objuserid \
			order by   t%d  desc  Limit %u,15",
            this->get_table_name(),p_in->startdate, p_in->enddate,objuserid_str, 
			p_in->sort_index, p_in->index*30);

    STD_QUERY_WHILE_BEGIN(this-> sqlstr, pp_item,p_count   );
		(*pp_item+i)->userid=atoi_safe(NEXT_FIELD); 
		(*pp_item+i)->count=atoi_safe(NEXT_FIELD); 
		(*pp_item+i)->one_count=atoi_safe(NEXT_FIELD); 
		(*pp_item+i)->reason1_count=atoi_safe(NEXT_FIELD); 
		(*pp_item+i)->reason2_count=atoi_safe(NEXT_FIELD); 
		(*pp_item+i)->reason3_count=atoi_safe(NEXT_FIELD); 
		(*pp_item+i)->reason4_count=atoi_safe(NEXT_FIELD); 
    STD_QUERY_WHILE_END();
}

int Cusermsg::get_by_date_userid(userid_t objuserid , 
		usermsg_get_report_by_date_userid_in *p_in, uint32_t * p_count,  
			usermsg_get_report_by_date_userid_out_item ** pp_item ) 
{
    sprintf( this->sqlstr, "select logdate, userid,reason from %s \
            where  objuserid=%u and  unix_timestamp(logdate)>=%u and unix_timestamp(logdate)<=%u \
			   Limit %u,100",
            this->get_table_name(), objuserid, p_in->startdate, p_in->enddate,p_in->index  );
    STD_QUERY_WHILE_BEGIN(this-> sqlstr, pp_item,p_count );
		memset((*pp_item+i)->date ,0, sizeof((*pp_item+i)->date));
		strcpy((*pp_item+i)->date ,NEXT_FIELD);
		(*pp_item+i)->srcuserid=atoi_safe(NEXT_FIELD); 
		(*pp_item+i)->reason=atoi_safe(NEXT_FIELD); 
    STD_QUERY_WHILE_END();
}

int Cusermsg::get_reportid_by_date_userid(userid_t objuserid , 
		 usermsg_clear_by_date_userid_in *p_in, uint32_t * p_count,  
			stru_id ** pp_item ) 
{
    sprintf( this->sqlstr, "select DISTINCT userid from %s \
            where objuserid=%u and  unix_timestamp(logdate)>=%u and unix_timestamp(logdate)<=%u \
			   Limit %u,1000",
            this->get_table_name(), objuserid, p_in->startdate, p_in->enddate,p_in->index  );
    STD_QUERY_WHILE_BEGIN(this-> sqlstr, pp_item,p_count );
		(*pp_item+i)->id=atoi_safe(NEXT_FIELD); 
    STD_QUERY_WHILE_END();
}

int Cusermsg::clear_by_date_userid(userid_t objuserid , usermsg_clear_by_date_userid_in *p_in ) 
{
    sprintf( this->sqlstr, "delete from %s \
            where  objuserid=%u and  unix_timestamp(logdate)>=%u and unix_timestamp(logdate)<=%u  ",
            this->get_table_name(), objuserid, p_in->startdate, p_in->enddate );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cusermsg::remove(usermsg_mushroom_guide_del_in *p_in ) 
{
    sprintf( this->sqlstr, "delete from %s where logdate>=FROM_UNIXTIME(%u) and logdate <= FROM_UNIXTIME(%u) \
			and objuserid = %u and userid = %u",
            this->get_table_name(),
		   	p_in->start_date, 
			p_in->end_date,
		   	p_in->objuserid,
		   	p_in->userid 
			);

	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cusermsg::get_del_count(usermsg_mushroom_guide_del_in *p_in, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s logdate>=FROM_UNIXTIME(%u) and logdate <= FROM_UNIXTIME(%u) \
			and objuserid = %u and userid = %u",
			this->get_table_name(),
			p_in->start_date,
			p_in->end_date,
			p_in->objuserid,
			p_in->userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}

int Cusermsg::drop_one_record(su_user_drop_super_mushroom_record_in *p_in)
{
	sprintf(this->sqlstr, "delete from %s where logdate = FROM_UNIXTIME(%u) and userid = %u and objuserid = %u",
			this->get_table_name(),
			p_in->logdate,
			p_in->userid,
			p_in->objuserid
			);

	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}
