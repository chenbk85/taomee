/*
 * =====================================================================================
 *
 *       Filename:  Cmms.cpp
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
#include "Cmms.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include <algorithm>

Cmms::Cmms(mysql_interface * db ) 
		:CtableRoute10x10( db,"MMS","t_mms","userid")
{ 

}

int Cmms:: init(uint32_t type, uint32_t userid  )
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,0,0, 0,0,0,0,0, 0x00000000 )", 
			this->get_table_name(userid),  
			type,	
			userid
	);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR );
}

int Cmms::get_mms( uint32_t userid ,mms_get_info_out *p_out  )
{
		mms_get_info_by_type_in in;
		mms_get_info_by_type_out out;
		in.mms_type=MMS_TYPE;
		int ret;
		ret=this->get_value_by_type(userid,&in,&out );
		if (ret!=SUCC) return ret;
		p_out->water=out.v1;						
		p_out->mud=out.v2;						
		p_out->get_count=out.v3;						
		return SUCC;
}
/*
int Cmms::get_db( uint32_t type ,uint32_t userid ,MMS_GET_INFO_OUT *p_out  )
{
	sprintf( this->sqlstr, "select "F_WATER","F_MUD","F_GETCOUNT" \
		   	from %s where  type=%d and userid=%u  ", 
			this->get_table_name(userid ),type, userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( p_out->water);	
		INT_CPY_NEXT_FIELD( p_out->mud);	
		INT_CPY_NEXT_FIELD( p_out->get_count);	
	STD_QUERY_ONE_END();
}
*/

int Cmms::get_value_by_type_db(userid_t userid,
		mms_get_info_by_type_in *p_in,mms_get_info_by_type_out *p_out )
{

	uint32_t  opt_time;
	uint32_t  now=get_date(time(NULL));

	sprintf( this->sqlstr, "select opt_time,opt_count, v1,v2,v3,v4,v5 \
		   	from %s where  type=%d and userid=%u  ", 
			this->get_table_name(userid ),p_in->mms_type, userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		opt_time=atoi_safe(NEXT_FIELD); 
		INT_CPY_NEXT_FIELD(p_out->opt_count);	
		if (opt_time!=now){
			p_out->opt_count=0;	
		}
		INT_CPY_NEXT_FIELD( p_out->v1);	
		INT_CPY_NEXT_FIELD( p_out->v2);	
		INT_CPY_NEXT_FIELD( p_out->v3);	
		INT_CPY_NEXT_FIELD( p_out->v4);	
		INT_CPY_NEXT_FIELD( p_out->v5);	
	STD_QUERY_ONE_END();

}
int Cmms::get_value_by_type(userid_t userid,
		mms_get_info_by_type_in *p_in,mms_get_info_by_type_out *p_out )
{
	memset( p_out,0,sizeof(*p_out) );
	int ret;
	ret=this->get_value_by_type_db(userid,p_in,p_out );
	if ( ret == USER_ID_NOFIND_ERR ){
		return this->init(p_in->mms_type,userid );				
	}else{
		return ret;  
	}
}

int Cmms::get_opt_list(uint32_t type,userid_t userid,
		uint32_t* p_opt_time, uint32_t* p_opt_count, mms_opt_list *p_out )
{
	sprintf( this->sqlstr, "select opt_time,opt_count ,opt_list \
			from %s where type=%u and userid=%d ", 
			 this->get_table_name(userid),type,userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_opt_time);	
		INT_CPY_NEXT_FIELD(*p_opt_count );	
		BIN_CPY_NEXT_FIELD (p_out, sizeof ( *p_out) );
	STD_QUERY_ONE_END();
}

int Cmms::mms_add_getcount(userid_t userid )
{
	sprintf( this->sqlstr, " update %s set "F_GETCOUNT"="F_GETCOUNT"+1 \
		where  type=%u and userid=%u " ,
		this->get_table_name(userid), 
		MMS_TYPE ,userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	

}

int Cmms::update_opt_list(uint32_t type,userid_t userid ,uint32_t opt_type,
	uint32_t opt_time, uint32_t opt_count, mms_opt_list * p_list )
{

	char mysql_list[mysql_str_len(sizeof (*p_list))];
	const char *opt_type_str;
	uint32_t add_count=0;
	if (opt_type==OPT_WATER)
		opt_type_str= F_WATER;
	else if (opt_type==OPT_MUD)
		opt_type_str= F_MUD ;
	else
		return MMS_OPT_TPYE_NO_DEFINE_ERR;

	if (type==MMS_TYPE ){
		add_count=1;
	}else if (type==2) {
		add_count=10;
	}else{
		add_count=1;
	}
	

	set_mysql_string(mysql_list,(char*)p_list, 
			4+sizeof(p_list->item[0])* p_list->count);
	sprintf( this->sqlstr, " update %s set opt_time=%u, opt_count=%u ,\
		   	%s=%s+%u, opt_list='%s' \
		where  type=%u and userid=%u " ,
		this->get_table_name(userid), 
		opt_time,
		opt_count,
		opt_type_str,
		opt_type_str,
		add_count,
		mysql_list,
		type ,userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cmms::mms_opt(userid_t userid,uint32_t type ,mms_opt_item *p_item)
{
	int ret;
	uint32_t i;
	uint32_t now;
	mms_opt_list out;
	uint32_t db_opt_time;
	uint32_t db_opt_count;
	uint32_t max_a_day;
	if (type==MMS_TYPE ){
		max_a_day=100;
	}else if (type==2)  {
		max_a_day=10;
	}else if (type==3){
		max_a_day=50;
	}
	
	ret=this->get_opt_list( type,userid,&db_opt_time,&db_opt_count,&out );	
	if ( ret!=SUCC) return ret;	

	now=get_date(time(NULL) ); 
	//检查是不是同一天
	if(db_opt_time== now ){
		//检查当天是不是超过最大操作数
		if (db_opt_count>=max_a_day){
			return MMS_OPT_DAY_COUNT_MAX_ERR;	
		}else{
			db_opt_count++;	//次数加1
		}
	}else{
		//每天的第一次
		db_opt_time=now;
		db_opt_count=1;
	}
	
	for (i=0;i<out.count;i++){
		if(out.item[i].userid==p_item->userid){
			break;	
		}
	}

	//没有找到
	if( i==out.count)
		out.count++;
	if (i==20) i=19;
	for (;i>0;i--){
			memcpy(&(out.item[i]),&(out.item[i-1]),sizeof(out.item[0]));
	}

	memcpy(&(out.item[0]),p_item,sizeof(out.item[0]));
	if (out.count>20) out.count=20;

	return this->update_opt_list(type,userid,
		p_item->opt_type,db_opt_time,db_opt_count,&out );
}

int Cmms::mms_gen_one_fruit(userid_t userid )
{
	//MMS_GET_INFO_OUT out;
	//int ret;
	//ret=this->get_mms(userid,&out );
	//if (ret !=SUCC) {
		//return ret ;  
	//}
    return this->mms_add_getcount(userid );
}

int Cmms::get_list(userid_t userid, uint32_t *p_count, 
	mms_get_type_list_out_item **pp_out_item )
{
	sprintf( this->sqlstr, "select type, v1,v2,v3,v4,v5 from %s \
			where userid=%u ", 
			this->get_table_name(userid),userid ); 
	
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_out_item, p_count ) ;
			(*pp_out_item+i)->type=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->v1=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->v2=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->v3=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->v4=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->v5=atoi_safe(NEXT_FIELD ); 
	STD_QUERY_WHILE_END();
}


int Cmms::get_list_ex(userid_t userid, uint32_t *p_count, 
	mms_get_type_list_ex_out_item **pp_out_item )
{
	sprintf( this->sqlstr, "select type,opt_time,opt_count, v1,v2,v3,v4,v5 from %s \
			where userid=%u ", 
			this->get_table_name(userid),userid ); 
	
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_out_item, p_count ) ;
			(*pp_out_item+i)->type=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->opt_time=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->opt_count=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->v1=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->v2=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->v3=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->v4=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->v5=atoi_safe(NEXT_FIELD ); 
	STD_QUERY_WHILE_END();
}


int Cmms::set_value(userid_t userid, mms_set_value_in *p_in )
{
	sprintf( this->sqlstr, " update %s set opt_time=%u,opt_count=%u, \
			v1=%u,	v2=%u,	v3=%u,	v4=%u,	v5=%u	\
		where  type=%u and userid=%u " ,
		this->get_table_name(userid) ,
		p_in->opt_time,
		p_in->opt_count,
		p_in->v1,
		p_in->v2,
		p_in->v3,
		p_in->v4,
		p_in->v5,
		p_in->type ,userid 
	   	);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

