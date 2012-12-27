/*
 * =====================================================================================
 * 
 *       Filename:  Cclientproto.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年01月10日 18时22分49秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#ifndef  CCLIENTPROTO_INC
#define  CCLIENTPROTO_INC
/*
 * =====================================================================================
 *        Class:  Cclientproto
 *  Description:  
 * =====================================================================================
 */

#include <string.h>
#include "proto.h"
#include "common.h"
#include "Csync_log.h"
typedef struct  as_msg{
		uint32_t msglen;
		uint8_t version;
		uint32_t v1;
		uint32_t v2;
		uint32_t v3;
		uint32_t v4;
		uint32_t v5;
		uint32_t v6;
		char     nick[16];
		uint32_t v7;
		uint32_t logtime;
		uint32_t v8;
		uint32_t as_msglen;
		char msg[2000];
} __attribute__((packed)) AS_MSG;

typedef struct  switch_msg{
		uint32_t serverid;
		uint32_t userid;
		uint32_t v;
		int16_t msglen;
		char msg[2000];
} __attribute__((packed)) SWITCH_MSG;


class Cclientproto
{
  public:

    /* ====================  LIFECYCLE   ========================================= */
	//基础库注册
	int userinfo_register( userid_t userid, USERINFO_REGISTER_IN *pri, 
	   	userid_t * maped_userid );

	//基础库登入
	int userinfo_login( uint32_t userid,char* passwd,uint32_t addr_type, uint32_t ip , 
	   	USERINFO_LOGIN_OUT * p_out );

	//摩尔庄园注册
	int mole_register( userid_t userid, uint32_t petcolor );
	//摩尔庄园登入
	int mole_login(userid_t userid); 

	int pp_register(userid_t userid,pp_register_in * p_in ); 


	int send_register_cmd(userid_t userid, USER_REGISTER_IN *pri, 
	   	userid_t * maped_userid  );


	int send_user_set_vip_flag_cmd(uint32_t userid,uint32_t flag);
	//game score
	int game_init_game_list(uint32_t gameid , uint32_t count  );
	int game_get_score_list (uint32_t gameid , 
			LIST_COUNT * out, SCORE_LIST_ITEM  ** pp_list );
	int game_update_game_score(uint32_t gameid , 
			userid_t userid,uint32_t score,char*nick  );
	//test
	int send_map_email_userid_cmd(userid_t *userid,char * email);
	int send_get_userid_by_email_cmd(char*email, uint32_t *userid);
	int send_del_map_email_userid_cmd(uint32_t userid,char * email);

	int send_login_by_email_cmd(char * email,char * passwd,
		uint32_t addr_type, uint32_t ip , 
	   	userid_t * userid, USERINFO_LOGIN_OUT * out);

	int send_get_pet_cmd(int pet_id);
	int pet_get_pet_with_msglist (userid_t  pet_id);
	int send_get_noused_serial_cmd(char*id);
	int send_check_serial_noused_cmd(char*id) ;
	int send_set_serial_used_cmd(char*id,uint32_t useid );
	int send_update_attire_used_cmd(userid_t userid);
	int send_get_attire_list_cmd(userid_t userid );
	int send_get_pet_list_cmd(int pet_id);

	int user_get_offline_msg_list(userid_t userid);

	int user_update_homeattirelist(userid_t userid);
	int send_get_user_cmd();
	int send_update_user_online_cmd(uint32_t userid);
	int send_user_del_blackuser_cmd(userid_t userid,userid_t blackuserid );
	int send_user_get_blackuser_cmd(userid_t userid);
	int send_set_user_enabled_cmd(userid_t userid);
	int send_user_del_friend_cmd(uint32_t userid);
	int user_get_home(uint32_t userid);
	int send_user_add_friend_cmd(userid_t userid, userid_t friendid);

	int  send_user_add_blackuser_cmd(userid_t userid, userid_t blackuserid );
	int pay_pay_attire_cmd(uint32_t userid);
	int pay_change_damee(uint32_t userid,int32_t value  );
	int f_USER_CHANGE_USER_VALUE(userid_t userid, USER_CHANGE_USER_VALUE_IN *p_in);

	int pay_init_user(uint32_t userid);
	int pay_set_month_by_damee (uint32_t userid);
	int pay_serial_increment(uint32_t userid,char* serialid,uint32_t *p_price,uint32_t *p_leave_damee );

//
	int serial_set_serial_used_cmd(uint64_t serialid, 
		uint32_t useflag,uint32_t *p_price );
	int serial_set_serial_unused_cmd(uint64_t serialid);

	int f_USER_BUY_ATTIRE_BY_XIAOMEE(userid_t userid  );
	int f_USER_SET_USER_BIRTHDAY_SEX(userid_t userid,uint32_t birthday, uint32_t sex );
	int f_PAY_SET_NO_AUTO_MONTH(uint32_t userid);
	int f_PAY_GET_MONTH_MSG(uint32_t userid);
	int f_PAY_VALIDATE_NCP(uint32_t userid);
	int f_PAY_VALIDATE_NCP(uint32_t transid, uint32_t damee  );
	int f_USER_CHANGE_PASSWD_NOCHECK ( userid_t userid);
	int msgboard_add_msg();
	int msgboard_get_msglist(uint32_t boardid, uint32_t pageid );
	int send_data(char* filename);

	int f_MSGBOARD_GETMSG_BY_MSGID();
	int f_GET_REGISTER_SERIAL(uint32_t register_serialid  , uint32_t * p_userid );
	int f_GET_REGISTER_SERIAL(char* register_serialid  , uint32_t * p_userid );
	int f_USER_SET_USER_PET_COMM(uint32_t userid);
	int f_USER_ADD_USER_PET(uint32_t userid);
	int f_USER_GET_USER_PET(uint32_t userid);
	int f_USER_SET_USER_PET_INFO(uint32_t userid);
	int f_USER_SWAP_ATTIRE_LIST (uint32_t userid);
	int f_PAY_AUTO_MONTH_RECORD_GEN ( );
	int f_PAY_SET_AUTO_MONTH_DEALMSG(uint32_t transid , uint32_t dealflag );
	int f_USERMSG_SET_REPORT_MSG(userid_t userid);
	int f_USERMSG_GET_REPORT_BY_DATE_USERID();
	int f_USERMSG_GET_WRITING();
	int f_SERIAL_GET_NOUSED_ID();
	int f_SU_LOGIN();

	int  f_USERMSG_ADD_WRITING();
	int f_PAY_GET_DAMEE_LIST(userid_t userid );
	int f_PAY_GET_PAY_INFO ( userid_t userid);
	int f_USERMSG_GET_REPORT_BY_DATE();
	int f_PAY_DEL_MONTH ( userid_t userid);

	int f_DV_REGISTER( userid_t userid,uint32_t sex,  char* passwd,char* nick,char*email   );
	int f_DV_CHANGE_PASSWD( userid_t userid,
 		 char* newpasswd);
	int f_DV_CHANGE_NICK( userid_t userid, char* newnick);

	int f_SYSARG_DAY_ADD ( userid_t userid);
	int f_PROC_SET_VIP_ONTIME();
	int f_USER_TEST_PROTO( userid_t userid );
	int f_USERINFO_REGISTER(userid_t userid,USERINFO_REGISTER_IN *p_in); 
	int f_USERINFO_DEL_USER(userid_t userid); 
	int user_add_offline_msg(uint32_t userid,const char * msg );

	int  user_add_attire(userid_t userid, uint32_t attiretype ,	
		uint32_t attireid, uint32_t count, uint32_t def_maxcount );

	int  user_del_attire(userid_t userid, uint32_t attiretype ,	
		uint32_t attireid, uint32_t count, uint32_t def_maxcount );

	int f_HOPE_GET_HOPE_LIST_BY_DATE( uint32_t logdate , uint32_t index,
		LIST_COUNT * out , HOPE_GET_HOPE_BY_DATE_OUT_ITEM ** pp_list  );
	int  f_USER_GET_FLAG ( userid_t userid, USER_GET_FLAG_OUT  *p_out );
	int f_USERINFO_GET_USER_EMAIL( userid_t userid,
		USERINFO_GET_USER_EMAIL_OUT  *p_out );
	int  f_USERINFO_GET_NICK( userid_t userid, USERINFO_GET_NICK_OUT  *p_out );
	int  f_USERINFO_GET_NICK_FLIST( userid_t userid, USERINFO_GET_NICK_FLIST_OUT *p_out );
	int  f_EMAILSYS_ADD_EMAIL( userid_t userid, EMAILSYS_ADD_EMAIL_IN_HEADER *p_in );
	int  f_SWITCH_ADD_USER_MSG( userid_t userid, const char * msg);



	int send_login_cmd( uint32_t userid,char* passwd,uint32_t addr_type, uint32_t ip , 
	   	USERINFO_LOGIN_OUT * p_out );
	int f_OTHER_SYNC_VIP(userid_t userid,uint32_t vipflag );
	char sendbuf[4096];
    Cclientproto (const char * aipaddr, u_short aport);  /* constructor */
    Cclientproto (){ }	/* constructor */

    void set_ip_port (const char * aipaddr, u_short aport); 

	void set_pub_buf(char *buf,short cmd_id,int id,short private_size);


  protected:
	int net_send();

	int	net_send_no_return();
	int  send_register_user_cmd( userid_t userid, USER_REGISTER_IN *pri );
	int user_del_user( uint32_t userid);


  private:
	int  sndlen;
	char * recvbuf;
	int  rcvlen;
	int  ret;
	short  result;
	int sockfd ;
	u_short port;
	char ipaddr[16];
}; /* -----  end of class  Cclientproto  ----- */

#endif   /* ----- #ifndef CCLIENTPROTO_INC  ----- */
