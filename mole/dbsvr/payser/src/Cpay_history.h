/*
 * =====================================================================================
 * 
 *       Filename:  Cpay_history.h
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

#ifndef  CPAY_HISTORY_INCL
#define  CPAY_HISTORY_INCL
#include "CtableMonth.h"
#include "proto.h"

class Cpay_history :public CtableMonth{
	protected:
	public:
		Cpay_history(mysql_interface * db ); 
		//产生预交易记录
		int gen_trans( PAY_STRU *p_month,uint32_t dealcode ,uint32_t *p_transid);

		int gen_true_trans (PAY_STRU *p_month,uint32_t *p_transid );
		int set_trans_dealcode(uint32_t transid,
				uint32_t damee , uint32_t dealcode, PAY_STRU * p_pay);
		//对账
		int validate(uint32_t transid,uint32_t damee );
		int set_dealcode(uint32_t transid,uint32_t dealcode  );
		int isrecorded(uint32_t out_transid, uint32_t paytype  );
};

#endif   /* ----- #ifndef CPAY_HISTORY_INCL  ----- */

