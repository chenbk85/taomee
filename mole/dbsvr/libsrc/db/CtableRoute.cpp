/*
 * =====================================================================================
 *
 *       Filename:  CtableRoute.cpp
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
#include "CtableRoute10.h"

#include "common.h"
#include "proto.h"
#include "benchapi.h"


CtableRoute::CtableRoute(mysql_interface * db,const  char * db_name_pre,  
			const char * table_name_pre,const  char* id_name ) 
	:Ctable(db,"","")	
{ 
	this->db_type=db_type;
	strncpy (this->table_name_pre,table_name_pre,sizeof(this->table_name_pre ) );
	strncpy (this->id_name,id_name,sizeof(this->id_name));
    strncpy(this->db_name_pre, db_name_pre,sizeof(this->db_name_pre));
}

char * CtableRoute::get_table_name(uint32_t id)
{

	sprintf (this->db_name,"%s_%02d", this->db_name_pre,id%100 );
	this->db->select_db(this->db_name);
	sprintf (this->db_table_name,"%s_%02d.%s_%02d",
	this->db_name_pre,id%100, this->table_name_pre,(id/100)%100);
	return this->db_table_name;
}

int CtableRoute::id_is_existed(uint32_t id, bool * existed)
{
	sprintf (this->sqlstr,"select  %s from %s where %s=%d ", \
	this->id_name, this->get_table_name(id), this->id_name,id);
		*existed=false;
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		*existed=true;
	STD_QUERY_ONE_END();
}

int CtableRoute::change_int_field(uint32_t userid ,const char * field_type ,
			   	int32_t changevalue, uint32_t max_value ,uint32_t *p_value )
{
	uint32_t db_value;	
	int ret; 
	ret=this->get_int_value(userid,field_type,&db_value  );
	if(ret!=SUCC) return ret;
	int value= (int)db_value + changevalue;

	if (value<0){
		return VALUE_NOENOUGH_E;
	}

	if ((uint32_t) value> max_value ){
		return VALUE_MAX_E;
	}
	*p_value=value;
	return this->set_int_value(userid,field_type,value );

}

int CtableRoute::change_int_field_without_max_check(uint32_t userid ,const char * field_type ,
			   	int32_t changevalue, uint32_t *p_value )
{
	uint32_t db_value;	
	int ret; 
	ret=this->get_int_value(userid,field_type,&db_value  );
	if(ret!=SUCC) return ret;
	int value= (int)db_value + changevalue;
	if (value<0){
		return VALUE_NOENOUGH_E;
	}
	*p_value=value;
	return this->set_int_value(userid,field_type,value );
}

int CtableRoute::get_int_value(uint32_t userid ,const char * field_type ,  uint32_t * p_value)
{
	sprintf( this->sqlstr, "select  %s from %s where %s=%u ", 
		field_type , this->get_table_name(userid),this->id_name , userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_value );
	STD_QUERY_ONE_END();
}

int CtableRoute::set_int_value(uint32_t userid ,const char * field_type , uint32_t  value)
{
	sprintf(this->sqlstr, "update %s set %s =%u where %s=%u " ,
		this->get_table_name(userid),field_type,value , this->id_name ,userid);
	STD_SET_RETURN(this->sqlstr,userid ,USER_ID_NOFIND_ERR );	
}

int CtableRoute::set_int_value_bit(uint32_t userid ,const char * field_type ,uint32_t bitid ,uint32_t  value)
{
	if (bitid==0 ||  bitid>32 || value>1   )
	{
		return ENUM_OUT_OF_RANGE_ERR;
	}

	if (value==1)
	{
		value=(1<<(bitid-1));
		sprintf( this->sqlstr, "update %s set %s =%s |%u    where %s=%u " ,
			this->get_table_name(userid),field_type,field_type ,  value , this->id_name ,userid );

	}
	else
	{
		value=0xFFFFFFFF-(1<<(bitid-1));
		sprintf( this->sqlstr, "update %s set %s =%s &%u    where %s=%u " ,
			this->get_table_name(userid),field_type,field_type ,  value , this->id_name ,userid );
	}

	STD_SET_RETURN(this->sqlstr,userid ,USER_ID_NOFIND_ERR);	
}

