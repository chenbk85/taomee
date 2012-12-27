/*
 * =====================================================================================
 *
 *       Filename:  Cuser_continue_login.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/24/2011 04:20:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "common.h"
#include "Cuser_continue_login.h"

enum {
	MALL_LOGIN_TYPE = 0,
	VIP_LAMU_LOGIN_VALUE = 1,
	NATIONAL_DAY_LOGIN_GIFT = 2,
	XHX_TASK_STATE = 10000,
	SPRINT_CONTINUE_LOGIN = 3,
	VIP_WEEK_CONTINUE_LOGIN = 4,
	WEEKEND_CONTINUE_LOGIN = 5,
	VIP_WEEK_CONTINUE_LOGIN2 = 6,
};
//type=0商城积分 1vip超拉成长值

Cuser_continue_login::Cuser_continue_login(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_continue_login", "userid")
{

}

int Cuser_continue_login::insert(userid_t userid, uint32_t type, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, 0)",
			this->get_table_name(userid),
			userid,
			type,
			count,
			get_date(time(0))
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_continue_login::insert(userid_t userid, uint32_t type, uint32_t count, uint32_t datetime)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, 0)",
			this->get_table_name(userid),
			userid,
			type,
			count,
			datetime
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_continue_login::insert(userid_t userid, uint32_t type, uint32_t count, 
		uint32_t datetime, uint32_t flags)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			type,
			count,
			datetime,
			flags
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_continue_login::update_count(userid_t userid, uint32_t type, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = %u where userid = %u and type = %u",
			this->get_table_name(userid),
			count,
			userid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_continue_login::update_flags(userid_t userid, uint32_t type, uint32_t flags)
{
	sprintf(this->sqlstr, "update %s set flags = %u where userid = %u and type = %u",
			this->get_table_name(userid),
			flags,
			userid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}



int Cuser_continue_login::update_date(userid_t userid, uint32_t type, uint32_t date)
{
	sprintf(this->sqlstr, "update %s set date = %u where userid = %u and type = %u",
			this->get_table_name(userid),
			date,
			userid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_continue_login::update_all(userid_t userid, uint32_t type, uint32_t count, uint32_t date)
{
	sprintf(this->sqlstr, "update %s set count = %u, date = %u where userid = %u and type = %u",
			this->get_table_name(userid),
			count,
			date,
			userid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_continue_login::select(userid_t userid, uint32_t type, uint32_t *count, uint32_t *date)
{
	sprintf(this->sqlstr, "select count, date from %s where userid = %u and type = %u",
			this->get_table_name(userid),
			userid,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
		INT_CPY_NEXT_FIELD (*date);
	STD_QUERY_ONE_END();
}

int Cuser_continue_login::get_flags(userid_t userid, uint32_t type, uint32_t *flags)
{
    sprintf(this->sqlstr, "select flags from %s where userid = %u and type = %u",
            this->get_table_name(userid),
            userid,
            type
            );

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD (*flags);
    STD_QUERY_ONE_END();
}

int Cuser_continue_login::get_count(userid_t userid, uint32_t type, uint32_t *count)
{
    sprintf(this->sqlstr, "select count from %s where userid = %u and type = %u",
            this->get_table_name(userid),
            userid,
            type
            );

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD (*count);
    STD_QUERY_ONE_END();
}

int Cuser_continue_login::cal_continue(userid_t userid, uint32_t type, user_continue_login_shop_out *p_out)
{
	uint32_t date = 0, count = 0;
	int ret = select(userid, type, &count, &date);
	if(ret == USER_ID_NOFIND_ERR){
		insert(userid, type);
		count = 1;
	}
	else{
		uint32_t now_time = time(0);
		uint32_t lastday = get_date(now_time - 3600*24);	
		uint32_t today = get_date(now_time);
		if(lastday == date){//表示连续登陆
			count = (count + 1) > 30 ? 1: (count + 1);
		}
		else{// 不是连续登陆
			if(today == date){//不是连续登陆情况下，今天连续登陆,不给积分，直接返回
				p_out->login_times = 1;
				p_out->get_count = 0;
				return 0;
			}
			count = 1;//隔天登陆，次数置为1
		}
		update_all(userid, type, count, today);

	}
	if(count == 1){//连续1次登陆送5分
		p_out->get_count = 5;
	}
	else if(count == 2){//连续2次登陆送8分
		p_out->get_count = 8;
	}
	else{//连续登陆3次到30次，送10分
		p_out->get_count = 10;
	}
	p_out->login_times = count;

	return 0;
}

/*
 * @brief 连续登陆计算超拉成长值
 * @prama *add_val:累计增加的成长值，当累计第15天时，增加6点
 */
int Cuser_continue_login::cal_superlamu_value_continue(userid_t userid, uint32_t type, uint32_t* add_val)
{
	uint32_t date = 0;
	uint32_t count = 0;
	uint32_t today = get_date(time(0));
	uint32_t lastday = get_date(time(0) - 3600 * 24);
	
	int ret = this->select(userid, type, &count, &date);
	if (ret == USER_ID_NOFIND_ERR) {
		ret = this->insert(userid, type);
		*add_val = 1;
		return SUCC;
	} else if (ret == SUCC) {
		if (date == today) {
			*add_val = 0;
			return SUCC;
		} else if (date == lastday) {
			count += 1;
		} else {
			count = 1;
		}
	} else {
		return ret;
	}

	*add_val = 1;
	if (count == 15) {
		*add_val = 6;
		count = 0;
	}
	ret = this->update_all(userid, type, count, today);
	return ret;
}

/*
 * @brief 国庆七天连续登陆
 */
int Cuser_continue_login::cal_national_day_continue(userid_t userid, uint32_t type)
{
	uint32_t today = get_date(time(0));
#ifndef TW_VER
	if (today >= 20111001 && today <= 20111007) {
#else
	if (today >= 20111014 && today <= 20111020) {
#endif
		uint32_t date = 0;
		uint32_t count = 0;
		int ret = this->select(userid, type, &count, &date);
		if (ret == USER_ID_NOFIND_ERR) {
			ret = this->insert(userid, type, 1<<16);
			return ret;
		} else if (ret == SUCC) {
			if (date == today) {
				return SUCC;
			} else {
				count += (1<<16);
				return this->update_all(userid, type, count, today);
			}
		} else {
			return ret;
		}
	}
	return SUCC;
}

/*
 * @brief 国庆7天连续登陆，查询登陆情况
 */
int Cuser_continue_login::get_national_day_info(userid_t userid, uint32_t type, uint32_t* count)
{
	uint32_t date;
	*count = 0;
	this->select(userid, type, count, &date);
	return SUCC;
}

/*
 * @brief 国庆7天连续登陆，领取奖励
 */
int Cuser_continue_login::set_national_day(userid_t userid, uint32_t type, uint32_t gift_cnt, uint32_t *state)
{
	uint32_t date;
	uint32_t count;
	int ret = this->select(userid, type, &count, &date);
	if (ret != SUCC) {
		return ret;
	}

	if (count & (1 << (gift_cnt - 1))) {
		*state = 0;
	} else {
		count = (count | (1 << (gift_cnt - 1)));
		*state = 1;
		ret = this->update_count(userid, type, count);
	}
	return SUCC;
}

/*
 * @breif type为10000时,count值表示某种状态
 */
int Cuser_continue_login::get_xhx_task_state(userid_t userid, uint32_t flag, uint32_t type, uint32_t *state)
{
	uint32_t date;
	int ret = this->select(userid, type, state, &date);
	if (ret == USER_ID_NOFIND_ERR) {
		ret = this->insert(userid, type, 0);
		*state = 0;
	}

	if (flag == 2) {	//如果flag不等于1并且*state=0时，表示要进行洗礼
		if (*state == 0) {
			*state = rand() % 2 + 1;
			ret = this->update_count(userid, type, *state);
		} else if (*state == 2) {	//返回state=4表示洗礼成功后，重新洗礼了
			*state = 4;
		}
	}
	return SUCC;
}

int Cuser_continue_login::update_inc(uint32_t userid, uint32_t type, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u and type = %u",
			this->get_table_name(userid),
			count,
			userid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_continue_login::get_two_cols(userid_t userid, uint32_t type, const char* col_1, 
		const char* col_2, uint32_t *value_1, uint32_t *value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u and type = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value_1);
		INT_CPY_NEXT_FIELD (*value_2);
	STD_QUERY_ONE_END();
}

int Cuser_continue_login::update_info(userid_t userid, uint32_t type)
{
	uint32_t date = 0, count = 0;
	int ret = select(userid, type, &count, &date);
	uint32_t now = time(0);
	if(ret != SUCC){
		ret = insert(userid, type, 1, now, 1);
	}
	else{
		uint32_t tmp_count = 0;
		uint32_t week_seq = get_week_id(now);
		uint32_t last_week_seq = get_week_id(date);
		DEBUG_LOG("now: %u, date: %u, week_sql: %u, last_week_seq: %u", now, date, week_seq, last_week_seq);
		if(week_seq == last_week_seq + 1){
			count = count + 1;
		}
		else if( week_seq != last_week_seq){
			count = 1;
		}
		tmp_count = count;
		if(count > 3){
			count = 3;
		}
		if(count >= 1 && count <= 3 && week_seq != last_week_seq){
			uint32_t flags = 0; 
			get_flags(userid, type, &flags);
			switch(count){
				case 1:
					{
						if((flags & 0x03) == 0 || (flags & 0x03) == 2){
							flags = flags & ~(3<<0);
							flags = flags | 0x01;
						}		
						break;
					}
				case 2:
					{
						if(((flags >> 2) & 0x03) == 0 || ((flags >> 2) & 0x03) == 2){
							flags = flags & ~(3 << 2);
							flags = flags | (1 << 2);
						}
						break;
					}
				case 3:
					{
						DEBUG_LOG("flags: %u ===old", flags);
						if(((flags >> 4) & 0x03) == 0 || ((flags >> 4) & 0x03) == 2){
							flags = flags & ~(3 << 4);
							flags = flags | (1 << 4);
						}
						DEBUG_LOG("flags: %u ===new", flags);
						//else{
							//if(((flags >> 2) & 0x03) == 0){
								 //flags = flags | (1 << 2);
							//}
						//}
						break;
					}
			}
			ret = this->update_flags(userid, type, flags);
		}
		ret = update_all(userid, type, tmp_count, now);
	}
	return ret;
}

int Cuser_continue_login::set_vip_login_flags(userid_t userid, uint32_t type, uint32_t index,
	   	uint32_t *state)
{
	uint32_t flags = 0;
	int ret = get_flags(userid, type, &flags);
	
	if(((flags >> ((index-1) *2)) & 0x03) == 1){
		flags = flags & (~(3 << (index-1)*2));
		ret = this->update_flags(userid, type, flags|(2<<((index-1)*2)));	
		if(ret == SUCC){
			*state = 1;
		}
	}	
	return ret;
}

int Cuser_continue_login::set_continue_login_weekend(userid_t userid, uint32_t type)
{
	uint32_t datetime = 0, count = 0;
    int ret = select(userid, type, &count, &datetime);
	DEBUG_LOG("continue login count: %u, datetime: %u", count, datetime);
	uint32_t no_change_count = count;
	uint32_t now = time(0); 
    if(ret != SUCC){
        ret = insert(userid, type, 1, now);
    }
	else{
		uint32_t week_day = get_week_day(now);
		if(week_day == 0 || week_day == 6 || week_day == 5){
			uint32_t last_week_id = get_week_id(datetime);
			uint32_t cur_week_id = get_week_id(now);
			DEBUG_LOG("continue login count: %u, datetime: %u, last_week_id: %u, cur_week_id: %u", count, datetime, last_week_id,
					cur_week_id);
			if(cur_week_id == last_week_id + 1){
				count = count + 1;
			}
			else if(cur_week_id != last_week_id){
				count = 1;	
			}
			if(count != no_change_count){
				ret = update_all(userid, type, count, now);
				ret = this->update_flags(userid, type, 0);
			}
			//uint32_t last_date = 0;
			//uint32_t table_date = get_date(datetime);
			//uint32_t table_week_day = get_week_day(datetime);
			//switch(week_day){
				//case 0:
					//{
						//if(table_week_day == 0){
							//last_date = get_date(now - 7*3600*24);		
						//}
						//else if(table_week_day == 5){
							//last_date = get_date(now - 7*3600*24 - 2*3600*24);
						//}
						//else{
							//last_date = get_date(now - 7*3600*24 - 1*3600*24);

						//}
						//break;
					//}
				//case 5:
					//{
						//if(table_week_day == 0){
							//last_date = get_date(now - 7*3600*24 + 2*3600*24);
						//}
						//else if(table_week_day == 5){
							 //last_date = get_date(now - 7*3600*24);
						//}
						//else{
							 //last_date = get_date(now - 7*3600*24 + 1*3600*24);
						//}
						//break;
					//}
				//case 6:
					//{
						//if(table_week_day == 0){
							//last_date = get_date(now - 7*3600*24 + 1*3600*24);
						//}
						//else if(table_week_day == 5){
							//last_date = get_date(now - 7*3600*24 - 1*3600*24);
						//}
						//else{
							//last_date = get_date(now - 7*3600*24);
						//}
						//break;
					//}
			//}//switch
			
			//if (last_date == table_date){
				//count = count + 1;
			//}
			//else{
				//count = 1;
			//}
		}
	}//else
	return ret;
}
