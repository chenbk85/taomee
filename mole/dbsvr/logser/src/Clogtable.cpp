/*
 * =====================================================================================
 *
 *       Filename:  Clogin.cpp
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
#include "Clogtable.h"
#include "benchapi.h"
#include <time.h>

int Clogin::insert(char * logtime,TLOGIN* t)
{
	sprintf( this->sqlstr, "insert into %s values ('%s',%u,%u,%u,%u)",
			this->get_table_name(),logtime,t->userid,t->usertype,t->logintime,t->onlinetime );
	STD_INSERT_RETURN(this->sqlstr,SUCC );
}

int Cuser_opt::insert(uint32_t logtime,
		userid_t userid,uint32_t flag ,uint32_t regpost,uint32_t  ip )
{
	sprintf( this->sqlstr, "insert into %s values ('%s',%u,%u,%u,%u)",
			this->get_table_name(logtime),get_datetime (logtime),
		userid,flag,regpost,ip);
	STD_INSERT_RETURN(this->sqlstr,SUCC );
}

int Cxiaomee_use::insert(uint32_t date,uint32_t hour, uint32_t usecount  ) 
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u)",
		this->get_table_name(),date,hour, usecount );
	STD_INSERT_RETURN(this->sqlstr,SUCC );
}
int Cday_count::insert( uint32_t type, uint32_t date, uint32_t count ) 
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u)",
		this->get_table_name(),type ,date , count );
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cday_count::update( uint32_t type, uint32_t date, uint32_t count ) 
{
	sprintf( this->sqlstr, "update %s set count = %u \
			where type=%u and logdate=%u ",
		this->get_table_name(), count ,type ,date );
	STD_INSERT_RETURN(this->sqlstr, FAIL );
}

int Cday_count_ex::insert( uint32_t type, uint32_t date, uint32_t id1,uint32_t id2 ,uint32_t count ) 
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,%u,%u)",
		this->get_table_name(),type ,date , id1,id2,count );
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cday_count_ex::update( uint32_t type, uint32_t date, uint32_t id1,uint32_t id2, uint32_t count ) 
{
	sprintf( this->sqlstr, "update %s set count = %u \
			where type=%u and logdate=%u and id1=%u  and id2=%u ",
		this->get_table_name(), count ,type ,date, id1,id2 );
	STD_SET_RETURN_EX(this->sqlstr, FAIL );
}


int Cday_count_ex::add(uint32_t type, uint32_t date,uint32_t id1,uint32_t id2 , uint32_t count  ) 
{
	sprintf( this->sqlstr, "update %s set count = count+%u \
			where type=%u and logdate=%u and id1=%u  and id2=%u ",
		this->get_table_name(), count ,type ,date, id1,id2 );
	STD_SET_RETURN_EX(this->sqlstr, FAIL );
}

int Cday_count_ex::add_or_insert(uint32_t type, uint32_t date,uint32_t id1,uint32_t id2 , uint32_t count  ) 
{
	if ( this->add( type, date, id1,id2,count )!=SUCC){
		return this->insert(type, date, id1,id2,count);
	}else{
		return SUCC;
	}
}

int Cxiaomee_use::add_xiaomee( uint32_t date,uint32_t hour, uint32_t usecount ) 
{
	sprintf( this->sqlstr, "update %s set usecount=usecount+%u where logdate=%u and hour=%u",
			this->get_table_name(), usecount ,date,hour);
	int dbret; 
	int acount; 
	if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){ 
		if (acount ==1){
			return SUCC;
		}else{
			return this->insert(date, hour,usecount ) ;
		}
	}else {
		return DB_ERR;
	} 
}

int Clogin::getstatisticsdata(char * startdate,char * enddate,uint8_t usertype, TUSERINFO * tp )
{
	sprintf( this->sqlstr, "select  count(*) ,  count(DISTINCT userid) , sum(onlinetime) \
		   	from  %s where usertype=%u and logtime>='%s' and logtime<'%s'" ,
			this->get_table_name(),usertype,startdate,enddate);

	STD_QUERY_ONE_BEGIN(this->sqlstr,DB_ERR);
			tp->logincount = atoi_safe(row[0]);
			tp->logincountindependent=atoi_safe (row[1]);
			tp->onlinetimetotal= atoi_safe (row[2]);
	STD_QUERY_ONE_END( ) ;

	this->getonlinetimelevelcount(startdate, enddate, tp->usertype,0,10,&(tp->logincountonlinetimelevel1) );	
	this->getonlinetimelevelcount(startdate, enddate, tp->usertype,10,30,&(tp->logincountonlinetimelevel2) );	
	this->getonlinetimelevelcount(startdate, enddate, tp->usertype,30,60,&(tp->logincountonlinetimelevel3) );	
	this->getonlinetimelevelcount(startdate, enddate, tp->usertype,60,120,&(tp->logincountonlinetimelevel4) );	
	this->getonlinetimelevelcount(startdate, enddate, tp->usertype,120,180,&(tp->logincountonlinetimelevel5) );	
	this->getonlinetimelevelcount(startdate, enddate, tp->usertype,180,5000,&(tp->logincountonlinetimelevel6) );	
	
	return SUCC;
}

int Clogin::getonlinetimelevelcount(char * startdate,char * enddate,uint8_t usertype,
	   	uint32_t start, uint32_t end ,uint32_t *count )
{
	sprintf( this->sqlstr, "select count(*)  from %s  where usertype=%u and logtime>='%s' \
			and logtime<'%s' and onlinetime>=%u and onlinetime<%u ",
			this->get_table_name(),usertype,startdate,enddate, start,end );

	STD_QUERY_ONE_BEGIN(this->sqlstr,DB_ERR);
		*count=atoi (row[0]);
	STD_QUERY_ONE_END( ) ;
	
}

int Conline_usercount::insert(TONLINE_USERCOUNT *  tonline ) 
{
	struct tm tm_tmp; 
  	localtime_r((time_t*)&(tonline->logtime), &tm_tmp );
	uint32_t logdate,msid;
	logdate=(tm_tmp.tm_year+1900)*10000+ (tm_tmp.tm_mon+1)*100+tm_tmp.tm_mday;
	msid=tm_tmp.tm_hour*60+tm_tmp.tm_min;
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,%u)",
			this->get_table_name(),logdate,msid,tonline->serverid,tonline->onlinecount);
	STD_INSERT_RETURN(this->sqlstr,SUCC );
}

int Conline_usercount::ms_all(char* date  )
{	
	time_t t=time(NULL);
	struct tm tm_tmp; 
  	localtime_r(&t, &tm_tmp );
	int startms= ((tm_tmp.tm_hour-1)*60+1440)%1440;
	int i;
	uint32_t count;

	for(i=startms;i<startms+60; i++ ){
		sprintf( this->sqlstr, "select  sum(onlinecount)\
				from  %s where logdate=%s and msid=%u and  serverid<>0 " ,
				this->get_table_name(),date,i);
		STD_QUERY_ONE_BEGIN(this->sqlstr,DB_ERR);
			count= atoi_safe(row[0]);
		STD_QUERY_ONE_END_WITHOUT_RETURN( ) ;

		sprintf( this->sqlstr, "insert into %s values (%s,%u,0,%u)",
			this->get_table_name(),date, i ,count);
		STD_INSERT_WITHOUT_RETURN(this->sqlstr,SUCC);
	}
	return SUCC;
}	

int Cpetgrade::insert(char * logtime )
{
	sprintf( this->sqlstr, "insert into %s values ('%s',%u,%u)",
			this->get_table_name(),logtime,0,0);
	STD_INSERT_RETURN(this->sqlstr,SUCC );
}

int Cserver::insert(char * logtime, char *ip,  uint32_t logcount ) 
{
	sprintf( this->sqlstr, "insert into %s values ('%s','%s',%u)",
			this->get_table_name(), logtime, ip,logcount);
	STD_INSERT_RETURN(this->sqlstr,SUCC );
}
int Cserver::check(char * logtime, char * ip , bool * isloged )
{
	sprintf( this->sqlstr, "select ip from %s where logtime='%s' and ip='%s'",
			this->get_table_name(), logtime,ip);
	*isloged=false;
	STD_QUERY_ONE_BEGIN(this->sqlstr,DB_ERR);
		*isloged=true;
	STD_QUERY_ONE_END( ) ;

}
int Cuserinfo::insert(TUSERINFO* tp)
{
	sprintf( this->sqlstr, "insert into %s values ('%s',%u,%u,%u,%u, %u,%u,%u,%u, %u, %u, %u, %u, %u, %u)",
			this->get_table_name(),tp->logtime, tp->usertype, 
			tp->usercount, tp->addusercount,
			tp->logincount,
		tp->logincountindependent, tp->maxonlinecount , tp->onlinetimetotal,
		tp->arpu,
		tp->logincountonlinetimelevel1, tp->logincountonlinetimelevel2, 
		tp->logincountonlinetimelevel3, tp->logincountonlinetimelevel4, 
	tp->logincountonlinetimelevel5, tp->logincountonlinetimelevel6);
	STD_INSERT_RETURN(this->sqlstr,SUCC);
}

int Cuserinfo::get( char *logtime, uint8_t usertype, TUSERINFO* tp )
{

	sprintf( this->sqlstr, "select *  from  %s where logtime='%s' and usertype=%d " ,
			this->get_table_name(),logtime,usertype);

	STD_QUERY_ONE_BEGIN(this->sqlstr,DB_ERR);
		strcpy (tp->logtime,logtime);
		tp->usertype=atoi_safe(row[1]);
		tp->usercount=atoi_safe(row[2]);
		tp->addusercount=atoi_safe(row[3]);
		tp->logincount=atoi_safe(row[4]);
		tp->logincountindependent=atoi_safe(row[5]);
		tp->maxonlinecount =atoi_safe(row[6]);
		tp->onlinetimetotal=atoi_safe(row[7]);
		tp->arpu=atoi_safe(row[8]);
		tp->logincountonlinetimelevel1=atoi_safe(row[9]);
		tp->logincountonlinetimelevel2=atoi_safe(row[10]);
		tp->logincountonlinetimelevel3=atoi_safe(row[11]);
		tp->logincountonlinetimelevel4=atoi_safe(row[12]);
		tp->logincountonlinetimelevel5=atoi_safe(row[13]);
		tp->logincountonlinetimelevel6=atoi_safe(row[14]);
	STD_QUERY_ONE_END( ) ;


}
int Cuserinfo::remove(char *logtime, uint8_t usertype )
{
	sprintf( this->sqlstr, "delete from %s where logtime='%s' and usertype=%d",
			this->get_table_name(),logtime, usertype);
	STD_REMOVE_RETURN_EX(this->sqlstr,SUCC);
}

int Cpetinfo::insert(TPETINFO* tp)
{
	sprintf( this->sqlstr, "insert into %s values ('%s',%u,%u,%u,%u, %u,%u, %u, %u, %u, %u, %u, %u)",
		this->get_table_name(), tp->logtime, 
		tp->apple,
		tp->watermelon,
		tp->strawberry,
		tp->banana,
		tp->pear,
		tp->peach,
		tp->addapple,
		tp->addwatermelon,
		tp->addstrawberry,
		tp->addbanana,
		tp->addpear,
		tp->addpeach
	);
	STD_INSERT_RETURN(this->sqlstr,SUCC);
}

int Cpetinfo::remove(char *logtime)
{
	sprintf( this->sqlstr, "delete from %s where logtime='%s' ",
			this->get_table_name(),logtime);
	STD_REMOVE_RETURN_EX(this->sqlstr,SUCC);
}

int Cpetgrade::get_register_count( char * startdate,char * enddate, uint32_t *count,uint32_t * addcount)
{	
	return SUCC;
/*
	sprintf( this->sqlstr, "select  count(*) ,  count(DISTINCT userid) , sum(onlinetime) \
		   	from  %s where usertype=%u and logtime>='%s' and logtime<'%s'" ,
			this->get_table_name(),tp->usertype,startdate,enddate);

	STD_QUERY_ONE_BEGIN(this->sqlstr,DB_ERR);
			tp->logincount = atoi_safe(row[0]);
			tp->logincountindependent=atoi_safe (row[1]);
			tp->onlinetimetotal= atoi_safe (row[2]);
	STD_QUERY_ONE_END( ) ;
*/


}

int Cpetgrade::get_register_addcount( char * startdate,char * enddate, uint8_t usertype,  uint32_t *addcount)
{
	sprintf( this->sqlstr, "select  count(*)  \
		   	from  %s where petgrade=0 and usertype=%u and logtime>='%s' and logtime<'%s'" ,
			this->get_table_name(),usertype,startdate,enddate);
	STD_QUERY_ONE_BEGIN(this->sqlstr,DB_ERR);
			*addcount= atoi_safe(row[0]);
	STD_QUERY_ONE_END( ) ;
}

int Cpetgrade::get_register_count_with_enddate( char * enddate, uint8_t usertype,  uint32_t *count)
{
	sprintf( this->sqlstr, "select  count(*)  \
		   	from  %s where petgrade=0 and usertype=%u  and logtime<'%s'" ,
			this->get_table_name(),usertype,enddate);
	STD_QUERY_ONE_BEGIN(this->sqlstr,DB_ERR);
			*count= atoi_safe(row[0]);
	STD_QUERY_ONE_END( ) ;


}
int Cpetgrade::getpetinfor(char * startdate,char * enddate, TPETINFO *tp)
{
	return SUCC;
	/*
	MYSQL_RES *res;
	MYSQL_ROW  row;
	//以下实现，对TPETINFO的结构要求严格。水果娃娃的顺序不能变。
	uint32_t *addlist;
	int index=0;
	memset(tp,0,sizeof(TPETINFO) );
	safe_copy_string(tp->logtime,startdate);
	//get PET add 
	addlist=&(tp->addapple);
	sprintf( this->sqlstr, "select  count(*) \
		   		from  %s where logtime>='%s' and logtime<'%s' and petgrade=0 " ,
			this->get_table_name(),startdate,enddate);
	if (( this->db->exec_query_sql(this->sqlstr,&res))==DB_SUCC){
		while((row = mysql_fetch_row(res))){
			*(addlist+index)=atoi_safe(row[1]);
		}
		mysql_free_result(res);
	}else {
		return DB_ERR;
	}

	//get PET total 
	addlist=&(tp->apple);
	sprintf( this->sqlstr, "select pettype, count(*)  \
		   		from  %s where logtime<'%s'  and petgrade=0 group by pettype" ,
			this->get_table_name(),enddate);
	if (( this->db->exec_query_sql(this->sqlstr,&res))==DB_SUCC){
		while((row = mysql_fetch_row(res))){
			index=atoi_safe(row[0]);
			*(addlist+index)=atoi_safe(row[1]);
		}
		mysql_free_result(res);
	}else {
		return DB_ERR;
	}

	*/
	return SUCC;

}
