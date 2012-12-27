/*
 * =====================================================================================
 *
 *       Filename:  ROOMINFO_rank_buf.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/06/2010 03:16:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_rank_buf.h"
#include "proto.h"
#define USERNUM 50

Cuser_rank_buf::Cuser_rank_buf(mysql_interface * db )
	:CtableRoute100x10( db, "USER","t_user_rank_buf","userid")
{ 
}

/*
 * @brief 插入记录
 */

int Cuser_rank_buf::insert(userid_t userid, uint32_t type, users_rank_buf_in *p_in)
{
	char mysql_rank_buf[mysql_str_len(sizeof(users_rank_buf_in))];
	set_mysql_string(mysql_rank_buf,(char *)p_in,sizeof(users_rank_buf_in));
	uint32_t date=get_date(time(NULL));
	sprintf(this->sqlstr,"insert into %s values(%u,%u,'%s',%u)",
		this->get_table_name(userid),
		userid,
		type,
		mysql_rank_buf,
		date
		);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 设置今天的排行buf
 */
int Cuser_rank_buf::update_rank_buf(userid_t userid, uint32_t type, userid_t other)
{
    struct user_rank_buf box = {0};
    sprintf(this->sqlstr,"select buff from %s where userid=%u and type = %u",
			this->get_table_name(userid), userid, type);
    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD(&box,sizeof(box));
    STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (type > 10000) {
		if(box.count > 10) {
			memset(box.users + 10, 0, sizeof(date_id) * (MAX_USERNUM_IN_BUF - 10));
			box.count = 10; 
		}
		if (box.count == 10) { //10 表示返回的最大记录数
			box.users[box.count - 1].id = other;
			box.users[box.count - 1].date = time(NULL);
		} else {
			box.users[box.count].id = other;
			box.users[box.count].date = time(NULL);
			++box.count;
		}
	} else {
		box.users[box.count].id = other;
		box.users[box.count].date = time(NULL);
		++box.count;
	}
  
	std::sort(box.users,box.users+box.count,cmp_date_record);
    char sql_bag[mysql_str_len(sizeof(box))];
    set_mysql_string(sql_bag,(char*)&box,sizeof(box));
    sprintf(this->sqlstr,"update %s set buff='%s' where userid=%u and type = %u",
			this->get_table_name(userid),sql_bag,userid,type);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
  

/*
 * @brief 获取buf
 */

int Cuser_rank_buf::get_rank_buf(userid_t userid ,uint32_t type, user_rank_buf_out* p_out)
{
	sprintf(this->sqlstr, "select buff from %s where userid = %u and type = %u",
			this->get_table_name(userid),
			userid,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_out, sizeof(*p_out) );
	STD_QUERY_ONE_END();

}

/*
 * @brief 清理buf
 */
int Cuser_rank_buf::clear_buf(userid_t userid, uint32_t type)
{
	if(type > 10000) {
		return SUCC;
	}
	//uint32_t date=0;
	//get_date_record(userid,type,&date);
	//DEBUG_LOG("%d--%d",date,now);

	uint32_t now = get_date(time(NULL));
	sprintf(this->sqlstr,"update %s set buff = 0x00000000,date = %u where userid = %u and type = %u \
			and date != %u",
			this->get_table_name(userid),
			now,
			userid,
			type,
			now
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @brief 获取日期
 */
/* 
int Cuser_rank_buf::get_date_record(userid_t userid, uint32_t type,uint32_t *date)
{
	sprintf(this->sqlstr,"select date from %s where userid = %u and type = %u",
			this->get_table_name(userid),
			userid,
			type
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*date);
	STD_QUERY_ONE_END();

}
*/
