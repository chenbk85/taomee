/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_mvp_day.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/27/2012 05:45:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_mvp_day.h"

Csysarg_mvp_day::Csysarg_mvp_day(mysql_interface *db):Ctable(db, "SYSARG_DB", "t_sysarg_mvp_day")
{
}
int  Csysarg_mvp_day::insert(uint32_t day)
{
     sprintf(this->sqlstr, "insert into %s values(%u,%u,%u)",
			             this->get_table_name(),
						 day,
						 0,
						 0); 
	 STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR); 
}
int Csysarg_mvp_day::update(const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u",
			this->get_table_name(),
			col,
			value);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int  Csysarg_mvp_day::remove(uint32_t day)
{
	sprintf(this->sqlstr, "delete from %s where day = %u",
			    this->get_table_name(),
				day);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int  Csysarg_mvp_day::update_one(uint32_t day,const char* col, uint32_t value)
{
	uint32_t day_old = -1;
	uint32_t value_old = -1;
	int ret = this->get_one(col,&value_old);
	if(ret == USER_ID_NOFIND_ERR)
	{
		ret = this->insert(day);
		ret = this->update(col,value);
		return ret;
	}
	this->get_one("day",&day_old);
    if(day_old == day)//在同一天，则在原有基础之上继续++
	{
        value_old += value; 
		ret = this->update(col,value_old);
	}
	else//不在同一天，则将value更新
	{
        value_old = value; 
		ret = this->remove(day_old);
		ret = this->insert(day);
		ret = this->update(col,value_old);
	}
	return ret;
}
/*  int  Csysarg_mvp_day::get_two(uint32_t day,uint32_t* data1,uint32_t*data2)
{
    uint32_t count1 = 0,count2 = 0;
	sprintf(this->sqlstr, "select   value_1,value_2  from  %s where day < %u",
			this->get_table_name(),day);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
	count1=atoi_safe(NEXT_FIELD);
	count2=atoi_safe(NEXT_FIELD);
	DEBUG_LOG("count1:%u,count2:%u\n",count1,count2);
    if(count1>count2)
	{
        (*data1)++;
	}
	else if(count1<count2)
		(*data2)++;
	STD_QUERY_ONE_END();

}*/
int  Csysarg_mvp_day::get_two(uint32_t day,struct p_list** pp_list,uint32_t *count,uint32_t *data1,uint32_t* data2)
{
	sprintf(this->sqlstr, "select   value_1,value_2  from  %s where day < %u",
			this->get_table_name(),day);
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, count ) ;
        (*pp_list+i)->putao = atoi_safe(NEXT_FIELD);
        (*pp_list+i)->nono = atoi_safe(NEXT_FIELD);
    if((*pp_list+i)->putao > (*pp_list+i)->nono)
	{
        (*data1)++;
	}
	else if((*pp_list+i)->putao < (*pp_list+i)->nono)
		(*data2)++;
	STD_QUERY_WHILE_END();

}
int  Csysarg_mvp_day::get_current(uint32_t day,uint32_t* data1,uint32_t*data2)
{
    sprintf(this->sqlstr, "select value_1,value_2 from %s where day = %u",
			this->get_table_name(),
			day
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
    *data1=atoi_safe(NEXT_FIELD); 
	*data2=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END(); 
}
int Csysarg_mvp_day::get_one(const char* col,uint32_t* data)
{
    sprintf(this->sqlstr, "select %s from %s",
			col,
			this->get_table_name());
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
    *data = atoi_safe(NEXT_FIELD); 
    STD_QUERY_ONE_END();	
}
int Csysarg_mvp_day::get_last_day(uint32_t* oldday)
{
    sprintf(this->sqlstr,"select day from %s order by day desc limit 1",this->get_table_name());
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
	*oldday = atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END(); 
}
int Csysarg_mvp_day::get_one_by_col(uint32_t day,const char* col, uint32_t* value)
{
    sprintf(this->sqlstr, "select %s from %s where day = %u",
			col,
			this->get_table_name(),
			day);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
	*value=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}
int Csysarg_mvp_day::update_oldday(uint32_t day,const char* col, uint32_t value)
{
    sprintf(this->sqlstr, "update %s set %s = %u where day = %u",
        this->get_table_name(),
        col,
        value,
		day);
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR); 
}
int Csysarg_mvp_day::insert_newday(uint32_t day,const char* col,uint32_t value)
{
	if(strcmp(col,"value_1"))
    sprintf(this->sqlstr, "insert into %s values(%u,%u,%u)",
			this->get_table_name(),
			day,
			value,
			0);
	else
	sprintf(this->sqlstr, "insert into %s values(%u,%u,%u)",
			this->get_table_name(),
			day,
			0,
			value);
	 STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

