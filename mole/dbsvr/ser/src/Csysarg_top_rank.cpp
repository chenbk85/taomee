/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_top_rank.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  12/13/2010 03:22:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_top_rank.h"
Csysarg_top_rank::Csysarg_top_rank(mysql_interface * db) :
	Ctable(db, "SYSARG_DB","t_sysarg_top_rank")
{

}

/*
 * @brief 更新前十排行表中的（送出或收到）苞子花数目最少的且时间最近的记录
 */
int Csysarg_top_rank::update(userid_t userid, uint32_t type, uint32_t count, uint32_t datetime)
{
	int ret = update_one_user(userid, type, count, datetime);
	if (ret == SUCC) {
		return ret;
	}
	/*
	sprintf(this->sqlstr, "update %s set userid = if(count = %u,userid, %u),\
			count = if(count = %u, count, %u), datetime = if(count = %u, datetime, %u)\
			where type = %u order by count asc, datetime desc limit 1",
			this->get_table_name(), value, userid, value, value, value, datetime, type);
	*/
	if (type == 1) {
		sprintf(this->sqlstr, "update %s set userid = %u, count = %u, datetime = %u where type = %u \
							and count < %u order by count asc, datetime desc limit 1",
					this->get_table_name(), userid, count, datetime, type, count);
	} else {
		sprintf(this->sqlstr, "update %s set userid = %u, count = %u where type = %u \
							and count < %u order by count asc, datetime desc limit 1",
					this->get_table_name(), userid, count, type, count);
	}
	STD_SET_RETURN_EX(this->sqlstr, SUCC);	
}

/*
 * @brief 获取表中的（收到或送出）的前十个记录
 */
int Csysarg_top_rank::get_top_rank(uint32_t type, sysarg_get_ranklist_out_item **pp_items, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select userid, count from %s where type = %u order by count desc, datetime asc limit 10",
		   			this->get_table_name(), type);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_items, p_count);
		INT_CPY_NEXT_FIELD((*pp_items + i)->userid);
		INT_CPY_NEXT_FIELD((*pp_items + i)->count);
		(*pp_items + i)->rank = i + 1;
	STD_QUERY_WHILE_END();
}

/*
 * @brief 如果用户已经存在于前10位中，则update这个用户的数据
 */
int Csysarg_top_rank::update_one_user(userid_t userid, uint32_t type, uint32_t count, uint32_t datetime)
{
	if (type == 1) {
		sprintf(this->sqlstr, "update %s set count = %u, datetime = %u where userid = %u and type = %u",
					this->get_table_name(), count, datetime, userid, type);
	} else {
		sprintf(this->sqlstr, "update %s set count = %u where userid = %u and type = %u",
					this->get_table_name(), count, userid, type);
	}
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_top_rank::insert(uint32_t type)
{
	sprintf(this->sqlstr, "insert into %s values(0, %u, 1, %u)",
			this->get_table_name(),
			type,
			(uint32_t)time(0)
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_top_rank::update_count(uint32_t type, uint32_t cnt)
{
	sprintf(this->sqlstr, "update %s set count = %u where userid = 0 and type = %u",
			this->get_table_name(),
			cnt,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_top_rank::update_count_time(uint32_t type, uint32_t cnt)
{
	sprintf(this->sqlstr, "update %s set count = %u, datetime = %u where userid = 0 and type = %u",
			this->get_table_name(),
			cnt,
			(uint32_t)time(0),
			type
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_top_rank::select_count_datetime(uint32_t type, uint32_t *count, uint32_t *datetime)
{
	sprintf(this->sqlstr, "select count, datetime from %s where userid = 0 and type = %u",
			this->get_table_name(),
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*count=atoi_safe(NEXT_FIELD); 
		*datetime=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Csysarg_top_rank::add(uint32_t type, uint32_t *cur_count)
{
	uint32_t datetime = 0, count = 0;
	int ret = select_count_datetime(type, &count, &datetime);
	if(ret == USER_ID_NOFIND_ERR){
		insert(type);
	}
	else{
		if(count == 1999){
			ret = update_count_time(type, count+1);
		}
		else{
			ret = update_count(type, count+1);
		}
		*cur_count = count+1;
	}

	return ret;
}
