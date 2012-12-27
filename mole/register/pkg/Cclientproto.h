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

//派派:注册
#define PP_REGISTER_CMD                     (0x1800 |NEED_UPDATE)
#define PP_LOGIN_CMD                     (0x1803 )
struct  mole_register_in{
    uint32_t color;
    char nick [NICK_LEN];
}__attribute__((packed));

typedef  mole_register_in pp_register_in;
struct mole_login_out{
	uint32_t	vip_flag;
	uint32_t	friendcount;
	uint32_t	frienditem[FRIEND_COUNT_MAX];
}__attribute__((packed));



typedef  mole_login_out  pp_login_out ;





class Cclientproto
{
  public:

    /* ====================  LIFECYCLE   ========================================= */
	//基础库注册
	int userinfo_register( userid_t userid, userinfo_register_ex_in *pri, 
	   	userid_t * maped_userid );

	//基础库登入
	int userinfo_login_ex( userid_t userid, char * passwd , 
	   	uint32_t *p_gameflag  );

	//基础库登入byemail
	int userinfo_login_by_email(  char * email,char * passwd,
	    userid_t *p_userid , 	uint32_t *p_gameflag  );

	
	//得到加入的产品标志
	int userinfo_get_gameflag(  userid_t userid , 	uint32_t *p_gameflag  );

	//设置加入哪个产品
	int userinfo_add_game(  userid_t userid , uint32_t 	gameidflag  );

	//摩尔庄园
	int mole_register(userid_t userid, mole_register_in * p_in ); 
	int mole_login( userid_t userid, mole_login_out * p_out );

	//pp
	int pp_register(userid_t userid,pp_register_in * p_in );
	int pp_login(userid_t userid,pp_login_out * p_in );






//======================================================
	//基础库登入
	int userinfo_login( uint32_t userid,char* passwd,uint32_t addr_type, uint32_t ip , 
	   	userinfo_login_out * p_out );


	//摩尔庄园注册
	int mole_register( userid_t userid, uint32_t petcolor );


	int send_register_cmd(userid_t userid, user_register_in *pri, 
	   	userid_t * maped_userid  );


	int mole_login( uint32_t userid );

	int send_user_set_vip_flag_cmd(uint32_t userid,uint32_t flag);
	//game score
	int game_init_game_list(uint32_t gameid , uint32_t count  );
	int game_get_score_list (uint32_t gameid , 
			stru_count * out, score_list_item  ** pp_list );
	int game_update_game_score(uint32_t gameid , 
			userid_t userid,uint32_t score,char*nick  );
	//test
	int send_get_userid_by_email_cmd(char*email, uint32_t *userid);
	int send_del_map_email_userid_cmd(uint32_t userid,char * email);

	int send_login_by_email_cmd(char * email,char * passwd,
		uint32_t addr_type, uint32_t ip , 
	   	userid_t * userid, mole_login_out * out);

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
	int f_user_change_user_value(userid_t userid, user_change_user_value_in *p_in);

	int pay_init_user(uint32_t userid);
	int pay_set_month_by_damee (uint32_t userid);
	int pay_serial_increment(uint32_t userid,char* serialid,uint32_t *p_price,uint32_t *p_leave_damee );

//
	int serial_set_serial_used_cmd(uint64_t serialid, 
		uint32_t useflag,uint32_t *p_price );
	int serial_set_serial_unused_cmd(uint64_t serialid);

	int f_user_buy_attire_by_xiaomee(userid_t userid  );
	int f_user_set_user_birthday_sex(userid_t userid,uint32_t birthday, uint32_t sex );
	int f_pay_set_no_auto_month(uint32_t userid);
	int f_pay_get_month_msg(uint32_t userid);
	int f_pay_validate_ncp(uint32_t userid);
	int f_pay_validate_ncp(uint32_t transid, uint32_t damee  );
	int f_user_change_passwd_nocheck ( userid_t userid);
	int msgboard_add_msg();
	int msgboard_get_msglist(uint32_t boardid, uint32_t pageid );
	int send_data(char* filename);

	int f_msgboard_getmsg_by_msgid();
	int f_get_register_serial(uint32_t register_serialid  , uint32_t * p_userid );
	int f_get_register_serial(char* register_serialid  , uint32_t * p_userid );
	int f_user_set_user_pet_comm(uint32_t userid);
	int f_user_add_user_pet(uint32_t userid);
	int f_user_get_user_pet(uint32_t userid);
	int f_user_set_user_pet_info(uint32_t userid);
	int f_user_swap_attire_list (uint32_t userid);
	int f_pay_auto_month_record_gen ( );
	int f_pay_set_auto_month_dealmsg(uint32_t transid , uint32_t dealflag );
	int f_usermsg_set_report_msg(userid_t userid);
	int f_usermsg_get_report_by_date_userid();
	int f_usermsg_get_writing();
	int f_serial_get_noused_id();
	int f_su_login();

	int  f_usermsg_add_writing();
	int f_pay_get_damee_list(userid_t userid );
	int f_pay_get_pay_info ( userid_t userid);
	int f_usermsg_get_report_by_date();
	int f_pay_del_month ( userid_t userid);

	int f_dv_register( userid_t userid,uint32_t sex,  char* passwd,char* nick,char*email   );
	int f_dv_change_passwd( userid_t userid,
 		 char* newpasswd);
	int f_dv_change_nick( userid_t userid, char* newnick);

	int f_sysarg_day_add ( userid_t userid);
	int f_proc_set_vip_ontime();
	int f_user_test_proto( userid_t userid );
	int f_userinfo_register(userid_t userid,userinfo_register_in *p_in); 
	int f_userinfo_del_user(userid_t userid); 
	int user_add_offline_msg(uint32_t userid,const char * msg );

	int  user_add_attire(userid_t userid, uint32_t attiretype ,	
		uint32_t attireid, uint32_t count, uint32_t def_maxcount );

	int  user_del_attire(userid_t userid, uint32_t attiretype ,	
		uint32_t attireid, uint32_t count, uint32_t def_maxcount );

	int f_hope_get_hope_list_by_date( uint32_t logdate , uint32_t index,
		stru_count * out , hope_get_hope_by_date_out_item ** pp_list  );
	int  f_user_get_flag ( userid_t userid, user_get_flag_out  *p_out );
	int f_userinfo_get_user_email( userid_t userid,
		userinfo_get_user_email_out  *p_out );
	int  f_USERINFO_GET_NICK( userid_t userid, userinfo_get_nick_out  *p_out );
	int  f_userinfo_get_nick_flist( userid_t userid, userinfo_get_nick_flist_out *p_out );
	int  f_emailsys_add_email( userid_t userid, emailsys_add_email_in_header *p_in );
	int  f_switch_add_user_msg( userid_t userid, const char * msg);



	int send_login_cmd( uint32_t userid,char* passwd,uint32_t addr_type, uint32_t ip , 
	   	mole_login_out * p_out );

	int  send_map_email_userid_cmd(userid_t userid,char * email,
		userid_t *p_map_userid );
	int f_other_sync_vip(userid_t userid,uint32_t vipflag );
	char sendbuf[4096];
    Cclientproto (const char * aipaddr, u_short aport);  /* constructor */
    Cclientproto (){ }	/* constructor */

    void set_ip_port (const char * aipaddr, u_short aport); 

	void set_pub_buf(char *buf,short cmd_id,int id,short private_size);


  protected:
	int net_send();

	int	net_send_no_return();
	int  send_register_user_cmd( userid_t userid, user_register_in *pri );
	int user_del_user( uint32_t userid);
	int f_userinfo_register_ex(userid_t userid,userinfo_register_ex_in *p_in);


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
