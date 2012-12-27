/**
 * ====================================================================
 *  @file       Cuser_bank.h
 *
 *  @brief      添加，获取，删除银行的存款记录
 *
 *  platform   Debian 4.1.1-12 
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ====================================================================
 */


#ifndef  CBANK_INC
#define  CBANK_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

/**
 * @class Cuser_bank
 * @brief 实现添加，获取，删除银行的记录
 * @note 
 * @see 
 */

class Cuser_bank : public CtableRoute100x10 {
	
public:
	friend class Cuser;
	/*构造函数*/
	Cuser_bank(mysql_interface * db);

	/*得到某个用户的存款记录总数*/
	int get_record_num(const uint32_t userid, uint32_t *count);

	/*插入用户的存款记录*/
	int insert_bank_money(const uint32_t userid, const user_bank_add_in *record, const uint32_t time);

	/*得到一个存款的利息和成本的总数*/
	int get_record_total_money(const user_bank_add_in *info, uint32_t *interet, uint32_t *totalmoney,
			uint32_t *p_tax, uint32_t *p_flag);

	/*得到一条存款记录的信息*/
	int get_record_info(const uint32_t userid, const uint32_t time, user_bank_add_out *info);

	/* 得到存款总数，不包括利率 */
	int get_total_money_in_bank(const uint32_t userid, uint32_t* total_money);
	/* 返回flag*/
	int get_record_ret(const user_bank_add_in *info, uint32_t *flag );
	/*得到全部的存款记录*/
	int get_all_record(uint32_t userid, user_bank_add_out **pp_list, uint32_t *count);

	/*删除一条记录*/
	int delete_bank_money(uint32_t userid, uint32_t time);

	/*计算日期天数之差*/
	int day_between(const time_t timet);

	/*更新用户存款信息*/
	int set_info(const userid_t userid,user_bank_set_account_in *p_in );

	/*判断是否有存款记录到期*/
	int  record_out_of_day(const uint32_t userid,uint32_t  * p_isok );

	int update_value(const userid_t userid, const uint32_t time, const uint32_t money); 
};
#endif   /* ----- #ifndef CBANK_INC  ----- */

