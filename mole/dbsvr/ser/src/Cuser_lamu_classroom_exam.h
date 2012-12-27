/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamu_classroom_exam.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/16/2010 11:54:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_LAMU_CLASSROOM_EXAM_INCL
#define CUSER_LAMU_CLASSROOM_EXAM_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_lamu_classroom_exam : public CtableRoute100x10 {
public:
	Cuser_lamu_classroom_exam(mysql_interface * db);
	int insert(userid_t userid, user_lamu_classroom_exam_info exam_info);
	int get_exam_info(userid_t userid, uint32_t exam_times, user_lamu_classroom_get_exam_info_out_header *p_out);
	int tw_get_exam_info(userid_t userid, uint32_t exam_times, user_lamu_classroom_tw_get_exam_result_out *p_out);
	int set_exam_prize(userid_t userid, uint32_t exam_times, uint32_t prizes_cnt, user_lamu_classroom_get_prize_in_item *p_item);
	int delete_info(userid_t userid);
private:
	int set_prize(userid_t userid, uint32_t exam_times, user_exam_prize_list prize_list);
};

#endif /* ----- #ifndef CUSER_LAMU_CLASSROOM_EXAM_INCL ----- */
