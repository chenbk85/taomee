/*
 * =====================================================================================
 *
 *       Filename:  Cuser_email.cpp
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
#include "Cuser_email.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include <algorithm>
#include <sstream>
using namespace std;
// proto.h

//user 
Cuser_email::Cuser_email(mysql_interface * db ) 
		:CtableRoute10x10( db,"EMAIL_SYS","t_user_email","id")
{ 

}

int Cuser_email:: insert(userid_t userid, user_email_item_without_id *p_item )
{
	char msg_mysql[mysql_str_len(USER_EMAIL_LEN+4)];
	char nick_mysql[mysql_str_len(NICK_LEN)];

	set_mysql_string(nick_mysql,(char*)(p_item-> sendernick), 16); 
	set_mysql_string(msg_mysql,(char*)(&(p_item->msglen)),p_item->msglen+4); 
	sprintf( this->sqlstr, "insert into %s values (0,%u,0,%u,%u,%u,'%s',%u,'%s',0)", 
			this->get_table_name(userid),  
			userid,
			p_item->type,	
			p_item->sendtime,
			p_item->senderid,
			nick_mysql,		
			p_item->mapid,
			msg_mysql
	);
	STD_INSERT_RETURN(this->sqlstr, DB_ERR);	
}

int Cuser_email::get_email(userid_t userid,
		uint32_t emailid , uint32_t *p_use_flag , user_email_item  *p_item )
{
	memset(p_item,0,sizeof(*p_item));

	sprintf( this->sqlstr, "select id, flag,type,sendtime,senderid,sendernick,mapid,msg from %s \
			where id=%u  ", 
			this->get_table_name(userid),emailid );
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_EMAIL_NOFIND_ERR );
			INT_CPY_NEXT_FIELD(p_item->emailid);
			INT_CPY_NEXT_FIELD(*p_use_flag );
			INT_CPY_NEXT_FIELD(p_item->type);
			INT_CPY_NEXT_FIELD(p_item->sendtime);
			INT_CPY_NEXT_FIELD(p_item->senderid);
			MEM_CPY_NEXT_FIELD(p_item->sendernick,NICK_LEN);
			INT_CPY_NEXT_FIELD(p_item->mapid);
			BIN_CPY_NEXT_FIELD(&(p_item->msglen),USER_EMAIL_LEN+4 );
	STD_QUERY_ONE_END();
}

int Cuser_email::get_user_email_list(userid_t userid, uint32_t *p_count, email_info_t **pp_list)
{
	sprintf(this->sqlstr, "select id, type, senderid, sendernick, sendtime, flag, is_award from %s \
			where userid = %u limit 0,500",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list,p_count );
			(*pp_list+i)->id = atoi_safe(NEXT_FIELD); 
			(*pp_list+i)->emailid = atoi_safe(NEXT_FIELD); 
			(*pp_list+i)->senderid = atoi_safe(NEXT_FIELD); 
			MEM_CPY_NEXT_FIELD((*pp_list+i)->sendernick,NICK_LEN);
			(*pp_list+i)->sendtime = atoi_safe(NEXT_FIELD); 
			(*pp_list+i)->flag = atoi_safe(NEXT_FIELD); 
			(*pp_list+i)->is_award = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}
int Cuser_email::get_flag_emailid_list(userid_t userid, 
		uint32_t	*p_count, flag_emailid_stru **pp_list )
{
	sprintf( this->sqlstr, "select flag, id from %s where userid=%u  \
			limit 0,1000 ", 
			this->get_table_name(userid),userid ); 

	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list,p_count );
			(*pp_list+i)->flag=atoi_safe(NEXT_FIELD); 
			(*pp_list+i)->emailid=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

int Cuser_email::get_unread_count(userid_t userid, uint32_t	*p_count )
{
	//uint32_t old_time  = time(0) - 3*30*24*3600;
	//remove_by_sendtime(userid, old_time);
	sprintf( this->sqlstr, "select count(1) from %s where userid=%u  and\
			flag=0", 
			this->get_table_name(userid),userid ); 

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
 		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}


/*
static int
cmp_flag_emailid(const void *p1, const void *p2)
{
	int ret;
	//DEBUG_LOG("flag1:%d flag2:%d",((flag_emailid_stru* )p1 )->flag ,
	//		((flag_emailid_stru* )p2 )->flag    );
	ret=((flag_emailid_stru* )p1 )->flag - ((flag_emailid_stru* )p2 )->flag ;
	if (ret==0){
		ret=((flag_emailid_stru* )p2 )->emailid- ((flag_emailid_stru* )p1 )->emailid;
	}
	return ret;
}
*/
inline int
cmp_flag_emailid( const flag_emailid_stru &p1, const flag_emailid_stru &p2)
{
	if (p1.flag==p2.flag ){
		return p2.emailid<p1.emailid;
	}else{
		return p1.flag <p2.flag; 
	}
	/*
	int ret;
	ret=p1.flag -p2.flag ;
	if (ret==0){
		ret=p2.emailid - p1 .emailid;
	}
	return ret;
	*/
}

int Cuser_email::get_emailid_list(userid_t userid, 
	uint32_t *p_noread_count , uint32_t *p_count, userid_t **pp_list )
{
	struct flag_emailid_stru *p_list;
	int ret;
	ret=this->get_flag_emailid_list( userid, p_count, &p_list);
		

	if (ret!=SUCC) return ret;
	//qsort(p_list,  *p_count, sizeof(flag_emailid_stru),cmp_flag_emailid );
	sort(p_list, p_list+ (*p_count) , cmp_flag_emailid);

	//for (uint32_t i=0;i< *p_count; i++ ){
		//DEBUG_LOG("++[%d][%d]",(p_list+i)->flag, (p_list+i)->emailid);
	//}


	*p_noread_count=0;

	if ((*pp_list =( typeof(*pp_list))malloc(
				sizeof(typeof(**pp_list) ) *(*p_count))) ==NULL){
		free(p_list);
		return SYS_ERR;
	}
			
	//for (uint32_t i=0;i< *p_count; i++ ){
		//DEBUG_LOG("++[%d][%d]",(p_list+i)->flag, (p_list+i)->emailid);
	//}

	for (uint32_t i=0;i< *p_count; i++ ){
		if( (p_list+i)->flag==0 )
			(*p_noread_count)++;		
		*(*pp_list +i)=(p_list+i)->emailid; 
	}
	free(p_list);

	return ret;
}
inline int cmp_email_info( const email_info_t &p1, const email_info_t &p2)
{
	if (p1.flag==p2.flag ){
		return p2.emailid<p1.emailid;
	}else{
		return p1.flag < p2.flag; 
	}
}
int Cuser_email::get_email_info_list(userid_t userid, uint32_t *non_read_count, uint32_t *read_count,
	email_info_t **pp_list)
{
	uint32_t old_time  =time(0) - 3*30*24*3600;
	remove_by_sendtime(userid, old_time);

	int ret = this->get_user_email_list(userid, non_read_count, pp_list);
	if(ret != SUCC)
		return ret;
	sort((*pp_list), (*pp_list) + (*non_read_count), cmp_email_info);
	for(uint32_t k = 0; k < *non_read_count; ++k){
		if((*pp_list + k)->flag == 1){
			++(*read_count);
		}//if
	}//for
	(*non_read_count) -= (*read_count);
	return ret;
}

int Cuser_email::set_read(userid_t userid,uint32_t emailid )
{
	sprintf( this->sqlstr, " update %s set flag=1 \
					where id=%u    ", 
				this->get_table_name(userid), 
				emailid );
	STD_SET_RETURN(this->sqlstr,emailid, USER_EMAIL_NOFIND_ERR );	
}

int Cuser_email::set_is_award(userid_t userid, uint32_t emailid)
{
	sprintf(this->sqlstr, "update %s set is_award = 1 where id = %u",
			this->get_table_name(userid),
			emailid
			);
	STD_SET_RETURN(this->sqlstr,emailid, USER_EMAIL_NOFIND_ERR );	
}

int Cuser_email::remove_by_sendtime(userid_t userid, uint32_t old_time)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and  sendtime <= %u",
			this->get_table_name(userid),
			userid,
			old_time
		   );

	STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}
int Cuser_email::remove(userid_t userid,uint32_t emailid )
{
	sprintf( this->sqlstr, " delete from %s \
					where id=%u     ", 
				this->get_table_name(userid), 
			emailid );

	STD_REMOVE_RETURN_EX (this->sqlstr, USER_EMAIL_NOFIND_ERR );
}

int Cuser_email::remove_by_userid(userid_t userid)
{
	sprintf( this->sqlstr, " delete from %s \
					where userid=%u   ", 
					this->get_table_name(userid), userid );
	STD_REMOVE_RETURN_EX (this->sqlstr, SUCC );
}

int Cuser_email::remove_email_list(userid_t userid, uint32_t p_count, emailsys_del_list_in_item* p_in_item)
{
	std::ostringstream in_str;
    for(uint32_t i = 0; i < p_count; ++i){
        in_str << (p_in_item + i)->email_index;
        if(i < p_count - 1){
            in_str << ',';
        }
    }
    in_str <<  ',' << userid;
    sprintf(this->sqlstr, " delete from %s where id in (%s)",
            this->get_table_name(userid),
            in_str.str().c_str()
           );
	
	STD_REMOVE_RETURN_EX (this->sqlstr, SUCC );
}
