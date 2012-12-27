/*
 * =====================================================================================
 *
 *       Filename:  Cappeal_ex.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年02月09日 11时36分17秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Cappeal_ex.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

//create
Cappeal_ex::Cappeal_ex(mysql_interface * db ) :Ctable( db, "APPEAL_DB","t_appeal_ex" )
{ 

}
/* 
int Cappeal_ex::get_count(uint32_t *p_count){
	sprintf( this->sqlstr, "select count(1) from %s  ", 
			this->get_table_name());
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr,DB_ERR);
			INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}
*/
int Cappeal_ex::insert( stru_appeal_ex *p_item,  uint32_t *p_id) 
{
		char realname_mysql[mysql_str_len(sizeof(p_item->realname))];
		char moblie_mysql[mysql_str_len(sizeof(p_item->moblie))];
		char telephone_mysql[mysql_str_len(sizeof(p_item->telephone))];
		char email_mysql[mysql_str_len(sizeof(p_item->email))];
		char last_place_mysql[mysql_str_len(sizeof(p_item->last_place))];
		char vip_no_mysql[mysql_str_len(sizeof(p_item->vip_no))];
		char seer_fairy_name_mysql[mysql_str_len(sizeof(p_item->seer_fairy_name))];
	
		set_mysql_string(realname_mysql,(p_item->realname),sizeof(p_item->realname));
		set_mysql_string(moblie_mysql,(p_item->moblie),sizeof(p_item->moblie));
		set_mysql_string(telephone_mysql,(p_item->telephone),sizeof(p_item->telephone));
		set_mysql_string(email_mysql,(p_item->email),sizeof(p_item->email));
		set_mysql_string(last_place_mysql,(p_item->last_place),sizeof(p_item->last_place));
		set_mysql_string(vip_no_mysql,(p_item->vip_no),sizeof(p_item->vip_no));
		set_mysql_string(seer_fairy_name_mysql,(p_item->seer_fairy_name),sizeof(p_item->seer_fairy_name));
	

	p_item->adminid=0;
	p_item->logtime=time(NULL);
	p_item->state=1;
	p_item->dealtime=0;

	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,%u,%u,%u,%u,%u,'%s','%s','%s','%s',%u,%u,%u,%u,'%s',%u,%u,'%s',%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,'%s',%u,%u)",
			this->get_table_name(), 
		p_item->id,
		p_item->appeal_type,
		p_item->gameid,
		p_item->userid,
		p_item->state,
		p_item->adminid,
		p_item->logtime,
		p_item->dealtime,
		realname_mysql,
		moblie_mysql,
		telephone_mysql,
		email_mysql,
		p_item->games,
		p_item->birth,
		p_item->first_login,
		p_item->last_login,
		last_place_mysql,
		p_item->vip,
		p_item->vip_type,
		vip_no_mysql,
		p_item->vip_time,
		p_item->mole_tasks,
		p_item->mole_WizardLevel,
		p_item->mole_KnightLevel,
		p_item->mole_IsLastWish,
		p_item->mole_BankBalance1,
		p_item->mole_BankBalance2,
		p_item->seer_tasks,
		p_item->seer_has_teacher,
		p_item->seer_has_student,
		p_item->seer_expirence1,
		p_item->seer_expirence2,
		p_item->seer_money1,
		p_item->seer_money2,
		p_item->seer_fairy_num1,
		p_item->seer_fairy_num2,
		seer_fairy_name_mysql,
		p_item->seer_fairy_level,
		p_item->seer_tower_level
		
	);
	STD_INSERT_GET_ID (this->sqlstr, KEY_EXISTED_ERR ,*p_id );	
}
int Cappeal_ex::set_deal(appeal_ex_deal_in *p_in )
{
	uint32_t now=time(NULL);
	sprintf( this->sqlstr,"update  %s  set  state=%u,dealtime=%u,adminid=%u \
			where id=%u " ,
		this->get_table_name(), p_in->state, now ,p_in->adminid,p_in->id  ); 
	STD_SET_RETURN_EX( this->sqlstr ,  KEY_NOFIND_ERR  );

}
int Cappeal_ex::get_state(uint32_t id, uint32_t *p_state )
{
	sprintf( this->sqlstr,"select  state from %s  where id=%u " ,
		this->get_table_name(), id  ); 
	STD_QUERY_ONE_BEGIN(this-> sqlstr,KEY_NOFIND_ERR );
		INT_CPY_NEXT_FIELD(*p_state );
	STD_QUERY_ONE_END();
}

int Cappeal_ex::get_user_last_info(uint32_t userid ,appeal_ex_get_user_last_state_out *p_out)
{
	sprintf( this->sqlstr,"select id, state,logtime from %s \
		   	where userid=%u order by logtime desc limit 1" ,
		this->get_table_name(), userid ); 
	STD_QUERY_ONE_BEGIN(this-> sqlstr,KEY_NOFIND_ERR );
		INT_CPY_NEXT_FIELD(p_out->id);
		INT_CPY_NEXT_FIELD(p_out->state);
		INT_CPY_NEXT_FIELD(p_out->logtime);
	STD_QUERY_ONE_END();
}




int Cappeal_ex::getlist( appeal_ex_get_list_in *p_in,
				uint32_t *p_count, stru_appeal_ex ** pp_list  )
{
	char where_str[300]="true";
	if(p_in->userid>0){
		snprintf(where_str,sizeof(where_str), "userid=%u",p_in->userid );
	}else {
		if (p_in->state>0){
			snprintf(where_str,sizeof(where_str), "state=%u ",p_in->state);
		}
	}


	sprintf( this->sqlstr,"select  \
				id,\
				userid,\
				state,\
				adminid,\
				logtime,\
				dealtime,\
				realname,\
				moblie,\
				telephone,\
				email,\
				games,\
				birth,\
				first_login,\
				last_login,\
				last_place,\
				vip,\
				vip_type,\
				vip_no,\
				vip_time,\
				mole_tasks,\
				mole_WizardLevel,\
				mole_KnightLevel,\
				mole_IsLastWish,\
				mole_BankBalance1,\
				mole_BankBalance2,\
				seer_tasks,\
				seer_has_teacher,\
				seer_has_student,\
				seer_expirence1,\
				seer_expirence2,\
				seer_money1,\
				seer_money2,\
				seer_fairy_num1,\
				seer_fairy_num2,\
				seer_fairy_name,\
				seer_fairy_level,\
				seer_tower_level \
				from %s  where %s \
		 		order by logtime asc limit %u,4" ,
		this->get_table_name(), where_str, p_in->pageid*4 ); 

	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_list+i)->id);
		INT_CPY_NEXT_FIELD((*pp_list+i)->userid);
		INT_CPY_NEXT_FIELD((*pp_list+i)->state);
		INT_CPY_NEXT_FIELD((*pp_list+i)->adminid);
		INT_CPY_NEXT_FIELD((*pp_list+i)->logtime);
		INT_CPY_NEXT_FIELD((*pp_list+i)->dealtime);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->realname,sizeof((*pp_list+i)->realname));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->moblie,sizeof((*pp_list+i)->moblie));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->telephone,sizeof((*pp_list+i)->telephone));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->email,sizeof((*pp_list+i)->email));
		INT_CPY_NEXT_FIELD((*pp_list+i)->games);
		INT_CPY_NEXT_FIELD((*pp_list+i)->birth);
		INT_CPY_NEXT_FIELD((*pp_list+i)->first_login);
		INT_CPY_NEXT_FIELD((*pp_list+i)->last_login);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->last_place,sizeof((*pp_list+i)->last_place));
		INT_CPY_NEXT_FIELD((*pp_list+i)->vip);
		INT_CPY_NEXT_FIELD((*pp_list+i)->vip_type);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->vip_no,sizeof((*pp_list+i)->vip_no));
		INT_CPY_NEXT_FIELD((*pp_list+i)->vip_time);
		INT_CPY_NEXT_FIELD((*pp_list+i)->mole_tasks);
		INT_CPY_NEXT_FIELD((*pp_list+i)->mole_WizardLevel);
		INT_CPY_NEXT_FIELD((*pp_list+i)->mole_KnightLevel);
		INT_CPY_NEXT_FIELD((*pp_list+i)->mole_IsLastWish);
		INT_CPY_NEXT_FIELD((*pp_list+i)->mole_BankBalance1);
		INT_CPY_NEXT_FIELD((*pp_list+i)->mole_BankBalance2);
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_tasks);
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_has_teacher);
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_has_student);
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_expirence1);
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_expirence2);
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_money1);
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_money2);
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_fairy_num1);
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_fairy_num2);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->seer_fairy_name,sizeof((*pp_list+i)->seer_fairy_name));
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_fairy_level);
		INT_CPY_NEXT_FIELD((*pp_list+i)->seer_tower_level);
	
	STD_QUERY_WHILE_END();
}

