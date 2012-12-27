/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_info.h
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

#ifndef  CUSER_INFO_INC
#define  CUSER_INFO_INC
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 
using namespace std;
#include "md5.h" 

//VIP...
#define  USER_FLAG1_CHANGE_PAYPASSWD 		0x10000000
#define  USER_FLAG1_CHANGE_PASSWD 			0x08000000
#define  USER_FLAG1_ISSET_PEMAIL 			0x04000000
#define  USER_FLAG1_ISSET_PAYPASSWD 		0x02000000
#define  USER_FLAG1_ISCONFIRM_PASSWDEMAIL 	0x00100000
#define  USER_FLAG1_ISSET_PASSWDEMAIL 		0x00200000

//冻结账号标志
#define  USER_FLAG1_DISABLE_ACCOUNT 		0x00400000

#define  FLAG_STR    		  		"flag" 
#define  FLAG1_STR    		  		"flag1" 

#define  FRIEND_LIST_STR 	  		"friendlist" 
#define  BLACKUSER_LIST_STR 		"blacklist" 
#define  MEE_FANS_STR 		"mee_fans" 

struct stat_info {
	uint32_t userid;
	uint32_t login_channel;
	uint32_t ip;
}__attribute__((packed));

class Cuser_info :public CtableRoute {
	private:
		inline int get_flag(userid_t userid,const char *  flag_type,  uint32_t * p_flag); 
		inline int update_flag(userid_t userid,const char * flag_type , uint32_t  flag);
		inline int get_email_by_fieldname(userid_t userid, const char * fieldname ,char * email );
		char *msglog_file;
		MD5 md5;
		
	public:
		Cuser_info(mysql_interface * db ); 
		int check_flag(userid_t userid,const char *flag_type,  uint32_t flag, bool * p_existed ) ;
		int insert( userid_t userid,  user_info_item * u);
		int del(userid_t userid );
		int	check_existed( userid_t userid);
		int set_flag( userid_t userid,const char * flag_type , uint32_t flag_bit, bool is_true );
		int update_passwd(userid_t userid  ,char * newpasswd  );

		int change_passwd(userid_t userid, char * oldpasswd,char *newpasswd  );
		int user_check(userid_t userid, char * passwd);

		int isset_birthday(userid_t userid);

		int update_birthday(userid_t userid  ,  uint32_t birthday );
		int update_email(userid_t userid  ,  char * email );
		int get_email(userid_t userid ,char * email );
		int get_passwdemail(userid_t userid ,char * email );

		int get_info_all (userid_t userid ,user_info_item *p_out );

		int update_for_pay(userid_t userid  , userinfo_set_payinfo_in *p_in );
		int check_question(userid_t userid , char*  question, char * answer );
		int isset_question(userid_t userid);
		int get_security_flag(userid_t userid,uint32_t *p_flag );
		int update_pemail(userid_t userid  ,  char * pemail );

//		int get_nick_fiendlist(userid_t userid , char * nick , id_list * idlist );

//		int get_friend_black_list(userid_t userid ,uint32_t *p_flag2  , id_list *p_friendlist, 
//	   		id_list *  p_blacklist );
//		int add_friend(userid_t userid , userid_t id );
//		int check_friend(userid_t userid , userid_t friendid );
//		int check_friend(userid_t userid , userid_t friendid, uint32_t *p_is_existed );
		int update_paypasswd(userid_t userid  ,  char * paypasswd);
		int set_paypasswd(userid_t userid,char* paypasswd  );
		int user_check_paypasswd(userid_t userid ,  char * paypasswd);
		int update_qa(userid_t userid , userinfo_set_qa_in *p_in );
		int get_question(userid_t userid , char*  question  );
		int set_passwdemail(userid_t userid , char*  passwdemail );
		int update_passwdemail(userid_t userid , char*  passwdemail );
		int login_ex(userid_t userid ,  char * passwd, uint32_t* p_game_flag );

		int get_gameflag(userid_t userid ,uint32_t * p_gameflag);
		int add_game(userid_t userid , uint32_t  gameid_flag);

		int get_history_passwd(userid_t userid, history_passwd *p_history_passwd );

		int update_history_passwd(userid_t userid, history_passwd *p_history_passwd );
		int get_passwd(userid_t userid ,  char * p_passwd);

		int login_ex_with_stat(userid_t userid, userinfo_log_with_stat_in *p_in, uint32_t *p_game_flag);
		int set_history_passwd(userid_t userid );
		int login_ex_with_email(userid_t userid,  char *passwd, uint32_t *p_falg, char *email);
		int get_bit_flag (userid_t userid, const char *flag_type, uint32_t flag_bit, uint32_t *p_flag);
		int get_login_info(userid_t userid ,userinfo_get_login_info_out *p_out );
		int set_question2(userid_t userid  , userinfo_set_question2_in *p_in );
		int get_question2(userid_t userid ,userinfo_get_question2_out *p_out );
		int get_meefan_gameflag(userid_t userid ,userinfo_get_meefan_gameflag_out *p_out );
		int get_question2_set_flag(userid_t userid ,uint32_t  *p_is_setd);
		int get_answer2(userid_t userid , stru_answer2 *p_out );
		int verify_question2(userid_t userid , stru_answer2 *p_in);
		int login_by_md5_two(userid_t userid, userinfo_login_by_md5_two_in*p_in, uint32_t *p_game_flag);
		int login_passwd_md5_two(userid_t userid ,  char * passwd_md5_two, uint32_t* p_game_flag );
	
};


#endif   /* ----- #ifndef CUSER_INFO  ----- */

