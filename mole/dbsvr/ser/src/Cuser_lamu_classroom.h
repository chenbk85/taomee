/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamuclass.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/01/2010 05:05:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_LAMU_CLASSROOM_INCL
#define CUSER_LAMU_CLASSROOM_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_lamu_classroom: public CtableRoute100x10 {
public:
	Cuser_lamu_classroom(mysql_interface * db);

	int insert(userid_t userid, user_lamu_classroom_create_classroom_in* p_in);
	int set_lamu_classroom_name(userid_t userid, char roomname[]);
	int set_lamu_classroom_inner_style(userid_t userid, uint32_t inner_style);
//	int set_lamu_classroom_grade(userid_t userid, uint32_t grade);
	int new_lamu_classroom(userid_t userid, uint32_t difficulty);
	int get_class_difficulty(userid_t userid, uint32_t &difficulty);
	int set_class_difficulty(userid_t userid, uint32_t difficulty);
	int abandon_lamu_classroom(userid_t userid);
	int get_lamu_classroom_info(userid_t userid, user_lamu_classroom_get_class_info_out_header *p_out);
	int get_lamu_classroom_record(userid_t userid, user_lamu_classroom_get_record_out_header *p_out);
	int get_lamu_classroom_teach_plan(userid_t userid, user_lamu_classroom_get_teach_plan_out *p_out);
	int set_class_begin(userid_t userid, uint32_t course_id, uint32_t class_flag);
	int set_class_mode(userid_t userid, uint32_t class_flag);
	int update_when_class_end(userid_t userid, user_lamu_classroom_class_end_out class_end, uint32_t course_cnt);
	int is_class_end_right(userid_t userid, uint32_t course_id, uint32_t course_time);
	int is_class_graduate_right(userid_t userid);
	int is_exam_state(userid_t userid, uint32_t &exam_times);
	int get_class_end_info(userid_t userid, user_lamu_classroom_class_end_out *p_out, uint32_t course_cnt);
	int get_exp_course_cnt(userid_t userid, user_lamu_classroom_get_exp_course_out *p_out);
	int get_class_flag(userid_t userid, uint32_t &class_flag);
	int class_graduate(userid_t userid, uint32_t lamu_cnt, uint32_t great_cnt);
	int get_about_exam_info(userid_t userid, uint32_t &lovely, uint32_t &difficulty, uint32_t &exam_times, uint32_t &exp);
	int set_classroom_after_exam(userid_t userid, user_lamu_classroom_exam_info exam_info);
	int is_class_time(userid_t userid, user_lamu_classroom_is_class_time_out *p_out);
	int is_end_on_time(userid_t userid, uint32_t *pflag);
	int is_user_have_class(userid_t userid, uint32_t *pflag);
	int update_classroom_per_day(userid_t userid);
	int update_today_add(userid_t userid, uint32_t today_add);
	int	check_add_course(userid_t userid, uint32_t add_cnt);
	int get_exam_times(userid_t userid, uint32_t &exam_times);
	int get_exp_levels(userid_t userid, uint32_t &exp_out, uint32_t &level_s);
	int set_honor_flag(userid_t userid, uint32_t flag);
//	int get_event_num(userid_t userid, uint32_t &cnt);
//	int set_event_num(userid_t userid);
	int get_teacher_exp(userid_t userid, uint32_t &exp);
	int add_teacher_exp(userid_t userid, uint32_t exp);
	int get_teacher_evaluate(userid_t userid, uint32_t &evaluate);
	int cal_teacher_after_exam(user_lamu_classroom_exam_info &exam_info, 
			uint32_t lamu_cnt, uint32_t lovely, uint32_t difficulty, uint32_t exam_times);
	double cal_class_difficulty(user_lamu_classroom_add_lamus_in_item *p_item, double difficulty, uint32_t lamu_cnt);
	int check_lamu_classroom_class_begin(userid_t userid);
	int recover_teacher_attr(userid_t userid, user_lamu_classroom_recover_teacher_attr_in *p_in);
	int get_outstand_sum(userid_t userid, uint32_t *p_outstand_sum);
	int check_class_course_id(userid_t userid, uint32_t course_id, uint32_t type = 0);
	int set_cur_course_flag(userid_t userid);
	int check_class_student_cnt(uint32_t userid, uint32_t count);
	int set_event_cnt(userid_t userid, uint32_t course_id, uint32_t lamu_cnt = 0);
	int set_day_networking_class(userid_t userid,userid_t other);
};

#endif  /* ----- #ifndef CUSER_LAMU_CLASSROOM_INCL  ----- */

