/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_email.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_EMAIL_INCL
#define  CUSER_EMAIL_INCL
#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"
struct flag_emailid_stru{
	uint32_t flag;
	uint32_t emailid;
 inline struct flag_emailid_stru& operator=(const struct flag_emailid_stru & a) {
	 flag=a.flag ;
	 emailid=a.emailid;
	 return *this; 
 }
 /*inline int operator<(struct flag_emailid_stru & a)const { */
 /*return flag<a.flag; */
 /*}*/
} ;
class Cuser_email:public CtableRoute10x10 {
	private:
	public:
		Cuser_email(mysql_interface * db ); 
		int insert(userid_t userid, user_email_item_without_id *p_item );
		int get_emailid_list(userid_t userid, 
			uint32_t *p_noread_count , uint32_t *p_count, userid_t **pp_list );
		int get_flag_emailid_list(userid_t userid, 
			uint32_t *p_count, flag_emailid_stru **pp_list );

		int get_email(userid_t userid,
		uint32_t emailid , uint32_t *p_use_flag , user_email_item  *p_item );
		int set_read(userid_t userid,uint32_t emailid );
		int remove(userid_t userid,uint32_t emailid );
		int get_unread_count(userid_t userid, uint32_t	*p_count );
		int remove_by_userid(userid_t userid);
		int get_user_email_list(userid_t userid, uint32_t *p_count, email_info_t **pp_list);
		int set_is_award(userid_t userid, uint32_t emailid);
		int get_email_info_list(userid_t userid, uint32_t *non_read_count, uint32_t *read_count,
				    email_info_t **pp_list);
		int remove_email_list(userid_t userid, uint32_t p_count, emailsys_del_list_in_item* p_in_item);
		int remove_by_sendtime(userid_t userid, uint32_t old_time);
};

#endif   /* ----- #ifndef CUSER_EMAIL_INCL  ----- */

