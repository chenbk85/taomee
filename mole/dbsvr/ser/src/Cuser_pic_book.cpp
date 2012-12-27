/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_pic_book.cpp
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年12月22日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zheng, zheng@taomee.com
 *
 * =====================================================================================
 */

#include "Cuser_pic_book.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include<ctime>

/**
 * @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_pic_book:: Cuser_pic_book(mysql_interface * db ) 
	:CtableRoute( db, "USER", "t_user_pic_book", "user_id")
{ 
}

/**
 * @brief 插入一个图鉴
 * @param userid 哪个用户
 * @param attireid 插入的哪个图鉴
 */
int Cuser_pic_book::insert(userid_t userid, uint32_t attireid)
{
	sprintf( this->sqlstr, " insert into %s(user_id, attire_id, begin_time) values(%u, %u, %u)",
		this->get_table_name(userid), 
		userid,
		attireid,
		(uint32_t)std::time(0)
		);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

/**
 * @brief 取得已经存在的图鉴
 * @param userid 相应的用户米米号
 */
int Cuser_pic_book::get(userid_t userid, user_pic_book_get_out_item **pp_list, uint32_t *p_count)
{
    sprintf( this->sqlstr, "select attire_id from %s where user_id=%d order by begin_time DESC",
			 this->get_table_name(userid),
			 userid
		   );
    STD_QUERY_WHILE_BEGIN(this-> sqlstr, pp_list, p_count);
            INT_CPY_NEXT_FIELD((*pp_list + i)->attireid);
    STD_QUERY_WHILE_END();
}

