/*
 * =====================================================================================
 *
 *       Filename:  Cuser_info_ex.cpp
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
 *  common.h
 * =====================================================================================
 */
#include "Cuser_info_ex.h"
#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"

using namespace std;
//user 
Cuser_info_ex::Cuser_info_ex(mysql_interface * db ):CtableRoute( db , "USER_INFO" , "t_user_info_ex" , "userid") 
{ 

}

int Cuser_info_ex::del(userid_t userid )
{
	sprintf( this->sqlstr, "delete from %s where userid=%u " ,
			this->get_table_name(userid),  userid); 
	STD_REMOVE_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}

int Cuser_info_ex::update_for_pay(userid_t userid  , userinfo_set_payinfo_in *p_in )
{

	char telephone_mysql[mysql_str_len(TELEPHONE_LEN)];
	char mobile_mysql[mysql_str_len(MOBILE_LEN )];
	char addr_mysql[mysql_str_len(ADDR_LEN)];
	char interest_mysql[mysql_str_len(INTEREST_LEN)];
	set_mysql_string(telephone_mysql,p_in->telephone,TELEPHONE_LEN);
	set_mysql_string(mobile_mysql,p_in->mobile,MOBILE_LEN);
	set_mysql_string(addr_mysql,p_in->addr,ADDR_LEN);
	set_mysql_string(interest_mysql,p_in->interest,INTEREST_LEN );

	//请求的生日是无效的
	if (!( p_in->birthday>19000000 && p_in->birthday<30000000 )){
	 	return	VALUE_OUT_OF_RANGE_ERR ;
	}

	sprintf( this->sqlstr, "update %s  set\
			birthday='%u',\
			telephone='%s',\
			mobile='%s',\
			mail_number='%u',\
			addr_province='%u',\
			addr_city='%u',\
			addr='%s',\
			interest='%s'\
			where userid=%u ", 
			this->get_table_name(userid),
			p_in->birthday,
			telephone_mysql,
			mobile_mysql,
			p_in->mail_number,
			p_in->addr_province,
			p_in->addr_city,
			addr_mysql,
			interest_mysql, 
			userid
	   	);
	STD_SET_RETURN(this->sqlstr, userid,USER_ID_NOFIND_ERR);	
}

int Cuser_info_ex::insert(userid_t userid, user_info_ex_item * u)
{
	char telephone_mysql[mysql_str_len(TELEPHONE_LEN)];
	char mobile_mysql[mysql_str_len(MOBILE_LEN )];
	char addr_mysql[mysql_str_len(ADDR_LEN)];
	char interest_mysql[mysql_str_len(INTEREST_LEN)];

	set_mysql_string(telephone_mysql,u->telephone,TELEPHONE_LEN);
	set_mysql_string(mobile_mysql,u->mobile,MOBILE_LEN);
	set_mysql_string(addr_mysql,u->addr,ADDR_LEN);
	set_mysql_string(interest_mysql,u->interest,INTEREST_LEN );
	

	sprintf( this->sqlstr, "insert into %s values (\
		%u,%u,%u,'%s','%s',%u,%u,%u,'%s','%s' )", 
			this->get_table_name(userid), userid,
			u->flag,
			u->birthday,
			telephone_mysql,
			mobile_mysql,
			u->mail_number,
			u->addr_province,
			u->addr_city,
			addr_mysql,
			interest_mysql
	   	);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cuser_info_ex::update(userid_t userid, user_info_ex_item * u)
{
	char telephone_mysql[mysql_str_len(TELEPHONE_LEN)];
	char mobile_mysql[mysql_str_len(MOBILE_LEN )];
	char addr_mysql[mysql_str_len(ADDR_LEN)];
	char interest_mysql[mysql_str_len(INTEREST_LEN)];

	set_mysql_string(telephone_mysql,u->telephone,TELEPHONE_LEN);
	set_mysql_string(mobile_mysql,u->mobile,MOBILE_LEN);
	set_mysql_string(addr_mysql,u->addr,ADDR_LEN);
	set_mysql_string(interest_mysql,u->interest,INTEREST_LEN );
	

	sprintf( this->sqlstr, "update %s  set\
			flag='%u',\
			birthday='%u',\
			telephone='%s',\
			mobile='%s',\
			mail_number='%u',\
			addr_province='%u',\
			addr_city='%u',\
			addr='%s',\
			interest='%s'\
			where userid=%u ", 
			this->get_table_name(userid),
			u->flag,
			u->birthday,
			telephone_mysql,
			mobile_mysql,
			u->mail_number,
			u->addr_province,
			u->addr_city,
			addr_mysql,
			interest_mysql, 
			userid
	   	);
	STD_SET_RETURN(this->sqlstr, userid,USER_ID_NOFIND_ERR);	
}

int Cuser_info_ex::get(userid_t userid, user_info_ex_item * p_out)
{
	memset(p_out,0,sizeof (*p_out));
	sprintf( this->sqlstr, "select flag,birthday, telephone, mobile, mail_number, addr_province,\
			addr_city, addr, interest from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->flag);
		INT_CPY_NEXT_FIELD(p_out->birthday);
		BIN_CPY_NEXT_FIELD(p_out->telephone,TELEPHONE_LEN);
		BIN_CPY_NEXT_FIELD(p_out->mobile,MOBILE_LEN);
		INT_CPY_NEXT_FIELD(p_out->mail_number);
		INT_CPY_NEXT_FIELD(p_out->addr_province);
		INT_CPY_NEXT_FIELD(p_out->addr_city);
		BIN_CPY_NEXT_FIELD(p_out->addr,ADDR_LEN);
		BIN_CPY_NEXT_FIELD(p_out->interest,INTEREST_LEN);
	STD_QUERY_ONE_END();
}

int Cuser_info_ex::set_flag ( userid_t userid  ,const  char * flag_type  ,  uint32_t flag_bit ,  bool is_true )
{
	uint32_t flag;
	int ret;
	bool old_is_true;
	if ((ret =this->get_flag(userid, flag_type ,&flag))!=SUCC){
			return ret;
	}	

	if ( (flag & flag_bit) == flag_bit ) old_is_true=true;
	else old_is_true =false; 

	if (old_is_true==is_true){
		//已经设置了
		return FLAY_ALREADY_SET_ERR;	
	}
		
	if (is_true ) flag+=flag_bit;  		
	else flag-=flag_bit;
	return this->update_flag(userid, flag_type ,flag);
}

int Cuser_info_ex::get_flag(userid_t userid ,const char * flag_type   ,  uint32_t * flag)
{
	sprintf( this->sqlstr, "select  %s from %s where userid=%u ", 
		flag_type, this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
			*flag=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}

int Cuser_info_ex::isset_birthday(userid_t userid)
{
	sprintf( this->sqlstr, "select birthday \
			from %s  where userid=%u \
			and birthday>19000000 and  birthday<30000000  " ,
		this->get_table_name(userid),userid );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR );
	STD_QUERY_ONE_END();
}

int Cuser_info_ex::update_birthday_sex(userid_t userid  ,  
		uint32_t birthday, uint32_t sex )
{
	uint32_t birthday_db;
	if (sex >0 ) sex=0x02;

	//请求的生日是无效的
	if (!( birthday>19000000 && birthday<30000000 )){
	 	return	DATE_INVALID_ERR;
	}

	//检查数据库中的数据是否已经设置过了.
	sprintf( this->sqlstr, "select birthday \
			from %s  where userid=%u " ,
		this->get_table_name(userid),userid );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR );
			birthday_db=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (birthday_db>19000000 && birthday_db<30000000 ){
	 	return	USER_BIRTHDAY_IS_SET_ERR;
	}
	
	sprintf( this->sqlstr, " update %s set \
					birthday=%u, \
					flag=(flag & 0xFFFFFFFD)|%u \
		   			where userid=%u " ,
				this->get_table_name(userid), 
				birthday, sex,userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_info_ex::update_flag(userid_t userid ,const char * flag_type  , uint32_t  flag)
{
	sprintf( this->sqlstr, "update %s set %s =%u where userid=%u " ,
		this->get_table_name(userid), flag_type,flag,userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

