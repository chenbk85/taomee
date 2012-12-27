/*
 * =====================================================================================
 *
 *       Filename:  Cuser_throu_time_mail.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/29/2011 03:54:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_THROUGH_TIME_INC
#define CUSER_THROUGH_TIME_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_through_time_mail:public CtableRoute100x10
{
	private:

	public:
		Cuser_through_time_mail(mysql_interface *db);
		int insert(userid_t userid, roominfo_thr_tim_sp_t *cont);
		int get_mails(userid_t userid, roominfo_thr_tim_mail_t **pp_list, uint32_t *p_count);
		int remove(userid_t userid, uint32_t time);
		int get_mails_sendtime(userid_t userid, uint32_t **pp_list, uint32_t *p_count);
		int get_random_mail(userid_t userid, user_get_time_mail_out *p_out);
		int get_non_dig_cnt(userid_t userid, uint32_t *count);


};

#endif
