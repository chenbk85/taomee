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

#ifndef  CROUTE_FUNC
#define  CROUTE_FUNC
#include "benchapi.h"

#include "Croute_func_def.h"
#include "common.h"
#include "Cattire_conf.h"
#include "Cincrement.h"
#include "Cclientproto.h"
#include "Csync_user.h"
#include "Cpay_history.h"
#include "Cdamee_history.h"


typedef  struct init_cfg_value{
	int  damee;
	int  month_damee;
	int  lastmonth_date;
} INIT_CFG_VALUE;

/*
 * =====================================================================================
 *        Class:  Croute_func
 *  Description:  
 * =====================================================================================
 */


class Croute_func:public Cfunc_route_base < CMD_MAP >
{
  private:
	INIT_CFG_VALUE cfg;
	mysql_interface * db;
	Cattire_conf attire_conf;
	Cincrement increment;
	Csync_user sync_user;
	Cclientproto serial_cp;
	Cpay_history pay_history;
	Cdamee_history damee_history ;
  public:
    Croute_func (int attire_shm_key,int sysc_user_msg_key,
		  	mysql_interface * db , char * serial_ip, ushort serial_port  ) ;  /* constructor */
    virtual ~Croute_func () {
			
	}
	int f_PAY_BUY_ATTIRE_BY_DAMEE(DEAL_FUN_ARG);
	int f_PAY_INIT_USER(DEAL_FUN_ARG);
	int f_PAY_MONTH_BY_DAMEE(DEAL_FUN_ARG);
	int f_PAY_CHANGE_DAMEE(DEAL_FUN_ARG);
	int f_PAY_MONTH_DEAL_DAMEE(DEAL_FUN_ARG);
	int f_PAY_SET_AUTO_MONTH_DEALMSG (DEAL_FUN_ARG);
	int f_PAY_AUTO_MONTH_RECORD_GEN(DEAL_FUN_ARG);
	int f_PAY_SET_NO_AUTO_MONTH(DEAL_FUN_ARG);
	int f_PAY_DEL_MONTH(DEAL_FUN_ARG);
	int f_PAY_GET_PAY_INFO(DEAL_FUN_ARG);



	int f_PAY_PAY_PRE(DEAL_FUN_ARG);
	int f_PAY_PAY_TRUE(DEAL_FUN_ARG);
	int f_PAY_PAY_ONCE(DEAL_FUN_ARG);
	int f_PAY_GET_DAMEE_LIST(DEAL_FUN_ARG);

	//序列号充值
	int f_PAY_ADD_DAMEE_BY_SERIAL(DEAL_FUN_ARG);

	int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen );
	
	int add_monthcount(userid_t userid, uint32_t paytype, 
		uint32_t damee,	uint32_t month_count,uint32_t automonthed, bool isRecard  );
	int deal_pay(  PAY_STRU *p_pay );



    /* Use compiler-generated copy constructor, assignment operator and destructor */
}; /* -----  end of class  Croute_func  ----- */
#endif   /* ----- #ifndef CROUTE_FUNC  ----- */

