/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamu_classroom_score.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/17/2010 12:15:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "common.h"
#include "Cuser_lamu_classroom_score.h"

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_lamu_classroom_score::Cuser_lamu_classroom_score(mysql_interface * db):
	        CtableRoute100x10(db, "USER", "t_user_lamu_classroom_score", "userid")
{

}

/*
 * @brief 插入
 */
int Cuser_lamu_classroom_score::insert(userid_t userid, user_lamu_classroom_score_info score_info)
{
	char course_list_mysql[mysql_str_len(sizeof(score_info.course_list))];
	set_mysql_string(course_list_mysql, (char*)(&(score_info.course_list.count)),
						score_info.course_list.count * (sizeof(score_info.course_list.course_score[0])) + 4);

	sprintf( this->sqlstr, "insert into %s (userid, lamu_id, exam_times, score, course_info) \
		   						values(%u, %u, %u, %u, '%s')",
					this->get_table_name(userid),
					userid,
					score_info.lamu_id,
					score_info.exam_times,
					score_info.score,
					course_list_mysql
					);
	
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

/* 
 * @brief 得到所有学生的考试信息 
 */
int Cuser_lamu_classroom_score::get_exam_score(userid_t userid, 
			uint32_t exam_times, user_lamu_classroom_get_exam_info_out_item **pp_list, uint32_t *p_count)
{
	sprintf( this->sqlstr, "select lamu_id, score, course_info \
								from %s where userid = %u and exam_times = %u",
					this->get_table_name(userid),
					userid,
					exam_times
					);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lamu_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->score);
		BIN_CPY_NEXT_FIELD(&((*pp_list + i)->score_list), sizeof((*pp_list + i)->score_list));
	STD_QUERY_WHILE_END();
}

/*
 * @brief 删除所有学生的考试信息
 */
int Cuser_lamu_classroom_score::delete_score(userid_t userid)
{
	sprintf( this->sqlstr, "delete from %s where userid = %u",
			        this->get_table_name(userid),
					userid
					);
	
	STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
}
