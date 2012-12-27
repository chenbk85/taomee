/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_writing.h
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

#ifndef  CUSER_WRITING_INCL
#define  CUSER_WRITING_INCL
#include "Ctable.h"
#include "proto.h"
class Cuser_writing : public Ctable{
	protected:
	public:
		Cuser_writing(mysql_interface * db ); 
		int insert(  uint32_t type, userid_t userid , msg_item *msg) ;
		int get( usermsg_get_writing_in *p_in ,usermsg_get_writing_out *p_out  ) ;

		int get_count(  usermsg_get_writing_count_in *p_in ,uint32_t *p_count);
		int set_report(userid_t userid, char * date, uint32_t type, 
				uint32_t flag, msg_item * reportmsg );
		int clear_by_delete_flag( usermsg_clear_writing_in *p_in);
		int get_by_userid(usermsg_get_writing_in *p_in ,
				uint32_t *p_count );
		int set_type(userid_t userid, char * date, uint32_t type );
				virtual char*  get_table_name(uint32_t type );

		int set_del_flag(userid_t userid, char * date , msg_item *p_msg_item );

		int get_list( usermsg_searchkey_item *p_in ,
	 		uint32_t *p_count, usermsg_get_writing_list_out_item **pp_out_item );
		int set_sql_for_get ( usermsg_searchkey_item *p_in , const char * select_field_str , uint32_t limit_count);
};
#endif   /* ----- #ifndef CUSER_WRITING_INCL  ----- */

