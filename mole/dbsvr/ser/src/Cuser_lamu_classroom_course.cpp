/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamu_classroom_course.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/02/2010 05:08:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "common.h"
#include "Cuser_lamu_classroom_course.h"

const uint32_t g_course_level[] = {10, 40, 100, 200};

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_lamu_classroom_course::Cuser_lamu_classroom_course(mysql_interface * db): 
	CtableRoute100x10(db, "USER", "t_user_lamu_classroom_course", "userid")
{
}

/*
 * @brief 添加一门课程
 */
int Cuser_lamu_classroom_course::insert(userid_t userid, uint32_t course_id)
{
	sprintf( this->sqlstr, "insert into %s (userid, course_id, course_cnt) values(%u, %u, 0)",
			this->get_table_name(userid),
			userid,
			course_id
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

/*
 * @brief 得到course_id的课时数
 */
int Cuser_lamu_classroom_course::get_course_count(userid_t userid, uint32_t course_id, uint32_t& cnt_out)
{
	sprintf( this->sqlstr, "select course_cnt from %s where userid = %u and course_id = %u",
			this->get_table_name(userid),
			userid,
			course_id
			);

	//下课时，用户id不存在或课程id不是表中的课程id
	STD_QUERY_ONE_BEGIN( this->sqlstr, USER_LAMU_CLASSROOM_CLASS_END_ERR);
	    INT_CPY_NEXT_FIELD(cnt_out);
	STD_QUERY_ONE_END();
}

/*
 * @brief 得到couser_level
 */
int Cuser_lamu_classroom_course::get_course_level(uint32_t userid, uint32_t course_id, uint32_t *plevel)
{
	uint32_t course_cnt = 0;
	int ret = this->get_course_count(userid, course_id, course_cnt);
	if (ret != SUCC) {
		return ret;
	}
	*plevel = get_value_index_insc(course_cnt, g_course_level, sizeof(g_course_level) / sizeof(g_course_level[0]), 0);
	//DEBUG_LOG("----get course cnt and level: %u, %u ----", course_cnt, *plevel);
	return ret;
}

/*
 * @brief 下课后更新course_id的课时数
 */
int Cuser_lamu_classroom_course::set_course_count(userid_t userid, uint32_t course_id, uint32_t cnt)
{
	sprintf( this->sqlstr, "update %s set course_cnt = course_cnt + %u where userid = %u and course_id = %u",
			this->get_table_name(userid),
			cnt,
			userid,
			course_id
			);
	STD_SET_RETURN_EX(this->sqlstr, USER_LAMU_CLASSROOM_CLASS_END_ERR);
}

/*
 * @brief 判断是否有course_id这门课
 */
int Cuser_lamu_classroom_course::find_course_id(userid_t userid, uint32_t course_id, uint32_t &count)
{
	sprintf( this->sqlstr, "select count(*) from %s where userid = %u and course_id = %u",
			this->get_table_name(userid),
			userid,
			course_id
		   );

	count = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

/*
 * @brief 拉取老师的课程信息
 */ 
int Cuser_lamu_classroom_course::get_course(userid_t userid, 
		user_lamu_classroom_get_record_out_item **pp_list, uint32_t *pcount)
{
	sprintf( this->sqlstr, "select course_id, course_cnt from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, pcount);
		INT_CPY_NEXT_FIELD((*pp_list + i)->course_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->course_cnt);
	STD_QUERY_WHILE_END();
}

