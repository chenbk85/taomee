/*
 * =====================================================================================
 *
 *
 *       Filename:  Cserial.cpp
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
#include "Cserial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"


Cserial::Cserial(mysql_interface * db ) :Ctable( db,"SERIAL_DB","t_serial")
{ 
	pre_tableid=config_get_intval("PRE_TABLEID",0);

	this->serail_def.clear();
	/*
	this->serail_def.insert(SERAIL2TYPE::value_type(get_serialid("2FRT2824Y5QW"),1542));
	*/
	this->serail_def.insert(SERAIL2TYPE::value_type(get_serialid("2EAA6TFB3W5C"),1542));
}

char * Cserial::get_table_name(uint64_t serialid)
{
	uint32_t tableid; 
	if (serialid<100)  tableid=(uint32_t)serialid;
	else  if (serialid<100000000)  tableid=5;  
	else tableid=(uint32_t) (serialid/10000000000000ULL)%100; 
	sprintf (this->db_table_name,"%s.%s_%02u",
			this->db_name, this->table_name,tableid );
	return this->db_table_name;
}
int Cserial::get_useflag(uint64_t serialid , uint32_t *p_useflag)
{
	sprintf( this->sqlstr, "select useflag from %s where serialid =%"PRIu64 ,
		this->get_table_name(serialid ),serialid );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,SERIAL_ID_NOFIND_ERR );
			*p_useflag=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}

int Cserial::set_used(uint64_t  serialid , uint32_t useflag,uint32_t * p_price)
{	
	uint32_t t_useflag;
	uint32_t t_type;
	sprintf( this->sqlstr, "select useflag, type from %s where serialid =%"PRIu64, 
		this->get_table_name(serialid ),serialid );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,SERIAL_ID_NOFIND_ERR );
			t_useflag=atoi_safe(NEXT_FIELD);
			t_type=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	//check type
	if (t_type>=this->price_type_list.count ){
		return VALUE_OUT_OF_RANGE_ERR;	
	}

	if (t_useflag==0 ){//no used
		*p_price=this->price_type_list.list[t_type];
		return this->update_useflag(serialid,useflag );  
	}else {
		return SERIAL_ID_IS_USED_ERR;
	}

}

void Cserial::set_serail_str( char * serialid_str  )
{
	memcpy(this->serail_str_c, serialid_str, SERIAL_STR_LEN);
	this->serail_str_c[SERIAL_STR_LEN]='\0';
	if(serialid_str[0]=='0'){//是8个的序列号
		this->serial_id=atoi_safe(this->serail_str_c);
	}else{
		this->serial_id=get_serialid(this->serail_str_c);
	}
}

int Cserial::get_info(uint64_t serialid , uint32_t *p_useflag,  uint32_t * p_type )
{
	sprintf( this->sqlstr, "select useflag, type from %s where serialid =%"PRIu64, 
		this->get_table_name(serialid ), serialid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,SERIAL_ID_NOFIND_ERR );
			*p_useflag=atoi_safe(NEXT_FIELD);
			*p_type=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (*p_type==0){
		return SERIAL_ID_NOFIND_ERR;
	}else{
		return SUCC;
	}

}

int Cserial::get_info_by_serialid_str(char * serialid_str, uint32_t *p_useflag,  uint32_t * p_type )
{
	//得到uint64_t 的序列号
	this->set_serail_str( serialid_str );

	SERAIL2TYPE::iterator it;
	//查找预定义的序列号,
	it=this->serail_def.find(this->serial_id);
	if (it!=this->serail_def.end()){
		*p_type=it->second;
		*p_useflag=0;
		return SUCC;
	}else{
		return this-> get_info( this->serial_id,p_useflag,p_type); 
	}

}

int Cserial::set_used_by_serialid_str(char*  serialid_str, uint32_t * p_type)
{
	//得到uint64_t 的序列号
	this->set_serail_str( serialid_str );


	SERAIL2TYPE::iterator it;
	it=this->serail_def.find(this->serial_id);
	if (it!=this->serail_def.end()){
		*p_type=it->second;
		return SUCC;
	}else{
		uint32_t t_useflag;
		int ret=this-> get_info( this->serial_id,&t_useflag,p_type); 
		if (ret!=SUCC){ return ret; }

		if ( t_useflag == 0 ){//未使用
			return this->update_useflag(serial_id,time(NULL));  //设置使用时间
		}  else if ( t_useflag == 1 ){//已到期
			return SERIAL_ID_IS_DUE_ERR;

		} else {
			return SERIAL_ID_IS_USED_ERR;
		}
	}
}

int Cserial::set_unused(uint64_t serialid)
{	
	uint32_t useflag;
	int ret=this->get_useflag(serialid, &useflag ); 
	if (ret==DB_SUCC ) {
		if ( useflag != USERID_NULL ){
			return this->update_useflag(serialid,USERID_NULL);  
		}else{// no need to set  
			return FLAY_ALREADY_SET_ERR; 
		}
	}else{
		return ret;
	}
}

int Cserial::update_useflag(uint64_t serialid , uint32_t useflag)
{
	sprintf( this->sqlstr, "update %s set useflag=%u where serialid=%"PRIu64 ,
			this->get_table_name(serialid), useflag, serialid );
	STD_SET_RETURN_EX(this->sqlstr,SERIAL_ID_NOFIND_ERR );	
}

int Cserial::get_noused_id( uint32_t type,  uint64_t * p_serialid)
{
	uint32_t rand_tmp,id_tmp;
	int acount ;
	int ret, dbret;
    char serialid_str[30];
    FILE *fp=fopen("/dev/urandom","r");
	while (true) {
		fread(&rand_tmp,4,1,fp);
		fread(&id_tmp,4,1,fp);
		sprintf (serialid_str,"%04u%02u%06u%07u",1000 ,
			this->pre_tableid ,
			id_tmp%1000000,rand_tmp%10000000);
		*p_serialid=atoll(serialid_str );
		sprintf(sqlstr,"insert into %s values( %"PRIu64",%u,0 )  ",
	 	this->get_table_name(*p_serialid  ) , *p_serialid ,type );
	
		if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){
			ret= DB_SUCC;
			break;	
		}else {
			if (dbret==ER_DUP_ENTRY){
				 continue ; //重新插入
			}else{
				ret=DB_ERR ;
				break;	
			}	
		}
	}
	fclose(fp);
	return ret;
}
