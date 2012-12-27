/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamu_classroom_exam.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/17/2010 12:00:07 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_lamu_classroom_exam.h"
#include "common.h"

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_lamu_classroom_exam::Cuser_lamu_classroom_exam(mysql_interface * db):
	    CtableRoute100x10(db, "USER", "t_user_lamu_classroom_exam", "userid")
{

}

/*
 * @brief 插入考试信息
 */
int Cuser_lamu_classroom_exam::insert(userid_t userid, user_lamu_classroom_exam_info exam_info)
{
	sprintf( this->sqlstr, "insert into %s (userid,	\
							quality, \
							score, \
							difficulty, \
							level_s, \
							level_a, \
							level_b, \
							exp, \
							exam_times, \
							evaluate,\
							prize) \
							values( %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, 0x00000000 )",
						this->get_table_name(userid),
						userid,
						exam_info.quality,
						exam_info.score,
						exam_info.difficulty,
						exam_info.level_s,
						exam_info.level_a,
						exam_info.level_b,
						exam_info.exp,
						exam_info.exam_times,
						exam_info.evaluate
						);

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 得到考试信息
 */
int Cuser_lamu_classroom_exam::get_exam_info(userid_t userid, 
		uint32_t exam_times, user_lamu_classroom_get_exam_info_out_header *p_out)
{
	sprintf( this->sqlstr, "select quality, \
						score, \
						difficulty, \
						level_s, \
						level_a, \
						level_b, \
						exp, \
						evaluate, \
						prize \
						from %s where userid = %u and exam_times = %u",
					this->get_table_name(userid),
					userid,
					exam_times
					);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->quality);
		INT_CPY_NEXT_FIELD(p_out->score);
		INT_CPY_NEXT_FIELD(p_out->difficulty);
		INT_CPY_NEXT_FIELD(p_out->level_s);
		INT_CPY_NEXT_FIELD(p_out->level_a);
		INT_CPY_NEXT_FIELD(p_out->level_b);
		INT_CPY_NEXT_FIELD(p_out->exp);
		INT_CPY_NEXT_FIELD(p_out->evaluate);
		BIN_CPY_NEXT_FIELD((char*)&(p_out->prize_list), sizeof(p_out->prize_list));
	STD_QUERY_ONE_END();
}

/*
 * @brief 考評資訊 (質量、評分、考評、獲得經驗、獲得獎勵) 
 */
int Cuser_lamu_classroom_exam::tw_get_exam_info(userid_t userid,
		uint32_t exam_times, user_lamu_classroom_tw_get_exam_result_out *p_out)
{
	sprintf(this->sqlstr, "select quality, score, exp, evaluate, prize from %s where userid = %u and exam_times = %u",
					this->get_table_name(userid), userid, exam_times);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->quality);
	    INT_CPY_NEXT_FIELD(p_out->score);
		INT_CPY_NEXT_FIELD(p_out->get_exp);
		INT_CPY_NEXT_FIELD(p_out->evaluate);
		BIN_CPY_NEXT_FIELD((char*)&(p_out->prize_list), sizeof(p_out->prize_list));
	STD_QUERY_ONE_END();
}

/*
 * @brief 删除考试信息
 */
int Cuser_lamu_classroom_exam::delete_info(userid_t userid)
{
	sprintf( this->sqlstr, "delete from %s where userid = %u",
					this->get_table_name(userid),
					userid
					);

	STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
}

/*
 * @brief 设置奖品信息
 */
int Cuser_lamu_classroom_exam::set_prize(userid_t userid, uint32_t exam_times, user_exam_prize_list prize_list)
{
	char prize_list_mysql[mysql_str_len(sizeof(prize_list))];
	set_mysql_string(prize_list_mysql, (char*)(&(prize_list.count)), prize_list.count * (sizeof(prize_list.exam_prize[0])) + 4);
	
	sprintf( this->sqlstr, "update %s set prize = '%s' where userid = %u and exam_times = %u",
					this->get_table_name(userid),
					prize_list_mysql,
					userid,
					exam_times
					);

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 设置奖品
 */
int Cuser_lamu_classroom_exam::set_exam_prize(userid_t userid, 
		uint32_t exam_times, uint32_t prizes_cnt, user_lamu_classroom_get_prize_in_item *p_item)
{
	user_exam_prize_list prize_list = {0};
	//memcpy((char*)&(prize_list.exam_prize[0]), p_item->prize_id, 
	//	sizeof(prizes_cnt) + prizes_cnt * sizeof(user_lamu_classroom_get_prize_in_item));
	prize_list.count = prizes_cnt;
	for (uint32_t i = 0; i < prizes_cnt; i++) {
		prize_list.exam_prize[i].prize_id = (p_item + i)->prize_id;
		prize_list.exam_prize[i].prize_cnt = (p_item + i)->prize_cnt;
	}
	int ret = this->set_prize(userid, exam_times, prize_list);
	return ret;
}
