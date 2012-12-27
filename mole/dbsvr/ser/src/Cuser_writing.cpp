/*
 * =====================================================================================
 *
 *       Filename:  Cuser_writing.cpp
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
#include "Cuser_writing.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
Cuser_writing::Cuser_writing(mysql_interface * db ) :Ctable( db, "USERMSG_DB","t_user_writing" )
{ 

}

int Cuser_writing::set_type(userid_t userid, char * date, uint32_t type )
{	
	sprintf( this->sqlstr, "update %s   set type=%u \
			where userid=%u  and logdate='%s' ",
		this->get_table_name(type ), type, userid,date );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cuser_writing::set_report(userid_t userid, char * date, uint32_t type,uint32_t flag, msg_item * reportmsg   )
{	
	char msg_mysql[mysql_str_len(1004)];
	set_mysql_string(msg_mysql,(char*)(reportmsg), reportmsg->itemlen+4); 
	char date_str[30];
	mysql_date( date_str, time(NULL),sizeof(date_str));
	

	sprintf( this->sqlstr, "update %s   set flag=%u, reportdate='%s',  report='%s' \
			where userid=%u  and logdate='%s' ",
		this->get_table_name(0),flag,date_str,
		msg_mysql, userid,date );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cuser_writing::insert( uint32_t type, userid_t userid, msg_item *msg) 
{
	char date_str[30];
	char msg_mysql[mysql_str_len(4004)];
	mysql_date( date_str, time(NULL),sizeof(date_str));
	set_mysql_string(msg_mysql,(char*)(msg), msg->itemlen+4); 

	sprintf(this->sqlstr, "insert into %s values ('%s',%u,0,0,%u,0,'%s',0x00000000)", 
			this->get_table_name(type), date_str, type, 
			userid, msg_mysql   );
	STD_INSERT_RETURN(this->sqlstr, EMAIL_EXISTED_ERR);	
}

int Cuser_writing::get_count(  usermsg_get_writing_count_in *p_in ,uint32_t *p_count)
{
	char  flag_fmt[200];
	char  type_fmt[200];


	char  mysql_search_key[mysql_str_len(sizeof(p_in->search_key))];
	char  search_key_fmt[1000]={};

	if ( p_in->search_key[0]!='\0' ){//有关键字
		 p_in->search_key[sizeof(p_in->search_key)-1]='\0';
		 set_mysql_string(mysql_search_key,p_in->search_key,strlen(p_in->search_key));
		 sprintf ( search_key_fmt, " msg  like '%%%s%%' ",mysql_search_key);
	}else{
		 strcpy( search_key_fmt, "true");
	}

    if  (p_in->userid!=0){
        sprintf( this->sqlstr, "select count(1) from %s \
            where userid=%u ",
            this->get_table_name(p_in->type),
            p_in-> userid);
    }else{
        if (p_in->flag==10){//非垃圾箱
            strcpy(flag_fmt, "flag<>1" );
        }else if (p_in->flag==2 ){ //已回复
            strcpy(flag_fmt, "(flag=2 or flag>=20000)" );
        }else{
            sprintf(flag_fmt, "flag=%u" , p_in->flag);
        }

        if (p_in->type==0){//全部
            strcpy(type_fmt, "true" );
        }else if  (p_in->type==1005 ){
            strcpy(type_fmt, "(type=1005 or type>2000 )" );
        }else{
            sprintf(type_fmt, "type=%u" , p_in->type);
        }
		const char * opt_date_str;
		if (p_in->flag>=20000){
			opt_date_str="reportdate";
		}else{
			opt_date_str="logdate";
		}


        sprintf( this->sqlstr, "select count(1) from %s \
                where %s>=%u and %s<%u and %s and %s and %s ",
                this->get_table_name(p_in->type),opt_date_str ,  p_in->startdate,
                opt_date_str, p_in->enddate, type_fmt,flag_fmt, search_key_fmt );
    }



	STD_QUERY_ONE_BEGIN(this-> sqlstr,NO_DEFINE_ERR );
			*p_count=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}	


int Cuser_writing::get_list( usermsg_searchkey_item *p_in ,
	 uint32_t *p_count, usermsg_get_writing_list_out_item **pp_out_item  )
{
	this->set_sql_for_get(p_in,"logdate,type,flag,value,userid,msg,report ", 5);	
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_out_item, p_count ) ;
		BIN_CPY_NEXT_FIELD((*pp_out_item+i)->date,sizeof((*pp_out_item+i)->date)); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->type);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->flag);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->msgid);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->userid);
		BIN_CPY_NEXT_FIELD((*pp_out_item+i)->msg ,sizeof((*pp_out_item+i)->msg ) );
		BIN_CPY_NEXT_FIELD((*pp_out_item+i)->reportmsg,sizeof((*pp_out_item+i)->reportmsg) );
	STD_QUERY_WHILE_END();
}

int Cuser_writing:: set_sql_for_get ( usermsg_searchkey_item *p_in , 
		const char * select_field_str , uint32_t limit_count)
{
	char  flag_fmt[200];
	char  type_fmt[200];
	char  mysql_search_key[mysql_str_len(sizeof(p_in->search_key))];
	char  search_key_fmt[1000]={};

	if ( p_in->search_key[0]!='\0' ){//有关键字
		 p_in->search_key[sizeof(p_in->search_key)-1]='\0';
		 set_mysql_string(mysql_search_key,p_in->search_key,strlen(p_in->search_key));
		 sprintf ( search_key_fmt, " msg  like '%%%s%%' ",mysql_search_key);
	}else{
		 strcpy( search_key_fmt, "true");
	}

	if  (p_in->userid!=0){ 
		sprintf( this->sqlstr, "select %s from %s \
			where userid=%u  order by logdate  desc Limit %u,1", 
			 select_field_str  ,this->get_table_name(p_in->type), 
		   	p_in-> userid,p_in->index);
	}else{

		if (p_in->flag==10){//非垃圾箱
			strcpy(flag_fmt, "flag<>1" );
		}else if (p_in->flag==2 ){ //已回复
			strcpy(flag_fmt, "(flag=2 or flag>=20000)" );
			
		}else{
			sprintf(flag_fmt, "flag=%u" , p_in->flag);
		}
		const char * opt_date_str;
		if (p_in->flag>=20000){
			opt_date_str="reportdate";
		}else{
			opt_date_str="logdate";
		}

		if (p_in->type==0){//全部
			strcpy(type_fmt, "true" );
		}else if  (p_in->type==1005 ){//会员
			strcpy(type_fmt, "(type=1005 or type>2000 )" );
		}else{
			sprintf(type_fmt, "type=%u" , p_in->type);
		}

		//logdate,type,flag,userid,msg,report 
		sprintf( this->sqlstr, "select %s,reportdate  from %s \
				where %s>=%u and %s <%u and %s and %s  and  %s order by %s Limit %u,%u", 
				 select_field_str,  this->get_table_name(p_in->type),
				 opt_date_str, p_in->startdate,opt_date_str, p_in->enddate,
				 type_fmt,flag_fmt,search_key_fmt ,opt_date_str , p_in->index, limit_count);
	}
	return 0;
}
int Cuser_writing::get(  usermsg_get_writing_in *p_in ,usermsg_get_writing_out *p_out  )
{
	struct  msg_max  msg, reportmsg;

	sprintf( this->sqlstr, "select logdate,type,flag,value,userid,msg,report \
			from %s where userid=%u and logdate='%s'  ", 
			this->get_table_name(p_in->type),p_in->srcuserid,p_in->date);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,NO_DEFINE_ERR );
			strcpy(p_out-> date,NEXT_FIELD);
			INT_CPY_NEXT_FIELD(p_out->type);
			INT_CPY_NEXT_FIELD(p_out->flag);
			INT_CPY_NEXT_FIELD(p_out->msgid);
			INT_CPY_NEXT_FIELD(p_out->userid);
			BIN_CPY_NEXT_FIELD(&msg , 4004);
			BIN_CPY_NEXT_FIELD(&reportmsg, 1004);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if(reportmsg.msglen==0)reportmsg.msglen=4; 
	p_out-> msglen=msg.msglen;
	p_out->reportlen=(reportmsg.msglen<4020-msg.msglen?reportmsg.msglen:4020-msg.msglen);
	memcpy(p_out->msg,msg.msg,p_out->msglen);
	memcpy(p_out->msg+p_out->msglen,reportmsg.msg,p_out->reportlen);
	return SUCC;
}	

int  Cuser_writing::clear_by_delete_flag( usermsg_clear_writing_in *p_in)
{
	sprintf( this->sqlstr, "delete from %s  where  logdate>=%u and logdate<%u  and flag=1" ,
		this->get_table_name(0),p_in->startdate, p_in->enddate);
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cuser_writing::set_del_flag(userid_t userid, char * date , msg_item *p_msg_item )
{

	char msg_mysql[mysql_str_len(1004)];
	set_mysql_string(msg_mysql,(char*)(p_msg_item), p_msg_item->itemlen+4); 

	sprintf( this->sqlstr, "update %s  set flag=1 ,report='%s' \
			where userid=%u  and logdate='%s' " ,
		this->get_table_name(0),msg_mysql , 
		userid,date );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}



char* Cuser_writing::get_table_name(uint32_t type )
{
	if (type<1010 || type>=2000 ) {
    	sprintf (this->db_table_name,"%s.%s",
        	this->db_name, this->table_name );
	}else{
    	sprintf (this->db_table_name,"%s.%s%s",
        	this->db_name, this->table_name,"_questionnaire" );
	}	
    return this->db_table_name;
}

