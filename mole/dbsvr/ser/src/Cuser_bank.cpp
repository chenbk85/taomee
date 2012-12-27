
/**
 * ============================================================================
 * @file    Cuser_bank.cpp
 *
 * @brief   添加，删除，查询银行存款记录函数。
 *
 * compiler gcc version 4.1.2
 *
 * platfomr Debian 4.1.1-12
 *
 * copyright TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ============================================================================
 */




#include <time.h> 
#include <math.h>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "Cuser_bank.h"
#include "common.h"


/**
 * @brief  	Cuser_bank类的构造函数 
 * @param   db数据库类指针
 * @return  无返回值 
 * @notice  基类可以根据userid找到相应的数据库和表，USER数据库前缀，t_user_bank表的前缀，
 * 			userid根据userid查询表
 */
Cuser_bank :: Cuser_bank(mysql_interface *db) : CtableRoute100x10(db, "USER", "t_user_bank", "userid")
{
	
}


/**
 * @brief  	得到某个用户的存款记录数 
 * @param   userid 用户ID
 * @param	count 存款记录数
 * @return  DB_SUCC 查询成功
 * @return  DB_ERR  查询失败
 */
int Cuser_bank :: get_record_num(const uint32_t userid, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(userid) from %s where userid=%u",
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		/*存储查询结果*/
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

  
/**
 * @brief   插入一条存款记录到表中 
 * @param   userid 用户ID
 * @param   money 存入的钱数
 * @param   flag 是否允许中途取出标志位
 * @param	timelimit 存款的期限
 * @return  DB_SUCC 查询成功
 * @return  RECORD_EXIST 记录已经存在
 */
int Cuser_bank :: insert_bank_money(const uint32_t userid, const user_bank_add_in *record, const uint32_t time)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u)",
			this->get_table_name(userid), userid, time,
			record->xiaomee, record->flag, record->timelimit);
	/*不能插入key完全相同的值*/
	STD_INSERT_RETURN(this->sqlstr, RECORD_EXIST_ERR);
}

/**
 * @brief 得到目前银行所有存款，不包括利息
 * @param userid
 */
int Cuser_bank:: get_total_money_in_bank(const uint32_t userid, uint32_t* total_money)
{
	sprintf(this->sqlstr, "select sum(money) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*total_money);
	STD_QUERY_ONE_END();
}

/**
 * 
 */

int Cuser_bank:: get_record_ret(const user_bank_add_in *info, uint32_t *flag )
{
	switch(info->timelimit)	{
		case 1:
			{
				if(info->time >=  1256227200){
					*flag = 1;
				}
			}
		case 2:
			{
				if(info->time >=  1256227200){
					*flag = 1;
				}
			}

	}
	return SUCC;
}

/**
 * @brief   得到一条存款的本金和利息总数 
 * @param   addxiaomee 得到的利息数
 * @param   totalmoney 钱总数
 * @return  0 成功
 * @return  REFUSE_GET 提取失败
 */
int Cuser_bank :: get_record_total_money(const user_bank_add_in *info, uint32_t *p_addxiaome, uint32_t *p_totalmoney,
		                                 uint32_t *p_tax, uint32_t *p_flag)
{	
	/*判断是否到期，如到期计算利息和总数*/
	switch(info->timelimit)	{
	case 1:
		/* 10月23日 1256227200, 1256875200*/
		if (info->time < 1256227200) {
			if (day_between(info->time) >= FIVE_DAY) {
				*p_addxiaome = (uint32_t)round(info->xiaomee * FIVE_DAY_INTEREST);
				*p_totalmoney = info->xiaomee + *p_addxiaome;
				return SUCC;
			}
		} else {
			if (day_between(info->time) >= FIVE_DAY) {
				*p_addxiaome = (uint32_t)round(info->xiaomee * FIVE_DAY_INTEREST_AFTER);
				if( info->xiaomee >= 100000) {
					uint32_t tax = (uint32_t)round(*p_addxiaome * 0.2);
					*p_tax = tax;
					*p_addxiaome -= tax;
				} else {
					*p_tax = 0;
				}
				*p_totalmoney = info->xiaomee + *p_addxiaome;
				*p_flag = 1;
				return SUCC;
			}
		}
		break;

	case 2:
		/* 10月23日 */
		if (info->time < 1256227200) {
			if (day_between(info->time) >= TEN_DAY ) {
				*p_addxiaome = (uint32_t)round(info->xiaomee * TEN_DAY_INTEREST);
				*p_totalmoney = info->xiaomee + *p_addxiaome; 
				return SUCC;
			}
		} else {
			if (day_between(info->time) >= TEN_DAY ) {
				*p_addxiaome = (uint32_t)round(info->xiaomee * TEN_DAY_INTEREST_AFTER);
				if (info->xiaomee >= 100000) {
					uint32_t tax = (uint32_t)round(*p_addxiaome * 0.2);
					*p_tax = tax;
					*p_addxiaome -= tax;
				} else {
					*p_tax = 0;
				}
				*p_totalmoney = info->xiaomee + *p_addxiaome; 
				*p_flag = 1;
				return SUCC;
			}
		}
		break;
		
	default:
		return INVALID_TIME_LIMIT_ERR;
		break;
	}

	/*中途允许提取*/	
	if (info->flag == 1) {
		*p_addxiaome = 0;
		*p_totalmoney = info->xiaomee;
		*p_tax = 0;
		return SUCC;
	}else{
		return REFUSE_GET_ERR;
	}
}


/**
 * @brief   得到一条存款记录的信息
 * @param   userid 用户ID
 * @param   time 存入时间 
 * @return  DB_SUCC 成功
 * @return  DB_ERR 失败
 */
int Cuser_bank :: get_record_info(const uint32_t userid, const uint32_t time, user_bank_add_out *info)
{
	sprintf(this->sqlstr, "select time, money, flag, timelimit from %s where userid = %u \
			AND time = %u", this->get_table_name(userid), userid, time);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		/*存储记录信息*/
		INT_CPY_NEXT_FIELD(info->time);
	    INT_CPY_NEXT_FIELD(info->xiaomee);	
	    INT_CPY_NEXT_FIELD(info->flag);
	    INT_CPY_NEXT_FIELD(info->timelimit);
	STD_QUERY_ONE_END();
}
/**
 * @brief   删除一条存款记录
 * @param   userid 用户ID
 * @param   time 存款存入时间
 * @return  DB_SUCC 成功
 * @return  DB_ERR 失败
 */
int Cuser_bank :: delete_bank_money(const uint32_t userid, const uint32_t time)
{
	sprintf(this->sqlstr, "delete from %s where userid=%u AND time=%u",
			this->get_table_name(userid), userid, time);
	STD_INSERT_RETURN(this->sqlstr, RECORD_NOT_EXIST_ERR);
}

int Cuser_bank:: update_value(const userid_t userid, const uint32_t time, 
		const uint32_t money )
{
	sprintf(this->sqlstr, "update %s set money = %u where userid = %u and time = %u",
			this->get_table_name(userid),
			money,
			userid,
			time
			);

	STD_INSERT_RETURN(this->sqlstr, RECORD_NOT_EXIST_ERR);
}

/**
 * @brief   得到一个用户的全部记录信息 
 * @param   userid 用户ID
 * @param   pp_list 存储记录信息
 * @param   count存储记录数
 * @return  DB_SUCC 成功
 * @return  DB_ERR 失败
 */
int Cuser_bank :: get_all_record(uint32_t userid, user_bank_add_out **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select time, money, flag, timelimit from %s where userid = %u",
			this->get_table_name(userid), userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->xiaomee);
		INT_CPY_NEXT_FIELD((*pp_list + i)->flag);
		INT_CPY_NEXT_FIELD((*pp_list + i)->timelimit);
	STD_QUERY_WHILE_END();
}


/**
 * @brief   计算某一时间与当前时间相差的天数 
 * @param   pre_timet 某一时间
 * @return 	相差的天数 
 */
int Cuser_bank :: day_between(const time_t pre_time)
{
	        
	struct tm *yymmdd;
	time_t old_time;
	time_t now_time;
				         
	yymmdd = localtime(&pre_time);
	yymmdd->tm_sec = 0;
	yymmdd->tm_min = 0;
	yymmdd->tm_hour = 0;
	old_time = mktime(yymmdd);

	now_time = time(NULL);
	yymmdd = localtime(&now_time);
	yymmdd->tm_sec = 0;
	yymmdd->tm_min = 0;
	yymmdd->tm_hour = 0;
	now_time = mktime(yymmdd);
	return ((now_time - old_time) / (60 * 60 * 24));
}

/**
 * @brief  更新存款记录信息
 * @param  userid 用户ID号
 * @param  p_in 更改的信息
 */
int Cuser_bank::set_info(const userid_t userid, user_bank_set_account_in *p_in )
{
	sprintf( this->sqlstr, " update %s set \
			time=%u, \
			money=%u, \
			flag=%u, \
			timelimit=%u \
			where userid=%u and time=%u " ,
			this->get_table_name(userid),  
			p_in->time,p_in->xiaomee,p_in->flag,p_in->timelimit,
			 userid, p_in->old_time ); 
	STD_SET_RETURN_EX (this->sqlstr,  SUCC);
}

/**
 * @brief 判断是否有记录到期
 * @param userid 用户ID号
 * @return 0 没有存款到期 
 * 		   1 有存款到期
 */
int Cuser_bank :: record_out_of_day(const uint32_t userid, uint32_t *p_isok )
{
	user_bank_get_account_out_item *record_list;
	uint32_t i;
	int delay_day;
	int ret;
	uint32_t record_num;
	*p_isok = 0;

	ret = this->get_all_record(userid, &record_list, &record_num);
	if (ret!=SUCC) {
		return ret;
	}

	for (i = 0; i < record_num; i++) {
		delay_day = record_list[i].timelimit * FIVE_DAY;
		if (this->day_between(record_list[i].time) >= delay_day) {
			*p_isok = 1;
			break;
		}
	}
	free(record_list );
	return SUCC;
}
