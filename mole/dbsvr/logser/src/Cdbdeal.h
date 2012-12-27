/*
 * =====================================================================================
 * 
 *       Filename:  dbdeal.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 16时40分57秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  DBDEAL_INC
#define  DBDEAL_INC
#include "mysql_iface.h"
#include "Clogtable.h"
#include "logproto.h"
#include "Cfunc_route_base.h"
/*
 * =====================================================================================
 *        Class:  Cdbdeal
 *  Description:  
 * =====================================================================================
 */

class Cdbdeal;
//定义处理函数指针
typedef   int(Cdbdeal::*P_DEALFUN_T)(char *recvbuf);
typedef   Ccmdmap <P_DEALFUN_T> DEALFUN ;
typedef   Ccmdmaplist< DEALFUN >  CMDMAP_LIST;	

//------------------定义处理函数-----------end

class Cdbdeal:public Cfunc_route_base < Ccmdmap <P_DEALFUN_T>  >
{
  private:
	Clogin login;
	Cpetgrade petgrade;
	char logtime[20];
	Cserver server;
	Cuserinfo userinfo;
	Cpetinfo petinfo;
	Conline_usercount online_usercount ;
	Cxiaomee_use xiaomee_use;
	Cday_count day_count;
	Cday_count_ex day_count_ex;
	Cuser_opt user_opt;
  public:
    /* ====================  LIFECYCLE   ========================================= */
    Cdbdeal (mysql_interface * db);  /* constructor */
	void setlogtime(char * a_logtime){sprintf(this->logtime,"%s0000",a_logtime);}
	int checkserverloged(char *ip, bool * isloged  );
	int insertserver(char *ip, uint32_t logcount);

	int daydeal_userinfo( char * startdate , char *enddate,int8_t usertype );
	int daydeal_petinfo( char * startdate , char *enddate );
	int online_usercount_all(char * startdate);

	int deal(char *recvbuf );
	int login_cmd(char *recvbuf );
	int user_reg_cmd(char *recvbuf );
	int online_usercount_cmd(char *recvbuf );
	int xiaomee_use_cmd(char *recvbuf );
	int day_count_cmd(char *recvbuf );

	int user_on_off_line(char *recvbuf );
    /* Use compiler-generated copy constructor, assignment operator and destructor */
		
}; /* -----  end of class  Cdbdeal  ----- */
#endif   /* ----- #ifndef DBDEAL_INC  ----- */
