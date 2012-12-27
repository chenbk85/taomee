/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_cup.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_CUP_INCL
#define  CUSER_CUP_INCL
#include "CtableDate.h"
#include "proto.h"
class Cuser_cup : public CtableDate{
	protected:
		//共享内存中数据
		CUP_MSG_LIST *cup_list;
		uint32_t cup1_max;
		uint32_t cup2_max;
		uint32_t cup3_max;

		int init(userid_t userid, int cup_type,int value );
		//计算出最终值，如果cup1,cup2,cup3 大于限制，返回错误
		bool cal_cup (CUP_STU *p_cup_stu, int cup_type,int changevalue );
		int update(userid_t userid, CUP_STU *p_cup_stu  );
	public:
		Cuser_cup(mysql_interface * db );
		int add_cup(userid_t userid,uint32_t groupid , uint32_t gameid, uint32_t cup_type );
		int get_last_userlist(CUP_MSG_LIST * p_cup_list  );
		int get_cup(userid_t userid, CUP_STU *p_cup_stu  );
};

#endif   /* ----- #ifndef CUSER_CUP_INCL  ----- */

