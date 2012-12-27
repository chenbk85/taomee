/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_info_ex.h
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

#ifndef  CUSER_INFO_EX
#define  CUSER_INFO_EX
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 
using namespace std;
#define  USEREX_FLAG_SEX    	  		0x00000002
#define  FLAG_STR    		  			"flag" 

class Cuser_info_ex :public CtableRoute {
	private:
		inline int get_flag(userid_t userid,const char *  flag_type,  uint32_t * p_flag); 
		inline int update_flag(userid_t userid,const char * flag_type , uint32_t  flag);
	public:
		Cuser_info_ex(mysql_interface * db ); 
		int insert( userid_t userid,  user_info_ex_item * u);
		int del(userid_t userid );
		int	check_existed( userid_t userid);
		int set_flag( userid_t userid,const char * flag_type , uint32_t flag_bit, bool is_true );
		int isset_birthday(userid_t userid);
		int update_birthday_sex(userid_t userid  ,  uint32_t birthday,uint32_t sex );
		int update(userid_t userid, user_info_ex_item * u);
		int get(userid_t userid, user_info_ex_item * p_out);
		int update_for_pay(userid_t userid  , userinfo_set_payinfo_in *p_in );
};


#endif   /* ----- #ifndef CUSER_INFO_EX  ----- */

