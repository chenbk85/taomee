/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamu_classroom_lamu.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/02/2010 09:04:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CUSER_LAMU_CLASSROOM_LAMU_INCL
#define CUSER_LAMU_CLASSROOM_LAMU_INCL

#include "proto.h"
#include "CtableRoute100x10.h"
#include "benchapi.h"

struct lamu_values_stru{   //每次上完课后拉姆属性的变化情况
	int32_t	latent;
	int32_t moral;
	int32_t intelligence;
	int32_t sport;
	int32_t art;
	int32_t labor;
	uint32_t mood;
}__attribute__((packed));

struct lamu_course_score_stru{
	uint32_t lamu_id;
	user_lamu_course_list course_list;
}__attribute__((packed));

class Cuser_lamu_classroom_lamu : public CtableRoute100x10 {
public:
	Cuser_lamu_classroom_lamu(mysql_interface * db);
	int add_lamu(userid_t userid, user_lamu_classroom_add_lamus_in_item *p_item, uint32_t lamu_id);
	int delete_lamu(userid_t userid);
	int get_lamus_info(userid_t userid, user_lamu_classroom_get_lamus_info_out_item **p_list, uint32_t *pcount);
	int get_lamu_info_ex(userid_t userid, user_lamu_classroom_lamu_info_ex_item **pp_list, uint32_t *pcount);
	int get_lamus_count(userid_t userid, uint32_t *pcount, uint32_t flag = 2);
	int update_lamus_when_class_end(userid_t userid, 
			user_lamu_classroom_class_end_in *p_in, uint32_t energy, uint32_t lovely, uint32_t class_flag);
	int get_lamu_show_info(userid_t userid, user_lamu_classroom_get_exam_info_out_item *p_item);
	
	//int update_course(userid_t userid, uint32_t course_id);
	//int is_courseid_in_list(userid_t userid, uint32_t course_id, lamu_courses_list **pplist, uint32_t *pcount, uint32_t *pindex);
	int get_class_end_or_exam_info(userid_t userid, lamu_courses_list **pp_list, uint32_t *pcount);
	//int get_one_lamu_course_info(userid_t userid, user_lamu_course_list *p_course_list);

	int get_lovely_mood(userid_t userid);
	int update_lovely_mood(userid_t userid, uint32_t lamu_id, uint32_t lovely, uint32_t mood);
	int update_one_lamu_course(userid_t userid, uint32_t lamu_id, uint32_t course_id, int score);
	uint32_t cal_course_score_class_end(uint32_t conf_val,
		    uint32_t *attr_vals, uint32_t attr[], uint32_t energy, uint32_t exam_times, uint32_t mood, uint32_t lamu_cnt);
	int cal_exam_values(user_lamu_classroom_score_info *p_score_info, user_lamu_about_exam_list *p_list,
			user_lamu_classroom_begin_exam_in_item *p_item, uint32_t lovely, uint32_t exam_times);
	int insc_lamu_level(userid_t userid, uint32_t lamu_cnt);
	int set_one_lamu_mood(userid_t userid, user_lamu_classroom_set_lamu_mood_in *p_in);
	int update_one_lamu_attr(userid_t userid, user_lamu_classroom_update_lamu_attr_in *p_in, uint32_t course_id = 0);
	int add_lamu_course_score(userid_t userid, user_lamu_classroom_add_course_score_in *p_in);
	int add_one_lamu_attrs(uint32_t userid, user_lamu_classroom_add_lamu_attrs_in *p_in);
	int add_lamu_attrs(uint32_t userid, user_lamu_classroom_add_lamu_attrs_in *p_in);
private:
	int update_one_lamu_when_class_end(userid_t userid, uint32_t lamu_id, 
				lamu_values_stru lamu_stru, user_lamu_course_list course_list);
	int cal_lamus_values(lamu_values_stru *pvalue_stru, uint32_t *p_in_attr, uint32_t energy, uint32_t nature, uint32_t mood);
	int cal_lamus_values_ex(lamu_values_stru *p_value_stru, uint32_t *p_in_attr, uint32_t lovely, uint32_t nature, uint32_t mood);
	uint32_t cal_lamu_score(uint32_t course_score, uint32_t *attr_vals, uint32_t attr[], uint32_t lovely, uint32_t exam_times);
	int is_courseid_in_course_list(uint32_t course_id, user_lamu_course_list course_list);
	int get_one_lamu_course_info(uint32_t userid, uint32_t lamu_id, user_lamu_course_list *p_course_list);
	int set_one_lamu_course_info(userid_t userid, uint32_t lamu_id, user_lamu_course_list lamu_course);
	int get_lamus_course_list(uint32_t userid, lamu_course_score_stru **pp_list, uint32_t *p_count);
};

#endif /* ----- #ifndef CUSER_LAMU_CLASSROOM_LAMU_INCL ----- */
