/*
 * =====================================================================================
 *
 *       Filename:  Cpay_history.cpp
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
#include "Cpay_history.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"

//create
Cpay_history::Cpay_history(mysql_interface * db ) 
	:CtableMonth( db,"INCREMENT_DB","t_pay_history","transid")
{ 

}
int Cpay_history::validate(uint32_t transid,uint32_t damee )
{
	uint32_t db_damee;
	uint32_t db_validateflag;
	//get db data
	sprintf( this->sqlstr, "select damee,validateflag from %s  \
			where transid=%u  " ,
	this->get_table_name(transid),transid);
	STD_QUERY_ONE_BEGIN(this->sqlstr,TRANS_ID_NOFIND_ERR );
			db_damee=atoi_safe(NEXT_FIELD );
			db_validateflag=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	//check data 
	if (db_validateflag !=0 ) return IS_VALIDATED_ERR; 
	if (db_damee!=damee ) return VALIDATE_ERR; 

	//upate
	sprintf( this->sqlstr, "update %s set validateflag=%u  \
			where transid=%u" ,
	this->get_table_name(transid),1,transid);
	STD_SET_RETURN_EX(this->sqlstr,TRANS_ID_NOFIND_ERR );	
}

int Cpay_history::gen_true_trans (PAY_STRU *p_month,uint32_t *p_transid )
{
	char mysql_private_msg [mysql_str_len (sizeof(p_month->private_msg))];
	set_mysql_string(mysql_private_msg, 
		set_space_end(p_month->private_msg,sizeof(p_month->private_msg)),
		sizeof(p_month->private_msg) );
	

	sprintf(this->sqlstr, "insert into %s values (0,%u,%u,%u,%u,%u,%u,'%s',%u,%u)", 
			this->get_table_name(), p_month->out_transid, p_month->date, 
			p_month->userid,p_month->paytype,p_month->damee, 
			p_month->months, mysql_private_msg ,0,0); // 0:ok..
	STD_INSERT_GET_ID(this->sqlstr, TRANS_ID_EXISTED_ERR,*p_transid );	
	
}

int Cpay_history::gen_trans(PAY_STRU *p_month, uint32_t dealcode  , uint32_t *p_transid )
{
	char mysql_private_msg [mysql_str_len (sizeof(p_month->private_msg))];
	set_mysql_string(mysql_private_msg, 
		set_space_end(p_month->private_msg,sizeof(p_month->private_msg)),
		sizeof(p_month->private_msg) );

	sprintf(this->sqlstr, "insert into %s values (0,%u,%u,%u,%u,%u,%u,'%s',%u,%u)", 
			this->get_table_name(), p_month->out_transid, p_month->date, 
			p_month->userid,p_month->paytype,p_month->damee, 
			p_month->months, mysql_private_msg ,dealcode  ,0); 
	STD_INSERT_GET_ID(this->sqlstr, TRANS_ID_EXISTED_ERR,*p_transid  );	
	
}

int Cpay_history::set_dealcode(uint32_t transid,uint32_t dealcode  )
{
	sprintf( this->sqlstr, "update %s set dealcode=%u  \
			where transid=%u and dealcode=2" , //未处理
	this->get_table_name(transid),dealcode,transid );
	STD_SET_RETURN_EX(this->sqlstr,TRANS_ID_NOFIND_ERR );	
}

int Cpay_history::set_trans_dealcode(uint32_t transid,
			uint32_t damee , uint32_t dealcode, PAY_STRU *p_pay )
{
	uint32_t db_dealcode;
	sprintf( this->sqlstr, "select userid ,paytype,monthcount,\
			private_msg ,dealcode from %s \
			where transid=%u and damee=%u " ,
			this->get_table_name( transid), transid,damee); 

	STD_QUERY_ONE_BEGIN(this->sqlstr,TRANS_ID_NOFIND_ERR );
		 p_pay->userid =atoi(NEXT_FIELD );
		 p_pay->paytype =atoi(NEXT_FIELD );
		 p_pay->months=atoi(NEXT_FIELD );
		 memcpy(p_pay->private_msg,NEXT_FIELD,
				 sizeof(p_pay->private_msg ) );
		db_dealcode=atoi(NEXT_FIELD);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	if (db_dealcode== dealcode ){
		//以完成设置
		return  FLAY_ALREADY_SET_ERR;
	}
	return  this->set_dealcode(transid,dealcode );
}

int Cpay_history::isrecorded(uint32_t out_transid, uint32_t paytype  )
{
	sprintf( this->sqlstr, "select userid  from %s \
			where out_transid=%u and   paytype=%u " ,
			this->get_table_name(), out_transid, paytype ); 
	STD_QUERY_ONE_BEGIN(this->sqlstr,TRANS_ID_NOFIND_ERR );
	STD_QUERY_ONE_END();
}
