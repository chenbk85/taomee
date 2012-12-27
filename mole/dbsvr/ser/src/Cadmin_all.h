/*
 * =====================================================================================
 * 
 *       Filename:  Cadmin_all.h
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

#ifndef  CADMIN_ALLINCL
#define  CADMIN_ALLINCL
#include "CtableWithKey.h"
#include "proto.h"


class Cadmin_all : public CtableWithKey{
	protected:
		
		int update_flag(userid_t adminid ,const char * flag_type  , uint32_t  flag);
		int	get_flag(userid_t adminid ,const char * flag_type   ,  uint32_t * flag);
	public:
		Cadmin_all(mysql_interface * db ); 

		int set_flag( userid_t adminid  ,const  char * flag_type  , 
			   	uint32_t flag_bit ,  bool is_true );
		int get_admin_alllist(uint32_t index,uint32_t *p_count,  admin_all_item ** pp_item );
		int insert( admin_add_admin_in *p_in  );
		int change_passwd(userid_t  adminid,  char * oldpasswd,char *newpasswd );
		int del(userid_t adminid);
		int user_check(userid_t  adminid,  char * passwd, char  *nick );
		int update_passwd(userid_t adminid, char * newpasswd  );
		int get_adminid_by_nick(char *nick, uint32_t * p_adminid );
};
#endif   /* ----- #ifndef CADMIN_ALLINCL  ----- */

