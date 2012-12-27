/*
 * =====================================================================================
 *
 *       Filename:  Cuser_throu_time_mail.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/29/2011 03:54:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_through_time_mail.h"
#include <stdlib.h>
#include <ctime>

Cuser_through_time_mail::Cuser_through_time_mail(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_through_time_mail", "userid")
{

}

int Cuser_through_time_mail::insert(userid_t userid, roominfo_thr_tim_sp_t *cont)
{
	char mysql_mail[mysql_str_len(THROUGH_TIME_MAIL_LEN)];
	set_mysql_string(mysql_mail, (char*)(cont), THROUGH_TIME_MAIL_LEN);
	sprintf(this->sqlstr, "insert into %s values(%u, %u, '%s')",
			this->get_table_name(userid),
			userid,
			(uint32_t)(time(0)),
			mysql_mail
			);

	STD_INSERT_RETURN(this->sqlstr, DB_ERR);	
}

int Cuser_through_time_mail::get_mails(userid_t userid, roominfo_thr_tim_mail_t **pp_list,
	   	uint32_t *p_count)
{
	uint32_t now = time(0) - 24 * 3600;
	sprintf(this->sqlstr, "select sendtime, msg from %s where userid = %u and sendtime < %u",
			this->get_table_name(userid),
			userid,
			now
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list,p_count );
		(*pp_list+i)->time = atoi_safe(NEXT_FIELD); 
		BIN_CPY_NEXT_FIELD((*pp_list + i)->msg, THROUGH_TIME_MAIL_LEN);
	STD_QUERY_WHILE_END();
}

int Cuser_through_time_mail::get_non_dig_cnt(userid_t userid, uint32_t *count)
{
	uint32_t now = time(0) - 24 *3600;
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and sendtime < %u",
			this->get_table_name(userid),
			userid,
			now
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		*count = atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_through_time_mail::get_mails_sendtime(userid_t userid, uint32_t **pp_list,
	   	uint32_t *p_count)
{
	sprintf(this->sqlstr, "select sendtime from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list,p_count );
		*(*pp_list+i) = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

int Cuser_through_time_mail::remove(userid_t userid, uint32_t time)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and sendtime = %u",
			this->get_table_name(userid),
			userid,
			time
			);

	STD_SET_RETURN(this->sqlstr,emailid, USER_EMAIL_NOFIND_ERR );	
}

int Cuser_through_time_mail::get_random_mail(userid_t userid, user_get_time_mail_out *p_out)
{
	roominfo_thr_tim_mail_t *p_temp = 0;
	uint32_t cnt = 0;
	get_mails(userid, &p_temp, &cnt);
	if(cnt != 0){
		uint32_t index = rand()%cnt;
		p_out->msg_len = *((uint16_t*)(p_temp + index)->msg);	
		p_out->time = (p_temp + index)->time;	
		memcpy((char*)p_out + sizeof(user_get_time_mail_out), (p_temp + index)->msg + 2 , p_out->msg_len);
		remove(userid, (p_temp + index)->time);
   }
   if(cnt - 1 != 0){
	   p_out->flag = 1;
   }
	if(p_temp != 0){
		free(p_temp);
	}
	
	return 0;
}
