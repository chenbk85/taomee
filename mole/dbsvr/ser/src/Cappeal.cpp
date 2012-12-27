/*
 * =====================================================================================
 *
 *       Filename:  Cappeal.cpp
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

#include "Cappeal.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

//create
Cappeal::Cappeal(mysql_interface * db ) :Ctable( db, "APPEAL_DB","t_appeal" )
{ 

}

int Cappeal::get_count(uint32_t *p_count){
	sprintf( this->sqlstr, "select count(1) from %s  ", 
			this->get_table_name());
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr,DB_ERR);
			INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Cappeal::insert( stru_appeal *p_item,  uint32_t *p_id) 
{

	char why_mysql[mysql_str_len(sizeof(p_item->why))];
	char question_mysql[mysql_str_len(sizeof(p_item->question))];
	char answer_mysql[mysql_str_len(sizeof(p_item->answer))];
	char reg_email_mysql[mysql_str_len(sizeof(p_item->reg_email))];

	char realname_mysql[mysql_str_len(sizeof(p_item->realname))];
	char email_mysql[mysql_str_len(sizeof(p_item->email))];
	char mobile_mysql[mysql_str_len(sizeof(p_item->mobile))];
	char telephone_mysql[mysql_str_len(sizeof(p_item->telephone))];
	char realpasswd1_mysql[mysql_str_len(sizeof(p_item->realpasswd1))];
	char realpasswd2_mysql[mysql_str_len(sizeof(p_item->realpasswd2))];
	char realpasswd3_mysql[mysql_str_len(sizeof(p_item->realpasswd3))];
	char addr_mysql[mysql_str_len(sizeof(p_item->addr))];
	char othermsg_mysql[mysql_str_len(sizeof(p_item->othermsg ))];

	set_mysql_string(why_mysql,(p_item->why),sizeof(p_item->why));
	set_mysql_string(reg_email_mysql,(p_item->reg_email),sizeof(p_item->reg_email));
	set_mysql_string(question_mysql,(p_item->question),sizeof(p_item->question));
	set_mysql_string(answer_mysql,(p_item->answer),sizeof(p_item->answer));
	set_mysql_string(realname_mysql,(p_item->realname),sizeof(p_item->realname));
	set_mysql_string(email_mysql,(p_item->email),sizeof(p_item->email));
	set_mysql_string(mobile_mysql,(p_item->mobile),sizeof(p_item->mobile));
	set_mysql_string(telephone_mysql,(p_item->telephone),sizeof(p_item->telephone));
	set_mysql_string(realpasswd1_mysql,(p_item->realpasswd1),sizeof(p_item->realpasswd1));
	set_mysql_string(realpasswd2_mysql,(p_item->realpasswd2),sizeof(p_item->realpasswd2));
	set_mysql_string(realpasswd3_mysql,(p_item->realpasswd3),sizeof(p_item->realpasswd3));
	set_mysql_string(addr_mysql,(p_item->addr ),sizeof(p_item->addr ));
	set_mysql_string(othermsg_mysql,(p_item->othermsg ),sizeof(p_item->othermsg ));
	


	p_item->dealtime= time(NULL);
	sprintf( this->sqlstr, "insert into %s values (%u,1,1,%u,0,%u,%u, \
		%u,'%s',\
		%u,%u,%u,'%s',%u,%u,'%s','%s',\
		'%s', '%s', '%s', '%s',   '%s', '%s', '%s', '%s','%s' )", 
			this->get_table_name(), 
			p_item->id,			
			0,
			(uint32_t)time(NULL),			
			p_item->userid,
			p_item->flag,
			why_mysql,
			p_item->vip,
			p_item->vip_way,
			p_item->reg_time,
			reg_email_mysql,
			p_item->birthday,
			p_item->Ol_last,
			question_mysql, 	
			answer_mysql,
			realname_mysql,
			email_mysql,
			mobile_mysql,
			telephone_mysql,
			realpasswd1_mysql,
			realpasswd2_mysql,
			realpasswd3_mysql,
			addr_mysql,
			othermsg_mysql
	);
	STD_INSERT_GET_ID (this->sqlstr, KEY_EXISTED_ERR ,*p_id );	
}

int Cappeal::set_deal(uint32_t id, uint32_t state,uint32_t dealflag,uint32_t adminid )
{
	uint32_t now=time(NULL);
	sprintf( this->sqlstr,"update  %s  set state=%u, dealflag=%u,dealtime=%u,adminid=%u \
			where id=%u " ,
		this->get_table_name(), state,dealflag,now ,adminid,id  ); 
	STD_SET_RETURN_EX( this->sqlstr ,  KEY_NOFIND_ERR  );

}

int Cappeal::get(uint32_t id, stru_appeal * p_list  )
{
	sprintf( this->sqlstr,"select  \
				id,\
				state,\
				dealflag,\
				dealtime,\
				adminid,\
				logtime,\
				userid,\
				flag,\
				why,\
				vip,\
				vip_way,\
				reg_time,\
				reg_email,\
				birthday,\
				Ol_last,\
				question,\
				answer,\
				realname,\
				email,\
				mobile,\
				telephone,\
				realpasswd1,\
				realpasswd2,\
				realpasswd3,\
				addr,\
				othermsg  \
				from %s  where id=%u\
		 		" ,
		this->get_table_name(), id  ); 
	STD_QUERY_ONE_BEGIN(this-> sqlstr,KEY_NOFIND_ERR );
		INT_CPY_NEXT_FIELD(p_list->id);
		INT_CPY_NEXT_FIELD(p_list->state);
		INT_CPY_NEXT_FIELD(p_list->dealflag);
		INT_CPY_NEXT_FIELD(p_list->dealtime);
		INT_CPY_NEXT_FIELD(p_list->adminid);
		INT_CPY_NEXT_FIELD(p_list->logtime);
		INT_CPY_NEXT_FIELD(p_list->userid);
		INT_CPY_NEXT_FIELD(p_list->flag);
		BIN_CPY_NEXT_FIELD(p_list->why,sizeof(p_list->why));
		INT_CPY_NEXT_FIELD(p_list->vip);
		INT_CPY_NEXT_FIELD(p_list->vip_way);
		INT_CPY_NEXT_FIELD(p_list->reg_time);
		BIN_CPY_NEXT_FIELD(p_list->reg_email,sizeof(p_list->reg_email));
		INT_CPY_NEXT_FIELD(p_list->birthday);
		INT_CPY_NEXT_FIELD(p_list->Ol_last);

		BIN_CPY_NEXT_FIELD(p_list->question,sizeof(p_list->question));
		BIN_CPY_NEXT_FIELD(p_list->answer,sizeof(p_list->answer));
		BIN_CPY_NEXT_FIELD(p_list->realname,sizeof(p_list->realname));
		BIN_CPY_NEXT_FIELD(p_list->email,sizeof(p_list->email));
		BIN_CPY_NEXT_FIELD(p_list->mobile,sizeof(p_list->mobile));
		BIN_CPY_NEXT_FIELD(p_list->telephone,sizeof(p_list->telephone));
		BIN_CPY_NEXT_FIELD(p_list->realpasswd1,sizeof(p_list->realpasswd1));
		BIN_CPY_NEXT_FIELD(p_list->realpasswd2,sizeof(p_list->realpasswd2));
		BIN_CPY_NEXT_FIELD(p_list->realpasswd3,sizeof(p_list->realpasswd3));
		BIN_CPY_NEXT_FIELD(p_list->addr,sizeof(p_list->addr));
		BIN_CPY_NEXT_FIELD(p_list->othermsg,sizeof(p_list->othermsg));
	STD_QUERY_ONE_END();
}


int Cappeal::getlist( appeal_get_list_in *p_in,
				uint32_t *p_count, stru_appeal ** pp_list  )
{
	char where_str[300]="true";
	if(p_in->userid>0){
		snprintf(where_str,sizeof(where_str), "userid=%u",p_in->userid );
	}else {
		if (p_in->state==1){
			snprintf(where_str,sizeof(where_str), "state=%u ",p_in->state);
		}else if (p_in->state==2) {
			if (p_in->dealflag>0){
				snprintf(where_str,sizeof(where_str), "state=%u and dealflag=%u ",
						p_in->state,p_in->dealflag);
			}else{
				snprintf(where_str,sizeof(where_str), "state=%u",p_in->state);
			}
		}
	}


	sprintf( this->sqlstr,"select  \
				id,\
				state,\
				dealflag,\
				dealtime,\
				adminid,\
				logtime,\
				userid,\
				flag,\
				why,\
				vip,\
				vip_way,\
				reg_time,\
				reg_email,\
				birthday,\
				Ol_last,\
				question,\
				answer,\
				realname,\
				email,\
				mobile,\
				telephone,\
				realpasswd1,\
				realpasswd2,\
				realpasswd3,\
				addr,\
				othermsg  \
				from %s  where %s \
		 		order by logtime asc limit %u,4" ,
		this->get_table_name(), where_str, p_in->pageid*4 ); 

	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_list+i)->id);
		INT_CPY_NEXT_FIELD((*pp_list+i)->state);
		INT_CPY_NEXT_FIELD((*pp_list+i)->dealflag);
		INT_CPY_NEXT_FIELD((*pp_list+i)->dealtime);
		INT_CPY_NEXT_FIELD((*pp_list+i)->adminid);
		INT_CPY_NEXT_FIELD((*pp_list+i)->logtime);
		INT_CPY_NEXT_FIELD((*pp_list+i)->userid);
		INT_CPY_NEXT_FIELD((*pp_list+i)->flag);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->why,sizeof((*pp_list+i)->why));


		INT_CPY_NEXT_FIELD((*pp_list+i)->vip);
		INT_CPY_NEXT_FIELD((*pp_list+i)->vip_way);
		INT_CPY_NEXT_FIELD((*pp_list+i)->reg_time);

		BIN_CPY_NEXT_FIELD((*pp_list+i)->reg_email,sizeof((*pp_list+i)->reg_email));
		INT_CPY_NEXT_FIELD((*pp_list+i)->birthday);
		INT_CPY_NEXT_FIELD((*pp_list+i)->Ol_last);

		BIN_CPY_NEXT_FIELD((*pp_list+i)->question,sizeof((*pp_list+i)->question));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->answer,sizeof((*pp_list+i)->answer));
		
		BIN_CPY_NEXT_FIELD((*pp_list+i)->realname,sizeof((*pp_list+i)->realname));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->email,sizeof((*pp_list+i)->email));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->mobile,sizeof((*pp_list+i)->mobile));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->telephone,sizeof((*pp_list+i)->telephone));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->realpasswd1,sizeof((*pp_list+i)->realpasswd1));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->realpasswd2,sizeof((*pp_list+i)->realpasswd2));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->realpasswd3,sizeof((*pp_list+i)->realpasswd3));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->addr,sizeof((*pp_list+i)->addr));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->othermsg,sizeof((*pp_list+i)->othermsg));
	STD_QUERY_WHILE_END();
}

