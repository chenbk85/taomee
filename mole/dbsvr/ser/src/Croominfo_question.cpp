/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_question.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2009 04:41:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_question.h"
/**
 * @brief 构造函数，分成十个库每个库十个表
 */
Croominfo_question :: Croominfo_question(mysql_interface *db) : CtableRoute10x10(db, "ROOMINFO", "t_question", "userid")
{

}

/**
 * @brief 更新记录信息
 * @param userid 用户的ID号
 * @param right 用户答题是否答对，如答对为1，答错为0
 * @param question_Id 题目的ID号
 */
int Croominfo_question :: set(const userid_t &userid, const int &right, const int &question_id)
{
	int ret = this->update(userid, right, question_id);
	return ret;
}

/**
 * @brief 得到用户目前答的题号，本周答对的总数，本周答的题目的总数
 * @param userid 用户的ID号
 * @param out 保存返回的值
 */
int Croominfo_question :: get(const userid_t &userid, roominfo_question_get_out &out)
{
	sprintf(this->sqlstr, "select question_id, right_count, question_count from %s \
						   where userid = %u",
						   this->get_table_name(userid),
						   userid
		  );

	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(out.question_id);
        INT_CPY_NEXT_FIELD(out.right_count);
        INT_CPY_NEXT_FIELD(out.question_count);
	STD_QUERY_ONE_END();
}

/**
 * @brief 插入一条用户的记录，初始化为零
 * @param userid 用户的ID号
 */
int Croominfo_question :: insert(const userid_t &userid)
{
	sprintf(this->sqlstr, "insert into %s values(%u, 0, 0, 0, 0, 0)",
						   this->get_table_name(userid), 
						   userid
			);

	STD_INSERT_RETURN(this->sqlstr, RECORD_EXIST_ERR);
}


/**
 * @brief 更新用户的信息，答题总数加一
 * @param userid 用户的ID号
 * @right 用户是否答对题目
 * @new_question_id  答题的ID号
 */
int Croominfo_question :: update(const userid_t &userid, const int &right, const int &new_question_id)
{
	sprintf(this->sqlstr, "update %s set question_id =question_id+1 ,\
						right_count = right_count + %u,\
					       question_count = question_count + 1, total_right = total_right + %u,\
						   total_question = total_question + 1 where userid = %u",
					       this->get_table_name(userid),
					       right,
						   right,
					       userid
	       );

	STD_SET_RETURN_EX(this->sqlstr, RECORD_NOT_EXIST_ERR);
}
