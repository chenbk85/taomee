/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_question.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2009 04:40:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"

/**
 * @brief 对ROOMINFO数据库中的t_roominfo_question表进行操作。包括插入更新记录，
 * 插入记录，得到记录的信息
 */

class Croominfo_question : public CtableRoute10x10 {
	public:
		/* 信息以十个库保存i,每个库中包含十个表 */
		Croominfo_question(mysql_interface *db);
		/*对用户保存的信息进行设置*/
		int set(const userid_t &userid, const int &right, const int &question_id);
		/*在用户登入的时候返回相应的信息*/
		int get(const userid_t &userid, roominfo_question_get_out &out);
		/*插入用户记录*/
		int insert(const userid_t &userid);
		
	private:
		/*更新用户的信息*/
		int update(const userid_t &userid, const int &right, const int &new_question_id);
};
