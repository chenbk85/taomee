/*
 * =====================================================================================
 * 
 *       Filename:  Cadmin.h
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

#ifndef  CADMIN_INCL
#define  CADMIN_INCL
#include "Ctable.h"
#include "proto.h"
#define  ADMIN_USED_FLAG 0x00000001
#define  ADMIN_FLAG_STR  "flag" 


class Cadmin : public Ctable{
	protected:
		
		int update_flag(userid_t adminid ,const char * flag_type  , uint32_t  flag);
		int	get_flag(userid_t adminid ,const char * flag_type   ,  uint32_t * flag);
	public:
		Cadmin(mysql_interface * db ); 

		int get_adminlist(uint32_t index,uint32_t *p_count,  admin_item ** pp_item );
		int insert(userid_t adminid, char *  nick );
		int user_check(userid_t  adminid,  char * passwd,char * p_nick,uint32_t *p_flag );

		int get_adminid_by_nick(char *nick, uint32_t * p_adminid );
		int change_passwd(userid_t  adminid,  char * oldpasswd,char *newpasswd );
		//int set_flag(userid_t adminid, uint32_t flag );
		int del(userid_t adminid);

		int set_flag( userid_t adminid  ,const  char * flag_type  , 
			   	uint32_t flag_bit ,  bool is_true );
};
#endif   /* ----- #ifndef CADMIN_INCL  ----- */

